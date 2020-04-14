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

#ifndef QOLYESTER_DAEMON_SET_GATE_HXX
# define QOLYESTER_DAEMON_SET_GATE_HXX 1

# include "gate.hh"

namespace olsr {
  
  namespace set {

# ifdef QOLYESTER_ENABLE_NUMPREFIX
    GateEntry::GateEntry(const address_t& naddr, const unsigned prefix) :
      naddr_(naddr), prefix_(prefix) {}
# else      
    GateEntry::GateEntry(const address_t& naddr, const unsigned prefix) :
      naddr_(naddr), nmask_(address_t::netmask(prefix)), prefix_(prefix) {}
# endif
      
    GateSet::GateSet()
      : gset_(),
	hna_gset_(gset_)
    {}
    
    std::pair<GateSet::iterator, bool>
    GateSet::insert(const gset_t::value_type& x) {
      std::pair<iterator, bool>	p = gset_.insert(x);
      if (p.second)
	hna_gset_.insert(p.first);
      return p;
    }

    void
    GateSet::erase(iterator pos) {
      hna_gset_.erase(pos);
      gset_.erase(pos);
    }

    bool
    GateSet::empty() const {
      return gset_.empty();
    }

  } // namespace set

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_SET_GATE_HXX
