%locations // -*- C++ -*-
%pure-parser
%error-verbose

%{
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

%}

%union yystype {
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
}

%token	STRICT GRAPH DIGRAPH NODE EDGE SUBGRAPH

%token	SEMI LBRACE RBRACE LBRACK RBRACK LPAREN RPAREN COMMA EQUAL COLON AT

%token <str>	EDGEOP
%token <str>	ID

%type <str>			nodeid
%type <graph_t>			graph
%type <stmtlist_t>		stmtlist
%type <stmt_t>			stmt
%type <attrstmt_t>		attrstmt
%type <attrlist_t>		attrlist alist
%type <attrassignment_t>	attrassignment
%type <nodestmt_t>		nodestmt
%type <edgestmt_t>		edgestmt
%type <edgerhs_t>		edgeRHS

%destructor { delete $$; }	ID EDGEOP graph stmt attrstmt attrassignment nodestmt edgestmt edgeRHS
%destructor { for (std::list<Stmt*>::iterator i = $$->begin(); i != $$->end(); ++i) delete *i; delete $$; } stmtlist
%destructor { for (std::list<AttrAssignment*>::iterator i = $$->begin(); i != $$->end(); ++i) delete *i; delete $$; } attrlist

%start	graph

%%

graph:		  strict graphtype ID LBRACE stmtlist RBRACE	{ the_graph = new Graph(@$, $3, $5); }
		;

strict:		  STRICT					{}
		|						{}
		;

graphtype:	  DIGRAPH					{}
		| GRAPH						{}
		;

stmtlist:	 						{ $$ = new std::list<Stmt*>; }
		| stmtlist stmt					{ $1->push_back($2); $$ = $1; }
		| stmtlist stmt SEMI				{ $1->push_back($2); $$ = $1; }
		| stmtlist error SEMI				{ std::cerr << "REC" << std::endl; $$ = $1; }
		;

stmt:		  attrstmt					{ $$ = $1; }
		| nodestmt					{ $$ = $1; }
		| edgestmt					{ $$ = $1; }
		| subgraphstmt					{}
		| attrassignment				{}
		;

attrstmt:	  GRAPH attrlist				{ $$ = new GraphAttrStmt(@$, $2); }
		| NODE attrlist					{ $$ = new NodeAttrStmt(@$, $2); }
		| EDGE attrlist					{ $$ = new EdgeAttrStmt(@$, $2); }
		;

attrlist:	  LBRACK alist RBRACK				{ $$ = $2; }
		| LBRACK RBRACK					{ $$ = new std::list<AttrAssignment*>; }
		;

alist:		  attrassignment				{ $$ = new std::list<AttrAssignment*>(1, $1); }
		| attrassignment COMMA alist			{ $3->push_front($1); $$ = $3; }
		| attrassignment alist				{ $2->push_front($1); $$ = $2; }
		;

attrassignment:	  ID EQUAL ID					{ $$ = new AttrAssignment(@$, $1, $3); }
		;

    nodestmt:	  nodeid					{ $$ = new NodeStmt(@$, $1, new std::list<AttrAssignment*>); }
		| nodeid attrlist				{ $$ = new NodeStmt(@$, $1, $2); }
		;

nodeid:		  ID						{ $$ = $1; }
		| ID port					{ $$ = $1; }
		;

port:		  portlocation					{}
		| portangle					{}
		| portlocation portangle			{}
		| portangle portlocation			{}
		;

portlocation:	  COLON ID					{ delete $2; }
		| COLON ID LPAREN ID COMMA ID RPAREN		{ delete $2; delete $4; delete $6; }
		;

portangle:	  AT ID						{ delete $2; }
		;

edgestmt:	  nodeid edgeRHS				{ $$ = new EdgeStmt(@$, $1, $2, new std::list<AttrAssignment*>); }
		| nodeid edgeRHS attrlist			{ $$ = new EdgeStmt(@$, $1, $2, $3); }
		| subgraph edgeRHS				{}
		| subgraph edgeRHS attrlist			{}
		;

edgeRHS:	  EDGEOP nodeid					{ $$ = new EdgeRHS(@$, *$1 == "--" ? EdgeRHS::edge : EdgeRHS::arc, $2, 0); delete $1; }
		| EDGEOP nodeid edgeRHS				{ $$ = new EdgeRHS(@$, *$1 == "--" ? EdgeRHS::edge : EdgeRHS::arc, $2, $3); delete $1; }
		;

subgraphstmt:	  SUBGRAPH ID LBRACE stmtlist RBRACE		{ delete $2; delete $4; }
		| LBRACE stmtlist RBRACE			{ delete $2; }
		| SUBGRAPH ID SEMI				{ delete $2; }
		;

subgraph:	  SUBGRAPH ID					{ delete $2; }
		| LBRACE stmtlist RBRACE			{ delete $2; }
		| SUBGRAPH ID LBRACE stmtlist RBRACE		{ delete $2; delete $4; }
		;

%%

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
