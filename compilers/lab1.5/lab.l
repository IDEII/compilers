%option noyywrap bison-bridge bison-locations
%{
    #include <stdio.h>
    #include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	
    #define TAG_INTVAR 1
	#define TAG_FLOATVAR 2
	#define TAG_NUMBER 3
	#define TAG_SIGN 4
	
	void remove_S(char *newstr, char *string) {
		int non_space_count = 0;
		//char newstr[strlen(string)];
		for (int i = 0; string[i] != '\0'; i++)
		{
			if ((string[i] != ' ') && (string[i] != '\t'))
			{
				//printf("%c", string[i]);
				newstr[non_space_count] = string[i];
				non_space_count++;
			}    
		}
		
		newstr[non_space_count] = '\0';
		char *p = newstr;
		//printf("%s",h);
		//printf("%s", p);
		//h = strtok(p,"");
		strcpy(newstr, p);
		//printf("%s",newstr);
		//h = &p;
		//printf("%s",h);
		//return newstr;
	}

	
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
	unsigned long long number_token;
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

INTVAR ([IJKLMN]([ \t]*[A-Za-z0-9]){0,5})
FLOATVAR ([A-HO-Za-z]([ \t]*[A-Za-z0-9]){0,5})
NUMBER ([0-9][0-9 \t]*[0-9])
SIGN ([+,])
SPACE [ \t\n]
%%

{INTVAR} {
	//char* h1;
	char newstr[strlen(yytext)];
	//h1 = &newstr;
	char* h1 = strtok(newstr,"");
	remove_S(h1,yytext);
	//
	//printf("%s",h1);
	yylval-> int_var_token = add_ident(&table, h1);
	return TAG_INTVAR;
	
}
{FLOATVAR} {
	//char* h1;
	char newstr[strlen(yytext)];
	//h1 = &newstr;
	char* h1 = strtok(newstr,"");
	remove_S(h1,yytext);
	//
	//printf("%s",h1);
	yylval-> float_var_token = add_ident(&table, h1);
	return TAG_FLOATVAR;
}
{SIGN} {
	strcpy(&yylval->sign_token, yytext);
	return TAG_SIGN;
}
{NUMBER} {
	//char* h1;
	char newstr[strlen(yytext)];
	//h1 = &newstr;
	char* h1 = strtok(newstr,"");
	remove_S(h1,yytext);
	//
	//printf("\n%s\n",h1);
	unsigned long long num = strtoll(h1, NULL, 10);
	yylval->number_token = num;
	//printf("%s",h1);

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
			unsigned long long x = value.number_token;
            printf(": %lli", x);
			if (x <= 65535) {
				printf(" %s", "u int");
			}
			else if (x <= 4294967295) {
				printf(" %s", "u long int");
				} else printf(" %s", "u long long int");
			
		}
        
        if (tag == TAG_SIGN){
            printf(": %c", value.sign_token);
        }

        if (tag == TAG_INTVAR){
			int x = value.int_var_token;
            printf(": %d %s", x, table.names[value.int_var_token]); 
			
        } 
		if (tag == TAG_FLOATVAR){
			int x = value.float_var_token;
            printf(": %d %s", x, table.names[value.float_var_token]); 
        } 
		
		
        printf("\n");        
    } while (tag != 0); 
    
    free(table.names);
    return 0;
}