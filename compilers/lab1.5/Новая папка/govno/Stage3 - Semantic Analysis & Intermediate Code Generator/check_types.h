#include "hashtbl.h"
#include "types.h"
#include "ast.h"

#ifndef checktypes
#define checktypes


AST_TS_Union *sem_check_MULT_PLUS_MINUS(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2);
AST_TS_Union *sem_check_AND_OR(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2);
AST_TS_Union *sem_check_DIV_MOD(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2);
AST_TS_Union *sem_check_DIVIDE(AST_TS_Union *exp1, AST_TS_Union *exp2);
void sem_check_INT(AST_TS_Union *exp);
void sem_check_BOOL(AST_TS_Union *exp);
AST_TS_Union *sem_check_NOT(AST_TS_Union *exp);
Type_Struct *sem_check_SIGN(Type_Struct *ts, Operation op);
AST_TS_Union *sem_check_exp_SIGN(AST_TS_Union *exp, Operation op);
AST_TS_Union *sem_check_RELOP(AST_TS_Union *exp1, Operation op, AST_TS_Union *exp2);
AST_TS_Union *sem_check_INOP(AST_TS_Union *node, AST_TS_Union *set);

Type_Struct *sem_check_CONST_ID(struct hashnode_s *node);

void sem_check_subrange(Type_Struct *ts1, Type_Struct *ts2);
void sem_check_ast_subrange(AST_node *node1, AST_node *node2);
Type_Struct *sem_check_subrange_ts(Type_Struct *ts);

void sem_check_set(Type_Struct *ts);
AST_node *sem_check_add_set(AST_node *left, AST_node *right); 

AST_TS_Union *sem_check_extract_field(AST_TS_Union *id, char *key);

AST_TS_Union *sem_check_array(AST_TS_Union *id, AST_node *dims);
int check_dims(Type_Struct *list, AST_node *root);

AST_node *sem_check_assign(AST_TS_Union *id, AST_TS_Union *exp);

List *sem_check_record_with(AST_TS_Union *id);

AST_node *sem_check_proc(char *key ,AST_node *tree);
AST_TS_Union *sem_check_func(char *key ,AST_node *tree);
int check_list_tree(List *list, AST_node *root);
#endif
