#ifndef _SEM_H
#define _SEM_H

#include <stdlib.h>
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

/**
 * 结构体类型*/
struct FieldList_{
	char* name;		//域的名字
	Type* type;		//域的类型
	struct FieldList_* tail;	//下一个域
};

/**
 * 变量类型
 * */
struct Type_{
	enum {BASIC,ARRAY,STRUCTURE} kind;
	union{
		int basic;			//基本类型
		struct {Type* elem;int size;}array; //数组类型
		FieldList* structure;	//结构体类型
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
	int num_par;		//参数个数
	Type* arc_type;			//参数类型
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


//遍历语法树进行语义分析
//void sem_analy(MultiTree*, HashList*);

//向哈系表添加元素
int add_hash(HashList*,HashList*);	

//获取变量的类型
VarList* get_varType(HashList*,char *);		

//获取函数的返回值类型以及参数类型
FunList* get_funType(HashList*,char*);  


//比较hash结构体积
int hash_cmp(HashList*,HashList*);

//哈希函数
unsigned int hash_func(char* name);


//打印错误信息
void print_err(int,char*);

#endif
