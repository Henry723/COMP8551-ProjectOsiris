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

#include "CMemMgrPool.h"
#include <cstdlib>

//============================================================================
// CMemMgrPool
// 
//    Class Constructor used to allocate an OS chunk of memory and distribute
//    it in to a double linked list of blocks.
//
// Parameters:
//    BlockNum - IN - Number of equal block sizes in total Pool.
//    BlockSize - IN - Number representing one blocks size.
//
//============================================================================
CMemMgrPool::CMemMgrPool(unsigned long BlockNum, unsigned long BlockSize) :
    m_MemPool_p(NULL), m_UsedBlock_p(NULL), m_FreeBlock_p(NULL),
    m_PoolSize(0), m_BlockSize(BlockSize), m_BlockTotal(BlockNum),
    m_UsedBlockCount(0)
{
    // Calculate pool size so we can allocate the amount from the OS.
    // Note: Pool size includes the _Block structure overhead for each block
    //       multiplied by total requested blocks
    m_PoolSize = (m_BlockSize + sizeof(struct _Block)) * m_BlockTotal;
    m_MemPool_p = malloc(m_PoolSize);

    // Was the allocation successful (valid pointer to memory returned)?
    if (m_MemPool_p != NULL)
    {
        // Loop to link all the memory blocks into a linked list.
        for (unsigned long i = 0; i < m_BlockTotal; i++)
        {
            // Calculate the next block in memory
            struct _Block* CurBlock_p =
                (struct _Block*)((char*)m_MemPool_p +  
                                 (i * (m_BlockSize + sizeof(struct _Block))));

            // Insert the new block at the head and init previous to NULL
            CurBlock_p->Next_p = m_FreeBlock_p;
            CurBlock_p->Prev_p = NULL;

            // Is there a free block available?
            if (m_FreeBlock_p != NULL)
            {
                // Yes, so make free block's previous point to the current block
                m_FreeBlock_p->Prev_p = CurBlock_p;
            }

            // Init free block for next iteration as the current block
            m_FreeBlock_p = CurBlock_p;
        }
    }
}

//============================================================================
//  ~CMemPool
// 
//    Class destructor used to free the allocated OS chunk of memory taken in 
//    in the constructor.
//
//  Parameters:
//    N/A
// 
//   Notes: It is up to the owner of this pool to deallocate the pool objects.
//          This includes if they were a class that had a destructor which must
//          be triggered before pool block is removed.
//
//============================================================================
CMemMgrPool::~CMemMgrPool()
{
    // Us the OS free function to give back the memory pool.
    free(m_MemPool_p);
}

//============================================================================
//  Alloc
//
//    Allocates a memory block for use by the client. If the memory pool 
//    cannot provide anymore memory blocks (all used up) or the requested 
//    size is beyond a block size, it will call the expensive OS system
//    alloc function to fulfill the request.
//
//  Parameters:
//    Size - IN - Memory unit size.
//    UseMemPool - IN - defaults to true but if false is specified then OS
//                      system malloc is used and has a higher cost than by
//                      using the memory pool blocks.
//
// Return Values:
//    Return a pointer to a memory block or a chuck of memory outside of the
//    memory manager pool.
//============================================================================
void* CMemMgrPool::Alloc(unsigned long Size, bool UseMemPool)
{
    // Check if we allocating from the memory pool or from OS system
    if (   Size > m_BlockSize       // Requested size > than pool block size?
        || UseMemPool == false      // Specifically requesting outside pool?
        || m_MemPool_p == NULL      // Mem Pool was never created or error state?
        || m_FreeBlock_p == NULL)   // Mem pool has no more blocks available?
    {
        // Condition exists that we must use the expensive OS system allocation
        return malloc(Size);
    }

    // Using the Mem Pool to grab an available free block and then use
    //  it's next as new free block
    struct _Block* CurBlock_p = m_FreeBlock_p;
    m_FreeBlock_p = CurBlock_p->Next_p;

    // Is the new free block an available block (not empty list)
    if (m_FreeBlock_p != NULL)
    {
        // Free block previous needs to be NULL so it is head of free list
        m_FreeBlock_p->Prev_p = NULL;
    }

    // Attach current block to front of used list
    CurBlock_p->Next_p = m_UsedBlock_p;

    // Is the used block list is usable (not NULL or not empty)
    if (m_UsedBlock_p != NULL)
    {
        // Attach used block previous to point current (front of list) block
        m_UsedBlock_p->Prev_p = CurBlock_p;
    }

    // Adjust used block list to point to current, now used, block as head
    m_UsedBlock_p = CurBlock_p;

    ++m_UsedBlockCount;     // Keep it tidy and increment used block count

    // return a void pointer to the data portion of the block
    return (void*)((char*)CurBlock_p + sizeof(struct _Block));
}

//============================================================================
//  Free
//
//    Frees a memory block belonging to the pool or an OS allocated block. 
//    If the pointer to free is a pool memory block pool it is returned to 
//    the pool; However, if the pointer is not from the pool, then its is 
//    assumed it was allocated through the OS and will use OS calls to 
//    return the block to the global heap.
//
//  Parameters:
//    Mem_p - IN - Memory block to free.
//
//============================================================================
void CMemMgrPool::Free(void* Mem_p)
{
    // Sanity check...if a null pointer passed in just return
    if (Mem_p == NULL)
        return;

    // Is the memory pointer passed in reside within the pool address space?
    if (   Mem_p > m_MemPool_p 
        && Mem_p < (void*)((char*)m_MemPool_p + m_PoolSize))
    {
        // This is a memory pool block so make the memory block data for it
        struct _Block* CurBlock_p = 
            (struct _Block*)((char*)Mem_p - sizeof(struct _Block));

        // Adjust the used block list to the next one from current
        m_UsedBlock_p = CurBlock_p->Next_p;

        // Is the used block usable (not NULL)
        if (m_UsedBlock_p != NULL)
        {
            // We are removing from front of used list so previous is NULL
            m_UsedBlock_p->Prev_p = NULL;
        }

        // Reattach the current block to front of free list
        CurBlock_p->Next_p = m_FreeBlock_p;

        // Is the free block list a usable block (not an empty list)
        if (m_FreeBlock_p != NULL)
        {
            // Adjust free list to make current the front
            m_FreeBlock_p->Prev_p = CurBlock_p;
        }

        // Make the current block the head of the free list
        m_FreeBlock_p = CurBlock_p;

        --m_UsedBlockCount;     // Keep it tidy and decrement used block count
    }
    else // Otherwise, we treat memory pointer as an OS allocated block
    {
        // Use the OS routines to return memory to the general heap
        free(Mem_p);
    }
}