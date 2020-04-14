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

#ifndef QOLYESTER_DAEMON_SET_HNA_HH
# define QOLYESTER_DAEMON_SET_HNA_HH 1

# include <set>
# include "sch/scheduler.hh"
# include "utl/timeval.hh"
# include "utl/set.hh"
# include "net/ipaddress.hh"

namespace olsr {

  namespace sch {

    class StatePrinter;

  }

  namespace set {

    class HNAEntry {
      typedef HNAEntry	This;

      inline HNAEntry();
    public:
      inline HNAEntry(const address_t& gaddr,
		      const address_t& naddr,
		      unsigned prefix,
		      const timeval_t& v);

      virtual ~HNAEntry() {}

      const address_t&	gw_addr()	const { return g_addr_; }
      const address_t&	net_addr()	const { return n_addr_; }
      unsigned		prefix()	const { return prefix_; }
      inline bool	is_valid()	const;
      inline void	set_time(const timeval_t& t);
      const timeval_t&	time() const { return time_; }

      inline bool	operator<(const This& rhs) const;
      static inline This	make_key(const address_t& gaddr,
					 const address_t& naddr,
					 unsigned prefix);
    private:
      const address_t	g_addr_;
      const address_t	n_addr_;
      const unsigned	prefix_;
      timeval_t	time_;

      static This	dummy_for_find_;

      friend class sch::StatePrinter;
    };

    class HNASet {
      typedef HNASet						This;
      typedef sch::Updatable<HNAEntry>				elem_t;
      typedef std::set<elem_t>					hset_t;
      typedef sch::upd::SetEraser<This, hset_t::iterator>	eraser_t;
      typedef sch::UpdateEvent<eraser_t>			updater_t;

      typedef utl::Subset<hset_t,
			  utl::NoAction<This, hset_t::iterator>,
			  utl::DefaultPredicate<This, hset_t::iterator>,
			  This>	val_hset_t;
    public:
      typedef val_hset_t	hnaset_t;

      hnaset_t&	hnaset() { return hnaset_; }

      inline HNASet();

      inline void	insert(const HNAEntry& x);
      inline void	erase(const hset_t::iterator& pos);
    private:
      hset_t	hset_;
      hnaset_t	hnaset_;

      friend class sch::StatePrinter;
    };

  } // namespace set

  typedef set::HNASet	hnaset_t;

} // namespace olsr

# include "hna.hxx"

#endif // ! QOLYESTER_DAEMON_SET_HNA_HH
