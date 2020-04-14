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

#ifndef QOLYESTER_DAEMON_SET_MID_HH
# define QOLYESTER_DAEMON_SET_MID_HH 1

# include <set>
# include "utl/timeval.hh"
# include "utl/set.hh"
# include "sch/scheduler.hh"
# include "net/ipaddress.hh"

namespace olsr {

  namespace sch {

    class StatePrinter;

  }

  namespace set {

    class MIDEntry {
      typedef MIDEntry	This;
      inline MIDEntry();
    public:
      inline MIDEntry(const address_t& iaddr,
		      const address_t& maddr,
		      const timeval_t& vtime);

      virtual ~MIDEntry() {}

      const address_t&	iface_addr() const { return i_addr_; }
      const address_t&	main_addr()  const { return m_addr_; }
      const timeval_t&	time() const { return time_; }

      inline bool	is_valid() const;
      inline void	set_time(const timeval_t& t);
      inline void	set_main_addr(const address_t& maddr);
      inline bool	operator<(const This& rhs) const;

      inline This&	operator=(const This& other);

      static inline This	make_key(const address_t& iaddr);
    private:
      const address_t	i_addr_;
      address_t		m_addr_;
      timeval_t		time_;

      static This	dummy_for_find_;

      friend class sch::StatePrinter;
    };

    class MIDSet {
      typedef MIDSet						This;
      typedef sch::Updatable<MIDEntry>				elem_t;
      typedef std::set<elem_t>					mset_t;
      typedef sch::upd::SetEraser<This, mset_t::iterator>	eraser_t;
      typedef sch::UpdateEvent<eraser_t>			updater_t;

      typedef utl::Subset<mset_t,
			  utl::NoAction<This, mset_t::iterator>,
			  utl::DefaultPredicate<This, mset_t::iterator>,
			  This>	val_mset_t;
    public:
      typedef val_mset_t	midset_t;

      midset_t&	midset() { return midset_; }

      inline MIDSet();

      inline void	insert(const MIDEntry& x);
      inline void	erase(const mset_t::iterator& pos);
    private:
      mset_t	mset_;
      midset_t	midset_;

      friend class sch::StatePrinter;

    };

  } // namespace set

  typedef set::MIDSet	midset_t;

} // namespace olsr

# include "mid.hxx"

#endif // ! QOLYESTER_DAEMON_SET_MID_HH
