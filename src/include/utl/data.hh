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

#ifndef QOLYESTER_UTL_DATA_HH
# define QOLYESTER_UTL_DATA_HH 1

# include <sys/types.h>
# include <cassert>

namespace olsr {

  namespace utl {

    // Forward declarations.
    struct Data;
    struct ConstData;

    namespace internal {

      // Constness related traits.
      //@{
      template <class T, class U>
      struct convert {
	enum { valid = true };
      }; // End of struct convert.

      template <>
      struct convert<ConstData, Data> {
	enum { valid = false };
      }; // End of struct convert<Const, NonConst>.

      template <class C>
      struct other {
	typedef ConstData return_type;
      }; // End of struct other.

      template <>
      struct other<ConstData> {
	typedef Data return_type;
      }; // End of struct other<Const>.
      //@}

      /// Data that need to be shared among instances of Data_.
      struct shared_data {
	shared_data(unsigned usecount, u_int8_t* buffer, ::size_t size);

	unsigned	usecount;
	u_int8_t*	buffer;
	::size_t	size;
      }; // End of struct shared_data.

    } // End of namespace internal.

    /// Base class for Data and ConstData.
    template <class Self>
    struct Data_
    {
      /// Constructors and destructor.
      //@{
      Data_();

      explicit
      inline Data_(const ::size_t size);

      inline Data_(const Data_& other);

      template <class T>
      inline Data_(const Data_<T>& other);

      inline ~Data_();
      //@}

      /// Affectation.
      template <class T>
      inline Self&	operator = (const Data_<T>& other);

      inline Self&	operator=(const Data_<Self>& other);

      /// Tell whether this object has an attached buffer or not.
      /// @warning *Not* equivalent to size() == 0.
      inline bool	empty() const;

      /// Get the available size at raw().
      inline ::size_t	size() const;

      /// Resize the buffer.
      inline void	fit(unsigned len);

      /// Create new smaller instances.
      //@{
      inline Self	shrink_by(unsigned len) const;
      inline Self	shrink_to(unsigned len) const;
      //@}

      /// Create new instances with greater offset (and smaller instance size).
      //@{
      inline Self	operator + (unsigned offset) const;
      inline Self&	operator += (unsigned offset);
      //@}

      /// Return size() - rhs.size().
      template <class T>
      inline ::size_t	operator - (const Data_<T>& rhs) const;

      /// Access the raw data.
      inline const u_int8_t*	raw() const;

      /// Copy the data to a specific location.
      //@{
      inline void		dump(u_int8_t* p) const;
      inline void		dump(Data_<Data>& d) const;
      //@}

      /// Retrieve the exact type of Data_<Self> (i.e. cast to self).
      //@{
      inline Self&		exact();
      inline const Self&	exact() const;
      //@}

    protected:
      /// Increment the buffer's usecount.
      inline void		use();
      /// Decrement the buffer's usecount.
      inline void		unuse();

      internal::shared_data*	data_;
      ::size_t			instance_size_;
      unsigned			offset_;

      friend
      class Data_<typename internal::other<Self>::return_type>;
    };

    struct Data : public Data_<Data>
    {
      inline Data();
      explicit inline Data(::size_t size);

      /// Access the raw data.
      inline u_int8_t*		raw();
    }; // End of struct Data.

    struct ConstData : public Data_<ConstData>
    {
      inline ConstData();
      explicit inline ConstData(::size_t size);
      inline ConstData(const Data& d);

      inline ConstData&	operator = (const Data& d);
    }; // End of struct ConstData.

  } // namespace utl

} // namespace olsr

# include "data.hxx"

#endif // ! QOLYESTER_UTL_DATA_HH
