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

#ifndef QOLYESTER_SWITCH_ALG_FORWARD_PACKET_HH
# define QOLYESTER_SWITCH_ALG_FORWARD_PACKET_HH 1

# include "net/ipaddress.hh"
# include "utl/data.hh"

namespace olsr {

  namespace alg {

    inline void forward_packet(const address_t& from, utl::ConstData p);

  } // namespace alg

} // namespace olsr

# include "forward_packet.hxx"

#endif // !QOLYESTER_SWITCH_ALG_FORWARD_PACKET_HH
