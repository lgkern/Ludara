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
#include <al/alut.h>

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
/*int mouseCurrentX = 0;
int mouseCurrentY = 0;
int mouseLastX = 0;
int mouseLastY = 0;
int mouseDeltaX = 0;
int mouseDeltaY = 0;

float mouseSensitivity = 1.0f;
float roty = 0.0f;
float rotx = 90.0f;

bool rightPressed = false;
bool leftPressed = false;
bool upPressed = false;
bool downPressed = false;
int running = 0;
bool spacePressed = false;
int crouch = false;
bool jumping = false;
bool jumpingUp = false;

bool started = false;

float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;

float posX = 0.0f;
float posY = 0.4f;
float posZ = 1.0f;

float newPosX = 0.0f;
float newPosY = 0.4f;
float newPosZ = 1.0f;
*/
/*
variavel auxiliar pra dar variação na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

float planeSize = 100.0f;


Dragon* myDragon;
Input* baseInput;



// more sound stuff (position, speed and orientation of the listener)
ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,1.0,
						0.0,1.0,0.0};

// now the position and speed of the sound source
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
ALfloat source0Vel[]={ 0.0, 0.0, 0.0};

// buffers for openal stuff
ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
ALsizei size,freq;
ALenum  format;
ALvoid  *data;



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
		myDragon->getPosX() + sin(myDragon->getRotY()*PI/180),
           myDragon->getPosY() + cos(myDragon->getRotX()*PI/180),
           myDragon->getPosZ() - cos(myDragon->getRotY()*PI/180),
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
		myDragon->getPosX() + sin(myDragon->getRotY()*PI/180),
           myDragon->getPosY() + cos(myDragon->getRotX()*PI/180),
           myDragon->getPosZ() - cos(myDragon->getRotY()*PI/180),
		0.0,1.0,0.0);

	listenerPos[0] = myDragon->getPosX();
	listenerPos[1] = myDragon->getPosY();
	listenerPos[2] = myDragon->getPosZ();

	source0Pos[0] = myDragon->getPosX();
	source0Pos[1] = myDragon->getPosY();
	source0Pos[2] = myDragon->getPosZ();
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

	initSound();

}

/**
Initialize openal and check for errors
*/
void initSound() {

	printf("Initializing OpenAl \n");

	// Init openAL
	alutInit(0, NULL);

	alGetError(); // clear any error messages

    // Generate buffers, or else no sound will happen!
    alGenBuffers(NUM_BUFFERS, buffer);

    if(alGetError() != AL_NO_ERROR)
    {
        printf("- Error creating buffers !!\n");
        exit(1);
    }
    else
    {
        printf("init() - No errors yet.\n");
    }

	alutLoadWAVFile("..\\res\\Footsteps.wav",&format,&data,&size,&freq,false);
    alBufferData(buffer[0],format,data,size,freq);
    //alutUnloadWAV(format,data,size,freq);

	alGetError(); /* clear error */
    alGenSources(NUM_SOURCES, source);

    if(alGetError() != AL_NO_ERROR)
    {
        printf("- Error creating sources !!\n");
        exit(2);
    }
    else
    {
        printf("init - no errors after alGenSources\n");
    }

	listenerPos[0] = myDragon->getPosX();
	listenerPos[1] = myDragon->getPosY();
	listenerPos[2] = myDragon->getPosZ();

	source0Pos[0] = myDragon->getPosX();
	source0Pos[1] = myDragon->getPosY();
	source0Pos[2] = myDragon->getPosZ();

	alListenerfv(AL_POSITION,listenerPos);
    alListenerfv(AL_VELOCITY,listenerVel);
    alListenerfv(AL_ORIENTATION,listenerOri);

	alSourcef(source[0], AL_PITCH, 1.0f);
    alSourcef(source[0], AL_GAIN, 1.0f);
    alSourcefv(source[0], AL_POSITION, source0Pos);
    alSourcefv(source[0], AL_VELOCITY, source0Vel);
    alSourcei(source[0], AL_BUFFER,buffer[0]);
    alSourcei(source[0], AL_LOOPING, AL_TRUE);

	printf("Sound ok! \n\n");
}

/**
Initialize the texture
*/

BITMAPINFO *BitmapInfo1; /* Bitmap information */
GLubyte    *BitmapBits1; /* Bitmap data */
GLuint TexObj1;

void load_floor(void)
{
  BitmapBits1 = LoadDIBitmap("..\\res\\grass.bmp", &BitmapInfo1);

  glGenTextures(1, &TexObj1);
  glEnable(GL_TEXTURE_2D);
  gluBuild2DMipmaps(GL_TEXTURE_2D,3,32,32,GL_RGB,GL_UNSIGNED_BYTE, &TexObj1);
  glBindTexture(GL_TEXTURE_2D, TexObj1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D,
    0,                // mipmap level
    GL_RGB,             // internal format
    BitmapInfo1->bmiHeader.biWidth,  // width
    BitmapInfo1->bmiHeader.biHeight, // height
    0,                // border
    GL_BGR_EXT,           // data format  (NOTE: Bitmap's format is BGR!!!!)
    GL_UNSIGNED_BYTE,       // element per pixel
    BitmapBits1);          // data

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
 }

BITMAPINFO *BitmapInfo2; /* Bitmap information */
GLubyte    *BitmapBits2; /* Bitmap data */
GLuint TexObj2;

void load_wall(void)
{
  BitmapBits2 = LoadDIBitmap("..\\res\\wall.bmp", &BitmapInfo2);

  glGenTextures(1, &TexObj2);
  glEnable(GL_TEXTURE_2D);
  gluBuild2DMipmaps(GL_TEXTURE_2D,3,32,32,GL_RGB,GL_UNSIGNED_BYTE, &TexObj2);
  glBindTexture(GL_TEXTURE_2D, TexObj2);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D,
    0,                // mipmap level
    GL_RGB,             // internal format
    BitmapInfo2->bmiHeader.biWidth,  // width
    BitmapInfo2->bmiHeader.biHeight, // height
    0,                // border
    GL_BGR_EXT,           // data format  (NOTE: Bitmap's format is BGR!!!!)
    GL_UNSIGNED_BYTE,       // element per pixel
    BitmapBits2);          // data


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


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

    load_wall();

    glBegin(GL_QUADS);
        glTexCoord2f(textureScaleX, 0.0f);
		glVertex3f(-planeSize, 0.0f, planeSize);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(planeSize, 0.0f, planeSize);
		glTexCoord2f(0.0f, textureScaleX);
		glVertex3f(planeSize, 1.0f, planeSize);
		glTexCoord2f(textureScaleX, textureScaleX);
		glVertex3f(-planeSize, 1.0f, planeSize);
    glEnd();

    glBegin(GL_QUADS);
        glTexCoord2f(textureScaleX, 0.0f);
		glVertex3f(planeSize, 0.0f, planeSize);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(planeSize, 0.0f, -planeSize);
		glTexCoord2f(0.0f, textureScaleX);
		glVertex3f(planeSize, 1.0f, -planeSize);
		glTexCoord2f(textureScaleX, textureScaleX);
		glVertex3f(planeSize, 1.0f, planeSize);
    glEnd();

    glBegin(GL_QUADS);
        glTexCoord2f(textureScaleX, 0.0f);
		glVertex3f(planeSize, 0.0f, -planeSize);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-planeSize, 0.0f, -planeSize);
		glTexCoord2f(0.0f, textureScaleX);
		glVertex3f(-planeSize, 1.0f, -planeSize);
		glTexCoord2f(textureScaleX, textureScaleX);
		glVertex3f(planeSize, 1.0f, -planeSize);
    glEnd();

    glBegin(GL_QUADS);
        glTexCoord2f(textureScaleX, 0.0f);
		glVertex3f(-planeSize, 0.0f, -planeSize);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-planeSize, 0.0f, planeSize);
		glTexCoord2f(0.0f, textureScaleX);
		glVertex3f(-planeSize, 1.0f, planeSize);
		glTexCoord2f(textureScaleX, textureScaleX);
		glVertex3f(-planeSize, 1.0f, -planeSize);
    glEnd();

    load_floor();

    //glBindTexture(GL_TEXTURE_2D, TexObj1);// texture[1]);


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

	//load_grass();
	 //glBindTexture(GL_TEXTURE_2D, TexObj2); //texture[0]);

	glDisable(GL_TEXTURE_2D);
	glColor4f(0.8f,0.8f,0.8f,1.0f);
	/*glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
		glVertex3f(planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
	}
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, -planeSize);
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, planeSize);
	}
	glEnd();
*/
	glPopMatrix();
}

void renderScene() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCam();
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
    //printf("Colision check\n");
    //printf("x:%f z:%f\n",newPosX,newPosZ);
    return;
    /*if(std::abs(std::abs(newPosX) - planeSize) > MIN_DISTANCE)
    {
      //  printf("x ok %f \t",newPosX);
        posX = newPosX;
    }
    if(std::abs(std::abs(newPosY) - planeSize) > MIN_DISTANCE)
    {
  //      printf("y ok %f \t",newPosY);
        posY = newPosY;
    }
    if(std::abs(std::abs(newPosZ) - planeSize) > MIN_DISTANCE)
    {
      //  printf("z ok %f ",newPosZ);
        posZ = newPosZ;
    }

    //printf("\n");

        newPosX = posX;
        newPosY = posY;
        newPosZ = posZ;
*/
}

/*
void mainCreateMenu() {
	glutCreateMenu(mainHandleMouseRightButtonMenuEvent);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
*/
/**
Render scene
*/
void mainRender() {
	baseInput->updateState();
	renderScene();
	glFlush();
	glutPostRedisplay();
#ifdef WIN32
	Sleep(30);
#endif
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
	mainWindowId = glutCreateWindow("FPS");
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

	/**
	Create GLUT mouse button menus
	*/
	//mainCreateMenu();

	glutMainLoop();

    return 0;
}
