//
//  physics.cpp
//  prototyper
//
//  Created by Marcus Gursch on 30.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "physics.hpp"

void Physics::accelerate(float deltaT)
{
    speed += force * deltaT / weight;
}

void Physics::translate(float deltaT)
{
    location += speed * deltaT;
}

void Physics::update(deltaT)
{
    accelerate (deltaT);
    force = glm::vec3(); // reset force
    translate (deltaT);
}
