//============================================================================
// Configuration Manager - Application Class Interface
//
// Defines Application Layer Class for the Configuration Manager subsystem.
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

#ifndef __ConfigurationManager__CfgMgrApplication__
#define __ConfigurationManager__CfgMgrApplication__

#include <stdio.h>
#include <vector>
#include <string>

enum class CMGameObjType : char {
    sound, input, shader, texture, model, entity, score, highscore };
enum class CKVType : char { 
    Int_t, Float_t, String_t, Block_t };

template< class T >
class CKeyValue
{
public:
    CKeyValue(T Type)
    {
        if (std::is_same<T, int> : value) { type_m = CKVType:Int_t; }
        else if (std::is_same<T, float> : value) { type_m = CKVType:Float_t; }
        else if (std::is_same<T, std::string> : value) { type_m = CKVType:String_t; }
        else if (std::is_same<T, char*> : value) { type_m = CKVType:Block_t; }
        else { static_assert(false, "Type must be int, float, std::string, or char*"); }
    }

    inline CKVType getType() { return _type; } 
    inline void setValue(T newValue, int size) { value_m = newValue; size_m = size; } 
    inline T getValue() { return value_m; } 

private:
    CKVType type_m;
    T value_m;
    int size_m;
};

class CCfgMgrApplication
{
public:
    CCfgMgrApplication();
    ~CCfgMgrApplication();

    // Mandatory formatted data files
 
     // Load the inital game configuration
    bool loadConfig();

//
// TODO: Define the object type classes derived from a base so gameobject is returned or something similar
// 
     // Get a game object of type.
    void* getGameObjByType(CMGameObjType type, int index);
     // Get all game objects of type.
    bool getAllGameObjByType(CMGameObjType type, std::vector<void*>& emptyList);

    // CUSTOM Data files
 
     // Load a custom data file to get a handle to the file contents (file must be in XML format)
    int loadCustomData(std::string filename);
    bool saveCustomData(int fileHandle);
     // Retrieve the keys found in a loaded file
    bool getDataKeys(int fileHandle, std::vector<std::pair<std::string, CKVType>>& emptyList);
     // Using a key pair get the data. Client responsible for correct CKeyValue
    bool getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<int>& theValue);
    bool getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<float>& theValue);
    bool getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<std::string>& theValue);
    bool getDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<char*>& theValue);
     // Stash the keys found in a loaded file. Client responsible for correct CKeyValue
    bool setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<int>& newValue);
    bool setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<float>& newValue);
    bool setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<std::string>& newValue);
    bool setDataValue(int fileHandle, std::pair<std::string, CKVType> key, CKeyValue<char*>& newValue);

private:
    std::vector<std::pair<int, std::string>> fileHandles;
};

#endif /* defined(__ConfigurationManager__CfgMgrApplication__) */
