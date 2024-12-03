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
#define YYNOCODE 58
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
#define YYNSTATE             94
#define YYNRULE              53
#define YY_MAX_SHIFT         93
#define YY_MIN_SHIFTREDUCE   119
#define YY_MAX_SHIFTREDUCE   171
#define YY_MIN_REDUCE        172
#define YY_MAX_REDUCE        224
#define YY_ERROR_ACTION      225
#define YY_ACCEPT_ACTION     226
#define YY_NO_ACTION         227
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
#define YY_ACTTAB_COUNT (439)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    17,   46,  225,  225,  225,  225,  225,   33,   31,   27,
 /*    10 */    21,   25,   34,   19,   32,   20,   56,   16,   29,   19,
 /*    20 */    32,   20,  226,   36,   91,  133,    1,  139,  132,   35,
 /*    30 */    17,  136,  137,  138,  140,  141,  142,   93,   31,   27,
 /*    40 */    21,   25,   34,   19,   32,   20,   58,   16,   29,   51,
 /*    50 */    75,   50,   38,  171,   91,   42,    1,  139,  121,   35,
 /*    60 */    17,  136,  137,  138,  140,  141,  142,   43,   31,   27,
 /*    70 */     8,   80,   44,   45,  146,  148,   92,   16,   29,   47,
 /*    80 */    82,   54,   78,   12,   91,   48,    1,  139,   55,   35,
 /*    90 */    17,  136,  137,  138,  140,  141,  142,   57,   31,   27,
 /*   100 */    59,   81,  150,   88,   85,   60,   52,   16,   29,    6,
 /*   110 */    86,   87,   79,   77,   91,   71,    1,  139,   61,   35,
 /*   120 */    17,  136,  137,  138,  140,  141,  142,   64,   31,   27,
 /*   130 */    65,   83,   72,   66,   67,   63,  144,   16,   29,  172,
 /*   140 */    62,   68,   39,    9,   91,   84,    1,  139,   53,   35,
 /*   150 */    17,  136,  137,  138,  140,  141,  142,   70,   31,   27,
 /*   160 */    49,    5,   10,   37,    2,   73,   15,   16,   29,   22,
 /*   170 */    76,   13,    7,    3,   91,   20,    1,  139,   14,   35,
 /*   180 */    40,  136,  137,  138,  140,  141,  142,   18,   41,   69,
 /*   190 */    74,   30,   28,   24,   23,   26,  131,   89,   90,   21,
 /*   200 */    25,   34,   19,   32,   20,    4,  147,  174,  174,  128,
 /*   210 */   174,   18,  174,   69,   74,   30,   28,   24,   23,   26,
 /*   220 */   174,  174,  174,   21,   25,   34,   19,   32,   20,  174,
 /*   230 */   174,  174,  174,  127,  174,   18,  174,   69,   74,   30,
 /*   240 */    28,   24,   23,   26,  174,  174,  174,   21,   25,   34,
 /*   250 */    19,   32,   20,  174,  174,  174,  174,  126,  174,   18,
 /*   260 */   174,   69,   74,   30,   28,   24,   23,   26,  174,  174,
 /*   270 */   174,   21,   25,   34,   19,   32,   20,  174,  174,  174,
 /*   280 */   174,  125,  174,   18,  174,   69,   74,   30,   28,   24,
 /*   290 */    23,   26,  174,  174,  174,   21,   25,   34,   19,   32,
 /*   300 */    20,  174,  174,  174,  174,  124,  174,   18,  174,   69,
 /*   310 */    74,   30,   28,   24,   23,   26,  174,  174,  174,   21,
 /*   320 */    25,   34,   19,   32,   20,  174,  174,  174,  174,  123,
 /*   330 */   174,   18,  174,   69,   74,   30,   28,   24,   23,   26,
 /*   340 */   174,  174,  174,   21,   25,   34,   19,   32,   20,   18,
 /*   350 */   174,   69,   74,   30,   28,   24,   23,   26,  174,  174,
 /*   360 */   174,   21,   25,   34,   19,   32,   20,  174,  174,  174,
 /*   370 */   174,  174,  174,  174,  174,  174,  174,  174,  174,   11,
 /*   380 */   129,   18,  174,   69,   74,   30,   28,   24,   23,   26,
 /*   390 */   174,  174,  174,   21,   25,   34,   19,   32,   20,  174,
 /*   400 */   174,  174,  174,  122,  174,   18,  174,   69,   74,   30,
 /*   410 */    28,   24,   23,   26,  174,  174,  174,   21,   25,   34,
 /*   420 */    19,   32,   20,  225,  225,   30,   28,   24,   23,   26,
 /*   430 */   174,  174,  174,   21,   25,   34,   19,   32,   20,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,   55,    5,    6,    7,    8,    9,   12,   10,   11,
 /*    10 */    13,   14,   15,   16,   17,   18,   55,   19,   20,   16,
 /*    20 */    17,   18,   51,   52,   26,   30,   28,   29,   30,   31,
 /*    30 */     2,   33,   34,   35,   36,   37,   38,    2,   10,   11,
 /*    40 */    13,   14,   15,   16,   17,   18,   55,   19,   20,   55,
 /*    50 */    56,   55,   24,   21,   26,   55,   28,   29,   53,   31,
 /*    60 */     2,   33,   34,   35,   36,   37,   38,   55,   10,   11,
 /*    70 */    27,   28,   55,   55,   39,   40,   41,   19,   20,   55,
 /*    80 */    45,   55,   30,   48,   26,   55,   28,   29,   55,   31,
 /*    90 */     2,   33,   34,   35,   36,   37,   38,   55,   10,   11,
 /*   100 */    55,   43,   55,   46,   47,   55,   55,   19,   20,   27,
 /*   110 */    55,   55,   29,   54,   26,   55,   28,   29,   55,   31,
 /*   120 */     2,   33,   34,   35,   36,   37,   38,   55,   10,   11,
 /*   130 */    55,   43,   55,   55,   55,   55,   55,   19,   20,    0,
 /*   140 */    55,   55,   24,   54,   26,   55,   28,   29,   55,   31,
 /*   150 */     2,   33,   34,   35,   36,   37,   38,   55,   10,   11,
 /*   160 */    55,   22,   54,   24,   25,   26,   54,   19,   20,   54,
 /*   170 */    26,   44,   27,   42,   26,   18,   28,   29,   44,   31,
 /*   180 */    24,   33,   34,   35,   36,   37,   38,    1,   24,    3,
 /*   190 */     4,    5,    6,    7,    8,    9,   30,   29,   28,   13,
 /*   200 */    14,   15,   16,   17,   18,   42,   39,   57,   57,   23,
 /*   210 */    57,    1,   57,    3,    4,    5,    6,    7,    8,    9,
 /*   220 */    57,   57,   57,   13,   14,   15,   16,   17,   18,   57,
 /*   230 */    57,   57,   57,   23,   57,    1,   57,    3,    4,    5,
 /*   240 */     6,    7,    8,    9,   57,   57,   57,   13,   14,   15,
 /*   250 */    16,   17,   18,   57,   57,   57,   57,   23,   57,    1,
 /*   260 */    57,    3,    4,    5,    6,    7,    8,    9,   57,   57,
 /*   270 */    57,   13,   14,   15,   16,   17,   18,   57,   57,   57,
 /*   280 */    57,   23,   57,    1,   57,    3,    4,    5,    6,    7,
 /*   290 */     8,    9,   57,   57,   57,   13,   14,   15,   16,   17,
 /*   300 */    18,   57,   57,   57,   57,   23,   57,    1,   57,    3,
 /*   310 */     4,    5,    6,    7,    8,    9,   57,   57,   57,   13,
 /*   320 */    14,   15,   16,   17,   18,   57,   57,   57,   57,   23,
 /*   330 */    57,    1,   57,    3,    4,    5,    6,    7,    8,    9,
 /*   340 */    57,   57,   57,   13,   14,   15,   16,   17,   18,    1,
 /*   350 */    57,    3,    4,    5,    6,    7,    8,    9,   57,   57,
 /*   360 */    57,   13,   14,   15,   16,   17,   18,   57,   57,   57,
 /*   370 */    57,   57,   57,   57,   57,   57,   57,   57,   57,   49,
 /*   380 */    32,    1,   57,    3,    4,    5,    6,    7,    8,    9,
 /*   390 */    57,   57,   57,   13,   14,   15,   16,   17,   18,   57,
 /*   400 */    57,   57,   57,   23,   57,    1,   57,    3,    4,    5,
 /*   410 */     6,    7,    8,    9,   57,   57,   57,   13,   14,   15,
 /*   420 */    16,   17,   18,    3,    4,    5,    6,    7,    8,    9,
 /*   430 */    57,   57,   57,   13,   14,   15,   16,   17,   18,
};
#define YY_SHIFT_USE_DFLT (-6)
#define YY_SHIFT_COUNT (93)
#define YY_SHIFT_MIN   (-5)
#define YY_SHIFT_MAX   (420)
static const short yy_shift_ofst[] = {
 /*     0 */    -6,   -2,   28,   58,   88,  118,  148,  148,  148,  148,
 /*    10 */   148,  148,  148,  148,  148,  148,  148,  148,  148,  148,
 /*    20 */   148,  148,  148,  148,  148,  148,  148,  148,  148,  148,
 /*    30 */   148,  148,  148,  148,  148,  148,  139,   32,   32,   32,
 /*    40 */    32,   32,  186,  210,  234,  258,  282,  306,  330,  348,
 /*    50 */   380,  404,  404,  404,  420,  420,  420,  420,  420,  420,
 /*    60 */   420,  420,   -3,   -3,   -3,   -3,   -3,   27,   27,   35,
 /*    70 */     3,    3,    3,   43,   57,   -5,   82,  144,  145,   52,
 /*    80 */    83,  127,  131,  134,  157,  156,  157,  157,  164,  166,
 /*    90 */   168,  170,  163,  167,
};
#define YY_REDUCE_USE_DFLT (-55)
#define YY_REDUCE_COUNT (41)
#define YY_REDUCE_MIN   (-54)
#define YY_REDUCE_MAX   (115)
static const signed char yy_reduce_ofst[] = {
 /*     0 */   -29,   -6,  -54,  -39,   -9,   -4,    0,   12,   17,   18,
 /*    10 */    24,   26,   30,   33,   42,   45,   47,   50,   51,   55,
 /*    20 */    56,   60,   63,   72,   75,   77,   78,   79,   80,   81,
 /*    30 */    85,   86,   90,   93,  102,  105,    5,   59,   89,  108,
 /*    40 */   112,  115,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   173,  225,  225,  225,  225,  225,  225,  225,  225,  225,
 /*    10 */   225,  225,  225,  225,  225,  225,  225,  225,  225,  225,
 /*    20 */   225,  225,  225,  225,  225,  225,  225,  225,  225,  225,
 /*    30 */   225,  225,  225,  225,  225,  225,  225,  223,  223,  223,
 /*    40 */   223,  223,  225,  225,  225,  225,  225,  225,  225,  225,
 /*    50 */   225,  187,  221,  188,  222,  215,  213,  209,  207,  217,
 /*    60 */   202,  216,  206,  208,  214,  212,  210,  198,  196,  225,
 /*    70 */   204,  218,  211,  225,  225,  225,  225,  225,  225,  225,
 /*    80 */   225,  225,  225,  225,  205,  225,  220,  219,  225,  225,
 /*    90 */   225,  183,  225,  225,
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
  "$",             "WHERE",         "NOT",           "IS",          
  "OCCUR",         "GT",            "GTEQ",          "LT",          
  "LTEQ",          "NEQ",           "COUNT",         "FIRST",       
  "COMMA",         "PLUS",          "MINUS",         "AMPERSAND",   
  "TIMES",         "DIVIDE",        "POWER",         "SIN",         
  "COS",           "OF",            "WRITE",         "SEMICOLON",   
  "TIME",          "TRACE",         "IDENTIFIER",    "ASSIGN",      
  "LSPAR",         "NUMTOKEN",      "RSPAR",         "LPAR",        
  "RPAR",          "BOOLTOKEN",     "IT",            "NOW",         
  "NULLTOKEN",     "STRTOKEN",      "TIMETOKEN",     "LIST",        
  "NUMBER",        "GREATER",       "THAN",          "OR",          
  "EQUAL",         "LESS",          "AFTER",         "BEFORE",      
  "WITHIN",        "TO",            "error",         "code",        
  "statementblock",  "statement",     "of",            "ex",          
  "exlist",      
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
 /*   4 */ "statement ::= WRITE TIME of ex SEMICOLON",
 /*   5 */ "statement ::= TRACE ex SEMICOLON",
 /*   6 */ "statement ::= TRACE TIME of ex SEMICOLON",
 /*   7 */ "statement ::= IDENTIFIER ASSIGN ex SEMICOLON",
 /*   8 */ "statement ::= IDENTIFIER LSPAR NUMTOKEN RSPAR ASSIGN ex SEMICOLON",
 /*   9 */ "statement ::= TIME of IDENTIFIER ASSIGN ex SEMICOLON",
 /*  10 */ "ex ::= LPAR ex RPAR",
 /*  11 */ "ex ::= IDENTIFIER",
 /*  12 */ "ex ::= IDENTIFIER LSPAR NUMTOKEN RSPAR",
 /*  13 */ "ex ::= LSPAR RSPAR",
 /*  14 */ "ex ::= LSPAR exlist RSPAR",
 /*  15 */ "exlist ::= ex",
 /*  16 */ "exlist ::= exlist COMMA ex",
 /*  17 */ "ex ::= BOOLTOKEN",
 /*  18 */ "ex ::= IT",
 /*  19 */ "ex ::= NOW",
 /*  20 */ "ex ::= NUMTOKEN",
 /*  21 */ "ex ::= NULLTOKEN",
 /*  22 */ "ex ::= STRTOKEN",
 /*  23 */ "ex ::= TIMETOKEN",
 /*  24 */ "ex ::= COUNT ex",
 /*  25 */ "ex ::= COS ex",
 /*  26 */ "ex ::= FIRST ex",
 /*  27 */ "ex ::= ex IS LIST",
 /*  28 */ "ex ::= ex IS NOT LIST",
 /*  29 */ "ex ::= ex IS NUMBER",
 /*  30 */ "ex ::= NOT ex",
 /*  31 */ "ex ::= SIN ex",
 /*  32 */ "ex ::= ex AMPERSAND ex",
 /*  33 */ "ex ::= ex DIVIDE ex",
 /*  34 */ "ex ::= ex GT ex",
 /*  35 */ "ex ::= ex IS GREATER THAN ex",
 /*  36 */ "ex ::= ex GTEQ ex",
 /*  37 */ "ex ::= ex IS GREATER THAN OR EQUAL ex",
 /*  38 */ "ex ::= ex NEQ ex",
 /*  39 */ "ex ::= ex MINUS ex",
 /*  40 */ "ex ::= ex LT ex",
 /*  41 */ "ex ::= ex IS LESS THAN ex",
 /*  42 */ "ex ::= ex LTEQ ex",
 /*  43 */ "ex ::= ex IS LESS THAN OR EQUAL ex",
 /*  44 */ "ex ::= ex OCCUR AFTER TIME of ex",
 /*  45 */ "ex ::= ex OCCUR BEFORE TIME of ex",
 /*  46 */ "ex ::= ex PLUS ex",
 /*  47 */ "ex ::= ex POWER ex",
 /*  48 */ "ex ::= ex TIMES ex",
 /*  49 */ "ex ::= ex WHERE ex",
 /*  50 */ "ex ::= ex IS WITHIN ex TO ex",
 /*  51 */ "of ::=",
 /*  52 */ "of ::= OF",
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
  { 51, 1 },
  { 52, 0 },
  { 52, 2 },
  { 53, 3 },
  { 53, 5 },
  { 53, 3 },
  { 53, 5 },
  { 53, 4 },
  { 53, 7 },
  { 53, 6 },
  { 55, 3 },
  { 55, 1 },
  { 55, 4 },
  { 55, 2 },
  { 55, 3 },
  { 56, 1 },
  { 56, 3 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 2 },
  { 55, 2 },
  { 55, 2 },
  { 55, 3 },
  { 55, 4 },
  { 55, 3 },
  { 55, 2 },
  { 55, 2 },
  { 55, 3 },
  { 55, 3 },
  { 55, 3 },
  { 55, 5 },
  { 55, 3 },
  { 55, 7 },
  { 55, 3 },
  { 55, 3 },
  { 55, 3 },
  { 55, 5 },
  { 55, 3 },
  { 55, 7 },
  { 55, 6 },
  { 55, 6 },
  { 55, 3 },
  { 55, 3 },
  { 55, 3 },
  { 55, 3 },
  { 55, 6 },
  { 54, 0 },
  { 54, 1 },
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
#line 231 "grammar.y"
{
	printf (cJSON_Print(yymsp[0].minor.yy0)); 
}
#line 983 "grammar.c"
        break;
      case 1: /* statementblock ::= */
#line 241 "grammar.y"
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STATEMENTBLOCK");
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToObject(res, "statements", arg); 
	yymsp[1].minor.yy0 = res;
}
#line 994 "grammar.c"
        break;
      case 2: /* statementblock ::= statementblock statement */
#line 250 "grammar.y"
{
	cJSON_AddItemToArray(cJSON_GetObjectItem ( yymsp[-1].minor.yy0, "statements"), yymsp[0].minor.yy0);
	yylhsminor.yy0 = yymsp[-1].minor.yy0;
}
#line 1002 "grammar.c"
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 3: /* statement ::= WRITE ex SEMICOLON */
#line 267 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-2].minor.yy0 = res; 
}
#line 1013 "grammar.c"
        break;
      case 4: /* statement ::= WRITE TIME of ex SEMICOLON */
#line 279 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE_TIME"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-4].minor.yy0 = res; 
}
#line 1023 "grammar.c"
        break;
      case 5: /* statement ::= TRACE ex SEMICOLON */
#line 291 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE"); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-2].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1034 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 6: /* statement ::= TRACE TIME of ex SEMICOLON */
#line 304 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE_TIME"); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-4].minor.yy0));
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1046 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 7: /* statement ::= IDENTIFIER ASSIGN ex SEMICOLON */
#line 317 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1058 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 8: /* statement ::= IDENTIFIER LSPAR NUMTOKEN RSPAR ASSIGN ex SEMICOLON */
#line 330 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, yymsp[-1].minor.yy0);
	cJSON_AddStringToObject(res, "type", "LIST_ASSIGN");
	cJSON_AddStringToObject(res, "listname", getValue(yymsp[-6].minor.yy0)); 
	cJSON_AddStringToObject(res, "index", getValue(yymsp[-4].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", arg); 
	yylhsminor.yy0 = res; 
}
#line 1073 "grammar.c"
  yymsp[-6].minor.yy0 = yylhsminor.yy0;
        break;
      case 9: /* statement ::= TIME of IDENTIFIER ASSIGN ex SEMICOLON */
#line 346 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN_TIME");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-5].minor.yy0 = res; 
}
#line 1085 "grammar.c"
        break;
      case 10: /* ex ::= LPAR ex RPAR */
#line 360 "grammar.y"
{ 
	yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0; 
}
#line 1092 "grammar.c"
        break;
      case 11: /* ex ::= IDENTIFIER */
#line 366 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IDENTIFIER"); 
	cJSON_AddStringToObject(res, "name", getValue(yymsp[0].minor.yy0)); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1103 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 12: /* ex ::= IDENTIFIER LSPAR NUMTOKEN RSPAR */
#line 376 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "LIST_ACCESS"); 
	cJSON_AddStringToObject(res, "name", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddStringToObject(res, "index", getValue(yymsp[-1].minor.yy0));
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-3].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1116 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 13: /* ex ::= LSPAR RSPAR */
#line 391 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "EMPTYLIST");
	yymsp[-1].minor.yy0 = res; 
}
#line 1126 "grammar.c"
        break;
      case 14: /* ex ::= LSPAR exlist RSPAR */
#line 399 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "LIST");
	cJSON_AddItemToObject(res, "elements", yymsp[-1].minor.yy0); 
	yymsp[-2].minor.yy0 = res; 
}
#line 1136 "grammar.c"
        break;
      case 15: /* exlist ::= ex */
#line 408 "grammar.y"
{
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, yymsp[0].minor.yy0);
	yylhsminor.yy0 = arg;
}
#line 1145 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 16: /* exlist ::= exlist COMMA ex */
#line 416 "grammar.y"
{
	cJSON_AddItemToArray(yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
	yylhsminor.yy0 = yymsp[-2].minor.yy0;
}
#line 1154 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 17: /* ex ::= BOOLTOKEN */
#line 427 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "BOOLTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1165 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 18: /* ex ::= IT */
#line 435 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IT"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1175 "grammar.c"
        break;
      case 19: /* ex ::= NOW */
#line 442 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NOW"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1184 "grammar.c"
        break;
      case 20: /* ex ::= NUMTOKEN */
#line 449 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "NUMTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1194 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 21: /* ex ::= NULLTOKEN */
#line 457 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NULLTOKEN"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1204 "grammar.c"
        break;
      case 22: /* ex ::= STRTOKEN */
#line 464 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STRTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1214 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 23: /* ex ::= TIMETOKEN */
#line 472 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "TIMETOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1225 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 24: /* ex ::= COUNT ex */
#line 487 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("COUNT", yymsp[0].minor.yy0); }
#line 1231 "grammar.c"
        break;
      case 25: /* ex ::= COS ex */
#line 488 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("COS", yymsp[0].minor.yy0); }
#line 1236 "grammar.c"
        break;
      case 26: /* ex ::= FIRST ex */
#line 489 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("FIRST", yymsp[0].minor.yy0); }
#line 1241 "grammar.c"
        break;
      case 27: /* ex ::= ex IS LIST */
#line 490 "grammar.y"
{yylhsminor.yy0 = unary ("IS_LIST", yymsp[-2].minor.yy0); }
#line 1246 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 28: /* ex ::= ex IS NOT LIST */
#line 491 "grammar.y"
{yylhsminor.yy0 = unary ("IS_NOT_LIST", yymsp[-3].minor.yy0); }
#line 1252 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 29: /* ex ::= ex IS NUMBER */
#line 492 "grammar.y"
{yylhsminor.yy0 = unary ("IS_NUMBER", yymsp[-2].minor.yy0); }
#line 1258 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 30: /* ex ::= NOT ex */
#line 493 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("NOT", yymsp[0].minor.yy0); }
#line 1264 "grammar.c"
        break;
      case 31: /* ex ::= SIN ex */
#line 494 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("SIN", yymsp[0].minor.yy0); }
#line 1269 "grammar.c"
        break;
      case 32: /* ex ::= ex AMPERSAND ex */
#line 500 "grammar.y"
{yylhsminor.yy0 = binary ("AMPERSAND", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1274 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 33: /* ex ::= ex DIVIDE ex */
#line 501 "grammar.y"
{yylhsminor.yy0 = binary ("DIVIDE", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1280 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 34: /* ex ::= ex GT ex */
#line 502 "grammar.y"
{yylhsminor.yy0 = binary ("GT", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1286 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 35: /* ex ::= ex IS GREATER THAN ex */
#line 503 "grammar.y"
{yylhsminor.yy0 = binary ("GT", yymsp[-4].minor.yy0, yymsp[0].minor.yy0); }
#line 1292 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 36: /* ex ::= ex GTEQ ex */
#line 504 "grammar.y"
{yylhsminor.yy0 = binary ("GTEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1298 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 37: /* ex ::= ex IS GREATER THAN OR EQUAL ex */
#line 505 "grammar.y"
{yylhsminor.yy0 = binary ("GTEQ", yymsp[-6].minor.yy0, yymsp[0].minor.yy0); }
#line 1304 "grammar.c"
  yymsp[-6].minor.yy0 = yylhsminor.yy0;
        break;
      case 38: /* ex ::= ex NEQ ex */
#line 506 "grammar.y"
{yylhsminor.yy0 = binary ("NEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1310 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 39: /* ex ::= ex MINUS ex */
#line 507 "grammar.y"
{yylhsminor.yy0 = binary ("MINUS", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1316 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 40: /* ex ::= ex LT ex */
#line 508 "grammar.y"
{yylhsminor.yy0 = binary ("LT", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1322 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 41: /* ex ::= ex IS LESS THAN ex */
#line 509 "grammar.y"
{yylhsminor.yy0 = binary ("LT", yymsp[-4].minor.yy0, yymsp[0].minor.yy0); }
#line 1328 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 42: /* ex ::= ex LTEQ ex */
#line 510 "grammar.y"
{yylhsminor.yy0 = binary ("LTEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1334 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 43: /* ex ::= ex IS LESS THAN OR EQUAL ex */
#line 511 "grammar.y"
{yylhsminor.yy0 = binary ("LTEQ", yymsp[-6].minor.yy0, yymsp[0].minor.yy0); }
#line 1340 "grammar.c"
  yymsp[-6].minor.yy0 = yylhsminor.yy0;
        break;
      case 44: /* ex ::= ex OCCUR AFTER TIME of ex */
#line 512 "grammar.y"
{yylhsminor.yy0 = binary ("OCCUR_AFTER", yymsp[-5].minor.yy0, yymsp[0].minor.yy0); }
#line 1346 "grammar.c"
  yymsp[-5].minor.yy0 = yylhsminor.yy0;
        break;
      case 45: /* ex ::= ex OCCUR BEFORE TIME of ex */
#line 513 "grammar.y"
{yylhsminor.yy0 = binary ("OCCUR_BEFORE", yymsp[-5].minor.yy0, yymsp[0].minor.yy0); }
#line 1352 "grammar.c"
  yymsp[-5].minor.yy0 = yylhsminor.yy0;
        break;
      case 46: /* ex ::= ex PLUS ex */
#line 514 "grammar.y"
{yylhsminor.yy0 = binary ("PLUS", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1358 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 47: /* ex ::= ex POWER ex */
#line 515 "grammar.y"
{yylhsminor.yy0 = binary ("POWER", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1364 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 48: /* ex ::= ex TIMES ex */
#line 516 "grammar.y"
{yylhsminor.yy0 = binary ("TIMES", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1370 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 49: /* ex ::= ex WHERE ex */
#line 517 "grammar.y"
{yylhsminor.yy0 = binary ("WHERE", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1376 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 50: /* ex ::= ex IS WITHIN ex TO ex */
#line 523 "grammar.y"
{yylhsminor.yy0 = ternary ("IS_WITHIN", yymsp[-5].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1382 "grammar.c"
  yymsp[-5].minor.yy0 = yylhsminor.yy0;
        break;
      default:
      /* (51) of ::= */ yytestcase(yyruleno==51);
      /* (52) of ::= OF */ yytestcase(yyruleno==52);
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
#line 201 "grammar.y"

  printf ("{\"error\" : true, \"message\": \"Syntax Error: Compiler reports unexpected token \\\"%s\\\" of type \\\"%s\\\" in line %s\"}\n", curtoken, curtype, linenumber);
  exit(0);
#line 1448 "grammar.c"
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
	if (strcmp(token, "AFTER") == 0) return AFTER;
	if (strcmp(token, "AMPERSAND") == 0) return AMPERSAND;
	if (strcmp(token, "ASSIGN") == 0) return ASSIGN;
	if (strcmp(token, "BEFORE") == 0) return BEFORE;
	if (strcmp(token, "BOOLTOKEN") == 0) return BOOLTOKEN;
	if (strcmp(token, "COMMA") == 0) return COMMA;
	if (strcmp(token, "COS") == 0) return COS;
	if (strcmp(token, "COUNT") == 0) return COUNT;
	if (strcmp(token, "DIVIDE") == 0) return DIVIDE;
	if (strcmp(token, "EQUAL") == 0) return EQUAL;
	if (strcmp(token, "FIRST") == 0) return FIRST;
	if (strcmp(token, "GREATER") == 0) return GREATER;
	if (strcmp(token, "GT") == 0) return GT;
	if (strcmp(token, "GTEQ") == 0) return GTEQ;
	if (strcmp(token, "IDENTIFIER") == 0) return IDENTIFIER;
	if (strcmp(token, "IS") == 0) return IS;
	if (strcmp(token, "IT") == 0) return IT;
	if (strcmp(token, "LPAR") == 0) return LPAR; 
	if (strcmp(token, "RPAR") == 0) return RPAR;
	if (strcmp(token, "LSPAR") == 0) return LSPAR;
	if (strcmp(token, "RSPAR") == 0) return RSPAR;
	if (strcmp(token, "LESS") == 0) return LESS;
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
	if (strcmp(token, "OR") == 0) return OR;
	if (strcmp(token, "OCCURS") == 0) return OCCUR;
	if (strcmp(token, "OCCURRED") == 0) return OCCUR;
	if (strcmp(token, "PLUS") == 0) return PLUS;
	if (strcmp(token, "POWER") == 0) return POWER;
	if (strcmp(token, "SEMICOLON") == 0) return SEMICOLON;
	if (strcmp(token, "SIN") == 0) return SIN;
	if (strcmp(token, "STRTOKEN") == 0) return STRTOKEN;
	if (strcmp(token, "TIME") == 0) return TIME;
	if (strcmp(token, "TIMES") == 0) return TIMES;
	if (strcmp(token, "TIMETOKEN") == 0) return TIMETOKEN;
	if (strcmp(token, "THAN") == 0) return THAN;
	if (strcmp(token, "THEY") == 0) return IT;
	if (strcmp(token, "TO") == 0) return TO;
	if (strcmp(token, "TRACE") == 0) return TRACE;
	if (strcmp(token, "WHERE") == 0) return WHERE;
	if (strcmp(token, "WITHIN") == 0) return WITHIN;
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

#line 1826 "grammar.c"
