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
/// @file   virtual/interfacedesc.hh
/// @author Benoit Bourdin
/// @date   Tue Jun 02 2004
///
/// @brief  empty definitions for compatibility

#include "config.hh"

#ifdef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_VIRTUAL_INTERFACEDESC_HH
#  define QOLYESTER_DAEMON_SYS_VIRTUAL_INTERFACEDESC_HH 1

#  include <set>

#  include "net/ipaddress.hh"

namespace olsr {

  namespace sys {

    namespace internal {


     ///
     /// @class InterfaceAddress
     /// @brief dummy class for compatibity with other sys classes
     ///
     class InterfaceAddress {
	typedef InterfaceAddress	This;
	InterfaceAddress();
      public:
	InterfaceAddress(unsigned p,
			 unsigned char s,
			 const address_t& a,
			 const address_t& b);

	unsigned		prefix()    const { return _prefix;    }
	unsigned char		scope()     const { return _scope;     }
	const address_t&	address()   const { return _address;   }
	const address_t&	broadcast() const { return _broadcast; }

	bool			operator<(const This& rhs) const;

	static This		make_key(const address_t& a);
      private:
	unsigned		_prefix;
	unsigned char		_scope;
	const address_t		_address;
	const address_t		_broadcast;

	static This	_dummy_for_find;
      };

      class InterfaceInfo {
	typedef InterfaceInfo		This;
	typedef InterfaceAddress	addr_t;
      public:
	typedef std::set<addr_t>	addrs_t;

	InterfaceInfo(const std::string& name);

	const std::string&	name()  const { return _name; }
	unsigned		index() const { return _index; }
	unsigned		mtu()   const { return _mtu;   }
	const addrs_t&		addrs() const { return _addrs; }

	bool		operator<(const This& rhs) const;


      private:
	std::string	_name;
	unsigned	_index;
	unsigned	_mtu;
	addrs_t		_addrs;
      };

    } // namespace internal

  } // namespace sys

} // namespace olsr

#  include "interfacedesc.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_VIRTUAL_INTERFACEDESC_HH

#endif // QOLYESTER_ENABLE_VIRTUAL
