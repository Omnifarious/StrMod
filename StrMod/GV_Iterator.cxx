/* $Header$ */

// $Log$
// Revision 1.3  1999/01/12 04:13:40  hopper
// All kinds of changes to make the code more portable and compile
// properly with the DEC compiler.
//
// Revision 1.2  1998/11/03 00:32:40  hopper
// Fixed small syntax errors that g++ should've caught.
//
// Revision 1.1  1996/05/08 10:56:23  hopper
// Initial functional revision
//

#ifdef __GNUG__
#  pragma implementation "GV_Iterator.h"
#endif

#include <StrMod/GV_Iterator.h>

const STR_ClassIdent GroupVector::Iterator::identifier(17UL);
// See class declaration for initial value.
const char GroupVector::Iterator::dummy_;

void GroupVector::Iterator::HandleRepositionForward()
{
   if (cur_group_ >= parent().numgroups_) {
      atend_ = true;
      cur_char_ = &dummy_;
   }
   if (atend_) {
      return;
   }
   if ((cur_group_ == 0) && (group_char_num_ == 0)) {
      cur_len_ = parent().groups_[0].iov_len;
   }
   while ((cur_group_ < parent().numgroups_) &&
	  (group_char_num_ >= cur_len_)) {
      group_char_num_ -= cur_len_;
      cur_group_++;
      if (cur_group_ < parent().numgroups_) {
	 cur_len_ = parent().groups_[cur_group_].iov_len;
      }
   }
   if (cur_group_ >= parent().numgroups_) {
      atend_ = true;
      cur_char_ = &dummy_;
   } else {
      cur_char_ =
	 static_cast<char *>(parent().groups_[cur_group_].iov_base)
	 + group_char_num_;
   }
}

void GroupVector::Iterator::HandleRepositionBack(unsigned int skip)
{
   if (skip == 0) {
      return;
   }
   if (atend_) {
      if (parent().numgroups_ <= 0) {
	 return;
      }
      cur_group_ = parent().numgroups_ - 1;
      group_char_num_ = parent().groups_[cur_group_].iov_len;
   }
   while ((skip > group_char_num_) && (cur_group_ > 0)) {
      skip -= group_char_num_;
      cur_group_--;
      group_char_num_ = parent().groups_[cur_group_].iov_len;
   }
   cur_len_ = parent().groups_[cur_group_].iov_len;
   if (skip >= group_char_num_) {
      group_char_num_ = 0;
   } else {
      group_char_num_ -= skip;
   }
}

#ifdef __GNUG__

GroupVector::Iterator GroupVector::begin() return r(*this);
{
   return;
}

GroupVector::Iterator GroupVector::end() return r(*this);
{
   r.SeekToEnd();
   return;
}

#else

GroupVector::Iterator GroupVector::begin()
{
   return(Iterator(*this));
}

GroupVector::Iterator GroupVector::end()
{
   Iterator r(*this);

   r.SeekToEnd();
   return(r);
}

#endif
