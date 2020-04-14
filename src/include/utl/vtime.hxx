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
/// @file   vtime.hxx
/// @author Ignacy Gawedzki
/// @date   Sat Sep 27 14:32:33 2003
/// 
/// @brief  Validity time processing implementation.
///

#ifndef QOLYESTER_UTL_VTIME_HXX
# define QOLYESTER_UTL_VTIME_HXX 1

# include "vtime.hh"

namespace olsr {

  namespace utl {

    Vtime::Vtime(const Vtime& other) :
      _value(other._value)
    {}

    Vtime::Vtime(const byte_t other) :
      _value(*(raw*)&other)
    {}

    // Construction of a Vtime from a second count.  This follows
    // exactly the procedure described in the draft in section 18.3.
    Vtime::Vtime(const float fsecs) :
      _value(0, 0) {
      assert(fsecs != 0.f);

      float	fsecsnormed(fsecs / C_CONSTANT);

      unsigned b = 0;
      while (fsecsnormed > (float)(1 << b))
	++b;
      --b;

      unsigned a = (unsigned)
	roundf(16 * (fsecs / (C_CONSTANT * (float)(1 << b)) - 1));
      b += a / 16;
      a %= 16;

      _value.mantissa = a;
      _value.exponent = b;
    }

    Vtime::Vtime(const timeval_t& other) :
      _value(Vtime(((::timeval)other).tv_sec +
		   (float) ((::timeval)other).tv_usec / 1e6f)._value)
    {}

    std::ostream&
    Vtime::output(std::ostream& o) const {
      return o << "{ m = " << (int) _value.mantissa
	       << ", e = " << (int) _value.exponent << " }";
    }

    inline
    std::ostream& operator<<(std::ostream& o, const Vtime& v)
    {
      return v.output(o);
    }

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_UTL_VTIME_HXX
