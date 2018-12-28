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

char *remove_ext(char *mystr, char dot, char sep);

int main(int argc, char **argv) {
    if (argc == 1) {
        yyin = stdin;
    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "Open file error\n");
            exit(-1);
        }

        filename = malloc(sizeof(char) * (strlen(argv[1]) + 1));
        memset(filename, 0, sizeof(filename));
        filename = remove_ext(argv[1], '.', '/');

        // write to filename.j
        char *filenameWithJ = malloc(sizeof(char) * (strlen(filename) + 3));
        memset(filenameWithJ, 0, sizeof(filenameWithJ));
        strcpy(filenameWithJ, filename);
        strcat(filenameWithJ, ".j");
        output = fopen(filenameWithJ, "w");

        // read file's name
        filename = basename(filename);
        codegen_InitProgram();
        yyin = fp;
    } else {
        fprintf(stderr, "Usage: ./parser [filename]\n");
        exit(0);
    }

    symTable = initSymTab();  // create symbol table
    yyparse();                // main procedure

    if (semError == true) {
        fprintf(stdout, "\n|--------------------------------|\n");
        fprintf(stdout, "|  There is no syntactic error!  |\n");
        fprintf(stdout, "|--------------------------------|\n");
    } else {
        fprintf(stdout, "\n|-------------------------------------------|\n");
        fprintf(stdout, "| There is no syntactic and semantic error! |\n");
        fprintf(stdout, "|-------------------------------------------|\n");
    }
    fclose(output);
    exit(0);
}

char *remove_ext(char *mystr, char dot, char sep) {
    // get file name without extension or "/" in path
    char *retstr, *lastdot, *lastsep;

    if (mystr == NULL) return NULL;
    if ((retstr = malloc(strlen(mystr) + 1)) == NULL) return NULL;

    strcpy(retstr, mystr);
    lastdot = strrchr(retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr(retstr, sep);

    if (lastdot != NULL) {
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                *lastdot = '\0';
            }
        } else {
            *lastdot = '\0';
        }
    }
    return retstr;
}
