#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#ifndef ast
#define ast


// for binary/unary operators and expression lists
typedef struct ast_node{

  Operation op;
  Type type;

  struct ast_node *left;
  struct ast_node *right;
}AST_node;


// for function/procedure calls
struct ast_func_proc_node{
	
	Operation op;
	Type type;
	
	struct ast_node *id_node;
	struct ast_node *arguments;
	struct ast_node *statements;
};


// for "if/else" statements
struct ast_if_node{
	
  Operation op;

  struct ast_node *condition;
  struct ast_node *if_branch;
  struct ast_node *else_branch;
};


// for "while" statements
struct ast_while_node{
	
  Operation op;

  struct ast_node *condition;
  struct ast_node *while_branch;
};

// for "for" statements
struct ast_for_node{
	
	Operation op;
	
	Increment inc;
	struct ast_node *init_condition;
	struct ast_node *end_condition;
	struct ast_node *for_branch;
};


// for constant or id nodes
struct ast_leaf_node{

  Operation op;
  char *name;

  Type_Struct *ts_value;
};

// for enum values
struct ast_enum_leaf_node{

  Operation op;
  char *name;

  Type_Struct *ts_value;
  Type_Struct *enumeration;
};

// for id nodes used in with statements
struct ast_record_leaf_node{
	
	Operation op;
	char *name;
	AST_node *record;
	
	Type_Struct *ts_value;
};

// for set nodes
struct ast_set_list_node{

  Operation op;

  Type_Struct *ts_value;
};


// for read/write statements
struct ast_io_node{

  Operation op;

  struct ast_node *params;
};

AST_node *new_ast_node(Operation op, Type type, struct ast_node * left, struct ast_node * right);
AST_node *new_ast_func_proc_node(struct ast_node *id_node, struct ast_node *arguments, struct ast_node *statements);
AST_node *new_ast_if_node(struct ast_node *condition, struct ast_node *if_branch, struct ast_node *else_branch);
AST_node *new_ast_while_node(struct ast_node *condition, struct ast_node *while_branch);
AST_node *new_ast_for_node(Increment inc, struct ast_node *init_condition, struct ast_node *end_condition, struct ast_node *for_branch);
AST_node *new_ast_leaf_node(Type_Struct *ts_value, char *name);
AST_node *new_ast_enum_leaf_node(Type_Struct *ts_value, char *name, Type_Struct *enumeration);
AST_node *new_ast_record_leaf_node(Type_Struct *ts_value, char *name, AST_node *record);
AST_node *new_ast_set_list_node(Type_Struct *ts_value);
AST_node *new_ast_io_node(Operation op, AST_node *params);
void free_ast(struct ast_node *root);
void *emalloc(size_t size);

void ast_inorder_traversal_print(AST_node *root);
#endif
