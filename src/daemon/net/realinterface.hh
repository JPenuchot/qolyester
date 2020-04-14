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

///
/// @file   net/realinterface.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 18:17:05 2004
///
/// @brief  Network interface definition (OS-independent version)
///
/// This is the network interface from the core's standpoint.
///

#include "config.hh"

#ifndef QOLYESTER_ENABLE_VIRTUAL

// This include is out of protection to resolve dependency cycles.
# include "sys/realinterfaceinfo.hh"
# include "pkt/packet.hh"

# ifndef QOLYESTER_DAEMON_NET_REALINTERFACE_HH
#  define QOLYESTER_DAEMON_NET_REALINTERFACE_HH 1

#  include <set>

#  include "net/ipaddress.hh"
#  include "sch/scheduler.hh"
#  include "sys/socket.hh"
#  include "utl/stampable.hh"

namespace olsr {

  namespace sch {

    class PacketSender;

  }

  namespace net {

    ///
    /// @class Interface
    /// @brief Core interface class
    ///
    class RealInterface : public utl::MultiStampable<1, address_t> {
      typedef RealInterface			This;
      typedef utl::MultiStampable<1, address_t>	Super;
      typedef std::set<sch::IOEvent*>		events_t;
      ///
      /// The private constructor
      ///
      /// This is defined solely for the initialization of
      /// @link olsr::net::RealInterface::dummy_for_find_ dummy_for_find_@endlink.
      ///
      explicit RealInterface();
    public:

      ///
      /// The constructor
      /// @arg info     System-dependent network interface information
      ///
      inline RealInterface(const sys::RealInterfaceInfo& info);

      inline RealInterface(const sys::RealInterfaceInfo& info,
			   const address_t& addr);

      ///
      /// The copy constructor
      ///
      inline RealInterface(const This& other);

      ///
      /// The destructor
      ///
      inline ~RealInterface();

      ///
      /// Address accessor
      /// @return RealInterface's address
      ///
      const address_t&		addr() const { return addr_; }

      ///
      /// MTU accessor
      /// @return MTU of interface
      ///
      inline unsigned		mtu() const;

      unsigned			prefix() const { return prefix_; }

      inline sch::IOEvent::p_t	recv_p() const;

      inline sch::IOEvent::p_t	send_p() const;

      inline void		insert_event(sch::IOEvent* e);
      inline void		erase_event(sch::IOEvent* e);

      inline void		destroy_all_events();

      ///
      /// Information accessor
      /// @return Reference to the interface's system-dependent information
      ///
      const sys::RealInterfaceInfo&	info() const { return info_; }

      ///
      /// Packet receiver
      /// @return A received packet
      ///
      inline pkt::Packet	receive() const;

      ///
      /// Packet sender
      /// @arg p	Packet to be sent
      ///
      inline void		send(const pkt::Packet& p) const;

      inline void		shipout(const pkt::Packet& p);

      unsigned			queue_size() const { return queue_size_; }

      ///
      /// Less-than comparator
      /// @arg rhs	The RHS operand
      ///
      inline bool		operator<(const This& rhs) const;

      ///
      /// Search key builder
      /// @arg a	Address of interface to search
      /// @return	Reference to modified @link olsr::net::RealInterface::dummy_for_find_ dummy_for_find_@endlink.
      ///
      static inline This&	make_key(const address_t& a);

      inline void		configure();
      inline void		unconfigure();
    private:

      const sys::RealInterfaceInfo	info_; ///< System-dependent information
      const address_t		addr_; ///< RealInterface address
      unsigned			prefix_;
#  ifdef QOLYESTER_TWO_SOCKETS
      sys::Socket		insock_; ///< RealInterface input socket
      sys::Socket		outsock_; ///< RealInterface output socket
#  else // !QOLYESTER_TWO_SOCKETS
      sys::Socket		sock_; ///< RealInterface socket
#  endif

      events_t			events_;
      sch::PacketSender*	sender_;
      unsigned*			usecount_;
      sys::RealInterfaceInfo::config_t*	config_;
      unsigned			queue_size_;

      static This		dummy_for_find_; ///< Static instance for key generation
    };

  } // namespace net

} // namespace olsr

#  include "realinterface.hxx"

# endif // ! QOLYESTER_DAEMON_NET_REALINTERFACE_HH

#endif // ! QOLYESTER_ENABLE_VIRTUAL
