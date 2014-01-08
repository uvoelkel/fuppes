/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREAD_MUTEX_H
#define _THREAD_MUTEX_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#include <shlwapi.h>
#else
#include <pthread.h>
#endif

namespace Threading
{

    class Mutex
    {
	      public:
            Mutex();
            ~Mutex();

            void lock();
            void unlock();

            bool locked() const { return m_locked; }
		
	      private:
		        #ifdef WIN32
		        CRITICAL_SECTION  m_mutex;
		        #else
		        pthread_mutex_t   m_mutex;
		        #endif
		        bool m_locked;
    };


    class MutexLocker
    {
        public:
            MutexLocker(Mutex* mutex) {
	              m_mutex = mutex;
	              m_mutex->lock();
            }
            ~MutexLocker() {
	              m_mutex->unlock();
            }

        private:
            Mutex*  m_mutex;
    };

}

#endif // _THREAD_MUTEX_H
