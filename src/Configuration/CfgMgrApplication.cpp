//============================================================================
// Configuration Manager - Application Class Implementation
//
// Implements Application Layer Class for the Configuration Manager subsystem.
//
// The Configuration Manager Subsystem purpose is to read external source(s)
// information containing game objects data backand forth through 4 layers.
// The four layers and their purpose are defined as :
//
//  Layer 1. Application : Implements the API between its clients and the
//                         link between the presentation layers.It hides
//                         from clients any management or access for
//                         configuration. Configuration access is with the
//                         use of game objects of a type IDand a given
//                         instance of type.
//        2. Presentation : Implements the link between layer 1 and 3 by
//                          unwrapping app game objects to the data layer
//                          format or wrapping data layer data into usable
//                          layer 1 app objects.
//        3. Data : Implements the link between layer 2 and 4 containing a
//                  raw data format of game object collections.
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

CCfgMgrApplication::CCfgMgrApplication()
{
    // TODO : Finish Implementation
}

CCfgMgrApplication::~CCfgMgrApplication()
{
    // TODO : Finish Implementation
}

bool CCfgMgrApplication::loadConfig()
{
    // TODO : Finish Implementation
    return false;
}

void* CCfgMgrApplication::getGameObjByType(CMGameObjType type, int index)
{
    // TODO : Finish Implementation
    return nullptr;
}

bool CCfgMgrApplication::getAllGameObjByType(CMGameObjType type, std::vector<void*>& emptyList)
{
    // TODO : Finish Implementation
    return false;
}

int CCfgMgrApplication::loadCustomData(std::string filename)
{
    // TODO : Finish Implementation
    return 0;
}

bool CCfgMgrApplication::saveCustomData(int fileHandle)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::getDataKeys(int fileHandle, std::vector<std::pair<std::string, CKVType>>& emptyList)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<int>& theValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<float>& theValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue <std::string> & theValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<char*>& theValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<int>& newValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<float>& newValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<std::string>& newValue)
{
    // TODO : Finish Implementation
    return false;
}

bool CCfgMgrApplication::setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<char*>& newValue)
{
    // TODO : Finish Implementation
    return false;
}
