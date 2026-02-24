%option noyywrap bison-bridge bison-locations
%{
    #include <stdio.h>
    #include <stdlib.h>
	
    #define TAG_INTVAR 1
	#define TAG_FLOATVAR 2
	#define TAG_NUMBER 3
	#define TAG_SIGN 4
	#define TAG_ERROR 5
	

    char *tag_names[] = {
		"END_OF_PROGRAM", "INTVAR", "FLOATVAR", "NUMBER", "SIGN"
    };

    typedef struct Position Position;
    struct Position {
        int line, pos, index;
    };

    void print_pos(Position *p) {
        printf("(%d,%d)",p->line,p->pos);
    }

    struct Fragment {
        Position starting, following;
    };
	
	
	
    typedef struct Fragment YYLTYPE;
    typedef struct Fragment Fragment;
	
    void print_frag(Fragment* f) {
    	print_pos(&(f->starting));
    	printf("-");
    	print_pos(&(f->following));
    }
	
	
	union Token {
	int int_var_token;
	int float_var_token;
	char *number_token;
	char sign_token;
	};

    typedef union Token YYSTYPE;
	
    int continued;
    struct Position cur;
	
    #define YY_USER_ACTION {             \
        int i;                           \
        if (!continued)                  \
            yylloc->starting = cur;      \
        continued = 0;                   \
        for ( i = 0; i < yyleng; i++){   \
            if ( yytext[i] == '\n'){     \
                cur.line++;              \
                cur.pos = 1;             \
            }                            \
            else                         \
                cur.pos++;               \
            cur.index++;                 \
        }                                \
        yylloc->following = cur;         \
    } 
	

    void init_scanner (const char *path){
        continued = 0;
        cur.line = 1;
        cur.pos = 1;
        cur.index = 0;
		yyin = fopen(path, "r");
    }

    void err(char *msg, char *lexem) {
    printf("Error");
    print_pos(&cur);
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
SPACE [ \t]
NEWLINE [\n]
%%

{INTVAR} {
	int len = strlen(yytext) - 1;
	yymore();

	if ((yytext[len] == '\n') || (yytext[len] == '\t') || (yytext[len] == ' ')) {
	yylval->int_var_token = add_ident(&table, yytext);
	yyless(len);
	return TAG_INTVAR;
	}
	else err("lex error", yytext);
}
{FLOATVAR} {
	int len = strlen(yytext) - 1;
	yymore();

	if ((yytext[len] == '\n') || (yytext[len] == '\t') || (yytext[len] == ' ')) {
	yylval->float_var_token = add_ident(&table, yytext);
	yyless(len);
	return TAG_FLOATVAR;
	}
	else err("lex error", yytext);
}
{SIGN} {
	yylval->sign_token = * yytext;
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
    create_ident_table(&table);
    do{ 
        tag = yylex(&value,&coords);
        if (tag == 0)
            break;

        printf("%s ",tag_names[tag]);
        print_frag(&coords);
        
        if (tag == TAG_NUMBER){
            printf(": %s", value.number_token);
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
    //free(buf);
    return 0;
}