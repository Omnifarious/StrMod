#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/UnixEventRegistry.h>
#include <UniEvent/UnixEventPoll.h>
#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class StdinEvent : public strmod::unievent::Event
{
 public:
   StdinEvent(strmod::unievent::UnixEventRegistry *ureg, int fd)
        : ureg_(ureg), fd_(fd) { }
   virtual void triggerEvent(strmod::unievent::Dispatcher *dispatcher = 0);

 private:
   strmod::unievent::UnixEventRegistry * const ureg_;
   const int fd_;
};

void StdinEvent::triggerEvent(strmod::unievent::Dispatcher *dispatcher)
{
   using std::cout;
   using strmod::unievent::UnixEventRegistry;
   char buf[321];
   cout << "You typed something on fd #" << fd_ << "!\n";
   cout.flush();
   int readbytes = ::read(fd_, buf, sizeof(buf) - 2);
   if (readbytes > 0)
   {
      cout << "You typed: \"";
      cout.write(buf, readbytes);
      cout << "\"\n";
   }
   else
   {
      cout << "I got an error or EOF\n";
   }
   cout.flush();
   UnixEventRegistry::FDCondSet tmp(UnixEventRegistry::FD_Readable);
   ureg_->registerFDCond(fd_, tmp, this);
}

int main(int argc, const char *argv[])
{
   strmod::unievent::SimpleDispatcher dispatcher;
   strmod::unievent::UnixEventPoll upoll(&dispatcher);
   using ::std::cerr;

   using strmod::unievent::UnixEventRegistry;
   upoll.printState(cerr);
   const UnixEventRegistry::FDCondSet rdcond(UnixEventRegistry::FD_Readable);
   upoll.registerFDCond(0, rdcond, new StdinEvent(&upoll, 0));
   for (int i = 1; i < argc; ++i)
   {
      cerr << "Opening up \"" << argv[i] << "\" for reading.\n";
      const int tempfd = open(argv[i], O_RDONLY | O_NONBLOCK);
      cerr << "Got fd # " << tempfd << "\n";
      if (tempfd >= 0)
      {
         upoll.registerFDCond(tempfd, rdcond, new StdinEvent(&upoll, tempfd));
      }
   }
//   upoll.printState(std::cerr);
   do {
      if (dispatcher.isQueueEmpty())
      {
         upoll.printState(::std::cerr);
         ::std::cerr << "\n";
         upoll.doPoll(true);
//         upoll.printState(std::cerr);
      }
      else
      {
//         upoll.printState(std::cerr);
         dispatcher.dispatchUntilEmpty();
//         upoll.printState(std::cerr);
      }
   } while (true);
}
