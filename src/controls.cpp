//
//  controls.cpp
//  prototyper
//
//  Created by Marcus Gursch on 18.08.17.
//  Copyright © 2017 Marcus Gursch. All rights reserved.
//

#include "proto.hpp"
#include <GLFW/glfw3.h>

KeyState::KeyState ()
{
    //  initialize every key as false

    unsigned int numKeys = sizeof(KeyState)/sizeof(bool);
    bool *setter = &W;
    for (int i = 0;  i < numKeys; i++, setter ++)
        *setter = false;
}

//  set the current state of the keyboard

void KeyState::writeKey(int key, int action, int mod)
{

    bool ret;
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
        ret = true;
    else
        ret = false;
    switch (key)
    {
            //  WASD

        case GLFW_KEY_W: this->W=ret;
            break;
        case GLFW_KEY_A: this->A=ret;
            break;
        case GLFW_KEY_S: this->S=ret;
            break;
        case GLFW_KEY_D: this->D=ret;
            break;

            //  IJKL

        case GLFW_KEY_I: this->I=ret;
            break;
        case GLFW_KEY_J: this->J=ret;
            break;
        case GLFW_KEY_K: this->K=ret;
            break;
        case GLFW_KEY_L: this->L=ret;
            break;

            //  QE

        case GLFW_KEY_Q: this->Q=ret;
            break;
        case GLFW_KEY_E: this->E=ret;
            break;

            //  arrow keys

        case GLFW_KEY_UP: this->UP=ret;
            break;
        case GLFW_KEY_DOWN: this->DOWN=ret;
            break;
        case GLFW_KEY_LEFT: this->LEFT=ret;
            break;
        case GLFW_KEY_RIGHT: this->RIGHT=ret;
            break;

            //  modifier

        case GLFW_KEY_LEFT_SHIFT: this->lSHIFT=ret;
            break;
        case GLFW_KEY_LEFT_ALT: this->lALT=true;
            break;
        case GLFW_KEY_LEFT_CONTROL: this->lCTRL=ret;
            break;
        case GLFW_KEY_LEFT_SUPER: this->lCMD=ret;
            break;

        case GLFW_KEY_SPACE: this->SPACE=ret;
            break;

            //  number keys

        case GLFW_KEY_0: this->N0=ret;
            break;
        case GLFW_KEY_1: this->N1=ret;
            break;
        case GLFW_KEY_2: this->N2=ret;
            break;
        case GLFW_KEY_3: this->N3=ret;
            break;
        case GLFW_KEY_4: this->N4=ret;
            break;
        case GLFW_KEY_5: this->N5=ret;
            break;
        case GLFW_KEY_6: this->N6=ret;
            break;
        case GLFW_KEY_7: this->N7=ret;
            break;
        case GLFW_KEY_8: this->N8=ret;
            break;
        case GLFW_KEY_9: this->N9=ret;
            break;
    }
}
