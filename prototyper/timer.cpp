//
//  timer.cpp
//  prototyper
//
//  Created by Marcus Gursch on 20.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "timer.hpp"


Timer::Timer()
{
    now=last=glfwGetTime();
    delta=0;
}
void Timer::update()
{
    last=now;
    now=glfwGetTime();
    delta = now - last;
}
