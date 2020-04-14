// Copyright (C) 2003-2009 Laboratoire de Recherche en Informatique

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

#include "config.hh"

#ifdef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_VIRTUALINTERFACEINFO_HXX
#  define QOLYESTER_DAEMON_SYS_VIRTUALINTERFACEINFO_HXX 1

#  include <cassert>
#  include <stdexcept>
#  include "utl/data.hh"
#  include "utl/exception.hh"
#  include "virtualinterfaceinfo.hh"

namespace olsr {

  namespace sys {

    VirtualInterfaceInfo::VirtualInterfaceInfo()
      : _name(),
	_addr(),
	_prefix(0),
	_index(0),
	_usecount(0),
	_sock(0)
    {}

    VirtualInterfaceInfo::VirtualInterfaceInfo(const std::string& name,
					       const address_t& addr,
					       const std::string& dname)
	: _name(name),
	  _addr(addr),
	  _prefix(ADDRESS_SIZE * 8),
	  _index(_index_map[name]),
	  _usecount(new unsigned(1)),
	  _sock(new UnixSocket) {
//       std::ostringstream	sname;
//       sname << "/tmp/qolyester-" << getpid() << '.' << name;
//       _sock->bind(sname.str());

      try {
	_sock->connect(dname);

	std::string	sender;

	utl::Data	sd(ADDRESS_SIZE);
	addr.dump(sd.raw());
	_sock->send(sd);

	utl::Data	rd = _sock->receive();

	assert(rd.size() == ADDRESS_SIZE);

	address_t	raddr(rd.raw(), rd.size());

	if (raddr == address_t()) {
	  if (addr == address_t())
	    throw std::runtime_error(name + " has no usable address");
	  else
	    throw std::runtime_error(name + " has no address " +
				     addr.to_string());
	}

	debug << "Received IP: " << raddr << std::endl;
	const_cast<address_t&>(_addr) = raddr;
      } catch (errnoexcept_t& e) {
	_sock->close();
	throw;
      }

      if (_index == 0) {
	if (_name_map.empty())
	  const_cast<unsigned&>(_index) = _index_map[name] = 1;
	else
	  const_cast<unsigned&>(_index) =
	    (_index_map[name] = _name_map.rbegin()->first + 1);
	_name_map[_index] = name;
      }
    }

    VirtualInterfaceInfo::VirtualInterfaceInfo(const This& other)
      : _name(other._name),
	_addr(other._addr),
	_prefix(other._prefix),
	_index(other._index),
	_usecount(other._usecount),
	_sock(other._sock) {
      use();
    }

    VirtualInterfaceInfo::~VirtualInterfaceInfo() {
      if (_usecount != 0) {
	unuse();
	if (*_usecount == 0) {
	  delete _usecount;
	  _sock->close();
	  delete _sock;
	}
      }
    }

    VirtualInterfaceInfo&
    VirtualInterfaceInfo::operator=(const This& other) {
      if (_usecount != 0) {
	unuse();
	if (*_usecount == 0) {
	  delete _usecount;
	  _sock->close();
	  delete _sock;
	}
      }
      const_cast<std::string&>(_name) = other._name;
      const_cast<address_t&>(_addr) = other._addr;
      const_cast<unsigned&>(_prefix) = other._prefix;
      const_cast<unsigned&>(_index) = other._index;
      _usecount = other._usecount;
      _sock = other._sock;
      use();

      return *this;
    }

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_SYS_VIRTUALINTERFACEINFO_HXX

#endif // QOLYESTER_ENABLE_VIRTUAL
