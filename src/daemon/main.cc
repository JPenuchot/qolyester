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

///
/// @file   main.cc
/// @author Ignacy Gawedzki
/// @date   Thu Sep 25 20:37:50 2003
///
/// @brief As its name suggests, this is the file that contains the
/// main function. =)
///
///


#include "config.hh"

#include "sch/scheduler.hh"
#include "sch/events.hh"
#include "utl/args.hh"
#include "utl/syslog.hh"

#include "cst/params.hcc"
#include "utl/log.hcc"

#include "daemon_static_members_init.hcc"
#include "static_members_init.hcc"

namespace olsr {

  // These are the separate ostreams for several different and
  // independently selectable dump outputs.  They all share the same
  // streambuf initially, before command line arguments are parsed.

  std::ostream  dump_state(dump.rdbuf());
  std::ostream  dump_prof(dump.rdbuf());
  std::ostream  dump_hello(dump.rdbuf());
  std::ostream  dump_tc(dump.rdbuf());
  std::ostream  dump_mid(dump.rdbuf());
  std::ostream  dump_hna(dump.rdbuf());

  // The only instance of Scheduler.

  sched_t	scheduler(*new sch::QolyesterLoopHandler);

} // namespace olsr

#include "gra/graph.hcc"
#include "set/duplicate.hcc"
#include "set/gate.hcc"
#include "set/hna.hcc"
#include "set/interfaces.hcc"
#include "set/mid.hcc"
#include "set/neighbors.hcc"
#include "set/routes.hcc"
#include "set/topology.hcc"

///
/// @namespace olsr
/// @brief	The main Qolyester namespace
///

namespace olsr {

  // Several markers used throughout the code.

  utl::Mark	terminate_now;
  utl::Mark	routes_recomp;
  utl::Mark	mprset_recomp;
  utl::Mark	advset_changed;

  // Standard names for the current program, to be used for output.

  const std::string	progname = "Qolyester";
  const std::string	shortname = "qolsrd";

  // Current version of the program, to be used for output.  In case
  // the program was distributed as a snapshot, also indicates the
  // snapshot date.

#ifdef QOLYESTER_SNAPSHOT_VERSION
  const std::string	version = VERSION "-" QOLYESTER_SNAPSHOT_VERSION;
#else
  const std::string	version = VERSION;
#endif

  // Pointers to several timed events.

  sch::TimedEvent*	hello_sender  = 0;
  sch::TimedEvent*	tc_sender     = 0;
  sch::TimedEvent*	mid_sender    = 0;
  sch::TimedEvent*	hna_sender    = 0;
  sch::TimedEvent*	state_printer = 0;

} // namespace olsr

using namespace olsr;

/// This is the main function.
///
/// @arg argc command line argument count.
/// @arg argv command line argument vector.
///
/// @return 0 for normal termination, 1 for error.
///

int main(int argc, char *argv[])
{
  utl::start_output();

  // Parse the command line arguments, and stop if there was any
  // problem during this operation.

  try {
    try {
      utl::parse_args(argc, argv);
    } catch (errnoexcept_t& e) {
      if (e.get_errno() == EPERM || e.get_errno() == EACCES)
	error << "Insufficient permissions to run "
	      << progname << '.' << std::endl;
      else
	throw;
      exit(1);
    }
  } catch (std::ofstream::failure&) {
    std::string	msg = strerror(errno);
    try {
      error << "Initialization failed:\n  " << msg << std::endl;
    } catch (std::ofstream::failure&) {
      std::cerr << "ERROR: Initialization failed:\nERROR:   " << msg << '\n'
		<< "ERROR: " << strerror(errno) << std::endl;
    }
    exit(1);
  } catch (std::exception& e) {
    try {
      error << "Initialization failed:\n  " << e.what() << std::endl;
    } catch (std::ofstream::failure&) {
      std::cerr << "ERROR: Initialization failed:\nERROR:   "
		<< e.what() << std::endl;
      std::cerr << "ERROR: " << strerror(errno) << std::endl;
    }
    exit(1);
  }

  try {

    // If no -i or --interfaces flag was given, stop here.

    if (iface_set.empty()) {
      error << "No OLSR interfaces specified, please use the -i option."
	    << std::endl;
      exit(1);
    }

    // Do some friendly output.

    dump << up << "Using the following OLSR interfaces:\n"
	 << indent;

    for (ifaceset_t::const_iterator i = iface_set.begin();
	 i != iface_set.end(); ++i)
      dump << i->addr() << '/' << i->prefix()
	   << " (name: " << i->info().name()
	   << ", index: " << i->info().index() << ")\n";

    dump << deindent;

    dump << "Using " << main_addr << " as main address." << std::endl << down;

    if (gate_set.empty())
      dump << up << "Gatewaying no external network." << std::endl << down;
    else {
      dump << up << "Gatewaying the following non-OLSR networks:\n"
	   << indent;
      for (gateset_t::iterator i = gate_set.begin();
	   i != gate_set.end(); ++i)
	dump << i->net_addr() << '/' << i->prefix() << '\n';
      dump << deindent << std::endl << down;
    }

    // Flush any routes that may have remained from a previous run.
    // During normal operation, routes are cleaned before exit, but the
    // last run may have ended tragically. =)

    alg::clean_routes();

    // Going background

    if (!nodetach) {
      pid_t	pid = fork();
      if (pid < 0)
	error << "Could not fork: " << strerror(errno) << std::endl;
      if (pid != 0) {
	// We are the parent.
# ifdef QOLYESTER_ENABLE_VIRTUAL
	// Socket lock prevents it from unlinking the socket from the
	// filesystem during closing.  The forked socket should not
	// remove the socket from the filesystem since the child is
	// responsible for it.

	sys::UnixSocket::lock();
# endif
	return 0;
      } else {
	// We are the child.

	if (!use_syslog)
	  utl::output_syslog();

	close(0);
	close(1);
	close(2);
	setsid();
      }
    }

    // Configure each interface.

    for (ifaceset_t::iterator i = iface_set.begin();
	 i != iface_set.end(); ++i)
      const_cast<iface_t&>(*i).configure();

    typedef msg::HELLOMessage	hello_t;
    typedef msg::TCMessage	tc_t;
    typedef msg::MIDMessage	mid_t;
    typedef msg::HNAMessage	hna_t;

    // Add the periodic TC message sender.
    scheduler.insert(tc_sender = new sch::MessageSender<tc_t>);

    // Add the periodic HELLO message sender.
    scheduler.insert(hello_sender = new sch::MessageSender<hello_t>);

    // Add the periodic MID message sender if using more than one interface.
    if (iface_set.size() > 1)
      scheduler.insert(mid_sender = new sch::MessageSender<mid_t>);

    // Add the periodic HNA message sender if gatewaying some external network.
    if (!gate_set.empty())
      scheduler.insert(hna_sender = new sch::MessageSender<hna_t>);

    // Add the periodic state printer, if so requested.
    if (do_dump_state)
      scheduler.insert(state_printer = new sch::StatePrinter);

    // Enter the scheduler loop.
    scheduler.loop();

    // Be polite.
    dump << up << "Thanks for using " << progname << ".  Have a nice day."
	 << std::endl << down;

    sch::remove_routes();

  } catch (std::ofstream::failure&) {
    try {
      error << strerror(errno) << std::endl;
    } catch (std::ofstream::failure&) {
      std::cerr << "ERROR: " << strerror(errno) << std::endl;
    }
  } catch (std::runtime_error& e) {
    try {
      error << e.what() << std::endl;
    } catch (std::ofstream::failure&) {
      if (nodetach) {
	std::cerr << "ERROR: " << e.what() << '\n';
	std::cerr << "ERROR: " << strerror(errno) << std::endl;
      }
    }
    sch::remove_routes();
    exit(1);
  } catch (std::exception& e) {
    if (nodetach)
      std::cerr << "ERROR: " << e.what() << std::endl;
    sch::remove_routes();
    exit(1);
  }

  return 0;
}
