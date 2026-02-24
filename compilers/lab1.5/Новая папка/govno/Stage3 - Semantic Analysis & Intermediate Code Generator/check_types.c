#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "check_types.h"
#include "hashtbl.h"
#include "ast.h"

#define MAX_STR_CONST 	200

extern void yyerror(char *message);
extern Const bisonIsConst; 
extern int sem_error;
extern char enum_limit1[MAX_STR_CONST];
extern char enum_limit2[MAX_STR_CONST];
extern HASHTBL *hashtbl;
extern int scope;
extern For_List *fstack;

AST_TS_Union *sem_check_MULT_PLUS_MINUS(AST_TS_Union *exp1, Operation op ,AST_TS_Union *exp2){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1, *rleaf2;
	struct ast_set_list_node *temp_set1, *temp_set2;
	Type t1, t2, res_type;
	Type_Struct *type1 = NULL; 
	Type_Struct *type2 = NULL;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		t1 = exp1->type_struct->type;
		t2 = exp2->type_struct->type;
	}else{
		if(exp1->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp1->ast_node;
			t1 = temp->ts_value->type;
		}else if(exp1->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp1->ast_node;
			t1 = rleaf1->ts_value->type;
		}else if(exp1->ast_node->op == SET_LIST){
			temp_set1 = (struct ast_set_list_node *)exp1->ast_node;
			type1 = temp_set1->ts_value->T_Union.set.subtype;
		}else{
			t1 = exp1->ast_node->type;
		}
		
		if(exp2->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp2->ast_node;
			t2 = temp->ts_value->type;
		}else if(exp2->ast_node->op == REC_ID){
			rleaf2 = (struct ast_record_leaf_node *)exp2->ast_node;
			t2 = rleaf2->ts_value->type;
		}else if(exp2->ast_node->op == SET_LIST){
			temp_set2 = (struct ast_set_list_node *)exp2->ast_node;
			type2 = temp_set2->ts_value->T_Union.set.subtype;
		}else{
			t2 = exp2->ast_node->type;
		}
	}
		
	if(t1 == INTEGER){
		if(t2 == INTEGER){
			res_type = INTEGER;
			if(bisonIsConst == CONST){
				switch (op){
					case PLUS:
						result->type_struct->T_Union.data.intdata = exp1->type_struct->T_Union.data.intdata + exp2->type_struct->T_Union.data.intdata;
						break;
					case MINUS:
						result->type_struct->T_Union.data.intdata = exp1->type_struct->T_Union.data.intdata - exp2->type_struct->T_Union.data.intdata;
						break;
					case MULT:
						result->type_struct->T_Union.data.intdata = exp1->type_struct->T_Union.data.intdata * exp2->type_struct->T_Union.data.intdata;
						break;
					default:
						break;
				}
			}
		}else if(t2 == REAL){
			res_type = REAL;
			if(bisonIsConst == CONST){
				switch (op){
					case PLUS:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.intdata + exp2->type_struct->T_Union.data.d_data;
						break;
					case MINUS:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.intdata - exp2->type_struct->T_Union.data.d_data;
						break;
					case MULT:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.intdata * exp2->type_struct->T_Union.data.d_data;
						break;
					default:
						break;
				}
			}
		}else{
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if(t1 == REAL){
		if(t2 == INTEGER){
			res_type = REAL;
			if(bisonIsConst == CONST){
				switch (op){
					case PLUS:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data + exp2->type_struct->T_Union.data.intdata;
						break;
					case MINUS:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data - exp2->type_struct->T_Union.data.intdata;
						break;
					case MULT:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data * exp2->type_struct->T_Union.data.intdata;
						break;
					default:
						break;
				}
			}
		}else if(t2 == REAL){
			res_type = REAL;
			if(bisonIsConst == CONST){
				switch (op){
					case PLUS:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data + exp2->type_struct->T_Union.data.d_data;
						break;
					case MINUS:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data - exp2->type_struct->T_Union.data.d_data;
						break;
					case MULT:
						result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data * exp2->type_struct->T_Union.data.d_data;
						break;
					default:
						break;
				}
			}
		}else{
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if (type1 != NULL && type2 != NULL){
		if (type1->type == CHARACTER){
			if (type2->type == CHARACTER){
				res_type = SET;
			}
			else{
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}else if (type1->type == BOOLEAN){
			if (type2->type == BOOLEAN){
				res_type = SET;
			}
			else{
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}else if (type1->type == SUBRANGE){
			if (type2->type == SUBRANGE){
				if (type1->T_Union.subrange.subtype == type2->T_Union.subrange.subtype){
					res_type = SET;
				}else{
					sem_error = 1;
					yyerror("Type mismatch");
				}
			}
			else{
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}else{
			if (type1->T_Union.set.subtype == type2->T_Union.set.subtype){
				res_type = SET;
			}
			else{
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		result->ast_node = new_ast_node(op, res_type, exp1->ast_node, exp2->ast_node);
	}else{
		result->type_struct->type = res_type;
	}
	
	return result;
} 


AST_TS_Union *sem_check_AND_OR(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1, *rleaf2;
	Type t1, t2, res_type;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		t1 = exp1->type_struct->type;
		t2 = exp2->type_struct->type;
	}else{
		if(exp1->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp1->ast_node;
			t1 = temp->ts_value->type;
		}else if(exp1->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp1->ast_node;
			t1 = rleaf1->ts_value->type;
		}else{
			t1 = exp1->ast_node->type;
		}
		
		if(exp2->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp2->ast_node;
			t2 = temp->ts_value->type;
		}else if(exp2->ast_node->op == REC_ID){
			rleaf2 = (struct ast_record_leaf_node *)exp2->ast_node;
			t2 = rleaf2->ts_value->type;
		}else{
			t2 = exp2->ast_node->type;
		}
	}
	
	if(t1 == BOOLEAN && t2 == BOOLEAN){
		res_type = BOOLEAN;
		if(bisonIsConst == CONST){
			switch (op){
				case AND:
					result->type_struct->T_Union.data.booldata = exp1->type_struct->T_Union.data.booldata & exp2->type_struct->T_Union.data.booldata;
					break;
				case OR:
					result->type_struct->T_Union.data.booldata = exp1->type_struct->T_Union.data.booldata | exp2->type_struct->T_Union.data.booldata;
					break;
				default:
					break;
			}
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		result->ast_node = new_ast_node(op, res_type, exp1->ast_node, exp2->ast_node);
	}else{
		result->type_struct->type = res_type;
	}
	
	return result;
}

AST_TS_Union *sem_check_DIV_MOD(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1, *rleaf2;
	Type t1, t2, res_type;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		t1 = exp1->type_struct->type;
		t2 = exp2->type_struct->type;
	}else{
		if(exp1->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp1->ast_node;
			t1 = temp->ts_value->type;
		}else if(exp1->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp1->ast_node;
			t1 = rleaf1->ts_value->type;
		}else{
			t1 = exp1->ast_node->type;
		}
		
		if(exp2->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp2->ast_node;
			t2 = temp->ts_value->type;
		}else if(exp2->ast_node->op == REC_ID){
			rleaf2 = (struct ast_record_leaf_node *)exp2->ast_node;
			t2 = rleaf2->ts_value->type;
		}else{
			t2 = exp2->ast_node->type;
		}
	}
	
	if(t1 == INTEGER && t2 == INTEGER){
		res_type = INTEGER;
		if(bisonIsConst == CONST){
			switch (op){
				case DIV:
					result->type_struct->T_Union.data.intdata = exp1->type_struct->T_Union.data.intdata / exp2->type_struct->T_Union.data.intdata;
					break;
				case MOD:
					result->type_struct->T_Union.data.intdata = exp1->type_struct->T_Union.data.intdata % exp2->type_struct->T_Union.data.intdata;
					break;
				default:
					break;
			}
		}
	}else{
		printf("%d %d\n",t1,t2);
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		result->ast_node = new_ast_node(op, res_type, exp1->ast_node, exp2->ast_node);
	}else{
		result->type_struct->type = res_type;
	}
	
	return result;
}

AST_TS_Union *sem_check_DIVIDE(AST_TS_Union *exp1, AST_TS_Union *exp2){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1, *rleaf2;
	Type t1, t2, res_type;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		t1 = exp1->type_struct->type;
		t2 = exp2->type_struct->type;
	}else{
		if(exp1->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp1->ast_node;
			t1 = temp->ts_value->type;
		}else if(exp1->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp1->ast_node;
			t1 = rleaf1->ts_value->type;
		}else{
			t1 = exp1->ast_node->type;
		}
		
		if(exp2->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp2->ast_node;
			t2 = temp->ts_value->type;
		}else if(exp2->ast_node->op == REC_ID){
			rleaf2 = (struct ast_record_leaf_node *)exp2->ast_node;
			t2 = rleaf2->ts_value->type;
		}else{
			t2 = exp2->ast_node->type;
		}
	}
	
	res_type = REAL;
	
	if(t1 == INTEGER){
		if(t2 == INTEGER){
			if(bisonIsConst == CONST){
				result->type_struct->T_Union.data.d_data = (double)exp1->type_struct->T_Union.data.intdata + (double)exp2->type_struct->T_Union.data.intdata;
			}
		}else if(t2 == REAL){
			if(bisonIsConst == CONST){
				result->type_struct->T_Union.data.d_data = (double)exp1->type_struct->T_Union.data.intdata + exp2->type_struct->T_Union.data.d_data;
			}
		}else{
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if(t1 == REAL){
		if(t2 == INTEGER){
			if(bisonIsConst == CONST){
				result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data + (double)exp2->type_struct->T_Union.data.intdata;
			}
		}else if(t2 == REAL){
			if(bisonIsConst == CONST){
				result->type_struct->T_Union.data.d_data = exp1->type_struct->T_Union.data.d_data + exp2->type_struct->T_Union.data.d_data;
			}
		}else{
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		result->ast_node = new_ast_node(DIVIDE, res_type, exp1->ast_node, exp2->ast_node);
	}else{
		result->type_struct->type = res_type;
	}
	
	return result;
}

void sem_check_INT(AST_TS_Union *exp){
	struct ast_leaf_node *leaf;
	
	if(exp->ast_node->op == LEAF){
		leaf = (struct ast_leaf_node *)exp->ast_node;
		if(!(leaf->ts_value->type == INTEGER)){
			sem_error = 1;
			yyerror("Type is not Integer");
		}
	}
	else{
		if(!(exp->ast_node->type == INTEGER)){
			sem_error = 1;
			yyerror("Type is not Integer");
		}
	}
}

void sem_check_BOOL(AST_TS_Union *exp){
	
	if(!(exp->ast_node->type == BOOLEAN)){
		sem_error = 1;
		yyerror("Type is not Boolen");
	}
}

AST_TS_Union *sem_check_NOT(AST_TS_Union *exp){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1;
	Type t1, res_type;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		t1 = exp->type_struct->type;
	}else{
		if(exp->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp->ast_node;
			t1 = temp->ts_value->type;
		}else if(exp->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp->ast_node;
			t1 = rleaf1->ts_value->type;
		}else{
			t1 = exp->ast_node->type;
		}
	}
	
	res_type = BOOLEAN;
	
	if(t1 == BOOLEAN){
		if(bisonIsConst == CONST){
			result->type_struct->T_Union.data.booldata = !exp->type_struct->T_Union.data.booldata;
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		result->ast_node = new_ast_node(NOT, res_type , NULL , exp->ast_node);
	}else{
		result->type_struct->type = res_type;
	}
	
	return result;
}

Type_Struct *sem_check_SIGN(Type_Struct *ts, Operation op){
		
	if(ts->type == INTEGER){
		if(bisonIsConst == CONST){
			switch (op){
				case PLUS:
					ts->T_Union.data.intdata = ts->T_Union.data.intdata;
					break;
				case MINUS:
					ts->T_Union.data.intdata = -ts->T_Union.data.intdata;
					break;
				default:
					break;
			}
		}
	}else if(ts->type == REAL){
		if(bisonIsConst == CONST){
			switch (op){
				case PLUS:
					ts->T_Union.data.d_data = ts->T_Union.data.d_data;
					break;
				case MINUS:
					ts->T_Union.data.d_data = -ts->T_Union.data.d_data;
					break;
				default:
					break;
			}
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	return ts;
}

AST_TS_Union *sem_check_exp_SIGN(AST_TS_Union *exp, Operation op){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1;
	Type res_type;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		res_type = exp->type_struct->type;
	}else{
		if(exp->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp->ast_node;
			res_type = temp->ts_value->type;
		}else if(exp->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp->ast_node;
			res_type = rleaf1->ts_value->type;
		}else{
			res_type = exp->ast_node->type;
		}
	}
	
	if(res_type == INTEGER){
		if(bisonIsConst == CONST){
			result->type_struct->type = INTEGER;
			switch (op){
				case PLUS:
					result->type_struct->T_Union.data.intdata = exp->type_struct->T_Union.data.intdata;
					break;
				case MINUS:
					result->type_struct->T_Union.data.intdata = -exp->type_struct->T_Union.data.intdata;
					break;
				default:
					break;
			}
		}
	}else if(res_type == REAL){
		if(bisonIsConst == CONST){
			result->type_struct->type = REAL;
			switch (op){
				case PLUS:
					result->type_struct->T_Union.data.d_data = exp->type_struct->T_Union.data.d_data;
					break;
				case MINUS:
					result->type_struct->T_Union.data.d_data = -exp->type_struct->T_Union.data.d_data;
					break;
				default:
					break;
			}
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		if (op == MINUS){
			if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
			result->ast_node = new_ast_node(op, res_type, NULL, exp->ast_node);
			return result;
		}else{
			return exp;
		}
	}else{
		return exp;
	}
	
}

AST_TS_Union *sem_check_RELOP(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2){
	AST_TS_Union *result;
	struct ast_leaf_node *temp;
	struct ast_record_leaf_node *rleaf1, *rleaf2;
	struct ast_set_list_node *temp_set1, *temp_set2;
	Type t1, t2, res_type;
	Type_Struct *type1 = NULL; 
	Type_Struct *type2 = NULL;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		t1 = exp1->type_struct->type;
		t2 = exp2->type_struct->type;
	}else{
		if(exp1->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp1->ast_node;
			t1 = temp->ts_value->type;
		}else if(exp1->ast_node->op == REC_ID){
			rleaf1 = (struct ast_record_leaf_node *)exp1->ast_node;
			t1 = rleaf1->ts_value->type;
		}else if(exp1->ast_node->op == SET_LIST){
			temp_set1 = (struct ast_set_list_node *)exp1->ast_node;
			type1 = temp_set1->ts_value->T_Union.set.subtype;
		}else{
			t1 = exp1->ast_node->type;
		}
		
		if(exp2->ast_node->op == LEAF){
			temp = (struct ast_leaf_node *)exp2->ast_node;
			t2 = temp->ts_value->type;
		}else if(exp2->ast_node->op == REC_ID){
			rleaf2 = (struct ast_record_leaf_node *)exp2->ast_node;
			t2 = rleaf2->ts_value->type;
		}else if(exp2->ast_node->op == SET_LIST){
			temp_set2 = (struct ast_set_list_node *)exp2->ast_node;
			type2 = temp_set2->ts_value->T_Union.set.subtype;
		}else{
			t2 = exp2->ast_node->type;
		}
	}
	
	res_type = BOOLEAN;
	
	if(t1 == INTEGER){
		if(t2 == INTEGER){
			if(bisonIsConst == CONST){
				switch (op){
					case GREATER:
						if(exp1->type_struct->T_Union.data.intdata > exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case LESS:
						if(exp1->type_struct->T_Union.data.intdata < exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case G_EQU:
						if(exp1->type_struct->T_Union.data.intdata >= exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case L_EQU:
						if(exp1->type_struct->T_Union.data.intdata <= exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case EQU:
						if(exp1->type_struct->T_Union.data.intdata == exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					default:
						break;
				}
			}
		}else if(t2 == REAL){
			if(bisonIsConst == CONST){
				switch (op){
					case GREATER:
						if((double)exp1->type_struct->T_Union.data.intdata > exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case LESS:
						if((double)exp1->type_struct->T_Union.data.intdata < exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case G_EQU:
						if((double)exp1->type_struct->T_Union.data.intdata >= exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case L_EQU:
						if((double)exp1->type_struct->T_Union.data.intdata <= exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case EQU:
						if((double)exp1->type_struct->T_Union.data.intdata == exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					default:
						break;
				}
			}
		}else{
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if(t1 == REAL){
		if(t2 == INTEGER){
			if(bisonIsConst == CONST){
				switch (op){
					case GREATER:
						if(exp1->type_struct->T_Union.data.d_data > (double)exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case LESS:
						if(exp1->type_struct->T_Union.data.d_data < (double)exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case G_EQU:
						if(exp1->type_struct->T_Union.data.d_data >= (double)exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case L_EQU:
						if(exp1->type_struct->T_Union.data.d_data <= (double)exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case EQU:
						if(exp1->type_struct->T_Union.data.d_data == (double)exp2->type_struct->T_Union.data.intdata){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					default:
						break;
				}
			}
		}else if(t2 == REAL){
			if(bisonIsConst == CONST){
				switch (op){
					case GREATER:
						if(exp1->type_struct->T_Union.data.d_data > exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case LESS:
						if(exp1->type_struct->T_Union.data.d_data < exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case G_EQU:
						if(exp1->type_struct->T_Union.data.d_data >= exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case L_EQU:
						if(exp1->type_struct->T_Union.data.d_data <= exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					case EQU:
						if(exp1->type_struct->T_Union.data.d_data == exp2->type_struct->T_Union.data.d_data){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
						break;
					default:
						break;
				}
			}
		}else{
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if(t1 == CHARACTER && t2 == CHARACTER){
		if(bisonIsConst == CONST){
			switch (op){
				case GREATER:
					if(exp1->type_struct->T_Union.data.c_data > exp2->type_struct->T_Union.data.c_data){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case LESS:
					if(exp1->type_struct->T_Union.data.c_data < exp2->type_struct->T_Union.data.c_data){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case G_EQU:
					if(exp1->type_struct->T_Union.data.c_data >= exp2->type_struct->T_Union.data.c_data){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case L_EQU:
					if(exp1->type_struct->T_Union.data.c_data <= exp2->type_struct->T_Union.data.c_data){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case EQU:
					if(exp1->type_struct->T_Union.data.c_data == exp2->type_struct->T_Union.data.c_data){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				default:
					break;
			}
		}
	}else if(t1 == BOOLEAN && t2 == BOOLEAN){
		if(bisonIsConst == CONST){
			switch (op){
				case GREATER:
					if(exp1->type_struct->T_Union.data.booldata > exp2->type_struct->T_Union.data.booldata){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case LESS:
					if(exp1->type_struct->T_Union.data.booldata < exp2->type_struct->T_Union.data.booldata){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case G_EQU:
					if(exp1->type_struct->T_Union.data.booldata >= exp2->type_struct->T_Union.data.booldata){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case L_EQU:
					if(exp1->type_struct->T_Union.data.booldata <= exp2->type_struct->T_Union.data.booldata){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				case EQU:
					if(exp1->type_struct->T_Union.data.booldata == exp2->type_struct->T_Union.data.booldata){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				default:
					break;
			}
		}
	}else if(t1 == ENUMERATION && t2 == ENUMERATION){
		if(bisonIsConst == CONST){
			int c1 = 0;
			int c2 = 0;
			Boolean found;
			List *l1, *l2;
			l1 = exp1->type_struct->T_Union.list;
			l2 = exp2->type_struct->T_Union.list;
			
			switch (op){
				case GREATER:
					while (l2 != NULL){
						l1 = exp1->type_struct->T_Union.list;
						found = FALSE;
						while (l1 != NULL){
							if (!strcmp(l1->key,l2->key)){
								c1++;
								found = TRUE;
							}
							l1 = l1->next;
						}
						if (found == FALSE){
							result->type_struct->T_Union.data.booldata = FALSE;
							return result;
						}
						c2++;
						l2 = l2->next;
					}
					if (c1 == c2){
						result->type_struct->T_Union.data.booldata = FALSE;
					}else{
						result->type_struct->T_Union.data.booldata = TRUE;
					}
					break;
				case LESS:
					while (l1 != NULL){
						l2 = exp2->type_struct->T_Union.list;
						found = FALSE;
						while (l2 != NULL){
							if (!strcmp(l1->key,l2->key)){
								c1++;
								found = TRUE;
							}
							l2 = l2->next;
						}
						if (found == FALSE){
							result->type_struct->T_Union.data.booldata = FALSE;
							return result;
						}
						c2++;
						l1 = l1->next;
					}
					if (c1 == c2){
						result->type_struct->T_Union.data.booldata = FALSE;
					}else{
						result->type_struct->T_Union.data.booldata = TRUE;
					}
					break;
				case G_EQU:
					while (l2 != NULL){
						l1 = exp1->type_struct->T_Union.list;
						found = FALSE;
						while (l1 != NULL){
							if (!strcmp(l1->key,l2->key)){
								c1++;
								found = TRUE;
							}
							l1 = l1->next;
						}
						if (found == FALSE){
							result->type_struct->T_Union.data.booldata = FALSE;
							return result;
						}
						c2++;
						l2 = l2->next;
					}
					result->type_struct->T_Union.data.booldata = TRUE;
					break;
				case L_EQU:
					while (l1 != NULL){
						l2 = exp2->type_struct->T_Union.list;
						found = FALSE;
						while (l2 != NULL){
							if (!strcmp(l1->key,l2->key)){
								c1++;
								found = TRUE;
							}
							l2 = l2->next;
						}
						if (found == FALSE){
							result->type_struct->T_Union.data.booldata = FALSE;
							return result;
						}
						c2++;
						l1 = l1->next;
					}
					result->type_struct->T_Union.data.booldata = TRUE;
					break;
				case EQU:
					while (l2 != NULL){
						l1 = exp1->type_struct->T_Union.list;
						found = FALSE;
						while (l1 != NULL){
							if (!strcmp(l1->key,l2->key)){
								c1++;
								found = TRUE;
							}
							l1 = l1->next;
						}
						if (found == FALSE){
							result->type_struct->T_Union.data.booldata = FALSE;
							return result;
						}
						c2++;
						l2 = l2->next;
					}
					if (c1 == c2){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						result->type_struct->T_Union.data.booldata = FALSE;
					}
					break;
				default:
					break;
			}
		}
	}else if(t1 == SUBRANGE && t2 == SUBRANGE){
		t1 = exp1->type_struct->T_Union.subrange.subtype;
		t2 = exp2->type_struct->T_Union.subrange.subtype;
		
		if (t1 != t2){
			sem_error = 1;
			yyerror("Type mismatch");
			
		}
		
		if (t1 == ENUMERATION){
			if (exp1->type_struct->T_Union.subrange.list != exp1->type_struct->T_Union.subrange.list){
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}
		if(bisonIsConst == CONST){
			switch (op){
				case GREATER:
					if (t1 == CHARACTER){
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data == exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data == exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = FALSE;
						}else if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data <= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data >= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}else if (t1 == BOOLEAN){
						result->type_struct->T_Union.data.booldata = FALSE;
					}else{
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata == exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata == exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = FALSE;
						}else if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata <= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata >= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}
					break;
				case LESS:
					if (t1 == CHARACTER){
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data == exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data == exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = FALSE;
						}else if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data >= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data <= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}else if (t1 == BOOLEAN){
						result->type_struct->T_Union.data.booldata = FALSE;
					}else{
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata == exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata == exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = FALSE;
						}else if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata >= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata <= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}
					break;
				case G_EQU:
					if (t1 == CHARACTER){
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data <= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data >= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}else if (t1 == BOOLEAN){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata <= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata >= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}
					break;
				case L_EQU:
					if (t1 == CHARACTER){
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data >= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data <= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}else if (t1 == BOOLEAN){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata >= exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata <= exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}
					break;
				case EQU:
					if (t1 == CHARACTER){
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.c_data == exp2->type_struct->T_Union.subrange.start_value->T_Union.data.c_data) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.c_data == exp2->type_struct->T_Union.subrange.end_value->T_Union.data.c_data)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}else if (t1 == BOOLEAN){
						result->type_struct->T_Union.data.booldata = TRUE;
					}else{
						if ((exp1->type_struct->T_Union.subrange.start_value->T_Union.data.intdata == exp2->type_struct->T_Union.subrange.start_value->T_Union.data.intdata) 
								&& (exp1->type_struct->T_Union.subrange.end_value->T_Union.data.intdata == exp2->type_struct->T_Union.subrange.end_value->T_Union.data.intdata)){
							result->type_struct->T_Union.data.booldata = TRUE;
						}else{
							result->type_struct->T_Union.data.booldata = FALSE;
						}
					}
					break;
				default:
					break;
			}
		}
	}else if (t1 == ARRAY && t2 == ARRAY){
		t1 = exp1->type_struct->T_Union.array.subtype;
		t2 = exp2->type_struct->T_Union.array.subtype;
		
		if (t1 != t2){
			sem_error = 1;
			yyerror("Type mismatch");
		}	return result;

	}else if (t1 == ARRAY){
		t1 = exp1->type_struct->T_Union.array.subtype;
		
		if (t1 != t2){
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if (t2 == ARRAY){
		t2 = exp2->type_struct->T_Union.array.subtype;
		
		if (t1 != t2){
			sem_error = 1;
			yyerror("Type mismatch");
		}
	}else if (type1 != NULL && type2 != NULL){
		if (type1->type == CHARACTER){
			if (type2->type != CHARACTER){
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}else if (type1->type == BOOLEAN){
			if (type2->type != BOOLEAN){
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}else if (type1->type == SUBRANGE){
			if (type2->type == SUBRANGE){
				if (type1->T_Union.subrange.subtype != type2->T_Union.subrange.subtype){
					sem_error = 1;
					yyerror("Type mismatch");
				}
			}
			else{
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}else{
			if (type1->T_Union.set.subtype != type2->T_Union.set.subtype){
				sem_error = 1;
				yyerror("Type mismatch");
			}
		}
	}else{
		sem_error = 1;
		yyerror("Type mismatch");
	}
	
	if(bisonIsConst == NO_CONST){
		result->ast_node = new_ast_node(op, res_type, exp1->ast_node, exp2->ast_node);
	}else{
		result->type_struct->type = res_type;
	}
	
	return result;
}

AST_TS_Union *sem_check_INOP(AST_TS_Union *node, AST_TS_Union *set){
	AST_TS_Union *result = NULL;
	struct ast_leaf_node *leaf;
	struct ast_enum_leaf_node *eleaf;
	struct ast_set_list_node *sleaf;
	
	if (bisonIsConst == NO_CONST){
		sem_error = 1;
		yyerror("Operator \"in\" cannot be used in constants");
	}
	
	if (set->ast_node->op != SET_LIST){
		sem_error = 1;
		yyerror("Expression is not a set");
	}
	
	sleaf = (struct ast_set_list_node *)set->ast_node;
	switch (sleaf->ts_value->T_Union.set.subtype->type){
		case CHARACTER:
			if (node->ast_node->op != LEAF){
				if (node->ast_node->type != CHARACTER){
					sem_error = 1;
					yyerror("Incompatible types of element and set");
				}
			}else{
				leaf = (struct ast_leaf_node *)node->ast_node;
				
				if (leaf->ts_value->type != CHARACTER){
					sem_error = 1;
					yyerror("Incompatible types of element and set");
				}
			}
			break;
		case BOOLEAN:
			if (node->ast_node->op != LEAF){
				if (node->ast_node->type != BOOLEAN){
					sem_error = 1;
					yyerror("Incompatible types of element and set");
				}
			}else{
				leaf = (struct ast_leaf_node *)node->ast_node;
				
				if (leaf->ts_value->type != BOOLEAN){
					sem_error = 1;
					yyerror("Incompatible types of element and set");
				}
			}
			break;
		case SUBRANGE:
			if (node->ast_node->op != LEAF){
				if (node->ast_node->type != sleaf->ts_value->T_Union.set.subtype->T_Union.subrange.subtype){
					sem_error = 1;
					yyerror("Incompatible types of element and set");
				}
			}else{
				leaf = (struct ast_leaf_node *)node->ast_node;
				
				if (leaf->ts_value->type != sleaf->ts_value->T_Union.set.subtype->T_Union.subrange.subtype){
					sem_error = 1;
					yyerror("Incompatible types of element and set");
				}
			}
			break;
		case ENUMERATION:
			if (node->ast_node->op != ENUM_LEAF){
				sem_error = 1;
				yyerror("Incompatible types of element and set");
			}
			
			eleaf = (struct ast_enum_leaf_node *)node->ast_node;
			if (eleaf->enumeration != sleaf->ts_value->T_Union.set.subtype){
				sem_error = 1;
				yyerror("Element and set are not of the same enum type");
			}
			break;
		default:
			break;
	}
	
	return result;
}

Type_Struct *sem_check_CONST_ID(struct hashnode_s *node){
	
	if(node->isConst == CONST){
		return node->ts;
	}
	sem_error = 1;
	yyerror("ID in subrange is not a LEAF");
	
	return node->ts;
}

void sem_check_subrange(Type_Struct *ts1, Type_Struct *ts2){
	
	List *curr;
	int value1, value2;
	int flag = 0;
	
	
	if (ts1->type != ts2->type || ts1->type == REAL || ts2->type == REAL){
		sem_error = 1;
		yyerror("Invalid types in subrange limits");
	}
	
	
	if (ts1->type == ENUMERATION){
		if (ts1 != ts2){
			sem_error = 1;
			yyerror("Invalid types in subrange limits(Different enum type)");
		}
		if (enum_limit1[0] == '\0'){
			sem_error = 1;
			yyerror("ERROR IN ENUM_LIMIT1");
		} 
		if (enum_limit2[0] == '\0'){
			sem_error = 1;
			yyerror("ERROR IN ENUM_LIMIT2");
		}
		curr = ts1->T_Union.list;
		while(curr != NULL){
			if (!strcmp(curr->key, enum_limit1)) {
				flag++;
				value1 = curr->value;
				printf("value1 = %d, with name: %s\n", value1, enum_limit1);
			}
			if (!strcmp(curr->key, enum_limit2)) {
				flag++;
				value2 = curr->value;
				printf("value2 = %d, with name: %s\n", value2, enum_limit2);
			}
			curr = curr->next;
		}
		if (flag != 2){
			sem_error = 1;
			yyerror("ERROR IN ENUM_LIMIT FLAG");
		}
		if (value1 >= value2){
			sem_error = 1;
			yyerror("Subrange's enum with wrong values");
		}
	}
}

void sem_check_ast_subrange(AST_node *node1, AST_node *node2){
	struct ast_leaf_node *leaf1, *leaf2;
	
	if (node1->op == LEAF){
		leaf1 = (struct ast_leaf_node *)node1;
		
		if (node2->op == LEAF){
			leaf2 = (struct ast_leaf_node *)node2;
			
			if (leaf1->ts_value->type != leaf2->ts_value->type){
				sem_error = 1;
				yyerror("Invalid types in subrange limits");
			}
		}else{
			if (leaf1->ts_value->type != node2->type){
				sem_error = 1;
				yyerror("Invalid types in subrange limits");
			}
		}
	}else{
		if (node2->op == LEAF){
			leaf2 = (struct ast_leaf_node *)node2;
			
			if (node1->type != leaf2->ts_value->type){
				sem_error = 1;
				yyerror("Invalid types in subrange limits");
			}
		}else{
			if (node1->type != node2->type){
				sem_error = 1;
				yyerror("Invalid types in subrange limits");
			}
		}
	}
	
}


Type_Struct *sem_check_subrange_ts(Type_Struct *ts){
	
	if (ts->type != SUBRANGE){
		sem_error = 1;
		yyerror("Id is not subrange type");
	}
	
	return ts;
}

void sem_check_set(Type_Struct *ts){
	
	List *curr;
	
	if (ts->type == REAL || ts->type == INTEGER){
		sem_error = 1;
		yyerror("Invalid type of set");
	}
	
	if (ts->type == SUBRANGE){
		if ((ts->T_Union.subrange.end_value->T_Union.data.intdata - ts->T_Union.subrange.start_value->T_Union.data.intdata + 1) > 128){
			sem_error = 1;
			yyerror("Subrange out of set's bounds");
		}
	}
	
	if (ts->type == ENUMERATION){
		curr = ts->T_Union.list;
		while(curr->next != NULL){
			curr = curr->next;
		}
		if( curr->value + 1 > 128){
			sem_error = 1;
			yyerror("Enumeration out of set's bounds");
		}
	}
}

AST_node *sem_check_add_set(AST_node *left, AST_node *right){
	struct ast_leaf_node *leaf1 = NULL;
	struct ast_leaf_node *leaf2;
	struct ast_node *result, *curr;
	int i;
	
	if (left->op != COMMA){
		leaf1 = (struct ast_leaf_node *)left;
		
		if (right->op == LEAF){
			leaf2 = (struct ast_leaf_node *)right;
			
			if (leaf1->ts_value->type != leaf2->ts_value->type){
				sem_error = 1;
				yyerror("Invalid type of set's elements1");
			}
		}else if(right->op == ENUM_LEAF){
			if (left->op != ENUM_LEAF){
				sem_error = 1;
				yyerror("Invalid type of set's elements3");
			}
			
		}else{
			if (leaf1->ts_value->type != right->type){
				sem_error = 1;
				yyerror("Invalid type of set's elements2");
			}
		}
		
		result = new_ast_node(COMMA, NOTHING, left, right);
		
		return result;
	}
	
	
	
	if (right->op == LEAF){
		leaf2 = (struct ast_leaf_node *)right;
		
		if (left->right->type != leaf2->ts_value->type){
			sem_error = 1;
			yyerror("Invalid type of set's elements");
		}
	}else if(right->op == ENUM_LEAF){
		if (left->right->op != ENUM_LEAF){
			sem_error = 1;
			yyerror("Invalid type of set's elements");
		}
		
	}else{
		if (left->right->type != right->type){
			sem_error = 1;
			yyerror("Invalid type of set's elements");
		}
	}
	
	i = 2;
	curr = left;
	while (curr->left->op == COMMA){
		i++;
		curr = curr->left;
	}
	
	if (i + 1 > 128){
		sem_error = 1;
		yyerror("Too many elements in set");
	}
	
	result = new_ast_node(COMMA, NOTHING, left, right);
	
	return result;
}


AST_TS_Union *sem_check_extract_field(AST_TS_Union *id, char *key){
	AST_TS_Union *result;
	Type_Struct *ts;
	struct ast_leaf_node *leaf;
	struct ast_node *new_leaf, *current;
	List *curr;
	
	if (bisonIsConst == CONST){
		sem_error = 1;
		yyerror("Element of record cannot be Constant");
	}
	
	if (id->ast_node->op == LEAF){
		leaf = (struct ast_leaf_node *)id->ast_node;
		
		if (leaf->ts_value->type != RECORD){
			sem_error = 1;
			yyerror("Identifier is not an record1");
		}
	}else{
		if (id->ast_node->op == DOT){
			for (current = id->ast_node->right; current->op != LEAF; current = (current->op == DOT)? current->right: current->left);
		}else{
			for (current = id->ast_node->left; current->op != LEAF; current = (current->op == BRACKS)? current->left: current->right);
		}
		
		leaf = (struct ast_leaf_node *)current;
		
		if (leaf->ts_value->type == ARRAY){
			if (leaf->ts_value->T_Union.array.subtype != RECORD){
				sem_error = 1;
				yyerror("Identifier is not an record2");
			}
		}else{
			if (leaf->ts_value->type != RECORD){
				sem_error = 1;
				yyerror("Identifier is not an record3");
			}
		}
	}
	
	if (leaf->ts_value->type == RECORD){
		curr = leaf->ts_value->T_Union.list;
	}else{
		for (ts = leaf->ts_value->T_Union.array.subts; ts->type != RECORD; ts = ts->T_Union.array.subts);
		curr = ts->T_Union.list;
	}
	
	while(curr != NULL){
		if(!strcmp(curr->key, key)) {
			break;
		}
		curr = curr->next;
	}
	if (curr == NULL){
		sem_error = 1;
		yyerror("There is no field with such name in this record");
		return NULL;
	}
	
	new_leaf = new_ast_leaf_node(curr->field, curr->key);
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	result->ast_node = new_ast_node(DOT, curr->field->type, id->ast_node, new_leaf);
	
	return result;
}

AST_TS_Union *sem_check_array(AST_TS_Union *id, AST_node *dims){
	struct ast_node *curr;
	struct ast_leaf_node *leaf;
	AST_TS_Union *result;
	Type_Struct *ts;
	Type subtype;
	int check;
	
	if (bisonIsConst == CONST){
		sem_error = 1;
		yyerror("Element of array cannot be Constant");
	}
	
	if (id->ast_node->op == LEAF){
		leaf = (struct ast_leaf_node *)id->ast_node;
		if (leaf->ts_value->type != ARRAY){
			sem_error = 1;
			yyerror("Identifier is not an array");
		}
		
		subtype = leaf->ts_value->T_Union.array.subtype;
	}else if (id->ast_node->op == BRACKS){
		for (curr = id->ast_node->left; curr->op != LEAF; curr = (curr->op == BRACKS)? curr->left: curr->right);
		leaf = (struct ast_leaf_node *)curr;
		
		if (leaf->ts_value->type != ARRAY){
			sem_error = 1;
			yyerror("Identifier is not an array");
		}
		
		for (ts = leaf->ts_value, subtype = ts->T_Union.array.subtype; subtype == ARRAY; ts = ts->T_Union.array.subts, subtype = ts->type);
	}else{
		for (curr = id->ast_node->right; curr->op != LEAF; curr = (curr->op == DOT)? curr->right: curr->left);
		leaf = (struct ast_leaf_node *)curr;
		
		if (leaf->ts_value->type != ARRAY){
			sem_error = 1;
			yyerror("Identifier is not an array");
		}
		
		subtype = leaf->ts_value->T_Union.array.subtype;
	}
	
	check = check_dims(leaf->ts_value->T_Union.array.dims, dims);
	switch (check){
		case 2:
			sem_error = 1;
			yyerror("This array has less dimensions than expected");
			break;
		case 1:
			sem_error = 1;
			yyerror("This array has more dimensions than expected");
			break;
		case -1:
			sem_error = 1;
			yyerror("Incompatible type of dimensions in array");
			break;
		case -2:
			sem_error = 1;
			yyerror("Incompatible value of enumeration in dimensions of array");
			break;
		default:
			break;
	}

	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	result->ast_node = new_ast_node(BRACKS, subtype, id->ast_node, dims);

	return result;
}

int check_dims(Type_Struct *list, AST_node *root){
	static int depth = 0;
	static Type_Struct *clist;
	struct ast_leaf_node *leaf;
	struct ast_enum_leaf_node *eleaf;
	int result;
	
	if (depth == 0){
		clist = list;
	}
	
	depth++;
	
	if(root->op == ENUM_LEAF){
		eleaf = (struct ast_enum_leaf_node *)root;
		if (clist->T_Union.subrange.list != eleaf->enumeration->T_Union.list){
			return -2;
		}
		
		clist = clist->next;
		depth--;
		return 0;
	}else if (root->op == LEAF || root->op == REC_ID){
		leaf = (struct ast_leaf_node *)root;
		if (clist->T_Union.subrange.subtype != leaf->ts_value->type){
			return -1;
		}
		
		clist = clist->next;
		depth--;
		return 0;
	}else if (root->op != COMMA){
		if (clist->T_Union.subrange.subtype != root->type){
			return -1;
		}
		
		clist = clist->next;
		depth--;
		return 0;
	}else{
		result = check_dims(list, root->left);
		if (result != 0){
			return result;
		}
		
		if (clist == NULL){
			return 2;
		}
		
		result = check_dims(list, root->right);
		if (result != 0){
			return result;
		}
		
		depth--;
		
		if(clist == NULL && depth != 0){
			return 2;
		}else if (clist != NULL && depth == 0){
			return 1;
		}else{
			return 0;
		}
	}
}

AST_node *sem_check_assign(AST_TS_Union *id, AST_TS_Union *exp){
	AST_node *result;
	struct ast_leaf_node *leaf, *fleaf, *lid;
	struct ast_record_leaf_node *rleaf;
	struct ast_set_list_node *slist;
	For_List *flist;
	
	if (id->ast_node->op == LEAF){
		lid = (struct ast_leaf_node*) id->ast_node;
		if(fstack != NULL){
			for (flist = fstack; flist != NULL; flist = flist->next){
				fleaf = (struct ast_leaf_node *)flist;
				if ((!strcmp(lid->name, fleaf->name)) && (lid->ts_value == fleaf->ts_value)){
					sem_error = 1;
					yyerror("Increment of for cannot be modified");
				}
			}
		}
		
		
		if (lid->ts_value->type != FUNCTION_NAME){
			if (lid->ts_value->type == PROCEDURE_NAME){
				sem_error = 1;
				yyerror("There cannot be value assigned to procedure");
			}
			
			if (exp->ast_node->op == LEAF){
				leaf = (struct ast_leaf_node*) exp->ast_node;
				if (lid->ts_value->type != leaf->ts_value->type){
					sem_error = 1;
					yyerror("Conflicting types in assigment1");
				}
			}else if (exp->ast_node->op == SET_LIST){
				if (lid->ts_value->type != SET){
					sem_error = 1;
					yyerror("Conflicting types in assigment2");
				}
				
				slist = (struct ast_set_list_node *)exp->ast_node;
				if (lid->ts_value->T_Union.set.subtype->type != slist->ts_value->T_Union.set.subtype->type){
					sem_error = 1;
					yyerror("Conflicting types in assigment3");
				}
			}else{
				if (lid->ts_value->type != exp->ast_node->type){
					sem_error = 1;
					yyerror("Conflicting types in assigment4");
				}
			}	
		}else{
			if (exp->ast_node->op == LEAF){
				leaf = (struct ast_leaf_node*) exp->ast_node;
				if (lid->ts_value->T_Union.func_proc.returnType != leaf->ts_value->type){
					sem_error = 1;
					yyerror("Conflicting types in assigment5");
				}
			}else{
				if (lid->ts_value->T_Union.func_proc.returnType != exp->ast_node->type){
					sem_error = 1;
					yyerror("Conflicting types in assigment6");
				}
			}
		}
	}else if (id->ast_node->op == REC_ID){
		rleaf = (struct ast_record_leaf_node*) id->ast_node;
		if (exp->ast_node->op == LEAF){
			leaf = (struct ast_leaf_node*) exp->ast_node;
			if (rleaf->ts_value->type != leaf->ts_value->type){
				sem_error = 1;
				yyerror("Conflicting types in assigment7");
			}
		}else{
			if (rleaf->ts_value->type != exp->ast_node->type){
				sem_error = 1;
				yyerror("Conflicting types in assigment8");
			}
		}
	}else if (id->ast_node->op == SET_LIST){
		if (exp->ast_node->op != SET_LIST){
				sem_error = 1;
				yyerror("Conflicting types in assigment9");
		}
	}else{
		if (exp->ast_node->op == LEAF){
			leaf = (struct ast_leaf_node*) exp->ast_node;
			if (id->ast_node->type != leaf->ts_value->type){
				sem_error = 1;
				yyerror("Conflicting types in assigment10");
			}
		}else{
			if (id->ast_node->type != exp->ast_node->type){
				sem_error = 1;
				printf("== %d %d",id->ast_node->type ,exp->ast_node->type);
				yyerror("Conflicting types in assigment11");
			}
		}
	}
	
	
	result = new_ast_node(ASSIGN, NOTHING, id->ast_node, exp->ast_node);
	printf("%p\n",result);
	return result;
}

List *sem_check_record_with(AST_TS_Union *id){
	struct ast_leaf_node *leaf;
	struct ast_node *current;
	Type_Struct *ts;
	
	if (id->ast_node->op == LEAF){
		leaf = (struct ast_leaf_node *)id->ast_node;
		
		if (leaf->ts_value->type != RECORD){
			sem_error = 1;
			yyerror("Identifier in \"with\" is not a record1");
		}
	}else{
		if (id->ast_node->op == DOT){
			for (current = id->ast_node->right; current->op != LEAF; current = (current->op == DOT)? current->right: current->left);
		}else{
			for (current = id->ast_node->left; current->op != LEAF; current = (current->op == BRACKS)? current->left: current->right);
		}
		
		leaf = (struct ast_leaf_node *)current;
		if (leaf->ts_value->type != RECORD){
			for (ts = leaf->ts_value->T_Union.array.subts; ts->type != RECORD; ts = ts->T_Union.array.subts);
			
			if (ts->type != RECORD){
				sem_error = 1;
				yyerror("Expression in \"with\" is not a record2");
			}
			return ts->T_Union.list;
		}
		
	}
	
	return leaf->ts_value->T_Union.list;
}

AST_node* sem_check_proc(char *key ,AST_node *tree){
	AST_node *result;
	AST_TS_Union *look;
	struct ast_leaf_node *leaf;
	int check;
	
	look = ht_extract_ast_ts_union(ht_lookup(hashtbl, scope, key), key);
	
	leaf = (struct ast_leaf_node *)look->ast_node;
	if (leaf->ts_value->type != PROCEDURE_NAME){
		if (leaf->ts_value->type == FUNCTION_NAME){
			sem_error = 1;
			yyerror("ID is a function, not a procedure");
		}else{
			sem_error = 1;
			yyerror("ID is not a procedure");
		}
	}
	
	check = check_list_tree(leaf->ts_value->T_Union.func_proc.list, tree);
	switch (check){
		case 4:
			sem_error = 1;
			yyerror("This procedure has no arguments in its prototype");
			break;
		case 3:
			sem_error = 1;
			yyerror("This procedure has arguments in its prototype");
			break;
		case 2:
			sem_error = 1;
			yyerror("This procedure has less arguments than its prototype");
			break;
		case 1:
			sem_error = 1;
			yyerror("This procedure has more arguments than its prototype");
			break;
		case -1:
			sem_error = 1;
			yyerror("Incompatible type of arguments in procedure");
			break;
		default:
			break;
	}
	
	result = new_ast_func_proc_node(look->ast_node, tree, leaf->ts_value->T_Union.func_proc.astree);
	
	return result;
}

AST_TS_Union *sem_check_func(char *key ,AST_node *tree){
	AST_node *result;
	AST_TS_Union *look;
	struct ast_leaf_node *leaf;
	int check;
	
	look = ht_extract_ast_ts_union(ht_lookup(hashtbl, scope, key), key);
	
	leaf = (struct ast_leaf_node *)look->ast_node;
	if (leaf->ts_value->type != FUNCTION_NAME){
		if (leaf->ts_value->type == PROCEDURE_NAME){
			sem_error = 1;
			yyerror("ID is a procedure, not a function");
		}else{
			sem_error = 1;
			yyerror("ID is not a function");
		}
	}
	
	check = check_list_tree(leaf->ts_value->T_Union.func_proc.list, tree);
	switch (check){
		case 4:
			sem_error = 1;
			yyerror("This procedure has no arguments in its prototype");
			break;
		case 3:
			sem_error = 1;
			yyerror("This procedure has arguments in its prototype");
			break;
		case 2:
			sem_error = 1;
			yyerror("This procedure has less arguments than its prototype");
			break;
		case 1:
			sem_error = 1;
			yyerror("This procedure has more arguments than its prototype");
			break;
		case -1:
			sem_error = 1;
			yyerror("Incompatible type of arguments in procedure");
			break;
		default:
			break;
	}
	
	result = new_ast_func_proc_node(look->ast_node, tree, leaf->ts_value->T_Union.func_proc.astree);
	
	free(look->ast_node);
	look->ast_node = result;
	
	return look;
}


int check_list_tree(List *list, AST_node *root){
	static int depth = 0;
	static List *clist;
	struct ast_leaf_node *leaf;
	int result;
	
	if(depth == 0){
		clist = list;
		
		if (list == NULL && root == NULL){
			return 0;
		}
		if (list == NULL && root != NULL){
			return 4;
		}
		if (list != NULL && root == NULL){
			return 3;
		}
	}
	
	depth++;
	
	if (root->op == LEAF){
		leaf = (struct ast_leaf_node *)root;
		if (clist->field->type != leaf->ts_value->type){
			return -1;
		}
		
		clist = clist->next;
		depth--;
		return 0;
	}else if (root->op != COMMA){
		if (clist->field->type != root->type){
			return -1;
		}
		
		clist = clist->next;
		depth--;
		return 0;
	}else{
		result = check_list_tree(clist, root->left);
		if (result != 0){
			return result;
		}
		
		if (clist == NULL){
			return 2;
		}
		result = check_list_tree(clist, root->right);
		if (result != 0){
			return result;
		}
		
		depth--;
		
		if(clist == NULL && depth != 0){
			return 2;
		}else if (clist != NULL && depth == 0){
			return 1;
		}else{
			return 0;
		}
	}
}
