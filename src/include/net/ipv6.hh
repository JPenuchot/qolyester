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
/// @file   ipv6.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 18:56:36 2004
///
/// @brief  IPv6 definitions
///

#ifndef QOLYESTER_IPV6_HH
# define QOLYESTER_IPV6_HH 1

# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <cassert>
# include <cstring>
# include <cerrno>
# include <ostream>
# include <stdexcept>

namespace olsr {

  namespace net {

    ///
    /// @class IPv6Address
    /// @brief IPv6 address class
    ///
    class IPv6Address {
      typedef IPv6Address	This;
    public:

      ///
      /// @typedef sockaddr_in6 sockaddr_t;
      /// @brief System socket type
      ///
      typedef sockaddr_in6	sockaddr_t;
      ///
      /// @typedef in6_addr addr_t
      /// @brief System address type
      ///
      typedef in6_addr		addr_t;

      enum {
	address_size  = sizeof (addr_t), ///< Address size in bytes
	proto         = PF_INET6,        ///< Protocol index
	family        = AF_INET6,        ///< Family index
	header_length = 48               ///< Length of IP header
      };

      ///
      /// Default constructor
      ///
      IPv6Address() {
	_addr.s6_addr32[0] = _addr.s6_addr32[1] =
	  _addr.s6_addr32[2] = _addr.s6_addr32[3] = 0;
      }

      ///
      /// Low-level constructor
      /// @arg s	Reference to sockaddr_t instance
      ///
      IPv6Address(const sockaddr_t& s)
	: _addr(s.sin6_addr)
      {}

      ///
      /// Low-level constructor
      /// @arg a	Reference to addr_t instance
      ///
      IPv6Address(const addr_t& a)
	: _addr(a)
      {}

      ///
      /// Low-level constructor
      /// @arg bytes	Pointer to buffer containing address
      /// @arg len	Length of the buffer
      ///
      IPv6Address(const void* bytes, unsigned len) {
	assert(len == address_size);
	memcpy(_addr.s6_addr, bytes, len);
      }

      ///
      /// User-level constructor
      /// @arg s	String containing human-readable address
      ///
      IPv6Address(const std::string& s)
	: _addr(resolve(s)._addr)
      {}

      ///
      /// User-level netmask constructor
      /// @arg prefix	Length of prefix in bits
      ///
      static This	netmask(unsigned prefix) {
	assert(prefix <= 128);

	addr_t	addr;

	for (unsigned i = 0; i < 4; ++i)
	  if (i < prefix / 32)
	    addr.s6_addr32[i] = 0xffffffff;
	  else if (i > prefix / 32)
	    addr.s6_addr32[i] = 0;
	  else if (prefix % 32 == 0) {
	    addr.s6_addr32[i] = 0;
	  }

	if (prefix % 32 == 0)
	  return addr;

	unsigned	index = prefix / 32 * 2;
	for (unsigned i = index; i < index + 2; ++i)
	  if (i < prefix / 16)
	    addr.s6_addr16[i] = 0xffff;
	  else if (i > prefix / 16)
	    addr.s6_addr16[i] = 0;
	  else if (prefix % 16 == 0) {
	    addr.s6_addr16[i] = 0;
	  }

	if (prefix % 16 == 0)
	  return addr;

	index = prefix / 16 * 2;
	for (unsigned i = index; i < index + 2; ++i)
	  if (i < prefix / 8)
	    addr.s6_addr[i] = 0xff;
	  else if (i > prefix / 8)
	    addr.s6_addr[i] = 0;
	  else if (prefix % 8 == 0) {
	    addr.s6_addr[i] = 0;
	  }

	if (prefix % 8 == 0)
	  return addr;

	addr.s6_addr[prefix / 8] = 0xff << (8 - (prefix % 8));
	return addr;
      }

      ///
      /// Raw data accessor
      /// @return	Pointer to the raw buffer
      ///
      const unsigned char*	bytes() const {
	return reinterpret_cast<const unsigned char*>(&_addr.s6_addr);
      }

      ///
      /// Utility "to string" converter
      /// @return	String containing human-readable address
      ///
      const std::string to_string() const {
	char	buffer[INET6_ADDRSTRLEN];
	return std::string(inet_ntop(AF_INET6, &_addr, buffer, sizeof buffer));
      }

      ///
      /// Low-level structure accessor
      /// @arg port	Port number
      /// @return	Instance of sockaddr_t
      ///
      const sockaddr_t make_sockaddr(u_int16_t port) const {
	sockaddr_t	sin6;

	memset(&sin6, 0, sizeof sin6);
	sin6.sin6_family = AF_INET6;
	sin6.sin6_addr   = _addr;
	sin6.sin6_port   = htons(port);

	return sin6;
      }

      ///
      /// Dumping method
      /// @arg p	Pointer to destination raw buffer
      ///
      void dump(void* p) const {
	*static_cast<addr_t*>(p) = _addr;
      }

      ///
      /// Link-local predicate
      /// @return true if the address is link-local, false otherwise
      ///
      bool	is_linklocal() const {
	return IN6_IS_ADDR_LINKLOCAL(&_addr);
      }

      ///
      /// Scope setting routine
      /// @arg sin	Instance of sockaddr_t
      /// @arg index	Index of scope to be set
      ///
      static void set_scope_index(sockaddr_t& sin, unsigned index) {
	sin.sin6_scope_id = index;
      }

      static const This addr_any; ///< Static INADDR6_ANY address

      ///
      /// Network address computation routine
      /// @arg a	Address of a node
      /// @arg prefix	Length of prefix
      /// @return	Address of the node's network
      ///
      static const This network(const This& a, unsigned prefix) {
	assert(prefix <= 128);
	This		ret(a);
	unsigned	cprefix = 128 - prefix;
	unsigned	i;

	for (i = 0; i + 32 <= cprefix; i += 32)
	  ret._addr.s6_addr32[3 - i / 32] = 0;

	if (i < cprefix && i + 16 <= cprefix) {
	  ret._addr.s6_addr16[7 - i / 16] = 0;
	  i += 16;
	}

	if (i < cprefix && i + 8 <= cprefix) {
	  ret._addr.s6_addr[15 - i / 8] = 0;
	  i += 8;
	}

	for (; i < cprefix; ++i)
	  ret._addr.s6_addr[15 - i / 8] &= ~(1 << (i % 8));

	return ret;
      }

      ///
      /// Prefix computation routine
      /// @arg nm	Netmask address
      /// @return	Length of the prefix
      ///
      static unsigned	netmask_to_prefix(const This& nm) {
	unsigned	prefix = 0;
	for (prefix = 0; prefix < 128;)
	  if (nm._addr.s6_addr32[prefix / 32] == 0xffffffff)
	    prefix += 32;
	  else
	    break;

	if (prefix < 128 && nm._addr.s6_addr16[prefix / 16] == 0xffff)
	  prefix += 16;

	if (prefix < 128 && nm._addr.s6_addr[prefix / 8] == 0xff)
	  prefix += 8;

	while (prefix < 128)
	  if (nm._addr.s6_addr[prefix / 8] & (1 << (7 - (prefix % 8))))
	    ++prefix;
	  else
	    break;

	return prefix;
      }

      ///
      /// Resolving routine
      /// @arg s	String containing human-readable address
      /// @return	Address corresponding to the string
      ///
      static const This resolve(const std::string& s) {
	size_t		hstbuflen = 1024;
	hostent		hostbuf;
	hostent*	hp;
	int		herr;
	int		ret;

	char*	tmphstbuf = new char[hstbuflen];

	while ((ret = gethostbyname2_r(s.c_str(),
				       AF_INET6,
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

	return IPv6Address(*((addr_t*)hp->h_addr));
      }

      ///
      /// Less-than comparison method
      /// @arg rhs	RHS operand
      /// @return	true if instance's address is lower than RHS's
      ///
      bool	operator<(const This& rhs) const {
	for (int i = 0; i < 4; ++i)
	  if (ntohl(_addr.s6_addr32[i]) < ntohl(rhs._addr.s6_addr32[i]))
	    return true;
	  else if (ntohl(rhs._addr.s6_addr32[i]) < ntohl(_addr.s6_addr32[i]))
	    break;
	return false;
      }

      ///
      /// Equality comparison method
      /// @arg rhs	RHS operand
      /// @return	true if both addresses are equal
      ///
      bool	operator==(const This& rhs) const {
	for (int i = 0; i < 4; ++i)
	  if (_addr.s6_addr32[i] != rhs._addr.s6_addr32[i])
	    return false;
	return true;
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
	return (unsigned long) _addr.s6_addr32[3];
      }

    private:
      in6_addr	_addr; ///< Address data
    };

    const IPv6Address	IPv6Address::addr_any(in6addr_any);

    std::ostream&
    operator << (std::ostream& o, const olsr::net::IPv6Address& a)
    {
      return o << a.to_string();
    }

  } // namespace net

} // namespace olsr

#endif // ! QOLYESTER_IPV6_HH
