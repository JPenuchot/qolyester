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

#ifndef QOLYESTER_SYS_LINUX_NETLINK_HH
# define QOLYESTER_SYS_LINUX_NETLINK_HH 1

# include <unistd.h>
# include <string>
# include <sys/uio.h>
# include <asm/types.h>
# include <sys/socket.h>
# include <linux/netlink.h>
# include <list>
# include <cassert>

namespace olsr {

  namespace sys {

    namespace netlink {

      class NLMessage;
      class NLAck;
      class NLError;
      class NLLinkAttr;
      class NLAddrAttr;
      class NLRouteAttr;
      class NLLinkAttrMTU;
      class NLLinkAttrName;
      class NLNewLink;
      class NLDelLink;
      class NLGetLink;
      class NLAddrAttrAddress;
      class NLAddrAttrLocal;
      class NLAddrAttrBroadcast;
      class NLAddrAttrAnycast;
      class NLAddrAttrLabel;
      class NLNewAddr;
      class NLDelAddr;
      class NLGetAddr;
      class NLRouteAttrDestination;
      class NLRouteAttrGateway;
      class NLRouteAttrOutInterface;
      class NLGetRoute;
      class NLNewRoute;
      class NLDelRoute;
      class RequestVisitor;
      class NLSocket;

      class Visitor {
      public:

# define VISIT(M) \
  virtual void visit(M&) = 0; \
  virtual void visit(const M&) = 0

	VISIT(NLAck);
	VISIT(NLError);

	VISIT(NLNewLink);
	VISIT(NLGetLink);
	VISIT(NLDelLink);

	VISIT(NLNewAddr);
	VISIT(NLGetAddr);
	VISIT(NLDelAddr);

	VISIT(NLNewRoute);
	VISIT(NLGetRoute);
	VISIT(NLDelRoute);

	VISIT(NLLinkAttrName);
	VISIT(NLLinkAttrMTU);

	VISIT(NLAddrAttrAddress);
	VISIT(NLAddrAttrLocal);
	VISIT(NLAddrAttrLabel);
	VISIT(NLAddrAttrBroadcast);
	VISIT(NLAddrAttrAnycast);

	VISIT(NLRouteAttrDestination);
	VISIT(NLRouteAttrGateway);
	VISIT(NLRouteAttrOutInterface);

# undef VISIT

	virtual ~Visitor() {}
      };

      class DefaultVisitor : public Visitor {
      public:
	DefaultVisitor() : Visitor() {}

# define VISIT(M) \
  virtual void visit(M&) {} \
  virtual void visit(const M&) {}

	VISIT(NLAck)
	VISIT(NLError)

	VISIT(NLNewLink)
	VISIT(NLGetLink)
	VISIT(NLDelLink)

	VISIT(NLNewAddr)
	VISIT(NLGetAddr)
	VISIT(NLDelAddr)

	VISIT(NLNewRoute)
	VISIT(NLGetRoute)
	VISIT(NLDelRoute)

	VISIT(NLLinkAttrName)
	VISIT(NLLinkAttrMTU)

	VISIT(NLAddrAttrAddress)
	VISIT(NLAddrAttrLocal)
	VISIT(NLAddrAttrLabel)
	VISIT(NLAddrAttrBroadcast)
	VISIT(NLAddrAttrAnycast)

	VISIT(NLRouteAttrDestination)
	VISIT(NLRouteAttrGateway)
	VISIT(NLRouteAttrOutInterface)

# undef VISIT

      };

      // Level 0

      class NLMessage {
      public:
	virtual ~NLMessage() {}
	virtual void accept(Visitor&) const = 0;
      };

      class NLAck : public NLMessage {
      public:
	virtual ~NLAck() {}
	virtual void accept(Visitor& v) const { v.visit(*this); }
      };

      class NLError : public NLMessage {
      public:
	NLError(int e)
	  : _errno(e)
	{}
	virtual ~NLError() {}
	int	get_errno() const { return _errno; }
	virtual void accept(Visitor& v) const { v.visit(*this); }
      private:
	int	_errno;
      };

      // Attributes
      // Level 0

      class NLLinkAttr {
      public:
	virtual ~NLLinkAttr() {}
	virtual void accept(Visitor&) const = 0;
      };

      class NLAddrAttr {
      public:
	virtual ~NLAddrAttr() {}
	virtual void accept(Visitor&) const = 0;
      };

      class NLRouteAttr {
      public:
	virtual ~NLRouteAttr() {}
	virtual void accept(Visitor&) const = 0;
      };

      // Level 1

      class NLLinkAttrMTU : public NLLinkAttr {
      public:
	NLLinkAttrMTU(unsigned mtu) : _mtu(mtu) {}

	virtual ~NLLinkAttrMTU() {}

	virtual void accept(Visitor& v) const { v.visit(*this);	}

	unsigned mtu() const { return _mtu; }
      private:
	const unsigned	_mtu;
      };

      class NLLinkAttrName : public NLLinkAttr {
      public:
	NLLinkAttrName(const std::string& s) : _name(s) {}
	virtual ~NLLinkAttrName() {}

	virtual void accept(Visitor& v) const { v.visit(*this); }

	const std::string& name() const { return _name; }
      private:
	const std::string	_name;
      };

      // Level 2

      class NLNewLink : public NLMessage {
      public:
	typedef std::list<NLLinkAttr*>	attrs_t;
	NLNewLink(unsigned char f,
		  unsigned short t,
		  int i,
		  unsigned int fl) :
	  _family(f),
	  _type(t),
	  _index(i),
	  _flags(fl),
	  _attrs()
	{}
	virtual ~NLNewLink() {
	  for (attrs_t::const_iterator i = _attrs.begin(); i != _attrs.end(); ++i)
	    delete *i;
	}

	virtual void accept(Visitor& v) const {
	  v.visit(*this);
// 	  for (attrs_t::const_iterator i = _attrs.begin(); i != _attrs.end(); ++i)
// 	    (*i)->accept(v);
	}

	void add_attr(NLLinkAttr* a) {
	  _attrs.push_back(a);
	}

	unsigned char	family() const { return _family; }
	unsigned short	type()   const { return _type;   }
	unsigned	index()  const { return _index;  }
	unsigned int	flags()  const { return _flags;  }
	const attrs_t&	attrs()  const { return _attrs;  }
      private:
	unsigned char	_family;
	unsigned short	_type;
	unsigned	_index;
	unsigned int	_flags;
	attrs_t		_attrs;
      };

      class NLDelLink : public NLMessage {
      public:
	virtual ~NLDelLink() {}
	// ...
      };

      class NLGetLink : public NLMessage {
      public:
	virtual ~NLGetLink() {}
	virtual void accept(Visitor& v) const { v.visit(*this); }
      };

      class NLAddrAttrAddress : public NLAddrAttr {
      public:
	NLAddrAttrAddress(const unsigned char* b, unsigned len) :
	  _bytes(new unsigned char[len]), _len(len) {
	  memcpy(_bytes, b, len);
	}
	virtual ~NLAddrAttrAddress() { delete[] _bytes; }

	virtual void accept(Visitor& v) const { v.visit(*this); }

	const unsigned char*	bytes()  const { return _bytes; }
	unsigned		length() const { return _len;   }
      private:
	unsigned char*	_bytes;
	unsigned	_len;
      };

      class NLAddrAttrLocal : public NLAddrAttr {
      public:
	NLAddrAttrLocal(const unsigned char* b, unsigned len) :
	  _bytes(new unsigned char[len]), _len(len) {
	  memcpy(_bytes, b, len);
	}
	virtual ~NLAddrAttrLocal() { delete[] _bytes; }

	virtual void		accept(Visitor& v) const { v.visit(*this); }
	const unsigned char*	bytes()            const { return _bytes;  }
	unsigned		length()           const { return _len;    }
      private:
	unsigned char*	_bytes;
	unsigned	_len;
      };

      class NLAddrAttrBroadcast : public NLAddrAttr {
      public:
	NLAddrAttrBroadcast(const unsigned char* b, unsigned len) :
	  _bytes(new unsigned char[len]), _len(len) {
	  memcpy(_bytes, b, len);
	}
	virtual ~NLAddrAttrBroadcast() { delete[] _bytes; }

	virtual void		accept(Visitor& v) const { v.visit(*this); }
	const unsigned char*	bytes()            const { return _bytes;  }
	unsigned		length()           const { return _len;    }
      private:
	unsigned char*	_bytes;
	unsigned	_len;
      };

      class NLAddrAttrAnycast : public NLAddrAttr {
      public:
	NLAddrAttrAnycast(const unsigned char* b, unsigned len) :
	  _bytes(new unsigned char[len]), _len(len) {
	  memcpy(_bytes, b, len);
	}
	virtual ~NLAddrAttrAnycast() { delete[] _bytes; }

	virtual void		accept(Visitor& v) const { v.visit(*this); }
	const unsigned char*	bytes()            const { return _bytes;  }
	unsigned		length()           const { return _len;    }
      private:
	unsigned char*	_bytes;
	unsigned	_len;
      };

      class NLAddrAttrLabel : public NLAddrAttr {
      public:
	NLAddrAttrLabel(const std::string& label) :
	  _label(label)
	{}
	virtual ~NLAddrAttrLabel() {}

	virtual void	accept(Visitor& v) const { v.visit(*this); }
	const std::string&	label()    const { return _label;  }
      private:
	const std::string	_label;
      };

      class NLNewAddr : public NLMessage {
      public:
	typedef std::list<NLAddrAttr*>	attrs_t;
	NLNewAddr(unsigned char f,
		  unsigned char p,
		  unsigned char fl,
		  unsigned char s,
		  int i) :
	  _family(f), _prefixlen(p), _flags(fl), _scope(s), _index(i), _attrs()
	{}
	virtual ~NLNewAddr() {
	  for (attrs_t::iterator i = _attrs.begin(); i != _attrs.end(); ++i)
	    delete *i;
	}

	virtual void accept(Visitor& v) const {
	  v.visit(*this);
// 	  for (attrs_t::const_iterator i = _attrs.begin(); i != _attrs.end(); ++i)
// 	    (*i)->accept(v);
	}
	unsigned char	family()    const { return _family;    }
	unsigned char	prefixlen() const { return _prefixlen; }
	unsigned char	flags()     const { return _flags;     }
	unsigned char	scope()     const { return _scope;     }
	unsigned	index()     const { return _index;     }
	const attrs_t&	attrs()     const { return _attrs;     }

	void add_attr(NLAddrAttr* a) { _attrs.push_back(a); }
      private:
	unsigned char	_family;
	unsigned char	_prefixlen;
	unsigned char	_flags;
	unsigned char	_scope;
	unsigned	_index;
	attrs_t		_attrs;
      };

      class NLGetAddr : public NLMessage {
      public:
	NLGetAddr(unsigned char f) : _family(f) {}
	virtual ~NLGetAddr() {}

	virtual void	accept(Visitor& v) const { v.visit(*this); }
	unsigned char	family()           const { return _family; }
      private:
	unsigned char	_family;
      };

      class NLDelAddr : public NLMessage {
      public:
	typedef std::list<NLAddrAttr*>	attrs_t;
	NLDelAddr(unsigned char f,
		  unsigned char p,
		  unsigned char fl,
		  unsigned char s,
		  int i) :
	  _family(f), _prefixlen(p), _flags(fl), _scope(s), _index(i), _attrs()
	{}
	virtual ~NLDelAddr() {
	  for (attrs_t::iterator i = _attrs.begin(); i != _attrs.end(); ++i)
	    delete *i;
	}

	virtual void accept(Visitor& v) const {
	  v.visit(*this);
// 	  for (attrs_t::const_iterator i = _attrs.begin();
// 	       i != _attrs.end(); ++i)
// 	    (*i)->accept(v);
	}
	unsigned char	family()    const { return _family;    }
	unsigned char	prefixlen() const { return _prefixlen; }
	unsigned char	flags()     const { return _flags;     }
	unsigned char	scope()     const { return _scope;     }
	int		index()     const { return _index;     }
	const attrs_t&	attrs()     const { return _attrs;     }

	void add_attr(NLAddrAttr* a) { _attrs.push_back(a); }
      private:
	unsigned char	_family;
	unsigned char	_prefixlen;
	unsigned char	_flags;
	unsigned char	_scope;
	int		_index;
	attrs_t		_attrs;
      };

      class NLRouteAttrDestination : public NLRouteAttr {
      public:
	NLRouteAttrDestination(const unsigned char* b, unsigned len)
	  : _bytes(new unsigned char[len]),
	    _len(len) {
	  memcpy(_bytes, b, len);
	}
	virtual ~NLRouteAttrDestination() { delete[] _bytes; }

	virtual void accept(Visitor& v) const { v.visit(*this); }

	const unsigned char*	bytes() const { return _bytes; }
	unsigned		length() const { return _len; }
      private:
	unsigned char*	_bytes;
	unsigned	_len;
      };

      class NLRouteAttrGateway : public NLRouteAttr {
      public:
	NLRouteAttrGateway(const unsigned char* b, unsigned len)
	  : _bytes(new unsigned char[len]),
	    _len(len) {
	  memcpy(_bytes, b, len);
	}
	virtual ~NLRouteAttrGateway() { delete[] _bytes; }

	virtual void accept(Visitor& v) const { v.visit(*this); }

	const unsigned char*	bytes() const { return _bytes; }
	unsigned		length() const { return _len; }
      private:
	unsigned char*	_bytes;
	unsigned	_len;
      };

      class NLRouteAttrOutInterface : public NLRouteAttr {
      public:
	NLRouteAttrOutInterface(int index)
	  : _index(index)
	{}
	virtual ~NLRouteAttrOutInterface() {}

	virtual void accept(Visitor& v) const { v.visit(*this); }

	int		index() const { return _index; }
      private:
	int		_index;
      };

      class NLGetRoute : public NLMessage {
      public:
	NLGetRoute(unsigned char f) : _family(f) {}
	virtual ~NLGetRoute() {}

	virtual void	accept(Visitor& v) const { v.visit(*this); }
	unsigned char	family()           const { return _family; }
      private:
	unsigned char	_family;
      };

      class NLNewRoute : public NLMessage {
      public:
	typedef std::list<NLRouteAttr*>	attrs_t;

	NLNewRoute(unsigned char f,
		   unsigned char dlen,
		   unsigned char slen,
		   unsigned char tos,
		   unsigned char table,
		   unsigned char proto,
		   unsigned char scope,
		   unsigned char type,
		   unsigned int flags)
	  : _family(f),
	    _dlen(dlen),
	    _slen(slen),
	    _tos(tos),
	    _table(table),
	    _proto(proto),
	    _scope(scope),
	    _type(type),
	    _flags(flags),
	    _attrs()
	{}
	virtual ~NLNewRoute() {
	  for (attrs_t::iterator i = _attrs.begin(); i != _attrs.end(); ++i)
	    delete *i;
	}
	virtual void	accept(Visitor& v) const {
	  v.visit(*this);
// 	  for (attrs_t::const_iterator i = _attrs.begin(); i != _attrs.end(); ++i)
// 	    (*i)->accept(v);
	}
	unsigned char	family() const { return _family; }
	unsigned char	dlen()   const { return _dlen; }
	unsigned char	slen()   const { return _slen; }
	unsigned char	tos()    const { return _tos; }
	unsigned char	table()  const { return _table; }
	unsigned char	proto()  const { return _proto; }
	unsigned char	scope()  const { return _scope; }
	unsigned char	type()   const { return _type; }
	unsigned int	flags()  const { return _flags; }

 	attrs_t&	attrs() { return _attrs; }
	const attrs_t&	attrs() const { return _attrs; }

	void		add_attr(NLRouteAttr* a) { _attrs.push_back(a); }
      private:
	unsigned char	_family;
	unsigned char	_dlen;
	unsigned char	_slen;
	unsigned char	_tos;
	unsigned char	_table;
	unsigned char	_proto;
	unsigned char	_scope;
	unsigned char	_type;
	unsigned int	_flags;
	attrs_t		_attrs;
      };

      class NLDelRoute : public NLMessage {
      public:
	typedef std::list<NLRouteAttr*>	attrs_t;

	NLDelRoute(unsigned char f,
		   unsigned char dlen,
		   unsigned char slen,
		   unsigned char tos,
		   unsigned char table,
		   unsigned char proto,
		   unsigned char scope,
		   unsigned char type,
		   unsigned int flags)
	  : _family(f),
	    _dlen(dlen),
	    _slen(slen),
	    _tos(tos),
	    _table(table),
	    _proto(proto),
	    _scope(scope),
	    _type(type),
	    _flags(flags),
	    _attrs()
	{}
	virtual ~NLDelRoute() {
	  for (attrs_t::iterator i = _attrs.begin(); i != _attrs.end(); ++i)
	    delete *i;
	}
	virtual void	accept(Visitor& v) const {
	  v.visit(*this);
// 	  for (attrs_t::const_iterator i = _attrs.begin(); i != _attrs.end(); ++i)
// 	    (*i)->accept(v);
	}
	unsigned char	family() const { return _family; }
	unsigned char	dlen()   const { return _dlen; }
	unsigned char	slen()   const { return _slen; }
	unsigned char	tos()    const { return _tos; }
	unsigned char	table()  const { return _table; }
	unsigned char	proto()  const { return _proto; }
	unsigned char	scope()  const { return _scope; }
	unsigned char	type()   const { return _type; }
	unsigned int	flags()  const { return _flags; }

	const attrs_t&	attrs() const { return _attrs; }
 	attrs_t&	attrs() { return _attrs; }

	void		add_attr(NLRouteAttr* a) { _attrs.push_back(a); }
      private:
	unsigned char	_family;
	unsigned char	_dlen;
	unsigned char	_slen;
	unsigned char	_tos;
	unsigned char	_table;
	unsigned char	_proto;
	unsigned char	_scope;
	unsigned char	_type;
	unsigned int	_flags;
	attrs_t		_attrs;
      };

      // Visitor for builing requests

      class RequestVisitor : public DefaultVisitor {
	typedef std::list<iovec>	buffers_t;
      public:
	inline RequestVisitor();

	inline ~RequestVisitor();

	virtual inline void	visit(const NLGetLink&);

	virtual inline void	visit(const NLNewAddr&);
	virtual inline void	visit(const NLGetAddr&);
	virtual inline void	visit(const NLDelAddr&);

	virtual inline void	visit(const NLGetRoute&);
	virtual inline void	visit(const NLNewRoute&);
	virtual inline void	visit(const NLDelRoute&);

	virtual inline void	visit(const NLAddrAttrAddress&);
	virtual inline void	visit(const NLAddrAttrLocal&);
// 	virtual inline void	visit(const NLAddrAttrLabel&);
	virtual inline void	visit(const NLAddrAttrBroadcast&);
	virtual inline void	visit(const NLAddrAttrAnycast&);

	virtual inline void	visit(const NLRouteAttrDestination&);
	virtual inline void	visit(const NLRouteAttrGateway&);
	virtual inline void	visit(const NLRouteAttrOutInterface&);

	inline const char*	buffer();
	inline unsigned		length();

	static unsigned		seqnum;
      private:

	unsigned totalsize() const;

	buffers_t	_buffers;
	char*		_buffer;
	unsigned	_length;
      };

      // Netlink sockets

      class NLSocket {
	typedef sockaddr_nl	sockaddr_t;

      public:
	typedef std::list<NLMessage*>	answer_t;

	inline NLSocket();

	inline ~NLSocket();

	inline void	send(const NLMessage& m);
      private:
	inline void	do_receive(char*& buffer, unsigned& length);
      public:
	inline answer_t receive();
      private:
	int	_fd;
      };

    } // namespace netlink

  } // namespace sys

} // namespace olsr

# include "netlink.hxx"

#endif // ! QOLYESTER_SYS_LINUX_NETLINK_HH
