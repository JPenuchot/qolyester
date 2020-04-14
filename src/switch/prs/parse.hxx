// Copyright (C) 2003-2009 Laboratoire de Recherche en Informatique

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_SRC_SWITCH_PRS_PARSE_HXX
# define QOLYESTER_SRC_SWITCH_PRS_PARSE_HXX 1

# include <ostream>

# include "utl/exception.hh"

# include "parse.hh"

namespace olsr {

  extern std::ostream	error;
  extern std::ostream	warning;
  extern std::ostream	notice;
  extern std::ostream	dump;

  extern debug_ostream_t	debug;

  namespace prs {

    namespace dot {

      InitVisitor::InitVisitor(gra::AdjGraph& ag,
			       aset_t& fas)
// WTF??
// 			       pathmap_t& pm,
// 			       addrmap_t& am)
	: DefaultVisitor(),
	  _ag(ag),
	  _faddrset(fas),
// 	  _pathmap(pm),
// 	  _addrmap(am),
	  _default_loss(0),
	  _default_delay(0),
	  _dloss(false),
	  _ddelay(false)
      {}

      void
      InitVisitor::visit(const Graph& g) {
	for (std::list<Stmt*>::const_iterator i = g.stmtlist().begin();
	     i != g.stmtlist().end(); ++i)
	  (*i)->accept(*this);
      }

      void
      InitVisitor::visit(const EdgeAttrStmt& s) {
	for (std::list<AttrAssignment*>::const_iterator i = s.alist().begin();
	     i != s.alist().end(); ++i)
	  if ((*i)->key() == "loss")
	    if (_dloss)
	      warning << "Default loss already defined, "
		      << (*i)->key().location() << std::endl;
	    else {
	      std::istringstream	is((*i)->val());
	      is >> _default_loss;
	      _dloss = true;
	    }
	  else if ((*i)->key() == "delay") {
	    if (_ddelay)
	      warning << "Default delay already defined, "
		      << (*i)->key().location() << std::endl;
	    else {
	      std::istringstream	is((*i)->val());
	      is >> _default_delay;
	      _ddelay = true;
	    }
	  }
      }

      void
      InitVisitor::visit(const NodeStmt& s) {
	const std::string&	label = s.nodeid();
	address_t		addr;
	bool			daddr = false;
	for (std::list<AttrAssignment*>::const_iterator i =
	       s.attrlist().begin();
	     i != s.attrlist().end(); ++i) {
	  debug << up << "Visiting attr " << (*i)->key() << std::endl << down;
	  if ((*i)->key() == "address") {
	    if (daddr)
	      warning << "Node address already defined, "
		      << (*i)->key().location() << std::endl;
	    else
	      try {
		addr = address_t((*i)->val());
		daddr = true;
	      } catch (errnoexcept_t& e) {
		parse_error((*i)->val().location(), e.what());
	      }
	  }
	}
	gra::AdjNode	node(addr, label);
	if (_ag.nodes().find(node) != _ag.nodes().end())
	  parse_error(s.location(),
		      (std::string("node with address ") + addr.to_string() +
		       " is already defined").c_str());
	_ag.insert_node(gra::AdjNode(addr, label));
	_faddrset.insert(addr);
 	_pathmap[addr] = label;
 	_addrmap[label] = addr;
      }

      void
      InitVisitor::visit(const EdgeStmt& s) {
	float		loss = 0;
	unsigned	delay = 0;
	double		lbegin = 0;
	double		lend = 0;
	bool		dloss = false;
	bool		ddelay = false;
	bool		dlbegin = false;
	bool		dlend = false;
	for (std::list<AttrAssignment*>::const_iterator i =
	       s.attrlist().begin();
	     i != s.attrlist().end(); ++i)
	  if ((*i)->key() == "loss") {
	    if (dloss)
	      warning << "Edge loss already defined, "
		      << (*i)->key().location() << std::endl;
	    else {
	      std::istringstream	is((*i)->val());
	      is >> loss;
	      dloss = true;
	    }
	  } else if ((*i)->key() == "delay") {
	    if (ddelay)
	      warning << "Edge delay already defined, "
		      << (*i)->key().location() << std::endl;
	    else {
	      std::istringstream	is((*i)->val());
	      is >> delay;
	      ddelay = true;
	    }
	  } else if ((*i)->key() == "lifebegin") {
	    if (dlbegin)
	      warning << "Edge lifetime begin already defined, "
		      << (*i)->key().location() << std::endl;
	    else {
	      std::istringstream	is((*i)->val());
	      is >> lbegin;
	      dlbegin = true;
	    }
	  } else if ((*i)->key() == "lifeend") {
	    if (dlend)
	      warning << "Edge lifetime end already defined, "
		      << (*i)->key().location() << std::endl;
	    else {
	      std::istringstream	is((*i)->val());
	      is >> lend;
	      dlend = true;
	    }
	  }
	LocString	ep1 = s.nodeid();
	addrmap_t::iterator	ax1 = _addrmap.find(ep1);
	if (ax1 == _addrmap.end())
	  parse_error(ep1.location(),
		      (std::string("node ") + ep1 + " is unknown").c_str());
	const EdgeRHS*	rhs = s.edgerhs();
	assert(rhs != 0);
	while (rhs != 0) {
	  LocString	ep2 = rhs->nodeid();
	  addrmap_t::iterator	ax2 = _addrmap.find(ep2);
	  if (ax2 == _addrmap.end())
	    parse_error(ep2.location(),
			(std::string("Node ") + ep2 + " is unknown").c_str());
	  _ag.insert_arc(gra::AdjInfo(ax1->second, ax2->second,
				      loss, delay, lbegin, lend));
	  if (rhs->edgeop() == EdgeRHS::edge)
	    _ag.insert_arc(gra::AdjInfo(ax2->second, ax1->second,
					loss, delay,
					lbegin, lend));
	  ep1 = ep2;
	  ax1 = ax2;
	  rhs = rhs->edgerhs();
	}
      }

    } // namespace dot

  } // namespace prs

} // namespace olsr

#endif // QOLYESTER_SRC_SWITCH_PRS_PARSE_HXX
