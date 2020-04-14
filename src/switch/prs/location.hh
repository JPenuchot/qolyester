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

#ifndef QOLYESTER_SRC_SWITCH_PRS_LOCATION_HH
# define QOLYESTER_SRC_SWITCH_PRS_LOCATION_HH 1

# include <ostream>

# define YYLTYPE olsr::prs::Location

namespace olsr {

  namespace prs {

    class Position {
      typedef Position	This;
    public:
      static const int	initial_column = 1;
      static const int	initial_line = 1;

      Position()
	: filename(),
	  line(initial_line),
	  column(initial_column)
      {}

      void	column_add(int columns = 1) {
	column += columns;
      }

      void	line_add(int lines = 1) {
	column = initial_column;
	line += lines;
      }

      const This	operator+(const int width) const {
	This	ret = *this;
	ret.column_add(width);
	return ret;
      }

      const This&	operator+=(const int width) {
	column_add(width);
	return *this;
      }

      std::string	filename;
      int		line;
      int		column;
    };

    class Location {
      typedef Location	This;
    public:
      Location()
	: begin(),
	  end()
      {}

      void	step() {
	begin = end;
      }

      void	column(int columns = 1) {
	end += columns;
      }

      void	line(int lines = 1) {
	end.line_add(lines);
      }

      void	word(int columns) {
	step();
	column(columns);
      }

      const This	operator+(const This& rhs) const {
	This	ret = *this;
	ret.end = rhs.end;
	return ret;
      }

      const This	operator+(const int width) const {
	This	ret = *this;
	ret.column(width);
	return ret;
      }

      const This&	operator+=(const int width) {
	column(width);
	return *this;
      }

      Position	begin;
      Position	end;

    };

    inline
    std::ostream& operator<<(std::ostream& os, const Position& pos)
    {
      return os << pos.filename
		<< ", line " << pos.line
		<< ", char " << pos.column;
    }

    inline
    std::ostream& operator<<(std::ostream& os, const Location& loc)
    {
      if (loc.begin.filename != loc.end.filename)
	os << loc.begin << " - " << loc.end;
      else {
	os << loc.begin.filename << ", line "
	   << loc.begin.line << ", ";
	if (loc.begin.line == loc.end.line)
	  os << "chars " << loc.begin.column << '-' << loc.end.column;
	else
	  os << "char " << loc.begin.column
	     << " - line " << loc.end.line
	     << ", char " << loc.end.column;
      }
      return os;
    }

  } // namespace prs

} // namespace olsr

#endif // QOLYESTER_SRC_SWITCH_PRS_LOCATION_HH
