#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl.h>
#include <SDL/SDL.h>
#include "cam.h"
#include "map.h"
#include "constants.h"

Camera mainCam;


/* This holds the call-list for the tile */
GLuint tile;

World* testWorld;

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
	initMap();
	glShadeModel(GL_SMOOTH);
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glDepthFunc(GL_LEQUAL);

	testWorld = buildTestWorld();

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

	renderWorld(testWorld);

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
