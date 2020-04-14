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

#ifndef QOLYESTER_SRC_SWITCH_PRS_DATA_HH
# define QOLYESTER_SRC_SWITCH_PRS_DATA_HH 1

# include <string>
# include <list>

# include "prs/visitor.hh"
# include "prs/location.hh"

namespace olsr {

  namespace prs {

    namespace dot {

      struct GraphTag;
      struct NodeTag;
      struct EdgeTag;

      class LocString : public std::string {
      public:
	LocString(const Location& l, const std::string& s)
	  : std::string(s),
	    _location(l)
	{}
	const Location&	location() const { return _location; }
      private:
	Location	_location;
      };

      class Stmt;

      class Graph {
      public:
	inline Graph(const Location& l, LocString* id,
		     std::list<Stmt*>* stmtlist);
	virtual inline  ~Graph();
	virtual inline void	accept(Visitor& v) const;
	const LocString&	id() const { return *_id; }
	const std::list<Stmt*>&	stmtlist() const { return *_stmtlist; }
      private:
	Location		_location;
	LocString*		_id;
	std::list<Stmt*>*	_stmtlist;
      };

      class Stmt {
      protected:
	inline Stmt(const Location& l);
      public:
	virtual ~Stmt() {}
	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const Location&	location() const { return _location; }
      private:
	Location	_location;
      };

      class AttrStmt : public Stmt {
      protected:
	inline AttrStmt(const Location& l);
	virtual void	accept(Visitor& v) const { v.visit(*this); }
      public:
	virtual ~AttrStmt() {}
      };

      template <class Tag>
      class TagAttrStmt : public AttrStmt {
      public:
	inline TagAttrStmt(const Location& l,
			   std::list<AttrAssignment*>* alist);
	virtual inline ~TagAttrStmt();
	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const std::list<AttrAssignment*>&	alist() const {
	  return *_alist;
	}
      private:
	std::list<AttrAssignment*>*	_alist;
      };

      template class TagAttrStmt<GraphTag>;
      template class TagAttrStmt<NodeTag>;
      template class TagAttrStmt<EdgeTag>;

      class NodeStmt : public Stmt {
      public:
	inline NodeStmt(const Location& l,
			LocString* nodeid,
			std::list<AttrAssignment*>* attrlist);
	virtual inline ~NodeStmt();
	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const LocString&	nodeid() const { return *_nodeid; }
	const std::list<AttrAssignment*>&	attrlist() const {
	  return *_attrlist;
	}
      private:
	LocString*			_nodeid;
	std::list<AttrAssignment*>*	_attrlist;
      };

      class AttrAssignment {
      public:
	inline AttrAssignment(const Location& l, LocString* key,
			      LocString* val);
	virtual inline ~AttrAssignment();
	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const Location&	location() const { return _location; }
	const LocString&	key() const { return *_key; }
	const LocString&	val() const { return *_val; }
      private:
	Location	_location;
	LocString*	_key;
	LocString*	_val;
      };

      class EdgeStmt : public Stmt {
      public:
	inline EdgeStmt(const Location& l, LocString* nodeid, EdgeRHS* edgerhs,
			std::list<AttrAssignment*>* attrlist);
	virtual inline ~EdgeStmt();
	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const LocString&	nodeid() const { return *_nodeid; }
	const EdgeRHS*		edgerhs() const { return _edgerhs; }
	const std::list<AttrAssignment*>&	attrlist() const {
	  return *_attrlist;
	}
      private:
	LocString*			_nodeid;
	EdgeRHS*			_edgerhs;
	std::list<AttrAssignment*>*	_attrlist;
      };

      class EdgeRHS {
      public:
	enum EdgeOp { edge, arc };
	inline EdgeRHS(const Location& l, EdgeOp edgeop,
		       LocString* nodeid, EdgeRHS* edgerhs);
	virtual inline ~EdgeRHS();
	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const Location&	location() const { return _location; }
	EdgeOp		edgeop() const { return _edgeop; }
	const LocString&	nodeid() const { return *_nodeid; }
	const EdgeRHS*		edgerhs() const { return _edgerhs; }
      private:
	Location	_location;
	EdgeOp		_edgeop;
	LocString*	_nodeid;
	EdgeRHS*	_edgerhs;
      };

      typedef TagAttrStmt<GraphTag>	GraphAttrStmt;
      typedef TagAttrStmt<NodeTag>	NodeAttrStmt;
      typedef TagAttrStmt<EdgeTag>	EdgeAttrStmt;

    } // namespace dot

  } // namespace prs

} // namespace olsr

# include "data.hxx"

#endif // QOLYESTER_SRC_SWITCH_PRS_DATA_HH
