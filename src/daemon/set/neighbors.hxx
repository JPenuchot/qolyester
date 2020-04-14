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

#ifndef QOLYESTER_DAEMON_SET_NEIGHBORS_HXX
# define QOLYESTER_DAEMON_SET_NEIGHBORS_HXX 1

# include "alg/mainaddrof.hh"
# include "gra/graph.hh"
# include "utl/mark.hh"

namespace olsr {

  extern sched_t	scheduler;
  extern utl::Mark	mprset_recomp;
  extern utl::Mark	advset_changed;
  extern pathnet_t	path_net;
  extern cproxy_t	cproxy;

  namespace set {

# ifdef QOLYESTER_ENABLE_LINKHYS
    Link::LinkQuality::LinkQuality(Link& i, float v)
      : value_(v),
	instance_(i),
	expired_(0),
        blocked_(false) {
      assert(0.f <= v && v <= 1.f);
    }

    Link::LinkQuality&
    Link::LinkQuality::operator++() {
//       blocked_ = true;
      debug << up << "LinkQuality::++(): "
	    << instance_.local_addr() << " -> " << instance_.remote_addr()
	    << ' ' << of(value_) << " -> ";
      value_ *= 1 - cst::hyst_scaling;
      value_ += cst::hyst_scaling;
      debug << of(value_) << std::endl << down;
      update();
      return *this;
    }

    Link::LinkQuality&
    Link::LinkQuality::operator--() {
      if (blocked_) {
	debug << up << "LinkQuality::--(): "
	      << instance_.local_addr() << " -> " << instance_.remote_addr()
	      << ' ' << of(value_) << " not changed since blocked"
	      << std::endl << down;
	blocked_ = false;
	return *this;
      }

      debug << up << "LinkQuality::--(): "
	    << instance_.local_addr() << " -> " << instance_.remote_addr()
	    << ' ' << of(value_) << " -> ";
      value_ *= 1 - cst::hyst_scaling;
      debug << of(value_) << std::endl << down;
      update();
      return *this;
    }

    void
    Link::LinkQuality::	update() {
      if (value_ > cst::hyst_threshold_high) {
	instance_.set_pending(false);
	instance_.set_losttime(timeval_t::now() - 1000);
      } else if (value_ < cst::hyst_threshold_low && !instance_.pending()) {
	instance_.set_pending(true);
	instance_.set_losttime(timeval_t::now() + cst::neighb_hold_time);
	if (instance_.time() < instance_.losttime())
	  instance_.set_losttime(instance_.time());
      } /* else {} */
    }

    Link::Link()
      : l_addr_(),
	r_addr_(),
	m_addr_(),
	asymtime_(0),
	symtime_(0),
	time_(0),
	was_sym_(false),
	losttime_(0),
	nexttime_(0),
	htime_(0),
	pending_(false),
	quality_(*this, 0),
	last_seqnum_(0)
    {}

    Link::Link(const address_t& l, const address_t& r, const address_t& m,
	       const timeval_t& v, const seqnum_t& s, const timeval_t& ht)
      : l_addr_(l),
	r_addr_(r),
	m_addr_(m),
	asymtime_(0),
	symtime_(timeval_t::now() - 1000),
	time_(timeval_t::in(v)),
	was_sym_(false),
	losttime_(timeval_t::now() - 1000),
	nexttime_(timeval_t::now() - 1000),
	htime_(ht),
	pending_(true),
	quality_(*this),
	last_seqnum_(s)
    {}

    Link::Link(const This& other)
      : Super(other),
	l_addr_(other.l_addr_),
	r_addr_(other.r_addr_),
	m_addr_(other.m_addr_),
	asymtime_(other.asymtime_),
	symtime_(other.symtime_),
	time_(other.time_),
	was_sym_(other.was_sym_),
	losttime_(other.losttime_),
	nexttime_(other.nexttime_),
	htime_(other.htime_),
	pending_(other.pending_),
	quality_(*this, other.quality_),
	last_seqnum_(other.last_seqnum_)
    {}

    void
    Link::set_last_seqnum(const seqnum_t& s) {
      if (s != ++last_seqnum_) {
	debug << "--quality(" << m_addr_ << "): lost packet (expected "
	      << last_seqnum_ << ", but got " << s << ')' << std::endl;
	quality_.lost(s > last_seqnum_ ? s - last_seqnum_ : 0);
	--quality_;
	cproxy.update_state(main_addr());
	last_seqnum_ = s;
      } else
	quality_.lost(0);

      ++quality_;
      cproxy.update_state(main_addr());
    }
# else // !QOLYESTER_ENABLE_LINKHYS
    Link::Link()
      : l_addr_(),
	r_addr_(),
	m_addr_(),
	asymtime_(0),
	symtime_(0),
	time_(0),
	was_sym_(false)
    {}

    Link::Link(const address_t& l, const address_t& r, const address_t& m,
	       const timeval_t& v)
      : l_addr_(l),
	r_addr_(r),
	m_addr_(m),
	asymtime_(0),
	symtime_(timeval_t::now() - 1000),
	time_(timeval_t::in(v)),
	was_sym_(false)
    {}

    Link::Link(const This& other)
      : Super(other),
	l_addr_(other.l_addr_),
	r_addr_(other.r_addr_),
	m_addr_(other.m_addr_),
	asymtime_(other.asymtime_),
	symtime_(other.symtime_),
	time_(other.time_),
	was_sym_(other.was_sym_)
    {}
# endif

    // Simple validity checker
    bool
    Link::is_valid()	const { return !time_.is_past(); }

    bool
    Link::is_sym()	const {
# ifdef QOLYESTER_ENABLE_LINKHYS
      return losttime_.is_past() && !pending_ && !symtime_.is_past();
# else // !QOLYESTER_ENABLE_LINKHYS
      return !symtime_.is_past();
# endif
    }

    // Plain comparison operator
    bool
    Link::operator<(const This& rhs) const {
      if (l_addr_ == rhs.l_addr_)
	return r_addr_ < rhs.r_addr_;
      return l_addr_ < rhs.l_addr_;
    }

    // Utility static method for the creation of dummy objects to search sets
    const Link&
    Link::make_key(const address_t& local, const address_t& remote) {
      const_cast<address_t&>(dummy_for_find_.l_addr_) = local;
      const_cast<address_t&>(dummy_for_find_.r_addr_) = remote;
      return dummy_for_find_;
    }

    const Link&
    Link::make_key_local(const address_t& local) {
      const_cast<address_t&>(dummy_for_find_.l_addr_) = local;
      return dummy_for_find_;
    }

    namespace upd {

      void
      LinkUpdater::operator()() {
# ifdef QOLYESTER_ENABLE_LINKHYS
	if (iter_->nexttime().is_past()) {
	  debug << up << "--quality(" << iter_->main_addr()
		<< "): expired nexttime by "
		<< iter_->nexttime().diff() << std::endl << down;
	  const_cast<elem_t&>(*iter_).quality().expire();
	  --const_cast<elem_t&>(*iter_).quality();
	  const_cast<elem_t&>(*iter_).set_nexttime(iter_->nexttime() +
						   iter_->htime() *
						   cst::hello_grace);
	}
# endif // !QOLYESTER_ENABLE_LINKHYS
	if (iter_->is_valid()) {
	  set_.update_state(iter_->main_addr());
	  sch::TimedEvent*	e = iter_->updater();
	  scheduler.erase(e);
	  e->set_next(min_time(*iter_));
	  scheduler.insert(e);
	} else
	  set_.erase(iter_);
      }

    } // namespace upd

    //********************************
    //*  The Neighbor class
    //*

    Neighbor::Neighbor()
      : m_addr_(),
	lset_(),
	val_lset_(*this, lset_),
	sym_(false),
	mpr_(false),
	mprsel_time_(0, 0),
	was_mprsel_(false),
	willingness_(0)
    {}

    Neighbor::Neighbor(const address_t& maddr,
		       const std::set<ulink_t>::iterator& i,
		       unsigned w)
      : m_addr_(maddr),
	lset_(),
	val_lset_(*this, lset_),
	sym_(false),
	mpr_(false),
	mprsel_time_(timeval_t::now() - 1000),
	was_mprsel_(false),
	willingness_(w) {
      lset_.insert(i);
    }

    Neighbor::Neighbor(const This& other)
      : Super(other),
	m_addr_(other.m_addr_),
	lset_(other.lset_),
	val_lset_(*this, lset_),
	sym_(other.sym_),
	mpr_(other.mpr_),
	mprsel_time_(other.mprsel_time_),
	was_mprsel_(other.was_mprsel_),
	willingness_(other.willingness_)
    {}

    void
    Neighbor::set_willingness(unsigned w) {
      willingness_ = w;
    }

    bool
    Neighbor::is_mprsel() const {
      return !mprsel_time_.is_past();
    }

    bool
    Neighbor::was_mprsel() {
      if (was_mprsel_) {
	was_mprsel_ = false;
	return true;
      }
      return false;
    }

    void
    Neighbor::set_mprsel(const timeval_t& validity) {
      mprsel_time_ = timeval_t::in(validity);
      if (is_mprsel())
	was_mprsel_ = true;
    }

    void
    Neighbor::unset_mprsel() {
      mprsel_time_ = timeval_t::now() - 1000;
    }

    std::pair<Neighbor::linkset_t::const_iterator,
	      Neighbor::linkset_t::const_iterator>
    Neighbor::find_lifaces(const address_t& laddr) const {
      return const_cast<linkset_t&>(val_lset_).equal_range(Link::make_key_local(laddr));
    }

//     Neighbor::linkset_t::const_iterator
//     Neighbor::find_liface(const address_t& laddr) const {
//       return const_cast<linkset_t&>(val_lset_).find(Link::make_key_local(laddr));
//     }

//     bool
//     Neighbor::is_liface(const address_t& iaddr) const {
//       return find_liface(iaddr) != const_cast<linkset_t&>(val_lset_).end();
//     }

    void
    Neighbor::insert(const std::set<ulink_t>::iterator& pos) {
      lset_.insert(pos);
    }

    void
    Neighbor::erase(const lset_t::iterator& pos) {
      lset_.erase(pos);
    }

    void
    Neighbor::erase(const std::set<ulink_t>::iterator& pos) {
      lset_.erase(pos);
    }

    bool
    Neighbor::operator<(const This& rhs) const {
      return m_addr_ < rhs.m_addr_;
    }

    const Neighbor&
    Neighbor::make_key(const address_t& m) {
      const_cast<address_t&>(dummy_for_find_.m_addr_) = m;
      return dummy_for_find_;
    }

    namespace upd {

      void
      NeighborUpdater::operator()() {
	set_.unset_mprsel(iter_);
      }

    } // namespace upd

    CoherenceProxy::CoherenceProxy()
      : lset_(),
	nset_(),
	linkset_(lset_),
	idx_lset_(lset_),
	hello_linkset_(idx_lset_),
	neighborset_(*this, nset_),
	sym_neighborset_(neighborset_),
	idx_nset_(nset_),
	val_idx_nset_(*this, idx_nset_),
	tc_neighborset_(val_idx_nset_),
	advset_seqnum_(0),
	advset_changed_(false),
	sym_count_(0),
	mpr_count_(0),
	mprsel_empty_(true)
    {}

    CoherenceProxy::~CoherenceProxy() {
      while (!lset_.empty())
	erase(lset_.begin());
    }

# ifdef QOLYESTER_ENABLE_LINKHYS
    std::pair<CoherenceProxy::neighborset_t::iterator, bool>
    CoherenceProxy::insert_link(const msg::Message::header& mh,
				const int linktype,
				const int will,
				const timeval_t& htime) {
      // Insertion/search of the link and neighbor.

      std::pair<lset_t::iterator, bool> lp =
	lset_.insert(Link(mh.receiver, mh.sender,
			  mh.originator, mh.validity,
			  mh.pseqnum, htime));
# else // !QOLYESTER_ENABLE_LINKHYS
    std::pair<CoherenceProxy::neighborset_t::iterator, bool>
    CoherenceProxy::insert_link(const msg::Message::header& mh,
				const int linktype,
				const int will) {
      // Insertion/search of the link and neighbor.

      std::pair<lset_t::iterator, bool> lp =
	lset_.insert(Link(mh.receiver, mh.sender,
			  mh.originator, mh.validity));
# endif
      std::pair<nset_t::iterator, bool> np =
	nset_.insert(Neighbor(mh.originator, lp.first, will));

      Link&	l = const_cast<Link&>(static_cast<const Link&>(*lp.first));

      // If the previous operation appears to have been an
      // insertion, insert the new neighbor into idx0_nset and
      // idx1_nset (the sets on which HELLO::dump() and TC::dump()
      // will actually iterate.
      // Otherwise, update validity and insert the link into the
      // "already known" neighbor.

      if (lp.second) { // link is new
	idx_lset_.insert(lp.first);
	if (np.second) { // neighbor is new
	  idx_nset_.insert(np.first);
	} else
	  const_cast<uneighbor_t&>(*np.first).insert(lp.first);
      } else { // link is not new
# ifdef QOLYESTER_ENABLE_LINKHYS
	l.set_htime(htime);
# endif // !QOLYESTER_ENABLE_LINKHYS
      }

# ifdef QOLYESTER_ENABLE_LINKHYS
      l.set_nexttime(timeval_t::in(htime * cst::hello_grace));
# endif // !QOLYESTER_ENABLE_LINKHYS

      if (!np.second)
	set_willingness(np.first, will);

      // Update L_ASYM_time
      l.set_asymtime(timeval_t::in(mh.validity));

      // Update L_SYM_time and L_time
      switch (linktype) {
      case LOST_LINK:
	l.set_symtime(timeval_t::now() - 1000); // expired
	break;
      case SYM_LINK:
      case ASYM_LINK:
	l.set_symtime(timeval_t::in(mh.validity));
	l.set_time(lp.first->symtime() + cst::neighb_hold_time);
	break;
      }

      if (l.time() < l.asymtime())
	l.set_time(l.asymtime());

      typedef upd::LinkUpdater	lu_t;

      sch::TimedEvent*	e = lp.first->updater();

      if (e == 0) {
	e = new link_updater_t(lu_t::min_time(l),
			       lu_t(*this, lp.first));
	const_cast<ulink_t&>(*lp.first).set_updater(e);
      } else {
	scheduler.erase(e);
	e->set_next(lu_t::min_time(l));
      }
      scheduler.insert(e);

      update_state(np.first);

      typedef std::pair<neighborset_t::iterator, bool>	ret_t;

      // neighborset_t is a MaskIterator, but we don't call check()
      // on return, because we assume it would never erase the
      // freshly created/update element.
      return ret_t(neighborset_t::iterator::
		   build(neighborset_t::mask_iterator(np.first, nset_, *this)),
		   np.second);

    }

    void
    CoherenceProxy::set_willingness(const nset_t::iterator& pos,
				    unsigned w) {
      if (pos->willingness() != w) {
	mprset_recomp.set_mark();
	routes_recomp.set_mark();
	const_cast<uneighbor_t&>(*pos).set_willingness(w);
	update_graph(pos);
      }
    }

    void
    CoherenceProxy::update_graph(const nset_t::iterator& pos) {
      unsigned	w = pos->willingness();
      if (pos->is_sym()) {

	// A node's weight is used by the Dijkstra algorithm to prefer
	// nodes with larger weight, in case of ties.  If the weight
	// is 0, then the relaxation is not performed at all, this is
	// used to forbid routing through nodes with willingness of
	// WILL_NEVER.  Those nodes have to be in path_net anyway,
	// just in case the willingness changes and all the 2-hop
	// neighbors going through it must be reachable again.

	unsigned new_weight =
	  w != 0 ?
	  w + (pos->is_mprsel() ? WILL_DEFAULT : 0) :
	  0;
	std::pair<gra::AdjGraph::nset_t::const_iterator, bool>	p =
	  path_net.insert_node(gra::AdjNode(pos->main_addr(), new_weight));
	if (!p.second)
	  const_cast<gra::AdjNode&>(*p.first).set_weight(new_weight);
      } else {
	gra::AdjGraph::nset_t::iterator	x =
	  gra::AdjGraph::nset_t::iterator::
	  build(path_net.nodes().find(gra::AdjNode::
				      make_key(pos->main_addr())));
	if (x != path_net.nodes().end()) {
	  path_net.remove_node_if_alone(x);
	}
      }
    }

    bool
    CoherenceProxy::update_state(const nset_t::iterator& pos) {
      assert(pos != nset_.end());
      bool	sym = false;
      bool	valid = false;
      // The following loop has to be executed entirely without
      // breaking out as soon as a symmetric link is found (cleanup).
      for (Neighbor::linkset_t::iterator i =
	     const_cast<uneighbor_t&>(*pos).linkset().begin();
	   i != const_cast<uneighbor_t&>(*pos).linkset().end(); ++i) {
	bool	link_sym = i->is_sym();
	valid = true;
	if (link_sym != i->was_sym()) {
	  routes_recomp.set_mark();
	  i->set_was_sym(link_sym);
	}
	if (link_sym && !sym)
	  sym = true;
      }
      if (sym) {
	set_sym(pos);
      } else {
	unset_sym(pos);
      }
      // Don't touch the graph here as it's taken care of in
      // {,un}set_{sym,mpr,mprsel} methods.
      return valid;
    }

    void
    CoherenceProxy::update_state(const address_t& maddr) {
      nset_t::iterator	x = nset_.find(Neighbor::make_key(maddr));
      if (x != nset_.end())
	update_state(x);
    }

    void
    CoherenceProxy::set_sym(const nset_t::iterator& pos) {
      assert(pos != nset_.end());
      if (!pos->is_sym()) {
	++sym_count_;
	const_cast<uneighbor_t&>(*pos).set_sym(true);
	mprset_recomp.set_mark();
	routes_recomp.set_mark();
	update_graph(pos);
	if (tc_redundancy == wholeset) {
	  advset_changed_ = true;
	  advset_changed.set_mark();
	}
      }
    }

    void
    CoherenceProxy::unset_sym(const nset_t::iterator& pos) {
      assert(pos != nset_.end());
      if (pos->is_sym()) {
	unset_mpr(pos);
	unset_mprsel(pos);
	--sym_count_;
	const_cast<uneighbor_t&>(*pos).set_sym(false);
	mprset_recomp.set_mark();
	routes_recomp.set_mark();
	update_graph(pos);
	if (tc_redundancy == wholeset) {
	  advset_changed_ = true;
	  advset_changed.set_mark();
	}
      }
    }

    void
    CoherenceProxy::set_mpr(const nset_t::iterator& pos) {
      assert(pos != nset_.end());
      assert(pos->is_sym());
      if (!pos->is_mpr()) {
	++mpr_count_;
	const_cast<uneighbor_t&>(*pos).set_mpr(true);
	update_graph(pos);
	if (tc_redundancy >= mprselset_mprset) {
	  advset_changed_ = true;
	  advset_changed.set_mark();
	}
      }
    }

    void
    CoherenceProxy::set_mpr(const sym_neighborset_t::iterator& pos) {
      set_mpr(pos.mask_super().mask_super());
    }

    void
    CoherenceProxy::unset_mpr(const nset_t::iterator& pos) {
      assert(pos != nset_.end());
      assert(pos->is_sym());
      if (pos->is_mpr()) {
	--mpr_count_;
	const_cast<uneighbor_t&>(*pos).set_mpr(false);
	update_graph(pos);
	if (tc_redundancy >= mprselset_mprset) {
	  advset_changed_ = true;
	  advset_changed.set_mark();
	}
      }
    }

    void
    CoherenceProxy::unset_mpr(const sym_neighborset_t::iterator& pos) {
      unset_mpr(pos.mask_super().mask_super());
    }

    void
    CoherenceProxy::set_mprsel(const nset_t::iterator& pos,
			       const timeval_t& v) {
      assert(pos != nset_.end());
      // This used to be an assertion, but ought to be a check.
      if (!pos->is_sym())
	return;
      assert(timeval_t(0) < v);
      if (!pos->is_mprsel()) {
	mprsel_empty_ = false;
	advset_changed_ = true;
	advset_changed.set_mark();
      }
      const_cast<uneighbor_t&>(*pos).set_mprsel(v);
      typedef upd::NeighborUpdater	nu_t;
      sch::TimedEvent*	e = pos->updater();
      if (e == 0) {
	e = new neighbor_updater_t(timeval_t::in(v), nu_t(*this, pos));
	const_cast<uneighbor_t&>(*pos).set_updater(e);
      } else {
	scheduler.erase(e);
	e->set_next(timeval_t::in(v));
      }
      scheduler.insert(e);
    }

    void
    CoherenceProxy::unset_mprsel(const nset_t::iterator& pos) {
      assert(pos != nset_.end());
      //assert(pos->is_sym());
      sch::TimedEvent*	e = pos->updater();
      if (const_cast<uneighbor_t&>(*pos).was_mprsel()) {
	assert(e != 0);
	scheduler.destroy(e);
	const_cast<uneighbor_t&>(*pos).set_updater(0);
	const_cast<uneighbor_t&>(*pos).unset_mprsel();
	advset_changed_ = true;
	advset_changed.set_mark();
      } else if (e != 0) {
	scheduler.destroy(e);
	const_cast<uneighbor_t&>(*pos).set_updater(0);
      }
//       if (pos->is_used()) {
// // 	const NeighborUpdater&	nu = *pos->del_pos();
// // 	del_set.erase(pos->del_pos());
// // 	delete &nu;
// // 	del_set.shift();
// 	const_cast<nelem_&>(*pos).unuse();
//       }
    }

    bool
    CoherenceProxy::is_advset_empty() {
      if (mprsel_empty_)
	return true;
      mprsel_empty_ = true;
      for (sym_neighborset_t::iterator i = sym_neighborset_.begin();
	   i != sym_neighborset_.end(); ++i)
	if (i->is_mprsel()) {
	  mprsel_empty_ = false;
	  break;
	}
      return mprsel_empty_;
    }

    bool
    CoherenceProxy::is_valid(const nset_t::iterator& pos) {
      return update_state(pos);
    }

    bool
    CoherenceProxy::is_valid(const idx_nset_t::iterator& pos) {
      return update_state(*pos.deref_super());
    }

    void
    CoherenceProxy::erase(lset_t::iterator pos) {
      nset_t::iterator	npos =
	nset_.find(Neighbor::make_key(pos->main_addr()));
      if (npos != nset_.end()) {
	const_cast<uneighbor_t&>(*npos).erase(pos);
	if (!is_valid(npos))
	  erase_from_all(npos);
      }
//       const LinkUpdater&	lu = *pos->del_pos();
//       del_set.erase(pos->del_pos());
//       delete &lu;
      idx_lset_.erase(pos);
      lset_.erase(pos);
    }

    void
    CoherenceProxy::erase_from_all(nset_t::iterator pos) {
      // First call CoherenceProxy::unset_sym() to update counters
      // and remove the node from the graph.
      unset_sym(pos);
      // Second remove the neighbor from the indices.
      idx_nset_.erase(pos);
      // Third remove the node /per se/.
      nset_.erase(pos);
    }

    void
    CoherenceProxy::add_interface(const address_t& a) {
      idx_lset_.add_key(a);
      idx_nset_.add_key(a);
    }

    void
    CoherenceProxy::remove_interface(const address_t& a) {
      for (linkset_t::iterator i = linkset_.begin(); i != linkset_.end();
	   /* ++i */)
	if (i->local_addr() == a)
	  erase(i++);
	else
	  ++i;
      idx_lset_.remove_key(a);
      idx_nset_.remove_key(a);
    }


    //********************************************
    //* TwoHopNeighbor definition
    //*

    TwoHopNeighbor::TwoHopNeighbor() :
      mainaddr_(),
      twohopaddr_(),
      time_(0, 0)
    {}

    TwoHopNeighbor::TwoHopNeighbor(const address_t& a,
				   const address_t& tha,
				   const timeval_t&   validity) :
      mainaddr_(a),
      twohopaddr_(tha),
      time_(timeval_t::in(validity))
    {}

    bool
    TwoHopNeighbor::operator<(const This& rhs) const {
      if (mainaddr_ == rhs.mainaddr_)
	return twohopaddr_ < rhs.twohopaddr_;
      return mainaddr_ < rhs.mainaddr_;
    }

    TwoHopNeighbor&
    TwoHopNeighbor::operator=(const This& other) {
      assert(mainaddr_ == other.mainaddr_);
      assert(twohopaddr_ == other.twohopaddr_);
      time_ = other.time_;
      return *this;
    }

    const TwoHopNeighbor&
    TwoHopNeighbor::make_key(const address_t& ma, const address_t& tha) {
      const_cast<address_t&>(dummy_for_find_.mainaddr_) = ma;
      const_cast<address_t&>(dummy_for_find_.twohopaddr_) = tha;
      return dummy_for_find_;
    }

    TwoHopNeighborSet::TwoHopNeighborSet()
      : tset_(),
	thnset_(*this, tset_)
    {}

    void
    TwoHopNeighborSet::insert(const TwoHopNeighbor& x) {
      std::pair<tset_t::iterator, bool>	p = tset_.insert(x);
      if (!p.second) {
	sch::TimedEvent*	e = p.first->updater();
	scheduler.erase(e);
	e->set_next(x.time());
	scheduler.insert(e);

	const_cast<elem_t&>(*p.first).set_time(x.time());
      } else {
	if (cproxy.sym_neighborset().find(Neighbor::make_key(alg::main_addr_of(x.twohop_addr()))) == cproxy.sym_neighborset().end())
	  mprset_recomp.set_mark();
	routes_recomp.set_mark();
	sch::TimedEvent*	e = new updater_t(x.time(),
						  eraser_t(*this, p.first));
	const_cast<elem_t&>(*p.first).set_updater(e);
	scheduler.insert(e);
	path_net.insert_node(gra::AdjNode(x.twohop_addr()));
	path_net.insert_arc(gra::AdjInfo(x.main_addr(),
					 x.twohop_addr(),
					 twohop));
      }
    }

    void
    TwoHopNeighborSet::erase(const tset_t::iterator& pos) {
      assert(pos != tset_.end());
      bool	is_1hop_sym =
	cproxy.sym_neighborset().find(Neighbor::make_key(pos->twohop_addr()))
	!= cproxy.sym_neighborset().end();
      path_net.remove_arc(gra::AdjInfo::make_key(pos->main_addr(),
						 pos->twohop_addr(),
						 twohop));
      if (!is_1hop_sym)
	path_net.remove_node_if_alone(gra::AdjNode::make_key(pos->twohop_addr()));
      if (!is_1hop_sym)
	mprset_recomp.set_mark();
      routes_recomp.set_mark();
      sch::TimedEvent*	e = pos->updater();
      tset_.erase(pos);
      scheduler.destroy(e);
    }

    void
    TwoHopNeighborSet::erase(const tset_t::value_type& x) {
      tset_t::iterator	pos = tset_.find(x);
      if (pos != tset_.end())
	erase(pos);
    }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_NEIGHBORS_HXX
