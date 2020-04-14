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

#ifndef QOLYESTER_SWITCH_SET_CLIENT_HH
# define QOLYESTER_SWITCH_SET_CLIENT_HH 1

# include <queue>
# include <map>

# include "net/ipaddress.hh"
# include "sch/scheduler.hh"
# include "sys/unixsocket.hh"

namespace olsr {

  namespace sch {

    class PacketSender;

  } // namespace sch

  namespace set {

    class Client {
      typedef Client	This;
      inline Client(const This&);
    public:
      inline Client(const address_t& a, const sys::UnixSocket& s);
      virtual inline ~Client();
      inline void	send_now(const utl::ConstData& d);
      inline void	push(const utl::ConstData& d);
      inline void	send_most();
      inline utl::Data	receive();
      inline size_t	queue_size() const;
      inline sch::IOEvent::p_t	recv_p() const;
      inline sch::IOEvent::p_t	send_p() const;
      const address_t&	address() const { return address_; }
      inline void		insert_event(sch::IOEvent* e);
      inline void		erase_event(sch::IOEvent* e);

      inline void		destroy_all_events();
    private:
      address_t				address_;
      sys::UnixSocket			socket_;
      std::queue<utl::ConstData>	packet_queue_;
      std::set<sch::IOEvent*>		events_;
      sch::PacketSender*		packet_sender_;
    };

  } // namespace set

  typedef std::map<address_t, set::Client*>	clientmap_t;

} // namespace olsr

# include "client.hxx"

#endif // !QOLYESTER_SWITCH_SET_CLIENT_HH
