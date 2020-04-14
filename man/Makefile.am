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

EXTRA_DIST = qolsrd-hna1.man qolsrd-hna2.man qolsrd-mid1.man \
             qolsrd-mid2.man qolsrd-mprred.man qolsrd-nomid1.man \
	     qolsrd-tables.man qolsrd-tcred.man qolsrd-virtual1.man \
	     qolsrd-virtual2.man qolsrd-dirtypromischack.man


man_MANS = qolsrd.8

if COND_VIRTUAL
  dist_man_MANS = qswitchd.8
else
  EXTRA_DIST += qswitchd.8
endif
