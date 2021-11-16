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

void CCfgMgrPhysical::LoadObjects(EntityManager& em)
{
    tinyxml2::XMLElement* game = doc.FirstChildElement("game");
    tinyxml2::XMLElement* entity = game->FirstChildElement("entity");
    while(entity)
    {
        tinyxml2::XMLElement* object = entity->FirstChildElement("gameobject");
        while (object)
        {
            //Create empty entity
            Entity e = em.create();
            
            //Check for name, add game object if it exists
            const char* object_name = object->Attribute("name");
            if (object_name) e.assign<GameObject>(object_name);

            //Check for model data and add if exists
            tinyxml2::XMLElement* model_data = object->FirstChildElement("model3D");
            if (model_data)
            {
                tinyxml2::XMLElement* model_src = model_data->FirstChildElement("model_src");
                tinyxml2::XMLElement* text_src = model_data->FirstChildElement("text_src");
                tinyxml2::XMLElement* vert_src = model_data->FirstChildElement("vert_src");
                tinyxml2::XMLElement* frag_src = model_data->FirstChildElement("frag_src");
                //Make sure all neccesary data is present.
                if (model_src && text_src && vert_src && frag_src) e.assign<Model3D>(model_src->GetText(), 
                                                                                    vert_src->GetText(), 
                                                                                    frag_src->GetText(), 
                                                                                    text_src->GetText());
            }

            //Check for transform data and add if exists
            tinyxml2::XMLElement* transform_data = object->FirstChildElement("transform");
            if (transform_data)
            {
                tinyxml2::XMLElement* position = transform_data->FirstChildElement("position");
                tinyxml2::XMLElement* scale = transform_data->FirstChildElement("scale");
                tinyxml2::XMLElement* rotation = transform_data->FirstChildElement("rotation");

                //Make sure all neccesary data is present.
                if (position && scale && rotation) e.assign<Transform>(ParseVec3(position->GetText()), 
                                                                        ParseVec4(rotation->GetText()), 
                                                                        ParseVec3(scale->GetText()));
            }

            //Check for rigidbody data and add if exists
            tinyxml2::XMLElement* rigidbody_data = object->FirstChildElement("rigidbody");
            if (rigidbody_data)
            {
                tinyxml2::XMLElement* position = rigidbody_data->FirstChildElement("position");
                tinyxml2::XMLElement* colliders = rigidbody_data->FirstChildElement("colliders");
                tinyxml2::XMLElement* collider = colliders->FirstChildElement("collider");

                //Parse position data
                glm::vec2 position_value = ParseVec2(position->GetText());

                //Iterate over collider elements
                vector<Collider> collider_list;
                while (collider)
                {
                    //Get expected pieces of a colliders
                    const char* shape = collider->Attribute("shape");
                    bool sensor = collider->BoolAttribute("sensor");
                    const char* size = collider->Attribute("size");
                    const char* name = collider->Attribute("name");

                    //Determine collider shape, default to circle
                    Collider::Shape shape_value = Collider::Shape::CIRCLE;
                    if (strcmp(shape, "circle") == 0) shape_value = Collider::Shape::CIRCLE;
                    else if (strcmp(shape, "box") == 0) shape_value = Collider::Shape::BOX;

                    //Get actual values from XML data
                    float size_value = std::stof((char*)size);
                    glm::vec2 position_value = ParseVec2(collider->GetText());
                    
                    //Create the collider and add it to a vector
                    collider_list.push_back(Collider(shape_value, position_value, sensor, size_value, name));
                    collider = collider->NextSiblingElement("collider");
                }

                //Determine the rigidbody type, default to player which is a dynamic body
                const char* type = rigidbody_data->Attribute("type");
                Rigidbody::ColliderType type_value = Rigidbody::ColliderType::PLAYER;
                if (strcmp(type, "player") == 0) type_value = Rigidbody::ColliderType::PLAYER;
                else if (strcmp(type, "enemy") == 0) type_value = Rigidbody::ColliderType::ENEMY;
                else if (strcmp(type, "collectible") == 0) type_value = Rigidbody::ColliderType::COLLECTIBLE;
                else if (strcmp(type, "wall") == 0) type_value = Rigidbody::ColliderType::WALL;
                else if (strcmp(type, "player attack") == 0) type_value = Rigidbody::ColliderType::PLAYER_ATTACK;
                else if (strcmp(type, "enemy attack") == 0) type_value = Rigidbody::ColliderType::ENEMY_ATTACK;

                //Make sure all neccesary data is present.
                if (position && type) e.assign<Rigidbody>(collider_list, type_value, position_value);
            }
            object = object->NextSiblingElement("gameobject");
        }
        entity = entity->NextSiblingElement("entity");
    }
}

glm::vec2 CCfgMgrPhysical::ParseVec2(const char* data)
{
    float vals[2];
    int index = 0;
    char* split = strtok((char*)data, ",");
    while (split != NULL)
    {
        vals[index++] = std::stof(std::string(split));
        split = strtok(NULL, ",");
    }
    return glm::vec2(vals[0], vals[1]);
}

glm::vec3 CCfgMgrPhysical::ParseVec3(const char* data)
{
    float vals[3];
    int index = 0;
    char* split = strtok((char*)data, ",");
    while (split != NULL)
    {
        vals[index++] = std::stof(std::string(split));
        split = strtok(NULL, ",");
    }
    return glm::vec3(vals[0], vals[1], vals[2]);
}

glm::vec4 CCfgMgrPhysical::ParseVec4(const char* data)
{
    float vals[4];
    int index = 0;
    char* split = strtok((char*)data, ",");
    while (split != NULL)
    {
        vals[index++] = std::stof(std::string(split));
        split = strtok(NULL, ",");
    }
    return glm::vec4(vals[0], vals[1], vals[2], vals[3]);
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

const char* CCfgMgrPhysical::GetElement(const char* name)
{
    return "";
}