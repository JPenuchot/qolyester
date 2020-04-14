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
/* Line 1285 of yacc.c.  */
#line 92 "parsedot.hh"
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




