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
// Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_DAEMON_MSG_HELLO_HXX
# define QOLYESTER_DAEMON_MSG_HELLO_HXX 1

# include <ostream>
# include <list>

# include "alg/mainaddrof.hh"
# include "utl/vtime.hh"
# include "set/neighbors.hh"

# include "hello.hh"

namespace olsr {

  extern std::ostream	dump_hello;
  extern bool		do_dump_hello;
  extern cproxy_t	cproxy;
  extern thnset_t	thn_set;

  namespace msg {

    // Dump method for the HELLO message.

    // A few words on this:

    // As HELLO messages can be partial (i.e. not all the link set is
    // advertised at once because of possible lack of space), we have
    // to keep track of the last time a link was advertised on a given
    // interface.  Moreover, we want to include links by order of
    // increasing last time of advertisement, in order to prioritize
    // older -- in terms of advertised time -- links.

    // To achieve these goals, we maintain a special data structure in
    // linkset_t that allows for each interface separately to keep the
    // neighbors sorted in order of inscreasing last advertisement
    // time.  We consider the neighbors and not the links for a simple
    // reason: a link cannot exist by itself, it is always associated
    // to a neighbor -- be it an asymmetrical one -- and we need
    // information about the associated neighbor.  Moreover, one
    // neighbor cannot have more than exactly one link with one local
    // interface.

    bool
    HELLOMessage::dump(utl::Data& d, const address_t& interface) const {

      // Check if there is space left for at least one message.
      if (d.size() < Message::min_length + min_length)
	return true; // try again immediately

      if (do_dump_hello) {
	dump_hello << "HELLO preparation (" << interface << ") "
		   << Message::seqnum << " {\n";
      }

      // Fill Message header fields (raw->size is set at the end)

      Message::raw*	mraw = reinterpret_cast<Message::raw*>(d.raw());

      mraw->type     = HELLO_MESSAGE;			// Message Type
      mraw->vtime    = utl::Vtime(cst::neighb_hold_time);	// VTime
      // mraw->size  = 0;				// set later
      main_addr.dump(mraw->addr);			// Main address
      mraw->ttl      = 1;				// Time To Live
      mraw->hopcount = 0;				// Hop Count
      mraw->seqnum   = htons(Message::seqnum);		// Sequence number

      // Fill HELLOMessage header fields

      raw*		hraw = reinterpret_cast<raw*>(mraw->data);

      hraw->reserved    = 0;
      hraw->htime       = utl::Vtime(cst::hello_interval);
      hraw->willingness = willingness;

      d += sizeof *mraw + sizeof *hraw;

      // We are now ready to build the body of the HELLO message.
      // We must first classify the links by linkcodes.
      // We need one set per linkcode.

      typedef std::pair<address_t,
	                cproxy_t::hello_linkset_t::iterator>	lpair_t;
      typedef std::list<lpair_t>	linklist_t;

      typedef enum {
	nu = NOT_NEIGH << 2 | UNSPEC_LINK,
	na = NOT_NEIGH << 2 | ASYM_LINK,
	nl = NOT_NEIGH << 2 | LOST_LINK,

	su = SYM_NEIGH << 2 | UNSPEC_LINK,
	sa = SYM_NEIGH << 2 | ASYM_LINK,
	ss = SYM_NEIGH << 2 | SYM_LINK,
	sl = SYM_NEIGH << 2 | LOST_LINK,

	mu = MPR_NEIGH << 2 | UNSPEC_LINK,
	ma = MPR_NEIGH << 2 | ASYM_LINK,
	ms = MPR_NEIGH << 2 | SYM_LINK,
	ml = MPR_NEIGH << 2 | LOST_LINK
      }								linkcode_t;

      linklist_t	nu_list;
      linklist_t	na_list;
      linklist_t	nl_list;

      linklist_t	su_list;
      linklist_t	sa_list;
      linklist_t	ss_list;
      linklist_t	sl_list;

      linklist_t	mu_list;
      linklist_t	ma_list;
      linklist_t	ms_list;
      linklist_t	ml_list;

      // Now we have to iterate on the neighbors (using
      // linkset_t::mnset_iterator gives the neighbors in stamp order
      // for the current interface, so we get older stamped first).
      // We also check whether we can add the link.  We check the
      // potential link expiration on the next run to return proper
      // value (true if we need another HELLO message right away or
      // false if we can wait till the next generation.

      ::size_t	bytes_so_far = 0;
      bool	ret          = false;
      bool	just_stamped = false;

      std::set<address_t>	already_advertised;

      debug << up(2) << indent << "Considering links {\n" << indent;

      for (cproxy_t::hello_linkset_t::iterator l =
	     cproxy.hello_linkset().begin(interface);
	   l != cproxy.hello_linkset().end(interface); just_stamped ? l : ++l) {

	just_stamped = false;

	debug << "link to " << l->remote_addr() << '\n';

	// We don't want to advertise links too often either, so we
	// check if this link has just been sent in a previous run of
	// the HELLOMessage::dump() method.  As neighbors are iterated
	// on in increasing stamp time, we can safely stop iterating
	// here if this message has just been stamped.
	if (cproxy.hello_linkset().stamp(l) == timeval_t::now()) {
	  debug << up
		<< "Stopping HELLO advertisement, since remaining already advertised\n"
		<< down;
	  break; // the remaining have just been advertised
	}

	// Check if there is enough space left in the packet.
	if (d.size() - bytes_so_far < ADDRESS_SIZE) {

	  debug << up << "No space left in packet\n" << down;

	  // Check if the refresh interval would expire for this
	  // neighbor if we wait till the next iteration.  If yes,
	  // return true to force immediate additional message
	  // emission.
	  if (cproxy.hello_linkset().expired(l, cst::refresh_interval,
					     timeval_t::in(cst::hello_interval))) {

	    debug << up << "Some links would expire, request additional message\n" << down;

	    ret = true;
	    break;
	  }

	  // If the refresh interval would not expire, continue
	  // checking the following entries.
	  continue;
	}

	// Compute the Link Code
	cproxy_t::neighborset_t::iterator	n =
	  cproxy.neighborset().find(set::Neighbor::make_key(l->main_addr()));

	assert(n != cproxy.neighborset().end());

	int	link_type = 0;

	if (l->local_addr() != interface) {
	  // Check if there are links for this neighbor from this interface
	  std::pair<set::Neighbor::linkset_t::const_iterator,
 	            set::Neighbor::linkset_t::const_iterator>	er =
	    n->find_lifaces(interface);

	  // If there exists at least one link from this interface to
	  // the neighbor, we will advertise it in due time, skip this
	  // link.
	  if (er.first != er.second ||
	      already_advertised.find(n->main_addr()) != already_advertised.end()) {
	    cproxy_t::hello_linkset_t::iterator	tmp = l++;
	    cproxy.hello_linkset().set_stamp(tmp);
	    just_stamped = true;
	    continue;
	  }

	  already_advertised.insert(n->main_addr());

	  link_type = UNSPEC_LINK;
	} else {
# ifdef QOLYESTER_ENABLE_LINKHYS
	  if (!l->losttime().is_past())
	    link_type = LOST_LINK;
	  else if (l->pending()) {
	    debug << up << "Skipped pending link\n" << down;
	    continue;
	  } else
# endif // !QOLYESTER_ENABLE_LINKHYS
	  if (!l->symtime().is_past())
	    link_type = SYM_LINK;
	  else if (!l->asymtime().is_past())
	    link_type = ASYM_LINK;
	  else
	    link_type = LOST_LINK;
	}

	// Get the neighbor type code.
	int	neighbor_type = 0;

	if (n->is_mpr())
	  neighbor_type = MPR_NEIGH;
	else if (n->is_sym())
	  neighbor_type = SYM_NEIGH;
	else
	  neighbor_type = NOT_NEIGH;

	if (do_dump_hello) {
	  dump_hello << "  " << (link_type != UNSPEC_LINK ?
				 l->remote_addr() :
				 n->main_addr()) << ' ';
	  switch (link_type) {
	  case UNSPEC_LINK: dump_hello << "U "; break;
	  case   ASYM_LINK: dump_hello << "A "; break;
	  case    SYM_LINK: dump_hello << "S "; break;
	  case   LOST_LINK: dump_hello << "L "; break;
	  }
	  switch (neighbor_type) {
	  case NOT_NEIGH: dump_hello << "N\n"; break;
	  case SYM_NEIGH: dump_hello << "S\n"; break;
	  case MPR_NEIGH: dump_hello << "M\n"; break;
	  }
	}

	// We use a pointer to the apropriate set, depending on the
	// link code.  We can avoid the indirection here using a
	// preprocessor macro, but the indirection is affordable and
	// preferable to the additional code generated.
	linklist_t*	list_ptr = 0;

	switch (linkcode_t(neighbor_type << 2 | link_type)) {

	case nu: list_ptr = &nu_list; break;
	case na: list_ptr = &na_list; break;
	case nl: list_ptr = &nl_list; break;

	case su: list_ptr = &su_list; break;
	case sa: list_ptr = &sa_list; break;
	case ss: list_ptr = &ss_list; break;
	case sl: list_ptr = &sl_list; break;

	case mu: list_ptr = &mu_list; break;
	case ma: list_ptr = &ma_list; break;
	case ms: list_ptr = &ms_list; break;
	case ml: list_ptr = &ml_list; break;

	  // We want to catch this case and make it understandable.
	default: list_ptr = 0; assert(list_ptr != 0);
	}

	// If this is the first link entry of this link code, we must
	// take into account the link message header that would be
	// added.
	if (list_ptr->empty())
	  // Check for free space.
	  if (d.size() - bytes_so_far <
	      sizeof (linksetraw) + ADDRESS_SIZE) {
	    // If there is not enough room, check if we can delay the
	    // advertisement of this link until the next iteration.
	    if (cproxy.hello_linkset().expired(l, cst::refresh_interval,
					       timeval_t::in(cst::hello_interval)))
	      ret = true;
	    // Same as before, we still can add some links.
	    continue;
	  } else
	    bytes_so_far += sizeof (linksetraw) + ADDRESS_SIZE;
	else
	  bytes_so_far += ADDRESS_SIZE;

	// If there is no link on this interface for the given
	// neighbor (the link code is UNSPEC_LINK), we advertise the
	// neighbor's main address, otherwise, we advertise the remote
	// interface address.
	if (link_type == UNSPEC_LINK)
	  list_ptr->push_back(lpair_t(n->main_addr(), l));
	else
	  list_ptr->push_back(lpair_t(l->remote_addr(), l));
      }

      debug << deindent << '}' << deindent << std::endl << down(2);

      if (do_dump_hello)
	dump_hello << '}' << std::endl;

      // Here comes the macro to generate each link message.  We could
      // replace this with a function, in order to spare code size.

# define PROC_XX_SET(Code)                                                \
      do {                                                                \
        if (Code ## _list.empty())                                        \
          break;                                                          \
                                                                          \
        linksetraw*	lraw = reinterpret_cast<linksetraw*>(d.raw());    \
                                                                          \
        lraw->linkcode = linkcode_t(Code);                                \
        lraw->reserved = 0;                                               \
                                                                          \
        d += sizeof *lraw;                                                \
                                                                          \
        for (linklist_t::iterator i  = Code ## _list.begin();             \
   	                          i != Code ## _list.end();               \
  	     d += ADDRESS_SIZE, ++i) {                                    \
   	  i->first.dump(d.raw());                                         \
   	  cproxy.hello_linkset().set_stamp(i->second);                    \
        }                                                                 \
                                                                          \
        lraw->linkmessagesize = htons(d.raw() -	                          \
				      reinterpret_cast<u_int8_t*>(lraw)); \
      } while (0)

      PROC_XX_SET(nu);
      PROC_XX_SET(na);
      PROC_XX_SET(nl);

      PROC_XX_SET(su);
      PROC_XX_SET(sa);
      PROC_XX_SET(ss);
      PROC_XX_SET(sl);

      PROC_XX_SET(mu);
      PROC_XX_SET(ma);
      PROC_XX_SET(ms);
      PROC_XX_SET(ml);

      mraw->size = htons(d.raw() - reinterpret_cast<u_int8_t*>(mraw));

      return ret;
    }

    // Parsing routine for HELLO messages.
    void HELLOMessage::parse(const utl::ConstData& d,
			     const Message::header& mh) {

      // Extract the message header.
      const raw*	h = reinterpret_cast<const raw*>(d.raw());

      if (do_dump_hello) {
	dump_hello << "HELLO from " << mh.originator
		   << " (" << mh.sender << " -> " << mh.receiver
		   << ") M(" << mh.mseqnum << ") P("
		   << mh.pseqnum << ") Ht(" << (float) utl::Vtime(h->htime) << ") {\n";
      }

      utl::ConstData	payload = d + sizeof (raw);

      int				link_type  = UNSPEC_LINK;
      int				neigh_type = NOT_NEIGH;
      typedef std::set<address_t>	thns_t;
      thns_t				thns;

      // Process the link messages.
      for (const linksetraw* lraw =
	     reinterpret_cast<const linksetraw*>(payload.raw());

	   payload.size() > 0;

	   payload += ntohs(lraw->linkmessagesize),

	     lraw = reinterpret_cast<const linksetraw*>
	     ((lraw->data - sizeof (linksetraw) +
	       ntohs(lraw->linkmessagesize)))) {
	// Extract link message size.
	unsigned	len = (ntohs(lraw->linkmessagesize) -
			       sizeof (linksetraw)) / ADDRESS_SIZE;
	const u_int8_t	(*addrs)[ADDRESS_SIZE] =
	  reinterpret_cast<const u_int8_t (*)[ADDRESS_SIZE]>(lraw->data);

	// Extract link and neighbor codes.
	u_int8_t	ltype = lraw->linkcode & 0x3;
	u_int8_t	ntype = lraw->linkcode >> 2 & 0x03;

	// Process the address list.
	for (unsigned i = 0; i < len; ++i) {
	  // Extract link and main addresses.
	  address_t	addr(addrs[i], ADDRESS_SIZE);
	  address_t	maddr = alg::main_addr_of(addr);

	  if (do_dump_hello) {
	    dump_hello << "  " << addr << ' ';
	    switch (ltype) {
	    case UNSPEC_LINK: dump_hello << "U "; break;
	    case   ASYM_LINK: dump_hello << "A "; break;
	    case    SYM_LINK: dump_hello << "S "; break;
	    case   LOST_LINK: dump_hello << "L "; break;
	    }
	    switch (ntype) {
	    case NOT_NEIGH: dump_hello << "N\n"; break;
	    case SYM_NEIGH: dump_hello << "S\n"; break;
	    case MPR_NEIGH: dump_hello << "M\n"; break;
	    }
	  }

	  if (addr == mh.receiver) {
	    // This is the information about the local node.  We
	    // extract here information about the link state between
	    // the local and the neighbor nodes.
	    link_type  = ltype;
	    neigh_type = ntype;
	  } else if (maddr == main_addr)
	    // This is the local node, as seen on another interface.
	    // Ignore it.
	    continue;
	  else {
	    // This is another node, most probably a 2-hop neighbor.
	    if (addr != maddr)
	      // In case this is not the main address of this 2-hop
	      // node, remove any possible previous information from
	      // the 2-hop neighbor set in case we did not known the
	      // multiple interface association of the 2-hop node at
	      // the time of addition.
	      thn_set.erase(set::TwoHopNeighbor::make_key(mh.originator,
							  addr));
	    if (ntype == SYM_NEIGH || ntype == MPR_NEIGH) {
	      // If the 2-hop node is a symmetric neighbor of the
	      // 1-hop node, add an entry in the 2-hop neighbor set.
	      thns.insert(maddr);
	    } else {
	      // Otherwise, remove any association between the two
	      // nodes from the 2-hop neighbor set.
	      thn_set.erase(set::TwoHopNeighbor::make_key(mh.originator,
							  maddr));
	    }
	  }
	}
      }

      if (do_dump_hello)
	dump_hello << '}' << std::endl;

      // Now that we have processed all the message,
      cproxy_t::neighborset_t::iterator	n =
	cproxy.insert_link(mh, link_type, h->willingness
# ifdef QOLYESTER_ENABLE_LINKHYS
			   , utl::Vtime(h->htime)
# endif
			   ).first;

      if (link_type != UNSPEC_LINK) {
	if (neigh_type == MPR_NEIGH)
	  cproxy.set_mprsel(n, mh.validity);
	else if (neigh_type == SYM_NEIGH)
	  cproxy.unset_mprsel(n);
      }

      if (n->is_sym()) { // process this neighbor for THN

	for (thns_t::const_iterator i = thns.begin(); i != thns.end(); ++i)
	  thn_set.insert(set::TwoHopNeighbor(mh.originator, *i, mh.validity));

      }

    }

  } // namespace msg

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_MSG_HELLO_HXX
