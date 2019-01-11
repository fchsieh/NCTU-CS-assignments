MAKEFLAGS += --silent
TARGET = compiler
OBJECT = lex.yy.c y.tab.c y.tab.o lex.yy.o y.output compiler y.tab.h main.o semcheck.o symtable.o codegen.o
DELFILES = lex.yy.c y.tab.c y.tab.o lex.yy.o y.output y.tab.h main.o semcheck.o symtable.o codegen.o
CC = gcc -g
LEX = flex
YACC = byacc -v
YACCFLAG = -d
LIBS = -lfl

all: $(TARGET)

$(TARGET): y.tab.o lex.yy.o main.o semcheck.o symtable.o codegen.o
	$(CC) -o $(TARGET) y.tab.o lex.yy.o main.o semcheck.o symtable.o codegen.o $(LIBS)
	rm -f $(DELFILES)

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

codegen.o: codegen.c
	$(CC) -c codegen.c

clean:
	rm -f $(TARGET) $(OBJECT)

.PHONY: all
