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

#ifndef QOLYESTER_DAEMON_SET_DUPLICATE_HXX
# define QOLYESTER_DAEMON_SET_DUPLICATE_HXX 1

# include "duplicate.hh"

namespace olsr {

  extern sch::Scheduler	scheduler;

  namespace set {

    DuplicateEntry::DuplicateEntry() :
      addr_(),
      seqnum_(),
      retransmitted_(false),
      ifaces_(),
      time_(0)
    {}

    DuplicateEntry::DuplicateEntry(const address_t& a, u_int16_t s,
				   const address_t& i, bool r) :
      addr_(a),
      seqnum_(s),
      retransmitted_(r), 
      ifaces_(),
      time_(timeval_t::in(cst::dup_hold_time)) {
      ifaces_.insert(i);
    }

    bool 
    DuplicateEntry::in_ifaces(const address_t& a) const {
      return ifaces_.find(a) != ifaces_.end();
    }

    void
    DuplicateEntry::add_iface(const address_t& a) {
      ifaces_.insert(a);
    }

    bool
    DuplicateEntry::operator<(const This& rhs) const {
      if (addr_ == rhs.addr_)
	return seqnum_ < rhs.seqnum_;
      return addr_ < rhs.addr_;
    }

    DuplicateEntry
    DuplicateEntry::make_key(const address_t& a, u_int16_t s) {
      This	tmp = dummy_for_find_;
      const_cast<address_t&>(tmp.addr_) = a;
      const_cast<u_int16_t&>(tmp.seqnum_) = s;
      return tmp;
    }

    DuplicateSet::DuplicateSet()
      : dset_(),
	dupset_(*this, dset_)
    {}

    void
    DuplicateSet::insert(const DuplicateEntry& x) {
      std::pair<dset_t::iterator, bool>	p = dset_.insert(x);
      if (!p.second) {
	sch::TimedEvent*	e = p.first->updater();
	scheduler.erase(e);
	e->set_next(x.time());
	scheduler.insert(e);
	
	const_cast<elem_t&>(*p.first).set_time(x.time());
	const_cast<elem_t&>(*p.first).ifaces().insert(x.ifaces().begin(),
						     x.ifaces().end());
	if (x.retransmitted())
	  const_cast<elem_t&>(*p.first).set_retransmitted();
      } else {
	updater_t*	e = new updater_t(x.time(), eraser_t(*this, p.first));
	const_cast<elem_t&>(*p.first).set_updater(e);
	scheduler.insert(e);
      }
    }

    void
    DuplicateSet::erase(const dset_t::iterator& pos) {
      sch::TimedEvent*	e = pos->updater();
      dset_.erase(pos);
      scheduler.destroy(e);
    }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_DUPLICATE_HXX
