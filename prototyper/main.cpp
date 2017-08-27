//
//  main.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <GLFW/glfw3.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
#include <string>
//#include <map>
//#include <math.h>


//#include "viewer.hpp"

#include "timer.hpp"
#include "camera.hpp"
#include "model.hpp"


using namespace gl;


/*  callback functions interface  */

void error_callback(int, const char*);
void framebuffer_size_callback(GLFWwindow*, int, int);
void key_callback(GLFWwindow*, int, int, int, int);
void scroll_callback (GLFWwindow*, double, double);
void cursor_pos_callback (GLFWwindow*,double,double);

/*  global variables  */

unsigned int DISPLAY_WIDTH, DISPLAY_HEIGHT;
KeyState keypress;
Camera camera;
Cursor cursor;
Timer timer;

int main(int argc, const char * argv[]) {
    
    //  check for input arguments
    
    if (argc==1)
    {
        std::cout << "ERROR: not enough arguments" << std::endl;
        return -1;
    }
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    
    //  get current display resolution of the primary monitor

    GLFWmonitor *primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *currentDisplayMode = glfwGetVideoMode(primary);
    DISPLAY_WIDTH = currentDisplayMode->width;
    DISPLAY_HEIGHT = currentDisplayMode->height;
    
    // create window with half the display resolution
    
    GLFWwindow* window = glfwCreateWindow(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, "prototyper", NULL, NULL);
    
    if(window == NULL)
    {
        std::cout << "GLFW Window creation failed... shutting down" << std::endl;
        glfwTerminate();
        return -666;
    }
    
    //  set some window related GLFW options (resizing, aspect)
    
    //glfwSetWindowAspectRatio(window, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    glfwSetWindowSizeLimits(window, DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_CURSOR_HIDDEN, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &cursor.prevX, &cursor.prevY);
    
    //  set callback functions, bind openGL
    
    glfwMakeContextCurrent(window);
    glbinding::Binding::useCurrentContext();
    glfwSetErrorCallback(error_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    
    /*  load shaders  */
    
    Shader shader = Shader("/Users/marcusgursch/Programming/Xcode/Projects/prototyper/shaders/vShader.glsl", "/Users/marcusgursch/Programming/Xcode/Projects/prototyper/shaders/fShader.glsl", nullptr);
    
    /*  setup openGL  */
    
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    /*  initiate timer  */
    
    timer.update();
    
    /*  load model  */
    
    std::vector <Model> models;
    
    for (unsigned int i = 1; i < argc; i++)
        models.push_back(Model(argv[i]));
    
    timer.update();
    
    std::cout << "Loading time: " << timer.delta << " seconds" << std::endl;

    
    /*  main loop  */
    
    
    while (!glfwWindowShouldClose(window))
    {
        //  get time
        timer.last = timer.now;
        timer.now = glfwGetTime();
        timer.delta = timer.now-timer.last;
        

        // ------
        gl::glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //  process events
        camera.updatePos(&keypress, timer.delta);
        
        //  render model
        shader.setMat4("view", camera.view);
        shader.setMat4("projection", camera.projection);
        
        for (unsigned int i = 0; i < models.size(); i++)
            models[i].draw(shader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void error_callback(int errorNr,const char* errorMessage)
{
    std::cout << "Error " << errorNr << " :" << errorMessage << std::endl;
}

void framebuffer_size_callback (GLFWwindow* window,int width,int height)
{
    gl::glViewport(0, 0, width, height);

    camera.updateProjection(width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    keypress.writeKey(key, action, mod);
}

void scroll_callback (GLFWwindow* window, double dx, double dy)
{
//    dx*=2.0f;
//    dy*=2.0f;
//    camera.setAngle(dx, dy);
    camera.speed /= 1+(0.01*(float)dy);
    camera.speed = glm::max(camera.speed, 0.003f);
}

void cursor_pos_callback (GLFWwindow* window,double currX,double currY)
{
    float dx = currX - cursor.prevX;
    float dy = currY - cursor.prevY;
    dx /= 3.0f;
    dy /= 3.0f;
    camera.setAngle(dx, dy);
    cursor.prevX = currX;
    cursor.prevY = currY;
}
