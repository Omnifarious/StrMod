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

const STR_ClassIdent STR_ClassIdent::identifier(0UL);

/*
   0UL	STR_ClassIdent
   1UL	StreamModule
   2UL	StrPlug
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
  16UL	GroupVector
  17UL	GroupVector::Iterator
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
*/
