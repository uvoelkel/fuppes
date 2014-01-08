/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */


#include "Thread.h"
#include "ThreadPool.h"

#include "../Common/Exception.h"

#include <errno.h>
#ifdef HAVE_CLOCK_GETTIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <assert.h>



//#include <sys/syscall.h>


#include <iostream>
using namespace std;
#include <stdio.h>

using namespace Threading;

Thread::Thread(const std::string name)
{
    m_arg = NULL;
	m_name = name; // + " " + GenerateUUID();
	m_handle = 0;
	m_running = false;
	m_finished = false;
	m_stop = false;
    
#ifndef WIN32
    pthread_cond_init(&m_exitCondition, NULL);
    pthread_mutex_init(&m_mutex, NULL);
#else
    m_threadId = 0;
#endif
}

Thread::~Thread() 
{	
/*
	if(!close()) {
    error(Log::unknown, Log::normal) << "error closing thread:" << m_name;
	}
*/ 

    //this->clearEventLoop();
    
	if(m_handle) {
    //Log::error(Log::unknown, Log::normal, __FILE__, __LINE__, "FATAL: destroying an unfinished thread %s", m_name.c_str());
    throw fuppes::Exception(__FILE__, __LINE__, "destroying an unfinished thread %s", m_name.c_str()); 
	}

#ifndef WIN32
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_exitCondition);
#endif
}

	
bool Thread::start(void* arg /* = NULL*/)
{
	if(m_running)
		return false;
	
	if(0 != m_handle) {
	    std::string msg = "starting a thread (" + m_name + ") with an already used handle. did you forget to call close()?";
	    throw fuppes::Exception(__FILE__, __LINE__, msg.c_str());
	}

    m_arg = arg;
    m_stop = false;
    m_finished = false;
  
#ifdef WIN32
    m_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Thread::threadFunc, this, 0, &m_threadId);
    m_running = (m_handle != NULL);
#else 
    int ret = pthread_create(&m_handle, NULL, &Thread::threadFunc, this);
    m_running = (0 == ret);
#endif

	return m_running;	
}


bool Thread::close()
{
	//CSharedLog::Log(L_NORM, __FILE__, __LINE__, "close thread %s", m_name.c_str());
	
	if(!m_handle)
		return true;
	
	m_stop = true;
  bool result = false;
	
#ifdef WIN32
  DWORD nErrNo = WaitForSingleObject(m_handle, 5000);
  switch(nErrNo) {
    case WAIT_ABANDONED:
      /*cout << "WAIT_ABANDONED :: " << nErrNo << endl;*/
      break;
    case WAIT_OBJECT_0:
      //cout << "WAIT_OBJECT_0 :: " << nErrNo << endl;
      CloseHandle(m_handle);
      result = true;
      break;
    case WAIT_TIMEOUT:
      /*cout << "fuppesThreadClose() :: WAIT_TIMEOUT (" << nErrNo << ")" << endl; */
      break;
    case WAIT_FAILED:
      /*cout << "fuppesThreadClose() :: WAIT_FAILED (" << nErrNo << ")" << endl;*/
      break;
    default:
      /*cout << "fuppesThreadClose - DEFAULT :: " << nErrNo << endl; */
      break;            
  }
#else    
  result = true;
  int err = 0;

	pthread_mutex_lock(&m_mutex);
	if(!m_finished) {
	

		timespec timeout;
#ifdef HAVE_CLOCK_GETTIME
		clock_gettime(CLOCK_REALTIME, &timeout);		
		timeout.tv_sec += 5;
#else
		timeval time;
		gettimeofday(&time, NULL);
		timeout.tv_sec = time.tv_sec + 5;
#endif
		
		err = pthread_cond_timedwait(&m_exitCondition, &m_mutex, &timeout);
		if(err == ETIMEDOUT && !m_finished) {
            std::cout << "FATAL ERROR: pthread_cond_timedwait failed on thread:" << m_name << std::endl;
			result = false;
		}
		
	}
	pthread_mutex_unlock(&m_mutex);

  if(!result)
    return result;
	
	err = pthread_join(m_handle, NULL);  
  if (err != 0) {
    result = false;
    /*switch(nErrNo) {
      case EINVAL:
        cout << "pthread_join() :: " << nErrNo << " EINVAL = handle does not refer to a joinable thread" << endl;      
        break;
      case ESRCH:
        cout << "pthread_join() :: " << nErrNo << " ESRCH = No thread found with the given thread handle" << endl;
        break;
      case EDEADLK:
        cout << "pthread_join() :: " << nErrNo << " EDEADLK = deadlock detected" << endl;      
        break;
    }*/
  }
#endif

	m_handle = 0;
  return result;
}

#ifdef WIN32
DWORD Thread::threadFunc(void* thread)
#else
void* Thread::threadFunc(void* thread)
#endif
{
    Thread* pt = static_cast<Thread*>(thread);

    ThreadPool::add(pt);

    //pid_t tid = (pid_t) syscall (SYS_gettid);    
    //cout << pt->threadId() << " " << pt->m_name << " " << tid << endl;
    pt->starting();
    pt->run();

    ThreadPool::remove(pt);

    #ifdef WIN32
    pt->m_running = false;
    pt->m_finished = true;
    ExitThread(0);
    return 0;
    #else
    pthread_mutex_lock(&pt->m_mutex);
    pt->m_running = false;
    pt->m_finished = true;

    pthread_cond_signal(&pt->m_exitCondition);
    pthread_mutex_unlock(&pt->m_mutex);
    pthread_exit(NULL);
    #endif
}

void Thread::msleep(unsigned int milliseconds)
{
  #ifdef WIN32
  Sleep(milliseconds);
  #else
  if(milliseconds < 1000)
    usleep(milliseconds * 1000);
  else
    sleep(milliseconds / 1000);  
  #endif
}
