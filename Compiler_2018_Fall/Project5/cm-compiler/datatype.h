#ifndef _DATATYPE_H_
#define _DATATYPE_H_
#include <stdbool.h>
#define MAX_ENTRY 1024
typedef enum {
    FUNCTION_t = 1,
    PARAMETER_t,
    VARIABLE_t,
    CONSTANT_t,
} KIND;
typedef enum {
    VOID_t = 1,
    INTEGER_t,
    BOOLEAN_t,
    STRING_t,
    FLOAT_t,
    DOUBLE_t,
    ARRAY_t,
    ERROR_t,
} BTYPE;
typedef enum {
    ADD_t = 1,
    SUB_t,
    MUL_t,
    DIV_t,
    MOD_t,
    LT_t,
    LE_t,
    EQ_t,
    GE_t,
    GT_t,
    NE_t,
    AND_t,
    OR_t,
    NOT_t,
    NONE_t
} OPERATOR;

typedef enum { Redecl_t, Other_t, NoErr_t } PARERROR;

typedef enum {
    ReferenceError,
    NodeIsFunction,
    IDreused,
    IDNotDecl,
    ConstDeclTypeMismatch,
    ParCountMismatch,
    ParTypeMismatch,
    FuncRetTypeMismatch,
    FuncReDef,
    ArrayIndexNotInt,
    StatementIsArrType,
    StatementNotBool,
    OperandTypeMismatch,
    ConstAssignment,
    UnaryOperandShouldBeReal,
    ParameterError,
    InitVarTypeErr,
    ArrTypeAssignment,
    NodeIsNotFunction,
    AssignmentTypeMismatch,
    StatementIsString,
    ArrSizeShouldG0,
    ControlStatNotBool,
    ContinueOutOfLoop,
    BreakOutOfLoop,
    OtherErr,
} ERRTYPE;  // errors

typedef struct ARRAYDIMNODE {
    int size;
    struct ARRAYDIMNODE *next;
} ArrayDimNode;

typedef struct EXTTYPE {
    bool isError;
    bool isArray;
    BTYPE type;
    int dimNum;
    ArrayDimNode *dim;
} ExtType;

typedef struct CONSTATTR {
    BTYPE type;
    union {
        int integerVal;
        float floatVal;
        double doubleVal;
        bool booleanVal;
        char *stringVal;
    } value;
    bool isNegative;
} ConstAttr;

typedef struct TYPEPOINTER {
    ExtType *value;
    struct TYPEPOINTER *next;
} ExtTypePointer;

typedef struct FUNCATTR {
    int paramNum;
    ExtTypePointer *params;
} FuncAttr;

typedef struct IDNODE_SEM {
    char *value;
    struct IDNODE_SEM *next;
    BTYPE category;
} IDNodeSemantic;

typedef struct PAR_SEM {
    IDNodeSemantic *idlist;
    ExtType *extType;
    struct PAR_SEM *next;
} ParamSemantic;

typedef struct CONSTPARAM {
    char *name;
    ConstAttr *value;
    struct CONSTPARAM *next;
} constParam;

typedef struct TYPENODE {
    BTYPE value;
    struct TYPENODE *next;
} typeNode;

typedef struct VAR_REF_SEM {
    char *id;
    int dimNum;
    typeNode *dim;
} var_ref_sem;

typedef struct EXPR_SEM {
    OPERATOR startOperator;
    bool isDeref;
    var_ref_sem *varRef;
    ExtType *extType;
    struct EXPR_SEM *next;
} ExprSemantic;

typedef struct VARDECL {
    ParamSemantic *para;
    ExprSemantic *expr;
    bool isArray;
    bool isInit;
    struct VARDECL *next;
} VarDecl;

typedef struct SYMATTR {
    ConstAttr *constVal;
    FuncAttr *funcParam;
} SymAttr;

typedef struct SYMNODE {
    char *name;
    int scope;
    KIND kind;
    ExtType *type;
    SymAttr *attribute;
    struct SYMNODE *next;
    struct SYMNODE *prev;

    bool declAndDefined;
    bool isFuncDefine;
} SymNode;

typedef struct SYMTABLE {
    SymNode *entry[MAX_ENTRY];
} SymTable;

#endif
