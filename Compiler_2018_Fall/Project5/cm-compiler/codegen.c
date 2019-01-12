#include "codegen.h"
#include <stdarg.h>
#include <string.h>
#include "datatype.h"

struct loop_stack loopStack;
int label_count = -1;

void codegen_PushIns(char *instr) {
    insList.list[insList.size++] = strdup(instr);
}

void codegen_GenToList(char *fmt, ...) {
    char tmp[256];
    memset(tmp, 0, sizeof(tmp));
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);
    codegen_PushIns(tmp);
}

void codegen_InitProgram() {
    loopStack.top = -1;
    fprintf(output, "; %s.j\n", filename);
    fprintf(output, ".class public %s\n", filename);
    fprintf(output, ".super java/lang/Object\n");
    fprintf(output, ".field public static _sc Ljava/util/Scanner;\n");
}

// ===== Function =====
// function definition
void codegen_BuildMainFunction() {
    // main function
    fprintf(output, "\n; ########## main function ##########\n");
    fprintf(output, ".method public static main([Ljava/lang/String;)V\n");
    fprintf(output, ".limit stack 500\n");
    fprintf(output, ".limit locals 500\n");
    fprintf(output, "\tnew java/util/Scanner\n");
    fprintf(output, "\tdup\n");
    fprintf(output, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
    fprintf(
        output,
        "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
    fprintf(output, "\tputstatic %s/_sc Ljava/util/Scanner;\n", filename);
    fprintf(output, "; === assign global variable's value ===\n");
    int count = 0;
    int top = 0;
    while (1) {
        struct idNode *idn = idDeq(idq);
        if (idn == NULL) {
            break;
        } else {
            // fprintf(output, "; %s: %d\n", idn->data, idn->hasVal);
            if (idn->hasVal) {
                struct valNode *val = valDeq(valq);
                // load value
                switch (val->type) {
                    case INTEGER_t:
                        fprintf(output, "ldc %d\n", (int)val->val);
                        break;
                    case DOUBLE_t:
                    case FLOAT_t:
                        fprintf(output, "ldc %lf\n", val->val);
                        break;
                    case BOOLEAN_t:
                        fprintf(output, "iconst_%d\n", (int)val->val);
                        break;
                }
                if (globalConvertList[top]) {
                    fprintf(output, "i2f\nputstatic %s/%s F\n", filename,
                            idn->data);
                } else {
                    fprintf(output, "putstatic %s/%s ", filename, idn->data);
                    switch (val->type) {
                        case INTEGER_t:
                            fprintf(output, "I\n");
                            break;
                        case DOUBLE_t:
                        case FLOAT_t:
                            fprintf(output, "F\n");
                            break;
                        case BOOLEAN_t:
                            fprintf(output, "Z\n");
                            break;
                    }
                }
            }
            top++;
        }
    }
    fprintf(output, "; ======================================\n");
}

void codegen_BuildOtherFunction(char *id, ParamSemantic *param,
                                ExtType *retType) {
    fprintf(output, "\n; ########## function %s ##########\n", id);
    fprintf(output, ".method public static %s(", id);
    ParamSemantic *parPtr = param;
    // print parameters' types
    while (parPtr != NULL) {
        IDNodeSemantic *idPtr = parPtr->idlist;
        while (idPtr != NULL) {
            switch (parPtr->extType->type) {
                case INTEGER_t:
                    fprintf(output, "I");
                    break;
                case FLOAT_t:
                case DOUBLE_t:
                    fprintf(output, "F");
                    break;
                case BOOLEAN_t:
                    fprintf(output, "Z");
                    break;
            }
            idPtr = idPtr->next;
        }
        parPtr = parPtr->next;
    }
    // print function's return type
    fprintf(output, ")");
    switch (retType->type) {
        case INTEGER_t:
            fprintf(output, "I");
            break;
        case FLOAT_t:
        case DOUBLE_t:
            fprintf(output, "F");
            break;
        case BOOLEAN_t:
            fprintf(output, "Z");
            break;
        default:
            fprintf(output, "V");
            break;
    }
    fprintf(output, "\n");
    fprintf(output, ".limit stack 500\n");
    fprintf(output, ".limit locals 500\n\n");
}

void codegen_FunctionEnd(ExtType *retType) {
    switch (retType->type) {
        case INTEGER_t:
        case BOOLEAN_t:
            strcat(insBuf, "ireturn\n");
            break;
        case FLOAT_t:
        case DOUBLE_t:
            strcat(insBuf, "freturn\n");
            break;
        default:  // void type
            strcat(insBuf, "return\n");
            break;
    }
    strcat(insBuf, ".end method\n");
    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));
    codegen_ExprIns();
}

// function invoke
void codegen_FunctionInvoke(char *id, bool isNeg) {
    // find declared function node in symtable to get more info
    SymNode *funcNode = lookupSymbol(symTable, id, 0, false);
    if (funcNode != NULL) {
        snprintf(insBuf, sizeof(insBuf), "invokestatic %s/%s(", filename, id);
        // print parameters
        ExtTypePointer *params = funcNode->attribute->funcParam->params;
        while (params != NULL) {
            switch (params->value->type) {
                case INTEGER_t:
                    strcat(insBuf, "I");
                    break;
                case FLOAT_t:
                case DOUBLE_t:
                    strcat(insBuf, "F");
                    break;
                case BOOLEAN_t:
                    strcat(insBuf, "Z");
                    break;
            }
            params = params->next;
        }
        // print return type
        strcat(insBuf, ")");
        switch (funcNode->type->type) {
            case INTEGER_t:
                strcat(insBuf, "I");
                break;
            case FLOAT_t:
            case DOUBLE_t:
                strcat(insBuf, "F");
                break;
            case BOOLEAN_t:
                strcat(insBuf, "Z");
                break;
            case VOID_t:
                strcat(insBuf, "V");
                break;
        }
        strcat(insBuf, " ;    call function ");
        strcat(insBuf, id);
        strcat(insBuf, "\n");
        // call function with negative prefix
        if (isNeg) {
            if (funcNode->type->type == INTEGER_t) {
                strcat(insBuf, "ineg\n");
            } else if (funcNode->type->type == FLOAT_t ||
                       funcNode->type->type == DOUBLE_t) {
                strcat(insBuf, "fneg\n");
            }
        }
        codegen_PushIns(insBuf);
        codegen_ExprIns();
        memset(insBuf, 0, sizeof(insBuf));
    }
}

void codegen_FunctionReturn(ExtType *retType) {
    switch (retType->type) {
        case INTEGER_t:
        case BOOLEAN_t:
            strcat(insBuf, "ireturn\n");
            break;
        case FLOAT_t:
        case DOUBLE_t:
            strcat(insBuf, "freturn\n");
            break;
        default:  // void type
            strcat(insBuf, "return\n");
            break;
    }
    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));
    codegen_ExprIns();
}
// ====================

// ===== Simple statement =====
void codegen_PrintStart() {
    snprintf(insBuf, sizeof(insBuf),
             "\n;    ### Start print ###\ngetstatic java/lang/System/out "
             "Ljava/io/PrintStream;\n");
    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));
}

void codegen_PrintEnd(ExprSemantic *expr) {
    switch (expr->extType->type) {
        case INTEGER_t:
            fprintf(output, "invokevirtual java/io/PrintStream/print(I)V\n");
            break;
        case FLOAT_t:
        case DOUBLE_t:
            fprintf(output, "invokevirtual java/io/PrintStream/print(F)V\n");
            break;
        case BOOLEAN_t:
            fprintf(output, "invokevirtual java/io/PrintStream/print(Z)V\n");
            break;
        case STRING_t:
            fprintf(output,
                    "invokevirtual "
                    "java/io/PrintStream/print(Ljava/lang/String;)V\n");
            break;
    }
    fprintf(output, ";    ### End of print ###\n\n");
}

void codegen_ConstLiteral(ConstAttr *constAttr) {
    char tmp[100];
    switch (constAttr->type) {
        case STRING_t:
            snprintf(insBuf, sizeof(insBuf), "ldc \"%s\"\n",
                     constAttr->value.stringVal);
            break;
        case INTEGER_t:
            if (scope != 0) {
                snprintf(insBuf, sizeof(insBuf), "ldc %d ; is integer\n",
                         constAttr->value.integerVal);
            } else {
                valEnQ(valq, constAttr->value.integerVal, INTEGER_t);
            }
            break;
        case FLOAT_t:
            if (scope != 0) {
                snprintf(insBuf, sizeof(insBuf), "ldc %lf ; is float \n",
                         constAttr->value.floatVal);
            } else {
                valEnQ(valq, constAttr->value.floatVal, FLOAT_t);
            }
            break;
        case DOUBLE_t:
            if (scope != 0) {
                snprintf(insBuf, sizeof(insBuf), "ldc %lf ; is double \n",
                         constAttr->value.doubleVal);
            } else {
                valEnQ(valq, constAttr->value.doubleVal, DOUBLE_t);
            }
            break;
        case BOOLEAN_t:
            if (scope != 0) {
                snprintf(insBuf, sizeof(insBuf), "iconst_%d ; is bool\n",
                         constAttr->value.booleanVal);
            } else {
                valEnQ(valq, constAttr->value.booleanVal, BOOLEAN_t);
            }
            break;
    }
    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));
}

void codegen_Read(ExprSemantic *expr) {
    fprintf(output, ";    ### Start read ###\n");
    fprintf(output, "getstatic %s/_sc Ljava/util/Scanner;\n", filename);
    if (expr->varRef != NULL) {
        char *id = expr->varRef->id;
        SymNode *node = lookupSymbol(symTable, id, scope, false);
        if (node->kind == VARIABLE_t) {  // is var node
            if (node->scope == 0) {      // global var
                switch (expr->extType->type) {
                    case INTEGER_t:
                        fprintf(output,
                                "invokevirtual java/util/Scanner/nextInt()I\n");
                        fprintf(output, "putstatic %s/%s I\n", filename,
                                node->name);
                        break;
                    case FLOAT_t:
                    case DOUBLE_t:
                        fprintf(output,
                                "invokevirtual "
                                "java/util/Scanner/nextFloat()F\n");
                        fprintf(output, "putstatic %s/%s F\n", filename,
                                node->name);
                        break;
                    case BOOLEAN_t:
                        fprintf(output,
                                "invokevirtual "
                                "java/util/Scanner/nextBoolean()Z\n");
                        fprintf(output, "putstatic %s %s Z\n", filename,
                                node->name);
                        break;
                }
            } else if (node->scope != 0) {  // local var
                switch (expr->extType->type) {
                    case INTEGER_t:
                        fprintf(output,
                                "invokevirtual java/util/Scanner/nextInt()I\n");
                        fprintf(output,
                                "istore %d ; local var num of %s is %d\n",
                                node->varNumber, node->name, node->varNumber);
                        break;
                    case FLOAT_t:
                    case DOUBLE_t:
                        fprintf(output,
                                "invokevirtual "
                                "java/util/Scanner/nextFloat()F\n");
                        fprintf(output,
                                "fstore %d ; local var num of %s is %d\n",
                                node->varNumber, node->name, node->varNumber);
                        break;
                    case BOOLEAN_t:
                        fprintf(output,
                                "invokevirtual "
                                "java/util/Scanner/nextBoolean()Z\n");
                        fprintf(output,
                                "istore %d ; local var num of %s is %d\n",
                                node->varNumber, node->name, node->varNumber);
                        break;
                }
            }
        }
    }
}
// ============================

// ===== Variable =====
void codegen_GlobalVar(VarDecl *ptr, ExtType *type) {
    char *name = ptr->para->idlist->value;
    switch (type->type) {
        case INTEGER_t:
            snprintf(insBuf, sizeof(insBuf), ".field public static %s I\n",
                     name);
            break;
        case BOOLEAN_t:
            snprintf(insBuf, sizeof(insBuf), ".field public static %s Z\n",
                     name);
            break;
        case FLOAT_t:
        case DOUBLE_t:
            snprintf(insBuf, sizeof(insBuf), ".field public static %s F\n",
                     name);
            break;
    }
    fprintf(output, "%s", insBuf);
    memset(insBuf, 0, sizeof(insBuf));
}

void codegen_ExprIns() {
    for (int i = 0; i < insList.size; i++) {
        fprintf(output, "%s", insList.list[i]);
        free(insList.list[i]);
    }
    insList.size = 0;
}

void codegen_SaveExpr(ExprSemantic *expr, ExprSemantic *right) {
    if (!expr->varRef) return;

    SymNode *node = lookupSymbol(symTable, expr->varRef->id, scope, false);
    if (!node) return;

    if (node->kind == VARIABLE_t &&
        node->scope != 0) {  // is variable, but not global
        switch (expr->extType->type) {
            case INTEGER_t:
                snprintf(insBuf, sizeof(insBuf), "istore %d ; save to %s \n",
                         node->varNumber, node->name);
                break;
            case FLOAT_t:
            case DOUBLE_t:
                if (right &&
                    right->extType->type == INTEGER_t) {  // need to convert
                    snprintf(insBuf, sizeof(insBuf),
                             "i2f ; do type convert here\nfstore %d\n",
                             node->varNumber);
                } else {
                    snprintf(insBuf, sizeof(insBuf),
                             "fstore %d ; save to %s \n", node->varNumber,
                             node->name);
                }
                break;
            case BOOLEAN_t:
                snprintf(insBuf, sizeof(insBuf), "istore %d  ; save to %s \n",
                         node->varNumber, node->name);
                break;
        }
    } else if (node->kind == VARIABLE_t && node->scope == 0) {
        switch (expr->extType->type) {
            case INTEGER_t:
                snprintf(insBuf, sizeof(insBuf), "putstatic %s/%s I\n",
                         filename, node->name);
                break;
            case FLOAT_t:
            case DOUBLE_t:
                if (right && right->extType->type == INTEGER_t) {  // convert
                    snprintf(insBuf, sizeof(insBuf), "i2f\nputstatic %s/%s F\n",
                             filename, node->name);
                } else {
                    snprintf(insBuf, sizeof(insBuf), "putstatic %s/%s F\n",
                             filename, node->name);
                }
                break;
            case BOOLEAN_t:
                snprintf(insBuf, sizeof(insBuf), "putstatic %s/%s Z\n",
                         filename, node->name);
                break;
        }
    }

    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));  // clear codes
}

void codegen_Negative(ExprSemantic *expr) {
    switch (expr->extType->type) {
        case INTEGER_t:
            codegen_GenToList("ineg\n");
            break;
        case FLOAT_t:
        case DOUBLE_t:
            codegen_GenToList("fneg\n");
            break;
    }
}

void codegen_LoadVar(ExprSemantic *var) {
    if (var && var->varRef) {
        SymNode *node = lookupSymbol(symTable, var->varRef->id, scope, false);
        if (node && node->kind != CONSTANT_t) {  // variable type
            if (node->scope != 0) {
                switch (node->type->type) {
                    case INTEGER_t:
                    case BOOLEAN_t:
                        snprintf(insBuf, sizeof(insBuf),
                                 "iload %d ; var name: %s\n", node->varNumber,
                                 node->name);
                        break;
                    case FLOAT_t:
                    case DOUBLE_t:
                        snprintf(insBuf, sizeof(insBuf),
                                 "fload %d ; var name: %s\n", node->varNumber,
                                 node->name);
                        break;
                }
            } else if (node->scope == 0) {
                switch (node->type->type) {
                    case INTEGER_t:
                        snprintf(insBuf, sizeof(insBuf), "getstatic %s/%s I\n",
                                 filename, node->name);
                        break;
                    case BOOLEAN_t:
                        snprintf(insBuf, sizeof(insBuf), "getstatic %s/%s Z\n",
                                 filename, node->name);
                        break;
                    case FLOAT_t:
                    case DOUBLE_t:
                        snprintf(insBuf, sizeof(insBuf), "getstatic %s/%s F\n",
                                 filename, node->name);
                        break;
                }
            }
        } else if (node &&
                   node->kind == CONSTANT_t) {  // constant, load from symtable
            switch (node->attribute->constVal->type) {
                case BOOLEAN_t:
                    snprintf(insBuf, sizeof(insBuf), "ldc %d\n",
                             node->attribute->constVal->value.booleanVal);
                    break;
                case INTEGER_t:

                    if (node->type->type != INTEGER_t) {
                        snprintf(insBuf, sizeof(insBuf), "ldc %d\ni2f\n",
                                 node->attribute->constVal->value.integerVal);
                    } else {
                        snprintf(insBuf, sizeof(insBuf), "ldc %d\n",
                                 node->attribute->constVal->value.integerVal);
                    }
                    break;
                case DOUBLE_t:
                case FLOAT_t:
                    snprintf(insBuf, sizeof(insBuf), "ldc %lf\n",
                             node->attribute->constVal->value.floatVal);
                    break;
                case STRING_t:
                    snprintf(insBuf, sizeof(insBuf), "ldc \"%s\"\n",
                             node->attribute->constVal->value.stringVal);
                    break;
            }
        }
        codegen_PushIns(insBuf);
        memset(insBuf, 0, sizeof(insBuf));
    }
}

void codegen_LocalVar(ExtType *type, int varNo) {
    if (scope == 0) return;
    switch (type->type) {
        case INTEGER_t:
            snprintf(insBuf, sizeof(insBuf), "istore %d\n", varNo);
            break;
        case FLOAT_t:
        case DOUBLE_t:
            snprintf(insBuf, sizeof(insBuf), "fstore %d\n", varNo);
            break;
        case BOOLEAN_t:
            snprintf(insBuf, sizeof(insBuf), "istore %d\n", varNo);
            break;
    }
    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));  // clear codes
}
// ====================

// ===== Operations =====
void codegen_Arithmetic(ExprSemantic *op1, OPERATOR operator,
                        ExprSemantic *op2) {
    if (op1 && op2) {
        //  check coercion
        if ((op1->extType->type == INTEGER_t &&
             (op2->extType->type == FLOAT_t ||
              op2->extType->type == DOUBLE_t))) {
            fprintf(output, "i2f\n");
            codegen_ExprIns();
            op1->extType->type = DOUBLE_t;
        } else if ((op2->extType->type == INTEGER_t &&
                    (op1->extType->type == FLOAT_t ||
                     op1->extType->type == DOUBLE_t))) {
            codegen_GenToList("i2f\n");
        }
    }
    switch (operator) {
        case ADD_t:
            if (op1->extType->type == INTEGER_t) {
                snprintf(insBuf, sizeof(insBuf), "iadd\n");
            } else if (op1->extType->type == FLOAT_t ||
                       op1->extType->type == DOUBLE_t) {
                snprintf(insBuf, sizeof(insBuf), "fadd\n");
            }
            break;
        case SUB_t:
            if (op1->extType->type == INTEGER_t) {
                snprintf(insBuf, sizeof(insBuf), "isub\n");
            } else if (op1->extType->type == FLOAT_t ||
                       op1->extType->type == DOUBLE_t) {
                snprintf(insBuf, sizeof(insBuf), "fsub\n");
            }
            break;
        case MUL_t:
            if (op1->extType->type == INTEGER_t) {
                snprintf(insBuf, sizeof(insBuf), "imul\n");
            } else if (op1->extType->type == FLOAT_t ||
                       op1->extType->type == DOUBLE_t) {
                snprintf(insBuf, sizeof(insBuf), "fmul\n");
            }
            break;
        case DIV_t:
            if (op1->extType->type == INTEGER_t) {
                snprintf(insBuf, sizeof(insBuf), "idiv\n");
            } else if (op1->extType->type == FLOAT_t ||
                       op1->extType->type == DOUBLE_t) {
                snprintf(insBuf, sizeof(insBuf), "fdiv\n");
            }
            break;
        case MOD_t:
            snprintf(insBuf, sizeof(insBuf), "irem\n");
            break;
    }
    codegen_PushIns(insBuf);
    memset(insBuf, 0, sizeof(insBuf));
}
// ======================

// ===== BOOLEAN =====
void codegen_BooleanOp(ExprSemantic *op1, OPERATOR op, ExprSemantic *op2) {
    codegen_ExprIns();
    switch (op) {
        case AND_t:
            codegen_GenToList("iand\n");
            break;
        case OR_t:
            codegen_GenToList("ior\n");
            break;
        case NOT_t:
            codegen_GenToList("iconst_1\nixor\n");
            break;
    }
}
void codegen_Relation(ExprSemantic *op1, OPERATOR op, ExprSemantic *op2) {
    loopStack.top++;
    label_count++;
    loopStack.stack[loopStack.top] = label_count;
    if (op1->extType->type == INTEGER_t) {
        codegen_PushIns("isub\n");
    } else if (op1->extType->type == FLOAT_t ||
               op1->extType->type == DOUBLE_t) {
        codegen_PushIns("fcmpl\n");
    }
    switch (op) {
        case LT_t:
            codegen_GenToList("iflt Ltrue_%d\n",
                              loopStack.stack[loopStack.top]);
            break;
        case LE_t:
            codegen_GenToList("ifle Ltrue_%d\n",
                              loopStack.stack[loopStack.top]);
            break;
        case NE_t:
            codegen_GenToList("ifne Ltrue_%d\n",
                              loopStack.stack[loopStack.top]);
            break;
        case GE_t:
            codegen_GenToList("ifge Ltrue_%d\n",
                              loopStack.stack[loopStack.top]);
            break;
        case GT_t:
            codegen_GenToList("ifgt Ltrue_%d\n",
                              loopStack.stack[loopStack.top]);
            break;
        case EQ_t:
            codegen_GenToList("ifeq Ltrue_%d\n",
                              loopStack.stack[loopStack.top]);
            break;
    }
    codegen_PushIns("iconst_0\n");  // false
    codegen_GenToList("goto Lfalse_%d\n", loopStack.stack[loopStack.top]);
    codegen_GenToList("Ltrue_%d:\n", loopStack.stack[loopStack.top]);
    codegen_PushIns("iconst_1\n");  // true
    codegen_GenToList("Lfalse_%d:\n", loopStack.stack[loopStack.top]);
    loopStack.top--;
}

// ===== For global Var queue =====
// ===== ID Q =====
struct idNode *idNewNode(char *id, bool hasVal, BTYPE type) {
    struct idNode *tmp = (struct idNode *)malloc(sizeof(struct idNode));
    tmp->data = (char *)malloc(sizeof(char) * 50);
    // memset(tmp->data, 0, sizeof(tmp->data));
    strcpy(tmp->data, id);
    tmp->hasVal = hasVal;
    tmp->type = type;
    tmp->next = NULL;
    return tmp;
}

struct idQueue *initIDQueue() {
    struct idQueue *q = (struct idQueue *)malloc(sizeof(struct idQueue));
    q->front = q->rear = NULL;
    return q;
}

void idEnQ(struct idQueue *q, char *id, bool hasVal, BTYPE type) {
    struct idNode *tmp = idNewNode(id, hasVal, type);
    if (q->rear == NULL) {
        q->front = q->rear = tmp;
        return;
    }
    q->rear->next = tmp;
    q->rear = tmp;
}

struct idNode *idDeq(struct idQueue *q) {
    if (q->front == NULL) return NULL;  // empty
    struct idNode *tmp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    return tmp;
}

struct idNode *searchIDQueue(struct idQueue *q, char *id) {
    struct idNode *ptr = q->front;
    while (ptr != NULL) {
        if (strcmp(ptr->data, id) == 0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}
// ================
// ===== VALUE Q =====
struct valNode *valNewNode(double val, BTYPE type) {
    struct valNode *tmp = (struct valNode *)malloc(sizeof(struct valNode));
    tmp->val = val;
    tmp->type = type;
    tmp->next = NULL;
    return tmp;
}

struct valQueue *initValQueue() {
    struct valQueue *q = (struct valQueue *)malloc(sizeof(struct valQueue));
    q->front = q->rear = NULL;
    return q;
}

void valEnQ(struct valQueue *q, double val, BTYPE type) {
    struct valNode *tmp = valNewNode(val, type);
    if (q->rear == NULL) {
        q->front = q->rear = tmp;
        return;
    }
    q->rear->next = tmp;
    q->rear = tmp;
}

struct valNode *valDeq(struct valQueue *q) {
    if (q->front == NULL) return NULL;  // empty
    struct valNode *tmp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    return tmp;
}
// ================
// ===== For Local Var =====
struct localNode *localNewNode(char *id, BTYPE type, int varNo) {
    struct localNode *tmp =
        (struct localNode *)malloc(sizeof(struct localNode));
    tmp->id = malloc(sizeof(char) * (50));
    // memset(tmp->id, 0, sizeof(tmp->id));
    strcpy(tmp->id, id);
    tmp->type = type;
    tmp->varNo = varNo;
    return tmp;
}

struct localStack *initLocalStack() {
    struct localStack *stack =
        (struct localStack *)malloc(sizeof(struct localStack));
    stack->top = -1;
    stack->stack = (struct localNode **)malloc(sizeof(struct localNode) * 50);
    return stack;
}

void localPush(struct localStack *stack, char *id, BTYPE type, int varNo) {
    stack->stack[++stack->top] = localNewNode(id, type, varNo);
}

struct localNode *locPop(struct localStack *stack) {
    if (stackIsEmpty(stack)) return NULL;
    return stack->stack[stack->top--];
}

bool stackIsEmpty(struct localStack *stack) { return stack->top == -1; }

struct localNode *searchLocStack(struct localStack *stack, char *id) {
    int top = stack->top;
    struct localNode *head = stack->stack[top];
    while (top != -1) {
        if (strcmp(stack->stack[top]->id, id) == 0) {
            return stack->stack[top];
        }
        top--;
    }
    return NULL;
}
// ================================
