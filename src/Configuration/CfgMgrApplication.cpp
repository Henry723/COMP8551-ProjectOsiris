//============================================================================
// Configuration Manager - Application Class Implementation
//
// Implements Application Layer Class for the Configuration Manager subsystem.
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

#include "CfgMgrApplication.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


CCfgMgrApplication::CCfgMgrApplication()
    : handleCount(0)
{
    // Add anything extra
}

CCfgMgrApplication::~CCfgMgrApplication()
{
    // Add anything extra
}

bool CCfgMgrApplication::loadConfig(std::string fileName)
{
    // TODO : Finish Implementation
    return false;
}

int CCfgMgrApplication::loadCustomData(std::string fileName)
{
    int newHndl = -1;
    ifstream cfgFile;

    // Open the file
    cfgFile.open(fileName, fstream::in);

    // If success add the file info to filehandles
    if (cfgFile)
    {
        handleData_t fileData;
        string rawLnData;

        fileData.handle = ++handleCount;
        fileData.fileName = fileName;

        // Populate the file data by reading through the file (one time read)
        while (getline(cfgFile, rawLnData))
        {
            // Parse through the line using proper format { keyName, type, value }
            keyInfo_t keyInfo;
            string type;
            istringstream data(rawLnData);
            stringstream ss;
            getline(data, keyInfo.keyName, '\t');
            getline(data, type, '\t');
            ss << type;
            int itype;
            ss >> itype;
            keyInfo.type = (CKVType)itype;
            getline(data, keyInfo.value, '\t');

            // Push to the vector the key record if it is a valid type
            if (keyInfo.type >= CKVType::Int_t && keyInfo.type <= CKVType::String_t)
            {
                fileData.keys.push_back(keyInfo);
            }
        }

        // close the file and return the file handle reference to client
        cfgFile.close();
        fileData.isLoaded = true;
        fileHandles.push_back(fileData);
        newHndl = fileData.handle;
    }

    return newHndl;
}

// Re-writes the file (if records dirty) with current values
bool CCfgMgrApplication::saveCustomData(int fileHandle)
{
    bool rslt = false;

    // Lookup the key records for file handle
    int fDIdx = findRecordSet(fileHandle);

    // Did we find a handle thats loaded and dirty?
    if (fDIdx != -1 && fileHandles[fDIdx].isDirty)
    {
        fstream cfgFile;

        // Open the file erasing content and for output
        cfgFile.open(fileHandles[fDIdx].fileName, fstream::out | fstream::trunc);

        // If success add the file info to filehandles
        if (cfgFile)
        {
            // Loop through the key records writing each to file as a line
            for (auto aKey : fileHandles[fDIdx].keys)
            {
                stringstream rawLnData;

                rawLnData << aKey.keyName << '\t' << (int)aKey.type << '\t' << aKey.value << endl;
                cfgFile << rawLnData.str();
            }

            // close the file and return the result
            cfgFile.close();
            fileHandles[fDIdx].isDirty = false; // Now that saved they are no longer dirty
            rslt = true;
        }
    }

    return rslt;
}


bool CCfgMgrApplication::getDataKeys(int fileHandle, std::vector<AKey_t>& emptyList)
{
    bool rslt = false;
    int fDIdx = findRecordSet(fileHandle);

    // Did we find a handle thats loaded?
    if (fDIdx != -1)
    {
        // Loop through all the key records adding them to the return vector
        for (auto key : fileHandles[fDIdx].keys)
        {
            AKey_t AKey;
            AKey.first = key.keyName;
            AKey.second = key.type;

            // Push to the provided emptyList
            emptyList.push_back(AKey);
        }
        rslt = true;
    }

    return rslt;
}

bool CCfgMgrApplication::getDataValue(int fileHandle, AKey_t key, CKeyValue& value)
{
    bool rslt = false;

    // Lookup the key records for file handle
    int fDIdx = findRecordSet(fileHandle);

    // Did we find a handle thats loaded?
    if (fDIdx != -1)
    {
        // Search through key records for matching key
        for (auto aKey : fileHandles[fDIdx].keys)
        {
            if (aKey.keyName == key.first)
            {
                // Key found so copy over values
                CKeyValue rtrnVal(key.second);

                rtrnVal.setValue(aKey.value);
                value = rtrnVal;
                rslt = true;
                break;
            }
        }
    }

    return rslt;
}

bool CCfgMgrApplication::setDataValue(int fileHandle, AKey_t key, CKeyValue* pNewValue)
{
    bool rslt = false;

    // Lookup the key records for file handle
    int fDIdx = findRecordSet(fileHandle);

    // Did we find a handle thats loaded?
    if (fDIdx != -1)
    {
        // Search through key records for matching key
        for (int i = 0; i < fileHandles[fDIdx].keys.size() ; i++)
        {
            if (   fileHandles[fDIdx].keys[i].keyName == key.first 
                && fileHandles[fDIdx].keys[i].type == key.second)
            {
                // Key found so copy over values
                fileHandles[fDIdx].keys[i].value = pNewValue->getValueString();
                fileHandles[fDIdx].isDirty = true;
                rslt = true;
                break;
            }
        }

        // If no key was found then create a new one and add it.
        if (rslt == false)
        {
            // Add as a new key
            keyInfo_t newKey;
            newKey.keyName = key.first;
            newKey.type = key.second;
            newKey.value = pNewValue->getValueString();
            fileHandles[fDIdx].keys.push_back(newKey);

            // Very important to update dirty or it will never save
            fileHandles[fDIdx].isDirty = true; 
            rslt = true;
        }
    }

    return rslt;
}


int CCfgMgrApplication::findRecordSet(int fileHandle)
{
    int fDIdx = -1;

    // Try and find the handle data
    for (int i = 0; i < fileHandles.size(); i++)
    {
        if (fileHandles[i].handle == fileHandle)
        {
            if (fileHandles[i].isLoaded)
            {
                // Have a matching handle so break and process
                fDIdx = i;
            }
            break;
        }
    }

    return fDIdx;
}

