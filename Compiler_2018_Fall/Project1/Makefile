all:
	 lex scanner.l
	 gcc lex.yy.c -lfl -o scanner
	 rm -f lex.yy.c
clean:
	 rm -f lex.yy.c scanner
	 