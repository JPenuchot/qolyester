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

#ifndef QOLYESTER_SWITCH_SCH_EVENTS_HXX
# define QOLYESTER_SWITCH_SCH_EVENTS_HXX 1

# include "alg/forward_packet.hh"
# include "sys/unixsocket.hh"
# include "set/client.hh"
# include "set/freeaddr.hh"

# include "events.hh"

namespace olsr {

  extern clientmap_t		client_map;
  extern sys::UnixSocket	switch_socket;
  extern faddrset_t		faddr_set;

  namespace sch {

# ifdef DEBUG
    ConnectionAccepter::ConnectionAccepter()
      : Super(switch_socket.read_p(), "ConnectionAccepter")
    {}
# else // !DEBUG
    ConnectionAccepter::ConnectionAccepter()
      : Super(switch_socket.read_p())
    {}
#endif

    void
    ConnectionAccepter::handle() {
      p().pfd.revents = 0;
      scheduler.insert(this);
      std::string	sender;
      try {
	sys::UnixSocket	s = switch_socket.accept(sender);

	debug << "Accepted connection from \""
	      << sender << "\"" << std::endl;

	utl::Data	rd = s.receive();
	debug << "Received initial packet from \""
	      << sender << "\"" << std::endl;

	faddrset_t::iterator	fx;

	// If the client supplied an address in the request, try to
	// allocate if possible, otherwise allocate the first free
	// address.
	assert(rd.size() == ADDRESS_SIZE);

	address_t	addr(rd.raw(), rd.size());

	if (addr == address_t()) {
	  debug << "Wants default address" << std::endl;
	  fx = faddr_set.begin();
	} else {
	  debug << "Wants address " << addr << std::endl;
	  fx = faddr_set.find(addr);
	}

	// If an address could not be allocated, reply with empty
	// packet, otherwise, reply with the allocated address.
	if (fx == faddr_set.end()) {
	  debug << "Could not allocate address" << std::endl;
	  utl::Data	resp(ADDRESS_SIZE);
	  address_t().dump(resp.raw());
	  try {
	    s.send(resp);
	  } catch (sys::UnixClosedConnection&) {
	    debug << "Client already dead" << std::endl;
	  } catch (errnoexcept_t&) {
	    s.close();
	    throw;
	  }
	  s.close();
	} else {
	  debug << "Allocated address " << *fx << std::endl;
	  utl::Data	resp(ADDRESS_SIZE);
	  fx->dump(resp.raw());
	  try {
	    s.send(resp);
	  } catch (sys::UnixClosedConnection&) {
	    s.close();
	    debug << "Client dead too early" << std::endl;
	    return;
	  } catch (errnoexcept_t&) {
	    s.close();
	    throw;
	  }
	  set::Client*		c = new set::Client(*fx, s);
	  client_map.insert(clientmap_t::value_type(*fx, c));
	  faddr_set.erase(fx);
	  sch::PacketReceiver*	e = new PacketReceiver(c);
	  c->insert_event(e);
	  scheduler.insert(e);
	}
      } catch (std::runtime_error& ex) {
	warning << "ConnectionAccepter: " << ex.what() << std::endl;
      }
    }

# ifdef DEBUG
    PacketGenerator::PacketGenerator(const timeval_t& n,
				     const utl::ConstData d,
				     const address_t& dest)
      : Super(n, "PacketGenerator"),
	packet_(d),
	dest_(dest)
    {}
# else // !DEBUG
    PacketGenerator::PacketGenerator(const timeval_t& n,
				     const utl::ConstData d,
				     const address_t& dest)
      : Super(n),
	packet_(d),
	dest_(dest)
    {}
# endif

    void
    PacketGenerator::handle() {
      clientmap_t::iterator	cx = client_map.find(dest_);
      if (cx != client_map.end()) {
	debug << "Sending delayed packet from "
	      << address_t(packet_.raw(), ADDRESS_SIZE)
	      << " to " << dest_ << std::endl;
 	cx->second->push(packet_);
// 	try {
// 	  cx->second->send_now(packet_);
// 	} catch (sys::UnixClosedConnection&) {
// 	  debug << "Client dead, putting " << dest_
// 		<< " back into the free list" << std::endl;
// 	  faddr_set.insert(dest_);
// 	  set::Client*	c = cx->second;
// 	  client_map.erase(cx);
// 	  c->destroy_all_events();
// 	  delete c;
// 	}
      }
      scheduler.destroy(this);
    }

# ifdef DEBUG
    PacketSender::PacketSender(set::Client* c)
      : Super(c->send_p(), "PacketSender"),
	client_(c)
    {}
# else // !DEBUG
    PacketSender::PacketSender(set::Client* c)
      : Super(c->send_p()),
	client_(c)
    {}
# endif

    void
    PacketSender::handle()
    {
      p().pfd.revents = 0;
      try {
	client_->send_most();
      } catch(sys::UnixClosedConnection&) {
	debug << "Client dead, putting " << client_->address()
	      << " back into the free list" << std::endl;
	faddr_set.insert(client_->address());
	client_map.erase(client_->address());
	client_->destroy_all_events();
	delete client_; // This deletes us ultimately too.
	return;
      }

      if (client_->queue_size() > 0) {
	debug << up << "Queue size > 0, wanting to send again"
	      << std::endl << down;
	scheduler.insert(this);
      } else
	scheduler.erase(this);
    }

# ifdef DEBUG
    PacketReceiver::PacketReceiver(set::Client* c)
      : Super(c->recv_p(), "PacketReceiver"),
	client_(c)
    {}
# else // !DEBUG
    PacketReceiver::PacketReceiver(set::Client* c)
      : Super(c->recv_p()),
	client_(c)
    {}
# endif

    void
    PacketReceiver::handle() {
      p().pfd.revents = 0;
      scheduler.insert(this);
      std::string	sender;
      try {
	utl::Data	rd = utl::Data(client_->receive());

	debug << "Received packet of " << rd.size() << " bytes from "
	      << client_->address() << std::endl;

	alg::forward_packet(client_->address(), rd);
      } catch (sys::UnixClosedConnection&) {
	debug << "Client dead, putting " << client_->address()
	      << " back into the free list" << std::endl;
	faddr_set.insert(client_->address());
	client_map.erase(client_->address());
	client_->destroy_all_events();
	delete client_;
	return;
      }
    }

    bool
    SwitchLoopHandler::operator()() const {
      return Super::operator()();
    }

  } // namespace sch

} // namespace olsr

#endif // !QOLYESTER_SWITCH_SCH_EVENTS_HXX
