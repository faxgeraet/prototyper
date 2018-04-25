//
//  model.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "proto.hpp"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF



Model::Model (std::string const &path)
{
    //  abort if wrong input format

//    if(path.find("obj")==std::string::npos && path.find("OBJ")==std::string::npos && path.find("Obj")==std::string::npos)
//    {
//        std::cout << "ERROR: wrong file format" << std::endl;
//        return;
//    }

    directory = path.substr(0, path.find_last_of('/')+1);

    loadModel(path);

    // set model matrix

    model = glm::mat4();
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

    //  load materials

    materials.push_back(tinyobj::material_t()); //default material

    for (unsigned int i = 0; i < materials.size(); i++)
    {
            Material mat;
            mat.name = materials[i].name;
            mat.diff_texture.name = materials[i].diffuse_texname;
            material.push_back(mat);
    }

    // load textures into openGL and store their ids and name in class

    loadMaterialTexture(1, &materials); // diffuse maps
//    loadMaterialTexture(2, &materials); // specular maps
//    loadMaterialTexture(3, &materials); // normal maps
//    loadMaterialTexture(4, &materials); // bump maps

    //  temp storage for per material vertices

    std::vector<std::vector<Vertex> > verticesPerMaterial;
    for ( int i = 0; i < materials.size()+1; i++)
        verticesPerMaterial.push_back(*new std::vector<Vertex>);

    //  load shapes into openGL and store vertices category

    for ( unsigned int i = 0; i < shapes.size(); i++)
    {
        for (unsigned int j = 0; j < shapes[i].mesh.indices.size(); j++)
        {
            Vertex vertex;

            int coord_index =  3 * shapes[i].mesh.indices[j].vertex_index;
            vertex.coordinates.x = attrib.vertices[coord_index];
            vertex.coordinates.y = attrib.vertices[coord_index+1];
            vertex.coordinates.z = attrib.vertices[coord_index+2];

            int normal_index = 3 * shapes[i].mesh.indices[j].normal_index;

            if( normal_index>0)
            {
                vertex.normal.x = attrib.normals[normal_index];
                vertex.normal.y = attrib.normals[normal_index+1];
                vertex.normal.z = attrib.normals[normal_index+2];
            }
            int tex_index = 2 * shapes[i].mesh.indices[j].texcoord_index;

            if(tex_index>0)
            {
                vertex.texcoord.x = attrib.texcoords[tex_index];
                vertex.texcoord.y = attrib.texcoords[tex_index+1];
            }

            int matID = shapes[i].mesh.material_ids[j/3];

            if( matID<materials.size() && matID>=0)
                verticesPerMaterial[matID].push_back(vertex);
            else
                verticesPerMaterial[0].push_back(vertex);
        }
    }



    for (unsigned int j = 0; j < verticesPerMaterial.size(); j++)
    {
        //  add missing normals

        for (unsigned int i = 0; i < verticesPerMaterial[j].size(); i += 3)
        {
            if (glm::length(verticesPerMaterial[j][i].normal) == 0)
            {
                glm::vec3 v1,v2;
                v1 = verticesPerMaterial[j][i+1].coordinates - verticesPerMaterial[j][i].coordinates;
                v2 = verticesPerMaterial[j][i+2].coordinates - verticesPerMaterial[j][i].coordinates;
                verticesPerMaterial[j][i].normal = glm::normalize(glm::cross(v1,v2));
                verticesPerMaterial[j][i+1].normal = verticesPerMaterial[j][i].normal;
                verticesPerMaterial[j][i+2].normal = verticesPerMaterial[j][i].normal;
            }
        }

        //  create openGL buffers and make a VAO for each material

        Shape newShape;
        glGenVertexArrays(1, &newShape.VAO);
        glBindVertexArray(newShape.VAO);

        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*verticesPerMaterial[j].size(), &verticesPerMaterial[j][0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

        glBindVertexArray(0);

        //  store shape/material object

        newShape.matID = j;
        newShape.numVertices = verticesPerMaterial[j].size();
        shape.push_back(newShape);
    }
}

void Model::loadMaterialTexture (int texArg, std::vector<tinyobj::material_t>* materials)
{
    //  check for which kind of texture shall be loaded

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

    for (unsigned int i = 0; i < (*materials).size(); i++)
    {
        std::string texName;
        switch (texArg)
        {
            case 1: texName = (*materials)[i].diffuse_texname;
                break;
            case 2: texName = (*materials)[i].specular_texname;
                break;
            case 3: texName = (*materials)[i].normal_texname;
                break;
            case 4: texName = (*materials)[i].bump_texname;
                break;
            default: texName = (*materials)[i].diffuse_texname;
        }


        //  check for filename

        if (texName.size() > 0)
        {
            std::cout << "material" << i << "." << texCategory << "_texname = " << texName << std::endl;
            std::cout << "loading...   ";

            //  don't load texture if already loaded, otherwise try loading

            if( textures.find(texName) == textures.end() )
            {
                GLuint texID;
                int texWidth, texHeight, texComp;

                std::string texFilePath = directory+texName;

                stbi_set_flip_vertically_on_load(true);

                unsigned char* texData = stbi_load(texFilePath.c_str(), &texWidth, &texHeight, &texComp, STBI_default);

                if(!texData)
                {
                    //  abort loading if failure ocurred

                    std::cout << "failed... " << texFilePath << std::endl;
                    stbi_image_free(texData);
                }
                else
                {
                    //  create openGL buffer and setup parameters

                    glGenTextures(1, &texID);
                    glBindTexture(GL_TEXTURE_2D, texID);

                    glGenerateMipmap(GL_TEXTURE_2D);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    GLfloat fLargest;
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
                    glTexParameteri(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);



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
                            std::cout << "Unknown texture format!" << std::endl;
                    }

                    //  transfer texture data

                    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, texData);

                    //  unbind texture and free data

                    glBindTexture(GL_TEXTURE_2D, 0);
                    stbi_image_free(texData);

                    //  save texture id and name in shape object

                    material[i].diff_texture.name=texName;
                    material[i].diff_texture.id=texID;
                    if (format == GL_RGBA)
                    {
                        material[i].alpha=true;
                        std::cout << " has transparency !" << std::endl;
                    }


                    //  map out loaded textures with alpha mode and ID for reference

                    textures.insert(std::make_pair(texName, texID));
                    textureMode.insert(std::make_pair(texName, material[i].alpha));

                    std::cout << " success !" << std::endl;
                }
            }
            else
            {
                material[i].diff_texture.name=texName;
                material[i].diff_texture.id=textures[texName];
                material[i].alpha = textureMode[texName];
                if (material[i].alpha)
                {
                    std::cout << " has transparency !" << std::endl;
                }
                std::cout << " was already loaded!" << std::endl;
            }
        }
    }
}


void Model::draw(Shader shader)
{
    shader.use();
    shader.setMat4("model", model);

    //  draw each material

    // first draw non transparent materials

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    for ( unsigned int i = 0; i < shape.size(); i++)
    {
        if( !material[shape[i].matID].alpha)
        {
//            if(i == 13) continue;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material[shape[i].matID].diff_texture.id);
            glBindVertexArray(shape[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, shape[i].numVertices);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
        }
    }

    // then transparent materials

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    for ( unsigned int i = 0; i < shape.size(); i++)
    {
        if(material[shape[i].matID].alpha)
        {
//            if(i == 13) continue;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material[shape[i].matID].diff_texture.id);
            glBindVertexArray(shape[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, shape[i].numVertices);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
        }
    }

    glDisable(GL_BLEND);

}
