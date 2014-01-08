/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_THREAD_POOL_H
#define _THREADING_THREAD_POOL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Thread.h"

#include <string>
#include <map>
#include <list>

namespace Threading
{
    class ThreadPool: public Threading::Thread
    {
        public:
            static void init();
            static void uninit();

            static void add(Thread* thread);
            static void remove(Thread* thread);

            static Threading::Thread* threadById(threadId_t id);

            static void deleteLater(Thread* thread);

            //static void lock() { return m_instance->m_mutex.lock(); }
            static std::map<threadId_t, Thread*>* threads() { return &m_instance->m_threads; }
            //static void unlock() { return m_instance->m_mutex.unlock(); }

        private:
            ThreadPool();
            ~ThreadPool();

            void run();

            static ThreadPool* m_instance;

            Thread*                            m_mainThread;
            threadId_t                         m_mainThreadId;

            Threading::Mutex                   m_threadsMutex;
            std::map<threadId_t, Thread*>      m_threads;

            Threading::Mutex                   m_deleteLaterMutex;
            std::list<Thread*>                 m_deleteLater;
    };

}

#endif // _THREADING_THREAD_POOL_H
