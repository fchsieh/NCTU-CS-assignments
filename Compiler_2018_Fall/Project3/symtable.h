#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include <stdbool.h>

#define MAX_ENTRY 10000

typedef enum { REDECL_t } ERRTYPE;
typedef enum { FUNCTION_t, VARIABLE_t, CONSTANT_t, PARAMETER_t } KIND;
typedef enum {
    INTEGER_t,
    BOOLEAN_t,
    STRING_t,
    DOUBLE_t,
    FLOAT_t,
    VOID_t,
} TYPE;

typedef struct ARRDIM {
    int size;
    struct ARRDIM *next;
} ArrDim;

typedef struct PARTYPE {
    TYPE type;
    int dimNum;
    ArrDim *dim;
} ParType;

typedef struct CONSTVAL {
    int intVal;
    float floatVal;
    double doubleVal;
    bool boolVal;
    char *stringVal;
} ConstVal;

typedef struct PARTYPELIST {
    ParType *value;
    struct PARTYPELIST *next;
} ParTypeList;

typedef struct FUNCATTR {
    int parNum;
    ParTypeList *params;
} FuncAttr;

typedef struct CONSTATTR {
    KIND kind;
    ConstVal *value;
} ConstAttr;

typedef struct CONSTPARAM {
    char *name;
    ConstAttr *value;
    struct CONSTPARAM *next;
} ConstParam;

typedef struct SYMATTR {
    ConstAttr *constVal;
    FuncAttr *paramList;
} SymAttr;

typedef struct IDNODE {
    KIND kind;
    char *value;
    struct IDNODE *next;
} IDNode;

typedef struct PARAMS {
    IDNode *idlist;
    ParType *parType;
    struct PARAMS *next;
} Params;

typedef struct VARPARAM {
    Params *para;
    struct VARPARAM *next;
} VarParam;

typedef struct SYMNODE {
    char *name;
    int scope;
    KIND kind;
    ParType *type;
    SymAttr *attr;
    struct SYMNODE *next;
    struct SYMNODE *prev;
} SymNode;

typedef struct SYMTABLE {
    SymNode *entry[MAX_ENTRY];
    int tail;
} SymTable;

// ========== Printing SymTable ==========
char *typeBuffer(ParType *type);
void printAttr(KIND kind, SymNode *ptr);
void printSymTable(SymTable *table, int scope);
void deleteScopeNode(SymTable *table, int scope);
// =======================================

// ========== Shared function ==========
SymTable *symTableInit();
void insertIntoSymTable(SymTable *table, SymNode *newNode);
ParType *createParamType(TYPE type);
bool IDRedecl(SymTable *table, int scope, char *id);
void printError(ERRTYPE err, int errorLine, char *id);
ParType *copyParType(ParType *pt);
void combineDeclare(KIND kind, void *left, void *right);
// =====================================

// ========== FUNCTION ==========
void insertFuncNode(SymTable *table, char *id, Params *par, ParType *retType);
SymNode *createFuncNode(char *id, ParType *retType, FuncAttr *params);
// ==============================

// ========== VARIABLE ==========
VarParam *createVarParam(Params *par);
SymNode *createVarNode(char *id, int scope, ParType *parType, ParType *retType);
// ==============================

// ========== PARAMETER ==========
SymNode *createParNode(char *id, int scope, ParType *type);
// ===============================

// ========== VAR and PAR ==========
void increaseArrayDim(ParType *parType, int dimSize);
IDNode *createIDList(char *id);
Params *createParam(IDNode *idlist, ParType *parType);
void insertParamNode(SymTable *table, Params *params, int scope);
// =================================

// ========== CONSTANT ==========
ConstAttr *createConstAttr(TYPE type, ConstVal *constVal);
ConstParam *createConstParam(ConstAttr *constNode, char *id);
SymNode *createConstNode(char *id, int scope, ParType *parType,
                         ConstAttr *constAttr);
// ==============================

#endif