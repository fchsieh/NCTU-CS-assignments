#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern bool semError;
extern int linenum;

//#define DEBUG

SymTable *symTableInit() {
    SymTable *table = malloc(sizeof(SymNode) * MAX_ENTRY);
    table->tail = 0;
    return table;
}

void insertIntoSymTable(SymTable *table, SymNode *newNode) {
    int location = 0;
    if (table->entry[location] == 0) {  // the first
        table->entry[location] = newNode;
    } else {
        SymNode *nodePtr;
        for (nodePtr = table->entry[location]; nodePtr->next != NULL;
             nodePtr = nodePtr->next)
            ;
        nodePtr->next = newNode;
        newNode->prev = nodePtr;
    }
    table->tail += 1;
#ifdef DEBUG
    printf("After insert, current tail: %d\n", table->tail);
#endif
}

ConstAttr *createConstAttr(TYPE type, ConstVal *value) {
    ConstAttr *result = malloc(sizeof(ConstAttr));
    result->value = malloc(sizeof(ConstVal));
    result->kind = type;
    switch (type) {
        case INTEGER_t:
            result->value->intVal = value->intVal;
            break;
        case FLOAT_t:
            result->value->floatVal = value->floatVal;
            break;
        case DOUBLE_t:
            result->value->doubleVal = value->doubleVal;
            break;
        case BOOLEAN_t:
            result->value->boolVal = value->boolVal;
            break;
        case STRING_t:
            result->value->stringVal =
                malloc(sizeof(char) * (strlen(value->stringVal) + 1));
            strcpy(result->value->stringVal, value->stringVal);
            break;
        default:
            break;
    }
    return result;
}

ConstParam *createConstParam(ConstAttr *constNode, char *id) {
    ConstParam *result = malloc(sizeof(ConstParam));
    result->value = constNode;
    result->name = malloc(sizeof(char) * 32);
    memset(result->name, 0, sizeof(result->name));
    strcpy(result->name, id);
    result->next = 0;
    return result;
}

SymNode *createConstNode(char *id, int scope, ParType *parType,
                         ConstAttr *constAttr) {
    SymNode *newNode = malloc(sizeof(SymNode));
    // set name
    newNode->name = malloc(sizeof(char) * 32);
    memset(newNode->name, 0, sizeof(newNode->name));
    strcpy(newNode->name, id);
    // set scope
    newNode->scope = scope;
    // setup type
    newNode->type = parType;
    // set kind
    newNode->kind = CONSTANT_t;
    // set attr
    newNode->attr = malloc(sizeof(SymAttr));
    newNode->attr->constVal = constAttr;

    newNode->next = 0;
    newNode->prev = 0;
    return newNode;
}

ParType *createParamType(TYPE type) {
    ParType *result = malloc(sizeof(ParType));
    result->type = type;
    result->dimNum = 0;
    result->dim = NULL;
    return result;
}

bool IDRedecl(SymTable *table, int scope, char *id) {
    SymNode *ptr;
    for (int i = 0; i < table->tail; i++) {
        for (ptr = table->entry[i]; ptr != NULL; ptr = ptr->next) {
            if (ptr->scope == scope &&
                strcmp(ptr->name, id) == 0) {  // search in same scope
                return true;
            }
        }
    }
    return false;  // not found, not yet declared
}

char *typeBuffer(ParType *type) {
    // concat array dimensions
    char *buffer = malloc(sizeof(char) * 100);
    memset(buffer, 0, sizeof(buffer));
    ParType *parType = type;

    switch (parType->type) {
        case INTEGER_t:
            strcat(buffer, "int");
            break;
        case FLOAT_t:
            strcat(buffer, "float");
            break;
        case DOUBLE_t:
            strcat(buffer, "double");
            break;
        case BOOLEAN_t:
            strcat(buffer, "bool");
            break;
        case STRING_t:
            strcat(buffer, "string");
            break;
        case VOID_t:
            strcat(buffer, "void");
            break;
        default:
            break;
    }

    int i;
    ArrDim *arrPtr;
    for (i = 0, arrPtr = parType->dim; i < parType->dimNum;
         i++, arrPtr = arrPtr->next) {
        char *buf = malloc(sizeof(char) * 30);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "[%d]", arrPtr->size);
        strcat(buffer, buf);
    }
    return buffer;
}

void printAttr(KIND kind, SymNode *ptr) {
    if (kind == FUNCTION_t) {
        int i;
        ParTypeList *parTypePtr;
        for (i = 0, parTypePtr = ptr->attr->paramList->params;
             i < ptr->attr->paramList->parNum;
             i++, parTypePtr = parTypePtr->next) {
            char *buf = typeBuffer(parTypePtr->value);
            printf("%s", buf);
            if (i < ptr->attr->paramList->parNum - 1) {
                printf(",");
            }
        }
    } else if (kind == CONSTANT_t) {
        switch (ptr->attr->constVal->kind) {
            case INTEGER_t:
                printf("%d", ptr->attr->constVal->value->intVal);
                break;
            case FLOAT_t:
                printf("%lf", ptr->attr->constVal->value->floatVal);
                break;
            case DOUBLE_t:
                printf("%lf", ptr->attr->constVal->value->doubleVal);
                break;
            case BOOLEAN_t:
                if (ptr->attr->constVal->value->boolVal == true)
                    printf("true");
                else
                    printf("false");
                break;
            case STRING_t:
                printf("%s", ptr->attr->constVal->value->stringVal);
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
    SymNode *ptr;
    for (int i = 0; i < MAX_ENTRY; i++) {
        for (ptr = table->entry[i]; ptr != NULL; ptr = ptr->next) {
            if (ptr->scope == scope) {
                printf("%-32s ", ptr->name);
                switch (ptr->kind) {
                    case FUNCTION_t:
                        printf("function  ");
                        break;
                    case PARAMETER_t:
                        printf("parameter ");
                        break;
                    case CONSTANT_t:
                        printf("constant  ");
                        break;
                    case VARIABLE_t:
                        printf("variable  ");
                        break;
                    default:
                        break;
                }

                if (ptr->scope == 0) {
                    printf(" 0(global)   ");
                } else {
                    printf("%2d(local)    ", ptr->scope);
                }
                char *buf = typeBuffer(ptr->type);
                printf("%-19s", buf);
                printAttr(ptr->kind, ptr);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < 100; i++) printf("=");
    printf("\n");
}

IDNode *createIDList(char *id) {
    IDNode *newNode = malloc(sizeof(IDNode));
    newNode->value = malloc(sizeof(char) * 33);
    memset(newNode->value, 0, sizeof(newNode->value));
    strcpy(newNode->value, id);
    newNode->next = NULL;
    return newNode;
}

Params *createParam(IDNode *idlist, ParType *parType) {
    Params *newPar = malloc(sizeof(Params));
    newPar->idlist = idlist;
    newPar->next = NULL;
    newPar->parType = parType;
    return newPar;
}

VarParam *createVarParam(Params *par) {
    VarParam *newPar = malloc(sizeof(VarParam));
    newPar->para = par;
    return newPar;
}

SymNode *createVarNode(char *id, int scope, ParType *parType,
                       ParType *retType) {
    SymNode *newNode = malloc(sizeof(SymNode));
    // set name
    newNode->name = malloc(sizeof(char) * 32);
    memset(newNode->name, 0, sizeof(newNode->name));
    strcpy(newNode->name, id);
    // set scope
    newNode->scope = scope;
    // set type
    newNode->type = parType;
    if (newNode->type->type == VOID_t) {
        // set real return type
        newNode->type->type = retType->type;
    }
    // set KIND
    newNode->kind = VARIABLE_t;
    // no attr for variable
    newNode->attr = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void deleteScopeNode(SymTable *table, int scope) {
    for (int i = 0; i < table->tail; i++) {
        if (table->entry[i] == NULL) {
            // empty node, don't need to delete
            continue;
        } else if (table->entry[i]->next == NULL) {
            if (table->entry[i]->scope == scope) {
                table->entry[i] = NULL;
                table->tail -= 1;
            }
        } else {
            SymNode *current, *prev;
            for (prev = (table->entry[i]), current = (table->entry[i]->next);
                 current != NULL; prev = current, current = (current->next)) {
                if (prev->scope == scope) {
                    if (prev->prev == NULL) {
                        prev->next->prev = 0;
                        table->entry[i] = current;
                        table->tail -= 1;
                    } else {
                        prev->prev->next = current;
                        current->prev = prev->prev;
                        table->tail -= 1;
                    }
                }
            }
            if (prev->scope == scope) {
                if (prev->prev == NULL) {
                    table->entry[i] = NULL;
                } else {
                    prev->prev->next = NULL;
                }
            }
        }
    }
#ifdef DEBUG
    printf("After delete, current tail: %d\n", table->tail);
#endif
}

void increaseArrayDim(ParType *parType, int dimSize) {
    parType->dimNum = parType->dimNum + 1;
    ArrDim *newDim = malloc(sizeof(ArrDim));
    newDim->size = dimSize;
    newDim->next = NULL;
    ArrDim *arrPtr;
    if (parType->dim == NULL) {
        parType->dim = newDim;
    } else {
        for (arrPtr = parType->dim; arrPtr->next != NULL; arrPtr = arrPtr->next)
            ;
        arrPtr->next = newDim;
    }
}

void insertFuncNode(SymTable *table, char *id, Params *par, ParType *retType) {
    FuncAttr *paramList = malloc(sizeof(FuncAttr));
    paramList->params = NULL;
    paramList->parNum = 0;
    if (par != NULL) {
        Params *parPtr;
        IDNode *idPtr;
        ParTypeList *lastPtr = 0;
        for (parPtr = par; parPtr != NULL; parPtr = parPtr->next) {
            for (idPtr = parPtr->idlist; idPtr != NULL; idPtr = idPtr->next) {
                if (paramList->parNum == 0) {
                    paramList->params = malloc(sizeof(ParTypeList));
                    paramList->params->value = copyParType(parPtr->parType);
                    paramList->params->next = NULL;
                    paramList->parNum = paramList->parNum + 1;
                    lastPtr = paramList->params;
                } else {
                    ParTypeList *ParTypeListPtr = malloc(sizeof(ParTypeList));
                    ParTypeListPtr->value = copyParType(parPtr->parType);
                    ParTypeListPtr->next = NULL;
                    lastPtr->next = ParTypeListPtr;
                    paramList->parNum = paramList->parNum + 1;
                    lastPtr = lastPtr->next;
                }
            }
        }
    }
    SymNode *newNode = createFuncNode(id, retType, paramList);
    insertIntoSymTable(table, newNode);
}

ParType *copyParType(ParType *pt) {
    if (pt == NULL)
        return NULL;
    else {
        ParType *parPtr = malloc(sizeof(ParType));
        parPtr->type = pt->type;
        parPtr->dimNum = pt->dimNum;
        if (pt->dim == NULL) {
            parPtr->dim = NULL;
        } else {
            ArrDim *dest = malloc(sizeof(ArrDim));
            dest->size = pt->dim->size;
            dest->next = 0;

            ArrDim *ptr = dest;
            ArrDim *arrPtr;
            for (arrPtr = pt->dim->next; arrPtr != NULL;
                 arrPtr = arrPtr->next) {
                ArrDim *newNode = malloc(sizeof(ArrDim));
                newNode->size = arrPtr->size;
                newNode->next = 0;
                ptr->next = newNode;
                ptr = newNode;
            }
            parPtr->dim = dest;
        }
        return parPtr;
    }
}

SymNode *createFuncNode(char *id, ParType *retType, FuncAttr *params) {
    SymNode *newNode = malloc(sizeof(SymNode));
    // set function identifier
    newNode->name = malloc(sizeof(char) * 32);
    memset(newNode->name, 0, sizeof(newNode->name));
    strcpy(newNode->name, id);
    // set function scope (global)
    newNode->scope = 0;
    // set function return type
    newNode->type = retType;
    // kind is FUNCTION
    newNode->kind = FUNCTION_t;
    // set parameters
    newNode->attr = malloc(sizeof(SymAttr));
    newNode->attr->paramList = params;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void combineDeclare(KIND kind, void *left, void *right) {
    if (kind == VARIABLE_t) {
        VarParam *ptr;
        VarParam *ptrRight = (VarParam *)right;
        for (ptr = left; ptr->next != NULL; ptr = ptr->next)
            ;
        ptr->next = ptrRight;
    } else if (kind == CONSTANT_t) {
        ConstParam *ptr;
        ConstParam *ptrRight = (ConstParam *)right;
        for (ptr = left; ptr->next != NULL; ptr = ptr->next)
            ;
        ptr->next = ptrRight;
    } else if (kind == PARAMETER_t) {
        Params *ptr;
        Params *ptrRight = (Params *)right;
        for (ptr = left; ptr->next != NULL; ptr = ptr->next)
            ;
        ptr->next = ptrRight;
    }
}

void insertParamNode(SymTable *table, Params *params, int scope) {
    Params *ptr;
    IDNode *idPtr;
    for (ptr = params; ptr != NULL; ptr = ptr->next) {
        for (idPtr = ptr->idlist; idPtr != NULL; idPtr = idPtr->next) {
            if (IDRedecl(table, scope, idPtr->value) == false) {
                // check if is redeclared
                SymNode *newNode =
                    createParNode(idPtr->value, scope, ptr->parType);
                insertIntoSymTable(table, newNode);
            } else {
                printError(REDECL_t, linenum, idPtr->value);
                semError = true;
            }
        }
    }
}

SymNode *createParNode(char *id, int scope, ParType *type) {
    SymNode *newNode = malloc(sizeof(SymNode));
    // set scope
    newNode->scope = scope;
    // set name
    newNode->name = malloc(sizeof(char) * 32);
    memset(newNode->name, 0, sizeof(newNode->name));
    strcpy(newNode->name, id);
    // set node type
    newNode->type = type;
    // kind is parameter
    newNode->kind = PARAMETER_t;
    newNode->attr = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void printError(ERRTYPE err, int errorLine, char *id) {
    // for future lab
    switch (err) {
        case REDECL_t:
            printf(
                "########## Error at Line #%d: identifier '%s' is redeclared. "
                "##########\n",
                errorLine, id);
            break;
        default:
            break;
    }
}
