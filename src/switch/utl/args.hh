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

#ifndef QOLYESTER_SWITCH_UTL_ARGS_HH
# define QOLYESTER_SWITCH_UTL_ARGS_HH 1

# include <map>
# include <string>
# include <sstream>
# include <limits>

# include "utl/log.hh"
# include "cst/constants.hh"
# include "net/ipaddress.hh"

namespace olsr {

  extern std::ostream	dump;

  bool	do_dump_prof  = false;
  bool	nodetach   = false;

# ifdef DEBUG
  bool	debugtrace = false;
  std::string	debugtrace_file;
  unsigned	debugtrace_log_level = std::numeric_limits<unsigned>::max();
  bool		debugtrace_timestamps = true;
# else
  const bool debugtrace = false;
# endif

  std::string	switch_sockname = VIRTUAL_SOCKNAME;

  unsigned	current_log_level = 3;
  bool		timestamps = false;
  bool		use_syslog = false;

  std::string	error_file;
  std::string	warning_file;
  std::string	notice_file;
  std::string	dump_file;
# ifdef DEBUG
  std::string	debug_file;
# endif

  static inline std::string usage();
  static inline std::string putversion();
  static inline void die(const char* format, ...);

  namespace utl {

    inline void parse_args(int& argc, char**& argv);

    inline void output_syslog();

  } // namespace utl

} // namespace olsr

# include "args.hxx"

#endif // ! QOLYESTER_SWITCH_UTL_ARGS_HH
