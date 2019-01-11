#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdbool.h>
#include "datatype.h"
extern int varNumber;

// ============= Build SymTable =============
SymTable *initSymTab();
bool insertParamIntoSymTable(SymTable *table, ParamSemantic *params, int scope);
void insertFuncIntoSymTable(SymTable *table, char *id, ParamSemantic *params,
                            ExtType *retType, int scope, bool isDefine);
void insertIntoSymTable(SymTable *table, SymNode *newNode);
void deleteScope(SymTable *table, int scope);
// ==========================================

// ============= Printing SymTable =============
char *typeBuffer(ExtType *type);
void printAttr(SymNode *ptr);
void printSymTable(SymTable *table, int scope);
// =============================================

// ============= Create SymNodes  =============
SymNode *createNode(char *name, int scope, ExtType *scalarType, KIND kind,
                    void *attr, int varNumber);
ConstAttr *createConstantAttribute(BTYPE type, void *value);
constParam *createConstParam(ConstAttr *constNode, char *id);
ParamSemantic *createParam(IDNodeSemantic *ids, ExtType *extType);
VarDecl *createVarDecl(ParamSemantic *par, ExprSemantic *exp);
ExprSemantic *createExprSem(char *id);
ExtType *createExtType(BTYPE type);
IDNodeSemantic *createIdList(char *str);
void concateParam(char *type, void *left, void *right);
void increaseDim(ExprSemantic *expr, BTYPE dimType);
void increaseArrayDim(ExtType *extType, int size);
ExtType *copyExtType(ExtType *src);
// ============================================

#endif