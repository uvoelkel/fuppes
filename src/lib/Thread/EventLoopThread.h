/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_EVENT_LOOP_THREAD_H
#define _THREADING_EVENT_LOOP_THREAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Thread.h"
#include "EventLoop.h"
#include "AsyncWatcher.h"
#include "AsyncEventReceiverInterface.h"

namespace Threading
{
    class EventLoopThread: public Threading::Thread, public Threading::AsyncEventReceiverInterface
    {

      public:
            EventLoopThread(const std::string name);
            virtual ~EventLoopThread();
            
            void starting();
            virtual void stop();

            void asyncEvent(Threading::AsyncWatcher* watcher);
		
        protected:
	        virtual void run() = 0;

	        void startEventLoop();
	        void unloop();
            void clearEventLoop();

            Threading::EventLoop* getEventLoop();
		
        private:
            Threading::EventLoop*       m_eventLoop;
            Threading::AsyncWatcher*    m_asyncWatcher;
    };

}

#endif // _THREADING_EVENT_LOOP_THREAD_H
