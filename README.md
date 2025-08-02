# Buddy Memory Allocation System in C

## Description

This project implements a simple Buddy Memory Allocation System in C, simulating how dynamic memory is allocated and deallocated using the buddy system algorithm. Memory is managed in power-of-two block sizes using `mmap()` and free lists.

---

## Memory Layout

Memory blocks range from 32 bytes to 4096 bytes. These are categorized into 8 levels:

| Level | Block Size |
|-------|------------|
| 0     | 32 bytes   |
| 1     | 64 bytes   |
| 2     | 128 bytes  |
| 3     | 256 bytes  |
| 4     | 512 bytes  |
| 5     | 1024 bytes |
| 6     | 2048 bytes |
| 7     | 4096 bytes |

---

## Program Menu

- Press `1` for Allocation  
- Press `2` for Deallocation  
- Press `3` for Printing the Allocated Blocks  
- Press `4` for Exit

## 📌 Features

* `balloc(int size)` – Allocate memory block of suitable level
* `bfree(void *ptr)` – Deallocate memory and merge buddies if possible
* Uses `mmap()` to simulate a 4096-byte memory pool
* Free blocks are tracked using `freelist[8]`
* Splits larger blocks into smaller ones when needed
* Merges free buddy blocks automatically
* Supports up to 100 allocation records using a static table
* CLI-based user interface

---

##  Code Structure

* `balloc(int size)` – Allocates a memory block of suitable level  
* `bfree(void *ptr)` – Frees and merges buddy blocks  
* `freelist[8]` – Array of linked lists tracking free blocks per level  
* `split_block()` – Splits larger blocks into smaller ones  
* `merge_blocks()` – Merges buddy blocks into a larger block  
* `getlevel()` – Calculates the appropriate level based on size  

---
## Notes

* Only 4096 bytes (4KB) total are available in this simulation  
* Allocation fails if requested size > 4096 bytes  
* Allocated blocks are tracked in a static table: `table[100]`  
* Merging occurs only when both buddy blocks are free and at the same level


