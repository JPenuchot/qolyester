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

#ifndef QOLYESTER_DAEMON_ALG_DIJKSTRA_HXX
# define QOLYESTER_DAEMON_ALG_DIJKSTRA_HXX 1

# include <ext/hash_map>

# include "gra/graph.hh"
# include "net/ipaddress.hh"
# include "utl/comparator.hh"
# include "utl/log.hh"

# include "dijkstra.hh"

namespace olsr {

  extern debug_ostream_t	debug;
  extern gra::AdjGraph		path_net;

  namespace alg {
    // Declarations of the two dijkstra data structures.  nextmap_t
    // maps node addresses into addresses of the next node towards the
    // first node.  hopsmap_t maps node addresses into the actual
    // number of hops.  Infinite number of hops is reprensented by the
    // absence of a node in the hopsmap.
    typedef std::hash_map<address_t, unsigned>	weightmap_t;
    typedef std::hash_map<address_t, address_t>	nextmap_t;
    typedef std::hash_map<address_t, unsigned>	hopsmap_t;

    // Sorting comparator of the nodes remaining to be processed.
    struct sort_less {
      // We need to pass a reference to the instance of hopsmap_t to
      // use.
      sort_less(const hopsmap_t& hm,
		const weightmap_t& wm)
	: _hm(hm),
	  _wm(wm)
      {}

      bool operator()(const gra::AdjNode* a, const gra::AdjNode* b) const {
	// Get the number of hops towards each of the nodes.
	weightmap_t::const_iterator	wa = _wm.find(a->endpoint());
	weightmap_t::const_iterator	wb = _wm.find(b->endpoint());

	hopsmap_t::const_iterator     ha = _hm.find(a->endpoint());
	hopsmap_t::const_iterator     hb = _hm.find(b->endpoint());

	// If both of the nodes are at infinite number of hops, order
	// simply by node address.  If only one of the two is at
	// infinity, make it greater than the other.
	if (wb == _wm.end()) {
	  if (wa == _wm.end())
	    return a->endpoint() < b->endpoint();
	  else
	    return true;
	}

	if (wa == _wm.end())
	  return false;

	if (wa->second > wb->second)
	  return true;
	else if (wa->second < wb->second)
	  return false;

	if (hb == _hm.end()) {
	  if (ha == _hm.end())
	    return a->endpoint() < b->endpoint();
	  else
	    return true;
	}

	if (ha == _hm.end())
	  return false;

	// From now on, we know that both nodes are not at infinity.

	// Order the nodes primary by number of hops.
	if (ha->second < hb->second)
	  return true;
	else if (hb->second < ha->second)
	  return false;

	// Order the nodes secondary by weight.
	if (b->weight() < a->weight())
	  return true;
	else if (a->weight() < b->weight())
	  return false;

	// Last, order the nodes by address.
	return a->endpoint() < b->endpoint();
      }
    private:
      const hopsmap_t&		_hm;
      const weightmap_t&	_wm;
    };

    // Declaration of the node hash set type.
    typedef std::hash_set<const gra::AdjNode*,
			  gra::hash_AdjNode,
			  utl::pequal_to<gra::AdjNode> >	npset_t;

    // Declaration of the remaining node set type.
    typedef std::set<const gra::AdjNode*, sort_less>	sortremaining_t;

    // The Dijkstra algorithm.  The first argument is a reference to
    // the route set to be filled and the second argument is the set
    // of source nodes addresses.
    void        dijkstra(rrouteset_t& routes, lmap_t& sources_map) {
      nextmap_t		next_map;
      hopsmap_t		hops_map;
      weightmap_t	weight_map;

      debug << path_net << std::endl;

      npset_t	rem;

      // Insert all the nodes of the graph into the remaining set.
      for (gra::nodeset_t::const_iterator n = path_net.nodes().begin();
	   n != path_net.nodes().end(); ++n)
	rem.insert(&*n);

      debug << "dijkstra {\n";

      // Initialize the hops and next map, by inserting the source
      // nodes.
      debug << "  sources {";
      for (lmap_t::const_iterator s = sources_map.begin();
	   s != sources_map.end(); ++s) {
	pathnet_t::nset_t::const_iterator	n =
	  path_net.nodes().find(gra::AdjNode::make_key(s->first));

	if (n == path_net.nodes().end())
	  n = path_net.nodes().
	    find(gra::AdjNode::make_key(s->second.second->main_addr()));

	assert(n != path_net.nodes().end());

	debug << ' ' << s->first;
        next_map[s->first] = s->first;
        hops_map[s->first] = 0;
	weight_map[s->first] = n->weight();
      }
      debug << " }\n";

      // Loop while there are remaining nodes to be reached.
      while (!hops_map.empty() && !rem.empty()) {

	// Populate the sorted remaining set.
	sortremaining_t	srem(rem.begin(), rem.end(),
			     sort_less(hops_map, weight_map));

	debug << "  round {\n";
	for (sortremaining_t::const_iterator i = srem.begin();
	     i != srem.end(); ++i) {
	  debug << "    " << (*i)->endpoint() << ' ';
	  nextmap_t::const_iterator n = next_map.find((*i)->endpoint());
	  hopsmap_t::const_iterator h = hops_map.find((*i)->endpoint());
	  weightmap_t::const_iterator w = weight_map.find((*i)->endpoint());
	  if (n == next_map.end())
	    debug << "_ ";
	  else
	    debug << n->second << ' ';
	  if (h == hops_map.end())
	    debug << "_ ";
	  else
	    debug << h->second << ' ';
	  if (w == weight_map.end())
	    debug << "_\n";
	  else
	    debug << w->second << '\n';
	}
	debug << "  }" << std::endl;

	// The first node (in iteration order) in the sorted remaining
	// set is the node that is to be added next to the reached
	// set.
	const gra::AdjNode&	closest = **srem.begin();

	// Remove the node from the remaining set.
	rem.erase(&closest);
	srem.erase(&closest);

	// Get new hop count and next node address for relaxation.

	hopsmap_t::iterator	hops = hops_map.find(closest.endpoint());
	nextmap_t::iterator	next = next_map.find(closest.endpoint());
	weightmap_t::iterator	weight = weight_map.find(closest.endpoint());

	if (hops == hops_map.end() ||
	    next == next_map.end() ||
	    weight == weight_map.end())
	  break; // We are now considering an unreachable node, stop here.

	unsigned	next_hops = hops->second + 1;

	// Remove the node from the hops map.
	hops_map.erase(hops);
	weight_map.erase(weight);

	if (closest.weight() == 0)
	  continue;

	// The relaxation /per se/

	// Iterate on the node set.
	for (npset_t::const_iterator n = rem.begin(); n != rem.end(); ++n) {
	  // Consider only the nodes that are adjacent to the current
	  // node.
	  pathnet_t::aset_t::const_iterator	a =
	    path_net.arcs().find(gra::AdjInfo::make_key(closest.endpoint(),
							(*n)->endpoint(),
							topo));
	  // The following case is put to allow the use of 2-hop
	  // neighbors information on the condition that the 1-hop
	  // neighbor is symmetric, i.e. it is one of the source
	  // nodes.
	  if (a == path_net.arcs().end() &&
	      sources_map.find(closest.endpoint()) != sources_map.end())
	    a = path_net.arcs().find(gra::AdjInfo::make_key(closest.endpoint(),
							    (*n)->endpoint(),
							    twohop));

	  if (a != path_net.arcs().end()) {
	    // Get the hops map entry and update it, along with the
	    // next map entry.
	    unsigned			next_weight = MAX(weight->second,
							  (*n)->weight());
	    //	      weight->second + a->weight();
	    hopsmap_t::iterator		h = hops_map.find((*n)->endpoint());
	    weightmap_t::iterator	w = weight_map.find((*n)->endpoint());
	    if (((w == weight_map.end() || w->second <= next_weight) &&
		 (h == hops_map.end() || h->second > next_hops)) ||
		((w == weight_map.end() || w->second < next_weight) &&
		 (h == hops_map.end() || h->second >= next_hops))) {
	      weight_map[(*n)->endpoint()] = next_weight;
	      hops_map[(*n)->endpoint()] = next_hops;
	      next_map[(*n)->endpoint()] = next->second;
	    }
	  }
	}
      }

      // Remove the source nodes from the next map.
      for (lmap_t::const_iterator s = sources_map.begin();
	   s != sources_map.end(); ++s)
	next_map.erase(s->first);

      // Build the route set based on the next map.
      for (nextmap_t::const_iterator i = next_map.begin();
	   i != next_map.end(); ++i) {
	debug << "  route { " << i->first << " : " << i->second << " }\n";

	assert(sources_map.find(i->second) != sources_map.end());
	routes.insert(net::RemoteRoute(i->first, ADDRESS_SIZE * 8,
				       sources_map[i->second].first));
      }

      debug << '}' << std::endl;;
    }

  } // namespace alg

} // namespace olsr

#endif // !QOLYESTER_DAEMON_ALG_DIJKSTRA_HXX
