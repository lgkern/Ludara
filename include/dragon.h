#ifndef __DRAGON__
#define __DRAGON__

#include "objects.h"

///turn into class
typedef struct
{
    GLfloat posX, posY, posZ;
    GLfloat directionX, directionY, directionZ;
}projectile;

class Dragon : Object
{
    float speedX = 0.0f;
    float speedY = 0.0f;
    float speedZ = 0.0f;

    GLfloat posX = 1.0f;
    GLfloat posY = 10.0f;
    GLfloat posZ = 1.0f;

    float rotX = 90.0f;
    float rotY = 0.0f;

    float maxSpeed = 10.0f;
    float minSpeed = 1.0f;

    GLfloat lookX = 0.0f;
    GLfloat lookY = 0.0f;
    GLfloat lookZ = 0.0f;

    float planeSize = 100.0f;

    bool projectileUp = false;
    projectile currentProjectile;

public:
    GLfloat getPosX() {return posX;}
    GLfloat getPosY() {return posY;}
    GLfloat getPosZ() {return posZ;}

    GLfloat getLookX() {return lookX;}
    GLfloat getLookY() {return lookY;}
    GLfloat getLookZ() {return lookZ;}

    void rotateHead(float, float);

    void moveForward();
    void moveBackward();
    void strafeLeft();
    void strafeRight();
    void flyUp();
    void flyDown();
    void draw();
    void attack();

private:
    void rotateX(float);
    void rotateY(float);

    void updateDirection();
};


#endif // __DRAGON__
