//
//  mesh.hpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef mesh_hpp
#define mesh_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>


#include "shader.hpp"

using namespace gl;


struct Vertex {
    //  vertex coordinates
    glm::vec3 coord;
    
    //  normal vector
    glm::vec3 normal;
    
    //  texture coordinates
    glm::vec2 texCoord;
    
    //  tangent
    glm::vec3 tangent;
    
    //  bitangent
    glm::vec3 bitangent;
};


struct Texture
{
    unsigned int ID;
    std::string type;
//    aiString path;
};


class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned long numVertices;
    unsigned int VAO;
    
    Mesh (std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(Shader shader);
private:
    unsigned int VBO, EBO;
    void feedBuffers();
};

#endif /* mesh_hpp */
