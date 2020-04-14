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
// Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_UTL_LOG_HH
# define QOLYESTER_UTL_LOG_HH 1

# include <ostream>
# include <sstream>
# include <cassert>
# include <limits>

# include <sys/time.h>

namespace olsr {

  extern unsigned	current_log_level;
  extern bool		timestamps;

  namespace utl {

    struct of {
      of(const double& v, char f = ' ', std::streamsize p = 2, int w = 6)
	: _val(v),
	  _f(f),
	  _p(p),
	  _w(w)
      {}
      double		_val;
      char		_f;
      std::streamsize	_p;
      int			_w;
    };

    inline
    std::ostream& operator<<(std::ostream& os, const of& f)
    {
      if (f._val <= -10.)
	return os << " < -10";

      std::ios::fmtflags	flags = os.setf(std::ios::showbase |
						std::ios::right |
						std::ios::fixed,
						std::ios::showbase |
						std::ios::right |
						std::ios::fixed);
      char		c = os.fill(f._f);
      std::streamsize	p = os.precision(f._p);
      int		w = os.width(f._w);
      os << f._val;
      os.fill(c);
      os.precision(p);
      os.width(w);
      os.flags(flags);
      return os;
    }

    class LevelableBuf {
    public:
      virtual ~LevelableBuf() {}
      virtual void	increase_level(unsigned i = 1) = 0;
      virtual void	decrease_level(unsigned i = 1) = 0;
      virtual void	reset_level() = 0;
    };

    template <class F, class C, class T = std::char_traits<C> >
    class PrefixBuf : public std::basic_streambuf<C, T>,
		      public LevelableBuf {
      typedef PrefixBuf<F, C, T>		This;
      typedef std::basic_streambuf<C, T>	Super;
      typedef typename Super::char_type		char_type;
      typedef typename Super::traits_type	traits_type;
      typedef typename Super::int_type		int_type;
      typedef typename Super::pos_type		pos_type;
      typedef typename Super::off_type		off_type;
    public:
      PrefixBuf(Super* other, bool m = true)
	: other_(other),
	  proxy_(dynamic_cast<LevelableBuf*>(other_)),
	  mark_(m)
      {}

      PrefixBuf(Super* other, const F& f, bool m = true)
	: other_(other),
	  proxy_(dynamic_cast<LevelableBuf*>(other_)),
	  mark_(m),
	  f_(f)
      {}

      Super*	get_streambuf() {
	return other_;
      }

      void	set_streambuf(Super* other) {
	other_->pubsync();
	other_ = other;
	proxy_ = dynamic_cast<LevelableBuf*>(other_);
      }

      virtual void	increase_level(unsigned i = 1) {
	if (proxy_)
	  proxy_->increase_level(i);
      }
      virtual void	decrease_level(unsigned i = 1) {
	if (proxy_)
	  proxy_->decrease_level(i);
      }
      virtual void	reset_level() {
	if (proxy_)
	  proxy_->reset_level();
      }
    protected:
      virtual void imbue(const std::locale &l) {
	other_->pubimbue(l);
      }

      virtual int_type overflow(int_type c = traits_type::eof()) {
	if (mark_) {
	  mark_ = false;
	  const std::basic_string<C, T>	s = f_(c);
	  other_->sputn(s.data(), s.size());
	}
	if (c == char_type('\n'))
	  mark_ = true;
	return other_->sputc(c);
      }

      virtual std::basic_streambuf<char_type, traits_type>*
      setbuf(char_type* s, std::streamsize n) {
	return other_->pubsetbuf(s, n);
      }

      virtual int	sync() {
	return other_->pubsync();
      }
    private:
      Super*		other_;
      LevelableBuf*	proxy_;
      bool		mark_;
      F			f_;
    };

    struct StringPrefixer {
      StringPrefixer(const std::string& s,
		     const std::string& vs = "")
	: s_(s),
	  vs_(vs)
      {}

      const std::string&	operator()(int c = 0) const {
	if (c == '\n')
	  return vs_;
	return s_;
      }
    private:
      std::string	s_;
      std::string	vs_;
    };

    struct Timestamper {
      std::string	operator()(int c = 0) const {
	::timeval	tv;
	gettimeofday(&tv, 0);
	::tm	tm;
	gmtime_r(&tv.tv_sec, &tm);
	char	buf[9];
	strftime(buf, sizeof buf, "%T", &tm);
	std::ostringstream	os;
	os << buf << '.';
	os.fill('0');
	os.width(6);
	os << std::right << tv.tv_usec << (c == '\n' ? "" : " ");
	return os.str();
      }
    };

    template <class C, class T>
    inline
    std::basic_ostream<C, T>&	indent(std::basic_ostream<C, T>& os)
    {
      os.rdbuf(new PrefixBuf<StringPrefixer, C, T>(os.rdbuf(),
						   StringPrefixer("  ")));
      return os;
    }

    template <class C, class T>
    inline
    std::basic_ostream<C, T>&	deindent(std::basic_ostream<C, T>& os)
    {
      typedef PrefixBuf<StringPrefixer, C, T>	pb_t;
      assert(dynamic_cast<pb_t*>(os.rdbuf()) != 0);
      pb_t*	sb = static_cast<pb_t*>(os.rdbuf());
      os.rdbuf(sb->get_streambuf());
      delete sb;
      return os;
    }

    template <typename C, typename T = std::char_traits<C> >
    struct basic_indent_ {
      std::basic_string<C, T>	s;
    };

    template <typename C, typename T>
    inline
    basic_indent_<C, T>	basic_indent(const std::basic_string<C, T>& s =
				     std::basic_string<C, T>(2, ' ')) {
      basic_indent_<C, T>	indent__;
      indent__.s = s;
      return indent__;
    }

# define indent(S) utl::basic_indent<char, std::char_traits<char> >(S)

    template <typename C, typename T>
    inline
    std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const basic_indent_<C, T>& i)
    {
      os.rdbuf(new PrefixBuf<StringPrefixer, C, T>(os.rdbuf(),
						   StringPrefixer(i.s)));
      return os;
    }

    template <typename C, typename T = std::char_traits<C> >
    class basic_dummyostream : public std::basic_ostream<C, T> {
      typedef basic_dummyostream<C, T>		This;
      typedef std::basic_ostream<C, T>		Super;
      typedef typename Super::char_type		char_type;
      typedef typename Super::traits_type	traits_type;
      typedef typename Super::int_type		int_type;
      typedef typename Super::pos_type		pos_type;
      typedef typename Super::off_type		off_type;
    public:
      explicit basic_dummyostream(std::basic_streambuf<C, T>* sb)
	: Super(sb)
      {}
      basic_dummyostream(Super& other)
	: Super(other.rdbuf()) {
	copyfmt(other);
      }
      template <typename U>
      const This&	operator<<(const U&) const { return *this; }
      const This&	operator<<(std::ostream& (*)(std::ostream&)) const {
	return *this;
      }
    };

    template <typename C, typename T = std::char_traits<C> >
    class basic_ybuf : public std::basic_streambuf<C, T>,
		       public LevelableBuf {
      typedef std::basic_streambuf<C, T>	Super;
      typedef typename Super::char_type		char_type;
      typedef typename Super::traits_type	traits_type;
      typedef typename Super::int_type		int_type;
      typedef typename Super::pos_type		pos_type;
      typedef typename Super::off_type		off_type;
    public:
      basic_ybuf(Super* other1, Super* other2)
	: other1_(other1),
	  other2_(other2),
	  proxy1_(dynamic_cast<LevelableBuf*>(other1_)),
	  proxy2_(dynamic_cast<LevelableBuf*>(other2_))
      {}
      Super*	get_streambuf1() {
	return other1_;
      }
      void	set_streambuf1(Super* other) {
	other1_->pubsync();
	other1_ = other;
	proxy1_ = dynamic_cast<LevelableBuf*>(other1_);
      }
      Super*	get_streambuf2() {
	return other2_;
      }
      void	set_streambuf2(Super* other) {
	other2_->pubsync();
	other2_ = other;
	proxy2_ = dynamic_cast<LevelableBuf*>(other2_);
      }
      virtual void	increase_level(unsigned i = 1) {
	if (proxy1_)
	  proxy1_->increase_level(i);
	if (proxy2_)
	  proxy2_->increase_level(i);
      }
      virtual void	decrease_level(unsigned i = 1) {
	if (proxy1_)
	  proxy1_->decrease_level(i);
	if (proxy2_)
	  proxy2_->decrease_level(i);
      }
      virtual void	reset_level() {
	if (proxy1_)
	  proxy1_->reset_level();
	if (proxy2_)
	  proxy2_->reset_level();
      }
    protected:
      virtual void imbue(const std::locale &l) {
	other1_->pubimbue(l);
	other2_->pubimbue(l);
      }

      virtual int_type overflow(int_type c = traits_type::eof()) {
	int_type	ret1 = other1_->sputc(c);
	int_type	ret2 = other2_->sputc(c);
	if (ret1 == traits_type::eof() || ret2 == traits_type::eof())
	  return traits_type::eof();
	return 0;
      }

      virtual int	sync() {
	int	ret1 = other1_->pubsync();
	int	ret2 = other2_->pubsync();
	if (ret1 < 0 || ret2 < 0)
	  return -1;
	return 0;
      }

      virtual std::streamsize xsputn(char_type* s, std::streamsize n) {
	std::streamsize	ret1 = other1_->sputn(s, n);
	std::streamsize	ret2 = other2_->sputn(s, n);

	return ret1 < ret2 ? ret1 : ret2;
      }
    private:
      Super*		other1_;
      Super*		other2_;
      LevelableBuf*	proxy1_;
      LevelableBuf*	proxy2_;
    };

    template <typename C, typename T = std::char_traits<C> >
    class basic_levelbuf : public std::basic_streambuf<C, T>,
			   public LevelableBuf {
      typedef std::basic_streambuf<C, T>	Super;
      typedef typename Super::char_type		char_type;
      typedef typename Super::traits_type	traits_type;
      typedef typename Super::int_type		int_type;
      typedef typename Super::pos_type		pos_type;
      typedef typename Super::off_type		off_type;
    public:
      inline basic_levelbuf(unsigned level, Super* other,
			    const unsigned& minlevel = current_log_level);

      Super*	get_streambuf() {
	return other_;
      }

      void	set_streambuf(Super* other) {
	other_->pubsync();
	other_ = other;
      }

      virtual void	increase_level(unsigned i = 1) {
	assert(level_ <= std::numeric_limits<unsigned>::max() - i);
	level_ += i;
      }

      virtual void	decrease_level(unsigned i = 1) {
	assert(level_ >= i);
	level_ -= i;
      }

      virtual void	reset_level() {
	level_ = baselevel_;
      }
    protected:
      virtual void imbue(const std::locale &l) {
	other_->pubimbue(l);
      }

      virtual inline int_type overflow(int_type c = traits_type::eof());

      virtual std::basic_streambuf<char_type, traits_type>*
      setbuf(char_type* s, std::streamsize n) {
	return other_->pubsetbuf(s, n);
      }

      virtual int	sync() {
	bool	would = level_ <= minlevel_;
	if (would)
	  return other_->pubsync();
	return 0;
      }
    private:
      Super*			other_;
      unsigned			level_;
      unsigned			baselevel_;
      const unsigned&		minlevel_;
    };

    template <typename C, typename T = std::char_traits<C> >
    class basic_dummybuf : public std::basic_streambuf<C, T> {
      typedef std::basic_streambuf<C, T>	Super;
      typedef typename Super::char_type	char_type;
      typedef typename Super::traits_type	traits_type;
      typedef typename Super::int_type	int_type;
      typedef typename Super::pos_type	pos_type;
      typedef typename Super::off_type	off_type;
    public:
      basic_dummybuf() : Super() {}
    protected:
      virtual int_type overflow(int_type = traits_type::eof()) {
	return 0;
      }

      virtual int_type pbackfail(int_type = traits_type::eof()) {
	return 0;
      }
    };

    template <typename C, typename T>
    inline
    std::basic_ostream<C, T>&	up(std::basic_ostream<C, T>& os) {
      LevelableBuf*	l;
      if ((l = dynamic_cast<LevelableBuf*>(os.rdbuf())) == 0)
	return os;
      l->increase_level();
      return os;
    }

    template <typename C, typename T>
    inline
    std::basic_ostream<C, T>&	down(std::basic_ostream<C, T>& os) {
      LevelableBuf*	l;
      if ((l = dynamic_cast<LevelableBuf*>(os.rdbuf())) == 0)
	return os;
      l->decrease_level();
      return os;
    }

    template <typename C, typename T>
    inline
    std::basic_ostream<C, T>&	reset(std::basic_ostream<C, T>& os) {
      LevelableBuf*	l;
      if ((l = dynamic_cast<LevelableBuf*>(os.rdbuf())) == 0)
	return os;
      l->reset_level();
      return os;
    }

    struct up_ {
      unsigned i;
    };

    struct down_ {
      unsigned i;
    };

    inline
    up_	up(unsigned i = 1) {
      up_	up__;
      up__.i = i;
      return up__;
    }

    inline
    down_	down(unsigned i = 1) {
      down_	down__;
      down__.i = i;
      return down__;
    }

    template <typename C, typename T>
    inline
    std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const up_& u)
    {
      LevelableBuf*	l;
      if ((l = dynamic_cast<LevelableBuf*>(os.rdbuf())) == 0)
	return os;
      l->increase_level(u.i);
      return os;
    }

    template <typename C, typename T>
    inline
    std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const down_& d)
    {
      LevelableBuf*	l;
      if ((l = dynamic_cast<LevelableBuf*>(os.rdbuf())) == 0)
	return os;
      l->decrease_level(d.i);
      return os;
    }

  } // namespace utl

  using utl::of;

  using utl::StringPrefixer;
  using utl::Timestamper;

  typedef utl::PrefixBuf<StringPrefixer, char>	StringPrefixBuf;
  typedef utl::PrefixBuf<Timestamper, char>	TimestampBuf;

  typedef utl::basic_dummyostream<char>	DummyOStream;
  typedef utl::basic_levelbuf<char>	LevelBuf;
  typedef utl::basic_ybuf<char>		YBuf;
  typedef utl::basic_dummybuf<char>	DummyBuf;

# ifdef DEBUG
  typedef std::ostream			debug_ostream_t;
# else // !DEBUG
  typedef DummyOStream			debug_ostream_t;
# endif

  using utl::up;
  using utl::down;
  using utl::reset;
  using utl::indent;
  using utl::deindent;

} // namespace olsr

# include "log.hxx"

#endif // ! QOLYESTER_LOG_HH
