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

# include "alg/routingtable.hh"

#ifndef QOLYESTER_DAEMON_SYS_ROUTING_HH
# define QOLYESTER_DAEMON_SYS_ROUTING_HH 1

# include "config.hh"
# include "net/route.hh"
# ifndef QOLYESTER_ENABLE_VIRTUAL
#  include "linux/routingsocket.hh"
# else
#  include "virtual/routingsocket.hh"
# endif

namespace olsr {

  namespace sys {

    class RoutingActions {
    public:
      inline void	print_kernel_routes();
      inline void	remove_old_routes();
      inline void	add_local_route(const net::LocalRoute& r);
      inline void	add_remote_route(const net::RemoteRoute& r);
      inline void	remove_local_route(const net::LocalRoute& r);
      inline void	remove_remote_route(const net::RemoteRoute& r);
    private:
      internal::RoutingSocket	_rs;
    };

  } // namespace sys

} // namespace olsr

# include "routing.hxx"

#endif // ! QOLYESTER_DAEMON_SYS_ROUTING_HH
