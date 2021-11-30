#include "ModelImporter.h"
#define TEST_MODELIMPORTER      false

void ModelImporter::loadModel(const char* modelSource, vector<float>& attribArray, vector<unsigned int>& indexArray) {

    if (model_loaded(modelSource))
    {
        cout << modelSource << " has already been loaded. Returning mapped values..." << endl;
        //LoadedModelData& modelData = models[modelSource];

        attribArray = models[modelSource]->attribs;
        indexArray = models[modelSource]->indices;

    }
    else
    {

        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(modelSource, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
            return;
        }

        // This is a new collection of attributes and indices.
        vector<float>& loadedAttribs = vector<float>();
        vector<unsigned int>& loadedIndices = vector<unsigned int>();

        cout << modelSource << " not found in MAP. loading..." << endl;

        // Load the attribs and indices, storing them in the newly created collections
#if TEST_MODEL3D
        cout << "In ModelImporter: Assigning " << modelSource << endl;
        cout << "Attributes assigned to vector ID #0x" << &attribArray << endl;
        cout << "Indices assigned to vector ID #0x" << &indexArray << endl;

        // Attribute loading / assigning
        cout << "=====================================" << endl;
        cout << "In Importer: BEFORE attrib load: " << attribArray.size() << endl;
#endif
        processNodeAttributes(scene->mRootNode, scene, loadedAttribs);
#if TEST_MODEL3D
        cout << "In Importer: AFTER load complete: " << attribArray.size() << endl;

        // Index loading / assigning
        cout << "In Importer: BEFORE index load: " << indexArray.size() << endl;
#endif
        processNodeIndices(scene->mRootNode, scene, loadedIndices);
#if TEST_MODEL3D
        cout << "In Importer: AFTER load complete: " << indexArray.size() << endl;
        cout << "=====================================" << endl;
#endif

        // Create a new struct to store the model data and insert it into the map.
        LoadedModelData* modelData = new LoadedModelData(loadedAttribs, loadedIndices);

        // Assign the newly loaded values to the target reference.
        models.insert(pair<const char*, LoadedModelData*>(modelSource, modelData));

        attribArray = modelData->attribs; // loadedAttribs;
        indexArray = modelData->indices;  // loadedIndices;

    }

}

bool ModelImporter::model_loaded(const char* modelSource)
{
    return models.count(modelSource); // will be 1 if in the map
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

}

ModelImporter::~ModelImporter()
{
    // TODO Iterate through map and deallocate pointers
}