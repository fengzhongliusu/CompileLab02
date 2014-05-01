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


void sem_analy(MultiTree* root)
{
	int i;
	if(root != NULL){
		if(strcmp(root->node_name,"ExtDef")==0){		//全局变量,函数,结构体积定义
			Type* type = &type_heap[type_heap_no++];
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
		printf("func:extdef()---->goto struct process\n");
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
			add_hash(hash_table, hash_node);


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
		printf("func:extdef()---->goto func process\n");
		FunList* funlist = (FunList*)malloc(sizeof(FunList));
		funlist->re_type = type;	
		parse_fundec(funlist,root->child[1]);
		parse_compst(root,root->child[2]);
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
 *处理单个变量,处理VarDec
 **/
void walk_vardec(Type* type,MultiTree* root)
{
	if(strcmp(root->child[0]->node_name,"ID")==0){
		//HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		HashList *hash_node = &hash_heap[hash_heap_no++];
		hash_node->list_type = 0;
		hash_node->var.name = root->child[0]->val.id;
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
		print_err(15, root->lineno, "assignment in struct");	//TODO struct赋值
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
		printf("add name=%s, type=%d to struct %s\n", id, ptr->type->kind, type->u.structure->name);
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
		printf("add name=%s, kind=%d, ", id, ptr->tail->type->kind);
		if(ptr->tail->type->kind == 0)
			printf("basic=%d ", ptr->tail->type->u.basic);
		else if(ptr->tail->type->kind == 1)
			printf("size=%d ", ptr->tail->type->u.array.size);
		else
			printf("name=%s ", ptr->tail->type->u.structure->name);
		printf("to struct %s\n", type->u.structure->name);
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
		printf("Func:walk_dec--->now deal exp:int i;");
		walk_vardec(type,root->child[0]);
	}
	else			//定义时赋值
	{
		walk_vardec(type, root->child[0]);
		//TODO 获得root->child[2]的类型并检查	
	}
}

/****************************************************************************************************/

void parse_fundec(FunList* funlist,MultiTree* root)
{	
	funlist->name = strdup(root->child[0]->val.id);    //函数名
	nearest_func = strdup(funlist->name);			   //最近的函数名


	if(root->child[3]==NULL)			//无参函数
	{
		funlist->num_arc= 0;
		funlist->arc_type = NULL;

		// HashList* hash_node = (HashList*)malloc(sizeof(HashList));
		HashList *hash_node = &hash_heap[hash_heap_no++];		
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
			Type* embed_type = &type_heap[type_heap_no++];
			memcpy(embed_type,type,sizeof(Type));
			printf("embed type is %d\n",embed_type->kind);

			type->kind = ARRAY;
			type->u.array.size = root->child[2]->val.intNum;
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
void walk_deflist(Type* type, MultiTree* root)
{	
	assert(root != NULL);
	assert(get_childnum(root) >= 1);
	parse_def(type, root->child[0]);

	if(get_childnum(root) > 1)
		walk_deflist(type, root->child[1]);
}

void walk_stmtlist(Type* type, MultiTree* root)
{
	assert(root != NULL);
	walk_stmt(type, root->child[0]);
	if(get_childnum(root) > 1)
		walk_stmtlist(type, root);
}

void walk_stmt(Type* type, MultiTree* root)
{
	assert(root != NULL);
	assert(get_childnum(root) >= 1);

	if(memcmp(root->child[0]->node_name, "Exp", 3) == 0)
		parse_exp(type, root->child[0]);
	else if(memcmp(root->child[0]->node_name, "CompSt", 6) == 0)
		parse_compst(root, root->child[0]);
	else if(memcmp(root->child[0]->node_name, "RETURN", 6) == 0)
	{
		parse_exp(type, root->child[1]);
		FunList *func = get_funType(hash_table, nearest_func);
		if(typecmp(type, func->re_type) != 0)	//和函数返回值类型不同
		{
			print_err(8, root->child[1]->lineno, NULL);		
		}
		return_flag = 1;
		
	}

	else if(memcmp(root->child[0]->node_name, "IF", 2) == 0)
	{
		parse_exp(type, root->child[2]);
		if(type->kind == BASIC && type->u.basic == 1);
		else 
			print_err(16, root->child[2]->lineno, "if");	// exp是int型值
		walk_stmt(type, root->child[4]);

		if(get_childnum(root) > 5)
			walk_stmt(type, root);
	}

	else if(memcmp(root->child[0]->node_name, "while", 5) == 0)
	{
		parse_exp(type, root->child[2]);
		if(type->kind == BASIC && type->u.basic == 1);
		else
			print_err(16, root->child[2]->lineno, "while");		
		walk_stmt(type, root->child[4]);
	}
}

void parse_compst(MultiTree* parent, MultiTree* root)
{
	assert(root != NULL);
	assert(get_childnum(root) >= 2);

	Type* type = &type_heap[type_heap_no++];
	
	if(get_childnum(root) == 3)
	{
		if(memcmp(root->child[1]->node_name, "DefList", 7) == 0)
			walk_deflist(type, root->child[1]);
		else if(memcmp(root->child[1]->node_name, "StmtList", 8) == 0)
			walk_stmtlist(type, root->child[1]);
	}

	else if(get_childnum(root) == 4)
	{
		walk_deflist(type, root->child[1]);
		walk_stmtlist(type, root->child[2]);
	}

	if(memcmp(parent->node_name, "Stmt", 4) != 0)
	{
		if(return_flag == 0)
			print_err(17, root->child[3]->lineno, nearest_func);		// 没有返回值
		return_flag = 0;
	}

}	

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
				VarList* var = get_varType(hash_table,root->child[0]->val.id);
				memcpy(type,var->type,sizeof(Type));
			}
		}
		else
		{
			if(strcmp(root->child[0]->node_name,"LP")==0){	  //Exp--->(Exp)
				parse_exp(type,root->child[1]);
			}
			else if(strcmp(root->child[0]->node_name,"Exp")==0){ //exp-->exp[exp]
				parse_exp(type,root->child[0]);
				Type* sign_type = &type_heap[type_heap_no++];
				parse_exp(sign_type,root->child[2]);
				if(sign_type->kind != BASIC || sign_type->u.basic != 0)   //数组下表不是int
					print_err(12,root->child[0]->lineno,NULL);	//
			}
			else
			{
				assert(strcmp(root->child[0]->node_name,"ID")==0);
				FunList* fun = get_funType(hash_table,root->child[0]->val.id);  //获取函数返回类型
				if(fun == NULL)   //未定义的函数						
				{
					print_err(2,root->child[0]->lineno,root->child[0]->val.id);   
				}
				else
				{
					memcpy(type,fun->re_type,sizeof(Type));				//函数返回类型
					if(strcmp(root->child[0]->node_name,"RP")!=0){	// exp-->ID(args)
						Type* type_arg = &type_heap[type_heap_no++];
						memcpy(type_arg,fun->arc_type,sizeof(Type));
						walk_arg(root->child[0]->val.id,fun->num_arc,type_arg,root->child[2]);  //处理参数是否正确
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
			}
		}

		else if(strcmp(root->op,"+")==0 ||strcmp(root->op,"=")==0 ||strcmp(root->op,"*")==0			||strcmp(root->op,"/")==0 ||strcmp(root->op,"&")==0 ||strcmp(root->op,"|")==0)			
		{
			parse_exp(l_type,root->child[0]);
			parse_exp(r_type,root->child[1]);				
			if(typecmp(l_type,r_type)!=0)  //两个操作数类型不匹配 
			{
				print_err(7,root->child[0]->lineno,NULL);
			}			
			else{
				memcpy(type,l_type,sizeof(Type));   
			}
		}

		else if(strcmp(root->op,"-")==0)
		{   //可能有一个或两个操作数字
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
				print_err(13,root->child[0]->lineno,root->op);	//.右边不是结构体
			}
			else{
				if(get_structvar(l_type,root->child[1]->val.id)==NULL){
					print_err(14,root->child[1]->lineno,root->child[1]->val.id);    //结构体积没有id
				}
				else{
					memcpy(type,get_structvar(l_type,root->child[1]->val.id),sizeof(Type));
				}
			}
		}

	}
}

//handle the args
void walk_arg(char* func_name,int arc_num,Type* arc_type,MultiTree* root)			//有错就返回
{
	static int num = -1;
	num++;
	if(num>arc_num){
		print_err(9,root->lineno,func_name);  //参数个数不正确
		return;
	}
	//判断第一个参数是否相同
	Type *type_1 = &type_heap[type_heap_no++];
	parse_exp(type_1,root->child[0]);
	if(typecmp(&arc_type[num],type_1)!=0){
		print_err(9,root->lineno,func_name);   // 函数参数错误
		return;
	}

	if(root->child[1]==NULL){
		return;
	}
	else{
		walk_arg(func_name,arc_num,arc_type,root->child[2]);
	}
}

Type* get_structvar(Type* type, char* id)
{
	FieldList* fieldptr = type->u.structure->structure;
	for(; fieldptr != NULL; fieldptr = fieldptr->tail)
	{
		if(memcmp(fieldptr->name, id, strlen(id)) == 0)
			return fieldptr->type;
	}
	return NULL;
}	
