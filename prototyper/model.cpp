//
//  model.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



Model::Model (std::string const &path, bool gamma = false): gammaCorrection(gamma)
{
    loadModel(path);
    model = glm::mat4();
//    float f = 1.0/20000.0;
//    model = glm::scale(model, glm::vec3(f,f,f));
}

void Model::loadModel (std::string const &path)
{
    //  load data from filepath with assimp
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate|
                                             aiProcess_FlipUVs|
                                             aiProcess_CalcTangentSpace|
                                             aiProcess_GenNormals);
    
    //  check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    
    //  get directory path from filepath
    directory = path.substr(0, path.find_last_of('/'));
    
    //  process nodes starting with assimp root node
    processNode(scene->mRootNode, scene);
    
}

void Model::processNode (aiNode *node, const aiScene *scene)
{
    //  process each mesh related to the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        //  note: an aiNode contains only indices
        //  the actual data is stored in the aiScene
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    
    //  recursive solving
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh (aiMesh *mesh, const aiScene *scene)
{
    //  data buffers
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    //  go through all vertices of the current mesh
    for (unsigned int i = 0; i< mesh->mNumVertices; i++)
    {
        //  convert vector format from assimp to glm
        Vertex vertex;
        glm::vec3 v3conv;
        glm::vec2 v2conv;
        
        //  coordinates
        v3conv.x = mesh->mVertices[i].x;
        v3conv.y = mesh->mVertices[i].y;
        v3conv.z = mesh->mVertices[i].z;
        vertex.coord = v3conv;
        
        //  normal vector
        v3conv.x = mesh->mNormals[i].x;
        v3conv.y = mesh->mNormals[i].y;
        v3conv.z = mesh->mNormals[i].z;
        vertex.normal = v3conv;
        
        //  texture coordinates
        
        //  check if mesh contains texture coordinates
        if(mesh->mTextureCoords[0])
        {
        v2conv.x = mesh->mTextureCoords[0][i].x;
        v2conv.y = mesh->mTextureCoords[0][i].y;
        vertex.texCoord = v2conv;
        }
        else
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        
//        //  tangent vector
//        v3conv.x = mesh->mTangents[i].x;
//        v3conv.y = mesh->mTangents[i].y;
//        v3conv.z = mesh->mTangents[i].z;
//        vertex.tangent = v3conv;
//        
//        //  bitangent vector
//        v3conv.x = mesh->mBitangents[i].x;
//        v3conv.y = mesh->mBitangents[i].y;
//        v3conv.z = mesh->mBitangents[i].z;
//        vertex.bitangent = v3conv;
        
        //  add vertex to mesh
        vertices.push_back(vertex);
    }
    
    //  go through all faces (triangles with their indices) of the current mesh and save their indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        for (unsigned int j = 0; j < mesh->mFaces->mNumIndices; j++)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }
    //  process mesh materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    
    //  convention for sampler names:
    //  e.g. diffuse: 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    
    /*------------------------------*/
    //   diffuse: texture_diffuseN  //
    //  specular: texture_specularN //
    //   normal: texture_normalN    //
    /*------------------------------*/
    
    //  (1) diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    
    //  (2) specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    
    //  (3) texture normal
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    
    // (4) texture height
    
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    //  finally return mesh with extracted data
    return Mesh(vertices, indices, textures);
}


// check all material textures of a given type and load respective textures if not loaded yet

std::vector<Texture> Model::loadMaterialTextures (aiMaterial *material, aiTextureType type, std::string typeName)
{
    std::vector<Texture> texRet;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        
        //  check if already loaded
        
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
            {
                texRet.push_back(textures_loaded[j]);
                skip = true;  //  texture was already loaded -> skip
                break;
            }
        }
        if(!skip)  //  load if not already loaded
        {
            Texture texture;
            texture.ID = TextureFromFile(str.C_Str(), this->directory, nullptr);
            texture.type = typeName;
            texture.path = str;
            texRet.push_back(texture);
            
            //  store it for entire model to avoid duplicate textures
            textures_loaded.push_back(texture);
        }
    }
    return texRet;
}

void Model::draw(Shader shader)
{
    shader.setMat4("model", model);
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader);
}

unsigned int TextureFromFile (const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if(data)
    {
        //  get color format
        
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        if (nrComponents == 3)
            format = GL_RGB;
        if (nrComponents == 4)
            format = GL_RGBA;
        
        //  bind texture and transfer loaded data, generate mipmaps
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //  set texture properties
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        //  free unnecessary redundant
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;    
}
