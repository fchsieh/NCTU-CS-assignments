%{
#include <string.h>
#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<\"%s\">\n", #t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char buf[MAX_LINE_LENGTH];
%}

digit       [0-9]
nzero       [1-9]
character   [a-zA-Z]
delimiter   [\,\;\(\)\[\]\{\}]
mathop      [\+\-\*\/\%]
relational  []
logical     []
identifier  [_a-zA-Z][_a-zA-Z0-9]

%Start IN_COMMENT STRING

%%
    /* ===== Comment ===== */
<INITIAL>"//".*\n { // Cpp-style comment
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

<INITIAL>"/*" { // start of C-style comment
    LIST;
    BEGIN IN_COMMENT;
}

<IN_COMMENT>"*/" { // end of C-style comment
    LIST;
    BEGIN INITIAL;
}
<IN_COMMENT>. {
    LIST;
}
    /* END OF COMMENT SECTION */

    /* ===== String ===== */
<INITIAL>"\"" { // start of string
    LIST;
    BEGIN STRING;
}
<STRING>"\"" {  // end of string
    tokenString("string", buf);
    buf[0] = '\0';
    BEGIN INITIAL;
}
    /* END OF STRING SECTION */

    /* ===== Identifier ===== */


\n {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
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
        printf("frequencies of identifiers:\n");
        
    }

    exit(0);
}
