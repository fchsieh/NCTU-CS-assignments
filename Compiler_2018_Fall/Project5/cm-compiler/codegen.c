#include "codegen.h"
#include "datatype.h"

void codegen_InitProgram() {
    fprintf(output, "; %s.j\n", filename);
    fprintf(output, ".class public %s\n", filename);
    fprintf(output, ".super java/lang/Object\n");
    fprintf(output, ".field public static _sc Ljava/util/Scanner;\n");
}

void codegen_BuildMainFunction() {
    // main function
    fprintf(output, "\n; ########## main function ##########\n");
    fprintf(output, ".method public static main([Ljava/lang/String;)V\n");
    fprintf(output, ".limit stack 500\n");
    fprintf(output, ".limit locals 500\n");
}

void codegen_BuildOtherFunction(char *id, ParamSemantic *param,
                                ExtType *retType) {
    fprintf(output, "\n; ########## %s function ##########\n", id);
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
                    fprintf(output, "F");
                    break;
                case DOUBLE_t:
                    fprintf(output, "D");
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
            fprintf(output, "F");
            break;
        case DOUBLE_t:
            fprintf(output, "D");
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
    fprintf(output, ".limit locals 500\n");
}

void codegen_FunctionEnd(ExtType *retType) {
    switch (retType->type) {
        case INTEGER_t:
        case BOOLEAN_t:
            fprintf(output, "ireturn\n");
            break;
        case FLOAT_t:
        case DOUBLE_t:
            fprintf(output, "freturn\n");
        default:  // void type
            fprintf(output, "return\n");
            break;
    }
    fprintf(output, ".end method\n");
}

void codegen_PrintStart() {
    fprintf(output, "\n;    ### Start print ###\n");
    fprintf(output, "getstatic java/lang/System/out Ljava/io/PrintStream;\n");
}

void codegen_PrintEnd(ExprSemantic *expr) {
    switch (expr->extType->type) {
        case INTEGER_t:
            fprintf(output, "invokevirtual java/io/PrintStream/print(I)V\n");
            break;
        case FLOAT_t:
            fprintf(output, "invokevirtual java/io/PrintStream/print(F)V\n");
            break;
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
    fprintf(output, ";    ### End of print ###\n");
}

void codegen_ConstLiteral(ConstAttr *constAttr) {
    switch (constAttr->type) {
        case STRING_t:
            fprintf(output, "ldc \"%s\"\n", constAttr->value.stringVal);
            break;
        case INTEGER_t:
            fprintf(output, "ldc %d\n", constAttr->value.integerVal);
            break;
        case FLOAT_t:
            fprintf(output, "ldc %lf\n", constAttr->value.floatVal);
            break;
        case DOUBLE_t:
            fprintf(output, "ldc \"%f\"\n", constAttr->value.doubleVal);
            break;
        case BOOLEAN_t:
            fprintf(output, "iconst_%d\n", constAttr->value.booleanVal);
            break;
    }
}

void codegen_FuncInvoke(char *id, bool isNeg) {
    // find declared function node in symtable to get more info
    SymNode *funcNode = lookupSymbol(symTable, id, 0, false);
    if (funcNode != NULL) {
        fprintf(output, "invokestatic %s/%s(", filename, id);
        // print parameters
        ExtTypePointer *params = funcNode->attribute->funcParam->params;
        while (params != NULL) {
            switch (params->value->type) {
                case INTEGER_t:
                    fprintf(output, "I");
                    break;
                case DOUBLE_t:
                    fprintf(output, "D");
                    break;
                case FLOAT_t:
                    fprintf(output, "F");
                    break;
                case BOOLEAN_t:
                    fprintf(output, "Z");
                    break;
            }
            params = params->next;
        }
        // print return type
        fprintf(output, ")");
        switch (funcNode->type->type) {
            case INTEGER_t:
                fprintf(output, "I");
                break;
            case DOUBLE_t:
                fprintf(output, "D");
                break;
            case FLOAT_t:
                fprintf(output, "F");
                break;
            case BOOLEAN_t:
                fprintf(output, "Z");
                break;
            case VOID_t:
                fprintf(output, "V");
                break;
        }
        fprintf(output, " ;    call function %s\n", id);
        // call function with negative prefix
        if (isNeg) {
            if (funcNode->type->type == INTEGER_t) {
                fprintf(output, "ineg\n");
            } else if (funcNode->type->type == FLOAT_t ||
                       funcNode->type->type == DOUBLE_t) {
                fprintf(output, "fneg\n");
            }
        }
    }
}