#ifndef StrChunk_EHChunk

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

/* $Log$
/* Revision 1.1  1995/07/22 04:46:50  hopper
/* Initial revision
/*
 * Revision 0.19  1995/01/13  17:16:21  hopper
 * Merge version 0.18, and 0.18.0.3
 *
 * Revision 0.18.0.3  1995/01/13  17:10:47  hopper
 * Added an enum element for OS2SocketModuleChunk.
 *
 * Revision 0.18.0.2  1994/06/16  02:42:33  hopper
 * Added #pragma interface declaration.
 *
 * Revision 0.18.0.1  1994/05/16  02:51:37  hopper
 * No changes, but I needed a head for the inevitable WinterFire-OS/2
 * branch.
 *
 * Revision 0.18  1994/05/07  03:40:55  hopper
 * Move enclosing #ifndef ... #define bits around. Changed names of some
 * non-multiple include constants. Changed directories for certain
 * type of include files.
 *
 * Revision 0.17  1992/05/05  17:41:31  hopper
 * Changed SocketModule to SocketModuleChunk to fit a little better
 * with a new scheme I thought up for SockListenModule
 *
 * Revision 0.16  1992/05/05  01:38:38  hopper
 * Added SocketModule to enum
 *
 * Revision 0.15  1992/04/14  20:49:01  hopper
 * Added DataBlockStrChunk to the enum of stream chunk types.
 *
 * Revision 0.14  1992/02/10  05:18:55  hopper
 * Added Ulaw8_StrChunk to enum
 *
 * Revision 0.13  1992/02/10  04:20:04  hopper
 * Change #define to enum nested within a class.
 *
 * Revision 0.12  1991/11/24  22:56:21  hopper
 * Fixed some silly syntax errors.
 *
 * Revision 0.11  1991/11/21  21:49:15  acm
 * Added the $Id$ string.
 *
 * Revision 0.10  1991/11/21  21:46:45  hopper
 * Genesis!
 *
*/

#define StrChunk_EHChunk

#ifndef NO_RcsID
static char *_EH_ChunkType_H_rcsID =
      "$Id$";
#endif

class EHChunk { // This class is here solely for the imbedded enum. I think a
                // class makes a very good nested scope thingy for enum's
   EHChunk() {}
 public:
   enum EHChunkTypes { StrChunk = 0, BufferStrChunk, DataBlockStrChunk,
		       SocketModuleChunk, OS2SocketModuleChunk };

   void TurnOffWarning() {}

   ~EHChunk() {}
};   

// SocketModule is kind of a wierd case up there. It's there so that
// SockListenModule's fit in better. :-)

#endif
