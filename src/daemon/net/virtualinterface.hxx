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

#ifdef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_NET_VIRTUALINTERFACE_HXX
#  define QOLYESTER_DAEMON_NET_VIRTUALINTERFACE_HXX 1

#  include "sch/events.hh"
#  include "virtualinterface.hh"

namespace olsr {

  namespace net {

    VirtualInterface::VirtualInterface()
      : info_(),
	addr_(info_.addr()),
	prefix_(0),
	events_(),
	sender_(0),
	queue_size_(cst::queue_size)
    {}

    VirtualInterface::VirtualInterface(const sys::VirtualInterfaceInfo& info)
      : info_(info),
	addr_(info_.addr()),
	prefix_(info_.prefix()),
	events_(),
	sender_(0),
	queue_size_(cst::queue_size)
    {}

    VirtualInterface::VirtualInterface(const This& other)
      : Super(other),
	info_(other.info_),
	addr_(info_.addr()),
	prefix_(info_.prefix()),
	events_(),
	sender_(0),
	queue_size_(other.queue_size_)
    {}

    VirtualInterface::~VirtualInterface() {
      debug << "Destroying interface instance {\n";
      for (events_t::iterator i = events_.begin(); i != events_.end();
	   /* ++i elsewhere */) {
	debug << "  Destroying " << (*i)->name() << '\n';
	events_t::iterator tmp = i++;
	scheduler.destroy(*tmp);
      }
      debug << '}' << std::endl;
    }

    unsigned
    VirtualInterface::mtu() const {
      return info_.mtu();
    }

    sch::IOEvent::p_t
    VirtualInterface::recv_p() const {
      return info_.socket().read_p();
    }

    sch::IOEvent::p_t
    VirtualInterface::send_p() const {
      return info_.socket().write_p();
    }

    void
    VirtualInterface::insert_event(sch::IOEvent* e) {
      debug << "Inserting " << e->name() << " into interface" << std::endl;
      events_.insert(e);
    }

    void
    VirtualInterface::erase_event(sch::IOEvent* e) {
      debug << "Erasing " << e->name() << " from interface" << std::endl;
      if (e == sender_)
	sender_ = 0;
      events_.erase(e);
    }

    void
    VirtualInterface::destroy_all_events() {
      while (!events_.empty()) {
	sch::IOEvent*	e = *events_.begin();
	events_.erase(events_.begin());
	scheduler.destroy(e);
      }
    }

    pkt::Packet
    VirtualInterface::receive() const {
// 	std::string		fromf;
//         utl::Data		d = info_.socket().receive(fromf);
      try {
        utl::Data		d = info_.socket().receive();
	address_t		froma(d.raw(), address_t::address_size);
	d += address_t::address_size;
	return pkt::Packet(froma, d);
      } catch (sys::UnixClosedConnection&) {
	terminate_now.set_mark();
	throw std::runtime_error("VirtualInterface::receive(): "
				 "closed connection");
      }
    }

    void
    VirtualInterface::send(const pkt::Packet& p) const {
      try {
	info_.socket().send(p.data());
      } catch (sys::UnixClosedConnection&) {
	terminate_now.set_mark();
	throw std::runtime_error("VirtualInterface::send(): "
				 "closed connection");
      }
    }

    void
    VirtualInterface::shipout(const pkt::Packet& p) {
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
    VirtualInterface::operator<(const This& rhs) const {
      return addr_ < rhs.addr_;
    }

    VirtualInterface&
    VirtualInterface::make_key(const address_t& a) {
      const_cast<address_t&>(dummy_for_find_.addr_) = a;
      return dummy_for_find_;
    }

  } // namespace net

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_NET_VIRTUALINTERFACE_HXX

#endif // QOLYESTER_ENABLE_VIRTUAL
