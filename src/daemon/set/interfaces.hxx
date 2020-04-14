// Copyright (C) 2003, 2004, 2005, 2006 Laboratoire de Recherche en Informatique

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

#ifndef QOLYESTER_DAEMON_SET_INTERFACES_HXX
# define QOLYESTER_DAEMON_SET_INTERFACES_HXX 1

# include <map>
# include <cassert>

# include "config.hh"

# include "pkt/packet.hh"
# include "sch/events.hh"
# include "sch/scheduler.hh"
# include "set/gate.hh"
# include "utl/comparator.hh"

# include "interfaces.hh"

namespace olsr {

  extern address_t	main_addr;
  extern gateset_t	gate_set;
  extern cproxy_t	cproxy;
  extern sched_t	scheduler;
  extern ifaceset_t	iface_set;

  namespace set {

    InterfaceSet::InterfaceSet()
      : _iset(),
	_mid_iset(_iset),
	_min_mtu(0)
    {}

    InterfaceSet::iterator
    InterfaceSet::begin() const {
      return _iset.begin();
    }

    InterfaceSet::iterator
    InterfaceSet::end() const {
      return _iset.end();
    }

    bool
    InterfaceSet::empty() const {
      return _iset.empty();
    }

    size_t
    InterfaceSet::size() const {
      return _iset.size();
    }

    const iface_t&
    InterfaceSet::operator[](const address_t& iaddr) const {
      const_iterator	i = find(iaddr);
      assert(i != end());
      return *i;
    }

    std::pair<InterfaceSet::iterator, bool>
    InterfaceSet::insert(const iset_t::value_type& x) {
      if (main_addr == address_t())
	main_addr = x.addr();
      if (_iset.find(x) == _iset.end())
	_mid_iset.add_key(x.addr());
      std::pair<iset_t::iterator, bool>	p = _iset.insert(x);
      if (p.second) {
	_mid_iset.insert(p.first);
	if (_min_mtu == 0 || x.mtu() < _min_mtu)
	  _min_mtu = x.mtu();
	cproxy.add_interface(x.addr());
	gate_set.hna_gset().add_key(x.addr());
	scheduler.insert(new sch::PacketReceiver(const_cast<iface_t*>(&*p.first)));
      }
//       const_cast<iset_t::value_type&>(x).configure();
      return p;
    }

    void
    InterfaceSet::erase(iterator pos) {
      assert(pos != end());
//       const_cast<iset_t::value_type&>(*pos).unconfigure();
      const_cast<iface_t&>(*pos).destroy_all_events();
      gate_set.hna_gset().remove_key(pos->addr());
      cproxy.remove_interface(pos->addr());
      if (pos->addr() == main_addr)
	main_addr = address_t();
      _mid_iset.remove_key(pos->addr());
      _mid_iset.erase(pos);
      _iset.erase(pos);
      _min_mtu = 0;
      for (iset_t::iterator i = _iset.begin(); i != _iset.end(); ++i)
	if (_min_mtu == 0 || i->mtu() < _min_mtu)
	  _min_mtu = i->mtu();
      if (main_addr == address_t() && !_iset.empty())
	main_addr = _iset.begin()->addr();
    }

    InterfaceSet::iterator
    InterfaceSet::find(const address_t& iaddr) const {
      return _iset.find(iface_t::make_key(iaddr));
    }

    void
    InterfaceSet::send_messages() {

      assert(iface_set.empty() || min_mtu() > 0);

      // FIXME: this should not be that way, we should remove pending
      // messages if there are no current interfaces.
      if (iface_set.empty()) {
	pending_messages.clear();
	return;
      }

      while (!pending_messages.empty()) {

	typedef std::map<iface_t*,
	                 pkt::Packet,
	                 utl::pless<iface_t> >	packets_t;

	packets_t	packets;

	// Create a packet for each interface.
	for (iset_t::iterator i = _iset.begin(); i != _iset.end(); ++i)
	  packets.insert(packets_t::value_type(const_cast<iface_t*>(&*i),
					       pkt::Packet(_min_mtu,
							   i->addr())));

	// Add each message in all the packets.
	for (pending_t::iterator m = pending_messages.begin();
	     m != pending_messages.end(); /* ++m elsewhere */) {
	  // Add the message to the first packet.
	  bool	resend = packets.begin()->second.add_message(**m);

	  // Add the message to the remaining packets and check that
	  // it fits the same way.

	  // FIXME: this should be avoided, since the packets should
	  // be identical on every interface for we always broadcast
	  // messages on all interfaces.
	  for (packets_t::iterator i = ++packets.begin();
	       i != packets.end(); ++i) {
# ifndef NDEBUG
	    bool	resend_this = i->second.add_message(**m);
	    assert(resend_this == resend);
# else // !NDEBUG
	    i->second.add_message(**m);
# endif
	  }

	  // Increment the message sequence number.
	  const_cast<msg::Message*>(*m)->inc_seqnum();

	  // Destroy the message if it is not to be resent.
	  if (!resend) {
	    pending_t::iterator	tmp = m++;
	    const msg::Message*	mptr = *tmp;
	    pending_messages.erase(tmp);
	    delete mptr;
	  }
	}

	// Now send the packets.
	for (packets_t::iterator i = packets.begin();
	     i != packets.end(); ++i) {
	  i->second.close(i->first->addr());
	  if (i->second.size() > pkt::Packet::min_length)
	    i->first->shipout(i->second);
	}
      }
    }

//       for (iset_t::iterator i = iface_set._iset.begin();
// 	   i != iface_set._iset.end(); ++i) {
// 	pending_t	local_pm = pending_messages;

// 	while (!local_pm.empty()) {
// 	  pkt::Packet	p(min_mtu(), i->addr());

// 	  p.fill(local_pm);

// 	  if (p.size() > pkt::Packet::min_length) {
// 	    i->send(p);
// 	  }
// 	}
//       }
//       for (pending_t::iterator i = pending_messages.begin();
// 	   i != pending_messages.end(); ++i)
// 	delete *i;
//       pending_messages.clear();
//     }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_INTERFACES_HXX
