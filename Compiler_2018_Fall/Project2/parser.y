%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */

extern int yylex(void);
extern int yyparse(void);
%}

// Delimiters
%token COMMA SEMICOLON

// Brackets
%token L_PAREN R_PAREN L_SQUARE_BRACKET R_SQUARE_BRACKET L_CURLY_BRACKET R_CURLY_BRACKET

// Mathematical calculation
%token PLUS MINUS MULTIPLY DIVIDE MOD

// Assignment
%token ID ASSIGN

// Logical operator
%token LT LE NE GE GT EQ AND OR NOT

// Type
%token INT BOOL VOID FLOAT DOUBLE STRING CONST

// Keywords
%token READ WHILE DO IF ELSE TRUE FALSE FOR PRINT CONTINUE BREAK RETURN

// Literals
%token INTEGER_LITERAL FLOAT_LITERAL SCIENTIFIC STRING_LITERAL

%%

program : declaration_list funct_def decl_and_def_list;

declaration_list : declaration_list var_decl
                 | declaration_list const_decl
                 | declaration_list funct_decl
                 | /* starts with function def */
                 ;

decl_and_def_list : decl_and_def_list var_decl
                  | decl_and_def_list const_decl
                  | decl_and_def_list funct_decl
                  | decl_and_def_list funct_def
                  |
                  ;

funct_def : general_func_def
          | void_func_def   /* procedure */
          ; 

general_func_def : type ID L_PAREN R_PAREN compound  /* type ID() */
                 | type ID L_PAREN arg_list R_PAREN compound /* type ID(args) */
                 ;

void_func_def : VOID ID L_PAREN R_PAREN compound
              | VOID ID L_PAREN arg_list R_PAREN compound
              ;

funct_decl : general_func_decl
           | void_func_decl /* procedure */
           ;

general_func_decl : type ID L_PAREN R_PAREN SEMICOLON
                  | type ID L_PAREN arg_list R_PAREN SEMICOLON
                  ;

void_func_decl : VOID ID L_PAREN R_PAREN SEMICOLON
               | VOID ID L_PAREN arg_list R_PAREN SEMICOLON
               ;

arg_list : arg_list COMMA type ID
         | arg_list COMMA type arr_decl
         | type arr_decl
         | type ID
         ;

var_decl : type id_list SEMICOLON;

id_list : ID
        | ID ASSIGN expr
        | arr_decl
        | arr_decl ASSIGN arr_init
        | id_list COMMA ID
        | id_list COMMA ID ASSIGN expr
        | id_list COMMA arr_decl ASSIGN arr_init
        | id_list COMMA arr_decl
        ;

    /* int a[5] = {2+5, 3, 3||5}; , can be an expression */
arr_init : L_CURLY_BRACKET literal_list R_CURLY_BRACKET;

literal_list : literal_list COMMA expr
             | expr
             | /* empty init: int a[4] = {}; */
             ;

const_decl : CONST type const_list SEMICOLON;

const_list : const_list COMMA ID ASSIGN literal
           | ID ASSIGN literal
           ;

arr_decl : ID arr_indice;

arr_indice : single_arr_indice arr_indice
           | single_arr_indice
           ; 

    /* array declaration can only be integer like a[5][2] */
single_arr_indice : L_SQUARE_BRACKET INTEGER_LITERAL R_SQUARE_BRACKET;

compound : L_CURLY_BRACKET statments_list R_CURLY_BRACKET;

statments_list : statments_list var_decl
               | statments_list const_decl
               | statments_list statement 
               | /* empty */
               ;

statement : compound
          | simple_stat
          | if_else_stat
          | while_stat
          | for_stat
          | jump_stat
          | call_funct_stat
          ;

simple_stat : var_ref ASSIGN expr SEMICOLON
            | PRINT expr SEMICOLON
            | READ var_ref SEMICOLON
            ;

if_else_stat : single_if
             | if_else
             ;

single_if : IF L_PAREN expr R_PAREN compound;

if_else : IF L_PAREN expr R_PAREN compound ELSE compound;

while_stat : WHILE L_PAREN expr R_PAREN compound
           | DO compound WHILE L_PAREN expr R_PAREN SEMICOLON
           ;

for_stat : FOR L_PAREN for_expr_list SEMICOLON for_expr_list SEMICOLON for_expr_list R_PAREN compound;

for_expr_list : for_expr
              | /* can be empty, but SEMICOLON will remain exist */
              ;

for_expr : for_expr COMMA var_ref ASSIGN expr
         | for_expr COMMA expr
         | var_ref ASSIGN expr
         | expr
         ;

jump_stat : RETURN expr SEMICOLON
          | BREAK SEMICOLON
          | CONTINUE SEMICOLON
          ;

call_funct_stat : ID L_PAREN expr_list R_PAREN SEMICOLON
                | ID L_PAREN R_PAREN SEMICOLON
                ;

var_ref : ID
        | arr_list
        ;

expr : expr AND logic_term
     | logic_term
     ;

logic_term : logic_term OR logic_factor
           | logic_factor
           ;

logic_factor : NOT logic_factor
             | relat_expr
             ;

relat_expr : arith_expr relat_op arith_expr
              | arith_expr
              ;

relat_op : LT | LE | GE | GT | EQ | NE;

arith_expr : arith_expr PLUS general_term
           | arith_expr MINUS general_term
           | relat_expr
           | general_term
           ;

general_term : general_term MULTIPLY general_factor
             | general_term DIVIDE general_factor
             | general_term MOD general_factor
             | general_factor
             ;

general_factor : var_ref
               | MINUS general_factor
               | L_PAREN expr R_PAREN
               | MINUS L_PAREN expr R_PAREN
               | ID L_PAREN expr_list R_PAREN
               | ID L_PAREN R_PAREN
               | MINUS ID L_PAREN expr R_PAREN
               | MINUS ID L_PAREN R_PAREN
               | literal
               ; 

expr_list : expr_list COMMA expr
          | expr
          ;

arr_list : ID right_arr_indice; /* array as a rvalue, index can be an expression */

right_arr_indice : right_arr_indice single_r_arr_indice
                 | single_r_arr_indice
                 ; 

single_r_arr_indice : L_SQUARE_BRACKET expr R_SQUARE_BRACKET;

type : INT | DOUBLE | FLOAT | BOOL | STRING;

literal : INTEGER_LITERAL       /* a = 2  */
        | MINUS INTEGER_LITERAL /* a = -2 */
        | FLOAT_LITERAL         /* a = 0.4 */
        | MINUS FLOAT_LITERAL
        | SCIENTIFIC
        | MINUS SCIENTIFIC
        | STRING_LITERAL
        | boolean
        ;

boolean : TRUE | FALSE;

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

int  main( int argc, char **argv )
{
    if( argc != 2 ) {
        fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
        exit(0);
    }

    FILE *fp = fopen( argv[1], "r" );
    
    if( fp == NULL )  {
        fprintf( stdout, "Open  file  error\n" );
        exit(-1);
    }
    
    yyin = fp;
    yyparse();

    fprintf( stdout, "\n" );
    fprintf( stdout, "|--------------------------------|\n" );
    fprintf( stdout, "|  There is no syntactic error!  |\n" );
    fprintf( stdout, "|--------------------------------|\n" );
    exit(0);
}

