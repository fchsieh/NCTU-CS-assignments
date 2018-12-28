#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "semcheck.h"

extern SymTable *symTable;
extern FILE *output;
extern char *filename;

// init function
void codegen_InitProgram();

// function definition
void codegen_BuildMainFunction();
void codegen_BuildOtherFunction(char *id, ParamSemantic *param,
                                ExtType *retType);
void codegen_FunctionEnd(ExtType *retType);

// function invoke
void codegen_FuncInvoke(char *id, bool isNeg);

// simple statement
void codegen_PrintStart();
void codegen_PrintEnd(ExprSemantic *expr);
void codegen_ConstLiteral(ConstAttr *constAttr);

#endif