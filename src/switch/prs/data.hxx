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

#ifndef QOLYESTER_SRC_SWITCH_PRS_DATA_HXX
# define QOLYESTER_SRC_SWITCH_PRS_DATA_HXX 1

# include "data.hh"

namespace olsr {

  namespace prs {

    namespace dot {

      Graph::Graph(const Location& l, LocString* id,
		   std::list<Stmt*>* stmtlist)
	: _location(l),
	  _id(id),
	  _stmtlist(stmtlist)
      {}

      Graph::~Graph() {
	delete _id;
	for (std::list<Stmt*>::iterator i = _stmtlist->begin();
	     i != _stmtlist->end(); ++i)
	  delete *i;
	delete _stmtlist;
      }

      void
      Graph::accept(Visitor& v) const {
	v.visit(*this);
      }

      Stmt::Stmt(const Location& l)
	: _location(l)
      {}

      AttrStmt::AttrStmt(const Location& l)
	: Stmt(l)
      {}

      template <class Tag>
      TagAttrStmt<Tag>::TagAttrStmt(const Location& l,
				    std::list<AttrAssignment*>* alist)
	: AttrStmt(l),
	  _alist(alist)
      {}


      template <class Tag>
      TagAttrStmt<Tag>::~TagAttrStmt() {
	for (std::list<AttrAssignment*>::iterator i = _alist->begin();
	     i != _alist->end(); ++i)
	  delete *i;
	delete _alist;
      }

      NodeStmt::NodeStmt(const Location& l,
			 LocString* nodeid,
			 std::list<AttrAssignment*>* attrlist)
	: Stmt(l),
	  _nodeid(nodeid),
	  _attrlist(attrlist)
      {}

      NodeStmt::~NodeStmt() {
	delete _nodeid;
	for (std::list<AttrAssignment*>::iterator i = _attrlist->begin();
	     i != _attrlist->end(); ++i)
	  delete *i;
	delete _attrlist;
      }

      AttrAssignment::AttrAssignment(const Location& l,
				     LocString* key,
				     LocString* val)
	: _location(l),
	  _key(key),
	  _val(val)
      {}

      AttrAssignment::~AttrAssignment() {
	delete _key;
	delete _val;
      }

      EdgeStmt::EdgeStmt(const Location& l,
			 LocString* nodeid,
			 EdgeRHS* edgerhs,
			 std::list<AttrAssignment*>* attrlist)
	: Stmt(l),
	  _nodeid(nodeid),
	  _edgerhs(edgerhs),
	  _attrlist(attrlist)
      {}

      EdgeStmt::~EdgeStmt() {
	delete _nodeid;
	delete _edgerhs;
	for (std::list<AttrAssignment*>::iterator i = _attrlist->begin();
	     i != _attrlist->end(); ++i)
	  delete *i;
	delete _attrlist;
      }

      EdgeRHS::EdgeRHS(const Location& l, EdgeOp edgeop,
		       LocString* nodeid, EdgeRHS* edgerhs)
	: _location(l),
	  _edgeop(edgeop),
	  _nodeid(nodeid),
	  _edgerhs(edgerhs)
      {}

      EdgeRHS::~EdgeRHS() {
	delete _nodeid;
	if (_edgerhs != 0)
	  delete _edgerhs;
      }

    } // namespace dot

  } // namespace prs

} // namespace olsr

#endif // QOLYESTER_SRC_SWITCH_PRS_DATA_HXX
