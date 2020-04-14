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

# include "set/client.hh"

#ifndef QOLYESTER_SWITCH_SCH_EVENTS_HH
# define QOLYESTER_SWITCH_SCH_EVENTS_HH 1

# include <queue>

# include "sch/scheduler.hh"
# include "utl/data.hh"
# include "utl/timeval.hh"

namespace olsr {

  namespace set {

    class Client;

  } // namespace set

  namespace sch {

    class ConnectionAccepter : public IOEvent {
      typedef ConnectionAccepter	This;
      typedef IOEvent			Super;
    public:
      inline ConnectionAccepter();
      virtual ~ConnectionAccepter() {}

      virtual inline void	handle();
    };

    class PacketGenerator : public TimedEvent {
      typedef PacketGenerator	This;
      typedef TimedEvent	Super;
    public:
      inline PacketGenerator(const timeval_t& n, const utl::ConstData d,
			     const address_t& dest);

      virtual ~PacketGenerator() {}

      virtual inline void	handle();
    private:
      utl::ConstData	packet_;
      const address_t	dest_;
    };

    class PacketSender : public IOEvent {
      typedef PacketSender	This;
      typedef IOEvent		Super;
    public:
      inline PacketSender(set::Client*);
      virtual inline void	handle();
    private:
      set::Client*	client_;
    };

    class PacketReceiver : public IOEvent {
      typedef PacketReceiver	This;
      typedef IOEvent		Super;
    public:
      inline PacketReceiver(set::Client* c);
      virtual inline void	handle();
    private:
      set::Client*	client_;
    };

    class SwitchLoopHandler : public LoopHandler {
      typedef SwitchLoopHandler	This;
      typedef LoopHandler		Super;
    public:
      virtual inline bool	operator()() const;
    private:
    };

  } // namespace sch

} // namespace olsr

# include "events.hxx"

#endif // !QOLYESTER_SWITCH_SCH_EVENTS_HH
