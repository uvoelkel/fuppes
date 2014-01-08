/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Mutex.h"

using namespace Threading;

Mutex::Mutex() 
{
    m_locked = false;

    #ifdef WIN32
    InitializeCriticalSection(&m_mutex);
    #else
    pthread_mutex_init(&m_mutex, NULL);
    #endif
}

Mutex::~Mutex() 
{
    #ifdef WIN32
    DeleteCriticalSection(&m_mutex);
    #else
    pthread_mutex_destroy(&m_mutex);
    #endif
}

void Mutex::lock() 
{
    #ifdef WIN32
    EnterCriticalSection(&m_mutex);
    #else
    pthread_mutex_lock(&m_mutex);
    #endif
    m_locked = true;
}

void Mutex::unlock() 
{
    #ifdef WIN32
    LeaveCriticalSection(&m_mutex);
    #else
    pthread_mutex_unlock(&m_mutex);
    #endif
    m_locked = false;
}
