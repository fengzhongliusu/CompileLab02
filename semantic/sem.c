/*
 * =====================================================================================
 *
 *       Filename:  sem.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2014 08:45:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chengshuo (cshuo), chengshuo357951@gmail.com
 *        Company:  Nanjing Unversity
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "../util.h"
#include <string.h>
#include "sem.h"


extern HashList hash_table[HASH_SIZE];


void sem_analy(MultiTree* root)
{
	int i;
	if(root != NULL){
		if(strcmp(root->node_name,"ExtDef")==0){		//全局变量,函数,结构体积定义
			Type* type = (Type*)malloc(sizeof(Type));
			parse_extdef(type,root);
			printf("--now rule : ExtDef\n");
		}
		else{
			if(root->child!=NULL)
			{
				for(i=0;root->child[i]!=NULL;i++)
				{
					sem_analy(root->child[i]);
				}
			}
		}
	}
}


/**
 *处理全局变量的定义
 */
void parse_extdef(Type* type,MultiTree* root)
{
	MultiTree* spe_node = root->child[0];
	
	if(strcmp(spe_node->child[0]->node_name,"TYPE")==0){		
		type->kind = BASIC;
		assert(spe_node->child[0]->val.id!=NULL);
		if(strcmp(spe_node->child[0]->val.id,"int")==0){
			type->u.basic = 0;	
		}else if(strcmp(spe_node->child[0]->val.id,"float")==0){
			type->u.basic = 1;
		}else{
			printf("unsport!!\n");
		}
	}
	else{	//结构体类型
		//TODO
	}

	if(strcmp(root->child[1]->node_name,"ExtDecList")==0)
	{
		walk_extdeclist(type,root->child[1]);
	}

	else if(strcmp(root->child[1]->node_name,"FunDec")==0)
	{		
		printf("func:extdef()---->goto func process\n");
		FunList* funlist = (FunList*)malloc(sizeof(FunList));
		funlist->re_type = type;	
		parse_fundec(funlist,root->child[1]);
	}

	else
	{
		printf("SEMI\n");
	}
	
}


/****************************************************************************************************/
/**
 *处理变量序列
 */
void walk_extdeclist(Type* type,MultiTree* root)
{
	printf("Func:walk_extdeclist()--->now in declist\n");
	walk_vardec(type,root->child[0]);
	if(root->child[1]!=NULL){	//ExtDecList --> VarDec COMMA ExtDecList
		printf("Func:walk_extdeclist()---multi decs\n");
		walk_extdeclist(type,root->child[2]);
	}
}

/**
 *处理单个变量
 **/
void walk_vardec(Type* type,MultiTree* root)
{
	if(strcmp(root->child[0]->node_name,"ID")==0){
		HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		hash_node->list_type = 0;
		hash_node->var.name = strdup(root->child[0]->val.id);
		hash_node->var.type = type;
		hash_node->next = NULL;
		if(add_hash(hash_table,hash_node)==-1){  //插入哈希表失败
			print_err(3,root->child[0]->lineno,hash_node->var.name);			
		}
	}
	else{			//varDec --> varDec [ INT ]
		if(root->child[2]->type != 1){   // 下标不是int
			print_err(12,root->child[0]->lineno,NULL);
		}
		else{
			Type* embed_type = (Type*)malloc(sizeof(Type));
			embed_type->kind = ARRAY;
			embed_type->u.array.size = root->child[2]->val.intNum;
			embed_type->u.array.elem = type;
			walk_vardec(embed_type,root->child[0]);
		}
	}
}

/****************************************************************************************************/


/**
 *处理局部变量
 */
void parse_def(Type* type,MultiTree* root)
{
	MultiTree* spe_node = root->child[0];

	if(strcmp(spe_node->child[0]->node_name,"TYPE")==0){		
		type->kind = BASIC;
		if(strcmp(spe_node->child[0]->val.id,"int")==0){
			type->u.basic = 0;	
		}else if(strcmp(spe_node->child[0]->val.id,"float")==0){
			type->u.basic = 1;
		}else{
			printf("unsport!!\n");
		}
	}
	else{	//结构体类型
		//TODO
	}

	walk_declist(type,root->child[1]);
}


/**
 *递归处理DecList
 */
void walk_declist(Type* type,MultiTree* root)
{
	walk_dec(type,root->child[0]);
	if(root->child[1]!=NULL)			//DecList-->Dec,DecList
	{
		printf("Func:walk_declist--->multi dec\n");
		walk_declist(type,root->child[2]);
	}
}


/**
 * 处理Dec
 * */
void walk_dec(Type* type,MultiTree* root)
{
	if(root->child[1]==NULL)   //定义时没赋值
	{
		printf("Func:walk--->now deal exp:int i;");
		walk_vardec(type,root->child[0]);
	}
	else			//定义时赋值
	{
		//TODO
	}
}

/****************************************************************************************************/

void parse_fundec(FunList* funlist,MultiTree* root)
{	
	funlist->name = strdup(root->child[0]->val.id);    //函数名
	if(root->child[3]==NULL)			//无参函数
	{
		funlist->num_arc= 0;
		funlist->arc_type = NULL;

		HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		hash_node->list_type = 1;		//节点类型:函数
		hash_node->func = *funlist;
		free(funlist);					/***TODO:test****/
		hash_node->next = NULL;
		printf("func:funcdec()--->name:%s\n",hash_node->func.name);
		add_hash(hash_table,hash_node);		//加入符号表
	}
	else					//有参数
	{
		funlist->num_arc = count_arc(root->child[2]);		//参数个数
		printf("参数个数 %d\n",funlist->num_arc);
		funlist->arc_type = (Type*)malloc(funlist->num_arc);
		walk_varlist(funlist,root->child[2]);
	}

}

void walk_varlist(FunList* funlist,MultiTree* root)		//TODO:test-同时两个函数
{													
	static int arc_num = -1;
	arc_num++;

	if(root->child[1]==NULL){
		walk_param(funlist,arc_num,root->child[0]);		

		//HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		HashList hash_node;
		hash_node.list_type = 1;		//节点类型:函数
		hash_node.func = *funlist;
		free(funlist);					/***TODO:test****/
		hash_node.next = NULL;		
		printf("func:varlist()--->name:%s\n",hash_node.func.name);
		int i;
		for(i = 0;i< hash_node.func.num_arc;i++)
		{
			printf("arc%d type: %d\n",i,hash_node.func.arc_type[i].kind);
		}
		add_hash(hash_table,&hash_node);		//加入符号表
	}
	else
	{
		walk_param(funlist,arc_num,root->child[0]);	
		walk_varlist(funlist,root->child[2]);
	}
}


void walk_param(FunList* funlist,int arc_num,MultiTree* root)
{
	printf("num of arc is %d\n",arc_num);
	MultiTree* spe_node = root->child[0];
	if(strcmp(spe_node->child[0]->node_name,"TYPE")==0)
	{		
		funlist->arc_type[arc_num].kind = BASIC;
		if(strcmp(spe_node->child[0]->val.id,"int")==0)
		{
			funlist->arc_type[arc_num].u.basic = 0;	
		}
		else if(strcmp(spe_node->child[0]->val.id,"float")==0)
		{
			funlist->arc_type[arc_num].u.basic = 1;
		}
		else
		{
			printf("unsupport!!\n");
		}
	}
	else{	//结构体类型
		//TODO
	}
	walk_funcvar(&(funlist->arc_type[arc_num]),root->child[1]);
}


void walk_funcvar(Type* type,MultiTree* root)
{
	if(strcmp(root->child[0]->node_name,"ID") !=0){	//是ID的话类型不需在改变
		if(root->child[2]->type != 1){   // 下标不是int
			print_err(12,root->child[0]->lineno,NULL);   //12号错误
		}
		else{
			Type* embed_type = (Type*)malloc(sizeof(Type));
			memcpy(embed_type,type,sizeof(Type));
			printf("embed type is %d\n",embed_type->kind);

			type->kind = ARRAY;
			type->u.array.size = root->child[0]->val.intNum;
			type->u.array.elem = embed_type;
			walk_funcvar(type,root->child[0]);
		}
	}
}

int count_arc(MultiTree* root)
{
	static int i = 0;
	i++;
	if(root->child[1]==NULL){
		printf("ret\n");
		return i;
	}
	else{
		count_arc(root->child[2]);
	}
}


/*************************************处理函数体*****************************************************/
