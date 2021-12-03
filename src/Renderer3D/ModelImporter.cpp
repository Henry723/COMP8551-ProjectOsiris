#include "ModelImporter.h"
#define TEST_MODELIMPORTER      false

string ModelImporter::key_from_filePath(const char* filePath) {
    string delimiter = "/";
    size_t pos = 0;
    string token;

    string command = filePath;
    vector<string> tokens = vector<string>();

    // While there are still tokens in the string...
    while ((pos = command.find(delimiter)) != std::string::npos) {

        // Grab the current token.
        token = command.substr(0, pos);

        // Push the token into the vector.
        tokens.push_back(token);

        // Erase the token at the given position.
        command.erase(0, pos + delimiter.length());
    }

    // The final token will input out of the loop
    token = command.substr(0, pos);

    tokens.push_back(token);

    command.erase(0, pos + delimiter.length());

    if (tokens.size() > 1) {
        return tokens[tokens.size() - 1];
    }

    else {
        return "";
    }
    
}

void ModelImporter::get_model(const char* modelSource, vector<float>& attribArray, vector<unsigned int>& indexArray) {

    string modelKey = key_from_filePath(modelSource);

    if (model_loaded(modelKey)) {
        //cout << modelSource << " loaded." << endl;
        //LoadedModelData& modelData = models[modelSource];

        attribArray = *models[modelKey]->attribs;
        indexArray = *models[modelKey]->indices;
    }
    else {
        //cout << "map does not contain key " << modelSource << endl;
        const char* def = "./src/Renderer3D/Models/cube.obj";

        //cout << " Default value mapped to " << models[def]->attribs->size() << endl;

        modelKey = key_from_filePath(def);

        attribArray = *models[modelKey]->attribs;
        indexArray = *models[modelKey]->indices;
    }
}

unsigned char* ModelImporter::get_texture(const char* texSource, int& width, int& height, int& nrChannels) {

    string textureKey = key_from_filePath(texSource);

    if (texture_loaded(textureKey)) {

        width = textures[textureKey]->imgWidth;
        height = textures[textureKey]->imgHeight;
        nrChannels = textures[textureKey]->nrChannels;

        return textures[textureKey]->textureData;

    }
    else {
        const char* def = "./src/Renderer3D/Textures/tex_floorTile1.jpg";
        textureKey = key_from_filePath(def);

        width = textures[textureKey]->imgWidth;
        height = textures[textureKey]->imgHeight;
        nrChannels = textures[textureKey]->nrChannels;

        return textures[textureKey]->textureData;
    }
}

void ModelImporter::populate_maps() {
    // Player
    const char* model_def = "./src/Renderer3D/Models/cube.obj";
    const char* tex_def = "./src/Renderer3D/Textures/tex_floorTile1.jpg";

    // Player
    const char* model_player = "./src/Renderer3D/Models/wayfarer.obj";
    const char* tex_player = "./src/Renderer3D/Textures/tex_wayfarer.jpg";
    
    // Player Poses
    const char* model_playerSwingPose1 = "./src/Renderer3D/Models/wayfarer_swingPose1.obj";
    const char* model_playerSwingPose2 = "./src/Renderer3D/Models/wayfarer_swingPose2.obj";
    const char* model_playerSwingPose3 = "./src/Renderer3D/Models/wayfarer_swingPose3.obj";

    // Enemy
    const char* model_enemy = "./src/Renderer3D/Models/scarab.obj";
    const char* tex_enemy = "./src/Renderer3D/Textures/tex_scarab.jpg";

    // Treasure
    const char* model_treasure = "./src/Renderer3D/Models/treasure_pile.obj";
    const char* tex_treasure = "./src/Renderer3D/Textures/tex_treasurePile.jpg";

    // Tiles
    const char* model_tile1 = "./src/Renderer3D/Models/stone_tile_1.obj";
    const char* model_tile2 = "./src/Renderer3D/Models/stone_tile_2.obj";
    const char* model_tile3 = "./src/Renderer3D/Models/stone_tile_3.obj";

    const char* tex_tile1 = "./src/Renderer3D/Textures/tex_floorTile1.jpg";
    const char* tex_tile2 = "./src/Renderer3D/Textures/tex_floorTile2.jpg";
    const char* tex_tile3 = "./src/Renderer3D/Textures/tex_floorTile3.jpg";

    // Walls
    const char* model_wall = "./src/Renderer3D/Models/stone_wall_1.obj";
    const char* tex_wall = "./src/Renderer3D/Textures/tex_wallTile1.jpg";

    // Doors & Keys
    const char* model_key = "./src/Renderer3D/Models/key_totem.obj";
    const char* tex_key = "./src/Renderer3D/Textures/tex_key_totem.jpg";
    const char* model_door = "./src/Renderer3D/Models/stone_door_closed_1.obj";
    const char* tex_door = "./src/Renderer3D/Textures/tex_Door.jpg";


    // Load all models, add them to the map.
    add_model_to_map(model_def);
    add_model_to_map(model_player);
    add_model_to_map(model_enemy);
    add_model_to_map(model_treasure);
    add_model_to_map(model_tile1);
    add_model_to_map(model_tile2);
    add_model_to_map(model_tile3);
    add_model_to_map(model_wall);
    add_model_to_map(model_key);
    add_model_to_map(model_door);
    add_model_to_map(model_playerSwingPose1);
    add_model_to_map(model_playerSwingPose2);
    add_model_to_map(model_playerSwingPose3);

    // Load all textures, add them to the map
    add_texture_to_map(tex_def);
    add_texture_to_map(tex_player);
    add_texture_to_map(tex_enemy);
    add_texture_to_map(tex_treasure);
    add_texture_to_map(tex_tile1);
    add_texture_to_map(tex_tile2);
    add_texture_to_map(tex_tile3);
    add_texture_to_map(tex_wall);
    add_texture_to_map(tex_key);
    add_texture_to_map(tex_door);

}

void ModelImporter::add_texture_to_map(const char* texSource) {

    LoadedTextureData* textureData = new LoadedTextureData(texSource);

    //int imgWidth, imgHeight, nrChannels;
    //stbi_set_flip_vertically_on_load(false); // accounts for conversion between 1.0y and 0.0y to prevent upside-down textures.

    //unsigned char* textureData = stbi_load(texSource, &imgWidth, &imgHeight, &nrChannels, 0);

    // Assign the newly loaded values to the target reference.
    string textureKey = key_from_filePath(texSource);
    textures.insert(pair<string, LoadedTextureData*>(textureKey, textureData));
}

void ModelImporter::add_model_to_map(const char* modelSource) {
    Assimp::Importer import;

    // TEMP - load simple plane model
    //const char* tempModelSource = "./src/Renderer3D/Models/cube.obj";
    const aiScene* scene = import.ReadFile(modelSource, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }

    //cout << "Adding " << modelSource << endl;

    // This is a new collection of attributes and indices.
    vector<float> loadedAttribs = vector<float>();
    vector<unsigned int> loadedIndices = vector<unsigned int>();

    processNodeAttributes(scene->mRootNode, scene, loadedAttribs);

    processNodeIndices(scene->mRootNode, scene, loadedIndices);

    //cout << " After model load: " << 

    // Create a new struct to store the model data and insert it into the map.
    LoadedModelData* modelData = new LoadedModelData(loadedAttribs, loadedIndices);

    // Assign the newly loaded values to the target reference.
    string modelKey = key_from_filePath(modelSource);
    models.insert(pair<string, LoadedModelData*>(modelKey, modelData));

    /*cout << "In Importer: AFTER load complete: " << models[modelSource]->attribs->size() << endl;
    cout << "In Importer: AFTER load complete: " << models[modelSource]->indices->size() << endl;
    cout << "=====================================" << endl;*/
    
}



bool ModelImporter::model_loaded(string modelKey)
{
    return models.count(modelKey); // will be 1 if in the map
}

bool ModelImporter::texture_loaded(string texKey)
{
    return textures.count(texKey); // will be 1 if in the map
}

void ModelImporter::processNodeAttributes(aiNode* node, const aiScene* scene, vector<float>& attribArray)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(processMesh(mesh, scene));

        // Here we try to grab all data from all the mesh
        processMeshAttributes(mesh, scene, attribArray);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodeAttributes(node->mChildren[i], scene, attribArray);
    }
}

void ModelImporter::processNodeIndices(aiNode* node, const aiScene* scene, vector<unsigned int>& indexArray)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(processMesh(mesh, scene));

        // Here we try to grab all data from all the mesh
        processMeshIndices(mesh, scene, indexArray);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodeIndices(node->mChildren[i], scene, indexArray);
    }
}

void ModelImporter::processMeshAttributes(aiMesh* mesh, const aiScene* scene, vector<float>& attributes)
{
    // data to fill
    //vector<float> attributes;//contain position, texture coordiates
    //vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        //Vertex vertex;
        glm::vec3 position; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;

        attributes.push_back(position.x);
        attributes.push_back(position.y);
        attributes.push_back(position.z);

        //normals
        if (mesh->HasNormals())
        {
            glm::vec3 normals;
            normals.x = mesh->mNormals[i].x;
            normals.y = mesh->mNormals[i].y;
            normals.z = mesh->mNormals[i].z;

            attributes.push_back(normals.x);
            attributes.push_back(normals.y);
            attributes.push_back(normals.z);

        }

        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 texCoordinates;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            texCoordinates.x = mesh->mTextureCoords[0][i].x;
            texCoordinates.y = mesh->mTextureCoords[0][i].y;

            attributes.push_back(texCoordinates.x);
            attributes.push_back(texCoordinates.y);

            //// tangent
            //vector.x = mesh->mTangents[i].x;
            //vector.y = mesh->mTangents[i].y;
            //vector.z = mesh->mTangents[i].z;
            //vertex.Tangent = vector;
            //// bitangent
            //vector.x = mesh->mBitangents[i].x;
            //vector.y = mesh->mBitangents[i].y;
            //vector.z = mesh->mBitangents[i].z;
            //vertex.Bitangent = vector;
        }
        else
        {
            glm::vec2 texCoordinates;
            texCoordinates = glm::vec2(0.0f, 0.0f);
            attributes.push_back(texCoordinates.x);
            attributes.push_back(texCoordinates.y);
        }
    }
}

void ModelImporter::processMeshIndices(aiMesh* mesh, const aiScene* scene, vector<unsigned int>& indexArray) {

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indexArray.push_back(face.mIndices[j]);
    }
}

ModelImporter::ModelImporter() {
    populate_maps();
}

ModelImporter::~ModelImporter()
{
    //cout << "Model Importer is being deallocated " << endl;

    map<string, LoadedModelData*>::iterator it;
    for (it = models.begin(); it != models.end(); it++)
    {
        //cout << "Deallocating model data for " << it->first << endl;
        delete it->second;
    }

    map<string, LoadedTextureData*>::iterator tex_it;
    for (tex_it = textures.begin(); tex_it != textures.end(); tex_it++)
    {
        //cout << "Deallocating texture data for " << tex_it->first << endl;
        delete tex_it->second;
    }

    textures.clear();
    models.clear();

}