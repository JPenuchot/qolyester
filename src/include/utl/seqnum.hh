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
/// @file   seqnum.hh
/// @author Ignacy Gawedzki
/// @date   Sun Sep 28 17:58:32 2003
/// 
/// @brief  Sequence number class declarations.
/// 

#ifndef QOLYESTER_UTL_SEQNUM_HH
# define QOLYESTER_UTL_SEQNUM_HH 1

# include <sys/types.h>

namespace olsr {

  namespace utl {

    namespace internal {

      //*******************************************************
      //*  Static type information and metaprogramming tools  *
      //*******************************************************

      template <typename T> struct maxvalue_trait {};

# define DECLARE_MAX_TRAIT(Type,Max) \
      template <> struct maxvalue_trait<Type> { static const Type value = (Max); }

      DECLARE_MAX_TRAIT(int8_t, 127);
      DECLARE_MAX_TRAIT(u_int8_t, 255);
      DECLARE_MAX_TRAIT(int16_t, 32767);
      DECLARE_MAX_TRAIT(u_int16_t, 65535);
      DECLARE_MAX_TRAIT(int32_t, 2147483647);
      DECLARE_MAX_TRAIT(u_int32_t, 4294967295U);

      template <typename T> struct assert_unsigned {};

# define DECLARE_UNSIGNED(Type) \
      template <> struct assert_unsigned<Type> { struct this_type_is_not_unsigned; }

      DECLARE_UNSIGNED(u_int8_t);
      DECLARE_UNSIGNED(u_int16_t);
      DECLARE_UNSIGNED(u_int32_t);

# define ASSERT_UNSIGNED(Type) \
      typedef typename internal::assert_unsigned<Type>::this_type_is_not_unsigned _unsigned_dummy_t
    
    } // namespace internal

    //*****************************************************
    //*  Seqnum class for sequence number implementation  *
    //*****************************************************

    ///
    /// @class Seqnum
    /// @brief Sequence number class.
    /// @param T implementation type
    ///
    /// This class implements all the underlining processing details
    /// about sequence numbers (see draft section 19).
    ///
    template <typename T>
    class Seqnum {
      typedef Seqnum<T>	This;	// Convenience definition
      ASSERT_UNSIGNED(T);		// Static assertion
    public:

      // Default and copy constructors
      Seqnum() :
	_value(0)
      {}

      Seqnum(const This& other) :
	_value(other._value)
      {}

      Seqnum(const T& other) :
	_value(other)
      {}

      // Comparators
      bool
      operator>(const This& rhs) const {
	if (_value > rhs._value && _value - rhs._value <= This::_halfmax)
	  return true;
	if (rhs._value > _value && rhs._value - _value > This::_halfmax)
	  return true;
	return false;
      }

      bool
      operator<(const This& rhs) const {
	if (rhs._value > _value && rhs._value - _value <= This::_halfmax)
	  return true;
	if (_value > rhs._value && _value - rhs._value > This::_halfmax)
	  return true;
	return false;
      }

      bool
      operator==(const This& rhs) const {
	return _value == rhs._value;
      }

      bool
      operator>=(const This& rhs) const {
	return *this > rhs || *this == rhs;
      }

      bool
      operator<=(const This& rhs) const {
	return *this < rhs || *this == rhs;
      }

      // Pre- and post-incrementors
      This&
      operator++() {
	++_value;
	return *this;
      }

      This
      operator++(int) {
	This tmp(*this);
	++*this;
	return tmp;
      }
      
      // Cast to base value type operator
      operator T() const {
	return _value;
      }

      // Reset operator (just in case)
      void
      reset() {
	_value = 0;
      }

    private:
      T			_value;
      static const T	_halfmax = internal::maxvalue_trait<T>::value / 2;
    };

  } // namespace utl

  typedef utl::Seqnum<u_int16_t>	seqnum_t;

} // namespace olsr

#endif // ! QOLYESTER_UTL_SEQNUM_HH
