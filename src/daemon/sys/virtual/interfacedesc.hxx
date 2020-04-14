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

#include "config.hh"

#ifdef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_SYS_VIRTUAL_INTERFACEDESC_HXX
#  define QOLYESTER_SYS_VIRTUAL_INTERFACEDESC_HXX 1

#  include "interfacedesc.hh"

namespace olsr {

  namespace sys {

    namespace internal {

      InterfaceAddress::InterfaceAddress() :
	_prefix(0),
	_scope(0),
	_address(),
	_broadcast()
      {}

      InterfaceAddress::InterfaceAddress(unsigned p,
					 unsigned char s,
					 const address_t& a,
					 const address_t& b) :
	_prefix(p),
	_scope(s),
	_address(a),
	_broadcast(b)
      {}

      bool
      InterfaceAddress::operator<(const This& rhs) const {
	return _address < rhs._address;
      }

      InterfaceAddress
      InterfaceAddress::make_key(const address_t& a) {
	This	tmp = _dummy_for_find;

	const_cast<address_t&>(tmp._address) = a;
	return tmp;
      }

      InterfaceInfo::InterfaceInfo(const std::string&) :
	_name(),
	_index(0),
	_mtu(0),
	_addrs()
      {}

      bool
      InterfaceInfo::operator<(const This& rhs) const {
	return _index < rhs._index;
      }

    } // namespace internal

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_SYS_VIRTUAL_INTERFACEDESC_HXX

#endif // QOLYESTER_ENABLE_VIRTUAL
