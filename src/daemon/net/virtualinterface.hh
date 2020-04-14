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
/// @file   virtualinterface.hh
/// @author Benoit Bourdin
/// @date   Tue Jun 02 2004
///
/// @brief  Interface object for virtual interfaces
///

#include "config.hh"

#ifdef QOLYESTER_ENABLE_VIRTUAL

# include "sys/virtualinterfaceinfo.hh"
# include "pkt/packet.hh"

# ifndef QOLYESTER_DAEMON_NET_VIRTUALINTERFACE_HH
#  define QOLYESTER_DAEMON_NET_VIRTUALINTERFACE_HH 1

#  include <set>
#  include "utl/stampable.hh"
#  include "sch/scheduler.hh"
#  include "sys/unixsocket.hh"
#  include "net/ipaddress.hh"

namespace olsr {

  namespace sch {

    class PacketSender;

  }

  namespace net {

    ///
    /// @class VirtualInterface
    /// @brief Interface class adapted to virtual networks
    ///
    /// Here is an element of the iface_set, not unlike
    /// the Interface class, but with needed modifications

    class VirtualInterface : public utl::MultiStampable<1, address_t> {
      typedef VirtualInterface			This;
      typedef utl::MultiStampable<1, address_t>	Super;
      typedef std::set<sch::IOEvent*>		events_t;
      ///
      /// The default constructor, only for compatibility
      ///
      explicit inline VirtualInterface();
    public:

      ///
      /// The default constructor
      /// @arg info the system-dependant interface's informations
      ///
      inline VirtualInterface(const sys::VirtualInterfaceInfo& info);
//       ///
//       /// This constructor is only here for compatibility
//       /// @arg info the system-dependant interface's informations
//       ///
//       VirtualInterface(const sys::VirtualInterfaceInfo& info, const address_t&);
      ///
      /// The copy constructor
      /// @arg other the other object to copy
      ///
      inline VirtualInterface(const This& other);

      ///
      /// The default desctructor
      ///
      inline ~VirtualInterface();

      ///
      /// Accessor to the IP address
      /// @return the IP address
      ///
      const address_t&		addr() const { return addr_; }

      ///
      /// Accessor to the MTU
      /// @return the interface's MTU
      ///
      inline unsigned		mtu() const;

      ///
      /// The current prefix
      ///
      unsigned			prefix() const { return prefix_; }

      inline sch::IOEvent::p_t	recv_p() const;

      inline sch::IOEvent::p_t	send_p() const;

      ///
      /// Inserts an IO event to event queue
      /// @arg e the event to insert
      ///
      inline void	insert_event(sch::IOEvent* e);
      ///
      /// Removes an IO event to event queue
      /// @arg e the event to remove
      ///
      inline void	erase_event(sch::IOEvent* e);

      inline void	destroy_all_events();

      ///
      /// Returns system-dependant informations, here containing the
      /// socket object.
      /// @return a VirtualInterfaceInfo object
      ///
      const sys::VirtualInterfaceInfo&	info() const { return info_; }

      ///
      /// Calls receive primitive to wait a packet
      /// @return the packet received
      ///
      inline pkt::Packet	receive() const;
      ///
      /// Calls send primitive to send a packet
      /// @arg p the packet to send
      ///
      inline void		send(const pkt::Packet& p) const;

      inline void		shipout(const pkt::Packet& p);

      unsigned			queue_size() const { return queue_size_; }

      ///
      /// Comparating operator for searching
      /// @arg rhs the object to compare to
      /// @return if rhs > this interface
      ///
      inline bool		operator<(const This& rhs) const;

      ///
      /// Search key builder
      /// @arg a 	Address of interface to search
      /// @return	Reference to modified @link olsr::net::Interface::dummy_for_find_ dummy_for_find_@endlink.
      ///
      static inline This&	make_key(const address_t& a);

      void			configure() {}
      void			unconfigure() {}
    private:
      const sys::VirtualInterfaceInfo	info_;  ///> the interface's infos
      const address_t		addr_;		///> the interface's address
      unsigned			prefix_;	///> the prefix
      events_t			events_;	///> event queue
      sch::PacketSender*	sender_;
      unsigned			queue_size_;

      static This		dummy_for_find_; ///> dummy static object
    };

  } // namespace net

} // namespace olsr

#  include "virtualinterface.hxx"

# endif // ! QOLYESTER_DAEMON_NET_VIRTUALINTERFACE_HH

#endif // QOLYESTER_ENABLE_VIRTUAL
