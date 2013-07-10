#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/glut.h>

#define PI 3.14159265

//openal (sound lib)
#include <al/alut.h>

#include "objects.h"

bool Object::checkCollision(GLfloat posX,GLfloat posY,GLfloat posZ, GLfloat collisionRange)
{
    GLfloat distance = sqrtf(pow(this->posX-posX,2)+pow(this->posY-posY,2)+pow(this->posZ-posZ,2));
    return distance < collisionRange || distance < this->collisionRange;
}
