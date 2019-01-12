#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "semcheck.h"

extern SymTable *symTable;
extern FILE *output;
extern char *filename;
extern int scope;

extern struct insList insList;
extern int globalCount;
extern struct idQueue *idq;
extern struct valQueue *valq;

struct insList {
    char *list[2048];
    int size;
};
char insBuf[256];

struct loop_stack {
    int stack[100];
    int top;
};
struct cond_stack {
    int stack[100];
    int top;
};

// for printing codes
void codegen_PushIns(char *instr);
void codegen_GenToList(char *fmt, ...);

// start
void codegen_InitProgram();

// ===== Function =====
// function definition
void codegen_BuildMainFunction();
void codegen_BuildOtherFunction(char *id, ParamSemantic *param,
                                ExtType *retType);
void codegen_FunctionEnd(ExtType *retType);  // end of function def

// function invoke
void codegen_FunctionInvoke(char *id, bool isNeg);
void codegen_FunctionReturn(ExtType *retType);  // returning a function
// ====================

// ===== Simple statement =====
void codegen_PrintStart();
void codegen_PrintEnd(ExprSemantic *expr);
void codegen_ConstLiteral(ConstAttr *constAttr);
void codegen_Read(ExprSemantic *expr);
// ============================

// ===== Variable =====
void codegen_GlobalVarQueue_int(int value);
void codegen_GlobalVarQueue_real(double value);
void codegen_GlobalVar(VarDecl *ptr, ExtType *type);
void codegen_ExprIns();
void codegen_SaveExpr();
void codegen_LoadVar(ExprSemantic *var);
void codegen_Negative();
void codegen_LocalVar(ExtType *type, int varNo);
// ====================

// ===== Operations =====
void codegen_Arithmetic(ExprSemantic *op1, OPERATOR op, ExprSemantic *op2);
// ======================

// ===== BOOLEAN =====
void codegen_BooleanOp(ExprSemantic *op1, OPERATOR op, ExprSemantic *op2);
void codegen_Relation(ExprSemantic *op1, OPERATOR op, ExprSemantic *op2);
// ===================

// for global variable queue
// ===== ID Q =====
struct idNode {
    char *data;
    bool hasVal;
    struct idNode *next;
};

struct idQueue {
    struct idNode *front, *rear;
};

struct idNode *idNewNode(char *id, bool hasVal);
struct idQueue *initIDQueue();
void idEnQ(struct idQueue *q, char *id, bool hasVal);
struct idNode *idDeq(struct idQueue *q);

struct valNode {
    double val;
    BTYPE type;
    struct valNode *next;
};

struct valQueue {
    struct valNode *front, *rear;
};

struct valNode *valNewNode(double val, BTYPE type);
struct valQueue *initValQueue();
void valEnQ(struct valQueue *q, double val, BTYPE type);
struct valNode *valDeq(struct valQueue *q);
// ===============

// ===== For Local Var =====
struct localNode {
    char *id;
    BTYPE type;
    int varNo;
};

struct localStack {
    int top;
    struct localNode **stack;
};

struct localNode *localNewNode(char *id, BTYPE type, int varNo);
struct localStack *initLocalStack();
void localPush(struct localStack *stack, char *id, BTYPE type, int varNo);
struct localNode *locPop(struct localStack *stack);
bool stackIsEmpty(struct localStack *stack);
// =========================
#endif