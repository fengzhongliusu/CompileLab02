%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>
	#include "util.h"
	#include "lex.yy.c"
	MultiTree* root;
	extern int error;
%}

%union {
	MultiTree *node;
	char *op;
}


%token	<node>	INT FLOAT ID
%token	<op>	PLUS MINUS MUL DIV
%token	<node>	SEMI COMMA TYPE LP RP LB RB LC RC 
%token	<op>	ASSIGNOP RELOP AND OR DOT NOT 
%token	<node>	STRUCT RETURN IF ELSE WHILE
%type	<node>	Program	ExtDefList ExtDef ExtDecList Specifier OptTag Tag VarDec FunDec	VarList	ParamDec CompSt	StmtList Stmt Def DecList Dec Exp Args StructSpecifier DefList	

%right	ASSIGNOP
%left	OR
%left	AND
%left	RELOP
%left	PLUS MINUS
%left	MUL DIV
%right	NOT
%left	DOT LP RP LB RB 

%nonassoc   LOWER_THAN_ELSE
%nonassoc	ELSE

%%
Program		:	ExtDefList	{
			$$ = newMultiTree(NULL, "Program", 3, NULL, $1->lineno, 1, $1);
			//assert($1->node_name != NULL);
			assert($$->child[0] == $1);
			root = $$;}
			;

ExtDefList	:	/*empty*/	{
			$$ = NULL;}
			|	ExtDef ExtDefList	{
			$$ = newMultiTree(NULL, "ExtDefList", 3, NULL, $1->lineno, 2, $1, $2);}
			;

ExtDef		:	Specifier ExtDecList SEMI {
			$$ = newMultiTree(NULL, "ExtDef", 3, NULL, $1->lineno, 3, $1, $2, $3);}
			|	Specifier SEMI {
			$$ = newMultiTree(NULL, "ExtDef", 3, NULL, $1->lineno, 2, $1, $2);}
			|	Specifier FunDec CompSt	{
			$$ = newMultiTree(NULL, "ExtDef", 3, NULL, $1->lineno, 3, $1, $2, $3);}
			;

ExtDecList	:	VarDec	{
			$$ = newMultiTree(NULL, "ExtDecList", 3, NULL, $1->lineno, 1, $1);}
			|	VarDec COMMA ExtDecList		{
			$$ = newMultiTree(NULL, "ExtDecList", 3, NULL, $1->lineno, 3, $1, $2, $3);}
			;

Specifier	:	TYPE	{
			$$ = newMultiTree(NULL, "Specifier", 3, NULL, $1->lineno, 1, $1);}
			|	StructSpecifier		{
			$$ = newMultiTree(NULL, "Specifier", 3, NULL, $1->lineno, 1, $1);}
			;

StructSpecifier	   :	STRUCT OptTag LC DefList RC		{
						$$ = newMultiTree(NULL, "StructSpecifier", 3, NULL, $1->lineno, 5, $1, $2, $3, $4, $5);}
				   |	STRUCT Tag		{
		$$ = newMultiTree(NULL, "StructSpecifier", 3, NULL, $1->lineno, 2, $1, $2);}	
				   ;

OptTag	:	/*empty*/	{
	    $$ = NULL;}
		|	ID		{
		$$ = newMultiTree(NULL, "OptTag", 3, NULL, $1->lineno, 1, $1);}
		;

Tag		:	ID		{
			$$ = newMultiTree(NULL, "Tag", 3, NULL, $1->lineno, 1, $1);}
		;

VarDec	:	ID	{
			$$ = newMultiTree(NULL, "VarDec", 3, NULL, $1->lineno, 1, $1);}
		|	VarDec LB INT RB	{
		$$ = newMultiTree(NULL, "VarDec", 3, NULL, $1->lineno, 4, $1, $2, $3, $4);}
		|	VarDec LB error RB {}
		;

FunDec	:	ID LP VarList RP	{
			$$ = newMultiTree(NULL, "FunDec", 3, NULL, $1->lineno, 4, $1, $2, $3, $4);}
		|	ID LP RP			{
		$$ = newMultiTree(NULL, "FunDec", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		|	error RP			{}
		;

VarList	:	ParamDec COMMA VarList		{
				$$ = newMultiTree(NULL, "VarList", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		|	ParamDec					{
		$$ = newMultiTree(NULL, "VarList", 3, NULL, $1->lineno, 1, $1);}
		;

ParamDec	:	Specifier VarDec	{
				$$ = newMultiTree(NULL, "ParamDec", 3, NULL, $1->lineno, 2, $1, $2);}
			;

CompSt	:	LC DefList StmtList RC	{
			$$ = newMultiTree(NULL, "CompSt", 3, NULL, $1->lineno, 4, $1, $2, $3, $4);}
			| error RC	{}
		;

StmtList	:	/*empty*/			{
		$$ = NULL;}
		|	Stmt StmtList		{
		$$ = newMultiTree(NULL, "StmtList", 3, NULL, $1->lineno, 2, $1, $2);}
		;

Stmt	:	Exp SEMI				{
			$$ = newMultiTree(NULL, "Stmt", 3, NULL, $1->lineno, 2, $1, $2);}
		|	CompSt					{
		$$ = newMultiTree(NULL, "Stmt", 3, NULL, $1->lineno, 1, $1);}
		|	RETURN Exp SEMI			{
		$$ = newMultiTree(NULL, "Stmt", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		|	IF LP Exp RP Stmt	%prec LOWER_THAN_ELSE	{
		$$ = newMultiTree(NULL, "Stmt", 3, NULL, $1->lineno, 5, $1, $2, $3, $4, $5);}
		|	IF LP Exp RP Stmt ELSE Stmt		{
		$$ = newMultiTree(NULL, "Stmt", 3, NULL, $1->lineno, 7, $1, $2, $3, $4, $5, $6, $7);}
		|	WHILE LP Exp RP Stmt	{
		$$ = newMultiTree(NULL, "Stmt", 3, NULL, $1->lineno, 5, $1, $2, $3, $4, $5);}
		|	error SEMI				{}
		;

DefList    :	/*empty*/	{
				$$ = NULL;}
		   |	 Def DefList	{
		$$ = newMultiTree(NULL, "DefList", 3, NULL, $1->lineno, 2, $1, $2);}
		   ;

Def		:		Specifier DecList SEMI {
			$$ = newMultiTree(NULL, "Def", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		;

DecList		:	Dec		{
				$$ = newMultiTree(NULL, "DecList", 3, NULL, $1->lineno, 1, $1);}
			|	Dec COMMA DecList	{
		$$ = newMultiTree(NULL, "DecList", 3, NULL, $1->lineno, 3, $1, $2, $3);}
			;

Dec		:	VarDec		{
			$$ = newMultiTree(NULL, "Dec", 3, NULL, $1->lineno, 1, $1);}
		|	VarDec ASSIGNOP Exp		{
		$$ = newMultiTree($2, "Dec", 3, NULL, $1->lineno, 2, $1, $3);}
		;

Exp		:	Exp ASSIGNOP Exp	{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp AND Exp			{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp OR Exp			{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp RELOP Exp		{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp PLUS Exp		{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp MINUS Exp		{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp MUL Exp			{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	Exp DIV	Exp			{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	LP Exp RP			{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		|	MINUS Exp			{
		$$ = newMultiTree($1, "Exp", 3, NULL, $2->lineno, 1, $2);}
		|	NOT Exp				{
		$$ = newMultiTree($1, "Exp", 3, NULL, $2->lineno, 1, $2);}
		|	ID LP Args RP		{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 4, $1, $2, $3, $4);}
		|	ID LP RP			{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		|	Exp	LB Exp RB		{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 4, $1, $2, $3, $4);}
		|	Exp	DOT ID			{
		$$ = newMultiTree($2, "Exp", 3, NULL, $1->lineno, 2, $1, $3);}
		|	ID					{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 1, $1);}
		|	INT					{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 1, $1);}
		|	FLOAT				{
		$$ = newMultiTree(NULL, "Exp", 3, NULL, $1->lineno, 1, $1);}
		|	error RP			{}
		|	Exp LB error RB			{}
		;

Args	:	Exp	COMMA Args		{
			$$ = newMultiTree(NULL, "Args", 3, NULL, $1->lineno, 3, $1, $2, $3);}
		|	Exp					{
		$$ = newMultiTree(NULL, "Args", 3, NULL, $1->lineno, 1, $1);}	
		;

%%

yyerror(char* msg){
	error += 1;
	fprintf(stderr,"error in %d : %s\n",yylineno,msg);
}

