%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "symtable.h"

extern int linenum;
extern FILE	*yyin;
extern char	*yytext;
extern char buf[256];

int yylex();
int yyerror( char *msg );

SymTable *symTable;		// pass to main.c
extern int Opt_Symbol;	// from lex.l
int scope = 0;			// to memoized current scope
bool semError = false;	// pass to main.c
%}

%union {
    int intVal;
    float floatVal;
    double doubleVal;
    char *id;
    char *str;
    struct CONSTATTR *constVal;
    struct CONSTPARAM *constNodes;
    struct PARTYPE *parType;
    struct PARTYPE *arrdim;
    struct VARPARAM *idlist;
    struct PARAMS *parameters;
};

%token <id>ID
%token <intVal>INT_CONST
%token <floatVal>FLOAT_CONST
%token <doubleVal>SCIENTIFIC
%token <str>STR_CONST

%token  LE_OP
%token  NE_OP
%token  GE_OP
%token  EQ_OP
%token  AND_OP
%token  OR_OP

%token  READ
%token  BOOLEAN
%token  WHILE
%token  DO
%token  IF
%token  ELSE
%token  TRUE
%token  FALSE
%token  FOR
%token  INT
%token  PRINT
%token  BOOL
%token  VOID
%token  FLOAT
%token  DOUBLE
%token  STRING
%token  CONTINUE
%token  BREAK
%token  RETURN
%token  CONST

%token  L_PAREN
%token  R_PAREN
%token  COMMA
%token  SEMICOLON
%token  ML_BRACE
%token  MR_BRACE
%token  L_BRACE
%token  R_BRACE
%token  ADD_OP
%token  SUB_OP
%token  MUL_OP
%token  DIV_OP
%token  MOD_OP
%token  ASSIGN_OP
%token  LT_OP
%token  GT_OP
%token  NOT_OP

%type<parType> scalar_type
%type<arrdim> dim
%type<constVal> literal_const
%type<constNodes> const_list
%type<idlist> identifier_list
%type<parameters> array_decl parameter_list

%start program
%%
program : decl_list funct_def decl_and_def_list
        {
            if(Opt_Symbol == 1) {
                printSymTable(symTable, scope);
            }
            //printf("Final tail: %d\n", symTable->tail);
        }
        ;

decl_list : decl_list var_decl
          | decl_list const_decl
          | decl_list funct_decl
          |
          ;


decl_and_def_list : decl_and_def_list var_decl
                  | decl_and_def_list const_decl
                  | decl_and_def_list funct_decl
                  | decl_and_def_list funct_def
                  |
                  ;


funct_def : scalar_type ID L_PAREN R_PAREN
            {
                if(IDRedecl(symTable, 0, $2) == false) {
                    insertFuncNode(symTable, $2, NULL, $1);
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
            }
            compound_statement
          | scalar_type ID L_PAREN parameter_list R_PAREN
            {
                if(IDRedecl(symTable, 0, $2) == false) {
                    insertParamNode(symTable, $4, scope+1);
                    insertFuncNode(symTable, $2, $4, $1);
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
            }
            compound_statement
          | VOID ID L_PAREN R_PAREN
            {
                if(IDRedecl(symTable, 0, $2) == false) {
                    insertFuncNode(symTable, $2, NULL, createParamType(VOID_t));
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
            }
            compound_statement
          | VOID ID L_PAREN parameter_list R_PAREN
            {
                if(IDRedecl(symTable, 0, $2) == false) {
                    insertParamNode(symTable, $4, scope+1);
                    insertFuncNode(symTable, $2, $4, createParamType(VOID_t));
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
            }
            compound_statement
          ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON
            {
                if(IDRedecl(symTable, 0, $2) == false) {
                    insertFuncNode(symTable, $2, NULL, $1);
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
            }
           | scalar_type ID L_PAREN parameter_list R_PAREN SEMICOLON
           {
               if(IDRedecl(symTable, 0, $2) == false) {
                    insertFuncNode(symTable, $2, $4, $1);
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
           }
           | VOID ID L_PAREN R_PAREN SEMICOLON
           {
               if(IDRedecl(symTable, 0, $2) == false) {
                    insertFuncNode(symTable, $2, NULL, createParamType(VOID_t));
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
           }
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON
           {
               if(IDRedecl(symTable, 0, $2) == false) {
                    insertFuncNode(symTable, $2, $4, createParamType(VOID_t));
                }
                else {
                    printError(REDECL_t, linenum, $2);
                    semError = true;
                }
           }
           ;

parameter_list  : parameter_list COMMA scalar_type ID
               {
                   Params *ptr;
                   ptr = createParam(createIDList($4), $3);
                   combineDeclare(PARAMETER_t, $1, ptr);
                   $$ = $1;
               }
               | parameter_list COMMA scalar_type array_decl
               {
                   $4->parType->type = $3->type;
                   combineDeclare(PARAMETER_t, $1, $4);
                   $$ = $1;
               }
               | scalar_type array_decl
               {
                   $2->parType->type = $1->type;
                   $$ = $2;
               }
               | scalar_type ID
               {
                   $$ = createParam(createIDList($2), $1);
               }
               ;

var_decl : scalar_type identifier_list SEMICOLON
         {
            VarParam *ptr;
            for(ptr=$2; ptr!=NULL; ptr=(ptr->next)) {
                if(IDRedecl(symTable, scope, ptr->para->idlist->value) == false) {
                    SymNode *newNode = createVarNode(ptr->para->idlist->value, scope, ptr->para->parType, $1);
                    insertIntoSymTable(symTable, newNode);
                }
                else {
                    printError(REDECL_t, linenum, ptr->para->idlist->value);
                    semError = true;
                }
            }
         }
         ;

identifier_list : identifier_list COMMA ID
                {
                    Params *ptr = createParam(createIDList($3), createParamType(VOID_t));
                    VarParam *vptr = createVarParam(ptr);
                    combineDeclare(VARIABLE_t, $1, vptr);
                }
                | identifier_list COMMA ID ASSIGN_OP logical_expression
                {
                    Params *ptr = createParam(createIDList($3), createParamType(VOID_t));
                    VarParam *vptr = createVarParam(ptr);
                    combineDeclare(VARIABLE_t, $1, vptr);
                }
                | identifier_list COMMA array_decl ASSIGN_OP initial_array
                {
                    VarParam *ptr = createVarParam($3);
                    combineDeclare(VARIABLE_t, $1, ptr);
                    $$ = $1;
                }
                | identifier_list COMMA array_decl
                {
                    VarParam *ptr = createVarParam($3);
                    combineDeclare(VARIABLE_t, $1, ptr);
                    $$ = $1;
                }
                | array_decl ASSIGN_OP initial_array
                {
                    $$ = createVarParam($1);
                }
                | array_decl
                {
                    $$ = createVarParam($1);
                }
                | ID ASSIGN_OP logical_expression
                {
                    Params *ptr = createParam(createIDList($1), createParamType(VOID_t));
                    $$ = createVarParam(ptr);
                }
                | ID
                {
                    Params *ptr = createParam(createIDList($1), createParamType(VOID_t));
                    $$ = createVarParam(ptr);
                }
                ;

initial_array : L_BRACE literal_list R_BRACE
              ;

literal_list : literal_list COMMA logical_expression
             | logical_expression
             |
             ;

const_decl 	: CONST scalar_type const_list SEMICOLON
            {
                SymNode *newNode;
                ConstParam *constListPtr;
                for(constListPtr = $3; constListPtr != NULL; constListPtr = constListPtr->next) {
                    newNode = createConstNode(constListPtr->name, scope, $2, constListPtr->value);
                    if(IDRedecl(symTable, scope, constListPtr->name) == false) {
                        insertIntoSymTable(symTable, newNode);
                    }
                    else {
                        printError(REDECL_t, linenum, constListPtr->name);
                        semError = true;
                    }
                }
            }
            ;

const_list : const_list COMMA ID ASSIGN_OP literal_const
            {
                combineDeclare(CONSTANT_t, $1, createConstParam($5, $3));
                $$ = $1;
            }
           | ID ASSIGN_OP literal_const {$$ = createConstParam($3, $1);}
           ;

array_decl : ID dim
           {
               $$ = createParam(createIDList($1), $2);
           }
           ;

dim : dim ML_BRACE INT_CONST MR_BRACE { increaseArrayDim($1, $3); }
    | ML_BRACE INT_CONST MR_BRACE     { $$ = createParamType(VOID_t); increaseArrayDim($$, $2); }
    ;

compound_statement: {
                        scope++;
                    }
                    L_BRACE var_const_stmt_list R_BRACE
                    {
                        if(Opt_Symbol) {
                            printSymTable(symTable, scope);
                        }
                        deleteScopeNode(symTable, scope);
                        scope--;
                    }
                    ;

var_const_stmt_list : var_const_stmt_list statement
                    | var_const_stmt_list var_decl
                    | var_const_stmt_list const_decl
                    |
                    ;

statement : compound_statement
          | simple_statement
          | conditional_statement
          | while_statement
          | for_statement
          | function_invoke_statement
          | jump_statement
          ;

simple_statement : variable_reference ASSIGN_OP logical_expression SEMICOLON
                 | PRINT logical_expression SEMICOLON
                 | READ variable_reference SEMICOLON
                 ;

conditional_statement : IF L_PAREN logical_expression R_PAREN compound_statement
                      | IF L_PAREN logical_expression R_PAREN compound_statement
                        ELSE compound_statement
                      ;


while_statement : WHILE L_PAREN logical_expression R_PAREN compound_statement
                | DO compound_statement WHILE L_PAREN logical_expression R_PAREN SEMICOLON
                ;



for_statement : FOR L_PAREN initial_expression_list SEMICOLON control_expression_list SEMICOLON increment_expression_list R_PAREN compound_statement
              ;

initial_expression_list : initial_expression
                        |
                        ;

initial_expression : initial_expression COMMA variable_reference ASSIGN_OP logical_expression
                   | initial_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression
                   ;

control_expression_list : control_expression
                        |
                        ;

control_expression : control_expression COMMA variable_reference ASSIGN_OP logical_expression
                   | control_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression
                   ;

increment_expression_list : increment_expression
                          |
                          ;

increment_expression : increment_expression COMMA variable_reference ASSIGN_OP logical_expression
                     | increment_expression COMMA logical_expression
                     | logical_expression
                     | variable_reference ASSIGN_OP logical_expression
                     ;

function_invoke_statement : ID L_PAREN logical_expression_list R_PAREN SEMICOLON
                          | ID L_PAREN R_PAREN SEMICOLON
                          ;

jump_statement : CONTINUE SEMICOLON
               | BREAK SEMICOLON
               | RETURN logical_expression SEMICOLON
               ;

variable_reference : ID
                   | variable_reference dimension
                   ;

dimension : ML_BRACE arithmetic_expression MR_BRACE
          ;

logical_expression : logical_expression OR_OP logical_term
                   | logical_term
                   ;

logical_term : logical_term AND_OP logical_factor
             | logical_factor
             ;

logical_factor : NOT_OP logical_factor
               | relation_expression
               ;

relation_expression : arithmetic_expression relation_operator arithmetic_expression
                    | arithmetic_expression
                    ;

relation_operator : LT_OP
                  | LE_OP
                  | EQ_OP
                  | GE_OP
                  | GT_OP
                  | NE_OP
                  ;

arithmetic_expression : arithmetic_expression ADD_OP term
                      | arithmetic_expression SUB_OP term
                      | relation_expression
                      | term
                      ;

term : term MUL_OP factor
     | term DIV_OP factor
     | term MOD_OP factor
     | factor
     ;

factor : variable_reference
       | SUB_OP variable_reference
       | L_PAREN logical_expression R_PAREN
       | SUB_OP L_PAREN logical_expression R_PAREN
       | ID L_PAREN logical_expression_list R_PAREN
       | SUB_OP ID L_PAREN logical_expression_list R_PAREN
       | ID L_PAREN R_PAREN
       | SUB_OP ID L_PAREN R_PAREN
       | literal_const
       ;

logical_expression_list : logical_expression_list COMMA logical_expression
                        | logical_expression
                        ;

scalar_type : INT {$$ = createParamType(INTEGER_t);}
            | DOUBLE {$$ = createParamType(DOUBLE_t);}
            | STRING {$$ = createParamType(STRING_t);}
            | BOOL   {$$ = createParamType(BOOLEAN_t);}
            | FLOAT  {$$ = createParamType(FLOAT_t);}
            ;

literal_const : INT_CONST
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->intVal = $1;
                  $$ = createConstAttr(INTEGER_t, tmp);
              }
              | SUB_OP INT_CONST
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->intVal = -$2;
                  $$ = createConstAttr(INTEGER_t, tmp);
              }
              | FLOAT_CONST
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->floatVal = $1;
                  $$ = createConstAttr(FLOAT_t, tmp);
              }
              | SUB_OP FLOAT_CONST
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->floatVal = -$2;
                  $$ = createConstAttr(FLOAT_t, tmp);
              }
              | SCIENTIFIC
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->doubleVal = $1;
                  $$ = createConstAttr(DOUBLE_t, tmp);
              }
              | SUB_OP SCIENTIFIC
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->doubleVal = -$2;
                  $$ = createConstAttr(DOUBLE_t, tmp);
              }
              | STR_CONST
              {
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->stringVal = malloc(sizeof(char)*1024);
                  memset(tmp->stringVal, 0, sizeof(tmp->stringVal));
                  strcpy(tmp->stringVal, $1);
                  $$ = createConstAttr(STRING_t, tmp);
              }
              | TRUE
              {
                  bool val = true;
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->boolVal = val;
                  $$ = createConstAttr(BOOLEAN_t, tmp);
              }
              | FALSE
              {
                  bool val = false;
                  ConstVal *tmp = malloc(sizeof(ConstVal));
                  tmp->boolVal = val;
                  $$ = createConstAttr(BOOLEAN_t, tmp);
              }
              ;

%%

int yyerror( char *msg )
{
    fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
    fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
    fprintf( stderr, "|\n" );
    fprintf( stderr, "| Unmatched token: %s\n", yytext );
    fprintf( stderr, "|--------------------------------------------------------------------------\n" );
    exit(-1);
}
