//============================================================================
// Configuration Manager - Application Class Interface
//
// Defines Application Layer Class for the Configuration Manager subsystem.
//
// The Configuration Manager Subsystem purpose is to read external source(s)
// information containing game objects data or custom data back and forth
// through 2 layers. The two layers and their purpose are defined as:
//
//  Layer 1. Application : Implements the API between its clients and the
//                         link between the physical layer.
//        4. Physical : Implements the mechanism to retrieve or write data
//                      from sources presented in their correct formats.
//
//  This layer methodology assures that each layer is independent and could
//  change without affecting its adjacent layer providing the signatures
//  between the two remain the same.
//
//  Created by Richard Tesch
// 
//============================================================================

#ifndef __ConfigurationManager__CfgMgrAppTester__
#define __ConfigurationManager__CfgMgrAppTester__

#include <stdio.h>
#include <vector>
#include <string>
#include "CfgMgrApplication.h"


class CCfgMgrAppTester
{
public:
    enum class report { silent, verbose };

    CCfgMgrAppTester();
    ~CCfgMgrAppTester();

    void run_test(report type);
    int num_tests() { return m_NumOfTest; }
    int passed();
    int failed();
    bool tests_all_passed() { return (failed() == 0); };


private:
    CCfgMgrApplication* cfgMgr_p;

    bool m_Rslt_load_Config;
    bool m_Rslt_load_Custom_Data;
    bool m_Rslt_get_Data_Keys;
    bool m_Rslt_get_Data_Value;
    bool m_Rslt_set_Data_Value;
    bool m_Rslt_save_Custom_Data;

    std::string m_TestFile;
    int m_handle;
    int m_NumOfTestRec;
    int m_NumOfTest;
    int m_NumTestType[3];


    bool Test_load_Config();
    bool Test_load_Custom_Data();
    bool Test_get_Data_Keys();
    bool Test_get_Data_Value();
    bool Test_set_Data_Value();
    bool Test_save_Custom_Data();
};
#endif /* defined(__ConfigurationManager__CfgMgrAppTester__) */
