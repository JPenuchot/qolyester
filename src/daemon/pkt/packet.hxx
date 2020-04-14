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
// Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_DAEMON_PKT_PACKET_HXX
# define QOLYESTER_DAEMON_PKT_PACKET_HXX 1

# include "alg/mainaddrof.hh"
# include "set/neighbors.hh"

# include "pkt/packet.hh"

namespace olsr {

  extern cproxy_t	cproxy;

  namespace pkt {

    // Packet sequence number
    std::map<address_t, utl::Seqnum<u_int16_t> >
    Packet::seqnum_map = std::map<address_t, utl::Seqnum<u_int16_t> >();

    // Ctor for locally generated packets.  The first parameter is the
    // maximum size of the packet and the second is the interface on
    // which we wish to send it.
    Packet::Packet(const ::size_t mtu, const address_t& addr) :
      _size(sizeof (raw)),
      _sender(addr),
# ifdef QOLYESTER_ENABLE_VIRTUAL
	// we must allocate size to put an IP address in header
      _data(mtu+address_t::address_size),
      _raw()
# else // !QOLYESTER_ENABLE_VIRTUAL
      _data(mtu),
      _raw(reinterpret_cast<raw*>(_data.raw()))
# endif
    {
# ifdef QOLYESTER_ENABLE_VIRTUAL
        // add in header source IP address
      addr.dump(_data.raw());
	// we must modify the size here (to add messages after)
      _size+=address_t::address_size;
      _raw=reinterpret_cast<raw*>((_data+address_t::address_size).raw());
# endif
      assert(mtu >= min_length);
      _raw->seqnum = 0;
    }

    // Ctor for received packets.  The first argument is the sender of
    // the packet and the second is the buffer with the received data.
    Packet::Packet(const address_t& addr, const utl::Data& d) :
      _size(d.size()),
      _sender(addr),
      _data(d),
      _raw(reinterpret_cast<raw*>(_data.raw()))
    {}

    // Simple proxy method to add a message.
    // FIXME: is that really necessary?
    bool
    Packet::add_message(const msg::Message& m) {
      utl::Data	d(_data + _size);
      bool	ret = m.dump(d, _sender);
      _size = _data - d;
      return ret;
    }

    void
    Packet::close(const address_t& i) {
      _data.fit(_size);
# ifdef QOLYESTER_ENABLE_VIRTUAL
	// say in the header that the size is (address_size) less
      _raw->length = htons(_size-address_t::address_size);
# else
      _raw->length = htons(_size);
# endif

      if (_size > min_length)
	_raw->seqnum = htons(seqnum_map[i]++);
    }

    // Packet parsing routine.
    void
    Packet::parse(utl::Data d,
		  const address_t& sender,
		  const address_t& receiver) {
      // Ignore locally generated packets.
      if (alg::main_addr_of(sender) == main_addr)
	return;

      // Check if there is too little data to ever hold any message.
      if (d.size() < Packet::min_length + msg::Message::min_length) {
	warning << "Packet::parse(): Short packet from " << sender
		<< std::endl;
	return;
      }

      // Extract the packet header.
      const raw*	pheader = reinterpret_cast<const raw*>(d.raw());

      // Check if there the actual data size is the same as packet length.
      if ((unsigned short)(ntohs(pheader->length)) != d.size()) {
	warning << "Packet::parse(): Packet length mismatch from " << sender
		<< ' ' << ntohs(pheader->length) << " != " << d.size()
		<< std::endl;
	return;
      }

      seqnum_t	pseqnum(ntohs(pheader->seqnum));

# ifdef QOLYESTER_ENABLE_LINKHYS
      // This is done to decrease link quality on packet loss.

      // The 1-hop neighbor set is updated once the messages have been
      // processed, in order to avoid breaking link symmetry if this
      // packet contains a HELLO message that would increase the
      // quality.
      cproxy_t::linkset_t::iterator	x =
	cproxy.linkset().find(set::Link::make_key(receiver, sender));

      if (x != cproxy.linkset().end())
	x->set_last_seqnum(pseqnum);
# endif

      // Parsing messages

      utl::Data	payload = d + sizeof (raw);

      while (payload.size() > 0) {
	// Extract message header.
	const msg::Message::raw*	mheader =
	  reinterpret_cast<const msg::Message::raw*>(payload.raw());

	// Check that a message header fits.
	if (payload.size() < msg::Message::min_length) {
	  warning << "Packet::parse(): Short message from " << sender
		  << std::endl;
	  break;
	}

	// Check that this message fits.
	if (payload.size() < (unsigned short)(ntohs(mheader->size))) {
	  warning << "Packet::parse(): Truncated message from " << sender
		  << std::endl;
	  break;
	}

	// Check that the message is not funny.
	if ((u_int16_t)(ntohs(mheader->size)) < msg::Message::min_length) {
	  warning << "Packet::parse(): corrupted message header from "
		  << sender << std::endl;
	  break;
	}

	// Parse the message.
	msg::Message::parse(payload.shrink_to(ntohs(mheader->size)),
			    sender, receiver, pseqnum);

	payload += ntohs(mheader->size);
      }

# ifdef QOLYESTER_ENABLE_LINKHYS
      // Try to find the link again, since the current packet could
      // contain a HELLO message.
      if (x == cproxy.linkset().end())
	x = cproxy.linkset().find(set::Link::make_key(receiver, sender));

      if (x != cproxy.linkset().end()) {
	x->set_nexttime(timeval_t::in(x->htime() * cst::hello_grace));

	typedef set::upd::LinkUpdater	lu_t;

	sch::TimedEvent*	e = x->updater();

	assert(e != 0);
	scheduler.erase(e);
	e->set_next(lu_t::min_time(*x));
	scheduler.insert(e);
      }
# endif

    }

  } // namespace pkt

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_PKT_PACKET_HXX
