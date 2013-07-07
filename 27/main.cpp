#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>

#include <gl\glut.h>

//openal (sound lib)
#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"

//handle for the al.obj model
#include "ModelAl.h"

//handle generic obj models
#include "3DObject.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

#define DEBUG printf("** %s\n",__FUNCTION__);

#define PI 3.14159265
#define MAX_BUILDINGS 21

#define MAX_TEXTURES 24
#define MAX_MAPS 1
#define MAX_OBJECTS 2

#define PLAYER_START_NODE 22
#define ENEMY_START_NODE 0

//MODES
#define VIEW 0
#define PLAY 1

//ESCAPEMODE
#define PERFECT 0
#define RANDOM 1

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

//#######//
//GLOBALS//
//#######//

class timer {
	private:
		unsigned long begTime;
	public:
		void start() {
			begTime = clock();
		}

		unsigned long elapsedTime() {
			return ((unsigned long) clock() - begTime) / CLOCKS_PER_SEC;
		}

		bool isTimeout(unsigned long seconds) {
			return seconds >= elapsedTime();
		}
};

////////////////////////////////
// For software configuration //
////////////////////////////////
int windowWidth = 640;
int windowHeight = 480;
int windowXPos = 200;
int windowYPos = 200;

int mainWindowId;
float backgrundColor[] = {0.52 , 0.80 , 0.96, 1.0};

FILE *buildFile;

int MODE, ESCAPEMODE;

//camera stuff
float eye_x, eye_y, eye_z;
float center_x, center_y, center_z;
float up_x, up_y, up_z;

float posYOffset = 0.2;
float roty = 0.0f;
float rotx = 90.0f;
int mouseLastX = 0;
int mouseLastY = 0;

//texture stuff
int i;                              /* Looping var */
int j;                              /* Looping var */
int k;                              /* Looping var */
BITMAPINFO  *Tinfo[MAX_TEXTURES];
BITMAPINFO  *Minfo[MAX_MAPS];
GLubyte	    *Tbits[MAX_TEXTURES];   /* Bitmap RGB pixels for texture*/
GLubyte	    *Mbits[MAX_MAPS];       /* Bitmap RGB pixels for map*/
GLubyte     *ptr;                   /* Pointer into bit buffer */
GLubyte	    *rgba;                  /* RGBA pixel buffer */
GLubyte	    *rgbaptr;               /* Pointer into RGBA buffer */
GLubyte     temp;                   /* Swapping variable */
GLenum      type[MAX_TEXTURES];     /* Texture type */
GLuint      texture[MAX_TEXTURES];  /* Texture object */
GLuint      mapTexture[MAX_MAPS];   /* Map file for objects */

int floortex1; //asfalto
int floortex2; //calçada
int buildtex1;
int buildtex2;
int buildtex3;
int rooftex1; //laje

// For gaming

typedef struct {
    GLfloat init_x, init_y;
    GLfloat fin_x, fin_y;
    GLfloat height;
    int texType;
}Building;

//teclas
bool frontPressed;
bool backPressed;
bool leftPressed;
bool rightPressed;
bool upPressed;
bool downPressed;
bool spacePressed;
bool rotateLeft;
bool rotateRight;
int cameraType = 0;

//posições possíveis
typedef struct{
    float pointX;
    float pointZ;
}Grafo;

Grafo cidade[27];

int conexoes[27][27];

//posição do jogador
float posX;
float posY;
float posZ;

float posXT;
float posYT;
float posZT;

float LastposX;
float LastposY;
float LastposZ;

//posição do inimigo
float EposX;
float EposY;
float EposZ;
float Eangle;

float EposXT;
float EposZT;

int lastNode = ENEMY_START_NODE;
int nextNode = 5;

float treshold = 0.5;
float walkTreshold = 0.2;

int gameTime = 75;
timer t;
bool timeUp = false;
bool crashed = false;
bool lose = false;
bool win = false;

//velocidades de movimentação
float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;
float speed = 3.0f;

//controle da velocidade
bool dandoRe = false;
float iSpeedRe = 0.1f;
float iSpeed = 0.1f;
float maxFrontSpeed = 2.7f;
float maxBackSpeed = 1.5f;
float forwardFactor;
float backwardFactor;
float angle = 0.03f;
float curveAngle = 1.5f;

//configurações do mapa do jogo
float citySize = 50.0f;
float mapSize = 5.0f;
float clockSize = 5.0f;

bool colision = false;
int coliddedId;

float tX;
float tZ;

Building buildings[MAX_BUILDINGS];
Models models[MAX_OBJECTS];

//#########//
//TEMPLATES//
//#########//

void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void initTexture();
void initBuilding();
void initNodes();
void initModel();
void initLight();
void mainInit();
void renderFloor();
void renderBuildings();
void renderScene();
void updateState();
void updateState2();
void mainRender();
void onWindowReshape(int x, int y);
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onMousePassiveMove(int x, int y);
int main(int argc, char **argv);


//#########//
//FUNCTIONS//
//#########//

void setWindow() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	eye_x = posX;
    eye_y = posY + posYOffset;
    eye_z = posZ;
    center_x = posX + sin(roty*PI/180);
    center_y = posY + posYOffset + cos(rotx*PI/180);
    center_z = posZ -cos(roty*PI/180);
	up_x = 0.0;
	up_y = 1.0;
	up_z = 0.0;

	/*gluLookAt(eye_x,    eye_y,    eye_z,
              center_x, center_y, center_z,
              up_x,     up_y,     up_z);*/
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {

	glViewport(left, bottom, right - left, top - bottom);
}

void initTexture() {

    DEBUG

    //##########################################//

    char* file1 = "src\\textures\\floor.bmp";

    char* file2 = "src\\textures\\building1.bmp";
    char* file3 = "src\\textures\\building1.bmp";
    char* file4 = "src\\textures\\building1.bmp";
    char* file5 = "src\\textures\\building1.bmp";

    char* file6 = "src\\textures\\building2.bmp";
    char* file7 = "src\\textures\\building2.bmp";
    char* file8 = "src\\textures\\building2.bmp";
    char* file9 = "src\\textures\\building2.bmp";

    char* file10 = "src\\textures\\rooftop.bmp";

    char* file11 = "src\\textures\\sidewalk.bmp";

    char* file12 = "src\\textures\\wall.bmp";
    char* file13 = "src\\textures\\wall.bmp";
    char* file14 = "src\\textures\\wall.bmp";
    char* file15 = "src\\textures\\wall.bmp";

    char* file16 = "src\\textures\\building3.bmp";
    char* file17 = "src\\textures\\building3.bmp";
    char* file18 = "src\\textures\\building3.bmp";
    char* file19 = "src\\textures\\building3.bmp";

    char* file20 = "src\\textures\\building4.bmp";
    char* file21 = "src\\textures\\building4.bmp";
    char* file22 = "src\\textures\\building4.bmp";
    char* file23 = "src\\textures\\building4.bmp";

    char* file24 = "src\\textures\\grass.bmp";

    //##########################################//

    Tbits[0] = LoadDIBitmap(file1, &Tinfo[0]);

    Tbits[1] = LoadDIBitmap(file2, &Tinfo[1]);
    Tbits[2] = LoadDIBitmap(file3, &Tinfo[2]);
    Tbits[3] = LoadDIBitmap(file4, &Tinfo[3]);
    Tbits[4] = LoadDIBitmap(file5, &Tinfo[4]);

    Tbits[5] = LoadDIBitmap(file6, &Tinfo[5]);
    Tbits[6] = LoadDIBitmap(file7, &Tinfo[6]);
    Tbits[7] = LoadDIBitmap(file8, &Tinfo[7]);
    Tbits[8] = LoadDIBitmap(file9, &Tinfo[8]);

    Tbits[9] = LoadDIBitmap(file10, &Tinfo[9]);

    Tbits[10] = LoadDIBitmap(file11, &Tinfo[10]);

    Tbits[11] = LoadDIBitmap(file12, &Tinfo[11]);
    Tbits[12] = LoadDIBitmap(file13, &Tinfo[12]);
    Tbits[13] = LoadDIBitmap(file14, &Tinfo[13]);
    Tbits[14] = LoadDIBitmap(file15, &Tinfo[14]);

    Tbits[15] = LoadDIBitmap(file16, &Tinfo[15]);
    Tbits[16] = LoadDIBitmap(file17, &Tinfo[16]);
    Tbits[17] = LoadDIBitmap(file18, &Tinfo[17]);
    Tbits[18] = LoadDIBitmap(file19, &Tinfo[18]);

    Tbits[19] = LoadDIBitmap(file20, &Tinfo[19]);
    Tbits[20] = LoadDIBitmap(file21, &Tinfo[20]);
    Tbits[21] = LoadDIBitmap(file22, &Tinfo[21]);
    Tbits[22] = LoadDIBitmap(file23, &Tinfo[22]);

    Tbits[23] = LoadDIBitmap(file24, &Tinfo[23]);

    //##########################################//

    floortex1 = 0; //grass
    floortex2 = 23;
    buildtex1 = 1;
    buildtex2 = 5;
    buildtex3 = 11;
    rooftex1 = 9;


    //##########################################//

    for (int tex=0 ; tex<MAX_TEXTURES ; tex++)
    {
        if (Tbits[tex] == (GLubyte *)0) {
            printf ("Error loading texture!\n\n");
            return;
        }

        // Figure out the type of texture
        if (Tinfo[tex]->bmiHeader.biHeight == 1)
            type[tex] = GL_TEXTURE_1D;
        else
            type[tex] = GL_TEXTURE_2D;

        // Create and bind a texture object
        glGenTextures(1, &texture[tex]);
        glBindTexture(type[tex], texture[tex]);

        // Create an RGBA image
        rgba = (GLubyte *)malloc(Tinfo[tex]->bmiHeader.biWidth * Tinfo[tex]->bmiHeader.biHeight * 4);

        i = 0;
        for( rgbaptr = rgba, ptr = Tbits[tex];  i < Tinfo[tex]->bmiHeader.biWidth * Tinfo[tex]->bmiHeader.biHeight; i++, rgbaptr += 4, ptr += 3)
        {
            rgbaptr[0] = ptr[0];
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[2];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
        }

        // Set texture parameters
        glTexParameteri(type[tex], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (tex == 0)
            glTexParameteri(type[tex], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        else
            glTexParameteri(type[tex], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(type[tex], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(type[tex], GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (tex == 0)
            gluBuild2DMipmaps(type[tex], GL_RGBA, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, Tbits[tex]);
        else
            glTexImage2D(type[tex], 0, 4, Tinfo[tex]->bmiHeader.biWidth, Tinfo[tex]->bmiHeader.biHeight,
                      0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

        printf("Textura %d (Id: %d)\n", tex,texture[tex]);
    }
	printf("Textures ok.\n\n");
}

void initBuilding() {

    DEBUG

    char line[30];
    int i=0;
    buildFile = fopen("src\\files\\buildings.txt","rt");
    while(fgets(line, 30, buildFile) != NULL)
    {
         buildings[i].init_x = atoi(strtok(line," "));
         buildings[i].init_y = atoi(strtok(NULL," "));
         buildings[i].fin_x = atoi(strtok(NULL," "));
         buildings[i].fin_y = atoi(strtok(NULL," "));
         buildings[i].height = atoi(strtok(NULL," "));
         buildings[i].texType = atoi(strtok(NULL,"\n"));

         //buildings[i].height = rand()%LOL + 5;

         i++;
    }
    fclose(buildFile);
}

void initNodes() {

    DEBUG

    char line[30];
    int i = 0;
    buildFile = fopen("src\\files\\graph.txt","rt");
    while(fgets(line, 30, buildFile) != NULL)
    {
         cidade[i].pointX = atof(strtok(line,","));
         cidade[i].pointZ = atof(strtok(NULL,"\n"));
         i++;
    }
    fclose(buildFile);

                  //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27
    conexoes[0]  = {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[1]  = {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[2]  = {0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[3]  = {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[4]  = {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[5]  = {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[6]  = {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[7]  = {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[8]  = {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    conexoes[9]  = {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
    conexoes[10] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[11] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    conexoes[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    conexoes[17] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
    conexoes[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    conexoes[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0};
    conexoes[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1};
    conexoes[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0};
    conexoes[22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0};
    conexoes[23] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0};
    conexoes[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0};
    conexoes[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1};
    conexoes[26] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0};
}

void initModel() {

    DEBUG

	models[0] = Models();
	models[0].Init("src/objects/L200-OBJ.obj");
	models[0].RotateY(PI);

	models[1] = Models();
	models[1].Init("src/objects/L200-OBJ2.obj");
	Eangle = 270;

	printf("Models ok.\n");
}

void initLight() {
    glEnable(GL_LIGHTING);

    //posição inicial das 2 fontes de luz
    GLfloat light_position1[] = {0.0, 0.0, 0.0, 1.0 };
	GLfloat light_position2[] = {0.0, 15.0, 0.0, 1.0 };

    //light0 referente à lanterna do personagem
    GLfloat direction[] = { -2.0, 1.0, -2.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, light_position1 );
    glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, direction);
    glLightf ( GL_LIGHT0, GL_SPOT_EXPONENT , 20.0);
    glLightf ( GL_LIGHT0, GL_SPOT_CUTOFF, 20.0);

    //glEnable(GL_LIGHT0);

    //light1 referente a luz que gira na sala
	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	glEnable(GL_LIGHT1);
}

void mainInit() {

    DEBUG

	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);

	// habilita remocao de faces ocultas
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);

    //initSound();

	initTexture();

    initBuilding();

    initNodes();

	initModel();

	initLight();

    EposX = cidade[0].pointX;
    EposY = 0.42f;
    EposZ = cidade[0].pointZ;

    posX = cidade[22].pointX;
    posY = 0.42f;
    posZ = cidade[22].pointZ;
}

void renderFloor() {

	// set things up to render the floor with the texture
	glShadeModel(GL_SMOOTH);
	glBindTexture(type[floortex1], texture[floortex1]);
	glEnable(type[floortex1]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();

    glTranslatef(-(float)citySize/2.0f, 0.0f, -(float)citySize/2.0f);

    glColor4f(1.0f,1.0f,1.0f,1.0f);
    int xQuads = 80;
    int zQuads = 80;

    for (int i = 0; i < xQuads; i++)
    {
        for (int j = 0; j < zQuads; j++)
        {
            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(i * (float)citySize/xQuads, 0.0f, (j+1) * (float)citySize/zQuads);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((i+1) * (float)citySize/xQuads, 0.0f, (j+1) * (float)citySize/zQuads);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((i+1) * (float)citySize/xQuads, 0.0f, j * (float)citySize/zQuads);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(i * (float)citySize/xQuads, 0.0f, j * (float)citySize/zQuads);

            glEnd();
        }
    }
	glDisable(type[floortex1]);

	glBindTexture(type[23], texture[23]);
	glEnable(type[23]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(44.0f, 0.01f, 6.0f);

        glTexCoord2f(0.0f, 0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(35.0f, 0.01f, 6.0f);

        glTexCoord2f(0.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(35.0f, 0.01f, 15.0f);

        glTexCoord2f(1.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(44.0f, 0.01f, 15.0f);
    glEnd();

    glDisable(type[23]);

    //glDisable(GL_LIGHT1);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glVertex3f(35.0f, -20.0f, 25.0f);
        glVertex3f(15.0f, -20.0f, 25.0f);
        glVertex3f(15.0f,   0.0f, 25.0f);
        glVertex3f(35.0f,   0.0f, 25.0f);
    glEnd();

    glEnable(GL_LIGHT1);

	glPopMatrix();
}

void renderBuildings() {

    glPushMatrix();

    glTranslatef(-(float)citySize/2.0f, 0.0f, -(float)citySize/2.0f);

    for (int i=0; i<MAX_BUILDINGS ; i++)
    {
        // set things up to render the building with the texture
        glShadeModel(GL_SMOOTH);
        glBindTexture(type[buildings[i].texType], texture[buildings[i].texType]);
        glEnable(type[buildings[i].texType]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glColor4f(1.0f,1.0f,1.0f,1.0f);

        //frontface
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(0.0f,0.0f,1.0f);
            glVertex3f(buildings[i].init_x, 0.0f, buildings[i].fin_y+1);

            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,0.0f,1.0f);
            glVertex3f(buildings[i].fin_x+1, 0.0f, buildings[i].fin_y+1);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(0.0f,0.0f,1.0f);
            glVertex3f(buildings[i].fin_x+1, buildings[i].height, buildings[i].fin_y+1);

            glTexCoord2f(1.0f, 1.0f);
            glNormal3f(0.0f,0.0f,1.0f);
            glVertex3f(buildings[i].init_x, buildings[i].height, buildings[i].fin_y+1);
        glEnd();

        glDisable(type[buildings[i].texType]);

        glBindTexture(type[buildings[i].texType+1], texture[buildings[i].texType+1]);
        glEnable(type[buildings[i].texType+1]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        //backface
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(0.0f,0.0f,-1.0f);
            glVertex3f(buildings[i].fin_x+1, 0.0f, buildings[i].init_y);

            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,0.0f,-1.0f);
            glVertex3f(buildings[i].init_x, 0.0f, buildings[i].init_y);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(0.0f,0.0f,-1.0f);
            glVertex3f(buildings[i].init_x, buildings[i].height, buildings[i].init_y);

            glTexCoord2f(1.0f, 1.0f);
            glNormal3f(0.0f,0.0f,-1.0f);
            glVertex3f(buildings[i].fin_x+1, buildings[i].height, buildings[i].init_y);
        glEnd();

        glDisable(type[buildings[i].texType]+1);

        glBindTexture(type[buildings[i].texType+2], texture[buildings[i].texType+2]);
        glEnable(type[buildings[i].texType+2]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        //rightface
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].fin_x+1, 0.0f, buildings[i].fin_y+1);

            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].fin_x+1, 0.0f, buildings[i].init_y);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].fin_x+1, buildings[i].height, buildings[i].init_y);

            glTexCoord2f(1.0f, 1.0f);
            glNormal3f(1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].fin_x+1, buildings[i].height, buildings[i].fin_y+1);
        glEnd();

        glDisable(type[buildings[i].texType]+2);

        glBindTexture(type[buildings[i].texType+3], texture[buildings[i].texType+3]);
        glEnable(type[buildings[i].texType+3]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        //leftface
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(-1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].init_x, 0.0f, buildings[i].init_y);

            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(-1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].init_x, 0.0f, buildings[i].fin_y+1);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(-1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].init_x, buildings[i].height, buildings[i].fin_y+1);

            glTexCoord2f(1.0f, 1.0f);
            glNormal3f(-1.0f,0.0f,0.0f);
            glVertex3f(buildings[i].init_x, buildings[i].height, buildings[i].init_y);
        glEnd();

        glDisable(type[buildings[i].texType+3]);

        glBindTexture(type[rooftex1], texture[rooftex1]);
        glEnable(type[rooftex1]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        //top
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(buildings[i].init_x, buildings[i].height, buildings[i].fin_y+1);

            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(buildings[i].fin_x+1, buildings[i].height, buildings[i].fin_y+1);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(buildings[i].fin_x+1, buildings[i].height, buildings[i].init_y);

            glTexCoord2f(1.0f, 1.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(buildings[i].init_x, buildings[i].height, buildings[i].init_y);
        glEnd();

        glDisable(type[rooftex1]);
    }
	glPopMatrix();
}

void displayData() {

    static int minutes,seconds;
    static int lastseconds = -1;

    minutes = (gameTime - t.elapsedTime())/60;
    seconds = (gameTime - t.elapsedTime())%60;

    if(t.elapsedTime() >= gameTime && MODE == PLAY) {
        timeUp = true;
    }

    if (MODE == PLAY)
    {
        if (seconds != lastseconds)
        {
            system("cls");
            printf("Time remaining: %2d:%02d\n",minutes,seconds);
        }
    }


    stringstream dummy, dummy2;
    string tempo, minutos, segundos;

    dummy << minutes;
    dummy2 << seconds;
    minutos = dummy.str();
    segundos = dummy2.str();

    minutos+=":";
    if (seconds < 10)
        minutos+="0"+segundos;
    else
        minutos+=segundos;

    tempo = "Tempo restante: ";
    tempo += minutos;

    //cout<<minutos; //contem a string do horário

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT1);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0 , 0.0 , 0.0);
    glRasterPos3f(7, -12.5 , -1);

    for (int i = 0 ; i < tempo.length() ; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tempo[i]);
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);

    lastseconds = seconds;
}

void renderScene() {

	renderFloor();
	renderBuildings();
}

void updateState() {

	if (frontPressed || backPressed)
	{
        speedX =  0.05 * sin(roty*PI/180) * speed;
        speedZ = -0.05 * cos(roty*PI/180) * speed;

        if (frontPressed)
        {
            posX += speedX;
            posZ += speedZ;
        }
        else
        {
            posX -= speedX;
            posZ -= speedZ;
        }
	}

	if (leftPressed || rightPressed)
	{
        speedX = -0.05 * cos(roty*PI/180) * speed;
        speedZ = -0.05 * sin(roty*PI/180) * speed;

        if (leftPressed)
        {
            posX += speedX;
            posZ += speedZ;
        }
        else
        {
            posX -= speedX;
            posZ -= speedZ;
        }
	}

	if (upPressed || downPressed)
	{
        speedY = 0.05 * speed;

        if (upPressed)
            posY += speedY;
        else
            posY -= speedY;
	}

    /*for (int i=0 ; i<MAX_BUILDINGS ; i++)
	{
        if (posY+treshold <= buildings[i].height)
        {
            if (posX+citySize/2. >= buildings[i].init_x && posX+citySize/2. <= buildings[i].fin_x + 1)
            {
                if (posZ+citySize/2. >= buildings[i].init_y && posZ+citySize/2. <= buildings[i].fin_y + 1)
                {
                    posY = buildings[i].height;
                }
            }
        }
	}*/

	/*for (int i=0 ; i<MAX_BUILDINGS ; i++)
	{
	    if (posX+citySize/2. >= buildings[i].init_x && posX+citySize/2. <= buildings[i].fin_x + 1)
	    {
	        if (posZ+citySize/2. >= buildings[i].init_y && posZ+citySize/2. <= buildings[i].fin_y + 1)
	        {
	            if (posY >= 0.0f && posY <= buildings[i].height)
	            {
	                //is colliding

	            }
	        }
	    }
	}*/

    /*if (rotateLeft)
    {
        glPushMatrix();

        models[0].RotateY(angle);
        models[0].Draw();

        glPopMatrix();
    }*/

    /*if (rotateRight)
    {
        glPushMatrix();

        models[0].RotateY(-angle);
        models[0].Draw();

        glPopMatrix();
    }*/

	glPushMatrix();

    models[0].Translate(cidade[PLAYER_START_NODE].pointX,0.42f,cidade[PLAYER_START_NODE].pointZ);
    models[0].Draw();

    models[1].Translate(cidade[ENEMY_START_NODE].pointX,0.42f,cidade[ENEMY_START_NODE].pointZ);
    models[1].Draw();

	glPopMatrix();
}

void updateState2() {

    LastposX = posX;
    LastposY = posY;
    LastposZ = posZ;

    float LastEangle = Eangle;

    //faz o controle de movimento do jogador
    if (spacePressed)
    {
        if (!dandoRe)
        {
            if (iSpeed <= 0.0f)
            {
                iSpeed = 0.0f;
            }
            else
            {
                posX += speedX * iSpeed;
                posZ += speedZ * iSpeed;
                iSpeed -= 0.055f;
            }
        }
        else // ta dando ré, dá o efeito de desaceleração da RÉ
        {
            if (iSpeedRe <= 0)
            {
                iSpeedRe = 0;
            }
            else
            {
                posX -= speedX * iSpeedRe;
                posZ -= speedZ * iSpeedRe;
                iSpeedRe -= 0.055f;
            }
        }
    }
    else
    {
        if (frontPressed || backPressed) //movimentando
        {
            speedX =  0.05 * sin(roty*PI/180);
            speedZ = -0.05 * cos(roty*PI/180);

            if (frontPressed) //acelerando
            {
                dandoRe = false; //não tá dando ré
                if (iSpeedRe <= 0.0f)
                {
                    if (iSpeed >= maxFrontSpeed) //estabiliza velocidade da aceleração pra "frente"
                    {
                        posX += speedX * iSpeed;
                        posZ += speedZ * iSpeed;
                    }
                    else // aumenta contiuamente a aceleração
                    {
                        posX += speedX * iSpeed;
                        posZ += speedZ * iSpeed;
                        iSpeed += 0.025f;
                    }
                }
                else
                {
                    posX -= speedX * iSpeedRe;
                    posZ -= speedZ * iSpeedRe;
                    iSpeedRe -= 0.15f;
                }
            }
            else
            {
                dandoRe = true;

                if (iSpeed <= 0.0f) //ta parado
                {
                    if (iSpeedRe >= maxBackSpeed) //estabiliza velocidade da ré
                    {
                        posX -= speedX * iSpeedRe;
                        posZ -= speedZ * iSpeedRe;
                    }
                    else //aumenta velocidade de ré
                    {
                        posX -= speedX * iSpeedRe;
                        posZ -= speedZ * iSpeedRe;
                        iSpeedRe += 0.025f;
                    }
                }
                else //vai freando enquanto anda e dá ré, funciona como freio
                {
                    posX += speedX * iSpeed;
                    posZ += speedZ * iSpeed;
                    iSpeed -= 0.15f;
                }
            }
        }
        else //não ta apertando nada, para sozinho
        {
            if (!dandoRe)
            {
                if (iSpeed <= 0.0f)
                {
                    iSpeed = 0.0f;
                }
                else
                {
                    posX += speedX * iSpeed;
                    posZ += speedZ * iSpeed;
                    iSpeed -= 0.055f;
                }
            }
            else // ta dando ré, dá o efeito de desaceleração da RÉ
            {
                if (iSpeedRe <= 0)
                {
                    iSpeedRe = 0;
                }
                else
                {
                    posX -= speedX * iSpeedRe;
                    posZ -= speedZ * iSpeedRe;
                    iSpeedRe -= 0.055f;
                }
            }
        }
    }

    //delimita a intensidade da curva de acordo com
    //a velocidade do veículo
	forwardFactor = iSpeed / maxFrontSpeed;
	backwardFactor = iSpeedRe / maxBackSpeed;


    //faz o controle do movimento do inimigo
    if ((abs((EposX+citySize/2) - (cidade[nextNode].pointX+citySize/2))) > walkTreshold ||
        (abs((EposZ+citySize/2) - (cidade[nextNode].pointZ+citySize/2))) > walkTreshold) //não chegou no próximo nodo
    {
        //anda linear de acordo com o ângulo
        EposX += 0.15 * cos(Eangle*PI/180);
        EposZ -= 0.15 * sin(Eangle*PI/180);
    }
    else //se chegou
    {
        if (ESCAPEMODE == PERFECT)
        {
            //determina o next node
            int possibilidades[3];
            int index = 0;
            for (int i=0 ; i < 27 ; i++)
            {
                if (conexoes[nextNode][i] == 1 && i != lastNode) //é um destino possível e não é o anterior
                {
                    possibilidades[index] = i; //armazena um nodo de destino (i)
                    index++;
                }
            }
            lastNode = nextNode; //atualiza o ultimo nodo
            float distancias[3];
            for (int i=0 ; i<index ; i++) //calcula a distancia mais longe do personagem
            {
                distancias[i] = sqrt(pow(posX - cidade[possibilidades[i]].pointX,2) +
                                     pow(posZ - cidade[possibilidades[i]].pointZ,2));
            }
            float maiorDistancia = distancias[0];
            int maiorDistIndex = 0;
            for (int i=1 ; i<index ; i++)
            {
                if (distancias[i] > maiorDistancia) //diz qual é o mais longe
                {
                    maiorDistancia = distancias[i];
                    maiorDistIndex = i;
                }
            }
            nextNode = possibilidades[maiorDistIndex]; //seta o próximo nodo
        }
        if (ESCAPEMODE == RANDOM)
        {
            //determina o next node
            int possibilidades[3];
            int index = 0;
            for (int i=0 ; i < 27 ; i++)
            {
                if (conexoes[nextNode][i] == 1 && i != lastNode) //é um destino possível e não é o anterior
                {
                    possibilidades[index] = i; //armazena um nodo de destino (i)
                    index++;
                }
            }
            lastNode = nextNode; //atualiza o ultimo nodo

            nextNode = possibilidades[rand()%index]; //seta o próximo nodo
        }

        if (abs((cidade[nextNode].pointX + citySize/2.) - (EposX + citySize/2.)) > walkTreshold) //variação em X
        {
            if ((cidade[nextNode].pointX + citySize/2.) > (EposX + citySize/2.)) //ponto está para a direita
                Eangle = 0;
            else if ((cidade[nextNode].pointX + citySize/2.) < (EposX + citySize/2.)) //ponto está para a esquerda
                Eangle = 180;
            else //manteve a direção
                Eangle = LastEangle;
        }
        else if (abs((cidade[nextNode].pointZ + citySize/2.) - (EposZ + citySize/2.)) > walkTreshold)//variação em Z
        {
            if ((cidade[nextNode].pointZ + citySize/2.) > (EposZ + citySize/2.)) //ponto está para a baixo
                Eangle = 270;
            else if ((cidade[nextNode].pointZ + citySize/2.) < (EposZ + citySize/2.)) //ponto está para a cima
                Eangle = 90;
            else //manteve a direção
                Eangle = LastEangle;
        }
    }

    posXT = posX + 0.8 * sin(roty*PI/180);
    posZT = posZ - 0.8 * cos(roty*PI/180);

    EposXT = EposX + 0.8 * sin(Eangle*PI/180);
    EposZT = EposZ - 0.8 * cos(Eangle*PI/180);

    for (int i=0 ; i<MAX_BUILDINGS ; i++) //teste de colisão
	{
	    if (posXT+citySize/2. >= buildings[i].init_x - treshold && posXT+citySize/2. <= buildings[i].fin_x + 1 + treshold)
	    {
	        if (posZT+citySize/2. >= buildings[i].init_y - treshold && posZT+citySize/2. <= buildings[i].fin_y + 1 + treshold)
	        {
	            //is colliding
	            posX = LastposX;
	            posY = LastposY;
	            posZ = LastposZ;

	            iSpeed = 0.0f;
	            iSpeedRe = 0.0f;
	        }
	    }
	}
	if (abs((posXT+citySize/2) - (EposXT+citySize/2)) < 0.6)
        if (abs((posZT+citySize/2) - (EposZT+citySize/2)) < 0.6)
            crashed = true; //colisão com o adversário

	glPushMatrix();

        models[0].Translate(posX + sin(roty*PI/180) , posY , posZ - cos(roty*PI/180));

        models[1].Translate(EposX,EposY,EposZ);

        if (LastEangle >= Eangle) //rotaciona o veículo de acordo com os ângulos
            models[1].RotateY(-(LastEangle - Eangle)*PI/180);
        else
            models[1].RotateY((Eangle - LastEangle)*PI/180);

        if (leftPressed && !(frontPressed || backPressed)) //esquerda sem acelerar
        {
            if (forwardFactor > backwardFactor)
            {
                roty -= curveAngle * forwardFactor;
                models[0].RotateY(curveAngle*forwardFactor*PI/180);
            }
            else
            {
                roty += curveAngle * backwardFactor;
                models[0].RotateY(-curveAngle*backwardFactor*PI/180);
            }
        }
        else if (rightPressed && !(frontPressed || backPressed)) //direita sem acelerar
        {
            if (forwardFactor > backwardFactor)
            {
                roty += curveAngle * forwardFactor;
                models[0].RotateY(-curveAngle*forwardFactor*PI/180);
            }
            else
            {
                roty -= curveAngle * backwardFactor;
                models[0].RotateY(curveAngle*backwardFactor*PI/180);
            }
        }

        else if (leftPressed && frontPressed)
        {
            roty -= curveAngle * forwardFactor;
            models[0].RotateY(curveAngle*forwardFactor*PI/180);
        }
        else if (leftPressed && backPressed )
        {
            roty += curveAngle * backwardFactor;
            models[0].RotateY(-curveAngle*backwardFactor*PI/180);
        }
        else if (rightPressed && frontPressed)
        {
            roty += curveAngle * forwardFactor;
            models[0].RotateY(-curveAngle*forwardFactor*PI/180);
        }
        else if (rightPressed && backPressed)
        {
            roty -= curveAngle * backwardFactor;
            models[0].RotateY(curveAngle*backwardFactor*PI/180);
        }

        models[0].Draw();

        models[1].Draw();

    glPopMatrix();
}

void mainRender() {

    glEnable(GL_SCISSOR_TEST);
    glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);

    if (MODE == VIEW)
    {
        eye_x = posX;
        eye_y = posY + posYOffset;
        eye_z = posZ;
        center_x = posX + sin(roty*PI/180);
        center_y = posY + posYOffset + cos(rotx*PI/180);
        center_z = posZ - cos(roty*PI/180);
        up_x = 0.0;
        up_y = 1.0;
        up_z = 0.0;
    }
    if (cameraType == 0 && MODE == PLAY) //FAR view
    {
        eye_x = posX - 2*sin(roty*PI/180);
        eye_y = posY + 5*posYOffset;
        eye_z = posZ + 2*cos(roty*PI/180);
        center_x = posX + 4*sin(roty*PI/180);
        center_y = posY + cos(rotx*PI/180);
        center_z = posZ - 4*cos(roty*PI/180);
        up_x = 0.0;
        up_y = 1.0;
        up_z = 0.0;
    }
    if (cameraType == 1 && MODE == PLAY) //NEAR view
    {
        eye_x = posX;
        eye_y = posY + 2*posYOffset;
        eye_z = posZ;
        center_x = posX + 4*sin(roty*PI/180);
        center_y = posY + posYOffset + cos(rotx*PI/180);
        center_z = posZ - 4*cos(roty*PI/180);
        up_x = 0.0;
        up_y = 1.0;
        up_z = 0.0;
    }
    if (cameraType == 2 && MODE == PLAY) //HOOD view
    {
        eye_x = posX + 3*sin(roty*PI/180);
        eye_y = posY + posYOffset;
        eye_z = posZ - 3*cos(roty*PI/180);
        center_x = posX + 4*sin(roty*PI/180);
        center_y = posY + posYOffset + cos(rotx*PI/180);
        center_z = posZ - 4*cos(roty*PI/180);
        up_x = 0.0;
        up_y = 1.0;
        up_z = 0.0;
    }


    //MAIN VIEW PORT
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, windowWidth , windowHeight);
    gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glScissor(0, 0, windowWidth , windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eye_x,    eye_y,    eye_z,
              center_x, center_y, center_z,
              up_x,     up_y,     up_z);

    renderScene();
    if (MODE == VIEW)
        updateState();
    if (MODE == PLAY)
        updateState2();

    //MAP VIEW PORT
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, windowHeight-citySize*mapSize, citySize*mapSize , citySize*mapSize);
    glOrtho(-citySize/2, citySize/2, -citySize/2, citySize/2, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glScissor(0, windowHeight-citySize*mapSize, citySize*mapSize , citySize*mapSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 20.0, 0.0, //20 é acima do maior prédio
              0.0, 19.0, 0.0, //aí ele mostra a cidade inteira ;)
              0.0, 0.0, -1.0);

	renderScene();
    if (MODE == VIEW)
        updateState();
    if (MODE == PLAY)
        updateState2();

    if (MODE == PLAY)
    {
        //TIMER VIEW PORT
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, windowHeight-citySize*mapSize-20, citySize*mapSize , 20);
        //glOrtho(-citySize/2, citySize/2, -citySize/2, citySize/2, 0.1, 100);
        glOrtho(-10, 10, -10, 0, 0.1, 100);
        glMatrixMode(GL_MODELVIEW);
        glScissor(0, windowHeight-citySize*mapSize-20, citySize*mapSize , 20);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(0.0, -5.0, -25.0,
                  0.0, -5.0, -24.0,
                  0.0,  1.0,  0.0);

        renderScene();
        displayData();
    }

	glDisable(GL_SCISSOR_TEST);

	if (crashed || timeUp)
	{
	    if (crashed)
	    {
            win = true;
        }
        if (timeUp)
        {
            lose = true;
        }
	}

	if (win)
	{
	    printf("Voce venceu o NFS-Cold Pursuit!\n\n");
	    printf("Criado por Raphael Lupchinski\n");
	    printf("\n\nPressione q para prosseguir: ");
        char tecla = 'a';
        while (tecla != 'q')
        {
            cin>>tecla;
        }
	    exit(0);
	}
	if (lose)
	{
	    printf("Game Over\n\nO tempo se esgotou e o perseguido fugiu...\n\n");
	    printf("Criado por Raphael Lupchinski\n");
	    printf("\n\nPressione q para prosseguir: ");
        char tecla = 'a';
        while (tecla != 'q')
        {
            cin>>tecla;
        }
	    exit(0);
	}

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();

	Sleep(30);
}

void onWindowReshape(int x, int y) {

	windowWidth = x;
	windowHeight = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

void onKeyDown(unsigned char key, int x, int y) {
	//printf("%d \n", key);
	switch (key) {
		case 32: //space
            spacePressed = true;
			break;
		case 119: //w
		case 87:  //W
		//case GLUT_KEY_UP:
            frontPressed = true;
			break;
		case 115: //s
		case 83:  //S
		//case GLUT_KEY_DOWN:
            backPressed = true;
			break;
		case 97:  //a
		case 65:  //A
		//case GLUT_KEY_LEFT:
            leftPressed = true;
			break;
		case 100: //d
		case 68:  //D
		//case GLUT_KEY_RIGHT:
            rightPressed = true;
			break;
        case 113: //q
        case 81:  //Q
            upPressed = true;
            break;
        case 101: //e
        case 69:  //E
            downPressed = true;
            break;
		case 108: //l
		case 76:  //L
            rotateLeft = true;
			break;
		case 114: //r
		case 82:  //R
            rotateRight = true;
			break;
        case 118: //v
		case 86:  //V
            cameraType++;
            if (cameraType > 2)
            {
                cameraType = 0;
            }
			break;
        case 27:
			exit(0);
			break;
		default:
			break;
	}
}

void onKeyUp(unsigned char key, int x, int y) {

	switch (key) {
		case 32: //space
            spacePressed = false;
			break;
		case 119: //w
		case 87:  //W
        //case GLUT_KEY_UP:
            frontPressed = false;
			break;
		case 115: //s
		case 83:  //S
		//case GLUT_KEY_DOWN:
            backPressed = false;
			break;
		case 97:  //a
		case 65:  //A
		//case GLUT_KEY_LEFT:
            leftPressed = false;
			break;
		case 100: //d
		case 68:  //D
		//case GLUT_KEY_RIGHT:
            rightPressed = false;
			break;
        case 113: //q
        case 81:  //Q
            upPressed = false;
            break;
        case 101: //e
        case 69:  //E
            downPressed = false;
            break;
		case 108: //l
		case 76:  //L
            rotateLeft = false;
			break;
		case 114: //r
		case 82:  //R
            rotateRight = false;
			break;
		default:
			break;
	}
}

void onMouseButton(int button, int state, int x, int y) {
	//printf("onMouseButton button: %d \n", button);
	glutPostRedisplay();
}

void onMouseMove(int x, int y) {

	/*mouseLastX = x;
	mouseLastY = y;*/

	glutPostRedisplay();
}

void onMousePassiveMove(int x, int y) {

	roty += (x - mouseLastX);

	rotx -= (y - mouseLastY);

	if (rotx < -128.0) {
		rotx = -128.0;
	}

	if (rotx > -45.0) {
		rotx = -45.0;
	}

	mouseLastX = x;
	mouseLastY = y;

	//glutPostRedisplay();
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	//Store main window id so that glui can send it redisplay events
	mainWindowId = glutCreateWindow("NFS - Cold Pursuit");
	glutDisplayFunc(mainRender);
	glutReshapeFunc(onWindowReshape);

	cout<<"Use mode:"<<endl;
	cout<<"VIEW 0"<<endl;
	cout<<"PLAY 1"<<endl;
	cin>>MODE;

    if (MODE == VIEW)
    {
        //Register mouse events handlers
        glutMouseFunc(onMouseButton);
        glutMotionFunc(onMouseMove);
        glutPassiveMotionFunc(onMousePassiveMove);
    }
    else
    {
        cout<<"\nDifficulty Level:"<<endl;
        cout<<"MODERATE 0"<<endl;
        cout<<"RETARDED 1"<<endl;
        cin>>ESCAPEMODE;
    }

	//Register keyboard events handlers
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	mainInit();

    if (MODE == PLAY)
        t.start();

	glutMainLoop();

    return 0;
}
