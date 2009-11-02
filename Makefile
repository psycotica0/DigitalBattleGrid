grid: main.c cam.h cam.o map.h map.o chunk_string.o constants.h Makefile
	gcc -o grid main.c cam.o chunk_string.o map.o -I/System/Library/Frameworks/OpenGL.framework/Headers -L/System/Library/Frameworks/OpenGL.framework/Libraries -lgl `sdl-config --libs`

cam.o: cam.c cam.h
	gcc -c -o cam.o cam.c -I/System/Library/Frameworks/OpenGL.framework/Headers

map.o: map.c map.h chunk_string.h constants.h
	gcc -c -o map.o map.c -I/System/Library/Frameworks/OpenGL.framework/Headers

chunk_string.o: chunk_string.h chunk_string.c
	gcc -c -o chunk_string.o chunk_string.c

.PHONY: test
test: grid
	./grid
