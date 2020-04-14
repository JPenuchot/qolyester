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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

///
/// @file   daemon/gra/graph.hh
/// @author Ignacy Gawedzki
/// @date   Thu Sep 25 20:54:36 2003
///
/// @brief  Graph-specific declarations.
///
/// The graph is defined as an adjacency hash table.  The graph @e per
/// @e se is a hash table of nodes and a hash table of arcs.  An
/// additional hash map is used to find the subset of arcs connecting
/// a given node in either direction.
///

#ifndef QOLYESTER_DAEMON_GRA_GRAPH_HH
# define QOLYESTER_DAEMON_GRA_GRAPH_HH 1

# include <string>
# include <ext/hash_map>
# include <sstream>
# include <ostream>
# include "net/ipaddress.hh"
# include "utl/iterator.hh"
# include "utl/set.hh"

// These are the graph-specific declarations.  The directed graph is
// implemented as an adjacency hash table, to allow quick access.

namespace olsr {

  namespace gra {

    enum AdjTag { topo = 0, twohop = 1 };

    ///
    /// @class	AdjInfo
    /// @brief	An arc of the graph.
    ///
    /// It is aimed to be augmented with additional attributes, to
    /// reflect link state.
    ///
    class AdjInfo
    {
      typedef AdjInfo	This;

      ///
      /// The private constructor.
      ///
      /// It is defined solely for the initialization of
      /// @link olsr::gra::AdjInfo::_dummy_for_find _dummy_for_find@endlink.
      ///
      explicit AdjInfo();
    public:
      ///
      /// The general constructor.
      ///
      /// @arg ep1 first endpoint
      /// @arg ep2 second endpoint
      /// @arg tag tag of the arc
      ///
      inline AdjInfo(const address_t& ep1, const address_t& ep2,
		     AdjTag tag = topo);

      ///
      /// First endpoint accessor.
      ///
      /// @return Reference to first endpoint
      ///
      const address_t&	endpoint1() const { return _ep1; }

      ///
      /// Second endpoint accessor.
      ///
      /// @return Reference to second endpoint
      ///
      const address_t&	endpoint2() const { return _ep2; }

      AdjTag		tag() const { return _tag; }

      ///
      /// Equality operator.  Compares only endpoint addresses for the
      /// moment.  Defined maily for hashing structures.
      ///
      /// @arg rhs right-hand-size operand
      ///
      /// @return true if equal, false otherwise
      ///
      inline bool	operator==(const This& rhs) const;

      std::string	to_string() const
      {
	std::stringstream	s;
	s << "{ " << _ep1 << " -- " << _tag << " --> " << _ep2 << " }";
	return s.str();
      }

      ///
      ///	Key builder for find operations.
      ///
      /// @arg ep1 first endpoint
      /// @arg ep2 second endpoint
      /// @arg tag tag of the arc
      ///
      /// @return Reference to modified @link olsr::gra::AdjInfo::_dummy_for_find _dummy_for_find@endlink.
      ///
      static inline const This&	make_key(const address_t& ep1,
					 const address_t& ep2,
					 AdjTag tag = topo);

      ///
      /// Arc inverter.
      /// @arg x The arc to invert
      ///
      /// @return The inverted arc
      ///
      static inline This	invert(const This& x);
    private:
      const address_t	_ep1;	///< First endpoint
      const address_t	_ep2;	///< Second endpoint
      const AdjTag	_tag;

      static This	_dummy_for_find; ///< Static arc for key generation
    };

    ///
    /// @class	hash_AdjInfo
    /// @brief	Hashing functor for @link AdjInfo AdjInfo@endlink.
    ///
    struct hash_AdjInfo
    {
      ::size_t operator()(const AdjInfo& a) const
      {
	return std::hash<address_t>()(a.endpoint1()) ^
	  std::hash<address_t>()(a.endpoint2());
      }
      ::size_t operator()(const AdjInfo* a) const
      {
	return std::hash<address_t>()(a->endpoint1()) ^
	  std::hash<address_t>()(a->endpoint2());
      }
    };

    ///
    /// @class	AdjNode
    /// @brief	A Node of the graph.
    ///
    /// It is aimed to be augmented with information about nodes.
    ///
    class AdjNode
    {
      typedef AdjNode					This;

      ///
      /// The private constructor.
      ///
      /// It is defined solely for the initialization of
      /// @link olsr::gra::AdjNode::_dummy_for_find _dummy_for_find@endlink.
      ///
      explicit inline AdjNode();
    public:
      /// The general constructor.
      ///
      /// @arg ep endpoint
      /// @arg w weight
      ///
      inline AdjNode(const address_t& ep, unsigned w = 1);

      ///
      /// Endpoint accessor.
      ///
      /// @return reference to endpoint
      ///
      const address_t&	endpoint() const { return _endpoint; }
      ///
      /// Weight accessor.
      ///
      /// @return weight
      ///
      unsigned		weight() const { return _weight; }

      ///
      /// Weight modifier.
      ///
      /// @arg w new weight
      ///
      void		set_weight(unsigned w) { _weight = w; }

      ///
      /// Equality operator.  Compares only endpoint addresses for the
      /// moment.  Defined mainly for hash structures.
      ///
      /// @arg rhs right-hand-side operand
      ///
      /// @return true if equal, false if not
      ///
      inline bool	operator==(const This& rhs) const;

      std::string	to_string() const
      {
	std::stringstream	s;
	s << "{ " << _endpoint << ' ' << _weight << " }";
	return s.str();
      }

      ///
      /// Key builder for find operations.
      ///
      /// @arg ep endpoint.
      ///
      /// @return reference to modified @link olsr::gra::AdjNode::_dummy_for_find
      /// _dummy_for_find@endlink.
      ///
      static inline const This&	make_key(const address_t& ep);
    private:
      const address_t	_endpoint; ///< Endpoint
      unsigned		_weight; ///< Weight

      static This	_dummy_for_find; ///< Static node for key generation
    };

    ///
    /// @class	hash_AdjNode
    /// @brief	Hashing functor for @link AdjNode AdjNode@endlink.
    ///
    struct hash_AdjNode
    {
      ::size_t operator()(const AdjNode& a) const
      {
	return std::hash<address_t>()(a.endpoint());
      }
      ::size_t operator()(const AdjNode* a) const
      {
	return std::hash<address_t>()(a->endpoint());
      }
    };

    namespace internal {
      typedef std::hash_set<AdjInfo, hash_AdjInfo>	iset_t;
      typedef iset_t::const_iterator			iset_const_iterator;
      typedef utl::DeconstIterator<iset_const_iterator> iset_iterator;

      typedef std::hash_set<AdjNode, hash_AdjNode>	nset_t;
      typedef nset_t::const_iterator			nset_const_iterator;
      typedef utl::DeconstIterator<nset_const_iterator> nset_iterator;
    } // namespace internal

    ///
    /// @typedef utl::Set<AdjInfo> arcset_t
    /// @brief	Set of arcs implemented as a hash set.
    ///
    typedef utl::Set<AdjInfo, internal::iset_t, internal::iset_iterator>
    arcset_t;

    ///
    /// @typedef utl::Set<AdjNode> nodeset_t
    /// @brief	Set of nodes implemented as a hash set.
    ///
    typedef utl::Set<AdjNode, internal::nset_t, internal::nset_iterator>
    nodeset_t;

    // This is graph /per se/.
    ///
    /// @class AdjGraph
    /// @brief	The graph @e per @e se.
    ///
    class AdjGraph
    {
      typedef AdjGraph		This;

      // We need a way to find all the arcs with a given node as either
      // endpoint quickly.  So we want a hash map (apsetmap_t)
      // associating a pointer to a node to a hash set of pointers to
      // arcs (apset_t).

      ///
      /// @typedef std::hash_set<const AdjInfo*> apset_t
      /// @brief	Set of pointers to arc.
      ///
      /// Used to build subsets of the set of arcs.
      ///
      typedef std::hash_set<const AdjInfo*, hash_AdjInfo,
			    utl::pequal_to<AdjInfo> >      apset_t;

      ///
      /// @typedef std::hash_map<const AdjNode*, apset_t>	apsetmap_t;
      /// @brief	Map from node pointer to @link olsr::gra::AdjGraph::apset_t apset_t@endlink.
      ///
      /// Used to find easily the set of arcs departing of or arriving
      /// at a given node.
      ///
      typedef std::hash_map<const AdjNode*, apset_t, hash_AdjNode,
			    utl::pequal_to<AdjNode> >      apsetmap_t;
    public:
      // The sets of actual nodes and arcs
      ///
      /// @typedef arcset_t aset_t
      /// @brief	Convenience alias.
      ///
      typedef arcset_t		aset_t;

      ///
      /// @typedef nodeset_t nset_t
      /// @brief	Convenience alias.
      ///
      typedef nodeset_t		nset_t;

      std::string	to_string() const
      {
	std::stringstream	s;
	s << "AdjGraph {\n";
	if (_nset.empty())
	  s << "  nodes {}\n";
	else {
	  s << "  nodes {\n";
	  for (nset_t::const_iterator i = _nset.begin(); i != _nset.end(); ++i)
	    s << "    " << i->to_string() << '\n';
	  s << "  }\n";
	}
	if (_aset.empty())
	  s << "  arcs {}\n";
	else {
	  s << "  arcs {\n";
	  for (aset_t::const_iterator i = _aset.begin(); i != _aset.end(); ++i)
	    s << "    " << i->to_string() << '\n';
	  s << "  }\n";
	}
	s << '}';
	return s.str();
      }

      ///
      /// Graph constructor
      ///
      inline AdjGraph();

      ///
      /// Graph copy constructor
      ///
      inline AdjGraph(const This& other);

      ///
      /// Graph assignment operator
      ///
      inline This&	operator=(const This& other);

      ///
      /// Accessor to the set of arcs.
      ///
      /// @return reference to the set of arcs
      ///
      const aset_t&	arcs() const { return _aset; }

      ///
      /// Accessor to the set of nodes.
      ///
      /// @return reference to the set of nodes
      ///
      const nset_t&	nodes() const { return _nset; }

      ///
      /// Node insertion method.
      ///
      /// @arg n node to be inserted
      ///
      inline
      std::pair<nset_t::const_iterator, bool>	insert_node(const AdjNode& n);

      ///
      /// Arc insertion method.
      ///
      /// @arg a arc to be inserted
      ///
      inline
      std::pair<aset_t::const_iterator, bool>	insert_arc(const AdjInfo& a);

      ///
      /// Edge insertion method.  Inserts both the arc and the inverted arc.
      ///
      /// @arg a the arc to be inserted
      ///
      inline void	insert_edge(const AdjInfo& a);

      ///
      /// Arc removal method.
      ///
      /// @arg pos iterator to the arc to be removed
      ///
      inline void	remove_arc(aset_t::iterator pos);

      ///
      /// Arc removal method.
      ///
      /// @arg a reference to the arc to be removed
      ///
      inline void	remove_arc(const AdjInfo& a);

      ///
      /// Edge removal method.  Removes both the arc and the inverted arc.
      ///
      /// @arg pos iterator to the arc to be removed
      ///
      inline void	remove_edge(aset_t::iterator pos);

      ///
      /// Edge removal method.  Removed both the arc and the inverted arc.
      ///
      /// @arg a reference to the arc to be removed
      ///
      inline void	remove_edge(const AdjInfo& a);

      ///
      /// Arc removal method.  Removes the endpoint nodes if they don't
      /// belong to other arcs.
      ///
      /// @arg pos iterator to the arc to be removed
      ///
      inline void	remove_arc_then_node(aset_t::iterator pos);

      ///
      /// Arc removal method.  Removes the endpoint nodes if they don't
      /// belong to other arcs.
      ///
      /// @arg a reference to the arc to be removed
      ///
      inline void	remove_arc_then_node(const AdjInfo& a);

      ///
      /// Edge removal method.  Removed both the arc and the inverted
      /// arc.  Removes the endpoint nodes if they don't belong to other
      /// arcs.
      ///
      /// @arg pos iterator to the arc to be removed
      ///
      inline void	remove_edge_then_node(aset_t::iterator pos);

      ///
      /// Edge removal method.  Removed both the arc and the inverted
      /// arc.  Removes the endpoint nodes if they don't belong to other
      /// arcs.
      ///
      /// @arg a reference to the arc to be removed
      ///
      inline void	remove_edge_then_node(const AdjInfo& a);

      ///
      /// Node removal method.
      ///
      /// @arg pos iterator to the node to be removed
      ///
      inline void	remove_node(nset_t::iterator pos);

      ///
      /// Node removal method.
      ///
      /// @arg n reference to the node to be removed
      ///
      inline void	remove_node(const AdjNode& n);

      inline void	remove_node_if_alone(nset_t::iterator pos);

      ///
      /// Non-connected-node removal method.
      ///
      /// @arg n reference to the node to be removed
      ///
      inline void	remove_node_if_alone(const AdjNode& n);

    private:
      ///
      /// @link olsr::gra::AdjGraph::_apset_map _apset_map@endlink update method.
      ///
      inline void	build_asetmap();

      aset_t		_aset;		///< Set of arcs
      nset_t		_nset;		///< Set of nodes
      apsetmap_t	_apset_map;	///< Map of nodes to connecting arcs
    };

    inline
    std::ostream& operator<<(std::ostream& o, const AdjGraph& g)
    {
      return o << g.to_string();
    }

  } // namespace gra

  using gra::topo;
  using gra::twohop;

  typedef gra::AdjGraph	pathnet_t;

} // namespace olsr

# include "graph.hxx"

#endif // ! QOLYESTER_DAEMON_GRA_GRAPH_HH
