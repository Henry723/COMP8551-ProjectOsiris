
#include "TinyXMLWrapper.h"

TinyXMLWrapper::TinyXMLWrapper(const char* fileName)
{
    tinyxml2::XMLError xmlerr = doc.LoadFile(fileName);

    if (xmlerr != tinyxml2::XML_SUCCESS)
    {
        cerr << "Could not read game object XML file!" << endl;
    }
}

void TinyXMLWrapper::PrintDocument()
{
    doc.Print(); //Print out the whole XML Doc
}

void TinyXMLWrapper::test()
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

const char* TinyXMLWrapper::getAttribute(const char* name, const char* attribute)
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
