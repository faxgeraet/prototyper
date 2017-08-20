//
//  timer.hpp
//  prototyper
//
//  Created by Marcus Gursch on 20.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

#include <stdio.h>
#include <GLFW/glfw3.h>

class Timer {
public:
    float now;
    float last;
    float delta;
    Timer();
    void update();
};


#endif /* timer_hpp */
