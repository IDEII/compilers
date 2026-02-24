#include "hashtbl.h"

#ifndef types
#define types

struct ast_node;
struct hashtbl;

typedef enum type_enum {NOTHING,INTEGER,REAL,CHARACTER,STRING,BOOLEAN,ARRAY,RECORD,ENUMERATION,SUBRANGE,SET,PROGRAM_NAME,FUNCTION_NAME,PROCEDURE_NAME} Type;
typedef enum param_enum {VALUE,VAR} Param;
typedef enum const_enum {NO_CONST,CONST} Const;
typedef enum boolean {FALSE,TRUE} Boolean;
typedef enum operation_enum {PLUS,MINUS,MULT,DIVIDE,DIV,MOD,AND,OR,NOT,
							 EQU,G_EQU,GREATER,L_EQU,LESS,
							 IN,WHILE,FOR,IF,FUNC_PROC,ASSIGN,LEAF,ENUM_LEAF,STATEMENT,BRACKS,COMMA,DOT,DOTDOT,WITH,REC_ID,SET_LIST,
							 READ,WRITE} Operation; 
typedef enum incr_enum {DOWNTO, TO} Increment;


typedef union Data_union{
    int intdata;
	Boolean booldata;
    double d_data;
    char c_data;
    char *str_data;
}Data;

typedef struct T_data{
	Type type;
	Data data;
}T_Data;

struct Type_Struct;

typedef struct list{
	char *key;
	Param param;
	int value;
	int offset;
	struct Type_Struct *field;
	struct list *next;
}List;


typedef struct Type_Struct{
	
	Type type;
	int size;						//megethos typou
	int offset;
	
	union {
		/***** CONSTANT *****/
		Data data;
		
		
		/***** ARRAY ******/
		struct{
			Type subtype;
			struct Type_Struct *subts;
			int dimensions;
			struct Type_Struct *dims;
		}array;
		
		
		/***** SUBRANGE *****/
		struct{
			Type subtype;
			struct Type_Struct *start_value;             /******can take multiple type values***************/
			struct Type_Struct *end_value;               /******can take multiple type values***************/
			List *list;										//in case we have enumeration
		}subrange;
				
		
		/***** List: RECORD/ENUMERATION *****/
		List *list;
		
		
		/***** SET *****/
		struct{
			Boolean is_vector;
			int size;
			struct Type_Struct *subtype;
			Boolean *vector;
			struct ast_node *ast_items;
		}set;
		
		
		
		/***** FUNCTION/PROCEDURE *****/
		struct{
			int no_args;  		//arguments of function/procedure
			Type returnType; 	//return value if function/procedure
			List *list;		//list of function/procedure arguments
			List *local;		//list of local variables
			struct ast_node *astree;
			
		}func_proc;   
            
	}T_Union;
	
	struct Type_Struct *next;
}Type_Struct;

typedef struct TS_Array{
	int dimensions;
	Type_Struct *dims;
}TS_Array;

typedef union{
	struct ast_node *ast_node;
	Type_Struct *type_struct;
}AST_TS_Union;

typedef struct Iter_str{
	AST_TS_Union *node1;
	struct ast_node *node2;
	Increment inc;
}Iter_str;

typedef struct Record_List{
	List *list;
	struct ast_node *record;
	struct Record_List *next;
}Record_List;

typedef struct For_List{
	char *key;
	struct ast_node *cond;
	struct For_List *next;
}For_List;


AST_TS_Union *ts_create_constant(Type type, Data data);

Type_Struct *ts_create_subrange(Type_Struct *ts1, Type_Struct *ts2);

TS_Array *ts_array_add_dims(TS_Array *tsa, Type_Struct *node);
Type_Struct *ts_create_array(TS_Array *tsa, Type_Struct *ts);

List *rl_create_node(char *name);
List *rl_add_node(List *rl, List *node);
Type_Struct *ts_create_standardType(Type type);
List *rl_addType(List *rl, Type_Struct *ts);
List *rl_addType_addPass(char *param, List *rl, Type_Struct *ts);
List *rl_connect_lists(List *rl1, List *rl2);
Type_Struct *ts_create_record(List *rl);
Type_Struct *ts_create_set(Type_Struct *ts);
Type_Struct *ts_create_set_list(struct ast_node *tree);
Type_Struct *ts_create_enum(List *el);
Type_Struct *ts_create_limit(Data data, Type type);
Type_Struct *ts_create_func(List *el, Type type);
Type_Struct *ts_create_proc(List *el);

AST_TS_Union *ast_ts_create_set(Type_Struct *ts);

int offset_calc(Type_Struct * ts);
int offset_calc_array(Type_Struct *ts);
int offset_calc_record(List *list);

int calc_size_offset_array(Type_Struct *dims, Type_Struct *ts);
int calc_size_offset_record(List *list);

void push_with(List *list, struct ast_node *record);
void pop_with(void);
AST_TS_Union *ast_ts_extract_id(struct hashtbl *hashtbl, int scope, char *key);

void push_for(struct ast_node *cond);
void pop_for(void);

void free_ast(struct ast_node *root);
void free_ts(Type_Struct *ts);
#endif
