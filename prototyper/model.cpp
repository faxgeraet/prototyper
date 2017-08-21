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


void Model::loadShapes(tinyobj::attrib_t *attrib, std::vector<tinyobj::shape_t> *shapes)
{
    //  initialize vertex vector with vertex coordinates
    
    for (unsigned int i = 0; i < attrib->vertices.size();)
    {
        Vertex vertex;
        vertex.coords[0] = attrib->vertices[i++];
        vertex.coords[1] = attrib->vertices[i++];
        vertex.coords[2] = attrib->vertices[i++];
        vertices.push_back(vertex);
    }
    
    //  load indices and add cooresponding normals and texcoords into the vertex vector
    
    for ( unsigned int i = 0; i < (*shapes).size(); i++)
    {

        Vaostrct VAO;
        VAO.nrVertices = (*shapes)[i].mesh.indices.size();
        std::vector<unsigned int> vIndices;
        unsigned int EBO;
        
        //  vertex coordinate indices
        for (unsigned int j = 0; j < VAO.nrVertices; j++)
        {
            int coord_index =  (*shapes)[i].mesh.indices[j].vertex_index;
            int normal_index = (*shapes)[i].mesh.indices[j].normal_index;
            int texc_index = (*shapes)[i].mesh.indices[j].texcoord_index;
            
            //  add normals to vertex vector
            if(normal_index>=0)
            {
                vertices[coord_index].normals[0] = attrib->normals[normal_index];
                vertices[coord_index].normals[1] = attrib->normals[normal_index+1];
                vertices[coord_index].normals[2] = attrib->normals[normal_index+2];
            }
            
            //  add texcoords
            if(texc_index>=0)
            {
                vertices[coord_index].texCoords[0] = attrib->texcoords[texc_index];
                vertices[coord_index].texCoords[1] = attrib->texcoords[texc_index+1];
            }
            vIndices.push_back(coord_index);
        }

        //  pass data to openGL and create a VAO for each shape
    
        glGenVertexArrays(1, &VAO.ID);
        glBindVertexArray(VAO.ID);
        
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*vIndices.size(), &vIndices[0], GL_STATIC_DRAW);

        //  set attrib pointers
        
        //  coordinates
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        
        //  normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));
        
        //  texCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        
        glBindVertexArray(0);
        
        VAOs.push_back(VAO);
        EBOs.push_back(EBO);
    }
    vertices.clear();
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
