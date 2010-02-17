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

/* $Header$ */

// See Changelog for a log of changes.
//
// Revision 1.1  1996/09/02 23:33:39  hopper
// Created abstract class (class StreamProcessor) for handling simple
// dataflows where one grabbed incoming data, did stuff to it, and sent
// the result along.
//

#ifdef __GNUG__
#  pragma implementation "StreamProcessor.h"
#endif

#include "StrMod/StreamProcessor.h"
#include "StrMod/StrChunkPtr.h"

const STR_ClassIdent StreamProcessor::identifier(25UL);

StreamProcessor::~StreamProcessor()
{
   // outgoing_ and incoming_ are smart pointers and will automagically handle
   // the deletion themselves.
}
