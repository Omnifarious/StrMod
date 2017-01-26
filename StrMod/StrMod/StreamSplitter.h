#ifndef _STR_StreamSplitter_H_  // -*-c++-*-

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
#  pragma interface
#endif

/* $Header$ */

// For a change log see ../ChangeLog

//! author="Eric Hopper" lib=StrMod

#include <cstddef>  // NULL
#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif
#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif

#define _STR_StreamSplitter_H_

namespace strmod {
namespace strmod {

/** \class StreamSplitterModule StreamSplitter.h StrMod/StreamSplitter.h
 * \brief This module is for treating two uni-directional streams as one
 * bi-directional stream.
 *
 * <p>SplitterModule's work this way:</p>
 * <pre>
 *              SideIn
 *  Module>---->--->-->--+
 *                       |       (bi-directional)
 *                      Splitter===>==<==>==<==>==<===Module
 *                       |      ^
 *  Module<----<---<--<--+      |
 *              SideOut         +--side SideBiDir
 * </pre>
 *
 *
 * <p>The SplitterModule takes three i/o streams, one that does both input and
 * output, one that does input, and one that does output, and connects them
 * together. They're connected in such a way as to cause the output of the
 * stream that does input and ouput goes into the stream that only takes input,
 * and the output of the stream that only does output goes to the input of the
 * combined stream.</p>
 *
 * <p>The SideOut plug is <b>never</b> writeable, and the SideIn plug is
 * <b>never</b> readable..</p>
 */
class StreamSplitterModule : public StreamModule
{
 public:
   //! Create a splitter module who's plugs are connected to nothing.
   StreamSplitterModule();
   //! Delete splitter module and destroy all plugs it owns.
   virtual ~StreamSplitterModule();

   //! On what sides can a plug be created?
   enum Sides {
      SideIn,  //!< Only accepts input
      SideOut,  //!< Only produces output
      SideBiDir //!< Writing here goes out SideOut and reading comes from SideIn
   };

   inline virtual bool canCreate(int side) const;
   virtual bool deletePlug(Plug *plug);
   inline virtual bool ownsPlug(const Plug *p) const;

 protected:
   class SPPlug;
   friend class SPPlug;
   /* Not Doxygen yet. 
    * \brief This does most of the work.  It just forwards stuff to the other
    * plugs.
    */
   class SPPlug : public Plug {
      friend class StreamSplitterModule;
    public:
      SPPlug(StreamSplitterModule &p, Sides s) : Plug(p), side_(s)           { }
      virtual ~SPPlug()                                                      { }

      inline StreamSplitterModule &getParent() const;

      virtual int side() const                          { return(side_); }

    protected:
      virtual const StrChunkPtr i_Read();
      virtual void i_Write(const StrChunkPtr &ptr);

      inline virtual bool needsNotifyReadable() const;
      inline virtual bool needsNotifyWriteable() const;

      virtual void otherIsReadable();
      virtual void otherIsWriteable();

      inline SPPlug *getReadPartner() const;
      inline SPPlug *getWritePartner() const;

    private:
      Sides side_;
   };

   /** See base class.  This one sets the read/writeable flags on the other
    * plugs to be right.
    *
    * It calls the base class version after doing its work.
    */
   virtual void plugDisconnected(Plug *plug);

   virtual Plug *i_MakePlug(int side);

 private:
   struct {
      int inmade : 1;
      int outmade : 1;
      int bimade : 1;
   } flags_;
   SPPlug inplug_;
   SPPlug outplug_;
   SPPlug biplug_;
};

//-----------------------------inline functions--------------------------------

bool StreamSplitterModule::canCreate(int side) const
{
   switch (side) {
    case SideIn:
      return(flags_.inmade == 0);
    case SideOut:
      return(flags_.outmade == 0);
    case SideBiDir:
      return(flags_.bimade == 0);
    default:
      return(false);
   }
}

inline bool StreamSplitterModule::ownsPlug(const Plug *p) const
{
   return((flags_.inmade && (&inplug_ == p))
	  || (flags_.outmade && (&outplug_ == p))
	  || (flags_.bimade && (&biplug_ == p)));
}

//--

inline StreamSplitterModule &StreamSplitterModule::SPPlug::getParent() const
{
   return(static_cast<StreamSplitterModule &>(Plug::getParent()));
}

inline bool StreamSplitterModule::SPPlug::needsNotifyReadable() const
{
   return((side_ == SideIn) || (side_ == SideBiDir));
}

inline bool StreamSplitterModule::SPPlug::needsNotifyWriteable() const
{
   return((side_ == SideOut) || (side_ == SideBiDir));
}

inline StreamSplitterModule::SPPlug *
StreamSplitterModule::SPPlug::getReadPartner() const
{
   switch (side_)
   {
    case SideOut:
      return(&(getParent().biplug_));
    case SideBiDir:
      return(&(getParent().inplug_));
    case SideIn:
    default:
      return(NULL);
   }
}

inline StreamSplitterModule::SPPlug *
StreamSplitterModule::SPPlug::getWritePartner() const
{
   switch (side_)
   {
    case SideIn:
      return(&(getParent().biplug_));
    case SideBiDir:
      return(&(getParent().outplug_));
    case SideOut:
    default:
      return(NULL);
   }
}

}  // namespace strmod
}  // namespace strmod

#endif
