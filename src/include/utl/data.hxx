// Copyright (C) 2003-2009 Laboratoire de Recherche en Informatique

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

#ifndef QOLYESTER_UTL_DATA_HXX
# define QOLYESTER_UTL_DATA_HXX 1

# include <iostream>
# include "data.hh"
# include "meta.hh"

namespace olsr {

  namespace utl {

    /*--------------.
    | shared_data.  |
    `--------------*/

    namespace internal {

      shared_data::shared_data(unsigned usecount,
			       u_int8_t* buffer,
			       ::size_t size)
	: usecount (usecount),
	  buffer (buffer),
	  size (size)
      {
      }

    } // End of namespace internal.

    /*------------------------------.
    | Constructors and destructor.  |
    `------------------------------*/

    template <class Self>
    Data_<Self>::Data_()
      : data_ (0),
	instance_size_ (0),
	offset_ (0)
    {
    }

    template <class Self>
    Data_<Self>::Data_(const ::size_t size)
      : data_ (new internal::shared_data (1, new u_int8_t[size], size)),
	instance_size_ (size),
	offset_ (0)
    {
    }

    template <class Self>
    Data_<Self>::Data_(const Data_& other)
      : data_ (other.data_),
	instance_size_ (other.instance_size_),
	offset_ (other.offset_)
    {
      assert(not data_ or (data_->buffer and data_->usecount));
      use();
    }

    template <class Self>
    template <class T>
    Data_<Self>::Data_(const Data_<T>& other)
      : data_ (other.data_),
	instance_size_ (other.instance_size_),
	offset_ (other.offset_)
    {
      i_static_assert(internal::convert<T, Self>::valid);
      assert(not data_ or (data_->buffer and data_->usecount));
      use();
    }

    template <class Self>
    Data_<Self>::~Data_()
    {
      unuse();
    }

    /*--------------.
    | Affectation.  |
    `--------------*/

    template <class Self>
    template <class T>
    Self&
    Data_<Self>::operator = (const Data_<T>& other)
    {
      i_static_assert(internal::convert<T, Self>::valid);

      if (static_cast<const void*> (&other) != this) {
	unuse();

	data_ = other.data_;
	offset_ = other.offset_;
	instance_size_ = other.instance_size_;

	use();
      }

      assert(not data_ or (data_->buffer and data_->usecount));

      return exact();
    }

    template <class Self>
    Self&
    Data_<Self>::operator = (const Data_<Self>& other)
    {
      return operator=<Self>(other);
    }

    /*----------------.
    | Misc. methods.  |
    `----------------*/

    template <class Self>
    bool
    Data_<Self>::empty() const
    {
      return data_ == 0;
    }

    template <class Self>
    ::size_t
    Data_<Self>::size() const
    {
      return instance_size_;
    }

    /*-------------------.
    | Resizing methods.  |
    `-------------------*/

    template <class Self>
    void
    Data_<Self>::fit(unsigned len)
    {
      assert(offset_ + len <= data_->size);
      instance_size_ = len;
    }

    template <class Self>
    Self
    Data_<Self>::shrink_by(unsigned len) const
    {
      assert(len <= instance_size_);

      Self	tmp (exact());
      tmp.instance_size_ -= len;
      return tmp;
    }

    template <class Self>
    Self
    Data_<Self>::shrink_to(unsigned len) const
    {
      Self	tmp (exact());
      tmp.fit(len);
      return tmp;
    }

    template <class Self>
    Self
    Data_<Self>::operator + (unsigned offset) const
    {
      Self	tmp (exact());
      tmp += offset;
      return tmp;
    }

    template <class Self>
    Self&
    Data_<Self>::operator += (unsigned offset)
    {
      assert(offset_ + offset <= data_->size);

      offset_ += offset;
      instance_size_ -= offset;
      return exact();
    }

    template <class Self>
    template <class T>
    ::size_t
    Data_<Self>::operator - (const Data_<T>& rhs) const
    {
      assert(rhs.instance_size_ <= instance_size_);

      return instance_size_ - rhs.instance_size_;
    }

    /*-------------.
    | Raw access.  |
    `-------------*/

    template <class Self>
    const u_int8_t*
    Data_<Self>::raw() const
    {
      return data_->buffer + offset_;
    }

    template <class Self>
    void
    Data_<Self>::dump(u_int8_t* p) const
    {
      memcpy(p, data_->buffer + offset_, instance_size_);
    }

    template <class Self>
    void
    Data_<Self>::dump(Data_<Data>& d) const
    {
      assert(d.instance_size_ >= instance_size_);

      memcpy(d.data_->buffer + d.offset_,
	     data_->buffer + offset_,
	     instance_size_);
    }

    /*----------.
    | exact().  |
    `----------*/

    template <class Self>
    Self&
    Data_<Self>::exact()
    {
      return *static_cast<Self*> (this);
    }

    template <class Self>
    const Self&
    Data_<Self>::exact() const
    {
      return *static_cast<const Self*> (this);
    }

    /*------------------.
    | Private methods.  |
    `------------------*/

    template <class Self>
    void
    Data_<Self>::use()
    {
      if (data_) {
	// Avoid wrap-around conditions.
	assert(data_->usecount < std::numeric_limits<unsigned>::max());
	++data_->usecount;
      }
    }

    template <class Self>
    void
    Data_<Self>::unuse()
    {
      if (data_) {
	assert(data_->usecount);
	if (not --data_->usecount) {
	  delete[] data_->buffer;
	  delete data_;
	  data_ = 0;
	}
      }
    }

    /*-------.
    | Data.  |
    `-------*/

    inline
    Data::Data() : Data_<Data> ()
    {
    }

    inline
    Data::Data(::size_t size) : Data_<Data> (size)
    {
    }

    inline
    u_int8_t*
    Data::raw()
    {
      return data_->buffer + offset_;
    }

    /*------------.
    | ConstData.  |
    `------------*/

    inline
    ConstData::ConstData() : Data_<ConstData> ()
    {
    }

    inline
    ConstData::ConstData(::size_t size) : Data_<ConstData> (size)
    {
    }

    ConstData::ConstData(const Data& d) : Data_<ConstData> (d)
    {
    }

    inline
    ConstData&
    ConstData::operator = (const Data& d)
    {
      return Data_<ConstData>::operator = (d);
    }

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_UTL_DATA_HXX
