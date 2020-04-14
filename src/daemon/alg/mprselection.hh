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
/// @file   mprselection.hh
/// @author Ignacy Gawedzki
/// @date   Tue Mar 16 14:19:06 2004
///
/// @brief  MPR selection heuristics.
///
/// The algorithm is described in RFC 3626, section 8.3.1.
///

#ifndef QOLYESTER_DAEMON_ALG_MPRSELECTION_HH
# define QOLYESTER_DAEMON_ALG_MPRSELECTION_HH 1

namespace olsr {

  namespace alg {

    // The MPR selection routine.
    inline void mprselection();

  } // namespace alg

} // Namespace olsr

# include "mprselection.hxx"

#endif // !QOLYESTER_DAEMON_ALG_MPRSELECTION_HH
