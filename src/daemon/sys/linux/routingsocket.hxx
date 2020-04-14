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

# ifndef QOLYESTER_DAEMON_SYS_LINUX_ROUTINGSOCKET_HXX
#  define QOLYESTER_DAEMON_SYS_LINUX_ROUTINGSOCKET_HXX 1

#  include <net/if.h>
#  include <stdexcept>
#  include <sstream>

#  include "cst/constants.hh"
#  include "sys/realinterfaceinfo.hh"

#  include "routingsocket.hh"

namespace olsr {

  namespace sys {

#  ifdef DEBUG

    namespace netlink {

      RoutingTableVisitor::RoutingTableVisitor(std::ostream& os)
	: os_(os),
	  is_via_(false),
	  is_index_(false)
      {}

      RoutingTableVisitor::~RoutingTableVisitor() {}

      void
      RoutingTableVisitor::visit(const NLError& e) {
	os_ << "  Error: " << strerror(e.get_errno()) << " ("
	    << e.get_errno() << ")\n";
      }

      void
      RoutingTableVisitor::visit(const NLNewRoute& e) {

	for (NLNewRoute::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);

	char	name[IFNAMSIZ];

	os_ << "  " << dst_ << '/' << (unsigned) e.dlen();

	if (is_via_)
	  os_ << " via " << via_;

	if (is_index_)
	  os_ << " dev " << if_indextoname(index_, name);

	os_ << " proto ";
	switch (e.proto()) {
	case RTPROT_UNSPEC:
	  os_ << "unknown";
	  break;
	case RTPROT_REDIRECT:
	  os_ << "redirect";
	  break;
	case RTPROT_KERNEL:
	  os_ << "kernel";
	  break;
	case RTPROT_BOOT:
	  os_ << "boot";
	  break;
	case RTPROT_STATIC:
	  os_ << "static";
	  break;
	default:
	  os_ << (unsigned) e.proto();
	}
	os_ << " scope ";
	switch (e.scope()) {
	case RT_SCOPE_UNIVERSE:
	  os_ << "universe";
	  break;
	case RT_SCOPE_SITE:
	  os_ << "site";
	  break;
	case RT_SCOPE_LINK:
	  os_ << "link";
	  break;
	case RT_SCOPE_HOST:
	  os_ << "host";
	  break;
	case RT_SCOPE_NOWHERE:
	  os_ << "nowhere";
	  break;
	default:
	  os_ << (unsigned) e.scope();
	}
	os_ << " table ";
	switch (e.table()) {
	case RT_TABLE_UNSPEC:
	  os_ << "unspec";
	  break;
	case RT_TABLE_DEFAULT:
	  os_ << "default";
	  break;
	case RT_TABLE_MAIN:
	  os_ << "main";
	  break;
	case RT_TABLE_LOCAL:
	  os_ << "local";
	  break;
	default:
	  os_ << (unsigned) e.table();
	}
	os_ << '\n';
      }

      void
      RoutingTableVisitor::visit(const NLRouteAttrDestination& e) {
	dst_ = address_t(e.bytes(), e.length());
      }

      void
      RoutingTableVisitor::visit(const NLRouteAttrGateway& e) {
	via_ = address_t(e.bytes(), e.length());
	is_via_ = true;
      }

      void
      RoutingTableVisitor::visit(const NLRouteAttrOutInterface& e) {
	index_ = e.index();
	is_index_ = true;
      }

    } // namespace netlink

#  endif // !DEBUG

    namespace internal {

      void
      RoutingSocket::print_kernel_routes() {
#  ifdef DEBUG
	using netlink::NLSocket;
	using netlink::NLNewRoute;
	using netlink::NLGetRoute;
	using netlink::NLRouteAttrDestination;
	using netlink::NLAck;
	using netlink::NLError;

	// First, get the list of the routing table entries.

	NLGetRoute	m(address_t::family);

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() == 0)
	  throw std::runtime_error("print_kernel_routes(): "
				   "response type mismatch");

	debug << up << "Routing table {\n";

	for (NLSocket::answer_t::iterator im = resp.begin();
	     im != resp.end(); ++im) {

	  netlink::RoutingTableVisitor	rtv(debug);

	  (*im)->accept(rtv);

	}

	debug << '}' << std::endl << down;

	for (NLSocket::answer_t::iterator ix = resp.begin();
	     ix != resp.end(); ++ix)
	  delete *ix;
#  endif // !DEBUG
      }

      void
      RoutingSocket::remove_old_routes() {
	using netlink::NLSocket;
	using netlink::NLNewRoute;
	using netlink::NLGetRoute;
	using netlink::NLDelRoute;
	using netlink::NLRouteAttrDestination;
	using netlink::NLAck;
	using netlink::NLError;

	// First, get the list of the routing table entries.

	NLGetRoute	m(address_t::family);

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() == 0)
	  throw std::runtime_error("remove_old_routes(): "
				   "response type mismatch");

	for (NLSocket::answer_t::iterator im = resp.begin();
	     im != resp.end(); ++im) {

	  NLNewRoute*	nr = dynamic_cast<NLNewRoute*>(*im);

	  if (nr == 0) {
	    std::ostringstream	msg;
	    msg << "remove_old_routes(): ";
	    NLError*	e = dynamic_cast<NLError*>(*im);
	    if (e == 0) {
	      for (NLSocket::answer_t::iterator ix = resp.begin();
		   ix != resp.end(); ++ix)
		delete *ix;
	      msg << "unknown response";
	      throw std::runtime_error(msg.str());
	    }
	    msg << strerror(e->get_errno());
	    int err = e->get_errno();
	    for (NLSocket::answer_t::iterator ix = resp.begin();
		 ix != resp.end(); ++ix)
	      delete *ix;
	    throw errnoexcept_t(msg.str(), err);
	  }

	  if (nr->proto() != RTPROT_OLSR)
	    continue;

	  NLDelRoute	m2(nr->family(), nr->dlen(), nr->slen(),
			   nr->tos(), nr->table(), nr->proto(),
			   nr->scope(), nr->type(), nr->flags());

	  m2.attrs().swap(nr->attrs());

	  _s.send(m2);

	  NLSocket::answer_t	resp2 = _s.receive();

	  if (resp2.size() != 1) {
	    for (NLSocket::answer_t::iterator im = resp2.begin();
		 im != resp2.end(); ++im)
	      delete *im;
	    for (NLSocket::answer_t::iterator ix = resp.begin();
		 ix != resp.end(); ++ix)
	      delete *ix;
	    throw std::runtime_error("remove_old_routes(): "
				     "response size mismatch");
	  }
	  if (dynamic_cast<NLAck*>(*resp2.begin()) == 0) {
	    std::ostringstream	msg;
	    msg << "remove_old_routes(): ";
	    NLError*	e = dynamic_cast<NLError*>(*resp2.begin());
	    if (e == 0) {
	      delete *resp2.begin();
	      for (NLSocket::answer_t::iterator ix = resp.begin();
		   ix != resp.end(); ++ix)
		delete *ix;
	      msg << "unknown response";
	      throw std::runtime_error(msg.str());
	    }
	    msg << strerror(e->get_errno());
	    int err = e->get_errno();
	    delete *resp2.begin();
	    for (NLSocket::answer_t::iterator ix = resp.begin();
		 ix != resp.end(); ++ix)
	      delete *ix;
	    throw errnoexcept_t(msg.str(), err);
	  }
	  delete *resp2.begin();
	}

	for (NLSocket::answer_t::iterator ix = resp.begin();
	     ix != resp.end(); ++ix)
	  delete *ix;
      }

      void
      RoutingSocket::add_local_route(const address_t& d, unsigned p,
				     const sys::RealInterfaceInfo& i) {
	using netlink::NLSocket;
	using netlink::NLNewRoute;
	using netlink::NLRouteAttrDestination;
	using netlink::NLRouteAttrOutInterface;
	using netlink::NLAck;
	using netlink::NLError;

	NLNewRoute	m(address_t::family, p, 0, 0, RT_TABLE_MAIN,
			  RTPROT_OLSR, RT_SCOPE_LINK, RTN_UNICAST, 0);

	m.add_attr(new NLRouteAttrDestination(d.bytes(), ADDRESS_SIZE));
	m.add_attr(new NLRouteAttrOutInterface(i.index()));

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() != 1) {
	  for (NLSocket::answer_t::iterator im = resp.begin();
	       im != resp.end(); ++im)
	    delete *im;
	  std::ostringstream	msg;
	  msg << "add_local_route(): "
	      << d.to_string() << '/' << p << " dev " << i.name()
	      << " : response size mismatch";
	  throw std::runtime_error(msg.str());
	}
	if (dynamic_cast<NLAck*>(*resp.begin()) == 0) {
	  std::ostringstream	msg;
	  msg << "add_local_route(): ";
	  NLError*	e = dynamic_cast<NLError*>(*resp.begin());
	  if (e == 0) {
	    delete *resp.begin();
	    msg << d.to_string() << '/' << p << " dev " << i.name()
		<< " : unknown response";
	    throw std::runtime_error(msg.str());
	  }
	  msg << d.to_string() << '/' << p << " dev " << i.name()
	      << " : " << strerror(e->get_errno());
	  int	err = e->get_errno();
	  delete *resp.begin();
	  throw errnoexcept_t(msg.str(), err);
	}
	delete *resp.begin();
      }

      void
      RoutingSocket::add_remote_route(const address_t& d, unsigned p,
				      const address_t& n) {
	using netlink::NLSocket;
	using netlink::NLNewRoute;
	using netlink::NLRouteAttrDestination;
	using netlink::NLRouteAttrGateway;
	using netlink::NLAck;
	using netlink::NLError;

	NLNewRoute	m(address_t::family, p, 0, 0, RT_TABLE_MAIN,
			  RTPROT_OLSR, RT_SCOPE_UNIVERSE, RTN_UNICAST, 0);

	m.add_attr(new NLRouteAttrDestination(d.bytes(), ADDRESS_SIZE));
	m.add_attr(new NLRouteAttrGateway(n.bytes(), ADDRESS_SIZE));

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() != 1) {
	  for (NLSocket::answer_t::iterator im = resp.begin();
	       im != resp.end(); ++im)
	    delete *im;
	  std::ostringstream	msg;
	  msg << "add_remote_route(): "
	      << d.to_string() << '/' << p << " via " << n.to_string()
	      << " : response size mismatch";
	  throw std::runtime_error(msg.str());
	}
	if (dynamic_cast<NLAck*>(*resp.begin()) == 0) {
	  std::ostringstream	msg;
	  msg << "add_remote_route(): ";
	  NLError*	e = dynamic_cast<NLError*>(*resp.begin());
	  if (e == 0) {
	    delete *resp.begin();
	    msg << d.to_string() << '/' << p << " via " << n.to_string()
		<< " : unknown response";
	    throw std::runtime_error(msg.str());
	  }
	  msg << d.to_string() << '/' << p << " via " << n.to_string()
	      << " : " << strerror(e->get_errno());
	  int err = e->get_errno();
	  delete *resp.begin();
	  throw errnoexcept_t(msg.str(), err);
	}
	delete *resp.begin();
      }

      void
      RoutingSocket::remove_local_route(const address_t& d, unsigned p,
					const sys::RealInterfaceInfo& i) {
	using netlink::NLSocket;
	using netlink::NLDelRoute;
	using netlink::NLRouteAttrDestination;
	using netlink::NLRouteAttrOutInterface;
	using netlink::NLRouteAttrGateway;
	using netlink::NLAck;
	using netlink::NLError;

	NLDelRoute	m(address_t::family, p, 0, 0, RT_TABLE_UNSPEC,
			  RTPROT_OLSR, RT_SCOPE_LINK, RTN_UNICAST, 0);

	m.add_attr(new NLRouteAttrDestination(d.bytes(), ADDRESS_SIZE));
	m.add_attr(new NLRouteAttrOutInterface(i.index()));
	m.add_attr(new NLRouteAttrGateway(address_t().bytes(), ADDRESS_SIZE));

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() != 1) {
	  for (NLSocket::answer_t::iterator im = resp.begin();
	       im != resp.end(); ++im)
	    delete *im;
	  std::ostringstream	msg;
	  msg << "remove_local_route(): "
	      << d.to_string() << '/' << p << " dev " << i.name()
	      << " : response size mismatch";
	  throw std::runtime_error(msg.str());
	}
	if (dynamic_cast<NLAck*>(*resp.begin()) == 0) {
	  std::ostringstream	msg;
	  msg << "remove_local_route(): ";
	  NLError*	e = dynamic_cast<NLError*>(*resp.begin());
	  if (e == 0) {
	    delete *resp.begin();
	    msg << d.to_string() << '/' << p << " dev " << i.name()
		<< " : unknown response";
	    throw std::runtime_error(msg.str());
	  }
	  msg << d.to_string() << '/' << p << " dev " << i.name()
	      << " : " << strerror(e->get_errno());
	  int err = e->get_errno();
	  delete *resp.begin();
	  throw errnoexcept_t(msg.str(), err);
	}
	delete *resp.begin();
      }

      void
      RoutingSocket::remove_remote_route(const address_t& d, unsigned p,
					 const address_t& n) {
	using netlink::NLSocket;
	using netlink::NLDelRoute;
	using netlink::NLRouteAttrDestination;
	using netlink::NLRouteAttrGateway;
	using netlink::NLAck;
	using netlink::NLError;

	NLDelRoute	m(address_t::family, p, 0, 0, RT_TABLE_UNSPEC,
			  RTPROT_OLSR, RT_SCOPE_UNIVERSE, RTN_UNICAST, 0);

	m.add_attr(new NLRouteAttrDestination(d.bytes(), ADDRESS_SIZE));
	m.add_attr(new NLRouteAttrGateway(n.bytes(), ADDRESS_SIZE));

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() != 1) {
	  for (NLSocket::answer_t::iterator im = resp.begin();
	       im != resp.end(); ++im)
	    delete *im;
	  std::ostringstream	msg;
	  msg << "remove_remote_route(): "
	      << d.to_string() << '/' << p << " via " << n.to_string()
	      << " : response size mismatch";
	  throw std::runtime_error(msg.str());
	}
	if (dynamic_cast<NLAck*>(*resp.begin()) == 0) {
	  std::ostringstream	msg;
	  msg << "remove_remote_route(): ";
	  NLError*	e = dynamic_cast<NLError*>(*resp.begin());
	  if (e == 0) {
	    delete *resp.begin();
	    msg << d.to_string() << '/' << p << " via " << n.to_string()
		<< " : unknown response";
	    throw std::runtime_error(msg.str());
	  }
	  msg << d.to_string() << '/' << p << " via " << n.to_string()
	      << " : " << strerror(e->get_errno());
	  int err = e->get_errno();
	  delete *resp.begin();
	  throw errnoexcept_t(msg.str(), err);
	}
	delete *resp.begin();
      }

      void
      RoutingSocket::add_addr(unsigned i, const InterfaceAddress& a) {
	using netlink::NLSocket;
	using netlink::NLNewAddr;
	using netlink::NLAddrAttrAddress;
	using netlink::NLAddrAttrBroadcast;
	using netlink::NLAck;
	using netlink::NLError;

	NLNewAddr	m(address_t::family, a.prefix(), 0, a.scope(), i);

	m.add_attr(new NLAddrAttrAddress(a.address().bytes(),
					 ADDRESS_SIZE));
	m.add_attr(new NLAddrAttrBroadcast(a.broadcast().bytes(),
					   ADDRESS_SIZE));

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() != 1) {
	  for (NLSocket::answer_t::iterator im = resp.begin();
	       im != resp.end(); ++im)
	    delete *im;
	  std::ostringstream	msg;
	  msg << "add_addr(): "
	      << a.address() << '/' << a.prefix()
	      << " : response size mismatch";
	  throw std::runtime_error(msg.str());
	}
	if (dynamic_cast<NLAck*>(*resp.begin()) == 0) {
	  std::ostringstream	msg;
	  msg << "add_addr(): ";
	  NLError*	e = dynamic_cast<NLError*>(*resp.begin());
	  if (e == 0) {
	    delete *resp.begin();
	    msg << a.address() << '/' << a.prefix()
		<< " : unknown response";
	    throw std::runtime_error(msg.str());
	  }
	  msg << a.address() << '/' << a.prefix()
	      << " : " << strerror(e->get_errno());
	  int err = e->get_errno();
	  delete *resp.begin();
	  throw errnoexcept_t(msg.str(), err);
	}
	delete *resp.begin();
      }

      void
      RoutingSocket::del_addr(unsigned i, const InterfaceAddress& a) {
	using netlink::NLSocket;
	using netlink::NLDelAddr;
	using netlink::NLAddrAttrAddress;
	using netlink::NLAddrAttrBroadcast;
	using netlink::NLAck;
	using netlink::NLError;

	NLDelAddr	m(address_t::family, a.prefix(), 0, a.scope(), i);

	m.add_attr(new NLAddrAttrAddress(a.address().bytes(),
					 ADDRESS_SIZE));
	m.add_attr(new NLAddrAttrBroadcast(a.broadcast().bytes(),
					   ADDRESS_SIZE));

	_s.send(m);

	NLSocket::answer_t	resp = _s.receive();

	if (resp.size() != 1) {
	  for (NLSocket::answer_t::iterator im = resp.begin();
	       im != resp.end(); ++im)
	    delete *im;
	  std::ostringstream	msg;
	  msg << "del_addr(): "
	      << a.address() << '/' << a.prefix()
	      << " : response size mismatch";
	  throw std::runtime_error(msg.str());
	}
	if (dynamic_cast<NLAck*>(*resp.begin()) == 0) {
	  std::ostringstream	msg;
	  msg << "del_addr(): ";
	  NLError*	e = dynamic_cast<NLError*>(*resp.begin());
	  if (e == 0) {
	    delete *resp.begin();
	    msg << a.address() << '/' << a.prefix()
		<< " : unknown response";
	    throw std::runtime_error(msg.str());
	  }
	  msg << a.address() << '/' << a.prefix()
	      << " : " << strerror(e->get_errno());
	  int err = e->get_errno();
	  delete *resp.begin();
	  throw errnoexcept_t(msg.str(), err);
	}
	delete *resp.begin();
      }

    } // namespace internal

  } // namespace sys

} // namespace olsr

# endif // ! QOLYESTER_DAEMON_SYS_LINUX_ROUTINGSOCKET_HXX

#endif // ! QOLYESTER_ENABLE_VIRTUAL
