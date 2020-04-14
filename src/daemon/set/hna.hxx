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

#ifndef QOLYESTER_DAEMON_SET_HNA_HXX
# define QOLYESTER_DAEMON_SET_HNA_HXX 1

# include "utl/mark.hh"

# include "hna.hh"

namespace olsr {

  extern sch::Scheduler	scheduler;
  extern utl::Mark	routes_recomp;

  namespace set {

    HNAEntry::HNAEntry() :
      g_addr_(),
      n_addr_(),
      prefix_(0),
      time_(0, 0)
    {}

    HNAEntry::HNAEntry(const address_t& gaddr,
		       const address_t& naddr,
		       unsigned prefix,
		       const timeval_t& v) :
      g_addr_(gaddr),
      n_addr_(naddr),
      prefix_(prefix),
      time_(timeval_t::in(v))
    {}

    bool
    HNAEntry::is_valid() const { return !time_.is_past(); }

    void
    HNAEntry::set_time(const timeval_t& t) { time_ = t; }

    bool
    HNAEntry::operator<(const This& rhs) const {
      if (g_addr_ == rhs.g_addr_) {
	if (n_addr_ == rhs.n_addr_)
	  return prefix_ < rhs.prefix_;
	else
	  return n_addr_ < rhs.n_addr_;
      }
      return g_addr_ < rhs.g_addr_;
    }

    HNAEntry
    HNAEntry::make_key(const address_t& gaddr,
		       const address_t& naddr,
		       unsigned	prefix) {
      This	tmp = dummy_for_find_;

      const_cast<address_t&>(tmp.g_addr_) = gaddr;
      const_cast<address_t&>(tmp.n_addr_) = naddr;
      const_cast<unsigned&>(tmp.prefix_)  = prefix;

      return tmp;
    }

    HNASet::HNASet()
      : hset_(),
	hnaset_(*this, hset_)
    {}

    void
    HNASet::insert(const HNAEntry& x) {
      std::pair<hset_t::iterator, bool>	p = hset_.insert(x);
      if (!p.second) {
	sch::TimedEvent*	e = p.first->updater();
	scheduler.erase(e);
	e->set_next(x.time());
	scheduler.insert(e);

	const_cast<elem_t&>(*p.first).set_time(x.time());
      } else {
	updater_t*	e = new updater_t(x.time(), eraser_t(*this, p.first));
	const_cast<elem_t&>(*p.first).set_updater(e);
	scheduler.insert(e);
	routes_recomp.set_mark();
      }
    }

    void
    HNASet::erase(const hset_t::iterator& pos) {
      assert(pos != hset_.end());
//       del_set.erase(pos->del_pos());
      sch::TimedEvent*	e = pos->updater();
      hset_.erase(pos);
      scheduler.destroy(e);
      routes_recomp.set_mark();
    }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_HNA_HXX
