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

#ifndef QOLYESTER_DAEMON_MSG_HNA_HXX
# define QOLYESTER_DAEMON_MSG_HNA_HXX 1

# include "set/gate.hh"
# include "set/hna.hh"
# include "set/neighbors.hh"
# include "utl/args.hh"

# include "hna.hh"

namespace olsr {

  extern std::ostream	dump_hna;
  extern gateset_t	gate_set;
  extern hnaset_t	hna_set;

  namespace msg {

    HNAMessage::HNAMessage() : _data() {}

    HNAMessage::HNAMessage(const utl::Data& d) : _data(d) {}

    bool
    HNAMessage::dump(utl::Data& d, const address_t& interface) const {
      bool	ret = false;

      if (_data.empty()) { // generate HNA message

	assert(!gate_set.empty());

	if (d.size() < Message::min_length + min_length)
	  return true; // try again immediately

	if (do_dump_hna)
	  dump_hna << "HNA preparation (" << interface << ") "
		   << Message::seqnum << " {\n";

	Message::raw*	mraw = reinterpret_cast<Message::raw*>(d.raw());

	d += sizeof *mraw;

	mraw->type     = HNA_MESSAGE;
	mraw->vtime    = utl::Vtime(cst::hna_hold_time);
	mraw->size     = 0; // set later
	main_addr.dump(mraw->addr);
	mraw->ttl      = 255;
	mraw->hopcount = 0;
	mraw->seqnum   = htons(Message::seqnum);

	typedef std::list<gateset_t::hna_gset_t::iterator>	hset_t;
	hset_t							h_set;

# ifdef QOLYESTER_ENABLE_NUMPREFIX
	for (gateset_t::hna_gset_t::iterator i =
	       gate_set.hna_gset().begin(interface);
	     i != gate_set.hna_gset().end(interface);
	     d += ADDRESS_SIZE + sizeof (HNAMessage::raw), ++i) {

	  if (gate_set.hna_gset().stamp(i) == timeval_t::now())
	    break;

	  if (d.size() < ADDRESS_SIZE + sizeof (HNAMessage::raw)) {
	    if (gate_set.hna_gset().expired(i,
					    cst::hna_interval,
					    timeval_t::in(cst::hna_interval)))
	      ret = true;
	    break;
	  }

	  if (do_dump_hna)
	    dump_hna << "  " << i->net_addr() << '/' << i->prefix() << '\n';

	  HNAMessage::raw*	hnaraw = reinterpret_cast<HNAMessage::raw*>(d.raw());
	  i->net_addr().dump(hnaraw->addr);
	  hnaraw->prefix = htons(i->prefix());
	  hnaraw->reserved0 = 0;

	  h_set.push_back(i);
	}
# else
	for (gateset_t::hna_gset_t::iterator i =
	       gate_set.hna_gset().begin(interface);
	     i != gate_set.hna_gset().end(interface);
	     d += ADDRESS_SIZE * 2, ++i) {

	  if (gate_set.hna_gset().stamp(i) == timeval_t::now())
	    break;

	  if (d.size() < ADDRESS_SIZE * 2) {
	    if (gate_set.hna_gset().expired(i,
					    cst::hna_interval,
					    timeval_t::in(cst::hna_interval)))
	      ret = true;
	    break;
	  }

	  if (do_dump_hna)
	    dump_hna << "  " << i->net_addr() << '/' << i->prefix() << '\n';

	  i->net_addr().dump(d.raw());
	  i->netmask_addr().dump((d + ADDRESS_SIZE).raw());

	  h_set.push_back(i);
	}
# endif

	mraw->size = htons(d.raw() - reinterpret_cast<u_int8_t*>(mraw));

	for (hset_t::const_iterator i = h_set.begin();
	     i != h_set.end(); ++i)
	  gate_set.hna_gset().set_stamp(*i);

	if (do_dump_hna)
	  dump_hna << '}' << std::endl;

      } else {

	if (d.size() < _data.size())
	  return true;

	_data.dump(d);
	d += _data.size();
      }

      return ret;
    }

    void
    HNAMessage::parse(const utl::ConstData& d, const Message::header& mh) {
      if (do_dump_hna)
	dump_hna << "HNA from " << mh.originator
		 << " (" << mh.sender << " -> " << mh.receiver << ") M("
		 << mh.mseqnum << ") P(" << mh.pseqnum << ") {\n";

      cproxy_t::linkset_t::iterator	l =
	cproxy.linkset().find(set::Link::make_key(mh.receiver, mh.sender));

      if (l == cproxy.linkset().end() || !l->is_sym()) {

	if (do_dump_hna)
	  dump_hna << "  (ignored since not from symmetric neighbor)\n}"
		   << std::endl;
	return;
      }

# ifdef QOLYESTER_ENABLE_NUMPREFIX
      const HNAMessage::raw*	hraw = reinterpret_cast<const HNAMessage::raw*>(d.raw());

      unsigned	len = d.size() / sizeof (HNAMessage::raw);

      for (unsigned i = 0; i < len; ++i) {
	address_t	net_addr(hraw[i].addr, ADDRESS_SIZE);
	unsigned	prefix = ntohs(hraw[i].prefix);
	if (prefix > ADDRESS_SIZE * 8)
	  prefix = ADDRESS_SIZE * 8;

	if (do_dump_hna)
	  dump_hna << "  " << net_addr << '/' << prefix << '\n';

	hna_set.insert(set::HNAEntry(mh.originator,
				     net_addr,
				     prefix,
				     mh.validity));
      }
# else
      const u_int8_t	(*addrs)[ADDRESS_SIZE] =
	reinterpret_cast<const u_int8_t (*)[ADDRESS_SIZE]>(d.raw());
      unsigned	len = d.size() / ADDRESS_SIZE;

      for (unsigned i = 0; i < len; i += 2) {
	address_t	net_addr(addrs[i], ADDRESS_SIZE);
	unsigned	prefix =
	  address_t::netmask_to_prefix(address_t(addrs[i + 1], ADDRESS_SIZE));

	if (do_dump_hna)
	  dump_hna << "  " << net_addr << '/' << prefix << '\n';

	hna_set.insert(set::HNAEntry(mh.originator,
				     net_addr,
				     prefix,
				     mh.validity));
      }
# endif

      if (do_dump_hna)
	dump_hna << '}' << std::endl;
    }

  } // namespace msg

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_MSG_HNA_HXX
