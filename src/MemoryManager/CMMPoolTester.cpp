//============================================================================
// Memory Manager Pool Class TESTER
// 
//   Tests the Memory Manger Pool Class for funtional requirements.
// 
//   - Bounds checking the pool against full and empty contions.
//   - Validate that all Capacity methods are functional through entire
//     pool usage
//   - Block sizing validation on various pool creation sizes
//   - Validate that allocation works inside an outside the pool/
//
//  Created by Richard Tesch
// 
//============================================================================

#include "CMMPoolTester.h"
#include <stdio.h>
#include <iostream>
#include <stack>
#include <vector>
#include <time.h>

const int poolSize_c = 100;

CMMPoolTester::CMMPoolTester()
    : m_Rslt_Linear_Alloc_Free(false), m_Rslt_Random_Alloc_Free(false), 
      m_Rslt_size(false), m_Rslt_max_size(false), m_Rslt_block_size(false),
      m_Rslt_empty(false), m_Rslt_full(false), m_Rslt_InPoolOut_Alloc(false),
      m_NumOfTest(8)
{
    pool_p = new CMemMgrPool(poolSize_c, sizeof(int));
}

CMMPoolTester::~CMMPoolTester()
{
    if (pool_p != NULL)
        delete pool_p;
}

void CMMPoolTester::run_test(report type)
{
    // Run Tests storing results in object
    m_Rslt_Linear_Alloc_Free = Test_Linear_Alloc_Free();
    m_Rslt_Random_Alloc_Free = Test_Random_Alloc_Free();
    m_Rslt_size = Test_size();
    m_Rslt_max_size = Test_max_size();
    m_Rslt_block_size = Test_block_size();
    m_Rslt_empty = Test_empty();
    m_Rslt_full = Test_full();
    m_Rslt_InPoolOut_Alloc = Test_InPoolOut_Alloc();

    if (type == report::verbose)
    {
        std::cout << std::endl << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Memory Manager Test START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        std::cout << "Pool Test" << std::endl;
        std::cout << "\tfailed: " << failed() << "/" << num_tests() << std::endl;
        std::cout << "\tpassed: " << passed() << "/" << num_tests() << std::endl;
        std::string rslt = tests_all_passed() ? std::string("OK") : std::string("ERROR");
        std::cout << "\tPool Test = " << rslt << std::endl;
        std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Memory Manager Test END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    }
}

int CMMPoolTester::passed()
{
    int rslt = 0;

    // Tally all tests that passed
    rslt += m_Rslt_Linear_Alloc_Free ? 1 : 0;
    rslt += m_Rslt_Random_Alloc_Free ? 1 : 0;
    rslt += m_Rslt_size ? 1 : 0;
    rslt += m_Rslt_max_size ? 1 : 0;
    rslt += m_Rslt_block_size ? 1 : 0;
    rslt += m_Rslt_empty ? 1 : 0;
    rslt += m_Rslt_full ? 1 : 0;
    rslt += m_Rslt_InPoolOut_Alloc ? 1 : 0;

    return rslt;
}

int CMMPoolTester::failed()
{
    int rslt = 0;

    // Tally all tests that failed
    rslt += m_Rslt_Linear_Alloc_Free ? 0 : 1;
    rslt += m_Rslt_Random_Alloc_Free ? 0 : 1;
    rslt += m_Rslt_size ? 0 : 1;
    rslt += m_Rslt_max_size ? 0 : 1;
    rslt += m_Rslt_block_size ? 0 : 1;
    rslt += m_Rslt_empty ? 0 : 1;
    rslt += m_Rslt_full ? 0 : 1;
    rslt += m_Rslt_InPoolOut_Alloc ? 0 : 1;

    return rslt;
}

bool CMMPoolTester::Test_Linear_Alloc_Free()
{
    int TotalBlocks = 100;
    CMemMgrPool testPool(TotalBlocks, sizeof(int));
    std::stack<int*> poolBlocks;
    unsigned int rawChkSum = 0;

    // Test fails if can't get total blocks requested the same
    if (   testPool.max_size() != TotalBlocks
        || !testPool.empty()
        || testPool.full()
        || testPool.block_size() != sizeof(int)
        || testPool.size() != 0)
        return false;

    // Loop through all blocks allocating to the end of pool
    for (int i = 0; i < testPool.max_size(); i++)
    {
        int* curBlock_p = (int*)testPool.Alloc(sizeof(int));
        if (curBlock_p != NULL)
        {
            *curBlock_p = i;
            poolBlocks.push(curBlock_p);

            // Build a checksum to assure that the data stored
            // and then retrieved is the same.
            rawChkSum += i;
        }
    }

    // Loop through all blocks in our stack and free them
    for (int i = 0; i < testPool.max_size(); i++)
    {
        int* curBlock_p = poolBlocks.top();
        poolBlocks.pop();

        // Recreate the checksum to assure that the data stored
        // and now retrieved is the same as before.
        rawChkSum -= *curBlock_p;
        testPool.Free(curBlock_p);
    }

    // Sanoty check for the pass...
    return (    testPool.empty()        // Pool must now be empty
            && !testPool.full()         // Pool cannot be full
            &&  testPool.size() == 0    // Pool size used must be 0
            &&  rawChkSum == 0);        // The data values stored and 
                                        //  retieved are the same.
}

bool CMMPoolTester::Test_Random_Alloc_Free()
{
    const int totalBlocks = 1000;
    CMemMgrPool testPool(totalBlocks, sizeof(long long int));
    std::vector<long long int*> poolBlocks;
    long long int rawChkSum = 0;
    std::srand(time(0));
    std::vector<long long int*> elToDelete;

    // Loop through all blocks allocating to the end of pool
    for (long long int i = 0; i < testPool.max_size(); i++)
    {
        long long int* curBlock_p = (long long int*)testPool.Alloc(sizeof(long long int));
        if (curBlock_p != NULL)
        {
            *curBlock_p = i + 1;
            poolBlocks.push_back(curBlock_p);
            rawChkSum += i + 1;
        }
    }

    // Remove 1/5 of total pool elements randomly
    long long int j = 0, RndIdx;
    const long long int elToRemove = totalBlocks / 5;
    do
    {
        // Generate a random index in vector that we want to delete
        RndIdx = rand() % (totalBlocks - j);

        // Get block pointer at index then remove it from our blocks list
        long long int* curBlock_p = poolBlocks.at(RndIdx);
        poolBlocks.erase(poolBlocks.begin() + RndIdx);

        // Before removing block from pool adjust checksum with its data
        rawChkSum -= *curBlock_p;
        testPool.Free(curBlock_p);
        j++;
    } while (j < elToRemove);

    // Quick sanity check: the pool size reported should now be total - 1/5 of size
    if (testPool.size() != (totalBlocks - elToRemove))
    {
        return false;
    }

    // Alloc back the blocks taken away. Note: Pool blocks are now fragmented.
    for (long long int i = 0; i < elToRemove; i++)
    {
        long long int* curBlock_p = (long long int*)testPool.Alloc(sizeof(long long int));
        if (curBlock_p != NULL)
        {
            *curBlock_p = i + 1;
            poolBlocks.push_back(curBlock_p);
            rawChkSum += i + 1;
        }
    }

    // Sanity check: pool size reported should now be original size and full
    if (    testPool.size() != totalBlocks
        || !testPool.full()
        ||  testPool.empty())
    {
        return false;
    }

    // Loop through all blocks in our stack and free them
    for (int i = 0; i < testPool.max_size(); i++)
    {
        long long int* curBlock_p = poolBlocks.back();
        poolBlocks.pop_back();

        // Recreate the checksum to assure that the data stored
        // and now retrieved is the same as before.
        rawChkSum -= *curBlock_p;
        testPool.Free(curBlock_p);
    }

    // Sanoty check for the pass...
    return (    testPool.empty()        // Pool must now be empty
            && !testPool.full()         // Pool cannot be full
            &&  testPool.size() == 0    // Pool size used must be 0
            &&  rawChkSum == 0);        // The data values stored and 
                                        //  retieved are the same.
}

bool CMMPoolTester::Test_size()
{
    int cntSize = 0;
    std::stack<int*> poolBlocks;

    // Max sure we have a pool_p that's valid at begining of test
    if (pool_p == NULL)
    {
        pool_p = new CMemMgrPool(poolSize_c, sizeof(int));
    }

    // Use the size in the dynamic allocated pool of int by adding
    //  values which on every alloc should inc size by one.
    for (int i = 0; i < poolSize_c; i++)
    {
        if (pool_p->size() != cntSize)
        {
            return false;   // If we are here then size method failed
        }

        // Allocate a block from pool making sure it happened
        int* curBlock_p = (int*)pool_p->Alloc(sizeof(int));
        if (curBlock_p == NULL)
        {
            return false;   // If we are here then pool alloc failed
        }

        // Record the block so we can remove later while inc cntSize
        poolBlocks.push(curBlock_p);
        cntSize++;
    }

    // We can't check the last add so do it here
    if (pool_p->size() != cntSize)
    {
        return false;   // If we are here then size method failed
    }

    // Use the size in the dynamic allocated pool of int by removing
    //  values which on every free should dec size by one.
    for (int i = 0; i < poolSize_c; i++)
    {
        // Free a block to pool making sure it happened
        int* curBlock_p = poolBlocks.top();
        poolBlocks.pop();

        pool_p->Free(curBlock_p);
        cntSize--;

        if (pool_p->size() != cntSize)
        {
            return false;   // If we are here then size method failed
        }
    }

    return true;
}

bool CMMPoolTester::Test_max_size()
{
    // Test will iterate for 20 turns
    for (int i = 0; i < 20; i++)
    {
        // Pool size allocation is always 10 + current turn
        int poolSize = 10 + i;

        // Always make sure we have the proper pool allocated
        if (pool_p != NULL) { delete pool_p; }
        pool_p = new CMemMgrPool(poolSize, sizeof(bool));

        // Test that pool max size is valid on this pool creation
        // Note in free and alloc tests we are covering other 
        // max size testing conditions.
        if (pool_p->max_size() != poolSize)
        {
            return false;   // Test failed
        }
    }

    // Cleanup making sure the pool_p is the default on class creation
    delete pool_p;
    pool_p = new CMemMgrPool(poolSize_c, sizeof(int));

    return true;
}

bool CMMPoolTester::Test_block_size()
{
    // Test will iterate for 20 turns
    for (int i = 0; i < 20; i++)
    {
        // Pool size allocation is always 10 + current turn
        int poolSize = 10 + i;

        // Always make sure we have the proper pool allocated
        if (pool_p != NULL) { delete pool_p; }
        pool_p = new CMemMgrPool(poolSize, i + 4);

        // Test the block size better be what we requested
        if (pool_p->block_size() != (i + 4))
        {
            return false;   // Test failed
        }
    }

    // Cleanup making sure the pool_p is the default on class creation
    delete pool_p;
    pool_p = new CMemMgrPool(poolSize_c, sizeof(int));

    return true;
}

bool CMMPoolTester::Test_empty()
{
    const int totalBlocks_c = 41;
    CMemMgrPool testPool(totalBlocks_c, sizeof(int));
    std::stack<int*> poolBlocks;
    unsigned int rawChkSum = 0;

    // Test fails if can't get total blocks requested the same
    if (    testPool.max_size() != totalBlocks_c
        || !testPool.empty()
        ||  testPool.full()
        ||  testPool.block_size() != sizeof(int)
        ||  testPool.size() != 0)
        return false;

    // Loop through all blocks allocating to the end of pool
    for (int i = 0; i < totalBlocks_c; i++)
    {
        if (testPool.empty() && i != 0)
        {
            return false;   // Test failed
        }

        int* curBlock_p = (int*)testPool.Alloc(sizeof(int));
        if (curBlock_p != NULL)
        {
            *curBlock_p = i;
            poolBlocks.push(curBlock_p);

            // Build a checksum to assure that the data stored
            // and then retrieved is the same.
            rawChkSum += i;
        }
    }

    // We would miss the last one in loop so test now
    if (testPool.empty())
    {
        return false;   // Test failed
    }

    // Loop through all blocks in our stack and free them
    for (int i = 0; i < totalBlocks_c; i++)
    {
        int* curBlock_p = poolBlocks.top();
        poolBlocks.pop();

        // Recreate the checksum to assure that the data stored
        // and now retrieved is the same as before.
        rawChkSum -= *curBlock_p;
        testPool.Free(curBlock_p);

        if (testPool.empty() && i < totalBlocks_c - 1)
        {
            return false;   // Test failed
        }
    }

    // Sanity check for the pass...
    return (    testPool.empty()        // Pool must now be empty
            && !testPool.full()         // Pool cannot be full
            &&  testPool.size() == 0    // Pool size used must be 0
            &&  rawChkSum == 0);        // The data values stored and 
                                        //  retieved are the same.
}

bool CMMPoolTester::Test_full()
{
    const int totalBlocks_c = 41;
    CMemMgrPool testPool(totalBlocks_c, sizeof(int));
    std::stack<int*> poolBlocks;
    unsigned int rawChkSum = 0;

    // Test fails if can't get total blocks requested the same
    if (testPool.max_size() != totalBlocks_c
        || !testPool.empty()
        || testPool.full()
        || testPool.block_size() != sizeof(int)
        || testPool.size() != 0)
        return false;

    // Loop through all blocks allocating to the end of pool
    for (int i = 0; i < totalBlocks_c; i++)
    {
        if (testPool.full() && i != totalBlocks_c)
        {
            return false;   // Test failed
        }

        int* curBlock_p = (int*)testPool.Alloc(sizeof(int));
        if (curBlock_p != NULL)
        {
            *curBlock_p = i;
            poolBlocks.push(curBlock_p);

            // Build a checksum to assure that the data stored
            // and then retrieved is the same.
            rawChkSum += i;
        }
    }

    // We would miss the last one in loop so test now
    if (!testPool.full())
    {
        return false;   // Test failed
    }

    // Loop through all blocks in our stack and free them
    for (int i = 0; i < totalBlocks_c; i++)
    {
        int* curBlock_p = poolBlocks.top();
        poolBlocks.pop();

        // Recreate the checksum to assure that the data stored
        // and now retrieved is the same as before.
        rawChkSum -= *curBlock_p;
        testPool.Free(curBlock_p);

        if (testPool.full() && i != totalBlocks_c)
        {
            return false;   // Test failed
        }
    }

    // Sanity check for the pass...
    return (    testPool.empty()        // Pool must now be empty
            && !testPool.full()         // Pool cannot be full
            &&  testPool.size() == 0    // Pool size used must be 0
            &&  rawChkSum == 0);        // The data values stored and 
                                        //  retieved are the same.
}

bool CMMPoolTester::Test_InPoolOut_Alloc()
{
    const int totalBlocks_c = 5;
    CMemMgrPool testPool(totalBlocks_c, sizeof(int));
    std::stack<int*> poolBlocks;
    unsigned int rawChkSum = 0;

    // Test fails if can't get total blocks requested the same
    if (    testPool.max_size() != totalBlocks_c
        || !testPool.empty()
        ||  testPool.full()
        ||  testPool.block_size() != sizeof(int)
        ||  testPool.size() != 0)
        return false;

    // Loop through all blocks allocating to the end of pool PLUS 1
    // The one should be from general memory and not pool
    for (int i = 0; i < totalBlocks_c + 1; i++)
    {
        int* curBlock_p = (int*)testPool.Alloc(sizeof(int));
        if (curBlock_p != NULL)
        {
            *curBlock_p = i;
            poolBlocks.push(curBlock_p);

            // Build a checksum to assure that the data stored
            // and then retrieved is the same.
            rawChkSum += i;
        }
    }

    // Test fails if pool size (allocations) is greater than total pool blocks
    if (testPool.size() > totalBlocks_c)
    {
        return false;
    }

    // Grab the last pushed as this is the out of pool memory
    int* outOfPoolMem_p = poolBlocks.top();
    poolBlocks.pop();

    // Loop through all blocks in our stack and free them
    for (int i = 0; i < totalBlocks_c; i++)
    {
        int* curBlock_p = poolBlocks.top();
        poolBlocks.pop();

        // Recreate the checksum to assure that the data stored
        // and now retrieved is the same as before.
        rawChkSum -= *curBlock_p;
        testPool.Free(curBlock_p);
    }

    // Test that pool is now ...
    bool rslt = (    testPool.empty()        // Pool must now be empty
                 && !testPool.full()         // Pool cannot be full
                 &&  testPool.size() == 0    // Pool size used must be 0
                 &&  rawChkSum == *outOfPoolMem_p); // Data values stored and 
                                                    //  retieved are the same.

    // Free the out of mem pointer using the pool free and it shouldn't throw
    testPool.Free(outOfPoolMem_p);

    return rslt;
}
