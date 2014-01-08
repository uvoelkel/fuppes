/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_ASYNC_WATCHER_H
#define _THREADING_ASYNC_WATCHER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseWatcher.h"
#include "AsyncEventReceiverInterface.h"


namespace Threading
{

    class AsyncWatcher: public BaseWatcher
    {
        public:
            #if defined(HAVE_LIBEV)
            ~AsyncWatcher() {
                //stop();
            }
            #endif
            
            void init(struct event_loop* loop, AsyncEventReceiverInterface* receiver) {

                m_running = false;
                m_loop = loop;
                m_receiver = receiver;
                
                #if defined(HAVE_LIBEV)
                m_watcher.data = this;
                ev_async* watcher = &m_watcher;
                ev_async_init(watcher, &AsyncWatcher::callback);
                #elif defined(HAVE_LIBEVENT2)
                m_watcher = event_new(m_loop, -1, 0, &AsyncWatcher::callback, this); // EV_SIGNAL
                #endif
            }

            void send() {
                #if defined(HAVE_LIBEV)
                ev_async_send(m_loop, &m_watcher);
                #elif defined(HAVE_LIBEVENT2)
                event_active(m_watcher, 0, 0);
                #endif
            }

            #if defined(HAVE_LIBEV)
            static void callback(struct ev_loop *loop, ev_async *watcher, int revents) {
                AsyncWatcher* self = (AsyncWatcher*)watcher->data;
                self->m_receiver->asyncEvent(self);
            }
            #elif defined(HAVE_LIBEVENT2)
            static void callback(evutil_socket_t, short, void* data) {
                AsyncWatcher* self = (AsyncWatcher*)data;
                self->m_receiver->asyncEvent(self);
            }
            #endif
    
            #if defined(HAVE_LIBEV)
            void start() {

                if(true == m_running) {
                    throw "async watcher start error";
                }

                ev_async_start(m_loop, &m_watcher);
                m_running = true;
            }

            void stop() {
                if(false == m_running) {
                    throw "async watcher stop error";
                }
                ev_async_stop(m_loop, &m_watcher);
                m_running = false;
            }
            #endif

        private:
            #if defined(HAVE_LIBEV)
            ev_async                       m_watcher;
            #endif

            bool m_running;
            AsyncEventReceiverInterface*   m_receiver;
    };

}

#endif // _THREADING_ASYNC_WATCHER_H
