/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    do { \
        if ((result = fread((char*)buf, sizeof(char), max_size, fin)) < 0) \
            YY_FATAL_ERROR("read() in flex scanner failed"); \
    } while (0)

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

#define SHOW_ERROR(msg) \
    do { \
        cool_yylval.error_msg = (msg); \
        return ERROR; \
    } while (0)

#define EXTEND_STR(c) \
    do { \
        if (string_buf_ptr + 1 > &string_buf[MAX_STR_CONST - 1]) { \
            BEGIN(INVALID_STRING); \
            SHOW_ERROR("String muy pequena"); \
        } \
        *string_buf_ptr++ = (c); \
    } while (0)

int nivelcm;

%}

/*
 * Define names for regular expressions here.
 */



DARROW   =>
ASSIGN   <-      
LESSEQ   <=
DIGIT    [0-9]+
TYPEID   [A-Z][a-zA-Z0-9_]*
OBJECTID [a-z][a-zA-Z0-9_]*
WS       [\t\n\v\f\r ]

%x COMMENT STRING INVALID_STRING

%%

 /*
  *  The multiple-character operators.
  */

ASSIGN return (ASSIGN);
LESSEQ return (LE);


"("         return '(';
")"         return ')';
"{"         return '{';
"}"         return '}';
"*"         return '*';
"/"         return '/';
"+"         return '+';
"-"         return '-';
":"         return ':';
","         return ',';
";"         return ';';
"<"         return '<';
"="         return '=';
"."         return '.';
"@"         return '@';
"~"         return '~';	

DARROW return (DARROW);
 
 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */


[Cc][Ll][Aa][Ss][Ss]  {return CLASS;}
[Ii][Ff] {return IF;}
[Ff][Ii] {return FI;}
[Cc][Aa][Ss][Ee] {return CASE;}
[Ee][Ss][Aa][Cc] {return ESAC;}
[Tt][Hh][Ee][Nn] {return THEN;}
[Ee][Ll][Ss][Ee] {return ELSE;}
[Ww][Hh][Ii][Ll][Ee] {return WHILE;}
[Ll][Oo][Oo][Pp] {return LOOP;}
[Pp][Oo][Oo][Ll] {return POOL;}
[Ii][Nn] {return IN;}
[Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss] {return INHERITS;}
[Ii][Ss][Vv][Oo][Ii][Dd] {return ISVOID;}
[Ll][Ee][Tt] {return LET;}
[Nn][Ee][Ww] {return NEW;}
[Oo][Ff] {return OF;}
[Nn][Oo][Tt] {return NOT;}

t[Rr][Uu][Ee] {cool_yylval.boolean = true;
               return BOOL_CONST;
              }
f[Aa][Ll][Ss][Ee] {cool_yylval.boolean = false;
                   return BOOL_CONST;
                  }


TYPEID {cool_yylval.symbol = idtable.add_string(yytext);
        return TYPEID;
       }

OBJECTID {cool_yylval.symbol = idtable.add_string(yytext);
          return OBJECTID;
         }

DIGIT {cool_yylval.symbol = inttable.add_string(yytext);
       return INT_CONST;
      }

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */

\" { string_buf_ptr = string_buf;
     BEGIN(STRING);
   }
<STRING>{
    \" {*string_buf_ptr = '\0';
         BEGIN(INITIAL);
         cool_yylval.symbol = stringtable.add_string(string_buf);
         return STR_CONST;
       }
    \\?\0 {BEGIN(INVALID_STRING);
           SHOW_ERROR("Hay caracteres nulos");
           }
    \n {++curr_lineno;
        BEGIN(INITIAL);
        SHOW_ERROR("String inconcluso");
        }
    <<EOF>> {BEGIN(INITIAL); 
            SHOW_ERROR("EOF en String");
           }
    \\b EXTEND_STR('\b');
    \\f EXTEND_STR('\f');       
    \\t EXTEND_STR('\t'); 
    \\n EXTEND_STR('\n'); 
    \\\n { ++curr_lineno;
           EXTEND_STR('\n');
         }
    \\. EXTEND_STR(yytext[1]);

    [^\\\n\0\"]+ { if (string_buf_ptr + yyleng > &string_buf[MAX_STR_CONST - 1]) {
                    BEGIN(INVALID_STRING);
                    SHOW_ERROR("String muy largo");
                    }
                  strcpy(string_buf_ptr, yytext);
                  string_buf_ptr += yyleng;
                 }
}
<INVALID_STRING>{
    \"          BEGIN(INITIAL);
    \n          {
                    ++curr_lineno;
                    BEGIN(INITIAL);
                }
    \\\n        ++curr_lineno;
    \\.         ;
    [^\\\n\"]+  ;
}


/*
  *  Comments
  */

"--".* 

 /*
  *  Nested comments
  */

                 ;
"*)"                    SHOW_ERROR("No hay apertura de comentario para *)");
<INITIAL,COMMENT>"(*" {BEGIN(COMMENT);
                       ++nivelcm;
                      }
<COMMENT>{
    "*"+")"  {if (--nivelcm < 1)
              BEGIN(INITIAL);
             }
    <<EOF>>  {BEGIN(INITIAL);
             SHOW_ERROR("EOF en comentario");
             }
    \\\n ++nivelcm;
    \\. ;
    [^(*\\\n]* ;
    "("+[^(*\\\n]* ; 
    "*"+[^)*\\\n]* ;
    \n  ++nivelcm;
}



[\t\v\f\r ]+ ;
\n+  curr_lineno += yyleng;
. SHOW_ERROR(yytext);


%%
