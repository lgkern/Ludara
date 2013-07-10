#ifndef __INPUT__
#define __INPUT__

#define MOUSESENSITIVITY 0.5f

#include "dragon.h"

using namespace std;

class Input
{
    vector<Object> scenario;
    Dragon* currentDragon;
    int mouseCurrentX = 0;
    int mouseCurrentY = 0;
    int mouseLastX = 0;
    int mouseLastY = 0;
    int mouseDeltaX = 0;
    int mouseDeltaY = 0;

    bool rightPressed = false;
    bool leftPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    int running = 0;
    bool spacePressed = false;
    bool fallPressed = false;
    bool jumping = false;
    bool jumpingUp = false;

    bool started = false;

public:
    Input(Dragon*);
    void mainHandleMouseRightButtonMenuEvent(int option);
    void onMouseButton(int button, int state, int x, int y);
    void onMouseMove(int x, int y);
    void onMousePassiveMove(int x, int y);
    void onKeyDown(unsigned char key, int x, int y);
    void onKeyUp(unsigned char key, int x, int y);
    void mouseControl();
    void updateState();
    void drawElements();
    void addElement(Object* element);
    bool checkCollision(GLfloat posX,GLfloat posY,GLfloat posZ, GLfloat collisionRange);
private:
};



#endif // input
