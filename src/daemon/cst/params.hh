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

#ifndef QOLYESTER_DAEMON_CST_PARAMS_HH
# define QOLYESTER_DAEMON_CST_PARAMS_HH 1

# include "cst/constants.hh"

namespace olsr {

  namespace cst {

# ifdef QOLYESTER_ENABLE_LINKHYS
    extern float	hyst_threshold_high;
    extern float	hyst_threshold_low;
    extern float	hyst_scaling;
# endif

    extern timeval_t	hello_interval;
    extern timeval_t	refresh_interval;
    extern timeval_t	tc_interval;

    extern float	hello_grace;

# ifdef QOLYESTER_ENABLE_MID
    extern timeval_t	mid_interval;
    extern timeval_t	mid_hold_time;
# endif

# ifdef QOLYESTER_ENABLE_HNA
    extern timeval_t	hna_interval;
    extern timeval_t	hna_hold_time;
# endif

    extern timeval_t	neighb_hold_time;
    extern timeval_t	top_hold_time;
    extern timeval_t	dup_hold_time;

    extern timeval_t	maxjitter;
    extern timeval_t	dump_interval;

    extern unsigned	queue_size;

  } // namespace cst

  extern TCRedundancy	tc_redundancy;
  extern unsigned	willingness;

# ifdef QOLYESTER_ENABLE_MPRRED
  extern unsigned	mprcoverage;
# endif

} // namespace olsr

#endif // QOLYESTER_DAEMON_CST_PARAMS_HH
