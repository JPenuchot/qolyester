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

# include "msg/message.hh"

#ifndef QOLYESTER_DAEMON_PKT_PACKET_HH
# define QOLYESTER_DAEMON_PKT_PACKET_HH 1

# include <sys/types.h>
# include <map>

# include "net/ipaddress.hh"
# include "utl/data.hh"
# include "utl/seqnum.hh"

namespace olsr {

  namespace pkt {

    // The packet class.

    class Packet {
    public:
      // There are two ways to create a packet:
      // * create a new one for local generation.
      inline Packet(const ::size_t mtu, const address_t& addr);
      // * pack received data into a packet to include network-layer
      //   information (source address, destination address).
      inline Packet(const address_t& addr, const utl::Data& d);

      // Accessors
      ::size_t		size()   const { return _size; }
      const utl::Data&	data()   const { return _data;   }
      const address_t&	sender() const { return _sender; }

      // Modifiers
      // Add a single message.
      inline bool	add_message(const msg::Message& m);

      inline void	close(const address_t& i);

//       // Add as much messages from pm as possible.
//       void	fill(pending_t& pm, pending_t& spm);

      // Forward declaration of parsing static method
      static inline void	parse(utl::Data,
				      const address_t& sender,
				      const address_t& receiver);
    protected:

      // Convenience data structure of the packet header
      struct raw {
	u_int16_t	length;
	u_int16_t	seqnum;
	u_int8_t	data[0];
      };
    public:
      static const ::size_t	min_length = sizeof (raw);
    private:
      ::size_t		_size;
      address_t		_sender;
      utl::Data		_data;
      raw*		_raw;
    protected:
      static std::map<address_t, utl::Seqnum<u_int16_t> >	seqnum_map;
    };

  } // namespace pkt

} // namespace olsr

# include "packet.hxx"

#endif // ! QOLYESTER_DAEMON_PKT_PACKET_HH
