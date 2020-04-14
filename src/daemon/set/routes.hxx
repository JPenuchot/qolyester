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

# include "routes.hh"

#ifndef QOLYESTER_DAEMON_SET_ROUTES_HXX
# define QOLYESTER_DAEMON_SET_ROUTES_HXX 1

namespace olsr {

  rrouteset_t	operator*(const rrouteset_t& a, const lrouteset_t& b)
  {
    rrouteset_t	ret;

    for (rrouteset_t::const_iterator i = a.begin(); i != a.end(); ++i) {
      lrouteset_t::const_iterator	x =
	b.find(net::LocalRoute::make_key(i->dest_addr(), i->prefix()));
      if (x != b.end())
	ret.insert(*i);
    }

    return ret;
  }

  rrouteset_t	operator-(const rrouteset_t& a, const lrouteset_t& b)
  {
    rrouteset_t	ret;

    for (rrouteset_t::const_iterator i = a.begin(); i != a.end(); ++i) {
      lrouteset_t::const_iterator	x =
	b.find(net::LocalRoute::make_key(i->dest_addr(), i->prefix()));
      if (x == b.end())
	ret.insert(*i);
    }

    return ret;
  }

} // namespace olsr

#endif // !QOLYESTER_DAEMON_SET_ROUTES_HXX
