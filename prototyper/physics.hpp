//
//  physics.hpp
//  prototyper
//
//  Created by Marcus Gursch on 30.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef physics_hpp
#define physics_hpp

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>

#include <model.hpp>
#include <controls.hpp>
#include <timer.hpp>

// TODO:  make physics a subclass of "Model"-class to be able to access model data

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

    void update (delta);
    void rotate (glm::vec3 rotation);
    void accelerate (glm::vec3 forceIn, float deltaT);
    void translate (glm::vec3 speed, float deltaT);

    // interaction with other objects

    bool collision (Model model); // note: needs access to vertex data
    glm::vec3 collisionPoint(Model model);

    // misc write functions
    void applyForce (glm::vec3 inForce);
};

/*  notes
1. setup properties: weight, initial place and rotation
2. analyze situation
- is the object colliding with another ?
- is a force being applied to the object ?
- is the object moving ?
- how much time has passed since the phase of analysis ?
-
 */

#endif /* physics_hpp */
