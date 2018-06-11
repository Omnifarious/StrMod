#ifndef _STR_EchoModule_H_  // -*-c++-*-

/*
 * Copyright 1991-2010 Eric M. Hopper <hopper@omnifarious.org>
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

#ifdef __GNUG__
#pragma interface
#endif

/* $Header$ */

// For log information, see ../ChangeLog

// $Revision$

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif

#include <cassert>

#define _STR_EchoModule_H_

namespace strmod {
namespace strmod {

class StrChunk;

/** \class EchoModule EchoModule.h StrMod/EchoModule.h
 * This module echoes everything that comes in.
 *
 * Everything that is written to its one plug is read from that same plug.
*/
class EchoModule : public StreamModule
{
   class EPlug;
   friend class EPlug;
 public:
   EchoModule();
   virtual ~EchoModule();

   inline bool canCreate(int side = 0) const override;
   inline Plug *makePlug(int side = 0);
   bool ownsPlug(const Plug *plug) const override   { return i_OwnsPlug(plug); }
   bool deletePlug(Plug *plug) override;

 protected:
   void plugDisconnected(Plug *plug) override;

   virtual Plug *i_MakePlug(int side) override;

 private:
   class EPlug : public Plug {
      friend class EchoModule;
    public:
      inline EchoModule &getParent() const;

      int side() const override                          { return(0); }

    protected:
      inline EPlug(EchoModule &parnt);
      inline virtual ~EPlug();

      bool needsNotifyReadable() const override          { return(true); }
      bool needsNotifyWriteable() const override         { return(true); }

      void otherIsReadable() override;
      void otherIsWriteable() override;

      const StrChunkPtr i_Read() override;
      void i_Write(const StrChunkPtr &ptr) override;
   };

   bool plugcreated_;
   EPlug eplug_;

   inline bool i_OwnsPlug(const Plug *plug) const;
};

//-------------------------------inline functions------------------------------

bool EchoModule::canCreate(int side) const
{
   return(side == 0 && !plugcreated_);
}

inline StreamModule::Plug *EchoModule::makePlug(int side)
{
   return(StreamModule::makePlug(side));
}

inline bool EchoModule::i_OwnsPlug(const Plug *plug) const
{
   return(plugcreated_ && (plug == &eplug_));
}

inline EchoModule::Plug *EchoModule::i_MakePlug(int side)
{
   assert(side == 0 && !plugcreated_);
   plugcreated_ = true;
   //lint -save -e1536
   return(&eplug_);
   //lint -restore
}

//=========EchoModule::EPlug inlines========

inline EchoModule::EPlug::EPlug(EchoModule &parnt) : Plug(parnt)
{
}

inline EchoModule::EPlug::~EPlug()
{
}

inline EchoModule &EchoModule::EPlug::getParent() const
{
   return(static_cast<EchoModule &>(Plug::getParent()));
}

}  // namespace strmod
}  // namespace strmod

#endif
