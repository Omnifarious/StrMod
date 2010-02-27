/*
 * Copyright 2001-2010 Eric M. Hopper <hopper@omnifarious.org>
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

#include "UniEvent/SimpleDispatcher.h"
#include "UniEvent/UNIXSignalHandler.h"
#include "UniEvent/Event.h"
#include "UniEvent/EventPtr.h"
#include <iostream>
#include <unistd.h>

using strmod::unievent::Dispatcher;
using strmod::unievent::SimpleDispatcher;
using strmod::unievent::Event;
using strmod::unievent::EventPtr;
using strmod::unievent::UNIXSignalHandler;

class MyEvent : public Event {
 public:
   inline MyEvent();
   virtual ~MyEvent();

   virtual void triggerEvent(Dispatcher *dispatcher);
};

inline MyEvent::MyEvent()
{
   cerr << "Created MyEvent\n";
}

MyEvent::~MyEvent()
{
   cerr << "Destroying MyEvent\n";
}

void MyEvent::triggerEvent(Dispatcher *dispatcher)
{
   cerr << "in MyEvent::triggerEvent\n";
}

class SleepEvent : public Event {
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
   sleep(1);
   dispatcher->onQueueEmpty(this);
}

int main(int argc, char *argv[])
{
   SimpleDispatcher disp;
   UNIXSignalHandler sighand(disp);
   EventPtr myev = new MyEvent;
   disp.onQueueEmpty(new SleepEvent);
   sighand.onSignal(2, myev, false);
   while (1)
   {
      disp.dispatchEvent();
   }
}
