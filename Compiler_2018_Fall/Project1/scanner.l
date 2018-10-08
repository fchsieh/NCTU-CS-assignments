%{
#include <string.h>
#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<%s>\n", #t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define STRCAT(t)           { strncat(str, t, sizeof(str) - strlen(str) - 1); }
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char buf[MAX_LINE_LENGTH];
char str[MAX_LINE_LENGTH];

struct idList {
    char id[MAX_ID_LENGTH];
    int count;
    struct idList *next;
};

struct idList *head = NULL;

void print();
void print_freq();

%}

ws          [ \t]+
char        [a-zA-Z]
digit       [0-9]
nzero       [1-9]
integer     0|{nzero}{digit}*
float       ({integer})[.]({integer})
identifier  [_a-zA-Z][_a-zA-Z0-9]*
scNotation  0*({float}|{integer})[Ee][\+\-]?0*{integer}

%x CPP_COMMENT
%x C_COMMENT
%x STRING
%x PRAGMA
%x PRAGMA_AFTER
%x PRAGMA_SOURCE
%x PRAGMA_STATISTIC
%x PRAGMA_TOKEN

%%
    /* ===== Delim ===== */
"," 	{ tokenString("delim", ","); }
";" 	{ tokenString("delim", ";"); }
"("	    { tokenString("delim", "("); }
")" 	{ tokenString("delim", ")"); }
"["	    { tokenString("delim", "["); }
"]"	    { tokenString("delim", "]"); }
"{"	    { tokenString("delim", "{"); }
"}"	    { tokenString("delim", "}"); }

    /* ===== Operators ===== */
"+"   { token("+"); }
"-"   { token("-"); }
"*"   { token("*"); }
"/"   { token("/"); }
"%"   { token("%"); }
"="   { token("="); }
"<"   { token("<"); }
"<="  { token("<="); }
"!="  { token("!="); }
">="  { token(">="); }
">"   { token(">"); }
"=="  { token("=="); }
"&&"  { token("&&"); }
"||"  { token("||"); }
"!"   { token("!"); }

    /* ===== Keywords ===== */
"while"     { tokenString("KW", "while"); }
"do"	    { tokenString("KW", "do"); }
"if"	    { tokenString("KW", "if"); }
"else"	    { tokenString("KW", "else"); }
"true"	    { tokenString("KW", "true"); }
"false"     { tokenString("KW", "false"); }
"for"	    { tokenString("KW", "for"); }
"int"	    { tokenString("KW", "int"); }
"print"	    { tokenString("KW", "print"); }
"const"	    { tokenString("KW", "const"); }
"read"	    { tokenString("KW", "read"); }
"boolean"   { tokenString("KW", "boolean"); }
"bool"	    { tokenString("KW", "bool"); }
"void"	    { tokenString("KW", "void"); }
"float"	    { tokenString("KW", "float"); }
"double"    { tokenString("KW", "double"); }
"string"    { tokenString("KW", "string"); }
"continue"  { tokenString("KW", "continue"); }
"break"	    { tokenString("KW", "break"); }
"return"    { tokenString("KW", "return"); }

    /* ===== Identifiers =====*/
{identifier} {
    tokenString("id", yytext);
    if(Opt_Statistic){

    }
}

    /* ===== Numbers ===== */
{integer} { tokenString("integer", yytext); }
{float} { tokenString("float", yytext); }
{scNotation} { tokenString("scientific", yytext); }

    /* ===== Whitespace ===== */
{ws} { LIST; }

    /* ===== String ===== */
"\"" {  /* start of string */
    LIST;
    BEGIN STRING;
}
<STRING>[^"\\\n]+ { /* without \", \n, \\ */
    LIST;
    STRCAT(yytext);
}
<STRING>[\\][\"] {  /* double quote in string */
    LIST;
    STRCAT("\"");
}
<STRING>[\"] { /* end of string */
    //LIST;
    tokenString("string", str);
    str[0] = '\0';  /* clear string */
    BEGIN INITIAL;
}
<STRING>[\\][\\] { /* back slash */
    LIST;
    STRCAT("\\");
}
<STRING>[\\][n] {
    LIST;
    STRCAT("\\n");
}
<STRING>\n { /* string should not change newline without \ */
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

    /* ===== Comment ===== */
"//" {  /* C++ style comment */
    LIST;
    BEGIN CPP_COMMENT;
}
<CPP_COMMENT>[^\n]+ {
    LIST;
}
<CPP_COMMENT>\n {
    LIST;
    print();
    BEGIN INITIAL;
}

"/*" {  /* C style comment */
    LIST;
    BEGIN C_COMMENT;
}
<C_COMMENT>"*/" {
    LIST;
    BEGIN INITIAL;
}
<C_COMMENT>\n {
    LIST;
    print();
}
<C_COMMENT>. {
    LIST;
}

    /* ===== Pragma ===== */
^#pragma {  /* start of pragma */
    LIST;
    BEGIN PRAGMA;
}
<PRAGMA>source      { LIST; BEGIN PRAGMA_SOURCE; }
<PRAGMA>token       { LIST; BEGIN PRAGMA_TOKEN; }
<PRAGMA>statistic   { LIST; BEGIN PRAGMA_STATISTIC; }
<PRAGMA>{ws}        { LIST; }
    /* no other keyword allowed after pragma */
<PRAGMA>.           { fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);exit(1); }

<PRAGMA_SOURCE>{ws}on {
    LIST;
    Opt_Source = 1;
    BEGIN PRAGMA_AFTER;
}
<PRAGMA_SOURCE>\n {
    LIST;
    print();
    BEGIN INITIAL;
}


\n {
    LIST;
    print();
}

. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

%%

int main( int argc, char **argv )
{
    if (argc >= 2)
    {
        yyin = fopen( argv[1], "r" );
        if ( NULL == yyin ) {
            fprintf( stderr, "Cannot open: %s\n", argv[1] );
            exit(-1);
        }
    }
    else
    {
        yyin = stdin;
    }

    yylex();

    if (Opt_Statistic)
    {
        // Print frequency table
        print_freq();
    }

    exit(0);
}

void print(){
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

void print_freq(){
    printf("frequencies of identifiers:\n");
}