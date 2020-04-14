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

#ifndef QOLYESTER_UTL_LOG_HXX
# define QOLYESTER_UTL_LOG_HXX 1

# include "log.hh"

namespace olsr {

  namespace utl {

    template <typename C, typename T>
    basic_levelbuf<C, T>::basic_levelbuf(unsigned level, Super* other,
					 const unsigned& minlevel)
      : other_(other),
	level_(level),
	baselevel_(level),
	minlevel_(minlevel)
    {}

    template <typename C, typename T>
    typename basic_levelbuf<C, T>::int_type
    basic_levelbuf<C, T>::overflow(int_type c) {
      if (level_ <= minlevel_)
	return other_->sputc(c);
      return 0;
    }

  } // namespace utl

  extern std::ostream	error;
  extern std::ostream	warning;
  extern std::ostream	notice;
  extern std::ostream	dump;

  extern debug_ostream_t	debug;

} // namespace olsr

#endif // ! QOLYESTER_LOG_HXX
