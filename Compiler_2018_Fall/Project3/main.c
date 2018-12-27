#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

extern int yyparse();
extern FILE *yyin;

extern char fileName[256];
extern SymTable *symTable;

extern bool semError;

int main(int argc, char **argv) {
    if (argc == 1) {
        yyin = stdin;
    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "Open file error\n");
            exit(-1);
        }
        yyin = fp;
    } else {
        fprintf(stderr, "Usage: ./parser [filename]\n");
        exit(0);
    }

    symTable = symTableInit();
    yyparse();

    if (semError == true) {
        fprintf(stdout, "\n|--------------------------------|\n");
        fprintf(stdout, "|  There is no syntactic error!  |\n");
        fprintf(stdout, "|--------------------------------|\n");
    } else {
        fprintf(stdout, "\n|-------------------------------------------|\n");
        fprintf(stdout, "| There is no syntactic and semantic error! |\n");
        fprintf(stdout, "|-------------------------------------------|\n");
    }

    exit(0);
}
