#include "util.h"

A_node A_build(MultiTree* root)
{
	A_node program = (A_node)malloc(*A_node);
	program->kind = A_nonLeaf;
	program->u.op = root->op;
	program->u.lineno = root->lineno;
	program->u.non_leaf.id = "program";
	program->u.sibling = NULL;
	program->u.child_num = 1;		//program has one child:Extdef
	program->u.non_leaf.child = (A_node*)malloc(A_node);
	program->u.non_leaf.child[0] = A_extractExtdeflist(root->child[0]);

	return program;

}

A_node A_extractExtdeflist(MultiTree* extdeflist)
{
	int child_num = get_childnum(extdeflist);
	if(child_num == 0)
		return NULL;

	if(child_num == 1)
		return A_extractExtdeflist(extdeflist->child[0]);

	A_node extdef;
	A_extdef(&extdef, extdeflist->child[0]);
	extdef->u.non_leaf.sibling = A_extractExtdef(extdeflist->child[1]);
	return extdef;
}

void A_extdef(A_node* extdef_a, MultiTree* extdef_b)
{
	*extdef_a = (A_node)malloc(*A_node);
	*extdef_a->kind = A_nonLeaf;
	*extdef_a->u.non_leaf.op = extdef_b->op;
	*extdef_a->u.non_leaf.lineno = extdef_b->lineno;
	*extdef_a->u.non_leaf.id = "extdef";
	*extdef_a->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(extdef_b);

	if(child_num == 2)
	{

		*extdef_a->u.non_leaf.child_num = 1;
		*extdef_a->u.non_leaf.child = (A_node*)malloc(A_node);		//drop SEMI

	else if(child_num == 3)
	{
		if(memcmp(extdef_b->child[2]->node_name, "SEMI", 4) == 0)
		{
			*extdef_a->u.non_leaf.child_num = 2;
			*extdef_a->u.non_leaf.child = (A_node*)malloc(2 * A_node);
			*extdef_a->u.non_leaf.child[1] = A_extractExtdeclist(extdef_b->child[1]);
		}
		else
		{
			*extdef_a->u.non_leaf.child_num = 3;
			*extdef_a->u.non_leaf.child = (A_node*)malloc(3 * A_node);
			A_fundec(&(*extdef_a->u.non_leaf.child[1]), extdef_b->child[1]);
			A_compst(&(*extdef_a->u.non_leaf.child[2]), extdef_b->child[2]);
		}
	}

	A_specifier(&(*extdef_a->u.non_leaf.child[0]), extdef_b->child[0]);
}

A_node A_extractExtdeclist(MultiTree* extdeclist)
{
	int child_num = get_childnum(extdeclist);
	A_node vardec;


	vardec = A_extractVardec(extdeclist->child[0]);
	if(child_num == 3)
		vardec->u.non_leaf.sibling = A_extractExtdeclist(extdeclist->child[2]);

	return vardec;
}

void A_fundec(A_node* fundec_a, MultiTree* fundec_b)
{
	int child_num = get_childnum(fundec_b);

	*fundec_a = (A_node)malloc(*A_node);
	*fundec_a->kind = A_nonLeaf;
	*fundec_a->u.non_leaf.op = NULL;
	*fundec_a->u.non_leaf.lineno = fundec_b->lineno;
	*fundec_a->u.non_leaf.id = "fundec";
	*fundec_a->u.non_leaf.sibling = NULL;
	
	if(child_num == 3)
	{
		*fundec_a->u.non_leaf.child_num = 1;
		*fundec_a->u.non_leaf.child = (A_node*)malloc(A_node);
		A_id(&(*fundec_a->u.non_leaf.child[0]), fundec_b->child[0]);
	}
	else
	{
		*fundec_a->u.non_leaf.child_num = 2;
		*fundec_a->u.non_leaf.child = (A_node*)malloc(2 * A_node);
		A_id(&(*fundec_a->u.non_leaf.child[0]), fundec_b->child[0]);
		*fundec_a->u.non_leaf.child[1] = A_extractVarlist(fundec_b->child[3]);
	}
}

void A_compst(A_node* compst_a, MultiTree* compst_b)
{
	*compst_a = (A_node)malloc(*A_node);
	*compst_a->kind = A_nonLeaf;
	*compst_a->u.non_leaf.op = NULL;
	*compst_a->u.non_leaf.lineno = compst_b->lineno;
	*compst_a->u.non_leaf.id = "compst";
	*compst_a->u.non_leaf.sibling = NULL;
	*compst_a->u.non_leaf.child_num = 2;
	*compst_a->u.non_leaf.child = (A_node*)malloc(2 * A_node);
	*compst_a->u.non_leaf.child[0] = A_extractDeflist(compst_b->child[1]);
	*compst_a->u.non_leaf.child[1] = A_extractStmtlist(compst_b->child[2]);
}

void A_specifier(A_node* specifier_a, MultiTree* specifier_b)
{
	*specifier_a = (A_node)malloc(*A_node);
	*specifier_a->kind = A_nonLeaf;
	*specifier_a->u.non_leaf.op = NULL;
	*specifier_a->u.non_leaf.lineno = specifier_b->lineno;
	*specifier_a->u.non_leaf.id = "specifier";
	*specifier_a->u.non_leaf.sibling = NULL;
	*specifier_a->u.non_leaf.child_num = 1;
	*specifier_a->u.non_leaf.child = (A_node*)malloc(A_node);

	if(memcmp(specifier_b->child[0]->node_name, "TYPE", 4) == 0)
		A_id(&(*specifier_a->u.non_leaf.child[0]), specifier_b->child[0]);
	else
		A_structspecifier(&(*specifier_a->u.non_leaf.child[0]), specifier_b->child[0]);
}

A_node A_extractVardec(MultiTree* vardec)
{
	A_node vardec_p;
	int child_num = get_childnum(vardec);

	if(child_num == 1)
		A_id(&vardec_p, vardec->child[0]);
	else
	{
		A_intNum(&vardec_p, vardec->child[2]);
		vardec_p->u.non_leaf.sibling = A_extractVardec(vardec->child[0]);
	}

	return vardec_p;
}

void A_id(A_node* id_a, MultiTree* id_b)
{
	*id_a = (A_node)malloc(*A_node);
	*id_a->kind = A_id;
	*id_a->u.id_leaf.id = id_b->val.id;
	*id_a->u.id_leaf.lineno = id_b->lineno;
}
		
A_node A_extractVarlist(MultiTree* varlist)
{
	A_node paramdec;
	int child_num = get_childnum(varlist);
	A_paramdec(&paramdec, varlist->child[0]);

	if(child_num == 3)
		paramdec->u.non_leaf.sibling = A_extractVarlist(varlist->child[2]);

	return paramdec;
}

A_node A_extractDeflist(MultiTree* deflist)
{
	A_node def;
	int child_num = get_childnum(deflist);

	if(child_num == 0)
		return NULL;

	A_def(&def, deflist->child[0]);
	def->u.non_leaf.sibling = A_extractDeflist(deflist->child[1]);
	return def;
}

A_node A_extractStmtlist(MultiTree* stmtlist)
{
	A_node stmt;
	int child_num = get_childnum(stmtlist);

	if(child_num == 0)
		return NULL;

	A_stmt(&stmt, stmtlist->child[0]);		//A_stmt有所不同，它其中有递归，因此综合了extract和非extract的特点。
	stmt->u.non_leaf.sibling = A_extractStmtlist(stmtlist->child[1]);
}

void A_structspecifier(A_node* structspecifier_a, MultiTree* structspecifier_b)
{
	*structspecifier_a = (A_node)malloc(*A_node);
	*structspecifier_a->kind = A_nonLeaf;
	*structspecifier_a->u.non_leaf.op = NULL;
	*structspecifier_a->u.non_leaf.lineno = structspecifier_b->lineno;
	*structspecifier_a->u.non_leaf.id = "structspecifier";
	*structspecifier_a->u.non_leaf.sibling = NULL;

	int child_num = get_childnum(structspecifier_b);
	if(child_num == 2) 
	{
		*structspecifier_a->u.non_leaf.child_num = 1;
		*structspecifier_a->u.non_leaf.child = (A_node*)malloc(A_node);
		A_tag(&(*structspecifier_a->u.non_leaf.child[0]), structspecifier_b->child[1]);
	}
	else
	{
		*structspecifier_a->u.non_leaf.child_num = 2;
		*structspecifier_a->u.non_leaf.child = (A_node*)malloc(2 * A_node);
		A_opttag(&(*structspecifier_a->u.non_leaf.child[0]), structspecifier_b->child[1]);
		*structspecifier_a->u.non_leaf.child[1] = A_extractDeflist(structspecifier_b->child[3]);
	}
}

void A_intNum(A_node* intNum_a, MultiTree* intNum_b)
{
	*intNum_a = (A_node)malloc(*A_node);
	*intNum_a->kind = A_intNum;
	*intNum_a->u.int_leaf.value = intNum_b->val.intNum;
	*intNum_a->u.int_leaf.lineno = intNum_b->lineno;
}

void A_paramdec(A_node* paramdec_a, MultiTree* paramdec_b)
{
	*paramdec_a = (A_node)malloc(*A_node);
	*paramdec_a->kind = A_nonLeaf;
	*paramdec_a->u.non_leaf.op = NULL;
	*paramdec_a->u.non_leaf.lineno = paramdec_b->lineno;
	*paramdec_a->u.non_leaf.id = "paramdec";
	*paramdec_a->u.non_leaf.sibling = NULL;
	*paramdec_a->u.non_leaf.child_num = 2;
	*paramdec_a->u.non_leaf.child = (A_node*)malloc(2 * A_node);

	A_specifier(&(*paramdec_a->u.non_leaf.child[0]), paramdec_b->child[0]);
	*paramdec_a->u.non_leaf.child[1] = A_extractVardec(parmadec_b->child[1]);
}

void A_def(A_node* def_a, MultiTree* def_b)
{
	*def_a = (A_node)malloc(*A_node);
	*def_a->kind = A_nonLeaf;
	*def_a->u.non_leaf.op = NULL;
	*def_a->u.non_leaf.lineno = def_b->lineno;
	*def_a->u.non_leaf.id = "def";
	*def_a->u.non_leaf.sibling = NULL;
	*def_a->u.non_leaf.child_num = 2;
	*def_a->u.non_leaf.child = (A_node*)malloc(*def_a->u.non_leaf.child_num * A_node);
	A_specifier(&(*def_a->u.non_leaf.child[0]), def_b->child[0]);
	*def_a->u.non_leaf.child[1] = A_extractDeclist(def_b->child[1]);
}


void A_stmt(A_node* stmt_a, MultiTree* stmt_b)
{
	*stmt_a = (A_node)malloc(*A_node);
	*stmt_a->kind = A_nonLeaf;
	*stmt_a->u.non_leaf.op = NULL;
	*stmt_a->u.non_leaf.lineno = stmt_b->lineno;
	*stmt_a->u.non_leaf.id = "stmt";
	*stmt_a->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(stmt_b);

	if(child_num == 1)
	{
		*stmt_a->u.non_leaf.child_num = 1;
		*stmt_a->u.non_leaf.child = (A_node*)malloc(*stmt_a->u.non_leaf.child_num * A_node);
		A_compst(&(*stmt_a->u.non_leaf.child[0]), stmt_b->child[0]);
	}
	else if(child_num == 2)
	{
		*stmt_a->u.non_leaf.child_num = 1;
		*stmt_a->u.non_leaf.child = (A_node*)malloc(*stmt_a->u.non_leaf.child_num * A_node);
		A_exp(&(*stmt_a->u.non_leaf.child[0]), stmt_b->child[0]);
	}
	else if(child_num == 3)
	{
		*stmt_a->u.non_leaf.child_num = 2;
		*stmt_a->u.non_leaf.child = (A_node*)malloc(*stmt_a->u.non_leaf.child_num * A_node);
		A_string(&(*stmt_a->u.non_leaf.child[0]), "return");
		A_exp(&(*stmt_a->u.non_leaf.child[1]), stmt_b->child[1]);
	}
	else if(child_num == 5)
	{
		*stmt_a->u.non_leaf.child_num = 3;
		*stmt_a->u.non_leaf.child = (A_node*)malloc(*stmt_a->u.non_leaf.child_num * A_node);
		if(memcmp(stmt_b->child[0]->node_name, "WHILE", 5) == 0)
			A_string(&(*stmt_a->u.non_leaf.child[0]), "while");
		else
			A_string(&(*stmt_a->u.non_leaf.child[0]), "if");
		A_exp(&(*stmt_a->u.non_leaf.child[1]), stmt_b->child[2]);
		A_stmt(&(*stmt_a->u.non_leaf.child[2]), stmt_b->child[4]);
	}
	else
	{
		*stmt_a->u.non_leaf.child_num = 5;
		*stmt_a->u.non_leaf.child = (A_node*)malloc(*stmt_a->u.non_leaf.child_num * A_node);
		A_string(&(*stmt_a->u.non_leaf.child[0]), "if");
		A_exp(&(*stmt_a->u.non_leaf.child[1]), stmt_b->child[2]);
		A_stmt(&(*stmt_a->u.non_leaf.child[2]), stmt_b->child[4]);
		A_string(&(*stmt_a->u.non_leaf.child[3]), "else");
		A_stmt(&(*stmt_a->u.non_leaf.child[4]), stmt_b->child[6]);
	}
}

void A_tag(A_node* tag_a, MultiTree* tag_b)
{
	*tag_a = (A_node)malloc(*A_node);
	*tag_a->kind = A_nonLeaf;
	*tag_a->u.non_leaf.op = NULL;
	*tag_a->u.non_leaf.lineno = tag_b->lineno;
	*tag_a->u.non_leaf.id = "tag";
	*tag_a->u.non_leaf.sibling = NULL;
	*tag_a->u.non_leaf.child_num = 1;
	*tag_a->u.non_leaf.child = (A_node*)malloc(*tag_a->u.non_leaf.child_num * A_node);
	A_id(&(*tag_a->u.non_leaf.child[0]), tag_b->child[0]);
}

void A_opttag(A_node* optopttag_a, MultiTree* optopttag_b)
{
	*opttag_a = (A_node)malloc(*A_node);
	*opttag_a->kind = A_nonLeaf;
	*opttag_a->u.non_leaf.op = NULL;
	*opttag_a->u.non_leaf.lineno = opttag_b->lineno;
	*opttag_a->u.non_leaf.id = "opttag";
	*opttag_a->u.non_leaf.sibling = NULL;
	*opttag_a->u.non_leaf.child_num = 1;
	*opttag_a->u.non_leaf.child = (A_node*)malloc(*opttag_a->u.non_leaf.child_num * A_node);
	A_id(&(*opttag_a->u.non_leaf.child[0]), opttag_b->child[0]);
}

A_node A_extractDeclist(MultiTree* declist)
{
	A_node dec;
	int child_num = get_childnum(declist);
	A_dec(&dec, declist->child[0]);

	if(child_num == 3)
		dec->u.non_leaf.sibling = A_extractDeclist(declist->child[2]);

	return dec;
}

void a_exp(A_node* exp_a, MultiTree* exp_b)
{
	*exp_a = (A_node)malloc(*A_node);
	*exp_a->kind = A_nonLeaf;
	*exp_a->u.non_leaf.op = exp_b->op;
	*exp_a->u.non_leaf.lineno = exp_b->lineno;
	*exp_a->u.non_leaf.id = "exp";
	*exp_a->u.non_leaf.sibling = NULL;
	int child_num = get_childnum(exp_b);

	if(child_num == 1)
	{
		*exp_a->u.non_leaf.child_num =  child_num;
		*exp_a->u.non_leaf.child = (A_node*)malloc(*exp_a->u.non_leaf.child_num * A_node);
		if(memcmp(exp_b->child[0]->node_name, "ID", 2) == 0)
			A_id(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
		else if(memcmp(exp_b->child[0]->node_name, "INT", 3) == 0)
			A_intNum(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
		else
			A_floatNum(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
	}
	else if(child_num == 2)
	{
		*exp_a->u.non_leaf.child_num = 1;	//单目操作符
		*exp_a->u.non_leaf.child = (A_node*)malloc(*exp_a->u.non_leaf.child_num * A_node);
		A_exp(&(*exp_a->u.non_leaf.child[0]), exp_b->child[1]);
	}
	else if(child_num == 3)
	{
		if(*exp_a->u.non_leaf.op != NULL)
		{
			*exp_a->u.non_leaf.child_num = 2;
			*exp_a->u.non_leaf.child = (A_node*)malloc(*exp_a->u.non_leaf.child_num * A_node);
			if(memcmp(exp_b->child[1]->op, "DOT", 3) != 0)
			{
				A_exp(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
				A_exp(&(*exp_a->u.non_leaf.child[1]), exp_b->child[2]);
			}
			else
			{
				A_exp(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
				A_id(&(*exp_a->u.non_leaf.child[1]), exp_b->child[2]);
			}
		}
		else {
			*exp_a->u.non_leaf.child_num = 1;
			*exp_a->u.non_leaf.child = (A_node*)malloc(*exp_a->u.non_leaf.child_num * A_node);
			if(memcmp(exp_b->child[0]->node_name, "ID", 2) == 0)
			{
				*exp_a->u.non_leaf.op = "()";
				A_id(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
			}
			else
				A_exp(&(*exp_a->u.non_leaf.child[0]), exp_b->child[1]);
		}
	}
	else
	{
		*exp_a->u.non_leaf.child_num = 2;
		*exp_a->u.non_leaf.child = (A_node*)malloc(*exp_a->u.non_leaf.child_num * A_node);
		if(memcmp(exp_b->child[0]->node_name, "ID", 2) == 0)
		{
			*exp_a->u.non_leaf.op = "()";
			A_id(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
			exp_a->u.non_leaf.child[1] = A_extractArgs(exp_b->child[2]);
		}
		else
		{
			*exp_a->u.non_leaf.op = "[]";
			A_exp(&(*exp_a->u.non_leaf.child[0]), exp_b->child[0]);
			A_exp(&(*exp_a->u.non_leaf.child[1]), exp_b->child[2]);
		}
	}
}

void A_string(A_node* string_a, char* string_b)
{
	*string_a = (A_node)malloc(*A_node);
	*string_a->kind = A_string;
	*stirng_a->u.string_leaf.string = string_b;
}

void A_floatNum(A_node* floatNum_a, MultiTree* floatNum_b)
{
	*floatNum_a = (A_node)malloc(*A_node);
	*floatNum_a->kind = A_floatNum;
	*floatNum_a->u.int_leaf.value = floatNum_b->val.floatNum;
	*floatNum_a->u.int_leaf.lineno = floatNum_b->lineno;
}

A_node A_extractArgs(MultiTree* args)
{
	A_node exp;
	A_exp(&exp, args->child[0]);
	int child_num = get_childnum(args);
	
	if(child_num == 3)
		exp->u.non_leaf.sibling = A_extractArgs(args->child[2]);

	return exp;
}







	








	




