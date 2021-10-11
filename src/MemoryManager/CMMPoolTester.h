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

#ifndef __MemoryManagerPool__CMMPoolTester__
#define __MemoryManagerPool__CMMPoolTester__

#include "CMemMgrPool.h"


class CMMPoolTester
{
public:
    enum class report {silent, verbose};

public:
    CMMPoolTester();
    ~CMMPoolTester();

    void run_test(report type);
    int num_tests() { return m_NumOfTest;  }
    int passed();
    int failed();
    bool tests_all_passed() { return (failed() == 0); };

private:
    CMemMgrPool* pool_p;

    bool m_Rslt_Linear_Alloc_Free;
    bool m_Rslt_Random_Alloc_Free;
    bool m_Rslt_size;
    bool m_Rslt_max_size;
    bool m_Rslt_block_size;
    bool m_Rslt_empty;
    bool m_Rslt_full;
    bool m_Rslt_InPoolOut_Alloc;
    int m_NumOfTest;

    bool Test_Linear_Alloc_Free();
    bool Test_Random_Alloc_Free();
    bool Test_size();
    bool Test_max_size();
    bool Test_block_size();
    bool Test_empty();
    bool Test_full();
    bool Test_InPoolOut_Alloc();
};

#endif /* defined(__MemoryManagerPool__CMMPoolTester__) */
