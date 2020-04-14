/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRICT = 258,
     GRAPH = 259,
     DIGRAPH = 260,
     NODE = 261,
     EDGE = 262,
     SUBGRAPH = 263,
     SEMI = 264,
     LBRACE = 265,
     RBRACE = 266,
     LBRACK = 267,
     RBRACK = 268,
     LPAREN = 269,
     RPAREN = 270,
     COMMA = 271,
     EQUAL = 272,
     COLON = 273,
     AT = 274,
     EDGEOP = 275,
     ID = 276
   };
#endif
#define STRICT 258
#define GRAPH 259
#define DIGRAPH 260
#define NODE 261
#define EDGE 262
#define SUBGRAPH 263
#define SEMI 264
#define LBRACE 265
#define RBRACE 266
#define LBRACK 267
#define RBRACK 268
#define LPAREN 269
#define RPAREN 270
#define COMMA 271
#define EQUAL 272
#define COLON 273
#define AT 274
#define EDGEOP 275
#define ID 276




/* Copy the first part of user declarations.  */
#line 5 "../../src/switch/prs/parsedot.yy"

#include <list>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdarg>
#include <cassert>
#include "config.hh"

#ifdef QOLYESTER_ENABLE_DEBUG
# define DEBUG
#else
# define NDEBUG
#endif

#include "prs/location.hh"
#include "prs/visitor.hh"
#include "prs/data.hh"
#include "utl/log.hh"

namespace olsr {

#ifdef DEBUG
  extern bool			debugtrace;
#endif
  extern unsigned		current_log_level;
  extern debug_ostream_t	debug;

  using utl::up;
  using utl::down;
  using utl::reset;

  namespace prs {

    namespace dot {

#include "parsedot.hh"

    } // namespace dot

  } // namespace prs

} // namespace olsr

namespace olsr {

  namespace prs {

    namespace dot {

      extern int yylex(YYSTYPE*, YYLTYPE*);

      YYLTYPE yylloc;

#ifdef DEBUG
# define YYDEBUG 1
#endif
#define YYERROR_VERBOSE 1
#define YYPRINT(file, type, value) yyprint(yylloc, type, value)
#define YYFPRINTF(file, format, args...) yyprintf(file, format, ## args)
#define yyerror(Msg) parse_error(yylloc, Msg);

      extern void parse_error(const Location& loc, const char* m);

#define YYLLOC_DEFAULT(Current, Rhs, N) \
  do { \
    (Current).begin = (Rhs)[1].begin; \
    (Current).end = (Rhs)[(N)].end; \
  } while (0)

#ifdef DEBUG
      static void	yyprint(const Location& loc, int type,
				const yystype value);

      static void	yyprintf(FILE *, const char* format, ...);
#endif

      static Graph*	the_graph;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 88 "../../src/switch/prs/parsedot.yy"
typedef union yystype {
  LocString*			str;
  Graph*			graph_t;
  std::list<Stmt*>*		stmtlist_t;
  Stmt*				stmt_t;
  AttrStmt*			attrstmt_t;
  std::list<AttrAssignment*>*	attrlist_t;
  AttrAssignment*		attrassignment_t;
  NodeStmt*			nodestmt_t;
  EdgeStmt*			edgestmt_t;
  EdgeRHS*			edgerhs_t;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 214 "libparsedot_a-parsedot.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 238 "libparsedot_a-parsedot.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYLTYPE_IS_TRIVIAL) && YYLTYPE_IS_TRIVIAL \
             && defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   96

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  19
/* YYNRULES -- Number of rules. */
#define YYNRULES  47
/* YYNRULES -- Number of states. */
#define YYNSTATES  73

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,    10,    12,    13,    15,    17,    18,    21,
      25,    29,    31,    33,    35,    37,    39,    42,    45,    48,
      52,    55,    57,    61,    64,    68,    70,    73,    75,    78,
      80,    82,    85,    88,    91,    99,   102,   105,   109,   112,
     116,   119,   123,   129,   133,   137,   140,   144
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      23,     0,    -1,    24,    25,    21,    10,    26,    11,    -1,
       3,    -1,    -1,     5,    -1,     4,    -1,    -1,    26,    27,
      -1,    26,    27,     9,    -1,    26,     1,     9,    -1,    28,
      -1,    32,    -1,    37,    -1,    39,    -1,    31,    -1,     4,
      29,    -1,     6,    29,    -1,     7,    29,    -1,    12,    30,
      13,    -1,    12,    13,    -1,    31,    -1,    31,    16,    30,
      -1,    31,    30,    -1,    21,    17,    21,    -1,    33,    -1,
      33,    29,    -1,    21,    -1,    21,    34,    -1,    35,    -1,
      36,    -1,    35,    36,    -1,    36,    35,    -1,    18,    21,
      -1,    18,    21,    14,    21,    16,    21,    15,    -1,    19,
      21,    -1,    33,    38,    -1,    33,    38,    29,    -1,    40,
      38,    -1,    40,    38,    29,    -1,    20,    33,    -1,    20,
      33,    38,    -1,     8,    21,    10,    26,    11,    -1,    10,
      26,    11,    -1,     8,    21,     9,    -1,     8,    21,    -1,
      10,    26,    11,    -1,     8,    21,    10,    26,    11,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,   127,   127,   130,   131,   134,   135,   138,   139,   140,
     141,   144,   145,   146,   147,   148,   151,   152,   153,   156,
     157,   160,   161,   162,   165,   168,   169,   172,   173,   176,
     177,   178,   179,   182,   183,   186,   189,   190,   191,   192,
     195,   196,   199,   200,   201,   204,   205,   206
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRICT", "GRAPH", "DIGRAPH", "NODE",
  "EDGE", "SUBGRAPH", "SEMI", "LBRACE", "RBRACE", "LBRACK", "RBRACK",
  "LPAREN", "RPAREN", "COMMA", "EQUAL", "COLON", "AT", "EDGEOP", "ID",
  "$accept", "graph", "strict", "graphtype", "stmtlist", "stmt",
  "attrstmt", "attrlist", "alist", "attrassignment", "nodestmt", "nodeid",
  "port", "portlocation", "portangle", "edgestmt", "edgeRHS",
  "subgraphstmt", "subgraph", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    22,    23,    24,    24,    25,    25,    26,    26,    26,
      26,    27,    27,    27,    27,    27,    28,    28,    28,    29,
      29,    30,    30,    30,    31,    32,    32,    33,    33,    34,
      34,    34,    34,    35,    35,    36,    37,    37,    37,    37,
      38,    38,    39,    39,    39,    40,    40,    40
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     6,     1,     0,     1,     1,     0,     2,     3,
       3,     1,     1,     1,     1,     1,     2,     2,     2,     3,
       2,     1,     3,     2,     3,     1,     2,     1,     2,     1,
       1,     2,     2,     2,     7,     2,     2,     3,     2,     3,
       2,     3,     5,     3,     3,     2,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     3,     0,     0,     1,     6,     5,     0,     0,     7,
       0,     0,     0,     0,     0,     0,     7,     2,    27,     8,
      11,    15,    12,    25,    13,    14,     0,    10,     0,    16,
      17,    18,    45,     0,     0,     0,     0,    28,    29,    30,
       9,     0,    26,    36,    38,    20,     0,     0,    21,    44,
       7,    43,    24,    33,    35,    31,    32,    27,    40,    37,
      39,    19,     0,    23,     0,     0,    41,    22,    42,     0,
       0,     0,    34
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,     3,     7,    10,    19,    20,    29,    47,    21,
      22,    23,    37,    38,    39,    24,    43,    25,    26
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -44
static const yysigned_char yypact[] =
{
      20,   -44,    25,     2,   -44,   -44,   -44,     5,    28,   -44,
       7,    30,    29,    29,    29,    32,   -44,   -44,    41,    40,
     -44,   -44,   -44,     4,   -44,   -44,    42,   -44,     8,   -44,
     -44,   -44,     0,    36,    35,    43,    45,   -44,    48,    50,
     -44,    49,   -44,    29,    29,   -44,    46,    56,     6,   -44,
     -44,    51,   -44,    47,   -44,   -44,   -44,    12,    42,   -44,
     -44,   -44,    52,   -44,    44,    53,   -44,   -44,    55,    60,
      57,    62,   -44
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -44,   -44,   -44,   -44,   -15,   -44,   -44,   -11,   -43,   -28,
     -44,    31,   -44,    54,    58,   -44,   -22,   -44,   -44
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -48
static const yysigned_char yytable[] =
{
      48,    33,    30,    31,    44,    63,     5,     6,    11,    49,
      50,    12,    42,    13,    14,    15,    28,    16,    17,    67,
      48,    45,    62,     1,    41,     4,     8,    46,    18,    46,
      35,    36,    59,    60,    48,    64,    66,    11,     9,    27,
      12,    28,    13,    14,    15,    11,    16,    51,    12,    40,
      13,    14,    15,    32,    16,    68,    52,    18,    34,    35,
      36,    65,    41,    34,    53,    18,    54,    36,    35,    61,
      57,   -46,    58,    46,    69,   -47,    70,    72,    71,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    56,     0,     0,    55
};

static const yysigned_char yycheck[] =
{
      28,    16,    13,    14,    26,    48,     4,     5,     1,     9,
      10,     4,    23,     6,     7,     8,    12,    10,    11,    62,
      48,    13,    16,     3,    20,     0,    21,    21,    21,    21,
      18,    19,    43,    44,    62,    50,    58,     1,    10,     9,
       4,    12,     6,     7,     8,     1,    10,    11,     4,     9,
       6,     7,     8,    21,    10,    11,    21,    21,    17,    18,
      19,    14,    20,    17,    21,    21,    21,    19,    18,    13,
      21,    20,    41,    21,    21,    20,    16,    15,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,    23,    24,     0,     4,     5,    25,    21,    10,
      26,     1,     4,     6,     7,     8,    10,    11,    21,    27,
      28,    31,    32,    33,    37,    39,    40,     9,    12,    29,
      29,    29,    21,    26,    17,    18,    19,    34,    35,    36,
       9,    20,    29,    38,    38,    13,    21,    30,    31,     9,
      10,    11,    21,    21,    21,    36,    35,    21,    33,    29,
      29,    13,    16,    30,    26,    14,    38,    30,    11,    21,
      16,    21,    15
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value, Location);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yytype, yyvaluep, yylocationp)
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  switch (yytype)
    {
      case 20: /* EDGEOP */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->str; };
#line 891 "libparsedot_a-parsedot.cc"
        break;
      case 21: /* ID */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->str; };
#line 896 "libparsedot_a-parsedot.cc"
        break;
      case 23: /* graph */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->graph_t; };
#line 901 "libparsedot_a-parsedot.cc"
        break;
      case 26: /* stmtlist */
#line 120 "../../src/switch/prs/parsedot.yy"
        { for (std::list<Stmt*>::iterator i = yyvaluep->stmtlist_t->begin(); i != yyvaluep->stmtlist_t->end(); ++i) delete *i; delete yyvaluep->stmtlist_t; };
#line 906 "libparsedot_a-parsedot.cc"
        break;
      case 27: /* stmt */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->stmt_t; };
#line 911 "libparsedot_a-parsedot.cc"
        break;
      case 28: /* attrstmt */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->attrstmt_t; };
#line 916 "libparsedot_a-parsedot.cc"
        break;
      case 29: /* attrlist */
#line 121 "../../src/switch/prs/parsedot.yy"
        { for (std::list<AttrAssignment*>::iterator i = yyvaluep->attrlist_t->begin(); i != yyvaluep->attrlist_t->end(); ++i) delete *i; delete yyvaluep->attrlist_t; };
#line 921 "libparsedot_a-parsedot.cc"
        break;
      case 31: /* attrassignment */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->attrassignment_t; };
#line 926 "libparsedot_a-parsedot.cc"
        break;
      case 32: /* nodestmt */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->nodestmt_t; };
#line 931 "libparsedot_a-parsedot.cc"
        break;
      case 37: /* edgestmt */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->edgestmt_t; };
#line 936 "libparsedot_a-parsedot.cc"
        break;
      case 38: /* edgeRHS */
#line 119 "../../src/switch/prs/parsedot.yy"
        { delete yyvaluep->edgerhs_t; };
#line 941 "libparsedot_a-parsedot.cc"
        break;

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  YYLTYPE *yylerrsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
  *++yylsp = yylloc;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, yylsp - yylen, yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 127 "../../src/switch/prs/parsedot.yy"
    { the_graph = new Graph(yyloc, yyvsp[-3].str, yyvsp[-1].stmtlist_t); }
    break;

  case 3:
#line 130 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 4:
#line 131 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 5:
#line 134 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 6:
#line 135 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 7:
#line 138 "../../src/switch/prs/parsedot.yy"
    { yyval.stmtlist_t = new std::list<Stmt*>; }
    break;

  case 8:
#line 139 "../../src/switch/prs/parsedot.yy"
    { yyvsp[-1].stmtlist_t->push_back(yyvsp[0].stmt_t); yyval.stmtlist_t = yyvsp[-1].stmtlist_t; }
    break;

  case 9:
#line 140 "../../src/switch/prs/parsedot.yy"
    { yyvsp[-2].stmtlist_t->push_back(yyvsp[-1].stmt_t); yyval.stmtlist_t = yyvsp[-2].stmtlist_t; }
    break;

  case 10:
#line 141 "../../src/switch/prs/parsedot.yy"
    { std::cerr << "REC" << std::endl; yyval.stmtlist_t = yyvsp[-2].stmtlist_t; }
    break;

  case 11:
#line 144 "../../src/switch/prs/parsedot.yy"
    { yyval.stmt_t = yyvsp[0].attrstmt_t; }
    break;

  case 12:
#line 145 "../../src/switch/prs/parsedot.yy"
    { yyval.stmt_t = yyvsp[0].nodestmt_t; }
    break;

  case 13:
#line 146 "../../src/switch/prs/parsedot.yy"
    { yyval.stmt_t = yyvsp[0].edgestmt_t; }
    break;

  case 14:
#line 147 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 15:
#line 148 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 16:
#line 151 "../../src/switch/prs/parsedot.yy"
    { yyval.attrstmt_t = new GraphAttrStmt(yyloc, yyvsp[0].attrlist_t); }
    break;

  case 17:
#line 152 "../../src/switch/prs/parsedot.yy"
    { yyval.attrstmt_t = new NodeAttrStmt(yyloc, yyvsp[0].attrlist_t); }
    break;

  case 18:
#line 153 "../../src/switch/prs/parsedot.yy"
    { yyval.attrstmt_t = new EdgeAttrStmt(yyloc, yyvsp[0].attrlist_t); }
    break;

  case 19:
#line 156 "../../src/switch/prs/parsedot.yy"
    { yyval.attrlist_t = yyvsp[-1].attrlist_t; }
    break;

  case 20:
#line 157 "../../src/switch/prs/parsedot.yy"
    { yyval.attrlist_t = new std::list<AttrAssignment*>; }
    break;

  case 21:
#line 160 "../../src/switch/prs/parsedot.yy"
    { yyval.attrlist_t = new std::list<AttrAssignment*>(1, yyvsp[0].attrassignment_t); }
    break;

  case 22:
#line 161 "../../src/switch/prs/parsedot.yy"
    { yyvsp[0].attrlist_t->push_front(yyvsp[-2].attrassignment_t); yyval.attrlist_t = yyvsp[0].attrlist_t; }
    break;

  case 23:
#line 162 "../../src/switch/prs/parsedot.yy"
    { yyvsp[0].attrlist_t->push_front(yyvsp[-1].attrassignment_t); yyval.attrlist_t = yyvsp[0].attrlist_t; }
    break;

  case 24:
#line 165 "../../src/switch/prs/parsedot.yy"
    { yyval.attrassignment_t = new AttrAssignment(yyloc, yyvsp[-2].str, yyvsp[0].str); }
    break;

  case 25:
#line 168 "../../src/switch/prs/parsedot.yy"
    { yyval.nodestmt_t = new NodeStmt(yyloc, yyvsp[0].str, new std::list<AttrAssignment*>); }
    break;

  case 26:
#line 169 "../../src/switch/prs/parsedot.yy"
    { yyval.nodestmt_t = new NodeStmt(yyloc, yyvsp[-1].str, yyvsp[0].attrlist_t); }
    break;

  case 27:
#line 172 "../../src/switch/prs/parsedot.yy"
    { yyval.str = yyvsp[0].str; }
    break;

  case 28:
#line 173 "../../src/switch/prs/parsedot.yy"
    { yyval.str = yyvsp[-1].str; }
    break;

  case 29:
#line 176 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 30:
#line 177 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 31:
#line 178 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 32:
#line 179 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 33:
#line 182 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[0].str; }
    break;

  case 34:
#line 183 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[-5].str; delete yyvsp[-3].str; delete yyvsp[-1].str; }
    break;

  case 35:
#line 186 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[0].str; }
    break;

  case 36:
#line 189 "../../src/switch/prs/parsedot.yy"
    { yyval.edgestmt_t = new EdgeStmt(yyloc, yyvsp[-1].str, yyvsp[0].edgerhs_t, new std::list<AttrAssignment*>); }
    break;

  case 37:
#line 190 "../../src/switch/prs/parsedot.yy"
    { yyval.edgestmt_t = new EdgeStmt(yyloc, yyvsp[-2].str, yyvsp[-1].edgerhs_t, yyvsp[0].attrlist_t); }
    break;

  case 38:
#line 191 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 39:
#line 192 "../../src/switch/prs/parsedot.yy"
    {}
    break;

  case 40:
#line 195 "../../src/switch/prs/parsedot.yy"
    { yyval.edgerhs_t = new EdgeRHS(yyloc, *yyvsp[-1].str == "--" ? EdgeRHS::edge : EdgeRHS::arc, yyvsp[0].str, 0); delete yyvsp[-1].str; }
    break;

  case 41:
#line 196 "../../src/switch/prs/parsedot.yy"
    { yyval.edgerhs_t = new EdgeRHS(yyloc, *yyvsp[-2].str == "--" ? EdgeRHS::edge : EdgeRHS::arc, yyvsp[-1].str, yyvsp[0].edgerhs_t); delete yyvsp[-2].str; }
    break;

  case 42:
#line 199 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[-3].str; delete yyvsp[-1].stmtlist_t; }
    break;

  case 43:
#line 200 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[-1].stmtlist_t; }
    break;

  case 44:
#line 201 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[-1].str; }
    break;

  case 45:
#line 204 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[0].str; }
    break;

  case 46:
#line 205 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[-1].stmtlist_t; }
    break;

  case 47:
#line 206 "../../src/switch/prs/parsedot.yy"
    { delete yyvsp[-3].str; delete yyvsp[-1].stmtlist_t; }
    break;


    }

/* Line 1010 of yacc.c.  */
#line 1485 "libparsedot_a-parsedot.cc"

  yyvsp -= yylen;
  yyssp -= yylen;
  yylsp -= yylen;

  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }

  yylerrsp = yylsp;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp, yylsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	  *++yylerrsp = yylloc;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  yylerrsp = yylsp;
  *++yylerrsp = yyloc;
  yylsp -= yylen;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
  YYLLOC_DEFAULT (yyloc, yylsp, yylerrsp - yylsp);
  *++yylsp = yyloc;

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 209 "../../src/switch/prs/parsedot.yy"


      void
      parse_error(const Location& loc, const char* m)
      {
        std::ostringstream	s;
        s << "Parse error at " << loc << ": " << m;
        throw std::runtime_error(s.str());
      }

#ifdef DEBUG
      static void
      yyprint(const Location&, int type, const yystype value)
      {
        switch(type) {
        case EDGEOP:
        case ID:
          debug << up << *value.str << down;
        }
      }

      static void
      yyprintf(FILE*, const char* format, ...)
      {
	va_list	list;
	va_start(list, format);
	int	size = 1024;
	while (true) {
	  char	buffer[size];
	  if (vsnprintf(buffer, size, format, list) < size) {
	    debug << up << buffer << down;
	    break;
	  }
	  size *= 2;
	}
	va_end(list);
      }
#endif // !DEBUG

      extern void scan_open(const std::string&);
      extern void scan_close();

      Graph*
      parse(const std::string& name) {
        scan_open(name);
	the_graph = 0;
#ifdef DEBUG
	yydebug = current_log_level >= 5 || debugtrace;
#endif
        yyparse();
	debug << std::flush;
        scan_close();
	return the_graph;
      }

    } // namespace dot

  } // namespace prs

} // namespace olsr

