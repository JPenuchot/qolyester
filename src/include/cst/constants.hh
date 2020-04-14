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

#ifndef QOLYESTER_CST_CONSTANTS_HH
# define QOLYESTER_CST_CONSTANTS_HH 1

# include "config.hh"
# include "utl/timeval.hh"

namespace olsr {

  namespace cst {

# define OLSR_PORT_NUMBER	698
# define C_CONSTANT		.0625f

// Protocol constant for rtnetlink, should be standardized
# define RTPROT_OLSR		15

// MPR Willingness
# define WILL_NEVER	0
# define WILL_LOW	1
# define WILL_DEFAULT	3
# define WILL_HIGH	6
# define WILL_ALWAYS	7

// Link status
# define UNSPEC_LINK	0
# define ASYM_LINK	1
# define SYM_LINK	2
# define LOST_LINK	3
# define LOWEST_LINK	UNSPEC_LINK
# define HIGHEST_LINK	LOST_LINK

// Neighbor status
# define NOT_NEIGH	0
# define SYM_NEIGH	1
# define MPR_NEIGH	2
# define LOWEST_NEIGH	NOT_NEIGH
# define HIGHEST_NEIGH	MPR_NEIGH

// Message types
# define HELLO_MESSAGE	1
# define TC_MESSAGE	2
# define MID_MESSAGE	3
# define HNA_MESSAGE	4

# ifdef QOLYESTER_ENABLE_LINKHYS
// Link hysteresis
#  ifdef QOLYESTER_HYST_THRESHOLD_HIGH
    const float	def_hyst_threshold_high = QOLYESTER_HYST_THRESHOLD_HIGH;
#  else
    const float	def_hyst_threshold_high = 0.8;
#  endif

#  ifdef QOLYESTER_HYST_THRESHOLD_LOW
    const float	def_hyst_threshold_low  = QOLYESTER_HYST_THRESHOLD_LOW;
#  else
    const float	def_hyst_threshold_low  = 0.3;
#  endif

#  ifdef QOLYESTER_HYST_SCALING
    const float	def_hyst_scaling        = QOLYESTER_HYST_SCALING;
#  else
    const float	def_hyst_scaling        = 0.5;
#  endif

# endif

// Virtual interface
# define VIRTUAL_MTU		1500
# define VIRTUAL_SOCKNAME	"/tmp/qolsrd.switch"

    // Constants definitions.  See the OLSR draft for more
    // information.

# ifdef QOLYESTER_HELLO_INTERVAL
    const timeval_t	def_hello_interval   = QOLYESTER_HELLO_INTERVAL;
# else
    const timeval_t	def_hello_interval   = 2000;
# endif

# ifdef QOLYESTER_REFRESH_INTERVAL
    const timeval_t	def_refresh_interval = QOLYESTER_REFRESH_INTERVAL;
# else
    const timeval_t	def_refresh_interval = 2000;
# endif

# ifdef QOLYESTER_TC_INTERVAL
    const timeval_t	def_tc_interval      = QOLYESTER_TC_INTERVAL;
# else
    const timeval_t	def_tc_interval      = 5000;
# endif

# ifdef QOLYESTER_HELLO_GRACE
    const float		def_hello_grace	     = QOLYESTER_HELLO_GRACE;
# else
    const float		def_hello_grace      = 1.2;
# endif

# ifdef QOLYESTER_ENABLE_MID
#  ifdef QOLYESTER_MID_INTERVAL
    const timeval_t	def_mid_interval     = QOLYESTER_MID_INTERVAL;
#  else
    const timeval_t	def_mid_interval     = def_tc_interval;
#  endif

#  ifdef QOLYESTER_MID_HOLD_TIME
    const timeval_t	def_mid_hold_time    = QOLYESTER_MID_HOLD_TIME;
#  else
    const timeval_t	def_mid_hold_time    = def_mid_interval * 3;
#  endif
# endif

# ifdef QOLYESTER_ENABLE_HNA
#  ifdef QOLYESTER_HNA_INTERVAL
    const timeval_t	def_hna_interval     = QOLYESTER_HNA_INTERVAL;
#  else
    const timeval_t	def_hna_interval     = def_tc_interval;
#  endif

#  ifdef QOLYESTER_HNA_HOLD_TIME
    const timeval_t	def_hna_hold_time    = QOLYESTER_HNA_HOLD_TIME;
#  else
    const timeval_t	def_hna_hold_time    = def_hna_interval * 3;
#  endif
# endif

# ifdef QOLYESTER_NEIGHB_HOLD_TIME
    const timeval_t	def_neighb_hold_time = QOLYESTER_NEIGHB_HOLD_TIME;
# else
    const timeval_t	def_neighb_hold_time = def_refresh_interval * 3;
# endif

# ifdef QOLYESTER_TOP_HOLD_TIME
    const timeval_t	def_top_hold_time    = QOLYESTER_TOP_HOLD_TIME;
# else
    const timeval_t	def_top_hold_time    = def_tc_interval * 3;
# endif

# ifdef QOLYESTER_DUP_HOLD_TIME
    const timeval_t	def_dup_hold_time    = QOLYESTER_DUP_HOLD_TIME;
# else
    const timeval_t	def_dup_hold_time    = 30000;
# endif

# ifdef QOLYESTER_MAX_JITTER_TIME
    const timeval_t	def_maxjitter        = QOLYESTER_MAX_JITTER_TIME;
# else
    const timeval_t	def_maxjitter        = def_hello_interval / 4;
# endif

# ifdef QOLYESTER_DUMP_INTERVAL
    const timeval_t	def_dump_interval    = QOLYESTER_DUMP_INTERVAL;
# else
    const timeval_t	def_dump_interval    = 1000;
# endif

# ifdef QOLYESTER_QUEUE_SIZE
    const unsigned	def_queue_size       = QOLYESTER_QUEUE_SIZE;
# else
    const unsigned	def_queue_size	     = 10;
# endif

  } // namespace cst

# ifdef QOLYESTER_ENABLE_TCRED
  enum TCRedundancy {
    mprselset        = 0,
    mprselset_mprset = 1,
    wholeset         = 2
  };

  const TCRedundancy	def_tc_redundancy = QOLYESTER_TC_DEFAULT_REDUNDANCY;
# endif

  const unsigned	def_willingness = QOLYESTER_DEFAULT_WILLINGNESS;

# ifdef QOLYESTER_ENABLE_MPRRED
  const unsigned	def_mprcoverage = QOLYESTER_MPR_DEFAULT_COVERAGE;
# endif

} // namespace olsr

# include "cst/params.hh"

#endif // ! QOLYESTER_CST_CONSTANTS_HH
