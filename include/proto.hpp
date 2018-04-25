//
//  model.hpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef proto_hpp
#define proto_hpp

#include <glad/glad.h>

//using namespace gl;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<GLFW/glfw3.h>

#include "tiny_obj_loader.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>





class Timer {
public:
    float now;
    float last;
    float delta;
    Timer();
    void update();
};


struct Cursor {
    double prevX;
    double prevY;
};


class KeyState {
public:
    bool W,A,S,D;
    bool I,J,K,L;
    bool Q,E;
    bool UP,DOWN,LEFT,RIGHT;
    bool lSHIFT,lCTRL,lALT,lCMD;
    bool SPACE;
    bool N1,N2,N3,N4,N5,N6,N7,N8,N9,N0;

    KeyState();
    void writeKey (int key, int action, int mod);
};


class Camera{
public:

    float alpha;
    float theta;
    float lookDownAngle;
    float lookUpAngle;

    glm::vec4 position;
    glm::vec3 angle;

    float sensitivity;
    float speed;

    glm::mat4 view;
    glm::mat4 projection;

    Camera();

    void updatePos(KeyState* keypress, float deltaT);
    void setAngle (float dx, float dy);
    void updateProjection (int width, int height);
private:
    glm::vec4 dv;
//    void updateView (glm::mat4 view, Shader shader);

};


class Shader {
public:
    unsigned int ID;

    //  initialize
    Shader (const char* vShaderPath, const char* fShaderPath, const char* gShaderPath);

    //  use
    void use();

    //  uniform setters
    void setBool(const std::string &name, bool value) const;
    void setInt (const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void checkCompileErrors (GLuint shader, std::string type);
};




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



const glm::vec3 gravity = {0.0, -9.81, 0.0};

class Physics {

public:
    void applyForce (glm::vec3 godlyforce);

    // object characteristics

    float weight;
    float friction;
    glm::vec3 location;
    glm::vec3 rotation; // angle vector (to where the object is faced);
    float scalingFactor; //proportianal scaling factor
    bool fixed; // is the object fixed in space ? i.e. the floor or non-interactive objects

    // movement state

    glm::vec3 speed;

    // movement change

    glm::vec3 force;
    float torque; //Drehmoment
    float momentOfInertia; //Trägheitsmoment
    float centerOfGravity;

    /* functions */

    // changing position

    void update (float delta);

    void accelerate (float deltaT);
    void translate (float deltaT);

    // interaction with other objects

    bool collision (Model model); // note: needs access to vertex data
    glm::vec3 collisionPoint(Model model);

    // misc write functions
//    void applyForce (glm::vec3 inForce);
};


#endif /* proto_hpp */
