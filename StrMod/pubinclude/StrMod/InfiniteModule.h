#ifndef _STR_InfiniteModule_H_  // -*-c++-*-

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


/* $Header$ */

// For a log, see ../ChangeLog

#include <StrMod/StreamModule.h>
#include <StrMod/StrChunkPtr.h>

#define _STR_InfiniteModule_H_

namespace strmod {
namespace strmod {

/** \class InfiniteModule InfiniteModule.h StrMod/InfiniteModule.h
 * \brief Sends a particular chunk repeatedly forever.  Eats any chunk sent to
 * it.
 *
 * This strongly resembles a /dev/zero where you decide what comes out instead
 * of an infinite string of zeros.
 */
class InfiniteModule : public StreamModule
{
   class IPlug;
   friend class IPlug;

 public:
   /** Construct given a chunk to repeatedly send.
    * @param chnk The chunk to repeatedly send.
   */
   InfiniteModule(const StrChunkPtr &chnk);
   //! Destroy an InfiniteModule
   InfiniteModule();
   virtual ~InfiniteModule();

   inline bool canCreate(int side = 0) const override;
   inline Plug *makePlug(int side = 0);
   inline bool ownsPlug(const Plug *plug) const override;
   bool deletePlug(Plug *plug) override;

 protected:
   Plug *i_MakePlug(int side) override;

 private:
   class IPlug : public Plug {
    public:
      friend class InfiniteModule;

      IPlug(InfiniteModule &parent) : Plug(parent)      { }
      ~IPlug() = default;

      inline InfiniteModule &getParent() const;
      int side() const override                         { return(0); }

    protected:
      inline const StrChunkPtr i_Read() override;
      inline void i_Write(const StrChunkPtr &tr) override;
   };

   StrChunkPtr feed_;
   bool plug_created_;
   IPlug plug_;
};

//-----------------------------inline functions--------------------------------

inline bool InfiniteModule::canCreate(int side) const
{
   return((side == 0) && !plug_created_);
}

inline StreamModule::Plug *InfiniteModule::makePlug(int side)
{
   return(StreamModule::makePlug(side));
}

inline bool InfiniteModule::ownsPlug(const Plug *plug) const
{
   return((&plug_ == plug) && plug_created_);
}

//--

inline InfiniteModule &InfiniteModule::IPlug::getParent() const
{
   return(static_cast<InfiniteModule &>(Plug::getParent()));
}

const StrChunkPtr InfiniteModule::IPlug::i_Read()
{
   return(getParent().feed_);
}

void InfiniteModule::IPlug::i_Write(const StrChunkPtr &)
{
}

}  // namespace strmod
}  // namespace strmod

#endif
