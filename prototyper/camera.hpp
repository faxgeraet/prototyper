//
//  camera.hpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef camera_hpp
#define camera_hpp

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "controls.hpp"
//#include "shader.hpp"

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




#endif /* camera_hpp */
