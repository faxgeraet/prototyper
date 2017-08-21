//
//  model.hpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef model_hpp
#define model_hpp

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <tiny_obj_loader.h>

#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>

#include "shader.hpp"


using namespace gl;

struct Vaostrct {
    unsigned int ID;
    unsigned int nrVertices;
};

struct Vertex {
    float coords[3]={0,0,0};
    float normals[3]={0,0,0};
    float texCoords[2]={0,0};
};

class Model {
public:
    std::vector<Vertex> vertices;
    std::map<std::string, GLuint> textures;
    std::string directory;
    glm::mat4 model;
    
    Model (std::string const &path);
    void draw(Shader shader);

    std::vector<Vaostrct> VAOs;
    
    
private:
    /*  functions  */
    
    //  loads complete model
    void loadModel (std::string const &path);
    
    /*  helper functions  */
    void loadMaterialTexture (int texArg, std::vector<tinyobj::material_t>* materials);
    void loadShapes(tinyobj::attrib_t *attrib, std::vector<tinyobj::shape_t> *shapes);
    
    /*  some variables */
    std::vector<unsigned int> EBOs;
    unsigned int VBO;
    unsigned int numVertices, numNormals, numTexcoord;
};

#endif /* model_hpp */
