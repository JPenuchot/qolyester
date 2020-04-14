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

#ifndef QOLYESTER_DAEMON_ALG_MAINADDROF_HXX
# define QOLYESTER_DAEMON_ALG_MAINADDROF_HXX 1

# include "set/mid.hh"
# include "set/interfaces.hh"

# include "mainaddrof.hh"

namespace olsr {

  extern midset_t	mid_set;
  extern ifaceset_t	iface_set;

  namespace alg {

    // Find the main address given any node's address.
    const address_t& main_addr_of(const address_t& addr) {
# ifdef QOLYESTER_ENABLE_MID
      midset_t::midset_t::iterator	m =
	mid_set.midset().find(set::MIDEntry::make_key(addr));

      if (m != mid_set.midset().end())
	return m->main_addr();

      // If the address belongs to no declared interface, look if it's
      // not ours.
      if (iface_set.find(addr) != iface_set.end())
	return main_addr;

      // If everything else fails, consider the address as being the
      // main address of the node.
# endif // ! QOLYESTER_ENABLE_MID
      return addr;
    }

  } // namespace alg

} // namespace olsr

#endif // !QOLYESTER_DAEMON_ALG_MAINADDROF_HXX
