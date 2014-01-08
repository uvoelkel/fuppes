/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ThreadPool.h"

#include "../Common/Exception.h"

#include <errno.h>
#ifdef HAVE_CLOCK_GETTIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <assert.h>

#include <iostream>
using namespace std;
#include <stdio.h>

using namespace Threading;

ThreadPool* ThreadPool::m_instance = NULL;

void ThreadPool::init() // static
{
    assert(m_instance == NULL);
    m_instance = new ThreadPool();
    //m_instance->start();
}

void ThreadPool::uninit() // static
{
    assert(m_instance != NULL);
    m_instance->stop();
    m_instance->close();
    delete m_instance;
    m_instance = NULL;
}

ThreadPool::ThreadPool() :
        Thread("ThreadPool")
{
    m_mainThreadId = Thread::currentThreadId();
    m_mainThread = new MainThread(m_mainThreadId);
}

ThreadPool::~ThreadPool()
{
    delete m_mainThread;
}

void ThreadPool::add(Thread* thread) // static
{
    if (NULL == m_instance) {
        return;
    }

    MutexLocker lock(&m_instance->m_threadsMutex);
    m_instance->m_threads[thread->threadId()] = thread;
}

void ThreadPool::remove(Thread* thread) // static
{
    if (NULL == m_instance) {
        return;
    }

    MutexLocker lock(&m_instance->m_threadsMutex);
    m_instance->m_threads.erase(thread->threadId());
}

Thread* ThreadPool::threadById(threadId_t id) // static
{
    MutexLocker lock(&m_instance->m_threadsMutex);

    if (m_instance->m_mainThreadId == id) {
        return m_instance->m_mainThread;
    }
    else {
        return m_instance->m_threads[id];
    }

}

void ThreadPool::deleteLater(Thread* thread) // static 
{
    MutexLocker lock(&m_instance->m_deleteLaterMutex);
    m_instance->m_deleteLater.push_back(thread);

    if (false == m_instance->running()) {
        if (m_instance->finished()) {
            m_instance->close();
        }
        m_instance->start();
    }
}

void ThreadPool::run()
{
    std::list<Thread*>::iterator iter;

    while (!stopRequested()) {

        msleep(500);

        if (m_deleteLater.size() == 0) {
            return;
        }

        m_deleteLaterMutex.lock();
        for (iter = m_deleteLater.begin(); iter != m_deleteLater.end();) {

            if ((*iter)->finished()) {
                (*iter)->close();
                delete *iter;
                iter = m_deleteLater.erase(iter);
            }
            else {
                iter++;
            }
        }
        m_deleteLaterMutex.unlock();
    }

    // delete all remaining threads
    for (iter = m_deleteLater.begin(); iter != m_deleteLater.end(); iter++) {
        (*iter)->stop();
    }

    for (iter = m_deleteLater.begin(); iter != m_deleteLater.end();) {
        (*iter)->close();
        delete *iter;
        iter = m_deleteLater.erase(iter);
    }
}

