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
/// @file   routingtable.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 14:23:32 2004
///
/// @brief  Routing table management
///
/// Two kinds of routes are considered: local and remote routes.  A
/// local route is a route to a symmetric 1-hop neighbor, whereas a
/// remote route is a multihop route to a node not in the symmetric
/// 1-hop neighborhood.  Local routes are computed using the neighbor
/// tables directly, whereas remote routes are computed using a
/// shortest path algorithm on the partial topology graph.  Newly
/// calculated routes need to be flushed to the kernel in order for
/// them to take effect.
///

#include "set/routes.hh"

#ifndef QOLYESTER_DAEMON_ALG_ROUTINGTABLE_HH
# define QOLYESTER_DAEMON_ALG_ROUTINGTABLE_HH 1

# include "alg/dijkstra.hh"

namespace olsr {

  namespace alg {

    ///
    /// Routine to calculate new routing tables
    ///
    inline void	compute_routes();

    ///
    /// Routine to apply new routing tables
    ///
    inline void	flush_routes(const lrouteset_t& local_rs,
			     const rrouteset_t& remote_rs);

    inline void	clean_routes();

  } // namespace alg

} // namespace olsr

# include "routingtable.hxx"

#endif // !QOLYESTER_DAEMON_ALG_ROUTINGTABLE_HH
