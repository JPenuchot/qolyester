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

#ifndef QOLYESTER_DAEMON_SET_MID_HXX
# define QOLYESTER_DAEMON_SET_MID_HXX 1

# include "utl/mark.hh"

# include "mid.hh"

namespace olsr {

  extern utl::Mark	routes_recomp;

  namespace set {

    MIDEntry::MIDEntry()
      : i_addr_(),
	m_addr_(),
	time_(0, 0)
    {}
    
    MIDEntry::MIDEntry(const address_t& iaddr, 
		       const address_t& maddr,
		       const timeval_t& vtime)
      : i_addr_(iaddr), 
	m_addr_(maddr), 
	time_(timeval_t::in(vtime))
    {}

    bool
    MIDEntry::is_valid() const { return !time_.is_past(); }

    void
    MIDEntry::set_time(const timeval_t& t) { time_ = t; }

    void
    MIDEntry::set_main_addr(const address_t& maddr) {
      m_addr_ = maddr;
    }

    bool
    MIDEntry::operator<(const This& rhs) const {
      return i_addr_ < rhs.i_addr_;
    }

    MIDEntry&
    MIDEntry::operator=(const This& other) {
      assert(i_addr_ == other.i_addr_);
      assert(m_addr_ == other.m_addr_);
      time_ = other.time_;
      return *this;
    }

    MIDEntry
    MIDEntry::make_key(const address_t& iaddr) {
      This	tmp = dummy_for_find_;
      
      const_cast<address_t&>(tmp.i_addr_) = iaddr;

      return tmp;
    }

    MIDSet::MIDSet()
      : mset_(),
	midset_(*this, mset_)
    {}

    void
    MIDSet::insert(const MIDEntry& x) {
      std::pair<mset_t::iterator, bool>	p = mset_.insert(x);
      if (!p.second) {
	sch::TimedEvent*	e = p.first->updater();
	scheduler.erase(e);
	e->set_next(x.time());
	scheduler.insert(e);

	if (p.first->main_addr() != x.main_addr()) {
	  const_cast<elem_t&>(*p.first).set_main_addr(x.main_addr());
	  routes_recomp.set_mark();
	}
	const_cast<elem_t&>(*p.first).set_time(x.time());
      }	else {
	sch::TimedEvent*	e = new updater_t(x.time(),
						  eraser_t(*this, p.first));
	const_cast<elem_t&>(*p.first).set_updater(e);
	scheduler.insert(e);
	routes_recomp.set_mark();
      }
    }

    void
    MIDSet::erase(const mset_t::iterator& pos) {
      sch::TimedEvent*	e = pos->updater();
      mset_.erase(pos);
      scheduler.destroy(e);
      routes_recomp.set_mark();
    }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_MID_HXX
