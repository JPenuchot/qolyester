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
/// @file   vtime.hh
/// @author Ignacy Gawedzki
/// @date   Sat Sep 27 14:02:25 2003
///
/// @brief  Validity time processing.
///
/// This is the location of the Vtime class declaration which
/// encapsulates all the processing and computation of validity time
/// to and from message headers.
///
#ifndef QOLYESTER_UTL_VTIME_HH
# define QOLYESTER_UTL_VTIME_HH 1

# include <sys/types.h>
# include <cmath>
# include "timeval.hh"
# include "cst/constants.hh"

namespace olsr {

  namespace utl {

    ///
    /// @class Vtime
    /// @brief Validity time processing and calculation.
    ///
    class Vtime {
      typedef u_int8_t	byte_t;
      typedef unsigned	secs_t;
    public:
      ///
      /// Copy constructor
      ///
      inline Vtime(const Vtime& other);

      ///
      /// Constructor.  Initializes from an encoded byte.
      ///
      /// @arg other byte from header of message
      ///
      explicit inline Vtime(const byte_t other);

      ///
      /// Constructor.
      ///
      /// @arg fsecs validity time in seconds
      ///
      inline Vtime(const float fsecs);

      ///
      /// Constructor.
      ///
      /// @arg other validity time
      ///
      inline Vtime(const timeval_t& other);

      ///
      /// Operator for conversion to byte.
      ///
      operator byte_t() { return *(byte_t*)&_value; }

      ///
      /// Operator for conversion to seconds.
      ///
      operator float() {
	return C_CONSTANT * (1 + (float) _value.mantissa / 16) *
	  (float)(1 << _value.exponent);
      }

      ///
      /// Operator for conversion to time.
      ///
      operator timeval_t() {
	float secs = floorf(*this);
	return timeval_t((unsigned) secs,
			 (unsigned)(((float) *this - secs) * 1e6f));
      }

      ///
      /// Printing method.
      ///
      inline std::ostream&	output(std::ostream& o) const;

    private:
      ///
      /// @struct raw
      /// @brief	Convenience data structure for raw access.
      ///
      struct raw {
	raw(const int m, const int e) : exponent(e), mantissa(m) {}
	byte_t exponent:4;
	byte_t mantissa:4;
      };
      raw		_value;	///< Internal raw value
    };

    inline
    std::ostream& operator<<(std::ostream&, const Vtime&);

  } // namespace utl

} // namespace olsr

# include "vtime.hxx"

#endif // ! QOLYESTER_UTL_VTIME_HH
