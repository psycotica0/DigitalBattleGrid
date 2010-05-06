#include <string.c>
#include "map.h"

World* iniWorldWorld;
int iniWorldX;
int iniWorldY;
Room* iniWorldCurrentRoom;

World* buildWorld(FILE* input) {
	iniWorldWorld = newWorld();
	yyin = input;
	yyparse();

	return iniWorldWorld;
}
