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
/// @file   meta.hh
/// @author Ignacy Gawedzki
/// @date   Sun Sep 28 14:35:19 2003
///
/// @brief  Metaprogramming tools.
///

#ifndef QOLYESTER_UTL_META_HH
# define QOLYESTER_UTL_META_HH 1

# include <ostream>

namespace olsr {

  namespace utl {

    class NullType {};

    namespace internal {

    // Metaprogramming utilities go here.

    // Static assertion.  Its goal is to catch assertions about static
    // operands at compile time.
      template <bool assertion> struct static_assert {};
      template <> struct static_assert<true> {
	static inline void	icheck() {}
	static const int	dcheck = 1;
      };

    // Static type equality check.
      template <typename T1, typename T2> struct same_type {};
      template <typename T> struct same_type<T, T> {
	static void		icheck() {}
	static const int	dcheck = 1;
      };

    } // namespace internal

// Utility definitions.
///
/// @def i_static_assert(A)
/// @brief	Static assertion in instruction context.
///
# define i_static_assert(Assertion...) utl::internal::static_assert<(Assertion)>::icheck()

///
/// @def d_static_assert(A)
/// @brief	Static assertion in declaration context.
///
# define d_static_assert(Assertion...) utl::internal::static_assert<(Assertion)>::dcheck

///
/// @def i_same_type(T1, T2)
/// @brief	Static type equality assertion in instruction context.
///
# define i_same_type(Type1, Type2) utl::internal::same_type<Type1, Type2 >::icheck()

///
/// @def d_same_type(T1, T2)
/// @brief	Static type equality assertion in declaration context.
///
# define d_same_type(Type1, Type2) utl::internal::same_type<Type1, Type2 >::dcheck

    inline
    std::ostream& operator<<(std::ostream&, const NullType&);

  } // namespace utl

  typedef utl::NullType	null_t;

} // namespace olsr

# include "meta.hxx"

#endif // ! QOLYESTER_UTL_META_HH
