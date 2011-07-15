grid: main.c cam.h cam.o map.h map.o chunk_string.o constants.h buildMap.h iniWorld.o iniWorld.yy.o Makefile
	gcc -o grid main.c cam.o chunk_string.o map.o iniWorld.o iniWorld.yy.o `pkg-config --libs --cflags gl` `sdl-config --libs` -lm

cam.o: cam.c cam.h
	gcc -c -o cam.o cam.c `pkg-config --cflags gl` -g

map.o: map.c map.h chunk_string.h constants.h
	gcc -c -o map.o map.c `pkg-config --cflags gl` -g

chunk_string.o: chunk_string.h chunk_string.c
	gcc -c -o chunk_string.o chunk_string.c -g

iniWorld.o: iniWorld.c buildMap.h map.h
	gcc -c -o iniWorld.o iniWorld.c `pkg-config --cflags gl` -g 

iniWorld.yy.c: iniWorld.l map.h
	lex -oiniWorld.yy.c iniWorld.l -g 

iniWorld.yy.o: iniWorld.yy.c
	gcc -c -o iniWorld.yy.o iniWorld.yy.c `pkg-config --cflags gl` -g

testMap.o: map.h buildMap.h testMap.c
	gcc -c -o testMap.o testMap.c `pkg-config --cflags gl` -g

iniOutput.o: iniOutput.c outputMap.h
	gcc -c -o iniOutput.o iniOutput.c `pkg-config --cflags gl` -g

testScan: iniWorld.o testScan.c buildMap.h map.o iniWorld.yy.o iniOutput.o
	gcc -o testScan testScan.c iniWorld.o map.o iniWorld.yy.o iniOutput.o `pkg-config --cflags --libs gl` -lm -g


.PHONY: test clean
test: grid
	./grid

clean:
	rm -f *.o
	rm -f grid
