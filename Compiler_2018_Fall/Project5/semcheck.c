#include "semcheck.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "symtable.h"

bool checkCoercion(BTYPE decl, BTYPE def) {
    // define type should follow declared type
    // For coercion: double > float > int
    // printf("Line: %d Master: %d\tSlave: %d\n", linenum, decl, def);
    if (decl == DOUBLE_t && (def == INTEGER_t || def == FLOAT_t)) return true;
    if (decl == FLOAT_t && def == INTEGER_t) return true;
    // other types coercion are not allowed
    return false;
}

SymNode *lookupSymbol(SymTable *table, char *id, int scope, bool currentScope) {
    SymNode *nodePtr = table->entry[0];
    while (nodePtr != NULL) {
        if (strcmp(nodePtr->name, id) == 0 && nodePtr->scope == scope) {
            return nodePtr;  // node found!
        }
        nodePtr = nodePtr->next;  // not found, keep searching in current scope.
    }

    if (scope == 0)  // node not found in global scope, doesn't exists.
        return NULL;
    else {  // current is not the global scope,  keep searching.
        if (currentScope == true) {
            // not found in current scope
            return NULL;
        } else {
            // check outer scope
            return lookupSymbol(table, id, scope - 1, false);
        }
    }
}

ArrayDimNode *copyArrayDimList(ArrayDimNode *src) {
    if (src == NULL) {
        return NULL;
    } else {
        ArrayDimNode *dest = malloc(sizeof(ArrayDimNode));
        dest->size = src->size;
        dest->next = 0;

        ArrayDimNode *ptr = dest;
        ArrayDimNode *arrPtr = src->next;
        while (arrPtr != NULL) {
            ArrayDimNode *newNode = malloc(sizeof(ArrayDimNode));
            newNode->size = arrPtr->size;
            newNode->next = 0;

            ptr->next = newNode;
            ptr = newNode;
            arrPtr = arrPtr->next;
        }
        return dest;
    }
}

BTYPE checkArrayIndex(ExprSemantic *expr) {
    // array index must be integer type
    // printf("%d: %d\n", linenum, expr->startOperator);
    if (expr->isDeref == false || expr->extType->isArray == true ||
        expr->extType->type != INTEGER_t) {
        printError(ArrayIndexNotInt);
        return ERROR_t;
    } else if (traversalCheckIndex(expr) == false) {
        printError(ArrayIndexNotInt);
        return ERROR_t;
    } else {
        return INTEGER_t;
    }
}
bool traversalCheckIndex(ExprSemantic *arr_expr) {
    if (arr_expr->startOperator == NONE_t)
        return true;
    else
        return false;
}

bool checkRedeclaration(SymTable *table, char *str, int scope) {
    if (lookupSymbol(table, str, scope, true) == NULL) {
        // not found in the symbol table.
        return true;
    } else {
        printError(IDreused);
        return false;
    }
}

bool checkExistence(SymTable *table, ExprSemantic *expr, int scope,
                    bool isAssign) {
    SymNode *node = lookupSymbol(table, expr->varRef->id, scope, false);

    if (node == NULL) {
        printError(IDNotDecl);
        expr->extType = createExtType(ERROR_t);
        return false;
    } else {
        expr->isDeref = true;

        if (node->kind == FUNCTION_t) {
            printError(NodeIsFunction);
            expr->extType = createExtType(ERROR_t);
            return false;
        } else if (node->kind == CONSTANT_t && isAssign == true) {
            printError(ConstAssignment);
            expr->extType = createExtType(ERROR_t);
            return false;
        } else {
            if (expr->varRef->dimNum == 0) {
                expr->extType = copyExtType(node->type);
            } else {
                if (node->type->dimNum < expr->varRef->dimNum) {
                    printError(ReferenceError);
                    expr->extType = createExtType(ERROR_t);
                    return false;
                } else if (node->type->dimNum == expr->varRef->dimNum) {
                    expr->extType = createExtType(node->type->type);
                } else {
                    expr->extType = createExtType(node->type->type);
                    expr->extType->isArray = true;
                    expr->extType->dimNum =
                        (node->type->dimNum) - (expr->varRef->dimNum);

                    int i = 0;
                    ArrayDimNode *arrPtr = node->type->dim;
                    while (i < expr->varRef->dimNum) {
                        i++;
                        arrPtr = arrPtr->next;
                    }
                    expr->extType->dim = copyArrayDimList(arrPtr);
                }
            }
        }
    }
    return true;
}

void checkUnaryMinus(ExprSemantic *expr) {
    expr->startOperator = SUB_t;
    BTYPE type = expr->extType->type;
    if (expr->isDeref == false) {
        printError(UnaryOperandShouldBeReal);
        expr->isDeref = true;
        expr->extType->type = ERROR_t;
    } else {
        if (!((type == INTEGER_t) || (type == FLOAT_t) || (type == DOUBLE_t))) {
            printError(UnaryOperandShouldBeReal);
            expr->extType->type = ERROR_t;
        }
    }
}

void checkUnaryNOT(ExprSemantic *expr) {
    expr->startOperator = NOT_t;

    if (expr->extType->dimNum != 0) {
        printError(StatementIsArrType);
        expr->extType->type = ERROR_t;
    } else if (expr->extType->type != BOOLEAN_t) {
        printError(StatementNotBool);
        expr->extType->type = ERROR_t;
    } else {
        expr->extType->type = BOOLEAN_t;
    }
}

void checkAssignmentTypeMatch(ExprSemantic *LHS, ExprSemantic *RHS) {
    if ((LHS->extType->type) == ERROR_t || (RHS->extType->type) == ERROR_t) {
        // printError(OtherErr);
        // some error occured
    } else if (LHS->extType->type != RHS->extType->type) {
        // different type, check coercion
        if (checkCoercion(LHS->extType->type, RHS->extType->type) == false) {
            printError(AssignmentTypeMismatch);
        }
    } else {
        // same type, pass
    }
    if ((LHS->extType->dimNum > 0) || (RHS->extType->dimNum > 0)) {
        printError(ArrTypeAssignment);
    }
}

bool checkVarInitValue(ExtType *scalar, VarDecl *var, SymTable *table,
                       int scope) {
    var->para->extType->type = scalar->type;
    if (var->isInit == true) {
        if (var->isArray == true) {
            return checkArrayInitVal(scalar, var, table, scope);
        } else {
            if (var->expr->isDeref == false) {
                if (checkExistence(table, var->expr, scope, false) == false) {
                    return false;
                }
            }
            if (var->expr->extType->type != scalar->type) {
                if (checkCoercion(var->expr->extType->type, scalar->type) ==
                    false) {
                    printError(InitVarTypeErr);
                    return false;

                } else if (var->expr->extType->isArray == true) {
                    printError(InitVarTypeErr);
                    return false;
                }
            } else if (var->expr->extType->isArray == true) {
                printError(InitVarTypeErr);
                return false;
            }
        }
    }
    return true;
}

bool checkArrayInitVal(ExtType *scalar, VarDecl *var, SymTable *table,
                       int scope) {
    ArrayDimNode *ptr = var->para->extType->dim;
    int elem = 1;
    while (ptr != NULL) {
        elem = elem * (ptr->size > 0 ? ptr->size : 1);
        ptr = ptr->next;
    }
    int initElem = 0;

    ExprSemantic *exprPtr = var->expr;
    while (exprPtr != NULL) {
        initElem++;
        if (initElem > elem) {
            var->para->extType->type = ERROR_t;
            printError(InitVarTypeErr);
            return false;
        }
        if (exprPtr->isDeref == false) {
            if (checkExistence(table, exprPtr, scope, false) == false) {
                return false;
            }
        }
        if (exprPtr->extType->type == ERROR_t) {
            var->para->extType->type = ERROR_t;
            return false;
        } else if (exprPtr->extType->isArray == true) {
            var->para->extType->type = ERROR_t;
            printError(InitVarTypeErr);
            return false;
        } else if (var->para->extType->type != exprPtr->extType->type) {
            if (checkCoercion(exprPtr->extType->type, scalar->type)) {
                printError(InitVarTypeErr);
                return false;
            }
        }
        exprPtr = exprPtr->next;
    }

    return true;
}

void checkArithmeticOp(ExprSemantic *op1, OPERATOR operator,
                       ExprSemantic *op2) {
    BTYPE type1 = op1->extType->type;
    BTYPE type2 = op2->extType->type;
    if (type1 == ERROR_t || type2 == ERROR_t) {
        // printf("Hi\n");
        // printError(OtherErr);
        op1->extType->type = ERROR_t;
    } else if (op2->startOperator != NONE_t) {
        // printf("OP1: %d\tOP2: %d\n", type1, type2);
        printError(OtherErr);
        op1->extType->type = ERROR_t;
    } else if (op1->extType->dimNum != 0 || op2->extType->dimNum != 0) {
        // array type can't do operations
        printError(StatementIsArrType);
        op1->extType->type = ERROR_t;
    } else {
        if (type1 == STRING_t || type2 == STRING_t) {
            // string type can't do operations
            printError(StatementIsString);
        } else if ((type1 == INTEGER_t && type2 == INTEGER_t) ||
                   (type1 == FLOAT_t && type2 == FLOAT_t) ||
                   (type1 == DOUBLE_t && type2 == DOUBLE_t)) {
            // both are integers/float/double, pass
        } else if ((type1 == INTEGER_t && type2 == FLOAT_t) ||
                   (type1 == FLOAT_t && type2 == INTEGER_t)) {
            // convert type1 to float
            if (type1 != FLOAT_t) {
                op1->extType->type = FLOAT_t;
            } else if (type2 != FLOAT_t) {
                // convert type2 to float
                op2->extType->type = FLOAT_t;
            }
        } else if (((type1 == INTEGER_t || type1 == FLOAT_t) &&
                    type2 == DOUBLE_t) ||
                   ((type2 == INTEGER_t || type2 == FLOAT_t) &&
                    type1 == DOUBLE_t)) {
            // convert type1 to double
            if (type1 != DOUBLE_t) {
                op1->extType->type = DOUBLE_t;
            } else if (type2 != DOUBLE_t) {
                // convert type2 to double
                op2->extType->type = DOUBLE_t;
            }
        } else {
            printError(OperandTypeMismatch);
            op1->extType->type = ERROR_t;
        }
    }
}

void checkRelOp(ExprSemantic *op1, OPERATOR operator, ExprSemantic *op2) {
    BTYPE type1 = op1->extType->type;
    BTYPE type2 = op2->extType->type;
    if (type1 == ERROR_t || type2 == ERROR_t) {
        // some other errors
        op1->extType->type = ERROR_t;
    } else if (op2->startOperator != NONE_t) {
        printError(OtherErr);
        op1->extType->type = ERROR_t;
    } else if ((op1->extType->dimNum) != 0 || (op2->extType->dimNum) != 0) {
        printError(StatementIsArrType);
        op1->extType->type = ERROR_t;
    } else if (!((type1 == INTEGER_t || type1 == FLOAT_t ||
                  type1 == DOUBLE_t) &&
                 (type2 == INTEGER_t || type2 == FLOAT_t ||
                  type2 == DOUBLE_t))) {
        if (type1 == BOOLEAN_t && type2 == BOOLEAN_t &&
            (operator== EQ_t || operator== NE_t)) {
            // boolean can only do comparison, no other operations
        } else {
            // other type can't do operations
            printError(OperandTypeMismatch);
            op1->extType->type = ERROR_t;
        }
    } else {
        // pass verification
        op1->extType->type = BOOLEAN_t;
    }
}

void checkAndOrOp(ExprSemantic *op1, OPERATOR operator, ExprSemantic *op2) {
    if ((op1->extType->type) == ERROR_t || (op2->extType->type) == ERROR_t) {
        op1->extType->type = ERROR_t;
    } else if ((op1->extType->type) != BOOLEAN_t ||
               (op2->extType->type) != BOOLEAN_t) {
        // shoud be boolean type for AND/OR
        printError(OperandTypeMismatch);
        op1->extType->type = ERROR_t;
    } else if ((op1->extType->dimNum) != 0 || (op2->extType->dimNum) != 0) {
        // array type can't do AND/OR
        printError(StatementIsArrType);
        op1->extType->type = ERROR_t;
    } else if (op2->startOperator != NONE_t) {
        printError(OtherErr);
        op1->extType->type = ERROR_t;
    } else {
        op1->extType->type = BOOLEAN_t;
    }
}

ExprSemantic *checkFuncInvoke(char *id, ExprSemantic *exprList, SymTable *table,
                              int scope) {
    ExprSemantic *result = malloc(sizeof(ExprSemantic));

    result->startOperator = NONE_t;
    result->isDeref = true;
    result->varRef = 0;
    result->next = 0;

    SymNode *node = lookupSymbol(table, id, 0, false);

    if (node == NULL) {
        printError(IDNotDecl);
        result->extType = createExtType(ERROR_t);
    } else if (node->kind != FUNCTION_t) {
        printError(NodeIsNotFunction);
        result->extType = createExtType(ERROR_t);
    } else {
        if (node->attribute->funcParam->paramNum == 0) {
            if (exprList != NULL) {
                // declare has no param, but invoked has pass parameter to
                // it
                printError(ParCountMismatch);
                result->extType = createExtType(ERROR_t);
            } else {
                result->extType = createExtType(node->type->type);
            }
        } else {
            bool mismatch = false;
            ExtTypePointer *typePtr = node->attribute->funcParam->params;
            ExprSemantic *exprPtr = exprList;
            while (typePtr != NULL && exprPtr != NULL) {
                if (typePtr->value->type != exprPtr->extType->type) {
                    // printf("lhs %d\trhs %d\n",
                    // typePtr->value->type,exprPtr->extType->type);
                    if (checkCoercion(typePtr->value->type,
                                      exprPtr->extType->type) == false) {
                        mismatch = true;
                    }
                }

                if (typePtr->value->dimNum != exprPtr->extType->dimNum) {
                    mismatch = true;
                } else {
                    ArrayDimNode *lhsPtr = typePtr->value->dim;
                    ArrayDimNode *rhsPtr = exprPtr->extType->dim;
                    while (lhsPtr != NULL) {
                        if (lhsPtr->size != rhsPtr->size) {
                            mismatch = true;
                            break;
                        }
                        lhsPtr = lhsPtr->next;
                        rhsPtr = rhsPtr->next;
                    }
                }
                typePtr = typePtr->next;
                exprPtr = exprPtr->next;
            }
            if (mismatch == true) {
                printError(ParTypeMismatch);
                result->extType = createExtType(ERROR_t);
            } else {
                if (typePtr != 0) {
                    printError(ParCountMismatch);
                    result->extType = createExtType(ERROR_t);
                } else if (exprPtr != 0) {
                    printError(ParCountMismatch);
                    result->extType = createExtType(ERROR_t);
                } else {
                    result->extType = createExtType(node->type->type);
                }
            }
        }
    }
    // if result->extType->type is not ERROR_t, invoke is ok
    return result;
}

void checkBooleanExpr(ExprSemantic *expr) {
    if (expr->extType->type != BOOLEAN_t) {
        printError(StatementNotBool);
    }
}

void checkReturnStatement(ExprSemantic *expr, ExtType *funcReturn) {
    // need to follow funcReturn
    if (funcReturn->type == VOID_t) {
        printError(FuncRetTypeMismatch);
    } else if (funcReturn->type != expr->extType->type) {
        if (checkCoercion(funcReturn->type, expr->extType->type) == false) {
            printError(FuncRetTypeMismatch);
        }
    } else if (funcReturn->dimNum != expr->extType->dimNum) {
        printError(FuncRetTypeMismatch);
    } else if ((expr != NULL && funcReturn == NULL) ||
               (expr == NULL && funcReturn != NULL)) {
        printError(FuncRetTypeMismatch);
    } else {
        ArrayDimNode *returnDim = funcReturn->dim;
        ArrayDimNode *exprDim = expr->extType->dim;
        int i = 0;
        while (returnDim != NULL) {
            // traversal check
            if (returnDim->size != exprDim->size) {
                printError(FuncRetTypeMismatch);
                break;
            }
            i++;
            returnDim = returnDim->next;
            exprDim = exprDim->next;
        }
    }
}

PARERROR checkFuncParam(ParamSemantic *params) {
    // no parameters, don't need to check
    IDRecordList *idlist = buildIDList();
    if (params == NULL) {
        return NoErr_t;
    } else {
        ParamSemantic *ptr = params;
        while (ptr != NULL) {
            if (ptr->extType->isError == true) {
                return Other_t;
            } else {
                IDNodeSemantic *idPtr;
                for (idPtr = ptr->idlist; idPtr != NULL; idPtr = idPtr->next) {
                    if (traversalCheckIDRepeat(idlist, idPtr->value) == true) {
                        free(idlist);
                        return Redecl_t;
                    } else {
                        insertIDIntoRecList(idlist, idPtr->value);
                    }
                }
            }
            ptr = ptr->next;
        }
    }
    free(idlist);
    return NoErr_t;
}

SymNode *findFuncDeclaration(SymTable *table, char *name) {
    SymNode *node = lookupSymbol(table, name, 0, false);
    if (node == NULL) {
        return NULL;
    } else
        // node found!
        return node;
}

bool checkFuncDeclaration(SymTable *table, ParamSemantic *par,
                          ExtType *scalarType, SymNode *node) {
    if (node->isFuncDefine == true) {
        // found node is function define node
        printError(FuncReDef);
        return false;
    } else if (node->declAndDefined == true) {
        // found node is function declaration, and has been defined
        printError(FuncReDef);
        return false;
    } else if (node->kind != FUNCTION_t) {
        printError(IDreused);
        return false;
    } else {
        // check parameters
        if (scalarType->type != node->type->type) {
            // function decl and def can also do coercion
            if (checkCoercion(node->type->type, scalarType->type)) {
                printError(FuncRetTypeMismatch);
                return false;
            }
        } else if (node->attribute->funcParam->paramNum == 0) {
            if (par != 0) {
                printError(ParCountMismatch);
                return false;
            }
        } else {
            ExtTypePointer *typePtr = node->attribute->funcParam->params;
            while (typePtr != NULL && par != NULL) {
                // check type
                if (typePtr->value->type != par->extType->type) {
                    printError(ParTypeMismatch);
                    return false;
                }
                // check dimension
                if (typePtr->value->dimNum != par->extType->dimNum) {
                    printError(ParTypeMismatch);
                    return false;
                } else {
                    ArrayDimNode *lhsPtr = typePtr->value->dim;
                    ArrayDimNode *rhsPtr = par->extType->dim;
                    while (lhsPtr != NULL) {
                        // in some dimension, size is different
                        if (lhsPtr->size != rhsPtr->size) {
                            printError(ParTypeMismatch);
                            return false;
                        }
                        lhsPtr = lhsPtr->next;
                        rhsPtr = rhsPtr->next;
                    }
                }
                typePtr = typePtr->next;
                par = par->next;
            }

            if (typePtr != NULL || par != NULL) {
                printError(ParCountMismatch);
                return false;
            }
        }
    }
    return true;
}

IDRecordList *buildIDList() {
    IDRecordList *newList = malloc(sizeof(IDRecordList));
    for (int i = 0; i < 100; i++) {
        newList->entry[i] = NULL;
    }
    return newList;
}

void insertIDIntoRecList(IDRecordList *list, char *newID) {
    if (list->entry[0] == NULL) {
        list->entry[0] = malloc(sizeof(IDRecNode));
        list->entry[0]->id = malloc(sizeof(char) * (strlen(newID) + 1));
        strcpy(list->entry[0]->id, newID);
    } else {
        IDRecNode *nodePtr = list->entry[0];
        while (nodePtr->next != NULL) {
            nodePtr = nodePtr->next;
        }
        IDRecNode *newNode = malloc(sizeof(IDRecNode));
        newNode->id = malloc(sizeof(char) * (strlen(newID) + 1));
        strcpy(newNode->id, newID);
        nodePtr->next = newNode;
    }
}

bool traversalCheckIDRepeat(IDRecordList *list, char *checkID) {
    IDRecNode *nodePtr = list->entry[0];
    while (nodePtr != NULL) {
        if (strcmp(nodePtr->id, checkID) == 0) {
            return true;
        }
        nodePtr = nodePtr->next;
    }
    return false;
}

void printError(ERRTYPE errType) {
    semError = true;
    switch (errType) {
        case ReferenceError:
            fprintf(stdout,
                    "########## Error at Line#%d: reference error ##########\n",
                    linenum);
            break;
        case NodeIsFunction:
            fprintf(stdout,
                    "########## Error at Line#%d: function type can't do this "
                    "operation ##########\n",
                    linenum);
            break;
        case NodeIsNotFunction:
            fprintf(stdout,
                    "########## Error at Line#%d: id is not a function "
                    "##########\n",
                    linenum);
            break;
        case IDNotDecl:
            fprintf(stdout,
                    "########## Error at Line#%d: id has not declared yet "
                    "##########\n",
                    linenum);
            break;
        case IDreused:
            fprintf(stdout,
                    "########## Error at Line#%d: id has already been used "
                    "##########\n",
                    linenum);
            break;
        case ConstAssignment:
            fprintf(stdout,
                    "########## Error at Line#%d: const's type can't be "
                    "assigned ##########\n",
                    linenum);
            break;
        case ConstDeclTypeMismatch:
            fprintf(stdout,
                    "########## Error at Line#%d: const's type is not "
                    "consistent with value's type ##########\n",
                    linenum);
            break;
        case ParCountMismatch:
            fprintf(stdout,
                    "########## Error at Line#%d: parameter count mismatch. "
                    "##########\n",
                    linenum);
            break;
        case ParTypeMismatch:
            fprintf(stdout,
                    "########## Error at Line#%d: parameter type mismatch. "
                    "##########\n",
                    linenum);
            break;
        case FuncRetTypeMismatch:
            fprintf(stdout,
                    "########## Error at Line#%d: function return type "
                    "mismatch ##########\n",
                    linenum);
            break;
        case FuncReDef:
            fprintf(stdout,
                    "########## Error at Line#%d: function has already been "
                    "defined ##########\n",
                    linenum);
            break;
        case ArrayIndexNotInt:
            fprintf(stdout,
                    "########## Error at Line#%d: array index is not integer "
                    "or >= 0 "
                    "##########\n",
                    linenum);
            break;
        case StatementIsString:
            fprintf(stdout,
                    "########## Error at Line#%d: statement's operand can't be "
                    "string type ##########\n",
                    linenum);
            break;
        case StatementIsArrType:
            fprintf(stdout,
                    "########## Error at Line#%d: statement's operand can't be "
                    "array type ##########\n",
                    linenum);
            break;
        case StatementNotBool:
            fprintf(stdout,
                    "########## Error at Line#%d: statement's operand is not "
                    "boolean type ##########\n",
                    linenum);
            break;
        case ParameterError:
            fprintf(stdout,
                    "########## Error at Line#%d: function's parameter has "
                    "some error ##########\n",
                    linenum);
            break;
        case OperandTypeMismatch:
            fprintf(stdout,
                    "########## Error at Line#%d: operands type are not "
                    "the same "
                    "##########\n",
                    linenum);
            break;
        case UnaryOperandShouldBeReal:
            fprintf(stdout,
                    "########## Error at Line#%d: operand should be a real "
                    "number ##########\n",
                    linenum);
            break;
        case InitVarTypeErr:
            fprintf(stdout,
                    "########## Error at Line#%d: initial value type or "
                    "dimension error "
                    "##########\n",
                    linenum);
            break;
        case AssignmentTypeMismatch:
            fprintf(stdout,
                    "########## Error at Line#%d: assignment type mismatch "
                    "##########\n",
                    linenum);
            break;
        case ArrTypeAssignment:
            fprintf(stdout,
                    "########## Error at Line#%d: array type assignment "
                    "##########\n",
                    linenum);
            break;
        case ArrSizeShouldG0:
            fprintf(stdout,
                    "########## Error at Line#%d: initial value's array size "
                    "should > 0 ##########\n",
                    linenum);
            break;
        case ControlStatNotBool:
            fprintf(stdout,
                    "########## Error at Line#%d: control expression is not "
                    "boolean type ##########\n",
                    linenum);
            break;
        case ContinueOutOfLoop:
            fprintf(
                stdout,
                "########## Error at Line#%d: 'continue' can't be outside of "
                "a loop ##########\n",
                linenum);
            break;
        case BreakOutOfLoop:
            fprintf(stdout,
                    "########## Error at Line#%d: 'break' can't be outside of "
                    "a loop ##########\n",
                    linenum);
            break;
        default:
            fprintf(stdout,
                    "########## Error at Line#%d: an unknown error "
                    "occured "
                    "##########\n",
                    linenum);
            break;
    }
}