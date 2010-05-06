%{
#include <string.h>
#include <stdio.h>
#include "iniWorld.tab.h"

extern yylval;

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
%}

%%

[[]Room(\ [a-zA-Z0-9]*)?[]] yylval=srtdup(getId(yytext)); return ROOM;
[[]Map(\ [a-zA-Z0-9]*)?[]] yylval=strdup(getId(yytext)); return MAP;
[[]Defs(\ [a-zA-Z0-9]*)?[]] yylval=strdup(getId(yytext)); return DEFS;

[^=\n]*=[^\n:]*:[[:alnum:]]{2} yylval=strdup(yytext); return NAME_VALUE_REP;

[^=\n]*=[^\n]* yylval=strdup(yytext); return NAME_VALUE;

[[:alnum:].]{2} yylval=strdup(yytext); return CODE;

\n+ return NEWLINE;
[ ]* /* Ignore For Now */;

%%
