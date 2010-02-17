#ifndef _STR_GV_Iterator_H_  // -*-c++-*-

/*
 * Copyright (C) 1991-9 Eric M. Hopper <hopper@omnifarious.mn.org>
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
//
// Revision 1.2  1996/09/21 20:08:20  hopper
// Fixed stupid bug in GroupVector::Iterator::operator +=(unsigned int) that
// caused advances of only one character, no matter what skip said.
//
// Revision 1.1  1996/05/08 10:56:33  hopper
// Initial functional revision
//

#include <StrMod/GroupVector.h>
#include <LCore/Object.h>

#define _STR_GV_Iterator_H_

class GroupVector::Iterator : public Object {
   friend class GroupVector;

 public:
   static const STR_ClassIdent identifier;

   inline Iterator(GroupVector &g);
   inline Iterator(const Iterator &b);
   inline const Iterator &operator =(const Iterator &b);
   inline virtual ~Iterator();

   inline virtual int AreYouA(const ClassIdent &cid) const;

   inline const char &operator *() const;

   inline const Iterator &operator ++();
   const Iterator &operator +=(unsigned int skip);

   inline const Iterator &operator --();
   const Iterator &operator -=(unsigned int skip);

   inline operator bool();

   inline void SeekToBegin();
   inline void SeekToEnd();

 protected:
   void HandleRepositionForward();
   void HandleRepositionBack(unsigned int skip);
   inline GroupVector &parent();
   inline const GroupVector &parent() const;

 private:
   GroupVector *parent_;
   GV_Size cur_group_;
   size_t cur_len_;
   size_t group_char_num_;
   bool atend_;
   const char *cur_char_;
   static const char dummy_ = '\0';

   void operator ++(int);
   void operator --(int);
};

//-----------------------------inline functions--------------------------------

inline GroupVector &GroupVector::Iterator::parent()
{
   return(*parent_);
}

inline const GroupVector &GroupVector::Iterator::parent() const
{
   return(*parent_);
}

inline GroupVector::Iterator::Iterator(GroupVector &g)
     : parent_(&g), cur_group_(0), group_char_num_(0), atend_(false)
{
   parent().FreeAllIOVecDescs();
   parent().AddIterator();
   HandleRepositionForward();
}

inline GroupVector::Iterator::Iterator(const Iterator &b)
     : parent_(b.parent_), cur_group_(b.cur_group_), cur_len_(b.cur_len_),
       group_char_num_(b.group_char_num_), atend_(b.atend_),
       cur_char_(b.cur_char_)
{
   parent().AddIterator();
}

inline const GroupVector::Iterator &
GroupVector::Iterator::operator =(const Iterator &b)
{
   if (this != &b) {
      parent().RemoveIterator();
      parent_ = b.parent_;
      parent().AddIterator();
      cur_group_ = b.cur_group_;
      cur_len_ = b.cur_len_;
      group_char_num_ = b.group_char_num_;
      atend_ = b.atend_;
      cur_char_ = b.cur_char_;
   }
   return(*this);
}

inline GroupVector::Iterator::~Iterator()
{
   parent().RemoveIterator();
}

inline int GroupVector::Iterator::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || Object::AreYouA(cid));
}

inline const char &GroupVector::Iterator::operator *() const
{
   return(*cur_char_);
}

inline const GroupVector::Iterator &GroupVector::Iterator::operator ++()
{
   if (!atend_) {
      group_char_num_++;
      if (group_char_num_ < cur_len_) {
	 cur_char_++;
      } else {
	 HandleRepositionForward();
      }
   }
   return(*this);
}

inline const GroupVector::Iterator &
GroupVector::Iterator::operator +=(unsigned int skip)
{
   if (!atend_) {
      group_char_num_ += skip;
      if (group_char_num_ < cur_len_) {
	 cur_char_ += skip;
      } else {
	 HandleRepositionForward();
      }
   }
   return(*this);
}

inline const GroupVector::Iterator &
GroupVector::Iterator::operator --()
{
   if (!atend_ && (group_char_num_ > 0)) {
      cur_char_--;
   } else {
      HandleRepositionBack(1);
   }
   return(*this);
}

inline const GroupVector::Iterator &
GroupVector::Iterator::operator -=(unsigned int skip)
{
   if (!atend_ && (group_char_num_ >= skip)) {
      cur_char_ -= skip;
   } else {
      HandleRepositionBack(skip);
   }
   return(*this);
}

inline GroupVector::Iterator::operator bool()
{
   return(!atend_);
}

inline void GroupVector::Iterator::SeekToBegin()
{
   cur_group_ = 0;
   group_char_num_ = 0;
   atend_ = false;
}

inline void GroupVector::Iterator::SeekToEnd()
{
   atend_ = true;
   cur_char_ = &dummy_;
}

#endif
