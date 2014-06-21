#include "util.h"
#include "semantic/sem.h"

int file;
FILE* f;
int alloc_num = 1;

void M_init() {
	if((file = open("output.s", O_CREAT|O_EXCL|O_RDWR, 00777)) < 0)
	{
		if(errno == EEXIST)
		{
			unlink("output.s");
			file = open("output.s", O_CREAT|O_EXCL|O_RDWR, 00777);
		}
	}
	f = fdopen(file, "a");
}


int M_varNum(A_node compSt)
{
	A_node ptr_def, ptr_dec;
	int num = 0;
	if(compSt->u.non_leaf.child[0]->kind != A_def)
		return 0;
	else
	{
		ptr_def = compSt->u.non_leaf.child[0];
		while(ptr_def != NULL)
		{
			ptr_dec = ptr_def->u.non_leaf.child[1];
			while(ptr_dec != NULL)
			{
				num++;
				ptr_dec = ptr_dec->u.non_leaf.sibling;
			}
			ptr_def = ptr_def->u.non_leaf.sibling;
		}
		return num;
	}
}




	

void M_alloc(A_node root)
{
	if(root->kind == A_nonLeaf)
	{
		int child_num = root->u.non_leaf.child_num;
		int i = 0;
		for(; i < child_num; i++)
			M_alloc(root->u.non_leaf.child[i]);
	}

	else if(root->kind == A_defineFunction)
	{
		if(memcmp(root->u.non_leaf.child[1]->u.non_leaf.id, "main", 4) != 0)
		{
			printf("expect main function!\n");
			exit(0);
		}
		else
		{
			fprintf(f, ".data\n.globl main\n.text\nmain:\n");
			M_alloc(root->u.non_leaf.child[2]);
		}
		fprintf(f, " jr $ra\n");
	}
	else if(root->kind == A_compSt)
	{
		if(root->u.non_leaf.child_num == 0)
			return;
		else
		{
			int var_num = M_varNum(root);
			fprintf(f, " addi $sp, $sp, -%d\n sw $ra, 0($sp)\n", (var_num + 1) * 4);
			M_alloc(root->u.non_leaf.child[0]);
			if(root->u.non_leaf.child_num == 2)
				M_alloc(root->u.non_leaf.child[1]);
		}
	}
	else if(root->kind == A_def)	
	{
		M_alloc(root->u.non_leaf.child[1]);
		if(root->u.non_leaf.sibling != NULL)
			M_alloc(root->u.non_leaf.sibling);
	}
	else if(root->kind == A_dec)
	{
		if(root->u.non_leaf.child[0]->kind != A_id)
		{
			printf("array reference is not allowed!\n");
			exit(0);
		}
		VarList* var = get_varType(root->u.non_leaf.child[0]->u.id_leaf.id);
		var->sp = (alloc_num++) * 4;
		if(root->u.non_leaf.child_num == 2)
			reg_dec(f, root->u.non_leaf.child[0], root->u.non_leaf.child[1]);
		if(root->u.non_leaf.sibling != NULL)
			M_alloc(root->u.non_leaf.sibling);
	}
	else if(root->kind == A_expStmt)
	{
		reg_exp(f, root->u.non_leaf.child[0]);
		if(root->u.non_leaf.sibling != NULL)
			M_alloc(root->u.non_leaf.sibling);
	}
	else if(root->kind == A_returnStmt)
	{
		if(root->u.non_leaf.child[0]->u.non_leaf.child_num == 1)
		{
			A_node lf = root->u.non_leaf.child[0]->u.non_leaf.child[0];
			if(lf->kind == A_id || lf->kind == A_intNum)
			{
				if(lf->kind == A_id)
				{
					fprintf(f, " lw $t0, %d($sp)\n", lf->u.id_leaf.sp);
					fprintf(f, " move $v0, $t0\n");
				}
				else
					fprintf(f, " li $v0, %d\n", lf->u.int_leaf.value);
			}
			else
			{
				printf("return type invalid!\n");
				exit(0);
			}
		}
		else {
			printf("return type invalid!\n");
			exit(0);
		}
	}
	else {
		printf("this is the simplest code generater! Please drop out advanced grammar!\n");
		exit(0);
	}
}






			
			

