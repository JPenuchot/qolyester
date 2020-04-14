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

# ifndef QOLYESTER_DAEMON_SYS_LINUX_INTERFACECONFIG_HH
#  define QOLYESTER_DAEMON_SYS_LINUX_INTERFACECONFIG_HH 1

#  include <string>

namespace olsr {

  namespace sys {

    namespace internal {

      /*
	ipv?/eth?/accept_redirects = 0

	ipv4/eth?/rp_filter = 0
	ipv4/all/secure_redirects = 0
	ipv4/eth?/secure_redirects = 0
	ipv4/all/send_redirects = 0
	ipv4/eth?/send_redirects = 0
	ipv4/all/shared_media = 0
	ipv4/eth?/shared_media = 0

	ipv6/eth?/accept_ra = 0
	ipv6/eth?/autoconf = 0
	ipv6/eth?/dad_transmits = 0

	The following should not be like this, but separate forwarding
	for each interface seems not to be supported for IPv6.

	ipv4/eth?/forwarding = 1
	ipv6/all/forwarding = 1
      */

      class InterfaceConfig {
      public:
	InterfaceConfig(const std::string& name)
	  : _name(name),
	    _configured(false)
	{}

	void setup() {
	  if (_configured)
	    return;

#  if QOLYESTER_FAMILY_INET == 4
	  std::string	prefix("/proc/sys/net/ipv4/conf/");
#  elif QOLYESTER_FAMILY_INET == 6
	  std::string	prefix("/proc/sys/net/ipv6/conf/");
#  endif

#  define SET(Name, Value) \
  do { \
    get_and_set(prefix + _name + "/" #Name, #Value, _ ## Name); \
  } while (0)

#  define SET_ALL(Name, Value) \
  do { \
    get_and_set(prefix + "all/" #Name, #Value, _all_ ## Name); \
  } while (0)

	  SET(accept_redirects, 0);
#  if QOLYESTER_FAMILY_INET == 4
	  SET(forwarding, 1);
	  SET(rp_filter, 0);
	  SET_ALL(secure_redirects, 0);
	  SET(secure_redirects, 0);
	  SET_ALL(send_redirects, 0);
	  SET(send_redirects, 0);
	  SET_ALL(shared_media, 0);
	  SET(shared_media, 0);
#  elif QOLYESTER_FAMILY_INET == 6
	  SET_ALL(forwarding, 1);
	  SET(accept_ra, 0);
	  SET(autoconf, 0);
	  SET(dad_transmits, 0);
#  endif

#  undef SET
#  undef SET_ALL
	  _configured = true;
	}

	void revert() {
	  if (!_configured)
	    return;

#  if QOLYESTER_FAMILY_INET == 4
	  std::string	prefix("/proc/sys/net/ipv4/conf/");
#  elif QOLYESTER_FAMILY_INET == 6
	  std::string	prefix("/proc/sys/net/ipv6/conf/");
#  endif

#  define RESET(Name) \
  do { \
    set(prefix + _name + "/" #Name, _ ## Name); \
  } while (0)

#  define RESET_ALL(Name) \
  do { \
    set(prefix + "all/" #Name, _all_ ## Name); \
  } while (0)

	  RESET(accept_redirects);

#  if QOLYESTER_FAMILY_INET == 4
	  RESET(forwarding);
	  RESET(rp_filter);
	  RESET_ALL(secure_redirects);
	  RESET(secure_redirects);
	  RESET_ALL(send_redirects);
	  RESET(send_redirects);
	  RESET_ALL(shared_media);
	  RESET(shared_media);
#  elif QOLYESTER_FAMILY_INET == 6
	  RESET_ALL(forwarding);
	  RESET(accept_ra);
	  RESET(autoconf);
	  RESET(dad_transmits);
#  endif

#  undef RESET
#  undef RESET_ALL
	  _configured = false;
	}

      private:
	inline void	get_and_set(const std::string& path,
				    const std::string& value,
				    std::string& old_value);

	inline void	set(const std::string& path, const std::string& value);

	std::string	_name;
	bool		_configured;

	std::string	_accept_redirects;

#  if QOLYESTER_FAMILY_INET == 4
	std::string	_forwarding;
	std::string	_rp_filter;
	std::string	_all_secure_redirects;
	std::string	_secure_redirects;
	std::string	_all_send_redirects;
	std::string	_send_redirects;
	std::string	_all_shared_media;
	std::string	_shared_media;
#  else // QOLYESTER_FAMILY_INET != 4
	std::string	_all_forwarding;
	std::string	_accept_ra;
	std::string	_autoconf;
	std::string	_dad_transmits;
#  endif
      };

    } // namespace internal

  } // namespace sys

} // namespace olsr

#  include "interfaceconfig.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_LINUX_INTERFACECONFIG_HH

#endif // ! QOLYESTER_ENABLE_VIRTUAL
