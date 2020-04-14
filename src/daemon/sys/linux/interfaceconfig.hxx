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

#include "config.hh"

#ifndef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_LINUX_INTERFACECONFIG_HXX
#  define QOLYESTER_DAEMON_SYS_LINUX_INTERFACECONFIG_HXX 1

#  include <fstream>
#  include <cerrno>

#  include "utl/exception.hh"
#  include "utl/log.hh"

#  include "interfaceconfig.hh"

namespace olsr {

  extern debug_ostream_t	debug;

  namespace sys {

    namespace internal {

      void
      InterfaceConfig::get_and_set(const std::string& path,
				   const std::string& value,
				   std::string& old_value) {
	std::fstream	variable(path.c_str());

	if (variable.fail())
	  throw errnoexcept_t(std::string("Opening of ") + path +
			      " failed: " + strerror(errno), errno);

	char		buffer[256];

	variable.getline(buffer, sizeof buffer);

	old_value.assign(buffer);

	variable.seekp(0);

	variable << value << std::endl;

	debug << "sysctl: " << path << " = " << old_value << " <- "
	      << value << std::endl;

      }

      void
      InterfaceConfig::set(const std::string& path, const std::string& value) {
	std::fstream	variable(path.c_str(), std::ios::out);
	if (variable.fail())
	  throw errnoexcept_t(std::string("Opening of ") + path +
			      " failed: " + strerror(errno), errno);

	variable << value << std::endl;

	debug << "sysctl: " << path << " <- " << value << std::endl;
      }

    } // namespace internal

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_SYS_LINUX_INTERFACECONFIG_HXX

#endif // ! QOLYESTER_ENABLE_VIRTUAL
