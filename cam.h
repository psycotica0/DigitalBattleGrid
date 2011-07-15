#include <GL/gl.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define DegToRad M_PI/180

/* 
This struct keeps track of the camera position and rotation 

yaw is rotation around the y axis, where positive turns to the right
pitch is the rotation around the x axis, where positive looks up
roll is rotation around z axis where positive tilts to the right
*/

typedef struct {
	float x,y,z;
	float yaw, pitch, roll;
} Camera;

float camMatrix[16];

void UseCam(Camera*);
