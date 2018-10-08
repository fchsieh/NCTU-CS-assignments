%{
#include <string.h>
#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<%s>\n", #t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char stringBuf[MAX_LINE_LENGTH];
char buf[MAX_LINE_LENGTH];
int id_count = 0;
int freq[10000]={0}, i = 0;
char id_name[10000][MAX_ID_LENGTH];
int check = 0;

%}

letter  [A-Za-z]
digit [0-9]
ID ({letter})({digit}|{letter})*
integer 0|[1-9][0-9]*
E [Ee][+-]?{digit}+
float {digit}+"."{digit}+
scientificNotation {digit}+"."{digit}+({E})?
%x C_COMMENT
%x CPP_COMMENT
%x STRING_HANDLE
%x PRAGMA
%x PRAGMA_SOURCE
%x PRAGMA_STAT
%x PRAGMA_TOKEN
%%

"("	{tokenString("delim", "("); }
")" 	{tokenString("delim", ")"); }
"," 	{tokenString("delim", ","); }
";" 	{tokenString("delim", ";"); }
"["	{tokenString("delim", "["); }
"]"	{tokenString("delim", "]"); }
"{"	{tokenString("delim", "{"); }
"}"	{tokenString("delim", "}"); }

"+"	{token("+");}
"-"	{token("-");}
"*"	{token("*");}
"/"	{token("/");}
"%"	{token("%");}
"="	{token("=");}
"<"	{token("<");}
"<="	{token("<=");}
"!="	{token("!=");}
">="	{token(">=");}
">"	{token(">");}
"=="	{token("==");}
"&&"	{token("&&");}
"||"	{token("||");}
"!"	{token("!");}

"while"	{tokenString("KW", "while");}
"do"	{tokenString("KW", "do");}
"if"	{tokenString("KW", "if");}
"else"	{tokenString("KW", "else");}
"true"	{tokenString("KW", "true");}
"false" {tokenString("KW", "false");} 
"for"	{tokenString("KW", "for");}
"int"	{tokenString("KW", "int");}
"print"	{tokenString("KW", "print");}
"const"	{tokenString("KW", "const");}
"read"	{tokenString("KW", "read");}
"boolean" {tokenString("KW", "boolean");}
"bool"	{tokenString("KW", "bool");}
"void"	{tokenString("KW", "void");}
"float"	{tokenString("KW", "float");}
"double" {tokenString("KW", "double");}
"string" {tokenString("KW", "string");}
"continue" {tokenString("KW", "continue");}
"break"	{tokenString("KW", "break");}
"return" {tokenString("KW", "return");}

[ \t]+ {LIST;}
{ID}	{
	tokenString("id", yytext);
	//printf("%s", yytext);
	if(Opt_Statistic){
		for(i=1; i<=id_count; i++){
			if(strcmp(id_name[i], yytext) == 0){
				freq[i]++;
				check = 1;
				break;
			}	    
		}
		if(check == 0){
			id_count++;
			freq[id_count]++;
			strcpy(id_name[id_count], yytext);
		}
		check = 0;
	}
}

{float} {tokenString("float", yytext);}
{scientificNotation} {tokenString("scientific", yytext);}
{integer} {tokenString("integer", yytext);}
[\"]			{
	LIST;
	BEGIN(STRING_HANDLE);
}
<STRING_HANDLE>[\"]	{
	//LIST;
	tokenString("string", stringBuf);
	memset(stringBuf, 0, sizeof stringBuf);
	BEGIN(INITIAL);
}
<STRING_HANDLE>[\\][\"]  {
	LIST;
	strncat(stringBuf, "\"", sizeof(stringBuf) - strlen(stringBuf) - 1);
	
}
<STRING_HANDLE>[\\]{2}  {
	LIST;
	strncat(stringBuf, "\\", sizeof(stringBuf) - strlen(stringBuf) - 1);
	
}

<STRING_HANDLE>\n { // string can't contain newline
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}
<STRING_HANDLE>[^"\n\\]+	{
	LIST;
	strncat(stringBuf, yytext, sizeof(stringBuf) - strlen(stringBuf) - 1);
}


"/*"		{
		LIST; 
		BEGIN(C_COMMENT); 
}
<C_COMMENT>"*/" {
		LIST; 
		BEGIN(INITIAL); 
}
<C_COMMENT>\n  	{ 
	    	LIST;
	    	if (Opt_Source)
			printf("%d:%s\n", linenum, buf);
	    	++linenum;
	    	buf[0] = '\0';
}
<C_COMMENT>.    {LIST;}

"//"		{
		LIST;
		BEGIN(CPP_COMMENT);
}
<CPP_COMMENT>\n  { 
	    	LIST;
	    	if (Opt_Source)
			printf("%d:%s\n", linenum, buf);
	    	++linenum;
	    	buf[0] = '\0';
		BEGIN(INITIAL);
}
<CPP_COMMENT>.    {LIST;}
^"#pragma"	{
		LIST;
		BEGIN(PRAGMA);
}

<PRAGMA>[ ]	{LIST;}
<PRAGMA>"source" {
		LIST;
		BEGIN(PRAGMA_SOURCE);
}
<PRAGMA>"token" {
		LIST;
		BEGIN(PRAGMA_TOKEN);
}
<PRAGMA>"statistic" {
		LIST;
		BEGIN(PRAGMA_STAT);
}

<PRAGMA_SOURCE>[ ] {LIST;}
<PRAGMA_SOURCE>"on" {
	LIST;
	Opt_Source = 1;
	
}
<PRAGMA_SOURCE>"off" {
	LIST;
	Opt_Source = 0;
}
<PRAGMA_SOURCE>"/*"  {
	LIST; 
	BEGIN(C_COMMENT); 
}
<PRAGMA_SOURCE>"//"  {
	LIST;
	BEGIN(CPP_COMMENT);
}
<PRAGMA_SOURCE>\n  { 
    	LIST;
    	if (Opt_Source)
		printf("%d:%s\n", linenum, buf);
    	++linenum;
    	buf[0] = '\0';
	BEGIN(INITIAL);
}
<PRAGMA_SOURCE>. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}


<PRAGMA_TOKEN>[ ] {LIST;}
<PRAGMA_TOKEN>"on" {
	LIST;
	Opt_Token = 1;
}
<PRAGMA_TOKEN>"off" {
	LIST;
	Opt_Token = 0;
}
<PRAGMA_TOKEN>"/*"  {
	LIST; 
	BEGIN(C_COMMENT); 
}
<PRAGMA_TOKEN>"//"  {
	LIST;
	BEGIN(CPP_COMMENT);
}
<PRAGMA_TOKEN>\n  { 
    	LIST;
    	if (Opt_Source)
		printf("%d:%s\n", linenum, buf);
    	++linenum;
    	buf[0] = '\0';
	BEGIN(INITIAL);
}
<PRAGMA_TOKEN>. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}


<PRAGMA_STAT>[ ] {LIST;}
<PRAGMA_STAT>"on" {
	LIST;
	Opt_Statistic = 1;
}
<PRAGMA_STAT>"off" {
	LIST;
	Opt_Statistic = 0;
}
<PRAGMA_STAT>"/*"  {
	LIST; 
	BEGIN(C_COMMENT); 
}
<PRAGMA_STAT>"//"  {
	LIST;
	BEGIN(CPP_COMMENT);
}
<PRAGMA_STAT>\n  { 
    	LIST;
    	if (Opt_Source)
		printf("%d:%s\n", linenum, buf);
    	++linenum;
    	buf[0] = '\0';
	BEGIN(INITIAL);
}
<PRAGMA_STAT>. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}


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
	for(i=1;i<=id_count;i++){
		printf("%s	%d\n", id_name[i], freq[i]);	
	}
    }

    exit(0);
}
