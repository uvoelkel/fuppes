/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_BASE_WATCHER_H
#define _THREADING_BASE_WATCHER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(HAVE_LIBEV)
    #include <ev.h>
    #define event_loop ev_loop
#elif defined(HAVE_LIBEVENT2)
    #include <event2/event.h>
    #define event_loop event_base
#endif


#include <stdio.h>

#include <iostream>

namespace Threading
{
    class EventLoop;

    class BaseWatcher
    {
        friend class EventLoop;

        public:
            BaseWatcher() {
                m_loop = NULL;
                #if defined(HAVE_LIBEVENT2)
                m_watcher = NULL;
                m_counter = 0;
                #endif
            }

            virtual ~BaseWatcher() {
                #if defined(HAVE_LIBEVENT2)
                if(NULL != m_watcher) {
                    //event_del(m_watcher);
                    event_free(m_watcher);
                    m_watcher = NULL;
                }
                #endif
            }
            
            #if defined(HAVE_LIBEV)
            
            virtual void start() = 0;
            virtual void stop() = 0;
            
            #elif defined(HAVE_LIBEVENT2)     
            
            void start() {

                if(0 < m_counter) {
                    throw "base watcher start error";
                }
                m_counter++;

                //m_loop = loop;
                event_add(m_watcher, NULL); //, const struct timeval *timeout);
                //cout << "AbstractWatcher::start: " << ret << endl;
            }
            
            void stop() {

                if(0 == m_counter) {
                    throw "base watcher stop error";
                }
                m_counter--;

                event_del(m_watcher);

                //cout << "AbstractWatcher::stop: " << ret << endl;
            }
            
            #endif

        protected:
            struct event_loop*  m_loop;

            #if defined(HAVE_LIBEVENT2)
            struct event*   m_watcher;
            int m_counter;
            #elif defined(HAVE_LIBEV)
            // libev uses different types depending on the usage 
            #endif
    };

}

#endif // _THREADING_BASE_WATCHER_H
