# Build all
all:
	 lex scanner.l
	 gcc lex.yy.c -lfl -o scanner
	 rm -f lex.yy.c
run:
	 @lex scanner.l
	 @gcc lex.yy.c -lfl -o scanner
	 @rm -f lex.yy.c
	 @echo "******** Test Output *********"
	 @./scanner input.c ;true
	 @echo "******** End of Test *********"
test:
	@lex scanner.l
	@gcc lex.yy.c -lfl -o scanner  -D ERR_FD=stdout
	@rm -f lex.yy.c
	@./main_tester 





