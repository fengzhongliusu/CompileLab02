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
#include <assert.h>

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


typedef struct A_node_ *A_node;

struct A_node_
{
	enum {A_intNum, A_floatNum, A_id, A_nonLeaf, A_string} kind;
	union {struct {int value; int lineno;} int_leaf;
		   struct {float value; int lineno;} float_leaf;
		   struct {char* id; int lineno;} id_leaf;
		   struct {char* op; int lineno; char* id; int child_num; A_node* child; A_node sibling;} non_leaf;
		   struct {char* string;} string_leaf;
	} u;
};


A_node A_build(MultiTree* root);

A_node A_extractExtdeflist(MultiTree* extdeflist);

void A_extdef(A_node* extdef_a, MultiTree* extdef_b);

A_node A_extractExtdeclist(MultiTree* extdeclist);

void A_fundec(A_node* fundec_a, MultiTree* fundec_b);

void A_compst(A_node* compst_a, MultiTree* compst_b);

void A_specifier(A_node* specifier_a, MultiTree* specifier_b);

A_node A_extractVardec(MultiTree* vardec);

void A_id(A_node* id_a, MultiTree* id_b);

A_node A_extractVarlist(MultiTree* varlist);

A_node A_extractDeflist(MultiTree* deflist);

A_node A_extractStmtlist(MultiTree* stmtlist);

void A_structspecifier(A_node* structspecifier_a, MultiTree* structspecifier_b);

void A_intNum(A_node* intNum_a, MultiTree* intNum_b);

void A_paramdec(A_node* paramdec_a, MultiTree* paramdec_b);

void A_def(A_node* def_a, MultiTree* def_b);

void A_stmt(A_node* stmt_a, MultiTree* stmt_b);

void A_tag(A_node* tag_a, MultiTree* tag_b);

void A_opttag(A_node* optopttag_a, MultiTree* optopttag_b);

A_node A_extractDeclist(MultiTree* declist);

void a_exp(A_node* exp_a, MultiTree* exp_b);

void A_string(A_node* string_a, char* string_b);

void A_floatNum(A_node* floatNum_a, MultiTree* floatNum_b);

A_node A_extractArgs(MultiTree* args);


/**/
#endif
