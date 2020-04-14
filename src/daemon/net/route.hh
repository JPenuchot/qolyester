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

///
/// @file   route.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 19:37:22 2004
///
/// @brief  Route definitions
///

// This include is out of protection to resolve dependency cycles.
# include "sys/interfaceinfo.hh"

#ifndef QOLYESTER_DAEMON_NET_ROUTE_HH
# define QOLYESTER_DAEMON_NET_ROUTE_HH 1

# include <ext/hash_set>
# include "net/ipaddress.hh"

namespace olsr {

  namespace net {

    ///
    /// @class LocalRoute
    /// @brief Local route class
    ///
    /// A local route is simply a destination address, a prefix length
    /// and an output interface.
    ///
    class LocalRoute {
      typedef LocalRoute	This;
      ///
      /// Private constructor
      ///
      /// This is provided only for @link
      /// olsr::net::LocalRoute::_dummy_for_find
      /// _dummy_for_find@endlink.
      ///
      explicit inline LocalRoute();
    public:
      ///
      /// Constructor
      /// @arg d	Destination address
      /// @arg p	Prefix length
      /// @arg i	Output interface
      ///
      inline LocalRoute(const address_t& d, unsigned p, const ifaceinfo_t& i);

      inline LocalRoute(const This& other);
      ///
      /// Destination address accessor
      /// @return	Destination address
      ///
      const address_t&	dest_addr() const { return _d_addr; }

      ///
      /// Prefix length accessor
      /// @return	Prefix length
      ///
      unsigned		prefix() const { return _prefix; }

      ///
      /// Interface accessor
      /// @return	Output interface
      ///
      const ifaceinfo_t&	interface_info() const { return _info; }

      ///
      /// Equality comparison operator
      /// @arg rhs	RHS operand
      /// @return	true if routes are equal
      ///
      bool			operator==(const This& rhs) const {
	return _d_addr == rhs._d_addr &&
	  _prefix == rhs._prefix &&
	  (_info.index() == rhs._info.index() ||
	   _info.index() == 0 ||
	   rhs._info.index() == 0);
      }

      ///
      /// Key builder
      /// @arg d	Destionation address
      /// @arg p	Prefix length
      /// @return	Modified instance of @link
      /// olsr::net::LocalRoute::_dummy_for_find
      /// _dummy_for_find@endlink
      ///
      static inline const This&	make_key(const address_t& d, unsigned p);
    private:
      const address_t		_d_addr; ///< Destination address
      const unsigned		_prefix; ///< Prefix length
      const ifaceinfo_t		_info;  ///< Output interface information

      static This		_dummy_for_find; ///< Static instance for key building
    };

    ///
    /// @class RemoteRoute
    ///
    class RemoteRoute {
      typedef RemoteRoute	This;
      explicit RemoteRoute();
    public:
      inline RemoteRoute(const address_t& d, unsigned p, const address_t& n);
      inline RemoteRoute(const This& other);

      const address_t&	dest_addr() const { return _d_addr; }
      unsigned		prefix() const { return _prefix; }
      const address_t&	next_addr() const { return _n_addr; }

      bool			operator==(const This& rhs) const {
	return _d_addr == rhs._d_addr &&
	  _prefix == rhs._prefix &&
	  (_n_addr == rhs._n_addr ||
	   _n_addr == address_t() ||
	   rhs._n_addr == address_t());
      }

      static inline const This&	make_key(const address_t& d, unsigned p);
    private:
      const address_t	_d_addr;
      const unsigned	_prefix;
      const address_t	_n_addr;

      static This	_dummy_for_find;
    };

    struct hash_LocalRoute {
      ::size_t operator()(const LocalRoute& r) const {
	return std::hash<address_t>()(r.dest_addr());
      }
    };

    struct hash_RemoteRoute {
      ::size_t operator()(const RemoteRoute& r) const {
	return std::hash<address_t>()(r.dest_addr());
      }
    };

  } // namespace net;

} // namespace olsr

# include "route.hxx"

#endif // ! QOLYESTER_DAEMON_NET_ROUTE_HH
