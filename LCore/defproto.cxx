/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "LCore/Protocol.h"

 /* UNIX:@@:defs/Protocol.cc:@@: */
/* MSDOS:@@:DEFS/DEFPROTO.CPP:@@: */

/**
 * Globally unique identifier for this class.
 *
 * Every class that might need to be identified should have a public static
 * const identifier object in it.
 */
const LCore_ClassIdent Protocol::identifier(7UL);
