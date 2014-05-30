#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"
#include <string.h>
extern int error;

MultiTree *newMultiTree(char *op, char *node_name, int type, union value* val, int lineno, int num, ...)
{
	int i;
	int count =0;
	va_list al;
	MultiTree* root_node;

	root_node = (MultiTree*)malloc(sizeof(MultiTree));
	root_node->child = (MultiTree**)malloc((num+1)*sizeof(MultiTree*));
	
	root_node->op = op;
	root_node->type = type;
	root_node->node_name = strdup(node_name);
	root_node->lineno = lineno;

	switch(type) {
		case 1:
			root_node->val.intNum = val->intNum;
			break;
		case 2:
			root_node->val.floatNum = val->floatNum;
			break;
		case 3:
			if(val != NULL)
				root_node->val.id = strdup(val->id);
			else
				root_node->val.id = NULL;
			break;
	}

	if(num == 0){				//终结符
		free(root_node->child);
		root_node->child = NULL;
	}
	else{						//非终结符
		va_start(al,num);
		MultiTree* node = NULL;
		for(i=0;i<num;i++){
			node = va_arg(al,MultiTree*);
			if(node == NULL){
				continue;
			}
			assert(node->node_name != NULL);
			root_node->child[count] = node;
			count++;
		}
		root_node->child[count] = NULL;			//遍历子节点时遇到NULL停止
		va_end(al);	
	}	
	return root_node;
}

void walk_tree(MultiTree* root)
{
	int i;
	static int k  = 1;
	printf("%*c",k,' ');
	assert(root->node_name != NULL);
	if(root!=NULL){
		printf("%s",root->node_name);
	}
	if(root->child == NULL){
		switch(root->type){
			case 1: printf(" : %d",root->val.intNum); break;
			case 2: printf(" : %f",root->val.floatNum); break;
			case 3: printf(" : %s",root->val.id); break;
		}
		printf("\n");
	}
	else{
		printf(" (%d)\n",root->lineno);
		k += 2;
		if(root->op == NULL){			
			for(i=0;root->child[i] != NULL;i++){
				walk_tree(root->child[i]);
			}
		}
		else{
			if(get_childnum(root)==1){
				printf("%*c",k,' ');
				printf("%s\n",root->op);
				walk_tree(root->child[0]);
			}
			else{	//double op
				walk_tree(root->child[0]);
				printf("%*c",k,' ');
				printf("%s\n",root->op);
				walk_tree(root->child[1]);
			}
		}		
		k -= 2;

	}	
}

int get_childnum(MultiTree* node)
{	
	if(node == NULL)
		return 0;
	int i;
	for(i=0;node->child[i]!=NULL;i++);
	return i;
}

A_stack create_stack()
{
	A_stack stack;
	stack = (A_stack)malloc(sizeof(*stack));
	stack->bottom = (int*)malloc(100 * sizeof(int));
	stack->top = stack->bottom - 1;
	stack->volume = 100;
	return stack;
}

int* pop(A_stack stack)
{
	if(stack->top == stack->bottom - 1)
		return NULL;
	else
	{
		stack->top--; return stack->top + 1;
	}
}

int push(A_stack stack, int a)
{
	if(stack->top == stack->bottom + stack->volume)
		return 0;
	else
	{
		stack->top++; *(stack->top) = a; return 1;
	}
}
