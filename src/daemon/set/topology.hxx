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

#ifndef QOLYESTER_DAEMON_SET_TOPOLOGY_HXX
# define QOLYESTER_DAEMON_SET_TOPOLOGY_HXX 1

# include "gra/graph.hh"
# include "set/neighbors.hh"

# include "topology.hh"

namespace olsr {

  extern sched_t	scheduler;
  extern pathnet_t	path_net;
  extern cproxy_t	cproxy;

  namespace set {

    TopologyEntry::TopologyEntry() :
      daddr_(),
      laddr_(),
      seqnum_(0),
      time_(0, 0)
    {}
  
    TopologyEntry::TopologyEntry(const address_t& d, const address_t& l,
				 u_int16_t sn, const timeval_t& v) :
      daddr_(d),
      laddr_(l),
      seqnum_(sn),
      time_(timeval_t::in(v))
    {}

    bool
    TopologyEntry::is_valid() const { return !time_.is_past(); }

    void
    TopologyEntry::set_time(const timeval_t& t) { time_ = t; }

    bool
    TopologyEntry::operator<(const This& rhs) const {
      return laddr_ < rhs.laddr_;
    }

    const TopologyEntry&
    TopologyEntry::make_key(const address_t& l) {
      const_cast<address_t&>(dummy_for_find_.laddr_) = l;
      return dummy_for_find_;
    }

    TopologySet::TopologySet()
      : tset_(),
	toposet_(*this, tset_)
    {}

    void
    TopologySet::insert(const TopologyEntry& x) {
      tset_t::iterator	pos = tset_.insert(x);

      sch::TimedEvent*	e = new updater_t(x.time(), eraser_t(*this, pos));
      const_cast<elem_t&>(*pos).set_updater(e);
      scheduler.insert(e);

      path_net.insert_node(gra::AdjNode(x.last_addr()));
      path_net.insert_node(gra::AdjNode(x.dest_addr()));
      path_net.insert_arc(gra::AdjInfo(x.last_addr(), x.dest_addr(), topo));
      routes_recomp.set_mark();
    }

    void
    TopologySet::erase(const tset_t::iterator& pos) {
      path_net.remove_arc(gra::AdjInfo::make_key(pos->last_addr(),
						 pos->dest_addr(),
						 topo));
      if (cproxy.sym_neighborset().find(Neighbor::make_key(pos->last_addr())) ==
	  cproxy.sym_neighborset().end())
	path_net.remove_node_if_alone(gra::AdjNode::make_key(pos->last_addr()));
      if (cproxy.sym_neighborset().find(Neighbor::make_key(pos->dest_addr())) ==
	  cproxy.sym_neighborset().end())
	path_net.remove_node_if_alone(gra::AdjNode::make_key(pos->dest_addr()));
//       del_set.erase(pos->del_pos());

      sch::TimedEvent*	e = pos->updater();
      tset_.erase(pos);
      scheduler.destroy(e);
      routes_recomp.set_mark();
    }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_TOPOLOGY_HXX
