//
//  controls.hpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#ifndef controls_hpp
#define controls_hpp

#include <stdio.h>
#include <GLFW/glfw3.h>

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

#endif /* controls_hpp */
