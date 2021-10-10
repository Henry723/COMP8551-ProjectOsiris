#include "ModelImporter.h"

void ModelImporter::loadModel(const char* modelSource) {
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(modelSource, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    //directory = path.substr(0, path.find_last_of('/'));

    if (isAttributes) {
        processNodeAttributes(scene->mRootNode, scene);
        isAttributes = false;
    }
    if (isIndices) {
        processNodeIndices(scene->mRootNode, scene);
        isIndices = false;
    }
    
}

void ModelImporter::processNodeAttributes(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(processMesh(mesh, scene));
        
        // Here we try to grab all data from all the mesh
        modelAttributes.push_back(processMeshAttributes(mesh, scene));  
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodeAttributes(node->mChildren[i], scene);
    }
}

void ModelImporter::processNodeIndices(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(processMesh(mesh, scene));

        // Here we try to grab all data from all the mesh
        modelIndices.push_back(processMeshIndices(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodeIndices(node->mChildren[i], scene);
    }
}

vector<float> ModelImporter::processMeshAttributes(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<float> attributes;//contain position, texture coordiates
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

    // process materials
    //aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    //vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //// 2. specular maps
    //vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //// 3. normal maps
    //std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //// 4. height maps
    //std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    //// return a mesh object created from the extracted mesh data
    //return Mesh(vertices, indices, textures);
    return attributes;
}

vector<unsigned int> ModelImporter::processMeshIndices(aiMesh* mesh, const aiScene* scene) {
    vector<unsigned int> indices;

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    return indices;
}

//later we will pass by reference with 2 arrays
void ModelImporter::getModelAttributes(const char* filePath, float &attributesPointer) {
    isAttributes = true;
    loadModel(filePath);
    attributesPointer = modelAttributes[0][0];

}

void ModelImporter::getModelIndices(const char* filePath, unsigned int &indicesPointer) {
    isIndices = true;
    loadModel(filePath);
    indicesPointer = modelIndices[0][0];
}
