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
/// @file   timeval.hh
/// @author Ignacy Gawedzki
/// @date   Sat Sep 27 16:33:41 2003
///
/// @brief  TimeVal class definitions.
///
/// All time operations are done with respect to a current time
/// reference which is synchronized with the system clock in certain
/// well-known locations (e.g. in the event loop).  This is mainly to
/// avoid synchronization on each time we want to know the current
/// time.  Moreover, this allows to assume that inside some parts of
/// the code, the time reference is the same (read: if you timestamp
/// some objects one after another between two clock synchonizations,
/// you know that the stamps are equal)
///

#ifndef QOLYESTER_UTL_TIMEVAL_HH
# define QOLYESTER_UTL_TIMEVAL_HH 1

# include <sys/time.h>
# include <cassert>
# include <ostream>

namespace olsr {

  namespace utl {

    ///
    /// @class TimeVal
    /// @brief	Proxy class for the timeval standard C data structure.
    ///
    /// This class encapsulates all the required time operations.
    ///
    class TimeVal {
      typedef TimeVal	This;
    public:
      ///
      /// Constructor.
      ///
      /// @arg sec seconds
      /// @arg usec microseconds
      ///
      inline
      TimeVal(unsigned sec, unsigned usec);

      ///
      /// Copy constructor.
      ///
      inline
      TimeVal(const This& rhs);

      ///
      /// Copy constructor from timeval standard data structure.
      ///
      inline
      TimeVal(const ::timeval& rhs);

      ///
      /// Constructor from period in milliseconds.
      ///
      inline
      TimeVal(const unsigned period);

      ///
      /// Constructor from period in milliseconds.  Provided for
      /// convenience.
      ///
      inline
      TimeVal(const int period);

      ///
      /// Constructor from floating point second count.
      ///
      inline
      TimeVal(const double seconds);

      inline
      int	poll_time() const;

      ///
      /// Internal addition.
      ///
      inline
      This	operator+(const This& rhs) const;

      ///
      /// Internal subtraction.
      ///
      inline
      This	operator-(const This& rhs) const;

      ///
      /// Internal self-addition.
      ///
      inline
      This&	operator+=(const This& rhs);

      ///
      /// Equality operator.
      ///
      inline
      bool	operator==(const This& rhs) const;

      inline
      bool	operator!=(const This& rhs) const;

      ///
      /// Comparison operator.
      ///
      inline
      bool	operator<(const This& rhs) const;

      ///
      /// Comparison operator.
      ///
      inline
      bool	operator<=(const This& rhs) const;

      ///
      /// Comparison operator.
      ///
      bool	operator>(const This& rhs) const { return !operator<=(rhs); }

      ///
      /// Comparison operator.
      ///
      bool	operator>=(const This& rhs) const { return !operator<(rhs); }

      inline
      float	diff(const This& rhs = now()) const;

      ///
      /// Absolute difference computation method.
      ///
      inline
      This	distance(const This& rhs) const;

      ///
      /// Realistic comparison operator.  This takes into account the
      /// jitter in signal delivery time.  FIXME: this should not be
      /// necessary, according to the specifications, but seems to be
      /// according to experiments.
      ///
      /// @arg now right-hand-side operand, default is now
      ///
      inline
      bool	is_past(const This& now = now()) const;

      ///
      /// External addition with integer number of milliseconds.
      ///
      inline
      This	operator+(unsigned msec) const;

      ///
      /// External self-addition with integer number of milliseconds.
      ///
      inline
      This&	operator+=(unsigned msec);

      ///
      /// External multiplication with double.
      ///
      inline
      This	operator*(const double& f) const;

      ///
      /// External division with double.
      ///
      inline
      This	operator/(const double& f) const;

      ///
      /// Absolute value operator.
      ///
      inline
      This	abs() const;

      ///
      /// Conversion operator to timeval data structure.
      ///
      operator ::timeval() const {
	return _tv;
      }

      ///
      /// Jitter calculation.
      ///
      /// @arg j upper bound of random jitter
      ///
      inline
      static This	jitter(const This& j);

      ///
      /// Clock stepping.  Called once in a while to set current time.
      ///
      inline
      static void	set_now(const This& now = TimeVal::real_now());

      ///
      /// Method to convert to floating point number of seconds.
      ///
      inline
      float		to_float() const;

      ///
      /// Static accessor to current time reference.
      ///
      static const TimeVal&	now() {	return _now; }

      ///
      /// Static accessor to current system time.
      ///
      inline
      static const TimeVal	real_now();

      ///
      /// Static convenience future time calculation.
      ///
      /// @arg x time delay
      /// @return now + x
      ///
      inline
      static const TimeVal	in(const This& x);

      ///
      /// Static convenience future jitter time calculation.
      ///
      /// @arg x time jitter
      /// @return now + jitter(x)
      ///
      static const TimeVal	in_jitter(const This& x) {
	return in(jitter(x));
      }
    private:
      timeval		_tv;	///< The time stored in timeval data structure
      static This	_now;	///< Static time reference
    };

    std::ostream& operator<<(std::ostream&, const TimeVal&);

  } // namespace utl

  typedef utl::TimeVal	timeval_t;

} // namespace olsr

# include "timeval.hxx"

#endif // ! QOLYESTER_UTL_TIMEVAL_HH
