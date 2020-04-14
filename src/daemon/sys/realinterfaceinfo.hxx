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

#ifndef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_REALINTERFACEINFO_HXX
#  define QOLYESTER_DAEMON_SYS_REALINTERFACEINFO_HXX 1

#  include <cassert>
#  include <stdexcept>

#  include "config.hh"

#  include "set/interfaces.hh"
#  include "sys/linux/routingsocket.hh"

#  include "realinterfaceinfo.hh"

namespace olsr {

  extern ifaceset_t	iface_set;

  namespace sys {

    RealInterfaceInfo::RealInterfaceInfo()
      : _name(),
	_mtu(0),
	_index(0),
	_addrs()
    {}

    RealInterfaceInfo::RealInterfaceInfo(const std::string& name)
      : _name(name),
	_mtu(0),
	_index(0),
	_addrs() {
      internal::InterfaceInfo	info(name);
      _mtu   = info.mtu();
      _index = info.index();
      assert(_index != 0);
      _addrs = info.addrs();
    }

    RealInterfaceInfo&
    RealInterfaceInfo::operator=(const This& other) {
      const_cast<std::string&>(_name) = other._name;
      _mtu = other._mtu;
      _index = other._index;
      _addrs = other._addrs;
      return *this;
    }

    const address_t&
    RealInterfaceInfo::get_addr() const {
      const address_t*	paddr = 0;

#  if QOLYESTER_FAMILY_INET == 6
      bool		have_ll = false;
#  endif

      for (addrs_t::iterator i = _addrs.begin(); i != _addrs.end(); ++i) {
	if (paddr == 0 && !i->address().is_linklocal() &&
	    i->prefix() == ADDRESS_SIZE * 8) {
	  paddr = &i->address();
#  if QOLYESTER_FAMILY_INET == 4
	  break;
#  endif
	}
#  if QOLYESTER_FAMILY_INET == 6
	if (i->address().is_linklocal())
	  have_ll = true;
#  endif
      }

      if (paddr == 0)
	throw std::runtime_error(_name + " has no usable address, a global address with full prefix is needed");

#  if QOLYESTER_FAMILY_INET == 6
      if (!have_ll)
	throw std::runtime_error(_name + " has usable address, but no link-local address");
#  endif
      return *paddr;
    }

    const address_t&
    RealInterfaceInfo::get_addr(const address_t& a) const {

      if (_addrs.find(addr_t::make_key(a)) == _addrs.end())
	throw std::runtime_error(_name + " has no address " + a.to_string());

      return a;
    }

#  if defined QOLYESTER_LL_MCAST && QOLYESTER_FAMILY_INET == 6
    const address_t
    RealInterfaceInfo::get_bcast(const address_t&) const {
      return address_t(QOLYESTER_LINKLOCAL_MCAST);
    }
#  else
    const address_t
    RealInterfaceInfo::get_bcast(const address_t& a) const {
#   if QOLYESTER_FAMILY_INET == 6
      if (a.is_linklocal())
	return address_t(QOLYESTER_LINKLOCAL_MCAST);
      return address_t(QOLYESTER_GLOBAL_MCAST);
#   else // QOLYESTER_FAMILY_INET != 6
      addrs_t::iterator	x = _addrs.find(addr_t::make_key(a));
      if (x != _addrs.end() && x->broadcast() != address_t())
	return x->broadcast();
      return address_t("255.255.255.255");
#   endif
    }
#  endif

    unsigned
    RealInterfaceInfo::get_prefix(const address_t& a) const {
      addrs_t::iterator x = _addrs.find(addr_t::make_key(a));
      if (x != _addrs.end())
	return x->prefix();
      return 0;
    }

    void
    RealInterfaceInfo::add_addr(const address_t& a, unsigned p) {
      internal::RoutingSocket().add_addr(_index, addr_t(p, IFA_F_PERMANENT,
							a, address_t()));
      *this = RealInterfaceInfo(_name);
    }

    void
    RealInterfaceInfo::del_addr(const address_t& a) {
      addrs_t::iterator	x = _addrs.find(addr_t::make_key(a));

      if (x == _addrs.end())
	throw std::runtime_error("Attempt to delete inexistant address " +
				 a.to_string() + " from " + _name);

      if (iface_set.find(a) != iface_set.end())
	throw std::runtime_error("Attempt to delete address " + a.to_string() +
				 " from " + _name + ", while using it");

      internal::RoutingSocket().del_addr(_index, *x);
      *this = RealInterfaceInfo(_name);
    }

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_SYS_REALINTERFACEINFO_HXX

#endif // ! QOLYESTER_ENABLE_VIRTUAL
