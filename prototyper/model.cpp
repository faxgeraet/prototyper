//
//  model.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "model.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



Model::Model (std::string const &path)
{
    directory = path.substr(0, path.find_last_of('/')+1);
    loadModel(path);
    model = glm::mat4();
//    float f = 1.0/20000.0;
//    model = glm::scale(model, glm::vec3(f,f,f));
}

void Model::loadModel (std::string const &path)
{
    //  create buffers for tinyobjloader to fill
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    
    
    //  fill those buffers with triangulated data
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), directory.c_str(), true);
    

//
    //  check for errors

    if(!ret)
        std::cerr << "ERROR::TINYOBJLOADER::LOADING::FAILED "<< path << std::endl;
    if (!err.empty())
        std::cout << "ERROR::TINYOBJLOADER::" << err << std::endl;
    
    //  do some counting
    
    std::cout << "# of vertices: " << (attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals: " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords: " << (attrib.texcoords.size() / 2) << std::endl;
    std::cout << "# of materials: " << materials.size() << std::endl;
    std::cout << "# of shapes: " << shapes.size() << std::endl;
    
    //  add default material
    
    materials.push_back(tinyobj::material_t());
    
    // load textures into openGL and store their ids and name in class
    loadMaterialTexture(1, &materials); // diffuse maps
    loadMaterialTexture(2, &materials); // specular maps
    loadMaterialTexture(3, &materials); // normal maps
    loadMaterialTexture(4, &materials); // bump maps
    
    //  load shapes into openGL
    
    
    loadShapes(&attrib, &shapes);
    
}

void Model::loadMaterialTexture (int texArg, std::vector<tinyobj::material_t>* materials)
{
    std::string texCategory;
    switch (texArg)
    {
        case 1: texCategory = "diffuse";
            break;
        case 2: texCategory = "specular";
            break;
        case 3: texCategory = "normal";
            break;
        case 4: texCategory = "bump";
            break;
        default: texCategory = "unknownTexCategory";
    }
    
    //  process corresponding texture of each material
    
    for (unsigned int i = 1; i < (*materials).size(); i++)
    {
        std::string texName;
        switch (texArg)
        {
            case 1: texName = (*materials)[i-1].diffuse_texname;
                break;
            case 2: texName = (*materials)[i-1].specular_texname;
                break;
            case 3: texName = (*materials)[i-1].normal_texname;
                break;
            case 4: texName = (*materials)[i-1].bump_texname;
                break;
            default: texName = (*materials)[i-1].diffuse_texname;
        }
        
        
        //  check for filename
        
        if (texName.size() > 0)
        {
            std::cout << "material" << i << "." << texCategory << "_texname = " << texName << std::endl;
            std::cout << "loading...   ";
            
            //  only load if not already loaded
            
            if( textures.find(texName) == textures.end() )
            {
                GLuint texID;
                int texWidth, texHeight, texComp;
    
                std::string texFilePath = directory+texName;
                
                //  try loading
                
                unsigned char* texData = stbi_load(texFilePath.c_str(), &texWidth, &texHeight, &texComp, STBI_default);
                
                if(!texData)
                {
                    //  abort if loading failed
                    std::cout << "failed... " << texFilePath << std::endl;
                    stbi_image_free(texData);
                }
                else
                {
                    //  else pass stuff to openGL
                    
                    glGenTextures(1, &texID);
                    glBindTexture(GL_TEXTURE_2D, texID);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    
                    //  get texture format
                    GLenum format;
                    switch (texComp)
                    {
                        case 1: format = GL_RED;
                            break;
                        case 3: format = GL_RGB;
                            break;
                        case 4: format = GL_RGBA;
                            break;
                        default: format = GL_RGB;
                    }
                    
                    //  transfer texture data
                    
                    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, texData);
                    
                    //  unbind texture and free data
                    glBindTexture(GL_TEXTURE_2D, 0);
                    stbi_image_free(texData);
                    
                    //  save texture id and name in class object
                    
                    textures.insert(std::make_pair(texName, texID));
                    
                    std::cout << " success !" << std::endl;
                }
            }
        }
    }
}

void Model::loadVertices(tinyobj::attrib_t *attrib)
{
    numVertices = (*attrib).vertices.size();
    numNormals = (*attrib).normals.size();
    numTexcoord = (*attrib).texcoords.size();
    
    //    numVN = numVertices - numNormals;
    //    numVT = numVertices - numTexcoord;
    std::vector<float> temporary;
    
    //  write vertex data in sequential order since not every vertex corresponds to a normal or texcoord
    
    for (unsigned int i = 0; i < numVertices; i++)
    {
        temporary.push_back((*attrib).vertices[i]);
    }
    //    for (unsigned int i = 0; i < numNormals; i++)
    //    {
    //        temporary.push_back((*attrib).normals[i]);
    //    }
    ////    for (unsigned int i = 0; i < numVertices - numNormals; i++)
    ////    {
    ////        temporary.push_back(0);
    ////    }
    //    for (unsigned int i = 0; i < numTexcoord; i++)
    //    {
    //        temporary.push_back((*attrib).texcoords[i]);
    //    }
    ////    for (unsigned int i = 0; i < numVertices - numTexcoord; i++)
    ////    {
    ////        temporary.push_back(0);
    ////    }
    
    // load that stuff into openGL
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * temporary.size(), &temporary[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::loadShapes(tinyobj::attrib_t *attrib, std::vector<tinyobj::shape_t> *shapes)
{
    // first load vertices, then indices
    
    loadVertices(attrib);
    
    for ( unsigned int i = 0; i < (*shapes).size(); i++)
    {
        std::vector<unsigned int> temporary;

        for (unsigned int j = 0; j < (*shapes)[i].mesh.indices.size(); j++)
        {
            //  vertex coordinate indices
            
            unsigned int curr_index =  (*shapes)[i].mesh.indices[j].vertex_index;
            temporary.push_back(curr_index);
            
            
//            temporary.push_back(curr_index+1);
//            temporary.push_back(curr_index+2);
//
//            //  normal vector indices
//
//            if ( (*shapes)[i].mesh.indices[j].normal_index != -1)
//            {
//                curr_index = numVertices + 3 * (*shapes)[i].mesh.indices[j].normal_index;
//                temporary.push_back(curr_index);
//                temporary.push_back(curr_index+1);
//                temporary.push_back(curr_index+2);
//            }
//            else
//            {
//                temporary.push_back(-1);
//                temporary.push_back(-1);
//                temporary.push_back(-1);
//            }
//            
//            //  texcoord indices
//            if ( (*shapes)[i].mesh.indices[j].texcoord_index != -1)
//            {
//                curr_index = numVertices + numNormals + 3 * (*shapes)[i].mesh.indices[j].texcoord_index;
//                temporary.push_back(curr_index);
//                temporary.push_back(curr_index+1);
//            }
//            else
//            {
//                temporary.push_back(-1);
//                temporary.push_back(-1);
//            }
            
        
        }
        
        Vaostrct VAO;
        VAO.nrVertices = (*shapes)[i].mesh.indices.size();
        unsigned int EBO;
        
        glGenVertexArrays(1, &VAO.ID);
        glBindVertexArray(VAO.ID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*temporary.size(), &temporary[0], GL_STATIC_DRAW);
        
        //  set attrib pointers
        
        //  coordinates
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        
        //    //  normals
        //    glEnableVertexAttribArray(1);
        //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(numVertices * sizeof(float)));
        //
        //    //  texCoords
        //    glEnableVertexAttribArray(2);
        //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)( (2*numVertices) * sizeof(float)));
        
        glBindVertexArray(0);
        
        VAOs.push_back(VAO);
        EBOs.push_back(EBO);

    }

    
    
    
//
}

void Model::draw(Shader shader)
{
    shader.setMat4("model", model);
    for (unsigned int i = 0; i < VAOs.size(); i++)
    {
        glBindVertexArray(VAOs[i].ID);
        glDrawElements(GL_TRIANGLES, VAOs[i].nrVertices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    
}
