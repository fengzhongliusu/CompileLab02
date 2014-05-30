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

static char *nearest_func;
int return_flag = 0;
int redef_sign = 0;


void sem_analy(MultiTree* root)
{
	int i;
	if(root != NULL){
		if(strcmp(root->node_name,"ExtDef")==0){		//全局变量,函数,结构体积定义
			Type* type = &type_heap[type_heap_no++];
			parse_extdef(type,root);
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
		structtype *structhead = (structtype *)malloc(sizeof(structtype));
		type->u.structure = structhead;
		memset(structhead, 0, sizeof(structtype));

		type->kind = STRUCTURE;
		if(get_childnum(spe_node->child[0]) <= 1)
			type->u.structure->name = NULL;
		else if(memcmp(spe_node->child[0]->child[1]->node_name, "OptTag", 6) == 0) {
			type->u.structure->name = strdup(spe_node->child[0]->child[1]->
					child[0]->val.id);
			walk_structdeflist(type, spe_node->child[0]->child[3]);

			HashList *hash_node = &hash_heap[hash_heap_no++];
			hash_node->var.type = &type_heap[type_heap_no++];

			hash_node->list_type = 0;	//变量，实际上是结构体名字
			hash_node->var.name = NULL;
			memcpy(hash_node->var.type, type, sizeof(Type));
			hash_node->next = NULL;
			hash_node->block_next = NULL;

			if(cmp_struct(symbol_head[block_no].block_next,hash_node) == -1){
				print_err(18, spe_node->child[0]->lineno, hash_node->var.type->u.structure->name);
			}
			else{
				//add to the block var list
				//printf("%d %s\n",block_no,hash_node->var.type->u.structure->name);
				hash_node->block_next = symbol_head[block_no].block_next;
				symbol_head[block_no].block_next = hash_node;

				//add to the structure_table
				add_hash(hash_node);
			}


		}
		else if(memcmp(spe_node->child[0]->child[1]->node_name, "Tag", 3) == 0) {
			type->u.structure->name = strdup(spe_node->child[0]->child[1]->
					child[0]->val.id);
		}

	}

	if(strcmp(root->child[1]->node_name,"ExtDecList")==0)
	{
		walk_extdeclist(type,root->child[1]);
	}

	else if(strcmp(root->child[1]->node_name,"FunDec")==0)
	{	
		block_no++;
		FunList* funlist = (FunList*)malloc(sizeof(FunList));
		funlist->re_type = &type_heap[type_heap_no++];
		memcpy(funlist->re_type, type, sizeof(Type));
		parse_fundec(funlist,root->child[1]);
		if(redef_sign == 0){
			parse_compst(root,root->child[2],1);
		}
		redef_sign = 0;
	}

	else{}

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
 *处理单个变量,处理VarDec
 **/
void walk_vardec(Type* type,MultiTree* root)
{
	if(strcmp(root->child[0]->node_name,"ID")==0){
		//HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		HashList *hash_node = &hash_heap[hash_heap_no++];
		hash_node->list_type = 0;
		hash_node->var.name = root->child[0]->val.id;
		hash_node->var.type = &type_heap[type_heap_no++];
		memcpy(hash_node->var.type, type, sizeof(Type));
		hash_node->next = NULL;
		hash_node->block_next = NULL;

		if(cmp_local(symbol_head[block_no].block_next,hash_node) == -1)
			print_err(3,root->child[0]->lineno,hash_node->var.name);			
		else{
			//add to the block list
			//printf("%d %s\n",block_no,hash_node->var.name);			
			hash_node->block_next = symbol_head[block_no].block_next;
			symbol_head[block_no].block_next = hash_node;

			//add to the hash_table
			add_hash(hash_node);
		}
	}
	else{			//varDec --> varDec [ INT ]
		if(root->child[2]->type != 1){   // 下标不是int
			print_err(12,root->child[0]->lineno,NULL);
		}
		else{
			Type* embed_type = &type_heap[type_heap_no++];
			embed_type->kind = ARRAY;
			embed_type->u.array.size = root->child[2]->val.intNum;
			embed_type->u.array.elem = type;
			walk_vardec(embed_type,root->child[0]);
		}
	}
}

/**
 * 处理结构体定义时的DefList
 **/
void walk_structdeflist(Type* type, MultiTree* root)
{
	walk_structdef(type, root->child[0]);
	if(get_childnum(root) == 2)
		walk_structdeflist(type, root->child[1]);


}

void walk_structdef(Type* type, MultiTree* root)
{
	Type* fieldtype = &type_heap[type_heap_no++];
	MultiTree* spe_node = root->child[0];

	if(strcmp(spe_node->child[0]->node_name,"TYPE")==0){		
		fieldtype->kind = BASIC;
		if(strcmp(spe_node->child[0]->val.id,"int")==0){
			fieldtype->u.basic = 0;	
		}else if(strcmp(spe_node->child[0]->val.id,"float")==0){
			fieldtype->u.basic = 1;
		}else{
			printf("unsport!!\n");
		}
	}
	else{	//结构体类型
		//TODO
		type->kind = STRUCTURE;

		structtype *structhead = (structtype *)malloc(sizeof(structtype));
		fieldtype->u.structure = structhead;
		memset(structhead, 0, sizeof(structtype));

		if(get_childnum(spe_node->child[0]) <= 1)
			fieldtype->u.structure->name = NULL;
		else if(memcmp(spe_node->child[0]->child[1]->node_name, "OptTag", 6) == 0) {
			fieldtype->u.structure->name = strdup(spe_node->child[0]->child[1]->
					child[0]->val.id);
		}
		else if(memcmp(spe_node->child[0]->child[1]->node_name, "Tag", 3) == 0) {
			fieldtype->u.structure->name = strdup(spe_node->child[0]->child[1]->
					child[0]->val.id);
		}
	}
	walk_structdeclist(type, fieldtype, root->child[1]);
}

void walk_structdeclist(Type* type, Type* fieldtype, MultiTree* root)
{
	walk_structdec(type, fieldtype, root->child[0]);
	if(get_childnum(root) > 1)
		walk_structdeclist(type, fieldtype, root->child[2]);
}

void walk_structdec(Type* type, Type* fieldtype, MultiTree* root)
{
	walk_structvar(type, fieldtype, root->child[0]);
	if(get_childnum(root) > 1)
		print_err(19, root->lineno, NULL);	//TODO struct赋值
}

void walk_structvar(Type* type, Type* fieldtype, MultiTree* root)
{
	if(memcmp(root->child[0]->node_name, "ID", 2) == 0)
	{
		structtype_add(type, fieldtype, root->child[0]);
	}
	else
	{
		if(root->child[2]->type != 1)
			print_err(12,root->child[0]->lineno,NULL);
		else {
			Type* innertype = &type_heap[type_heap_no++];
			innertype->kind = ARRAY;
			innertype->u.array.size = root->child[2]->val.intNum;
			innertype->u.array.elem = fieldtype;
			walk_structvar(type, innertype, root->child[0]);
		}
	}
}

void structtype_add(Type* type, Type* fieldtype, MultiTree *root)
{
	FieldList* ptr;
	ptr = type->u.structure->structure;
	char *id = root->val.id;
	if(ptr == NULL)
	{
		ptr = (FieldList*)malloc(sizeof(FieldList));
		ptr->name = strdup(id);
		ptr->type = &type_heap[type_heap_no++];
		memcpy(ptr->type, fieldtype, sizeof(Type));
		ptr->tail = NULL;
		type->u.structure->structure = ptr;
		//printf("add name=%s, type=%d to struct %s\n", id, ptr->type->kind, type->u.structure->name);
	}
	else
	{
		for(; ptr->tail != NULL; ptr = ptr->tail)
		{
			//TODO 检查重名
			if(memcmp(ptr->name, id, strlen(id) + 1) == 0) {	//长度一致，最后一个字符都是\0
				print_err(15, root->lineno , id); 
			}
		}
		ptr->tail = (FieldList*)malloc(sizeof(FieldList));
		ptr->tail->name = strdup(id);
		ptr->tail->type = &type_heap[type_heap_no++];
		memcpy(ptr->tail->type, fieldtype, sizeof(Type));
		ptr->tail->tail = NULL;
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
		type->kind = STRUCTURE;

		structtype *structhead = (structtype *)malloc(sizeof(structtype));
		type->u.structure = structhead;
		memset(structhead, 0, sizeof(structtype));

		if(get_childnum(spe_node->child[0]) <= 1)
			type->u.structure->name = NULL;
		else if(memcmp(spe_node->child[0]->child[1]->node_name, "OptTag", 6) == 0) {
			type->u.structure->name = strdup(spe_node->child[0]->child[1]->
					child[0]->val.id);
		}
		else if(memcmp(spe_node->child[0]->child[1]->node_name, "Tag", 3) == 0) {
			type->u.structure->name = strdup(spe_node->child[0]->child[1]->
					child[0]->val.id);
		}
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
		//printf("Func:walk_declist--->multi dec\n");
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
		walk_vardec(type,root->child[0]);
	}
	else			//定义时赋值
	{
		walk_vardec(type, root->child[0]);
		// 获得root->child[1]的类型并检查	
		Type *tp = &type_heap[type_heap_no++];
		parse_exp(tp, root->child[1]);

		if(typecmp(type, tp) != 0)
			print_err(5, root->child[1]->lineno, NULL); 
	}
}

/*************************************************函数定义***************************************/

void parse_fundec(FunList* funlist,MultiTree* root)
{	
	funlist->name = root->child[0]->val.id;    //函数名
	nearest_func = funlist->name;			   //最近的函数名


	if(root->child[3]==NULL)			//无参函数
	{
		funlist->num_arc= 0;
		funlist->arc_type = NULL;

		HashList *hash_node = &hash_heap[hash_heap_no++];		
		hash_node->list_type = 1;		//节点类型:函数
		hash_node->func = *funlist;
		free(funlist);					/***TODO:test****/
		hash_node->next = NULL;
		hash_node->block_next = NULL;
		block_no--;
		if(cmp_local(symbol_head[block_no].block_next,hash_node)== -1){ //加入符号表,可能重定义
			print_err(4, root->child[0]->lineno, root->child[0]->val.id);
			redef_sign = 1;
		} else{
			//add to the block list 
			//printf("%d %s\n",block_no,root->child[0]->val.id);
			hash_node->block_next = symbol_head[block_no].block_next;
			symbol_head[block_no].block_next = hash_node;			

			//add to the hash table
			add_hash(hash_node);
			block_no++;
		}
	}
	else					//有参数
	{
		funlist->num_arc = count_arc(root->child[2], "new");		//参数个数
		funlist->arc_type = &type_heap[type_heap_no];
		type_heap_no += funlist->num_arc;
		walk_varlist(funlist,root->child[2]);
	}

}


/**
 *处理参数
 **/
void walk_varlist(FunList* funlist,MultiTree* root)		//TODO:test-同时两个函数
{													
	static int arc_num = -1;
	arc_num++;

	if(root->child[1]==NULL){
		walk_param(funlist,arc_num,root->child[0]);		

		//HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		HashList *hash_node = &hash_heap[hash_heap_no++];
		hash_node->list_type = 1;		//节点类型:函数
		memcpy(&hash_node->func, funlist, sizeof(FunList));
		free(funlist);					/***TODO:test****/
		hash_node->next = NULL;		
		hash_node->block_next = NULL;

		block_no--;
		if(cmp_local(symbol_head[block_no].block_next,hash_node)== -1){//加入符号表,可能重定义
			print_err(4, root->child[0]->lineno, hash_node->func.name);
			redef_sign = 1;
		} else {
			//add to the block list
			//printf("%d %s\n",block_no,hash_node->func.name);
			hash_node->block_next = symbol_head[block_no].block_next;
			symbol_head[block_no].block_next = hash_node;

			//addd to the hash table
			add_hash(hash_node);		
			block_no++;
		}

	}
	else
	{
		walk_param(funlist,arc_num,root->child[0]);	
		walk_varlist(funlist,root->child[2]);
	}
}

/*处理参数
 *主要是specifier部分
 */
void walk_param(FunList* funlist,int arc_num,MultiTree* root)
{
	//printf("num of arc is %d\n",arc_num);
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
		funlist->arc_type[arc_num].kind = STRUCTURE;
		VarList *var;
		if((var = get_struct_var(root->child[0]->child[0]->child[1]->child[0]->val.id)) == NULL){	
			printf("%s----\n",root->child[0]->child[0]->child[1]->child[0]->val.id);		
			print_err(1, spe_node->child[0]->lineno, root->child[0]->child[0]->child[1]->child[0]->val.id);
		}
		else
		{
			memcpy(&funlist->arc_type[arc_num], var->type, sizeof(Type));
		}
	}
	walk_funcvar(&(funlist->arc_type[arc_num]),root->child[1]);
}


/**
 *处理参数类型 vardec部分, 可能为ID,可能为数组
 */
void walk_funcvar(Type* type,MultiTree* root)
{
	if(strcmp(root->child[0]->node_name,"ID") !=0){	//是ID的话类型不需在改变
		if(root->child[2]->type != 1){   // 下标不是int
			print_err(12,root->child[0]->lineno,NULL);   //12号错误
		}
		else{
			Type* embed_type = &type_heap[type_heap_no++];
			memcpy(embed_type,type,sizeof(Type));

			type->kind = ARRAY;
			type->u.array.size = root->child[2]->val.intNum;
			type->u.array.elem = embed_type;
			walk_funcvar(type,root->child[0]);
		}
	} else{
		HashList *hash_node = &hash_heap[hash_heap_no++];
		hash_node->list_type = 0;
		hash_node->var.name = root->child[0]->val.id;
		hash_node->var.type = &type_heap[type_heap_no++];
		memcpy(hash_node->var.type, type, sizeof(Type));
		hash_node->next = NULL;
		hash_node->block_next = NULL;

		if(cmp_local(symbol_head[block_no].block_next,hash_node) == -1)
			print_err(3,root->child[0]->lineno,hash_node->var.name);			
		else{
			//add to the block list
			//printf("%d %s\n",block_no,hash_node->var.name);			
			hash_node->block_next = symbol_head[block_no].block_next;
			symbol_head[block_no].block_next = hash_node;

			//add to the hash_table
			add_hash(hash_node);
		}
	}
}

/**
 * 计算参数个数
 **/
int count_arc(MultiTree* root, char * sign)
{
	static int i = 0;
	if(sign != NULL)
		i = 0;
	i++;
	if(root->child[1]==NULL){
		return i;
	}
	else{
		count_arc(root->child[2], NULL);
	}
}


/*************************************处理函数体*****************************************************/
void walk_deflist(MultiTree* root)
{	
	assert(root != NULL);
	assert(get_childnum(root) >= 1);
	Type* type = (Type*)malloc(sizeof(Type));
	parse_def(type, root->child[0]);

	if(get_childnum(root) > 1)
		walk_deflist(root->child[1]);

	free(type);
}

void walk_stmtlist(MultiTree* root)
{
	assert(root != NULL);
	walk_stmt(root->child[0]);
	if(get_childnum(root) > 1)
		walk_stmtlist(root->child[1]);
}

/**
 *处理表达式
 */
void walk_stmt(MultiTree* root)
{
	assert(root != NULL);
	assert(get_childnum(root) >= 1);

	Type* type = (Type*)malloc(sizeof(Type));

	if(memcmp(root->child[0]->node_name, "Exp", 3) == 0)
		parse_exp(type, root->child[0]);
	else if(memcmp(root->child[0]->node_name, "CompSt", 6) == 0)
		parse_compst(root, root->child[0],0);
	else if(memcmp(root->child[0]->node_name, "RETURN", 6) == 0)
	{
		parse_exp(type, root->child[1]);
		FunList *func = get_funType(nearest_func);
		if(typecmp(type, func->re_type) != 0)	//和函数返回值类型不同
		{
			print_err(8, root->child[1]->lineno, NULL);		
		}
		return_flag = 1;

	}

	else if(memcmp(root->child[0]->node_name, "IF", 2) == 0)
	{
		parse_exp(type, root->child[2]);
		if(type->kind == BASIC && type->u.basic == 0);
		else 
			print_err(16, root->child[2]->lineno, "if");	// exp是int型值
		walk_stmt(root->child[4]);

		if(get_childnum(root) == 7)
			walk_stmt(root->child[6]);
	}

	else if(memcmp(root->child[0]->node_name, "WHILE", 5) == 0)
	{
		parse_exp(type, root->child[2]);
		if(type->kind == BASIC && type->u.basic == 0);
		else
			print_err(16, root->child[2]->lineno, "while");		
		walk_stmt(root->child[4]);
	}

	free(type);
}


/**
 *处理函数体
 */
void parse_compst(MultiTree* parent, MultiTree* root,int func_sign)
{
	if(func_sign ==  0) // not compst of func
		block_no++;
	assert(root != NULL);
	assert(get_childnum(root) >= 2);

	if(get_childnum(root) == 3)
	{
		if(memcmp(root->child[1]->node_name, "DefList", 7) == 0)
			walk_deflist(root->child[1]);
		else if(memcmp(root->child[1]->node_name, "StmtList", 8) == 0)
			walk_stmtlist(root->child[1]);
	}

	else if(get_childnum(root) == 4)
	{
		walk_deflist(root->child[1]);
		walk_stmtlist(root->child[2]);
	}

	if(memcmp(parent->node_name, "Stmt", 4) != 0)
	{
		if(return_flag == 0)
			print_err(17, root->child[1]->lineno, nearest_func);		// 没有返回值
		return_flag = 0;
	}

	memset(&symbol_head[block_no],0,sizeof(HashList));
	symbol_head[block_no].block_next = NULL;
	block_no--;
	//printf("back to symbol table%d \n",block_no);
}	


/**
 *处理表达式
 **/
void parse_exp(Type* type,MultiTree* root)
{
	//表达式类型返回方式:对type进行操作即可
	assert(root!=NULL);
	if(root->op == NULL)  //无操作符
	{	
		if(root->child[1]==NULL)	//int,id or float
		{  
			if(strcmp(root->child[0]->node_name,"INT")==0){	//INT
				type->kind = BASIC;
				type->u.basic = 0;
			}
			else if(strcmp(root->child[0]->node_name,"FLOAT")==0){	//FLOAT
				type->kind = BASIC;
				type->u.basic = 1;
			}
			else{	//ID				
				VarList* var = get_varType(root->child[0]->val.id);
				if(var == NULL) {
					/*type = NULL, 在这里无效
					*/
					print_err(1, root->child[0]->lineno, root->child[0]->val.id);
					type->kind = -1;
				}
				else {
					memcpy(type,var->type,sizeof(Type));
				}
			}
		}
		else
		{
			if(strcmp(root->child[0]->node_name,"LP")==0){	  //Exp--->(Exp)
				parse_exp(type,root->child[1]);
			}
			else if(strcmp(root->child[0]->node_name,"Exp")==0){ //exp-->exp[exp]
				parse_exp(type,root->child[0]);
				if(type->kind != ARRAY)
				{
					print_err(10, root->child[0]->lineno, NULL);
				}
				Type* sign_type = &type_heap[type_heap_no++];
				parse_exp(sign_type,root->child[2]);
				if(sign_type->kind != BASIC || sign_type->u.basic != 0)   //数组下表不是int
					print_err(12,root->child[0]->lineno,NULL);	//
			}
			else
			{
				assert(strcmp(root->child[0]->node_name,"ID")==0);
				FunList* fun = get_funType(root->child[0]->val.id);  //获取函数返回类型
				if(fun == NULL)   //未定义的函数						
				{
					if(get_varType(root->child[0]->val.id) != NULL)
						print_err(11, root->child[0]->lineno, root->child[0]->val.id);
					else	
						print_err(2,root->child[0]->lineno,root->child[0]->val.id);   
				}
				else
				{
					memcpy(type,fun->re_type,sizeof(Type));				//函数返回类型
					if(strcmp(root->child[0]->node_name,"RP")!=0){	// exp-->ID(args)
						Type* type_arg = &type_heap[type_heap_no];
						type_heap_no += fun->num_arc;						
						memcpy(type_arg,fun->arc_type,sizeof(Type)*fun->num_arc);
						//int k;
						//for(k=0;k<fun->num_arc;k++){
						//	memcpy(&type_arg[k],&fun->arc_type[k],sizeof(Type));
						//}						
						walk_arg(root->child[0]->val.id,fun->num_arc,type_arg,root->child[2], "new");  //处理参数是否正确
					}
				}

			}
		}	
	}
	else	//有操作符
	{	//比较右边产生内部变量类型是否匹配,并返回产生式类型
		//	Type* only_type = &type_heap[type_heap_no++]; 
		Type* l_type = &type_heap[type_heap_no++];
		Type* r_type = &type_heap[type_heap_no++];		

		if(strcmp(root->op,"!=")==0 ||strcmp(root->op,"==")==0 ||strcmp(root->op,">=")==0 ||
				strcmp(root->op,"<=")==0 ||strcmp(root->op,">")==0 ||strcmp(root->op,"<")==0)
		{
			type->kind = BASIC;
			type->u.basic = 0;
			parse_exp(l_type,root->child[0]);
			parse_exp(r_type,root->child[1]);
			if(typecmp(l_type,r_type)!=0)  //两个操作数类型不匹配 
			{
				print_err(7,root->child[0]->lineno,NULL);
				type->kind = -1;
			}
		}

		else if(strcmp(root->op,"+")==0 ||strcmp(root->op,"=")==0 ||strcmp(root->op,"*")==0			||strcmp(root->op,"/")==0 ||strcmp(root->op,"&")==0 ||strcmp(root->op,"|")==0)			
		{
			parse_exp(l_type,root->child[0]);
			parse_exp(r_type,root->child[1]);				
			if(memcmp(root->op, "=", 1) == 0)	//不是左值
			{
				if(ck_lvalue(root->child[0]) != 0)
					print_err(6, root->child[0]->lineno, NULL);
				type->kind = -1;
			}	
			if(typecmp(l_type,r_type)!=0)  //两个操作数类型不匹配 
			{
				print_err(7,root->child[0]->lineno,NULL);
				type->kind = -1;
			}			
			else{
				memcpy(type,l_type,sizeof(Type));   
			}
		}

		else if(strcmp(root->op,"-")==0)
		{   //可能有一个或两个操作数
			if(root->child[1]==NULL){
				parse_exp(type,root->child[0]);
			}
			else{
				assert(root->child[2]==NULL);
				parse_exp(l_type,root->child[0]);
				parse_exp(r_type,root->child[1]);
				if(typecmp(l_type,r_type)!=0)		
				{
					print_err(7,root->child[0]->lineno,NULL);// 类型不匹配
					type->kind = -1;
				}		
				else
				{
					memcpy(type,l_type,sizeof(Type));
				}
			}
		}

		else if(strcmp(root->op,"!")==0)
		{
			parse_exp(type,root->child[0]);				
		}

		else
		{
			assert(root->child[1]!=NULL);
			parse_exp(l_type,root->child[0]);
			if(l_type->kind != 2){
				print_err(13,root->child[0]->lineno,root->op);	//.左边不是结构体
				type->kind = -1;
			}
			else{
				if(get_structvar(l_type,root->child[1]->val.id)==NULL){
					print_err(14,root->child[1]->lineno,root->child[1]->val.id);    //结构体没有id
					type->kind = -1;
				}
				else{
					memcpy(type,get_structvar(l_type,root->child[1]->val.id),sizeof(Type));
				}
			}
		}

	}
}

//handle the args
void walk_arg(char* func_name,int arc_num,Type* arc_type,MultiTree* root, char * sign)			//有错就返回
{
	static int num = -1;
	if(sign != NULL)
		num = -1;
	num++;
	if(num>arc_num-1){
		print_err(21,root->lineno,func_name);  //参数个数不正确
		return;
	}
	//判断第一个参数是否相同
	Type *type_1 = &type_heap[type_heap_no++];
	parse_exp(type_1,root->child[0]);
	//printf("num is %d---------\n",num);
	if(typecmp(&arc_type[num],type_1)!=0){
		print_err(9,root->lineno,root->child[0]->child[0]->val.id);   // 函数参数错误
	}

	if(root->child[1]==NULL){
		if(num+1 != arc_num)
			print_err(20,root->lineno,func_name);  //参数个数不正确
		return;
	}
	else{
		walk_arg(func_name,arc_num,arc_type,root->child[2], NULL);
	}
}

Type* get_structvar(Type* type, char* id)
{
	//从符号表取结构体类型	
	VarList	*var_in_table = get_struct_var(type->u.structure->name);
	Type* type_in_var = var_in_table->type;

	FieldList* fieldptr = type_in_var->u.structure->structure;

	for(; fieldptr != NULL; fieldptr = fieldptr->tail)
	{
		if(memcmp(fieldptr->name, id, strlen(id)) == 0)
			return fieldptr->type;
	}
	return NULL;
}	

//检查是否为左值
int ck_lvalue(MultiTree *root)
{
	int chd_num = get_childnum(root);
	if(chd_num == 1)
	{
		if(memcmp(root->child[0]->node_name, "ID", 2) == 0)
			return 0;
		else
			return -1;
	}	
	else if(chd_num == 4 && memcmp(root->child[2]->node_name, "Exp", 3) == 0)
	{
		if((get_childnum(root->child[0]) == 1) && memcmp(
					root->child[0]->child[0]->node_name, "ID", 2) == 0)
			return 0;
		else
			return -1;
	}
	else if(chd_num == 2 && memcmp(root->op, ".", 1) == 0)
	{
		if((get_childnum(root->child[0]) == 1) && memcmp(
					root->child[0]->child[0]->node_name, "ID", 2) == 0)
			return 0;
		else
			return -1;
	}


	else 
		return -1;
}
