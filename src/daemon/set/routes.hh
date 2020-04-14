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

# include "net/route.hh"

#ifndef QOLYESTER_DAEMON_SET_ROUTES_HH
# define QOLYESTER_DAEMON_SET_ROUTES_HH 1

# include "utl/set.hh"
# include <ext/hash_set>

namespace olsr {

  namespace set {

    namespace internal {

      typedef std::hash_set<net::LocalRoute,
			    net::hash_LocalRoute>	lrset_t_;
      typedef std::hash_set<net::RemoteRoute,
			    net::hash_RemoteRoute>	rrset_t_;

    } // namespace internal


  } // namespace set

  typedef utl::Set<net::LocalRoute, set::internal::lrset_t_>	lrouteset_t;
  typedef utl::Set<net::RemoteRoute, set::internal::rrset_t_>	rrouteset_t;

  inline lrouteset_t	operator-(const lrouteset_t& a, const rrouteset_t& b);
  inline rrouteset_t	operator-(const rrouteset_t& a, const lrouteset_t& b);

} // namespace olsr

# include "routes.hxx"

#endif // ! QOLYESTER_DAEMON_SET_ROUTES_HH
