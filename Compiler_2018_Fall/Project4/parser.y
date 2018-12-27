%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "datatype.h"
#include "symtable.h"
#include "semcheck.h"

extern int linenum;
extern FILE	*yyin;
extern char	*yytext;
extern char buf[256];
extern int Opt_Symbol;

SymTable *symTable;

ExtType *funcReturn;
PARERROR paramError;
bool semError = false;
int scope = 0;
int inloop = 0;

int yylex();
%}

%union {
    int intVal;
    float floatVal;
    double doubleVal;
    char *idName;
    char *stringVal;
    struct IDNODE_SEM *id;
    struct CONSTATTR *constVal;
    struct EXTTYPE *exttype;
    struct PAR_SEM *par;
    struct EXPR_SEM *exprs;
    struct EXPR_SEM_NODE *exprNode;
    struct CONSTPARAM *constNode;
    struct VARDECL *varDeclNode;
};

%token	LE_OP NE_OP GE_OP EQ_OP AND_OP OR_OP
%token	READ BOOLEAN WHILE DO IF ELSE TRUE FALSE FOR INT PRINT BOOL VOID FLOAT DOUBLE STRING CONTINUE BREAK RETURN CONST
%token	L_PAREN R_PAREN COMMA SEMICOLON ML_BRACE MR_BRACE L_BRACE R_BRACE ADD_OP SUB_OP MUL_OP DIV_OP MOD_OP ASSIGN_OP LT_OP GT_OP NOT_OP

%token <idName>ID
%token <intVal>INT_CONST
%token <floatVal>FLOAT_CONST
%token <doubleVal>SCIENTIFIC
%token <stringVal>STR_CONST

%type<exttype> scalar_type dim
%type<par> array_decl parameter_list
%type<constVal> literal_const
%type<constNode> const_list
%type<exprs> variable_reference logical_expression logical_term logical_factor relation_expression arithmetic_expression term factor logical_expression_list literal_list initial_array function_invoke_statement
%type<intVal> relation_operator dimension
%type<varDeclNode> identifier_list


%start program
%%

program : decl_list funct_def decl_and_def_list
        {
            if(Opt_Symbol == 1)
                printSymTable( symTable, scope );
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


funct_def : scalar_type ID L_PAREN R_PAREN // function with return
            {
                funcReturn = $1;
                SymNode *node;
                node = findFuncDeclaration( symTable, $2 );

                if( node != 0 ){
                    // function declared and verified
                    if(checkFuncDeclaration( symTable, 0, $1, node ) == true)
                        node->declAndDefined = true;
                }
                else {
                    insertFuncIntoSymTable( symTable, $2, 0, $1, scope, true );
                }
                free($2);
                scope++;
            }
            funct_def_end
          | scalar_type ID L_PAREN parameter_list R_PAREN   // function with return and param
            {
                funcReturn = $1;

                paramError = checkFuncParam( $4 );
                bool canInsert = false;
                switch( paramError ){
                    case Redecl_t:
                        printError(IDreused);
                        break;
                    case NoErr_t:
                        // nothing happens
                        canInsert = true;
                        break;
                    case Other_t:
                        printError(ParameterError);
                        break;
                    default:
                        printError(ParameterError);
                        break;
                }
                // check and insert function into symbol table
                if(canInsert){
                    SymNode *node;
                    node = findFuncDeclaration( symTable, $2 );

                    if( node != 0 ){
                        if(checkFuncDeclaration( symTable, $4, $1, node ) == true){
                            insertParamIntoSymTable( symTable, $4, scope+1 );
                            node->declAndDefined = true;
                        }
                    }
                    else{
                        // node not declared yet, just insert into symtable.
                        insertParamIntoSymTable( symTable, $4, scope+1 );
                        insertFuncIntoSymTable( symTable, $2, $4, $1, scope, true );
                    }
                }
                free($2);
                scope++;
            }
            funct_def_end
          | VOID ID L_PAREN R_PAREN
            {
                funcReturn = createExtType(VOID_t);
                SymNode *node;
                node = findFuncDeclaration( symTable, $2 );

                if( node != 0 ){
                    if(checkFuncDeclaration( symTable, 0, createExtType( VOID_t ), node ) == true)
                        node->declAndDefined = true;
                }
                else{
                    insertFuncIntoSymTable( symTable, $2, 0, createExtType( VOID_t ), scope, true );
                }
            }
            compound_statement { funcReturn = NULL; }
          | VOID ID L_PAREN parameter_list R_PAREN
            {
                funcReturn = createExtType(VOID_t);

                paramError = checkFuncParam( $4 );
                bool canInsert = false;
                switch( paramError ){
                    case Redecl_t:
                        printError(IDreused);
                        break;
                    case NoErr_t:
                        // nothing happens
                        canInsert = true;
                        break;
                    case Other_t:
                        printError(ParameterError);
                        break;
                    default:
                        printError(ParameterError);
                        break;
                }
                // check and insert function into symbol table
                if(canInsert){
                    SymNode *node;
                    node = findFuncDeclaration( symTable, $2 );

                    if( node != 0 ){
                        if(checkFuncDeclaration( symTable, $4, createExtType( VOID_t ), node ) == true){
                            insertParamIntoSymTable( symTable, $4, scope+1 );
                            node->declAndDefined = true;
                        }
                    }
                    else{
                        insertParamIntoSymTable( symTable, $4, scope+1 );
                        insertFuncIntoSymTable( symTable, $2, $4, createExtType( VOID_t ), scope, true );
                    }
                }
            }
            compound_statement { funcReturn = NULL; }
          ;

funct_def_end : L_BRACE var_const_stmt_list function_invoke_statement R_BRACE   // end with invoke
                {
                    checkReturnStatement($3, funcReturn);
                    if(Opt_Symbol == 1) {
                        printSymTable(symTable, scope);
                    }
                    deleteScope(symTable, scope);
                    scope--;
                    funcReturn = NULL;
                }
              | L_BRACE var_const_stmt_list RETURN logical_expression SEMICOLON R_BRACE // end with return
                {
                    checkReturnStatement($4, funcReturn);
                    if(Opt_Symbol == 1) {
                        printSymTable(symTable, scope);
                    }
                    deleteScope(symTable, scope);
                    scope--;
                    funcReturn = NULL;
                }
              ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON
            {
                insertFuncIntoSymTable( symTable, $2, 0, $1, scope, false );
            }
           | scalar_type ID L_PAREN parameter_list R_PAREN SEMICOLON
            {
                paramError = checkFuncParam( $4 );
                bool canInsert = false;
                switch( paramError ){
                    case Redecl_t:
                        printError(IDreused);
                        break;
                    case NoErr_t:
                        // nothing happens
                        canInsert = true;
                        break;
                    case Other_t:
                        printError(ParameterError);
                        break;
                    default:
                        printError(ParameterError);
                        break;
                }
                if(canInsert) {
                    insertFuncIntoSymTable( symTable, $2, $4, $1, scope, false );
                }
            }
           | VOID ID L_PAREN R_PAREN SEMICOLON
            {
                insertFuncIntoSymTable( symTable, $2, 0, createExtType( VOID_t ), scope, false );
            }
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON
            {
                paramError = checkFuncParam( $4 );
                bool canInsert = false;
                switch( paramError ){
                    case Redecl_t:
                        printError(IDreused);
                        break;
                    case NoErr_t:
                        // nothing happens
                        canInsert = true;
                        break;
                    case Other_t:
                        printError(ParameterError);
                        break;
                    default:
                        printError(ParameterError);
                        break;
                }
                if(canInsert) {
                    insertFuncIntoSymTable( symTable, $2, $4, createExtType( VOID_t ), scope, false );
                }
            }
           ;

parameter_list : parameter_list COMMA scalar_type ID
               {
                ParamSemantic *ptr = createParam( createIdList( $4 ), $3 );
                concateParam( "ParamSem", $1, ptr );
                $$ = $1;
               }
               | parameter_list COMMA scalar_type array_decl
               {
                $4->extType->type= $3->type;
                concateParam( "ParamSem", $1, $4 );
                $$ = $1;
               }
               | scalar_type array_decl
               {
                $2->extType->type = $1->type;
                $$ = $2;
               }
               | scalar_type ID { $$ = createParam( createIdList( $2 ), $1 ); }
               ;

var_decl : scalar_type identifier_list SEMICOLON
            {
                VarDecl *ptr = $2;
                while( ptr != NULL ){
                    if( checkRedeclaration( symTable, ptr->para->idlist->value, scope ) == false ) { }
                    else {
                        if( checkVarInitValue( $1, ptr, symTable, scope ) ==  true ){
                            SymNode *newNode = createNode( ptr->para->idlist->value, scope, ptr->para->extType, VARIABLE_t, 0 );
                            insertIntoSymTable( symTable, newNode );
                        }
                    }
                    ptr = ptr->next;
                }
            }
            ;

identifier_list : identifier_list COMMA ID
                {
                    ParamSemantic *ptr = createParam( createIdList( $3 ), createExtType( VOID_t ) );
                    VarDecl *vptr = createVarDecl( ptr, 0 );
                    concateParam("VarDecl", $1, vptr );
                    $$ = $1;
                }
                | identifier_list COMMA ID ASSIGN_OP logical_expression
                {
                    ParamSemantic *ptr = createParam( createIdList( $3 ), createExtType( VOID_t ) );
                    VarDecl *vptr = createVarDecl( ptr, $5 );
                    vptr->isArray = true;
                    vptr->isInit = true;
                    concateParam("VarDecl", $1, vptr );
                    $$ = $1;

                }
                | identifier_list COMMA array_decl ASSIGN_OP initial_array
                {
                    VarDecl *ptr = createVarDecl( $3, $5 );
                    ptr->isArray = true;
                    ptr->isInit = true;
                    concateParam("VarDecl", $1, ptr );
                    $$ = $1;
                }
                | identifier_list COMMA array_decl
                {
                    VarDecl *ptr = createVarDecl( $3, 0 );
                    ptr->isArray = true;
                    concateParam("VarDecl", $1, ptr );
                    $$ = $1;
                }
                | array_decl ASSIGN_OP initial_array
                {
                    $$ = createVarDecl( $1 , $3 );
                    $$->isArray = true;
                    $$->isInit = true;
                }
                | array_decl
                {
                    $$ = createVarDecl( $1 , 0 );
                    $$->isArray = true;
                }
                | ID ASSIGN_OP logical_expression
                {
                    ParamSemantic *ptr = createParam( createIdList( $1 ), createExtType( VOID_t ) );
                    $$ = createVarDecl( ptr, $3 );
                    $$->isInit = true;
                }
                | ID
                {
                    ParamSemantic *ptr = createParam( createIdList( $1 ), createExtType( VOID_t ) );
                    $$ = createVarDecl( ptr, 0 );
                }
                ;

initial_array : L_BRACE literal_list R_BRACE { $$ = $2; }
              ;

literal_list : literal_list COMMA logical_expression
                {
                    ExprSemantic *ptr = $1;
                    while(ptr->next != NULL) { ptr = ptr->next; }
                    ptr->next = $3;
                    $$ = $1;
                }
             | logical_expression
                {
                    $$ = $1;
                }
             | { $$ = NULL; }
             ;

const_decl 	: CONST scalar_type const_list SEMICOLON
            {
                SymNode *newNode;
                constParam *ptr = $3;
                while(ptr != NULL) {
                    if( checkRedeclaration( symTable, ptr->name, scope ) == true ){
                        if( ptr->value->type != $2->type ){
                            if( !(($2->type==FLOAT_t || $2->type == DOUBLE_t ) && ptr->value->type==INTEGER_t) ) {
                                if(!($2->type==DOUBLE_t && ptr->value->type==FLOAT_t)){
                                    printError(ConstDeclTypeMismatch);
                                }
                                else{
                                    newNode = createNode( ptr->name, scope, $2, CONSTANT_t, ptr->value );
                                    insertIntoSymTable( symTable, newNode );
                                }
                            }
                            else{
                                newNode = createNode( ptr->name, scope, $2, CONSTANT_t, ptr->value );
                                insertIntoSymTable( symTable, newNode );
                            }
                        }
                        else{
                            newNode = createNode( ptr->name, scope, $2, CONSTANT_t, ptr->value );
                            insertIntoSymTable( symTable, newNode );
                        }
                    }
                    ptr = ptr->next;
                }
            }
            ;

const_list : const_list COMMA ID ASSIGN_OP literal_const
            {
                concateParam("ConstPar", $1, createConstParam( $5, $3 ) );
                $$ = $1;
            }
           | ID ASSIGN_OP literal_const
            {
                $$ = createConstParam( $3, $1 );
            }
           ;

array_decl : ID dim
            {
                $$ = createParam( createIdList( $1 ), $2 );
            }
           ;

dim : dim ML_BRACE INT_CONST MR_BRACE
        {
            if( $3 <= 0 ){
                printError(ArrSizeShouldG0);
            }
            else
                increaseArrayDim( $1, $3 );
        }
    | ML_BRACE INT_CONST MR_BRACE
        {
            if( $2 <= 0 ){
                printError(ArrSizeShouldG0);
            }
            else{
                $$ = createExtType( VOID_t );
                increaseArrayDim( $$, $2 );
            }
        }
    ;

compound_statement : {scope++;}L_BRACE var_const_stmt_list R_BRACE
                    {
                        // print contents of current scope
                        if( Opt_Symbol == 1 )
                            printSymTable( symTable, scope );
                            
                        // leaving this scope, delete nodes in this scope.
                        deleteScope( symTable, scope );	
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
                    {
                        bool flagLHS = checkExistence( symTable, $1, scope, true );
                        bool flagRHS = true;
                        if( $3->isDeref == false ) {
                            flagRHS = checkExistence( symTable, $3, scope, false );
                        }
                        if( flagLHS == true && flagRHS == true )
                            checkAssignmentTypeMatch( $1, $3 );
                    }
                 | PRINT logical_expression SEMICOLON
                    {
                        if ($2->extType->dim != 0) {
                            printError(StatementIsArrType);
                        }
                    }
                 | READ variable_reference SEMICOLON
                    {
                        if( checkExistence( symTable, $2, scope, true ) == true ) {
                            if ($2->extType->dim != 0) {
                                printError(StatementIsArrType);
                            }
                        }
                    }
                 ;

conditional_statement : IF L_PAREN conditional_if  R_PAREN compound_statement
                      | IF L_PAREN conditional_if  R_PAREN compound_statement
                        ELSE compound_statement
                      ;

conditional_if : logical_expression { checkBooleanExpr( $1 ); }
               ;

while_statement : WHILE L_PAREN logical_expression { checkBooleanExpr( $3 ); } R_PAREN { inloop++; }
                    compound_statement { inloop--; }
                | { inloop++; } DO compound_statement WHILE L_PAREN logical_expression R_PAREN SEMICOLON
                    {
                         checkBooleanExpr( $6 );
                         inloop--;

                    }
                ;



for_statement : FOR L_PAREN initial_expression SEMICOLON control_expression SEMICOLON increment_expression R_PAREN  { inloop++; }
                    compound_statement  { inloop--; }
              ;

initial_expression : initial_expression COMMA statement_for
                   | initial_expression COMMA logical_expression
                   | logical_expression
                   | statement_for
                   |
                   ;

control_expression : control_expression COMMA statement_for
                   {
                        printError(ControlStatNotBool);
                   }
                   | control_expression COMMA logical_expression
                   {
                        if( $3->extType->type != BOOLEAN_t ){
                            printError(ControlStatNotBool);
                        }
                   }
                   | logical_expression
                    {
                        if( $1->extType->type != BOOLEAN_t ){
                            printError(ControlStatNotBool);
                        }
                    }
                   | statement_for
                   {
                        printError(ControlStatNotBool);
                   }
                   |
                   ;

increment_expression : increment_expression COMMA statement_for
                     | increment_expression COMMA logical_expression
                     | logical_expression
                     | statement_for
                     |
                     ;

statement_for 	: variable_reference ASSIGN_OP logical_expression
                    {
                        // check if left hand side exists
                        bool left = checkExistence( symTable, $1, scope, true );
                        // check right hand side
                        bool right = true;
                        if( $3->isDeref == false ) {
                            right = checkExistence( symTable, $3, scope, false );
                        }
                        // if both exists, check their type
                        if( left == true && right == true )
                            checkAssignmentTypeMatch( $1, $3 );
                    }
                    ;


function_invoke_statement : ID L_PAREN logical_expression_list R_PAREN SEMICOLON
                            {
                                $$ = checkFuncInvoke( $1, $3, symTable, scope );
                            }
                          | ID L_PAREN R_PAREN SEMICOLON
                            {
                                $$ = checkFuncInvoke( $1, 0, symTable, scope );
                            }
                          ;

jump_statement : CONTINUE SEMICOLON
                {
                    if( inloop <= 0){
                        printError(ContinueOutOfLoop);
                    }
                }
               | BREAK SEMICOLON
                {
                    if( inloop <= 0 ){
                        printError(BreakOutOfLoop);
                    }
                }
               | RETURN logical_expression SEMICOLON
                {
                    checkReturnStatement( $2, funcReturn );
                }
               ;

variable_reference : ID { $$ = createExprSem( $1 ); }
                   | variable_reference dimension
                    {
                        increaseDim( $1, $2 );
                        $$ = $1;
                    }
                   ;

logical_expression : logical_expression OR_OP logical_term
                    {
                        checkAndOrOp( $1, OR_t, $3 );
                        $$ = $1;
                    }
                   | logical_term { $$ = $1; }
                   ;

logical_term : logical_term AND_OP logical_factor
                {
                    checkAndOrOp( $1, AND_t, $3 );
                    $$ = $1;
                }
             | logical_factor { $$ = $1; }
             ;

logical_factor : NOT_OP logical_factor
                {
                    checkUnaryNOT( $2 );
                    $$ = $2;
                }
               | relation_expression { $$ = $1; }
               ;

relation_expression : arithmetic_expression relation_operator arithmetic_expression
                    {
                        checkRelOp( $1, $2, $3 );
                        $$ = $1;
                    }
                    | arithmetic_expression { $$ = $1; }
                    ;

relation_operator : LT_OP { $$ = LT_t; }
                  | LE_OP { $$ = LE_t; }
                  | EQ_OP { $$ = EQ_t; }
                  | GE_OP { $$ = GE_t; }
                  | GT_OP { $$ = GT_t; }
                  | NE_OP { $$ = NE_t; }
                  ;

arithmetic_expression : arithmetic_expression ADD_OP term
                        {
                            checkArithmeticOp( $1, ADD_t, $3 );
                            $$ = $1;
                        }
                      | arithmetic_expression SUB_OP term
                        {
                            checkArithmeticOp( $1, SUB_t, $3 );
                            $$ = $1;
                        }
                      | relation_expression { $$ = $1; }
                      | term { $$ = $1; }
                      ;

term : term MUL_OP factor
        {
            checkArithmeticOp( $1, MUL_t, $3 );
            $$ = $1;
        }
     | term DIV_OP factor
        {
            checkArithmeticOp( $1, DIV_t, $3 );
            $$ = $1;
        }
     | term MOD_OP factor 
        {
            checkArithmeticOp( $1, MOD_t, $3 );
        }
     |  factor { $$ = $1; }
     ;

factor : variable_reference
        {
            checkExistence( symTable, $1, scope, false );
            $$ = $1;
            $$->startOperator = NONE_t;
        }
       | SUB_OP variable_reference
        {
            if( checkExistence( symTable, $2, scope, false ) == true ) {
                checkUnaryMinus( $2 );
            }
            $$ = $2;
            $$->startOperator = SUB_t;
        }
       | L_PAREN logical_expression R_PAREN
        {
            $2->startOperator = NONE_t;
            $$ = $2;
        }
       | SUB_OP L_PAREN logical_expression R_PAREN
        {
            checkUnaryMinus( $3 );
            $$ = $3;
            $$->startOperator = SUB_t;
        }
       | ID L_PAREN logical_expression_list R_PAREN
        {
            $$ = checkFuncInvoke( $1, $3, symTable, scope );
            $$->startOperator = NONE_t;
        }
       | SUB_OP ID L_PAREN logical_expression_list R_PAREN
        {
            $$ = checkFuncInvoke( $2, $4, symTable, scope );
            $$->startOperator = SUB_t;
        }
       | ID L_PAREN R_PAREN
        {
            $$ = checkFuncInvoke( $1, 0, symTable, scope );
            $$->startOperator = NONE_t;
        }
       | SUB_OP ID L_PAREN R_PAREN
        {
            $$ = checkFuncInvoke( $2, 0, symTable, scope );
            $$->startOperator = SUB_OP;
        }
       | literal_const
        {
              $$ = malloc(sizeof(ExprSemantic));
              $$->isDeref = true;
              $$->varRef = 0;
              $$->extType = createExtType( $1->type );
              $$->next = 0;
              if( $1->isNegative == true ) {
                  $$->startOperator = SUB_t;
              }
              else {
                $$->startOperator = NONE_t;
              }
        }
       ;

logical_expression_list : logical_expression_list COMMA logical_expression
                        {
                              ExprSemantic *exprPtr = $1;
                              while(exprPtr->next != NULL) { exprPtr = exprPtr->next; }
                              exprPtr->next = $3;
                              $$ = $1;
                        }
                        | logical_expression { $$ = $1; }
                        ;

dimension : ML_BRACE arithmetic_expression MR_BRACE { $$ = checkArrayIndex( $2 ); }
          ;

scalar_type : INT { $$ = createExtType( INTEGER_t ); }
            | DOUBLE { $$ = createExtType( DOUBLE_t ); }
            | STRING { $$ = createExtType( STRING_t ); }
            | BOOL { $$ = createExtType( BOOLEAN_t ); }
            | FLOAT { $$ = createExtType( FLOAT_t ); }
            ;

literal_const : INT_CONST
                {
                    int val = $1;
                    $$ = createConstantAttribute( INTEGER_t, &val );
                }
              | SUB_OP INT_CONST
                {
                    int val = -$2;
                    $$ = createConstantAttribute( INTEGER_t, &val );
                }
              | FLOAT_CONST
                {
                    float val = $1;
                    $$ = createConstantAttribute( FLOAT_t, &val );
                }
              | SUB_OP FLOAT_CONST
                {
                    float val = -$2;
                    $$ = createConstantAttribute( FLOAT_t, &val );
                }
              | SCIENTIFIC
                {
                    double val = $1;
                    $$ = createConstantAttribute( DOUBLE_t, &val );
                }
              | SUB_OP SCIENTIFIC
                {
                    double val = -$2;
                    $$ = createConstantAttribute( DOUBLE_t, &val );
                }
              | STR_CONST
                {
                    $$ = createConstantAttribute( STRING_t, $1 );
                }
              | TRUE
                {
                    bool val = true;
                    $$ = createConstantAttribute( BOOLEAN_t, &val );
                }
              | FALSE
                {
                    bool val = false;
                    $$ = createConstantAttribute( BOOLEAN_t, &val );
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
    //  fprintf( stderr, "%s\t%d\t%s\t%s\n", "Error found in Line ", linenum, "next token: ", yytext );
}