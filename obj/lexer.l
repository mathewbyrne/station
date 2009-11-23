%option noyywrap
%option yylineno

%{
#include "grammar.tab.h"

static YY_BUFFER_STATE root_buffer;
%}

DIGIT   [0-9]

%%

                    /*****************************************************/
                    /* OBJ scanner */

                    /* Keywords */
^v                  return V;
^vt                 return VT;
^vn                 return VN;
^f                  return F;
^g                  return G;
^usemtl             return USEMTL;

                    /* Operators */
"/"                 return SLASH;


                    /*****************************************************/
                    /* Common for both file types */
<*>{

#[^\n]*\n           /* Comment */

                    /* Integers */
[+-]?{DIGIT}+        {   yylval.integer = atoi(yytext);
                        return INTEGER;
                    }

                    /* Floats */
[+-]?{DIGIT}+"."({DIGIT}*)?([eE][+-]?{DIGIT}+)? |
[+-]?{DIGIT}+("."{DIGIT}*)?[eE][+-]?{DIGIT}+ |
[+-]?"."{DIGIT}+([eE][+-]?{DIGIT}+)? {
                        yylval.real = strtof(yytext, NULL);
                        return REAL;
                    }

                    /* Identifiers */
[a-zA-Z_][^ \t\n\r]* {   
                        yylval.str = yytext;
                        return STRING;
                    }

[ \t\n\r]+          /* Consume whitespace */
}

%%

