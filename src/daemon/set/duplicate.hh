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

#ifndef QOLYESTER_DAEMON_SET_DUPLICATE_HH
# define QOLYESTER_DAEMON_SET_DUPLICATE_HH 1

# include <sys/types.h>
# include <set>
# include "utl/timeval.hh"
# include "sch/scheduler.hh"
# include "utl/set.hh"
# include "cst/constants.hh"
# include "net/ipaddress.hh"

namespace olsr {

  namespace sch {

    class StatePrinter;

  }

  namespace set {

    class DuplicateEntry {
      typedef DuplicateEntry		This;
      typedef std::set<address_t>	ifaces_t;

      inline DuplicateEntry();
    public:
      inline DuplicateEntry(const address_t& a, u_int16_t s,
			    const address_t& i, bool r);

      virtual ~DuplicateEntry() {}

      bool		is_valid() const { return !time_.is_past(); }
      const timeval_t&	time() const { return time_; }
      void		set_time(const timeval_t& t =
				 timeval_t::in(cst::dup_hold_time)) {
	time_ = t;
      }

      bool retransmitted() const { return retransmitted_; }
      void set_retransmitted() { retransmitted_ = true; }

      ifaces_t&		ifaces() { return ifaces_; }
      const ifaces_t&	ifaces() const { return ifaces_; }

      inline bool in_ifaces(const address_t& a) const;
      inline void add_iface(const address_t& a);
      inline bool operator<(const This& rhs) const;

      static inline This	make_key(const address_t& a, u_int16_t s);
    private:
      const address_t	addr_;
      const u_int16_t	seqnum_;
      bool		retransmitted_;
      ifaces_t		ifaces_;
      timeval_t		time_;

      static This	dummy_for_find_;

      friend class sch::StatePrinter;
    };

    class DuplicateSet {
      typedef DuplicateSet					This;
      typedef sch::Updatable<DuplicateEntry>			elem_t;
      typedef std::set<elem_t>					dset_t;
      typedef sch::upd::SetEraser<This, dset_t::iterator>	eraser_t;
      typedef sch::UpdateEvent<eraser_t>			updater_t;

      typedef utl::Subset<dset_t,
			  utl::NoAction<This, dset_t::iterator>,
			  utl::DefaultPredicate<This, dset_t::iterator>,
			  This>					val_dset_t;
    public:
      typedef val_dset_t		dupset_t;

      dupset_t&	dupset() { return dupset_; }

      inline DuplicateSet();

      inline void	insert(const DuplicateEntry& x);
      inline void	erase(const dset_t::iterator& pos);
    private:
      dset_t	dset_;
      dupset_t	dupset_;

      friend class sch::StatePrinter;
    };

  } // namespace set

  typedef set::DuplicateSet	dupset_t;
  extern dupset_t		dup_set;

} // namespace olsr

# include "duplicate.hxx"

#endif // ! QOLYESTER_DAEMON_SET_DUPLICATE_HH
