#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/UnixEventRegistry.h>
#include <UniEvent/UnixEventPoll.h>
#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>
#include <iostream>
#include <cstring>

class StdinEvent : public strmod::unievent::Event
{
 public:
   StdinEvent(strmod::unievent::UnixEventRegistry *ureg)
        : ureg_(ureg) { }
   virtual void triggerEvent(strmod::unievent::Dispatcher *dispatcher = 0);

 private:
   strmod::unievent::UnixEventRegistry * const ureg_;
};

void StdinEvent::triggerEvent(strmod::unievent::Dispatcher *dispatcher)
{
   using std::cout;
   using std::cin;
   using strmod::unievent::UnixEventRegistry;
   char buf[321];
   cout << "You typed something!\n";
   cout.flush();
   cin.getline(buf, sizeof(buf) - 2);
   buf[strlen(buf) - 1] = '\0';
   cout << "You typed: \"" << buf << "\"\n";
   cout.flush();
   UnixEventRegistry::FDCondSet tmp(UnixEventRegistry::FD_Readable);
   ::std::cerr << "tmp == " << tmp.to_string() << "\n";
   ureg_->registerFDCond(0, tmp, this);
}

int main()
{
   strmod::unievent::SimpleDispatcher dispatcher;
   strmod::unievent::UnixEventPoll upoll(&dispatcher);

   using strmod::unievent::UnixEventRegistry;
   upoll.printState(std::cerr);
   UnixEventRegistry::FDCondSet tmp(UnixEventRegistry::FD_Readable);
   ::std::cerr << "tmp == " << tmp.to_string() << "\n";
   UnixEventRegistry::FDCondSet tmp2;
   tmp2 = tmp;
   ::std::cerr << "tmp2 == " << tmp2.to_string() << "\n";
   upoll.registerFDCond(0, tmp2, new StdinEvent(&upoll));
   upoll.printState(std::cerr);
   do {
      if (dispatcher.isQueueEmpty())
      {
         upoll.printState(std::cerr);
         upoll.doPoll(true);
         upoll.printState(std::cerr);
      }
      else
      {
         upoll.printState(std::cerr);
         dispatcher.dispatchUntilEmpty();
         upoll.printState(std::cerr);
      }
   } while (true);
}
