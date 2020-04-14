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

#ifndef QOLYESTER_SRC_SWITCH_PRS_PARSE_HH
# define QOLYESTER_SRC_SWITCH_PRS_PARSE_HH 1

# include <string>
# include <set>
# include <map>

# include "config.hh"

# include "gra/graph.hh"
# include "net/ipaddress.hh"
# include "prs/data.hh"
# include "prs/location.hh"

namespace olsr {

  namespace prs {

    namespace dot {

      extern void	parse_error(const Location&, const char*);
      extern Graph*	parse(const std::string&);

      class InitVisitor : public DefaultVisitor {
	typedef std::set<address_t>			aset_t;
	typedef std::map<address_t, std::string>	pathmap_t;
	typedef std::map<std::string, address_t>	addrmap_t;
      public:
	inline InitVisitor(gra::AdjGraph& ag,
			   std::set<address_t>& fas);
// 		    std::map<address_t, std::string>& pm,
// 		    std::map<std::string, address_t>& am);


	virtual inline void	visit(const Graph& g);
	virtual inline void	visit(const EdgeAttrStmt& s);
	virtual inline void	visit(const NodeStmt& s);
	virtual inline void	visit(const EdgeStmt& s);
      private:
	gra::AdjGraph&	_ag;
	aset_t&		_faddrset;
	pathmap_t	_pathmap;
	addrmap_t	_addrmap;
	float		_default_loss;
	unsigned	_default_delay;
	bool		_dloss;
	bool		_ddelay;
      };

    } // namespace dot

  } // namespace prs

} // namespace olsr

# include "parse.hxx"

#endif // QOLYESTER_SRC_SWITCH_PRS_PARSE_HH
