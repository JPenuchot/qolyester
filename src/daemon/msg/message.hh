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
/// @file   message.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 15:51:39 2004
///
/// @brief  General abstract message declarations
///
/// Essentially, a message can be created and dumped.  The idea is to
/// generate the actual message at dump time and not creation time,
/// allowing for more accurate advertisements.
///
#ifndef QOLYESTER_DAEMON_MSG_MESSAGE_HH
# define QOLYESTER_DAEMON_MSG_MESSAGE_HH 1

// The idea behind the message hierarchy is to make messages
// independent of the packets and allow messages to be aggregated into
// a single packet.

// When a message is created in the local node, no data is prepared
// until the actual dump of the message into a packet.  Messages that
// support partial generation can thus be generated differently
// depending on the space left in the current packet.  To avoid a
// total mess, non-local messages are forwarded unfragmented.

# include <list>

# include "net/ipaddress.hh"
# include "utl/data.hh"
# include "utl/seqnum.hh"
# include "utl/timeval.hh"

namespace olsr {

  namespace msg {

    // The abstract message class

    ///
    /// @class Message
    /// @brief The abstract message class.
    ///

    class Message {
    public:
      struct header;
      ///
      /// The message destructor.
      ///
      virtual ~Message() {}

      // The dump method.  Returns true if the message was partial and
      // immediate additional generation is required to avoid validity
      // timer expiration.

      ///
      /// The virtual dump method, forwarded to the actual message
      /// dump method.
      ///
      virtual bool	dump(utl::Data&, const address_t&) const = 0;

      ///
      /// The sequence number incrementing method.
      ///
      /// This is used for the case when a message is split in two
      /// separate messages.  The second message's sequence number
      /// then needs to be incremented.  FIXME: I don't see why this
      /// should be a virtual method right now...
      ///
      virtual void	inc_seqnum() { ++seqnum; }

      // Parsing routine.
      ///
      /// The parsing routine.
      /// @arg d Data buffer to parse
      /// @arg sender Address of sending interface
      /// @arg receiver Address of receiving interface
      /// @arg pseqnum Packet sequence number
      ///
      /// This is the general message parser, that further dispatches
      /// to specific message parsers if the message's type is known.
      ///
      static inline void	parse(utl::Data d, const address_t& sender,
				      const address_t& receiver,
				      const seqnum_t& pseqnum);

      // Default forward routine.
      ///
      /// The forwarding routine
      /// @arg d Data of the message to forward
      /// @arg mh message header
      ///
      /// This does forwarding for the message.  Actual forwarders for
      /// any known message are called.
      ///
      static inline void	forward(utl::Data& d, const header& mh);

      // Convenience data structure of the message header.
      ///
      /// @struct raw
      /// @brief  Convenience data structure of the message header
      ///
      /// This comes in handy when filling the message's header.
      ///
      struct raw {
	u_int8_t		type;
	u_int8_t		vtime;
	u_int16_t		size;
	u_int8_t		addr[ADDRESS_SIZE];
	u_int8_t		ttl;
	u_int8_t		hopcount;
	u_int16_t		seqnum;
	u_int8_t		data[0];
      };

      struct header {
	header(const address_t& s, const address_t& r, const address_t& o,
	       const timeval_t& v, const seqnum_t& ps, const seqnum_t& ms,
	       const unsigned& hopcount)
	  : sender(s),
	    receiver(r),
	    originator(o),
	    validity(v),
	    pseqnum(ps),
	    mseqnum(ms),
	    hopcount(hopcount)
	{}
	const address_t&	sender;
	const address_t&	receiver;
	const address_t&	originator;
	const timeval_t&	validity;
	const seqnum_t&		pseqnum;
	const seqnum_t&		mseqnum;
	const unsigned&		hopcount;
      };

      static const ::size_t		min_length = sizeof (raw); ///< Minimum length of a message

    protected:
      static utl::Seqnum<u_int16_t>	seqnum; ///< The message sequence number
    };

    // Special message class of unknown or lost message type.  A known
    // message type is lost after the message has been
    // parsed/processed and is queued for forwarding.
    ///
    /// @class UnknownMessage
    /// @brief Unknown message class
    ///
    /// This is a trivial class for the handling of message which type
    /// is unknown.  An unknown message can only be created with
    /// actual data content and dumped to a given buffer.
    ///
    class UnknownMessage : public Message {
    public:
      ///
      /// The constructor
      /// @arg d %Message data
      ///
      inline UnknownMessage(utl::Data& d);

      ///
      /// The destructor
      ///
      virtual ~UnknownMessage() {}

      ///
      /// The dumping method
      /// @arg d Destination data buffer
      /// @return true if the message has to be resent
      ///
      virtual inline bool dump(utl::Data& d, const address_t&) const;

      ///
      /// The sequence number incrementing method
      ///
      virtual void	inc_seqnum() {}

    private:
      utl::Data	_data; ///< Data buffer
    };

  } // namespace msg

  typedef std::list<const msg::Message*>	pending_t;

  ///
  /// @var pending_t pending_messages
  /// @brief The queue of messages to be sent
  ///
  pending_t					pending_messages;

} // namespace olsr

# include "message.hxx"

#endif // ! QOLYESTER_DAEMON_MSG_MESSAGE_HH
