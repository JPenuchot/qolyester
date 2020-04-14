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
/// @file   meta.hxx
/// @author Ignacy Gawedzki
/// @date   Sun Sep 28 14:35:19 2003
///
/// @brief  Metaprogramming tools implementations.
///

#ifndef QOLYESTER_UTL_META_HXX
# define QOLYESTER_UTL_META_HXX 1

# include "meta.hh"

namespace olsr {

  namespace utl {

    std::ostream&	operator<<(std::ostream& os, const NullType&)
    {
      return os;
    }

  } // namespace utl

} // namespace olsr

#endif // !QOLYESTER_UTL_META_HXX
