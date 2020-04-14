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

///
/// @file   scheduler.hxx
/// @author Ignacy Gawedzki
/// @date   Sat Sep 27 14:01:25 2003
///
/// @brief  Scheduler implementation.
///
#ifndef QOLYESTER_SCH_SCHEDULER_HXX
#define QOLYESTER_SCH_SCHEDULER_HXX 1

# include <unistd.h>
# include <signal.h>
# include <sys/times.h>
# include <cassert>
# include <cstring>
# include <cerrno>
# include <map>
# include "scheduler.hh"
# include "utl/mark.hh"
# include "utl/log.hh"

namespace olsr {

  extern std::ostream		dump_prof;
  extern debug_ostream_t	debug;
  extern utl::Mark		terminate_now;
  extern bool			do_dump_prof;
  extern sched_t		scheduler;

  namespace sch {

    bool
    LoopHandler::operator()() const {
      if (terminate_now.mark())
	return true;
      return false;
    }

# ifdef DEBUG
    Event::Event(const std::string& name)
      : name_(name)
    {}
# else // !DEBUG
    Event::Event()
    {}
# endif

    template <class I>
    void
    PeriodicEvent_<I>::handle() {
      do
	increment_(next_, period_);
      while (next_.is_past());
      scheduler.insert(this);
    }

    namespace internal {

      template <class C, class A>
      void
      PollData::poll(std::multiset<IOEvent*, C, A>& s, int timeout) {
	typedef std::multiset<IOEvent*, C, A>	set_t;
	::pollfd		ufds[s.size()];
	typename set_t::iterator	ievs[s.size()];

	unsigned	counter = 0;

	for (typename set_t::iterator i = s.begin();
	     i != s.end(); ++i, ++counter) {
	  ufds[counter] = (*i)->p().pfd;
	  ufds[counter].revents = 0;
	  ievs[counter] = i;
	}

	int	ret = ::poll(ufds, s.size(), timeout);

	if (ret < 0 && errno != EINTR)
	  error << "poll(" << s.size()
		<< "): " << strerror(errno) << std::endl;

	assert(ret >= 0 || (ret < 0 && errno == EINTR));

	std::map<int, bool>	input_already;
	std::map<int, bool>	output_already;

	if (ret > 0) {
	  for (unsigned i = 0; i < counter; ++i) {
	    assert(ufds[i].events == (*ievs[i])->p().pfd.events);
	    if (ufds[i].events & ufds[i].revents) {
	      IOEvent*	e = *ievs[i];
	      s.erase(ievs[i]);
	      e->p().pfd = ufds[i];
	      if ((ufds[i].revents & POLLIN) != 0) {
		if (input_already[ufds[i].fd])
		  e->p().pfd.revents &= ~POLLIN;
		else
		  input_already[ufds[i].fd] = true;
	      }
	      if ((ufds[i].revents & POLLOUT) != 0) {
		if (output_already[ufds[i].fd])
		  e->p().pfd.revents &= ~POLLOUT;
		else
		  output_already[ufds[i].fd] = true;
	      }
	      s.insert(e);
	    }
	  }
	}
      }


      void	(*old_term_handler)(int) = SIG_IGN;
      void	(*old_int_handler)(int)  = SIG_IGN;
      void	(*old_quit_handler)(int) = SIG_IGN;

      void	install_sighandlers()
      {
# define GET_OLD_HANDLER(Signal, SmallSignal) \
        do { \
          struct sigaction	sa; \
          int			ret; \
          while ((ret = ::sigaction(Signal, NULL, &sa)) < 0 && \
                 errno == EINTR); \
          assert(ret == 0); \
          old_ ## SmallSignal ## _handler = sa.sa_handler; \
        } while (0)

        GET_OLD_HANDLER(SIGTERM, term);
	GET_OLD_HANDLER(SIGINT,  int);
	GET_OLD_HANDLER(SIGQUIT, quit);

# define PUT_NEW_HANDLER(Signal, SmallSignal) \
        do { \
          if (old_ ## SmallSignal ## _handler != SIG_IGN) { \
	    struct sigaction	sa; \
            memset(&sa, 0, sizeof sa); \
	    sa.sa_handler = SmallSignal ## _handler; \
            sigemptyset(&sa.sa_mask); \
            sa.sa_flags = SA_RESTART; \
            int			ret; \
	    while ((ret = ::sigaction(Signal, &sa, NULL)) < 0 && \
		   errno == EINTR); \
	    assert(ret == 0); \
	  } \
	} while (0)

	PUT_NEW_HANDLER(SIGTERM, term);
	PUT_NEW_HANDLER(SIGINT,  int);
	PUT_NEW_HANDLER(SIGQUIT, quit);

	{
	  struct sigaction	sa;
	  memset(&sa, 0, sizeof sa);
	  sa.sa_handler = SIG_IGN;
	  sa.sa_flags = 0;
	  int			ret;
	  while ((ret = ::sigaction(SIGPIPE, &sa, NULL)) < 0 and
		 errno == EINTR)
	    ;
	  assert(ret == 0);
	}
      }

    } // namespace internal

    // The scheduler ctor, which actually sets the signal handlers.  Not
    // only do we set the SIGALRM handler, but also termination
    // handlers, to clean up the routes before quitting.
    Scheduler::Scheduler(LoopHandler& lh)
      : loophandler_(lh),
	current_event_(0) {
      internal::install_sighandlers();
    }

    Scheduler::~Scheduler() {
      while (!ioevent_set_.empty())
	destroy(*ioevent_set_.begin());
      while (!tevent_set_.empty())
	destroy(*tevent_set_.begin());
    }

    // The registering methods.  The event has to be instantiated with
    // the new operator, as expired events are deleted.
    void
    Scheduler::insert(TimedEvent* e) {
      assert(e != 0);
      tevent_set_.insert(e);
      debug << up << "Registering TimedEvent " << e->name()
	    << ' ' << e << " to " << e->next().diff()
	    << std::endl << down;
    }

    void
    Scheduler::erase(TimedEvent* e) {
      assert(e != 0);
      typedef teventset_t::iterator	iter_t;
      std::pair<iter_t, iter_t>		ep = tevent_set_.equal_range(e);

      for (iter_t i = ep.first; i != ep.second;)
	if (*i == e) {
	  debug << up << "Unregistering " << e->name()
		<< ' ' << e << " to " << e->next().diff()
		<< std::endl << down;
	  iter_t	tmp = i++;
	  tevent_set_.erase(tmp);
	} else
	  ++i;
    }

    template <class E>
    void
    Scheduler::destroy(E* e) {
      erase(e);
      if (current_event_ == e)
	delete_ = true;
      else
	delete e;
    }

    // For the moment, we don't need to remove I/O events from the set,
    // so we don't delete them at this time.  Nevertheless, avoid
    // passing pointers to objects that should not be deleted, as this
    // may change in the future.
    void
    Scheduler::insert(IOEvent* e) {
      assert(e != 0);
      debug << up << "Registering IOEvent " << e->name()
	    << ' ' << e << std::endl << down;
      ioevent_set_.insert(e);
    }

    void
    Scheduler::erase(IOEvent* e) {
      assert(e != 0);
      typedef ioeventset_t::iterator	iter_t;
      std::pair<iter_t, iter_t>		ep = ioevent_set_.equal_range(e);

      for (iter_t i = ep.first; i != ep.second;)
	if (*i == e) {
	  iter_t	tmp = i++;
	  debug << up << "Unregistering IOEvent " << e->name()
		<< ' ' << e << std::endl << down;
	  ioevent_set_.erase(tmp);
	} else
	  ++i;
    }

    // This is where the handlers of timed events are called and the
    // timer reset.
    void
    Scheduler::handle_tevents() {
# ifdef DEBUG
      debug << up(2) << "TimedEvents before {\n" << indent;
      for (teventset_t::iterator i = tevent_set_.begin();
	   i != tevent_set_.end(); ++i)
	debug << (*i)->name() << ' ' << *i << ' ' << (*i)->next().diff()
	      << ((*i)->next().is_past() ? " A\n" : "\n");
      debug << deindent << '}' << std::endl << down(2);
# endif // !DEBUG

      // Iterate on the event set.
      while (!tevent_set_.empty()) {
	TimedEvent*		e = *tevent_set_.begin();

	if (e->next() > timeval_t::now())
	  break;

	current_event_ = e;
	tevent_set_.erase(tevent_set_.begin());

	delete_ = false;
	debug << up << "Handling TimedEvent " << e->name()
	      << ' ' << e << " with " << e->next().diff()
	      << std::endl << down;
	e->handle();
	if (delete_)
	  delete e;
	current_event_ = 0;
      }

# ifdef DEBUG
      debug << up(2) << "TimedEvents after {\n" << indent;
      for (teventset_t::iterator i = tevent_set_.begin();
	   i != tevent_set_.end(); ++i)
	debug << (*i)->name() << ' ' << *i << ' ' << (*i)->next().diff()
	      << ((*i)->next().is_past() ? " A\n" : "\n");
      debug << deindent << '}' << std::endl << down(2);
# endif // !DEBUG
    }

    // This is where I/O event handlers are called.
    void
    Scheduler::handle_ioevents() {
      // Iterate on the event set.
# ifdef DEBUG
      debug << up(2) << "IOEvents before {\n" << indent;
      for (ioeventset_t::iterator i = ioevent_set_.begin();
	   i != ioevent_set_.end(); ++i)
	debug << (*i)->name() << ' ' << *i << ' '
	      << ((*i)->p().active() ? " A\n" : "\n");
      debug << deindent
	    << '}' << std::endl << down(2);
# endif // !DEBUG

      while (!ioevent_set_.empty()) {
	IOEvent*	e = *ioevent_set_.begin();

	if (!e->p().active())
	  break;

	current_event_ = e;
	ioevent_set_.erase(ioevent_set_.begin());

	delete_ = false;
	debug << up << "Handling IOEvent " << e->name()
	      << ' ' << e << std::endl << down;
	e->handle();
	if (delete_)
	  delete e;
	current_event_ = 0;
      }

# ifdef DEBUG
      debug << up(2) << "IOEvents after {\n" << indent;
      for (ioeventset_t::iterator i = ioevent_set_.begin();
	   i != ioevent_set_.end(); ++i)
	debug << (*i)->name() << ' ' << *i << ' '
	      << ((*i)->p().active() ? " A\n" : "\n");
      debug << deindent
	    << '}' << std::endl << down(2);
# endif // !DEBUG

    }

    // The main scheduler event loop
    void
    Scheduler::loop() {

//       sys::InterfaceInfo	info;

      // The loop /per se/
      while (true) {

	// Some profiling declarations
	::tms	t;
	if (do_dump_prof)
	  ::times(&t);

	if (tevent_set_.empty())
	  IOEvent::p_t::poll(ioevent_set_);
	else if ((*tevent_set_.begin())->next().is_past())
	  IOEvent::p_t::poll(ioevent_set_, 0);
	else
	  IOEvent::p_t::poll(ioevent_set_, ((*tevent_set_.begin())->next() -
					    timeval_t::now()).poll_time());

	// Synchronize local clock.
	timeval_t::set_now();
	// Handle all the events.
	handle_ioevents();
	handle_tevents();

	if (loophandler_())
	  break;

	// Spit out profiling info if requested.
	if (do_dump_prof) {
	  ::tms	nt;
	  ::times(&nt);

	  dump_prof << "Seconds spent computing: ";
	  dump_prof.precision(3);
	  dump_prof.flags(std::ios::fixed);
	  if (nt.tms_utime >= t.tms_utime)
	    dump_prof << (float) (nt.tms_utime - t.tms_utime) /
	      sysconf(_SC_CLK_TCK)
		      << std::endl;
	  else
	    dump_prof << (float)
              (std::numeric_limits<long>::max() - nt.tms_utime + t.tms_utime) /
	      sysconf(_SC_CLK_TCK)
		      << std::endl;
	}
      }

    }

    // The sighandler for termination
    void term_handler(int)
    {
      terminate_now.set_mark();
    }

    void int_handler(int)
    {
      terminate_now.set_mark();
    }

    void quit_handler(int)
    {
      ::signal(SIGQUIT, SIG_DFL);
      ::raise(SIGQUIT);
    }

  } // namespace sch

} // namespace olsr

#endif // ! QOLYESTER_SCH_SCHEDULER_HXX
