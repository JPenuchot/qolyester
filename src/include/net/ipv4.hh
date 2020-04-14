// Copyright (C) 2003, 2004, 2005, 2006, 2008 Laboratoire de Recherche en Informatique

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
/// @file   ipv4.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 18:55:39 2004
///
/// @brief  IPv4 definitions
///

#ifndef QOLYESTER_IPV4_HH
# define QOLYESTER_IPV4_HH 1

# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <cassert>
# include <cstring>
# include <ostream>
# include <stdexcept>
# include <cerrno>

namespace olsr {

  namespace net {

    // The definition of IPv4 addresses.

    ///
    /// @class IPv4Address
    /// @brief IPv4 address class
    ///
    class IPv4Address {
      typedef IPv4Address	This;
    public:

      ///
      /// @typedef sockaddr_in sockaddr_t;
      /// @brief System socket type
      ///
      typedef sockaddr_in		sockaddr_t;
      ///
      /// @typedef in_addr addr_t
      /// @brief System address type
      ///
      typedef in_addr			addr_t;

      enum {
	address_size  = sizeof (addr_t), ///< Address size in bytes
	proto         = PF_INET,	 ///< Protocol index
	family        = AF_INET,	 ///< Family index
	header_length = 28		 ///< Length of IP header
      };

      ///
      /// Default constructor
      ///
      IPv4Address() {
	_addr.s_addr = 0;
      }

      ///
      /// Low-level constructor
      /// @arg s	Reference to sockaddr_t instance
      ///
      IPv4Address(const sockaddr_t& s)
	: _addr(s.sin_addr)
      {}

      ///
      /// Low-level constructor
      /// @arg a	Reference to addr_t instance
      ///
      IPv4Address(const addr_t& a)
	: _addr(a)
      {}

      ///
      /// Low-level constructor
      /// @arg bytes	Pointer to buffer containing address
      /// @arg len	Length of the buffer
      ///
      IPv4Address(const void* bytes, unsigned len) {
	assert(len == address_size);
	_addr.s_addr = *static_cast<const u_int32_t*>(bytes);
      }

      ///
      /// User-level constructor
      /// @arg s	String containing human-readable address
      ///
      IPv4Address(const std::string& s)
	: _addr(resolve(s)._addr)
      {}

      ///
      /// User-level netmask constructor
      /// @arg prefix	Length of prefix in bits
      ///
      static This	netmask(unsigned prefix) {
	assert(prefix <= 32);
	addr_t		addr;
	u_int8_t*	raw = reinterpret_cast<u_int8_t*>(&addr);

	addr.s_addr = 0;

	unsigned	i;

	for (i = 0; i + 8 < address_size * 8 && i + 8 < prefix; i += 8, ++raw)
	  *raw = 0xff;

	*raw = 0xff << (8 - (prefix - i));
	return addr;
      }

      ///
      /// Raw data accessor
      /// @return	Pointer to the raw buffer
      ///
      const unsigned char*	bytes() const {
	return reinterpret_cast<const unsigned char*>(&_addr.s_addr);
      }

      ///
      /// Utility "to string" converter
      /// @return	String containing human-readable address
      ///
      const std::string to_string() const {
	return std::string(::inet_ntoa(_addr));
      }

      ///
      /// Low-level structure accessor
      /// @arg port	Port number
      /// @return	Instance of sockaddr_t
      ///
      const sockaddr_t make_sockaddr(u_int16_t port) const {
	sockaddr_t	sin;

	memset(&sin, 0, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_addr   = _addr;
	sin.sin_port   = htons(port);

	return sin;
      }

      ///
      /// Dumping method
      /// @arg p	Pointer to destination raw buffer
      ///
      void	dump(void* p) const {
	*static_cast<addr_t*>(p) = _addr;
      }

      ///
      /// Link-local predicate
      /// @return false
      ///
      bool	is_linklocal() const {
	return false;
      }

      static const This	addr_any; ///< Static INADDR_ANY address

      ///
      /// Scope setting routine (does nothing)
      ///
      static void set_scope_index(sockaddr_t&, unsigned) {}

      ///
      /// Prefix computation routine
      /// @arg nm	Netmask address
      /// @return	Length of the prefix
      ///
      static unsigned	netmask_to_prefix(const This& nm) {
	unsigned	prefix = 0;

	if (ntohl(nm._addr.s_addr) == 0xffffffff)
	  return 32;
	else if (ntohl(nm._addr.s_addr) == 0)
	  return 0;

	if ((ntohl(nm._addr.s_addr) & 0xffff0000) == 0xffff0000)
	  prefix += 16;

	if ((ntohl(nm._addr.s_addr) & 0xffffff00) == 0xffffff00)
	  prefix += 8;

	while (prefix < 31)
	  if ((ntohl(nm._addr.s_addr) & (0xffffffff << (31 - prefix))) ==
	      (0xffffffff << (31 - prefix)))
	    ++prefix;
	  else
	    break;

	return prefix;
      }

      ///
      /// Network address computation routine
      /// @arg a	Address of a node
      /// @arg prefix	Length of prefix
      /// @return	Address of the node's network
      ///
      static const This	network(const This& a, unsigned prefix) {
	assert(prefix <= 32);
	This		ret(a);
	unsigned int	addr = ntohl(ret._addr.s_addr);
	unsigned	cprefix = 32 - prefix;
	unsigned	i;

	for (i = 0; i + 8 <= cprefix; i += 8)
	  addr &= ~(0xffU << i);

	for (; i < cprefix; ++i)
	  addr &= ~(1 << i);

	ret._addr.s_addr = htonl(addr);

	return ret;
      }

      ///
      /// Resolving routine
      /// @arg s	String containing human-readable address
      /// @return	Address corresponding to the string
      ///
      static const This	resolve(const std::string& s) {
	::size_t	hstbuflen = 1024;
	::hostent	hostbuf;
	::hostent*	hp;
	int		herr;
	int		ret;

	char*	tmphstbuf = new char[hstbuflen];

	while ((ret = gethostbyname2_r(s.c_str(),
				       AF_INET,
				       &hostbuf,
				       tmphstbuf,
				       hstbuflen,
				       &hp,
				       &herr)) == ERANGE) {
	  hstbuflen *= 2;
	  delete[] tmphstbuf;
	  tmphstbuf = new char[hstbuflen];
	}

	if (ret || hp == NULL) {
	  throw std::runtime_error(std::string("Resolving ") +
				   s +
				   std::string(": ") +
				   std::string(::hstrerror(herr)));
	}

	return IPv4Address(*((::in_addr*)hp->h_addr));
      }

      ///
      /// Less-than comparison method
      /// @arg rhs	RHS operand
      /// @return	true if instance's address is lower than RHS's
      ///
      bool	operator<(const This& rhs) const {
	return ntohl(_addr.s_addr) < ntohl(rhs._addr.s_addr);
      }

      ///
      /// Equality comparison method
      /// @arg rhs	RHS operand
      /// @return	true if both addresses are equal
      ///
      bool	operator==(const This& rhs) const {
	return _addr.s_addr == rhs._addr.s_addr;
      }

      ///
      /// Difference comparison method
      /// @arg rhs	RHS operand
      /// @return	true if addresses are different
      ///
      bool	operator!=(const This& rhs) const {
	return !operator==(rhs);
      }

      ///
      /// Hashing method
      /// @return	Hashed address
      ///
      unsigned long	hash() const {
	return (unsigned long) _addr.s_addr;
      }

    private:
      ::in_addr	_addr; ///< Address data
    };

    const IPv4Address	IPv4Address::addr_any((::in_addr) { INADDR_ANY });

    std::ostream&
    operator << (std::ostream& o, const IPv4Address& a)
    {
      return o << a.to_string();
    }

  } // namespace net

} // namespace olsr

#endif // ! QOLYESTER_IPV4_HH
