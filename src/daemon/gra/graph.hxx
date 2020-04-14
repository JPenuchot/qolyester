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

#ifndef QOLYESTER_DAEMON_GRA_GRAPH_HXX
# define QOLYESTER_DAEMON_GRA_GRAPH_HXX 1

# include "config.hh"
# include <cassert>
# include "graph.hh"
# include "utl/mark.hh"

namespace olsr {

  extern utl::Mark	routes_recomp;

  namespace gra {

    // The general ctors are pretty straightforward.
    AdjInfo::AdjInfo()
      : _ep1(),
	_ep2(),
	_tag(topo)
    {}

    AdjInfo::AdjInfo(const address_t& ep1, const address_t& ep2, AdjTag tag)
      : _ep1(ep1),
	_ep2(ep2),
	_tag(tag)
    {}

    // The equality operator simply compares endpoints' addresses.
    bool		
    AdjInfo::operator==(const This& rhs) const {
      return _ep1 == rhs._ep1 && _ep2 == rhs._ep2 && _tag == rhs._tag;
    }
    
    const AdjInfo&
    AdjInfo::make_key(const address_t& ep1, const address_t& ep2, AdjTag tag) {
      const_cast<address_t&>(_dummy_for_find._ep1) = ep1;
      const_cast<address_t&>(_dummy_for_find._ep2) = ep2;
      const_cast<AdjTag&>(_dummy_for_find._tag) = tag;
      return _dummy_for_find;
    }

    AdjInfo	
    AdjInfo::invert(const This& x) {
      return This(x._ep2, x._ep1, x._tag);
    }

    // Again, the straightforward ctors
    AdjNode::AdjNode() : _endpoint(), _weight(0) {}
  
    AdjNode::AdjNode(const address_t& ep, unsigned w)
      : _endpoint(ep),
	_weight(w)
    {}

    // The equality operator makes a node unique by its address.
    bool
    AdjNode::operator==(const This& rhs) const {
      return _endpoint == rhs._endpoint;
    }

    const AdjNode&
    AdjNode::make_key(const address_t& ep) {
      const_cast<address_t&>(_dummy_for_find._endpoint) = ep;
      return _dummy_for_find;
    }

    // Also straightforward graph ctors
    AdjGraph::AdjGraph()
      : _aset(),
	_nset(),
	_apset_map()
    {}

    AdjGraph::AdjGraph(const This& other)
      : _aset(other._aset),
	_nset(other._nset),
	_apset_map() {
      // Update apset_map
      build_asetmap();
    }

    AdjGraph&
    AdjGraph::operator=(const This& other) {
      _aset = other._aset;
      _nset = other._nset;
      _apset_map.clear();
      // Update apset_map
      build_asetmap();
      return *this;
    }

    std::pair<AdjGraph::nset_t::const_iterator, bool>
    AdjGraph::insert_node(const AdjNode& n) {
      // First, insert the node into the node set.
      std::pair<nset_t::iterator, bool>	p =
 	nset_t::iterator::build(_nset.insert(n));
      // Then, if the node wasn't already in the set, create a new entry
      // in the apset_map.
      if (p.second) {
# ifndef NDEBUG
	assert(_apset_map.insert(apsetmap_t::value_type(&*p.first,
							apset_t())).second);
# else
	_apset_map.insert(apsetmap_t::value_type(&*p.first, apset_t()));
# endif
      }
      // Ensure that the apset_map is not corrupted.
      assert(_apset_map.find(&*p.first) != _apset_map.end());
      return p;
    }

    std::pair<AdjGraph::aset_t::const_iterator, bool>
    AdjGraph::insert_arc(const AdjInfo& a) {
      // First, find the endpoints.
      nset_t::const_iterator n1 = _nset.find(AdjNode::make_key(a.endpoint1()));
      nset_t::const_iterator n2 = _nset.find(AdjNode::make_key(a.endpoint2()));
      // Ensure that the endpoints exist.
      assert(n1 != _nset.end());
      assert(n2 != _nset.end());
      // Insert the arc
      std::pair<aset_t::iterator, bool>	p = _aset.insert(a);
      // If the arc wasn't already in the arc set, create two new
      // entries in the apset_map, one for each endpoint and mark the
      // routes for recalculation.
      if (p.second) {
	_apset_map[&*n1].insert(&*p.first);
	_apset_map[&*n2].insert(&*p.first);
      }
      return p;
    }      

    // This is a non-directed version, to quickly insert two arcs, one
    // in each direction.
    void
    AdjGraph::insert_edge(const AdjInfo& a) {
      insert_arc(a);
      insert_arc(AdjInfo::invert(a));
    }

    void
    AdjGraph::remove_arc(aset_t::iterator pos) {
      // Ensure that the iterator is not invalid (by far not exhaustive check).
      assert(pos != _aset.end());
      // Find the two endpoints.
      nset_t::const_iterator n1 = _nset.find(AdjNode::make_key(pos->endpoint1()));
      nset_t::const_iterator n2 = _nset.find(AdjNode::make_key(pos->endpoint2()));
      // Ensure that the enpoints exist.
      assert(n1 != _nset.end());
      assert(n2 != _nset.end());
      // Remove the arc from the apset_map.
      _apset_map[&*n1].erase(&*pos);
      _apset_map[&*n2].erase(&*pos);
      // Remove the arc from the arc set.
      _aset.erase(pos);
    }

    // This is a variant in case we don't have any iterator on the arc
    // we want to remove.
    void
    AdjGraph::remove_arc(const AdjInfo& a) {
      aset_t::iterator	pos = _aset.find(a);
      if (pos != _aset.end())
	remove_arc(pos);
    }

    // This is a non-directed version, to quickly remove two arcs, one
    // in each direction.
    void
    AdjGraph::remove_edge(aset_t::iterator pos) {
      remove_arc(AdjInfo::invert(*pos));
      remove_arc(pos);
    }

    // This is a variant in case we don't have any iterator on the arc
    // we want to remove.
    void
    AdjGraph::remove_edge(const AdjInfo& a) {
      remove_arc(a);
      remove_arc(AdjInfo::invert(a));
    }

    // Yet another variant of arc removal.  This time we want to remove
    // the endpoints if the removal of the arc leaves the node
    // unconnected.
    void
    AdjGraph::remove_arc_then_node(aset_t::iterator pos) {
      nset_t::iterator	n1 = _nset.find(AdjNode::make_key(pos->endpoint1()));
      nset_t::iterator	n2 = _nset.find(AdjNode::make_key(pos->endpoint2()));
      assert(n1 != _nset.end());
      assert(n2 != _nset.end());
      remove_arc(pos);
      if (_apset_map[&*n1].size() == 0)
	remove_node(n1);
      if (_apset_map[&*n2].size() == 0)
	remove_node(n2);
    }

    // A no-iterator variant of the yet-another-variant.
    void
    AdjGraph::remove_arc_then_node(const AdjInfo& a) {
      aset_t::iterator	pos = _aset.find(a);
      if (pos != _aset.end())
	remove_arc_then_node(pos);
    }

    // The non-directed variant of the yet-another-variant.
    void
    AdjGraph::remove_edge_then_node(aset_t::iterator pos) {
      remove_arc_then_node(AdjInfo::invert(*pos));
      remove_arc_then_node(pos);
    }

    // The non-directed-no-iterater one.
    void
    AdjGraph::remove_edge_then_node(const AdjInfo& a) {
      remove_arc_then_node(a);
      remove_arc_then_node(AdjInfo::invert(a));
    }
    
    // Node removal taking care of the removal of all the arcs which
    // endpoint was the removed node.
    void
    AdjGraph::remove_node(nset_t::iterator pos) {
      // Ensure the iterator is not invalid (by far not exhaustive check).
      assert(pos != _nset.end());  
      // Mark the routes to be recalculated.
      routes_recomp.set_mark();
      // Find the set of arcs which have the node as either endpoint.
      apsetmap_t::iterator	ap = _apset_map.find(&*pos);
      // Ensure that the apset_map is not corrupted.
      assert(ap != _apset_map.end());
      // Iterator over the set of arcs.
      apset_t&		pos_as = ap->second;
      for (apset_t::iterator i = pos_as.begin(); i != pos_as.end(); ++i) {
	// Get the other endpoint of the current arc.
	nset_t::const_iterator	other;
	if (pos->endpoint() == (*i)->endpoint1())
	  other = _nset.find(AdjNode::make_key((*i)->endpoint2()));
	else
	  other = _nset.find(AdjNode::make_key((*i)->endpoint1()));
	// Ensure that the other endpoint exists.
	assert(other != _nset.end());
	// Get the set of arcs of the other endpoint.
	apsetmap_t::iterator	ap2 = _apset_map.find(&*other);
	// Ensure that this set of arcs exists.
	assert(ap2 != _apset_map.end());
	// Remove the current arc from the other set.
	ap2->second.erase(*i);
	// Remove the current arc from the current set.
	_aset.erase(**i);
      }
      // Remove the node from the apset_map.
      _apset_map.erase(&*pos);
      // Remove the node from the node set.
      _nset.erase(pos);
    }

    // The no-iterator variant.
    void
    AdjGraph::remove_node(const AdjNode& n) {
      nset_t::iterator	pos = _nset.find(n);
      if (pos != _nset.end())
	remove_node(pos);
    }

    void
    AdjGraph::remove_node_if_alone(nset_t::iterator pos) {
      assert(pos != _nset.end());
      if (_apset_map[&*pos].empty())
	remove_node(pos);
    }

    // A convenience routine to remove a node if it is isolated.
    void
    AdjGraph::remove_node_if_alone(const AdjNode& n) {
      nset_t::iterator	pos = _nset.find(n);
      if (pos != _nset.end())
	remove_node_if_alone(pos);
    }
  
    // The routine to update the apset_map.
    void
    AdjGraph::build_asetmap() {
      // Iterate over the arc set.
      for (aset_t::const_iterator a = _aset.begin(); a != _aset.end(); ++a) {
	// Get the endpoints.
	nset_t::const_iterator n1 = _nset.find(AdjNode::make_key(a->endpoint1()));
	nset_t::const_iterator n2 = _nset.find(AdjNode::make_key(a->endpoint2()));
	// Ensure they exist.
	assert(n1 != _nset.end());
	assert(n2 != _nset.end());
	// Insert the arc into the apset_map for either endpoint.
	_apset_map[&*n1].insert(&*a);
	_apset_map[&*n2].insert(&*a);
      }
    }

  } // namespace gra
  
} // namespace olsr

#endif // ! QOLYESTER_DAEMON_GRA_GRAPH_HXX
