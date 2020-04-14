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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_DAEMON_ALG_MPRSELECTION_HXX
# define QOLYESTER_DAEMON_ALG_MPRSELECTION_HXX 1

# include "set/neighbors.hh"

# include "mprselection.hh"

namespace olsr {

  extern thnset_t	thn_set;
  extern utl::Mark	advset_changed;

  namespace alg {

    // The MPR selection routine.  This is the extended version,
    // taking into account the MPR_COVERAGE parameter, as described in
    // section 16.2.
    void mprselection()
    {
      debug << "SELECTING MPRs" << std::endl;

      // Declaration of several convenience types.

      // An address set
      typedef std::set<address_t>		aset_t;

      // A simple 2-hop neighbor set
      typedef std::map<address_t, aset_t>	thnsimpleset_t;

      // A neighbor set (in fact this is a pointer set, to spare
      // memory usage)
      typedef std::set<const set::Neighbor*, utl::pless<set::Neighbor> >
						nset_t;
      // The iterator on the neighbor set, which can be used as though
      // the elements were references to neighbors themselves and not
      // pointers.
      typedef utl::DerefIterator<nset_t::const_iterator>
						nset_const_iterator;

      // A degree map, mapping 1-hop neighbor addresses into their
      // degree (see the RFC 3626 section 8.3.1 for more info).
      typedef std::map<address_t, unsigned>	degreemap_t;

      // A coverer map, mapping 2-hop neighbor addresses into the set
      // of 1-hop neighbors that reach them.
      typedef std::map<address_t, aset_t>	coverermap_t;

      aset_t				mprs;
      aset_t				total_n2;
      nset_t				rcandidates;
      thnsimpleset_t			thns;

      // Compute the global 2-hop neighborhood.

      // We want to exclude 1-hop neighbors that would certainly not
      // be chosen as MPR regardless of the interface (asym nodes and
      // nodes with willingness equal to WILL_NEVER) and 2-hop
      // neighbors that are also symmetric 1-hop neighbors.
      for (thnset_t::thnset_t::iterator t = thn_set.thnset().begin();
	   t != thn_set.thnset().end(); ++t) {
	cproxy_t::sym_neighborset_t::iterator	n =
	  cproxy.sym_neighborset().find(set::Neighbor::make_key(t->main_addr()));
	if (n == cproxy.sym_neighborset().end() ||
	    n->willingness() == WILL_NEVER)
	  continue; // Ignore asym and non-willing nodes
	if (cproxy.sym_neighborset().find(set::Neighbor::make_key(t->twohop_addr())) !=
	    cproxy.sym_neighborset().end())
	  continue; // Ignore symmetric nodes
	thns[t->main_addr()].insert(t->twohop_addr());
	total_n2.insert(t->twohop_addr());
      }

# ifdef QOLYESTER_ENABLE_MID

      // Cycle over all the interfaces to compute the per-interface MPR-set.
      for (ifaceset_t::const_iterator i = iface_set.begin();
	   i != iface_set.end();
	   ++i) {

# else // ! QOLYESTER_ENABLE_MID

      {
        iface_t*	i = &this_interface;

# endif

	thnsimpleset_t	lthns;
	nset_t		candidates;
	degreemap_t	degree_map;	// degree map
      	coverermap_t	coverer_map;	// covered map
	aset_t		n2;		// temp N2 set

	// Compute the 2-hop set and candidates.
	for (thnsimpleset_t::const_iterator oh = thns.begin();
	     oh != thns.end(); ++oh) {
	  cproxy_t::sym_neighborset_t::iterator	n =
	    cproxy.sym_neighborset().find(set::Neighbor::make_key(oh->first));
	  assert(n != cproxy.sym_neighborset().end());
	  std::pair<set::Neighbor::linkset_t::iterator,
	            set::Neighbor::linkset_t::iterator>	er =
	    n->find_lifaces(i->addr());
	  bool	sym_found = false;
	  for (set::Neighbor::linkset_t::iterator l = er.first;
	       l != er.second; ++l)
	    if (l->is_sym()) {
	      sym_found = true;
	      break;
	    }
	  if (!sym_found)
	    continue; // Ignore nodes unreachable through this iface
	  candidates.insert(&*n);
	  lthns.insert(*oh);
	  degree_map[oh->first] = oh->second.size();
	  for (aset_t::const_iterator th = oh->second.begin();
	       th != oh->second.end(); ++th) {
	    coverer_map[*th].insert(oh->first);
	    n2.insert(*th);
	  }
	}

# ifdef QOLYESTER_ENABLE_MID
	aset_t		lmprs;		// iface-local MPR-set
# else // ! QOLYESTER_ENABLE_MID
	aset_t&		lmprs = mprs;
# endif

	// Preliminary MPR selection

	// Get the WILL_ALWAYS neighbors into the iface-local MPR-set first
	// and don't add them to the global removal-candidates set.
	for (nset_const_iterator n =
	       nset_const_iterator::build(candidates.begin());
	     n != nset_const_iterator::build(candidates.end()); ++n)
	  if (n->willingness() == WILL_ALWAYS)
	    lmprs.insert(n->main_addr());

	// Get the neighbors that are necessary relays to some 2-hop
	// neighbors and update
	for (coverermap_t::const_iterator c = coverer_map.begin();
	     c != coverer_map.end(); ++c)
	  if (c->second.size() <= mprcoverage) {
	    lmprs.insert(c->second.begin(), c->second.end());
	    n2.erase(c->first);
	  }

	// End of preliminary selection

	// Reach counter initialization and 'remaining' 2-hop neighbor
	// set populating
	// For each node in N2, the number of already-selected MPRs
	// reaching it is recorded.  The purpose is to let us know
	// when a node in N2 is already covered.
	typedef std::map<address_t, unsigned>	reachcount_t;
	reachcount_t				reachcount;
	thnsimpleset_t				rthns = lthns;

	for (aset_t::const_iterator m = lmprs.begin();
	     m != lmprs.end(); ++m) {
	  candidates.erase(&set::Neighbor::make_key(*m));
	  thnsimpleset_t::const_iterator	oh = rthns.find(*m);
	  assert(oh != rthns.end());
	  const aset_t&	to_update = oh->second;
	  for (aset_t::const_iterator th = to_update.begin();
	       th != to_update.end(); ++th)
	    if (++reachcount[*th] >= mprcoverage) {
	      n2.erase(*th);
	      for (thnsimpleset_t::iterator i = rthns.begin();
		   i != rthns.end(); ++i)
		if (i->first != *m)
		  i->second.erase(*th);
	    }
	}

	// Cycle while N2 is not empty
	while (!n2.empty()) {

	  typedef std::map<address_t, unsigned>	reachmap_t;
	  reachmap_t				reachmap;

	  // Build the reachability map
	  for (nset_const_iterator n =
		 nset_const_iterator::build(candidates.begin());
	       n != nset_const_iterator::build(candidates.end()); ++n)
	    reachmap[n->main_addr()] = rthns[n->main_addr()].size();

	  // Get the neighbor with max willingness, reachability, degree.

	  bool			first = true;
	  const set::Neighbor*	maxn = 0;
	  unsigned		r = 0;
	  unsigned		d = 0;

	  for (nset_const_iterator n =
		 nset_const_iterator::build(candidates.begin());
	       n != nset_const_iterator::build(candidates.end()); ++n) {
	    unsigned n_r = reachmap[n->main_addr()];
	    if (first or
		n->willingness() > maxn->willingness() or
		(n->willingness() == maxn->willingness() and
		 (n_r > r or
		  (n_r == r and degree_map[n->main_addr()] > d)))) {
	      first = false;
	      maxn  = &*n;
	      r     = n_r;
	      d     = degree_map[n->main_addr()];
	    }
	  }

	  assert(!first);

	  // Update reach counters and remove any node in N2 that
	  // has enough coverage thanks to the selection of this MPR.
	  const aset_t&	to_update = rthns[maxn->main_addr()];
	  for (aset_t::const_iterator th = to_update.begin();
	       th != to_update.end(); ++th) {
	    if (++reachcount[*th] >= mprcoverage) {
	      n2.erase(*th);
	      for (thnsimpleset_t::iterator i = rthns.begin();
		   i != rthns.end(); ++i)
		if (i->first != maxn->main_addr())
		  i->second.erase(*th);
	    }
	  }

	  // Insert the MPR into the iface-local MPR-set
	  lmprs.insert(maxn->main_addr());
	  // and in the removal-candidates set
	  // and remove it from the set of candidates
	  rcandidates.insert(maxn);
	  candidates.erase(maxn);
	}

# ifdef QOLYESTER_ENABLE_MID
	// Merge the iface-local MPR-set into the global MPR-set
	mprs.insert(lmprs.begin(), lmprs.end());
# endif

      }

      // Optimize the MPR-set

      // Compute the coverage count for each node in N2.
      std::map<address_t, unsigned>	covcount_map;
      for (aset_t::const_iterator m = mprs.begin(); m != mprs.end(); ++m) {
	const aset_t&	m_thns = thns[*m];
	for (aset_t::const_iterator th = m_thns.begin(); th != m_thns.end(); ++th)
	  ++covcount_map[*th];
      }

      // Compute a willingness map.  The point is to sort
      // removal-candidates by order of increasing willingness.
      typedef std::multimap<unsigned, address_t>	wmap_t;
      wmap_t						wmap;
      for (nset_const_iterator rc =
	     nset_const_iterator::build(rcandidates.begin());
	   rc != nset_const_iterator::build(rcandidates.end()); ++rc)
	wmap.insert(wmap_t::value_type(rc->willingness(), rc->main_addr()));

      // Iterate on the willingness map and check if the candidate can
      // be actually removed, i.e. check if removal doesn't decrease
      // some N2 node's coverage count below the MPR_COVERAGE
      // threshold.
      for (wmap_t::const_iterator rc = wmap.begin(); rc != wmap.end(); ++rc) {
	aset_t&	rc_thns = thns[rc->second];
	bool	remove = true;
	for (aset_t::const_iterator th = rc_thns.begin(); th != rc_thns.end(); ++th)
	  if (covcount_map[*th] <= mprcoverage) {
	    remove = false;
	    break;
	  }
	if (remove) {
	  for (aset_t::const_iterator th = rc_thns.begin();
	       th != rc_thns.end(); ++th)
	    --covcount_map[*th];
	  mprs.erase(rc->second);
	}
      }

      bool	changed = false;

      // Apply the MPR changes.
      for (cproxy_t::sym_neighborset_t::iterator n =
	     cproxy.sym_neighborset().begin();
	   n != cproxy.sym_neighborset().end(); ++n) {
	if (n->is_mpr()) {
	  if (mprs.find(n->main_addr()) == mprs.end()) {
	    cproxy.unset_mpr(n);
	    changed = true;
	  }
	} else
	  if (mprs.find(n->main_addr()) != mprs.end()) {
	    cproxy.set_mpr(n);
	    changed = true;
	  }
      }

      if (changed) {
	routes_recomp.set_mark();
	if (tc_redundancy >= mprselset_mprset)
	  advset_changed.set_mark();
      }
    }

  } // namespace alg

} // Namespace olsr

#endif // !QOLYESTER_DAEMON_ALG_MPRSELECTION_HXX
