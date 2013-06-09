#ifndef __DRAGON__
#define __DRAGON__

class Dragon
{
    float speedX = 0.0f;
    float speedY = 0.0f;
    float speedZ = 0.0f;

    float posX = 0.0f;
    float posY = 10.0f;
    float posZ = 1.0f;

    float rotX = 90.0f;
    float rotY = 0.0f;

    float maxSpeed = 10.0f;
    float minSpeed = 1.0f;

    float lookX = 0.0f;
    float lookY = 0.0f;
    float lookZ = 0.0f;

public:
    float getPosX() {return posX;}
    float getPosY() {return posY;}
    float getPosZ() {return posZ;}

    //Maybe obsolete after integrating camera functions
    float getRotX() {return rotX;}
    float getRotY() {return rotY;}

    void rotateHead(float, float);

    void moveForward();
    void moveBackward();
    void strafeLeft();
    void strafeRight();
    void flyUp();
    void flyDown();

private:
    void rotateX(float);
    void rotateY(float);

    void updateDirection();
};

#endif // __DRAGON__
