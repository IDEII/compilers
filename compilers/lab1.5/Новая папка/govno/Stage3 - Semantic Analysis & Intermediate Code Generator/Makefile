CC=gcc
OBJ=pasc320.tab.c lex.yy.c hashtbl.o check_types.o types.o ast.o


pasc320: lex.yy.c pasc320.tab.c pasc320.tab.h hashtbl.o check_types.o types.o
	$(CC)  -Wall $(OBJ) -o $@ -lm
	
pasc320.tab.c pasc320.tab.h: pasc320.y check_types.o types.o ast.o
	bison  -v  -d  pasc320.y

lex.yy.c: pasc320.l pasc320.tab.h check_types.o types.o
	flex pasc320.l
	
check_types.o: types.h hashtbl.h check_types.c check_types.h
	$(CC) -o check_types.o -Wall -g -c check_types.c
	
hashtbl.o: types.h hashtbl.c hashtbl.h
	$(CC) -o hashtbl.o -Wall -g -c hashtbl.c

types.o: hashtbl.h types.c types.h
	$(CC) -o types.o -Wall -g -c types.c

ast.o: ast.h ast.c types.h types.o
	$(CC) -o ast.o -Wall -g -c ast.c

.PHONY: clean
clean: 
	rm -f *.o
	rm -f *.tab.*
	rm -f *.yy.c
	rm -f *.output
