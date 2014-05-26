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
	extdef->sibling = A_extractExtdef(extdeflist->child[1]);
	return extdef;
}

void A_extdef(A_node* extdef_a, MultiTree* extdef_b)
{
	*extdef_a = (A_node)malloc(*A_node);
	*extdef_a->kind = A_nonLeaf;
	*extdef_a->u.non_leaf.op = extdef_b->op;
	*extdef_a->u.non_leaf.lineno = extdef_b->lineno;
	*extdef_a->u.non_leaf.id = "extdef";
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

	return *vardec;
}

void A_fundec(A_node* fundec_a, MultiTree* fundec_b)
{
	int child_num = get_childnum(fundec_b);

	*fundec_a = (A_node)malloc(*A_node);
	*fundec_a->kind = A_nonLeaf;
	*fundec_a->u.non_leaf.op = NULL;
	*fundec_a->u.non_leaf.lineno = fundec_b->lineno;
	*fundec_a->u.non_leaf.id = "fundec";
	
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
	*paramdec_a->u.non_leaf.child_num = 2;
	*paramdec_a->u.non_leaf.child = (A_node*)malloc(2 * A_node);

	A_specifier(&(*paramdec_a->u.non_leaf.child[0]), paramdec_b->child[0]);
	*paramdec_a->u.non_leaf.child[1] = A_extractVardec(parmadec_b->child[1]);
}





	




