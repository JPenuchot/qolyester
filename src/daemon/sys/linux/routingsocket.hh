// Copyright (C) 2003-2006, Laboratoire de Recherche en Informatique

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

# ifndef QOLYESTER_DAEMON_SYS_LINUX_ROUTINGSOCKET_HH
#  define QOLYESTER_DAEMON_SYS_LINUX_ROUTINGSOCKET_HH 1

#  include "net/ipaddress.hh"
#  include "sys/linux/interfacedesc.hh"
#  include "sys/linux/netlink.hh"

namespace olsr {

  namespace sys {

    class RealInterfaceInfo;

#  ifdef DEBUG

    namespace netlink {

      class RoutingTableVisitor : public DefaultVisitor {
	typedef RoutingTableVisitor			This;
      public:
	RoutingTableVisitor(std::ostream& os);
	virtual ~RoutingTableVisitor();

	virtual void visit(const NLError& e);
	virtual void visit(const NLNewRoute& e);
	virtual void visit(const NLRouteAttrDestination& e);
	virtual void visit(const NLRouteAttrGateway& e);
	virtual void visit(const NLRouteAttrOutInterface& e);
      private:
	std::ostream&	os_;
	address_t	dst_;
	address_t	via_;
	bool		is_via_;
	unsigned	index_;
	bool		is_index_;
      };

    } // namespace netlink

#  endif // !DEBUG

    namespace internal {

      class RoutingSocket {
      public:
	void	print_kernel_routes();
	void	remove_old_routes();
	void	add_local_route(const address_t& d, unsigned p,
				const sys::RealInterfaceInfo& i);
	void	add_remote_route(const address_t& d, unsigned p,
				 const address_t& n);
	void	remove_local_route(const address_t& d, unsigned p,
				   const sys::RealInterfaceInfo& i);
	void	remove_remote_route(const address_t& d, unsigned p,
				    const address_t& n);
	void	add_addr(unsigned i, const InterfaceAddress& a);
	void	del_addr(unsigned i, const InterfaceAddress& a);
      private:
	netlink::NLSocket	_s;
      };

    } // namespace internal

  } // namespace sys

} // namespace olsr

#  include "routingsocket.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_LINUX_ROUTINGSOCKET_HH

#endif // ! QOLYESTER_ENABLE_VIRTUAL
