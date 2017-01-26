#ifndef _STR_ProcessorModule_H_  // -*-c++-*-

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

// For a log, see ../Changelog
//
// Revision 1.2  1996/09/21 18:38:21  hopper
// Added forward declaration of class ProcessorModule::Plug, presumably to
// work around a compiler bug.
//
// Revision 1.1  1996/09/02 23:31:20  hopper
// Added class that would turn two StreamProcessors into a StreamModule
// that used one StreamProcessor for the data flowing in each direction.
//

#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif
#ifndef _STR_StreamModule_H_
#  include <StrMod/StreamModule.h>
#endif

#include <cassert>

#define _STR_ProcessorModule_H_

namespace strmod {
namespace strmod {

class StreamProcessor;

/** \class ProcessorModule ProcessorModule.h StrMod/ProcessorModule.h
 * This wraps two objects that follow the unidirectional StreamProcessor
 * interface in a StreamModule interface.
 */
class ProcessorModule : public StreamModule
{
   class PMPlug;
   friend class PMPlug;
 public:
   //! Labels for the sides of this StreamModule implementation
   enum PlugSide {
      OneSide,  //!< Distinguishable name.
      OtherSide //!< Another distinguishable name.
   };

   /** Construct a ProcessorModule from the two given StreamProcessor objects.
    *
    * @param from_one The StreamProcessor that will process data going from
    * OneSide to OtherSide.
    *
    * @param from_other The StreamProcessor that will process data going from
    * OtherSide to OneSide.
    *
    * @param own Whether or not this module 'owns' or assumes the duty of
    * deleting the two passed in StreamProcessor objects.
    *
    * \note If \c own is \c false, it's your responsibility to make sure the
    * ProcessorModule goes away before the StreamProcessors do.  If you're
    * building a derived class, ~ProccesorModule is guaranteed not to require
    * the StreamProcessors to exist if \c own is \c false.
    */
   inline ProcessorModule(StreamProcessor &from_one,
			  StreamProcessor &from_other,
			  bool own = true);

   /** Destruct a ProcessorModule, possibly deleting the associated
    * StreamProcessor objects.
    *
    * If created with \c own equal to \c false, this function guaranteed not to
    * require subsidiary StreamProcessors to exist.
    */
   virtual ~ProcessorModule()                           { }

   inline virtual bool canCreate(int side) const;

   inline virtual bool ownsPlug(const Plug *plug) const;

   virtual bool deletePlug(Plug *plug);

 protected:
   inline virtual Plug *i_MakePlug(int side);

 private:
   class PMPlug : public Plug {
      friend class ProcessorModule;

    public:
      inline PMPlug(ProcessorModule &prnt, Plug &sibling, PlugSide side,
		    StreamProcessor &readproc, StreamProcessor &writeproc);
      virtual ~PMPlug();

      inline ProcessorModule &getParent() const;

      inline virtual int side() const                   { return(side_); }

    protected:
      virtual const StrChunkPtr i_Read();
      virtual void i_Write(const StrChunkPtr &chnk);

   private:
      Plug &sibling_;
      const PlugSide side_;
      StreamProcessor &readproc_;
      StreamProcessor &writeproc_;
   };

   PMPlug side_;
   PMPlug otherside_;
   struct {
      bool side : 1;
      bool otherside : 1;
      bool owns : 1;
   } pulled_;
};

//-----------------------------inline functions--------------------------------

//----------ProcessorModule::PMPlug inlines-----

inline ProcessorModule &ProcessorModule::PMPlug::getParent() const
{
   return(static_cast<ProcessorModule &>(Plug::getParent()));
}

inline ProcessorModule::PMPlug::PMPlug(ProcessorModule &prnt, Plug &sibling,
				       PlugSide side,
				       StreamProcessor &readproc,
				       StreamProcessor &writeproc)
     : Plug(prnt), sibling_(sibling), side_(side),
       readproc_(readproc), writeproc_(writeproc)
{
}

//----------ProcessorModule inlines-----

inline ProcessorModule::ProcessorModule(StreamProcessor &from_one,
					StreamProcessor &from_other,
					bool own)
     : side_(*this, otherside_, OneSide, from_other, from_one),
       otherside_(*this, side_, OtherSide, from_one, from_other)
{
   assert(&from_one != &from_other);
   pulled_.side = pulled_.otherside = false;
   pulled_.owns = own;
   side_.setWriteable(true);
   otherside_.setWriteable(true);
}

inline bool ProcessorModule::canCreate(int side) const
{
   assert((side == OneSide) || (side == OtherSide));
   return((side == OneSide) ? !pulled_.side : !pulled_.otherside);
}

inline bool ProcessorModule::ownsPlug(const Plug *plug) const
{
   const Plug * const sidep = &side_;
   const Plug * const othersidep = &otherside_;

   return((plug == sidep && pulled_.side) ||
	  (plug == othersidep && pulled_.otherside));
}

inline StreamModule::Plug *ProcessorModule::i_MakePlug(int side)
{
   assert((side == OneSide) || (side == OtherSide));
   if ((side == OneSide) && (!pulled_.side))
   {
      pulled_.side = true;
      return(&side_);
   }
   else if ((side == OtherSide) && (!pulled_.otherside))
   {
      pulled_.otherside = true;
      return(&otherside_);
   }
   else
   {
      assert(false);
      return(0);
   }
}

}  // namespace strmod
}  // namespace strmod

#endif
