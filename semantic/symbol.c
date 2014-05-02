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
	if(hash_node->var.name == NULL)	//结构体定义
		hash_no = hash_func(hash_node->var.type->u.structure->name);
	else if(hash_node->list_type == 0){		//变量
		hash_no = hash_func(hash_node->var.name);
		//printf("Func:add_hash()--->hash num is :%d\n",hash_no);
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
			//printf("Func:add_hash()--->alread exits\n");
			return -1;
		}
		else{											//open hashing,哈希值相同的插入链出链表
			//printf("Func:add_hash()--->open hashing\n");
			HashList* temp = (HashList*)malloc(sizeof(HashList));
			*temp = *hash_node;
			hash_head[hash_no].next = temp;
			assert(hash_head[hash_no].next!=NULL);
		}
	}
	//printf("Func:add_hash()--->insert success!!\n");
	return 0;
}


//获取变量的类型
VarList* get_varType(HashList* hash_head,char *node_name)		
{
	int hash_no;
	HashList* temp;
	
	hash_no = hash_func(node_name);

	if(hash_head[hash_no].list_type == -1){   //哈希槽为空
		//printf("Func:get_varType()---->no such variable!\n");
		return NULL;
	}
	else{
		if(hash_head[hash_no].next == NULL && hash_head[hash_no].list_type == 0){  //槽内只有一个值
			assert(&(hash_head[hash_no].var)!=NULL);
			return &(hash_head[hash_no].var);
		}
		else{
			for(temp = &hash_head[hash_no];temp!=NULL;temp = temp->next){
				//printf("Func:get_varType()---->in find\n");
				if(temp->var.name == NULL){					
					if(strcmp(temp->var.type->u.structure->name,node_name)==0){
						return &(temp->var);
					}
				}
				else{
					if(strcmp(temp->var.name,node_name)==0 && hash_head[hash_no].list_type == 0)
						return &(temp->var);
				}
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
		if(hash_head[hash_no].next == NULL && hash_head[hash_no].list_type == 1){
			return &(hash_head[hash_no].func);
		}
		else{			
			for(temp = &hash_head[hash_no];temp!=NULL;temp = temp->next){
				if(strcmp(temp->func.name,node_name)==0 && hash_head[hash_no].list_type == 1)
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
			if(a->var.name == NULL)
				return strcmp(a->var.type->u.structure->name,b->var.name)==0?0:-1;
			else
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

void hash_display(HashList* node)
{
	if(node==NULL){
		return;
	}
	if(node->list_type == 0){     //变量	
		printf("node_name:%s\n",node->var.name);
		if(node->var.type->kind == 0){
			printf("type:basic--%d\n",node->var.type->u.basic);
		}
		else if(node->var.type->kind == 1){
			printf("type: ");
			Type* ite_type = node->var.type;
			while(ite_type->kind == 1){
				printf("array---%d ",ite_type->u.array.size);
				ite_type = ite_type->u.array.elem;
			}
			printf("basic---%d\n",ite_type->u.basic);
		}
		else{
			printf("struct");
		}
	}
	else
	{			//函数
		assert(node->list_type == 1);
		FunList* func = &node->func;
		int i;

		printf("node_name:%s\n",func->name);
		printf("return type: ");		
		if(func->re_type->kind == 0)
			printf("basic: %d\n",func->re_type->u.basic);
		else
			printf("struct\n");

		if(func->num_arc != 0)
		{		//有参数
			for(i=0;i<func->num_arc;i++)
			{
				printf("type:%d\n",func->arc_type[i].kind);
				printf("arc %d: ",i);
				if(func->arc_type[i].kind == 0)
				{
					printf("basic: %d\n",func->arc_type[i].u.basic);
				}
				else if(func->arc_type[i].kind == 1)
				{
					printf("type: ");
					Type* ite_type = &(func->arc_type[i]);
					while(ite_type->kind == 1)
					{
						printf("array---%d ",ite_type->u.array.size);
						ite_type = ite_type->u.array.elem;
					}
					printf("basic---%d\n",ite_type->u.basic);
				}
				else
					printf("type:struct\n");
			}
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

//比较type是否相同
int typecmp(Type* type1, Type* type2)
{
	//assert(type1 != NULL && type2 != NULL);
	if(type1 == NULL || type2 == NULL)
		return -1;

	if(type1->kind != type2->kind)
		return -1;
	if(type1->kind == BASIC)
	{
		if(type1->u.basic != type2->u.basic)
		return -1;
		else
			return 0;
	}
	else if(type1->kind == ARRAY)
	{
		if(type1->u.array.size != type2->u.array.size)
			return -1;
		return typecmp(type1->u.array.elem, type2->u.array.elem);
	}
	else if(type1->kind == STRUCTURE)
	{ 
		if(memcmp(type1->u.structure->name, type2->u.structure->name,
					strlen(type1->u.structure->name)) != 0)
			return -1;
		else
			return 0;
	}
}
		

