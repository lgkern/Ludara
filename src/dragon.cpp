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

#include "dragon.h"

void Dragon::rotateX(float angle)
{

    this->rotX += angle;

 //   if(this->rotX < 0.0f)
 //       this->rotX = 0.0f;
 //   if(this->rotX > 180.0f)
 //       this->rotX = 180.0f;

    while(this->rotX > 360.0f)
        this->rotX -= 360.0f;
    while(this->rotX < 0.0f)
        this->rotX += 360.0f;

}
void Dragon::rotateY(float angle)
{

    this->rotY += angle;
    while(this->rotY > 360.0f)
        this->rotY -= 360.0f;
    while(this->rotY < 0.0f)
        this->rotY += 360.0f;
}

void Dragon::updateDirection()
{
    this->lookX = this->posX + sin(this->rotY*PI/180);
    this->lookY = this->posY + cos(this->rotX*PI/180);
    this->lookZ = this->posZ - cos(this->rotY*PI/180);
    printf("%f %f %f    %f %f\n",lookX, lookY, lookZ, rotX, rotY);
}

void Dragon::rotateHead(float xAngle, float yAngle)
{
    this->rotateX(xAngle);
    this->rotateY(yAngle);
    this->updateDirection();
}
void Dragon::moveForward()
{
    speedX = 0.025 * sin(rotY*PI/180) * 5;
	speedZ = -0.025 * cos(rotY*PI/180) * 5;

	posX += speedX;
	posZ += speedZ;
/// posY += speedY;

}
void Dragon::moveBackward()
{
        speedX = -0.25 * sin(rotY*PI/180) * 5;
		speedZ = 0.25 * cos(rotY*PI/180) * 5;

		posX += (0.5)*speedX * 5;
		posZ += (0.5)*speedZ * 5;

}
void Dragon::strafeLeft()
{
		speedX = 0.25 * sin((rotY-90.0f)*PI/180);
		speedZ = -0.25 * cos((rotY-90.0f)*PI/180);


		posX += speedX;
		posZ += speedZ;
}
void Dragon::strafeRight()
{

		speedX = 0.25 * sin((rotY+90.0f)*PI/180);
		speedZ = -0.25 * cos((rotY+90.0f)*PI/180);

		posX += speedX;
        posZ += speedZ;
}

void Dragon::flyUp()
{

    posY += 1.0f;
    printf("%f\n",posY);
}
void Dragon::flyDown()
{
    posY -= 1.0f;
}