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

#ifndef QOLYESTER_SWITCH_SET_CLIENT_HXX
# define QOLYESTER_SWITCH_SET_CLIENT_HXX 1

# include "sch/events.hh"

namespace olsr {

  namespace set {

    Client::Client(const address_t& a, const sys::UnixSocket& s)
      : address_(a),
	socket_(s),
	packet_queue_(),
	packet_sender_(new sch::PacketSender(this))
    {}
    Client::~Client() {
      delete packet_sender_;
    }
    void
    Client::send_now(const utl::ConstData& d) {
      socket_.send(d);
    }
    void
    Client::push(const utl::ConstData& d) {
      packet_queue_.push(d);
      debug << up << "Client " << address_ << " pushed one, remaining "
	    << packet_queue_.size() << " packets to send" << std::endl << down;
      if (packet_queue_.size() == 1) {
	scheduler.insert(packet_sender_);
      }
    }
    void
    Client::send_most() {
      unsigned	sent = 0;
      while (!packet_queue_.empty() &&
	     socket_.send(packet_queue_.front(), MSG_DONTWAIT)) {
	++sent;
	packet_queue_.pop();
      }
      debug << up << "Client " << address_ << " sent " << sent
	    << ", remaining " << packet_queue_.size()
	    << " packets to send" << std::endl << down;
    }
    utl::Data
    Client::receive() {
      return socket_.receive();
    }
    size_t
    Client::queue_size() const {
      return packet_queue_.size();
    }
    sch::IOEvent::p_t
    Client::recv_p() const {
      return socket_.read_p();
    }
    sch::IOEvent::p_t
    Client::send_p() const {
      return socket_.write_p();
    }

    void
    Client::insert_event(sch::IOEvent* e) {
      debug << "Inserting " << e->name() << " into client" << std::endl;
      events_.insert(e);
    }

    void
    Client::erase_event(sch::IOEvent* e) {
      debug << "Erasing " << e->name() << " from client" << std::endl;
      events_.erase(e);
    }

    void
    Client::destroy_all_events() {
      while (!events_.empty()) {
	sch::IOEvent*	e = *events_.begin();
	events_.erase(events_.begin());
	scheduler.destroy(e);
      }
      scheduler.erase(packet_sender_);
    }


  } // namespace set

} // namespace olsr

#endif // !QOLYESTER_SWITCH_SET_CLIENT_HXX
