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


    //printf("%f %f %f\n",posX, posY, posZ);
   // posX + sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
   // printf("%f %f %f\n",lookX, lookY, lookZ);
}

void Dragon::rotateHead(float xAngle, float yAngle)
{
    this->rotateX(xAngle);
    this->rotateY(yAngle);
    this->updateDirection();
}
void Dragon::moveForward()
{
    speedX = 0.25 * sin(rotY*PI/180) * 5;
	speedZ = -0.25 * cos(rotY*PI/180) * 5;
	speedY = 0.025 * cos(rotX*PI/180) * 5;

	posX += speedX;
	posZ += speedZ;
	posY += speedY;
	this->updateDirection();
/// posY += speedY;

}
void Dragon::moveBackward()
{
        speedX = -0.025 * sin(rotY*PI/180) * 5;
		speedZ = 0.025 * cos(rotY*PI/180) * 5;

		posX += (0.5)*speedX * 5;
		posZ += (0.5)*speedZ * 5;
		this->updateDirection();

}
void Dragon::strafeLeft()
{
		speedX = 0.25 * sin((rotY-90.0f)*PI/180);
		speedZ = -0.25 * cos((rotY-90.0f)*PI/180);


		posX += speedX;
		posZ += speedZ;
		this->updateDirection();
}
void Dragon::strafeRight()
{

		speedX = 0.25 * sin((rotY+90.0f)*PI/180);
		speedZ = -0.25 * cos((rotY+90.0f)*PI/180);

		posX += speedX;
        posZ += speedZ;
        this->updateDirection();
}

void Dragon::flyUp()
{

    posY += 0.1f;
    this->updateDirection();
}
void Dragon::flyDown()
{
    posY -= 0.1f;
    this->updateDirection();
}

void Dragon::draw()
{
    this->collisionRange = 2.5f;
//    glPushMatrix();
 //       glColor3f(1.0, 1.0, 1.0);
 //       glTranslatef(1.0f,1.0f,1.0f);
 //       glutSolidTeapot(1.0);
 //   glPopMatrix();
    if(projectileUp)
    {
        this->currentProjectile.posX += this->currentProjectile.directionX/10;
        this->currentProjectile.posY += this->currentProjectile.directionY/10;
        this->currentProjectile.posZ += this->currentProjectile.directionZ/10;
        glPushMatrix();
            glTranslatef(this->currentProjectile.posX,this->currentProjectile.posY, this->currentProjectile.posZ);
           //d glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glColor4f(1.0f, 0.6f, 0.0f, 0.5f);
            glutSolidSphere(1.0, 100, 100);
            glDisable(GL_BLEND);
          //  glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void Dragon::attack()
{
    //if(!projectileUp)
    {
        projectileUp = true;
        this->currentProjectile.posX = this->posX;
        this->currentProjectile.posY = this->posY-1.0;
        this->currentProjectile.posZ = this->posZ;
        this->currentProjectile.directionX = this->lookX - this->posX;
        this->currentProjectile.directionY = this->lookY - this->posY;
        this->currentProjectile.directionZ = this->lookZ - this->posZ;
    }

}

