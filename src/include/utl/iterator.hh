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

#ifndef QOLYESTER_UTL_ITERATOR_HH
# define QOLYESTER_UTL_ITERATOR_HH 1

# include <cassert>
# include <iterator>

namespace olsr {

  namespace utl {

    // Some generic functors

    template <class Set, class Iter = typename Set::iterator>
    struct DefaultPredicate {
      bool operator()(Set&, const Iter& pos) const {
	return pos->is_valid();
      }
    };
    
//     template <class Elem>
//     struct DefaultPredicate {
//       bool operator()(const Elem& e) const {
// 	return e.is_valid();
//       }
//       bool operator()(const Elem* e) const {
// 	return e->is_valid();
//       }
//       bool operator()(Elem& e) const {
// 	return e.is_valid();
//       }
//       bool operator()(Elem* e) const {
// 	return e->is_valid();
//       }
//     };

    template <class Set, class Iterator = typename Set::iterator>
    struct NoAction {
      void operator()(Set&, const Iterator&) const {}
    };

//     template <class Set, class Iterator = typename Set::iterator>
//     struct DefaultEraser {
//       void operator()(Set& s, const Iterator& pos) const {
// 	s.erase(pos);
//       }
//     };

    template <class T>
    struct type_traits {
      typedef T		mutable_val;
      typedef const T	const_val;
      typedef T&	mutable_ref;
      typedef const T&	const_ref;
      typedef T*	mutable_ptr;
      typedef const T*	const_ptr;
    };

    template <class T>
    struct type_traits<const T> {
      typedef T		mutable_val;
      typedef const T	const_val;
      typedef T&	mutable_ref;
      typedef const T&	const_ref;
      typedef T*	mutable_ptr;
      typedef const T*	const_ptr;
    };

    template <typename T>
    struct type_traits<T&> {
      typedef typename type_traits<T>::mutable_val	mutable_val;
      typedef typename type_traits<T>::const_val	const_val;
      typedef typename type_traits<T>::mutable_ref	mutable_ref;
      typedef typename type_traits<T>::const_ref	const_ref;
      typedef typename type_traits<T>::mutable_ptr	mutable_ptr;
      typedef typename type_traits<T>::const_ptr	const_ptr;
    };

    template <typename T>
    struct type_traits<T*> {
      typedef typename type_traits<T>::mutable_val	mutable_val;
      typedef typename type_traits<T>::const_val	const_val;
      typedef typename type_traits<T>::mutable_ref	mutable_ref;
      typedef typename type_traits<T>::const_ref	const_ref;
      typedef typename type_traits<T>::mutable_ptr	mutable_ptr;
      typedef typename type_traits<T>::const_ptr	const_ptr;
    };

    template <class Iter>
    class DeconstIterator : public Iter {
      typedef DeconstIterator<Iter>				This;
      typedef Iter						Super;
      typedef typename std::iterator_traits<Iter>::value_type	Elem;
      typedef typename type_traits<Elem>::mutable_ref		MRef;
      typedef typename type_traits<Elem>::mutable_ptr		MPtr;
    public:
      DeconstIterator() : Super() {}
      DeconstIterator(const This& other) : Super(other) {}

      MRef	operator*() const {
	return const_cast<MRef>(*static_cast<const Super&>(*this));
      }

      MPtr	operator->() const {
	return const_cast<MPtr>(&*static_cast<const Super&>(*this));
      }

      This&	operator++() {
	++static_cast<Super&>(*this);
	return *this;
      }

      This	operator++(int) {
	return This::build(static_cast<Super&>(*this)++);
      }

      static This	build(const Super& other) {
	return This(static_cast<const This&>(other));
      }

      static std::pair<This, bool>	build(const std::pair<Super, bool>& p) {
	return std::pair<This, bool>(This(static_cast<const This&>(p.first)),
				     p.second);
      }
    };

    template <class Iter>
    class DerefIterator : public Iter {
      typedef DerefIterator	This;
      typedef Iter		Super;
      typedef typename std::iterator_traits<Iter>::value_type	Elem;
      typedef typename std::iterator_traits<Elem>::value_type	Value;
      typedef typename std::iterator_traits<Elem>::reference	Ref;
      typedef typename std::iterator_traits<Elem>::pointer	Ptr;
    public:
      DerefIterator() : Super() {}
      DerefIterator(const This& other) : Super(other) {}

      Ref operator*() const {
	return **static_cast<const Super&>(*this);
      }

      Ptr operator->() const {
	return &**static_cast<const Super&>(*this);
      }

      This&	operator++() {
	++static_cast<Super&>(*this);
	return *this;
      }

      This	operator++(int) {
	return This::build(static_cast<Super&>(*this)++);
      }

      const Super&	deref_super() const { return *this; }
      Super&		deref_super() { return *this; }

      static This	build(const Super& other) {
	return This(static_cast<const This&>(other));
      }
    };

    template <class Key, class Iter>
    class KeyedIterator : public Iter {
      typedef KeyedIterator<Key, Iter>	This;
      typedef Iter			Super;
    public:
      KeyedIterator(const Super& other, const Key& k)
	: Super(other), 
	  _key(k)
      {}

      KeyedIterator(const This& other)
	: Super(other),
	  _key(other._key)
      {}
      
      const Key&	key() const {
	return _key;
      }
    private:
      Key	_key;
    };

//     template <class Set, class Iter = typename Set::iterator>
//     struct EndPredicate {
//       static bool check(const Iter& pos, const Set& instance) {
// 	return !(pos != instance.end());
//       }
//       static bool check(const Iter& pos, Set& instance) {
// 	return !(pos != instance.end());
//       }
//     };

//     template <class Set, class Key, class Iter>
//     struct EndPredicate<Set, KeyedIterator<Key, Iter> > {
//       static bool check(const KeyedIterator<Key, Iter>& pos,
// 			const Set& instance) {
// 	return !(pos != instance.end(pos.key()));
//       }
//     };

    template <class Set,
	      class Action = NoAction<Set>,
	      class Predicate = DefaultPredicate<Set>,
	      class ActionSet = Set>
    class MaskIterator {
      typedef typename Set::iterator				Super;
      typedef typename std::iterator_traits<Super>::value_type	Elem;
      typedef MaskIterator<Set, Action, Predicate, ActionSet>	This;
    public:
      operator Super() const {
	return _super;
      }

      const Super&	mask_super() const { return _super; }
      Super&		mask_super() { return _super; }

      MaskIterator()
	: _super(),
	  _instance(0),
	  _action_instance(0)
      {}

      MaskIterator(const Super& other, Set& instance)
	: _super(other),
	  _instance(&instance),
	  _action_instance(&instance)
      {}

      MaskIterator(const Super& other, Set& instance, ActionSet& ainstance)
	: _super(other),
	  _instance(&instance),
	  _action_instance(&ainstance)
      {}

      const Elem&	operator*() const {
	return *_super;
      }

      const Elem*	operator->() const {
	return &*_super;
      }

      This& operator++() {
	++_super;
	skip();
	return *this;
      }

      This operator++(int) {
	This	tmp(*this);
	operator++();
	return tmp;
      }

      bool operator==(const This& rhs) const {
	return _super == rhs._super;
      }

      bool operator!=(const This& rhs) const {
	return _super != rhs._super;
      }

      bool	is_valid() const {
//  	return EndPredicate<Set, Super>::check(_super, *_instance) ||
 	return _super == _instance->end() ||
	Predicate()(*_action_instance, _super);
      }

      This& skip() {
	while (!is_valid()) {
	  This	tmp(*this);
	  ++_super;
	  Action()(*_action_instance, tmp._super);
	}
	return *this;
      }

      void act() {
	Action()(*_action_instance, _super);
	_super = _instance->end();
      }

      This& check() {
	assert(_instance != 0);
	if (!is_valid())
	  act();
	return *this;
      }

    private:
      Super		_super;
      Set*		_instance;
      ActionSet*	_action_instance;
    };

    template <class> struct MSAdapt;

    template <class Set, class Action, class Predicate, class ActionSet>
    class MaskIterator<MSAdapt<Set>, Action, Predicate, ActionSet> {
      typedef typename Set::iterator				Super;
      typedef typename std::iterator_traits<Super>::value_type	Elem;
      typedef typename Set::key_type				Key;
      typedef MaskIterator<MSAdapt<Set>, Action, Predicate, ActionSet>	This;
    public:
      operator Super() const {
	return _super;
      }

      const Super&	mask_super() const { return _super; }
      Super&		mask_super() { return _super; }

      MaskIterator()
	: _super(),
	  _instance(0),
	  _action_instance(0),
	  _key()
      {}

      MaskIterator(const Super& other, Set& instance, const Key& key)
	: _super(other),
	  _instance(&instance),
	  _action_instance(&instance),
	  _key(key)
      {}

      MaskIterator(const Super& other, Set& instance,
		   ActionSet& ainstance, const Key& key)
	: _super(other),
	  _instance(&instance),
	  _action_instance(&ainstance),
	  _key(key)
      {}

      const Elem&	operator*() const {
	return *_super;
      }

      const Elem*	operator->() const {
	return &*_super;
      }

      This& operator++() {
	++_super;
	skip();
	return *this;
      }

      This operator++(int) {
	This	tmp(*this);
	operator++();
	return tmp;
      }

      bool operator==(const This& rhs) const {
	return _super == rhs._super;
      }

      bool operator!=(const This& rhs) const {
	return _super != rhs._super;
      }

      bool	is_valid() const {
//  	return EndPredicate<Set, Super>::check(_super, *_instance) ||
	return _super == _instance->end(_key) || Predicate()(*_action_instance,
							     _super);
      }

      This& skip() {
	while (!is_valid()) {
	  This	tmp(*this);
	  ++_super;
	  Action()(*_action_instance, tmp._super);
	}
	return *this;
      }

      void destroy() {
	Action()(*_action_instance, _super);
	_super = _instance->end(_key);
      }

      This& check() {
	assert(_instance != 0);
	if (!is_valid())
	  destroy();
	return *this;
      }

    private:
      Super	_super;
      Set*	_instance;
      ActionSet*	_action_instance;
      Key	_key;
    };

  } // namespace utl

} // namespace olsr

namespace std {
  
  template <class Iter>
  struct iterator_traits<olsr::utl::DeconstIterator<Iter> > {
    typedef typename iterator_traits<Iter>::iterator_category
							iterator_category;
    typedef typename olsr::utl::type_traits<typename iterator_traits<Iter>::value_type>::mutable_val
							value_type;
    typedef typename iterator_traits<Iter>::difference_type
							difference_type;
    typedef typename olsr::utl::type_traits<typename iterator_traits<Iter>::reference>::mutable_ref
							reference;
    typedef typename olsr::utl::type_traits<typename iterator_traits<Iter>::pointer>::mutable_ptr
							pointer;
  };

  template <class Iter>
  struct iterator_traits<olsr::utl::DerefIterator<Iter> > {
    typedef typename iterator_traits<Iter>::iterator_category
							iterator_category;
    typedef typename iterator_traits<Iter>::difference_type
							difference_type;
    typedef typename iterator_traits<typename iterator_traits<Iter>::value_type>::value_type	value_type;
    typedef typename iterator_traits<typename iterator_traits<Iter>::value_type>::reference	reference;
    typedef typename iterator_traits<typename iterator_traits<Iter>::value_type>::pointer	pointer;
  };

  template <class Key, class Iter>
  struct iterator_traits<olsr::utl::KeyedIterator<Key, Iter> > {
    typedef typename iterator_traits<Iter>::iterator_category
							iterator_category;
    typedef typename iterator_traits<Iter>::difference_type
							difference_type;
    typedef typename iterator_traits<Iter>::value_type	value_type;
    typedef typename iterator_traits<Iter>::reference	reference;
    typedef typename iterator_traits<Iter>::pointer	pointer;
  };

  template <class Set, class Action, class Predicate, class ActionSet>
  struct iterator_traits<olsr::utl::MaskIterator<Set, Action, Predicate, ActionSet> > {
    typedef forward_iterator_tag			iterator_category;
    typedef typename iterator_traits<typename Set::iterator>::difference_type
							difference_type;
    typedef typename iterator_traits<typename Set::iterator>::value_type
							value_type;
    typedef typename iterator_traits<typename Set::iterator>::reference
							reference;
    typedef typename iterator_traits<typename Set::iterator>::pointer
							pointer;
  };

  template <class Set, class Action, class Predicate, class ActionSet>
  struct iterator_traits<olsr::utl::MaskIterator<olsr::utl::MSAdapt<Set>, Action, Predicate, ActionSet> > {
    typedef forward_iterator_tag			iterator_category;
    typedef typename iterator_traits<typename Set::iterator>::difference_type
							difference_type;
    typedef typename iterator_traits<typename Set::iterator>::value_type
							value_type;
    typedef typename iterator_traits<typename Set::iterator>::reference
							reference;
    typedef typename iterator_traits<typename Set::iterator>::pointer
							pointer;
  };

} // namespace std

#endif // ! QOLYESTER_UTL_ITERATOR_HH
