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

#define MIN_DISTANCE 2.5f
#define FLY_AMOUNT 0.5f


void Dragon::rotateX(float angle)
{

    this->rotX += angle;

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
	speedY = 0.25 * cos(rotX*PI/180) * 5;

    GLfloat newPosX = posX + speedX;
	GLfloat newPosZ = posZ + speedZ;
	GLfloat newPosY = posY + std::abs(posY - lookY)*speedY;

    if(std::abs(std::abs(newPosX) - planeSize) > MIN_DISTANCE)
    {
        posX = newPosX;
    }
    if(std::abs(std::abs(newPosY) - planeSize) > MIN_DISTANCE)
    {
        if(newPosY > MIN_DISTANCE)
            posY = newPosY;
    }
    if(std::abs(std::abs(newPosZ) - planeSize) > MIN_DISTANCE)
    {
        posZ = newPosZ;
    }

	this->updateDirection();
}
void Dragon::moveBackward()
{
        speedX = -0.025 * sin(rotY*PI/180) * 5;
        speedZ = 0.025 * cos(rotY*PI/180) * 5;
        speedY = 0.025 * cos(rotX*PI/180) * 5;

        GLfloat newPosX = posX + speedX;
        GLfloat newPosZ = posZ + speedZ;
        GLfloat newPosY = posY + std::abs(posY - lookY)*speedY;

        if(std::abs(std::abs(newPosX) - planeSize) > MIN_DISTANCE)
        {
            posX = newPosX;
        }
        if(std::abs(std::abs(newPosY) - planeSize) > MIN_DISTANCE)
        {
            if(newPosY > MIN_DISTANCE)
                posY = newPosY;
        }
        if(std::abs(std::abs(newPosZ) - planeSize) > MIN_DISTANCE)
        {
            posZ = newPosZ;
        }

		this->updateDirection();

}
void Dragon::strafeLeft()
{
		speedX = 0.25 * sin((rotY-90.0f)*PI/180) * 5;
		speedZ = -0.25 * cos((rotY-90.0f)*PI/180) * 5;

        GLfloat newPosX = posX + speedX;
        GLfloat newPosZ = posZ + speedZ;

        if(std::abs(std::abs(newPosX) - planeSize) > MIN_DISTANCE)
        {
            posX = newPosX;
        }
        if(std::abs(std::abs(newPosZ) - planeSize) > MIN_DISTANCE)
        {
            posZ = newPosZ;
        }

		this->updateDirection();
}
void Dragon::strafeRight()
{

		speedX = 0.25 * sin((rotY+90.0f)*PI/180) * 5;
		speedZ = -0.25 * cos((rotY+90.0f)*PI/180) * 5;

        GLfloat newPosX = posX + speedX;
        GLfloat newPosZ = posZ + speedZ;

        if(std::abs(std::abs(newPosX) - planeSize) > MIN_DISTANCE)
        {
            posX = newPosX;
        }
        if(std::abs(std::abs(newPosZ) - planeSize) > MIN_DISTANCE)
        {
            posZ = newPosZ;
        }

        this->updateDirection();
}

void Dragon::flyUp()
{

    posY = posY + FLY_AMOUNT;

    this->updateDirection();
}
void Dragon::flyDown()
{
       GLfloat newPosY = posY - FLY_AMOUNT;

        if(std::abs(std::abs(newPosY) - planeSize) > MIN_DISTANCE)
        {
            if(newPosY > MIN_DISTANCE)
                posY = newPosY;
        }
    this->updateDirection();
}

void Dragon::draw()
{
    if(projectileUp)
    {
        this->currentProjectile.posX += this->currentProjectile.directionX/2;
        this->currentProjectile.posY += this->currentProjectile.directionY/2;
        this->currentProjectile.posZ += this->currentProjectile.directionZ/2;

        if(this->currentProjectile.posY < MIN_DISTANCE || planeSize - this->currentProjectile.posY < MIN_DISTANCE)
        {
            projectileUp = false;
            return;
        }
        if(std::abs(std::abs(this->currentProjectile.posX) - planeSize) < MIN_DISTANCE)
        {
            projectileUp = false;
            return;
        }
        if(std::abs(std::abs(this->currentProjectile.posZ) - planeSize) < MIN_DISTANCE)
        {
            projectileUp = false;
            return;
        }

        glPushMatrix();
            glTranslatef(this->currentProjectile.posX,this->currentProjectile.posY, this->currentProjectile.posZ);
            glEnable(GL_BLEND);
            glColor4f(1.0f, 0.6f, 0.0f, 0.5f);
            glutSolidSphere(1.0, 100, 100);
            glDisable(GL_BLEND);
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
