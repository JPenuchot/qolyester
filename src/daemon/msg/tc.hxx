// Copyright (C) 2003-2006 Laboratoire de Recherche en Informatique

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

#ifndef QOLYESTER_DAEMON_MSG_TC_HXX
# define QOLYESTER_DAEMON_MSG_TC_HXX 1

# include "set/topology.hh"

# include "tc.hh"

namespace olsr {

  extern toposet_t	topo_set;
  extern std::ostream	dump_tc;
  extern bool		do_dump_tc;

  namespace msg {

    TCMessage::TCMessage() : _data() {}

    TCMessage::TCMessage(const utl::Data& d) : _data(d) {}

    bool
    TCMessage::dump(utl::Data& d, const address_t& interface) const {
      bool	ret = false;

      if (_data.empty()) { // we want to generate a new message

	if (d.size() < Message::min_length + min_length)
	  return true; // try again immediately

	if (!cproxy.is_advset_empty())
	  cproxy.stamp_hold();

	if (do_dump_tc)
	  dump_tc << "TC preparation (" << interface << ") (ANSN: "
		  << cproxy.advset_seqnum() << ") "
		  << Message::seqnum << " {\n";

	Message::raw*	mraw = reinterpret_cast<Message::raw*>(d.raw());
	raw*		traw = reinterpret_cast<raw*>(mraw->data);

	d += sizeof *mraw + sizeof *traw;

	// Dump message header
	mraw->type     = TC_MESSAGE;			// Message Type
	mraw->vtime    = utl::Vtime(cst::top_hold_time);// VTime
	// mraw->size  = 0;				// set later
	main_addr.dump(mraw->addr);
	mraw->ttl      = 255;				// Time To Live
	mraw->hopcount = 0;				// Hop Count
	mraw->seqnum   = htons(Message::seqnum);	// Sequence number

	traw->ansn     = htons(cproxy.advset_seqnum());
	traw->reserved = 0;

	typedef std::list<cproxy_t::tc_neighborset_t::iterator> nset_t;

	nset_t		mn_set;
	nset_t		others_set;
	bool		some_sym = false;
	unsigned	bytes_so_far = 0;

	for (cproxy_t::tc_neighborset_t::iterator n =
	       cproxy.tc_neighborset().begin(interface);
	     n != cproxy.tc_neighborset().end(interface); ++n) {

	  if (cproxy.tc_neighborset().stamp(n) ==
	      timeval_t::now())
	    break;

	  if (!n->is_mprsel()) {
	    if (tc_redundancy == mprselset) {
	      others_set.push_back(n);
	      continue;
	    } else if (!n->is_mpr())
	      if (tc_redundancy == mprselset_mprset) {
		others_set.push_back(n);
		continue;
	      }
	  } else
	    some_sym = true;


	  if (d.size() - bytes_so_far < ADDRESS_SIZE) {
	    if (cproxy.tc_neighborset().expired(n, cst::tc_interval,
						timeval_t::in(cst::tc_interval)))
	      ret = true;
	    break;
	  }

	  mn_set.push_back(n);
	  bytes_so_far += ADDRESS_SIZE;
	}

	if (some_sym) {
	  for (nset_t::iterator i = mn_set.begin();
	       i != mn_set.end(); d += ADDRESS_SIZE, ++i) {

	    if (do_dump_tc)
	      dump_tc << "  " << (*i)->main_addr() << '\n';

	    (*i)->main_addr().dump(d.raw());
	    cproxy.tc_neighborset().set_stamp(*i);
	  }
	}

	for (nset_t::iterator i = others_set.begin();
	     i != others_set.end(); ++i)
	  cproxy.tc_neighborset().set_stamp(*i);

	mraw->size = htons(d.raw() - reinterpret_cast<u_int8_t*>(mraw));

	if (do_dump_tc)
	  dump_tc << '}' << std::endl;

      } else {

	if (d.size() < _data.size())
	  return true;

	_data.dump(d);
	d += _data.size();
      }

      return ret;
    }

    void
    TCMessage::parse(const utl::ConstData& d, const Message::header& mh) {
      const raw*	h = reinterpret_cast<const raw*>(d.raw());

      if (do_dump_tc)
	dump_tc << "TC from " << mh.originator
		<< " (" << mh.sender << " -> " << mh.receiver << ") (ANSN: "
		<< ntohs(h->ansn) << ") M(" << mh.mseqnum << ") P("
		<< mh.pseqnum << ") Hc(" << mh.hopcount << ") {\n";

      cproxy_t::linkset_t::iterator	l =
	cproxy.linkset().find(set::Link::make_key(mh.receiver, mh.sender));

      if (l == cproxy.linkset().end() || !l->is_sym()) {

	if (do_dump_tc)
	  dump_tc << "  (ignored since not from symmetric neighbor)\n}"
		     << std::endl;
	return;
      }

      // From now on, the message is OK.

      utl::ConstData	payload = d + sizeof (raw);

      // Extract the topology tuples.
      std::pair<toposet_t::toposet_t::iterator, toposet_t::toposet_t::iterator>
	x = topo_set.toposet().equal_range(set::TopologyEntry::
					   make_key(mh.originator));

      // Check if there is a topology tuple with higher seqnum.
      for (toposet_t::toposet_t::iterator i = x.first; i != x.second; ++i)
	if (i->seqnum() > utl::Seqnum<u_int16_t>(ntohs(h->ansn))) {
	  if (do_dump_tc)
	    dump_tc << "  (ignored since out of order)\n}" << std::endl;
	  return;
	}

      // Erase all "old" tuples (with lower seqnum).
      for (toposet_t::toposet_t::iterator i = x.first;
	   i != x.second; /* at end */)
	if (i->seqnum() < utl::Seqnum<u_int16_t>(ntohs(h->ansn))) {
	  toposet_t::toposet_t::iterator	tmp = i++;
	  topo_set.erase(tmp);
	} else
	  ++i;

      // Get all the "current" tuples.
      x = topo_set.toposet().equal_range(set::TopologyEntry::
					 make_key(mh.originator));

      // Extract the payload of the message.
      const u_int8_t	(*addrs)[ADDRESS_SIZE] =
	reinterpret_cast<const u_int8_t (*)[ADDRESS_SIZE]>(payload.raw());
      unsigned		len = payload.size() / ADDRESS_SIZE;

      for (unsigned i = 0; i < len; ++i) {
	address_t		addr(addrs[i], ADDRESS_SIZE);

	if (do_dump_tc)
	  dump_tc << "  "  << addr << '\n';

	if (addr == main_addr)
	  continue;

	// The following distinction is necessary, since toposet_t is
	// a multiset, so duplicate entries may appear if no explicit
	// check for existing tuples is done.

	// Update validity of the tuple if it exists.
	bool			found = false;
	for (toposet_t::toposet_t::iterator k = x.first;
	     k != x.second; ++k)
	  if (k->dest_addr() == addr) {
	    sch::TimedEvent*	e = k->updater();
	    scheduler.erase(e);
	    e->set_next(timeval_t::in(mh.validity));
	    k->set_time(timeval_t::in(mh.validity));
	    scheduler.insert(e);
	    found = true;
	  }

	// Or insert a new tuple in the set.
	if (!found) {
	  topo_set.insert(set::TopologyEntry(addr,
					     mh.originator,
					     ntohs(h->ansn),
					     mh.validity));
	}
      }

      if (do_dump_tc)
	dump_tc << '}' << std::endl;
    }

  } // namespace msg

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_MSG_TC_HXX
