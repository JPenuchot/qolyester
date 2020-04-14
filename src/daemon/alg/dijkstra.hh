// Copyright (C) 2003-2006 Laboratoire de Recherche en Informatique

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

///
/// @file   dijkstra.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 14:14:27 2004
///
/// @brief  The multisource Dijkstra shortest path algorithm
///
/// The graph used is the current instance of the partial network
/// topology graph.  The set of source nodes is the set of 1-hop
/// neighbors.  This suffices to generate routing tables, since all
/// shortest paths to 1-hop neighbors are trivial.
///

#include "set/routes.hh"

#ifndef QOLYESTER_DAEMON_ALG_DIJKSTRA_HH
# define QOLYESTER_DAEMON_ALG_DIJKSTRA_HH 1

namespace olsr {

  namespace alg {

    typedef cproxy_t::sym_neighborset_t::iterator	sniter_t;
    typedef std::map<address_t, std::pair<address_t, sniter_t> >	lmap_t;

    // The Dijkstra algorithm used to calculate routes.  The first
    // argument is a reference to the route set and the second is a
    // set of source node addresses.
    inline void	dijkstra(rrouteset_t& routes, lmap_t& sources);

  } // namespace alg

} // namespace olsr

# include "dijkstra.hxx"

#endif // !QOLYESTER_DAEMON_ALG_DIJKSTRA_HH
