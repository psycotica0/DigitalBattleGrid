grid: main.c cam.h cam.o Makefile
	gcc -o grid main.c cam.o -I/System/Library/Frameworks/OpenGL.framework/Headers -L/System/Library/Frameworks/OpenGL.framework/Libraries -lgl `sdl-config --libs`

cam.o: cam.c cam.h
	gcc -c -o cam.o cam.c -I/System/Library/Frameworks/OpenGL.framework/Headers

.PHONY: test
test: grid
	./grid
