/* $Header$ */

// $Log$
// Revision 1.1  1997/05/12 16:24:40  hopper
// Preliminary, possibly incomplete test of RefCountPtr classes.
//

#include <iostream.h>
#include "LCore/RefCounting.h"
#include "LCore/RefCountPtr.h"
#include "LCore/RefCountPtrT.h"

static inline ostream &operator <<(ostream &os, const ReferenceCounting &obj)
{
   os << "RC(" << obj.NumReferences() << ")";
   return(os);
}

static inline ostream &operator <<(ostream &os, const RefCountPtr &ptr)
{
   if (ptr) {
      os << "->RC(" << ptr->NumReferences() << ")";
   } else {
      os << "->NULL";
   }
   return(os);
}

//---

class MyRC : public ReferenceCounting {
 public:
   static const EH_ClassIdent identifier;

   MyRC(ostream &os) : ReferenceCounting(0), logstr(os) { }
   virtual ~MyRC();

   inline virtual int AreYouA(const ClassIdent &cid) const;

 protected:
   inline virtual const ClassIdent *i_GetIdent() const { return(&identifier); }

 private:
   ostream &logstr;
};

const EH_ClassIdent MyRC::identifier(EH_ClassNum(EH_ClassNum::User0, 0));

MyRC::~MyRC()
{
   logstr << "MyRC dying: " << *this << "\n";
}

inline int MyRC::AreYouA(const ClassIdent &cid) const
{
   return((identifier == cid) || ReferenceCounting::AreYouA(cid));
}

//---

int main()
{
   ReferenceCounting obj(1);
   RefCountPtrT<MyRC> mptr = new MyRC(cout);
   cout << "1: " << obj << endl;

   {
      RefCountPtr ptr = &obj;
      RefCountPtr &rmptr = mptr;

      cout << "2: " << obj << endl;
      ptr = mptr;
      cout << "3: mptr == " << mptr << " && obj == " << obj << "\n";
      ptr = 0;
      cout << "4: mptr == " << mptr << " && obj == " << obj << "\n";
      ptr = rmptr;
      cout << "5: mptr == " << mptr << " && obj == " << obj << "\n";
      rmptr = 0;
      cout << "6: mptr == " << mptr << " && obj == " << obj << "\n";
      rmptr = ptr;
      cout << "7: mptr == " << mptr << " && obj == " << obj << "\n";
      rmptr = &obj;
      cout << "8: mptr == " << mptr << " && obj == " << obj << endl;
   }

   cout << "9: " << obj << endl;
   return(0);
}
