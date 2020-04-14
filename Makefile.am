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

EXTRA_DIST = DEVELOPERS BUGS doc/rfc3626.txt doc/virtual-network-v4.dot doc/virtual-network-v6.dot bootstrap

SUBDIRS = src man

if COND_DOC
  SUBDIRS += doc
  EXTRA_DIST += doc/daemon/html

if COND_VIRTUAL
    EXTRA_DIST += doc/switch/html
endif
endif

doc/daemon/html doc/switch/html :
	$(MAKE) -C doc

snapdist :
	$(MAKE) SNAPSHOT=1 VERSION=`date +"%Y%m%d"` dist

snapdistcheck :
	$(MAKE) SNAPSHOT=1 VERSION=`date +"%Y%m%d"` distcheck

distmulticheck :
	for fam in enable disable; do \
	  for iface in enable disable; do \
	    $(MAKE) \
	    DISTCHECK_CONFIGURE_FLAGS="--$${fam}-ipv4 --$${iface}-virtual" \
	    distcheck; \
	  done; \
	done

snapdistmulticheck :
	$(MAKE) SNAPSHOT=1 VERSION=`date +"%Y%m%d"` distmulticheck

dist-hook:
	$(MAKE) -C doc force-all; \
	if test x$(SNAPSHOT) = x1; then \
	  if test -r $(srcdir)/.version; then \
	    echo `cat $(srcdir)/.version`-$(VERSION) > $(distdir)/.version; \
          else \
            echo $(VERSION) > $(distdir)/.version; \
          fi; \
	else \
	  if test -r $(srcdir)/.version; then \
	   cp $(srcdir)/.version $(distdir)/.version; \
	  fi; \
	fi
