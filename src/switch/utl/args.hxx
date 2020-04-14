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

#ifndef QOLYESTER_SWITCH_UTL_ARGS_HXX
# define QOLYESTER_SWITCH_UTL_ARGS_HXX 1

# include <unistd.h>
# include <getopt.h>
# include <cstdarg>
# include <iostream>
# include <sstream>
# include <fstream>

# include "args.hh"

# include "utl/syslog.hh"

namespace olsr {

  extern const std::string	progname;
  extern const std::string	shortname;
  extern const std::string	version;
  extern std::ostream		dump_prof;

  static std::string usage()
  {
    std::ostringstream	os;
    os << "usage: qswitchd GRAPH...\n"
       << '\n'
       << "GRAPH is a DOT file describing the static network topology\n"
       << '\n'
       << "Options:\n"
       << "      --switch-sock=FILE                use this as the default switch socket\n"
       << "      --error-file=FILE                 print errors to FILE, default stderr\n"
       << "      --warning-file=FILE               print warnings to FILE, default stderr\n"
       << "      --notice-file=FILE                print notices to FILE, default stderr\n"
       << "      --dump-file=FILE                  print dumps to FILE, default stderr\n"
# ifdef DEBUG
       << "      --debug-file=FILE                 print debug to FILE, default stderr\n"
# endif
       << "      --all-file=FILE                   print all dumps to FILE, other\n"
       << "                                          --*-file options override this\n"
       << "  -h, --help                            list available command (this page)\n"
       << "  -n, --no-detach                       prevent going background\n"
       << "      --version                         display version information and exit\n"
       << "      --dump-prof                       print seconds spent computing\n"
       << "                                          in each event loop\n"
       << "      --debug-trace=FILE                activate debug trace mode\n"
       << "                                          and log into FILE\n"
       << "  -v, --verbose                         show debugging messages\n"
       << "                                          (use several times to show more)\n"
       << "  -T, --timestamps                      show timestamps in messages\n"
       << "  -S, --syslog                          send messages to syslog by default\n"
       << "                                          (when using -n)\n"
      ;
    return os.str();
  }

  static std::string putversion()
  {
    std::ostringstream	os;
    os << "This is " << progname << " version " << version << ",\n"
       << "  the switch process for Qolyester.\n"
       << "  Using IPv" << (address_t::family == AF_INET ?
			    4 :
			    address_t::family == AF_INET6 ?
			    6 :
			    0) << " address format\n";
    return os.str();
  }

  static void die(const char* format, ...)
  {
    va_list	list;
    va_start(list, format);
    vfprintf(stderr, format, list);
    va_end(list);
    exit(1);
  }

# define ARG_VERSION		256
# define ARG_ERRORFILE		263
# define ARG_WARNINGFILE	264
# define ARG_NOTICEFILE		265
# define ARG_DUMPFILE		266
# define ARG_DEBUGFILE		267
# define ARG_DUMPPROF		268
# define ARG_SWSOCK		270
# define ARG_ALLFILE		271
# define ARG_DEBUGTRACE		272

  namespace utl {

    void parse_args(int& argc, char**& argv)
    {
      ::option	opts[] = {
	{ "error-file", 1, NULL, ARG_ERRORFILE },
	{ "warning-file", 1, NULL, ARG_WARNINGFILE },
	{ "notice-file", 1, NULL, ARG_NOTICEFILE },
	{ "dump-file", 1, NULL, ARG_DUMPFILE },
# ifdef DEBUG
	{ "debug-file", 1, NULL, ARG_DEBUGFILE },
# endif
	{ "all-file", 1, NULL, ARG_ALLFILE },
	{ "help", 0, NULL, 'h' },
	{ "no-detach", 0, NULL, 'n' },
	{ "version", 0, NULL, ARG_VERSION },
	{ "dump-prof", 0, NULL, ARG_DUMPPROF },
	{ "verbose", 0, NULL, 'v' },
	{ "syslog", 0, NULL, 'S' },
	{ "timestamps", 0, NULL, 'T' },
	{ "switch-sock", 1, NULL, ARG_SWSOCK },
	{ "debug-trace", 1, NULL, ARG_DEBUGTRACE },
	{ NULL, 0, NULL, 0 }
      };

      int	opt_ind = 0;
      int	val;

      std::string	all_file;

# ifdef DEBUG
      std::ostringstream	cmdline;

      for (unsigned i = 0; i < (unsigned) argc; ++i)
	cmdline << ' ' << argv[i];
# endif // !DEBUG

      while ((val = getopt_long(argc, argv, "hnSTv", opts, &opt_ind)) >= 0) {

	switch (val) {
	case ARG_ERRORFILE:
	  if (!error_file.empty())
	    die("Multiple --error-file parameters\n");
	  error_file = optarg;
	  break;
	case ARG_WARNINGFILE:
	  if (!warning_file.empty())
	    die("Multiple --warning-file parameters\n");
	  warning_file = optarg;
	  break;
	case ARG_NOTICEFILE:
	  if (!notice_file.empty())
	    die("Multiple --notice-file parameters\n");
	  notice_file = optarg;
	  break;
	case ARG_DUMPFILE:
	  if (!dump_file.empty())
	    die("Multiple --dump-file parameters\n");
	  dump_file = optarg;
	  break;
# ifdef DEBUG
	case ARG_DEBUGFILE:
	  if (!debug_file.empty())
	    die("Multiple --debug-file parameters\n");
	  debug_file = optarg;
	  break;
# endif
	case ARG_ALLFILE:
	  if (!all_file.empty())
	    die("Multiple --all-file parameters\n");
	  all_file = optarg;
	  break;
	case ARG_DUMPPROF:
	  do_dump_prof = true;
	  break;
	case ARG_SWSOCK:
	  switch_sockname = optarg;
	  break;
# ifndef DEBUG
	case ARG_DEBUGTRACE:
	  die("Debug trace mode only available when compiled "
	      "with --enable-debug.\n");
	  break;
# else // DEBUG
	case ARG_DEBUGTRACE:
	  debugtrace = true;
	  debugtrace_file = optarg;
	  break;
# endif
	case ARG_VERSION:
	  std::cerr << putversion() << std::flush;
	  exit(0);
	  break;
	case 'n':
	  nodetach = true;
	  break;
	case 'h':
	  std::cerr << usage() << std::flush;
	  exit(1);
	  break;
	case 'S':
	  use_syslog = true;
	  break;
	case 'T':
	  timestamps = true;
	  break;
	case 'v':
	  ++current_log_level;
	  break;
	default:
	  dump << usage() << std::flush;
	  exit(1);
	  break;
	}

      }

      if (!all_file.empty()) {
	if (error_file.empty())
	  error_file = all_file;
	if (warning_file.empty())
	  warning_file = all_file;
	if (notice_file.empty())
	  notice_file = all_file;
	if (dump_file.empty())
	  dump_file = all_file;
      }
# ifdef DEBUG
      if (debug_file.empty())
	debug_file = all_file;
# endif

      typedef std::map<std::string, std::filebuf*>	bufmap_t;

      bufmap_t	buf_map;

      // Process file outputs
      // ====================
      //
      // The idea here is that dump and debug are ostreams with
      // LevelBufs as streambufs.
      //
      //    ostream
      //       |
      //       V
      //    LevelBuf
      //       |
      //       V
      //    streambuf
      //
      // In addition, error, warning and notice are ostreams with an
      // additional StringPrefixBuf between the LevelBuf and the
      // streambuf.
      //
      //    ostream
      //       |
      //       V
      //    LevelBuf
      //       |
      //       V
      //    StringPrefixBuf
      //       |
      //       V
      //    streambuf
      //
      // If the user asked to redirect the corresponding output from
      // standard output to some file, replace the original streambuf
      // with a filebuf.
      //
      //    ostream
      //       |
      //       V
      //    LevelBuf
      //       |
      //       V
      //  [ StringPrefixBuf
      //       |
      //       V            ]
      //    filebuf
      //
      // If the user requested timestamps, a TimestampBuf has to be
      // added before the ultimate filebuf or streambuf.
      //
      //    ostream
      //       |
      //       V
      //    LevelBuf
      //       |
      //       V
      //  [ StringPrefixBuf
      //       |
      //       V            ]
      //  [ TimestampBuf
      //       |
      //       V            ]
      //  { filebuf | streambuf }
      //

# define PROC_BUF(Name, Buf)						\
      do {								\
	bufmap_t::iterator	x = buf_map.find( Name ## _file);	\
	if (x == buf_map.end()) {					\
	  std::filebuf*	b = new std::filebuf;				\
	  b->open( Name ## _file.c_str(),				\
		   std::ios_base::out | std::ios_base::app);		\
	  if (!b->is_open())						\
	    die("Could not open \"%s\" for writing: %s\n",		\
		Name ## _file.c_str(), strerror(errno));		\
	  x = buf_map.insert(bufmap_t::					\
			     value_type( Name ## _file, b)).first;	\
	}								\
	Buf ->set_streambuf(x->second);					\
      } while (false)

# define SPB	StringPrefixBuf
# define PROC_LOG_PREFIXED(Name)					\
      do {								\
	if (! Name ## _file.empty()) {					\
	  assert(dynamic_cast<LevelBuf*>( Name .rdbuf()) != 0);		\
	  LevelBuf*	b = static_cast<LevelBuf*>( Name .rdbuf());	\
	  assert(dynamic_cast< SPB *>(b->get_streambuf()) != 0);	\
	  SPB *	sb = static_cast< SPB *>(b->get_streambuf());		\
	  if (timestamps) {						\
	    TimestampBuf*	tb =					\
	      new TimestampBuf(sb->get_streambuf());			\
	    PROC_BUF( Name , tb);					\
	    sb->set_streambuf(tb);					\
	  } else							\
	    PROC_BUF( Name , sb);					\
	} else if (timestamps) {					\
	  assert(dynamic_cast<LevelBuf*>( Name .rdbuf()) != 0);		\
	  LevelBuf*	b = static_cast<LevelBuf*>( Name .rdbuf());	\
	  assert(dynamic_cast< SPB *>(b->get_streambuf()) != 0);	\
	  SPB *	sb = static_cast< SPB *>(b->get_streambuf());		\
	  TimestampBuf*	tb = new TimestampBuf(sb->get_streambuf());	\
	  sb->set_streambuf(tb);					\
	}								\
      } while (false)

# define PROC_LOG(Name)							\
      do {								\
	if (! Name ## _file.empty()) {					\
	  assert(dynamic_cast<LevelBuf*>( Name .rdbuf()) != 0);		\
	  LevelBuf*	b = static_cast<LevelBuf*>( Name .rdbuf());	\
	  if (timestamps) {						\
	    TimestampBuf*	tb =					\
	      new TimestampBuf(b->get_streambuf());			\
	    PROC_BUF( Name , tb);					\
	    b->set_streambuf(tb);					\
	  } else							\
	    PROC_BUF( Name , b);					\
	} else if (timestamps) {					\
	  assert(dynamic_cast<LevelBuf*>( Name .rdbuf()) != 0);		\
	  LevelBuf*	b = static_cast<LevelBuf*>( Name .rdbuf());	\
	  TimestampBuf*	tb = new TimestampBuf(b->get_streambuf());	\
	  b->set_streambuf(tb);						\
	}								\
      } while (false)

      PROC_LOG_PREFIXED(error);
      PROC_LOG_PREFIXED(warning);
      PROC_LOG_PREFIXED(notice);
      PROC_LOG(dump);

# ifdef DEBUG
      PROC_LOG(debug);
# endif

# undef PROC_LOG
# undef PROC_LOG_PREFIXED
# undef SPB
# undef PROC_BUF

      // Now copy all that pretty stuff to various dump_* ostreams
      // from dump.

      dump_prof.rdbuf(dump.rdbuf());

# ifdef DEBUG

      // Prepare the debugtrace logging facility.  We want all
      // possible logging output with timestamps output to a file
      // specified along with the --debug-trace option.

      if (debugtrace) {

	// First, open the output file.

	std::filebuf*	b = new std::filebuf;
	b->open(debugtrace_file.c_str(), std::ios_base::out | std::ios_base::app);
	if (!b->is_open())
	  die("Could not open \"%s\" for writing: %s\n",
	      debugtrace_file.c_str(), strerror(errno));

	// Second, create the TimestampBuf with the new filebuf.

	TimestampBuf*	tb = new TimestampBuf(b);

	// Third, use a YBuf to duplicate the flow at its source
	// (before the original LevelBuf) and feed it to another
	// LevelBuf and possibly to a StringPrefixBuf and ultimately
	// to the TimestampBuf.
	//
	//    ostream
	//       |
	//       V
	//    YBuf ---> LevelBuf
	//       |         |
	//       |         V
	//       |    [ StringPrefixBuf
	//       |         |
	//       |         V            ]
	//       |    [ TimestampBuf
	//       |         |
	//       |         V            ]
	//       |    { filebuf | streambuf }
	//       V
	//    LevelBuf
	//       |
	//       V
	//  [ StringPrefixBuf
	//       |
	//       V            ]
	//    TimestampBuf
	//       |
	//       V
	//    filebuf
	//
	// The reason why we want to divert the flow before the
	// LevelBuf is that the current log level settings for the
	// original and the debugtrace are different (the one for
	// debugtrace is the highest possible, to ensure that
	// everything is logged).
	//

# define SPB	StringPrefixBuf
# define SP	StringPrefixer
# define PROC_LOG_PREFIXED(Name, Level, Prefix)				\
	do {								\
	  SPB *		spb = new SPB (tb, SP( Prefix ));		\
	  LevelBuf*	lb = new LevelBuf( Level , spb,			\
					   debugtrace_log_level);	\
	  Name .rdbuf(new YBuf( Name .rdbuf(), lb));			\
	} while (false)

# define PROC_LOG(Name, Level)						\
	do {								\
	  LevelBuf*	lb = new LevelBuf( Level , tb,			\
					   debugtrace_log_level);	\
	  Name .rdbuf(new YBuf( Name .rdbuf(), lb));			\
	} while (false)

	PROC_LOG_PREFIXED(error, 0, "ERROR: ");
	PROC_LOG_PREFIXED(warning, 1, "WARNING: ");
	PROC_LOG_PREFIXED(notice, 2, "NOTICE: ");
	PROC_LOG(dump, 3);
	PROC_LOG(debug, 5);

# undef PROC_LOG
# undef PROC_LOG_PREFIXED
# undef SP
# undef SPB

	// Now process the various dump_* for debugtrace.  If we have
	// been requested to do_dump_*, then we want to behave exactly
	// as dump.  Otherwise, we want dump_* to flow out
	// debugtrace's LevelBuf.

# define PROC_DTLOG(Name)					\
	do {							\
	  if (do_dump_ ## Name )				\
	    dump_ ## Name .rdbuf(dump.rdbuf());			\
	  else {						\
	    assert(dynamic_cast<YBuf*>(dump.rdbuf()) != 0);	\
	    YBuf*	yb = static_cast<YBuf*>(dump.rdbuf());	\
	    LevelBuf*	lb = 					\
	      static_cast<LevelBuf*>(yb->get_streambuf2());	\
	    dump_ ## Name .rdbuf(lb);				\
            do_dump_ ## Name = true;                            \
	  }							\
	} while (false)

	PROC_DTLOG(prof);

# undef PROC_DTLOG
      }
# endif // !DEBUG

      if (use_syslog)
	output_syslog();

      dump << up << putversion() << std::flush << down;

# ifdef DEBUG
      if (debugtrace)
	debug << up(500) << "Command line:" << cmdline.str()
	      << std::endl << down(500);
# endif // !DEBUG

      argc -= optind;
      argv += optind;

      if (argc == 0) {
	error << "Missing argument" << std::endl;
	dump << usage() << std::flush;
	exit(1);
      } else if (argc > 1) {
	error << "Invalid extra argument" << (argc > 2 ? "s:" : ":");
	for (int i = 1; i < argc; ++i)
	  error << ' ' << argv[i];
	error << std::endl;
	dump << usage() << std::flush;
	exit(1);
      }

    }

    void output_syslog()
    {
      if (!nodetach || use_syslog) {
	openlog(shortname.c_str(), LOG_NDELAY | LOG_PID, LOG_DAEMON);

	// Redirect all stderr output to syslog with correct priorities
	// If there is a TimestampBuf before the last streambuf, remove
	// it (and destroy it).

# define SPB	StringPrefixBuf
# define PROC_LOG_PREFIXED(Name, Level)					     \
	do {								     \
	  if ( Name ## _file.empty()) {					     \
	    std::streambuf*	b;					     \
									     \
	    if(debugtrace) {						     \
	      assert(dynamic_cast<YBuf*>( Name .rdbuf()) != 0);		     \
	      YBuf*	yb = static_cast<YBuf*>( Name .rdbuf());	     \
	      b = yb->get_streambuf1();					     \
	    } else							     \
	      b = Name .rdbuf();					     \
									     \
	    assert(dynamic_cast<LevelBuf*>(b) != 0);			     \
	    LevelBuf*	lb = static_cast<LevelBuf*>(b);			     \
	    assert(dynamic_cast< SPB *>(lb->get_streambuf()) != 0);	     \
	    SPB *	sb = static_cast<SPB *>(lb->get_streambuf());	     \
									     \
	    if (timestamps) {						     \
	      assert(dynamic_cast<TimestampBuf*>(sb->get_streambuf()) != 0); \
	      delete static_cast<TimestampBuf*>(sb->get_streambuf());	     \
	    }								     \
									     \
	    sb->set_streambuf(new utl::SyslogBuf( Level ));		     \
	  }								     \
	} while (false)

# define PROC_LOG(Name, Level)						     \
	do {								     \
	  if ( Name ## _file.empty()) {					     \
	    std::streambuf*	b;					     \
									     \
	    if(debugtrace) {						     \
	      assert(dynamic_cast<YBuf*>( Name .rdbuf()) != 0);		     \
	      YBuf*	yb = static_cast<YBuf*>( Name .rdbuf());	     \
	      b = yb->get_streambuf1();					     \
	    } else							     \
	      b = Name .rdbuf();					     \
									     \
	    assert(dynamic_cast<LevelBuf*>(b) != 0);			     \
	    LevelBuf*	lb = static_cast<LevelBuf*>(b);			     \
									     \
	    if (timestamps) {						     \
	      assert(dynamic_cast<TimestampBuf*>(lb->get_streambuf()) != 0); \
	      delete static_cast<TimestampBuf*>(lb->get_streambuf());	     \
	    }								     \
									     \
	    lb->set_streambuf(new utl::SyslogBuf( Level ));		     \
	  }								     \
	} while (false)

	PROC_LOG_PREFIXED(error, LOG_ERR);
	PROC_LOG_PREFIXED(warning, LOG_WARNING);
	PROC_LOG_PREFIXED(notice, LOG_NOTICE);
	PROC_LOG(dump, LOG_INFO);
# ifdef DEBUG
	PROC_LOG(debug, LOG_DEBUG);
# endif

# undef PROC_LOG
# undef PROC_LOG_PREFIXED
# undef SPB

      }

    }

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_SWITCH_UTL_ARGS_HXX
