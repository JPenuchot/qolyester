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

#ifndef QOLYESTER_DAEMON_SYS_ROUTING_HXX
# define QOLYESTER_DAEMON_SYS_ROUTING_HXX 1

# include <stdexcept>
# include <ostream>
# include "routing.hh"
# include "utl/log.hh"

namespace olsr {

  namespace sys {

    void
    RoutingActions::print_kernel_routes() {
      _rs.print_kernel_routes();
    }

    void
    RoutingActions::remove_old_routes() {
      try {
	_rs.remove_old_routes();
      } catch (std::runtime_error& e) {
	error << e.what() << std::endl;
      }
    }

    void
    RoutingActions::add_local_route(const net::LocalRoute& r) {
      try {
	_rs.add_local_route(r.dest_addr(), r.prefix(), r.interface_info());
      } catch (std::runtime_error& e) {
	error << e.what() << std::endl;
      }
    }

    void
    RoutingActions::add_remote_route(const net::RemoteRoute& r) {
      try {
	_rs.add_remote_route(r.dest_addr(), r.prefix(), r.next_addr());
      } catch (std::runtime_error& e) {
	error << e.what() << std::endl;
      }
    }

    void
    RoutingActions::remove_local_route(const net::LocalRoute& r) {
      try {
	_rs.remove_local_route(r.dest_addr(), r.prefix(), r.interface_info());
      } catch (std::runtime_error& e) {
	error << e.what() << std::endl;
      }
    }

    void
    RoutingActions::remove_remote_route(const net::RemoteRoute& r) {
      try {
	_rs.remove_remote_route(r.dest_addr(), r.prefix(), r.next_addr());
      } catch (std::runtime_error& e) {
	error << e.what() << std::endl;
      }
    }

  } // namespace sys

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SYS_ROUTING_HXX
