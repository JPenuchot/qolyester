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

#include "syslog.hh"

namespace olsr {

  namespace utl {

    SyslogBuf::SyslogBuf(int prio)
      : prio_(prio),
	ours_(true),
	size_(SYSLOG_BUFFER_SIZE),
	buffer_(new char_type[SYSLOG_BUFFER_SIZE + 1]) {
      setp(buffer_, buffer_ + SYSLOG_BUFFER_SIZE);
    }

    SyslogBuf::~SyslogBuf() {
      if (ours_)
	delete[] buffer_;
    }

    SyslogBuf::int_type
    SyslogBuf::overflow(int_type c) {
      pubsync();
      if (c == '\n')
	return 0;
      if (pptr() < epptr())
	sputc(c);
      else
	syslog(prio_, "%c", c);
      return 0;
    }

    std::streambuf*
    SyslogBuf::setbuf(char_type* s, std::streamsize n) {
      assert(n > 1);
      if (ours_) {
	delete[] buffer_;
	ours_ = false;
      }
      size_ = n - 1;
      buffer_ = s;
      setp(buffer_, buffer_ + n - 1);
      return this;
    }

    int
    SyslogBuf::sync() {
      char_type*	p = pbase();
      char_type*	p2 = 0;

      *pptr() = 0;

      while ((p2 = strchr(p, '\n')) != 0) {
	*p2 = 0;
	syslog(prio_, "%s", p);
	p = p2 + 1;
      }
      if (*p != 0)
	syslog(prio_, "%s", p);
      setp(buffer_, buffer_ + size_);
      return 0;
    }

  } // namespace utl

} // namespace olsr

