#ifndef _STR_RouterModule_H_  // -*-c++-*-

/*
 * Copyright 2000 Eric M. Hopper <hopper@omnifarious.mn.org>
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

// For a log, see ../ChangeLog

#include <StrMod/StreamModule.h>
#include <StrMod/STR_ClassIdent.h>
#include <deque>
#include <iterator>
#include <cstddef>

#define _STR_RouterModule_H_

class UNIDispatcher;

/** \class RouterModule RouterModule.h StrMod/RouterModule.h
 * \brief Provides an abstract base for classes that route chunks from a plug to
 * a set of other plugs based on contents.
 */
class RouterModule : public StreamModule {
 protected:
   class RPlug;
   friend class RPlug;
   typedef deque<RPlug *> RPlugList;
   typedef back_insert_iterator<RPlugList> RPlugAdder;

 public:
   static const STR_ClassIdent identifier;

   RouterModule(UNIDispatcher &disp);
   virtual ~RouterModule();

   virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || StreamModule::AreYouA(cid));
   }

   virtual bool canCreate(int side) const = 0;

   virtual bool ownsPlug(const Plug *plug) const;

   virtual bool deletePlug(Plug *plug);

 protected:
   virtual const ClassIdent *i_GetIdent() const         { return(&identifier); }

   virtual Plug *i_MakePlug(int side) = 0;

   /** Add a new plug to the internal list of all plugs.
    * Must be called before i_MakePlug returns a new plug!
    */
   void addNewPlug(RPlug *rp);

   /** If a scan event isn't posted, post one.
    * A scan is always posted because a piece of data came in from an RPlug, and
    * data is only allowed to in from a particular RPlug once per scan.  This is
    * to ensure that one RPlug cannot monopolize the RouterModule.
    */
   inline void postScan();

   /** Compute a list of destination plugs for a given incoming chunk.
    * \param chunk The chunk that's going somewhere.
    * \param source Where it came from.
    * \param begin Beginning of a list of all RPlugs in this RouterModule.
    * \param end STL style end of list started at begin.
    * \param destlist Output iterator to store destination plugs in.
    */
   virtual void getDestinations(const StrChunkPtr &chunk,
                                const RPlug &source,
                                const RPlugList::const_iterator &begin,
                                const RPlugList::const_iterator &end,
                                RPlugAdder &destlist) const = 0;

 private:
   class ScanEvent;
   friend class ScanEvent;
   UNIDispatcher &disp_;
   bool scan_posted_;
   ScanEvent * const scan_;
   bool inroutingdone_;
   RPlugList allplugs_;
   RPlugList writeable_;
   RPlugList nonwriteable_;
   StrChunkPtr routedchunk_;
   size_t outgoingcopies_;

   //! Actually post a scan event to dispatcher_.
   void doPost();
   void doScan();

   void routingDone();
   void processIncoming(RPlug &source, const StrChunkPtr &chunk);
};

//---

class RouterModule::RPlug : public Plug {
   friend class RouterModule;
 public:
   inline virtual int AreYouA(const ClassIdent &cid) const {
      return((identifier == cid) || Plug::AreYouA(cid));
   }

   inline RouterModule &getParent() const;

   virtual int side() const = 0;

   //! Get the 'deleted_' flag of this plug.
   bool getDeleted() const                               { return(deleted_); }

   //! Set the 'deleted_' flag of this plug.
   void setDeleted()                                     { deleted_ = true; }

 protected:
   inline RPlug(RouterModule &parent);

   virtual const ClassIdent *i_GetIdent() const          { return &identifier; }

   virtual const StrChunkPtr i_Read();
   virtual void i_Write(const StrChunkPtr &ptr); 

 private:
   bool deleted_;
};

//-----------------------------inline functions--------------------------------

inline void RouterModule::postScan()
{
   if (!scan_posted_)
   {
      scan_posted_ = true;
      doPost();
   }
}

//---

inline RouterModule &RouterModule::RPlug::getParent() const
{
   return static_cast<RouterModule &>(Plug::getParent());
}

inline RouterModule::RPlug::RPlug(RouterModule &parent)
     : Plug(parent), deleted_(false)
{
}

#endif
