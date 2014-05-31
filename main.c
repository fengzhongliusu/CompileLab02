/*
 * =====================================================================================
 *
 *       Filename:  test_var.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2014 11:32:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chengshuo (cshuo), chengshuo357951@gmail.com
 *        Company:  Nanjing Unversity
 *
 * =====================================================================================
 */


#include <stdio.h>
#include "util.h"
#include "semantic/sem.h"


extern int error;
extern MultiTree* root;
extern int yydebug;
A_node pg;

int main(int argc,char** argv)
{
	if(argc <=1){
		return 1;
	}

	FILE* f = fopen(argv[1],"r");
	if(!f){
		perror(argv[1]);
		return 1;
	}


	hash_heap_no = 0;
	type_heap_no = 0;
	block_no = 0;

	int i;
	for(i=0;i<HASH_SIZE;i++){
		hash_table[i].list_type = -1;
	}

	for(i=0; i<MAX_SYMBOL; i++){
		symbol_head[i].list_type = -1;
		symbol_head[i].next = NULL;
		symbol_head[i].block_next = NULL;
	}

	yyrestart(f);
	//yydebug = 1;
	yyparse();
	if(error != 0){
		exit(1);
	}

	//walk_tree(root);
	pg = A_Build(root);
	A_printTree(pg);
	//sem_analy(root);
	printf("\n");
	sem_analy(root);
	printf("\n");

	/*FunList* fun = get_funType("main");
	HashList node;
	node.list_type = 1;
	if(fun!=NULL){
		node.func = *fun;
		node.next = NULL;
		//hash_display(&node);
	}

	VarList* var = get_varType("a");
	if(var!=NULL){
		//printf("a find!!!\n");
	}

	VarList* var1 = get_struct_var("s");
	if(var1!=NULL){
		printf("not added!!\n");
	}*/

	
	hash_heap_no = 0;
	type_heap_no = 0;

	exit(0);
}

