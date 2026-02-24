#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h" 


AST_node *new_ast_node(Operation op,Type type, struct ast_node * left, struct ast_node * right){
	struct ast_node * ast_node = emalloc (sizeof (struct ast_node));
	
	ast_node->op = op;
	ast_node->type = type;
	
	ast_node->left = left;
	ast_node->right = right;

	return ast_node;
}

/*
AST_node *new_ast_function_node(struct symbol_node * symbol, struct ast_node * arguments){
	struct ast_function_node * ast_node = emalloc (sizeof (struct ast_function_node));

	ast_node->op = FUNC_PROC;

	ast_node->arguments = arguments;
	ast_node->symbol = symbol;

	return (struct ast_node *) ast_node;
}*/


AST_node *new_ast_func_proc_node(struct ast_node *id_node, struct ast_node *arguments, struct ast_node *statements){
	struct ast_func_proc_node * ast_node = emalloc (sizeof (struct ast_func_proc_node));
	struct ast_leaf_node *leaf;
	
	ast_node->op = FUNC_PROC;
	
	ast_node->id_node = id_node;
	
	leaf = (struct ast_leaf_node *)id_node;
	ast_node->type = leaf->ts_value->T_Union.func_proc.returnType;
	
	ast_node->arguments = arguments;
	ast_node->statements = statements;
	
	return (struct ast_node *) ast_node;
}


AST_node *new_ast_if_node(struct ast_node * condition, struct ast_node * if_branch, struct ast_node * else_branch){
	
	struct ast_if_node * ast_node = emalloc (sizeof (struct ast_if_node));

	ast_node->op = IF;

	ast_node->condition = condition;
	ast_node->if_branch = if_branch;
	ast_node->else_branch = else_branch;
  
	return (struct ast_node *) ast_node;
}


AST_node *new_ast_while_node(struct ast_node * condition, struct ast_node * while_branch){
	
	struct ast_while_node * ast_node = emalloc (sizeof (struct ast_while_node));

	ast_node->op = WHILE;

	ast_node->condition = condition;
	ast_node->while_branch = while_branch;
  
	return (struct ast_node *) ast_node;
}


AST_node *new_ast_for_node(Increment inc, struct ast_node *init_condition, struct ast_node *end_condition, struct ast_node *for_branch){
	
	struct ast_for_node * ast_node = emalloc (sizeof (struct ast_for_node));
	
	ast_node->op = FOR;
	
	ast_node->init_condition = init_condition;
	ast_node->end_condition = end_condition;
	ast_node->for_branch = for_branch;
	ast_node->inc = inc;
	
	return (struct ast_node *) ast_node;
}


AST_node *new_ast_leaf_node(Type_Struct *ts_value, char *name){
	
	struct ast_leaf_node * ast_node = emalloc (sizeof (struct ast_leaf_node));

	ast_node->op = LEAF;
	ast_node->name = name;

	ast_node->ts_value = ts_value;

	return (struct ast_node *) ast_node;
}

AST_node *new_ast_enum_leaf_node(Type_Struct *ts_value, char *name, Type_Struct *enumeration){
	
	struct ast_enum_leaf_node * ast_node = emalloc (sizeof (struct ast_enum_leaf_node));

	ast_node->op = ENUM_LEAF;
	ast_node->name = name;
	ast_node->enumeration = enumeration;

	ast_node->ts_value = ts_value;

	return (struct ast_node *) ast_node;
}


AST_node *new_ast_record_leaf_node(Type_Struct *ts_value, char *name, AST_node *record){
	
	struct ast_record_leaf_node * ast_node = emalloc (sizeof (struct ast_record_leaf_node));

	ast_node->op = REC_ID;
	ast_node->name = name;
	ast_node->record = record;

	ast_node->ts_value = ts_value;

	return (struct ast_node *) ast_node;
}


AST_node *new_ast_set_list_node(Type_Struct *ts_value){
	
	struct ast_set_list_node * ast_node = emalloc (sizeof (struct ast_leaf_node));

	ast_node->op = SET_LIST;

	ast_node->ts_value = ts_value;

	return (struct ast_node *) ast_node;	
}


AST_node *new_ast_io_node(Operation op, AST_node *params){
	
	struct ast_io_node * ast_node = emalloc (sizeof (struct ast_io_node));

	ast_node->op = op;

	ast_node->params = params;

	return (struct ast_node *) ast_node;	
}


void free_ast(struct ast_node *root){
	struct ast_while_node *wnode;
    struct ast_for_node *fnode;
	struct ast_if_node *ifnode;
	struct ast_func_proc_node *fpnode;
	struct ast_leaf_node *leaf;
	struct ast_enum_leaf_node *eleaf;
	struct ast_record_leaf_node *rleaf;
	struct ast_set_list_node *slist;
	struct ast_io_node *inode;
	
    switch (root->op){
        case NOT:
            free_ast(root->right);
            break;
        case MINUS:
            if (root->left != NULL){
                free_ast(root->left);
            }
            free_ast(root->right);
            break;
        case PLUS:
        case MULT:
		case DIVIDE:
		case DIV:
        case MOD:
        case AND:
        case OR:
        case EQU:
        case G_EQU:
        case GREATER:
        case L_EQU:
        case LESS:
        case ASSIGN:
        case IN:
        case STATEMENT:
        case BRACKS:
        case COMMA:
        case DOT:
        case DOTDOT:
        case WITH:
            free_ast(root->left);
            free_ast(root->right);
            break;
        case WHILE:
			wnode = (struct ast_while_node *)root;
            free_ast(wnode->condition);
            free_ast(wnode->while_branch);
            break;
        case FOR:
			fnode = (struct ast_for_node *)root;
            free_ast(fnode->init_condition);
            free_ast(fnode->end_condition);
			free_ast(fnode->for_branch);			
            break;
        case IF:
			ifnode = (struct ast_if_node *)root;
            free_ast(ifnode->condition);
            free_ast(ifnode->if_branch);
			if (ifnode->else_branch != NULL){
				free_ast(ifnode->else_branch);
			}
            break;
        case FUNC_PROC:
			fpnode = (struct ast_func_proc_node *)root;
            free_ast(fpnode->id_node);
            free_ast(fpnode->arguments);
			free_ast(fpnode->statements);
            break;
        case LEAF:
			leaf = (struct ast_leaf_node *)root;
            free(leaf->name);
            free_ts(leaf->ts_value);
            break;
        case ENUM_LEAF:
			eleaf = (struct ast_enum_leaf_node *)root;
            free(eleaf->name);
            free_ts(eleaf->ts_value);
			free_ts(eleaf->enumeration);
            break;
        case REC_ID:
			rleaf = (struct ast_record_leaf_node *)root;
            free(rleaf->name);
            free_ast(rleaf->record);
			free_ts(rleaf->ts_value);
            break;
        case SET_LIST:
			slist = (struct ast_set_list_node *)root;
            free_ts(slist->ts_value);
            break;
		case WRITE:
        case READ:
			inode = (struct ast_io_node *)root;
            free_ast(inode->params);
            break;
    }
    
    free(root);
}

void *emalloc(size_t size){
	
  void * pointer = malloc (size);

  if (!pointer)
	  printf("Error: malloc(%lu) failed!\n", size);
   // eprintf ("Error: malloc(%u) failed!\n", size);

  return pointer;
}


void ast_inorder_traversal_print(AST_node *root){
	static int pnl = 0;
	struct ast_leaf_node *leaf;
	struct ast_enum_leaf_node *eleaf;
	struct ast_if_node *ifnode;
	struct ast_while_node *wnode;
	struct ast_for_node *fnode;
	struct ast_record_leaf_node *rleaf;
	struct ast_set_list_node *slist;
	struct ast_io_node *inode;
	pnl++;
	
        //printf("++ %p, %d\n", root, root->op);
	switch (root->op){
		case LEAF:
			leaf = (struct ast_leaf_node *)root;
			if (leaf->name != NULL){
				printf("%s ", leaf->name);
			}else{
				switch(leaf->ts_value->type){
					case INTEGER:
						printf("%d ", leaf->ts_value->T_Union.data.intdata);
						break;
					case REAL:
						printf("%lf ", leaf->ts_value->T_Union.data.d_data);
						break;
					case CHARACTER:
						printf("%c ", leaf->ts_value->T_Union.data.c_data);
						break;
					case BOOLEAN:
						if (leaf->ts_value->T_Union.data.booldata == TRUE){
							printf("TRUE ");
						}else{
							printf("FALSE ");
						}
						break;
					case STRING:
						printf("\"%s\" ", leaf->ts_value->T_Union.data.str_data);
						break;
					default:
						break;
				}
			}
			break;
		case ENUM_LEAF:
			eleaf = (struct ast_enum_leaf_node *)root;
			printf("%d ",eleaf->ts_value->T_Union.data.intdata);
			break;
		case REC_ID:
			rleaf = (struct ast_record_leaf_node *)root;
			if (rleaf->name != NULL){
				printf("%s ", rleaf->name);
			}else{
				switch(rleaf->ts_value->type){
					case INTEGER:
						printf("%d ", rleaf->ts_value->T_Union.data.intdata);
						break;
					case REAL:
						printf("%lf ", rleaf->ts_value->T_Union.data.d_data);
						break;
					case CHARACTER:
						printf("%c ", rleaf->ts_value->T_Union.data.c_data);
						break;
					case BOOLEAN:
						if (rleaf->ts_value->T_Union.data.booldata == TRUE){
							printf("TRUE ");
						}else{
							printf("FALSE ");
						}
						break;
					default:
						break;
				}
			}
			break;
		case MULT:
			ast_inorder_traversal_print(root->left);
			printf("* ");
			ast_inorder_traversal_print(root->right);
			break;
		case PLUS:
			ast_inorder_traversal_print(root->left);
			printf("+ ");
			ast_inorder_traversal_print(root->right);
			break;
		case MINUS:
			if (root->left != NULL){
				ast_inorder_traversal_print(root->left);
			}
			printf("- ");
			ast_inorder_traversal_print(root->right);
			break;
		case NOT:
			printf("not ");
			ast_inorder_traversal_print(root->right);
		case DIVIDE:
			ast_inorder_traversal_print(root->left);
			printf("/ ");
			ast_inorder_traversal_print(root->right);
			break;
		case DIV:
			ast_inorder_traversal_print(root->left);
			printf("DIV ");
			ast_inorder_traversal_print(root->right);
			break;
		case MOD:
			ast_inorder_traversal_print(root->left);
			printf("MOD ");
			ast_inorder_traversal_print(root->right);
			break;
		case ASSIGN:
			ast_inorder_traversal_print(root->left);
			printf(":= ");
			ast_inorder_traversal_print(root->right);
			break;
		case EQU:
			ast_inorder_traversal_print(root->left);
			printf("= ");
			ast_inorder_traversal_print(root->right);
			break;
		case G_EQU:
			ast_inorder_traversal_print(root->left);
			printf(">= ");
			ast_inorder_traversal_print(root->right);
			break;
		case GREATER:
			ast_inorder_traversal_print(root->left);
			printf("> ");
			ast_inorder_traversal_print(root->right);
			break;
		case L_EQU:
			ast_inorder_traversal_print(root->left);
			printf("<= ");
			ast_inorder_traversal_print(root->right);
			break;
		case LESS:
			ast_inorder_traversal_print(root->left);
			printf("< ");
			ast_inorder_traversal_print(root->right);
			break;
		case STATEMENT:
			ast_inorder_traversal_print(root->left);
			printf("\n");
			ast_inorder_traversal_print(root->right);
			break;
		case BRACKS:
			ast_inorder_traversal_print(root->left);
			printf("[ ");
			ast_inorder_traversal_print(root->right);
			printf("] ");
			break;
		case IN:
			ast_inorder_traversal_print(root->left);
			printf("in ");
			ast_inorder_traversal_print(root->right);
			break;
		case DOT:
			ast_inorder_traversal_print(root->left);
			printf(". ");
			ast_inorder_traversal_print(root->right);
			break;
		case DOTDOT:
			ast_inorder_traversal_print(root->left);
			printf(".. ");
			ast_inorder_traversal_print(root->right);
			break;
		case COMMA:
			ast_inorder_traversal_print(root->left);
			printf(", ");
			ast_inorder_traversal_print(root->right);
			break;
		case FUNC_PROC:
			ast_inorder_traversal_print(root->left);
			printf("( ");
			if (root->right != NULL){
				ast_inorder_traversal_print(root->right);
			}
			printf(") ");
			break;
		case IF:
			ifnode = (struct ast_if_node *)root;
			
			printf("if ");
			ast_inorder_traversal_print(ifnode->condition);
			printf("then\n");
			ast_inorder_traversal_print(ifnode->if_branch);
			
			if(ifnode->else_branch != NULL){
				printf("\nelse\n");
				ast_inorder_traversal_print(ifnode->else_branch);
			}
			break;
		case WHILE:
			wnode = (struct ast_while_node *)root;
			
			printf("while ");
			ast_inorder_traversal_print(wnode->condition);
			printf("do\n");
			ast_inorder_traversal_print(wnode->while_branch);
			printf("\nend while");
			break;
		case FOR:
			fnode = (struct ast_for_node *)root;
			
			printf("for ");
			ast_inorder_traversal_print(fnode->init_condition);
			if (fnode->inc == TO){
				printf("to ");
			}else{
				printf("downto ");
			}
			ast_inorder_traversal_print(fnode->end_condition);
			printf("do\n");
			ast_inorder_traversal_print(fnode->for_branch);
			printf("\nend for");
			break;
		case WITH:
			printf("with ");
			ast_inorder_traversal_print(root->left);
			printf("do\n");
			ast_inorder_traversal_print(root->right);
			printf("\nend with\n");
			break;
		case SET_LIST:
			slist = (struct ast_set_list_node *)root;
			printf("[ ");
			ast_inorder_traversal_print(slist->ts_value->T_Union.set.ast_items);
			printf("] ");
			break;
		case READ:
			inode = (struct ast_io_node *)root;
			printf("read ( ");
			ast_inorder_traversal_print(inode->params);
			printf(") ");
			break;
		case WRITE:
			inode = (struct ast_io_node *)root;
			printf("write ( ");
			if (inode->params != NULL){
				ast_inorder_traversal_print(inode->params);
			}
			printf(") ");
			break;
		default:
			break;
	}
	
	pnl--;
	if(pnl == 0){
		printf("\n");
	}
}

