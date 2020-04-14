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

#include "config.hh"

#ifndef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_NET_REALINTERFACE_HXX
#  define QOLYESTER_DAEMON_NET_REALINTERFACE_HXX 1

#  include "cst/constants.hh"
#  include "sys/realinterfaceinfo.hh"
#  include "sys/socket.hh"
#  include "sch/events.hh"

#  include "realinterface.hh"

namespace olsr {

  extern sch::Scheduler	scheduler;

  namespace net {

    void
    RealInterface::configure() {
      assert(config_ != 0);
      config_->setup();
    }

    void
    RealInterface::unconfigure() {
      assert(config_ != 0);
      config_->revert();
    }

    RealInterface::RealInterface()
      : info_(),
	addr_(),
	prefix_(0),
#  ifdef QOLYESTER_TWO_SOCKETS
	insock_(sys::Socket::dummy()),
	outsock_(sys::Socket::dummy()),
#  else // !QOLYESTER_TWO_SOCKETS
        sock_(sys::Socket::dummy()),
#  endif
	events_(),
	sender_(0),
	usecount_(new unsigned(1)),
	config_(0),
	queue_size_(cst::queue_size)
    {}

    RealInterface::RealInterface(const sys::RealInterfaceInfo& info)
      : info_(info),
	addr_(info_.get_addr()),
	prefix_(info_.get_prefix(addr_)),
#  ifdef QOLYESTER_TWO_SOCKETS
	insock_(),
	outsock_(),
	events_(),
	sender_(0),
        usecount_(new unsigned(1)),
	config_(new sys::RealInterfaceInfo::config_t(info_.name())),
	queue_size_(cst::queue_size) {
      insock_.set_mtu(info_.mtu() - address_t::header_length);
      outsock_.set_mtu(info_.mtu() - address_t::header_length);

      address_t	bcast = info_.get_bcast(addr_);

#   if QOLYESTER_FAMILY_INET == 6
      insock_.bind_multicast(info_, bcast, OLSR_PORT_NUMBER);
      outsock_.set_multicast(info_);
      if (addr_.is_linklocal())
	outsock_.bind(addr_, OLSR_PORT_NUMBER, info_.index());
      else
	outsock_.bind(addr_, OLSR_PORT_NUMBER);
#   else // QOLYESTER_FAMILY_INET != 6
      //      outsock_.bind(addr_, OLSR_PORT_NUMBER);
      insock_.bind(bcast, OLSR_PORT_NUMBER);
      outsock_.bind_multicast(info_, addr_, OLSR_PORT_NUMBER);
#   endif

      outsock_.set_baddr(bcast);

      outsock_.set_priority();
#  else // !QOLYESTER_TWO_SOCKETS
        sock_(),
        events_(),
	sender_(0),
	usecount_(new unsigned(1)),
	config_(new sys::RealInterfaceInfo::config_t(info_.name())),
	queue_size_(cst::queue_size) {
      address_t	bcast = info_.get_bcast(addr_);
      sock_.set_mtu(info_.mtu() - address_t::header_length);
      sock_.bind_multicast(info_, bcast, OLSR_PORT_NUMBER);
      sock_.set_baddr(bcast);
#  endif
    }

    RealInterface::RealInterface(const sys::RealInterfaceInfo& info,
				 const address_t& addr)
      : info_(info),
        addr_(info_.get_addr(addr)),
        prefix_(info_.get_prefix(addr)),
#  ifdef QOLYESTER_TWO_SOCKETS
        insock_(),
        outsock_(),
        events_(),
	sender_(0),
        usecount_(new unsigned(1)),
	config_(new sys::RealInterfaceInfo::config_t(info_.name())),
	queue_size_(cst::queue_size) {
      insock_.set_mtu(info_.mtu() - address_t::header_length);
      outsock_.set_mtu(info_.mtu() - address_t::header_length);

      address_t	bcast = info_.get_bcast(addr_);

#   if QOLYESTER_FAMILY_INET == 6
      insock_.bind_multicast(info_, bcast, OLSR_PORT_NUMBER);
      outsock_.set_multicast(info_);
      if (addr_.is_linklocal())
	outsock_.bind(addr_, OLSR_PORT_NUMBER, info_.index());
      else
	outsock_.bind(addr_, OLSR_PORT_NUMBER);
#   else // QOLYESTER_FAMILY_INET != 6
      insock_.bind(bcast, OLSR_PORT_NUMBER);
      outsock_.bind_multicast(info_, addr_, OLSR_PORT_NUMBER);
#   endif

      outsock_.set_baddr(bcast);
      outsock_.set_priority();
#  else // !QOLYSTER_TWO_SOCKETS
        sock_(),
        events_(),
	sender_(0),
        usecount_(new unsigned(1)),
	config_(new sys::RealInterfaceInfo::config_t(info_.name())),
	queue_size_(cst::queue_size) {
      address_t	bcast = info_.get_bcast(addr_);
      sock_.set_mtu(info_.mtu() - address_t::header_length);
      sock_.bind_multicast(info_, bcast, OLSR_PORT_NUMBER);
      sock_.set_baddr(bcast);
#  endif
    }

    RealInterface::RealInterface(const This& other)
      : Super(other),
	info_(other.info_),
	addr_(other.addr_),
        prefix_(other.prefix_),
#  ifdef QOLYESTER_TWO_SOCKETS
	insock_(other.insock_),
        outsock_(other.outsock_),
#  else // !QOLYESTER_TWO_SOCKETS
        sock_(other.sock_),
#  endif
        events_(),
	sender_(0),
        usecount_(other.usecount_),
        config_(other.config_),
	queue_size_(other.queue_size_) {
      ++(*usecount_);
    }

    RealInterface::~RealInterface() {
      debug << "Destroying interface instance {\n";
      for (events_t::iterator i = events_.begin(); i != events_.end();
	   /* ++i elsewhere */) {
	debug << "  Destroying " << (*i)->name() << '\n';
	events_t::iterator tmp = i++;
	scheduler.destroy(*tmp);
      }
      debug << '}' << std::endl;
      --(*usecount_);
      if (*usecount_ == 0) {
	delete usecount_;
#  ifdef QOLYESTER_TWO_SOCKETS
	insock_.close();
	outsock_.close();
#  else // !QOLYESTER_TWO_SOCKETS
	sock_.close();
#  endif
	if (config_ != 0) {
	  unconfigure();
	  delete config_;
	}
      }
    }

    unsigned
    RealInterface::mtu() const {
      return info_.mtu() - address_t::header_length;
    }

    sch::IOEvent::p_t
    RealInterface::recv_p() const {
#  ifdef QOLYESTER_TWO_SOCKETS
      return insock_.read_p();
#  else
      return sock_.read_p();
#  endif
    }

    sch::IOEvent::p_t
    RealInterface::send_p() const {
#  ifdef QOLYESTER_TWO_SOCKETS
      return outsock_.write_p();
#  else
      return sock_.write_p();
#  endif
    }

    void
    RealInterface::insert_event(sch::IOEvent* e) {
      debug << "Inserting " << e->name() << " into interface" << std::endl;
      events_.insert(e);
    }

    void
    RealInterface::erase_event(sch::IOEvent* e) {
      debug << "Erasing " << e->name() << " from interface" << std::endl;
      if (e == sender_)
	sender_ = 0;
      events_.erase(e);
    }

    void
    RealInterface::destroy_all_events() {
      while (!events_.empty()) {
	sch::IOEvent*	e = *events_.begin();
	events_.erase(events_.begin());
	if (sender_ == e)
	  sender_ = 0;
	scheduler.destroy(e);
      }
    }

//     const sys::Socket&
//     RealInterface::socket() const {
//       return sock_;
//     }

    pkt::Packet
    RealInterface::receive() const {
      address_t		from;
#  ifdef QOLYESTER_TWO_SOCKETS
      utl::Data	data = insock_.receive(from);
#  else // !QOLYESTER_TWO_SOCKETS
      utl::Data	data = sock_.receive(from);
#  endif
      return pkt::Packet(from, data);
    }

    void
    RealInterface::send(const pkt::Packet& p) const {
#  ifdef QOLYESTER_TWO_SOCKETS
      outsock_.sendto_bcast(p.data());
#  else // !QOLYESTER_TWO_SOCKETS
      sock_.sendto_bcast(p.data());
#  endif
    }

    void
    RealInterface::shipout(const pkt::Packet& p) {
      debug << up << "Shipping out packet of " << p.size()
	    << " bytes through " << info_.name() << std::endl << down;
      if (sender_ == 0) {
	sender_ = new sch::PacketSender(this, p);
	scheduler.insert(sender_);
      } else {
	sender_->push_packet(p);
      }
    }

    bool
    RealInterface::operator<(const This& rhs) const {
      return addr_ < rhs.addr_;
    }

    RealInterface&
    RealInterface::make_key(const address_t& a) {
      const_cast<address_t&>(dummy_for_find_.addr_) = a;
      return dummy_for_find_;
    }

  } // namespace net

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_NET_REALINTERFACE_HXX

#endif // ! QOLYESTER_ENABLE_VIRTUAL
