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

#ifndef QOLYESTER_UTL_COMPARATOR_HH
# define QOLYESTER_UTL_COMPARATOR_HH 1

# include <iterator>

namespace olsr {

  namespace utl {

    template <typename T, class Compare = std::less<T> >
    struct pless {
      bool operator()(const T* const a, const T* const b) const {
	return Compare()(*a, *b);
      }
    };

    template <typename T, class Equal = std::equal_to<T> >
    struct pequal_to {
      bool operator()(const T* const a, const T* const b) const {
	return Equal()(*a, *b);
      }
    };

    template <class Iter,
	      class Compare = 
	      std::less<typename std::iterator_traits<Iter>::value_type> >
    struct iless {
      bool operator()(const Iter& a, const Iter& b) const {
	return Compare()(*a, *b);
      }
    };

    template <class Iter,
	      class Equal =
	      std::equal_to<typename std::iterator_traits<Iter>::value_type> >
    struct iequal_to {
      bool operator()(const Iter& a, const Iter& b) const {
	return Equal()(*a, *b);
      }
    };


  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_UTL_COMPARATOR_HH
