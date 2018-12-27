#ifndef _SEMCHECK_H_
#define _SEMCHECK_H_
#include <stdbool.h>
#include "datatype.h"
#include "symtable.h"

extern int linenum;
extern bool semError;

typedef struct IDRECNODE {
    char *id;
    struct IDRECNODE *next;
} IDRecNode;

typedef struct IDRECORD {
    IDRecNode *entry[100];
} IDRecordList;

// ========== Shared Function ==========
SymNode *lookupSymbol(SymTable *table, char *id, int scope, bool currentScope);
void printError(ERRTYPE);
bool checkRedeclaration(SymTable *table, char *str, int scope);
bool checkExistence(SymTable *table, ExprSemantic *expr, int scope,
                    bool isAssign);
// ====================================

// ========== Check Array declare ==========
BTYPE checkArrayIndex(ExprSemantic *expr);
ArrayDimNode *copyArrayDimList(ArrayDimNode *src);
bool checkArrayInitVal(ExtType *scalar, VarDecl *var, SymTable *table,
                       int scope);
bool traversalCheckIndex(ExprSemantic *arr_expr);
// ==========================================

// ========== Check Function declare ==========
PARERROR checkFuncParam(ParamSemantic *params);
bool checkFuncDeclaration(SymTable *table, ParamSemantic *par,
                          ExtType *scalarType, SymNode *node);
SymNode *findFuncDeclaration(SymTable *table, char *name);
ExprSemantic *checkFuncInvoke(char *id, ExprSemantic *exprList, SymTable *table,
                              int scope);
void checkReturnStatement(ExprSemantic *expr, ExtType *funcReturn);
IDRecordList *buildIDList();
void insertIDIntoRecList(IDRecordList *list, char *newID);
bool traversalCheckIDRepeat(IDRecordList *list, char *checkID);
// =============================================

// ========== Check assignment ==========
void checkAssignmentTypeMatch(ExprSemantic *LHS, ExprSemantic *RHS);
bool checkVarInitValue(ExtType *scalar, VarDecl *var, SymTable *table,
                       int scope);
// ======================================

// ========== Check calculation ==========
void checkArithmeticOp(ExprSemantic *op1, OPERATOR operator, ExprSemantic *op2);
void checkRelOp(ExprSemantic *op1, OPERATOR operator, ExprSemantic *op2);
void checkUnaryNOT(ExprSemantic *expr);
void checkUnaryMinus(ExprSemantic *expr);
void checkAndOrOp(ExprSemantic *op1, OPERATOR operator, ExprSemantic *op2);
void checkBooleanExpr(ExprSemantic *expr);
// =======================================
#endif