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

# include "net/interface.hh"

#ifndef QOLYESTER_DAEMON_SET_INTERFACES_HH
# define QOLYESTER_DAEMON_SET_INTERFACES_HH 1

# include <set>
# include "utl/set.hh"
# include "net/ipaddress.hh"

namespace olsr {

  namespace set {

    class InterfaceSet {
      typedef InterfaceSet				This;
      typedef std::set<iface_t>				iset_t;
      typedef utl::MSIndex<iset_t, 0, address_t>	idx_iset_t;
    public:
      typedef idx_iset_t				mid_iset_t;
      typedef iset_t::const_iterator			const_iterator;
      typedef const_iterator				iterator;

      mid_iset_t&	mid_iset() { return _mid_iset; }

      inline InterfaceSet();

      inline iterator	begin() const;
      inline iterator	end() const;
      inline iterator	find(const address_t& iaddr) const;
      inline bool	empty() const;
      inline size_t	size() const;

      unsigned		min_mtu() const { return _min_mtu; }

      inline const iface_t&	operator[](const address_t& iaddr) const;

      inline std::pair<iterator, bool>	insert(const iset_t::value_type& x);
      inline void		erase(iterator pos);

      inline void		send_messages();
    private:
      iset_t		_iset;
      mid_iset_t	_mid_iset;
      unsigned		_min_mtu;
    };

  } // namespace set

  typedef set::InterfaceSet	ifaceset_t;

} // namespace olsr

# include "interfaces.hxx"

#endif // ! QOLYESTER_DAEMON_SET_INTERFACES_HH
