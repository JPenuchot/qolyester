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

#ifndef QOLYESTER_UTL_SYSLOG_HH
# define QOLYESTER_UTL_SYSLOG_HH 1

# include <streambuf>
# include <syslog.h>
# include <cassert>
# include <cstring>

namespace olsr {

  namespace utl {

# define SYSLOG_BUFFER_SIZE 4096

    class SyslogBuf : public std::streambuf {
      typedef std::streambuf	Super;
      typedef Super::char_type		char_type;
      typedef Super::traits_type	traits_type;
      typedef Super::int_type		int_type;
      typedef Super::pos_type		pos_type;
      typedef Super::off_type		off_type;
    public:
      SyslogBuf(int prio);
      ~SyslogBuf();
      int_type	overflow(int_type c = traits_type::eof());
      Super*	setbuf(char_type* s, std::streamsize n);
      int	sync();
    private:
      int		prio_;
      bool		ours_;
      unsigned		size_;
      char_type*	buffer_;
    };

  } // namespace utl

} // namespace olsr

#endif // !QOLYESTER_UTL_SYSLOG_HH
