#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl.h>
#include <SDL/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

void viewPort(int width, int height) {
	GLfloat ratio = (GLfloat) height / (GLfloat) width;

	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -ratio, ratio, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initWindow() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(10.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void Draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-1.5f, 0.0f, -15.0f);
	glBegin(GL_TRIANGLES);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	glTranslatef(3.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);

	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f,-1.0f, 0.0f);

	glEnd();

	SDL_GL_SwapBuffers();

	return;
}

#define WIDTH 800
#define HEIGHT 400

int main(int argc, char* argv[]) {
	SDL_Surface *screen;
	int done=0;


	SDL_Init(SDL_INIT_VIDEO);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, SDL_OPENGL);
	if (!screen) {
		fprintf(stderr, "Couldn't Set Video Mode. Error: %s\n", SDL_GetError);
		SDL_Quit();
		exit(2);
	}

	SDL_WM_SetCaption("Battle Grid", "battlegrid");
	initWindow();
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
		Draw();
		usleep(125000);
	}

	SDL_Quit();
	return 0;
}
