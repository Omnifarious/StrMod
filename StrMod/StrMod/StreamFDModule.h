#ifndef _STR_StreamFDModule_H_  //-*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

 // $Log$
 // Revision 1.3  1996/07/05 18:47:04  hopper
 // Changed to use new StrChunkPtr stuff.
 //
 // Revision 1.2  1996/02/12 05:49:35  hopper
 // Changed to use new ANSI string class instead of RWCString.
 //
 // Revision 1.1.1.1  1995/07/22 04:46:50  hopper
 // Imported sources
 //
 // -> Revision 0.15  1995/04/14  15:39:49  hopper
 // -> Combined revision 0.14, and 0.14.0.4
 // ->
 // -> Revision 0.14.0.4  1995/04/05  04:53:01  hopper
 // -> Changed things for integration into the rest of my libraries.
 // ->
 // -> Revision 0.14.0.3  1994/06/16  02:18:44  hopper
 // -> Added #pragma interface declaration.
 // ->
 // -> Revision 0.14.0.2  1994/06/12  04:40:57  hopper
 // -> 	Made a change so that some methods in StrFDPlug could be conformant
 // -> with guidelines in Principles document.
 // ->
 // -> Revision 0.14.0.1  1994/05/16  04:26:26  hopper
 // -> Made some changes so this would use the Rogue Wave libraries instead
 // -> of mine. Also made a new revision branch for the WinterFire-OS/2
 // -> project.
 // ->
 // -> Revision 0.14  1994/05/07  03:40:55  hopper
 // -> Move enclosing #ifndef ... #define bits around. Changed names of some
 // -> non-multiple include constants. Changed directories for certain
 // -> type of include files.
 // ->
 // -> Revision 0.13  1992/04/26  01:28:41  hopper
 // -> Added support for blocks of a maximum size. Currently both
 // -> reads and writes use the same maximum, this may change though.
 // ->
 // -> Revision 0.12  1992/04/24  00:16:32  hopper
 // -> Added some stuff so I could specify whether I wanted a
 // -> StreamFDModule to be interrupted if the fd it's associated with becomes
 // -> readable or writeable. Before this was always on, and cause problems
 // -> when you wanted to only be worried about one or the other.
 // ->
 // -> Revision 0.11  1992/04/21  19:28:48  hopper
 // -> This is the real genesis, heh. This file has just about everything
 // -> added from the previous version.
 // ->
 // -> Revision 0.10  1992/03/23  02:37:37  hopper
 // -> Genesis! (It's really only partly completed. I just put
 // -> this in to shut up the RCS mode error messages.)
 // ->

#ifndef NO_RcsID
static char _StreamFDModule_H_rcsID[] =
      "$Id$";
#endif

#ifndef _STR_StreamModule_H_
#   include <StrMod/StreamModule.h>
#endif
#ifndef _STR_StrChunkPtr_H_
#   include <StrMod/StrChunkPtr.h>
#endif

#include <Dispatch/iohandler.h>

#include <string>

#define _STR_StreamFDModule_H_

class StrFDPlug;
class GroupVector;

//---------------------------class StreamFDModule------------------------------

class StreamFDModule : public StreamModule {
   friend class StrFDPlug;

 public:
   enum IOCheckFlags { CheckNone, CheckRead, CheckWrite, CheckBoth };

   static const STR_ClassIdent identifier;

   StreamFDModule(int fd,
		  IOCheckFlags f = CheckBoth, bool hangdelete = true);
   virtual ~StreamFDModule();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanCreate(int side) const;
   inline StrFDPlug *MakePlug(int side);
   inline virtual bool OwnsPlug(StrPlug *p) const;
   inline virtual bool DeletePlug(StrPlug *p);

   bool HasError() const                           { return(last_error != 0); }
   int ErrorNum() const                            { return(last_error); }
   void ClearError()                               { last_error = 0; }
   string ErrorString() const;
   int BestChunkSize();
   void SetMaxBlockSize(unsigned int mbs)          { max_block_size = mbs; }
   void UnsetMaxBlockSize()                        { max_block_size = 0; }
   unsigned int GetMaxBlockSize() const            { return(max_block_size); }

 protected:
   int fd, last_error;
   struct {
      unsigned int plugmade   : 1;
      unsigned int hangdelete : 1;
      unsigned int checkread  : 1;
      unsigned int checkwrite : 1;
   } flags;
   StrFDPlug *plug;
   StrChunkPtr cur_write, buffed_read;
   int write_pos;
   GroupVector *write_vec;
   unsigned int max_block_size;

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   inline virtual StrPlug *CreatePlug(int side);
};

//------------------------------class StrFDPlug--------------------------------

class StrFDPlug : public StrPlug, protected IOHandler {
friend class StreamFDModule;

 public:
   static const STR_ClassIdent identifier;

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline virtual bool CanWrite() const;
   virtual bool Write(const StrChunkPtr &);

   inline virtual bool CanRead() const;

   inline StreamFDModule *ModuleFrom() const;
   inline virtual int Side() const                     { return(0); }

 protected:
   int rdngfrm, wrtngto;

   StrFDPlug(StreamFDModule *parent);
   virtual ~StrFDPlug();

   virtual const ClassIdent *i_GetIdent() const        { return(&identifier); }

   virtual const StrChunkPtr InternalRead();

   virtual void ReadableNotify();
   virtual void WriteableNotify();

   virtual int inputReady(int fd);
   virtual int outputReady(int fd);
   virtual int exceptionRaised(int fd);
   virtual void timerExpired(long sec, long usec);
};
//-----------------inline functions for class StreamFDModule-------------------

inline int StreamFDModule::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StreamModule::AreYouA(cid));
}

inline bool StreamFDModule::CanCreate(int side) const
{
   return (side == 0 && !flags.plugmade);
}

inline StrFDPlug *StreamFDModule::MakePlug(int side)
{
   return((StrFDPlug *)(CreatePlug(side)));
}

inline bool StreamFDModule::OwnsPlug(StrPlug *p) const
{
   return(flags.plugmade && p == plug);
}

inline bool StreamFDModule::DeletePlug(StrPlug *p)
{
   if (OwnsPlug(p)) {
      flags.plugmade = 0;
      return(true);
   } else
      return(false);
}

inline StrPlug *StreamFDModule::CreatePlug(int side)
{
   if (CanCreate(side)) {
      flags.plugmade = 1;
      return(plug);
   } else
      return(0);
}

//-------------------inline functions for class StrFDPlug----------------------

inline int StrFDPlug::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || StrPlug::AreYouA(cid));
}

inline bool StrFDPlug::CanWrite() const
{
   return(!ModuleFrom()->cur_write);
}

inline bool StrFDPlug::CanRead() const
{
   return(ModuleFrom()->buffed_read);
}

inline StreamFDModule *StrFDPlug::ModuleFrom() const
{
   return(static_cast<StreamFDModule *>(StrPlug::ModuleFrom()));
}

#endif
