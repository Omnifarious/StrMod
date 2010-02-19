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

// For changes, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "STR_ClassIdent.h"
#endif

#include "StrMod/STR_ClassIdent.h"

namespace strmod {
/** The heart of the StreamModule System (<A HREF="http://www.omnifarious.org/StrMod">Homepage</A>)
 *
 * This is the namespace for classes in the StreamModule and StrChunk
 * subsystems.  Here are a list of the important top-level classes:
 *    - StreamModule A component in a communications pipeline or web.
 *    - StrChunk A reference counted piece of data moved around between StreamModule's.
 *    - StreamProcessor Implements a buffered filter that has an input that is processed in some way to produce an output.
 *    - ProcessorModule A kind of StreamModule that allows you to put together two StreamProcessor objects to make a bidirectional StreamModule.
 */
namespace strmod {

const STR_ClassIdent STR_ClassIdent::identifier(0UL);

}  // End namespace strmod
}  // End namespace strmod

/*
   0UL	STR_ClassIdent
   1UL	StreamModule
   2UL	StreamModule::Plug
   3UL	EchoModule
   4UL	EchoModule::EPlug
   6UL	StrChunk
   7UL  ChunkVisitor
   8UL	StreamFDModule
   9UL	StreamFDModule::FPlug
  10UL	StreamSplitterModule
  11UL	StreamSplitterModule::SPPlug
  12UL	SocketModule
  13UL	SockListenModule
  14UL	SockListenModule::SLPlug
  15UL	SocketModuleChunk
  16UL	====was GroupVector
  17UL	====was GroupVector::Iterator
  18UL	StrSubChunk
  19UL  StrChunkPtr
  20UL  GroupChunk
  21UL	SimpleMultiplexer
  22UL	SimpleMultiplexer::MultiPlug
  23UL	SimpleMultiplexer::SinglePlug
  24UL  BufferChunk
  25UL	StreamProcessor
  26UL	CharChopper
  27UL	NewlineChopper
  28UL	PassThrough
  29UL	ProcessorModule
  30UL	ProcessorModule::PMPlug
  31UL	EOFStrChunk
  32UL	SimpleMultiplexer::ScanEvent
  33UL	TelnetParser
  34UL	TelnetParser::TelnetData
  35UL	TelnetParser::SingleChar
  36UL	TelnetParser::Suboption
  37UL	TelnetParser::OptionNegotiation
  38UL	PreAllocBufferBase
  39UL	DynamicBuffer
  40UL	BufferChunk::Factory
  41UL	InfiniteModule
  42UL	InfiniteModule::IPlug
  43UL	SimpleTelnetClient
  44UL	SimpleTelnetClient::UPlug
  45UL	SimpleTelnetClient::SPlug
  46UL	StaticBuffer
  47UL	UseTrackingVisitor
  48UL	GraphVizVisitor
  49UL	TelnetChunkBuilder
  50UL	TelnetParser
  51UL	ApplyVisitor_Base
  52UL	RouterModule
  53UL	RouterModule::RPlug
  54UL	RouterModule::ScanEvent
*/
