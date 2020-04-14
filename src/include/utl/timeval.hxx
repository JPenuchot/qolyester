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

#ifndef QOLYESTER_UTL_TIMEVAL_HXX
# define QOLYESTER_UTL_TIMEVAL_HXX 1

# include <limits>

# include "timeval.hh"

namespace olsr {

  namespace utl {

    TimeVal::TimeVal(unsigned sec, unsigned usec) {
      assert(usec < 1000000);
      _tv.tv_sec  = sec;
      _tv.tv_usec = usec;
    }

    TimeVal::TimeVal(const This& rhs) {
      assert(rhs._tv.tv_usec < 1000000);
      _tv = rhs._tv;
    }

    TimeVal::TimeVal(const ::timeval& rhs) {
      assert(rhs.tv_usec < 1000000);
      _tv = rhs;
    }

    TimeVal::TimeVal(const unsigned period) {
      _tv.tv_usec  = period * 1000;
      _tv.tv_sec   = _tv.tv_usec / 1000000;
      _tv.tv_usec %= 1000000;
    }

    TimeVal::TimeVal(const int period) {
      assert(period >= 0);
      _tv.tv_usec  = period * 1000;
      _tv.tv_sec   = _tv.tv_usec / 1000000;
      _tv.tv_usec %= 1000000;
    }

    TimeVal::TimeVal(const double seconds) {
      _tv.tv_sec  = (unsigned) seconds;
      _tv.tv_usec = (unsigned) ((seconds - _tv.tv_sec) * 1e6);
    }
    
    int
    TimeVal::poll_time() const {
//    This is the way to have an exact condition, but since we don't
//    care, we prefer the second way, which is faster.
//       if (_tv.tv_sec == INT_MAX / 1000 &&
//           _tv.tv_usec > INT_MAX % 1000 * 1000 ||
// 	     _tv.tv_sec > INT_MAX / 1000)

      if (_tv.tv_sec >= std::numeric_limits<int>::max() / 1000)
	return std::numeric_limits<int>::max();
      return _tv.tv_sec * 1000 + _tv.tv_usec / 1000;
    }

    TimeVal
    TimeVal::operator+(const This& rhs) const {
      TimeVal	res(rhs);

      res._tv.tv_usec += _tv.tv_usec;
      res._tv.tv_sec  += _tv.tv_sec + res._tv.tv_usec / 1000000;
      res._tv.tv_usec %= 1000000;
      
      return res;
    }
    
    TimeVal
    TimeVal::operator-(const This& rhs) const {
      assert(rhs <= *this);

      TimeVal	res(*this);

      res._tv.tv_usec -= rhs._tv.tv_usec;
      res._tv.tv_sec  -= rhs._tv.tv_sec;

      res._tv.tv_usec %= 1000000;

      if (res._tv.tv_usec < 0) {
	res._tv.tv_usec += 1000000;
	--res._tv.tv_sec;
      }

      return res;
    }

    float
    TimeVal::diff(const This& rhs) const {
      float	val = distance(rhs).to_float();

      if (rhs <= *this)
	return val;
      return -val;
    }

    TimeVal
    TimeVal::distance(const This& rhs) const {
      const This&	r = rhs < *this ? rhs : *this;
      const This&	l = *this < rhs ? rhs : *this;

      return l - r;
    }

    TimeVal
    TimeVal::operator+(unsigned msec) const {
      return *this + TimeVal(msec / 1000, (msec % 1000) * 1000);
    }
    
    TimeVal&
    TimeVal::operator+=(unsigned msec) {
      _tv.tv_usec += msec * 1000;
      _tv.tv_sec  += _tv.tv_usec / 1000000;
      _tv.tv_usec %= 1000000;
      return *this; 
    }

    TimeVal&	
    TimeVal::operator+=(const This& rhs) {
      _tv.tv_usec += rhs._tv.tv_usec;
      _tv.tv_sec  += rhs._tv.tv_sec + _tv.tv_usec / 1000000;
      _tv.tv_usec %= 1000000;
      assert(_tv.tv_sec >= 0);
      return *this;
    }
    
    bool
    TimeVal::operator<(const This& rhs) const {
      if (_tv.tv_sec == rhs._tv.tv_sec)
	return _tv.tv_usec < rhs._tv.tv_usec;
      return _tv.tv_sec < rhs._tv.tv_sec;
    }
    
    TimeVal
    TimeVal::abs() const {
      ::timeval	ret = _tv;
	
      if (ret.tv_sec < 0)
	ret.tv_sec = -ret.tv_sec;

      return This(ret);
    }

    //
    // This method compares two TimeVals with a granularity of
    // utl::internal::delta.
    //
    bool
    TimeVal::is_past(const This& now) const {
//       return *this < (now + internal::delta);
      return *this <= now;
    }

    bool
    TimeVal::operator==(const This& rhs) const {
      return _tv.tv_sec == rhs._tv.tv_sec && _tv.tv_usec == rhs._tv.tv_usec;
    }
    
    bool
    TimeVal::operator!=(const This& rhs) const {
      return !operator==(rhs);
    }

    bool
    TimeVal::operator<=(const This& rhs) const {
      return *this < rhs || *this == rhs;
    }

    TimeVal
    TimeVal::operator*(const double& f) const {
      assert(f >= 0);

//       double	value = _tv.tv_sec + _tv.tv_usec * 1e-6;
//       return This(f * value);

      typedef std::numeric_limits<long>	nl_t;
      
      ::timeval	res = _tv;

      assert(res.tv_usec <= nl_t::max() / f);

      res.tv_usec = long(res.tv_usec * f);

      long	carry = res.tv_usec / 1000000;

      res.tv_usec %= 1000000;

      assert(res.tv_sec <= (nl_t::max() - carry) / f);

      res.tv_sec = long(res.tv_sec * f) + carry;

      res.tv_usec +=
	long((res.tv_sec * f - long(res.tv_sec * f)) * 1e6) % 1000000;

      assert((long) res.tv_sec <= nl_t::max() - res.tv_usec / 1000000);

      res.tv_sec += res.tv_usec / 1000000;

      res.tv_usec %= 1000000;

      return This(res);
    }

    TimeVal
    TimeVal::operator/(const double& f) const {
      assert(f > 0);
      ::timeval	res;
      double		sec  = _tv.tv_sec / f;
      res.tv_sec  = unsigned(sec);
      res.tv_usec = unsigned((sec - double(unsigned(sec))) * 1e6 +
			     _tv.tv_usec / double(f));
      return This(res);
    }

    TimeVal
    TimeVal::jitter(const This& j) {
      double usecs = j._tv.tv_sec * 1e6 + j._tv.tv_usec;
      double rands = usecs * (rand() / (RAND_MAX + 1.));
      ::timeval	res;
      res.tv_sec  = unsigned(rands / 1e6);
      res.tv_usec = unsigned(rands - res.tv_sec * 1e6);
      return This(res);
    }

    void
    TimeVal::set_now(const This& now) {
      _now = now;
    }

    float
    TimeVal::to_float() const {
      return _tv.tv_sec + _tv.tv_usec / 1e6f;
    }

    const TimeVal
    TimeVal::real_now() {
      ::timeval tmp;
# ifndef NDEBUG
      assert(::gettimeofday(&tmp, NULL) == 0);
# else
      ::gettimeofday(&tmp, NULL);
# endif
      return TimeVal(tmp);
    }

    const TimeVal
    TimeVal::in(const This& x) {
      return _now + x;
    }

    inline
    std::ostream& operator<<(std::ostream& o, const TimeVal& t)
    {
      return o << "{ sec = " << static_cast<timeval>(t).tv_sec
	       << ", usec = " << static_cast<timeval>(t).tv_usec << " }";
    }

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_UTL_TIMEVAL_HXX
