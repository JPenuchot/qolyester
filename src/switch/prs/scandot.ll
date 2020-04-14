%option nounput
%option noyywrap

%top {
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <list>
#include "prs/location.hh"
#include "prs/visitor.hh"
#include "prs/data.hh"

namespace olsr <%

  namespace prs <%

    namespace dot <%

#include "parsedot.hh"

}


%{

#define YY_NEVER_INTERACTIVE 1

    static std::string	current_file;
    static int		comment_level = 0;

    extern int yylex(YYSTYPE*, YYLTYPE*);
    extern void parse_error(const Location& loc, const char* m);

#define YY_USER_INIT                              \
  do {                                            \
    yylloc->begin.filename = current_file;         \
    yylloc->end.filename = current_file;           \
  } while (0)

    extern YYLTYPE yylloc;

%}

%x STATE_COMMENT

STRINGCHAR	[^"\\]
DIGIT		[[:digit:]]
SYMBOLHEAD	[[:alpha:]_]
SYMBOLTAIL	[[:alpha:]_[:digit:]]

ID		({SYMBOLHEAD}{SYMBOLTAIL}*)|{DIGIT}+|{DIGIT}*\.{DIGIT}+|\"({STRINGCHAR}|\\\")*\"

EDGEOP		(->|--)

%%

strict		yylloc->word(yyleng); return STRICT;
graph		yylloc->word(yyleng); return GRAPH;
digraph		yylloc->word(yyleng); return DIGRAPH;
node		yylloc->word(yyleng); return NODE;
edge		yylloc->word(yyleng); return EDGE;
subgraph	yylloc->word(yyleng); return SUBGRAPH;

";"		yylloc->word(yyleng); return SEMI;
"{"		yylloc->word(yyleng); return LBRACE;
"}"		yylloc->word(yyleng); return RBRACE;
"["		yylloc->word(yyleng); return LBRACK;
"]"		yylloc->word(yyleng); return RBRACK;
"("		yylloc->word(yyleng); return LPAREN;
")"		yylloc->word(yyleng); return RPAREN;
","		yylloc->word(yyleng); return COMMA;
"="		yylloc->word(yyleng); return EQUAL;
":"		yylloc->word(yyleng); return COLON;
"@"		yylloc->word(yyleng); return AT;

{ID}		{ 
	 	  yylloc->word(yyleng);
		  if (yytext[0] == '"') {
		    yylval->str = new LocString(*yylloc, "");
 		    for (unsigned i = 1; i < (unsigned) yyleng - 1; ++i) {
		      if (yytext[i] == '\\')
			continue;
		      yylval->str->append(1, yytext[i]);
		    }
		  } else
		    yylval->str = new LocString(*yylloc,
						std::string(yytext, yyleng));
		  return ID;
		}

{EDGEOP}	{
		  yylloc->word(yyleng);
		  yylval->str = new LocString(*yylloc,
					      std::string(yytext, yyleng));
		  return EDGEOP;
		}

"//"[^\n]*'\n'	yylloc->word(yyleng);

"/*"		{
		  yylloc->word(yyleng);
		  BEGIN(STATE_COMMENT);
		  ++comment_level;
		}

<STATE_COMMENT>{

  "/*"		yylloc->word(yyleng); ++comment_level;

  "*/"		{
		  yylloc->word(yyleng);
		  --comment_level;
		  if (comment_level == 0)
		    BEGIN(INITIAL);
		}

  "\n"+		yylloc->line(yyleng);

  .		yylloc->column();

  <<EOF>>	{
		  parse_error(*yylloc, "unterminated comment");
		  BEGIN(INITIAL);
		}

}

[ \t]+		yylloc->word(yyleng);

"\n"+		yylloc->line(yyleng);

.		parse_error(*yylloc, "unrecognized character");

%%

      void
      scan_open(const std::string& name)
      { 
        FILE*	fin = fopen(name.c_str(), "r");
        if (fin == 0)
          throw std::runtime_error(std::string("Could not open \"" + name + "\" for reading: " + strerror(errno)));
  
        current_file = name;
        yyin = fin;
      }
  
      void
      scan_close()
      {
        fclose(yyin);
      }

    }

  }

}
