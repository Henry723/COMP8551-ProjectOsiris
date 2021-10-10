//============================================================================
// Memory Manager Pool Class
// 
//   Supports a simple pool allocation schema to prevent expensive OS memory
//   allocation routines of the general heap.
//
//   Our design is to pull one large chunk of general heap memory which is
//   divided into equivalent blocks of memory.Using the pool's allocation
//   methods, the clients have a clean interface to hide the gritty details
//   for supporting the blocks.
//
//   The pool is defined on instantiation of the class object by defining
//   the type size(the block) and how many blocks are required in the pool.
//
//   Additional capacity methods are available to query status of the pool.
//
// Note: This call also supports a pool alloc request be outside of the 
//       pool blocks and be a general heap allocation. This can also occur
//       if the pool blocks are all used. Therefore, it is critical that 
//       clients profile their use of the pool to assure it is large enough
//       to support all the required pool block allocations.
// 
//   Portions of code were from 
// https://www.codeproject.com/articles/27487/why-to-use-memory-pool-and-how-to-implement-it
//   by Jude Deng.
//
//  Modifications and additional functinality by Richard Tesch
// 
//============================================================================

#ifndef __MemoryManagerPool__CMemMgrPool__
#define __MemoryManagerPool__CMemMgrPool__

#include <stdio.h>

class CMemMgrPool
{
private:

    // Structure definition of a block implemented as a linked list.
    struct _Block
    {
        struct _Block* Prev_p;
        struct _Block* Next_p;
    };

    // Pointer to the enitre memory allocated for Memory Pool.
    void* m_MemPool_p;               

    //Manage all unit with two linkedlist.
    struct _Block* m_UsedBlock_p;   // Head pointer to Allocated linked list.
    struct _Block* m_FreeBlock_p;   // Head pointer to Free linked list.

    unsigned long m_BlockSize;      // One pool block size.
    unsigned long m_BlockTotal;     // Total number of blocks in pool
    unsigned long m_PoolSize;       // Total pool size.
    unsigned long m_UsedBlockCount; // Total Blocks used

public:

    CMemMgrPool(unsigned long BlockNum = 50, unsigned long BlockSize = 1024);
    ~CMemMgrPool();

    // Pool Allocation

      // To allocate memory block from pool
    void* Alloc(unsigned long Size, bool UseMemPool = true);
      // To free a memory block back to pool
    void Free(void* Mem_p);

    // Capacity
 
      // returns pool used blocks
    inline unsigned int size() { return m_UsedBlockCount; }
      // returns pool total blocks
    inline unsigned int max_size() { return m_BlockTotal; }
      // returns pool block size in bytes
    inline unsigned int block_size() { return m_BlockSize; }
      // returns if pool blocks are all free
    inline bool empty() { return m_UsedBlockCount == 0; }
      // returns if pool has no more blocks
    inline bool full() { return m_UsedBlockCount == m_BlockTotal; }
};

#endif /* defined(__MemoryManagerPool__CMemMgrPool__) */
