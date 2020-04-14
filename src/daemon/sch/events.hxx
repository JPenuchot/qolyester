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

#ifndef QOLYESTER_DAEMON_SCH_EVENTS_HXX
# define QOLYESTER_DAEMON_SCH_EVENTS_HXX 1

# include "alg/routingtable.hh"
# include "alg/mprselection.hh"
# include "set/duplicate.hh"
# include "set/hna.hh"
# include "set/interfaces.hh"
# include "set/mid.hh"
# include "set/routes.hh"
# include "set/topology.hh"
# include "set/neighbors.hh"
# include "sys/routing.hh"

# include "events.hh"

namespace olsr {

  extern utl::Mark	mprset_recomp;
  extern utl::Mark	advset_changed;

  extern sch::TimedEvent*	tc_sender;
  extern dupset_t	dup_set;
  extern hnaset_t	hna_set;
  extern midset_t	mid_set;
  extern lrouteset_t	lroute_set;
  extern rrouteset_t	rroute_set;
  extern toposet_t	topo_set;
  extern ifaceset_t	iface_set;
  extern thnset_t	thn_set;

  extern std::ostream	dump_state;

  namespace sch {

    namespace internal {

# define SENDER_TRAITS(Msg, Id) \
      const timeval_t& sender_traits<msg::Msg ## Message>::interval = \
        cst::Id ## _interval; \
      const timeval_t sender_traits<msg::Msg ## Message>::in_jitter(1000); \
      const timeval_t& sender_traits<msg::Msg ## Message>::maxjitter = \
        cst::maxjitter; \
      const std::string sender_traits<msg::Msg ## Message>::name = #Msg ;

      SENDER_TRAITS(HELLO, hello);
      SENDER_TRAITS(TC, tc);
      SENDER_TRAITS(MID, mid);
      SENDER_TRAITS(HNA, hna);

#undef SENDER_TRAITS

    } // namespace internal

# ifdef DEBUG
    template <class M>
    MessageSender<M>::MessageSender()
      : Super(timeval_t::in_jitter(internal::sender_traits<M>::in_jitter),
	      internal::sender_traits<M>::interval,
	      internal::sender_traits<M>::maxjitter,
	      std::string("MessageSender(") +
	      internal::sender_traits<M>::name + ')')
    {}
# else // !DEBUG
    template <class M>
    MessageSender<M>::MessageSender()
      : Super(timeval_t::in_jitter(internal::sender_traits<M>::in_jitter),
	      internal::sender_traits<M>::interval,
	      internal::sender_traits<M>::maxjitter)
    {}
# endif

    template <class M>
    void
    MessageSender<M>::handle() {
      Super::handle();
      pending_messages.push_back(new M());
    }

# ifdef DEBUG
    MessageSender<msg::TCMessage>::MessageSender()
      : Super(timeval_t::in_jitter(internal::sender_traits<msg::TCMessage>::in_jitter),
	      internal::sender_traits<msg::TCMessage>::interval,
	      internal::sender_traits<msg::TCMessage>::maxjitter,
	      "MessageSender(TC)")
    {}
# else // !DEBUG
    MessageSender<msg::TCMessage>::MessageSender()
      : Super(timeval_t::in_jitter(internal::sender_traits<msg::TCMessage>::in_jitter),
	      internal::sender_traits<msg::TCMessage>::interval,
	      internal::sender_traits<msg::TCMessage>::maxjitter)
    {}
# endif

    void
    MessageSender<msg::TCMessage>::handle() {
      Super::handle();
      if (!cproxy.is_advset_empty() || !cproxy.is_hold_expired())
	pending_messages.push_back(new msg::TCMessage());
    }

# ifdef DEBUG
    MessageForwarder::MessageForwarder(const timeval_t& n,
				       const msg::UnknownMessage& m)
      : Super(n, "MessageForwarder"),
	message_(new msg::UnknownMessage(m))
    {}
# else // !DEBUG
    MessageForwarder::MessageForwarder(const timeval_t& n,
				       const msg::UnknownMessage& m)
      : Super(n),
	message_(new msg::UnknownMessage(m))
    {}
# endif

    MessageForwarder::~MessageForwarder() {
      if (message_ != 0)
	delete message_;
    }

    void
    MessageForwarder::handle() {
      if (message_ != 0) {
	pending_messages.push_back(message_);
	message_ = 0;
      }
      scheduler.destroy(this);
    }

# ifdef DEBUG
    StatePrinter::StatePrinter()
      : Super(timeval_t::now(), cst::dump_interval, "StatePrinter")
    {}
# else // !DEBUG
    StatePrinter::StatePrinter()
      : Super(timeval_t::now(), cst::dump_interval)
    {}
# endif

    // The state printer handler
    void
    StatePrinter::handle() {
      Super::handle();

      dump_state << "STATE {\n"
		 << indent;
      if (cproxy.nset_.empty())
	dump_state << "Neighbors {}\n";
      else {
	dump_state << "Neighbors {\n"
		   << indent;
	for (cproxy_t::nset_t::const_iterator i = cproxy.nset_.begin();
	     i != cproxy.nset_.end(); ++i)
	  dump_state << (i->is_sym()    ? "S " : "A ")
		     << (i->is_mpr()    ? "M " : "_ ")
		     << (i->is_mprsel() ? "s " : "_ ")
		     << i->main_addr() << " (" << i->lset_.size()
		     << " link(s)) "
		     << "s:" << of(i->mprsel_time().diff()) << ' '
		     << "w: " << i->willingness() << '\n';
	dump_state << deindent
		   << "}\n";
      }
      if (cproxy.lset_.empty())
	dump_state << "Links {}\n";
      else {
	dump_state << "Links {\n"
		   << indent;
	for (cproxy_t::lset_t::const_iterator i = cproxy.lset_.begin();
	     i != cproxy.lset_.end(); ++i) {
# ifndef QOLYESTER_ENABLE_LINKHYS
	  dump_state << (i->is_sym() ? "S " :
			 i->asymtime() > timeval_t::now() ?
			 "A " : "L ")
# else // QOLYESTER_ENABLE_LINKHYS
	  dump_state << (!i->losttime().is_past() ? "L " :
			 i->is_sym() ? "S " :
			 i->asymtime() > timeval_t::now() ?
			 "A " : "L ")
# endif
		     << i->local_addr() << " <- " << i->remote_addr() << " ("
		     << i->main_addr() << ")\n        "
		     << "t:" << of(i->time_.diff()) << ' '
		     << "A:" << of(i->asymtime_.diff()) << ' '
		     << "S:" << of(i->symtime_.diff()) << ' '
# ifdef QOLYESTER_ENABLE_LINKHYS
		     << "L:" << of(i->losttime_.diff()) << (i->pending_ ?
							    " P " : "   ")
		     << "n:" << of(i->nexttime_.diff()) << ' '
		     << "Q:" << of(i->quality_)
# endif // !QOLYESTER_ENABLE_LINKHYS
		     << '\n';
	}
	dump_state << deindent
		   << "}\n";
      }
      if (thn_set.tset_.empty())
	dump_state << "2-Hop neighbors {}\n";
      else {
	dump_state << "2-Hop neighbors {\n"
		   << indent;
	for (thnset_t::tset_t::const_iterator i = thn_set.tset_.begin();
	     i != thn_set.tset_.end(); ++i)
	  dump_state << i->main_addr() << " -> " << i->twohop_addr()
		     << ' ' << "t:" << of(i->time_.diff()) << '\n';
	dump_state << deindent
		   << "}\n";
      }
      if (topo_set.tset_.empty())
	dump_state << "Topology {}\n";
      else {
	dump_state << "Topology {\n"
		   << indent;
	for (toposet_t::tset_t::const_iterator i = topo_set.tset_.begin();
	     i != topo_set.tset_.end(); ++i)
	  dump_state << i->last_addr() << " -> " << i->dest_addr()
		     << ' ' << "t:" << of(i->time_.diff()) << '\n';
	dump_state << deindent
		   << '}' << std::endl;
      }
# ifdef QOLYESTER_ENABLE_MID
      if (mid_set.mset_.empty())
	dump_state << "MID set {}\n";
      else {
	dump_state << "MID set {\n"
		   << indent;
	for (midset_t::mset_t::const_iterator i = mid_set.mset_.begin();
	     i != mid_set.mset_.end(); ++i)
	  dump_state << i->iface_addr() << " (" << i->main_addr()
		     << ") " << "t:" << of(i->time_.diff()) << '\n';
	dump_state << deindent
		   << "}\n";
      }
# endif // !QOLYESTER_ENABLE_MID
# ifdef QOLYESTER_ENABLE_HNA
      if (hna_set.hset_.empty())
	dump_state << "HNA set {}\n";
      else {
	dump_state << "HNA set {\n"
		   << indent;
	for (hnaset_t::hset_t::const_iterator i = hna_set.hset_.begin();
	     i != hna_set.hset_.end(); ++i)
	  dump_state << i->gw_addr() << " -> "
	       << i->net_addr() << '/' << i->prefix() << ' '
	       << "t:" << of(i->time_.diff()) << '\n';
	dump_state << deindent
		   << "}\n";
      }
# endif // !QOLYESTER_ENABLE_HNA
      if (dup_set.dset_.empty())
	dump_state << up << "Duplicate set {}\n" << down;
      else {
	dump_state << up << "Duplicate set {\n"
		   << indent;
	for (dupset_t::dset_t::const_iterator i = dup_set.dset_.begin();
	     i != dup_set.dset_.end(); ++i) {
	  dump_state << i->addr_ << ' ' << i->seqnum_
		     << (i->retransmitted_ ? " R" : " _") << ' '
		     << i->time_.diff() << " { ";
	  for (set::DuplicateEntry::ifaces_t::const_iterator j =
		 i->ifaces_.begin();
	       j != i->ifaces_.end(); ++j)
	    dump_state << *j << ' ';
	  dump_state << "}\n";
	}
	dump_state << deindent
		   << "}\n" << down;
      }
      if (lroute_set.set_.empty() && rroute_set.set_.empty())
	dump_state << "Route set {}\n";
      else {
	dump_state << "Route set {\n"
		   << indent;
	for (lrouteset_t::set_t::const_iterator i = lroute_set.set_.begin();
	     i != lroute_set.set_.end(); ++i)
	  dump_state << i->dest_addr() << '/' << i->prefix() << " : "
		     << i->interface_info().name() << '\n';
	for (rrouteset_t::set_t::const_iterator i = rroute_set.set_.begin();
	     i != rroute_set.set_.end(); ++i)
	  dump_state << i->dest_addr() << '/' << i->prefix() << " : "
		     << i->next_addr() << '\n';
	dump_state << deindent
		   << '}' << std::endl;
      }
# ifdef DEBUG
      debug << indent;
      sys::RoutingActions().print_kernel_routes();
      debug << deindent;
# endif
      dump_state << up << path_net << '\n' << down;
      dump_state << deindent
		 << '}' << std::endl;
    }

# ifdef DEBUG
    PacketSender::PacketSender(iface_t* i, const pkt::Packet& p)
      : Super(i->send_p(), "PacketSender"),
	iface_(i),
	packets_() {
      packets_.push(p);
      i->insert_event(this);
    }
# else // !DEBUG
    PacketSender::PacketSender(iface_t* i, const pkt::Packet& p)
      : Super(i->send_p()),
	iface_(i),
	packets_() {
      packets_.push(p);
      i->insert_event(this);
    }
# endif

    void
    PacketSender::handle() {
      assert(!packets_.empty());
      p().pfd.revents = 0;
      try {
	iface_->send(packets_.front());
      } catch (errnoexcept_t& e) {
	warning << "PacketSender: " << e.what() << std::endl;
      }
      packets_.pop();
      if (packets_.empty()) {
	debug << up(2) << "PacketSender::handle(): no packets left, destroying"
	      << std::endl << down(2);
	iface_->erase_event(this);
	scheduler.destroy(this);
      } else {
	scheduler.insert(this);
	debug << up(2) << "PacketSender::handle(): " << packets_.size()
	      << " packets left" << std::endl << down(2);
      }
    }

    void
    PacketSender::push_packet(const pkt::Packet& p) {
      while (packets_.size() >= iface_->queue_size()) {
	notice << "PacketSender::push_packet(): queue overflow ("
	       << packets_.size() << "), popping front" << std::endl;
	packets_.pop();
      }
      packets_.push(p);
      debug << up(2) << "PacketSender::push_packet(): "
	    << packets_.size() << " packets left" << std::endl << down(2);
    }

# ifdef DEBUG
    PacketReceiver::PacketReceiver(iface_t* i)
      : Super(i->recv_p(), "PacketReceiver"),
	iface_(i) {
      i->insert_event(this);
    }
# else // !DEBUG
    PacketReceiver::PacketReceiver(iface_t* i)
      : Super(i->recv_p()),
	iface_(i) {
      i->insert_event(this);
    }
# endif

    void
    PacketReceiver::handle() {
      p().pfd.revents = 0;
      scheduler.insert(this);
      try {
	pkt::Packet	p = iface_->receive();
	pkt::Packet::parse(p.data(), p.sender(), iface_->addr());
      } catch (std::runtime_error& e) {
	warning << "PacketReceiver: " << e.what() << std::endl;
      }
    }

    bool
    QolyesterLoopHandler::operator()() const {

      // Here, we process the pending messages.
      if (!pending_messages.empty())
	iface_set.send_messages();

      if (mprset_recomp.mark()) {
	mprset_recomp.reset();
	alg::mprselection();
      }

      // Shift the sending of the next TC message if needed.
      if (advset_changed.mark()) {
	advset_changed.reset();
	assert(tc_sender != 0);
	scheduler.erase(tc_sender);
	tc_sender->set_next(timeval_t::in(timeval_t::jitter(cst::maxjitter)));
	scheduler.insert(tc_sender);
      }

      // Recalculate routes if needed.
      if (routes_recomp.mark()) {
	routes_recomp.reset();
	try {
	  alg::compute_routes();
	} catch (errnoexcept_t& e) {
	  warning << e.what() << std::endl;
	}
      }

      // Cleanly terminate if needed.
      if (terminate_now.mark())
	remove_routes();

      return Super::operator()();
    }

    void remove_routes()
    {
      debug << "Removing all routes" << std::endl;
      alg::flush_routes(lrouteset_t(), rrouteset_t());
    }

  } // namespace sch

} // namespace olsr

#endif // !QOLYESTER_DAEMON_SCH_EVENTS_HXX
