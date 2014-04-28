%{	
	#include "syntax.tab.h"
	#include <string.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include "util.h"
	int error = 0;
	union value textval;
%}
%option yylineno

intNum		[0-9]+
floatNum	[0-9]+\.[0-9]+
type	int|float|char
id		[_a-zA-Z][_a-zA-Z0-9]*
relopt	>|<|>=|<=|==|!=
newline	\n
delim [ \t]
blank {delim}+

%%

"struct"	{textval.id = yytext; yylval.node = newMultiTree(NULL, "STRUCT", 3, &textval, yylineno, 0); return STRUCT;}
"return"	{textval.id = yytext; yylval.node = newMultiTree(NULL, "RETURN", 3, &textval, yylineno, 0); return RETURN;}
"if"		{textval.id = yytext; yylval.node = newMultiTree(NULL, "IF", 3, &textval, yylineno, 0); return IF;}
"else"		{textval.id = yytext; yylval.node = newMultiTree(NULL, "ELSE", 3, &textval, yylineno, 0); return ELSE;}
"while"		{textval.id = yytext; yylval.node = newMultiTree(NULL, "WHILE", 3, &textval, yylineno, 0); return WHILE;}
{type}		{textval.id = yytext; yylval.node = newMultiTree(NULL, "TYPE", 3, &textval, yylineno, 0); return TYPE;}
";"			{textval.id = yytext; yylval.node = newMultiTree(NULL, "SEMI", 3, &textval, yylineno, 0); return SEMI;}
","			{textval.id = yytext; yylval.node = newMultiTree(NULL, "COMMA", 3, &textval, yylineno, 0); return COMMA;}
"="			{yylval.op = strdup(yytext); return ASSIGNOP;}
{relopt}	{yylval.op = strdup(yytext); return RELOP;}
"+"			{yylval.op = strdup(yytext); return PLUS;}
"-"			{yylval.op = strdup(yytext); return MINUS;}
"*"			{yylval.op = strdup(yytext); return MUL;}
"/"			{yylval.op = strdup(yytext); return DIV;}
"&&"		{yylval.op = strdup(yytext); return AND;}
"||"		{yylval.op = strdup(yytext); return OR;}
"."			{yylval.op = strdup(yytext); return DOT;}
"!"			{yylval.op = strdup(yytext); return NOT;}
"("			{textval.id = yytext; yylval.node = newMultiTree(NULL, "LP", 3, &textval, yylineno, 0); return LP;}
")"			{textval.id = yytext; yylval.node = newMultiTree(NULL, "RP", 3, &textval, yylineno, 0); return RP;}
"["			{textval.id = yytext; yylval.node = newMultiTree(NULL, "LB", 3, &textval, yylineno, 0); return LB;}
"]"			{textval.id = yytext; yylval.node = newMultiTree(NULL, "RB", 3, &textval, yylineno, 0); return RB;}
"{"			{textval.id = yytext; yylval.node = newMultiTree(NULL, "LC", 3, &textval, yylineno, 0); return LC;}
"}"			{textval.id = yytext; yylval.node = newMultiTree(NULL, "RC", 3, &textval, yylineno, 0); return RC;}
{intNum}	{textval.intNum = atoi(yytext); yylval.node = newMultiTree(NULL, "INT", 1, &textval,yylineno, 0); return INT;}
{floatNum}	{textval.floatNum =(float)atof(yytext); yylval.node = newMultiTree(NULL, "FLOAT", 2,&textval, yylineno, 0); return FLOAT;}
{id}		{textval.id = yytext; yylval.node = newMultiTree(NULL, "ID", 3, &textval, yylineno, 0); return ID;}
{blank}		{}
{newline}	{}
.			{
				printf("error in %d,error lexical: %s\n",yylineno,yytext);
				error += 1;
			}

%%








