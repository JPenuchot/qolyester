#! /usr/bin/env bash

set -e
test $# -eq 1

top_srcdir="$(cd $1 && pwd)"

include_CPPFLAGS='-I$(top_srcdir)/src/include -I$(top_srcdir)/src/daemon'
daemon_CPPFLAGS='-I$(top_srcdir)/src/include -I$(top_srcdir)/src/daemon'
switch_CPPFLAGS='-I$(top_srcdir)/src/include -I$(top_srcdir)/src/switch'
#daemon_CPPFLAGS="-I\"$top_srcdir\"/src/include -I\"$top_srcdir\"/src/daemon"
#switch_CPPFLAGS="-I\"$top_srcdir\"/src/include -I\"$top_srcdir\"/src/switch"

exec > "$top_srcdir"/src/tests/sanity/Makefile.am

browse_dir()
{
  local TESTS=
  for f in "$1"/*; do
    if [ -d "$f" ]; then
      browse_dir "$f" "$2"
      continue
    fi
    case "$f" in
      *.hh|*.hxx)
        trimmed="${f#*/}"
	canon="${f//[^[:alnum:]@]/_}"
	TESTS="$TESTS lib$canon.a"
	cat << EOF
lib${canon}_a_SOURCES = base.cc
lib${canon}_a_CPPFLAGS = -include '$trimmed' $2

EOF
      ;;
    esac
  done

#   for file in "$1"/**/*.h{h,xx}; do
#     trimed=${file#*/}
#     canon=${file//[^[:alnum:]@]/_}
#     TESTS="$TESTS lib$canon.a"
#     cat << EOF
# lib${canon}_a_SOURCES = base.cc
# lib${canon}_a_CPPFLAGS = -DHEADER='<$trimed>' $2

# EOF
#   done

  cat << EOF
check_LIBRARIES += $TESTS

EOF
}

cat << EOF
## Copyright (C) 2005-2006, 2008 Laboratoire de Recherche en Informatique.

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
## Foundation, Inc., 51 Franklin Street, Fifth Floor,
## Boston, MA  02110-1301, USA.

## This file was generated from src/tests/sanity/generate.sh.

AUTOMAKE_OPTIONS = -Wno-gnu
CXXFLAGS = @CHECK_CXXFLAGS@

dist_noinst_SCRIPTS = generate.sh
check_LIBRARIES =

EOF

for subdir in include daemon switch; do
  subdir_CPPFLAGS="$(eval "echo \"\$${subdir}_CPPFLAGS\"")"
  (cd "$top_srcdir"/src && browse_dir "$subdir" "$subdir_CPPFLAGS")
done
