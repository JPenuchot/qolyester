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
/// @file   msg/hna.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 17:20:18 2004
///
/// @brief  HNA message declaration
///
/// Implemented in accordance with RFC 3626, section 12.1.
///

#include "msg/message.hh"

#ifndef QOLYESTER_DAEMON_MSG_HNA_HH
# define QOLYESTER_DAEMON_MSG_HNA_HH 1

namespace olsr {

  namespace msg {

    ///
    /// @class HNAMessage
    /// @brief HNA message declaration
    ///
    class HNAMessage : public Message {
      typedef HNAMessage	This;
      typedef Message		Super;
    public:

      ///
      /// The default constructor
      ///
      inline HNAMessage();

      ///
      /// The constructor
      /// @arg d	%Message data buffer
      ///
      inline HNAMessage(const utl::Data& d);

      ///
      /// The destructor
      ///
      virtual ~HNAMessage() {}

      ///
      /// The dumping method
      /// @arg d		Destination data buffer
      /// @arg interface	Source interface address of the packet
      /// @return		true if the message has to be fragmented
      ///
      virtual inline bool	dump(utl::Data&, const address_t&) const;

      ///
      /// The parsing method
      /// @arg d		%Message data buffer
      /// @arg mh		%Message header
      ///
      static inline void	parse(const utl::ConstData&,
				      const Message::header&);

#ifdef QOLYESTER_ENABLE_NUMPREFIX
      static const ::size_t	min_length = ADDRESS_SIZE + 4;
      struct raw {
	u_int8_t	addr[ADDRESS_SIZE];
	u_int16_t	prefix;
	u_int16_t	reserved0;
      };
#else
      static const ::size_t	min_length = ADDRESS_SIZE * 2; ///< Minimum lenght of HNA message
#endif
    private:
      utl::Data			_data; ///< The message data buffer
    };

  } // namespace msg

} // namespace olsr

# include "hna.hxx"

#endif // ! QOLYESTER_DAEMON_MSG_HNA_HH
