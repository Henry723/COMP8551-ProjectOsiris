//============================================================================
// Configuration Manager - Physical Class Implementation
//
// Implements Physical Layer Class for the Configuration Manager subsystem.
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
    model_imp = new ModelImporter();

    if (xmlerr != tinyxml2::XML_SUCCESS)
    {
        cerr << "Could not read game object XML file!" << endl;
    }
}

CCfgMgrPhysical::~CCfgMgrPhysical() {
    //cout << " Config Mng Physical is being deallocated " << endl;
    delete model_imp;
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
    while (entity)
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
            if (model_data) e.assign<Models3D>(GetModels3DComponent(model_data));

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

//Specific function for loading a whole level laid out by XML
void CCfgMgrPhysical::LoadLevel(EntityManager& em)
{
    //Grab root game element
    tinyxml2::XMLElement* game = doc.FirstChildElement("game");

    //Grab level element
    tinyxml2::XMLElement* level = game->FirstChildElement("level");
   
    //Grab level layout
    tinyxml2::XMLElement* layout = level->FirstChildElement("layout");

    //Grab first row
    tinyxml2::XMLElement* row = layout->FirstChildElement("row");
    
    //Spacing data to track how far apart tiles should be.
    //  Osiris has a spacing of 2, for example.
    int spacing_data = level->IntAttribute("tilespacing");
    int spacing = (spacing_data == 0) ? 1 : spacing_data;
    
    //Where, in relation to origin, should the level start populating.
    //  Default is 0, 0. Osiris starts at -2, -2
    int rowStart = level->IntAttribute("startX");
    int rowCount = rowStart;
    int colStart = level->IntAttribute("startY");
    int colCount = colStart;

    //Iterate over each level in the layout XML
    while (row)
    {
        //Symbols in the row.
        const char* symbols = row->GetText();
        int i = 0; //Track current symbol
        //Loop over each symbol
        while (symbols[i])
        {
            //First, create floor tile here
            tinyxml2::XMLElement* tileData = FindObject("tile", game);
            if (tileData && symbols[i] != 'W' && symbols[i] != '_') //excludes squares with walls and '_' for blank
                CreateEntityAtPosition(tileData, em, colCount * spacing, rowCount * spacing);

            //If the space isn't empty, place something
            if (symbols[i] != '-')
            {
                //Get the first key and start looping them
                tinyxml2::XMLElement* key = level->FirstChildElement("key");
                while (key)
                {
                    //If the key matches the symbol we're on...
                    if (symbols[i] == key->Attribute("id")[0])
                    {
                        //Find the corresponding object in the XML.
                        tinyxml2::XMLElement* objectData = FindObject(key->GetText(), game);
                        //If the object was found, create it.
                        if (objectData)
                            CreateEntityAtPosition(objectData, em, colCount * spacing, rowCount * spacing);
                        break; //Break if object was created.
                    }
                    //Check next key
                    key = key->NextSiblingElement("key");
                }
            }
            colCount++; //Increment column
            i++;        //Increment symbol tracker
        } 
        colCount = colStart; //Reset column
        rowCount++; //Increment row
        row = row->NextSiblingElement("row"); //Check next row.
    }
}

//Find a named game object as a child of root
tinyxml2::XMLElement* CCfgMgrPhysical::FindObject(const char* name, tinyxml2::XMLElement* root)
{
    //Get first gameobject
    tinyxml2::XMLElement* object = root->FirstChildElement("gameobject");
    while (object)
    {
        if (strcmp(object->Attribute("name"), name) == 0)
            return object; //Return if found.
        object = object->NextSiblingElement("gameobject"); //Check next object
    }
    return nullptr; //Not found, return null
}

//Create an entitiy at specified, 2D location.
void CCfgMgrPhysical::CreateEntityAtPosition(tinyxml2::XMLElement* data, EntityManager& em, int x, int y)
{
    //Create empty entity
    Entity e = em.create();

    //Check for name, add game object if it exists
    const char* object_name = data->Attribute("name");
    if (object_name) e.assign<GameObject>(object_name);

    //Check for model data and add if exists
    //tinyxml2::XMLElement* model_data = data->FirstChildElement("model3D");
    //if (model_data) e.assign<Model3D>(GetModel3DComponent(model_data));

    tinyxml2::XMLElement* models_data = data->FirstChildElement("model3D");
    if (models_data) e.assign<Models3D>(GetModels3DComponent(models_data));

    //Check for transform data and add if exists
    tinyxml2::XMLElement* transform_data = data->FirstChildElement("transform");
    if (transform_data) e.assign<Transform>(CreateTransformAtPosition(transform_data, x, y));
    
    //Check for rigidbody data and add if exists
    tinyxml2::XMLElement* rigidbody_data = data->FirstChildElement("rigidbody");
    if (rigidbody_data) e.assign<Rigidbody>(CreateRigidbodyAtPosition(rigidbody_data, x, y));

    //Check for commandflags data and add if exists
    tinyxml2::XMLElement* cmdflags_data = data->FirstChildElement("commandflags");
    if (cmdflags_data) e.assign<CommandFlags>(CreateCommandFlags(cmdflags_data));
    
    tinyxml2::XMLElement* anim_data = data->FirstChildElement("animator");
    if (anim_data) e.assign<Animator>(CreateAnimator(anim_data));

    if (tinyxml2::XMLElement * h = data->FirstChildElement("health")) e.assign<Health>(CreateHealth(h));
    
    //Check for point lights data and add if exists
    tinyxml2::XMLElement* pointlight_data = data->FirstChildElement("pointlight");
    if (pointlight_data) e.assign<PointLight>(CreatePointLight(pointlight_data));
}

Transform CCfgMgrPhysical::CreateTransformAtPosition(tinyxml2::XMLElement* data, int x, int y)
{
    tinyxml2::XMLElement* yDepth = data->FirstChildElement("yDepth");
    tinyxml2::XMLElement* scale = data->FirstChildElement("scale");
    tinyxml2::XMLElement* rotation = data->FirstChildElement("rotation");
    float depth = atof(yDepth->GetText());

    return Transform(glm::vec3(x, depth, y), 
                        ParseVec4(rotation->GetText()), 
                        ParseVec3(scale->GetText()));
}

Rigidbody CCfgMgrPhysical::CreateRigidbodyAtPosition(tinyxml2::XMLElement* data, int x, int y)
{
    tinyxml2::XMLElement* colliders = data->FirstChildElement("colliders");
    tinyxml2::XMLElement* collider = colliders->FirstChildElement("collider");

    //Parse position data
    glm::vec2 position_value = glm::vec2(x, y);

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

Models3D CCfgMgrPhysical::GetModels3DComponent(tinyxml2::XMLElement* data)
{
    //Support for multiple potential models on one entity.
    //Model files
    vector<Model3D> models3DContainer;

    tinyxml2::XMLElement* model_src = data->FirstChildElement("model_src");
    vector<string> models; //Vector to store model strings
    stringstream model_data(model_src->GetText()); //Stream in each string
    string m;
    while (getline(model_data, m, ',')) models.push_back(m); //Push models into vector


    //Texture files, same as models
    tinyxml2::XMLElement* text_src = data->FirstChildElement("text_src");
    vector<string> textures;
    stringstream texture_data(text_src->GetText());
    string t;
    while (getline(texture_data, t, ',')) textures.push_back(t);

    tinyxml2::XMLElement* vert_src = data->FirstChildElement("vert_src");
    tinyxml2::XMLElement* frag_src = data->FirstChildElement("frag_src");

    //add idle pose model first always
    int modelIndex = rand() % models.size();
    models3DContainer.push_back(Model3D(models[modelIndex].c_str(),
        vert_src->GetText(),
        frag_src->GetText(),
        textures[rand() % textures.size()].c_str(),
        model_imp));
	
    //Poses
    // If there are poses add them.
    if (data->IntAttribute("poseCount")) {
        vector<Model3D> poses;
        tinyxml2::XMLElement* pose_src = data->FirstChildElement("pose_src");
        vector<string> pose_models; //Vector to store model strings
        stringstream pose_model_data(pose_src->GetText()); //Stream in each string
        string m;
        while (getline(pose_model_data, m, ',')) pose_models.push_back(m); //Push models into vector
          
        for (int i = 0; i < data->IntAttribute("poseCount"); i++) {          
            models3DContainer.push_back(Model3D(pose_models[i].c_str(),
                vert_src->GetText(),
                frag_src->GetText(),
                textures[rand() % textures.size()].c_str(), model_imp));
        }

    }     
  
    return Models3D(models3DContainer);
}

Animator CCfgMgrPhysical::CreateAnimator(tinyxml2::XMLElement* data) {
    
    tinyxml2::XMLElement* anim_src = data->FirstChildElement("animation");
    vector<Animation> anims;
    while (anim_src) {    
        vector<Keyframe> kfs;
        tinyxml2::XMLElement* keyframe = anim_src->FirstChildElement("keyframe");

        while (keyframe) {
            float timestamp = keyframe->FloatAttribute("timestamp");
            int index = keyframe->IntAttribute("index");      
            kfs.push_back(Keyframe(timestamp, index));
            keyframe = keyframe->NextSiblingElement("keyframe");
        }

        std::string s(anim_src->Attribute("name"));
        float length = anim_src->FloatAttribute("length");
        anims.push_back(Animation(s, length, kfs));
        anim_src = anim_src->NextSiblingElement("animation");

    }
    return Animator(anims);

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

Health CCfgMgrPhysical::CreateHealth(tinyxml2::XMLElement* data)
{
  return Health(atoi(data->Attribute("amount")));
}

CommandFlags CCfgMgrPhysical::CreateCommandFlags(tinyxml2::XMLElement* data)
{
    return CommandFlags();
}

PointLight CCfgMgrPhysical::CreatePointLight(tinyxml2::XMLElement* data)
{
    tinyxml2::XMLElement* ambient = data->FirstChildElement("ambient");
    tinyxml2::XMLElement* diffuse = data->FirstChildElement("diffuse");
    tinyxml2::XMLElement* specular = data->FirstChildElement("specular");
    tinyxml2::XMLElement* constant = data->FirstChildElement("constant");
    tinyxml2::XMLElement* linear = data->FirstChildElement("linear");
    tinyxml2::XMLElement* quadratic = data->FirstChildElement("quadratic");
    tinyxml2::XMLElement* color = data->FirstChildElement("color");
    float pl_constant = atof(constant->GetText());
    float pl_linear = atof(linear->GetText());
    float pl_quadratic = atof(quadratic->GetText());

    return PointLight(glm::vec3(0, 0, 0),ParseVec3(ambient->GetText()),
        ParseVec3(diffuse->GetText()),
        ParseVec3(specular->GetText()),
        pl_constant,
        pl_linear,
        pl_quadratic,
        ParseVec3(color->GetText()));
}

glm::vec2 CCfgMgrPhysical::ParseVec2(const char* data)
{
    vector<float> vals;
    stringstream values(data);
    string v;
    while (getline(values, v, ',')) vals.push_back(stof(v));
    return glm::vec2(vals[0], vals[1]);
}

glm::vec3 CCfgMgrPhysical::ParseVec3(const char* data)
{
    vector<float> vals;
    stringstream values(data);
    string v;
    while (getline(values, v, ',')) vals.push_back(stof(v));
    return glm::vec3(vals[0], vals[1], vals[2]);
}

glm::vec4 CCfgMgrPhysical::ParseVec4(const char* data)
{
    vector<float> vals;
    stringstream values(data);
    string v;
    while (getline(values, v, ',')) vals.push_back(stof(v));
    return glm::vec4(vals[0], vals[1], vals[2], vals[3]);
}

//Element toString function created for testing.
void CCfgMgrPhysical::elementtostring(tinyxml2::XMLElement* element)
{
    tinyxml2::XMLElement* child = element->FirstChildElement();
    while (child)
    {
        elementtostring(child);
        child = child->NextSiblingElement();
    }
    cout << element->Name() << " --- ";
    if (element->GetText()) cout << element->GetText() << endl;
}