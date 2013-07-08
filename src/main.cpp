/**
Implementa a movimentação simples de uma camera em primeira pessoa para um personagem que anda
sobre um plano.
*/
#ifdef WIN32
#include <windows.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/glut.h>

//openal (sound lib)
//#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"
#include "input.h"
#include "dragon.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

//#include <mmsystem.h>
//#pragma comment(lib, "winmm.lib")

#define PI 3.14159265

#define MIN_DISTANCE 0.2f

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1


void mainInit();
void initSound();
void initTexture();
void initTexture2();
void createGLUI();
void mainRender();
void mainCreateMenu();
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();
void mouseControl();
void load_grass(void);
void load_stone(void);
/**
Screen dimensions
*/
int windowWidth = 1024;
int windowHeight = 768;

/**
Screen position
*/
int windowXPos = 100;
int windowYPos = 150;

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;

float maxSpeed = 0.25f;

float planeSize = 100.0f;


Dragon* myDragon;
Input* baseInput;

// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture[2];         /* Texture object */

int i2;                       /* Looping var */
BITMAPINFO	*info2;           /* Bitmap information */
GLubyte	    *bits2;           /* Bitmap RGB pixels */
GLubyte     *ptr2;            /* Pointer into bit buffer */
GLubyte	    *rgba2;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr2;        /* Pointer into RGBA buffer */
GLubyte     temp2;            /* Swapping variable */
GLenum      type2;            /* Texture type */


//Passar para classe Dragon
void setWindow() {
	//roty = 0.0f;
    //rotx = 90.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(myDragon->getPosX(),myDragon->getPosY(),myDragon->getPosZ(),
		myDragon->getLookX(),
           myDragon->getLookY(),
           myDragon->getLookZ(),
		0.0,1.0,0.0);
}

/*
void setWindow() {
	//roty = 0.0f;
    //rotx = 90.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX,posY + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),
           posY + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),
           posZ -cos(roty*PI/180),
		0.0,1.0,0.0);
}
*/

/**
Atualiza a posição e orientação da camera
*/

/*
void updateCam() {
	gluLookAt(posX,posY + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),
           posY + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),
           posZ -cos(roty*PI/180),
		0.0,1.0,0.0);

	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;

	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;
}*/
void updateCam() {
	gluLookAt(myDragon->getPosX(),myDragon->getPosY(),myDragon->getPosZ(),
		myDragon->getLookX(),
        myDragon->getLookY(),
        myDragon->getLookZ(),
		0.0,1.0,0.0);
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {
	glViewport(left, bottom, right - left, top - bottom);
}


/**
Initialize
*/
void mainInit() {
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

}


/**
Initialize the texture
*/

BITMAPINFO *BitmapInfo1; /* Bitmap information */
GLubyte    *BitmapBits1; /* Bitmap data */
GLuint TexObj1;

void load_floor(void)
{
    return;
}

BITMAPINFO *BitmapInfo2; /* Bitmap information */
GLubyte    *BitmapBits2; /* Bitmap data */
GLuint TexObj2;

void load_wall(void)
{
    return;
}

void renderFloor() {

	// i want some nice, smooth, antialiased lines
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	// set things up to render the floor with the texture
	glShadeModel(GL_FLAT);
	glEnable (type);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(0.4f,0.4f,0.4f,1.0f);

	glPushMatrix();

	float textureScaleX = 100.0;
	float textureScaleY = 100.0;

	glBegin(GL_QUADS);
		glTexCoord2f(textureScaleX, 0.0f);   // coords for the texture
		glVertex3f(-planeSize, 0.0f, planeSize);

		glTexCoord2f(0.0f, 0.0f);  // coords for the texture
		glVertex3f(planeSize, 0.0f, planeSize);

		glTexCoord2f(0.0f, textureScaleY);  // coords for the texture
		glVertex3f(planeSize, 0.0f, -planeSize);

		glTexCoord2f(textureScaleX, textureScaleY);  // coords for the texture
		glVertex3f(-planeSize, 0.0f, -planeSize);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.8f,0.8f,0.8f,1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
		glVertex3f(planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
	}
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, -planeSize);
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, planeSize);
	}
	glEnd();

	glPopMatrix();
}

void renderScene() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCam();
	baseInput->drawElements();
	renderFloor();
}


void mainHandleMouseRightButtonMenuEventWrapper(int option) {
    baseInput->mainHandleMouseRightButtonMenuEvent(option);
}

void onMouseButtonWrapper(int button, int state, int x, int y) {
    baseInput->onMouseButton(button,state,x,y);
}

void onMouseMoveWrapper(int x, int y) {
    baseInput->onMouseMove(x,y);
}

void onMousePassiveMoveWrapper(int x, int y) {
    baseInput->onMousePassiveMove(x,y);
}


void onKeyDownWrapper(unsigned char key, int x, int y) {
    baseInput->onKeyDown(key,x,y);
}

void onKeyUpWrapper(unsigned char key, int x, int y) {
    baseInput->onKeyUp(key,x,y);
}



void colisionCheck()
{
    return;
}
/**
Render scene
*/
void mainRender() {
	baseInput->updateState();
	renderScene();
	glFlush();
	glutPostRedisplay();

	usleep(30000);

}


void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

/**
Glut idle funtion
*/
void mainIdle() {
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/
	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {

    myDragon = new Dragon();
    baseInput = new Input(myDragon);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("Ludara");
    //glutFullScreen();
	glutDisplayFunc(mainRender);

	glutReshapeFunc(onWindowReshape);

            /**
	Register mouse events handlers
	*/
	glutMouseFunc(onMouseButtonWrapper);
	glutMotionFunc(onMouseMoveWrapper);
	glutPassiveMotionFunc(onMousePassiveMoveWrapper);

	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDownWrapper);
	glutKeyboardUpFunc(onKeyUpWrapper);
    ShowCursor(false);

	mainInit();

	glutMainLoop();

    return 0;
}
