#ifndef __OBJECTS__
#define __OBJECTS__

#define COLLISION_RANGE

class Object
{
    GLfloat posX, posY, posZ;
    GLfloat collisionRange;
public:
    bool checkCollision(GLfloat posX,GLfloat posY,GLfloat posZ, GLfloat collisionRange);

};

#endif __OBJECTS__
