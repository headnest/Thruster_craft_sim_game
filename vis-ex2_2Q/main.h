#include <iostream>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glut.h>

#define VELOC 0
#define POSIT 1
#define HEADI 2
#define TOP 3
#define THRUST 4

#define GRAVITY 0.0005f

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
int stick = 0;




const float dt = 0.01f;

int numSpheres = 0;

double vertices[][3] = {//タイル1枚
	{0.0, 0.0, 15},
	{0.0, 0.0, 0.0},
	{15, 0.0, 0.0},
	{15, 0.0, 15}
};

double model_m[][3] = {//タイル1枚
	{0.0, 0.5, 0.0},
	{0.0, 0.0, 0.0},
	{0.5, 0.0, 0.0},
	{0.5, 0.5, 0.0}
};

void setCamera(void);

void display(void);

void displayAxis(float length);

void displaySphere(float radius, float dr, float dg, float db, float sr, float sg, float sb, float shininess);

void keyboardFunction(unsigned char key, int x, int y);

void update(void);
void getNormalVector(GLdouble normalVector[3], const double v1[3], const double v2[3], const double v3[3]);
void displayGround(void);

GLuint textureID;
GLuint loadTexture(const char* filename);
unsigned char* loadBMPFile(const char* filename, int* width, int* height);

float pow_self;

float stats_self[5][3];

float pitch_mat0[3];

float prevm_x;
float prevm_y;

float roll_upd;
float pitch_upd;
float yaw_upd;

float roll_ang;
float pitch_ang;
float yaw_ang;

void calc_status();
void stats_intg(int id0, int id1);

void initialize_stats();
//rodrigues rot func
void RRfunc_stats(float nx, float ny, float nz, const int id, float rot);
//headingx,y,z topx,y,z roll pitch yaw(rad)
void update_posture(float rr, float pr, float yr);
