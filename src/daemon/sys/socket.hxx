// Copyright (C) 2003-2009 Laboratoire de Recherche en Informatique

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

# ifndef QOLYESTER_DAEMON_SYS_SOCKET_HXX
#  define QOLYESTER_DAEMON_SYS_SOCKET_HXX 1

#  include <sys/socket.h>
#  include <fcntl.h>
#  include <sys/ioctl.h>
#  include <net/if.h>
#  include <netinet/ip.h>
#  include <sstream>

#  include "cst/constants.hh"
#  include "utl/exception.hh"

#  include "sys/realinterfaceinfo.hh"

#  include "socket.hh"

namespace olsr {

#  if QOLYESTER_FAMILY_INET == 6
  extern bool	dirty_promisc_hack;
#  endif

  namespace sys {

    Socket::Socket(_dummy_values)
      : _fd(-1),
	_mtu(0),
	_baddr(),
	_index(0)
    {}

    Socket::Socket()
      : _mtu(0),
	_baddr(),
	_index(0) {
      init();
    }

    Socket::Socket(unsigned mtu)
      : _mtu(mtu),
	_baddr(),
	_index(0) {
      init();
    }

    Socket::Socket(unsigned mtu, const addr_t& addr, port_t port)
      : _mtu(mtu),
	_baddr(),
	_index(0) {
      init();
      bind(addr, port);
    }

    Socket::Socket(unsigned mtu, const std::string& host, port_t port)
      : _mtu(mtu),
	_baddr(),
	_index(0) {
      init();
      bind(addr_t(host), port);
    }

    Socket::Socket(unsigned mtu, const char* host, port_t port)
      : _mtu(mtu),
	_baddr(),
	_index(0) {
      init();
      bind(addr_t(std::string(host)), port);
    }

    void
    Socket::close() {
      if (_fd >= 0)
	::close(_fd);
      _fd = -1;
    }

    void
    Socket::bind_to_device(const std::string& name) {
      if (::setsockopt(_fd, SOL_SOCKET, SO_BINDTODEVICE,
		       name.c_str(), name.length() + 1) < 0)
	throw errnoexcept_t(std::string("setsockopt(SO_BINDTODEVICE): ") +
			    strerror(errno), errno);
    }

    void
#  if QOLYESTER_FAMILY_INET != 6
    Socket::set_multicast(const RealInterfaceInfo&) {
      int val = 1;
      if (setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, &val, sizeof val) < 0)
	throw errnoexcept_t(std::string("setsockopt(SO_BROADCAST): ") +
					strerror(errno), errno);
#  else // QOLYESTER_FAMILY_INET == 6
    Socket::set_multicast(const RealInterfaceInfo& info) {
      unsigned	index = info.index();

      if (setsockopt(_fd, IPPROTO_IPV6, IPV6_MULTICAST_IF,
		     &index, sizeof index) < 0)
	throw errnoexcept_t(std::string("setsockopt(IPV6_MULTICAST_IF): ") +
			    strerror(errno), errno);
      int	val = 0;

      if (setsockopt(_fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP,
		     &val, sizeof val) < 0)
	throw errnoexcept_t(std::string("setsockopt(IPV6_MULTICAST_LOOP): ") +
					strerror(errno), errno);

      if (dirty_promisc_hack) {
	ifreq	ifr;
	strncpy(ifr.ifr_name, info.name().c_str(), IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	if (ioctl(_fd, SIOCGIFFLAGS, &ifr) < 0)
	  throw errnoexcept_t(std::string("ioctl(SIOCGIFFLAGS): ") +
			      strerror(errno), errno);
	if ((ifr.ifr_flags & IFF_PROMISC) == 0) {
	  ifr.ifr_flags |= IFF_PROMISC;
	  if (ioctl(_fd, SIOCSIFFLAGS, &ifr) < 0)
	    throw errnoexcept_t(std::string("ioctl(SIOCSIFFLAGS): ") +
				strerror(errno), errno);
	  ifr.ifr_flags &= ~IFF_PROMISC;
	  if (ioctl(_fd, SIOCSIFFLAGS, &ifr) < 0)
	    throw errnoexcept_t(std::string("ioctl(SIOCSIFFLAGS): ") +
				strerror(errno), errno);
	}
      }
#  endif
    }

    void
    Socket::set_priority() {
# if QOLYESTER_FAMILY_INET == 4
      char	tos = IPTOS_LOWDELAY;
      if (setsockopt(_fd, SOL_IP, IP_TOS, &tos, sizeof tos) < 0) {
	std::ostringstream	os;

	os << "setsockopt(SOL_IP, IP_TOS, " << tos << "): " << strerror(errno);

	throw errnoexcept_t(os.str(), errno);
      }
# endif
    }

    void
    Socket::bind(const sockaddr_t& sin) {
      if (::bind(_fd, (::sockaddr*) &sin, sizeof sin) < 0) {
	std::stringstream	msg;
	msg << "bind("
	    << address_t(sin) << ':' <<
#  if QOLYESTER_FAMILY_INET == 6
	  ntohs(sin.sin6_port)
#  else // QOLYESTER_FAMILY_INET != 6
	  ntohs(sin.sin_port)
#  endif
	    << "): "
	    << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }
    }

    void
    Socket::bind_multicast(const RealInterfaceInfo& info,
			   const address_t& a, port_t port) {
#  if QOLYESTER_FAMILY_INET == 6
      bind_to_device(info.name());
      _index = info.index();

      bind(a, port);

      set_multicast(info);

      int	hops = 1;

      if (setsockopt(_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
		     &hops, sizeof hops) < 0)
	throw errnoexcept_t(std::string("setsockopt(IPV6_MULTICAST_HOPS): ") +
			    strerror(errno), errno);

      ipv6_mreq	mreq;
      a.dump(reinterpret_cast<u_int8_t*>(&mreq.ipv6mr_multiaddr));
      mreq.ipv6mr_interface = info.index();

      if (setsockopt(_fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
		     &mreq, sizeof mreq) < 0) {
	std::stringstream	msg;
	msg << "setsockopt(IPV6_ADD_MEMBERSHIP, "
	    << a << ", " << info.index() << "): " << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }

      int	val = 0;

      if (setsockopt(_fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP,
		     &val, sizeof val) < 0)
	throw errnoexcept_t(std::string("setsockopt(IPV6_MULTICAST_LOOP): ") +
					strerror(errno), errno);

#  else // QOLYESTER_FAMILY_INET != 6

      bind(a, port);
      bind_to_device(info.name());
      set_multicast(info);

#  endif
    }

#  if QOLYESTER_FAMILY_INET == 6
    void
    Socket::bind(const addr_t& addr, port_t port, unsigned index) {
      sockaddr_t	sin = addr.make_sockaddr(port);
      sin.sin6_scope_id = index;
      bind(sin);
#  else // QOLYESTER_FAMILY_INET != 6
    void
    Socket::bind(const addr_t& addr, port_t port) {
      bind(addr.make_sockaddr(port));
#  endif
    }

    void
    Socket::connect(const sockaddr_t& sin) {
      if (::connect(_fd, (::sockaddr*) &sin, sizeof sin) < 0) {
	std::stringstream	msg;
	msg << "connect("
	    << address_t(sin) << ':' <<
#  if QOLYESTER_FAMILY_INET == 6
	  ntohs(sin.sin6_port)
#  else // QOLYESTER_FAMILY_INET != 6
	  ntohs(sin.sin_port)
#  endif
	    << "): "
	    << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }
    }

    void
    Socket::connect(const addr_t& addr, port_t port) {
      connect(addr.make_sockaddr(port));
    }

    utl::Data
    Socket::receive(address_t& sender) const {
      sockaddr_t	sin_from;
      socklen_t		sin_len = sizeof sin_from;
      utl::Data		data(_mtu);
      int		len;
      while ((len = ::recvfrom(_fd, data.raw(), data.size(), 0,
			       (sockaddr*) &sin_from, &sin_len)) < 0 and
	     errno == EINTR)
	;
      if (len < 0)
	throw errnoexcept_t(std::string("recvfrom(): ") + strerror(errno),
			    errno);

      sender = address_t(sin_from);

      return data.shrink_to(len);
    }

    void
    Socket::send(const utl::ConstData& d) const {
      int	len;
      while ((len = ::send(_fd, d.raw(), d.size(), 0)) < 0 and
	     errno == EINTR)
	;
      if (len < 0)
	throw errnoexcept_t(std::string("send(): ") + strerror(errno), errno);
    }

    void
    Socket::sendto(const utl::ConstData& d,
		   const address_t::sockaddr_t& s) const {
      int	len;
      while ((len = ::sendto(_fd, d.raw(), d.size(), 0,
			     reinterpret_cast<const sockaddr*>(&s),
			     sizeof s)) < 0 && errno == EINTR)
	;
      if (len < 0) {
	std::stringstream	msg;
	msg << "sendto("
	    << address_t(s) << ':' <<
#  if QOLYESTER_FAMILY_INET == 6
	  ntohs(s.sin6_port)
#  else // QOLYESTER_FAMILY_INET != 6
	  ntohs(s.sin_port)
#  endif
	    << "): "
	    << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }
    }

    void
    Socket::sendto_bcast(const utl::ConstData& d) const {
      address_t::sockaddr_t	sin = _baddr.make_sockaddr(OLSR_PORT_NUMBER);
#  if QOLYESTER_FAMILY_INET == 6
      sin.sin6_scope_id = _index;
#  endif // QOLYESTER_FAMILY_INET != 6
      int	len;
      while ((len = ::sendto(_fd, d.raw(), d.size(), 0,
			     reinterpret_cast<sockaddr*>(&sin),
			     sizeof (address_t::sockaddr_t))) < 0 and
	     errno == EINTR)
	;
      if (len < 0) {
	std::stringstream	msg;
	msg << "sendto("
	    << address_t(sin) << ':' <<
#  if QOLYESTER_FAMILY_INET == 6
	  ntohs(sin.sin6_port)
#  else // QOLYESTER_FAMILY_INET != 6
	  ntohs(sin.sin_port)
#  endif
	    << "): "
	    << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }
    }

    bool
    Socket::operator<(const This& rhs) const { return _fd < rhs._fd; }

    sch::IOEvent::p_t
    Socket::read_p() const {
      return sch::IOEvent::p_t(_fd, POLLIN);
    }

    sch::IOEvent::p_t
    Socket::write_p() const {
      return sch::IOEvent::p_t(_fd, POLLOUT);
    }

    void
    Socket::init() {
      if ((_fd = ::socket(addr_t::proto, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	throw errnoexcept_t(std::string("socket(SOCK_DGRAM, IPPROTO_UDP): ") +
			    strerror(errno), errno);
      if (::fcntl(_fd, F_SETFL, O_RDWR | O_NONBLOCK) < 0)
	throw errnoexcept_t(std::string("fcntl(F_SETFL, O_NONBLOCK): ") +
			    strerror(errno), errno);
    }

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_SYS_SOCKET_HXX

#endif // ! QOLYESTER_ENABLE_VIRTUAL
