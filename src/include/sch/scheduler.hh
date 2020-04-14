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

///
/// @file   scheduler.hh
/// @author Ignacy Gawedzki
/// @date   Fri Sep 26 16:06:29 2003
///
/// @brief  The declarations of the events and the scheduler.
///
/// The global scheme goes like this:
/// - For I/O events, use poll() in a loop
/// - For timed events, use poll()'s timeout.
///

#ifndef QOLYESTER_SCH_SCHEDULER_HH
# define QOLYESTER_SCH_SCHEDULER_HH 1

# include <sys/poll.h>
# include <string>
# include <set>
# include "config.hh"
# include "utl/meta.hh"
# include "utl/timeval.hh"

namespace olsr {

  namespace sch {

    class LoopHandler {
    public:
      virtual ~LoopHandler() {}
      virtual inline bool	operator()() const;
    };

    ///
    /// @class Event
    /// @brief	Abstract event class
    ///
    class Event {
    public:
      ///
      /// Event constructor.
      ///
# ifdef DEBUG
      inline Event(const std::string& name);
# else // !DEBUG
      inline Event();
# endif

      virtual ~Event() {}

      ///
      /// Virtual handling method.
      ///
      virtual void	handle() = 0;

# ifdef DEBUG
      const std::string&	name() const { return name_; }
# else // !DEBUG
      null_t			name() const { return null_t(); }
# endif
    private:
# ifdef DEBUG
      const std::string	name_; ///< Event name
# endif // !DEBUG
    };

    ///
    /// @class TimedEvent
    /// @brief	Timed event abstract class.
    ///
    class TimedEvent : public Event {
      typedef TimedEvent	This;
      typedef Event		Super;
    public:
      ///
      /// Constructor.
      ///
      /// @arg next time of next triggering
      /// @arg h pointer to handler object
      ///
# ifdef DEBUG
      TimedEvent(const timeval_t& n, const std::string& name)
	: Super(name),
	  next_(n)
      {}
# else // !DEBUG
      TimedEvent(const timeval_t& n)
	: Super(),
	  next_(n)
      {}
# endif

      ///
      /// Destructor.
      ///
      virtual ~TimedEvent() {}

      ///
      /// Next iteration accessor.
      ///
      /// @return reference to next time of triggering
      ///
      const timeval_t&	next() const {	return next_; }
      void		set_next(const timeval_t& n) { next_ = n; }

    protected:
      timeval_t		next_;	///< Next time of triggering
    };

    namespace upd {

      template <class S, class I = typename S::iterator>
      struct SetEraser {
	SetEraser(S& s, const I& i)
	  : s_(s),
	    i_(i)
	{}
	void	operator()() {
	  s_.erase(i_);
	}
      private:
	S&	s_;
	I		i_;
      };

    } // namespace upd

    template <class F>
    class UpdateEvent : public TimedEvent {
      typedef UpdateEvent<F>	This;
      typedef TimedEvent	Super;
    public:
# ifdef DEBUG
      UpdateEvent(const timeval_t& n, const F& f)
	: Super(n, "UpdateEvent"),
	  f_(f)
      {}
# else // !DEBUG
      UpdateEvent(const timeval_t& n, const F& f)
	: Super(n),
	  f_(f)
      {}
# endif
      virtual void	handle() {
	f_();
      }
    private:
      F	f_;
    };

    namespace internal {

      struct TrivialIncrement {
	void	operator()(timeval_t& n, timeval_t& p) const {
	  n += p;
	}
      };

    } // namespace internal

    template <class I = internal::TrivialIncrement>
    class PeriodicEvent_ : public TimedEvent {
      typedef PeriodicEvent_<I>	This;
      typedef TimedEvent		Super;
    public:
# ifdef DEBUG
      PeriodicEvent_(const timeval_t& n, const timeval_t& p,
		     const std::string& name)
	: Super(n, name),
	  period_(p)
      {}
      PeriodicEvent_(const timeval_t& n, const timeval_t& p, const I& i,
		     const std::string& name)
	: Super(n, name),
	  period_(p),
	  increment_(i)
      {}
# else // !DEBUG
      PeriodicEvent_(const timeval_t& n, const timeval_t& p)
	: Super(n),
	  period_(p)
      {}
      PeriodicEvent_(const timeval_t& n, const timeval_t& p, const I& i)
	: Super(n),
	  period_(p),
	  increment_(i)
      {}
# endif
      virtual void	handle();
    private:
      timeval_t	period_;
      I		increment_;
    };

    typedef PeriodicEvent_<>	PeriodicEvent;

    namespace internal {

      struct JitterIncrement {
	JitterIncrement()
	  : jitter_(0)
	{}
	JitterIncrement(const timeval_t& j)
	  : jitter_(j)
	{}
	void	operator()(timeval_t& n, timeval_t& p) const {
	  timeval_t	j = timeval_t::jitter(jitter_);
	  if (j > p)
	    return;
	  n += p - j;
	}
      private:
	timeval_t	jitter_;
      };

    } // namespace internal

    class JitteredEvent : public PeriodicEvent_<internal::JitterIncrement> {
      typedef JitteredEvent				This;
      typedef PeriodicEvent_<internal::JitterIncrement>	Super;
    public:
# ifdef DEBUG
      JitteredEvent(const timeval_t& n, const timeval_t& p, const timeval_t& j,
		    const std::string& name)
	: Super(n, p, j, name)
      {}
# else // !DEBUG
      JitteredEvent(const timeval_t& n, const timeval_t& p, const timeval_t& j)
	: Super(n, p, j)
      {}
# endif
    };

    template <class> class PollEvent_;
    namespace internal {
      struct PollData;
    }

    typedef PollEvent_<internal::PollData>	IOEvent;

    namespace internal {

      struct PollData {
	PollData(int fd, short events) {
	  pfd.fd = fd;
	  pfd.events = events;
	  pfd.revents = 0;
	}

	bool	active() const {
	  return (pfd.events & pfd.revents) != 0;
	}

	bool	operator<(const PollData& rhs) const {
	  if (active() != rhs.active())
	    return active();

	  const int	apri = priority(pfd);
	  const int	bpri = priority(rhs.pfd);

	  if (apri != bpri)
	    return apri > bpri;
	  else
	    return pfd.fd < rhs.pfd.fd;
	}

	::pollfd	pfd;

	template <class C, class A>
	static inline void	poll(std::multiset<IOEvent*, C, A>& s,
				     int timeout = -1);
      private:

	static int	priority(const ::pollfd& ufd)
	{
	  int r (0);

	  if (ufd.events & POLLOUT)
	    ++r;
	  if (ufd.events & POLLIN)
	    --r;
	  if (ufd.events & POLLPRI)
	    --r;

	  return r;
	}

      };

    } // namespace internal

    template <class P = internal::PollData>
    class PollEvent_ : public Event {
      typedef PollEvent_<P>	This;
      typedef Event		Super;
    public:
      typedef P		p_t;
# ifdef DEBUG
      PollEvent_(const P& p, const std::string& name)
	: Super(name),
	  p_(p)
      {}
# else // !DEBUG
      PollEvent_(const P& p)
	: Super(),
	  p_(p)
      {}
# endif

      virtual ~PollEvent_() {}

      const P&	p() const { return p_; }
      P&	p() { return p_; }
    private:
      P		p_;
    };

    typedef PollEvent_<>	IOEvent;

    ///
    /// @fn void term_handler(int)
    /// @brief	Termination signals handler function.
    ///
    void		term_handler(int);
    void		int_handler(int);
    void		quit_handler(int);

    ///
    /// @class Scheduler
    /// @brief	The scheduler class.
    ///
    class Scheduler {
      typedef Scheduler	This;

      struct tevent_less {
	bool operator()(const TimedEvent* a, const TimedEvent* b) const {
	  return a->next() < b->next();
	}
      };

      struct ioevent_less {
	bool operator()(const IOEvent* a, const IOEvent* b) const {
	  return a->p() < b->p();
	}
      };

    public:
      typedef std::multiset<TimedEvent*, tevent_less>	teventset_t;

      typedef std::multiset<IOEvent*, ioevent_less>	ioeventset_t;

      ///
      /// Constructor.
      ///
      inline Scheduler(LoopHandler&);
      inline ~Scheduler();
    private:
      inline Scheduler(const This& other);
      inline This& operator=(const This& other);

    public:
      ///
      /// Timed event registering method.  Adds the pointer to the timed
      /// events set and resets the timer.
      ///
      /// @arg e pointer to timed event to register
      ///
      inline void		insert(TimedEvent* e);

      inline void		erase(TimedEvent* e);

      ///
      /// I/O event registering method.  Add the pointer to the I/O
      /// events set.
      ///
      /// @arg e pointer to I/O event to register
      ///
      inline void		insert(IOEvent* e);

      inline void		erase(IOEvent* e);

      template <class E>
      inline void		destroy(E* e);

      bool		to_delete() const { return delete_; }
    private:
      ///
      /// Timed events handling method.  To be called from the event
      /// loop.
      ///
      inline void	handle_tevents();

      ///
      /// I/O events handling method.  To be called from the event
      /// loop.
      ///
      inline void	handle_ioevents();
    public:

      ///
      /// The main event loop of the scheduler.
      ///
      inline void	loop();

    private:
      teventset_t	tevent_set_;	///< Set of timed events
      ioeventset_t	ioevent_set_;	///< Set of I/O events
      LoopHandler&	loophandler_;
      const Event*	current_event_; ///< Currently processed event
      bool		delete_;
    };

    template <class E>
    class Updatable : public E {
      typedef Updatable<E>	This;
      typedef E			Super;
    public:
      Updatable(const E& e)
	: Super(e),
	  updater_(0)
      {}

      Updatable& operator = (const E& rhs)
      {
	static_cast<E&> (*this) = rhs;
	return *this;
      }

      void set_updater(TimedEvent* u) {
	updater_ = u;
      }
      TimedEvent*	updater() const { return updater_; }
    private:
      TimedEvent*	updater_;
    };

  } // namespace sch

  typedef sch::Scheduler	sched_t;

} // namespace olsr

# include "scheduler.hxx"

#endif // ! QOLYESTER_SCH_SCHEDULER_HH
