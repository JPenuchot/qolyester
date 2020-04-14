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

#ifndef QOLYESTER_DAEMON_MSG_MID_HXX
# define QOLYESTER_DAEMON_MSG_MID_HXX 1

# include "set/interfaces.hh"
# include "set/mid.hh"
# include "set/neighbors.hh"
# include "utl/args.hh"

# include "mid.hh"

namespace olsr {

  extern ifaceset_t	iface_set;
  extern midset_t	mid_set;
  extern cproxy_t	cproxy;
  extern std::ostream	dump_mid;

  namespace msg {

    MIDMessage::MIDMessage() : _data() {}

    MIDMessage::MIDMessage(const utl::Data& d, const address_t&) : _data(d) {}

    bool
    MIDMessage::dump(utl::Data& d, const address_t& interface) const {
      bool	ret = false;

      if (_data.empty()) { // generate a MID message

// 	assert(!iface_set.empty());

	if (d.size() < Message::min_length + min_length)
	  return true; // try again immediately

	if (do_dump_mid)
	  dump_mid << "MID preparation (" << interface << ") "
		   << Message::seqnum << " {\n";

	Message::raw*	mraw = reinterpret_cast<Message::raw*>(d.raw());

	d += sizeof *mraw;

	mraw->type     = MID_MESSAGE;
	mraw->vtime    = utl::Vtime(cst::mid_hold_time);
	mraw->size     = 0; // set later
	main_addr.dump(mraw->addr);
	mraw->ttl      = 255;
	mraw->hopcount = 0;
	mraw->seqnum   = htons(Message::seqnum);

	typedef std::list<ifaceset_t::mid_iset_t::iterator>	ipset_t;
	ipset_t			ip_set;

	for (ifaceset_t::mid_iset_t::iterator i =
	       iface_set.mid_iset().begin(interface);
	     i != iface_set.mid_iset().end(interface); ++i) {

	  if (iface_set.mid_iset().stamp(i) == timeval_t::now())
	    break;

	  if (i->addr() == main_addr)
	    continue;

	  if (d.size() < ADDRESS_SIZE) {
	    if (iface_set.mid_iset().expired(i,
					     cst::mid_interval,
					     timeval_t::in(cst::mid_interval)))
	      ret = true;
	    break;
	  }

	  if (do_dump_mid)
	    dump_mid << "  " << i->addr() << '\n';

	  i->addr().dump(d.raw());
	  ip_set.push_back(i);
	  d += ADDRESS_SIZE;
	}

	mraw->size = htons(d.raw() - reinterpret_cast<u_int8_t*>(mraw));

	if (do_dump_mid)
	  dump_mid << '}' << std::endl;

	for (ipset_t::const_iterator i = ip_set.begin();
	     i != ip_set.end(); ++i)
	  iface_set.mid_iset().set_stamp(*i);

      } else {
	if (d.size() < _data.size())
	  return true;

	_data.dump(d);
	d += _data.size();
      }

      return ret;
    }

    void
    MIDMessage::parse(const utl::ConstData& d, const Message::header& mh) {
      if (do_dump_mid)
	dump_mid << "MID from " << mh.originator
		 << " (" << mh.sender << " -> " << mh.receiver << ") M("
		 << mh.mseqnum << ") P(" << mh.pseqnum << ") {\n";

      // The following is the check that the message comes from a
      // symmetric neighbor.  We check against links and neighbors,
      // since we don't know the iface addr <-> main addr association
      // yet.
      cproxy_t::linkset_t::iterator	l =
	cproxy.linkset().find(set::Link::make_key(mh.receiver, mh.sender));

      if (l == cproxy.linkset().end() || !l->is_sym()) {

	if (do_dump_mid)
	  dump_mid << "  (ignored since not from symmetric neighbor)\n}"
		   << std::endl;
	return;
      }

      const u_int8_t	(*addrs)[ADDRESS_SIZE] =
	reinterpret_cast<const u_int8_t (*)[ADDRESS_SIZE]>(d.raw());
      unsigned	len = d.size() / ADDRESS_SIZE;

      for (unsigned i = 0; i < len; ++i) {
	address_t	addr(addrs[i], ADDRESS_SIZE);

	if (do_dump_mid)
	  dump_mid << "  " << addr << '\n';

	mid_set.insert(set::MIDEntry(addr, mh.originator, mh.validity));
      }

      if (do_dump_mid)
	dump_mid << '}' << std::endl;
    }

  } // namespace msg

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_MSG_MID_HXX
