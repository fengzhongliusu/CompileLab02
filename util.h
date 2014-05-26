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
	enum {A_intNum, A_floatNum, A_id, A_nonLeaf} kind;
	union {struct {int value; int lineno} int_leaf;
		   struct {float value; int lineno} float_leaf;
		   struct {char* id; int lineno} id_leaf;
		   struct {char* op; int lineno; char* id; int child_num; A_node* child; A_node sibling;} non_leaf;
	} u;
};

/**/
#endif
