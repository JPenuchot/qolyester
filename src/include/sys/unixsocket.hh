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
/// @file   unixsocket.hh
/// @author Benoit Bourdin
/// @date   Tue Jun 02 2004
///
/// @brief  Unix Socket implementation
///

#ifndef QOLYESTER_SYS_UNIXSOCKET_HH
# define QOLYESTER_SYS_UNIXSOCKET_HH 1

# include <sys/un.h>

# ifndef UNIX_PATH_MAX
#  define UNIX_PATH_MAX 108
# endif

# include "sch/scheduler.hh"
# include "utl/data.hh"

namespace olsr {

  namespace sys {

# define UNIX_BACKLOG		100

    ///
    /// @class UnixSocket
    /// @brief Unix socket (SOCK_DGRAM only) implementation
    ///
    /// Here's an high-level implementation of unix sockets, for Qolyester's needs;
    /// only UDP is allowed (SOCK_DGRAM), and some special socket options may be
    /// not available.
    /// Anyway, migrating to TCP sockets is a piece of cake (c) : all TCP-only
    /// primitives are already here.

    class UnixClosedConnection {};

    class UnixSocket {
      typedef UnixSocket	This;
      enum _dummy_values { _dummy_val };

      ///
      /// A dummy constructor for a dummy static object
      ///
      UnixSocket(_dummy_values);

      explicit inline UnixSocket(int fd);
    public:

      ///
      /// socket address type (sockaddr structure)
      ///
      typedef struct ::sockaddr_un	sockaddr_t;
      ///
      /// type for filenames
      ///
      typedef std::string		path_t;
      ///
      /// The default constructor, opens only a socket
      ///
      inline UnixSocket();
//       ///
//       /// A constructor to copy an object
//       ///
//       UnixSocket(const This& other);
      ///
      /// The normal constructor
      /// @arg fname filename to bind to
      ///
      inline UnixSocket(const path_t& fname);

      virtual inline ~UnixSocket();

//       ///
//       /// Sets the socket to be blockant, default is to be non-blockant
//       ///
//       void set_blocking() { _non_blocking = false; }

      ///
      /// bind primitive
      /// @arg fname the filename to bind to
      ///
      inline void	bind(const path_t& fname);

      inline void	listen();

      ///
      /// connect primitive
      /// @arg fname the filename to connect to
      ///
      inline void	connect(const path_t& fname);

      inline This	accept(path_t& fname);

      inline void	close();
//       ///
//       /// listen primitive, deprecated because of using SOCK_DGRAM
//       ///
//       void	listen();
//       ///
//       /// accept primitive, deprecated because of using SOCK_DGRAM
//       ///
//       void	accept(const path_t& fname);

      ///
      /// receive primitive, receives a packet
      /// @arg sender returned parameter : source address (filename)
      /// @return all the data received
      ///
//       utl::Data	receive(path_t& sender) const throw (errnoexcept_t);
      inline utl::Data	receive() const;

      ///
      /// send primitive, sends a packet, deprected because of using SOCK_DGRAM
      /// @arg d the data to send
      ///
      inline bool	send(const utl::ConstData& d, int flags =  0) const;
//       ///
//       /// sendto primitive, sends a packet to a specified filename
//       /// @arg d the data to send
//       /// @arg fname the destination
//       ///
//       void	sendto(const utl::ConstData& d, const path_t& fname) const throw (errnoexcept_t);

      inline sch::IOEvent::p_t	read_p() const;
      inline sch::IOEvent::p_t	write_p() const;

      ///
      /// comparating operator, needed for creating a socket set
      /// @arg rhs the other operand
      /// @return if rhs > this object
      ///
      inline bool	operator<(const This& rhs) const;

      ///
      /// accessor to dummy static object
      /// @return a dummy socket
      ///
      static const This&	dummy() { return _dummy; }

      static void	lock() {
	_locked = true;
      }

    private:

      ///
      /// Internal utility function used by constructors.
      ///
      inline void init();

      int		_fd;		///< the socket file descriptor
      bool		_bound;

      static bool	_locked;
      static This	_dummy;		///< a static dummy object
    };

    bool	UnixSocket::_locked = 0;

  } // namespace sys

} // namespace olsr

# include "unixsocket.hxx"

#endif // ! QOLYESTER_SYS_UNIXSOCKET_HH
