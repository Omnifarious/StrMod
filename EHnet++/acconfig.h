/* acconfig.h -- Used by autoconf to generate config.h.in
   Copyright (C) 1995 Eric Hopper

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* The RW library is the Rogue Wave Tools.h++ library.  The EHAll library is
   a library that I have written, and use extensively.  This program is
   looking for a usable String class in all of thsese places.  It wraps the
   String class in it's own class called NetString.  If it doesn't find a
   String class, it makes NetString be a really stupid String class.

   I should perhaps use the GNU String class, as I'm guaranteeing this will
   work the g++ 2.6.3, but I don't want to rely on outside sources for stuff
   to work.  The only reason it works with Rogue Wave is I had to make it
   work so I could use it on a project for the place I work for.  */

@TOP@
