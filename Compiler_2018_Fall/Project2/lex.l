%{
#include <string.h>
#include "y.tab.h"
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
#define MAX_STRING_LENGTH 257
#define LIST                strlcat(buf, yytext, MAX_LINE_LENGTH)
#define token(t)            {LIST; if (Opt_Token) printf("<%s>\n", #t);}
#define tokenInteger(t, i)  {LIST; if (Opt_Token) printf("<%s:%d>\n", t, i);}
#define tokenString(t, s)   {LIST; if (Opt_Token) printf("<%s:%s>\n", t, s);}
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic =1;
int linenum = 1;
int Last_State = 0;
char buf[MAX_LINE_LENGTH];
char strContent[MAX_STRING_LENGTH];
typedef struct ids{
	char id_name[MAX_ID_LENGTH];
	int freq;
	struct ids *next;
} id;
id *id_head;
void set(id **, char *);
id *new_id();
// revised strlcat so this can be run in workstation
size_t strlcat(char *restrict dst, const char *restrict src, size_t size);
%}

%x PRAGMA
%x SOURCE
%x TOKEN
%x STATISTIC
%x OTHER
%x STR
%x STR_ESCAPE
%x COMMENT

%%
<INITIAL>[\t ]+	{
	LIST;
}
^#pragma[\t ]+ {
	LIST;
	BEGIN PRAGMA;
}
<PRAGMA>"source"[\t ]+ {
		LIST;
		BEGIN SOURCE;
}
<PRAGMA>"token"[\t ]+ {
	LIST;
	BEGIN TOKEN;
}
<PRAGMA>"statistic"[\t ]+ {
	LIST;
	BEGIN STATISTIC;
}
<SOURCE>"on"[\t ]* {
	LIST;
	Opt_Source = 1;
	BEGIN OTHER;
}
<SOURCE>"off"[\t ]*	{
	LIST;
	Opt_Source = 0;
	BEGIN OTHER;
}
<SOURCE>[^\t ] {
	//printf("Error at source line %d: %s\n",linenum, yytext);
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
    exit(1);
}
<TOKEN>"on"[\t ]* {
	LIST;
	Opt_Token = 1;
	BEGIN OTHER;
}
<TOKEN>"off"[\t ]* {
	LIST;
	Opt_Token = 0;
	BEGIN OTHER;
}
<TOKEN>[^\t ] {
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
    exit(1);
}
<STATISTIC>"on"[\t ]*	{
		LIST;
		Opt_Statistic = 1;
		BEGIN OTHER;
	}
<STATISTIC>"off"[\t ]* {
	LIST;
	Opt_Statistic = 0;
	BEGIN OTHER;
}
<STATISTIC>[^\t ] {
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
    exit(1);
}
<OTHER>\/\/[^\n]* {
	LIST;		
}
<OTHER>"/*"	{
	LIST;
    Last_State = OTHER;
	BEGIN COMMENT;
}
<OTHER>\n {
	LIST;
  	if (Opt_Source)
  		printf("%d:%s\n", linenum, buf);
  	linenum++;
  	buf[0] = '\0';
	BEGIN INITIAL;
}
<OTHER>[^\n] {
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
    exit(1);
}
<PRAGMA>[^\t ] {
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
    exit(1);
}

","	{ tokenString("delim", ","); return COMMA; }	/* Delimiters */
";"	{ tokenString("delim", ";"); return SEMICOLON; }	/* Delimiters */
"("	{ tokenString("delim", "("); return L_PAREN; }	/* Brackets */
")"	{ tokenString("delim", ")"); return R_PAREN; }	/* Brackets */
"["	{ tokenString("delim", "["); return L_SQUARE_BRACKET; }	/* Brackets */
"]"	{ tokenString("delim", "]"); return R_SQUARE_BRACKET; }	/* Brackets */
"{"	{ tokenString("delim", "{"); return L_CURLY_BRACKET; }	/* Brackets */
"}"	{ tokenString("delim", "}"); return R_CURLY_BRACKET; }	/* Brackets */

"+"	{ token("+"); return PLUS; }	/* Math cal */
"-"	{ token("-"); return MINUS; }	/* Math cal */
"*"	{ token("*"); return MULTIPLY; }	/* Math cal */
"/"	{ token("/"); return DIVIDE; }	/* Math cal */
"%"	{ token("%"); return MOD; }	/* Math cal */

"="	{ token("="); return ASSIGN; }		/* Var assign */

"<"	{ token("<"); return LT; }	/* OP */
"<=" { token("<="); return LE; }	/* OP */
"!=" { token("!="); return NE;	}	/* OP */
">=" { token(">="); return GE; }	/* OP */
">"	{ token(">"); return GT; }	/* OP */
"==" { token("=="); return EQ; }	/* OP */
"&&" { token("&&"); return AND; }	/* OP */
"||" {token("||");return OR; }	/* OP */
"!"	{ token("!"); return NOT; }	/* OP */

int { tokenString("KW", yytext); return INT; }	/* Type */
boolean { tokenString("KW", yytext); return BOOL; }	/* Type */
bool { tokenString("KW", yytext); return BOOL; }	/* Type */
void { tokenString("KW", yytext); return VOID; }	/* Type */
float { tokenString("KW", yytext); return FLOAT; }	/* Type */
double { tokenString("KW", yytext); return DOUBLE; }	/* Type */
string { tokenString("KW", yytext); return STRING; }	/* Type */
const { tokenString("KW", yytext); return CONST; }	/* Type */

read { tokenString("KW", yytext); return READ; }	/* Keywords */
while { tokenString("KW", yytext); return WHILE; }	/* Keywords */
do { tokenString("KW", yytext); return DO; }	/* Keywords */
if { tokenString("KW", yytext); return IF; }	/* Keywords */
else { tokenString("KW", yytext); return ELSE; }	/* Keywords */
true { tokenString("KW", yytext); return TRUE; }	/* Keywords */
false { tokenString("KW", yytext); return FALSE; }	/* Keywords */
for { tokenString("KW", yytext); return FOR; }	/* Keywords */
print { tokenString("KW", yytext); return PRINT; }	/* Keywords */
continue { tokenString("KW", yytext); return CONTINUE; }	/* Keywords */
break { tokenString("KW", yytext); return BREAK; }	/* Keywords */
return { tokenString("KW", yytext); return RETURN; }	/* Keywords */

0|[1-9][0-9]* { tokenString("integer", yytext); return INTEGER_LITERAL; }	/* literals */
[0-9]+\.[0-9]+ { tokenString("float", yytext); return FLOAT_LITERAL; }	/* literals */
([0-9]+|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+) { tokenString("scientific", yytext); return SCIENTIFIC; }	/* literals */

"\"" { 
	LIST;
    strContent[0] = '\0';
	BEGIN STR;
}
<STR>"\\" {
	LIST;
	BEGIN STR_ESCAPE;
}
<STR_ESCAPE>\" {
	LIST;
	strlcat (strContent, "\"", MAX_STRING_LENGTH);
	BEGIN STR;
}
<STR_ESCAPE>n {
	LIST;
	strlcat( strContent, "\\n", MAX_STRING_LENGTH);
	BEGIN STR;
}
<STR_ESCAPE>t {
	LIST;
	strlcat( strContent, "\\t", MAX_STRING_LENGTH);
	BEGIN STR;
}
<STR_ESCAPE>\\ {
	LIST;
	strlcat( strContent, "\\", MAX_STRING_LENGTH);
	BEGIN STR;
}
<STR_ESCAPE>\n {
 	fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}
<STR_ESCAPE>[^\"nt\\] {
	LIST;
	strlcat( strContent, "\\", MAX_STRING_LENGTH);
	strlcat( strContent, yytext, MAX_STRING_LENGTH);
	BEGIN STR;
}
<STR>"\n" {
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
	exit(1);
}
<STR>\"	{
	tokenString("string", strContent);
	BEGIN INITIAL;
	return STRING_LITERAL;	/* literals */
}
<STR>[^\\\n\"] {
	LIST;
	strlcat( strContent, yytext, MAX_STRING_LENGTH);
}
"/*" {
	LIST;
    Last_State = INITIAL;
	BEGIN COMMENT;
}
<COMMENT>"\n" {
	LIST;
	if(Last_State == OTHER)Last_State = INITIAL;
	if (Opt_Source)
  	printf("%d:%s\n", linenum, buf);
	linenum++;
    buf[0] = '\0';
}			
<COMMENT>. {
	LIST;
}
<COMMENT>"*/" {
	LIST;
	BEGIN Last_State;
}
\/\/.* {
	LIST;
}

\n	{
  	LIST;
  	if (Opt_Source)
  	    printf("%d:%s\n", linenum, buf);
  	linenum++;
  	buf[0] = '\0';
}

[a-zA-Z_][a-zA-Z0-9_]{0,255} {
	if(Opt_Statistic){
		set(&id_head,yytext);
	}
	tokenString("id",yytext);
	return ID;
}
. {
	//printf("Error at wee line %d: %s\n",linenum, yytext);
    fprintf(stderr,"Error at line %d: %s\n",linenum, yytext);
	exit(1);
}
%%
size_t strlcat(char *restrict dst, const char *restrict src, size_t size) {
  size_t s_length = strlen(src);
  size_t remaining_size = size;

  while (*dst && remaining_size > 0) {
    dst++;
    remaining_size--;
  }
  // Length of destination is the lesser of `size` and 
  // offset of the null character, if any. 
  size_t d_length = size - remaining_size;

  if (d_length < remaining_size) {
    // We know there is at least room to write the \0

    size_t copy_length = remaining_size - 1;
    if (s_length < copy_length) {
      copy_length = s_length;
    }
    memcpy(dst, src, copy_length);
    dst[copy_length] = '\0';
  }
  return d_length + s_length;
}


/*
int main( int argc, char **argv )
{

	if( argc == 1 )
	{
		yyin = stdin;
	}
	else if( argc == 2 )
	{
		FILE *fp = fopen( argv[1], "r" );
		if( fp == NULL ) {
				fprintf( stderr, "Open file error\n" );
				exit(-1);
		}
		yyin = fp;
	}
	else
	{
	  	fprintf( stderr, "Usage: ./scanner [filename]\n" );
   		exit(0);
 	} 


  	id_head = NULL;
	yylex();
	
  	if(Opt_Statistic){
		printf("frequencies of identifiers:\n");
		id *offset = id_head;
		id *tobefree = id_head;
  		while(offset){
				printf("%s\t%d\n",offset->id_name, offset->freq);
    			offset = offset->next;
  		}
		//free linked list
		while(tobefree){
			offset = tobefree;
			tobefree = tobefree->next;
			free(offset);
		}
	}

  	exit(0);
}
*/
void set(id **head, char *yytext)
{
	id *offset = *head;
	id *prev = NULL;	
	if(offset == NULL){
		*head = new_id();
		strncpy((*head)->id_name, yytext,strlen(yytext));
		(*head)->freq = 1;
	}else{
		while(offset){
			if(!strncmp(offset->id_name, yytext, (strlen(yytext)>=strlen(offset->id_name)?strlen(yytext):strlen(offset->id_name)))){
				offset->freq+=1;
				break;
			}
			prev = offset;
			offset = offset -> next;
		}
		if(!offset){
			prev->next = new_id();
			strncpy(prev->next->id_name, yytext, strlen(yytext));
			prev->next->freq+=1;
		}
	}
}
id *new_id()
{
	id *temp = (id *)malloc(sizeof(id));
	temp->next = NULL;
	memset(temp->id_name, 0, 256);
	temp->freq = 0;
	return temp;
}			
