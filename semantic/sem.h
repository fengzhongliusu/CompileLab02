#ifndef _SEM_H
#define _SEM_H

#include <stdlib.h>
#include "../util.h"
/*
 * =====================================================================================
 *
 *       Filename:  sem.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2014 05:25:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chengshuo (cshuo), chengshuo357951@gmail.com
 *        Company:  Nanjing Unversity
 *
 * =====================================================================================
 */

typedef struct hash_list HashList;
typedef struct var_list VarList;
typedef struct fun_list FunList;
typedef struct Type_ Type;
typedef struct FieldList_ FieldList;

//符号表
#define HASH_SIZE 0x4000
#define MAX_NODE 100
#define MAX_TYPE 1024

/**
 * 结构体类型*/
struct FieldList_{
	char* name;		//域的名字
	Type* type;		//域的类型
	struct FieldList_* tail;	//下一个域
};

typedef struct {
    char* name;
	FieldList* structure;
} structtype;

/**
 * 变量类型
 * */
struct Type_{
	enum {BASIC,ARRAY,STRUCTURE} kind;
	union{
		int basic;			//基本类型 1:int; 2:float
		struct {Type* elem;int size;}array; //数组类型
		structtype* structure;	//结构体类型
	}u;

};


/**
 *符号表变量结构体
 */
struct var_list{
	char* name;
	Type* type;	
};

/**
 * 符号表函数结构体
 */
struct fun_list{
	char* name;
	int num_arc;		//参数个数
	Type* arc_type;		//参数类型
	Type* re_type;		//返回值类型
};


//open hash
struct hash_list{
	int list_type;
	union{
		VarList var;
		FunList func;
	};
	HashList* next;
};



//向哈系表添加元素
int add_hash(HashList*,HashList*);	

//获取变量的类型
VarList* get_varType(HashList*,char *);		

//获取函数的返回值类型以及参数类型
FunList* get_funType(HashList*,char*);  

//比较type是否相同
int typecmp(Type *, Type *);

//比较hash结构
int hash_cmp(HashList*,HashList*);

//哈希函数
unsigned int hash_func(char* name);

//打印错误信息
void print_err(int,int,char*);


/*****************/
/*语义分析模块***/
/****************/

//整体语义分析
void sem_analy(MultiTree* );

//全局变量，结构体，函数定义
void parse_extdef(Type* ,MultiTree* );

/***********全局变量的定义***************************/

void walk_extdeclist(Type* ,MultiTree* );  

void walk_vardec(Type* ,MultiTree* );

void hash_display(HashList* );

HashList hash_table[HASH_SIZE];

/***********局部变量的定义***************************/

void parse_def(Type*, MultiTree* );

void walk_declist(Type* ,MultiTree* );

void walk_dec(Type* ,MultiTree* );

void parse_exp(Type* , MultiTree* );

void walk_arg(char*,int,Type*,MultiTree*, char *);

void parse_compst(MultiTree* , MultiTree* );

void walk_deflist(MultiTree* );

void walk_stmtlist(MultiTree* );

void walk_stmt(MultiTree* );

int ck_lvalue(MultiTree *);


/********************函数的定义***********************/

void parse_fundec(FunList* ,MultiTree* );

void walk_varlist(FunList* ,MultiTree* );

void walk_param(FunList* ,int ,MultiTree* );

void walk_funcvar(Type* ,MultiTree* );

int count_arc(MultiTree *, char *);

/******************处理定义结构体********************/
void walk_structdeflist(Type *, MultiTree *);

void walk_structdef(Type *, MultiTree *);

void walk_structdeclist(Type *, Type *, MultiTree *);

void walk_structdec(Type *, Type *, MultiTree *);

void walk_structvar(Type *, Type *, MultiTree *);

void structtype_add(Type *, Type *, MultiTree *);

Type* get_structvar(Type*,char*);   //type of variable in a structure


int hash_heap_no;
HashList hash_heap[MAX_TYPE];		//用于申请hash节点空间的堆
int type_heap_no;
Type type_heap[MAX_TYPE];			//用于申请Type类型空间的堆

#endif
