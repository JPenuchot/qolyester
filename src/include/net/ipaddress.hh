// Copyright (C) 2003-2006 Laboratoire de Recherche en Informatique

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301, USA.

///
/// @file   ipaddress.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 18:26:41 2004
///
/// @brief  IP address declarations
///
/// This file is the switching point for IP address family.  Concrete
/// implementation choice conformance is checked using concept
/// checking techniques.
///

#ifndef QOLYESTER_NET_IPADDRESS_HH
# define QOLYESTER_NET_IPADDRESS_HH 1

# include "config.hh"

# if !defined QOLYESTER_FAMILY_INET
#  define QOLYESTER_FAMILY_INET 6
# endif

# if QOLYESTER_FAMILY_INET == 4

#  include "ipv4.hh"

namespace olsr {

  typedef net::IPv4Address	address_t;

} // namespace olsr

# elif QOLYESTER_FAMILY_INET == 6

#  include "ipv6.hh"

namespace olsr {

  typedef net::IPv6Address	address_t;

} // namespace olsr

# endif

namespace olsr {

# define ADDRESS_SIZE	(address_t::address_size)

  address_t		main_addr;

  ///
  /// @namespace olsr::concept_check
  /// @brief Concept checking declarations.
  ///
  /// This is only to ensure that the chosen implementation of IP
  /// addresses is conforming to requirements.  This is done using
  /// declarative concept checking, so no instanciation is actually
  /// performed.
  ///
  namespace concept_check {

    typedef address_t::sockaddr_t	tdcheck0;
    typedef address_t::addr_t		tdcheck1;

    enum {
      echeck0 = address_t::address_size,
      echeck1 = address_t::proto,
      echeck2 = address_t::family
    };

    struct address_check {
    protected:
      address_check() {
	// checking constructors
	address_t	dummy0;
	address_t	dummy1(*(const address_t::addr_t*) 0);
	address_t	dummy2(*(const address_t::sockaddr_t*) 0);
	address_t	dummy3(reinterpret_cast<const u_int8_t*>(""), 0U);
	address_t	dummy4(std::string(""));

	// checking methods
	const std::string (address_t::* pdummy0)() const =
	  &address_t::to_string;

	const address_t::sockaddr_t (address_t::* pdummy1)(u_int16_t) const =
	  &address_t::make_sockaddr;

	void (address_t::* pdummy2)(void*) const = &address_t::dump;

	const address_t*	pdummy3 = &address_t::addr_any;

	const address_t (*pdummy4)(const address_t&, unsigned) =
	  &address_t::network;

	const address_t (*pdummy5)(const std::string&) = &address_t::resolve;

	bool (address_t::* pdummy6)(const address_t&) const =
	  &address_t::operator<;

	bool (address_t::* pdummy7)(const address_t&) const =
	  &address_t::operator==;

	bool (address_t::* pdummy8)(const address_t&) const =
	  &address_t::operator!=;

	unsigned long (address_t::* pdummy9)() const = &address_t::hash;

	void (*pdummy10)(address_t::sockaddr_t&, unsigned) =
	  &address_t::set_scope_index;

	bool (address_t::* pdummy11)() const = &address_t::is_linklocal;

	unsigned (*pdummy12)(const address_t&) = &address_t::netmask_to_prefix;

	const unsigned char* (address_t::* pdummy13)() const =
	  &address_t::bytes;

	address_t (*pdummy14)(unsigned) = &address_t::netmask;

	// Shut the fcsk up GCC
	(void) dummy0;
	(void) dummy1;
	(void) dummy2;
	(void) dummy3;
	(void) dummy4;
	(void) pdummy0;
	(void) pdummy1;
	(void) pdummy2;
	(void) pdummy3;
	(void) pdummy4;
	(void) pdummy5;
	(void) pdummy6;
	(void) pdummy7;
	(void) pdummy8;
	(void) pdummy9;
	(void) pdummy10;
	(void) pdummy11;
	(void) pdummy12;
	(void) pdummy13;
	(void) pdummy14;
      }
    };

  } // concept_check

} // namespace olsr

# if defined __GNUC__ && __GNUC__ >= 3
#  if __GNUC__ >= 4 || __GNUC_MINOR__ >= 2
namespace __gnu_cxx {
#  else
namespace std {
#  endif
# endif

  template <>
  struct hash<olsr::address_t> {
    size_t operator()(const olsr::address_t& a) const {
      return a.hash();
    }
  };

# if defined __GNUC__ && __GNUC__ >= 3
#  if __GNUC__ >= 4 || __GNUC_MINOR__ >= 2
} // namespace __gnu_cxx
#  else
} // namespace std
#  endif
# endif

#endif // ! QOLYESTER_NET_IPADDRESS_HH
