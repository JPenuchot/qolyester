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
/// @file   hello.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 16:38:42 2004
///
/// @brief  HELLO message declaration
///
/// A HELLO message can be only generated locally, since no HELLO
/// message should ever be forwarded.
/// Implemented in accordance with RFC 3626, section 6.1.
///

#include "msg/message.hh"

#ifndef QOLYESTER_DAEMON_MSG_HELLO_HH
# define QOLYESTER_DAEMON_MSG_HELLO_HH 1

# include <sys/types.h>
# include "net/ipaddress.hh"
# include "utl/data.hh"

namespace olsr {

  namespace msg {

    // The HELLO message class

    ///
    /// @class HELLOMessage
    /// @brief The HELLO message class
    ///
    class HELLOMessage : public Message {
    public:

      ///
      /// The destructor
      ///
      virtual ~HELLOMessage() {}

      ///
      /// The message dumper
      /// @arg d		Destination data buffer
      /// @arg interface	Source interface address of the packet
      /// @return		true if the message has to be fragmented
      ///
      /// The HELLO message is generated on demand and the returned
      /// flag indicates whether the message has to be regenerated to
      /// avoid remote data expiration.
      ///
      virtual inline bool	dump(utl::Data&, const address_t&) const;

      ///
      /// The message parser
      /// @arg d		%Message data to parse
      /// @arg mh		%Message header
      ///
      static inline void	parse(const utl::ConstData&,
				      const Message::header&);

    protected:
      // Convenience data structure of the HELLO message header.
      ///
      /// @struct raw
      /// @brief Convenience structure of HELLO message header
      ///
      struct raw {
	u_int16_t	reserved;
	u_int8_t	htime;
	u_int8_t	willingness;
	u_int8_t	data[0];
      };

      // Additional convenience data structure of the header of the
      // link message.
      ///
      /// @struct linksetraw
      /// @brief Convenience structure of header of link set portion
      /// of HELLO message
      ///
      struct linksetraw {
	u_int8_t	linkcode;
	u_int8_t	reserved;
	u_int16_t	linkmessagesize;
	u_int8_t	data[0];
      };

      static const ::size_t	min_length = sizeof (raw); ///< Minimum length of HELLO message
    };

  } // namespace msg

} // namespace olsr

# include "hello.hxx"

#endif // ! QOLYESTER_DAEMON_MSG_HELLO_HH
