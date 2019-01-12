#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "datatype.h"
#include "symtable.h"

extern int yyparse();
extern FILE *yyin;

extern SymTable *symTable;
extern ExtType *funcReturn;
extern bool semError;

FILE *output;
char *filename;

// global var q's
struct idQueue *idq;
struct valQueue *valq;
struct localStack *locals;
bool globalConvertList[100];

int main(int argc, char **argv) {
    if (argc == 1) {
        yyin = stdin;
    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "Open file error\n");
            exit(-1);
        }
        filename = "output";
        output = fopen("output.j", "w");
        codegen_InitProgram();
        yyin = fp;
    } else {
        fprintf(stderr, "Usage: ./parser [filename]\n");
        exit(0);
    }
    // initializations
    idq = initIDQueue();
    valq = initValQueue();
    locals = initLocalStack();
    for (int i = 0; i < 100; i++) {
        globalConvertList[i] = false;
    }

    symTable = initSymTab();  // create symbol table
    yyparse();                // main procedure

    if (semError == true) {
        fprintf(stdout, "\n|--------------------------------|\n");
        fprintf(stdout, "|  There is no syntactic error!  |\n");
        fprintf(stdout, "|--------------------------------|\n");
        fclose(output);
        exit(255);
    } else {
        fprintf(stdout, "\n|-------------------------------------------|\n");
        fprintf(stdout, "| There is no syntactic and semantic error! |\n");
        fprintf(stdout, "|-------------------------------------------|\n");
        fclose(output);
        exit(0);
    }

    exit(0);
}
