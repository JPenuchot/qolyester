// Copyright (C) 2003-2008 Laboratoire de Recherche en Informatique

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

#ifndef QOLYESTER_SYS_UNIXSOCKET_HXX
# define QOLYESTER_SYS_UNIXSOCKET_HXX 1

# include "unixsocket.hh"

# include <sys/socket.h>
# include <sstream>

# include "cst/constants.hh"
# include "utl/exception.hh"

namespace olsr {

  namespace sys {

    UnixSocket::UnixSocket(_dummy_values)
      : _fd(-1),
	_bound(false)
    {}

    UnixSocket::UnixSocket() {
      init();
    }

    UnixSocket::UnixSocket(int fd)
      : _fd(fd),
	_bound(false)
    {}

    UnixSocket::UnixSocket(const path_t& fname) {
      init();
      bind(fname);
    }

    UnixSocket::~UnixSocket() {
    }

    void
    UnixSocket::bind(const path_t& fname) {
      assert(fname.length() < UNIX_PATH_MAX);
      sockaddr_t sun;
      sun.sun_family = AF_FILE;
      ::strncpy(sun.sun_path, fname.c_str(), UNIX_PATH_MAX);
      sun.sun_path[UNIX_PATH_MAX - 1] = 0;

      if (::bind(_fd, (::sockaddr*) &sun, sizeof sun) < 0) {
	std::stringstream	msg;
	msg << "bind(\""
	    << fname << "\"): "
	    << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }
      _bound = true;
    }

    void
    UnixSocket::listen() {
      if (::listen(_fd, UNIX_BACKLOG) < 0)
	throw errnoexcept_t(std::string("listen(): ") + strerror(errno),
			    errno);
    }

    UnixSocket
    UnixSocket::accept(path_t& fname) {
      ::sockaddr_un	sun;
      sun.sun_family = AF_UNIX;
      sun.sun_path[0] = 0;
      socklen_t		len = sizeof sun;
      int	fd = ::accept(_fd, (::sockaddr*) &sun, &len);
      if (fd < 0)
	throw errnoexcept_t(std::string("accept(): ") + strerror(errno),
			    errno);
      fname = sun.sun_path;
      return This(fd);
    }

    void
    UnixSocket::connect(const path_t& fname) {
      assert(fname.length() < UNIX_PATH_MAX);
      sockaddr_t sun;

      sun.sun_family = AF_UNIX;
      ::strncpy(sun.sun_path, fname.c_str(), UNIX_PATH_MAX);
      sun.sun_path[UNIX_PATH_MAX - 1] = 0;

      if (::connect(_fd, (::sockaddr*) &sun, sizeof sun) < 0) {
	std::stringstream	msg;
	msg << "connect(\""
	    << fname << "\"): "
	    << strerror(errno);
	throw errnoexcept_t(msg.str(), errno);
      }
    }

    void
    UnixSocket::close() {
      if (_fd >= 0 && !_locked) {
	sockaddr_t	sun;
	socklen_t		sun_len = sizeof sun;
	if (_bound)
	  if (getsockname(_fd, (sockaddr*) &sun, &sun_len) < 0)
	    throw std::runtime_error(std::string("getsockname() failed: ") +
				     strerror(errno));
	if (::close(_fd) < 0)
	  throw std::runtime_error(std::string("close() failed: ") +
				   strerror(errno));
	if (_bound && sun_len > sizeof sun.sun_family) {
	  assert(sun.sun_path[0] != 0);
	  if (unlink(sun.sun_path) < 0)
	    throw std::runtime_error(std::string("unlink(\"") +
				     sun.sun_path + "\") failed: " +
				     strerror(errno));
	}
	_bound = false;
	_fd = -1;
      }
    }

    utl::Data
    UnixSocket::receive() const {
      utl::Data		data(VIRTUAL_MTU);

      int		len = ::recv(_fd, data.raw(), data.size(), 0);

      if (len == 0 or (len < 0 and errno == ECONNRESET))
	throw UnixClosedConnection();

      if (len < 0)
	throw errnoexcept_t(std::string("recv(): ") + strerror(errno), errno);

      return data.shrink_to(len);
    }

    bool
    UnixSocket::send(const utl::ConstData& d, int flags) const {
      if (::send(_fd, d.raw(), d.size(), flags) < 0) {
	if (errno == EPIPE)
	  throw UnixClosedConnection();
	if (errno == EAGAIN and flags & MSG_DONTWAIT)
	  return false;
	throw errnoexcept_t(std::string("send(): ") + strerror(errno), errno);
      }
      return true;
    }

    bool
    UnixSocket::operator<(const This& rhs) const { return _fd < rhs._fd; }

    sch::IOEvent::p_t
    UnixSocket::read_p() const {
      return sch::IOEvent::p_t(_fd, POLLIN);
    }

    sch::IOEvent::p_t
    UnixSocket::write_p() const {
      return sch::IOEvent::p_t(_fd, POLLOUT);
    }

    void
    UnixSocket::init() {
      if ((_fd = ::socket(PF_UNIX, SOCK_SEQPACKET, 0)) < 0)
	throw errnoexcept_t(std::string("socket(PF_UNIX, SOCK_SEQPACKET, 0): ") +
			    strerror(errno), errno);
      _bound = false;
    }

  } // namespace sys

} // namespace olsr

#endif // ! QOLYESTER_SYS_UNIXSOCKET_HXX
