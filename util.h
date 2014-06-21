/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  04/24/2014 07:41:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chengshuo (cshuo), chengshuo357951@gmail.com
 *        Company:  Nanjing Unversity
 *
 * =====================================================================================
 */
#ifndef __UTIL_H_
#define __UTIL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

typedef struct A_stack_* A_stack;
struct A_stack_
{
	int* bottom;
	int* top;
	int volume;
};



union value {
	int intNum;
	float floatNum;
	char *id;
};

/*多叉树结构体*/
typedef struct multi_tree
{
	char * op;
	int type;
	char * node_name;  
	union  value val;
	int    lineno;
	struct multi_tree **child;

}MultiTree;

MultiTree *newMultiTree(char *op, char *node_name, int type, union value *val, int lineno, int num, ...);


void walk_tree(MultiTree *);

/*删除节点*/
void tree_free(MultiTree *);


typedef struct A_node_* A_node;

struct A_node_
{
	enum {A_intNum, A_floatNum, A_id, A_type, A_nonLeaf, A_string, A_param, A_exp, A_struct, A_dim, A_defineFunction,
		A_defineVar, A_defineStruct, A_function, A_parameter, A_def, A_dec, A_expStmt, A_ifStmt, A_ifElseStmt, A_whileStmt,
	   	A_returnStmt, A_compSt} kind;
	union {struct {int value; int lineno;} int_leaf;
		   struct {float value; int lineno;} float_leaf;
		   struct {char* id; int lineno; int sp;} id_leaf;
		   struct {char* op; int lineno; char* id; int child_num; A_node* child; A_node sibling;} non_leaf;
		   struct {char* string;} string_leaf;
		   struct {int value; A_node sibling;} dim;
	} u;
};

void A_printTree(A_node root);

void A_print(A_node root);

A_node A_Build(MultiTree* root);

A_node A_ExtractExtdeflist(MultiTree* extdeflist);

void A_Extdef(A_node* extdef_a, MultiTree* extdef_b);

A_node A_ExtractExtdeclist(MultiTree* extdeclist);

void A_Fundec(A_node* fundec_a, MultiTree* fundec_b);

void A_Compst(A_node* compst_a, MultiTree* compst_b);

void A_Specifier(A_node* specifier_a, MultiTree* specifier_b);

A_node A_ExtractVardec(MultiTree* vardec);

void A_Dim(A_node* dim_a, MultiTree* dim_b);

void A_Id(A_node* id_a, MultiTree* id_b);

A_node A_ExtractVarlist(MultiTree* varlist);

A_node A_ExtractDeflist(MultiTree* deflist);

A_node A_ExtractStmtlist(MultiTree* stmtlist);

void A_Structspecifier(A_node* structspecifier_a, MultiTree* structspecifier_b);

void A_IntNum(A_node* intNum_a, MultiTree* intNum_b);

void A_Paramdec(A_node* paramdec_a, MultiTree* paramdec_b);

void A_Def(A_node* def_a, MultiTree* def_b);

void A_Stmt(A_node* stmt_a, MultiTree* stmt_b);

A_node A_ExtractDeclist(MultiTree* declist);

void A_Dec(A_node* dec_a, MultiTree* dec_b);

void A_Exp(A_node* exp_a, MultiTree* exp_b);

void A_String(A_node* string_a, char* string_b);

void A_FloatNum(A_node* floatNum_a, MultiTree* floatNum_b);

A_node A_ExtractArgs(MultiTree* args);


A_stack create_stack();
int* pop(A_stack);
int push(A_stack, int);
/**/


void reg_dec(FILE*, A_node, A_node);
void reg_exp(FILE*, A_node);

void M_init();
void M_alloc(A_node);

#endif
