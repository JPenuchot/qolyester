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

#include "net/interface.hh"
#include "msg/hna.hh"
#include "msg/mid.hh"

#ifndef QOLYESTER_DAEMON_SCH_EVENTS_HH
# define QOLYESTER_DAEMON_SCH_EVENTS_HH 1

# include <queue>
# include "msg/hello.hh"
# include "msg/tc.hh"
# include "pkt/packet.hh"
# include "sch/scheduler.hh"

namespace olsr {

  namespace sch {

    namespace internal {

      template <class M>
      struct sender_traits {};

# define SENDER_TRAITS(Msg, Id) \
      template <> struct sender_traits<msg::Msg ## Message> { \
        static const timeval_t&	interval; \
        static const timeval_t	in_jitter; \
        static const timeval_t&	maxjitter; \
        static const std::string name; \
      }

      SENDER_TRAITS(HELLO, hello);
      SENDER_TRAITS(TC, tc);
      SENDER_TRAITS(MID, mid);
      SENDER_TRAITS(HNA, hna);

# undef SENDER_TRAITS

    } // namespace internal

    template <class M>
    class MessageSender : public JitteredEvent {
      typedef MessageSender<M>	This;
      typedef JitteredEvent	Super;
    public:
      inline MessageSender();
      virtual inline void	handle();
    };

    template <>
    class MessageSender<msg::TCMessage> : public JitteredEvent {
      typedef MessageSender<msg::TCMessage>	This;
      typedef JitteredEvent			Super;
    public:
      inline MessageSender();
      virtual inline void	handle();
    };

    class MessageForwarder : public TimedEvent {
      typedef MessageForwarder	This;
      typedef TimedEvent	Super;
      inline MessageForwarder(const This&);
    public:
      inline MessageForwarder(const timeval_t& n,
			      const msg::UnknownMessage& m);
      inline ~MessageForwarder();
      virtual inline void	handle();
    private:
      msg::UnknownMessage*	message_;
    };

    class StatePrinter : public PeriodicEvent {
      typedef StatePrinter	This;
      typedef PeriodicEvent	Super;
    public:
      inline StatePrinter();
      virtual inline void	handle();
    };

    class PacketSender : public IOEvent {
      typedef PacketSender		This;
      typedef IOEvent			Super;
      typedef std::queue<pkt::Packet>	pqueue_t;
    public:
      inline PacketSender(iface_t* i, const pkt::Packet& p);
      virtual inline void	handle();
      inline void		push_packet(const pkt::Packet& p);
    private:
      iface_t*		iface_;
      pqueue_t		packets_;
    };

    class PacketReceiver : public IOEvent {
      typedef PacketReceiver	This;
      typedef IOEvent		Super;
    public:
      inline PacketReceiver(iface_t* i);
      virtual inline void	handle();
    private:
      iface_t*	iface_;
    };

    class QolyesterLoopHandler : public LoopHandler {
      typedef QolyesterLoopHandler	This;
      typedef LoopHandler		Super;
    public:
      virtual inline bool	operator()() const;
    private:
    };

    inline void	remove_routes();

  } // namespace sch

} // namespace olsr

# include "events.hxx"

#endif // !QOLYESTER_DAEMON_SCH_EVENTS_HH
