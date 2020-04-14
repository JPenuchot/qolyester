// Copyright (C) 2003-2006 Laboratoire de Recherche en Informatique

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

#include "config.hh"

#ifndef QOLYESTER_ENABLE_VIRTUAL

# ifndef QOLYESTER_DAEMON_SYS_LINUX_INTERFACEDESC_HXX
#  define QOLYESTER_DAEMON_SYS_LINUX_INTERFACEDESC_HXX 1

#  include <cstring>
#  include <cassert>

#  include "utl/exception.hh"
#  include "utl/log.hh"

#  include "interfacedesc.hh"

namespace olsr {

  extern debug_ostream_t	debug;

  namespace sys {

    namespace internal {

      InterfaceAddress::InterfaceAddress() :
	_prefix(0),
	_scope(0),
	_address(),
	_broadcast()
      {}

      InterfaceAddress::InterfaceAddress(unsigned p,
					 unsigned char s,
					 const address_t& a,
					 const address_t& b) :
	_prefix(p),
	_scope(s),
	_address(a),
	_broadcast(b)
      {}

      bool
      InterfaceAddress::operator<(const This& rhs) const {
	return _address < rhs._address;
      }

      InterfaceAddress
      InterfaceAddress::make_key(const address_t& a) {
	This	tmp = _dummy_for_find;

	const_cast<address_t&>(tmp._address) = a;
	return tmp;
      }

    } // namespace internal

    namespace netlink {

#  ifdef DEBUG
      DebugVisitor::DebugVisitor(std::ostream& os) : _os(os) {
	_os << "DebugVisitor {\n";
      }

      DebugVisitor::~DebugVisitor() {
	_os << '}' << std::endl;
      }

      void
      DebugVisitor::visit(const NLError& e) {
	_os << "  Error: " << strerror(e.get_errno()) << " ("
	    << e.get_errno() << ")\n";
      }

      void
      DebugVisitor::visit(const NLNewLink& e) {
	_os << "  NewLink {\n"
	    << "    index = " << e.index() << '\n'
	    << "    attrs {\n";

	for (NLNewLink::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);

	_os << "    }\n"
	    << "  }\n";
      }

      void
      DebugVisitor::visit(const NLLinkAttrName& e) {
	_os << "      name = \"" << e.name() << "\"\n";
      }

      void
      DebugVisitor::visit(const NLLinkAttrMTU& e) {
	_os << "      MTU = " << e.mtu() << '\n';
      }

      void
      DebugVisitor::visit(const NLNewAddr& e) {
	if (e.family() != address_t::family)
	  return;

	_os << "  NLNewAddr {\n"
	    << "    family = " << (int) e.family() << '\n'
	    << "    index = " << e.index() << '\n'
	    << "    prefix = " << (int) e.prefixlen() << '\n'
	    << "    scope = " << (int) e.scope() << '\n'
	    << "    attrs {\n";

	for (NLNewAddr::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);

	_os << "    }\n"
	    << "  }\n";
      }

      void
      DebugVisitor::visit(const NLAddrAttrAddress& e) {
	_os << "      address = " << address_t(e.bytes(), e.length()) << '\n';
      }

      void
      DebugVisitor::visit(const NLAddrAttrBroadcast& e) {
	_os << "      broadcast = " << address_t(e.bytes(), e.length())
	    << '\n';
      }
#  endif // !DEBUG

      InitVisitor::InitVisitor(std::string& name,
			       unsigned& index,
			       unsigned& mtu,
			       addrs_t& addrs)
	: _name(name),
	  _index(index),
	  _mtu(mtu),
	  _addrs(addrs),
	  _prefix(0),
	  _scope(0),
	  _address(),
	  _broadcast()
      {}

      void
      InitVisitor::visit(const NLError& e) {
	throw errnoexcept_t(strerror(e.get_errno()), e.get_errno());
      }

      void
      InitVisitor::visit(const NLNewLink& e) {
	_index = e.index();

	for (NLNewLink::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);

	assert(!_name.empty());
	assert(_index != 0);
// 	assert(_mtu != 0);
      }

      void
      InitVisitor::visit(const NLLinkAttrName& e) {
	_name = e.name();
      }

      void
      InitVisitor::visit(const NLLinkAttrMTU& e) {
	_mtu = e.mtu();
      }

      void
      InitVisitor::visit(const NLNewAddr& e) {
	if (e.family() != address_t::family ||
	    e.index() != _index)
	  return;

	_prefix = e.prefixlen();
	_scope  = e.scope();

	for (NLNewAddr::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);

	assert(_address != address_t());

	_addrs.insert(addr_t(_prefix, _scope, _address, _broadcast));

	_prefix = 0;
	_scope = 0;
	_address = address_t();
	_broadcast = address_t();
      }

      void
      InitVisitor::visit(const NLAddrAttrAddress& e) {
	_address = address_t(e.bytes(), e.length());
      }

      void
      InitVisitor::visit(const NLAddrAttrBroadcast& e) {
	_broadcast = address_t(e.bytes(), e.length());
      }

    } // namespace netlink

    namespace internal {

      InterfaceInfo::InterfaceInfo(const std::string& name) :
	_name(),
	_index(0),
	_mtu(0),
	_addrs() {
#  ifdef DEBUG
	using netlink::DebugVisitor;
#  endif // !DEBUG
	using netlink::InitVisitor;
	using netlink::NLMessage;
	using netlink::NLSocket;
	using netlink::NLGetLink;
	using netlink::NLGetAddr;

	InitVisitor	v(_name, _index, _mtu, _addrs);

	NLSocket	s;

	s.send(NLGetLink());

        NLSocket::answer_t	li = s.receive();

	bool	found = false;

	while (!li.empty()) {
	  NLMessage*	m = li.front();
	  li.pop_front();

#  ifdef DEBUG
	  {
	    DebugVisitor	dv(debug);
	    m->accept(dv);
	  }
#  endif // !DEBUG

	  if (!found) {
	    m->accept(v);

	    if (_name == name)
	      found = true;
	  }

	  delete m;
	}

	if (!found)
	  throw std::runtime_error(name + " is not an interface");

	s.send(NLGetAddr(address_t::family));

	li = s.receive();

	while (!li.empty()) {
	  NLMessage*	m = li.front();
	  li.pop_front();
#  ifdef DEBUG
	  {
	    DebugVisitor	dv(debug);
	    m->accept(dv);
	  }
#  endif // !DEBUG
	  m->accept(v);
	  delete m;
	}
      }

      bool
      InterfaceInfo::operator<(const This& rhs) const {
	return _index < rhs._index;
      }

    } // namespace internal

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_SYS_LINUX_INTERFACEDESC_HXX

#endif // ! QOLYESTER_ENABLE_VIRTUAL
