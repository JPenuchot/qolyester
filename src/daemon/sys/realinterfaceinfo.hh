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

# ifndef QOLYESTER_DAEMON_SYS_REALINTERFACEINFO_HH
#  define QOLYESTER_DAEMON_SYS_REALINTERFACEINFO_HH 1

#  include <string>
#  include <set>
#  include "net/ipaddress.hh"
#  include "linux/interfacedesc.hh"
#  include "linux/interfaceconfig.hh"

namespace olsr {

  namespace sys {

    class RealInterfaceInfo {
      typedef RealInterfaceInfo			This;
      typedef internal::InterfaceAddress	addr_t;
      typedef std::set<addr_t>			addrs_t;

    public:
      typedef sys::internal::InterfaceConfig	config_t;

      inline RealInterfaceInfo();
      explicit inline RealInterfaceInfo(const std::string& name);

      inline This&	operator=(const This& other);

      const std::string&	name()  const { return _name;  }
      unsigned			mtu()   const { return _mtu;   }
      unsigned			index() const { return _index; }

      inline const address_t&		get_addr() const;
      inline const address_t&		get_addr(const address_t&) const;
      inline const address_t		get_bcast(const address_t&) const;
      inline unsigned			get_prefix(const address_t&) const;

      inline void			add_addr(const address_t&, unsigned);
      inline void			del_addr(const address_t&);
    private:
      const std::string	_name;
      unsigned		_mtu;
      unsigned		_index;
      addrs_t		_addrs;
    };

  } // namespace sys

} // namespace olsr

#  include "realinterfaceinfo.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_REALINTERFACEINFO_HH

#endif // ! QOLYESTER_ENABLE_VIRTUAL
