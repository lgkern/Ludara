#ifndef __OBJECTS__
#define __OBJECTS__


class Object
{
    GLfloat posX, posY, posZ;
    GLfloat sizeX, sizeY, sizeZ;
public:
    bool checkColosion(GLfloat posX,GLfloat posY,GLfloat posZ, GLfloat sizeX,GLfloat sizeY,GLfloat sizeZ);

};

#endif __OBJECTS__
