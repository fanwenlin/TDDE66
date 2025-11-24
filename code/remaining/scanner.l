%{

#include <stdlib.h>
#include <string.h>

#if defined(LAB1)

#include "scanner.hh"

#elif defined(LAB2)

#include "scanner.hh"
#include "symtab.hh"

#else

#include "ast.hh"
#include "parser.hh"

#endif

int column = 0;

extern YYLTYPE yylloc; // Used for position information, see below.

#define SETLOC_IN_SINGLE_LINE() yylloc.first_line = yylineno; \
                              yylloc.first_column = column; \
                              column += yyleng; 



%}

%option yylineno
%option 8bit
%option noyywrap
%option caseless
%option outfile = "scanner.cc"

/* If you want to include any flex declarations, this is where to do it. */

%x c_comment
DIGIT [0-9]
EXP     [eE][+-]?{DIGIT}+
DEC1    {DIGIT}+\.{DIGIT}*
DEC2    \.{DIGIT}+
/* {DIGIT}+"."{DIGIT}*
 ID [a-z][a-z0-9]* */

/* Position information is entered into the yylloc variable, so we will be
   able to generate good error messages later in later phases. */
/* Your code should be entered below the %%. Expressions to handle the
   following: Diesel comments, Diesel string constants, Diesel
   identifiers, integers, reals, and whitespace. */
%%
\{[^\}]*\} { SETLOC_IN_SINGLE_LINE(); }
'([^\n']*('')?)*' { SETLOC_IN_SINGLE_LINE(); yylval.str = sym_tab->pool_install(sym_tab->fix_string(yytext)); return T_STRINGCONST; }

{DIGIT}+ { SETLOC_IN_SINGLE_LINE(); yylval.ival = atol(yytext); return T_INTNUM; }

({DEC1}|{DEC2})({EXP})?|{DIGIT}+{EXP} { SETLOC_IN_SINGLE_LINE(); yylval.rval = atof(yytext); return T_REALNUM; }


\.                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_DOT;
                         }
;                        {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_SEMICOLON;
                         }
=                        {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_EQ;
                         }
\:                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_COLON;
                         }
\(                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_LEFTPAR;
                         }
\)                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_RIGHTPAR;
                         }
\[                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_LEFTBRACKET;
                         }
\]                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_RIGHTBRACKET;
                         }
,                        {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_COMMA;
                         }
\<                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_LESSTHAN;
                         }
\>                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_GREATERTHAN;
                         }
\+                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_ADD;
                         }
\-                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_SUB;
                         }
\*                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_MUL;
                         }
\/                       {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_RDIV;
                         }
":="                     {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_ASSIGN;
                         }
"<>"                     {
                            yylloc.first_line = yylineno;
                            yylloc.first_column = column;
                            column += yyleng;
                            return T_NOTEQ;
                         }


of                       { SETLOC_IN_SINGLE_LINE(); return T_OF; }
if                       { SETLOC_IN_SINGLE_LINE(); return T_IF; }
do                       { SETLOC_IN_SINGLE_LINE(); return T_DO; }
or                       { SETLOC_IN_SINGLE_LINE(); return T_OR; }
var                      { SETLOC_IN_SINGLE_LINE(); return T_VAR; }
end                      { SETLOC_IN_SINGLE_LINE(); return T_END; }
and                      { SETLOC_IN_SINGLE_LINE(); return T_AND; }
div                      { SETLOC_IN_SINGLE_LINE(); return T_IDIV; }
mod                      { SETLOC_IN_SINGLE_LINE(); return T_MOD; }
not                      { SETLOC_IN_SINGLE_LINE(); return T_NOT; }
then                     { SETLOC_IN_SINGLE_LINE(); return T_THEN; }
else                     { SETLOC_IN_SINGLE_LINE(); return T_ELSE; }
const                    { SETLOC_IN_SINGLE_LINE(); return T_CONST; }
array                    { SETLOC_IN_SINGLE_LINE(); return T_ARRAY; }
begin                    { SETLOC_IN_SINGLE_LINE(); return T_BEGIN; }
while                    { SETLOC_IN_SINGLE_LINE(); return T_WHILE; }
elsif                    { SETLOC_IN_SINGLE_LINE(); return T_ELSIF; }
return                   { SETLOC_IN_SINGLE_LINE(); return T_RETURN; }
program                  {
   SETLOC_IN_SINGLE_LINE(); return T_PROGRAM;
                            
                         }
function                 { SETLOC_IN_SINGLE_LINE(); return T_FUNCTION; }
procedure                { SETLOC_IN_SINGLE_LINE(); return T_PROCEDURE; }


[a-zA-Z_][a-zA-Z0-9_]* { SETLOC_IN_SINGLE_LINE(); yylval.pool_p = sym_tab->pool_install(sym_tab->capitalize(yytext)); return T_IDENT; }


\/\/.*$                  column = 0; /* Skip single-line comment */
"/\*"                    {
                            column += yyleng;
                            BEGIN(c_comment);
                         }

<c_comment>
{
    "\*/"                {
                            column += 2;
                            BEGIN(INITIAL);
                         }
    "/\*"                {
                            column += 2;
                            yyerror("Suspicious comment");
                         }
    [^\n]                column++; /* Skip stuff in comments */
    \n                   column = 0;
    <<EOF>>              {
                            yyerror("Unterminated comment");
                            yyterminate();
                         }
}

\n {column = 0;}
[ \t\r\f\v]+ { SETLOC_IN_SINGLE_LINE(); }
 

<<EOF>>                  yyterminate();
.                        yyerror("Illegal character");
