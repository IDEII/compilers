

#ifndef HASHTBL_H_INCLUDE_GUARD
#define HASHTBL_H_INCLUDE_GUARD

#include<stdlib.h>
#include"types.h"
#include "ast.h"
#include "sizes.h"

typedef size_t hash_size;
char buffer[101];


struct hashnode_s {
	char *key;
	int scope;
	Const isConst;
	struct hashnode_s *next;
	Type_Struct *ts;
};

typedef struct hashtbl {
	hash_size size;
	struct hashnode_s **nodes;
	hash_size (*hashfunc)(const char *);
} HASHTBL;


char *mystrdup(const char *s);
hash_size def_hashfunc(const char *key);

HASHTBL *ht_create(hash_size size, hash_size (*hashfunc)(const char *));
void ht_destroy(HASHTBL *hashtbl);
void ht_forced_exit(HASHTBL *hashtbl);

void ht_insert(HASHTBL *hashtbl, const char *key, Data data, Type type, int scope, Const isConst, int offset);
void ht_complex_insert(HASHTBL *hashtbl, const char *key, Type_Struct *ts, int scope);
List *ht_var_insert(List *list, Type_Struct *ts, int scope, Const isConst);

struct hashnode_s *ht_lookup(HASHTBL *hashtbl, int scope, char *key);
struct hashnode_s *ht_lookup_enums(HASHTBL *hashtbl, int scope, char *key);

int ht_remove(HASHTBL *hashtbl, const char *key,int scope);
void *ht_get(HASHTBL *hashtbl, int scope);

Type_Struct *ht_extract_ts(struct hashnode_s *node);
AST_TS_Union *ht_extract_ast_ts_union(struct hashnode_s *node, char *key);

#endif
