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

#ifndef QOLYESTER_DAEMON_SET_TOPOLOGY_HH
# define QOLYESTER_DAEMON_SET_TOPOLOGY_HH 1

# include <sys/types.h>

# include "net/ipaddress.hh"
# include "sch/scheduler.hh"
# include "utl/seqnum.hh"
# include "utl/set.hh"
# include "utl/timeval.hh"

namespace olsr {

  namespace sch {

    class StatePrinter;

  }

  namespace set {

    class TopologyEntry {
      typedef TopologyEntry	This;
      inline TopologyEntry();
    public:
      inline TopologyEntry(const address_t& d, const address_t& l,
			   u_int16_t sn, const timeval_t& v);

      virtual ~TopologyEntry() {}

      const address_t&		dest_addr() const { return daddr_;  }
      const address_t&		last_addr() const { return laddr_;  }
      utl::Seqnum<u_int16_t>	seqnum()    const { return seqnum_; }
      const timeval_t&		time()      const { return time_; }

      inline bool	is_valid() const;
      inline void	set_time(const timeval_t& t);
      inline bool	operator<(const This& rhs) const;

      static inline const This&	make_key(const address_t& l);
    private:
      const address_t		daddr_;
      const address_t		laddr_;
      utl::Seqnum<u_int16_t>	seqnum_;
      timeval_t			time_;

      static This		dummy_for_find_;

      friend class sch::StatePrinter;
    };

    class TopologySet {
      typedef TopologySet					This;
      typedef sch::Updatable<TopologyEntry>			elem_t;
      typedef std::multiset<elem_t>				tset_t;
      typedef sch::upd::SetEraser<This, tset_t::iterator>	eraser_t;
      typedef sch::UpdateEvent<eraser_t>			updater_t;

      typedef utl::Subset<utl::MultiAdapt<tset_t>,
			  utl::NoAction<This, tset_t::iterator>,
			  utl::DefaultPredicate<This, tset_t::iterator>,
			  This>		val_tset_t;
    public:
      typedef val_tset_t		toposet_t;

      toposet_t&	toposet() { return toposet_; }

      inline TopologySet();

      inline void	insert(const TopologyEntry& x);
      inline void	erase(const tset_t::iterator& pos);
    private:
      tset_t	tset_;
      toposet_t	toposet_;

      friend class sch::StatePrinter;
    };

  } // namespace set

  typedef set::TopologySet	toposet_t;

} // namespace olsr

# include "topology.hxx"

#endif // ! QOLYESTER_DAEMON_SET_TOPOLOGY_HH
