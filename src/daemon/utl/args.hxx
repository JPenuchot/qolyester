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

#ifndef QOLYESTER_DAEMON_UTL_ARGS_HXX
# define QOLYESTER_DAEMON_UTL_ARGS_HXX 1

# include <unistd.h>
# include <getopt.h>
# include <cstdarg>
# include <sstream>
# include <list>
# include <set>
# include <map>
# include <fstream>

# include "cst/constants.hh"
# include "net/ipaddress.hh"
# include "set/interfaces.hh"
# include "sys/interfaceinfo.hh"
# include "set/gate.hh"
# include "utl/log.hh"
# include "utl/syslog.hh"

namespace olsr {

  extern ifaceset_t		iface_set;
  extern const std::string	progname;
  extern const std::string	shortname;
  extern const std::string	version;
  extern std::ostream		dump;
  extern std::ostream		dump_state;
  extern std::ostream		dump_prof;
  extern std::ostream		dump_hello;
  extern std::ostream		dump_tc;
# ifdef QOLYESTER_ENABLE_MID
  extern std::ostream		dump_mid;
# endif
# ifdef QOLYESTER_ENABLE_HNA
  extern std::ostream		dump_hna;
# endif
  extern gateset_t		gate_set;

  static std::string usage()
  {
    std::ostringstream	os;
    os <<
# ifdef QOLYESTER_ENABLE_MID
      "usage: qolsrd -i NAME[=ADDR][,NAME[=ADDR]...]\n"
# else // !QOLYESTER_ENABLE_MID
      "usage: qolsrd -i NAME[=ADDR]\n"
# endif
# ifdef QOLYESTER_ENABLE_HNA
       << "             [-o address/plen[,address/plen...]]\n"
# endif
# ifdef QOLYESTER_ENABLE_TCRED
       << "             [-w willingness] [-t tcredundancy]\n"
# else // !QOLYESTER_ENABLE_TCRED
       << "             [-w willingness]\n"
# endif
# ifdef QOLYESTER_ENABLE_MPRRED
       << "             [-m mprcoverage] [-h] [-n]\n"
# else // !QOLYESTER_ENABLE_MPRRED
       << "             [-h] [-n]\n"
# endif
       << "Options:\n"
# ifdef QOLYESTER_ENABLE_VIRTUAL
#  ifdef QOLYESTER_ENABLE_MID
       << "  -i, --interfaces=NAME[=ADDR][;FILE],...  use these OLSR interfaces (optionally\n"
       << "                                          with the specified address\n"
       << "                                          and the specified socket file)\n"
#  else // !QOLYESTER_ENABLE_MID
       << "  -i, --interface=NAME[=ADDR][;FILE]    use this OLSR interface (optionally\n"
       << "                                          with the specified addressi\n"
       << "                                          and the specified socket file)\n"
#  endif
       << "      --switch-sock=FILE                use this as the default switch\n"
       << "                                          socket\n"
# else // !QOLYESTER_ENABLE_VIRTUAL
#  ifdef QOLYESTER_ENABLE_MID
       << "  -i, --interfaces=NAME[=ADDR],...      use these OLSR interfaces (optionally\n"
       << "                                          with the specified address)\n"
#  else // !QOLYESTER_ENABLE_MID
       << "  -i, --interface=NAME[=ADDR]          use this OLSR interface (optionally\n"
       << "                                          with the specified address)\n"
#  endif
# endif
# ifdef QOLYESTER_ENABLE_HNA
       << "  -o, --other-networks=ADDR/PREFIX,...  use these non-OLSR networks\n"
# endif
       << "  -w, --willingness=WILL                node willingness to be an MPR\n"
       << "                                          WILL must be `never', `low',\n"
       << "                                          `default', `high', `always' or\n"
       << "                                          an integer between 0 (never) and\n"
       << "                                          7 (always), the default is 3\n"
# ifdef QOLYESTER_ENABLE_TCRED
       << "  -t, --tc-redundancy=REDUNDANCY        TC information redundancy\n"
       << "                                          REDUNDANCY must be either `mprsel',\n"
       << "                                          `mprsel+mpr' or `symset',\n"
       << "                                          the default is `mprsel'\n"
# endif
# ifdef QOLYESTER_ENABLE_MPRRED
       << "  -m, --mpr-coverage=COVERAGE           MPR coverage, COVERAGE must be a\n"
       << "                                          positive integer\n"
# endif

       << "      --hello-interval=MSEC             set the HELLO emission interval in ms\n"
       << "      --refresh-interval=MSEC           set the refresh interval in ms\n"
       << "      --tc-interval=MSEC                set the TC emission interval in ms\n"
# ifdef QOLYESTER_ENABLE_MID
       << "      --mid-interval=MSEC               set the MID emission interval in ms\n"
# endif
# ifdef QOLYESTER_ENABLE_HNA
       << "      --hna-interval=MSEC               set the HNA emission interval in ms\n"
# endif
# ifdef QOLYESTER_ENABLE_MID
       << "      --mid-hold-time=MSEC              set the MID information holding time\n"
       << "                                          in ms\n"
# endif
# ifdef QOLYESTER_ENABLE_HNA
       << "      --hna-hold-time=MSEC              set the HNA information holding time\n"
       << "                                          in ms\n"
# endif

       << "      --neighb-hold-time=MSEC           set the neighbor holding time in ms\n"
       << "      --top-hold-time=MSEC              set the topology holding time in ms\n"
       << "      --dup-hold-time=MSEC              set the duplicates holding time in ms\n"
       << "      --max-jitter-time=MSEC            set the maximum jitter time in ms\n"

# ifdef QOLYESTER_ENABLE_LINKHYS
       << "      --hello-grace=FLOAT               set the HELLO interval grace\n"
       << "                                          the default is "
       << cst::hello_grace << '\n'
       << "      --hyst-threshold-high=FLOAT       set the high threshold for link\n"
       << "                                          hysteresis (default is "
       << cst::hyst_threshold_high << ")\n"
       << "      --hyst-threshold-low=FLOAT        set the low threshold for link\n"
       << "                                          hysteresis (default is "
       << cst::hyst_threshold_low << ")\n"
       << "      --hyst-scaling=FLOAT              set the scaling for link hysteresis\n"
       << "                                          (default is "
       << cst::hyst_scaling << ")\n"
# endif

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
# ifndef QOLYESTER_ENABLE_VIRTUAL
       << "      --no-tables                       prevent management of routing tables\n"
# endif // !QOLYESTER_ENABLE_VIRTUAL
       << "                                          (for testing only)\n"
       << "      --version                         display version information and exit\n"
       << "      --dump-hello                      dump HELLO messages\n"
       << "      --dump-tc                         dump TC messages\n"
# ifdef QOLYESTER_ENABLE_MID
       << "      --dump-mid                        dump MID messages\n"
# endif
# ifdef QOLYESTER_ENABLE_HNA
       << "      --dump-hna                        dump HNA messages\n"
# endif
       << "      --dump-all                        dump all known messages\n"
       << "      --dump-state                      dump the state every second\n"
       << "      --dump-prof                       print seconds spent computing\n"
       << "                                          in each event loop\n"
       << "      --dump-interval=MSEC              state dumping interval in ms\n"
# ifdef DEBUG
       << "      --debug-trace=FILE                activate debug trace mode\n"
       << "                                          and log into FILE\n"
# endif
       << "  -v, --verbose                         show debugging messages\n"
       << "                                          (use several times to show more)\n"
       << "  -T, --timestamps                      show timestamps in messages\n"
       << "  -S, --syslog                          send messages to syslog by default\n"
       << "                                          (when using -n)\n"
# if QOLYESTER_FAMILY_INET == 6
       << "      --dirty-promisc-hack	           enable and disable promiscuous\n"
       << "                                          mode to make buggy chipsets work\n"
# endif
       << "      --queue-size=N                    specify sending queue size per\n"
       << "                                          interface (defaults to "
       << cst::def_queue_size << ")\n"
      ;
    return os.str();
  }

  static std::string putversion()
  {
    std::ostringstream	os;
    os << "This is " << progname << " version " << version << ",\n"
       << "  an implementation of the OLSR protocol in C++.\n"
       << "  Using IPv" << (address_t::family == AF_INET ?
			    4 :
			    address_t::family == AF_INET6 ?
			    6 :
			    0) << " address format\n"
       << "  Optional features:\n"
# if !defined(QOLYESTER_ENABLE_LINKHYS) && !defined(QOLYESTER_ENABLE_MID) && \
     !defined(QOLYESTER_ENABLE_HNA) && !defined(QOLYESTER_ENABLE_TCRED) && \
     !defined(QOLYESTER_ENABLE_MPRRED) && !defined(QOLYESTER_ENABLE_VIRTUAL)
      "    (none)\n"
# else
#  if defined(QOLYESTER_ENABLE_LINKHYS)
      "    Link-Hysteresis\n"
#  endif
#  if defined(QOLYESTER_ENABLE_MID)
      "    MID\n"
#  endif
#  if defined(QOLYESTER_ENABLE_HNA)
      "    HNA\n"
#  endif
#  if defined(QOLYESTER_ENABLE_TCRED)
      "    TC-redundancy\n"
#  endif
#  if defined(QOLYESTER_ENABLE_MPRRED)
      "    MPR-coverage\n"
#  endif
#  ifdef QOLYESTER_ENABLE_VIRTUAL
      "    Virtual Interfaces\n"
#  endif
# endif
      ;
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
# define ARG_DUMPSTATE		257
# define ARG_DUMPHELLO		258
# define ARG_DUMPTC		259
# define ARG_DUMPMID		260
# define ARG_DUMPHNA		261
# define ARG_DUMPALL		262
# define ARG_ERRORFILE		263
# define ARG_WARNINGFILE	264
# define ARG_NOTICEFILE		265
# define ARG_DUMPFILE		266
# define ARG_DEBUGFILE		267
# define ARG_DUMPPROF		268
# define ARG_NOTABLES		270
# define ARG_SWSOCK		271
# define ARG_ALLFILE		272
# define ARG_DEBUGTRACE		273

# define ARG_HELLOINTERVAL	274
# define ARG_REFRESHINTERVAL	275
# define ARG_TCINTERVAL		276
# define ARG_MIDINTERVAL	277
# define ARG_HNAINTERVAL	278
# define ARG_MIDHOLDTIME	279
# define ARG_HNAHOLDTIME	280
# define ARG_NEIGHBHOLDTIME	281
# define ARG_TOPHOLDTIME	282
# define ARG_DUPHOLDTIME	283
# define ARG_MAXJITTERTIME	284
# define ARG_DUMPINTERVAL	287

# define ARG_DIRTYPROMISCHACK	288

# define ARG_QUEUESIZE		289

# define ARG_HELLOGRACE		290
# define ARG_HYSTHI		291
# define ARG_HYSTLO		292
# define ARG_HYSTSCALING	293

  namespace utl {

    namespace internal {
# ifdef QOLYESTER_ENABLE_VIRTUAL
      struct IfTuple{
	IfTuple(const std::string& n,
		const std::string& a,
		const std::string& s)
	  : name(n),
	    addr(a.empty() ? address_t() : a),
	    sock(s) {}
	std::string	name;
	address_t	addr;
	std::string	sock;
      };
# else // !QOLYESTER_ENABLE_VIRTUAL
      struct IfTuple{
	IfTuple(const std::string& n,
		const std::string& a)
	  : name(n),
	    addr(a.empty() ? address_t() : a) {}
	std::string	name;
	address_t	addr;
      };
# endif
    } // namespace internal

  } // namespace utl

} // namespace olsr

namespace std {

  template <>
  struct less< ::olsr::utl::internal::IfTuple> {
    bool	operator()(const ::olsr::utl::internal::IfTuple& a,
			   const ::olsr::utl::internal::IfTuple& b) const {
      return a.name < b.name;
    }
  };

} // namespace std

namespace olsr {

  namespace utl {

    void parse_args(int& argc, char**& argv)
    {
      ::option	opts[] = {

# ifdef QOLYESTER_ENABLE_MID
	{ "interfaces", 1, NULL, 'i' },
# else
	{ "interface", 1, NULL, 'i' },
# endif

# ifdef QOLYESTER_ENABLE_HNA
	{ "other-networks", 1, NULL, 'o' },
# endif

	{ "willingness", 1, NULL, 'w' },

# ifdef QOLYESTER_ENABLE_TCRED
	{ "tc-redundancy", 1, NULL, 't' },
# endif

# ifdef QOLYESTER_ENABLE_MPRRED
	{ "mpr-coverage", 1, NULL, 'm' },
# endif

	{ "hello-interval", 1, NULL, ARG_HELLOINTERVAL },
	{ "refresh-interval", 1, NULL, ARG_REFRESHINTERVAL },
	{ "tc-interval", 1, NULL, ARG_TCINTERVAL },

# ifdef QOLYESTER_ENABLE_MID
	{ "mid-interval", 1, NULL, ARG_MIDINTERVAL },
	{ "mid-hold-time", 1, NULL, ARG_MIDHOLDTIME },
# endif

# ifdef QOLYESTER_ENABLE_HNA
	{ "hna-interval", 1, NULL, ARG_HNAINTERVAL },
	{ "hna-hold-time", 1, NULL, ARG_HNAHOLDTIME },
# endif

	{ "neighb-hold-time", 1, NULL, ARG_NEIGHBHOLDTIME },
	{ "top-hold-time", 1, NULL, ARG_TOPHOLDTIME },
	{ "dup-hold-time", 1, NULL, ARG_DUPHOLDTIME },

	{ "max-jitter-time", 1, NULL, ARG_MAXJITTERTIME },

# ifdef QOLYESTER_ENABLE_LINKHYS
	{ "hello-grace", 1, NULL, ARG_HELLOGRACE },
	{ "hyst-threshold-high", 1, NULL, ARG_HYSTHI },
	{ "hyst-threshold-low", 1, NULL, ARG_HYSTLO },
	{ "hyst-scaling", 1, NULL, ARG_HYSTSCALING },
# endif

	{ "error-file", 1, NULL, ARG_ERRORFILE },
	{ "warning-file", 1, NULL, ARG_WARNINGFILE },
	{ "notice-file", 1, NULL, ARG_NOTICEFILE },
	{ "dump-file", 1, NULL, ARG_DUMPFILE },
# ifdef DEBUG
	{ "debug-file", 1, NULL, ARG_DEBUGFILE },
# endif
	{ "all-file", 1, NULL, ARG_ALLFILE },

	{ "help", 0, NULL, 'h' },
	{ "verbose", 0, NULL, 'v' },
	{ "syslog", 0, NULL, 'S' },
	{ "timestamps", 0, NULL, 'T' },
	{ "no-detach", 0, NULL, 'n' },
	{ "version", 0, NULL, ARG_VERSION },

	{ "dump-hello", 0, NULL, ARG_DUMPHELLO },
	{ "dump-tc", 0, NULL, ARG_DUMPTC },

# ifdef QOLYESTER_ENABLE_MID
	{ "dump-mid", 0, NULL, ARG_DUMPMID },
# endif

# ifdef QOLYESTER_ENABLE_HNA
	{ "dump-hna", 0, NULL, ARG_DUMPHNA },
# endif

	{ "dump-state", 0, NULL, ARG_DUMPSTATE },
	{ "dump-all", 0, NULL, ARG_DUMPALL },
	{ "dump-prof", 0, NULL, ARG_DUMPPROF },

	{ "dump-interval", 1, NULL, ARG_DUMPINTERVAL },
	{ "no-tables", 0, NULL, ARG_NOTABLES },

# ifdef QOLYESTER_ENABLE_VIRTUAL
	{ "switch-sock", 1, NULL, ARG_SWSOCK },
# endif

	{ "debug-trace", 1, NULL, ARG_DEBUGTRACE },
# if QOLYESTER_FAMILY_INET == 6
	{ "dirty-promisc-hack", 0, NULL, ARG_DIRTYPROMISCHACK },
# endif
	{ "queue-size", 1, NULL, ARG_QUEUESIZE },
	{ NULL, 0, NULL, 0 }
      };

      int	opt_ind = 0;
      int	val;

      typedef internal::IfTuple			iftuple_t;
      typedef std::pair<std::string, unsigned>	gate_t;

      std::list<iftuple_t>	ifaces;
      std::set<iftuple_t>	ifacechecks;
      std::list<gate_t>		gates;

      std::string	all_file;

# ifdef DEBUG
      std::ostringstream	cmdline;

      for (unsigned i = 0; i < (unsigned) argc; ++i)
	cmdline << ' ' << argv[i];
# endif // !DEBUG

      while ((val = getopt_long(argc, argv, "i:"
# ifdef QOLYESTER_ENABLE_HNA
				"o:"
# endif
				"w:"
# ifdef QOLYESTER_ENABLE_TCRED
				"t:"
# endif
# ifdef QOLYESTER_ENABLE_MPRRED
				"m:"
# endif
				"hnSTv", opts, &opt_ind)) >= 0) {

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
	case ARG_DUMPALL:
	  do_dump_hello = do_dump_tc
# ifdef QOLYESTER_ENABLE_MID
	    = do_dump_mid
# endif
# ifdef QOLYESTER_ENABLE_HNA
	    = do_dump_hna
# endif
	    = true;
	  break;
# ifdef QOLYESTER_ENABLE_HNA
	case ARG_DUMPHNA:
	  do_dump_hna = true;
	  break;
# endif
# ifdef QOLYESTER_ENABLE_MID
	case ARG_DUMPMID:
	  do_dump_mid = true;
	  break;
# endif
	case ARG_DUMPTC:
	  do_dump_tc = true;
	  break;
	case ARG_DUMPSTATE:
	  do_dump_state = true;
	  break;
	case ARG_DUMPHELLO:
	  do_dump_hello = true;
	  break;
# ifndef QOLYESTER_ENABLE_VIRTUAL
	case ARG_NOTABLES:
	  notables = true;
	  break;
# else // QOLYESTER_ENABLE_VIRTUAL
	case ARG_SWSOCK:
	  switch_sockname = optarg;
	  break;
# endif
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
# if QOLYESTER_FAMILY_INET == 6
	case ARG_DIRTYPROMISCHACK:
	  dirty_promisc_hack = true;
	  break;
# endif

# define GETNUMBER(Variable, Option)			\
  do {							\
    char*	p;					\
    unsigned	val = strtoul(optarg, &p, 10);		\
    if (p == optarg || *p != 0 || val == 0)		\
      die("Parse error in --" Option ": %s\n", optarg);	\
    cst:: Variable = val;				\
  } while (0)

# define GETFLOAT(Variable, Option)			\
  do {							\
    char*	p;					\
    double	val = strtod(optarg, &p);		\
    if (p == optarg || *p != 0 || val == 0)		\
      die("Parse error in --" Option ": %s\n", optarg);	\
    cst:: Variable = val;				\
  } while (0)

	case ARG_QUEUESIZE:
	  GETNUMBER(queue_size, "queue-size");
	  break;
	case ARG_HELLOINTERVAL:
	  GETNUMBER(hello_interval, "hello-interval");
	  break;
	case ARG_REFRESHINTERVAL:
	  GETNUMBER(refresh_interval, "refresh-interval");
	  break;
	case ARG_TCINTERVAL:
	  GETNUMBER(tc_interval, "tc-interval");
	  break;
# ifdef QOLYESTER_ENABLE_LINKHYS
	case ARG_HELLOGRACE:
	  GETFLOAT(hello_grace, "hello-grace");
	  break;
	case ARG_HYSTHI:
	  GETFLOAT(hyst_threshold_high, "hyst-threshold-high");
	  break;
	case ARG_HYSTLO:
	  GETFLOAT(hyst_threshold_low, "hyst-threshold-low");
	  break;
	case ARG_HYSTSCALING:
	  GETFLOAT(hyst_scaling, "hyst-scaling");
	  break;
# endif

# ifdef QOLYESTER_ENABLE_MID
	case ARG_MIDINTERVAL:
	  GETNUMBER(mid_interval, "mid-interval");
	  break;
	case ARG_MIDHOLDTIME:
	  GETNUMBER(mid_hold_time, "mid-hold-time");
	  break;
# endif
# ifdef QOLYESTER_ENABLE_HNA
	case ARG_HNAINTERVAL:
	  GETNUMBER(hna_interval, "hna-interval");
	  break;
	case ARG_HNAHOLDTIME:
	  GETNUMBER(hna_hold_time, "hna-hold-time");
	  break;
# endif
	case ARG_NEIGHBHOLDTIME:
	  GETNUMBER(neighb_hold_time, "neighb-hold-time");
	  break;
	case ARG_TOPHOLDTIME:
	  GETNUMBER(top_hold_time, "top-hold-time");
	  break;
	case ARG_DUPHOLDTIME:
	  GETNUMBER(dup_hold_time, "dup-hold-time");
	  break;
	case ARG_MAXJITTERTIME:
	  GETNUMBER(maxjitter, "max-jitter-time");
	  break;
	case ARG_DUMPINTERVAL:
	  GETNUMBER(dump_interval, "dump-interval");
	  break;
# undef GETNUMBER
# undef GETFLOAT

	case ARG_VERSION:
	  dump << putversion() << std::flush;
	  exit(0);
	  break;
	case 'n':
	  nodetach = true;
	  break;
	case 'i':
# ifdef QOLYESTER_ENABLE_VIRTUAL

	  /*
	   * The following is a determinized finite state automaton to
	   * parse the interface specification argument.  This allows
	   * to specify interface name, optional address to use and
	   * switch socket to connect to, in the following format:
           *
           * ^(([A-Za-z]+[0-9]+)(=(([^,;\\]|\\.)+))?(;(([^,\\]|\\.)+))?)(,(([A-Za-z]+[0-9]+)(=(([^,;\\]|\\.)+))?(;(([^,\\]|\\.)+))?))*$
           *
           * Then name is \2, address is \4 and socket is \7 and so on.
	   */

	  {
	    enum State { a, b, cdaeaf, db, dc, ddeaf, de, eb, ec, edf, ee,
			 accept, reset, error };

	    State	state = a;

	    std::string	s(optarg);
	    unsigned	i = 0;

	    std::string	name;
	    std::string	addr;
	    std::string sock;

	    do {

	      if (i == s.length()) {
		if (state == cdaeaf ||
		    state == ddeaf  ||
		    state == edf)
		  state = accept;
		else
		  state = error;
	      }

	      switch (state) {
	      case a:
		if (islower(s[i]) || isupper(s[i])) {
		  name.append(1, s[i++]);
		  state = b;
		} else
		  state = error;
		break;
	      case b:
		if (islower(s[i]) || isupper(s[i]))
		  name.append(1, s[i++]);
		else if (isdigit(s[i])) {
		  name.append(1, s[i++]);
		  state = cdaeaf;
		} else
		  state = error;
		break;
	      case cdaeaf:
		if (isdigit(s[i]))
		  name.append(1, s[i++]);
		else if (s[i] == '=') {
		  ++i;
		  state = db;
		} else if (s[i] == ';') {
		  ++i;
		  state = eb;
		} else if (s[i] == ',') {
		  ++i;
		  state = reset;
		} else
		  state = error;
		break;
	      case db:
		if (s[i] == '\\') {
		  ++i;
		  state = dc;
		} else if (s[i] != ',' && s[i] != ';') {
		  addr.append(1, s[i++]);
		  state = ddeaf;
		} else
		  state = error;
		break;
	      case dc:
		addr.append(1, s[i++]);
		state = ddeaf;
		break;
	      case ddeaf:
		if (s[i] == ';') {
		  ++i;
		  state = eb;
		} else if (s[i] == ',') {
		  ++i;
		  state = reset;
		} else if (s[i] == '\\') {
		  ++i;
		  state = de;
		} else
		  addr.append(1, s[i++]);
		break;
	      case de:
		addr.append(1, s[i++]);
		state = ddeaf;
		break;
	      case eb:
		if (s[i] == '\\') {
		  ++i;
		  state = ec;
		} else if (s[i] != ',') {
		  sock.append(1, s[i++]);
		  state = edf;
		} else
		  state = error;
		break;
	      case ec:
		sock.append(1, s[i++]);
		state = edf;
		break;
	      case edf:
		if (s[i] == ',') {
		  ++i;
		  state = reset;
		} else if (s[i] == '\\') {
		  ++i;
		  state = ee;
		} else
		  sock.append(1, s[i++]);
		break;
	      case ee:
		addr.append(1, s[i++]);
		state = edf;
		break;
	      case reset:
		state = a;
	      case accept:
		{
		  iftuple_t	t(name, addr, sock);
		  if (ifacechecks.find(t) != ifacechecks.end())
		    die("Cannot use %s twice\n", name.c_str());
		  ifacechecks.insert(t);
		  ifaces.push_back(t);
		}
		name.clear();
		addr.clear();
		sock.clear();
	      case error:
		break;
	      }

	    } while (state != accept && state != error);

	    if (state != accept)
	      die("Parse error in --interfaces : %s\n", optarg);
	  }
# else // !QOLYESTER_ENABLE_VIRTUAL

	  /*
	   * The following is a determinized finite state automaton to
	   * parse the interface specification argument.  This allows
	   * to specify interface name and optional address to use, in
	   * the following format:
           *
           * ^(([A-Za-z]+[0-9]+)(=(([^,;\\]|\\.)+))?)(,(([A-Za-z]+[0-9]+)(=(([^,;\\]|\\.)+))?))*$
           *
           * Then name is \2 and address is \4 and so on.
	   */

	  {
	    enum State { a, b, cdaf, db, dc, ddf, de, accept, reset, error };

	    State	state = a;

	    std::string	s(optarg);
	    unsigned	i = 0;

	    std::string	name;
	    std::string	addr;

	    do {

	      if (i == s.length()) {
		if (state == cdaf ||
		    state == ddf)
		  state = accept;
		else
		  state = error;
	      }

	      switch (state) {
	      case a:
		if (islower(s[i]) || isupper(s[i])) {
		  name.append(1, s[i++]);
		  state = b;
		} else
		  state = error;
		break;
	      case b:
		if (islower(s[i]) || isupper(s[i]))
		  name.append(1, s[i++]);
		else if (isdigit(s[i])) {
		  name.append(1, s[i++]);
		  state = cdaf;
		} else
		  state = error;
		break;
	      case cdaf:
		if (isdigit(s[i]))
		  name.append(1, s[i++]);
		else if (s[i] == '=') {
		  ++i;
		  state = db;
# ifdef QOLYESTER_ENABLE_MID
		} else if (s[i] == ',') {
		  ++i;
		  state = reset;
# endif
		} else
		  state = error;
		break;
	      case db:
		if (s[i] == '\\') {
		  ++i;
		  state = dc;
# ifdef QOLYESTER_ENABLE_MID
		} else if (s[i] != ',' && s[i] != ';') {
# else
		} else if (s[i] != ';') {
# endif
		  addr.append(1, s[i++]);
		  state = ddf;
		} else
		  state = error;
		break;
	      case dc:
		addr.append(1, s[i++]);
		state = ddf;
		break;
	      case ddf:
		if (s[i] == '\\') {
		  ++i;
		  state = de;
# ifdef QOLYESTER_ENABLE_MID
		} else if (s[i] == ',') {
		  ++i;
		  state = reset;
# endif
		} else
		  addr.append(1, s[i++]);
		break;
	      case de:
		addr.append(1, s[i++]);
		state = ddf;
		break;
	      case reset:
		state = a;
	      case accept:
		{
		  iftuple_t	t(name, addr);
		  if (ifacechecks.find(t) != ifacechecks.end())
		    die("Cannot use %s twice\n", name.c_str());
		  ifacechecks.insert(t);
		  ifaces.push_back(t);
		}
		name.clear();
		addr.clear();
	      case error:
		break;
	      }

	    } while (state != accept && state != error);

	    if (state != accept)
	      die("Parse error in --interfaces : %s\n", optarg);
	  }
# endif
	  break;
# ifdef QOLYESTER_ENABLE_HNA
	case 'o':
	  {
	    char* p = optarg;

	    while (true) {
	      char*	s = index(p, '/');

	      if (s == NULL)
		die("Parse error in --other-networks: %s\n", p);

	      std::string	addr(p, 0, s - p);
	      char*	ptr;
	      unsigned	prefix = strtoul(s + 1, &ptr, 10);

	      if (ptr == s + 1)
		die("Parse error in --other-networks: %s\n", p);

	      gates.push_back(std::pair<std::string, unsigned>(addr, prefix));

	      p = index(p, ',');

	      if (p == NULL)
		break;

	      ++p;
	    }
	  }
	  break;
# endif // ! QOLYESTER_ENABLE_HNA
	case 'w':
	  {
	    char*	p = optarg;
	    willingness = strtoul(p, &p, 10);
	    if (p != optarg && *p == '\0') {
	      if (willingness > WILL_ALWAYS)
		die("Parse error in --willingness: %s\n", optarg);
	      break;
	    }

	    std::string s(optarg);
	    if (s == "never")
	      willingness = WILL_NEVER;
	    else if (s == "low")
	      willingness = WILL_LOW;
	    else if (s == "default")
	      willingness = WILL_DEFAULT;
	    else if (s == "high")
	      willingness = WILL_HIGH;
	    else if (s == "always")
	      willingness = WILL_ALWAYS;
	    else
	      die("Parse error in --willingness: %s\n", optarg);
	  }
	  break;
# ifdef QOLYESTER_ENABLE_TCRED
	case 't':
	  {
	    std::string	s(optarg);

	    if (s == "mprsel")
	      tc_redundancy = mprselset;
	    else if (s == "mprsel+mpr")
	      tc_redundancy = mprselset_mprset;
	    else if (s == "whole")
	      tc_redundancy = wholeset;
	    else
	      die("Parse error in --tc-redundancy: %s\n", optarg);
	  }
	  break;
# endif // ! QOLYESTER_ENABLE_TCRED
# ifdef QOLYESTER_ENABLE_MPRRED
	case 'm':
	  {
	    char*	p = optarg;
	    mprcoverage = strtoul(p, &p, 10);
	    if (p == optarg || *p != '\0')
	      die("Parse error in --mpr-coverage: %s\n", optarg);
	  }
	  break;
# endif // ! QOLYESTER_ENABLE_MPRRED
	case 'h':
	  dump << usage() << std::flush;
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

      argc -= optind;
      argv += optind;

      if (argc > 0) {
	error << "Invalid argument" << (argc > 1 ? "s:" : ":");
	for (int i = 0; i < argc; ++i)
	  error << ' ' << argv[i];
	error << std::endl;
	dump << usage() << std::flush;
	exit(1);
      }

      if (cst::hyst_threshold_high < cst::hyst_threshold_low) {
	error << "Invalid link hysteresis threshold values" << std::endl;
	exit(1);
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
# ifdef DEBUG
	if (debug_file.empty())
	  debug_file = all_file;
# endif
      }

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

      dump_state.rdbuf(dump.rdbuf());
      dump_prof.rdbuf(dump.rdbuf());
      dump_hello.rdbuf(dump.rdbuf());
      dump_tc.rdbuf(dump.rdbuf());
# ifdef QOLYESTER_ENABLE_MID
      dump_mid.rdbuf(dump.rdbuf());
# endif
# ifdef QOLYESTER_ENABLE_HNA
      dump_hna.rdbuf(dump.rdbuf());
# endif

# ifdef DEBUG

      // Prepare the debugtrace logging facility.  We want all
      // possible logging output with timestamps output to a file
      // specified along with the --debug-trace option.

      if (debugtrace) {

	// First, open the output file.

	std::filebuf*	b = new std::filebuf;
	b->open(debugtrace_file.c_str(),
		std::ios_base::out | std::ios_base::app);

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
	    LevelBuf*	lb =					\
	      static_cast<LevelBuf*>(yb->get_streambuf2());	\
	    dump_ ## Name .rdbuf(lb);				\
	    do_dump_ ## Name = true;				\
	  }							\
	} while (false)

       	PROC_DTLOG(state);
	PROC_DTLOG(prof);
	PROC_DTLOG(hello);
	PROC_DTLOG(tc);
# ifdef QOLYESTER_ENABLE_MID
	PROC_DTLOG(mid);
# endif
# ifdef QOLYESTER_ENABLE_HNA
	PROC_DTLOG(hna);
# endif

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

      for (std::list<iftuple_t>::const_iterator i = ifaces.begin();
	   i != ifaces.end(); ++i)
# ifdef QOLYESTER_ENABLE_VIRTUAL
	iface_set.insert(iface_t(ifaceinfo_t(i->name,
					     i->addr,
					     i->sock.empty() ?
					     switch_sockname : i->sock)));
# else // !QOLYESTER_ENABLE_VIRTUAL
	if (i->addr != address_t())
	  iface_set.insert(iface_t(ifaceinfo_t(i->name),
				   i->addr));
	else
	  iface_set.insert(ifaceinfo_t(i->name));
# endif
# ifdef QOLYESTER_ENABLE_HNA
      for (std::list<std::pair<std::string, unsigned> >::const_iterator i =
	     gates.begin();
	   i != gates.end(); ++i)
	gate_set.insert(set::GateEntry(address_t::network(i->first, i->second),
				       i->second));
# endif // ! QOLYESTER_ENABLE_HNA
    }

    void output_syslog()
    {
      // process syslog outputs

      openlog(shortname.c_str(), LOG_NDELAY | LOG_PID, LOG_DAEMON);

      // Redirect all stderr output to syslog with correct priorities
      // If there is a TimestampBuf before the last streambuf, remove
      // it (and destroy it).

# define SPB	StringPrefixBuf
# define PROC_LOG_PREFIXED(Name, Level)					   \
      do {								   \
	if ( Name ## _file.empty()) {					   \
	  std::streambuf*	b;					   \
									   \
	  if(debugtrace) {						   \
	    assert(dynamic_cast<YBuf*>( Name .rdbuf()) != 0);		   \
	    YBuf*	yb = static_cast<YBuf*>( Name .rdbuf());	   \
	    b = yb->get_streambuf1();					   \
	  } else							   \
	    b = Name .rdbuf();						   \
									   \
	  assert(dynamic_cast<LevelBuf*>(b) != 0);			   \
	  LevelBuf*	lb = static_cast<LevelBuf*>(b);			   \
	  assert(dynamic_cast< SPB *>(lb->get_streambuf()) != 0);	   \
	  SPB *	sb = static_cast<SPB *>(lb->get_streambuf());		   \
									   \
	  if (timestamps) {						   \
	    assert(dynamic_cast<TimestampBuf*>(sb->get_streambuf()) != 0); \
	    delete static_cast<TimestampBuf*>(sb->get_streambuf());	   \
	  }								   \
									   \
	  sb->set_streambuf(new utl::SyslogBuf( Level ));		   \
	}								   \
      } while (false)

# define PROC_LOG(Name, Level)						   \
      do {								   \
	if ( Name ## _file.empty()) {					   \
	  std::streambuf*	b;					   \
									   \
	  if(debugtrace) {						   \
	    assert(dynamic_cast<YBuf*>( Name .rdbuf()) != 0);		   \
	    YBuf*	yb = static_cast<YBuf*>( Name .rdbuf());	   \
	    b = yb->get_streambuf1();					   \
	  } else							   \
	    b = Name .rdbuf();						   \
									   \
	  assert(dynamic_cast<LevelBuf*>(b) != 0);			   \
	  LevelBuf*	lb = static_cast<LevelBuf*>(b);			   \
									   \
	  if (timestamps) {						   \
	    assert(dynamic_cast<TimestampBuf*>(lb->get_streambuf()) != 0); \
	    delete static_cast<TimestampBuf*>(lb->get_streambuf());	   \
	  }								   \
									   \
	  lb->set_streambuf(new utl::SyslogBuf( Level ));		   \
	}								   \
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

    // The following is a buffer to be used for std::cerr buffering.

    namespace {
      char cerr_buffer[4096];
    }

    void start_output()
    {
      // First of all, we want to enable std::cerr complete buffering, to
      // avoid having a write(2) issued for each line.  We want to flush
      // explicitly the ostream and have the write(2) issued as rarely as
      // possible.  In fact no buffering happens at streambuf level since
      // std::cerr is ultimately using stderr's buffering, hence the use
      // of setvbuf.

      setvbuf(stderr, cerr_buffer, _IOFBF, sizeof cerr_buffer);

      // We want exceptions to be thrown each time something bad happens
      // with output.  As "bad" we mean "filesystem full".

      error.exceptions(std::ostream::badbit);
      warning.exceptions(std::ostream::badbit);
      notice.exceptions(std::ostream::badbit);
      dump.exceptions(std::ostream::badbit);
      dump_state.exceptions(std::ostream::badbit);
      dump_prof.exceptions(std::ostream::badbit);
      dump_hello.exceptions(std::ostream::badbit);
      dump_tc.exceptions(std::ostream::badbit);
      dump_mid.exceptions(std::ostream::badbit);
      dump_hna.exceptions(std::ostream::badbit);
#ifdef DEBUG
      debug.exceptions(std::ostream::badbit);
#endif

      atexit(stop_output);
    }

    void stop_output()
    {
      error.exceptions(std::ostream::goodbit);
      warning.exceptions(std::ostream::goodbit);
      notice.exceptions(std::ostream::goodbit);
      dump.exceptions(std::ostream::goodbit);
      dump_state.exceptions(std::ostream::goodbit);
      dump_prof.exceptions(std::ostream::goodbit);
      dump_hello.exceptions(std::ostream::goodbit);
      dump_tc.exceptions(std::ostream::goodbit);
      dump_mid.exceptions(std::ostream::goodbit);
      dump_hna.exceptions(std::ostream::goodbit);
#ifdef DEBUG
      debug.exceptions(std::ostream::goodbit);
#endif
    }

  } // namespace utl

} // namespace olsr

#endif // ! QOLYESTER_DAEMON_UTL_ARGS_HXX
