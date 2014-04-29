/*
 * =====================================================================================
 *
 *       Filename:  symbol.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2014 09:47:27 PM
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
#include <assert.h>
#include "sem.h"


//向哈系表添加元素	
int add_hash(HashList* hash_head,HashList* hash_node)	
{
	int hash_no;
	if(hash_node->list_type == 0){		//变量
		hash_no = hash_func(hash_node->var.name);
		printf("hash num is :%d\n",hash_no);
	}
	else{								//函数
		hash_no = hash_func(hash_node->func.name);
	}

	if(hash_head[hash_no].list_type == -1){    // 哈希槽为空
		hash_head[hash_no] = *hash_node;
		assert(hash_head[hash_no].next==NULL);
	}
	else{
		if(hash_cmp(&hash_head[hash_no],hash_node)==0){   //已存在相同名称变量或函数
			printf("alread exits\n");
			return -1;
		}
		else{											//open hashing,哈希值相同的插入链出链表
			printf("open hashing\n");
			HashList* temp = (HashList*)malloc(sizeof(HashList));
			*temp = *hash_node;
			hash_head[hash_no].next = temp;
			assert(hash_head[hash_no].next!=NULL);
		}
	}
	printf("insert success!!\n");
	return 0;
}


//获取变量的类型
VarList* get_varType(HashList* hash_head,char *node_name)		
{
	int hash_no;
	HashList* temp;

	hash_no = hash_func(node_name);

	if(hash_head[hash_no].list_type == -1){   //哈希槽为空
		printf("no such variable!\n");
		return NULL;
	}
	else{
		if(hash_head[hash_no].next == NULL){  //槽内只有一个值
			return &(hash_head[hash_no].var);
		}
		else{
			for(temp = &hash_head[hash_no];temp!=NULL;temp = temp->next){
				printf("in find\n");
				if(strcmp(temp->var.name,node_name)==0)
					return &(temp->var);
			}
		}
	}
	return NULL;    //对应槽有值，但名称不是node_name
}



//获取函数的返回值类型以及参数类型
FunList* get_funType(HashList* hash_head,char *node_name)  
{
	int hash_no;
	HashList* temp;

	hash_no = hash_func(node_name);
	if(hash_head[hash_no].list_type == -1){
		return NULL;
	}
	else{
		if(hash_head[hash_no].next == NULL){
			return &(hash_head[hash_no].func);
		}
		else{			
			for(temp = &hash_head[hash_no];temp!=NULL;temp = temp->next){
				if(strcmp(temp->func.name,node_name)==0)
					return &(temp->func);
			}
		}
	}
	return NULL;   //对应槽有值，但名称不是node_name;
}


//比较哈希表元素是否相同(比较名字)
int hash_cmp(HashList* a,HashList* b)
{
	if(a->list_type == b->list_type){
		if(a->list_type == 0){   //a,b:var
			return strcmp(a->var.name,b->var.name)==0?0:-1;
		}
		else{
			return strcmp(a->func.name,b->func.name)==0?0:-1;
		}
	}
	else{
		if(a->list_type == 0){	//a:var b:func
			return strcmp(a->var.name,b->func.name)==0?0:-1;
		}
		else{
			return strcmp(a->func.name,b->var.name)==0?0:-1;
		}
	}
}


//哈希函数
unsigned int hash_func(char* name)
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val;
}

