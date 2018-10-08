all: lex.yy.c
	gcc -o scanner.o lex.yy.c -lfl

lex.yy.c: scanner.l
	lex scanner.l

clean:
	rm -f scanner.o lex.yy.c