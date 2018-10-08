all: lex.yy.c
	gcc -o test.o lex.yy.c -lfl

lex.yy.c: test.l
	lex test.l

clean:
	rm -f test.o lex.yy.c