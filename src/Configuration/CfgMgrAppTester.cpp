//============================================================================
// Configuration Manager - Application Class Implementation
//
// Implements Application Layer Tester Class for the Configuration Manager
// subsystem.
//
//  Created by Richard Tesch
// 
//============================================================================

#include "CfgMgrAppTester.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

const string c_keyValues[12] = {
    "High Score 1", "High Score 2", "High Score 3", "High Score 4", "High Score 5",
    "Last Score",
    "Avg Score",
    "Name HS1", "Name HS2", "Name HS3", "Name HS4", "Name HS5" };
const CKVType c_types[] = {
    CKVType::Int_t, CKVType::Int_t, CKVType::Int_t, CKVType::Int_t, CKVType::Int_t,
    CKVType::Int_t,
    CKVType::Float_t,
    CKVType::String_t, CKVType::String_t, CKVType::String_t, CKVType::String_t, CKVType::String_t };
const int c_iValues[] = { 33, 32, 25, 13, 12, 25 };
const float c_fValues[] = { 22.0 };
const string c_sValues[] = { "Richard Tesch", "K.Olsson", "H Zhang", "R Tesch", "Kurtis L." };


CCfgMgrAppTester::CCfgMgrAppTester()
    : m_Rslt_load_Config(false), m_Rslt_load_Custom_Data(false),
      m_Rslt_save_Custom_Data(false), m_Rslt_get_Data_Keys(false), 
      m_Rslt_get_Data_Value(false), m_Rslt_set_Data_Value(false),
      m_TestFile("CfgTestFile.cfg"), m_handle(-1), 
      m_NumOfTestRec(0), m_NumOfTest(6)
{
    cfgMgr_p = new CCfgMgrApplication();

    m_NumTestType[0] = 0;
    m_NumTestType[1] = 0;
    m_NumTestType[2] = 0;

    // Create a test file to load
    fstream testFile;

    // Opens the test file and erases any previous content if existed
    testFile.open(m_TestFile, fstream::trunc | fstream::out);
    if (testFile)
    {
        for (int i = 0; i < 12; i++)
        {
            CKVType type = c_types[i];
            stringstream ss;

            ss << c_keyValues[i] << '\t' << (int)type << '\t';
            switch (type)
            {
                case CKVType::Int_t: ss << c_iValues[m_NumTestType[0]++]; break;
                case CKVType::Float_t: ss << c_fValues[m_NumTestType[1]++]; break;
                case CKVType::String_t: ss << c_sValues[m_NumTestType[2]++]; break;
                default:
                    break;
            }
            ss << endl;

            testFile << ss.str();
            m_NumOfTestRec++;
        }
        testFile.close();
    }
}

CCfgMgrAppTester::~CCfgMgrAppTester()
{
    if (cfgMgr_p != NULL)
        delete cfgMgr_p;
}

void CCfgMgrAppTester::run_test(report type)
{
    // Run Tests storing results in object
    m_Rslt_load_Config = Test_load_Config();
    m_Rslt_load_Custom_Data = Test_load_Custom_Data();
    m_Rslt_get_Data_Keys = Test_get_Data_Keys();
    m_Rslt_get_Data_Value = Test_get_Data_Value();
    m_Rslt_set_Data_Value = Test_set_Data_Value();
    m_Rslt_save_Custom_Data = Test_save_Custom_Data(); // Must be last for test to work

    if (type == report::verbose)
    {
        std::cout << std::endl << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Config Manager Test START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        std::cout << "Mgr Test" << std::endl;
        std::cout << "\tfailed: " << failed() << "/" << num_tests() << std::endl;
        std::cout << "\tpassed: " << passed() << "/" << num_tests() << std::endl;
        std::string rslt = tests_all_passed() ? std::string("OK") : std::string("ERROR");
        std::cout << "\tMgr Test = " << rslt << std::endl;
        std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Config Manager Test END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    }
}

int CCfgMgrAppTester::passed()
{
    int rslt = 0;

    // Tally all tests that passed
    rslt += m_Rslt_load_Config ? 1 : 0;
    rslt += m_Rslt_load_Custom_Data ? 1 : 0;
    rslt += m_Rslt_get_Data_Keys ? 1 : 0;
    rslt += m_Rslt_get_Data_Value ? 1 : 0;
    rslt += m_Rslt_set_Data_Value ? 1 : 0;
    rslt += m_Rslt_save_Custom_Data ? 1 : 0;

    return rslt;
}

int CCfgMgrAppTester::failed()
{
    int rslt = 0;

    // Tally all tests that failed
    rslt += m_Rslt_load_Config ? 0 : 1;
    rslt += m_Rslt_load_Custom_Data ? 0 : 1;
    rslt += m_Rslt_get_Data_Keys ? 0 : 1;
    rslt += m_Rslt_get_Data_Value ? 0 : 1;
    rslt += m_Rslt_set_Data_Value ? 0 : 1;
    rslt += m_Rslt_save_Custom_Data ? 0 : 1;

    return rslt;
}

bool CCfgMgrAppTester::Test_load_Config()
{
    bool rslt = false;

    // TODO : Complete test

    return rslt;
}

bool CCfgMgrAppTester::Test_load_Custom_Data()
{
    bool rslt = false;

    m_handle = cfgMgr_p->loadCustomData(m_TestFile);
    if (m_handle != -1)
    {
        // Confirm that it has the total records == m_NumOfTestRec
        vector<CCfgMgrApplication::AKey_t> keyList;
        cfgMgr_p->getDataKeys(m_handle, keyList);
        if (keyList.size() == m_NumOfTestRec)
        {
            rslt = true;
        }
    }

    return rslt;
}


bool CCfgMgrAppTester::Test_get_Data_Keys()
{
    bool rslt = false;
    int keyFound = 0;
    int testType[3] = { 0,0,0 };

    vector<CCfgMgrApplication::AKey_t> keyList;
    cfgMgr_p->getDataKeys(m_handle, keyList);

    // Validate the keyList agains hardcoded values
    for (int i = 0; i < keyList.size() ; i++)
    {
        CCfgMgrApplication::AKey_t key = keyList[i];

        if (key.first == c_keyValues[i] && key.second == c_types[i])
        {
            switch (key.second)
            {
                case CKVType::Int_t: testType[0]++; break;
                case CKVType::Float_t: testType[1]++; break;
                case CKVType::String_t: testType[2]++; break;
                default:
                    break;
            }
        }
    }

    // Final test is key found = total records written as all were valid
    if (testType[0] == m_NumTestType[0]
        && testType[1] == m_NumTestType[1]
        && testType[2] == m_NumTestType[2]
        && (testType[0] + testType[1] + testType[2]) == m_NumOfTestRec)
    {
        rslt = true;
    }

    return rslt;
}


bool CCfgMgrAppTester::Test_get_Data_Value()
{
    bool rslt = false;
    int ValidCount = 0;

    vector<CCfgMgrApplication::AKey_t> keyList;
    cfgMgr_p->getDataKeys(m_handle, keyList);

    // Validate the keyList agains hardcoded values
    for (int i = 0; i < keyList.size(); i++)
    {
        CCfgMgrApplication::AKey_t key = keyList[i];
        CKeyValue val(key.second);

        if (cfgMgr_p->getDataValue(m_handle, key, val))
        {
            // REALLY HARDCODE AND WILL BREAK EASILY - DO SOMETHING BETTER AS TIME PERMITS
            if (key.first == c_keyValues[i]
                && val.getType() == key.second)
            {
                if (i < 6) // these are ints 
                {
                    if (val.getValueInt() == c_iValues[i])
                    {
                    ValidCount++;
                    }
                }
                else if (i == 6) // one float
                {
                    if (val.getValueFloat() == c_fValues[i-6])
                    {
                        ValidCount++;
                    }
                }
                else // all the rest are strings
                {
                    if (val.getValueString() == c_sValues[i-7])
                    {
                        ValidCount++;
                    }
                }
            }
        }
    }

    if (ValidCount == m_NumOfTestRec)
    {
        rslt = true;
    }

    return rslt;
}


bool CCfgMgrAppTester::Test_set_Data_Value()
{
    bool rslt = false;
    int setCount = 0;

    vector<CCfgMgrApplication::AKey_t> keyList;
    cfgMgr_p->getDataKeys(m_handle, keyList);

    for (CCfgMgrApplication::AKey_t key : keyList)
    {
        CKeyValue Val(key.second);
        if (cfgMgr_p->getDataValue(m_handle, key, Val))
        {
            // Based on type we will set a new hardcoded value of the current as negative and string val backwards
            switch (key.second)
            {
                case CKVType::Int_t: Val.setValue(-Val.getValueInt()); break;
                case CKVType::Float_t: Val.setValue(-Val.getValueFloat()); break;
                case CKVType::String_t:
                    string toFlip = Val.getValueString();
                    reverse(toFlip.begin(), toFlip.end());
                    Val.setValue(toFlip);
                    break;
            }

            if (cfgMgr_p->setDataValue(m_handle, key, &Val))
            {
                setCount++;
            }
        }
    }

    // Check all the keys and check that they are all negative values and reverse string
    // Validate the keyList agains hardcoded values
    for (int i = 0; i < keyList.size(); i++)
    {
        CCfgMgrApplication::AKey_t key = keyList[i];
        CKeyValue Val(key.second);

        if (cfgMgr_p->getDataValue(m_handle, key, Val))
        {
            switch (key.second)
            {
            case CKVType::Int_t: 
                if (Val.getValueInt() <= (int)0)
                    setCount--;
                break;
            case CKVType::Float_t: 
                if (Val.getValueFloat() <= (float)0.0)
                    setCount--;
                break;
            case CKVType::String_t: 
                string toFlip = Val.getValueString();
                reverse(toFlip.begin(), toFlip.end());
                if (toFlip == c_sValues[i - 7])
                    setCount--;
                break;
            }
        }
    }

    if (setCount == 0)
    {
        rslt = true;
    }

    return rslt;
}


bool CCfgMgrAppTester::Test_save_Custom_Data()
{
    bool rslt = false;
    int setCount = 0;

    // Dependent on the fact that we have run the last test
    if (!cfgMgr_p->saveCustomData(m_handle))
    {
        return rslt;
    }

    m_handle = cfgMgr_p->loadCustomData(m_TestFile);
    if (m_handle != -1)
    {
        vector<CCfgMgrApplication::AKey_t> keyList;
        cfgMgr_p->getDataKeys(m_handle, keyList);

        // Check all the keys and check that they are all negative values and reverse string
        // Validate the keyList agains hardcoded values
        for (int i = 0; i < keyList.size(); i++)
        {
            CCfgMgrApplication::AKey_t key = keyList[i];
            CKeyValue Val(key.second);

            if (   key.first == c_keyValues[i]
                && key.second == c_types[i]
                && cfgMgr_p->getDataValue(m_handle, key, Val))
            {
                switch (key.second)
                {
                case CKVType::Int_t:
                    if (Val.getValueInt() <= (int)0)
                        setCount++;
                    break;
                case CKVType::Float_t:
                    if (Val.getValueFloat() <= (float)0.0)
                        setCount++;
                    break;
                case CKVType::String_t:
                    string toFlip = Val.getValueString();
                    reverse(toFlip.begin(), toFlip.end());
                    if (toFlip == c_sValues[i - 7])
                        setCount++;
                    break;
                }
            }
        }
    }

    if (setCount == m_NumOfTestRec)
    {
        rslt = true;
    }

    return rslt;
}
