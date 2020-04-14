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

///
/// @file   set.hh
/// @author Ignacy Gawedzki
/// @date   Sun Sep 28 18:26:04 2003
///
/// @brief  Simple generic set definitions.
///

#ifndef QOLYESTER_UTL_SET_HH
# define QOLYESTER_UTL_SET_HH 1

# include <set>
# include <map>
# include "iterator.hh"
# include "meta.hh"
# include "comparator.hh"
# include "stampable.hh"

namespace olsr {

  namespace sch {
    class StatePrinter;
  }

  namespace utl {

    namespace internal {

      template <class T>
      struct builder {
	static T make(const T& x) {
	  return T(x);
	}
      };

      template <class Iter>
      struct builder<DeconstIterator<Iter> > {
	static DeconstIterator<Iter> make(const Iter& x) {
	  return DeconstIterator<Iter>::build(x);
	}
      };

    } // namespace internal

    ///
    /// @class Set
    /// @brief	Simple generic set.
    /// @param Elem element class
    /// @param Iset implementation set class
    /// @param Iter mutable iterator on implementation set, defaults to Iset::iterator
    /// @param CIter immutable iterator on implementation set, defaults to Iset::const_iterator
    ///
    template <class Elem, class Iset,
	      class Iter = typename Iset::iterator,
	      class CIter = typename Iset::const_iterator>
    class Set {
      typedef Set<Elem, Iset, Iter, CIter>	This;
      typedef Iset				set_t;
    public:
      typedef CIter			const_iterator;
      typedef Iter			iterator;

      Set() : set_() {}

      ///
      /// Begining of the set.
      /// @return iterator on the first element
      ///
      iterator	begin() { return internal::builder<Iter>::make(set_.begin()); }

      ///
      /// End of the set.
      /// @return iterator past the last element
      ///
      iterator	end() { return internal::builder<Iter>::make(set_.end()); }

      ///
      /// Find a given element in the set.
      /// @arg e reference to the element to find
      /// @return iterator on the element or past the end if not found.
      ///
      iterator	find(const Elem& e) {
	return internal::builder<Iter>::make(set_.find(e));
      }

      ///
      /// Begining of the set.
      /// @return const iterator on the first element
      ///
      const_iterator	begin() const {
	return internal::builder<CIter>::make(set_.begin());
      }

      ///
      /// End of the set.
      /// @return const iterator past the last element
      ///
      const_iterator	end() const {
	return internal::builder<CIter>::make(set_.end());
      }

      ///
      /// Find a given element in the set.
      /// @arg e reference to the element to find
      /// @return const iterator on the element or past the end if not found.
      ///
      const_iterator	find(const Elem& e) const {
	return internal::builder<CIter>::make(set_.find(e));
      }

      ///
      /// Element insertion method.
      /// @arg e reference to the element to insert
      /// @return pair of iterator on the inserted element and bool
      /// indicating whether the element was actually inserted.
      ///
      std::pair<iterator, bool>	insert(const Elem& e) {
	std::pair<typename Iset::iterator, bool>	p =
	  set_.insert(e);
	return std::pair<iterator, bool>(internal::builder<Iter>::make(p.first),
					 p.second);
      }

      ///
      /// Element range insertion method.
      ///
      template <class InputIterator>
      void			insert(InputIterator f, InputIterator l) {
	set_.insert(f, l);
      }
      ///
      /// Element removal method.
      /// @arg pos iterator on the element to remove
      ///
      void			erase(iterator pos) { set_.erase(pos); }

      ///
      /// Element removal method.
      /// @arg e reference to the element to remove
      ///
      void			erase(const Elem& e) { set_.erase(e); }

      ///
      /// Empty predicate method.
      /// @return true if the set is empty
      ///
      bool	empty() const { return set_.empty(); }

      typename set_t::size_type
      size() const
      {
	return set_.size();
      }

      ///
      /// Hold
      ///
      bool	operator[](const Elem& e) const {
	return find(e) != end();
      }

      This&	operator|=(const This& rhs) {
	for (const_iterator i = rhs.begin(); i != rhs.end(); ++i)
	  insert(*i);
	return *this;
      }

      This	operator|(const This& rhs) const {
	return This(*this) |= rhs;
      }

      This&	operator-=(const This& rhs) {
	for (const_iterator i = rhs.begin(); i != rhs.end(); ++i)
	  erase(*i);
	return *this;
      }

      This	operator-(const This& rhs) const {
	return This(*this) -= rhs;
      }

      This&	operator&=(const This& rhs) {
	for (iterator i = begin(); i != end(); /* ++i elsewhere */)
	  if (!rhs[*i]) {
	    iterator tmp = i++;
	    erase(tmp);
	  } else
	    ++i;
	return *this;
      }

      This	operator&(const This& rhs) const {
	return This(*this) &= rhs;
      }
    private:
      set_t	set_;

      friend class sch::StatePrinter;

    };

    template <class Set>
    struct index_traits;

    template <template <class, class, class> class S,
	      class K, class C, class A>
    struct index_traits< S<K, C, A> > {
      typedef typename S<K, C, A>::iterator		iter;
      typedef typename S<K, C, A>::value_type		value;
      typedef typename type_traits<value>::const_val	const_elem;
      typedef typename type_traits<value>::const_ref	const_ref;
      typedef typename type_traits<value>::const_ptr	const_ptr;
      typedef typename S<K, C, A>::key_compare		key_comp;
    };

    template <template <class, class, class> class Cont,
	      class Set,
	      class Comp = iless<typename Set::iterator> >
    class Index;

    template <class Set, class Comp>
    class Index<std::set, Set, Comp> {
      typedef Index<std::set, Set, Comp>		This;
      typedef typename index_traits<Set>::iter		Iter;

      typedef std::set<Iter, Comp>			set_t;

      typedef typename set_t::const_iterator		c_iterator_;
      typedef typename index_traits<Set>::value		Value;
      typedef typename index_traits<Set>::const_elem	Elem;
    public:
      typedef Elem					value_type;
      typedef typename index_traits<Set>::const_ref	const_reference;
      typedef const_reference				reference;
      typedef typename index_traits<Set>::const_ptr	const_pointer;
      typedef const_pointer				pointer;
      typedef DerefIterator<c_iterator_>		const_iterator;
      typedef const_iterator				iterator;

      const_iterator	begin() const {
	return const_iterator::build(c_.begin());
      }
      const_iterator	end() const {
	return const_iterator::build(c_.end());
      }
      const_iterator	find(const Elem& x) const {
	Set	tmp;
	tmp.insert(x);
	return const_iterator::build(c_.find(tmp.begin()));
      }

      typename set_t::size_type	size() const { return c_.size(); }

      std::pair<const_iterator, bool>	insert(const Iter& x) {
	std::pair<typename set_t::iterator, bool>	p = c_.insert(x);
	return std::pair<const_iterator, bool>(const_iterator::build(p.first),
					       p.second);
      }

      void		erase(const_iterator pos) {
	c_.erase(pos);
      }

      void		erase(const Iter& x) {
	c_.erase(x);
      }
    private:
      set_t	c_;
    };

    template <class Set, class Comp>
    class Index<std::multiset, Set, Comp> {
      typedef Index<std::multiset, Set, Comp>		This;
      typedef typename index_traits<Set>::iter		Iter;

      typedef std::multiset<Iter, Comp>			set_t;

      typedef typename set_t::const_iterator		c_iterator_;
      typedef typename index_traits<Set>::value		Value;
      typedef typename index_traits<Set>::const_elem	Elem;
      typedef typename Set::key_compare			comp_t;
    public:
      typedef Elem					value_type;
      typedef typename index_traits<Set>::const_ref	const_reference;
      typedef const_reference				reference;
      typedef typename index_traits<Set>::const_ptr	const_pointer;
      typedef const_pointer				pointer;
      typedef DerefIterator<c_iterator_>		const_iterator;
      typedef const_iterator				iterator;

      Index(const typename Set::key_compare& kc = comp_t())
	: kc_(kc) {}

      const_iterator	begin() const {
	return const_iterator::build(c_.begin());
      }
      const_iterator	end() const { return const_iterator::build(c_.end()); }

      std::pair<const_iterator, const_iterator>
      equal_range(const Elem& x) const {
	Set	tmp;
	tmp.insert(x);
	std::pair<c_iterator_, c_iterator_>	er =
	  c_.equal_range(tmp.begin());
	return std::pair<const_iterator,
	                 const_iterator>(const_iterator::build(er.first),
					 const_iterator::build(er.second));
      }

      typename set_t::size_type	size() const { return c_.size(); }

      const_iterator	insert(const Iter& x) {
	return const_iterator::build(c_.insert(x));
      }

      void		erase(const_iterator pos) {
	c_.erase(pos);
      }

      void		erase(const Iter& x) {
	std::pair<typename set_t::iterator,
	          typename set_t::iterator>	set_er = c_.equal_range(x);
	std::pair<iterator, iterator>	er =
	  std::pair<iterator, iterator>(iterator::build(set_er.first),
					iterator::build(set_er.second));
	for (iterator i = er.first; i != er.second; /* ++i elsewhere */)
	  if (!kc_(*i, *x) && !kc_(*x, *i)) {
	    iterator	tmp = i++;
	    erase(tmp);
	  } else
	    ++i;
      }
    private:
      const typename Set::key_compare	kc_;
      set_t		c_;
    };

    template <class Set,
	      unsigned Dim,
	      class Key,
	      class Comp = imultistampable_less<Key, typename Set::iterator> >
    class MSIndex {
      typedef MSIndex<Set, Dim, Key, Comp>		This;
      typedef typename Set::iterator			Iter;
      typedef typename Set::key_compare			IterComp;
      typedef typename Set::value_type			Value;
      typedef typename type_traits<Value>::const_val	CElem;
      typedef std::multiset<Iter, Comp>			set_t;
      typedef std::map<Key, set_t>			setmap_t;
      typedef typename set_t::const_iterator		c_iterator_;
      typedef DerefIterator<c_iterator_>		d_iterator_;
    public:
      typedef Key					key_type;
      typedef Iter					value_type;
      typedef KeyedIterator<Key, d_iterator_>		const_iterator;
      typedef const_iterator				iterator;

      MSIndex(const Set& s)
	: m_(),
	  c_(s.key_comp()),
	  s_(s)
      {}

      iterator	begin(const Key& k) const {
	typename setmap_t::const_iterator	x = m_.find(k);
	assert(x != m_.end());
	return iterator(d_iterator_::build(x->second.begin()), k);
      }

      iterator	end(const Key& k) const {
	typename setmap_t::const_iterator	x = m_.find(k);
	assert(x != m_.end());
	return iterator(d_iterator_::build(x->second.end()), k);
      }

      iterator	find(const Key& k, const Iter& x) const {
	typename setmap_t::const_iterator	pos = m_.find(k);
	assert(pos != m_.end());
	return iterator(d_iterator_::build(pos->second.find(x)), k);
      }

      void	insert(const Iter& x) {
	for (typename setmap_t::iterator i = m_.begin(); i != m_.end(); ++i)
	  i->second.insert(x);
      }

      void	erase(const Iter& x) {
	for (typename setmap_t::iterator i = m_.begin(); i != m_.end(); ++i) {
	  std::pair<typename set_t::iterator, typename set_t::iterator>	p =
	    i->second.equal_range(x);
	  for (typename set_t::iterator j = p.first;
	       j != p.second; /* ++j elsewhere */)
	    if (!c_(*x, **j) && !c_(**j, *x)) {
	      typename set_t::iterator	tmp = j++;
	      i->second.erase(tmp);
	    } else
	      ++j;
	}
      }

      void	set_stamp(const_iterator pos,
			  const TimeVal& now = TimeVal::now()) {
	typename setmap_t::iterator	x = m_.find(pos.key());
	// This check is important to avoid looping of MaskIterator.
	if (stamp(pos) == now)
	  return;
	Iter	tmp = *static_cast<c_iterator_&>(pos);
	x->second.erase(pos);
	const_cast<Value&>(*tmp).set_stamp(Dim, pos.key(), now);
	x->second.insert(tmp);
      }

      const TimeVal	stamp(const_iterator pos) const {
	return (*static_cast<c_iterator_&>(pos))->stamp(Dim, pos.key());
      }

      bool	expired(const_iterator pos,
			const TimeVal& period,
			const TimeVal& now = TimeVal::now()) const {
	return (pos)->expired(Dim, pos.key(), period, now);
      }

      void	add_key(const Key& k) {
	assert(m_.find(k) == m_.end());
	typename setmap_t::value_type	new_entry(k, set_t(Comp(Dim, k)));
	for (typename Set::iterator i = s_.begin(); i != s_.end(); ++i)
	  new_entry.second.insert(i);
	m_.insert(new_entry);

      }

      void	remove_key(const Key& k) {
	typename setmap_t::iterator	x = m_.find(k);
	assert(x != m_.end());
	for (typename set_t::iterator i = x->second.begin();
	     i != x->second.end(); ++i)
	  const_cast<Value&>(**i).remove_stamp(Dim, k);
	m_.erase(x);
      }
    private:
      setmap_t		m_;
      const IterComp&	c_;
      const Set&	s_;
    };

    template <class> struct MSAdapt;

    template <class> struct MultiAdapt;

    template <class Set,
	      class Action = NoAction<Set>,
	      class Predicate = DefaultPredicate<Set>,
	      class ActionSet = Set>
    class Subset {
      typedef Subset<Set, Action, Predicate, ActionSet>		This;
      typedef MaskIterator<Set, Action, Predicate, ActionSet>	e_iterator_;
      typedef DeconstIterator<e_iterator_>			m_iterator_;
    public:
      typedef typename Set::iterator	set_iterator;
      typedef e_iterator_		mask_iterator;
      typedef typename Set::value_type	value_type;
      typedef m_iterator_		iterator;
      typedef iterator			const_iterator;

      Subset(Set& s)
	: actionset_(s),
	  set_(s)
      {}

      Subset(ActionSet& as, Set& s)
	: actionset_(as),
	  set_(s)
      {}

      iterator	begin() {
	return m_iterator_::build(e_iterator_(set_.begin(),
					      set_, actionset_).skip());
      }

      iterator	end() {
	return m_iterator_::build(e_iterator_(set_.end(),
					      set_, actionset_));
      }

      iterator	find(const typename Set::value_type& x) {
	return m_iterator_::build(e_iterator_(set_.find(x),
					      set_, actionset_).check());
      }

      void	cleanup() {
	for (iterator i = begin(); i != end(); ++i)
	  ;
      }

    private:
      ActionSet&	actionset_;
      Set&		set_;
    };

    template <class Set,
	      class Action,
	      class Predicate,
	      class ActionSet>
    class Subset<MultiAdapt<Set>, Action, Predicate, ActionSet> {
      typedef Subset<MultiAdapt<Set>, Action, Predicate, ActionSet>	This;
      typedef MaskIterator<Set, Action, Predicate, ActionSet>	e_iterator_;
      typedef DeconstIterator<e_iterator_>			m_iterator_;
    public:
      typedef typename Set::iterator	set_iterator;
      typedef e_iterator_		mask_iterator;
      typedef typename Set::value_type	value_type;
      typedef m_iterator_		iterator;
      typedef iterator			const_iterator;

      Subset(Set& s)
	: actionset_(s),
	  set_(s)
      {}

      Subset(ActionSet& as, Set& s)
	: actionset_(as),
	  set_(s)
      {}

      iterator	begin() {
	return m_iterator_::build(e_iterator_(set_.begin(),
					      set_, actionset_).skip());
      }

      iterator	end() {
	return m_iterator_::build(e_iterator_(set_.end(),
					      set_, actionset_));
      }

      std::pair<iterator, iterator>
      equal_range(const typename Set::value_type& x) {
	std::pair<set_iterator, set_iterator>	p = set_.equal_range(x);
	return std::pair<iterator, iterator>(m_iterator_::build(e_iterator_(p.first, set_, actionset_).skip()),
					     m_iterator_::build(e_iterator_(p.second, set_, actionset_).skip()));
      }
    private:
      ActionSet&	actionset_;
      Set&		set_;
    };

    template <class Set,
	      class Action, class Predicate, class ActionSet>
    class Subset<MSAdapt<Set>, Action, Predicate, ActionSet> {
      typedef Subset<MSAdapt<Set>, Action, Predicate, ActionSet>	This;
      typedef MaskIterator<MSAdapt<Set>, Action, Predicate, ActionSet>	e_iterator_;
      typedef DeconstIterator<e_iterator_>			m_iterator_;
    public:
      typedef typename Set::key_type	key_type;
      typedef typename Set::value_type	value_type;
      typedef typename Set::iterator	set_iterator;
      typedef e_iterator_		mask_iterator;
      typedef m_iterator_		iterator;
      typedef iterator			const_iterator;

      Subset(Set& s)
	: actionset_(s),
	  set_(s)
      {}

      Subset(ActionSet& as, Set& s)
	: actionset_(as),
	  set_(s)
      {}

      iterator	begin(const key_type& k) {
	return m_iterator_::build(e_iterator_(set_.begin(k),
					      set_, actionset_, k).skip());
      }

      iterator	end(const key_type& k) {
	return m_iterator_::build(e_iterator_(set_.end(k),
					      set_, actionset_, k));
      }

      void	set_stamp(const_iterator pos,
			  const TimeVal& now = TimeVal::now()) {
	set_.set_stamp(pos, now);
      }

      const TimeVal	stamp(const_iterator pos) const {
	return set_.stamp(pos);
      }

      bool	expired(const_iterator pos,
			const TimeVal& period,
			const TimeVal& now = TimeVal::now()) const {
	return set_.expired(pos, period, now);
      }
    private:
      ActionSet&	actionset_;
      Set&		set_;
    };

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_UTL_SET_HH
