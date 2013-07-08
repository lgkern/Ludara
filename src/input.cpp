
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/glut.h>

//openal (sound lib)
#include <al/alut.h>


#include "input.h"
#include "dragon.h"


Input::Input (Dragon* currentDragon)
{
    this->currentDragon = currentDragon;
}

/**
Handles events from the mouse right button menu
*/
void Input::mainHandleMouseRightButtonMenuEvent(int option) {
	switch (option) {
		case 1 :
			exit(0);
			break;
		default:
			break;
	}
}
/**
Mouse button event handler
*/
void Input::onMouseButton(int button, int state, int x, int y) {
	switch(button)
	{
        case GLUT_LEFT_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            break;
        case GLUT_MIDDLE_BUTTON:
            break;
        default:
            break;
	}

	glutPostRedisplay();
}

/**
Mouse move while button pressed event handler
*/
void Input::onMouseMove(int x, int y) {
 /*   mouseDeltaX = mouseLastX - x;
    mouseDeltaY = mouseLastY - y;
	mouseLastX = x;
	mouseLastY = y;

    printf("Mouse x y: %d %d \n", x, y);

<<<<<<< HEAD
   // mouseControl();
=======
   // mouseControl();*/

   	mouseDeltaY += (x - mouseLastX);

	mouseDeltaX -= (y - mouseLastY);

	//if (mouseDeltaX > -128.0) {
	//	mouseDeltaX = -128.0;
	//}

	//if (mouseDeltaX < -45.0) {
	//	mouseDeltaX = -45.0;
	//}

	mouseLastX = x;
	mouseLastY = y;

	this->mouseControl();
	glutPostRedisplay();
}

/**
Mouse move with no button pressed event handler
*/
void Input::onMousePassiveMove(int x, int y) {
/*    mouseDeltaX = mouseLastX - x;
    mouseDeltaY = mouseLastY - y;
	mouseLastX = x;
	mouseLastY = y;

	printf("Mouse x y: %d %d \n", x, y);
   // if(started)
<<<<<<< HEAD
   //     mouseControl();
=======
   //     mouseControl();*/
//printf("Mouse x y: %d %d \n", x, y);
   	mouseDeltaY += (x - mouseLastX);

	mouseDeltaX -= (y - mouseLastY);

/*	if (mouseDeltaX < -128.0) {
		mouseDeltaX = -128.0;
	}

	if (mouseDeltaX > -45.0) {
		mouseDeltaX = -45.0;
	}
*/
	mouseLastX = x;
	mouseLastY = y;

	this->mouseControl();
   glutPostRedisplay();
}

/**
Key press event handler
*/
void Input::onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
            spacePressed = true;
            break;
		case 119: //w
			if (!upPressed) {
				//alSourcePlay(source[0]);
				//PlaySound((LPCSTR) "..\\..\\Footsteps.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			}
			upPressed = true;
			break;
		case 115: //s
			downPressed = true;
			break;
		case 97: //a
			leftPressed = true;
			break;
		case 100: //d
			rightPressed = true;
			break;
        case 'r':
            running = 1;
            break;
        case 'c':
            fallPressed = true;
            break;
		default:
			break;
	}

	//glutPostRedisplay();
}

/**
Key release event handler
*/
void Input::onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
			spacePressed = false;
			break;
		case 119: //w
			if (upPressed) {
                //alSourceStop(source[0]);
				//PlaySound(NULL, 0, 0);
			}
			upPressed = false;
			break;
		case 115: //s
			downPressed = false;
			break;
		case 97: //a
			leftPressed = false;
			break;
		case 100: //d
			rightPressed = false;
			break;
        case 'r':
            running = 0;
            break;
        case 'c':
            fallPressed = false;
            break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}


void Input::mouseControl()
{
    currentDragon->rotateHead(mouseDeltaX * MOUSESENSITIVITY,mouseDeltaY * MOUSESENSITIVITY);
    printf("%d,\t%d\n",mouseDeltaX,mouseDeltaY);
//    SetCursorPos(windowHeight/2,windowWidth/2);
    //SetCursorPos(900/2,1440/2);

    //mouseLastX = 900/2;
    //mouseLastY = 1440/2;

    mouseDeltaX = 0;
    mouseDeltaY = 0;

}


void Input::updateState() {

   //mouseControl();

	if (upPressed)
    {
        currentDragon->moveForward();
	}

	if (downPressed)
    {
        currentDragon->moveBackward();
	}
	if (leftPressed)
	{
        currentDragon->strafeLeft();
	}

	if (rightPressed)
    {
        currentDragon->strafeRight();
	}

	if(spacePressed)
    {
        currentDragon->flyUp();
    }
    if(fallPressed)
    {
        currentDragon->flyDown();
    }
}

void Input::drawElements()
{
    currentDragon->draw();
}



