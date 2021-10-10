#pragma once
#include "CMemMgrPool.h"


class CMMPoolTester
{
public:
    CMMPoolTester();
    ~CMMPoolTester();

    void run_test();
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

    bool Test_Linear_Alloc_Free();
    bool Test_Random_Alloc_Free();
    bool Test_size();
    bool Test_max_size();
    bool Test_block_size();
    bool Test_empty();
    bool Test_full();
    bool Test_InPoolOut_Alloc();
};

