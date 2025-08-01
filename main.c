#include<stdio.h>
#include<assert.h>
#include <sys/mman.h>
#include<unistd.h>
struct block * balloc(int);
int getlevel(int);
int min_size=5;//minimum size of the block is 32 bytes (2^5)
	       // 32 bytes is the minimum size of a block, so the level starts from 0 for 32 bytes
	       // and goes up to 7 for 4096 bytes (2^12).
	       // The levels are defined as follows:
	       // Level 0: 32 bytes (2^5)
	       // Level 1: 64 bytes (2^6)
	       // Level 2: 128 bytes (2^7)
	       // Level 3: 256 bytes (2^8)
	       // Level 4: 512 bytes (2^9)
	       // Level 5: 1024 bytes (2^10)
	       // Level 6: 2048 bytes (2^11)
	       // Level 7: 4096 bytes (2^12)
typedef struct allocation
{
	int size;
	void *ptr;
}alloc;
typedef struct block
{
	int status;
	int level;
	struct block *next;
	struct block *prev;
}block;
block *freelist[8]={NULL};
block* split_block(block *head,int index);
void bfree(void *);
void add_to_free_list(block *right,int index);
block* findbuddy(block *head);
void remove_free_list(block* buddy);
block * merge_blocks(block *head);
int n=1;//flag to check if it is the first time allocation
alloc table[100];
int main()
{
	int num=0;
	int opt=0;
	int size_tr=0;
	printf("WELCOME TO BUDDY MEMORY ALLOCATION\n");
	while(1)
	{
		printf("PLEASE CHOOSE ANY ONE:\nPRESS 1 FOR ALLOCATION\nPRESS 2 FOR DEALLOCATION\nPRESS 3 FOR PRINTING THE ALLOCATED BLOCKS\nPRESS 4 FOR EXIT\n");
		scanf("%d",&opt);
		if(opt==1)
		{
			printf("ENTER THE SIZE IN BYTES TO ALLOCATE: ");
			scanf("%d", &num);
			void *p=balloc(num);
			table[size_tr].ptr=p;
			table[size_tr].size=num;
			size_tr++;
			if(p==NULL)
			{
				printf("MEMORY ALLOCATION FAILED\n");
				return 0;
			}
			printf("SUCCESSFULLY ALLOCATED THE ENTERED BLOCK\n");
		}
		else if(opt==2)
		{

			unsigned long fr=0;
			printf("ENTER THE ADDRESS OF THE BLOCK WHICH IS TO BE DEALLOCATED\n");
			scanf("%lx",&fr);
			int i=0;
			int found=0;
			for(int i=0;i<size_tr;i++)
			{
				if((unsigned long)table[i].ptr==fr&&table[i].ptr!=NULL)
				{
					bfree(table[i].ptr);
					printf("SUCCESSFULLY DEALLOCATED THE ENTERED BLOCK\n");
					table[i].ptr=NULL;
					found=1;
				}

			}
			if(!found)
			{
				printf("ADDRESS NOT FOUND\nPLEASE SELECT AN ALLOCATED BLOCK\n");
			}
		}
		else if(opt==3)
		{
			int i=0;
			int is=0;
			for(i=0;i<size_tr;i++)
			{
				if(table[i].ptr!=NULL)
				{
					is=1;
					printf("%d BYTES IS ALLOCATED AT THE MEMORY ADDRESS: %p\n",table[i].size,table[i].ptr);
				}
			}
			if(!is)
				printf("CURRENTLY NO BLOCKS ARE ALLOCATED\n");
		}
		else if(opt==4)
		{
			printf("EXITING FROM BUDDY MEMORY ALLOCATION.........\n");
			sleep(1);
			break;
		}
		else
		{
			printf("PLEASE CHOOSE A VALID OPTION\n");
			break;
		}

	}
}
block *balloc(int size)
{
	if(size<=0)
	{
		printf("PLEASE ENTER A VALID SIZE\n");
		return NULL;
	}
	if(size>4096)
	{

		printf("SIZE IS TOO BIG\nPLEASE ENTER A SIZE LESS THAN 4096\n");
		return NULL;//size is too big
	}
	int level=getlevel(size);
	if(n==1)//first time allocation
	{
		n=0;
		block *head=(block*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
		if(head==MAP_FAILED)
		{
			printf("MMAP FAILED");//print error if mmap fails
			return NULL; 
		}
		head->status=1;
		head->level=7;
		head->next=NULL;
		head->prev=NULL;
		freelist[7]=head;
	}
	block *head=freelist[level];
	if(head==NULL)//no free blocks available at the requested level
	{
		for(int i=level+1;i<8;i++)//check for higher levels
		{
			if(freelist[i]!=NULL)//checking if there is a free block at a higher level
			{
				head=freelist[i];
				remove_free_list(head);
				while(head->level>level)
				{
					block *right=split_block(head,head->level);//storing the right buddy address //marking the right buddy as free
					right->status=1;
					right->level=head->level-1;
					add_to_free_list(right,right->level);//adding the right buddy to the free list
					head->level--;
				}
				break;
			}
		}
	}
	head = freelist[level];
	if (head == NULL)
	{
		printf("NO FREE BLOCKS AVAILABLE\n");
		return NULL;
	}
	remove_free_list(head);
	head->status = 0;
	return (void *)(head + 1);
}
int getlevel(int size)
{
	int level=0;
	int size1=5;
	while(1<<size1<size)
	{
		size1++;
		level++;
	}
	return level;
}
block* split_block(block *head,int index)
{
	int mask=1<<(5+index);
	return (block*)((char*)head + mask / 2);//splitting using pointer arithmetic
}
void add_to_free_list(block *right,int index)
{
	right->status=1;
	right->level=index;
	right->next=freelist[index];
	right->prev=NULL;
	if(freelist[index]!=NULL)//if there is already a block in the free list at this level
	{
		freelist[index]->prev=right;//set the previous pointer of the current head to the new block
	}
	freelist[index]=right;
}
void bfree(void *ptr)
{
	block *head=((block*)ptr)-1;
	head->status=1;
	while(head->level<7)
	{
		block *buddy=findbuddy(head);
		if((buddy->status==1)&&(head->level==buddy->level))
		{
			remove_free_list(buddy);
			block *merged=merge_blocks(head);
			merged->level=head->level+1;
			merged->status=1;
			head=merged;
		}
		else
		{
			add_to_free_list(head,head->level);
			return;
		}
	}
	add_to_free_list(head, head->level);
}
block* findbuddy(block *buddy)
{
	int index = buddy->level;
	long int mask = 1 << (index + 5);
	return (block*)((long int)buddy ^ mask);
}
void remove_free_list(block *head)
{
	int index=head->level;
	if(freelist[index]==head)//it is pointing to head
	{
		freelist[index]=head->next;
	}
	if(head->prev!=NULL)//when the head is not the first block
	{
		head->prev->next=head->next;
	}
	if(head->next!=NULL)//when head is not the last block
	{
		head->next->prev=head->prev;
	}
}
block *merge_blocks(block *head)
{
	int index=head->level;
	long int mask=0xffffffffffffffff<<(1+index+5);
	return (block*)((long int)head&mask);
}
