%{
// #define DEBUG
#include <string.h>
#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define STRCAT(s)           { strncat(str, s, sizeof(str) - strlen(str) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<%s>\n", #t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define MAX_LINE_LENGTH 257
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char buf[MAX_LINE_LENGTH];
char str[MAX_LINE_LENGTH];
void print();
void error();
void clear();
void insert(char *);
void print_result();

typedef struct IdNode {
    char *id;
    int count;
    struct IdNode *next;
}NODE;
%}

ws    [ \t]
char    [a-zA-Z]
digit    [0-9]
nzero    [1-9]
integer    0|({nzero}({digit}*))
float    {digit}+"."{digit}+
SCInotation    ({digit}*(\.{digit}+)?)[Ee][+-]?{digit}+
identifier    [a-z_A-Z][a-zA-Z_0-9]*

%Start C_COMMENT
%Start CPP_COMMENT
%Start STRING
%Start PRAGMA 
%Start PRAGMA_SOU
%Start PRAGMA_TOK
%Start PRAGMA_STAT
%Start AFTER_PRAGMA_COMMENT
%Start PRAG_C_COMMENT
%%
    /* ===== COMMENT ===== */
<INITIAL>"//" { /* start of C++ style comment */
   LIST;
   BEGIN CPP_COMMENT;
}
<INITIAL>"/*" { /* start of C style comment */
   LIST;
   BEGIN C_COMMENT;
}
<C_COMMENT>"*/" {
   LIST;
   BEGIN INITIAL;
}
<PRAG_C_COMMENT>\n {
   LIST;
   print();
}
<C_COMMENT>\n {
   LIST;
   print();
}
<CPP_COMMENT>\n {
    BEGIN INITIAL;
    LIST;
    print();
}
<CPP_COMMENT>.  {LIST;}
<C_COMMENT>.    {LIST;}

    /* ===== PRAGMA ===== */
<INITIAL>^#pragma{ws}+ {
    LIST;
    BEGIN PRAGMA;
}
    /* === Source === */
<PRAGMA>source{ws}+ {
    LIST;
    BEGIN PRAGMA_SOU;
}
<PRAGMA_SOU>on {
    LIST;
    Opt_Source = 1;
    BEGIN AFTER_PRAGMA_COMMENT;
}
<PRAGMA_SOU>off {
    LIST;
    Opt_Source = 0;
    BEGIN AFTER_PRAGMA_COMMENT;
}
    /* === Token === */
<PRAGMA>token{ws}+ {
    LIST;
    BEGIN PRAGMA_TOK;
}
<PRAGMA_TOK>on {
    LIST;
    Opt_Token = 1;
    BEGIN AFTER_PRAGMA_COMMENT;
}
<PRAGMA_TOK>off {
    LIST;
    Opt_Token = 0;
    BEGIN AFTER_PRAGMA_COMMENT;
}
    /* === Statistic === */
<PRAGMA>statistic{ws}+ {
    LIST;
    BEGIN PRAGMA_STAT;
}
<PRAGMA_STAT>on {
    LIST;
    Opt_Statistic = 1;
    BEGIN AFTER_PRAGMA_COMMENT;
}
<PRAGMA_STAT>off {
    LIST;
    Opt_Statistic = 0;
    BEGIN AFTER_PRAGMA_COMMENT;
}
    /* === Comment after pragma === */
<AFTER_PRAGMA_COMMENT>"//" { /* comment is in C++ style */
   LIST;
   BEGIN CPP_COMMENT;   /* just do same thing as C++ comment */
}
<AFTER_PRAGMA_COMMENT>"/*" { /* comment is in C style */
   LIST;
   BEGIN PRAG_C_COMMENT;
}
<PRAG_C_COMMENT>.*"*/" {
   LIST;
   BEGIN AFTER_PRAGMA_COMMENT;
}

<PRAG_C_COMMENT>. {LIST;}
<PRAGMA>\n     |
<PRAGMA_SOU>\n       |
<PRAGMA_TOK>\n       |
<PRAGMA_STAT>\n       |
<AFTER_PRAGMA_COMMENT>\n  {
    BEGIN INITIAL;
    LIST;
    print();
}

    /* ===== DELIM ===== */
<INITIAL>,  {tokenString("delim", ",");}
<INITIAL>;  {tokenString("delim", ";");}
<INITIAL>\( {tokenString("delim", "(");}
<INITIAL>\) {tokenString("delim", ")");}
<INITIAL>\[ {tokenString("delim", "[");}
<INITIAL>\] {tokenString("delim", "]");}
<INITIAL>\{ {tokenString("delim", "{");}
<INITIAL>\} {tokenString("delim", "}");}

    /* ===== KEYWORD ===== */
<INITIAL>"while" {tokenString("KW", "while");}
<INITIAL>"do" {tokenString("KW", "do");}
<INITIAL>"if" {tokenString("KW", "if");}
<INITIAL>"else" {tokenString("KW", "else");}
<INITIAL>"true" {tokenString("KW", "true");}
<INITIAL>"false" {tokenString("KW", "false");}
<INITIAL>"for" {tokenString("KW", "for");}
<INITIAL>"int" {tokenString("KW", "int");}
<INITIAL>"print" {tokenString("KW", "print");}
<INITIAL>"const" {tokenString("KW", "const");}
<INITIAL>"read" {tokenString("KW", "read");}
<INITIAL>"boolean" {tokenString("KW", "boolean");}
<INITIAL>"bool" {tokenString("KW", "bool");}
<INITIAL>"void" {tokenString("KW", "void");}
<INITIAL>"float" {tokenString("KW", "float");}
<INITIAL>"double" {tokenString("KW", "double");}
<INITIAL>"string" {tokenString("KW", "string");}
<INITIAL>"continue" {tokenString("KW", "continue");}
<INITIAL>"break" {tokenString("KW", "break");}
<INITIAL>"return" {tokenString("KW", "return");}

    /* ===== IDENTIFIER ===== */
<INITIAL>{identifier} {
    tokenString("id", yytext);
#ifdef DEBUG
    printf("\t insert: %s\n", yytext);
#endif
    insert(yytext);
}

    /* ===== OPERATORS ===== */
<INITIAL>"+" {token("+");}
<INITIAL>"-" {token("-");}
<INITIAL>"*" {token("*");}
<INITIAL>"/" {token("/");}
<INITIAL>"%" {token("%");}
<INITIAL>"=" {token("=");}
<INITIAL>"<" {token("<");}
<INITIAL>"<=" {token("<=");}
<INITIAL>"!=" {token("!=");}
<INITIAL>">=" {token(">=");}
<INITIAL>">" {token(">");}
<INITIAL>"==" {token("==");}
<INITIAL>"&&" {token("&&");}
<INITIAL>"||" {token("||");}
<INITIAL>"!" {token("!");}

    /* ===== NUMBERS ===== */
<INITIAL>{integer} {tokenString("integer", yytext);}
<INITIAL>{float} {tokenString("float", yytext);}
<INITIAL>{SCInotation} {tokenString("scientific", yytext);}

    /* ===== STRING ===== */
<INITIAL>\" { /* start of string */
    LIST;
    str[0] = '\0';
    BEGIN STRING;
}
<STRING>\" { /* end of string */
    tokenString("string", str);
    str[0] = '\0';
    BEGIN INITIAL;
}
<STRING>[\\][\"] {
    LIST;
    STRCAT("\"");
}
<STRING>[\\]n {
    LIST;
    STRCAT("\n");
}
<STRING>[\\]t {
    LIST;
    STRCAT("\t");
}
<STRING>[\\][\\] {
    LIST;
    STRCAT("\\");
}
<STRING>. {
    LIST;
    STRCAT(yytext);
}
<STRING>\n { /* newline is not allowed in string */
    error();
    exit(1);
}

<INITIAL>{ws} {LIST;}

. { // Unrecognized character
    error();
    exit(1);
}

<INITIAL>\n {
    BEGIN INITIAL;
    LIST;
    print();
}
%%
NODE *head = NULL;

int main( int argc, char **argv ){
    if (argc >= 2){
        yyin = fopen( argv[1], "r" );
        if ( NULL == yyin ) {
            fprintf( stderr, "Cannot open: %s\n", argv[1] );
            exit(-1);
        }
    }
    else{
        yyin = stdin;
    }

    yylex();

    if (Opt_Statistic){
        // Print frequency table
        print_result();
        clear();
    }
    exit(0);
}

void print(){
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

void error(){
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    clear();
}

void clear(){
    while(head != NULL){
        NODE *current = head;
        head = head->next;
        free(current);
        current = NULL;
    }
}

void insert(char *iden){
    NODE* current = head;
    int find = 0;
    while(current != NULL){
        if(strcmp(iden, current->id) == 0){
            (current->count)++;
            find = 1;
            break;
        }
        else
            current = current->next;
    }

    if(find == 0){
        NODE* newNode = (NODE*)malloc(sizeof(NODE));
        char *newId = (char*)malloc(sizeof(char)*MAX_LINE_LENGTH);
        strcpy(newId, iden);
        newNode->id = newId;
        newNode->count = 1;
        newNode->next = NULL;
        if(head == NULL){
            head = newNode;
        }
        else{
            current = head;
            while(current->next != NULL){
                current = current->next;
            }
            current->next = newNode;
        }
    }
}

void print_result(){
    printf("frequencies of identifiers:\n");

    if(head == NULL){
        return;
    }

    NODE *current = head;
    while(current != NULL){
        printf("%s\t%d\n", current->id, current->count);
        current = current->next;
    }
}