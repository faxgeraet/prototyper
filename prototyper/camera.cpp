//
//  camera.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "camera.hpp"

Camera::Camera()
{
    alpha = 0;
    theta = 0;

    lookUpAngle = 100.0f;
    lookDownAngle = -100.0f;
    sensitivity = 0.5;
    speed = 0.075;
    
    position = glm::vec4(0.0,0.0,0.0,0.0);
    dv = glm::vec4(0.0,0.0,0.0,0.0);
    
    projection = glm::perspective(45.0f, 1.6f/1.0f, 0.1f, 100000.0f);
}

void Camera::updateProjection (int width, int height)
{
    projection = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 100000.0f);
}
void Camera::updatePos(KeyState *keypress, float deltaT)
{
    //  initialize positional differential
    
    dv = glm::vec4 (0.0,0.0,0.0,0.0);
    
    // initialize view matrix
    
    view = glm::mat4();
    
    //  poll keyboard input
    
    if (keypress->S)
    {
        dv[2] -= speed;
    }
    if (keypress->W)
    {
        dv[2] += speed;
    }
    if (keypress->A)
    {
        dv[0] += speed;
    }
    if (keypress->D)
    {
        dv[0] -= speed;
    }
    if (keypress->E)
    {
        dv[1] += speed;
    }
    if (keypress->Q)
    {
        dv[1] -= speed;
    }
    
    //  update view matrix
    
    view = glm::rotate (view, glm::radians(alpha), glm::vec3(1.0, 0.0, 0.0));
    view = glm::rotate (view, glm::radians(theta), glm::vec3(0.0, 1.0, 0.0));
    
    //  update position, lock view to y=0;
    
//    dv[1]=0;
    
    dv = dv * view ;
    
//    dv[1]=0;
    
    position += dv*(deltaT*60);
    
    view = glm::translate (view, glm::vec3(0.0,0.0,-2.0));
    view = glm::translate (view, glm::vec3(position[0],position[1],position[2]));
    
    
    
    
}

void Camera::setAngle(float dx, float dy)
{
    dx*=sensitivity;
    dy*=sensitivity;
    theta += dx;
    alpha += dy;
    if (theta >=180)
        theta -= 360;
    if (theta <=180)
        theta += 360;
    alpha = glm::clamp(alpha, -lookUpAngle, -lookDownAngle);
}
//void Camera::updateView (glm::mat4 view, Shader shader)
//{
//    shader.setMat4("view", view);
//}

