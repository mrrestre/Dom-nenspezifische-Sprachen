/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/
#line 1 "grammar.y"
 

#include "grammar.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <iostream>
#include "cJSON.h"


using namespace std;
int get_token_id (char*);
const char *getValue (cJSON *token);
const char *getLine (cJSON *token);
cJSON *unary (char *fname, cJSON *a); 
cJSON *binary (char *fname, cJSON *a, cJSON *b); 
cJSON *ternary (char *fname, cJSON *a, cJSON *b, cJSON *c); 
char *linenumber;
char *curtoken;
char *curtype;
#line 50 "grammar.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 48
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE cJSON *
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL
#define ParseARG_PDECL
#define ParseARG_FETCH
#define ParseARG_STORE
#define YYNSTATE             68
#define YYNRULE              46
#define YY_MAX_SHIFT         67
#define YY_MIN_SHIFTREDUCE   93
#define YY_MAX_SHIFTREDUCE   138
#define YY_MIN_REDUCE        139
#define YY_MAX_REDUCE        184
#define YY_ERROR_ACTION      185
#define YY_ACCEPT_ACTION     186
#define YY_NO_ACTION         187
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE

**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (454)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    24,   20,   14,   27,   25,   23,   18,   17,   21,   15,
 /*    10 */   186,   30,   62,   56,   60,   11,   22,   12,   25,   23,
 /*    20 */    18,   17,   21,   15,  106,  139,   28,   56,    1,  107,
 /*    30 */   111,  112,  113,  114,  115,  116,  117,  118,   24,   20,
 /*    40 */    41,   59,   15,   34,   95,    5,   56,   57,   36,    3,
 /*    50 */    63,   37,   40,   11,   22,   12,   31,   67,   26,   32,
 /*    60 */     9,   33,  106,   35,   28,   38,    1,   64,  111,  112,
 /*    70 */   113,  114,  115,  116,  117,  118,   24,   20,  122,  124,
 /*    80 */   125,   42,   61,   53,  108,   58,   47,   54,   55,   48,
 /*    90 */     6,   11,   22,   12,   43,   52,   66,    2,   51,   56,
 /*   100 */   106,   44,   28,   50,    1,   45,  111,  112,  113,  114,
 /*   110 */   115,  116,  117,  118,   24,   20,   49,   39,   46,    7,
 /*   120 */     8,  185,  123,  141,  141,  141,  141,  141,  141,   11,
 /*   130 */    22,   12,  141,  141,  141,  141,   10,  141,  106,  141,
 /*   140 */    28,  141,    1,  141,  111,  112,  113,  114,  115,  116,
 /*   150 */   117,  118,   24,   20,  141,  141,  141,  141,  141,  141,
 /*   160 */   141,  141,  141,  141,  141,  141,  141,   11,   22,   12,
 /*   170 */   141,  141,  141,    4,  141,  141,  106,  141,   28,  141,
 /*   180 */     1,  141,  111,  112,  113,  114,  115,  116,  117,  118,
 /*   190 */    24,   20,  141,  141,  141,  141,  141,  141,  141,  141,
 /*   200 */   141,  141,  141,  141,  141,   11,   22,   12,  141,  141,
 /*   210 */   141,  141,  141,  141,  106,  141,   28,  141,    1,  141,
 /*   220 */   111,  112,  113,  114,  115,  116,  117,  118,   13,  141,
 /*   230 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   240 */    15,  141,  141,  141,   56,  141,  104,  141,   13,  141,
 /*   250 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   260 */    15,  141,  141,  141,   56,  141,  103,  141,   13,  141,
 /*   270 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   280 */    15,  141,  141,  141,   56,  141,  102,  141,   13,  141,
 /*   290 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   300 */    15,  141,  141,  141,   56,  141,  101,  141,   13,  141,
 /*   310 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   320 */    15,  141,  141,  141,   56,  141,  100,  141,   13,  141,
 /*   330 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   340 */    15,  141,  141,  141,   56,  141,   99,  141,   13,  141,
 /*   350 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   360 */    15,  141,  141,  141,   56,  141,   98,  141,   13,  141,
 /*   370 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   380 */    15,  141,  141,  141,   56,  141,   97,  141,   13,  141,
 /*   390 */    16,   19,   29,   14,   27,   25,   23,   18,   17,   21,
 /*   400 */    15,  141,  141,  141,   56,  141,  141,  141,  141,  141,
 /*   410 */   141,  141,  141,  105,  141,  141,  141,   13,  141,   16,
 /*   420 */    19,   29,   14,   27,   25,   23,   18,   17,   21,   15,
 /*   430 */   141,  141,  141,   56,  141,   96,  141,   13,  141,   16,
 /*   440 */    19,   29,   14,   27,   25,   23,   18,   17,   21,   15,
 /*   450 */   141,  141,  141,   56,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    8,    9,   10,   11,   12,   13,   14,   15,
 /*    10 */    42,   43,   23,   19,   25,   16,   17,   18,   10,   11,
 /*    20 */    12,   13,   14,   15,   25,    0,   27,   19,   29,   30,
 /*    30 */    31,   32,   33,   34,   35,   36,   37,   38,    1,    2,
 /*    40 */    45,   46,   15,   45,   44,   20,   19,   22,   45,   24,
 /*    50 */    25,   45,   45,   16,   17,   18,   45,   18,    4,   45,
 /*    60 */    23,   45,   25,   45,   27,   45,   29,   45,   31,   32,
 /*    70 */    33,   34,   35,   36,   37,   38,    1,    2,   39,   40,
 /*    80 */    45,   45,   25,   45,   30,   45,   45,   45,   45,   45,
 /*    90 */    26,   16,   17,   18,   45,   45,   45,   22,   45,   19,
 /*   100 */    25,   45,   27,   45,   29,   45,   31,   32,   33,   34,
 /*   110 */    35,   36,   37,   38,    1,    2,   45,   45,   45,   26,
 /*   120 */    26,   19,   39,   47,   47,   47,   47,   47,   47,   16,
 /*   130 */    17,   18,   47,   47,   47,   47,   23,   47,   25,   47,
 /*   140 */    27,   47,   29,   47,   31,   32,   33,   34,   35,   36,
 /*   150 */    37,   38,    1,    2,   47,   47,   47,   47,   47,   47,
 /*   160 */    47,   47,   47,   47,   47,   47,   47,   16,   17,   18,
 /*   170 */    47,   47,   47,   22,   47,   47,   25,   47,   27,   47,
 /*   180 */    29,   47,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   190 */     1,    2,   47,   47,   47,   47,   47,   47,   47,   47,
 /*   200 */    47,   47,   47,   47,   47,   16,   17,   18,   47,   47,
 /*   210 */    47,   47,   47,   47,   25,   47,   27,   47,   29,   47,
 /*   220 */    31,   32,   33,   34,   35,   36,   37,   38,    3,   47,
 /*   230 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   240 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   250 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   260 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   270 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   280 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   290 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   300 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   310 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   320 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   330 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   340 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   350 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   360 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   370 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   380 */    15,   47,   47,   47,   19,   47,   21,   47,    3,   47,
 /*   390 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   400 */    15,   47,   47,   47,   19,   47,   47,   47,   47,   47,
 /*   410 */    47,   47,   47,   28,   47,   47,   47,    3,   47,    5,
 /*   420 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   430 */    47,   47,   47,   19,   47,   21,   47,    3,   47,    5,
 /*   440 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   450 */    47,   47,   47,   19,
};
#define YY_SHIFT_USE_DFLT (-12)
#define YY_SHIFT_COUNT (67)
#define YY_SHIFT_MIN   (-11)
#define YY_SHIFT_MAX   (434)
static const short yy_shift_ofst[] = {
 /*     0 */   -12,   -1,   37,   75,  113,  151,  189,  189,  189,  189,
 /*    10 */   189,  189,  189,  189,  189,  189,  189,  189,  189,  189,
 /*    20 */   189,  189,  189,  189,  189,  189,  189,  189,  189,  189,
 /*    30 */    25,  225,  245,  265,  285,  305,  325,  345,  365,  385,
 /*    40 */   414,  434,  434,  434,  434,  434,   -6,   -6,   -6,    8,
 /*    50 */     8,    8,    8,    8,    8,    8,   39,  -11,   27,   54,
 /*    60 */    64,   93,   57,   94,   80,  102,   80,   83,
};
#define YY_REDUCE_USE_DFLT (-33)
#define YY_REDUCE_COUNT (30)
#define YY_REDUCE_MIN   (-32)
#define YY_REDUCE_MAX   (73)
static const signed char yy_reduce_ofst[] = {
 /*     0 */   -32,   -5,   -2,    3,    6,    7,   11,   14,   16,   18,
 /*    10 */    20,   22,   35,   36,   38,   40,   41,   42,   43,   44,
 /*    20 */    49,   50,   51,   53,   56,   58,   60,   71,   72,   73,
 /*    30 */     0,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   140,  185,  185,  185,  185,  185,  185,  185,  185,  185,
 /*    10 */   185,  185,  185,  185,  185,  185,  185,  185,  185,  185,
 /*    20 */   185,  185,  185,  185,  185,  185,  185,  185,  185,  185,
 /*    30 */   185,  185,  185,  185,  185,  185,  185,  185,  185,  185,
 /*    40 */   185,  155,  184,  167,  165,  156,  173,  181,  178,  174,
 /*    50 */   175,  176,  177,  183,  180,  179,  185,  185,  182,  185,
 /*    60 */   185,  185,  185,  185,  172,  171,  166,  185,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "COUNT",         "FIRST",         "WHERE",       
  "COMMA",         "PLUS",          "MINUS",         "AMPERSAND",   
  "TIMES",         "DIVIDE",        "GT",            "GTEQ",        
  "LT",            "LTEQ",          "NEQ",           "POWER",       
  "SIN",           "COS",           "NOT",           "IS",          
  "WRITE",         "SEMICOLON",     "TIME",          "OF",          
  "TRACE",         "IDENTIFIER",    "ASSIGN",        "LPAR",        
  "RPAR",          "LSPAR",         "RSPAR",         "BOOLTOKEN",   
  "IT",            "NOW",           "NUMTOKEN",      "NULLTOKEN",   
  "STRTOKEN",      "TIMETOKEN",     "THEY",          "LIST",        
  "NUMBER",        "error",         "code",          "statementblock",
  "statement",     "ex",            "exlist",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "code ::= statementblock",
 /*   1 */ "statementblock ::=",
 /*   2 */ "statementblock ::= statementblock statement",
 /*   3 */ "statement ::= WRITE ex SEMICOLON",
 /*   4 */ "statement ::= WRITE TIME OF ex SEMICOLON",
 /*   5 */ "statement ::= WRITE TIME ex SEMICOLON",
 /*   6 */ "statement ::= TRACE ex SEMICOLON",
 /*   7 */ "statement ::= TRACE TIME OF ex SEMICOLON",
 /*   8 */ "statement ::= TRACE TIME ex SEMICOLON",
 /*   9 */ "statement ::= IDENTIFIER ASSIGN ex SEMICOLON",
 /*  10 */ "statement ::= TIME OF IDENTIFIER ASSIGN ex SEMICOLON",
 /*  11 */ "statement ::= TIME IDENTIFIER ASSIGN ex SEMICOLON",
 /*  12 */ "ex ::= LPAR ex RPAR",
 /*  13 */ "ex ::= IDENTIFIER",
 /*  14 */ "ex ::= LSPAR RSPAR",
 /*  15 */ "ex ::= LSPAR exlist RSPAR",
 /*  16 */ "exlist ::= ex",
 /*  17 */ "exlist ::= exlist COMMA ex",
 /*  18 */ "ex ::= BOOLTOKEN",
 /*  19 */ "ex ::= IT",
 /*  20 */ "ex ::= NOW",
 /*  21 */ "ex ::= NUMTOKEN",
 /*  22 */ "ex ::= NULLTOKEN",
 /*  23 */ "ex ::= STRTOKEN",
 /*  24 */ "ex ::= TIMETOKEN",
 /*  25 */ "ex ::= THEY",
 /*  26 */ "ex ::= COUNT ex",
 /*  27 */ "ex ::= COS ex",
 /*  28 */ "ex ::= FIRST ex",
 /*  29 */ "ex ::= ex IS LIST",
 /*  30 */ "ex ::= ex IS NOT LIST",
 /*  31 */ "ex ::= ex IS NUMBER",
 /*  32 */ "ex ::= NOT ex",
 /*  33 */ "ex ::= SIN ex",
 /*  34 */ "ex ::= ex AMPERSAND ex",
 /*  35 */ "ex ::= ex DIVIDE ex",
 /*  36 */ "ex ::= ex GT ex",
 /*  37 */ "ex ::= ex GTEQ ex",
 /*  38 */ "ex ::= ex NEQ ex",
 /*  39 */ "ex ::= ex MINUS ex",
 /*  40 */ "ex ::= ex LT ex",
 /*  41 */ "ex ::= ex LTEQ ex",
 /*  42 */ "ex ::= ex PLUS ex",
 /*  43 */ "ex ::= ex POWER ex",
 /*  44 */ "ex ::= ex TIMES ex",
 /*  45 */ "ex ::= ex WHERE ex",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ){
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yytos = NULL;
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    if( yyGrowStack(pParser) ){
      pParser->yystack = &pParser->yystk0;
      pParser->yystksz = 1;
    }
#endif
#ifndef YYNOERRORRECOVERY
    pParser->yyerrcnt = -1;
#endif
    pParser->yytos = pParser->yystack;
    pParser->yystack[0].stateno = 0;
    pParser->yystack[0].major = 0;
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
#ifndef YYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yytos->stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  do{
    i = yy_shift_ofst[stateno];
    if( i==YY_SHIFT_USE_DFLT ) return yy_default[stateno];
    assert( iLookAhead!=YYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
      if( iLookAhead>0 ){
#ifdef YYFALLBACK
        YYCODETYPE iFallback;            /* Fallback token */
        if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
               && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
          }
#endif
          assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
          iLookAhead = iFallback;
          continue;
        }
#endif
#ifdef YYWILDCARD
        {
          int j = i - iLookAhead + YYWILDCARD;
          if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
            j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
            j<YY_ACTTAB_COUNT &&
#endif
            yy_lookahead[j]==YYWILDCARD
          ){
#ifndef NDEBUG
            if( yyTraceFILE ){
              fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
                 yyTracePrompt, yyTokenName[iLookAhead],
                 yyTokenName[YYWILDCARD]);
            }
#endif /* NDEBUG */
            return yy_action[j];
          }
        }
#endif /* YYWILDCARD */
      }
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   ParseARG_FETCH;
   yypParser->yytos--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift '%s', go to state %d\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%sShift '%s'\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major]);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>=&yypParser->yystack[YYSTACKDEPTH] ){
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 42, 1 },
  { 43, 0 },
  { 43, 2 },
  { 44, 3 },
  { 44, 5 },
  { 44, 4 },
  { 44, 3 },
  { 44, 5 },
  { 44, 4 },
  { 44, 4 },
  { 44, 6 },
  { 44, 5 },
  { 45, 3 },
  { 45, 1 },
  { 45, 2 },
  { 45, 3 },
  { 46, 1 },
  { 46, 3 },
  { 45, 1 },
  { 45, 1 },
  { 45, 1 },
  { 45, 1 },
  { 45, 1 },
  { 45, 1 },
  { 45, 1 },
  { 45, 1 },
  { 45, 2 },
  { 45, 2 },
  { 45, 2 },
  { 45, 3 },
  { 45, 4 },
  { 45, 3 },
  { 45, 2 },
  { 45, 2 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno        /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[-yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=&yypParser->yystack[YYSTACKDEPTH-1] ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* code ::= statementblock */
#line 220 "grammar.y"
{
	printf (cJSON_Print(yymsp[0].minor.yy0)); 
}
#line 963 "grammar.c"
        break;
      case 1: /* statementblock ::= */
#line 230 "grammar.y"
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STATEMENTBLOCK");
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToObject(res, "statements", arg); 
	yymsp[1].minor.yy0 = res;
}
#line 974 "grammar.c"
        break;
      case 2: /* statementblock ::= statementblock statement */
#line 239 "grammar.y"
{
	cJSON_AddItemToArray(cJSON_GetObjectItem ( yymsp[-1].minor.yy0, "statements"), yymsp[0].minor.yy0);
	yylhsminor.yy0 = yymsp[-1].minor.yy0;
}
#line 982 "grammar.c"
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 3: /* statement ::= WRITE ex SEMICOLON */
#line 249 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-2].minor.yy0 = res; 
}
#line 993 "grammar.c"
        break;
      case 4: /* statement ::= WRITE TIME OF ex SEMICOLON */
#line 261 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE_TIME"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-4].minor.yy0 = res; 
}
#line 1003 "grammar.c"
        break;
      case 5: /* statement ::= WRITE TIME ex SEMICOLON */
#line 269 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE_TIME"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-3].minor.yy0 = res; 
}
#line 1013 "grammar.c"
        break;
      case 6: /* statement ::= TRACE ex SEMICOLON */
#line 281 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE"); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-2].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1024 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 7: /* statement ::= TRACE TIME OF ex SEMICOLON */
#line 294 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE_TIME"); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-4].minor.yy0));
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1036 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 8: /* statement ::= TRACE TIME ex SEMICOLON */
#line 303 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE_TIME");
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-3].minor.yy0));
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1048 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 9: /* statement ::= IDENTIFIER ASSIGN ex SEMICOLON */
#line 316 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1060 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 10: /* statement ::= TIME OF IDENTIFIER ASSIGN ex SEMICOLON */
#line 329 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN_TIME");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-5].minor.yy0 = res; 
}
#line 1072 "grammar.c"
        break;
      case 11: /* statement ::= TIME IDENTIFIER ASSIGN ex SEMICOLON */
#line 338 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN_TIME");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-4].minor.yy0 = res; 
}
#line 1083 "grammar.c"
        break;
      case 12: /* ex ::= LPAR ex RPAR */
#line 352 "grammar.y"
{ 
	yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0; 
}
#line 1090 "grammar.c"
        break;
      case 13: /* ex ::= IDENTIFIER */
#line 358 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IDENTIFIER"); 
	cJSON_AddStringToObject(res, "name", getValue(yymsp[0].minor.yy0)); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1101 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 14: /* ex ::= LSPAR RSPAR */
#line 372 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "EMPTYLIST");
	yymsp[-1].minor.yy0 = res; 
}
#line 1111 "grammar.c"
        break;
      case 15: /* ex ::= LSPAR exlist RSPAR */
#line 380 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "LIST");
	cJSON_AddItemToObject(res, "elements", yymsp[-1].minor.yy0); 
	yymsp[-2].minor.yy0 = res; 
}
#line 1121 "grammar.c"
        break;
      case 16: /* exlist ::= ex */
#line 389 "grammar.y"
{
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, yymsp[0].minor.yy0);
	yylhsminor.yy0 = arg;
}
#line 1130 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 17: /* exlist ::= exlist COMMA ex */
#line 397 "grammar.y"
{
	cJSON_AddItemToArray(yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
	yylhsminor.yy0 = yymsp[-2].minor.yy0;
}
#line 1139 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 18: /* ex ::= BOOLTOKEN */
#line 408 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "BOOLTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1150 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 19: /* ex ::= IT */
      case 25: /* ex ::= THEY */ yytestcase(yyruleno==25);
#line 416 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IT"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1161 "grammar.c"
        break;
      case 20: /* ex ::= NOW */
#line 423 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NOW"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1170 "grammar.c"
        break;
      case 21: /* ex ::= NUMTOKEN */
#line 430 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "NUMTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1180 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 22: /* ex ::= NULLTOKEN */
#line 438 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NULLTOKEN"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1190 "grammar.c"
        break;
      case 23: /* ex ::= STRTOKEN */
#line 445 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STRTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1200 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 24: /* ex ::= TIMETOKEN */
#line 453 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "TIMETOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1211 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 26: /* ex ::= COUNT ex */
#line 476 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("COUNT", yymsp[0].minor.yy0); }
#line 1217 "grammar.c"
        break;
      case 27: /* ex ::= COS ex */
#line 479 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("COS", yymsp[0].minor.yy0); }
#line 1222 "grammar.c"
        break;
      case 28: /* ex ::= FIRST ex */
#line 482 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("FIRST", yymsp[0].minor.yy0); }
#line 1227 "grammar.c"
        break;
      case 29: /* ex ::= ex IS LIST */
#line 485 "grammar.y"
{yylhsminor.yy0 = unary ("IS_LIST", yymsp[-2].minor.yy0); }
#line 1232 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 30: /* ex ::= ex IS NOT LIST */
#line 488 "grammar.y"
{yylhsminor.yy0 = unary ("IS_NOT_LIST", yymsp[-3].minor.yy0); }
#line 1238 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 31: /* ex ::= ex IS NUMBER */
#line 491 "grammar.y"
{yylhsminor.yy0 = unary ("IS_NUMBER", yymsp[-2].minor.yy0); }
#line 1244 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 32: /* ex ::= NOT ex */
#line 494 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("NOT", yymsp[0].minor.yy0); }
#line 1250 "grammar.c"
        break;
      case 33: /* ex ::= SIN ex */
#line 497 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("SIN", yymsp[0].minor.yy0); }
#line 1255 "grammar.c"
        break;
      case 34: /* ex ::= ex AMPERSAND ex */
#line 504 "grammar.y"
{yylhsminor.yy0 = binary ("AMPERSAND", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1260 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 35: /* ex ::= ex DIVIDE ex */
#line 507 "grammar.y"
{yylhsminor.yy0 = binary ("DIVIDE", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1266 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 36: /* ex ::= ex GT ex */
#line 510 "grammar.y"
{yylhsminor.yy0 = binary ("GT", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1272 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 37: /* ex ::= ex GTEQ ex */
#line 513 "grammar.y"
{yylhsminor.yy0 = binary ("GTEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1278 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 38: /* ex ::= ex NEQ ex */
#line 516 "grammar.y"
{yylhsminor.yy0 = binary ("NEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1284 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 39: /* ex ::= ex MINUS ex */
#line 519 "grammar.y"
{yylhsminor.yy0 = binary ("MINUS", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1290 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 40: /* ex ::= ex LT ex */
#line 522 "grammar.y"
{yylhsminor.yy0 = binary ("LT", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1296 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 41: /* ex ::= ex LTEQ ex */
#line 525 "grammar.y"
{yylhsminor.yy0 = binary ("LTEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1302 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 42: /* ex ::= ex PLUS ex */
#line 528 "grammar.y"
{yylhsminor.yy0 = binary ("PLUS", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1308 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 43: /* ex ::= ex POWER ex */
#line 531 "grammar.y"
{yylhsminor.yy0 = binary ("POWER", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1314 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 44: /* ex ::= ex TIMES ex */
#line 534 "grammar.y"
{yylhsminor.yy0 = binary ("TIMES", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1320 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 45: /* ex ::= ex WHERE ex */
#line 537 "grammar.y"
{yylhsminor.yy0 = binary ("WHERE", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1326 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact <= YY_MAX_SHIFTREDUCE ){
    if( yyact>YY_MAX_SHIFT ){
      yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
    }
    yymsp -= yysize-1;
    yypParser->yytos = yymsp;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }else{
    assert( yyact == YY_ACCEPT_ACTION );
    yypParser->yytos -= yysize;
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
#line 190 "grammar.y"

  printf ("{\"error\" : true, \"message\": \"Syntax Error: Compiler reports unexpected token \\\"%s\\\" of type \\\"%s\\\" in line %s\"}\n", curtoken, curtype, linenumber);
  exit(0);
#line 1390 "grammar.c"
/************ End %syntax_error code ******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  yypParser = (yyParser*)yyp;
  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput '%s'\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= &yypParser->yystack
            && yymx != YYERRORSYMBOL
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
#line 25 "grammar.y"


using namespace std;
typedef struct {char *value; int line;} token;

token* create_token (char *value, int line) {
	token *t = (token*) malloc (sizeof (token));
	t->value = strdup (value);
	t->line = line;
	return t;
}

const char * getValue (cJSON* token) {
	return cJSON_GetObjectItem (token, "value")->valuestring;
}


const char * getLine (cJSON* token) {
	return cJSON_GetObjectItem (token, "line")->valuestring;
}


int main(int argc, char* argv[]) {
	char *result;
	std::string line;
	std::string input = "";
	while (std::getline(std::cin, line)) {
      input += line + "\n";
    }
	if (input == "") {
		cout << "Empty input";
		exit(0);
	}
	
	cJSON *root = cJSON_Parse(input.c_str());
	
	if (!root) {
		cout << "JSON invalid\n";
		exit(0);
	}
	
	void* pParser = ParseAlloc (malloc);
	int num = cJSON_GetArraySize (root);
	
	for (int i = 0; i < num; i++ ) {
	
		// Knoten im Token-Stream auslesen
		cJSON *node = cJSON_GetArrayItem(root,i);
		
		char *line = cJSON_GetArrayItem(node,0)->valuestring;
		char *type = cJSON_GetArrayItem(node,1)->valuestring;
		char *value = cJSON_GetArrayItem(node,2)->valuestring;
		
		cJSON *tok = cJSON_CreateObject();
		cJSON_AddStringToObject(tok, "value", value);
		cJSON_AddStringToObject(tok, "line", line);

		linenumber = line;
		curtoken = value;
		curtype = type;
		// THE und Kommentare werden ueberlesen
		if (strcmp(type, "THE") == 0) continue;
		if (strcmp(type, "COMMENT") == 0) continue;
		if (strcmp(type, "MCOMMENT") == 0) continue;
		
		int tokenid = get_token_id (type);
		Parse (pParser, tokenid, tok);
		
	}
	Parse (pParser, 0, 0);
    ParseFree(pParser, free );
}




/////////////////////// 
/////////////////////// 
// TOKENS
///////////////////////
/////////////////////// 

int get_token_id (char *token) {
	if (strcmp(token, "AMPERSAND") == 0) return AMPERSAND;
	if (strcmp(token, "ASSIGN") == 0) return ASSIGN;
	if (strcmp(token, "BOOLTOKEN") == 0) return BOOLTOKEN;
	if (strcmp(token, "COMMA") == 0) return COMMA;
	if (strcmp(token, "COS") == 0) return COS;
	if (strcmp(token, "COUNT") == 0) return COUNT;
	if (strcmp(token, "DIVIDE") == 0) return DIVIDE;
	if (strcmp(token, "FIRST") == 0) return FIRST;
	if (strcmp(token, "GT") == 0) return GT;
	if (strcmp(token, "GTEQ") == 0) return GTEQ;
	if (strcmp(token, "IDENTIFIER") == 0) return IDENTIFIER;
	if (strcmp(token, "IS") == 0) return IS;
	if (strcmp(token, "IT") == 0) return IT;
	if (strcmp(token, "LPAR") == 0) return LPAR; 
	if (strcmp(token, "RPAR") == 0) return RPAR;
	if (strcmp(token, "LSPAR") == 0) return LSPAR;
	if (strcmp(token, "RSPAR") == 0) return RSPAR;
	if (strcmp(token, "LIST") == 0) return LIST;
	if (strcmp(token, "LT") == 0) return LT;
	if (strcmp(token, "LTEQ") == 0) return LTEQ;
	if (strcmp(token, "MINUS") == 0) return MINUS;
	if (strcmp(token, "NEQ") == 0) return NEQ;
	if (strcmp(token, "NOT") == 0) return NOT;
	if (strcmp(token, "NOW") == 0) return NOW;
	if (strcmp(token, "NUMBER") == 0) return NUMBER;
	if (strcmp(token, "NUMTOKEN") == 0) return NUMTOKEN;
	if (strcmp(token, "NULLTOKEN") == 0) return NULLTOKEN;
	if (strcmp(token, "OF") == 0) return OF;
	if (strcmp(token, "PLUS") == 0) return PLUS;
	if (strcmp(token, "POWER") == 0) return POWER;
	if (strcmp(token, "SEMICOLON") == 0) return SEMICOLON;
	if (strcmp(token, "SIN") == 0) return SIN;
	if (strcmp(token, "STRTOKEN") == 0) return STRTOKEN;
	if (strcmp(token, "TIME") == 0) return TIME;
	if (strcmp(token, "TIMES") == 0) return TIMES;
	if (strcmp(token, "TIMETOKEN") == 0) return TIMETOKEN;
	if (strcmp(token, "THEY") == 0) return THEY;
	if (strcmp(token, "TRACE") == 0) return TRACE;
	if (strcmp(token, "WHERE") == 0) return WHERE;
	if (strcmp(token, "WRITE") == 0) return WRITE;
	
	printf ("{\"error\" : true, \"message\": \"UNKNOWN TOKEN TYPE %s\"}\n", token);
	exit(0);
} 
  


cJSON* unary (char* fname, cJSON* a) 
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a);
	cJSON_AddStringToObject(res, "type", fname);
	cJSON_AddItemToObject(res, "arg", arg);
	return res;
} 

cJSON* binary (char *fname, cJSON *a, cJSON *b) 
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a); 
	cJSON_AddItemToArray(arg, b);
	cJSON_AddStringToObject(res, "type", fname); 
	cJSON_AddItemToObject(res, "arg", arg);
	return res;
}

cJSON* ternary (char *fname, cJSON *a, cJSON *b, cJSON *c) 
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a); 
	cJSON_AddItemToArray(arg, b); 
	cJSON_AddItemToArray(arg, c);
	cJSON_AddStringToObject(res, "type", fname); 
	cJSON_AddItemToObject(res, "arg", arg);
	return res;
}

#line 1757 "grammar.c"
