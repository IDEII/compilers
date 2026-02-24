%option noyywrap bison-bridge bison-locations
%{
    #include <stdio.h>
    #include <stdlib.h>
	
    #define TAG_INTVAR 1
	#define TAG_FLOATVAR 2
	#define TAG_NUMBER 3
	#define TAG_SIGN 4

    char *tag_names[] = {
		"END_OF_PROGRAM", "INTVAR", "FLOATVAR", "NUMBER", "SIGN"
    };

	extern int yylineno;
	extern int yycolumn;
	extern YYSTYPE yylloc;
    
	union Token {
	char *int_var_token;
	char *float_var_token;
	char *number_token;
	char sign_token;
	};

    typedef union Token YYSTYPE;
	

    void init_scanner (const char *path){
		yyin = fopen(path, "r");
    }

    void err(char *msg, char *lexem) {
    printf("Error");
    printf(": %s %s\n", msg, lexem);
}

    typedef struct{
        int size;
        char** names;
    } identTable;

    void create_ident_table(identTable * t){
        t->size = 0;
        t->names = NULL;
    }

    int add_ident(identTable* table, char* name){
        for (int i = 0; i < table->size; i++){
            if (strcmp(name, table->names[i]) == 0){
                return i;
            }
        }

        table->size++;
        if (table->size == 1){
            table->names = (char**)malloc(sizeof(char*) * (table->size));
        }
        else {
            table->names = (char**)realloc(table->names, sizeof(char*) * (table->size));
        }
        table->names[table->size - 1] = (char*)malloc(sizeof(char)*strlen(name));
        strcpy(table->names[table->size - 1], name);
        return table->size-1;
    }

    identTable table;
    
%}

INTVAR ([[IJKLMN][ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1})
FLOATVAR ([A-HO-Za-z][ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1}[ \t]*[A-Za-z0-9]{0,1})
NUMBER ([0-9][0-9 \t]*[0-9])
SIGN ([+,])
SPACE [ \t\r\n\v\f\A]
%%

{INTVAR}{SPACE} {
	//printf("%s\n\n", yytext);
	yylval->int_var_token = add_ident(&table, yytext);
	//yyless(strlen(yytext)-1);
	//if (yytext[strlen(yytext)] == "\n"){
	//	cur.line--;
	//}
	//else cur.pos--;
	//yylloc->following = cur; 
	return TAG_INTVAR;
	
}
{FLOATVAR}{SPACE} {
	//printf("%s\n\n", yytext);

	yylval->float_var_token = add_ident(&table, yytext);
	//yyless(strlen(yytext)-1);
	//if (yytext[strlen(yytext)] == "\n"){
	//	cur.line--;
	//}
	//else cur.pos--;
	//yylloc->following = cur; 
	return TAG_FLOATVAR;
}
{SIGN} {
	yylval->sign_token = yytext;
	return TAG_SIGN;
}
{NUMBER} {
	yylval->number_token = yytext;
	return TAG_NUMBER;
}
{SPACE} {}

. err("lex error", yytext);

<<EOF>> return 0;
%%

int main(int argc, const char **argv){
    int tag;
    YYSTYPE value;
    YYLTYPE coords;
   	FILE *input;
	long size;
	char *buf;
	union Token token;
	
    init_scanner(argv[1]);
    //create_ident_table(&table);
    do{ 
        tag = yylex(&value,&coords);
        if (tag == 0)
            break;

        printf("%s ",tag_names[tag]);
        //print_frag(&coords);
        
        if (tag == TAG_NUMBER){
            printf(": %d", value.number_token);
		}
        
        if (tag == TAG_SIGN){
            printf(": %d", value.sign_token);
        }

        if (tag == TAG_INTVAR){
            printf(": %d", value.int_var_token); 
        } 
		if (tag == TAG_FLOATVAR){
            printf(": %d", value.float_var_token); 
        } 
		
		
        printf("\n");        
    } while (tag != 0); 
    
    free(table.names);
	free(buf);
    return 0;
}