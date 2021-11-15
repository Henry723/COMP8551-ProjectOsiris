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

#ifndef __ConfigurationManager__CfgMgrApplication__
#define __ConfigurationManager__CfgMgrApplication__

#include <stdio.h>
#include <vector>
#include <string>

enum class CKVType : char { Int_t, Float_t, String_t };

//template< class T >
class CKeyValue
{
public:
    CKeyValue(CKVType type) : m_Type(type), m_RawValue(""), m_Size(0) { }

    inline CKVType getType() { return m_Type; } 
    inline void setValue(int newValue) { m_RawValue = std::to_string(newValue); m_Size = sizeof(int); }
    inline void setValue(float newValue) { m_RawValue = std::to_string(newValue); m_Size = sizeof(float); }
    inline void setValue(std::string newValue) { m_RawValue = newValue; m_Size = m_RawValue.size(); }
    inline int getValueInt() { return stoi(m_RawValue); }
    inline float getValueFloat() { return stof(m_RawValue); }
    inline std::string getValueString() { return m_RawValue; }

private:
    CKVType m_Type;
    std::string m_RawValue;
    int m_Size;
};

class CCfgMgrApplication
{
public:
    typedef std::pair<std::string, CKVType> AKey_t;

    CCfgMgrApplication();
    ~CCfgMgrApplication();

    // Mandatory formatted data files
 
     // Load the inital game configuration
    bool loadConfig(std::string fileName);

    // CUSTOM Data file Interface
 
     // Load a custom data file to get a handle to the file contents. Will create the file name if it doesn't exist.
    int loadCustomData(std::string fileName);
     // Saves the local changes to the file.
    bool saveCustomData(int fileHandle);
     // Retrieve the keys found in a loaded file

    bool getDataKeys(int fileHandle, std::vector<AKey_t>& emptyList);
     // Using a key pair get the data. Client responsible for correct CKeyValue
    bool getDataValue(int fileHandle, AKey_t key, CKeyValue& value);
     // Stash the keys found in a loaded file. Client responsible for correct CKeyValue
    bool setDataValue(int fileHandle, AKey_t key, CKeyValue* pNewValue);

private:
    struct keyInfo_t {
        std::string keyName;  
        CKVType type = CKVType::String_t;
        std::string value; 
    };
    struct handleData_t {
        int handle = -1; 
        std::string fileName; 
        bool isLoaded = false; 
        bool isDirty = false; 
        std::vector<keyInfo_t> keys;
    };
    std::vector<handleData_t> fileHandles;
    int handleCount;

    int findRecordSet(int fileHandle);
};

#endif /* defined(__ConfigurationManager__CfgMgrApplication__) */
