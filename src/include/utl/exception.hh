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

#ifndef QOLYESTER_UTL_EXCEPTION_HH
# define QOLYESTER_UTL_EXCEPTION_HH 1

# include <stdexcept>

namespace olsr {

  namespace utl {

    class ErrnoException : public std::runtime_error {
      typedef ErrnoException		This;
      typedef std::runtime_error	Super;
    public:
      ErrnoException(const std::string& what, int e)
	: Super(what),
	  errno_(e)
      {}
      
      int	get_errno() const { return errno_; }
    private:
      int	errno_;
    };

  } // namespace utl

  typedef utl::ErrnoException		errnoexcept_t;

} // namespace olsr

#endif // ! QOLYESTER_UTL_EXCEPTION_HH
