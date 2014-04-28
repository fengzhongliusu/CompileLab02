#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"
#include <string.h>
extern int error;

MultiTree *newMultiTree(char *op, char *node_name, int type, union value* val, int lineno, int num, ...)
{
	/*if(error > 0){
		printf("%s error arise!!,return\n",node_name);
		return NULL;
	}*/
	int i;
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
			//assert(node->node_name != NULL);
			//printf("%s\n",node->node_name);
			root_node->child[i] = node;
		}
		root_node->child[num] = NULL;			//遍历子节点时遇到NULL停止
		va_end(al);	
	}	
	return root_node;
}

void walk_tree(MultiTree* root)
{
	int i;
	static int k  = 1;
	printf("%*c",k,' ');
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
	int i;
	for(i=0;node->child[i]!=NULL;i++);
	return i;
}
/*int main(){
	int i;
	MultiTree* node[9];	

	for(i=3;i<9;i++){
		node[i] = newMultiTree(i,"child node",0);	
	}
	
	node[2] = newMultiTree(2,"node 2",3,node[3],node[4],node[5]);
	node[1] = newMultiTree(1,"node 1",3,node[6],node[7],node[8]);
	node[0] = newMultiTree(0,"node 0",2,node[1],node[2]);

	walk_tree(node[0]);
	
	return 0;
}*/
