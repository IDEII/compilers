#include<string.h>
#include<stdio.h>
#include"hashtbl.h"

extern HASHTBL *hashtbl;
extern int sem_error;
extern void yyerror(char *message);
extern Const bisonIsConst;
extern int offset;

char *mystrdup(const char *s)
{
	char *b;
	if( !(b=malloc(strlen(s)+1)) ) return NULL;
	strcpy(b, s);
	return b;
}

hash_size def_hashfunc(const char *key)
{
	hash_size hash = 0;
	
	while(*key) hash += (unsigned char) * key++;

	return hash;
}

HASHTBL *ht_create(hash_size size, hash_size (*hashfunc)(const char *))
{
	HASHTBL *hashtbl;

	if(!(hashtbl=malloc(sizeof(HASHTBL)))) return NULL;

	if(!(hashtbl->nodes=calloc(size, sizeof(struct hashnode_s*)))) {
		free(hashtbl);
		return NULL;
	}

	hashtbl->size=size;

	if(hashfunc) hashtbl->hashfunc=hashfunc;
	else hashtbl->hashfunc=def_hashfunc;

	return hashtbl;
}

void ht_destroy(HASHTBL *hashtbl)
{
	hash_size n;
	struct hashnode_s *node, *oldnode;
	
	for(n=0; n<hashtbl->size; ++n) {
		node=hashtbl->nodes[n];
		while(node) {
			free(node->key);
			oldnode=node;
			node=node->next;
			free(oldnode);
		}
	}
	free(hashtbl->nodes);
	free(hashtbl);
}

void ht_forced_exit(HASHTBL *hashtbl){
	printf("Error in memory allocation in hashtable\n");
	ht_destroy(hashtbl);
	exit(-1);
}

void ht_insert(HASHTBL *hashtbl, const char *key, Data data, Type type, int scope, Const isConst, int offset)
{
	struct hashnode_s *node;
	Type_Struct *ts;
	hash_size hash = hashtbl->hashfunc(key)%hashtbl->size;
	
	node = hashtbl->nodes[hash];
	while(node) {
		if(!strcmp(node->key, key) && (node->scope == scope)) {
			sprintf(buffer, "There is already an entry %s with the same name\n", key);
			sem_error = 1;
			yyerror(buffer);
		}
		node=node->next;
	}

	if(!(node=malloc(sizeof(struct hashnode_s)))) ht_forced_exit(hashtbl);
	if(!(ts=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	if(!(node->key=mystrdup(key))) {
		free(node);
		ht_forced_exit(hashtbl);
	}
	node->ts = ts;
	node->scope = scope;
	node->ts->type = type;
	node->isConst = isConst;
	node->ts->offset = offset;
	node->ts->T_Union.data = data;
	switch(type){
		case INTEGER: 
			node->ts->size = INT_SIZE;
			break;
		case REAL: 
			node->ts->size = REAL_SIZE;
			break;
		case CHARACTER: 
			node->ts->size = CHAR_SIZE;
			break;
		case BOOLEAN: 
			node->ts->size = BOOL_SIZE;
			break;
		default:
			break;
	}
	
	node->next = hashtbl->nodes[hash];
	hashtbl->nodes[hash] = node;
	

	printf("\t\t\t\t\tHT_INSERT(): KEY = %s, HASH = %ld, SCOPE = %d, SIZE = %d, OFFSET = %d, IS_CONST = %d ", key, hash, scope, node->ts->size, offset, isConst);
	
	switch (type){
		case INTEGER: 
			printf("TYPE = INTEGER, DATA = %d\n", data.intdata);
			break;
		case REAL: 
			printf("TYPE = REAL, DATA = %lf\n", data.d_data);
			break;
		case CHARACTER: 
			printf("TYPE = CHARACTER, DATA = %c\n", data.c_data);
			break;
		case BOOLEAN: 
			printf("TYPE = BOOLEAN, DATA = %d\n", data.booldata);
			break;
		case STRING: 
			printf("TYPE = STRING, DATA = %s\n", data.str_data);
			break;
		default:
			printf("TYPE = %d, DATA = %s\n", type, data.str_data);
			break;
	} 

}

void ht_complex_insert(HASHTBL *hashtbl, const char *key, Type_Struct *ts, int scope){
	
	struct hashnode_s *node;
	hash_size hash = hashtbl->hashfunc(key)%hashtbl->size;
	
	/*
	switch (type){
		case INTEGER: 
			printf("TYPE = INTEGER, DATA = %d\n", data.intdata);
			break;
		case REAL: 
			printf("TYPE = REAL, DATA = %lf\n", data.d_data);
			break;
		case CHARACTER: 
			printf("TYPE = CHARACTER, DATA = %c\n", data.c_data);
			break;
		case BOOLEAN: 
			printf("TYPE = BOOLEAN, DATA = %d\n", data.booldata);
			break;
		default:
			printf("TYPE = %d, DATA = %s\n", type, data.str_data);
			break;
	} 
	*/
	
	node = hashtbl->nodes[hash];
	while(node) {
		if(!strcmp(node->key, key) && (node->scope == scope)) {
			sprintf(buffer, "There is already an entry %s with the same name\n", key);
			sem_error = 1;
			yyerror(buffer);
		}
		node=node->next;
	}

	if(!(node=malloc(sizeof(struct hashnode_s)))) ht_forced_exit(hashtbl);
	
	if(!(node->key=mystrdup(key))) {
		free(node);
		ht_forced_exit(hashtbl);
	}
	
	if(!(node->ts=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	*(node->ts) = *(ts);
	
	node->scope = scope;
	
	if (ts->type == ENUMERATION){
		node->isConst = CONST;
	}else{
		node->isConst = NO_CONST;
	}
	
	printf("\t\t\t\t\tHT_COMPLEX_INSERT(): KEY = %s, HASH = %ld, SCOPE = %d, SIZE = %d, OFFSET = %d, IS_CONST = %d, TYPE = %d \n", node->key, hash, node->scope, node->ts->size, node->ts->offset, node->isConst, node->ts->type);
	
	node->next = hashtbl->nodes[hash];
	hashtbl->nodes[hash] = node;

}


List *ht_var_insert(List *list, Type_Struct *ts, int scope, Const isConst){
	Data data;
	List *curr = list;
	
	while( curr != NULL){
		ts->offset = offset_calc(ts);
		if (ts->type == CHARACTER){
			data.c_data = '1';
			ht_insert(hashtbl, curr->key, data, ts->type, scope, isConst, ts->offset);
		}else if (ts->type == REAL || ts->type == CHARACTER || ts->type == BOOLEAN || ts->type == INTEGER){
			data.intdata = 0;
			ht_insert(hashtbl, curr->key, data, ts->type, scope, isConst, ts->offset);
		}else{
			ht_complex_insert(hashtbl, curr->key, ts, scope);
		}
		
		curr->offset = ts->offset;
		curr = curr->next;
	}
	
	return list;
}


int ht_remove(HASHTBL *hashtbl, const char *key, int scope){
	struct hashnode_s *node, *prevnode = NULL;
	hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;

	node=hashtbl->nodes[hash];
	while(node) {
		if((!strcmp(node->key, key)) && (node->scope == scope)) {
			free(node->key);
			/*if (!ht_free_ts(node->ts)){
				return -2;
			}*/
			if(prevnode) prevnode->next=node->next;
			else hashtbl->nodes[hash]=node->next;
			free(node);
			return 0;
		}
		prevnode=node;
		node=node->next;
	}

	return -1;
}

struct hashnode_s *ht_lookup(HASHTBL *hashtbl, int scope, char *key){

	int s;
	struct hashnode_s *node;
	hash_size hash = hashtbl->hashfunc(key)%hashtbl->size;
	
	
	
	for(s = scope; s >= 0; s--){
		node = hashtbl->nodes[hash];
		while(node) {
			if(!strcmp(node->key, key) && (node->scope == s)) {
				printf("\t\t\t\t\tKey %s found in scope %d\n", key, s);
				return node;
			}
			node=node->next;
		}
	}
	
	printf("\t\t\t\t\tThere is no entry with the name: %s\n\t\t\t\t\tChecking for enumeration value...\n", key);
	
	node = ht_lookup_enums(hashtbl, scope, key);
	if(node == NULL){
		sem_error = 1;
		yyerror("There is no such entry into the symbol table");
		return NULL;
	}
	
	return node;
}

struct hashnode_s *ht_lookup_enums(HASHTBL *hashtbl, int scope, char *key){

	int s;
	hash_size i;
	List *curr;
	struct hashnode_s *node;
	
	for(s = scope; s >= 0; s--){
		for(i = 0; i < hashtbl->size; i++){
			node = hashtbl->nodes[i];
			while(node) {
				if (node->ts->type == ENUMERATION && node->scope == s){
					curr = node->ts->T_Union.list;
					while(curr != NULL){
						if(!strcmp(curr->key, key)) {
							printf("\t\t\t\t\tEnum: Value %s found in scope %d in key %s\n", key, s, node->key);
							return node;
						}
						curr = curr->next;
					}
				}
				node=node->next;
			}
		}
	}
	
	printf("\t\t\t\t\tThere is no enumeration with value: %s\n", key);
	return NULL;
}

Type_Struct *ht_extract_ts(struct hashnode_s *node){
	
	Type_Struct *result;
	
	if(!(result=malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
	
	*result = *(node->ts);
	result->next = NULL;
	
	return result;
}

AST_TS_Union *ht_extract_ast_ts_union(struct hashnode_s *node, char *key){
	AST_TS_Union *result;
	Type_Struct *ts, *ets;
	List *curr;
	
	if(!(result = malloc(sizeof(AST_TS_Union)))) ht_forced_exit(hashtbl);
	if(bisonIsConst == CONST){
		if(!(result->type_struct = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		
		result->type_struct = node->ts;
		result->type_struct->next = NULL;
	}else{
		if(!(ts = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
		
		if (!strcmp(node->key, key)){
			*ts = *(node->ts);
			result->ast_node = new_ast_leaf_node(ts, mystrdup(node->key));
		}else{
			if(!(ets = malloc(sizeof(Type_Struct)))) ht_forced_exit(hashtbl);
			*ets = *(node->ts);
			 
			for (curr = node->ts->T_Union.list; strcmp(key, curr->key); curr = curr->next);
			ts->type = INTEGER;
			ts->T_Union.data.intdata = curr->value;
			
			result->ast_node = new_ast_enum_leaf_node(ts, mystrdup(curr->key), ets);
		}
	}
	
	return result;
}


void *ht_get(HASHTBL *hashtbl, int scope){
	int n, rem;
	struct hashnode_s *node, *oldnode;
	
	for(n=0; n<hashtbl->size; ++n) {
		node=hashtbl->nodes[n];
		while(node) {
			if(node->scope == scope) {
				printf("\t\t\t\t\tHASHTBL_GET():\tSCOPE = %d, KEY = %s, TYPE = %d IS_CONST = %d\n", node->scope, node->key, node->ts->type, node->isConst);
				oldnode = node;
				node=node->next;
				rem = ht_remove(hashtbl, oldnode->key, scope);
			}else
				node=node->next;
		}
	}
	
	if (rem == -1)
		printf("\t\t\t\t\tHASHTBL_GET():\tThere are no elements in the hash table with this scope!\n\t\tSCOPE = %d\n", scope);
	
	return NULL;
}



