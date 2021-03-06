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

// No include protection here to resolve dependency cycles.

# include "config.hh"

# ifndef QOLYESTER_ENABLE_VIRTUAL
#  include "realinterfaceinfo.hh"

namespace olsr {

  typedef sys::RealInterfaceInfo	ifaceinfo_t;

} // namespace olsr

# else
#  include "virtualinterfaceinfo.hh"

namespace olsr {

  typedef sys::VirtualInterfaceInfo	ifaceinfo_t;

} // namespace olsr

# endif

