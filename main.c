#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl.h>
#include <SDL/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define DegToRad M_PI/180

#define tileSize 4.5f
#define edgeSize 0.5f

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

Camera mainCam;

float camMatrix[16];

void UseCam(Camera* cam) {
	/* Calculate All Angles */
	/* (Negate the angles becuase we are actually dealing with the object, not the cam) */
	float sinYaw = sin(DegToRad * -cam->yaw);
	float cosYaw = cos(DegToRad * -cam->yaw);
	float sinPitch = sin(DegToRad * -cam->pitch);
	float cosPitch = cos(DegToRad * -cam->pitch);
	float sinRoll = sin(DegToRad * -cam->roll);
	float cosRoll = cos(DegToRad * -cam->roll);

	camMatrix[0] = (cosYaw * cosRoll) + (sinYaw * sinPitch * sinRoll);
	camMatrix[1] = (cosYaw * sinRoll) - (sinYaw * sinPitch * cosRoll);
	camMatrix[2] = sinYaw * cosPitch;

	camMatrix[4] = -cosPitch * sinRoll;
	camMatrix[5] = cosPitch * cosRoll;
	camMatrix[6] = sinPitch;

	camMatrix[8] = (-sinYaw * cosRoll) + (cosYaw * sinPitch * sinRoll);
	camMatrix[9] = (-sinYaw * sinRoll) - (cosYaw * sinPitch * cosRoll);
	camMatrix[10] = cosYaw * cosPitch;

	/* Translate */
	/* In order to work normally it has to be translated with respect to the rotations we've performed above */
	camMatrix[12] = -1 * ((cam->x * camMatrix[0]) + (cam->y *camMatrix[4]) + (cam->z * camMatrix[8]));
	camMatrix[13] = -1 * ((cam->x * camMatrix[1]) + (cam->y *camMatrix[5]) + (cam->z * camMatrix[9]));
	camMatrix[14] = -1 * ((cam->x * camMatrix[2]) + (cam->y *camMatrix[6]) + (cam->z * camMatrix[10]));

	glLoadMatrixf(camMatrix);
}


/* This holds the call-list for the tile */
GLuint tile;

void viewPort(int width, int height) {
	GLfloat ratio = (GLfloat) height / (GLfloat) width;

	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0f, 5.0f, -5 * ratio, 5 * ratio, 5.0f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glDepthFunc(GL_LEQUAL);

	tile=glGenLists(1);
	glNewList(tile, GL_COMPILE);
	/* This tile is centred on the point given. */
	glBegin(GL_QUADS);
	/* First the tile */
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(-(tileSize/2), 0.0f, -(tileSize/2));
	glVertex3f(tileSize/2, 0.0f, -(tileSize/2));
	glVertex3f(tileSize/2, 0.0f, tileSize/2);
	glVertex3f(-(tileSize/2), 0.0f, tileSize/2);

	/* Now the edgdes */
	glColor3f(0.0f, 0.0f, 0.0f);
	/* Back Edge */
	glVertex3f(-(tileSize/2), 0.0f, -(tileSize/2));
	glVertex3f((tileSize/2)+edgeSize, 0.0f, -(tileSize/2));
	glVertex3f((tileSize/2)+edgeSize, 0.0f, -((tileSize/2)+edgeSize));
	glVertex3f(-(tileSize/2), 0.0f, -((tileSize/2)+edgeSize));

	/* Right Edge */
	glVertex3f(tileSize/2, 0.0f, -(tileSize/2));
	glVertex3f(tileSize/2, 0.0f, (tileSize/2)+edgeSize);
	glVertex3f((tileSize/2)+edgeSize, 0.0f, (tileSize/2)+edgeSize);
	glVertex3f((tileSize/2)+edgeSize, 0.0f, -(tileSize/2));

	/* Front Edge */
	glVertex3f(tileSize/2, 0.0f, tileSize/2);
	glVertex3f(-((tileSize/2)+edgeSize), 0.0f, tileSize/2);
	glVertex3f(-((tileSize/2)+edgeSize), 0.0f, (tileSize/2)+edgeSize);
	glVertex3f(tileSize/2, 0.0f, (tileSize/2)+edgeSize);

	/* Left Edge */
	glVertex3f(-(tileSize/2), 0.0f, tileSize/2);
	glVertex3f(-(tileSize/2), 0.0f, -((tileSize/2)+edgeSize));
	glVertex3f(-((tileSize/2)+edgeSize), 0.0f, -((tileSize/2)+edgeSize));
	glVertex3f(-((tileSize/2)+edgeSize), 0.0f, tileSize/2);

	glEnd();
	glEndList();

	/* Initialize Camera */
	mainCam.x = 10;
	mainCam.y = 8;
	mainCam.z = 20;

	mainCam.yaw = 0;
	mainCam.pitch = -30;
	mainCam.roll = 0;

	/* Initialize Cam Matrix */
	{
		int x;
		for (x = 0; x < 16; x++) {
			switch (x) {
				case 0:
				case 5:
				case 10:
				case 15:
					/* The Diagonals */
					/* We want to start with identity */
					camMatrix[x] = 1;
					break;
				default:
					camMatrix[x] = 0;
			}
		}
	}
}

void Draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	UseCam(&mainCam);
	{
		int x;
		int z;
		int y;
		for (y = 0; y < 2; y++) {
			for (z = 0; z < 5; z++) {
				for (x = 0; x < 5; x++) {
					glCallList(tile);
					glTranslatef(tileSize + edgeSize, 0.0f, 0.0f);
				}
				glTranslatef(-5 * (tileSize+edgeSize), 0.0f, tileSize + edgeSize);
			}
			glTranslatef(0.0f, -(tileSize + edgeSize)/2, -(tileSize + edgeSize)/2);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			for (x = 0; x < 5; x++) {
				glCallList(tile);
				glTranslatef(tileSize + edgeSize, 0.0f, 0.0f);
			}
			glTranslatef(-5 * (tileSize+edgeSize), 0.0f, 0.0f);
			glRotatef(90, -1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, -(tileSize + edgeSize)/2, (tileSize + edgeSize)/2);
		}
	}

	SDL_GL_SwapBuffers();

	return;
}

#define WIDTH 800
#define HEIGHT 400
#define DEG_SPEED 5.0f
#define FEET_SPEED 0.5f

int main(int argc, char* argv[]) {
	SDL_Surface *screen;
	int done=0;
	Uint8* keyState;

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, SDL_OPENGL);
	if (!screen) {
		fprintf(stderr, "Couldn't Set Video Mode. Error: %s\n", SDL_GetError);
		SDL_Quit();
		exit(2);
	}

	SDL_WM_SetCaption("Battle Grid", "battlegrid");
	init();
	viewPort(screen->w, screen->h);

	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case  SDL_QUIT:
					done=1;
					break;
			}
		}
		keyState = SDL_GetKeyState(NULL);

		if (keyState[SDLK_UP]) {
			mainCam.z -= FEET_SPEED;
		}
		if (keyState[SDLK_DOWN]) {
			mainCam.z += FEET_SPEED;
		}
		if (keyState[SDLK_LEFT]) {
			mainCam.x -= FEET_SPEED;
		}
		if (keyState[SDLK_RIGHT]) {
			mainCam.x += FEET_SPEED;
		}
		if (keyState[SDLK_i]) {
			mainCam.y += FEET_SPEED;
		}
		if (keyState[SDLK_k]) {
			mainCam.y -= FEET_SPEED;
		}
		if (keyState[SDLK_j]) {
			mainCam.yaw -= DEG_SPEED;
		}
		if (keyState[SDLK_l]) {
			mainCam.yaw += DEG_SPEED;
		}
		if (keyState[SDLK_w]) {
			mainCam.pitch += DEG_SPEED;
		}
		if (keyState[SDLK_s]) {
			mainCam.pitch -= DEG_SPEED;
		}
		if (keyState[SDLK_a]) {
			mainCam.roll -= DEG_SPEED;
		}
		if (keyState[SDLK_d]) {
			mainCam.roll += DEG_SPEED;
		}
		Draw();
		usleep(125000);
	}

	SDL_Quit();
	return 0;
}
