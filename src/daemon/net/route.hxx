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

#ifndef QOLYESTER_DAEMON_NET_ROUTE_HXX
# define QOLYESTER_DAEMON_NET_ROUTE_HXX 1

# include "route.hh"

namespace olsr {

  namespace net {

    LocalRoute::LocalRoute()
      : _d_addr(),
	_prefix(0),
	_info(ifaceinfo_t())
    {}

    LocalRoute::LocalRoute(const address_t& d, unsigned p,
			   const ifaceinfo_t& i)
      : _d_addr(address_t::network(d, p)),
	_prefix(p),
	_info(i) {
      assert(_info.index() != 0);
    }

    LocalRoute::LocalRoute(const This& other)
      : _d_addr(other._d_addr),
	_prefix(other._prefix),
	_info(other._info) {
      assert(_info.index() != 0);
    }

    const LocalRoute&
    LocalRoute::make_key(const address_t& d, unsigned p) {
      const_cast<address_t&>(_dummy_for_find._d_addr) = d;
      const_cast<unsigned&>(_dummy_for_find._prefix) = p;
      return _dummy_for_find;
    }

    RemoteRoute::RemoteRoute()
      : _d_addr(),
	_prefix(0),
	_n_addr()
    {}

    RemoteRoute::RemoteRoute(const address_t& d, unsigned p,
			     const address_t& n)
      : _d_addr(address_t::network(d, p)),
	_prefix(p),
	_n_addr(n) {
      assert(_n_addr != address_t());
    }

    RemoteRoute::RemoteRoute(const This& other)
      : _d_addr(other._d_addr),
	_prefix(other._prefix),
	_n_addr(other._n_addr) {
      assert(_n_addr != address_t());
    }

    const RemoteRoute&
    RemoteRoute::make_key(const address_t& d, unsigned p) {
      const_cast<address_t&>(_dummy_for_find._d_addr) = d;
      const_cast<unsigned&>(_dummy_for_find._prefix) = p;
      return _dummy_for_find;
    }

  } // namespace net

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_NET_ROUTE_HXX
