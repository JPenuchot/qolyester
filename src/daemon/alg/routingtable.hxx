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

#ifndef QOLYESTER_DAEMON_ALG_ROUTINGTABLE_HXX
# define QOLYESTER_DAEMON_ALG_ROUTINGTABLE_HXX 1

# include "alg/dijkstra.hh"
# include "set/hna.hh"
# include "set/mid.hh"
# include "set/routes.hh"
# include "set/neighbors.hh"
# include "sys/routing.hh"

# include "routingtable.hh"

namespace olsr {

  extern hnaset_t	hna_set;
  extern midset_t	mid_set;
  extern lrouteset_t	lroute_set;
  extern rrouteset_t	rroute_set;
  extern ifaceset_t	iface_set;
  extern bool		notables;

  namespace alg {

    // Route calculate and flush routine.
    // FIXME: mess left here.
    // FIXME: include multiple interface definitions inside the graph
    // and don't bother with them at this point.
    void	compute_routes()
    {
      debug << "CALCULATING ROUTING TABLE" << std::endl;

      using net::LocalRoute;
      using net::RemoteRoute;

      lrouteset_t		local_rs;
      rrouteset_t		remote_rs;

      std::set<address_t>	local_nodes;
      lmap_t			local_map;

      // Populate the local route set.
      for (cproxy_t::sym_neighborset_t::iterator n =
	     cproxy.sym_neighborset().begin();
	   n != cproxy.sym_neighborset().end(); ++n) {
	local_nodes.insert(n->main_addr());
	bool		found = false;
	address_t	remote_addr;
	for (set::Neighbor::linkset_t::iterator l = n->linkset().begin();
	     l != n->linkset().end(); ++l) {
	  assert(l->is_valid());
	  if (!l->is_sym())
	    continue;


# ifdef QOLYESTER_ENABLE_MID
	  local_rs.insert(LocalRoute(l->remote_addr(), 8 * ADDRESS_SIZE,
				     iface_set[l->local_addr()].info()));
# else // ! QOLYESTER_ENABLE_MID
	  local_rs.insert(LocalRoute(l->remote_addr(), 8 * ADDRESS_SIZE,
				     this_interface.info()));
# endif

	  if (!found) {
	    found = true;
	    remote_addr = l->remote_addr();
	  }

	  local_map.insert(std::make_pair(l->remote_addr(),
					  std::make_pair(remote_addr, n)));
	}
	assert(found);
	if (!local_rs[LocalRoute::make_key(n->main_addr(),
					   8 * ADDRESS_SIZE)]) {
	  remote_rs.insert(RemoteRoute(n->main_addr(), 8 * ADDRESS_SIZE,
				       remote_addr));
	  local_map.insert(std::make_pair(n->main_addr(),
					  std::make_pair(remote_addr, n)));
	} else
	  local_map.insert(std::make_pair(n->main_addr(),
					  std::make_pair(n->main_addr(), n)));

      }

      // Apply Dijkstra to populate the remote route set.
      alg::dijkstra(remote_rs, local_map);

# ifdef QOLYESTER_ENABLE_MID
      // Complete the sets with multiple interface information.
      for (midset_t::midset_t::iterator i = mid_set.midset().begin();
	   i != mid_set.midset().end(); ++i) {
	lrouteset_t::iterator	lr =
	  local_rs.find(LocalRoute::make_key(i->main_addr(),
					     8 * ADDRESS_SIZE));
	if (lr != local_rs.end()) {
	  // node in 1-hop
	  if (!local_rs[LocalRoute::make_key(i->iface_addr(),
					     8 * ADDRESS_SIZE)])
	    remote_rs.insert(RemoteRoute(i->iface_addr(), 8 * ADDRESS_SIZE,
					 i->main_addr()));
	} else {
	  rrouteset_t::iterator	rr =
	    remote_rs.find(RemoteRoute::make_key(i->main_addr(),
						 8 * ADDRESS_SIZE));
	  if (rr != remote_rs.end())
	    if (!local_rs[LocalRoute::make_key(i->iface_addr(),
					       8 * ADDRESS_SIZE)])
	      remote_rs.insert(RemoteRoute(i->iface_addr(), 8 * ADDRESS_SIZE,
					   rr->next_addr()));
	}
      }
# endif // ! QOLYESTER_ENABLE_MID

# ifdef QOLYESTER_ENABLE_HNA
      // Add routes to non-OLSR networks.
      for (hnaset_t::hnaset_t::iterator i = hna_set.hnaset().begin();
	   i != hna_set.hnaset().end(); ++i) {
	lrouteset_t::iterator	lr =
	  local_rs.find(LocalRoute::make_key(i->gw_addr(), 8 * ADDRESS_SIZE));
	if (lr != local_rs.end())
	  remote_rs.insert(RemoteRoute(i->net_addr(), i->prefix(),
				       lr->dest_addr()));
	else {
	  rrouteset_t::iterator	rr =
	    remote_rs.find(RemoteRoute::make_key(i->gw_addr(),
						 8 * ADDRESS_SIZE));
	  if (rr != remote_rs.end())
	    remote_rs.insert(RemoteRoute(i->net_addr(), i->prefix(),
					 rr->next_addr()));
	}
      }
# endif // ! QOLYESTER_ENABLE_HNA

# ifdef DEBUG
      debug << "Route set change from {\n";
      for (lrouteset_t::const_iterator i = lroute_set.begin();
	   i != lroute_set.end(); ++i) {
	debug << "  " << i->dest_addr() << '/' << i->prefix() << " : "
	      << i->interface_info().name() << '\n';
      }
      for (rrouteset_t::const_iterator i = rroute_set.begin();
	   i != rroute_set.end(); ++i)
	debug << "  " << i->dest_addr() << '/' << i->prefix() << " : "
	      << i->next_addr() << '\n';
      debug << "} to {\n";
      for (lrouteset_t::const_iterator i = local_rs.begin();
	   i != local_rs.end(); ++i) {
	debug << "  " << i->dest_addr() << '/' << i->prefix() << " : "
	      << i->interface_info().name() << '\n';
      }
      for (rrouteset_t::const_iterator i = remote_rs.begin();
	   i != remote_rs.end(); ++i)
	debug << "  " << i->dest_addr() << '/' << i->prefix() << " : "
	      << i->next_addr() << '\n';
      debug << '}' << std::endl;
# endif // !DEBUG

      // Flush the changes into the system.

      flush_routes(local_rs, remote_rs);
    }

    void	flush_routes(const lrouteset_t& local_rs,
			     const rrouteset_t& remote_rs)
    {
      lrouteset_t	addlroutes = local_rs - lroute_set;
      rrouteset_t	addrroutes = remote_rs - rroute_set;
      rrouteset_t	delrroutes = rroute_set - remote_rs;
      lrouteset_t	dellroutes = lroute_set - local_rs;

      debug << "Flushing routes {\n";

      if (!notables) {
	sys::RoutingActions	ra;

	ra.print_kernel_routes();

	// Remove old remote routes
	for (rrouteset_t::iterator r = delrroutes.begin();
	     r != delrroutes.end(); ++r) {
	  debug << "  - " << r->dest_addr() << '/' << r->prefix()
		<< " : " << r->next_addr() << '\n';
	  ra.remove_remote_route(*r);
	}

	// Remove old local routes
	for (lrouteset_t::iterator r = dellroutes.begin();
	     r != dellroutes.end(); ++r) {
	  debug << "  - " << r->dest_addr() << '/' << r->prefix()
		<< " : " << r->interface_info().name() << '\n';
	  ra.remove_local_route(*r);
	}

	// Add new local routes
	for (lrouteset_t::iterator r = addlroutes.begin();
	     r != addlroutes.end(); ++r) {
	  debug << "  + " << r->dest_addr() << '/' << r->prefix()
		<< " : " << r->interface_info().name() << '\n';
	  ra.add_local_route(*r);
	}

	// Add new remote routes
	for (rrouteset_t::iterator r = addrroutes.begin();
	     r != addrroutes.end(); ++r) {
	  debug << "  + " << r->dest_addr() << '/' << r->prefix()
		<< " : " << r->next_addr() << '\n';
	  ra.add_remote_route(*r);
	}

	ra.print_kernel_routes();

      }

      debug << '}' << std::endl;

      rroute_set = remote_rs;
      lroute_set = local_rs;
    }

    void	clean_routes()
    {
      if (!notables) {
	sys::RoutingActions	ra;
	debug << "Removing old routes";
	ra.remove_old_routes();
	debug << std::endl;
      }
    }

  } // namespace alg

} // namespace olsr

#endif // !QOLYESTER_DAEMON_ALG_ROUTINGTABLE_HXX
