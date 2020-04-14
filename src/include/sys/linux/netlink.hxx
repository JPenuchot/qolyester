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
// Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_SYS_LINUX_NETLINK_HXX
# define QOLYESTER_SYS_LINUX_NETLINK_HXX 1

# include <cstring>
# include <asm/types.h>
# include <sys/socket.h>
# include <linux/rtnetlink.h>
# include <linux/pkt_sched.h>
# include <stdexcept>
# include <cerrno>

# include "netlink.hh"

namespace olsr {

  namespace sys {

    namespace netlink {

      RequestVisitor::RequestVisitor()
	: _buffers(),
	  _buffer(0),
	  _length(0)
      {}

      RequestVisitor::~RequestVisitor() {
	for (buffers_t::iterator i = _buffers.begin();
	     i != _buffers.end();
	     ++i)
	  delete[] (char*) i->iov_base;
	if (_buffer != 0)
	  delete[] _buffer;
      }

# define PREPARE_MESSAGE(Payload, Name)				\
      nlmsghdr*	nh;						\
      Payload *	Name ;						\
      int	len = NLMSG_SPACE(sizeof ( Payload ));		\
      char*	r = new char[len];				\
      memset(r, 0, len);					\
								\
      nh = reinterpret_cast<nlmsghdr*>(r);			\
      Name = reinterpret_cast< Payload *>(NLMSG_DATA(nh));	\
								\
      nh->nlmsg_len = NLMSG_LENGTH(sizeof ( Payload ));		\
      nh->nlmsg_seq = RequestVisitor::seqnum++;			\
      nh->nlmsg_pid = 0;

# define PREPARE_MESSAGE_GET(Payload, Name)		\
      PREPARE_MESSAGE(Payload, Name);			\
      nh->nlmsg_flags = NLM_F_ROOT | NLM_F_REQUEST

# define PREPARE_MESSAGE_NEW(Payload, Name)		\
      PREPARE_MESSAGE(Payload, Name);			\
      nh->nlmsg_flags = NLM_F_CREATE | NLM_F_EXCL | 	\
                        NLM_F_REQUEST | NLM_F_ACK

# define PREPARE_MESSAGE_DEL(Payload, Name)		\
      PREPARE_MESSAGE(Payload, Name);			\
      nh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK

# define SHIPOUT_MESSAGE			\
      iovec	i = { r, len };			\
      _buffers.push_back(i)

      void
      RequestVisitor::visit(const NLGetLink&) {
	PREPARE_MESSAGE_GET(ifinfomsg, ii);

	nh->nlmsg_type  = RTM_GETLINK;

	ii->ifi_family  = AF_UNSPEC;

	SHIPOUT_MESSAGE;
      }

      void
      RequestVisitor::visit(const NLNewAddr& e) {
	PREPARE_MESSAGE_NEW(ifaddrmsg, ia);

	nh->nlmsg_type  = RTM_NEWADDR;

	ia->ifa_family    = e.family();
	ia->ifa_prefixlen = e.prefixlen();
	ia->ifa_flags     = e.flags();
	ia->ifa_scope     = e.scope();
	ia->ifa_index     = e.index();

	SHIPOUT_MESSAGE;

	for (NLNewAddr::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);
      }

      void
      RequestVisitor::visit(const NLGetAddr& e) {
	PREPARE_MESSAGE_GET(ifaddrmsg, ia);

	nh->nlmsg_type  = RTM_GETADDR;

	ia->ifa_family  = e.family();

	SHIPOUT_MESSAGE;
      }

      void
      RequestVisitor::visit(const NLDelAddr& e) {
	PREPARE_MESSAGE_DEL(ifaddrmsg, ia);

	nh->nlmsg_type  = RTM_DELADDR;

	ia->ifa_family    = e.family();
	ia->ifa_prefixlen = e.prefixlen();
	ia->ifa_flags     = e.flags();
	ia->ifa_scope     = e.scope();
	ia->ifa_index     = e.index();

	SHIPOUT_MESSAGE;

	for (NLDelAddr::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);
      }

      void
      RequestVisitor::visit(const NLGetRoute& e) {
	PREPARE_MESSAGE_GET(rtmsg, rt);

	nh->nlmsg_type  = RTM_GETROUTE;

	rt->rtm_family  = e.family();

	SHIPOUT_MESSAGE;
      }

      void
      RequestVisitor::visit(const NLNewRoute& e) {
	PREPARE_MESSAGE_NEW(rtmsg, rt);

	nh->nlmsg_type  = RTM_NEWROUTE;

	rt->rtm_family   = e.family();
	rt->rtm_dst_len  = e.dlen();
	rt->rtm_src_len  = e.slen();
	rt->rtm_tos      = e.tos();
	rt->rtm_table    = e.table();
	rt->rtm_protocol = e.proto();
	rt->rtm_scope    = e.scope();
	rt->rtm_type     = e.type();
	rt->rtm_flags    = e.flags();

	SHIPOUT_MESSAGE;

	for (NLNewRoute::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);
      }

      void
      RequestVisitor::visit(const NLDelRoute& e) {
	PREPARE_MESSAGE_DEL(rtmsg, rt);

	nh->nlmsg_type  = RTM_DELROUTE;

	rt->rtm_family   = e.family();
	rt->rtm_dst_len  = e.dlen();
	rt->rtm_src_len  = e.slen();
	rt->rtm_tos      = e.tos();
	rt->rtm_table    = e.table();
	rt->rtm_protocol = e.proto();
	rt->rtm_scope    = e.scope();
	rt->rtm_type     = e.type();
	rt->rtm_flags    = e.flags();

	SHIPOUT_MESSAGE;

	for (NLDelRoute::attrs_t::const_iterator i = e.attrs().begin();
	     i != e.attrs().end(); ++i)
	  (*i)->accept(*this);
      }

      void
      RequestVisitor::visit(const NLAddrAttrAddress& e) {
	unsigned	len = RTA_SPACE(e.length());
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(e.length());
	a->rta_type = IFA_ADDRESS;
	memcpy(RTA_DATA(a), e.bytes(), e.length());

	iovec		i = { buffer, len };

	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

      void
      RequestVisitor::visit(const NLAddrAttrLocal& e) {
	unsigned	len = RTA_SPACE(e.length());
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(e.length());
	a->rta_type = IFA_LOCAL;
	memcpy(RTA_DATA(a), e.bytes(), e.length());

	iovec		i = { buffer, len };

	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

//       void
//       RequestVisitor::visit(const NLAddrAttrLabel& e) {
// 	unsigned	len = RTA_SPACE(e.label().length() + 1);
// 	char*		buffer = new char[len];
// 	memset(buffer, 0, len);

// 	rtattr*		a = (rtattr*) buffer;
// 	a->rta_len  = RTA_LENGTH(e.label().length() + 1);
// 	a->rta_type = IFA_LABEL;
// 	e.label().copy(RTA_DATA(a), e.label().length());

// 	iovec		i = { (void*) buffer, len };

// 	_buffers.push_back(i);

// 	nlmsghdr*	h =
// 	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

// 	h->nlmsg_len += NLMSG_ALIGN(len);
//       }

      void
      RequestVisitor::visit(const NLAddrAttrBroadcast& e) {
	unsigned	len = RTA_SPACE(e.length());
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(e.length());
	a->rta_type = IFA_BROADCAST;
	memcpy(RTA_DATA(a), e.bytes(), e.length());

	iovec		i = { buffer, len };

	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

      void
      RequestVisitor::visit(const NLAddrAttrAnycast& e) {
	unsigned	len = RTA_SPACE(e.length());
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(e.length());
	a->rta_type = IFA_ANYCAST;
	memcpy(RTA_DATA(a), e.bytes(), e.length());

	iovec		i = { buffer, len };

	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

      void
      RequestVisitor::visit(const NLRouteAttrDestination& e) {
	unsigned	len = RTA_SPACE(e.length());
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(e.length());
	a->rta_type = RTA_DST;
	memcpy(RTA_DATA(a), e.bytes(), e.length());

	iovec		i = { buffer, len };
	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

      void
      RequestVisitor::visit(const NLRouteAttrGateway& e) {
	unsigned	len = RTA_SPACE(e.length());
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(e.length());
	a->rta_type = RTA_GATEWAY;
	memcpy(RTA_DATA(a), e.bytes(), e.length());

	iovec		i = { buffer, len };
	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

      void
      RequestVisitor::visit(const NLRouteAttrOutInterface& e) {
	unsigned	len = RTA_SPACE(sizeof (int));
	char*		buffer = new char[len];
	memset(buffer, 0, len);

	rtattr*		a = reinterpret_cast<rtattr*>(buffer);
	a->rta_len  = RTA_LENGTH(sizeof (int));
	a->rta_type = RTA_OIF;
	*(int*)RTA_DATA(a) = e.index();

	iovec		i = { buffer, len };
	_buffers.push_back(i);

	nlmsghdr*	h =
	  reinterpret_cast<nlmsghdr*>(_buffers.begin()->iov_base);

	h->nlmsg_len += NLMSG_ALIGN(len);
      }

      const char*
      RequestVisitor::buffer() {
	if (_buffer != 0)
	  return _buffer;

	_length = totalsize();
	_buffer = new char[_length];
	memset(_buffer, 0, _length);

	unsigned index = 0;
	for (buffers_t::const_iterator i = _buffers.begin();
	     i != _buffers.end(); ++i) {
	  memcpy(_buffer + index, i->iov_base, i->iov_len);
	  index += NLMSG_ALIGN(i->iov_len);
	}
	return _buffer;
      }

      unsigned
      RequestVisitor::length() {
	if (_buffer == 0)
	  buffer();
	return _length;
      }

      unsigned
      RequestVisitor::totalsize() const {
	unsigned size = 0;
	for (buffers_t::const_iterator i = _buffers.begin();
	     i != _buffers.end(); ++i)
	  size += NLMSG_ALIGN(i->iov_len);
	return size;
      }

      unsigned RequestVisitor::seqnum = time(NULL);

      NLSocket::NLSocket()
	: _fd(socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) {
	if (_fd < 0)
	  throw std::runtime_error(strerror(errno));
	sockaddr_t	snl;

	snl.nl_family = AF_NETLINK;
	snl.nl_pad    = 0;
	snl.nl_pid    = 0;
	snl.nl_groups = 0;

	if (bind(_fd, (sockaddr*) &snl, sizeof snl) < 0)
	  throw std::runtime_error(strerror(errno));
      }

      NLSocket::~NLSocket() {
	close(_fd);
      }

      void
      NLSocket::send(const NLMessage& m) {
	RequestVisitor	r;

	m.accept(r);



	int ret = ::send(_fd, r.buffer(), r.length(), 0);

	if (ret < 0 /* || (unsigned) ret != r.length()*/)
	  throw std::runtime_error(strerror(errno));
      }

      void
      NLSocket::do_receive(char*& buffer, unsigned& length) {
	int ret;

	do {
	  if (buffer != 0)
	    delete[] buffer;

	  length *= 2;
	  buffer = new char[length];

	  ret = recv(_fd, buffer, length, MSG_PEEK);

	  if (ret < 0)
	    throw std::runtime_error(strerror(errno));

	} while ((unsigned) ret == length);

	ret = recv(_fd, buffer, length, 0);

	if (ret < 0)
	  throw std::runtime_error(strerror(errno));

	assert((unsigned) ret < length);

	length = ret;
      }

      std::list<NLMessage*>
      NLSocket::receive() {
	char*			buffer = 0;
	unsigned		length = 2048;
	unsigned		size = 0;
	bool			done = true;
	std::list<NLMessage*>	output;

	nlmsghdr*	nh = 0;

	try {
	  do {

	    try {
	      do_receive(buffer, length);
	    } catch (std::exception& e) {
	      if (buffer != 0) {
		delete[] buffer;
		buffer = 0;
	      }
	      throw;
	    }

	    size = length;

	    for (nh = (nlmsghdr*) buffer;
		 size > 0;
		 nh = NLMSG_NEXT(nh, size)) {

	      if (!NLMSG_OK(nh, size))
		throw std::runtime_error("Netlink message truncated");

	      if (nh->nlmsg_flags & NLM_F_MULTI)
		done = false;

	      switch (nh->nlmsg_type) {
	      case NLMSG_ERROR: {
		nlmsgerr*		me = (nlmsgerr*) NLMSG_DATA(nh);
		if (me->error == 0)
		  output.push_back(new NLAck);
		else
		  output.push_back(new NLError(-me->error));
	      }
		break;
	      case NLMSG_DONE:
		done = true;
		break;
	      case RTM_NEWLINK: {
		// FIXME: The following line is a dirty hack to
		// workaround a kernel bug fixed in 2.6.13-rc1.  This
		// should go away as soon as possible.
		done = false;

		ifinfomsg*	ih = (ifinfomsg*) NLMSG_DATA(nh);
		unsigned	size = IFLA_PAYLOAD(nh);

		NLNewLink*	m = new NLNewLink(ih->ifi_family,
						  ih->ifi_type,
						  ih->ifi_index,
						  ih->ifi_flags);

		for (rtattr* rt = IFLA_RTA(ih);
		     size > 0;
		     rt = RTA_NEXT(rt, size)) {

		  if (!RTA_OK(rt, size)) {
		    delete m;
		    throw std::runtime_error("RTA truncated");
		  }

		  switch (rt->rta_type) {
		  case IFLA_IFNAME:
		    m->add_attr(new NLLinkAttrName((char*) RTA_DATA(rt)));
		    break;
		  case IFLA_MTU:
		    m->add_attr(new NLLinkAttrMTU(*(unsigned*)RTA_DATA(rt)));
		    break;
		  }
		}

		output.push_back(m);
	      }
		break;

	      case RTM_NEWADDR: {
		// FIXME: The following line is a dirty hack to
		// workaround a kernel bug fixed in 2.6.13-rc1.  This
		// should go away as soon as possible.
		done = false;

		ifaddrmsg*	ia = (ifaddrmsg*) NLMSG_DATA(nh);
		unsigned	size = IFA_PAYLOAD(nh);

		NLNewAddr*	m = new NLNewAddr(ia->ifa_family,
						  ia->ifa_prefixlen,
						  ia->ifa_flags,
						  ia->ifa_scope,
						  ia->ifa_index);

		for (rtattr* rt = IFA_RTA(ia);
		     size > 0;
		     rt = RTA_NEXT(rt, size)) {
		  if (!RTA_OK(rt, size)) {
		    delete m;
		    throw std::runtime_error("RTA truncated");
		  }

		  switch (rt->rta_type) {
		  case IFA_ADDRESS:
		    m->add_attr(new NLAddrAttrAddress((unsigned char*)
						      RTA_DATA(rt),
						      RTA_PAYLOAD(rt)));
		    break;
		  case IFA_LOCAL:
		    m->add_attr(new NLAddrAttrLocal((unsigned char*)
						    RTA_DATA(rt),
						    RTA_PAYLOAD(rt)));
		    break;
		  case IFA_LABEL:
		    m->add_attr(new NLAddrAttrLabel((char*) RTA_DATA(rt)));
		    break;
		  case IFA_BROADCAST:
		    m->add_attr(new NLAddrAttrBroadcast((unsigned char*)
							RTA_DATA(rt),
							RTA_PAYLOAD(rt)));
		    break;
		  case IFA_ANYCAST:
		    m->add_attr(new NLAddrAttrAnycast((unsigned char*)
						      RTA_DATA(rt),
						      RTA_PAYLOAD(rt)));
		    break;
		  }
		}
		output.push_back(m);
	      }
		break;
	      case RTM_NEWROUTE: {
		// FIXME: The following line is a dirty hack to
		// workaround a kernel bug fixed in 2.6.13-rc1.  This
		// should go away as soon as possible.
		done = false;

		rtmsg*		rt = (rtmsg*) NLMSG_DATA(nh);
		unsigned	size = RTM_PAYLOAD(nh);

		NLNewRoute*	m = new NLNewRoute(rt->rtm_family,
						   rt->rtm_dst_len,
						   rt->rtm_src_len,
						   rt->rtm_tos,
						   rt->rtm_table,
						   rt->rtm_protocol,
						   rt->rtm_scope,
						   rt->rtm_type,
						   rt->rtm_flags);

		for (rtattr* ra = RTM_RTA(rt); size > 0;
		     ra = RTA_NEXT(ra, size)) {
		  if (!RTA_OK(ra, size)) {
		    delete m;
		    throw std::runtime_error("RTA truncated");
		  }

		  switch (ra->rta_type) {
		  case RTA_DST:
		    m->add_attr(new NLRouteAttrDestination((unsigned char*)
							   RTA_DATA(ra),
							   RTA_PAYLOAD(ra)));
		    break;
		  case RTA_GATEWAY:
		    m->add_attr(new NLRouteAttrGateway((unsigned char*)
						       RTA_DATA(ra),
						       RTA_PAYLOAD(ra)));
		    break;
		  case RTA_OIF:
		    m->add_attr(new NLRouteAttrOutInterface(*(int*)
							    RTA_DATA(ra)));
		    break;
		  }
		}

		output.push_back(m);
	      }
		break;
	      }

	    }

	  } while (!done);

	} catch (std::exception& e) {
	  if (buffer != 0)
	    delete[] buffer;
	  for (std::list<NLMessage*>::iterator i = output.begin();
	       i != output.end(); ++i)
	    delete *i;
	  throw;
	}

	if (buffer != 0)
	  delete[] buffer;

	return output;
      }

    } // namespace netlink

  } // namespace sys

} // namespace olsr

#endif // ! QOLYESTER_SYS_LINUX_NETLINK_HXX
