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

#ifndef QOLYESTER_DAEMON_MSG_MESSAGE_HXX
# define QOLYESTER_DAEMON_MSG_MESSAGE_HXX 1

# include "utl/vtime.hh"
# include "set/duplicate.hh"
# include "sch/events.hh"
# include "msg/hello.hh"
# include "msg/hna.hh"
# include "msg/mid.hh"
# include "msg/tc.hh"

# include "message.hh"

namespace olsr {

  namespace msg {

    // Message parsing routine.
    void
    Message::parse(utl::Data d,
		   const address_t& sender,
		   const address_t& receiver,
		   const seqnum_t& pseqnum) {

      // Extract the header.
      raw*	h = reinterpret_cast<raw*>(d.raw());

      if (h->ttl == 0)
	return;

      // Extract the originator's address from the header.
      address_t		orig(h->addr, ADDRESS_SIZE);

      // Discard the message if the local node is the originator.
      if (orig == main_addr)
	return;

      // Check the message size, isolate message content and extract
      // validity time.
      assert(d.size() == (u_int16_t)(ntohs(h->size)));

      utl::Data		payload = d + sizeof (raw);
      timeval_t		validity = utl::Vtime(h->vtime).operator timeval_t();
      //      utl::Vtime	validity(h->vtime);

      seqnum_t		seqnum(ntohs(h->seqnum));

      header	mh(sender, receiver, orig, validity,
		   pseqnum, seqnum, h->hopcount);

      // If the message is a HELLO message, parse it right away.
      if (h->type == HELLO_MESSAGE) {
	HELLOMessage::parse(payload, mh);
	return;
      }

      // Check that the message is not a duplicate.
      dupset_t::dupset_t::iterator	x =
	dup_set.dupset().find(set::DuplicateEntry::make_key(orig, seqnum));

      if (x == dup_set.dupset().end()) {
	// The message is not a duplicate.
	// We have to process the message.
	// Dispatch the message to its rightful parser.
	switch (h->type) {
	case HELLO_MESSAGE:
	  assert(HELLO_MESSAGE != HELLO_MESSAGE);
	  break;
	case TC_MESSAGE:
	  TCMessage::parse(payload, mh);
	  break;
	case MID_MESSAGE:
	  MIDMessage::parse(payload, mh);
	  break;
	case HNA_MESSAGE:
	  HNAMessage::parse(payload, mh);
	  break;
	default:
	  debug << "Unknown message type, not parsing" << std::endl;
	  break;
	}

      }

      // Check if we have to forward the message.
      if (x == dup_set.dupset().end() || !x->in_ifaces(receiver)) {
	// We have to consider the message for forwarding.
	// Dispatch to the rightful forwarder.
	switch (h->type) {
	case HELLO_MESSAGE:
	  // WE DON'T FORWARD HELLO MESSAGES
	  // FIXME: This case should never be reached, as we check it
	  // earlier.
	  assert(HELLO_MESSAGE != HELLO_MESSAGE);
	  break;
	case TC_MESSAGE:
	  TCMessage::forward(d, mh);
	  break;
	case MID_MESSAGE:
	  MIDMessage::forward(d, mh);
	  break;
	case HNA_MESSAGE:
	  HNAMessage::forward(d, mh);
	  break;
	default:
	  // Default forwarding algorithm for unknown message types.
	  Message::forward(d, mh);
	  break;
	}
      }
    }

    // Default forwarding algorithm
    void
    Message::forward(utl::Data& d, const header& mh) {
      cproxy_t::linkset_t::iterator	lx =
	cproxy.linkset().find(set::Link::make_key(mh.receiver, mh.sender));

      // Don't forward the message if the originating interface is not
      // part of a symmetric link.
      if (lx == cproxy.linkset().end() || !lx->is_sym())
	return;

      // Extract message header.
      raw*	h = reinterpret_cast<raw*>(d.raw());

      // Look for an entry in the duplicate set with this sequence number.
      dupset_t::dupset_t::iterator	dx =
	dup_set.dupset().find(set::DuplicateEntry::make_key(mh.originator,
							    ntohs(h->seqnum)));

      // If the message is a duplicate that has already been processed
      // on this interface or has already been retransmitted, discard
      // it.
      if (dx != dup_set.dupset().end() &&
	  (dx->retransmitted() || dx->in_ifaces(mh.receiver)))
	return;

      // Get the main address of the originating interface.
      const address_t&		maddr = alg::main_addr_of(mh.sender);

      // Consider for forwarding only if the originating interface
      // belongs to a neighbor that is a MPR-selector.  Also check
      // that the TTL is greater than 1.
      cproxy_t::sym_neighborset_t::iterator	nx =
	cproxy.sym_neighborset().find(set::Neighbor::make_key(maddr));

      bool			retransmit = false;
      if (nx != cproxy.sym_neighborset().end() &&
	  nx->is_mprsel() &&
	  h->ttl > 1)
	retransmit = true;

      // Update the entry in the duplicate set or insert a new one.
      dup_set.insert(set::DuplicateEntry(mh.originator,
					 ntohs(h->seqnum),
					 mh.receiver, retransmit));

      // If the message was not to be forwarded, stop here.
      if (!retransmit)
	return;

      // Decrement the TTL and increment the hop count.
      --h->ttl;
      ++h->hopcount;

      // Schedule the packet to be sent.
      typedef UnknownMessage		um_t;
      typedef sch::MessageForwarder	mf_t;

      scheduler.insert(new mf_t(timeval_t::in_jitter(cst::maxjitter),
				um_t(d)));
    }

    UnknownMessage::UnknownMessage(utl::Data& d) : _data(d) {}

    // Simple dump method for unknown messages.
    bool
    UnknownMessage::dump(utl::Data& d, const address_t&) const {
      if (d.size() < _data.size())
	return true;

      _data.dump(d);
      d += _data.size();
      return false;
    }

  } // namespace msg

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_MSG_MESSAGE_HXX
