#include "util.h"

void A_print(A_node root)
{
	if(root->kind == A_type)
	{
		printf("%s  ", root->u.non_leaf.id);
		A_print(root->u.non_leaf.child[0]);

		//justify += 2;
	}
	else if(root->kind == A_struct)
	{
		printf("%s  %d\n", root->u.non_leaf.id, root->u.non_leaf.child_num);
		A_print(root->u.non_leaf.child[0]);
		if(root->u.non_leaf.child_num == 2)
			A_print(root->u.non_leaf.child[1]);

	}
	else if(root->kind == A_string)
	{
		printf("%s  ", root->u.string_leaf.string);
	}
	else if(root->kind == A_exp)
	{
		printf("%s  op:%s  %d\n", root->u.non_leaf.id, root->u.non_leaf.op, root->u.non_leaf.lineno);
		int child_num = root->u.non_leaf.child_num;
		int i = 0;
		for(; i < child_num; i++)
			A_print(root->u.non_leaf.child[i]);
	}
	else if(root->kind == A_intNum)
		printf("%d  %d\n", root->u.int_leaf.value, root->u.int_leaf.lineno);
	else if(root->kind == A_id)
		printf("%s  %d\n", root->u.id_leaf.id, root->u.id_leaf.lineno);
	else if(root->kind == A_floatNum)
		printf("%f  %d\n", root->u.float_leaf.value, root->u.float_leaf.lineno);
	else if(root->kind == A_dim)
	{
		printf("dim: %d\n", root->u.dim.value);
		A_print(root->u.dim.sibling);
	}
	else
	{
		int child_num = root->u.non_leaf.child_num;
		int i = 0;
		printf("%s  %d\n", root->u.non_leaf.id, root->u.non_leaf.lineno);

		for(; i < child_num; i++)
			A_print(root->u.non_leaf.child[i]);
		if(root->u.non_leaf.sibling != NULL)
			A_print(root->u.non_leaf.sibling);

	}
}


A_node A_Build(MultiTree* root)
{
	A_node program = (A_node)malloc(sizeof(*program));
	program->kind = A_nonLeaf;
	program->u.non_leaf.op = root->op;
	program->u.non_leaf.lineno = root->lineno;
	program->u.non_leaf.id = "program";
	program->u.non_leaf.sibling = NULL;
	program->u.non_leaf.child_num = 1;		//program has one child:Extdef
	program->u.non_leaf.child = (A_node*)malloc(sizeof(A_node));
	program->u.non_leaf.child[0] = A_ExtractExtdeflist(root->child[0]);

	return program;

}

A_node A_ExtractExtdeflist(MultiTree* extdeflist)
{
	int child_num = get_childnum(extdeflist);
	if(child_num == 0)
		return NULL;
	
	A_node extdef;
	A_Extdef(&extdef, extdeflist->child[0]);
	if(child_num == 1)
		return extdef;

	extdef->u.non_leaf.sibling = A_ExtractExtdeflist(extdeflist->child[1]);
	return extdef;
}

void A_Extdef(A_node* extdef_a, MultiTree* extdef_b)
{
	(*extdef_a) = (A_node)malloc(sizeof(**extdef_a));
	(*extdef_a)->kind = A_nonLeaf;
	(*extdef_a)->u.non_leaf.op = extdef_b->op;
	(*extdef_a)->u.non_leaf.lineno = extdef_b->lineno;
	(*extdef_a)->u.non_leaf.id = "global definition";
	(*extdef_a)->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(extdef_b);

	if(child_num == 2)
	{

		(*extdef_a)->u.non_leaf.child_num = 1;
		(*extdef_a)->u.non_leaf.child = (A_node*)malloc(sizeof(A_node));		//drop SEMI
	}

	else if(child_num == 3)
	{
		if(memcmp(extdef_b->child[2]->node_name, "SEMI", 4) == 0)
		{
			(*extdef_a)->u.non_leaf.child_num = 2;
			(*extdef_a)->u.non_leaf.child = (A_node*)malloc(2 * sizeof(A_node));
			(*extdef_a)->u.non_leaf.child[1] = A_ExtractExtdeclist(extdef_b->child[1]);
		}
		else
		{
			(*extdef_a)->u.non_leaf.child_num = 3;
			(*extdef_a)->u.non_leaf.child = (A_node*)malloc(3 * sizeof(A_node));
			A_Fundec(&((*extdef_a)->u.non_leaf.child[1]), extdef_b->child[1]);
			A_Compst(&((*extdef_a)->u.non_leaf.child[2]), extdef_b->child[2]);
		}
	}

	A_Specifier(&((*extdef_a)->u.non_leaf.child[0]), extdef_b->child[0]);
}

A_node A_ExtractExtdeclist(MultiTree* extdeclist)
{
	int child_num = get_childnum(extdeclist);
	A_node vardec;


	vardec = A_ExtractVardec(extdeclist->child[0]);
	if(child_num == 3)
		vardec->u.non_leaf.sibling = A_ExtractExtdeclist(extdeclist->child[2]);

	return vardec;
}

void A_Fundec(A_node* fundec_a, MultiTree* fundec_b)
{
	int child_num = get_childnum(fundec_b);

	(*fundec_a) = (A_node)malloc(sizeof(**fundec_a));
	(*fundec_a)->kind = A_nonLeaf;
	(*fundec_a)->u.non_leaf.op = NULL;
	(*fundec_a)->u.non_leaf.lineno = fundec_b->lineno;
	(*fundec_a)->u.non_leaf.id = "function";
	(*fundec_a)->u.non_leaf.sibling = NULL;
	
	if(child_num == 3)
	{
		(*fundec_a)->u.non_leaf.child_num = 1;
		(*fundec_a)->u.non_leaf.child = (A_node*)malloc(sizeof(A_node));
		A_Id(&((*fundec_a)->u.non_leaf.child[0]), fundec_b->child[0]);
	}
	else
	{
		(*fundec_a)->u.non_leaf.child_num = 2;
		(*fundec_a)->u.non_leaf.child = (A_node*)malloc(2 * sizeof(A_node));
		A_Id(&((*fundec_a)->u.non_leaf.child[0]), fundec_b->child[0]);
		(*fundec_a)->u.non_leaf.child[1] = A_ExtractVarlist(fundec_b->child[2]);
	}
}

void A_Compst(A_node* compst_a, MultiTree* compst_b)
{
	(*compst_a) = (A_node)malloc(sizeof(**compst_a));
	(*compst_a)->kind = A_nonLeaf;
	(*compst_a)->u.non_leaf.op = NULL;
	(*compst_a)->u.non_leaf.lineno = compst_b->lineno;
	(*compst_a)->u.non_leaf.id = "statement block";
	(*compst_a)->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(compst_b);
	(*compst_a)->u.non_leaf.child_num = child_num - 2;		//非常tricky！！
	(*compst_a)->u.non_leaf.child = (A_node*)malloc((*compst_a)->u.non_leaf.child_num * sizeof(A_node));
	if(child_num == 4)
	{
		(*compst_a)->u.non_leaf.child[0] = A_ExtractDeflist(compst_b->child[1]);
		(*compst_a)->u.non_leaf.child[1] = A_ExtractStmtlist(compst_b->child[2]);
	}
	else if(child_num == 3)
		(*compst_a)->u.non_leaf.child[0] = A_ExtractStmtlist(compst_b->child[1]);
}

void A_Specifier(A_node* specifier_a, MultiTree* specifier_b)
{
	(*specifier_a) = (A_node)malloc(sizeof(*(*specifier_a)));
	(*specifier_a)->kind = A_type;
	(*specifier_a)->u.non_leaf.op = NULL;
	(*specifier_a)->u.non_leaf.lineno = specifier_b->lineno;
	(*specifier_a)->u.non_leaf.id = "type";
	(*specifier_a)->u.non_leaf.sibling = NULL;
	(*specifier_a)->u.non_leaf.child_num = 1;
	(*specifier_a)->u.non_leaf.child = (A_node*)malloc(sizeof(A_node));

	if(memcmp(specifier_b->child[0]->node_name, "TYPE", 4) == 0)
		A_Id(&((*specifier_a)->u.non_leaf.child[0]), specifier_b->child[0]);
	else
		A_Structspecifier(&((*specifier_a)->u.non_leaf.child[0]), specifier_b->child[0]);
}

A_node A_ExtractVardec(MultiTree* vardec)
{
	A_node vardec_p;
	int child_num = get_childnum(vardec);

	if(child_num == 1)
		A_Id(&vardec_p, vardec->child[0]);
	else
	{
		A_Dim(&vardec_p, vardec->child[2]);
		vardec_p->u.dim.sibling = A_ExtractVardec(vardec->child[0]);
	}

	return vardec_p;
}

void A_Dim(A_node* dim_a, MultiTree* dim_b)
{
	*dim_a = (A_node)malloc(sizeof(**dim_a));
	(*dim_a)->kind = A_dim;
	(*dim_a)->u.dim.value = dim_b->val.intNum;
}

void A_Id(A_node* id_a, MultiTree* id_b)
{
	(*id_a) = (A_node)malloc(sizeof(**id_a));
	(*id_a)->kind = A_id;
	(*id_a)->u.id_leaf.id = id_b->val.id;
	(*id_a)->u.id_leaf.lineno = id_b->lineno;
}
		
A_node A_ExtractVarlist(MultiTree* varlist)
{
	A_node paramdec;
	int child_num = get_childnum(varlist);
	A_Paramdec(&paramdec, varlist->child[0]);

	if(child_num == 3)
		paramdec->u.non_leaf.sibling = A_ExtractVarlist(varlist->child[2]);

	return paramdec;
}

A_node A_ExtractDeflist(MultiTree* deflist)
{
	A_node def;
	int child_num = get_childnum(deflist);

	if(child_num == 0)
		return NULL;

	A_Def(&def, deflist->child[0]);
	def->u.non_leaf.sibling = A_ExtractDeflist(deflist->child[1]);
	return def;
}

A_node A_ExtractStmtlist(MultiTree* stmtlist)
{
	A_node stmt;
	int child_num = get_childnum(stmtlist);

	if(child_num == 0)
		return NULL;

	A_Stmt(&stmt, stmtlist->child[0]);		//A_stmt有所不同，它其中有递归，因此综合了extract和非extract的特点。
	stmt->u.non_leaf.sibling = A_ExtractStmtlist(stmtlist->child[1]);

	return stmt;
}

void A_Structspecifier(A_node* structspecifier_a, MultiTree* structspecifier_b)
{
	(*structspecifier_a) = (A_node)malloc(sizeof(**structspecifier_a));
	(*structspecifier_a)->kind = A_struct;
	(*structspecifier_a)->u.non_leaf.op = NULL;
	(*structspecifier_a)->u.non_leaf.lineno = structspecifier_b->lineno;
	(*structspecifier_a)->u.non_leaf.id = "struct";
	(*structspecifier_a)->u.non_leaf.sibling = NULL;

	int child_num = get_childnum(structspecifier_b);
	if(child_num == 2) 
	{
		(*structspecifier_a)->u.non_leaf.child_num = 1;
		(*structspecifier_a)->u.non_leaf.child = (A_node*)malloc(sizeof(A_node));
		A_Id(&((*structspecifier_a)->u.non_leaf.child[0]), structspecifier_b->child[1]->child[0]);
	}
	else
	{
		if(get_childnum(structspecifier_b->child[1]) == 0) {
			(*structspecifier_a)->u.non_leaf.child_num = 1;
			(*structspecifier_a)->u.non_leaf.child = (A_node*)malloc(1 * sizeof(A_node));
			(*structspecifier_a)->u.non_leaf.child[0] = A_ExtractDeflist(structspecifier_b->child[3]);
		}
		else {
			(*structspecifier_a)->u.non_leaf.child_num = 2;
			(*structspecifier_a)->u.non_leaf.child = (A_node*)malloc(2 * sizeof(A_node));
			A_Id(&((*structspecifier_a)->u.non_leaf.child[0]), structspecifier_b->child[1]->child[0]);
			(*structspecifier_a)->u.non_leaf.child[1] = A_ExtractDeflist(structspecifier_b->child[3]);
		}
	}
}

void A_IntNum(A_node* intNum_a, MultiTree* intNum_b)
{
	(*intNum_a) = (A_node)malloc(sizeof(**intNum_a));
	(*intNum_a)->kind = A_intNum;
	(*intNum_a)->u.int_leaf.value = intNum_b->val.intNum;
	(*intNum_a)->u.int_leaf.lineno = intNum_b->lineno;
}

void A_Paramdec(A_node* paramdec_a, MultiTree* paramdec_b)
{
	(*paramdec_a) = (A_node)malloc(sizeof(**paramdec_a));
	(*paramdec_a)->kind = A_nonLeaf;
	(*paramdec_a)->u.non_leaf.op = NULL;
	(*paramdec_a)->u.non_leaf.lineno = paramdec_b->lineno;
	(*paramdec_a)->u.non_leaf.id = "parameter";
	(*paramdec_a)->u.non_leaf.sibling = NULL;
	(*paramdec_a)->u.non_leaf.child_num = 2;
	(*paramdec_a)->u.non_leaf.child = (A_node*)malloc(2 * sizeof(A_node));

	A_Specifier(&((*paramdec_a)->u.non_leaf.child[0]), paramdec_b->child[0]);
	(*paramdec_a)->u.non_leaf.child[1] = A_ExtractVardec(paramdec_b->child[1]);
}

void A_Def(A_node* def_a, MultiTree* def_b)
{
	(*def_a) = (A_node)malloc(sizeof(**def_a));
	(*def_a)->kind = A_nonLeaf;
	(*def_a)->u.non_leaf.op = NULL;
	(*def_a)->u.non_leaf.lineno = def_b->lineno;
	(*def_a)->u.non_leaf.id = "local definition";
	(*def_a)->u.non_leaf.sibling = NULL;
	(*def_a)->u.non_leaf.child_num = 2;
	(*def_a)->u.non_leaf.child = (A_node*)malloc((*def_a)->u.non_leaf.child_num * sizeof(A_node));
	A_Specifier(&((*def_a)->u.non_leaf.child[0]), def_b->child[0]);
	(*def_a)->u.non_leaf.child[1] = A_ExtractDeclist(def_b->child[1]);
}


void A_Stmt(A_node* stmt_a, MultiTree* stmt_b)
{
	(*stmt_a) = (A_node)malloc(sizeof(**stmt_a));
	(*stmt_a)->kind = A_nonLeaf;
	(*stmt_a)->u.non_leaf.op = NULL;
	(*stmt_a)->u.non_leaf.lineno = stmt_b->lineno;
	(*stmt_a)->u.non_leaf.id = "statement";
	(*stmt_a)->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(stmt_b);

	if(child_num == 1)
	{
		(*stmt_a)->u.non_leaf.child_num = 1;
		(*stmt_a)->u.non_leaf.child = (A_node*)malloc((*stmt_a)->u.non_leaf.child_num * sizeof(A_node));
		A_Compst(&((*stmt_a)->u.non_leaf.child[0]), stmt_b->child[0]);
	}
	else if(child_num == 2)
	{
		(*stmt_a)->u.non_leaf.child_num = 1;
		(*stmt_a)->u.non_leaf.child = (A_node*)malloc((*stmt_a)->u.non_leaf.child_num * sizeof(A_node));
		A_Exp(&((*stmt_a)->u.non_leaf.child[0]), stmt_b->child[0]);
	}
	else if(child_num == 3)
	{
		(*stmt_a)->u.non_leaf.child_num = 2;
		(*stmt_a)->u.non_leaf.child = (A_node*)malloc((*stmt_a)->u.non_leaf.child_num * sizeof(A_node));
		A_String(&((*stmt_a)->u.non_leaf.child[0]), "return");
		A_Exp(&((*stmt_a)->u.non_leaf.child[1]), stmt_b->child[1]);
	}
	else if(child_num == 5)
	{
		(*stmt_a)->u.non_leaf.child_num = 3;
		(*stmt_a)->u.non_leaf.child = (A_node*)malloc((*stmt_a)->u.non_leaf.child_num * sizeof(A_node));
		if(memcmp(stmt_b->child[0]->node_name, "WHILE", 5) == 0)
			A_String(&((*stmt_a)->u.non_leaf.child[0]), "while");
		else
			A_String(&((*stmt_a)->u.non_leaf.child[0]), "if");
		A_Exp(&((*stmt_a)->u.non_leaf.child[1]), stmt_b->child[2]);
		A_Stmt(&((*stmt_a)->u.non_leaf.child[2]), stmt_b->child[4]);
	}
	else
	{
		(*stmt_a)->u.non_leaf.child_num = 5;
		(*stmt_a)->u.non_leaf.child = (A_node*)malloc((*stmt_a)->u.non_leaf.child_num * sizeof(A_node));
		A_String(&((*stmt_a)->u.non_leaf.child[0]), "if");
		A_Exp(&((*stmt_a)->u.non_leaf.child[1]), stmt_b->child[2]);
		A_Stmt(&((*stmt_a)->u.non_leaf.child[2]), stmt_b->child[4]);
		A_String(&((*stmt_a)->u.non_leaf.child[3]), "else");
		A_Stmt(&((*stmt_a)->u.non_leaf.child[4]), stmt_b->child[6]);
	}
}

A_node A_ExtractDeclist(MultiTree* declist)
{
	A_node dec;
	int child_num = get_childnum(declist);
	A_Dec(&dec, declist->child[0]);

	if(child_num == 3)
		dec->u.non_leaf.sibling = A_ExtractDeclist(declist->child[2]);

	return dec;
}

void A_Dec(A_node* dec_a, MultiTree* dec_b)
{
	(*dec_a) = (A_node)malloc(sizeof(**dec_a));
	(*dec_a)->kind = A_nonLeaf;
	(*dec_a)->u.non_leaf.op = dec_b->op;
	(*dec_a)->u.non_leaf.lineno = dec_b->lineno;
	(*dec_a)->u.non_leaf.id = "declaration";
	(*dec_a)->u.non_leaf.sibling = NULL;
	if(dec_b->op != NULL)
	{
		(*dec_a)->u.non_leaf.child_num = 2;
		(*dec_a)->u.non_leaf.child = (A_node*)malloc(2 * sizeof(A_node));
		A_Exp(&((*dec_a)->u.non_leaf.child[1]), dec_b->child[1]);
	}
	else
	{
		(*dec_a)->u.non_leaf.child_num = 1;
		(*dec_a)->u.non_leaf.child = (A_node*)malloc(sizeof(A_node));
	}
	(*dec_a)->u.non_leaf.child[0] = A_ExtractVardec(dec_b->child[0]);
}


void A_Exp(A_node* exp_a, MultiTree* exp_b)
{
	(*exp_a) = (A_node)malloc(sizeof(**exp_a));
	(*exp_a)->kind = A_exp;
	(*exp_a)->u.non_leaf.op = exp_b->op;
	(*exp_a)->u.non_leaf.lineno = exp_b->lineno;
	(*exp_a)->u.non_leaf.id = "exp";
	(*exp_a)->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(exp_b);

	if(child_num == 1)
	{
		(*exp_a)->u.non_leaf.child_num =  child_num;
		(*exp_a)->u.non_leaf.child = (A_node*)malloc((*exp_a)->u.non_leaf.child_num * sizeof(A_node));
		if(memcmp(exp_b->child[0]->node_name, "ID", 2) == 0)
			A_Id(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
		else if(memcmp(exp_b->child[0]->node_name, "INT", 3) == 0)
			A_IntNum(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
		else
			A_FloatNum(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
	}
	else if(child_num == 2)
	{
		(*exp_a)->u.non_leaf.child_num = 1;	//单目操作符
		(*exp_a)->u.non_leaf.child = (A_node*)malloc((*exp_a)->u.non_leaf.child_num * sizeof(A_node));
		A_Exp(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
	}
	else if(child_num == 3)
	{
		if((*exp_a)->u.non_leaf.op != NULL)
		{
			(*exp_a)->u.non_leaf.child_num = 2;
			(*exp_a)->u.non_leaf.child = (A_node*)malloc((*exp_a)->u.non_leaf.child_num * sizeof(A_node));
			if(memcmp(exp_b->child[1]->op, "DOT", 3) != 0)
			{
				A_Exp(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
				A_Exp(&((*exp_a)->u.non_leaf.child[1]), exp_b->child[1]);
			}
			else
			{
				A_Exp(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
				A_Id(&((*exp_a)->u.non_leaf.child[1]), exp_b->child[1]);
			}
		}
		else {
			(*exp_a)->u.non_leaf.child_num = 1;
			(*exp_a)->u.non_leaf.child = (A_node*)malloc((*exp_a)->u.non_leaf.child_num * sizeof(A_node));
			if(memcmp(exp_b->child[0]->node_name, "ID", 2) == 0)
			{
				(*exp_a)->u.non_leaf.op = "()";
				A_Id(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
			}
			else
				A_Exp(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[1]);
		}
	}
	else
	{
		(*exp_a)->u.non_leaf.child_num = 2;
		(*exp_a)->u.non_leaf.child = (A_node*)malloc((*exp_a)->u.non_leaf.child_num * sizeof(A_node));
		if(memcmp(exp_b->child[0]->node_name, "ID", 2) == 0)
		{
			(*exp_a)->u.non_leaf.op = "()";
			A_Id(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
			(*exp_a)->u.non_leaf.child[1] = A_ExtractArgs(exp_b->child[2]);
		}
		else
		{
			(*exp_a)->u.non_leaf.op = "[]";
			A_Exp(&((*exp_a)->u.non_leaf.child[0]), exp_b->child[0]);
			A_Exp(&((*exp_a)->u.non_leaf.child[1]), exp_b->child[2]);
		}
	}
}

void A_String(A_node* string_a, char* string_b)
{
	(*string_a) = (A_node)malloc(sizeof(**string_a));
	(*string_a)->kind = A_string;
	(*string_a)->u.string_leaf.string = string_b;
}

void A_FloatNum(A_node* floatNum_a, MultiTree* floatNum_b)
{
	(*floatNum_a) = (A_node)malloc(sizeof(**floatNum_a));
	(*floatNum_a)->kind = A_floatNum;
	(*floatNum_a)->u.int_leaf.value = floatNum_b->val.floatNum;
	(*floatNum_a)->u.int_leaf.lineno = floatNum_b->lineno;
}

A_node A_ExtractArgs(MultiTree* args)
{
	A_node exp;
	A_Exp(&exp, args->child[0]);
	int child_num = get_childnum(args);
	
	if(child_num == 3)
		exp->u.non_leaf.sibling = A_ExtractArgs(args->child[2]);

	return exp;
}







	








	




