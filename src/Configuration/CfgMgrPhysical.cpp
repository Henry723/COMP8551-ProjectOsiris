//============================================================================
// Configuration Manager - Physical Class Implementation
//
// Implements Physical Layer Class for the Configuration Manager subsystem.
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
//  NOTE: We are using XML as the physical protocol using TinyXML2 module
//        to do the work. So this is a wrapper as it should be. See the 
//        TinyXML2 module for appropriate credit of the work.
//
//  Created by Kristoffer Olsson
// 
//============================================================================

#include "CfgMgrPhysical.h"

CCfgMgrPhysical::CCfgMgrPhysical(const char* fileName)
{
    tinyxml2::XMLError xmlerr = doc.LoadFile(fileName);

    if (xmlerr != tinyxml2::XML_SUCCESS)
    {
        cerr << "Could not read game object XML file!" << endl;
    }
}

void CCfgMgrPhysical::PrintDocument()
{
    doc.Print(); //Print out the whole XML Doc
}

void CCfgMgrPhysical::test()
{
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< BEGIN TEST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    //Get first gameobject before iterating over it.
    tinyxml2::XMLElement* obj = doc.FirstChildElement("gameobject");
    while (obj)
    {
        cout << "Type of " << obj->Value() << endl;
        tinyxml2::XMLElement* child = obj->FirstChildElement("name");
        cout << "Object: " << child->GetText() << endl;
        //If an object has attributes, check em out
        tinyxml2::XMLElement* tempE = obj->FirstChildElement("attributes");
        if (tempE) {
            //Iterate over each attrib element of an object
            tempE = tempE->FirstChildElement("attrib");
            while (tempE)
            {
                //Process the normals of any object with normals.
                if (strcmp(tempE->FirstChildElement("name")->GetText(), "normal") == 0)
                {
                    tempE = tempE->FirstChildElement("data");
                    if (tempE) cout << tempE->GetText() << endl;
                }
                tempE = tempE->NextSiblingElement("attrib");
            }
        }
        obj = obj->NextSiblingElement("gameobject");
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< END TEST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
}

const char* CCfgMgrPhysical::getAttribute(const char* name, const char* attribute)
{
    tinyxml2::XMLElement* obj = doc.FirstChildElement("gameobject");
    while (obj)
    {
        tinyxml2::XMLElement* child = obj->FirstChildElement("name");
        if (strcmp(child->GetText(), name) == 0)
        {
            //Found the gameobject with the right name
            tinyxml2::XMLElement* e = obj->FirstChildElement("attributes");
            if (e)
            {
                //Object has attributes
                e = e->FirstChildElement("attrib");
                while (e)
                {
                    if (strcmp(e->FirstChildElement("name")->GetText(), attribute) == 0)
                    {
                        //Found the right attribute
                        e = e->FirstChildElement("data");
                        if (e) return e->GetText();
                        else return "No associated data";
                    }
                    e = e->NextSiblingElement("attrib");
                }
                return "No associated attribute";
            }
        }
        obj = obj->NextSiblingElement("gameobject");
    }
    return "Object not found";
}
