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
	int i;
	for(i=0;i<HASH_SIZE;i++){
		hash_table[i].list_type = -1;
	}

	yyrestart(f);
	//yydebug = 1;
	yyparse();
	/*if(error==0){
		walk_tree(root);
	}*/

	sem_analy(root);

	FunList* fun = get_funType(hash_table,"main");
	HashList node;
	node.list_type = 1;
	if(fun!=NULL){
		node.func = *fun;
		node.next = NULL;
		//hash_display(&node);
	}

	VarList* var = get_varType(hash_table,"a");
	if(var!=NULL){
		//printf("a find!!!\n");
	}

	VarList* var1 = get_varType(hash_table,"i");
	if(var1!=NULL){
		node.list_type = 0;
		node.var = *var1;
		//hash_display(&node);
	}


	
	hash_heap_no = 0;
	type_heap_no = 0;

	exit(0);
}

