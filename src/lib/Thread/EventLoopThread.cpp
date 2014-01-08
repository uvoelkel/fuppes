/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "EventLoopThread.h"

using namespace Threading;

EventLoopThread::EventLoopThread(const std::string name) :
        Thread(name)
{
    m_eventLoop = NULL;
    m_asyncWatcher = NULL;
}

EventLoopThread::~EventLoopThread()
{
    this->clearEventLoop();
}


void EventLoopThread::starting()
{
    if(NULL != m_eventLoop) {
        return;
    }
    m_eventLoop = new EventLoop();

    m_asyncWatcher = new Threading::AsyncWatcher();
    m_asyncWatcher->init(m_eventLoop->getLoop(), this);
    m_asyncWatcher->start();

    Thread::starting();
}

void EventLoopThread::stop()
{
    if(NULL != m_asyncWatcher) {
        m_asyncWatcher->send();
    }
    Thread::stop();
}


void EventLoopThread::asyncEvent(Threading::AsyncWatcher* watcher)
{
    watcher->stop();
    m_eventLoop->unloop();
}


void EventLoopThread::unloop()
{
    if(NULL == m_eventLoop) {
        return;
    }
    m_eventLoop->unloop();
}

void EventLoopThread::startEventLoop()
{
    if(NULL == m_eventLoop) {
        return;
    }

    m_eventLoop->loop();
}

void EventLoopThread::clearEventLoop()
{
    if(NULL != m_eventLoop) {
        delete m_eventLoop;
        m_eventLoop = NULL;
    }

    if(NULL != m_asyncWatcher) {
        delete m_asyncWatcher;
        m_asyncWatcher = NULL;
    }
}


Threading::EventLoop* EventLoopThread::getEventLoop()
{
    return m_eventLoop;
}
