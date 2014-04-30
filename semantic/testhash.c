/*
 * =====================================================================================
 *
 *       Filename:  testhash.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2014 11:28:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chengshuo (cshuo), chengshuo357951@gmail.com
 *        Company:  Nanjing Unversity
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sem.h"

#define HASH_SIZE 0x4000

void main()
{
	HashList hash_table[HASH_SIZE];
	VarList* temp;
	HashList a[5];
	int i;
	for(i=0;i<HASH_SIZE;i++){
		hash_table[i].list_type = -1;
	}

	for(i = 0;i<5;i++){
		a[i].list_type = 0;
		a[i].var.type = (Type*)malloc(sizeof(Type));
		a[i].var.type-> kind = 0;
		a[i].var.type->u.basic = 1;
		a[i].next = NULL;

	}
	a[0].var.name = strdup("a0");
	a[1].var.name = strdup("a1");
	a[2].var.name = strdup("a2");
	a[3].var.name = strdup("a3");
	a[4].var.name = strdup("hello");


	for(i=0;i<5;i++){
		printf("success? %d\n",add_hash(hash_table,&a[i]));
	}

	temp = get_varType(hash_table,"hello");
	if(temp!=NULL)
	printf("%s %d\n",temp->name,temp->type->kind);
}
