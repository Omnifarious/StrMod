/* $Header$ */

// For log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "UNIXError.h"
#endif

#include "UniEvent/UNIXError.h"
#include <string.h>

const string &UNIXError::getErrorString() const
{
   if (!did_strerror_)
   {
      errstr_ = strerror(errno_);
   }
   return(errstr_);
}

