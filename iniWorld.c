#include <string.h>
#include "map.h"
#include "buildMap.h"

int yylex();
extern FILE* yyin;

World* iniWorldWorld;

World* buildWorld(FILE* input) {
	iniWorldWorld = newWorld();
	yyin = input;
	yylex();

	return iniWorldWorld;
}
