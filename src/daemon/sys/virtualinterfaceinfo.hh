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
/// @file   virtualinterfaceinfo.hh
/// @author Benoit Bourdin
/// @date   Tue Jun 02 2004
///
/// @brief  Interface informations on a virtual interface
///
/// Here is the main routine of the virtual background, for the client side.

#include "config.hh"

#ifdef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_VIRTUALINTERFACEINFO_HH
#  define QOLYESTER_DAEMON_SYS_VIRTUALINTERFACEINFO_HH 1

#  include <string>
#  include <map>

#  include "cst/constants.hh"
#  include "net/ipaddress.hh"
#  include "sys/unixsocket.hh"

namespace olsr {

  namespace sys {

    ///
    /// @class VirtualInterfaceInfo
    /// @brief InterfaceInfo applied to the virtual interface
    ///
    /// Note that constructing this object is blockant : we don't want Qolyester
    /// to run with a default IP/prefix.

    class VirtualInterfaceInfo {
      typedef VirtualInterfaceInfo	This;
    public:
      ///
      /// The default constructor, using a default interface name.
      ///
      inline VirtualInterfaceInfo();
      ///
      /// The constructor, contains the main routine
      /// @arg name Interface name
      /// @arg addr preferred address (possibly address_t())
      /// @arg dname switch destination socket address
      ///
      inline VirtualInterfaceInfo(const std::string& name,
				  const address_t& addr,
				  const std::string& dname);

      inline VirtualInterfaceInfo(const This& other);

      inline ~VirtualInterfaceInfo();

      inline This&	operator=(const This& other);

      ///
      /// Name accessor
      /// @return	Interface's name.
      ///
      const std::string&	name() const { return _name; }
      ///
      /// Address accessor
      /// @return	Interface's IP address.
      ///
      const address_t&		addr() const { return _addr; }
      ///
      /// MTU accessor
      /// @return	Interface's MTU (maximum transfert unit).
      ///
      unsigned			mtu() const { return VIRTUAL_MTU; }
      ///
      /// Prefix accessor
      /// @return	Interface's prefix.
      ///
      unsigned			prefix() const { return _prefix; }
      ///
      /// Index accessor
      /// @return	Interface's index.
      ///
      unsigned			index() const { return _index; }
      ///
      /// Socket accessor
      /// @return	The Unix Socket to communicate with the switch.
      ///
      const UnixSocket&		socket() const { return *_sock; }

//       ///
//       /// Sets socket's filename of the switch
//       /// @arg f	Switch socket filename.
//       ///
//       static void		set_sock_filename(const std::string& f)
//       	{ _swsockfname = f; _sockfname_is_set = true; }

    private:
      void use() {
	++*_usecount;
      }
      void unuse() {
	assert(*_usecount > 0);
	--*_usecount;
      }
      const std::string	_name; 		///< Interface's name
      const address_t	_addr;		///< Interface's address
      const unsigned	_prefix;	///< Interface's prefix
      const unsigned	_index;		///< Interface's index
      unsigned*		_usecount;
      UnixSocket*	_sock;		///< Our socket

      typedef std::map<unsigned, std::string>	namemap_t;
      typedef std::map<std::string, unsigned>	indexmap_t;

      static namemap_t	_name_map;
      static indexmap_t	_index_map;
    };

    VirtualInterfaceInfo::namemap_t	VirtualInterfaceInfo::_name_map;
    VirtualInterfaceInfo::indexmap_t	VirtualInterfaceInfo::_index_map;

  } // namespace sys

} // namespace olsr

#  include "virtualinterfaceinfo.hxx"

# endif // ! QOLYESTER_DAEMON_SYS_VIRTUALINTERFACEINFO_HH

#endif // QOLYESTER_ENABLE_VIRTUAL
