#ifndef _STR_StreamModule_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */


// $Log$
// Revision 1.5  1996/10/23 10:16:34  hopper
// Made a number of cosmetic readability improvements.
//
// Revision 1.4  1996/07/06 01:24:47  hopper
// Added assert for error checking.
//
// Revision 1.3  1996/07/05 18:49:56  hopper
// Changed to use new StrChunkPtr interface.  Also changed to implement
// new parent module handling in StrPlug base class.
//
// Revision 1.2  1996/02/26 03:43:51  hopper
// Made StrPlug::PluggedInto method const as it ought to have been in the
// first place.
//
// Revision 1.1.1.1  1995/07/22 04:46:50  hopper
// Imported sources
//
// ->Revision 0.30  1995/04/05  01:49:15  hopper
// ->Changed things for integration into the rest of my libraries.
// ->
// ->Revision 0.29  1995/03/08  05:27:07  hopper
// ->Set it up so that setting the notify readable or writeable flags caused
// ->the readable or writeable messages to be sent if the object could
// ->currently be read or written to.
// ->
// ->Revision 0.28  1995/01/13  17:39:05  hopper
// ->Merged versions 0.27, and 0.27.0.6
// ->
// ->Revision 0.27.0.6  1995/01/13  17:36:35  hopper
// ->Added support for OS2SockListenModule class to enum's.
// ->Added a new convenience function, PlugIntoAndNotify(StrPlug *) to StrPlug
// ->
// ->Revision 0.27.0.5  1994/06/16  02:16:39  hopper
// ->Added #pragma interface declaration.
// ->
// ->Revision 0.27.0.4  1994/06/13  13:31:14  hopper
// ->Added two new plug types to <Plug>Num enum.
// ->Added two new module types to <Module>Num enum.
// ->
// ->Revision 0.27.0.3  1994/06/08  16:01:15  hopper
// ->Added three new plug types to <Plug>Num enum.
// ->
// ->Revision 0.27.0.2  1994/06/08  06:42:31  hopper
// ->Added three new module types to <Module>Num enum.
// ->
// ->Revision 0.27.0.1  1994/05/16  04:32:20  hopper
// ->No changes, but I needed a head for the inevitable WinterFire-OS/2
// ->branch.
// ->
// ->Revision 0.27  1994/05/07  03:40:55  hopper
// ->Move enclosing #ifndef ... #define bits around. Changed names of some
// ->non-multiple include constants. Changed directories for certain
// ->type of include files.
// ->
// ->Revision 0.26  1992/05/06  04:37:47  hopper
// ->StrPlug::Read() shouldn't have been a virtual function.
// ->
// ->Revision 0.25  1992/05/05  18:02:48  hopper
// ->Deleted out of date/irrelavent log entries.
// ->
// ->Revision 0.24  1992/05/05  17:58:13  hopper
// ->Changed an out of date comment.
// ->
// ->Revision 0.22-0.23  1992/05/05  17:40:37  hopper
// ->Added SockListenModuleNum to enum list
// ->Added ListeningPlugNum to enum list.
// ->
// ->Revision 0.21  1992/05/02  00:13:46  hopper
// ->Added SocketModuleNum to enum type list in
// ->class StreamModule
// ->
// ->Revision 0.18  1992/04/27  04:02:30  hopper
// ->Fixed a grievous error that cause a StrPlug to leave itself
// ->plugged in, even after it was deleted. Now it checks to see if
// ->it is plugged in, and UnPlug's itself if it is.
// ->
// ->Revision 0.15  1992/04/24  00:17:44  hopper
// ->Fixed a minor error that cause multiple copies of
// ->StrFDPlug::Unplug() to be generated.
// ->
// ->Revision 0.14  1992/04/21  19:30:45  hopper
// ->Fixed a few small errors, added
// ->StrPlug::ReadableNotify(), StrPlug::WriteableNotify(),
// ->StrPlug::DoReadableNotify(), and StrPlug::DoWriteableNotify()
// ->
// ->Revision 0.13  1992/04/14  04:39:07  hopper
// ->Made some major changes, and (I hope) finalized on an
// ->interface for StreamModule's, and StrPlug's.
// ->
// ->Revision 0.12  1991/11/18  04:32:01  hopper
// ->Added the $Id field to be conditionally compiled in depending on the
// ->value of the pre-processor constant NO_RcsID
// ->
// ->Revision 0.11  1991/11/18  04:16:07  hopper
// ->Added rcs droppings. Put the #ifndef #endif wrappings around the
// ->header file.
// ->

#ifndef NO_RcsID
static char _STR_StreamModule_H_rcsID[] = "$Id$";
#endif

#include <LCore/Object.h>

#ifndef _STR_STR_ClassIdent_H_
#  include <StrMod/STR_ClassIdent.h>
#endif
#ifndef _STR_StrChunkPtr_H_
#  include <StrMod/StrChunkPtr.h>
#endif

#include <assert.h>

#define _STR_StreamModule_H_

class StrPlug;

class StreamModule : public Object {
 public:
   static const STR_ClassIdent identifier;

   StreamModule()                                 { }
   virtual ~StreamModule()                        { }

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual bool CanCreate(int side) const = 0;
   StrPlug *MakePlug(int side)                    { return(CreatePlug(side)); }
   virtual bool OwnsPlug(StrPlug *plug) const = 0;
   virtual bool DeletePlug(StrPlug *plug) = 0;

 protected:
   virtual const ClassIdent *i_GetIdent() const   { return(&identifier); }

   virtual StrPlug *CreatePlug(int side) = 0;
};

class StrChunkPtr;

class StrPlug : public Object {
 public:
   static const STR_ClassIdent identifier;

   inline StrPlug(StreamModule *parent);
   inline virtual ~StrPlug();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   virtual bool CanWrite() const = 0;
   inline virtual void NotifyOnWriteable();
   virtual bool Write(const StrChunkPtr &) = 0;

   virtual bool CanRead() const = 0;
   inline virtual void NotifyOnReadable();
   const StrChunkPtr Read()                         { return(InternalRead()); }

   virtual bool PlugInto(StrPlug *p);
   inline bool PlugIntoAndNotify(StrPlug *p);
   StrPlug *PluggedInto() const                     { return(ConnPlug); }
   inline virtual void UnPlug();

   StreamModule *ModuleFrom() const                 { return(parent_); }
   virtual int Side() const = 0;

 protected:
   enum { NotifyingOnWriteable = 0x01, NotifyingOnReadable = 0x02 };

   virtual const ClassIdent *i_GetIdent() const     { return(&identifier); }

   virtual const StrChunkPtr InternalRead() = 0;

   virtual void ReadableNotify();
   virtual void WriteableNotify();

   inline void DoReadableNotify();
   inline void DoWriteableNotify();

 private:
   StrPlug *ConnPlug;
   StreamModule *parent_;
   unsigned char notify_flags;

   StrPlug(const StrPlug &);
   StrPlug &operator =(const StrPlug &);
};

//-----------------------------inline functions--------------------------------

inline int StreamModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

//----------------StrPlug inlines------------------

inline StrPlug::StrPlug(StreamModule *parent)
     : ConnPlug(0), notify_flags(0), parent_(parent)
{
   assert(parent_ != 0);
}

inline StrPlug::~StrPlug()
{
   if (PluggedInto())
      PluggedInto()->UnPlug();
}

inline int StrPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

inline void StrPlug::NotifyOnWriteable()
{
   if (PluggedInto()) {
      notify_flags |= NotifyingOnWriteable;
      if (CanWrite()) {
	 DoWriteableNotify();
      }
   }
}

inline void StrPlug::NotifyOnReadable()
{
   if (PluggedInto()) {
      notify_flags |= NotifyingOnReadable;
      if (CanRead()) {
	 DoReadableNotify();
      }
   }
}

inline bool StrPlug::PlugIntoAndNotify(StrPlug *p)
{
   if (PlugInto(p)) {
      NotifyOnReadable();
      NotifyOnWriteable();
      p->NotifyOnReadable();
      p->NotifyOnWriteable();
      return(1);
   } else
      return(0);
}

inline void StrPlug::UnPlug()
{
   if (ConnPlug) {
      StrPlug *temp = ConnPlug;

      ConnPlug = 0;
      notify_flags = 0;
      temp->UnPlug();
   }
}

inline void StrPlug::DoReadableNotify()
{
   if (PluggedInto() && (notify_flags & NotifyingOnReadable))
      PluggedInto()->ReadableNotify();
}

inline void StrPlug::DoWriteableNotify()
{
   if (PluggedInto() && (notify_flags & NotifyingOnWriteable))
      PluggedInto()->WriteableNotify();
}

#endif
