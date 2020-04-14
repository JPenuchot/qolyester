// Copyright (C) 2003, 2004, 2005 Laboratoire de Recherche en Informatique

// This file is part of Qolyester.

// Qolyester is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// Qolyester is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_SRC_SWITCH_PRS_VISITOR_HH
# define QOLYESTER_SRC_SWITCH_PRS_VISITOR_HH 1

//# include "prs/data_fwd.hh"

namespace olsr {

  namespace prs {

    namespace dot {

      struct GraphTag;
      struct NodeTag;
      struct EdgeTag;
      template <class> class TagAttrStmt;
      typedef TagAttrStmt<GraphTag>	GraphAttrStmt;
      typedef TagAttrStmt<NodeTag>	NodeAttrStmt;
      typedef TagAttrStmt<EdgeTag>	EdgeAttrStmt;
      class Graph;
      class Stmt;
      class AttrStmt;
      class NodeStmt;
      class AttrAssignment;
      class EdgeStmt;
      class EdgeRHS;


      class Visitor {
      public:

# define VISIT(M) \
  virtual void visit(M&) = 0; \
  virtual void visit(const M&) = 0

	VISIT(Graph);
	VISIT(Stmt);
	VISIT(AttrStmt);
	VISIT(GraphAttrStmt);
	VISIT(NodeAttrStmt);
	VISIT(EdgeAttrStmt);
	VISIT(NodeStmt);
	VISIT(AttrAssignment);
	VISIT(EdgeStmt);
	VISIT(EdgeRHS);

# undef VISIT

	virtual ~Visitor() {}
      };

      class DefaultVisitor : public Visitor {
      public:
	DefaultVisitor() : Visitor() {}

# define VISIT(M) \
  virtual void visit(M&) {} \
  virtual void visit(const M&) {}

	VISIT(Graph);
	VISIT(Stmt);
	VISIT(AttrStmt);
	VISIT(GraphAttrStmt);
	VISIT(NodeAttrStmt);
	VISIT(EdgeAttrStmt);
	VISIT(NodeStmt);
	VISIT(AttrAssignment);
	VISIT(EdgeStmt);
	VISIT(EdgeRHS);

# undef VISIT

      };

    } // namespace dot

  } // namespace prs

} // namespace olsr

#endif // QOLYESTER_SRC_SWITCH_PRS_VISITOR_HH
