#ifndef _UNEVT_UNIXSignalHandler_H_  // -*-c++-*-

/*
 * Copyright 2001 Eric M. Hopper <hopper@omnifarious.org>
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

#include <LCore/Protocol.h>
#include <UniEvent/UNEVT_ClassIdent.h>

#define _UNEVT_UNIXSignalHandler_H_

namespace strmod {
namespace unievent {

class Dispatcher;
class EventPtr;

/** \class UNIXSignalHandler UNIXSignalHandler.h UniEvent/UNIXSignalHandler.h
 * \brief Manages events associated with UNIX signals.
 */
class UNIXSignalHandler : public Protocol {
 public:
   static const UNEVT_ClassIdent identifier;

   UNIXSignalHandler(Dispatcher &dispatcher);
   virtual ~UNIXSignalHandler();

   virtual int AreYouA(const ClassIdent &cid) const {
      return (identifier == cid) || Protocol::AreYouA(cid);
   }

   /** On the given signal, post the given event.
    * @param signo The number of the signal to post an event for.
    * @param ev The event to post.
    * @param oneshot Whether or not the event is removed after it is
    * posted (defualts to true).
    */
   void onSignal(int signo, const EventPtr &ev, bool oneshot = true);
   void removeEvent(int signo, const EventPtr &ev, bool oneshot);
   void removeEvent(int signo, const EventPtr &ev);
   void removeEvent(const EventPtr &ev);

   void processOutstandingSignals();

 protected:
   inline virtual const ClassIdent *i_GetIdent() const   { return &identifier; }

 private:
   struct Imp;
   class SigBlockRegion;
   static UNIXSignalHandler *S_curhandler;
   Dispatcher &disp_;
   Imp &imp_;

   static void signalHandler(int signo);
   void handleSignal(int signo);
   void unHandleSignal(int signo);
   void sigOccured(int signo);
   void postEventsFor(unsigned int usigno);
};

}; // namespace unievent
}; // namespace strmod

#endif
