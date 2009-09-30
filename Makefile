grid: main.c Makefile
	gcc -o grid main.c -I/System/Library/Frameworks/OpenGL.framework/Headers -L/System/Library/Frameworks/OpenGL.framework/Libraries -lgl `sdl-config --libs`

.PHONY: test
test: grid
	./grid
