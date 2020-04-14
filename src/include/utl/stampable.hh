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

#ifndef QOLYESTER_UTL_STAMPABLE_HH
# define QOLYESTER_UTL_STAMPABLE_HH 1

# include <map>
# include "comparator.hh"
# include "timeval.hh"

namespace olsr {

  namespace utl {

    class Stampable {
    protected:  // this is an abstract class
      inline Stampable();
    public:
      void		set_stamp(const TimeVal& now = TimeVal::now()) {
	_stamp = now;
      }
      const TimeVal	stamp() const {	return _stamp; }
      inline bool	expired(const TimeVal& period,
				const TimeVal& now = TimeVal::now()) const;

      bool		operator<(const Stampable& rhs) const {
	return _stamp < rhs._stamp;
      }
    private:
      TimeVal	_stamp;
    };

    template <unsigned Dim, class Key, class Compare = pless<Key> >
    class MultiStampable {
      typedef MultiStampable<Dim, Key, Compare>	This;
      typedef std::map<Key, TimeVal>		stampset_t;
    protected:
      inline MultiStampable();
    public:
      inline void	set_stamp(unsigned d, const Key& k,
				  const TimeVal& now = TimeVal::now());
      inline void	remove_stamp(unsigned d, const Key& k);
      inline const TimeVal	stamp(unsigned d, const Key& k) const;
      inline bool	expired(unsigned d,
				const Key& k,
				const TimeVal& period,
				const TimeVal& now = TimeVal::now()) const;
      inline bool	less(unsigned d, const Key& k, const This& rhs) const;
    private:
      stampset_t	_stampset[Dim];
    };

    template <class Key, class Compare>
    class MultiStampable<0, Key, Compare>;

    template <class Key, class Compare>
    class MultiStampable<1, Key, Compare> {
      typedef MultiStampable<1, Key, Compare>	This;
      typedef std::map<Key, TimeVal>		stampset_t;
    protected:
      inline MultiStampable();
    public:
      inline void	set_stamp(const Key& k,
				  const TimeVal& now = TimeVal::now());
      void		set_stamp(unsigned d, const Key& k,
				  const TimeVal& now = TimeVal::now()) {
	assert(d == 0);
	(void) d;
	return set_stamp(k, now);
      }

      inline void	remove_stamp(const Key& k);
      void		remove_stamp(unsigned d, const Key& k) {
	assert(d == 0);
	(void) d;
	return remove_stamp(k);
      }
      inline const TimeVal	stamp(const Key& k) const;
      const TimeVal	stamp(unsigned d, const Key& k) const {
	assert(d == 0);
	(void) d;
	return stamp(k);
      }
      inline bool	expired(const Key& k,
				const TimeVal& period,
				const TimeVal& now = TimeVal::now()) const;
      bool		expired(unsigned d, const Key& k,
				const TimeVal& period,
				const TimeVal& now = TimeVal::now()) const {
	assert(d == 0);
	(void) d;
	return expired(k, period, now);
      }
      inline bool	less(const Key& k, const This& rhs) const;
      bool		less(unsigned d, const Key& k, const This& rhs) const {
	assert(d == 0);
	(void) d;
	return less(k, rhs);
      }
    private:
      stampset_t	_stampset;
    };

    template <class Key, class Iter>
    struct imultistampable_less {
    private:
      typedef imultistampable_less<Key, Iter>	This;
    public:
      imultistampable_less(unsigned d, const Key& k)
	: _d(d),
	  _k(k)
      {}

      bool operator()(const Iter& a, const Iter& b) const {
	return a->less(_d, _k, *b);
      }
    private:
      const unsigned	_d;
      const Key		_k;
    };

    template <unsigned Dim, class Key, class Compare = pless<Key> >
    struct multistampable_less {
    private:
      typedef multistampable_less<Dim, Key, Compare>	This;
    public:
      multistampable_less(unsigned d, const Key& k)
	: _d(d),
	  _k(k)
      {}

      bool operator()(const MultiStampable<Dim, Key, Compare>& a,
		      const MultiStampable<Dim, Key, Compare>& b) const {
	return a.less(_d, _k, b);
      }

      bool operator()(const MultiStampable<Dim, Key, Compare>* a,
		      const MultiStampable<Dim, Key, Compare>* b) const {
	return a->less(_d, _k, *b);
      }
    private:
      const unsigned	_d;
      const Key		_k;
    };

    template <class Key, class Compare>
    struct multistampable_less<0, Key, Compare> {};

    template <class Key, class Compare>
    struct multistampable_less<1, Key, Compare> {
    private:
      typedef multistampable_less<1, Key, Compare>	This;
    public:
      multistampable_less(const Key& k)
	: _k(k)
      {}

      bool operator()(const MultiStampable<1, Key, Compare>& a,
		      const MultiStampable<1, Key, Compare>& b) const {
	return a.less(_k, b);
      }

      bool operator()(const MultiStampable<1, Key, Compare>* a,
		      const MultiStampable<1, Key, Compare>* b) const {
	return a->less(_k, *b);
      }
    private:
      const Key		_k;
    };

  } // namespace utl

} // namespace olsr

# include "stampable.hxx"

#endif // ! QOLYESTER_UTL_STAMPABLE_HH
