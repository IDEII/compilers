#include "types.h"
#include "sizes.h"

#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAX_STR_CONST 	200

#ifndef ASCII_CHARS
#define ASCII_CHARS

#define ASCII_size 62
char ASCII_chars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
					'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
					'0','1','2','3','4','5','6','7','8','9'};		//to be continued...

#endif


extern Const bisonIsConst; 

extern HASHTBL *hashtbl;
extern int scope;
extern char enum_limit1[MAX_STR_CONST];
extern char enum_limit2[MAX_STR_CONST];
extern int offset;
extern Record_List *stack;
extern For_List *fstack;


AST_TS_Union *ts_create_constant(Type type, Data data){
	AST_TS_Union *result;
	Type_Struct *ts;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	
	if(!(ts = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	ts->type = type;
	switch (type){
		case INTEGER: 
			ts->size = INT_SIZE;
			break;
		case CHARACTER:
			ts->size = CHAR_SIZE;
			break;
		case BOOLEAN:
			ts->size = BOOL_SIZE;
			break;
		case REAL:
			ts->size = REAL_SIZE;
			break;
		case STRING:
			ts->size = strlen(data.str_data) * CHAR_SIZE;
			break;
		default:
			break;
	}
	ts->T_Union.data = data;
	
	if ((bisonIsConst == CONST) || (type == STRING)){
		result->type_struct = ts;
	}else{
		result->ast_node = new_ast_leaf_node(ts, NULL);
	}
	
	return result;
}

Type_Struct *ts_create_subrange(Type_Struct *ts1, Type_Struct *ts2){
	
	Type_Struct *result;
	Type_Struct *start_value, *end_value;
	
	if(!(result=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	if(!(start_value=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	if(!(end_value=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = SUBRANGE;
	result->T_Union.subrange.subtype = ts1->type;
	
	if (ts1->type == ENUMERATION){
		List *list;
		
		start_value->type = INTEGER;
		end_value->type = INTEGER;
		list = ts1->T_Union.list;
		while( list != NULL){
			if (!strcmp(list->key, enum_limit1)){
				start_value->T_Union.data.intdata = list->value;
			}
			if (!strcmp(list->key, enum_limit2)){
				end_value->T_Union.data.intdata = list->value;
				break;
			}
			list = list->next;
		}
		
		result->T_Union.subrange.start_value = start_value;
		result->T_Union.subrange.end_value = end_value;
		result->T_Union.subrange.list = ts1->T_Union.list;
		//result->size = (end_value->T_Union.data.intdata - start_value->T_Union.data.intdata) * ENUM_SIZE;
	}else{
		start_value->type = ts1->type;
		start_value->T_Union.data = ts1->T_Union.data;
		
		end_value->type = ts2->type;
		end_value->T_Union.data = ts2->T_Union.data;
		
		result->T_Union.subrange.start_value = start_value;
		result->T_Union.subrange.end_value = end_value;
		
		result->T_Union.subrange.list = NULL;
		/*
		if(ts1->type == INTEGER){
			result->size = (end_value->T_Union.data.intdata - start_value->T_Union.data.intdata + 1) * INT_SIZE;
		}else if(ts1->type == CHARACTER){
			result->size = (end_value->T_Union.data.intdata - start_value->T_Union.data.intdata + 1) * CHAR_SIZE;
		}else{
			//BOOLEAN
			result->size = 2 * BOOL_SIZE;
		}*/
		result->size = (end_value->T_Union.data.intdata - start_value->T_Union.data.intdata + 1);
	}
		
	result->next = NULL;
	
	return result;
}

TS_Array *ts_array_add_dims(TS_Array *tsa, Type_Struct *node){
	Type_Struct *curr;
	
	for (curr = tsa->dims; curr->next != NULL; curr = curr->next);
	
	tsa->dimensions++;
	curr->next = node;
	
	return tsa;
}


Type_Struct *ts_create_array(TS_Array *tsa, Type_Struct *ts){
	Type_Struct *result;
	
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = ARRAY;
	result->T_Union.array.subtype = ts->type;
	result->T_Union.array.dimensions = tsa->dimensions;
	result->T_Union.array.dims = tsa->dims;
	result->T_Union.array.subts = ts;
	result->size = calc_size_offset_array(tsa->dims, ts);
	result->offset = -1;
	
	return result;
}

List *rl_create_node(char *name){
	
	List *result;
		
	if(!(result = malloc(sizeof(List)))) ht_forced_exit(hashtbl);
	result->key = name;
	result->value = -1;
	result->next = NULL;
	
	return result;
}

List *rl_add_node(List *rl, List *node){
	
	List *curr;
	
	curr = rl;
	while(curr->next != NULL){
		curr = curr->next;
	}
	curr->next = node;
	
	return rl;
}


List *rl_addType(List *rl, Type_Struct *ts){
	
	List *curr;
	Type_Struct *temp;
	
	curr = rl;
	while(curr != NULL){
		if(!(temp = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		*temp = *ts;
		curr->field = temp;
		
		curr = curr->next;
	}

	free(ts);
	return rl;
}

List *rl_addType_addPass(char *param, List *rl, Type_Struct *ts){
	
	List *curr;
	Type_Struct *temp;
	
	curr = rl;
	while(curr != NULL){
		if(!(temp = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		*temp = *ts;
		curr->field = temp;
		if(!strcmp(param, "VAR")) {
			curr->param = VAR;
		}else{
			curr->param = VALUE;
		}
		curr = curr->next;
	}
	
	free(ts);
	return rl;
}

List *rl_connect_lists(List *rl1, List *rl2){
	List *curr;
	
	curr = rl1;
	while(curr->next != NULL){
		curr = curr->next;
	}
	curr->next = rl2;
	
	return rl1;
}

Type_Struct *ts_create_record(List *rl){
	Type_Struct *result;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = RECORD;
	result->T_Union.list = rl;
	result->size = calc_size_offset_record(rl);
	result->offset = -1;
	
	return result;
}


Type_Struct *ts_create_standardType(Type type){
	
	Type_Struct *result;
	
	if(!(result=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = type;
	
	switch (result->type){
		case INTEGER: 
			result->size = INT_SIZE;
			break;
		case CHARACTER:
			result->size = CHAR_SIZE;
			break;
		case BOOLEAN:
			result->size = BOOL_SIZE;
			break;
		case REAL:
			result->size = REAL_SIZE;
			break;
		default:
			break;
	}
	
	return result;
}


Type_Struct *ts_create_set(Type_Struct *ts){
	int size;
	Type_Struct *result;
	List *curr;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = SET;
	result->T_Union.set.is_vector = TRUE;
	result->T_Union.set.subtype = ts;
	
	if (ts->type == CHARACTER){
		result->T_Union.set.size = ASCII_size;
		if(!(result->T_Union.set.vector = malloc(ASCII_size * BOOL_SIZE))) ht_forced_exit(hashtbl);
	}
	
	if (ts->type == BOOLEAN){
		result->T_Union.set.size = 2;
		if(!(result->T_Union.set.vector = malloc(2 * BOOL_SIZE))) ht_forced_exit(hashtbl);
	}
	
	if (ts->type == SUBRANGE){
		result->T_Union.set.size = ts->T_Union.subrange.end_value->T_Union.data.intdata - ts->T_Union.subrange.start_value->T_Union.data.intdata + 1;
		if(!(result->T_Union.set.vector = malloc(result->T_Union.set.size * BOOL_SIZE))) ht_forced_exit(hashtbl);
	}
	
	if (ts->type == ENUMERATION){
		size = 1;
		curr = ts->T_Union.list;
		while(curr->next != NULL){
			size++;
			curr = curr->next;
		}
		result->T_Union.set.size = size;
		if(!(result->T_Union.set.vector = malloc(size * BOOL_SIZE))) ht_forced_exit(hashtbl);
	}
	
	
	return result;
}

Type_Struct *ts_create_set_list(struct ast_node *tree){
	struct ast_node *tr;
	struct ast_leaf_node *leaf;
	Type_Struct *result, *temp;
	Type type, subtype;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->T_Union.set.is_vector = FALSE;
	
	tr = tree;
	if (tree->op == COMMA){
		tr = tr->right;
	}
	
	if (tr->op == LEAF){
		leaf = (struct ast_leaf_node *)tr;
		temp = leaf->ts_value;
		type = temp->type;
		
	}else if (tr->op == ENUM_LEAF){
		type = ENUMERATION;
	}else{
		if (tr->op == DOTDOT){
			type = ENUMERATION;
			
			if (tr->left->op == LEAF){
				leaf = (struct ast_leaf_node *)tr;
				subtype = leaf->ts_value->type;
			}else{
				subtype = tr->left->type;
			}
		}else{
			type = tr->type;
		}
	}
	
	switch (type){
		case BOOLEAN:
			if(!(result->T_Union.set.subtype = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
			result->T_Union.set.subtype->type = BOOLEAN;
			break;
		case CHARACTER:
			if(!(result->T_Union.set.subtype = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
			result->T_Union.set.subtype->type = CHARACTER;
			break;
		case SUBRANGE:
			if(!(result->T_Union.set.subtype = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
			result->T_Union.set.subtype->type = subtype;
			break;
		case ENUMERATION:
			if(!(result->T_Union.set.subtype = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
			result->T_Union.set.subtype->type = ENUMERATION;
			break;
		default:
			break;
	}
	
	result->T_Union.set.ast_items = tree;
	
	return result;
}

Type_Struct *ts_create_enum(List *el){
	
	Type_Struct *result;
	List *curr;
	int value = 0;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		
	curr = el;
	while(curr != NULL){
		curr->value = value;
		value++;
		curr = curr->next;
	}
	
	result->type = ENUMERATION;
	result->T_Union.list = el;
	
	return result;
	
}

Type_Struct *ts_create_limit(Data data, Type type){
	
	Type_Struct *result;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = type;
	result->T_Union.data = data;
	
	return result;
}

Type_Struct *ts_create_func(List *el, Type type){
	
	Type_Struct *result;
	List *curr;
	int no_args= 0;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = FUNCTION_NAME;
	result->T_Union.func_proc.list = el;
	result->T_Union.func_proc.returnType = type;
	
	curr = el;
	while(curr != NULL){
		no_args++;
		if (curr->field->type == INTEGER || curr->field->type == REAL || curr->field->type == CHARACTER || curr->field->type == BOOLEAN){
			ht_insert(hashtbl, curr->key, curr->field->T_Union.data, curr->field->type, scope + 1, NO_CONST, curr->offset);
		}else{
			ht_complex_insert(hashtbl, curr->key, curr->field, scope + 1);
		}
		curr = curr->next;
	}
	result->T_Union.func_proc.no_args = no_args;
	
	return result;
}

Type_Struct *ts_create_proc(List *el){
	
	Type_Struct *result;
	List *curr;
	int no_args= 0;
	
	if(!(result = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	result->type = PROCEDURE_NAME;
	result->T_Union.func_proc.list = el;
	result->T_Union.func_proc.returnType = NOTHING;
	
	curr = el;
	while(curr != NULL){
		no_args++;
		if (curr->field->type == INTEGER || curr->field->type == REAL || curr->field->type == CHARACTER || curr->field->type == BOOLEAN){
			ht_insert(hashtbl, curr->key, curr->field->T_Union.data, curr->field->type, scope + 1, NO_CONST, curr->offset);
		}else{
			ht_complex_insert(hashtbl, curr->key, curr->field, scope + 1);
		}
		curr = curr->next;
	}
	result->T_Union.func_proc.no_args = no_args;
	
	return result;
	
}

AST_TS_Union *ast_ts_create_set(Type_Struct *ts){
	AST_TS_Union *result;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	//if(!(result->ast_node = malloc(sizeof(struct ast_node *)))) ht_forced_exit(hashtbl);
	
	result->ast_node = new_ast_set_list_node(ts);
	
	return result;
}


int offset_calc(Type_Struct * ts){
	int temp, result;
	
	switch(ts->type){
		case INTEGER:
			if (offset % INT_SIZE != 0){
				offset += INT_SIZE - (offset % INT_SIZE);
			}
			result = offset;
			offset += INT_SIZE;
			break;
		case REAL:
			if (offset % REAL_SIZE != 0){
				offset += REAL_SIZE -(offset % REAL_SIZE);
			}
			result = offset;
			offset += REAL_SIZE;
			break;
		case CHARACTER:
			if (offset % CHAR_SIZE != 0){
				offset += CHAR_SIZE - (offset % CHAR_SIZE);
			}
			result = offset;
			offset += CHAR_SIZE;
			break;
		case BOOLEAN:
			if (offset % BOOL_SIZE != 0){
				offset += BOOL_SIZE - (offset % BOOL_SIZE);
			}
			result = offset;
			offset += BOOL_SIZE;
			break;
		case ARRAY:
			temp = offset_calc_array(ts->T_Union.array.subts);
			if (offset % temp != 0){
				offset += temp - (offset % temp);
			}
			result = offset;
			offset += ts->size;
			break;
		case ENUMERATION:
			if (offset % ENUM_SIZE != 0){
				offset += ENUM_SIZE - (offset % ENUM_SIZE);
			}
			result = offset;
			offset += ENUM_SIZE;
			break;
		case RECORD:
			temp = offset_calc_record(ts->T_Union.list);
			if (offset % temp != 0){
				offset += temp - (offset % temp);
			}
			result = offset;
			offset += ts->size;
			break;
		default:
			break;
	}
	
	return result;
}


int offset_calc_array(Type_Struct *ts){
	int result;
	
	switch(ts->type){
		case INTEGER:
			result = INT_SIZE;
			break;
		case REAL:
			result = REAL_SIZE;
			break;
		case CHARACTER:
			result = CHAR_SIZE;
			break;
		case BOOLEAN:
			result = BOOL_SIZE;
			break;
		case ARRAY:
			result = offset_calc_array(ts->T_Union.array.subts);
			break;
		case RECORD:
			result = offset_calc_record(ts->T_Union.list);
			break;
		case ENUMERATION:
			result = ENUM_SIZE;
			break;
		default:
			break;
	}
	
	return result;
}

int offset_calc_record(List *list){
	int temp;
	int result = 0;
	List *curr;
	
	for (curr = list; curr != NULL; curr = curr->next){
		switch(curr->field->type){
			case INTEGER:
				if (result < INT_SIZE){
					result = INT_SIZE;
				}
				break;
			case REAL:
				if (result < REAL_SIZE){
					result = REAL_SIZE;
				}
				break;
			case CHARACTER:
				if (result < CHAR_SIZE){
					result = CHAR_SIZE;
				}
				break;
			case BOOLEAN:
				if (result < BOOL_SIZE){
					result = BOOL_SIZE;
				}
				break;
			case ARRAY:
				temp = offset_calc_array(curr->field->T_Union.array.subts);
				if (result < temp){
					result = temp;
				}
				break;
			case RECORD:
				temp = offset_calc_record(curr->field->T_Union.list);
				if (result < temp){
					result = temp;
				}
				break;
			case ENUMERATION:
				if (result < ENUM_SIZE){
					result = ENUM_SIZE;
				}
				break;
			default:
				break;
		}
	}
	
	return result;
}

int calc_size_offset_array(Type_Struct *dims, Type_Struct *ts){
	int mult = 1;
	int temp1, temp2;
	Type_Struct *curr;
	
	curr = dims;
	while (curr != NULL){
		mult *= curr->size;
		curr = curr->next;
	}
	
	switch (ts->type){
		case INTEGER: 
			mult *= INT_SIZE;
			break;
		case CHARACTER:
			mult *= CHAR_SIZE;
			break;
		case BOOLEAN:
			mult *= BOOL_SIZE;
			break;
		case REAL:
			mult *= REAL_SIZE;
			break;
		case ENUMERATION:
			mult *= ENUM_SIZE;
			break;
		case RECORD:
			temp1 = offset_calc_record(ts->T_Union.list);
			temp2 = calc_size_offset_record(ts->T_Union.list);
			if (temp2 % temp1 == 0){
				mult *= temp2;
				ts->offset = temp2;
			}else{
				mult *= temp2 + temp1 - (temp2 % temp1);
				ts->offset = temp2  + temp1 - (temp2 % temp1);
			}
			break;
		case ARRAY:
			temp1 = offset_calc_array(ts->T_Union.array.subts);
			temp2 = calc_size_offset_array(ts->T_Union.array.dims, ts->T_Union.array.subts);
			if (temp2 % temp1 == 0){
				mult *= temp2;
				ts->offset = temp2;
			}else{
				mult *= temp2 + temp1 - (temp2 % temp1);
				ts->offset = temp2  + temp1 - (temp2 % temp1);
			}
			break;
		default:
			break;
	}
	
	return mult;
}

int calc_size_offset_record(List *list){
	int temp1, temp2;
	int result = 0;
	List *curr;
	
	
	for (curr = list; curr != NULL; curr = curr->next){
		switch(curr->field->type){
			case INTEGER:
				if (result % INT_SIZE != 0){
					result += INT_SIZE - (result % INT_SIZE);
				}
				curr->offset = INT_SIZE;
				result += curr->offset;
				break;
			case REAL:
				if (result % REAL_SIZE != 0){
					result += REAL_SIZE - (result % REAL_SIZE);
				}
				curr->offset = REAL_SIZE;
				result += curr->offset;
				break;
			case CHARACTER:
				if (result % CHAR_SIZE != 0){
					result += CHAR_SIZE - (result % CHAR_SIZE);
				}
				curr->offset = CHAR_SIZE;
				result += curr->offset;
				break;
			case BOOLEAN:
				if (result % BOOL_SIZE != 0){
					result += BOOL_SIZE - (result % BOOL_SIZE);
				}
				curr->offset = BOOL_SIZE;
				result += curr->offset;
				break;
			case ARRAY:
				temp1 = offset_calc_array(curr->field->T_Union.array.subts);
				temp2 = calc_size_offset_array(curr->field->T_Union.array.dims, curr->field->T_Union.array.subts);
				if (result % temp1 != 0){
					result += temp1 - (result % temp1);
				}
				curr->offset = temp2;
				result += curr->offset;
				break;
			case RECORD:
				temp1 = offset_calc_record(curr->field->T_Union.list);
				temp2 = calc_size_offset_record(curr->field->T_Union.list);
				if (result % temp1 != 0){
					result += temp1 - (result % temp1);
				}
				curr->offset = temp2;
				result += curr->offset;
				break;
			case ENUMERATION:
				if (result % ENUM_SIZE != 0){
					result += ENUM_SIZE - (result % ENUM_SIZE);
				}
				curr->offset = ENUM_SIZE;
				result += curr->offset;
				break;
			default:
				break;
		}
	}
	
	return result;
}

void push_with(List *list, struct ast_node *record){
	Record_List *node;
	
	if(!(node = (Record_List *) malloc(sizeof(Record_List)))) ht_forced_exit(hashtbl);
	
	node->list = list;
	node->record = record;
	
	if (stack == NULL){
		node->next = NULL;
	}else{
		node->next = stack;
	}
	stack = node;
	
}


void pop_with(void){
	Record_List *temp;
	
	temp = stack;
	stack = temp->next;
	
	free(temp);
}


AST_TS_Union *ast_ts_extract_id(struct hashtbl *hashtbl, int scope, char *key){
	AST_TS_Union *result;
	Record_List *curr;
	List *current;
	
	if (stack != NULL){
		for (curr = stack; curr != NULL; curr = curr->next){
			for (current = curr->list; current != NULL; current = current->next){
				if (!strcmp(current->key, key)){
					if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
					result->ast_node = new_ast_record_leaf_node(current->field, key, curr->record);
					return result;
				}
			}
		}
	}
	return ht_extract_ast_ts_union(ht_lookup(hashtbl, scope, key), key);
}


void push_for(struct ast_node *cond){
	For_List *node;
	
	if(!(node = (For_List *) malloc(sizeof(For_List)))) ht_forced_exit(hashtbl);
	
	node->cond = cond;
	
	if (stack == NULL){
		node->next = NULL;
	}else{
		node->next = fstack;
	}
	fstack = node;
	
}


void pop_for(void){
	For_List *temp;
	
	temp = fstack;
	fstack = temp->next;
	
	free(temp);
}

void free_ts(Type_Struct *ts){
	
	List *list_temp;

	switch (ts->type){
        case STRING:
            free(ts->T_Union.data.str_data);
            break;
		case ARRAY:
			free_ts(ts->T_Union.array.subts);
			free_ts(ts->T_Union.array.dims);
			break;
		case SUBRANGE:
			if (ts->T_Union.subrange.subtype == ENUMERATION){
				while(ts->T_Union.subrange.list != NULL){
					list_temp = ts->T_Union.subrange.list;
					ts->T_Union.subrange.list = ts->T_Union.subrange.list->next;
					free(list_temp->key);
                    free_ts(list_temp->field);
       				free(list_temp);
				} 		
			}
			free_ts(ts->T_Union.subrange.start_value);
			free_ts(ts->T_Union.subrange.end_value);
			break;
		case SET:
			free_ts(ts->T_Union.set.subtype);
			free_ast(ts->T_Union.set.ast_items);
			break;
		case RECORD:
			while(ts->T_Union.list != NULL){
				list_temp = ts->T_Union.list;
				ts->T_Union.list = ts->T_Union.list->next;
				free(list_temp->key);
                free_ts(list_temp->field);
   				free(list_temp);
			} 
			break;
		case ENUMERATION:
			while(ts->T_Union.list != NULL){
				list_temp = ts->T_Union.list;
				ts->T_Union.list = ts->T_Union.list->next;
				free(list_temp->key);
				free_ts(list_temp->field);
   				free(list_temp);
			} 
			break;
		case PROCEDURE_NAME:
			while(ts->T_Union.func_proc.list != NULL){
				list_temp = ts->T_Union.func_proc.list;
				ts->T_Union.func_proc.list = ts->T_Union.func_proc.list->next;
				free(list_temp->key);
				free_ts(list_temp->field);
   				free(list_temp);
			}
			while(ts->T_Union.func_proc.local != NULL){
				list_temp = ts->T_Union.func_proc.local;
				ts->T_Union.func_proc.local = ts->T_Union.func_proc.local->next;
				free(list_temp->key);
				free_ts(list_temp->field);
   				free(list_temp);
			} 
			free_ast(ts->T_Union.func_proc.astree);
			break;
		case FUNCTION_NAME:
			while(ts->T_Union.func_proc.list != NULL){
				list_temp = ts->T_Union.func_proc.list;
				ts->T_Union.func_proc.list = ts->T_Union.func_proc.list->next;
				free(list_temp->key);
				free_ts(list_temp->field);
   				free(list_temp);
			}
			while(ts->T_Union.func_proc.local != NULL){
				list_temp = ts->T_Union.func_proc.local;
				ts->T_Union.func_proc.local = ts->T_Union.func_proc.local->next;
				free(list_temp->key);
				free_ts(list_temp->field);
   				free(list_temp);
			}
			free_ast(ts->T_Union.func_proc.astree);
			break;
        case PROGRAM_NAME:
            //No arguments
            break;
		default:
			break;
	}
	free(ts);
}

