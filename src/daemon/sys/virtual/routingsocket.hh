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
/// @file   virtual/routingsocket.hh
/// @author Benoit Bourdin
/// @date   Tue Jun 02 2004
///
/// @brief  empty definition for compatibility

#include "config.hh"

#ifdef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_VIRTUAL_ROUTINGSOCKET_HH
#  define QOLYESTER_DAEMON_SYS_VIRTUAL_ROUTINGSOCKET_HH 1

#  include "net/ipaddress.hh"
#  include "sys/virtual/interfacedesc.hh"
#  include "sys/virtualinterfaceinfo.hh"

namespace olsr {

  namespace sys {

    namespace internal {

      ///
      /// @class RoutingSocket
      /// @brief dummy class for compatibity with other sys classes
      ///
      class RoutingSocket {
      public:
	void	print_kernel_routes();
	void	remove_old_routes();
	void	add_local_route(const address_t&, unsigned,
				const sys::VirtualInterfaceInfo&);
	void	add_remote_route(const address_t&, unsigned,
				 const address_t&);
	void	remove_local_route(const address_t&, unsigned,
				   const sys::VirtualInterfaceInfo&);
	void	remove_remote_route(const address_t&, unsigned,
				    const address_t&);
	void	add_addr(unsigned, const InterfaceAddress&);
	void	del_addr(unsigned, const InterfaceAddress&);
      };

    } // namespace internal

  } // namespace sys

} // namespace olsr

#  include "routingsocket.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_VIRTUAL_ROUTINGSOCKET_HH

#endif // QOLYESTER_ENABLE_VIRTUAL
