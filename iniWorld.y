%{
#include <stdio.h>
#define YYSTYPE char*

void yyerror(const char* message) {
	fprintf(stderr, "Parse Error: %s\n", message);
}

int yywrap() {
	return 1;
}

%}

%token ROOM, MAP, DEFS, NAME_VALUE_REP, NAME_VALUE, CODE, NEWLINE

%%

file:
	/* empty */
	|
	file section
	;

section:
	RoomDef
	|
	MapDef
	|
	DefsDef
	;

RoomDef:
	ROOM NEWLINE ValueOrReps;

DefsDef:
	DEFS NEWLINE ValueOrReps;
MapDef:
	MAP NEWLINE MapRows;

MapRows:
	/* empty */
	|
	MapRows MapRow
	;

MapRow:
	/* empty */
	|
	MapRow NEWLINE
	|
	MapRow CODE
	;

ValueOrReps:
	/* empty */
	|
	ValueOrReps ValueOrRep NEWLINE
	;

ValueOrRep:
	NAME_VALUE_REP
	|
	NAME_VALUE
	;
