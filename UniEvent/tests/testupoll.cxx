/* $Header$ */

// For a log, see ChangeLog

#include <UniEvent/SimpleDispatcher.h>
#include <UniEvent/UnixEventRegistry.h>
#include <UniEvent/UnixEventPoll.h>
#include <UniEvent/Event.h>
#include <UniEvent/EventPtr.h>
#include <UniEvent/RegistryDispatcherGlue.h>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

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

class SigEvent : public strmod::unievent::Event
{
 public:
   SigEvent(strmod::unievent::UnixEventRegistry *ureg, int signo)
        : ureg_(ureg), signo_(signo), countdown_(false), repsleft_(0) { }
   SigEvent(strmod::unievent::UnixEventRegistry *ureg, int signo, unsigned int repct)
        : ureg_(ureg), signo_(signo), countdown_(true), repsleft_(repct) { }
   virtual void triggerEvent(strmod::unievent::Dispatcher *dispatcher = 0);

 private:
   strmod::unievent::UnixEventRegistry * const ureg_;
   const int signo_;
   const bool countdown_;
   unsigned int repsleft_;
};

void SigEvent::triggerEvent(strmod::unievent::Dispatcher *dispatcher)
{
   using std::cout;
   using strmod::unievent::UnixEventRegistry;
   cout << "I got signal #" << signo_ << " at event address " << this << "!\n";
   if (countdown_)
   {
      cout << "Repetitions left: " << --repsleft_ << "\n";
      if (repsleft_ == 0)
      {
         ureg_->clearSignal(signo_, this);
      }
   }
   cout.flush();
}

class TimerEvent : public strmod::unievent::Event
{
   typedef ::strmod::unievent::Timer::interval_t interval_t;
   typedef ::strmod::unievent::Timer::absolute_t absolute_t;
 public:
   TimerEvent()
        : created_(currentTime()), recur_(interval_t(0, 0)),
          last_(created_), recurbase_(created_), timer_(0)
   {
   }
   TimerEvent(strmod::unievent::Timer *timer,
              const interval_t &recurint)
        : created_(currentTime()), recur_(recurint),
          last_(created_), recurbase_(created_), timer_(timer)
   {
   }

   virtual void triggerEvent(strmod::unievent::Dispatcher *dispatcher = 0);

 private:
   const absolute_t created_;
   const interval_t recur_;
   absolute_t last_;
   absolute_t recurbase_;
   strmod::unievent::Timer * const timer_;

   static const absolute_t currentTime()
   {
      struct ::timeval tv;
      ::gettimeofday(&tv, 0);
      return absolute_t(tv.tv_sec, 0, tv.tv_usec * 1000U);
   }
   static double interval_to_double(const interval_t &intval)
   {
      return intval.seconds + double(intval.nanoseconds) / 1000000000U;
   }
};

void TimerEvent::triggerEvent(strmod::unievent::Dispatcher *dispatcher)
{
   using std::cout;
   using strmod::unievent::UnixEventRegistry;
   const absolute_t curtime = currentTime();
   const interval_t sincecreated = curtime - created_;
   const interval_t sincelast = curtime - last_;
   cout << "Got a timer event (" << this
        << ") at " << interval_to_double(sincecreated)
        << " since creation, and " << interval_to_double(sincelast)
        << " since the last time.\n";
   if ((recur_.seconds > 0) || (recur_.nanoseconds > 0))
   {
      cout << "Setting up another event in " << interval_to_double(recur_)
           << " seconds.\n";
      timer_->postAt(recurbase_ + recur_, this);
      recurbase_ = recurbase_ + recur_;
   }
   cout.flush();
   last_ = curtime;
}

int main(int argc, const char *argv[])
{
   using ::std::cerr;
   try {
      strmod::unievent::SimpleDispatcher dispatcher;
      strmod::unievent::UnixEventPoll upoll(&dispatcher);
      strmod::unievent::RegistryDispatcherGlue glue(&dispatcher, &upoll);

      using strmod::unievent::UnixEventRegistry;
      upoll.printState(cerr);
      cerr << "\n";
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
      upoll.onSignal(SIGINT, new SigEvent(&upoll, SIGINT));
      upoll.onSignal(SIGINT, new SigEvent(&upoll, SIGINT, 5));
      upoll.postIn(strmod::unievent::Timer::interval_t(10, 0), new TimerEvent());
      upoll.postIn(strmod::unievent::Timer::interval_t(3, 125000000U),
                   new TimerEvent(&upoll,
                                  strmod::unievent::Timer::interval_t(3, 125000000U)));
      upoll.printState(cerr);
      cerr << "\n";
      while (true)
      {
         cerr << "Dispatching!\n";
         dispatcher.dispatchUntilEmpty();
      }
   }
   catch (::std::range_error re)
   {
      cerr << "Died of a range_error: " << re.what() << "\n";
   }
   catch (::std::logic_error le)
   {
      cerr << "Died of a logic_error: " << le.what() << "\n";
   }
   catch (::std::runtime_error re)
   {
      cerr << "Died of a runtime_error: " << re.what() << "\n";
   }
   catch (::std::exception e)
   {
      cerr << "Died of an exception: " << e.what() << "\n";
   }
}
