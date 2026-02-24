CC=gcc
OBJ=pasc320.tab.c lex.yy.c hashtbl.o


pasc320: lex.yy.c pasc320.tab.c pasc320.tab.h hashtbl.o
	$(CC)  -Wall $(OBJ) -o $@ -lm
	
pasc320.tab.c pasc320.tab.h: pasc320.y
	bison  -v  -d  pasc320.y

lex.yy.c: pasc320.l pasc320.tab.h
	flex pasc320.l
	
hashtbl.o: hashtbl.c hashtbl.h
	$(CC) -o hashtbl.o -Wall -g -c hashtbl.c
