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

/*
    READ HERE if you want to add functionality for adding new components from XML.
    The loop in the following function will iterate over all entities and add them to the entity manager.
    Basically, it will check each element for all components and add them if the XML has a specification for it.

    For example:
        This line looks for a model3D element in the game object's XML:
            tinyxml2::XMLElement* model_data = object->FirstChildElement("model3D");

        If it exists, this line will call the function below to create it:
            if (model_data) e.assign<Model3D>(GetModel3DComponent(model_data));

    Each different component should have its own function so as not to clutter the LoadObjects function.

    For formatting the XML, try to store as much data as attributes as possible.
    This makes it easier to retrive data of different types from the element.

    For example:
        tinyXML has a few different Attribute functions

        element.Attribute("name")       Returns a const char*    
        element.BoolAttribute("name")   Returns a bool
        element.FloatAttribute("name")  Returns a float
    
        Where element is the XML element and "name" is the name of the attribute.

    Of course, for things vector data (position, transform, etc), it gets cumbersome to store as attributes.
    This is where we would more likely use the actual element data

        For <position>1, 1.5, 1</position>

        We could use:
            tinyxml2::XMLElement* position_data = gameobject->FirstChildElement("position");
            glm::vec3 position = ParseVec3(position_data->GetText());

        I've created a few convenience functions below for parsing elements into glm vectors.

    So, step by step:
    1. Create your component in components.hpp
    2. Add the data for those components into Game.xml on each entity that needs it.
    3. Create a new function for parsing the XML into a component.
        3a. Using elements, make sure the data is correctly parsed from the const char* its retrieved as.
        3b. Using attributes, make sure to use the right Attribute function to get the data.
        3c. Return the constructed component
    4. Add the returned component to the entity.

    Any questions or issues, please reach out to Kris and he'll lend a hand.
*/
void CCfgMgrPhysical::LoadObjects(EntityManager& em)
{
    //Grab root game element
    tinyxml2::XMLElement* game = doc.FirstChildElement("game");

    //Iterate over entities
    tinyxml2::XMLElement* entity = game->FirstChildElement("entity");
    while(entity)
    {
        //Iterate over game objects
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
            if (model_data) e.assign<Model3D>(GetModel3DComponent(model_data));

            //Check for transform data and add if exists
            tinyxml2::XMLElement* transform_data = object->FirstChildElement("transform");
            if (transform_data) e.assign<Transform>(GetTransformComponent(transform_data));

            //Check for rigidbody data and add if exists
            tinyxml2::XMLElement* rigidbody_data = object->FirstChildElement("rigidbody");
            if (rigidbody_data) e.assign<Rigidbody>(GetRigidbodyComponent(rigidbody_data));
            
            //Advance to next game object
            object = object->NextSiblingElement("gameobject");
        }
        //advance to next entity.
        entity = entity->NextSiblingElement("entity");
    }
}

Model3D CCfgMgrPhysical::GetModel3DComponent(tinyxml2::XMLElement* data)
{
    tinyxml2::XMLElement* model_src = data->FirstChildElement("model_src");
    tinyxml2::XMLElement* text_src = data->FirstChildElement("text_src");
    tinyxml2::XMLElement* vert_src = data->FirstChildElement("vert_src");
    tinyxml2::XMLElement* frag_src = data->FirstChildElement("frag_src");
    //Make sure all neccesary data is present.
    return Model3D(model_src->GetText(),
        vert_src->GetText(),
        frag_src->GetText(),
        text_src->GetText());
}

Transform CCfgMgrPhysical::GetTransformComponent(tinyxml2::XMLElement* data)
{
    tinyxml2::XMLElement* position = data->FirstChildElement("position");
    tinyxml2::XMLElement* scale = data->FirstChildElement("scale");
    tinyxml2::XMLElement* rotation = data->FirstChildElement("rotation");

    return Transform(ParseVec3(position->GetText()),
                    ParseVec4(rotation->GetText()),
                    ParseVec3(scale->GetText()));
}


Rigidbody CCfgMgrPhysical::GetRigidbodyComponent(tinyxml2::XMLElement* data)
{
    tinyxml2::XMLElement* position = data->FirstChildElement("position");
    tinyxml2::XMLElement* colliders = data->FirstChildElement("colliders");
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
    const char* type = data->Attribute("type");
    Rigidbody::ColliderType type_value = Rigidbody::ColliderType::PLAYER;
    if (strcmp(type, "player") == 0) type_value = Rigidbody::ColliderType::PLAYER;
    else if (strcmp(type, "enemy") == 0) type_value = Rigidbody::ColliderType::ENEMY;
    else if (strcmp(type, "collectible") == 0) type_value = Rigidbody::ColliderType::COLLECTIBLE;
    else if (strcmp(type, "wall") == 0) type_value = Rigidbody::ColliderType::WALL;
    else if (strcmp(type, "player attack") == 0) type_value = Rigidbody::ColliderType::PLAYER_ATTACK;
    else if (strcmp(type, "enemy attack") == 0) type_value = Rigidbody::ColliderType::ENEMY_ATTACK;

    //Make sure all neccesary data is present.
    return Rigidbody(collider_list, type_value, position_value);
}

glm::vec2 CCfgMgrPhysical::ParseVec2(const char* data)
{
    vector<float> vals;
    char* split = strtok((char*)data, ",");
    while (split != NULL)
    {
        vals.push_back(std::stof(std::string(split)));
        split = strtok(NULL, ",");
    }
    return glm::vec2(vals[0], vals[1]);
}

glm::vec3 CCfgMgrPhysical::ParseVec3(const char* data)
{
    vector<float> vals;
    char* split = strtok((char*)data, ",");
    while (split != NULL)
    {
        vals.push_back(std::stof(std::string(split)));
        split = strtok(NULL, ",");
    }
    return glm::vec3(vals[0], vals[1], vals[2]);
}

glm::vec4 CCfgMgrPhysical::ParseVec4(const char* data)
{
    vector<float> vals;
    char* split = strtok((char*)data, ",");
    while (split != NULL)
    {
        vals.push_back(std::stof(std::string(split)));
        split = strtok(NULL, ",");
    }
    return glm::vec4(vals[0], vals[1], vals[2], vals[3]);
}
