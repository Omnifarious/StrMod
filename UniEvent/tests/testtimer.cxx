/*
 * Copyright 2001 Eric M. Hopper <hopper@omnifarious.org>
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

/* $Header$ */

// For a log, see ChangeLog

#include "UniEvent/UNIXSignalHandler.h"
#include "UniEvent/UNIXTimer.h"
#include "UniEvent/Timer.h"
#include "UniEvent/SimpleDispatcher.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <signal.h>

using strmod::unievent::Dispatcher;
using strmod::unievent::SimpleDispatcher;
using strmod::unievent::Event;
using strmod::unievent::EventPtr;
using strmod::unievent::UNIXSignalHandler;
using strmod::unievent::UNIXTimer;
using strmod::unievent::Timer;

class SimpleEvent : public Event {
 public:
   SimpleEvent(double posttime) : posttime_(posttime) {}
   virtual ~SimpleEvent() {}

   virtual void triggerEvent(Dispatcher *dispatcher)
   {
      timeval tv;
      gettimeofday(&tv, 0);
      long double curtime = tv.tv_sec + (tv.tv_usec / 1000000.0L);
      cerr << "SimpleEvent: (curtime - posttime_) == "
           << (curtime - posttime_) << ")\n";
   }

 private:
   double posttime_;
};

class MyEvent : public Event {
 public:
   MyEvent(Timer &timer, const Timer::absolute_t &now);
   virtual ~MyEvent();

   virtual void triggerEvent(Dispatcher *dispatcher);

 private:
   Timer &timer_;
   Timer::absolute_t bot_;  //!< Beginning Of Time
   int count_;
   long double lasttime_;
};

MyEvent::MyEvent(Timer &timer, const Timer::absolute_t &bot)
     : timer_(timer), bot_(bot), count_(0), lasttime_(0)
{
   cerr << "Created MyEvent\n";
}

MyEvent::~MyEvent()
{
   cerr << "Destroying MyEvent\n";
}

void MyEvent::triggerEvent(Dispatcher *dispatcher)
{
   timeval tv;
   gettimeofday(&tv, 0);
   cerr << "In MyEvent::triggerEvent";
   long double curtime = tv.tv_sec + (tv.tv_usec / 1000000.0L);
   if (lasttime_ != 0)
   {
      cerr << " (curtime - lasttime_) == " << (curtime - lasttime_) << ")";
   }
   lasttime_ = curtime;
   cerr << "\n";
   if (count_ != 0 && count_ != 2)
   {
      if (count_ == 1)
      {
         Timer::absolute_t next = bot_ + Timer::interval_t(7, 500000000U);
         timer_.postAt(next, this);
      }
      else if (count_ < 6)
      {
         cerr << "In 18.75 seconds!\n";
         timer_.postIn(Timer::interval_t(18, 750000000U), new SimpleEvent(curtime));
      }
   }
   ++count_;
}

class SleepEvent : public Event
{
 public:
   inline SleepEvent();
   virtual ~SleepEvent();

   virtual void triggerEvent(Dispatcher *dispatcher);
};

inline SleepEvent::SleepEvent()
{
   cerr << "Created SleepEvent\n";
}

SleepEvent::~SleepEvent()
{
   cerr << "Destroying SleepEvent\n";
}

void SleepEvent::triggerEvent(Dispatcher *dispatcher)
{
   cerr << "in SleepEvent::triggerEvent\n";
   if (dispatcher->isQueueEmpty())
   {
      sigset_t set;
      sigemptyset(&set);
      sigsuspend(&set);
   }
   dispatcher->onQueueEmpty(this);
}

int main(int argc, char *argv[])
{
   SimpleDispatcher disp;
   UNIXSignalHandler sighand(disp);
   UNIXTimer timer(disp, sighand);
   disp.onQueueEmpty(new SleepEvent);
   {
      ::time_t nowtt = ::time(0);
      Timer::absolute_t now(nowtt);
      EventPtr myev = new MyEvent(timer, now);
      Timer::absolute_t cur = now + 1;
      timer.postAt(cur, myev);
      cur = now + 5;
      timer.postAt(cur, myev);
      cur = now + 10;
      timer.postAt(cur, myev);
      cur = now + 11;
      for (int i = 0; i < 10; ++i)
      {
         timer.postAt(cur, myev);
         cur = cur + Timer::interval_t(7, 500000000U);
      }
   }
   while (1)
   {
      disp.dispatchEvent();
   }
}
