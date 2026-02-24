/*Homework  Compilers
	Christodoulou Vasilis 1834
	Kostakis Nikolaos 1775
	Papadopoulou Chryssa 1743
	Papadopoulou Iliana 1742
*/
%{
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtbl.h"
#include "types.h"
#include "check_types.h"
#include "ast.h"
#define MAX_STR_CONST 	200
#define MAX_LINE_SIZE	200
#define MAX_ERRORS		1

int i;
char enum_limit1[MAX_STR_CONST];
char enum_limit2[MAX_STR_CONST];
Data temp_d, data;
HASHTBL *hashtbl;
Const bisonIsConst = NO_CONST;
AST_TS_Union *ast_ts;
Record_List *stack = NULL;
For_List *fstack = NULL;
Type_Struct *ts;

int scope = 0;
int sem_error = 0;		//Check for semantic error. Used in yyerror.

extern int yylex();
extern char *yytext;
extern FILE *yyin;

// extern void yyterminate(), error_string();
void yyerror(char *message);

/*Pointer we use to access the file and store the line which we found an error*/
FILE *yyin1;
/*Flag = 0 when yyin1 find eof. Flag = 1 otherwise*/
int flag=1;
/*error_count :# of errors encountered in flex*/
int error_count = 0;
/*lineno: # of lines in the program*/
extern int lineno;
char linestream[MAX_LINE_SIZE];
/*in string_buf we store the strings encountered in the program*/ 
extern char string_buf[MAX_STR_CONST];
extern char *string_buf_ptr;

int offset; //
%}


%union{
	int intval;
	Boolean boolval;
	double doubleval;
	char cval;
	char *strval;
	Type typeval;
	T_Data tdval;
	Type_Struct *tsval;
	List *rlval;
	AST_TS_Union *ast_tsval;
	AST_node *astval;
	TS_Array *tsaval;
	Iter_str iterval;
}
 
%start program
%token <strval> T_PROGRAM T_CONST T_TYPE T_ARRAY T_SET T_OF T_RECORD T_VAR T_FORWARD T_FUNCTION T_PROCEDURE T_INTEGER T_REAL 
%token <strval> T_BOOLEAN T_CHAR T_BEGIN T_END T_IF T_THEN T_ELSE T_WHILE T_DO T_FOR T_DOWNTO T_TO T_WITH T_READ T_WRITE
%token <strval> T_OROP T_MULT T_DIVIDE T_DIV T_MOD T_AND T_NOTOP T_INOP T_ID T_RPAREN T_SEMI T_PLUS T_MINUS T_LPAREN
%token <strval> T_GREATER T_GREATER_EQUAL T_LESS T_LESS_EQUAL
%token <strval> T_DOT T_COMMA T_EQU T_COLON T_LBRACK T_RBRACK T_ASSIGN T_DOTDOT T_EOF T_STRING
%token <doubleval> T_RCONST 
%token <intval> T_ICONST 
%token <boolval> T_BCONST

%token <cval> T_CCONST
%type <strval> program header constdefs constant_defs
%type <strval> type_defs typedefs
%type <strval> subprogram subprograms
%type <strval> pass  

%type <tsval> type_def limits limit typename sub_header setexpression
%type <tsaval> dims 
%type <rlval> declarations identifiers vardefs variable_defs fields field parameter_list formal_parameters

%type <typeval> standard_type

%type <ast_tsval> constant expression expression1 expression2 expression3 variable
%type <astval> expressions comp_statement statement statements
%type <astval> assignment if_statement if_tail while_statement for_statement with_statement subprogram_call
%type <astval> elexpression elexpressions 
%type <astval> io_statement read_item write_item read_list write_list 

%type <iterval> iter_space

%nonassoc IFX
%nonassoc T_ELSE
%nonassoc EXP1
%nonassoc EXP2
%nonassoc EXP3

%left T_OROP T_PLUS T_MINUS
%left T_MULT T_DIVIDE T_DIV T_MOD T_AND
%left T_GREATER T_GREATER_EQUAL T_LESS T_LESS_EQUAL T_EQU T_INOP



%%

program			 :header 														{scope++; offset = 0;}
				  declarations subprograms comp_statement T_DOT 				{ast_inorder_traversal_print($5); 
																				 ht_get(hashtbl, scope); scope--; 
																				 printf("\t\tprogram -> header declarations subprograms comp_statement DOT\n"); free_ast($5);
																				 printf("--- NUMBER OF ERRORS = %d ---\n", error_count);}
				 ;
	   
header			 :T_PROGRAM T_ID T_SEMI											{printf("\t\theader -> T_PROGRAM T_ID T_SEMI\n");}
				 |T_PROGRAM T_ID error											{printf("\theader -> T_PROGRAM T_ID (T_SEMI missing)\n");}
				 ;
	  
declarations	 :																{bisonIsConst = CONST;}
				  constdefs														{bisonIsConst = NO_CONST;}
				  typedefs vardefs												{$$ = $5;
																				 printf("\t\tdeclarations -> constdefs typedefs vardefs\n");}
				 ;
		    
constdefs:		 T_CONST constant_defs T_SEMI									{printf("\t\tconstdefs -> T_CONST constant_defs T_SEMI\n");}
				 |T_CONST constant_defs error									{printf("\t\tconstdefs -> T_CONST constant_defs (T_SEMI missing)\n");}
				 |/* ε */														{printf("\t\tconstdefs -> ε\n");}
				 ;
		 
constant_defs	 :constant_defs T_SEMI T_ID T_EQU expression					{ht_insert(hashtbl, $3, $5->type_struct->T_Union.data, $5->type_struct->type, scope, CONST, -1);
																				 printf("\t\tconstant_defs -> constant_defs SEMI ID EQU expression\n");}
				 |constant_defs error T_ID T_EQU expression						{ht_insert(hashtbl, $3, $5->type_struct->T_Union.data, $5->type_struct->type, scope, CONST, -1);
																				 printf("\t\tconstant_defs -> constant_defs (T_SEMI missing) ID EQU expression\n");}
				 |T_ID T_EQU expression											{ht_insert(hashtbl, $1, $3->type_struct->T_Union.data, $3->type_struct->type, scope, CONST, -1); 
																				 printf("\t\tconstant_defs -> ID EQU expression\n");}
				 ;

expression		 :expression T_GREATER expression1			%prec EXP1			{$$ = sem_check_RELOP($1,GREATER, $3);
																				 printf("\t\texpression -> expression T_GREATER expression\n");}
				 |expression T_GREATER_EQUAL expression1	%prec EXP1			{$$ = sem_check_RELOP($1,G_EQU, $3);
																				 printf("\t\texpression -> expression T_GREATER_EQUAL expression\n");} 
				 |expression T_LESS expression1				%prec EXP1			{$$ = sem_check_RELOP($1,LESS, $3);
																				 printf("\t\texpression -> expression T_LESS expression\n");} 
				 |expression T_LESS_EQUAL expression1		%prec EXP1			{$$ = sem_check_RELOP($1,L_EQU, $3);
																				 printf("\t\texpression -> expression T_LESS_EQUAL expression\n");} 
				 |expression T_EQU expression1				%prec EXP1			{$$ = sem_check_RELOP($1,EQU, $3);
																				 printf("\t\texpression -> expression T_EQU expression\n");}
				 |expression T_INOP expression1				%prec EXP1			{$$ = sem_check_INOP($1, $3);
																				 printf("\t\texpression -> expression T_INOP expression\n");}
				 |expression1								%prec EXP1			{printf("\t\texpression -> expression1\n");}
				 ;
		
expression1		 :expression1 T_PLUS expression2		%prec EXP2				{$$ = sem_check_MULT_PLUS_MINUS($1,PLUS, $3);
																				 printf("\t\texpression1 -> expression T_PLUS expression\n");}
				 |expression1 T_MINUS expression2		%prec EXP2				{$$ = sem_check_MULT_PLUS_MINUS($1,MINUS, $3);
																				 printf("\t\texpression1 -> expression T_MINUS expression\n");}
				 |expression1 T_OROP expression2		%prec EXP2				{$$ = sem_check_AND_OR($1,OR, $3);
																				 printf("\t\texpression1 -> expression T_OROP expression\n");}
				 |expression2							%prec EXP2				{printf("\t\texpression1 -> expression2\n");}
				 ;
			
expression2 	 :expression2 T_MULT expression3								{$$ = sem_check_MULT_PLUS_MINUS($1,MULT, $3);
																				 printf("\t\texpression2 -> expression T_MULT expression\n");}
				 |expression2 T_DIVIDE expression3								{$$ = sem_check_DIVIDE($1, $3);
																				 printf("\t\texpression2 -> expression T_DIVIDE expression\n");}
				 |expression2 T_DIV expression3									{$$ = sem_check_DIV_MOD($1,DIV, $3);
																				 printf("\t\texpression2 -> expression T_DIV expression\n");}
				 |expression2 T_MOD expression3									{$$ = sem_check_DIV_MOD($1,MOD, $3);
																				 printf("\t\texpression2 -> expression T_MOD expression\n");}
				 |expression2 T_AND expression3									{$$ = sem_check_AND_OR($1,AND, $3);
																				 printf("\t\texpression2 -> expression T_AND expression\n");}
				 |expression3													{printf("\t\texpression2 -> expression3\n");}
				 ;
			
expression3   	 :T_PLUS expression						%prec EXP3				{$$ = sem_check_exp_SIGN($2,PLUS);
																				 printf("\t\texpression3 -> T_PLUS expression\n");}
				 |T_MINUS expression					%prec EXP3				{$$ = sem_check_exp_SIGN($2,MINUS);
																				 printf("\t\texpression3 -> T_MINUS expression\n");}
				 |T_NOTOP expression					%prec EXP3				{$$ = sem_check_NOT($2);
																				 printf("\t\texpression3 -> T_NOTOP expression\n");}
				 |variable														{printf("\t\texpression3 -> variable\n");}
				 |T_ID T_LPAREN expressions T_RPAREN							{$$ = sem_check_func($1,$3);
																				 printf("\t\texpression3 -> T_LPAREN expressions T_RPAREN\n");}
				 |constant														{printf("\t\texpression3 -> constant\n");}
				 |T_LPAREN expression T_RPAREN									{$$ = $2;
																				 printf("\t\texpression3 -> T_LPAREN expression T_RPAREN\n");}
				 |setexpression													{$$ = ast_ts_create_set($1);
																				 printf("\t\texpression3 -> setexpression\n");}
				 ;
			
variable		 :T_ID															{$$ = ast_ts_extract_id(hashtbl, scope, $1);
																				 printf("\t\tvariable -> T_ID\n");}
				 |variable T_DOT T_ID											{$$ = sem_check_extract_field($1, $3);
																				 printf("\t\tvariable -> variable T_DOT T_ID\n");}
				 |variable T_LBRACK expressions T_RBRACK 						{$$ = sem_check_array($1,$3);
																				 printf("\t\tvariable -> variable T_LBRACK expressions T_RBRACK\n");}
				 ;
				 
expressions		 :expressions T_COMMA expression								{$$ = new_ast_node(COMMA, NOTHING, $1, $3->ast_node);
																				 printf("\t\texpressions -> expressions T_COMMA expression\n");}
				 |expressions T_COMMA error										{printf("\t\texpressions -> expressions T_COMMA (expression missing)\n");}
				 |expression													{$$ = $1->ast_node;printf("\t\texpressions -> expression\n");}
				 ;
		   
constant		 :T_ICONST														{data.intdata = $1; $$ = ts_create_constant(INTEGER, data); 
																				 printf("\t\tconstant -> T_ICONSTn");}
				 |T_RCONST														{data.d_data = $1; $$ = ts_create_constant(REAL, data);
																				printf("\t\tconstant -> T_RCONST\n");}
				 |T_BCONST														{data.booldata = $1; $$ = ts_create_constant(BOOLEAN, data);
																				 printf("\t\tconstant -> T_BCONST\n");}
				 |T_CCONST														{data.c_data = $1; $$ = ts_create_constant(CHARACTER, data);
																				printf("\t\tconstant -> T_CCONST\n");}
				 ;
		
setexpression	 :T_LBRACK elexpressions T_RBRACK								{$$ = ts_create_set_list($2);
																				 printf("\t\tsetexpression -> T_LBRACK elexpressions T_RBRACK\n");}
				 |T_LBRACK T_RBRACK												{$$ = ts_create_set_list(NULL);
																				 printf("\t\tsetexpression -> T_LBRACK T_RBRACK\n");}
				 ;
			 
elexpressions	 :elexpressions T_COMMA elexpression							{$$ = sem_check_add_set($1,$3);
																				 printf("\t\telexpressions -> elexpressions T_COMMA elexpression\n");}
				 |elexpression													{printf("\t\telexpressions -> elexpression\n");}
				 ;
			 
elexpression	 :expression T_DOTDOT expression								{sem_check_ast_subrange($1->ast_node, $3->ast_node);
																				 $$ = new_ast_node(DOTDOT, ENUMERATION, $1->ast_node, $3->ast_node);
																				 printf("\t\telexpression -> expression T_DOTDOT expression\n");}
				 |expression													{$$ = $1->ast_node;
																				 printf("\t\telexpression -> expression\n");}
				 ;
			
typedefs		 :T_TYPE type_defs T_SEMI										{printf("\t\ttypedefs -> T_TYPE type_defs T_SEMI\n");}
				 |T_TYPE type_defs error										{printf("\t\ttypedefs -> T_TYPE type_defs (T_SEMI missing)\n");}
				 |/* ε */														{printf("\t\ttypedefs -> ε\n");}
				 ;
		
type_defs		 :type_defs T_SEMI T_ID	T_EQU type_def							{ht_complex_insert(hashtbl, $3, $5, scope);
																				 printf("\t\ttype_defs -> type_defs T_SEMI T_ID T_EQU type_def\n");}
				 |type_defs error											 	{printf("\tT_SEMI missing\n");}
				  T_ID T_EQU type_def											{printf("\t\ttype_defs -> type_defs T_ID T_EQU type_def\n");}
				 |T_ID T_EQU type_def											{ht_complex_insert(hashtbl, $1, $3, scope);
																				 printf("\t\ttype_defs -> T_ID T_EQU type_def\n");}
				 ;
		 
type_def		 :T_ARRAY T_LBRACK dims T_RBRACK T_OF typename					{$$ = ts_create_array($3, $6);
																				 printf("\t\ttype_def -> T_ARRAY T_LBRACK dims T_RBRACK T_OF typename\n");}
				 |T_ARRAY T_LBRACK dims error T_OF typename						{$$ = ts_create_array($3, $6);
																				 printf("\t\ttype_def -> T_ARRAY T_LBRACK dims (T_LBRACK missing) T_OF typename\n");}
				 |T_SET T_OF typename											{sem_check_set($3); $$ = ts_create_set($3);
																				 printf("\t\ttype_def -> T_SET T_OF typename\n");}
				 |T_RECORD fields T_END											{$$ = ts_create_record($2);
																				 printf("\t\ttype_def -> T_RECORD fields T_END\n");}
				 |T_LPAREN identifiers T_RPAREN									{$$ = ts_create_enum($2);
																				 printf("\t\ttype_def -> T_LPAREN identifiers T_RPAREN\n");}
				 |limit T_DOTDOT limit											{sem_check_subrange($1, $3); $$ = ts_create_subrange($1, $3);
																				 for(i=0; i < MAX_STR_CONST; i++){enum_limit1[i] = '\0'; enum_limit2[i] = '\0';}
																				 printf("\t\ttype_def -> limit T_DOTDOT limit\n");}
				 ;
		
dims			 :dims T_COMMA limits											{$$ = ts_array_add_dims($1, $3); 
																				 printf("\t\tdims -> dims T_COMMA limits\n");}
				 |limits														{$$->dimensions = 1; $$->dims = $1;
																				 printf("\t\tdims -> limits\n");}	
				 ;
	
limits			 :limit T_DOTDOT limit											{sem_check_subrange($1, $3); $$ = ts_create_subrange($1, $3);
																				 for(i=0; i < MAX_STR_CONST; i++){enum_limit1[i] = '\0'; enum_limit2[i] = '\0';}
																				 printf("\t\tlimits -> limit T_DOTDOT limit\n");}
				 |T_ID															{$$ = sem_check_subrange_ts(ht_extract_ts(ht_lookup(hashtbl, scope, $1)));
																				 printf("\t\tlimits -> T_ID\n");}
				 ;
	  
limit			 :T_PLUS T_ICONST												{temp_d.intdata = $2; $$ = ts_create_limit(temp_d, INTEGER);
																				 printf("\t\tlimit -> T_PLUS T_ICONST\n");}
				 |T_MINUS T_ICONST												{temp_d.intdata = $2; $$ = ts_create_limit(temp_d, INTEGER); 
																				 printf("\t\tlimit -> T_MINUS T_ICONST\n");}
				 |T_PLUS T_ID													{$$ = sem_check_SIGN(sem_check_CONST_ID(ht_lookup(hashtbl, scope, $2)), '+');
																				 printf("\t\tlimit -> T_PLUS T_ID\n");}
				 |T_MINUS T_ID													{$$ = sem_check_SIGN(sem_check_CONST_ID(ht_lookup(hashtbl, scope, $2)), '-');
																				 printf("\t\tlimit -> T_MINUS T_ID\n");}
				 |T_ICONST														{temp_d.intdata = $1; $$ = ts_create_limit(temp_d, INTEGER); 
																				 printf("\t\tlimit -> T_ICONST\n");}
				 |T_CCONST														{temp_d.c_data = $1; $$ = ts_create_limit(temp_d, CHARACTER);
																				printf("\t\tlimit -> T_CCONST\n");}
				 |T_BCONST														{temp_d.booldata = $1; $$ = ts_create_limit(temp_d, BOOLEAN);
																				 printf("\t\tlimit -> T_BCONST\n");}
				 |T_ID															{if(enum_limit1[0] == '\0'){ strcpy(enum_limit1, $1);}else{
																				  strcpy(enum_limit2, $1);}
																				 $$ = sem_check_CONST_ID(ht_lookup(hashtbl, scope, $1));
																				 printf("\t\tlimit -> T_ID\n");}
				 ;
	 
typename		 :standard_type													{$$ = ts_create_standardType($1);
																				 printf("\t\ttypename -> standard_type\n");}
				 |T_ID															{$$ = ht_extract_ts(ht_lookup(hashtbl, scope, $1));
																				 printf("\t\ttypename -> T_ID\n");}
				 ;
		
standard_type	 :T_INTEGER 													{$$ = INTEGER;
																				 printf("\t\tstandard_type -> T_INTEGER\n");}
				 |T_REAL														{$$ = REAL;
																				 printf("\t\tstandard_type -> T_REAL\n");}
				 |T_BOOLEAN														{$$ = BOOLEAN;
																				 printf("\t\tstandard_type -> T_BOOLEAN\n");}
				 |T_CHAR														{$$ = CHARACTER;
																				 printf("\t\tstandard_type -> T_CHAR\n");}
				 |T_STRING														{$$ = STRING;
																				 printf("\t\tstandard_type -> T_STRING\n");}
				 ;
			 
fields			 :fields T_SEMI field											{$$ = rl_connect_lists($1, $3);
																				 printf("\t\tfields -> fields T_SEMI field\n");}
				 |fields error 													{printf("\tT_SEMI missing\n");}
				  field															{printf("\t\tfields -> fields field\n");}
				 |field															{printf("\t\tfields -> field\n");}
				 ;
	  
field			 :identifiers T_COLON typename									{$$ = rl_addType($1, $3);
																				 printf("\t\tfield -> dentifiers T_COLON typename\n");}
				 ;
	 
identifiers		 :identifiers T_COMMA T_ID										{$$ = rl_add_node($1, rl_create_node($3));
																				 printf("\t\tidentifiers -> identifiers T_COMMA T_ID\n");}
				 |T_ID															{$$ = rl_create_node($1);
																				 printf("\t\tidentifiers ->T_ID\n");}
				 ;
		   
vardefs			 :T_VAR variable_defs T_SEMI									{$$ = $2;
																				 printf("\t\tvardefs -> T_VAR variable_defs T_SEMI\n");}
				 |T_VAR variable_defs error										{$$ = $2;
																				 printf("\tT_SEMI missing\n"); printf("\t\tvardefs -> T_VAR variable_defs\n");}
				 |/* ε */														{$$ = NULL;
																				 printf("\t\tvardefs -> ε\n");}
				 ;

variable_defs	 :variable_defs T_SEMI identifiers T_COLON typename				{$$ = rl_connect_lists($1, ht_var_insert($3, $5, scope, bisonIsConst));
																				 printf("\t\tvariable_defs -> variable_defs T_SEMI identifiers T_COLON typename\n");}
				 |variable_defs error 											{printf("\tT_SEMI missing\n");}
				  identifiers T_COLON typename									{$$ = rl_connect_lists($1, ht_var_insert($4, $6, scope, bisonIsConst));
																				 printf("\t\tvariable_defs -> variable_defs identifiers T_COLON typename\n");}
				 |identifiers T_COLON typename									{$$ = ht_var_insert($1, $3, scope, bisonIsConst);
																				 printf("\t\tvariable_defs -> identifiers T_COLON typename\n");}
				 ;
			 
subprograms		 :subprograms subprogram T_SEMI									{printf("\t\tsubprograms -> subprograms subprogram T_SEMI\n");}
				 |subprograms subprogram error									{printf("\t\tsubprograms -> subprograms subprogram\n");}
				 |/* ε */														{printf("\t\tsubprograms -> ε\n");}
				 ;

subprogram		 :sub_header T_SEMI T_FORWARD									{printf("\t\tsubprogram -> sub_header T_SEMI T_FORWARD\n");}
				 |sub_header error T_FORWARD									{printf("\t\tsubprogram -> sub_header (T_SEMI missing) T_FORWARD\n");}
				 |sub_header T_SEMI												{offset = 0;} 
				  declarations subprograms comp_statement						{$1->T_Union.func_proc.astree = $6; $1->T_Union.func_proc.local = $4;
																				 ast_inorder_traversal_print($6);
																				 ht_get(hashtbl, scope); scope--;
																				 printf("\t\tsubprogram -> sub_header T_SEMI declarations subprograms comp_statement\n");}
				 |sub_header error												{offset = 0;}
				 declarations subprograms comp_statement						{$1->T_Union.func_proc.astree = $6; $1->T_Union.func_proc.local = $4; 
																				 ast_inorder_traversal_print($6);
																				 ht_get(hashtbl, scope); scope--;
																				 printf("\t\tsubprogram -> sub_header (T_SEMI missing) declarations subprograms comp_statement\n");}
				 ;
		  
sub_header		 :T_FUNCTION T_ID formal_parameters T_COLON standard_type		{ts = ts_create_func($3, $5); ht_complex_insert(hashtbl, $2, ts, scope); $$ = ts; scope++;
																				 printf("\t\tsub_header -> T_FUNCTION T_ID formal_parameters T_COLON standard_type\n");}
				 |T_PROCEDURE T_ID formal_parameters							{ts = ts_create_proc($3); ht_complex_insert(hashtbl, $2, ts, scope); $$ = ts; scope++;
																				 printf("\t\tsub_header -> T_PROCEDURE T_ID formal_parameters\n");}
				 |T_FUNCTION T_ID												{printf("\t\tsub_header -> T_FUNCTION T_ID\n");}
				 ;
		  
formal_parameters:T_LPAREN parameter_list T_RPAREN								{$$ = $2;
																				 printf("\t\tformal_parameters -> T_LPAREN parameter_list T_RPAREN\n");}
				 |/* ε */														{$$ = NULL;
																				 printf("\t\tformal_parameters -> ε\n");}
				 ;
				 
parameter_list	 :parameter_list T_SEMI pass identifiers T_COLON typename		{$$ = rl_connect_lists($1, rl_addType_addPass($3, $4, $6));
																				 printf("\t\tparameter_list -> parameter_list T_SEMI pass identifiers T_COLON typename\n");}
				 |parameter_list error      									{printf("\tT_SEMI missing\n");}
				 pass identifiers T_COLON typename		 						{printf("\t\tparameter_list -> parameter_list pass identifiers T_COLON typename\n");}
				 |pass identifiers T_COLON typename								{$$ = rl_addType_addPass($1, $2, $4);
																				 printf("\t\tparameter_list -> pass identifiers T_COLON typename\n");}
				 ;
			  
pass      		 :T_VAR															{$$ = "VAR";
																				 printf("\t\tpass -> T_VAR\n");}
				 |/* ε */														{$$ = "VALUE";
																				 printf("\t\tpass -> ε\n");}
				 ;

comp_statement	 :T_BEGIN statements T_END										{$$ = $2;
																				 printf("\t\tcomp_statement -> T_BEGIN statements T_END\n");}
				 ;
			  
statements		 :statements T_SEMI statement									{if ($3 == NULL){$$ = $1;}
																				 else{$$ = new_ast_node(STATEMENT, NOTHING, $1, $3);}
																				 printf("\t\tstatements -> statements T_SEMI statement\n");}
				 |statement														{printf("\t\tstatements -> statement\n");}
				 ;
		  
statement		 :assignment													{printf("\t\tstatement -> assignment\n");}
				 |if_statement													{printf("\t\tstatement -> if_statement\n");}
				 |while_statement												{printf("\t\tstatement -> while_statement\n");}
				 |for_statement													{printf("\t\tstatement -> for_statement\n");}
				 |with_statement												{printf("\t\tstatement -> with_statement\n");}
				 |subprogram_call												{printf("\t\tstatement -> subprogram_call\n");}
				 |io_statement													{printf("\t\tstatement -> io_statement\n");}
				 |comp_statement												{printf("\t\tstatement -> comp_statement\n");}
				 |/* ε */														{$$ = NULL;
																				 printf("\t\tstatement -> ε\n");}
				 ;

assignment		 :variable T_ASSIGN expression									{$$ = sem_check_assign($1, $3);
																				 printf("\t\tassignment -> variable T_ASSIGN expression\n");}
				 |variable T_ASSIGN T_STRING									{temp_d.str_data = mystrdup($3); 
																				 ast_ts = (AST_TS_Union *) malloc(sizeof(AST_TS_Union));
																				 ast_ts->ast_node = new_ast_leaf_node(ts_create_constant(STRING, temp_d)->type_struct, NULL);
																				 $$ = sem_check_assign($1, ast_ts);
																				 printf("\t\tassignment -> variable T_ASSIGN expression\n");}
				 ;
		  
if_statement	 :T_IF expression												{sem_check_BOOL($2);}
				 T_THEN statement if_tail										{$$ = new_ast_if_node($2->ast_node, $5, $6);
																				 printf("\t\tif_statement -> T_IF expression T_THEN statement if_tail\n");}
				 ;
			
if_tail			 :T_ELSE statement												{$$ = $2;
																				 printf("\t\tif_tail -> T_ELSE statement\n");}
				 |/* ε */%prec IFX												{$$ = NULL;
																				 printf("\t\tif_tail -> ε\n");}
				 ;
				 
while_statement	 :T_WHILE expression											{sem_check_BOOL($2);}
				 T_DO statement													{$$ = new_ast_while_node($2->ast_node, $5);
																				 printf("\t\twhile_statement -> T_WHILE expression T_DO statement\n");}
				 ;
			   
for_statement	 :T_FOR T_ID													{ast_ts = ht_extract_ast_ts_union(ht_lookup(hashtbl, scope, $2), $2);
																				 push_for(ast_ts->ast_node);}
                  T_ASSIGN iter_space T_DO statement							{$$ = new_ast_for_node($5.inc, sem_check_assign(ast_ts, $5.node1), $5.node2, $7);
																				 printf("\t\tfor_statement -> T_FOR T_ID T_ASSIGN iter_space T_DO statement\n");
																				 pop_for();}
				 ;
			 
iter_space		 :expression T_TO expression									{sem_check_INT($1); sem_check_INT($3);
																				 $$.node1 = $1; $$.node2 = $3->ast_node; $$.inc = TO;
																				 printf("\t\titer_space -> expression T_TO expression\n");}
				 |expression T_DOWNTO expression								{sem_check_INT($1); sem_check_INT($3);
																				 $$.node1 = $1; $$.node2 = $3->ast_node; $$.inc = DOWNTO;
																				 printf("\t\titer_space -> expression T_DOWNTO expression\n");}
				 ;
		  
with_statement	 :T_WITH variable 												{push_with(sem_check_record_with($2), $2->ast_node);}
				 T_DO statement													{$$ = new_ast_node(WITH, NOTHING, $2->ast_node, $5); pop_with();
																				 printf("\t\twith_statement -> T_WITH variable T_DO statement\n");}
				;
			  
subprogram_call	 :T_ID															{$$ = sem_check_proc($1, NULL);
																				 printf("\t\tsubprogram_call -> T_ID\n");}
				 |T_ID T_LPAREN expressions T_RPAREN							{$$ = sem_check_proc($1, $3);
																				 printf("\t\tsubprogram_call -> T_ID T_LPAREN expressions T_RPAREN\n");}
				 ;
			   
io_statement	 :T_READ T_LPAREN read_list T_RPAREN							{$$ = new_ast_io_node(READ, $3);
																				 printf("\t\tio_statement -> T_READ T_LPAREN read_list T_RPAREN\n");}
				 |T_WRITE T_LPAREN write_list T_RPAREN							{$$ = new_ast_io_node(WRITE, $3);
																				 printf("\t\tio_statement -> T_READ T_LPAREN write_list T_RPAREN\n");}
				 |T_WRITE T_LPAREN error T_RPAREN								{yyclearin; $$ = new_ast_io_node(WRITE, NULL);
																				 printf("\t\tio_statement -> T_WRITE T_LPAREN T_RPAREN\n"); }
				 ;
			
read_list		 :read_list T_COMMA read_item									{$$ = new_ast_node(COMMA, NOTHING, $1, $3);
																				 printf("\t\tread_list-> read_list T_COMMA read_item\n");}
				 |read_item														{printf("\t\tread_list-> read_item\n");}
				 ;
		 
read_item		 :variable														{$$ = $1->ast_node;
																				 printf("\t\tread_item-> variable\n");}
				 ;
		 
write_list		 :write_list T_COMMA write_item									{$$ = new_ast_node(COMMA, NOTHING, $1, $3);
																				 printf("\t\twrite_list-> write_list T_COMMA write_item\n");}
				 |write_item													{printf("\t\twrite_list-> write_item\n");}
				 ;
		  
write_item		 :expression													{printf("\t\twrite_item-> expression\n");}
				 |T_STRING														{data.str_data = mystrdup($1); printf("%s\n",$1);
																				 $$ = new_ast_leaf_node(ts_create_constant(STRING, data)->type_struct, NULL);
																				 printf("\t\twrite_item-> T_STRING\n");}
				 ;


%%


int main(int argc, char *argv[]){
	

	if(!(hashtbl = ht_create(10, NULL))) {
		fprintf(stderr, "ERROR: hashtbl_create() failed\n");
		exit(EXIT_FAILURE);
	}
	
	++argv; --argc;
	if( argc > 0){
		yyin = fopen(argv[0], "r");
		if (yyin == NULL){
			perror ("Error opening file"); return -1;
		}
		yyin1 = fopen(argv[0], "r");
		if (yyin1 == NULL){
			perror ("Error opening file"); return -1;
		}
	}
	yyparse();
	
	ht_destroy(hashtbl);
	
	fclose(yyin);
	return(0);
}



void yyerror(char *message){
	error_count++; 
	printf("(%d)ERROR: \"%s\" | TOKEN: %s | LINE: %d | ->%s<-\n",
	error_count, message, yytext, lineno, linestream);
	if(sem_error){
		printf("SEMANTIC ERROR DETECTED\n\tTermination...\n"); 
		exit(-1); 
 	}
	if(MAX_ERRORS == error_count){
 		printf("ERRORS DETECTED IN PROGRAM: %d\nERRORS ACCEPTED: %d\n\tTermination...\n", error_count, MAX_ERRORS); 
 		exit(-2);
	}
}



