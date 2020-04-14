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

#ifndef QOLYESTER_DAEMON_SET_GATE_HH
# define QOLYESTER_DAEMON_SET_GATE_HH 1

# include <set>
# include "config.hh"
# include "utl/timeval.hh"
# include "utl/set.hh"
# include "net/ipaddress.hh"

namespace olsr {

  namespace set {

    class GateEntry : public utl::MultiStampable<1, address_t> {
      typedef GateEntry				This;
      typedef utl::MultiStampable<1, address_t>	Super;
    public:
      inline GateEntry(const address_t& naddr, const unsigned prefix);

      virtual ~GateEntry() {}

      const address_t&	net_addr()	const { return naddr_; }
# ifndef QOLYESTER_ENABLE_NUMPREFIX
      const address_t&	netmask_addr()	const { return nmask_; }
# endif
      const unsigned&	prefix()	const { return prefix_; }

      bool		operator<(const This& rhs) const {
	if (naddr_ == rhs.naddr_)
	  return prefix_ < rhs.prefix_;
	return naddr_ < rhs.naddr_;
      }
    private:
      const address_t	naddr_;
# ifndef QOLYESTER_ENABLE_NUMPREFIX
      const address_t	nmask_;
# endif
      const unsigned	prefix_;
    };

    class GateSet {
      typedef GateSet					This;
      typedef GateEntry					elem_t;
      typedef std::set<elem_t>				gset_t;
      typedef utl::MSIndex<gset_t, 0, address_t>	idx_gset_t;
    public:
      typedef idx_gset_t				hna_gset_t;
      typedef gset_t::const_iterator			const_iterator;
      typedef const_iterator				iterator;

      hna_gset_t&	hna_gset() { return hna_gset_; }

      inline GateSet();

      iterator		begin() const { return gset_.begin(); }
      iterator		end() const { return gset_.end(); }

      inline std::pair<iterator, bool>	insert(const gset_t::value_type& x);

      inline void	erase(iterator pos);

      inline bool	empty() const;
    private:
      gset_t		gset_;
      idx_gset_t	hna_gset_;
    };

  } // namespace set

  typedef set::GateSet	gateset_t;

} // namespace olsr

# include "gate.hxx"

#endif // ! QOLYESTER_DAEMON_SET_GATE_HH
