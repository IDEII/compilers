CC=gcc
OBJ=lex.yy.c


compiler: lex.yy.c compilers.h

	$(CC)  $(OBJ) -o compiler -ll -lm
	

lex.yy.c: compilers.l compilers.h
	flex compilers.l