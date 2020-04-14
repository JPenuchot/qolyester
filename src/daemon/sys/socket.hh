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

#include "config.hh"

#ifndef QOLYESTER_ENABLE_VIRTUAL

# include "sys/realinterfaceinfo.hh"

# ifndef QOLYESTER_DAEMON_SYS_SOCKET_HH
#  define QOLYESTER_DAEMON_SYS_SOCKET_HH 1

#  include <sys/types.h>
#  include <string>

#  include "config.hh"

#  include "net/ipaddress.hh"
#  include "utl/data.hh"
#  include "sch/scheduler.hh"

namespace olsr {

  namespace sys {

    // Higher-than-address level socket definition.
    class Socket {
      typedef Socket	This;
      enum _dummy_values { _dummy_val };
      inline Socket(_dummy_values);
    public:
      // Some utility definitions
      typedef address_t::sockaddr_t	sockaddr_t;
      typedef address_t			addr_t;
      typedef u_int16_t			port_t;

      // Constructors, for each kind of funky use.

      inline Socket();
      inline Socket(unsigned mtu);
      inline Socket(unsigned mtu, const addr_t& addr, port_t port);
      inline Socket(unsigned mtu, const std::string& host, port_t port);
      inline Socket(unsigned mtu, const char* host, port_t port);

      inline void	close();

      unsigned	mtu() const { return _mtu; }

      void	set_mtu(unsigned mtu) { _mtu = mtu; }

      inline void	bind_to_device(const std::string& name);

      inline void	set_multicast(const RealInterfaceInfo& info);

      inline void	set_priority();

      inline void	bind(const sockaddr_t& sin);

      inline void	bind_multicast(const RealInterfaceInfo& info,
				       const address_t& a, port_t port);

#  if QOLYESTER_FAMILY_INET == 6
      inline void	bind(const addr_t& addr, port_t port,
			     unsigned index = 0);
#  else // QOLYESTER_FAMILY_INET != 6
      inline void	bind(const addr_t& addr, port_t port);
#  endif

      inline void	connect(const sockaddr_t& sin);

      inline void	connect(const addr_t& addr, port_t port);

      inline utl::Data	receive(address_t& sender) const;

      inline void	send(const utl::ConstData& d) const;

      void	set_baddr(const address_t& a) { _baddr = a; }

      inline void	sendto(const utl::ConstData& d,
			       const address_t::sockaddr_t& s) const;

      inline void	sendto_bcast(const utl::ConstData& d) const;

      inline sch::IOEvent::p_t	read_p() const;
      inline sch::IOEvent::p_t	write_p() const;

      inline bool	operator<(const This& rhs) const;

      static const This&	dummy() { return _dummy; }

    private:

      // Internal utility function used by constructors.
      void	init();

      int	_fd;
      unsigned	_mtu;
      address_t	_baddr;
      unsigned	_index;

      static This	_dummy;
    };

  } // namespace sys

} // namespace olsr

#  include "socket.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_SOCKET_HH

#endif // ! QOLYESTER_ENABLE_VIRTUAL
