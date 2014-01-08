/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_EVENT_LOOP_H
#define _THREADING_EVENT_LOOP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseWatcher.h"

#if defined(HAVE_LIBEV)
    #include <ev.h>
#elif defined(HAVE_LIBEVENT2)
    #include <event2/event.h>
    #include <event2/thread.h>
#endif

/*
#include <stdio.h>
#include <iostream>
*/

namespace Threading
{
    
    class EventLoop
    {
        public:

            static void init() {
                #if defined(HAVE_LIBEV)
                #elif defined(HAVE_LIBEVENT2)

                #ifndef WIN32
                evthread_use_pthreads();
                #else
                evthread_use_windows_threads();
                #endif

                #endif
            }

            static void uninit() {
                #if defined(HAVE_LIBEV)
                #elif defined(HAVE_LIBEVENT2)
                #endif
            }
            
            EventLoop() {
                
                #if defined(HAVE_LIBEV)
                m_loop = ev_loop_new(0);
                #elif defined(HAVE_LIBEVENT2)
                m_config = event_config_new();
                //m_loop = event_base_new_with_config(m_config);
                m_loop = event_base_new();
                #endif
                
                m_running = false;
            }

            ~EventLoop() {

               if(NULL != m_loop) {
                    #if defined(HAVE_LIBEV)
                    ev_loop_destroy(m_loop);
                    #elif defined(HAVE_LIBEVENT2)
                    event_base_free(m_loop);
                    event_config_free(m_config);
                    #endif
                    m_loop = NULL;
                }
            }

            struct event_loop* getLoop() {
                return m_loop;
            }

            /*
            void startWatcher(AbstractWatcher* watcher) {
                watcher->start();
            }

            void stopWatcher(AbstractWatcher* watcher) {
                watcher->stop(); //this->m_loop
            }
            */
            
            void loop() {

                m_running = true;
                #if defined(HAVE_LIBEV)
                ev_loop(m_loop, 0); // 0 == will not return until either no event watchers are active anymore or ev_unloop was called
                #elif defined(HAVE_LIBEVENT2) 
                event_base_loop(m_loop, 0);
                #endif
            }

            void unloop() {
                //cout << "unloop 1" << endl;
                #if defined(HAVE_LIBEV)
                ev_unloop(m_loop, EVUNLOOP_ALL); //EVUNLOOP_ONE);
                #elif defined(HAVE_LIBEVENT2)
                event_base_loopbreak(m_loop);
                //evenet_base_loopexit
                #endif
                m_running = false;
                //cout << "unloop 2" << endl;
            }
            
            bool isRunning() {
                return m_running;
            }
            
        protected:

            #if defined(HAVE_LIBEV)
            struct event_loop* m_loop;
            #elif defined(HAVE_LIBEVENT2)
            struct event_config* m_config;
            struct event_loop* m_loop;
            #endif
            
            bool m_running;
    };

    class DefaultEventLoop: public EventLoop
    {
        public:
            DefaultEventLoop() {
                #if defined(HAVE_LIBEV)
                m_loop = ev_default_loop(0);
                #elif defined(HAVE_LIBEVENT2)
                m_config = event_config_new();
                m_loop = event_base_new_with_config(m_config);
                #endif
                m_running = false;
            }
            
            ~DefaultEventLoop() {
                m_loop = NULL;
            }
    };

}

#endif // _THREADING_EVENT_LOOP_H
