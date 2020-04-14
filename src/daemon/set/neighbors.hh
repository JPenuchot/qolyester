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

///
/// @file   neighbors.hh
/// @author Ignacy Gawedzki
/// @date   Sun Apr  4 05:08:23 2004
///
/// @brief  Link, Neighbor and TwoHopNeighbor sets and related
///

# include "msg/message.hh"

#ifndef QOLYESTER_DAEMON_SET_NEIGHBORS_HH
# define QOLYESTER_DAEMON_SET_NEIGHBORS_HH 1

# include "config.hh"

# include "cst/constants.hh"
# include "net/ipaddress.hh"
# include "sch/scheduler.hh"
# include "utl/set.hh"
# include "utl/seqnum.hh"
# include "utl/stampable.hh"
# include "utl/timeval.hh"

namespace olsr {

  namespace sch {

    class StatePrinter;

  }

  namespace set {

    //************************************
    //*  The Link class
    //*

    class Link : public utl::MultiStampable<1, address_t> {
      typedef Link				This;
      typedef utl::MultiStampable<1, address_t>	Super;

      explicit Link();

# ifdef QOLYESTER_ENABLE_LINKHYS
      class LinkQuality {
	typedef LinkQuality	This;
      public:
	inline LinkQuality(Link& i, float v = cst::hyst_scaling);
	virtual ~LinkQuality() {}
	operator float() const {
	  return value_;
	}
	inline This&	operator++();
	This	operator++(int) {
	  This	tmp(*this);
	  ++*this;
	  return tmp;
	}
	inline This&	operator--();
	This	operator--(int) {
	  This	tmp(*this);
	  --*this;
	  return tmp;
	}
	void	expire() {
	  ++expired_;
	}
	void	lost(unsigned n) {
	  if (n > expired_ || n == 0)
	    blocked_ = false;
	  else
	    blocked_ = true;
	  expired_ = 0;
	}
      private:
	inline void	update();

	float		value_;
	Link&		instance_;
	unsigned	expired_;
	bool		blocked_;
      };
    public:
      inline Link(const address_t& l, const address_t& r, const address_t& m,
		  const timeval_t& v, const seqnum_t& s, const timeval_t& ht);
# else // !QOLYESTER_ENABLE_LINKHYS
    public:
      inline Link(const address_t& l, const address_t& r, const address_t& m,
		  const timeval_t& v);
# endif

      inline Link(const This& other);

      virtual ~Link() {}
      // Modifiers
      void	set_asymtime(const timeval_t& t) { asymtime_ = t; }
      void	set_symtime (const timeval_t& t) { symtime_  = t; }
      void	set_time    (const timeval_t& t) { time_     = t; }
# ifdef QOLYESTER_ENABLE_LINKHYS
      void	set_losttime(const timeval_t& t) { losttime_ = t; }
      void	set_nexttime(const timeval_t& t) { nexttime_ = t; }
      void	set_htime   (const timeval_t& t) { htime_    = t; }
      void	set_pending (const bool p)       { pending_  = p; }

      inline void	set_last_seqnum(const seqnum_t& s);
# endif // !QOLYESTER_ENABLE_LINKHYS

      // Accessors
      const address_t&	local_addr()	const { return l_addr_; }
      const address_t&	remote_addr()	const { return r_addr_; }
      const address_t&	main_addr()	const { return m_addr_; }
      const timeval_t&	asymtime()	const { return asymtime_; }
      const timeval_t&	symtime()	const { return symtime_; }
      const timeval_t&	time()		const { return time_; }
# ifdef QOLYESTER_ENABLE_LINKHYS
      const timeval_t&	losttime()	const { return losttime_; }
      const timeval_t&	nexttime()      const { return nexttime_; }
      const timeval_t&  htime()         const { return htime_; }
      bool		pending()	const { return pending_; }

      const LinkQuality&	quality() const { return quality_; }
      LinkQuality&		quality() { return quality_; }
# endif // !QOLYESTER_ENABLE_LINKHYS

      // Simple validity checker
      inline bool	is_valid() const;
      inline bool	is_sym() const;

      bool	was_sym()     const { return was_sym_; }
      void	set_was_sym(bool ws) { was_sym_ = ws; }

      // Plain comparison operator
      inline bool	operator<(const This& rhs) const;

      // Utility static method for the creation of dummy objects to search sets
      static inline const This&	make_key(const address_t& local,
					 const address_t& remote);
      static inline const This&	make_key_local(const address_t& local);
    private:
      const address_t	l_addr_;
      const address_t	r_addr_;
      const address_t	m_addr_;
      timeval_t		asymtime_;
      timeval_t		symtime_;
      timeval_t		time_;
      bool		was_sym_;
# ifdef QOLYESTER_ENABLE_LINKHYS
      timeval_t		losttime_;
      timeval_t		nexttime_;
      timeval_t		htime_;
      bool		pending_;
      LinkQuality	quality_;
      seqnum_t		last_seqnum_;
# endif // !QOLYESTER_ENABLE_LINKHYS

      // This is used for dummy objects used in set searching
      static This	dummy_for_find_;

      friend class sch::StatePrinter;
    };

    //********************************
    //*  The Neighbor class
    //*

    // The following block of declarations is only to provide Neighbor
    // with nested type definitions that are to occur in
    // CoherenceProxy later in the code.  This is mainly to avoid
    // having to declare Neighbor after CoherenceProxy and is
    // obviously the best way to deal with type interdependance.

    class CoherenceProxy;

    namespace upd {

      class LinkUpdater {
	typedef LinkUpdater			This;
	typedef CoherenceProxy			Set;
	typedef sch::Updatable<Link>		elem_t;
	typedef std::set<elem_t>::iterator	Iter;
	typedef sch::UpdateEvent<This>		updater_t;
      public:
	LinkUpdater(Set& s, const Iter& i)
	  : set_(s),
	    iter_(i)
	{}
	inline void	operator()();
	static timeval_t	min_time(const Link& l) {
	  timeval_t	min_time(0);
	  if (!l.time().is_past())
	    min_time = l.time();
	  if (!l.asymtime().is_past() &&
	      (min_time == 0 ||
	       l.asymtime() < min_time))
	    min_time = l.asymtime();
	  if (!l.symtime().is_past() &&
	      (min_time == 0 ||
	       l.symtime() < min_time))
	    min_time = l.symtime();
# ifdef QOLYESTER_ENABLE_LINKHYS
	  if (!l.losttime().is_past() &&
	      (min_time == 0 ||
	       l.losttime() < min_time))
	    min_time = l.losttime();
	  if (!l.nexttime().is_past() &&
	      (min_time == 0 ||
	       l.nexttime() < min_time))
	    min_time = l.nexttime();
# endif // !QOLYESTER_ENABLE_LINKHYS
	  return min_time;
	}
      private:
	Set&	set_;
	Iter	iter_;
      };

    } // namespace upd

    class Neighbor : public utl::MultiStampable<1, address_t> {
      typedef Neighbor				This;
      typedef utl::MultiStampable<1, address_t>	Super;

      typedef sch::Updatable<Link>		ulink_t;

      struct ilinkless {
	typedef std::set<ulink_t>::iterator	iter_t;
	bool	operator()(const iter_t& a, const iter_t& b) const {
	  return a->local_addr() < b->local_addr();
	}
      };

      typedef utl::Index<std::multiset,
			 std::set<ulink_t>,
			 ilinkless>		lset_t;

      struct lset_valid {
	bool operator()(This&, const lset_t::iterator& pos) const {
	  return pos->is_valid();
	}
      };

      typedef utl::Subset<utl::MultiAdapt<lset_t>,
			  utl::NoAction<This, lset_t::iterator>,
			  lset_valid,
			  This>			val_lset_t;

      explicit inline Neighbor();
    public:
      typedef val_lset_t			linkset_t;

      inline Neighbor(const address_t& maddr,
		      const std::set<ulink_t>::iterator& i,
		      unsigned w);

      inline Neighbor(const This&);

      virtual ~Neighbor() {}

      const address_t&	main_addr()   const { return m_addr_;      }
      unsigned		willingness() const { return willingness_; }
      const timeval_t&	mprsel_time() const { return mprsel_time_; }

      inline void	set_willingness(unsigned w);

      bool		is_sym() const { return sym_; }
      bool		is_mpr() const { return mpr_; }
      inline bool	is_mprsel() const;
      inline bool	was_mprsel();

      void		set_sym(bool state) { sym_ = state; }

      void		set_mpr(bool state) { mpr_ = state; }

      inline void	set_mprsel(const timeval_t& validity);

      inline void	unset_mprsel();

      inline std::pair<linkset_t::const_iterator, linkset_t::const_iterator>
			find_lifaces(const address_t& laddr) const;

//       bool		is_liface(const address_t& iaddr) const;

      linkset_t&	linkset() { return val_lset_; }

      inline void	insert(const std::set<ulink_t>::iterator& pos);

      inline void	erase(const lset_t::iterator& pos);
      inline void	erase(const std::set<ulink_t>::iterator& pos);

      inline bool	operator<(const This& rhs) const;

      static inline const This&	make_key(const address_t& m);

    private:
      const address_t	m_addr_;
      lset_t		lset_;
      val_lset_t	val_lset_;
      bool		sym_;
      bool		mpr_;
      timeval_t		mprsel_time_;
      bool		was_mprsel_;
      unsigned		willingness_;

      static Neighbor	dummy_for_find_;

      friend class sch::StatePrinter;
    };

    namespace upd {

      class NeighborUpdater {
	typedef NeighborUpdater			This;
	typedef CoherenceProxy			Set;
	typedef sch::Updatable<Neighbor>	elem_t;
	typedef std::set<elem_t>::iterator	Iter;
	typedef sch::UpdateEvent<This>		updater_t;
      public:
	NeighborUpdater(Set& s, const Iter& i)
	  : set_(s),
	    iter_(i)
	{}
	inline void	operator()();
      private:
	Set&	set_;
	Iter	iter_;
      };

    } // namespace upd

    /////////////////////////////////////////////////
    //  The coherence management proxy
    //

    class CoherenceProxy {
      typedef CoherenceProxy	This;

//       typedef internal::nested_type_traits<This>	ntt_t;

      typedef sch::Updatable<Link>			ulink_t;
      typedef sch::UpdateEvent<upd::LinkUpdater>	link_updater_t;
      typedef std::set<ulink_t>				lset_t;

      typedef sch::Updatable<Neighbor>			uneighbor_t;
      typedef sch::UpdateEvent<upd::NeighborUpdater>	neighbor_updater_t;

      struct stampable_nset_ : public std::set<uneighbor_t>,
			       public utl::Stampable {};

      typedef stampable_nset_	nset_t;

      // Some functors
      template <class Set, class Iter>
      struct nset_valid {
	bool operator()(Set& explicit_this, const Iter& pos) const {
	  return explicit_this.is_valid(pos);
	}
      };

      // Valid Link set
      typedef utl::Subset<lset_t>	val_lset_t;

      // Valid Neighbor set
      typedef utl::Subset<nset_t,
			  utl::NoAction<This, nset_t::iterator>,
			  nset_valid<This, nset_t::iterator>,
			  This>		val_nset_t;

      template <class Set>
      struct nset_sym {
	bool operator()(Set&, const typename Set::iterator& pos) const {
	  return pos->is_sym();
	}
      };

      // Symmetric Neighbor set
      typedef utl::Subset<val_nset_t,
			  utl::NoAction<val_nset_t>,
			  nset_sym<val_nset_t> >	sym_val_nset_t;

      typedef utl::MSIndex<lset_t, 0, address_t>	idx_lset_t;

      // First multistamped index on Neighbors
      typedef utl::MSIndex<nset_t, 0, address_t>	idx_nset_t;

//       // Second multistamped index on Neighbors
//       typedef utl::MSIndex<nset_t, 1, address_t>	idx1_nset_t;

      typedef utl::Subset<utl::MSAdapt<idx_lset_t>,
			  utl::NoAction<idx_lset_t>,
			  utl::DefaultPredicate<idx_lset_t>,
			  idx_lset_t>			val_idx_lset_t;

      // First valid multistamped index on Neighbors
      typedef utl::Subset<utl::MSAdapt<idx_nset_t>,
			  utl::NoAction<This, idx_nset_t::iterator>,
			  nset_valid<This, idx_nset_t::iterator>,
			  This>				val_idx_nset_t;

//       // Second valid multistamped index on Neighbors
//       typedef utl::Subset<utl::MSAdapt<idx1_nset_t>,
// 			  utl::NoAction<This, idx1_nset_t::iterator>,
// 			  nset_valid<This, idx1_nset_t::iterator>,
// 			  This>		val_idx1_nset_t;

      struct val_idx_nset_stamper {
	void operator()(val_idx_nset_t& set_ref,
			val_idx_nset_t::iterator pos) const {
	  set_ref.set_stamp(pos);
	}
      };

      typedef utl::Subset<utl::MSAdapt<val_idx_nset_t>,
			  val_idx_nset_stamper,
			  nset_sym<val_idx_nset_t>,
			  val_idx_nset_t>		sym_val_idx_nset_t;
    public:
      inline CoherenceProxy();

      virtual inline ~CoherenceProxy();

      typedef val_lset_t		linkset_t;
      typedef val_idx_lset_t		hello_linkset_t;

      typedef val_nset_t		neighborset_t;
      typedef sym_val_nset_t		sym_neighborset_t;
      typedef sym_val_idx_nset_t	tc_neighborset_t;

      ///
      /// Link accounting method.
      /// @arg local receiving interface address
      /// @arg remote sending interface address
      /// @arg main address of sending neighbor
      /// @arg linktype link type as advertised by the neighbor
      /// @arg validity validity time of the link
      /// @return iterator to the neighbor that was updated and a flag
      /// if this neighbor is new.
      ///
      /// This is to be called by msg::HELLO::parse() and takes care
      /// of all the coherence maintenance between link and neighbor
      /// sets.
      ///
      ///
# ifdef QOLYESTER_ENABLE_LINKHYS
      inline std::pair<neighborset_t::iterator, bool>
      insert_link(const msg::Message::header& mh,
		  const int linktype,
		  const int will,
		  const timeval_t& htime);
# else // !QOLYESTER_ENABLE_LINKHYS
      inline std::pair<neighborset_t::iterator, bool>
      insert_link(const msg::Message::header& mh,
		  const int linktype,
		  const int will);
# endif
      ///
      /// Willingness setting proxy method.
      /// @arg pos iterator to the neighbor
      /// @arg w willingness to set it to
      ///
      /// This method takes care of the coherence wrt. the path graph.
      /// If the willingness is set to <tt>> WILL_NEVER</tt>, a node
      /// in the graph must exist with a weight of <tt>w - 1</tt> if
      /// the node is not an MPR and a weight of <tt>w - 1 +
      /// WILL_DEFAULT</tt> otherwise.  If the willingness is set to
      /// @c WILL_DEFAULT, the node must be removed from the graph.
      ///
      inline void	set_willingness(const nset_t::iterator& pos,
					unsigned w);

      ///
      /// Graph updating method.
      /// @arg pos iterator to the neighbor that has changed
      ///
      inline void	update_graph(const nset_t::iterator& pos);

      ///
      /// Neighbor state updating method.
      /// @arg pos iterator to the neighbor to be updated
      /// @return true if the neighbor is still valid, false otherwise.
      ///
      inline bool	update_state(const nset_t::iterator& pos);

      inline void	update_state(const address_t& maddr);

      ///
      /// Method to set a neighbor's symmetric flag
      /// @arg pos iterator to the neighbor
      ///
      inline void	set_sym(const nset_t::iterator& pos);

      ///
      /// Method to unset a neighbor's symmetric flag
      /// @arg pos iterator to the neighbor
      ///
      inline void	unset_sym(const nset_t::iterator& pos);

      inline void	set_mpr(const nset_t::iterator& pos);
      inline void	set_mpr(const sym_neighborset_t::iterator& pos);

      inline void	unset_mpr(const nset_t::iterator& pos);
      inline void	unset_mpr(const sym_neighborset_t::iterator& pos);

      inline void	set_mprsel(const nset_t::iterator& pos,
				   const timeval_t& v);

      inline void	unset_mprsel(const nset_t::iterator& pos);

      inline bool	is_advset_empty();

      bool	is_hold_expired() const {
	return nset_.expired(cst::top_hold_time);
      }

      void	stamp_hold() {
	nset_.set_stamp();
      }

      inline bool	is_valid(const nset_t::iterator& pos);
      inline bool	is_valid(const idx_nset_t::iterator& pos);
//    inline bool	is_valid(const idx1_nset_t::iterator& pos);

      inline void	erase(lset_t::iterator pos);
    private:
      inline void	erase_from_all(nset_t::iterator pos);
    public:
      // Subsets accessors

      linkset_t&		linkset() {
	return linkset_;
      }

      hello_linkset_t&		hello_linkset() {
	return hello_linkset_;
      }

      neighborset_t&		neighborset() {
	return neighborset_;
      }

      sym_neighborset_t&	sym_neighborset() {
	return sym_neighborset_;
      }

      tc_neighborset_t&		tc_neighborset() {
	return tc_neighborset_;
      }

      const utl::Seqnum<u_int16_t>&	advset_seqnum() {
	if (advset_changed_) {
	  ++advset_seqnum_;
	  advset_changed_ = false;
	}
	return advset_seqnum_;
      }

      // Interface addition for multistamped indexes
      inline void	add_interface(const address_t& a);
      // Interface removal for multistamped indexes
      inline void	remove_interface(const address_t& a);

    private:
      lset_t			lset_;
      nset_t			nset_;

      linkset_t			linkset_;

      idx_lset_t		idx_lset_;
      hello_linkset_t		hello_linkset_;

      neighborset_t		neighborset_;

      sym_neighborset_t		sym_neighborset_;

      idx_nset_t		idx_nset_;
      val_idx_nset_t		val_idx_nset_;
      tc_neighborset_t		tc_neighborset_;

      utl::Seqnum<u_int16_t>	advset_seqnum_;
      bool			advset_changed_;

      unsigned			sym_count_;
      unsigned			mpr_count_;
      bool			mprsel_empty_;

      friend class sch::StatePrinter;

    };

    class TwoHopNeighbor {
      typedef TwoHopNeighbor		This;

      inline TwoHopNeighbor();
    public:
      inline TwoHopNeighbor(const address_t& a,
			    const address_t& tha,
			    const timeval_t&   validity);

      virtual ~TwoHopNeighbor() {}

      // Accessors
      const address_t&	main_addr()   const { return mainaddr_;   }
      const address_t&	twohop_addr() const { return twohopaddr_; }
      const timeval_t&	time()        const { return time_;       }

      // Modifiers
      void	set_time(const timeval_t& t) { time_ = t; }

      bool	is_valid() const { return !time_.is_past(); }

      inline bool	operator<(const This& rhs) const;

      inline This&	operator=(const This& other);

      static inline const This&	make_key(const address_t& ma,
					 const address_t& tha);
    private:
      const address_t	mainaddr_;
      const address_t	twohopaddr_;
      timeval_t		time_;

      static This	dummy_for_find_;

      friend class sch::StatePrinter;
    };

    class TwoHopNeighborSet {
      typedef TwoHopNeighborSet					This;
      typedef sch::Updatable<TwoHopNeighbor>			elem_t;
      typedef std::set<elem_t>					tset_t;
      typedef sch::upd::SetEraser<This, tset_t::iterator>	eraser_t;
      typedef sch::UpdateEvent<eraser_t>			updater_t;

      typedef utl::Subset<tset_t,
			  utl::NoAction<This, tset_t::iterator>,
			  utl::DefaultPredicate<This, tset_t::iterator>,
			  This>		val_tset_t;
    public:
      typedef val_tset_t		thnset_t;

      thnset_t&	thnset() { return thnset_; }

      inline TwoHopNeighborSet();
      virtual ~TwoHopNeighborSet() {}

      inline void	insert(const TwoHopNeighbor& x);
      inline void	erase(const tset_t::iterator& pos);
      inline void	erase(const tset_t::value_type& x);

    private:
      tset_t	tset_;
      thnset_t	thnset_;

      friend class sch::StatePrinter;
    };

  } // namespace set

  typedef set::CoherenceProxy		cproxy_t;
  typedef set::TwoHopNeighborSet	thnset_t;

} // namespace olsr

# include "neighbors.hxx"

#endif // ! QOLYESTER_DAEMON_SET_NEIGHBORS_HH
