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

struct Vertex {
    glm::vec3 coordinates = glm::vec3 (0,0,0);
    glm::vec3 normal = glm::vec3 (0,0,0);
    glm::vec2 texcoord = glm::vec2 (0,0);
};

struct Texture {
    std::string name;
    unsigned int id;
};

struct Material {
    std::string name;
    Texture diff_texture;
    bool alpha = false;
};

struct Shape {
    unsigned int VAO;
    int matID;
    unsigned int numVertices;
};

class Model {
public:
    std::map<std::string, GLuint> textures;
    std::map<std::string, bool> textureMode;
    std::string directory;
    glm::mat4 model;
    std::vector<Shape> shape;
    
    Model (std::string const &path);
    void draw(Shader shader);
        
private:

    /*  helper functions  */
    void loadModel (std::string const &path);
    void loadMaterialTexture (int texArg, std::vector<tinyobj::material_t>* materials);
    
    /*  some variables */


    std::vector <Material> material;
};

#endif /* model_hpp */
