MAKEFLAGS += --silent
TARGET = parser
OBJECT = lex.yy.c y.tab.c y.tab.o lex.yy.o y.output parser y.tab.h main.o symtable.o mylist.o
DELFILE = lex.yy.c y.tab.c y.tab.o lex.yy.o y.output y.tab.h main.o symtable.o mylist.o
CC = gcc -g
LEX = flex
YACC = byacc -v
YACCFLAG = -d
LIBS = -lfl

all: $(TARGET)

parser: y.tab.o lex.yy.o main.o symtable.o
	$(CC) -o $(TARGET) y.tab.o lex.yy.o main.o symtable.o $(LIBS)
	rm -f $(DELFILE)

y.tab.o: y.tab.c
	$(CC) -c y.tab.c

y.tab.c: parser.y
	$(YACC) $(YACCFLAG) parser.y

lex.yy.o: lex.yy.c
	$(CC) -c lex.yy.c

lex.yy.c: lex.l
	$(LEX) lex.l

main.o: main.c
	$(CC) -c main.c

clean:
	rm -f $(TARGET) $(OBJECT)

.PHONY: all
