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

void print_pos(struct Position *p) {
    printf("(%d,%d)", p->line, p->pos);
}

struct Fragment {
    struct Position starting, following;
};

typedef struct Fragment YYLTYPE;
typedef struct Fragment Fragment;

void print_frag(struct Fragment *f) {
    print_pos(&(f->starting));
    printf("-");
    print_pos(&(f->following));
}

union Token {
	char *int_var_token;
	char *float_var_token;
	int number_token;
	char sign_token;
};

typedef union Token YYSTYPE;

int continued;
struct Position cur;

#define YY_USER_ACTION                       \
{                                            \
    int i;                                   \
    if (!continued)                          \
        yylloc->starting = cur;              \
    continued = 0;                           \
                                             \
    for (i = 0; i < yyleng; i++) {           \
        if (yytext [i] == '\n') {            \
            cur.line++;                      \
            cur.pos = 1;                     \
        } else {                             \
            cur.pos++;                       \
        }                                    \
        cur.index++;                         \
    }                                        \
    yylloc->following = cur;                 \
}
void err(char *msg, char *lexem) {
    printf("Error");
    print_pos(&cur);
    printf(": %s %s\n", msg, lexem);
}

void init_scanner(const char *path) {
    continued = 0;
    cur.line = 1;
    cur.pos = 1;
    cur.index = 0;
    yyin = fopen(path, "r");
}

typedef struct
{
	
    char** names;
    int size;
} Map;

void init_map(Map* map)
{
    map->size = 0;
    map->names = NULL;
}

int add_ident(Map* map, char* name)
{
    for (int i = 0; i < map->size; i++)
    {
        if (strcmp(name, map->names[i]) == 0)
        {
            return i;
        }
    }
	
	map->size++;
	if (map->size == 1) {
		map->names = (char**)malloc(sizeof(char*) * (map->size));
	}
	else{
		map->names = (char**)realloc(map->names, sizeof(char*) * (map->size))
	}
	map->names[map->size - 1] = (char*)malloc(sizeof(char)*strlen(name));
	strcpy(map->names[map->size - 1], name);
    return map->size - 1;
}

void clear_map(Map * map)
{
    for (int i = 0; i < map->size; i++)
    {
        free(map->names[i]);
    }
    free(map->names);
    map->size = 0;
    map->names = NULL;
}

void print_map(Map * map)
{
    for (int i = 0; i < map->size; i++)
    {
        printf("code %d: %s\n", map->names[i]);
    }
}
Map ident_codes;
%}

INTVAR ([I|J|K|L|M|N][A-Za-z0-9]{0,5})
FLOATVAR ([^IJKLMN][A-Za-z0-9]{0,5})
NUMBER (\d+)
SIGN ([+,])

SPACE [ \t\r\n\v\f]+
%%

{INTVAR} {
	yylval->int_var_token = yytext;
	return TAG_INTVAR;
}
{FLOATVAR} {
	yylval->float_var_token = yytext;
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
{SPACE} 

. err("lex error", yytext);

<<EOF>> return 0;

%%

int main(int argc, const char **argv) {
    int tag;
    YYSTYPE value;
    YYLTYPE coords;
    init_scanner(argv[1]);

    while(1) {
        tag = yylex(&value, &coords);
        if (tag == 0) {
            printf("\n");
            break;
        }
				
        else {
            printf("%s ", tag_names[tag]);
            print_frag(&coords);
            if (tag == TAG_INTVAR) {
                printf(": %d\n", value.int_var_token);
            }
			if (tag == TAG_FLOATVAR){
                printf(": %d\n", value.float_var_token);
			}
            if (tag == TAG_SIGN) {
                printf(": %d\n", value.sign_token);
            }
            if (tag == TAG_NUMBER) {
                printf(": %s\n", value.number_token);
            }
        }
    }
    print_map(&ident_codes);
    clear_map(&ident_codes);
    return 0;
}