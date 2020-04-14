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
/// @brief As its name suggests, this is the file that containes the
/// main function. =)
///
///

#include <syslog.h>
#include <cstdio>

#include "config.hh"

#include "prs/data.hh"
#include "prs/parse.hh"
#include "set/client.hh"
#include "utl/args.hh"
#include "utl/log.hh"
#include "utl/syslog.hh"

#include "cst/params.hcc"
#include "utl/log.hcc"

#include "static_members_init.hcc"

namespace olsr {

  namespace prs {

    namespace dot {

      template TagAttrStmt<GraphTag>::TagAttrStmt(const Location&,
						  std::list<AttrAssignment*>*);

      template TagAttrStmt<NodeTag>::TagAttrStmt(const Location&,
						 std::list<AttrAssignment*>*);

      template TagAttrStmt<EdgeTag>::TagAttrStmt(const Location&,
						 std::list<AttrAssignment*>*);

    } // namespace dot

  } // namespace prs

  namespace gra {

    AdjInfo	AdjInfo::_dummy_for_find;
    AdjNode	AdjNode::_dummy_for_find;

  } // namespace gra

  struct deleter {
    void operator()(clientmap_t::value_type& v) const {
      delete v.second;
    }
  };

  std::ostream  dump_state(dump.rdbuf());
  std::ostream  dump_prof(dump.rdbuf());

  sch::Scheduler	scheduler(*new sch::SwitchLoopHandler);

} // namespace olsr

#include "gra/graph.hcc"

namespace olsr {

  utl::Mark	terminate_now;

  const std::string	progname = "Qolyester Switch";
  const std::string	shortname = "qswitchd";

# ifdef QOLYESTER_SNAPSHOT_VERSION
  const std::string	version = VERSION "-" QOLYESTER_SNAPSHOT_VERSION;
# else
  const std::string	version = VERSION;
# endif

  faddrset_t	faddr_set;

  clientmap_t	client_map;

  sys::UnixSocket	switch_socket;

  timeval_t	start_time(timeval_t::now());

} // namespace olsr

using namespace olsr;

namespace {
  char	cerr_buffer[4096];
}

/// This is the main function.
///
/// @arg argc command line argument count.
/// @arg argv command line argument vector.
///
/// @return 0 for normal termination, 1 for error.
///
int main(int argc, char *argv[])
{
  setvbuf(stderr, cerr_buffer, _IOFBF, sizeof cerr_buffer);

  error.exceptions(std::ostream::badbit);
  warning.exceptions(std::ostream::badbit);
  notice.exceptions(std::ostream::badbit);
  dump.exceptions(std::ostream::badbit);
  dump_state.exceptions(std::ostream::badbit);
  dump_prof.exceptions(std::ostream::badbit);
#ifdef DEBUG
  debug.exceptions(std::ostream::badbit);
#endif

  try {
    utl::parse_args(argc, argv);
  } catch (errnoexcept_t& e) {
    if (e.get_errno() == EPERM || e.get_errno() == EACCES)
      error << "Insufficient persmissions to run "
	    << progname << '.' << std::endl;
    else
      error << "Initialization failed: \n  " << e.what() << std::endl;
    exit(1);
  } catch (std::runtime_error& e) {
    error << "Initialization failed: \n  " << e.what() << std::endl;
    exit(1);
  }

  prs::dot::Graph*	g = 0;

  try {
    g = prs::dot::parse(argv[0]);
  } catch (std::runtime_error& e) {
    //    delete yylval.
    error << e.what() << std::endl;
    exit(2);
  }

  try {
    prs::dot::InitVisitor	v(path_net, faddr_set);
    g->accept(v);
  } catch (std::runtime_error& e) {
    delete g;
    error << e.what() << std::endl;
    exit(3);
  }

  delete g;

  debug << "Loaded graph from " << argv[0] << '\n'
	<< path_net << std::endl;

  try {
    switch_socket.bind(switch_sockname);
    switch_socket.listen();
  } catch (std::runtime_error& e) {
    error << e.what() << std::endl;
    exit(3);
  }


  // Going background

  if (!nodetach) {
    pid_t	pid = fork();
    if (pid < 0)
      error << "Could not fork: " << strerror(errno) << std::endl;
    if (pid != 0) {
      // We are the parent.
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

  scheduler.insert(new sch::ConnectionAccepter());

  try {
    scheduler.loop();
  } catch (std::runtime_error& e) {
    try {
      error << e.what() << std::endl;
    } catch (std::runtime_error& ee) {
      std::cerr << e.what() << '\n';
      std::cerr << ee.what() << std::endl;
    }
    return 1;
  }

  try {
    switch_socket.close();
  } catch (std::runtime_error& e) {
    try {
      error << e.what() << std::endl;
    } catch (std::runtime_error& ee) {
      if (nodetach) {
	std::cerr << e.what() << '\n';
	std::cerr << ee.what() << std::endl;
      }
    }
    return 1;
  }

  std::for_each(client_map.begin(), client_map.end(), deleter());

  dump << up << "Thanks for using " << progname << ", have a nice day."
       << std::endl << down;

  return 0;
}

