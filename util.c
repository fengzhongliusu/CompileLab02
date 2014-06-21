#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"
#include "semantic/sem.h"
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

void reg_dec(FILE* fp,A_node node1,A_node node2)
{
	if(node1->kind != A_id){
		printf("node is wrong....\n");
		exit(1);
	}
}

void reg_exp(FILE* fp,A_node node)
{
	int i;
	A_node left_op,right_op,r_op1,r_op2;
	if(node->kind != A_exp){
		printf("reg_exp:参数错误..\n");
		exit(1);
	}

	if(node->u.non_leaf.child_num != 2){
		printf("操作数个数错误..\n");
		exit(1);
	}

	left_op = node->u.non_leaf.child[0]->u.non_leaf.child[0];
	if(left_op->kind != A_id){
		printf("左操作数必须是id类型..\n");
		exit(1);
	}
	VarList *var_left = get_varType(left_op->u.id_leaf.id);
	VarList *var_right1, *var_right2;

	right_op = node->u.non_leaf.child[1];
	if(right_op->kind == A_exp){	  //非叶子节点

		if(right_op->u.non_leaf.child_num == 1){
			r_op1 = right_op->u.non_leaf.child[0];
			if(r_op1->kind == A_id) {
				var_right1 = get_varType(r_op1->u.id_leaf.id);
				fprintf(fp," lw $t1,%d($sp)\n",var_right1->sp);
				fprintf(fp, " move $t0, $t1\n sw $t0, %d($sp)\n",var_left->sp);
			}
			else if(r_op1->kind == A_intNum)
				fprintf(fp, " li $t0, %d\n sw $t0, %d($sp)\n", r_op1->u.int_leaf.value, var_left->sp);
		}
		else if(right_op->u.non_leaf.child_num == 2){
			r_op1 = right_op->u.non_leaf.child[0]->u.non_leaf.child[0];
			r_op2 = right_op->u.non_leaf.child[1]->u.non_leaf.child[0];
			if(strcmp(right_op->u.non_leaf.op,"+")==0){			
				if(r_op1->kind == A_id){
					if(r_op2->kind == A_id){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," lw $t2, %d($sp)\n",var_right2->sp);
						fprintf(fp," add t0, t1, t2\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," addi $t0, $t1, %d\n",r_op2->u.int_leaf.value);
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_intNum){
					if(r_op2->kind == A_id){
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right2->sp);
						fprintf(fp," addi t0, t1, %d\n",r_op1->u.int_leaf.value);
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
						fprintf(fp," addi t0, %d, %d\n",r_op1->u.int_leaf.value,r_op2->u.int_leaf.value);
						fprintf(fp," sw $t0 %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_floatNum){
					printf("float num\n");
					exit(1);
				}
			}

			else if(strcmp(right_op->u.non_leaf.op,"-")==0){			
				if(r_op1->kind == A_id){
					if(r_op2->kind == A_id){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," lw $t2, %d($sp)\n",var_right2->sp);
						fprintf(fp," sub $t0, $t1, $t2\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," subi $t0, $t1, %d\n",r_op2->u.int_leaf.value);
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_intNum){
					if(r_op2->kind == A_id){
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right2->sp);
						fprintf(fp," subi t0, t1, %d\n",r_op1->u.int_leaf.value);
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
						fprintf(fp," subi t0, %d, %d\n",r_op1->u.int_leaf.value,r_op2->u.int_leaf.value);
						fprintf(fp," sw $t0 %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_floatNum){
					printf("float num\n");
					exit(1);
				}
			}
			else if(strcmp(right_op->u.non_leaf.op,"*")==0){			
				if(r_op1->kind == A_id){
					if(r_op2->kind == A_id){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," lw $t2, %d($sp)\n",var_right2->sp);
						fprintf(fp," mul t0, t1, t2\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," li $t2,%d\n",r_op2->u.int_leaf.value);
						fprintf(fp," mul $t0, $t1, $t2\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_intNum){
					if(r_op2->kind == A_id){
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right2->sp);
						fprintf(fp," li $t2, %d\n",r_op1->u.int_leaf.value);
						fprintf(fp," mul $t0, $t1, $t2\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
						fprintf(fp," li $t1, %d\n",r_op1->u.int_leaf.value);
						fprintf(fp," li $t2, %d\n",r_op2->u.int_leaf.value);
						fprintf(fp," mul $t0, $t1, $t2\n");
						fprintf(fp," sw $t0 %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_floatNum){
					printf("float num\n");
					exit(1);
				}
			}

			else if(strcmp(right_op->u.non_leaf.op,"/")==0){			
				if(r_op1->kind == A_id){
					if(r_op2->kind == A_id){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," lw $t2, %d($sp)\n",var_right2->sp);
						fprintf(fp," div $t1, $t2\n");						
						fprintf(fp," mflo $t0\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
			var_right1 = get_varType(r_op1->u.id_leaf.id);
						fprintf(fp," lw $t1, %d($sp)\n",var_right1->sp);
						fprintf(fp," li $t2, %d\n",r_op2->u.int_leaf.value);
						fprintf(fp," div $t1, $t2\n");
						fprintf(fp," mflo $t0\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_intNum){
					if(r_op2->kind == A_id){
			var_right2 = get_varType(r_op2->u.id_leaf.id);
						fprintf(fp," lw $t2, $%d($sp)\n",var_right2->sp);
						fprintf(fp," li $t1, %d\n",r_op1->u.int_leaf.value);
						fprintf(fp," div $t1, $t2\n");
						fprintf(fp," mflo $t0\n");
						fprintf(fp," sw $t0, %d($sp)\n",var_left->sp);
					}
					else if(r_op2->kind == A_intNum){
						fprintf(fp," li $t1, %d\n",r_op1->u.int_leaf.value);
						fprintf(fp," li $t2, %d\n",r_op2->u.int_leaf.value);
						fprintf(fp," div $t1, $t2\n");
						fprintf(fp," mflo $t0\n");
						fprintf(fp," sw $t0 %d($sp)\n",var_left->sp);
					}
					else{
						printf("float Num\n");
						exit(1);
					}
				}

				else if(r_op1->kind == A_floatNum){
					printf("float num\n");
					exit(1);
				}
			}

			else{
				printf("不支持的操作符..\n");
				exit(1);
			}
		}
		else{
			printf("not supported yet..\n");
			exit(1);
		}
	}
}

