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
#define YYNOCODE 70
#define YYACTIONTYPE unsigned short int
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
#define YYNSTATE             114
#define YYNRULE              60
#define YY_MAX_SHIFT         113
#define YY_MIN_SHIFTREDUCE   143
#define YY_MAX_SHIFTREDUCE   202
#define YY_MIN_REDUCE        203
#define YY_MAX_REDUCE        262
#define YY_ERROR_ACTION      263
#define YY_ACCEPT_ACTION     264
#define YY_NO_ACTION         265
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
#define YY_ACTTAB_COUNT (544)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    21,   56,   25,   30,   39,   23,   37,   24,   36,   32,
 /*    10 */    29,   23,   37,   24,  264,   43,   29,   20,   34,    4,
 /*    20 */    62,  113,   89,   50,    2,  111,   84,  106,  150,   61,
 /*    30 */    67,    9,   44,    6,  146,   61,    1,  169,  162,   40,
 /*    40 */    21,  166,  167,  168,  170,  171,  172,   38,   36,   32,
 /*    50 */    62,   21,   90,  145,   12,   95,  147,   20,   34,   36,
 /*    60 */    32,   24,  101,   99,   29,   69,   60,  106,   20,   34,
 /*    70 */    38,  145,   52,   45,   53,  163,    1,  169,  106,   40,
 /*    80 */    54,  166,  167,  168,  170,  171,  172,    1,  169,   49,
 /*    90 */    40,   21,  166,  167,  168,  170,  171,  172,  108,   36,
 /*   100 */    32,   55,   96,  263,  263,  263,  263,  263,   20,   34,
 /*   110 */    57,   25,   30,   39,   23,   37,   24,   65,  106,   29,
 /*   120 */    58,   66,   68,   10,   70,  100,   71,    1,  169,   63,
 /*   130 */    40,   21,  166,  167,  168,  170,  171,  172,   86,   36,
 /*   140 */    32,   87,   98,   82,   72,  176,  178,  107,   20,   34,
 /*   150 */    75,   97,   76,  102,   16,   83,   77,   78,  106,   74,
 /*   160 */   103,  202,   73,   11,   46,   79,   85,    1,  169,   64,
 /*   170 */    40,   81,  166,  167,  168,  170,  171,  172,   22,   59,
 /*   180 */    80,   88,   35,   33,   28,   27,   31,   92,   13,   14,
 /*   190 */    25,   30,   39,   23,   37,   24,   41,   42,   29,   19,
 /*   200 */    26,    5,  158,   91,   22,   93,   80,   88,   35,   33,
 /*   210 */    28,   27,   31,   17,   94,    7,   25,   30,   39,   23,
 /*   220 */    37,   24,   18,   47,   29,   29,   48,  161,  157,  104,
 /*   230 */    22,  105,   80,   88,   35,   33,   28,   27,   31,  151,
 /*   240 */     3,    8,   25,   30,   39,   23,   37,   24,  177,  110,
 /*   250 */    29,  149,  148,  205,  156,  205,   22,  205,   80,   88,
 /*   260 */    35,   33,   28,   27,   31,  205,  205,  205,   25,   30,
 /*   270 */    39,   23,   37,   24,  205,  205,   29,  205,  205,  205,
 /*   280 */   155,  205,   22,  205,   80,   88,   35,   33,   28,   27,
 /*   290 */    31,  205,  205,  205,   25,   30,   39,   23,   37,   24,
 /*   300 */   205,  205,   29,  205,  205,  205,  154,  205,   22,  205,
 /*   310 */    80,   88,   35,   33,   28,   27,   31,  205,  205,  205,
 /*   320 */    25,   30,   39,   23,   37,   24,  205,  205,   29,  205,
 /*   330 */   205,  205,  153,  205,   22,  205,   80,   88,   35,   33,
 /*   340 */    28,   27,   31,  205,  205,  205,   25,   30,   39,   23,
 /*   350 */    37,   24,  205,   22,   29,   80,   88,   35,   33,   28,
 /*   360 */    27,   31,  205,  205,  205,   25,   30,   39,   23,   37,
 /*   370 */    24,    4,  205,   29,  205,  205,  205,  111,   84,  205,
 /*   380 */   205,  109,  205,    9,   44,    6,  205,  205,  205,  205,
 /*   390 */   205,  205,   15,  205,  159,  205,   22,  205,   80,   88,
 /*   400 */    35,   33,   28,   27,   31,  205,  205,  205,   25,   30,
 /*   410 */    39,   23,   37,   24,  205,  205,   29,  205,  205,  205,
 /*   420 */   152,  205,   22,  205,   80,   88,   35,   33,   28,   27,
 /*   430 */    31,  205,   21,  205,   25,   30,   39,   23,   37,   24,
 /*   440 */    36,   32,   29,  205,   51,  205,  205,  205,  205,   20,
 /*   450 */    34,    4,  205,  112,  205,  205,  205,  111,   84,  106,
 /*   460 */   205,  205,  205,    9,   44,    6,  205,  205,    1,  169,
 /*   470 */   205,   40,  205,  166,  167,  168,  170,  171,  172,   22,
 /*   480 */   205,   80,   88,   35,   33,   28,   27,   31,  205,  205,
 /*   490 */   205,   25,   30,   39,   23,   37,   24,  205,  205,   29,
 /*   500 */   263,  263,   35,   33,   28,   27,   31,  203,  205,  205,
 /*   510 */    25,   30,   39,   23,   37,   24,  205,  205,   29,  205,
 /*   520 */   205,  205,  205,  205,  205,  205,  205,  205,  205,    4,
 /*   530 */   205,  205,  205,  205,  205,  111,   84,  205,  205,  205,
 /*   540 */   205,    9,   44,    6,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,   65,   13,   14,   15,   16,   17,   18,   10,   11,
 /*    10 */    21,   16,   17,   18,   61,   62,   21,   19,   20,   22,
 /*    20 */    65,   24,   67,   26,   27,   28,   29,   29,   64,   65,
 /*    30 */    65,   34,   35,   36,   64,   65,   38,   39,   40,   41,
 /*    40 */     2,   43,   44,   45,   46,   47,   48,   12,   10,   11,
 /*    50 */    65,    2,   67,   63,   37,   38,   66,   19,   20,   10,
 /*    60 */    11,   18,   56,   57,   21,   65,   65,   29,   19,   20,
 /*    70 */    12,   63,   65,   35,   65,   40,   38,   39,   29,   41,
 /*    80 */    65,   43,   44,   45,   46,   47,   48,   38,   39,   31,
 /*    90 */    41,    2,   43,   44,   45,   46,   47,   48,    2,   10,
 /*   100 */    11,   65,   53,    5,    6,    7,    8,    9,   19,   20,
 /*   110 */    65,   13,   14,   15,   16,   17,   18,   65,   29,   21,
 /*   120 */    65,   65,   65,   37,   65,   65,   65,   38,   39,   65,
 /*   130 */    41,    2,   43,   44,   45,   46,   47,   48,   65,   10,
 /*   140 */    11,   65,   53,   65,   65,   49,   50,   51,   19,   20,
 /*   150 */    65,   55,   65,   65,   58,   65,   65,   65,   29,   65,
 /*   160 */    65,   33,   65,   37,   35,   65,   65,   38,   39,   65,
 /*   170 */    41,   65,   43,   44,   45,   46,   47,   48,    1,   65,
 /*   180 */     3,    4,    5,    6,    7,    8,    9,   68,   68,   68,
 /*   190 */    13,   14,   15,   16,   17,   18,   62,   62,   21,   68,
 /*   200 */    68,   62,   25,   29,    1,   40,    3,    4,    5,    6,
 /*   210 */     7,    8,    9,   54,   39,   52,   13,   14,   15,   16,
 /*   220 */    17,   18,   54,   35,   21,   21,   35,   40,   25,   39,
 /*   230 */     1,   38,    3,    4,    5,    6,    7,    8,    9,   25,
 /*   240 */    30,   52,   13,   14,   15,   16,   17,   18,   49,   29,
 /*   250 */    21,   25,   25,   69,   25,   69,    1,   69,    3,    4,
 /*   260 */     5,    6,    7,    8,    9,   69,   69,   69,   13,   14,
 /*   270 */    15,   16,   17,   18,   69,   69,   21,   69,   69,   69,
 /*   280 */    25,   69,    1,   69,    3,    4,    5,    6,    7,    8,
 /*   290 */     9,   69,   69,   69,   13,   14,   15,   16,   17,   18,
 /*   300 */    69,   69,   21,   69,   69,   69,   25,   69,    1,   69,
 /*   310 */     3,    4,    5,    6,    7,    8,    9,   69,   69,   69,
 /*   320 */    13,   14,   15,   16,   17,   18,   69,   69,   21,   69,
 /*   330 */    69,   69,   25,   69,    1,   69,    3,    4,    5,    6,
 /*   340 */     7,    8,    9,   69,   69,   69,   13,   14,   15,   16,
 /*   350 */    17,   18,   69,    1,   21,    3,    4,    5,    6,    7,
 /*   360 */     8,    9,   69,   69,   69,   13,   14,   15,   16,   17,
 /*   370 */    18,   22,   69,   21,   69,   69,   69,   28,   29,   69,
 /*   380 */    69,   32,   69,   34,   35,   36,   69,   69,   69,   69,
 /*   390 */    69,   69,   59,   69,   42,   69,    1,   69,    3,    4,
 /*   400 */     5,    6,    7,    8,    9,   69,   69,   69,   13,   14,
 /*   410 */    15,   16,   17,   18,   69,   69,   21,   69,   69,   69,
 /*   420 */    25,   69,    1,   69,    3,    4,    5,    6,    7,    8,
 /*   430 */     9,   69,    2,   69,   13,   14,   15,   16,   17,   18,
 /*   440 */    10,   11,   21,   69,   23,   69,   69,   69,   69,   19,
 /*   450 */    20,   22,   69,   24,   69,   69,   69,   28,   29,   29,
 /*   460 */    69,   69,   69,   34,   35,   36,   69,   69,   38,   39,
 /*   470 */    69,   41,   69,   43,   44,   45,   46,   47,   48,    1,
 /*   480 */    69,    3,    4,    5,    6,    7,    8,    9,   69,   69,
 /*   490 */    69,   13,   14,   15,   16,   17,   18,   69,   69,   21,
 /*   500 */     3,    4,    5,    6,    7,    8,    9,    0,   69,   69,
 /*   510 */    13,   14,   15,   16,   17,   18,   69,   69,   21,   69,
 /*   520 */    69,   69,   69,   69,   69,   69,   69,   69,   69,   22,
 /*   530 */    69,   69,   69,   69,   69,   28,   29,   69,   69,   69,
 /*   540 */    69,   34,   35,   36,
};
#define YY_SHIFT_USE_DFLT (-12)
#define YY_SHIFT_COUNT (113)
#define YY_SHIFT_MIN   (-11)
#define YY_SHIFT_MAX   (507)
static const short yy_shift_ofst[] = {
 /*     0 */   -12,   -2,  430,  430,  430,   -3,   38,   49,   89,  129,
 /*    10 */   430,  430,  430,  430,  430,  430,  430,  430,  430,  430,
 /*    20 */   430,  430,  430,  430,  430,  430,  430,  430,  430,  430,
 /*    30 */   430,  430,  430,  430,  430,  430,  430,  430,  430,  430,
 /*    40 */   430,  349,  429,  507,  128,  128,  128,  128,  128,  -12,
 /*    50 */   -12,  -12,  177,  203,  229,  255,  281,  307,  333,  352,
 /*    60 */   395,  421,  478,  478,  478,  497,  497,  497,  497,  497,
 /*    70 */   497,  497,  497,   98,   98,   98,   98,   98,  -11,  -11,
 /*    80 */    96,   -5,   -5,   -5,   17,   43,   43,   43,    6,   35,
 /*    90 */    58,   86,  174,  126,  165,  175,  159,  163,  168,  188,
 /*   100 */   204,  191,  204,  204,  187,  190,  193,  189,  199,  214,
 /*   110 */   210,  220,  226,  227,
};
#define YY_REDUCE_USE_DFLT (-65)
#define YY_REDUCE_COUNT (51)
#define YY_REDUCE_MIN   (-64)
#define YY_REDUCE_MAX   (139)
static const short yy_reduce_ofst[] = {
 /*     0 */   -47,  -45,  -36,  -15,  -30,  -10,  -64,  -35,    0,    1,
 /*    10 */     7,    9,   15,   36,   45,   52,   55,   56,   57,   59,
 /*    20 */    60,   61,   64,   73,   76,   78,   79,   85,   87,   88,
 /*    30 */    90,   91,   92,   94,   95,   97,  100,  101,  104,  106,
 /*    40 */   114,    8,    8,    8,  119,  120,  121,  131,  132,  134,
 /*    50 */   135,  139,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   204,  263,  263,  263,  263,  263,  263,  263,  263,  263,
 /*    10 */   263,  263,  263,  263,  263,  263,  263,  263,  263,  263,
 /*    20 */   263,  263,  263,  263,  263,  263,  263,  263,  263,  263,
 /*    30 */   263,  263,  263,  263,  263,  263,  263,  263,  263,  263,
 /*    40 */   263,  263,  263,  263,  261,  261,  261,  261,  261,  204,
 /*    50 */   204,  204,  263,  263,  263,  263,  263,  263,  263,  263,
 /*    60 */   263,  263,  224,  259,  225,  260,  253,  251,  246,  244,
 /*    70 */   255,  239,  254,  243,  245,  252,  250,  247,  235,  233,
 /*    80 */   263,  241,  256,  248,  263,  242,  258,  257,  263,  263,
 /*    90 */   263,  263,  263,  263,  263,  263,  263,  263,  263,  263,
 /*   100 */   240,  263,  249,  234,  263,  263,  220,  263,  263,  263,
 /*   110 */   263,  263,  263,  263,
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
  "COS",           "LIST_SHORTHAND",  "IF",            "THEN",        
  "ENDIF",         "SEMICOLON",     "ELSE",          "ELSEIF",      
  "FOR",           "IDENTIFIER",    "IN",            "DO",          
  "ENDDO",         "OF",            "WRITE",         "TIME",        
  "TRACE",         "ASSIGN",        "LSPAR",         "NUMTOKEN",    
  "RSPAR",         "LPAR",          "RPAR",          "BOOLTOKEN",   
  "IT",            "NOW",           "NULLTOKEN",     "STRTOKEN",    
  "TIMETOKEN",     "LIST",          "NUMBER",        "GREATER",     
  "THAN",          "OR",            "EQUAL",         "LESS",        
  "AFTER",         "BEFORE",        "WITHIN",        "TO",          
  "error",         "code",          "statementblock",  "statement",   
  "ifstatement",   "ex",            "elseif",        "exlist",      
  "of",          
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "code ::= statementblock",
 /*   1 */ "statementblock ::=",
 /*   2 */ "statementblock ::= statementblock statement",
 /*   3 */ "statement ::= IF ifstatement",
 /*   4 */ "ifstatement ::= ex THEN statementblock elseif",
 /*   5 */ "elseif ::= ENDIF SEMICOLON",
 /*   6 */ "elseif ::= ELSE statementblock ENDIF SEMICOLON",
 /*   7 */ "elseif ::= ELSEIF ifstatement",
 /*   8 */ "statement ::= FOR IDENTIFIER IN exlist DO statementblock ENDDO SEMICOLON",
 /*   9 */ "statement ::= WRITE ex SEMICOLON",
 /*  10 */ "statement ::= WRITE TIME of ex SEMICOLON",
 /*  11 */ "statement ::= TRACE ex SEMICOLON",
 /*  12 */ "statement ::= TRACE TIME of ex SEMICOLON",
 /*  13 */ "statement ::= IDENTIFIER ASSIGN ex SEMICOLON",
 /*  14 */ "statement ::= IDENTIFIER LSPAR NUMTOKEN RSPAR ASSIGN ex SEMICOLON",
 /*  15 */ "statement ::= TIME of IDENTIFIER ASSIGN ex SEMICOLON",
 /*  16 */ "ex ::= LPAR ex RPAR",
 /*  17 */ "ex ::= IDENTIFIER",
 /*  18 */ "ex ::= IDENTIFIER LSPAR NUMTOKEN RSPAR",
 /*  19 */ "ex ::= LSPAR RSPAR",
 /*  20 */ "ex ::= LSPAR exlist RSPAR",
 /*  21 */ "exlist ::= ex",
 /*  22 */ "exlist ::= exlist COMMA ex",
 /*  23 */ "ex ::= BOOLTOKEN",
 /*  24 */ "ex ::= IT",
 /*  25 */ "ex ::= NOW",
 /*  26 */ "ex ::= NUMTOKEN",
 /*  27 */ "ex ::= NULLTOKEN",
 /*  28 */ "ex ::= STRTOKEN",
 /*  29 */ "ex ::= TIMETOKEN",
 /*  30 */ "ex ::= COUNT ex",
 /*  31 */ "ex ::= COS ex",
 /*  32 */ "ex ::= FIRST ex",
 /*  33 */ "ex ::= ex IS LIST",
 /*  34 */ "ex ::= ex IS NOT LIST",
 /*  35 */ "ex ::= ex IS NUMBER",
 /*  36 */ "ex ::= NOT ex",
 /*  37 */ "ex ::= SIN ex",
 /*  38 */ "ex ::= ex AMPERSAND ex",
 /*  39 */ "ex ::= ex DIVIDE ex",
 /*  40 */ "ex ::= ex GT ex",
 /*  41 */ "ex ::= ex IS GREATER THAN ex",
 /*  42 */ "ex ::= ex GTEQ ex",
 /*  43 */ "ex ::= ex IS GREATER THAN OR EQUAL ex",
 /*  44 */ "ex ::= ex NEQ ex",
 /*  45 */ "ex ::= ex MINUS ex",
 /*  46 */ "ex ::= ex LIST_SHORTHAND ex",
 /*  47 */ "ex ::= ex LT ex",
 /*  48 */ "ex ::= ex IS LESS THAN ex",
 /*  49 */ "ex ::= ex LTEQ ex",
 /*  50 */ "ex ::= ex IS LESS THAN OR EQUAL ex",
 /*  51 */ "ex ::= ex OCCUR AFTER TIME of ex",
 /*  52 */ "ex ::= ex OCCUR BEFORE TIME of ex",
 /*  53 */ "ex ::= ex PLUS ex",
 /*  54 */ "ex ::= ex POWER ex",
 /*  55 */ "ex ::= ex TIMES ex",
 /*  56 */ "ex ::= ex WHERE ex",
 /*  57 */ "ex ::= ex IS WITHIN ex TO ex",
 /*  58 */ "of ::=",
 /*  59 */ "of ::= OF",
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
  { 61, 1 },
  { 62, 0 },
  { 62, 2 },
  { 63, 2 },
  { 64, 4 },
  { 66, 2 },
  { 66, 4 },
  { 66, 2 },
  { 63, 8 },
  { 63, 3 },
  { 63, 5 },
  { 63, 3 },
  { 63, 5 },
  { 63, 4 },
  { 63, 7 },
  { 63, 6 },
  { 65, 3 },
  { 65, 1 },
  { 65, 4 },
  { 65, 2 },
  { 65, 3 },
  { 67, 1 },
  { 67, 3 },
  { 65, 1 },
  { 65, 1 },
  { 65, 1 },
  { 65, 1 },
  { 65, 1 },
  { 65, 1 },
  { 65, 1 },
  { 65, 2 },
  { 65, 2 },
  { 65, 2 },
  { 65, 3 },
  { 65, 4 },
  { 65, 3 },
  { 65, 2 },
  { 65, 2 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 5 },
  { 65, 3 },
  { 65, 7 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 5 },
  { 65, 3 },
  { 65, 7 },
  { 65, 6 },
  { 65, 6 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 6 },
  { 68, 0 },
  { 68, 1 },
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
#line 241 "grammar.y"
{
	printf (cJSON_Print(yymsp[0].minor.yy0)); 
}
#line 1027 "grammar.c"
        break;
      case 1: /* statementblock ::= */
#line 251 "grammar.y"
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STATEMENTBLOCK");
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToObject(res, "statements", arg); 
	yymsp[1].minor.yy0 = res;
}
#line 1038 "grammar.c"
        break;
      case 2: /* statementblock ::= statementblock statement */
#line 260 "grammar.y"
{
	cJSON_AddItemToArray(cJSON_GetObjectItem ( yymsp[-1].minor.yy0, "statements"), yymsp[0].minor.yy0);
	yylhsminor.yy0 = yymsp[-1].minor.yy0;
}
#line 1046 "grammar.c"
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 3: /* statement ::= IF ifstatement */
      case 7: /* elseif ::= ELSEIF ifstatement */ yytestcase(yyruleno==7);
#line 270 "grammar.y"
{ yymsp[-1].minor.yy0 = yymsp[0].minor.yy0; }
#line 1053 "grammar.c"
        break;
      case 4: /* ifstatement ::= ex THEN statementblock elseif */
#line 273 "grammar.y"
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "IF");
	cJSON_AddItemToObject(res, "condition", yymsp[-3].minor.yy0); 
	cJSON_AddItemToObject(res, "thenbranch", (yymsp[-1].minor.yy0)); 
	cJSON_AddItemToObject(res, "elsebranch", (yymsp[0].minor.yy0));
	yylhsminor.yy0 = res;
}
#line 1065 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 5: /* elseif ::= ENDIF SEMICOLON */
#line 283 "grammar.y"
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STATEMENT_BLOCK");
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToObject(res, "statements", arg);
	yymsp[-1].minor.yy0 = res;
}
#line 1077 "grammar.c"
        break;
      case 6: /* elseif ::= ELSE statementblock ENDIF SEMICOLON */
#line 292 "grammar.y"
{ yymsp[-3].minor.yy0 = yymsp[-2].minor.yy0; }
#line 1082 "grammar.c"
        break;
      case 8: /* statement ::= FOR IDENTIFIER IN exlist DO statementblock ENDDO SEMICOLON */
#line 302 "grammar.y"
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "FOR");
	cJSON_AddItemToObject(res, "variable", yymsp[-6].minor.yy0); 
	cJSON_AddItemToObject(res, "list", yymsp[-4].minor.yy0); 
	cJSON_AddItemToObject(res, "do", yymsp[-2].minor.yy0);
	yymsp[-7].minor.yy0 = res;
}
#line 1094 "grammar.c"
        break;
      case 9: /* statement ::= WRITE ex SEMICOLON */
#line 323 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-2].minor.yy0 = res; 
}
#line 1104 "grammar.c"
        break;
      case 10: /* statement ::= WRITE TIME of ex SEMICOLON */
#line 335 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE_TIME"); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-4].minor.yy0 = res; 
}
#line 1114 "grammar.c"
        break;
      case 11: /* statement ::= TRACE ex SEMICOLON */
#line 347 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE"); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-2].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1125 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 12: /* statement ::= TRACE TIME of ex SEMICOLON */
#line 360 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE_TIME"); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-4].minor.yy0));
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1137 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 13: /* statement ::= IDENTIFIER ASSIGN ex SEMICOLON */
#line 373 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yylhsminor.yy0 = res; 
}
#line 1149 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 14: /* statement ::= IDENTIFIER LSPAR NUMTOKEN RSPAR ASSIGN ex SEMICOLON */
#line 386 "grammar.y"
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
#line 1164 "grammar.c"
  yymsp[-6].minor.yy0 = yylhsminor.yy0;
        break;
      case 15: /* statement ::= TIME of IDENTIFIER ASSIGN ex SEMICOLON */
#line 402 "grammar.y"
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN_TIME");
	cJSON_AddStringToObject(res, "varname", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddItemToObject(res, "arg", yymsp[-1].minor.yy0); 
	yymsp[-5].minor.yy0 = res; 
}
#line 1176 "grammar.c"
        break;
      case 16: /* ex ::= LPAR ex RPAR */
#line 416 "grammar.y"
{ 
	yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0; 
}
#line 1183 "grammar.c"
        break;
      case 17: /* ex ::= IDENTIFIER */
#line 422 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IDENTIFIER"); 
	cJSON_AddStringToObject(res, "name", getValue(yymsp[0].minor.yy0)); 
	cJSON_AddStringToObject(res, "line", getLine(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1194 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 18: /* ex ::= IDENTIFIER LSPAR NUMTOKEN RSPAR */
#line 432 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "LIST_ACCESS"); 
	cJSON_AddStringToObject(res, "name", getValue(yymsp[-3].minor.yy0)); 
	cJSON_AddStringToObject(res, "index", getValue(yymsp[-1].minor.yy0));
	cJSON_AddStringToObject(res, "line", getLine(yymsp[-3].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1207 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 19: /* ex ::= LSPAR RSPAR */
#line 447 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "EMPTYLIST");
	yymsp[-1].minor.yy0 = res; 
}
#line 1217 "grammar.c"
        break;
      case 20: /* ex ::= LSPAR exlist RSPAR */
#line 455 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "LIST");
	cJSON_AddItemToObject(res, "elements", yymsp[-1].minor.yy0); 
	yymsp[-2].minor.yy0 = res; 
}
#line 1227 "grammar.c"
        break;
      case 21: /* exlist ::= ex */
#line 464 "grammar.y"
{
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, yymsp[0].minor.yy0);
	yylhsminor.yy0 = arg;
}
#line 1236 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 22: /* exlist ::= exlist COMMA ex */
#line 472 "grammar.y"
{
	cJSON_AddItemToArray(yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
	yylhsminor.yy0 = yymsp[-2].minor.yy0;
}
#line 1245 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 23: /* ex ::= BOOLTOKEN */
#line 483 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "BOOLTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1256 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 24: /* ex ::= IT */
#line 491 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IT"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1266 "grammar.c"
        break;
      case 25: /* ex ::= NOW */
#line 498 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NOW"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1275 "grammar.c"
        break;
      case 26: /* ex ::= NUMTOKEN */
#line 505 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "NUMTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1285 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 27: /* ex ::= NULLTOKEN */
#line 513 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NULLTOKEN"); 
	yymsp[0].minor.yy0 = res; 
}
#line 1295 "grammar.c"
        break;
      case 28: /* ex ::= STRTOKEN */
#line 520 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STRTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1305 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 29: /* ex ::= TIMETOKEN */
#line 528 "grammar.y"
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "TIMETOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(yymsp[0].minor.yy0)); 
	yylhsminor.yy0 = res; 
}
#line 1316 "grammar.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 30: /* ex ::= COUNT ex */
#line 543 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("COUNT", yymsp[0].minor.yy0); }
#line 1322 "grammar.c"
        break;
      case 31: /* ex ::= COS ex */
#line 544 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("COS", yymsp[0].minor.yy0); }
#line 1327 "grammar.c"
        break;
      case 32: /* ex ::= FIRST ex */
#line 545 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("FIRST", yymsp[0].minor.yy0); }
#line 1332 "grammar.c"
        break;
      case 33: /* ex ::= ex IS LIST */
#line 546 "grammar.y"
{yylhsminor.yy0 = unary ("IS_LIST", yymsp[-2].minor.yy0); }
#line 1337 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 34: /* ex ::= ex IS NOT LIST */
#line 547 "grammar.y"
{yylhsminor.yy0 = unary ("IS_NOT_LIST", yymsp[-3].minor.yy0); }
#line 1343 "grammar.c"
  yymsp[-3].minor.yy0 = yylhsminor.yy0;
        break;
      case 35: /* ex ::= ex IS NUMBER */
#line 548 "grammar.y"
{yylhsminor.yy0 = unary ("IS_NUMBER", yymsp[-2].minor.yy0); }
#line 1349 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 36: /* ex ::= NOT ex */
#line 549 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("NOT", yymsp[0].minor.yy0); }
#line 1355 "grammar.c"
        break;
      case 37: /* ex ::= SIN ex */
#line 550 "grammar.y"
{yymsp[-1].minor.yy0 = unary ("SIN", yymsp[0].minor.yy0); }
#line 1360 "grammar.c"
        break;
      case 38: /* ex ::= ex AMPERSAND ex */
#line 556 "grammar.y"
{yylhsminor.yy0 = binary ("AMPERSAND", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1365 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 39: /* ex ::= ex DIVIDE ex */
#line 557 "grammar.y"
{yylhsminor.yy0 = binary ("DIVIDE", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1371 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 40: /* ex ::= ex GT ex */
#line 558 "grammar.y"
{yylhsminor.yy0 = binary ("GT", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1377 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 41: /* ex ::= ex IS GREATER THAN ex */
#line 559 "grammar.y"
{yylhsminor.yy0 = binary ("GT", yymsp[-4].minor.yy0, yymsp[0].minor.yy0); }
#line 1383 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 42: /* ex ::= ex GTEQ ex */
#line 560 "grammar.y"
{yylhsminor.yy0 = binary ("GTEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1389 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 43: /* ex ::= ex IS GREATER THAN OR EQUAL ex */
#line 561 "grammar.y"
{yylhsminor.yy0 = binary ("GTEQ", yymsp[-6].minor.yy0, yymsp[0].minor.yy0); }
#line 1395 "grammar.c"
  yymsp[-6].minor.yy0 = yylhsminor.yy0;
        break;
      case 44: /* ex ::= ex NEQ ex */
#line 562 "grammar.y"
{yylhsminor.yy0 = binary ("NEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1401 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 45: /* ex ::= ex MINUS ex */
#line 563 "grammar.y"
{yylhsminor.yy0 = binary ("MINUS", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1407 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 46: /* ex ::= ex LIST_SHORTHAND ex */
#line 564 "grammar.y"
{yylhsminor.yy0 = binary ("LIST_SHORTHAND", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1413 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 47: /* ex ::= ex LT ex */
#line 565 "grammar.y"
{yylhsminor.yy0 = binary ("LT", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1419 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 48: /* ex ::= ex IS LESS THAN ex */
#line 566 "grammar.y"
{yylhsminor.yy0 = binary ("LT", yymsp[-4].minor.yy0, yymsp[0].minor.yy0); }
#line 1425 "grammar.c"
  yymsp[-4].minor.yy0 = yylhsminor.yy0;
        break;
      case 49: /* ex ::= ex LTEQ ex */
#line 567 "grammar.y"
{yylhsminor.yy0 = binary ("LTEQ", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1431 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 50: /* ex ::= ex IS LESS THAN OR EQUAL ex */
#line 568 "grammar.y"
{yylhsminor.yy0 = binary ("LTEQ", yymsp[-6].minor.yy0, yymsp[0].minor.yy0); }
#line 1437 "grammar.c"
  yymsp[-6].minor.yy0 = yylhsminor.yy0;
        break;
      case 51: /* ex ::= ex OCCUR AFTER TIME of ex */
#line 569 "grammar.y"
{yylhsminor.yy0 = binary ("OCCUR_AFTER", yymsp[-5].minor.yy0, yymsp[0].minor.yy0); }
#line 1443 "grammar.c"
  yymsp[-5].minor.yy0 = yylhsminor.yy0;
        break;
      case 52: /* ex ::= ex OCCUR BEFORE TIME of ex */
#line 570 "grammar.y"
{yylhsminor.yy0 = binary ("OCCUR_BEFORE", yymsp[-5].minor.yy0, yymsp[0].minor.yy0); }
#line 1449 "grammar.c"
  yymsp[-5].minor.yy0 = yylhsminor.yy0;
        break;
      case 53: /* ex ::= ex PLUS ex */
#line 571 "grammar.y"
{yylhsminor.yy0 = binary ("PLUS", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1455 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 54: /* ex ::= ex POWER ex */
#line 572 "grammar.y"
{yylhsminor.yy0 = binary ("POWER", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1461 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 55: /* ex ::= ex TIMES ex */
#line 573 "grammar.y"
{yylhsminor.yy0 = binary ("TIMES", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1467 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 56: /* ex ::= ex WHERE ex */
#line 574 "grammar.y"
{yylhsminor.yy0 = binary ("WHERE", yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1473 "grammar.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 57: /* ex ::= ex IS WITHIN ex TO ex */
#line 580 "grammar.y"
{yylhsminor.yy0 = ternary ("IS_WITHIN", yymsp[-5].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
#line 1479 "grammar.c"
  yymsp[-5].minor.yy0 = yylhsminor.yy0;
        break;
      default:
      /* (58) of ::= */ yytestcase(yyruleno==58);
      /* (59) of ::= OF */ yytestcase(yyruleno==59);
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
#line 211 "grammar.y"

  printf ("{\"error\" : true, \"message\": \"Syntax Error: Compiler reports unexpected token \\\"%s\\\" of type \\\"%s\\\" in line %s\"}\n", curtoken, curtype, linenumber);
  exit(0);
#line 1545 "grammar.c"
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
	if (strcmp(token, "DO") == 0) return DO;
	if (strcmp(token, "DIVIDE") == 0) return DIVIDE;
	if (strcmp(token, "EQUAL") == 0) return EQUAL;
	if (strcmp(token, "ELSE") == 0) return ELSE;
	if (strcmp(token, "ELSEIF") == 0) return ELSEIF;
	if (strcmp(token, "ENDIF") == 0) return ENDIF;
	if (strcmp(token, "ENDDO") == 0) return ENDDO;
	if (strcmp(token, "FIRST") == 0) return FIRST;
	if (strcmp(token, "FOR") == 0) return FOR;
	if (strcmp(token, "GREATER") == 0) return GREATER;
	if (strcmp(token, "GT") == 0) return GT;
	if (strcmp(token, "GTEQ") == 0) return GTEQ;
	if (strcmp(token, "IDENTIFIER") == 0) return IDENTIFIER;
	if (strcmp(token, "IF") == 0) return IF;
	if (strcmp(token, "IN") == 0) return IN;
	if (strcmp(token, "IS") == 0) return IS;
	if (strcmp(token, "IT") == 0) return IT;
	if (strcmp(token, "LPAR") == 0) return LPAR; 
	if (strcmp(token, "RPAR") == 0) return RPAR;
	if (strcmp(token, "LSPAR") == 0) return LSPAR;
	if (strcmp(token, "RSPAR") == 0) return RSPAR;
	if (strcmp(token, "LESS") == 0) return LESS;
	if (strcmp(token, "LIST") == 0) return LIST;
	if (strcmp(token, "LIST_SHORTHAND") == 0) return LIST_SHORTHAND;
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
	if (strcmp(token, "THEN") == 0) return THEN;
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

#line 1933 "grammar.c"
