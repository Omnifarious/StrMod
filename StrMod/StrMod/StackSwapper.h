#ifndef _STR_StackSwapper_H_  // -*-c++-*-

#ifdef __GNUG__
#  pragma interface
#endif

/* $Header$ */

// For a log, see ../ChangLog

#define _STR_StackSwapper_H_

//: An exception safe class to do a difficult to describe thing.
// <p>This could be thought of as maintaining a particular variable
// (<code>classvar</code>) as the top stack value in a recursive function that
// maintained a stack of values on the execution stack.</p>
// <p>Using the constructor and destructor to maintain these values causes
// this manipulation to be exception safe.  It eliminates the need for a lot
// of <code>try { } catch (...) { }</code> blocks to clean stuff up and make
// things exception safe.</p>
template <class T> class StackSwapper {
 public:
   inline StackSwapper(T &classvar, const T &newval);
   inline ~StackSwapper();

 private:
   T &classvar_;
   T oldval_;

   // Left private and undefined on purpose.
   void operator =(const StackSwapper<T> &b);
   StackSwapper(const StackSwapper &b);
};

//-----------------------------inline functions--------------------------------

template <class T>
inline StackSwapper<T>::StackSwapper(T &classvar, const T &newval)
     : classvar_(classvar), oldval_(classvar)
{
   classvar = newval;
}

template <class T>
inline StackSwapper<T>::~StackSwapper()
{
   classvar_ = oldval_;
}

#endif
