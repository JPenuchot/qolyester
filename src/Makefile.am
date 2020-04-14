## Copyright (C) 2003, 2004, 2005 Laboratoire de Recherche en Informatique.

## This file is part of Qolyester.

## Qolyester is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.

## Qolyester is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

AUTOMAKE_OPTIONS = nostdinc

SUBDIRS = tests

AM_CXXFLAGS = -W -Wall
AM_LDFLAGS = -L.

bin_PROGRAMS = qolsrd

libparsedot_a_LFLAGS = --nounistd --bison-locations
libparsedot_a_YFLAGS = --defines=parsedot.hh --locations

noinst_LIBRARIES = libutl.a
libutl_a_CPPFLAGS = -I$(srcdir)/include -Iinclude
libutl_a_SOURCES = include/utl/syslog.cc \
                   include/utl/syslog.hh

LDADD = libutl.a

qolsrd_CPPFLAGS = -I$(srcdir)/daemon -I$(srcdir)/include -Iinclude
qolsrd_SOURCES = daemon/main.cc \
                 daemon/daemon_static_members_init.hcc \
                 daemon/alg/dijkstra.hh \
                 daemon/alg/dijkstra.hxx \
                 daemon/alg/mainaddrof.hh \
                 daemon/alg/mainaddrof.hxx \
                 daemon/alg/mprselection.hh \
                 daemon/alg/mprselection.hxx \
                 daemon/alg/routingtable.hh \
                 daemon/alg/routingtable.hxx \
                 daemon/cst/params.hh \
                 daemon/cst/params.hcc \
                 daemon/gra/graph.hh \
                 daemon/gra/graph.hxx \
                 daemon/gra/graph.hcc \
                 daemon/msg/hello.hh \
                 daemon/msg/hello.hxx \
                 daemon/msg/hna.hh \
                 daemon/msg/hna.hxx \
                 daemon/msg/message.hh \
                 daemon/msg/message.hxx \
                 daemon/msg/mid.hh \
                 daemon/msg/mid.hxx \
                 daemon/msg/tc.hh \
                 daemon/msg/tc.hxx \
                 daemon/net/interface.hh \
                 daemon/net/route.hh \
                 daemon/net/route.hxx \
                 daemon/pkt/packet.hh \
                 daemon/pkt/packet.hxx \
                 daemon/sch/events.hh \
                 daemon/sch/events.hxx \
                 daemon/set/duplicate.hh \
                 daemon/set/duplicate.hxx \
                 daemon/set/duplicate.hcc \
                 daemon/set/gate.hh \
                 daemon/set/gate.hxx \
                 daemon/set/gate.hcc \
                 daemon/set/hna.hh \
                 daemon/set/hna.hxx \
                 daemon/set/hna.hcc \
                 daemon/set/interfaces.hh \
                 daemon/set/interfaces.hxx \
                 daemon/set/interfaces.hcc \
                 daemon/set/mid.hh \
                 daemon/set/mid.hxx \
                 daemon/set/mid.hcc \
                 daemon/set/neighbors.hh \
                 daemon/set/neighbors.hxx \
                 daemon/set/neighbors.hcc \
                 daemon/set/routes.hh \
                 daemon/set/routes.hxx \
                 daemon/set/routes.hcc \
                 daemon/set/topology.hh \
                 daemon/set/topology.hxx \
                 daemon/set/topology.hcc \
                 daemon/sys/interfaceinfo.hh \
                 daemon/sys/routing.hh \
                 daemon/sys/routing.hxx \
                 daemon/utl/args.hh \
                 daemon/utl/args.hxx \
                 \
                 include/static_members_init.hcc \
                 include/cst/constants.hh \
                 include/net/ipaddress.hh \
                 include/net/ipv4.hh \
                 include/net/ipv6.hh \
                 include/sch/scheduler.hh \
                 include/sch/scheduler.hxx \
                 include/utl/comparator.hh \
                 include/utl/data.hh \
                 include/utl/data.hxx \
                 include/utl/exception.hh \
                 include/utl/iterator.hh \
                 include/utl/log.hh \
                 include/utl/log.hxx \
                 include/utl/log.hcc \
                 include/utl/mark.hh \
                 include/utl/meta.hh \
                 include/utl/meta.hxx \
                 include/utl/seqnum.hh \
                 include/utl/set.hh \
                 include/utl/stampable.hh \
                 include/utl/stampable.hxx \
                 include/utl/timeval.hh \
                 include/utl/timeval.hxx \
                 include/utl/vtime.hh \
                 include/utl/vtime.hxx

if !COND_VIRTUAL
  qolsrd_SOURCES += daemon/net/realinterface.hh \
                    daemon/net/realinterface.hxx \
                    daemon/sys/realinterfaceinfo.hh \
                    daemon/sys/realinterfaceinfo.hxx \
                    daemon/sys/socket.hh \
                    daemon/sys/socket.hxx \
                    daemon/sys/linux/interfacedesc.hh \
                    daemon/sys/linux/interfacedesc.hxx \
                    daemon/sys/linux/interfaceconfig.hh \
                    daemon/sys/linux/interfaceconfig.hxx \
                    daemon/sys/linux/routingsocket.hh \
                    daemon/sys/linux/routingsocket.hxx\
                    \
                    include/sys/linux/netlink.hh \
                    include/sys/linux/netlink.hxx
else COND_VIRTUAL
  qolsrd_SOURCES += daemon/net/virtualinterface.hh \
                    daemon/net/virtualinterface.hxx \
                    daemon/sys/virtual/interfacedesc.hh \
                    daemon/sys/virtual/interfacedesc.hxx \
                    daemon/sys/virtual/routingsocket.hh \
                    daemon/sys/virtual/routingsocket.hxx \
                    daemon/sys/virtualinterfaceinfo.hh \
                    daemon/sys/virtualinterfaceinfo.hxx \
                    include/sys/unixsocket.hh \
                    include/sys/unixsocket.hxx
  BUILT_SOURCE = parsedot.hh
  noinst_LIBRARIES += libparsedot.a
  libparsedot_a_CPPFLAGS = -I$(srcdir)/switch -I$(srcdir)/include -Iinclude
  libparsedot_a_SOURCES = switch/prs/data.hh \
                          switch/prs/visitor.hh \
                          switch/prs/location.hh \
                          switch/prs/parsedot.yy \
                          switch/prs/scandot.ll \
                          include/utl/log.hh \
			  parsedot.hh

  bin_PROGRAMS += qswitchd
  qswitchd_CPPFLAGS = -I$(srcdir)/switch -I$(srcdir)/include -Iinclude
  qswitchd_LDADD = libparsedot.a libutl.a
  qswitchd_SOURCES = switch/main.cc \
                     \
                     switch/alg/forward_packet.hh \
                     switch/alg/forward_packet.hxx \
                     switch/cst/params.hh \
                     switch/cst/params.hcc \
                     switch/gra/graph.hh \
                     switch/gra/graph.hxx \
                     switch/gra/graph.hcc \
                     switch/prs/data.hh \
                     switch/prs/data.hxx \
                     switch/prs/location.hh \
                     switch/prs/visitor.hh \
		     switch/prs/parse.hh \
		     switch/prs/parse.hxx \
		     switch/set/client.hh \
		     switch/set/client.hxx \
                     switch/set/freeaddr.hh \
                     switch/sch/events.hh \
                     switch/sch/events.hxx \
                     switch/utl/args.hh \
                     switch/utl/args.hxx \
                     \
                     include/static_members_init.hcc \
                     include/cst/constants.hh \
                     include/net/ipaddress.hh \
                     include/net/ipv4.hh \
                     include/net/ipv6.hh \
                     include/sch/scheduler.hh \
                     include/sch/scheduler.hxx \
                     include/sys/unixsocket.hh \
                     include/sys/unixsocket.hxx \
                     include/utl/comparator.hh \
                     include/utl/data.hh \
                     include/utl/data.hxx \
                     include/utl/exception.hh \
                     include/utl/iterator.hh \
                     include/utl/log.hh \
                     include/utl/log.hxx \
                     include/utl/mark.hh \
                     include/utl/meta.hh \
                     include/utl/meta.hxx \
                     include/utl/seqnum.hh \
                     include/utl/set.hh \
                     include/utl/stampable.hh \
                     include/utl/stampable.hxx \
                     include/utl/timeval.hh \
                     include/utl/timeval.hxx \
                     include/utl/vtime.hh \
                     include/utl/vtime.hxx
endif

