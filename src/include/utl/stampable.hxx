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

#ifndef QOLYESTER_UTL_STAMPABLE_HXX
# define QOLYESTER_UTL_STAMPABLE_HXX 1

# include "stampable.hh"

namespace olsr {

  namespace utl {

    //
    // Stampable methods
    //
    Stampable::Stampable() : _stamp(0) {}
    
    bool	
    Stampable::expired(const TimeVal& period, const TimeVal& now) const {
      return (_stamp + period).is_past(now);
    }

    //
    // MultiStampable methods
    //
    // Constructor, generalized version
    template <unsigned Dim, class Key, class Compare>
    MultiStampable<Dim, Key, Compare>::MultiStampable()
      : _stampset()
    {}

    // Constructor, specialized version
    template <class Key, class Compare>
    MultiStampable<1, Key, Compare>::MultiStampable()
      : _stampset()
    {}

    // Modifier, generalized version
    template <unsigned Dim, class Key, class Compare>
    void
    MultiStampable<Dim, Key, Compare>::set_stamp(unsigned d, const Key& k,
						 const TimeVal& now) {
      assert(d < Dim);
      std::pair<typename stampset_t::iterator, bool>	p =
	_stampset[d].insert(stampset_t::value_type(k, now));

      if (!p.second)
	p.first->second = now;
    }

    // Modifier, specialized version
    template <class Key, class Compare>
    void
    MultiStampable<1, Key, Compare>::set_stamp(const Key& k,
					       const TimeVal& now) {
      std::pair<typename stampset_t::iterator, bool>	p =
	_stampset.insert(typename stampset_t::value_type(k, now));

      if (!p.second)
	p.first->second = now;
    }

    // Modifier, generalized version
    template <unsigned Dim, class Key, class Compare>
    void
    MultiStampable<Dim, Key, Compare>::remove_stamp(unsigned d, const Key& k) {
      _stampset[d].erase(k);
    }

    // Modifier, specialized version
    template <class Key, class Compare>
    void
    MultiStampable<1, Key, Compare>::remove_stamp(const Key& k) {
      _stampset.erase(k);
    }

    // Accessor, generalized version
    template <unsigned Dim, class Key, class Compare>
    const TimeVal
    MultiStampable<Dim, Key, Compare>::stamp(unsigned d, const Key& k) const {
      assert(d < Dim);
      typename stampset_t::const_iterator	x = _stampset[d].find(k);
      if (x == _stampset[d].end())
	return TimeVal(0);
      return x->second;
    }

    // Accessor, specialized version
    template <class Key, class Compare>
    const TimeVal
    MultiStampable<1, Key, Compare>::stamp(const Key& k) const {
      typename stampset_t::const_iterator	x = _stampset.find(k);
      if (x == _stampset.end())
	return TimeVal(0);
      return x->second;
    }

    // Accessor, generalized version
    template <unsigned Dim, class Key, class Compare>
    bool
    MultiStampable<Dim, Key, Compare>::expired(unsigned d,
					       const Key& k,
					       const TimeVal& period,
					       const TimeVal& now) const {
      assert(d < Dim);
      return (stamp(d, k) + period).is_past(now);
    }

    // Accessor, specialized version
    template <class Key, class Compare>
    bool
    MultiStampable<1, Key, Compare>::expired(const Key& k,
					     const TimeVal& period,
					     const TimeVal& now) const {
      return (stamp(k) + period).is_past(now);
    }

    // Comparator, generalized version
    template <unsigned Dim, class Key, class Compare>
    bool
    MultiStampable<Dim, Key, Compare>::less(unsigned d, const Key& k,
					    const This& rhs) const {
      return stamp(d, k) < rhs.stamp(d, k);
    }

    // Comparator, specialized version
    template <class Key, class Compare>
    bool
    MultiStampable<1, Key, Compare>::less(const Key& k,
					  const This& rhs) const {
      return stamp(k) < rhs.stamp(k);
    }

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_UTL_STAMPABLE_HXX
