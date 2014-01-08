/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_THREAD_H
#define _THREADING_THREAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#include <shlwapi.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#include <string>
#include <map>
#include <list>

#include "Mutex.h"

namespace Threading
{

    #ifdef WIN32
    typedef DWORD       threadId_t;
    #else
    typedef pthread_t   threadId_t;
    #endif

    class Thread
    {

      public:
            Thread(const std::string name);
            virtual ~Thread();

            bool start(void* arg = NULL);
            virtual void stop() { m_stop = true; }
            bool stopRequested() const { return m_stop; };
            bool close();

            bool running() const { return m_running; }
            bool finished() const { return m_finished; }

            void name(const std::string name) { m_name = name; }
            std::string name() const { return m_name; }

            #ifdef WIN32
            threadId_t threadId() { return m_threadId; }    
            static inline threadId_t currentThreadId() { return GetCurrentThreadId(); }
            #else
            threadId_t threadId() { return m_handle; }
            static inline threadId_t currentThreadId() { return pthread_self(); }
            #endif
		
        protected:
            virtual void starting() { }
	        virtual void run() = 0;
		
	        #ifdef WIN32
            static DWORD threadFunc(void*);
            #else
            static void* threadFunc(void*);
            #endif

            void msleep(unsigned int milliseconds);

            

        private:
    
            std::string m_name;
            void*       m_arg;
            bool	    m_running;
            bool        m_finished;
            bool        m_stop;

            #ifdef WIN32
            HANDLE		    m_handle;
            DWORD           m_threadId;
            #else
            pthread_t		m_handle;
            pthread_cond_t	m_exitCondition;
            pthread_mutex_t m_mutex;
            #endif

    };


    class MainThread: public Threading::Thread
    {
        public:
            MainThread(threadId_t id):Thread("main") {
                m_threadId = id;
            }

            threadId_t threadId() { return m_threadId; }

        private:
            void run() { }
            threadId_t  m_threadId;
    };
}

#endif // _THREADING_THREAD_H
