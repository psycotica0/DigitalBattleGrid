%{
#include <string.h>
#include <stdio.h>
#include "map.h"

extern World* iniWorldWorld;

int iniWorldX;
int iniWorldY;
Room* iniWorldCurrentRoom;

int yywrap() {
	return 1;
}

/* This function parses out the ID for a section, or returns NULL if there isn't one */
char* getId(char* section) {
	char* temp;
	if (temp = strpbrk(section, " ]")) {
		/* Skip all space chars */
		temp += strspn(temp, " ");
		if (*temp == ']') {
			return NULL;
		}
		/* Return the string from temp to the first space or bracket */
		return strsep(&temp, " ]");
	}
	return NULL;
}

/* This disects a definition like S2=Stitch(r1,S1):01 and, in this case, would return: */
/*
code = "S2"
function = "Stitch"
args = "r1,S1"
repType = "01"

and return 1
*/
/* If it decides that it is of an invalid format it could return 0 */
/* Input could easily be mangled by this function */
int getTileDefFields(char* input, char** code, char** function, char** args, char** repType) {
	(*code) = strtok(input, " =");
	(*function) = strtok(NULL, " (");
	(*args) = strtok(NULL, ")");
	(*repType) = strtok(NULL, " :");

	return 1;
}



%}

%s ROOM DEFS MAP
%%

[[]Room(\ [a-zA-Z0-9]*)?[]] {
	char* id = getId(yytext);
	iniWorldCurrentRoom = getRoom(iniWorldWorld, id);
	BEGIN ROOM;
}

[[]Map(\ [a-zA-Z0-9]*)?[]] {
	char* id = getId(yytext);
	if (id != NULL) {
		iniWorldCurrentRoom = getRoom(iniWorldWorld, id);
	}
	iniWorldX = 0;
	iniWorldY = 0;
	BEGIN MAP;
}

[[]Defs(\ [a-zA-Z0-9]*)?[]] {
	char* id = getId(yytext);
	if (id != NULL) {
		iniWorldCurrentRoom = getRoom(iniWorldWorld, id);
	}
	BEGIN DEFS;
}

<DEFS>[^=\n]*=[^\n:]*:[[:alnum:]]{2} | 

<DEFS>[^=\n]*=[^\n]* {
	char* code;
	char* function;
	char* args;
	char* repType;

	if (getTileDefFields(yytext, &code, &function, &args, &repType)) {
		setTileDef(iniWorldCurrentRoom, code, function, args, repType);
	}
}


<ROOM>[^=\n]*=[^\n]* {
	char* name;
	char* value;

	value = strpbrk(yytext, "=");
	(*value) = '\0';
	value++;
	name = yytext;

	setMetadata(iniWorldCurrentRoom, name, value);
}

<MAP>[[:alnum:].]{2} {
	setTile(iniWorldCurrentRoom, iniWorldX, iniWorldY, yytext);
	iniWorldX++;
}

<MAP>\n+ {
	if (iniWorldX != 0) {
		/* If iniWorldX is still 0, then this is a blank line we're ending */
		iniWorldX = 0;
		iniWorldY++;
	}
}

\n |
[ ]* /* Ignore For Now */;

%%
