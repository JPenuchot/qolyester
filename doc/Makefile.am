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

if COND_DOC
if COND_VIRTUAL
all : daemon/html/index.html switch/html/index.html

force-all : force-daemon force-switch

force-daemon daemon/html/index.html :
	$(DOXYGEN) Doxyfile-daemon

force-switch switch/html/index.html :
	$(DOXYGEN) Doxyfile-switch

distclean-local :
	-rm -fr daemon switch
else
all : daemon/html/index.html

force-all : force-daemon

force-daemon daemon/html/index.html :
	$(DOXYGEN) Doxyfile-daemon

distclean-local :
	-rm -fr daemon
endif
else !COND_DOC
all :
endif

