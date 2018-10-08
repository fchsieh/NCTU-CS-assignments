all:
	lex scanner.l
	gcc -o scanner lex.yy.c -lfl
	rm -f lex.yy.c

test:
	@lex scanner.l
	@gcc -o scanner lex.yy.c -lfl -D ERR=stdout
	@rm -f lex.yy.c

clean:
	rm -f scanner
