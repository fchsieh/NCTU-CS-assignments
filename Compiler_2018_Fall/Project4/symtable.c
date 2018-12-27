#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "semcheck.h"

// ============= Build SymTable =============
SymTable *initSymTab() {
    SymTable *table = malloc(sizeof(SymTable));
    for (int i = 0; i < MAX_ENTRY; i++) {
        table->entry[i] = NULL;
    }
    return table;
}

void insertIntoSymTable(SymTable *table, SymNode *newNode) {
    if (table->entry[0] == NULL) {  // symbol table is empty
        table->entry[0] = newNode;
    } else {
        SymNode *nodePtr = table->entry[0];
        while (nodePtr->next != NULL) {
            nodePtr = nodePtr->next;
        }
        nodePtr->next = newNode;
        newNode->prev = nodePtr;
    }
}

void insertFuncIntoSymTable(SymTable *table, char *id, ParamSemantic *params,
                            ExtType *retType, int scope, bool isDefine) {
    if (checkRedeclaration(table, id, scope) == false) {
        // function redeclared
        return;
    } else {
        FuncAttr *funcParam = malloc(sizeof(FuncAttr));
        funcParam->paramNum = 0;
        funcParam->params = 0;

        if (params == 0) {
        } else {
            ParamSemantic *parPtr;
            IDNodeSemantic *idPtr;

            ExtTypePointer *lastPtr = 0;

            for (parPtr = params; parPtr != 0; parPtr = (parPtr->next)) {
                for (idPtr = (parPtr->idlist); idPtr != 0;
                     idPtr = (idPtr->next)) {
                    if (funcParam->paramNum == 0) {
                        funcParam->params = malloc(sizeof(ExtTypePointer));
                        funcParam->params->value = copyExtType(parPtr->extType);
                        funcParam->params->next = 0;

                        ++(funcParam->paramNum);

                        lastPtr = funcParam->params;
                    } else {
                        ExtTypePointer *typePtr =
                            malloc(sizeof(ExtTypePointer));
                        typePtr->value = copyExtType(parPtr->extType);
                        typePtr->next = 0;
                        lastPtr->next = typePtr;
                        funcParam->paramNum += 1;

                        lastPtr = lastPtr->next;
                    }
                }
            }
        }

        SymNode *newNode = createNode(id, 0, retType, FUNCTION_t, funcParam);
        newNode->isFuncDefine = isDefine;
        insertIntoSymTable(table, newNode);
    }
}

bool insertParamIntoSymTable(SymTable *table, ParamSemantic *params,
                             int scope) {
    bool result = false;

    if (params == 0) {
        result = false;
    } else {
        ParamSemantic *parPtr;
        IDNodeSemantic *idPtr;
        SymNode *newNode;
        for (parPtr = params; parPtr != 0; parPtr = (parPtr->next)) {
            if (parPtr->extType->isError == true) {
                result = true;
            } else {
                for (idPtr = (parPtr->idlist); idPtr != 0;
                     idPtr = (idPtr->next)) {
                    if (checkRedeclaration(table, idPtr->value, scope) ==
                        false) {
                        result = true;
                    } else {
                        newNode =
                            createNode(idPtr->value, scope, parPtr->extType,
                                       PARAMETER_t, NULL);
                        insertIntoSymTable(table, newNode);
                    }
                }
            }
        }
    }
    return result;
}

void deleteScope(SymTable *table, int scope) {
    for (int i = 0; i < MAX_ENTRY; i++) {
        if (table->entry[i] == NULL) {
            continue;
        } else if (table->entry[i]->next == NULL) {
            if (table->entry[i]->scope == scope) {
                // found node in this scope, clear the node.
                table->entry[i] = NULL;
            }
        } else {
            SymNode *current = table->entry[i];
            SymNode *next = current->next;
            while (next != NULL) {
                if (current->scope == scope) {
                    if (current->prev == NULL) {
                        current->next->prev = NULL;
                        table->entry[i] = next;

                    } else {
                        current->prev->next = next;
                        next->prev = current->prev;
                    }
                }
                current = current->next;
                next = next->next;
            }
            if (current->scope == scope) {
                if (current->prev == NULL) {
                    table->entry[0] = NULL;
                } else {
                    current->prev->next = NULL;
                }
            }
        }
    }
}
// ==========================================

// ============= Printing SymTable =============
char *typeBuffer(ExtType *type) {
    if (type == NULL) {
        return NULL;
    }

    char *strBuffer = malloc(sizeof(char) * 20);

    ExtType *extType = type;
    memset(strBuffer, 0, sizeof(strBuffer));

    switch (extType->type) {
        case INTEGER_t:
            strncpy(strBuffer, "int", 3);
            break;
        case FLOAT_t:
            strncpy(strBuffer, "float", 5);
            break;
        case DOUBLE_t:
            strncpy(strBuffer, "double", 6);
            break;
        case BOOLEAN_t:
            strncpy(strBuffer, "bool", 4);
            break;
        case STRING_t:
            strncpy(strBuffer, "string", 6);
            break;
        case VOID_t:
            strncpy(strBuffer, "void", 4);
            break;
        default:
            strncpy(strBuffer, "unknown", 7);
            break;
    }

    if (extType->isArray) {
        char strTemp[20];
        ArrayDimNode *ptrr = extType->dim;
        while (ptrr != NULL) {
            memset(strTemp, 0, sizeof(strTemp));
            sprintf(strTemp, "[%d]", ptrr->size);
            if (strlen(strBuffer) + strlen(strTemp) < 20)
                strcat(strBuffer, strTemp);
            else {
                strBuffer[16] = '.';
                strBuffer[17] = '.';
                strBuffer[18] = '.';
            }
            ptrr = ptrr->next;
        }
    }
    return strBuffer;
}

void printAttr(SymNode *ptr) {
    if (ptr->kind == FUNCTION_t) {
        int parameterCounts = ptr->attribute->funcParam->paramNum;
        ExtTypePointer *extTypePtr = (ptr->attribute->funcParam->params);

        char *attrBuffer = malloc(sizeof(char) * 30);
        memset(attrBuffer, 0, sizeof(attrBuffer));

        for (int i = 0; i < parameterCounts; i++) {
            char *printType = typeBuffer(extTypePtr->value);
            strcat(attrBuffer, printType);
            if (i < parameterCounts - 1) {
                // not the tail of parameter list.
                strcat(attrBuffer, ",");
            }
            extTypePtr = (extTypePtr->next);
        }
        printf("%s", attrBuffer);
    } else if (ptr->kind == CONSTANT_t) {
        // print constant literal
        switch (ptr->attribute->constVal->type) {
            case INTEGER_t:
                printf("%d", ptr->attribute->constVal->value.integerVal);
                break;
            case FLOAT_t:
                printf("%lf", ptr->attribute->constVal->value.floatVal);
                break;
            case DOUBLE_t:
                printf("%lf", ptr->attribute->constVal->value.doubleVal);
                break;
            case BOOLEAN_t:
                if (ptr->attribute->constVal->value.booleanVal == true)
                    printf("true");
                else
                    printf("false");
                break;
            case STRING_t:
                printf("%s", ptr->attribute->constVal->value.stringVal);
                break;
            default:
                printf("Unkown error occured.");
                break;
        }
    }
}

void printSymTable(SymTable *table, int scope) {
    for (int i = 0; i < 100; i++) printf("=");
    printf("\n");
    printf("%s", "Name");
    for (int i = 0; i < 29; i++) printf(" ");
    printf("%s", "Kind");
    for (int i = 0; i < 7; i++) printf(" ");
    printf("%s", "Level");
    for (int i = 0; i < 7; i++) printf(" ");
    printf("%s", "Type");
    for (int i = 0; i < 15; i++) printf(" ");
    printf("%s", "Attribute");
    printf("\n");
    for (int i = 0; i < 100; i++) printf("-");
    printf("\n");
    for (int i = 0; i < MAX_ENTRY; i++) {
        SymNode *ptr = table->entry[i];
        while (ptr != NULL) {
            if (ptr->scope == scope) {
                printf("%-32s ", ptr->name);
                switch (ptr->kind) {
                    case VARIABLE_t:
                        printf("variable  ");
                        break;
                    case CONSTANT_t:
                        printf("constant  ");
                        break;
                    case FUNCTION_t:
                        printf("function  ");
                        break;
                    case PARAMETER_t:
                        printf("parameter ");
                        break;
                    default:
                        printf("unknown   ");
                        break;
                }

                if (ptr->scope == 0) {
                    printf(" 0(global)   ");
                } else {
                    printf("%2d(local)    ", ptr->scope);
                }
                char *printType = typeBuffer(ptr->type);
                printf("%-19s", printType);
                printAttr(ptr);
                printf("\n");
            }
            ptr = ptr->next;
        }
    }
    for (int i = 0; i < 100; i++) printf("=");
    printf("\n");
}
// =============================================

// ============= Create SymNodes  =============
SymNode *createNode(char *name, int scope, ExtType *scalarType, KIND kind,
                    void *attr) {
    SymNode *newNode = malloc(sizeof(SymNode));
    // set node's name
    newNode->name = malloc(sizeof(char) * 32);
    memset(newNode->name, 0, sizeof(newNode->name));
    strcpy(newNode->name, name);
    // set node's scope
    if (kind == FUNCTION_t)
        newNode->scope = 0;
    else
        newNode->scope = scope;
    // set node's scalar type
    newNode->type = scalarType;
    // set node's kind
    newNode->kind = kind;
    // set node's attribute, if any
    switch (kind) {
        case FUNCTION_t:
            newNode->attribute = malloc(sizeof(SymAttr));
            newNode->attribute->funcParam = (FuncAttr *)attr;
            break;
        case PARAMETER_t:
            newNode->attribute = NULL;
            break;
        case CONSTANT_t:
            newNode->attribute = malloc(sizeof(SymAttr));
            newNode->attribute->constVal = (ConstAttr *)attr;
            break;
        case VARIABLE_t:
            newNode->attribute = NULL;
            break;
        default:
            newNode->attribute = NULL;
            break;
    }

    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

ConstAttr *createConstantAttribute(BTYPE type, void *value) {
    ConstAttr *newConstAttr = malloc(sizeof(ConstAttr));
    newConstAttr->type = type;
    newConstAttr->isNegative = false;

    switch (type) {
        case INTEGER_t:
            newConstAttr->value.integerVal = *(int *)value;
            if (*(int *)value < 0) {
                newConstAttr->isNegative = true;
            }
            break;
        case FLOAT_t:
            newConstAttr->value.floatVal = *(float *)value;
            if (*(float *)value < 0.0) {
                newConstAttr->isNegative = true;
            }
            break;
        case STRING_t:
            newConstAttr->value.stringVal =
                malloc(sizeof(char) * (strlen((char *)value) + 1));
            strcpy(newConstAttr->value.stringVal, (char *)value);
            break;
        case BOOLEAN_t:
            newConstAttr->value.booleanVal = *(int *)value;
            break;
        case DOUBLE_t:
            newConstAttr->value.doubleVal = *(double *)value;
            if (*(double *)value < 0.0) {
                newConstAttr->isNegative = true;
            }
            break;
    }
    return newConstAttr;
}

constParam *createConstParam(ConstAttr *constNode, char *id) {
    constParam *newConstPar = malloc(sizeof(constParam));
    newConstPar->value = constNode;
    newConstPar->name = malloc(sizeof(char) * (strlen(id) + 1));
    strcpy(newConstPar->name, id);
    newConstPar->next = 0;

    return newConstPar;
}

ParamSemantic *createParam(IDNodeSemantic *ids, ExtType *extType) {
    ParamSemantic *newPar = malloc(sizeof(ParamSemantic));
    newPar->idlist = ids;
    newPar->extType = extType;
    newPar->next = 0;

    return newPar;
}

VarDecl *createVarDecl(ParamSemantic *par, ExprSemantic *exp) {
    VarDecl *newVarDecl = malloc(sizeof(VarDecl));
    newVarDecl->para = par;
    newVarDecl->expr = exp;
    newVarDecl->isArray = false;
    newVarDecl->isInit = false;

    return newVarDecl;
}

ExprSemantic *createExprSem(char *id) {
    ExprSemantic *result = malloc(sizeof(ExprSemantic));

    result->startOperator = NONE_t;

    result->isDeref = false;

    result->varRef = malloc(sizeof(var_ref_sem));
    if (id != NULL) {
        result->varRef->id = malloc(sizeof(char) * (strlen(id) + 1));
        strcpy(result->varRef->id, id);
    }
    result->varRef->dimNum = 0;
    result->varRef->dim = NULL;
    result->extType = NULL;
    result->next = NULL;
    return result;
}

ExtType *createExtType(BTYPE type) {
    ExtType *result = malloc(sizeof(ExtType));
    result->type = type;
    result->dimNum = 0;
    result->dim = 0;
    result->isError = false;
    result->isArray = false;

    return result;
}

IDNodeSemantic *createIdList(char *str) {
    IDNodeSemantic *newNode = malloc(sizeof(IDNodeSemantic));
    newNode->value = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(newNode->value, str);
    newNode->next = 0;

    return newNode;
}

void concateParam(char *type, void *left, void *right) {
    if (strcmp(type, "ParamSem") == 0) {
        ParamSemantic *ptr = (ParamSemantic *)left;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = malloc(sizeof(ParamSemantic));
        ptr->next = (ParamSemantic *)right;
        left = ptr;
    }
    if (strcmp(type, "VarDecl") == 0) {
        VarDecl *ptr = (VarDecl *)left;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = malloc(sizeof(VarDecl));
        ptr->next = (VarDecl *)right;
        left = ptr;
    }
    if (strcmp(type, "ConstPar") == 0) {
        constParam *ptr = (constParam *)left;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = malloc(sizeof(constParam));
        ptr->next = (constParam *)right;
        left = ptr;
    }
}

void increaseDim(ExprSemantic *expr, BTYPE dimType) {
    typeNode *newNode = malloc(sizeof(typeNode));
    newNode->value = dimType;
    newNode->next = 0;

    if (expr->varRef->dim == 0) {
        expr->varRef->dimNum += 1;
        expr->varRef->dim = newNode;
    } else {
        typeNode *typePtr = expr->varRef->dim;
        while (typePtr->next != NULL) {
            typePtr = typePtr->next;
        }

        typePtr->next = newNode;
        expr->varRef->dimNum += 1;
    }
}

void increaseArrayDim(ExtType *extType, int size) {
    if (extType->isArray == false) extType->isArray = true;

    extType->dimNum += 1;

    ArrayDimNode *newDim = malloc(sizeof(ArrayDimNode));
    newDim->size = size;
    newDim->next = 0;

    if (extType->dim == 0)
        extType->dim = newDim;
    else {
        ArrayDimNode *ptrr = extType->dim;
        while (ptrr->next != NULL) {
            ptrr = ptrr->next;
        }
        ptrr->next = newDim;
    }
}

ExtType *copyExtType(ExtType *src) {
    if (src == NULL) {
        return NULL;
    } else {
        ExtType *dest = malloc(sizeof(ExtType));
        dest->isError = src->isError;
        dest->isArray = src->isArray;
        dest->type = src->type;
        dest->dimNum = src->dimNum;
        dest->dim = copyArrayDimList(src->dim);
        return dest;
    }
}
// ============================================