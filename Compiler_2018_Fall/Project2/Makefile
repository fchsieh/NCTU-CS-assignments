MAKEFLAGS += --silent

all: 
	byacc -d -v parser.y
	flex lex.l
	gcc lex.yy.c y.tab.c -ly -ll -o parser
	rm -f y.tab.c y.tab.h y.output lex.yy.c
clean:
	rm -f parser y.tab.c y.tab.h y.output lex.yy.c

.PHONY: all
