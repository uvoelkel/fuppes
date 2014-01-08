/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_IO_WATCHER_H
#define _THREADING_IO_WATCHER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseWatcher.h"
#include "IoEventReceiverInterface.h"

#include <iostream>

namespace Threading
{

    /*
    class IoWatcher: public AbstractWatcher
    {
        public:
            
            void init(struct event_loop* loop, int sd, int flags, IoEventReceiverInterface* receiver) {

                m_loop = loop;
                
                #if defined(HAVE_LIBEV)
                m_watcher.data = this;
                ev_io* watcher = &m_watcher;
                ev_io_init(watcher, &IoWatcher::callback, sd, flags);
                #elif defined(HAVE_LIBEVENT2)
                short tmp = 0;
                m_watcher = event_new(m_loop, sd, tmp, &IoWatcher::callback, this);
                #endif

                m_receiver = receiver;
            }

            #if defined(HAVE_LIBEV)
            static void callback(struct ev_loop *loop, ev_io *watcher, int revents) {
                IoWatcher* self = (IoWatcher*)watcher->data;
                self->m_receiver->ioEvent(self);
            }
            #elif defined(HAVE_LIBEVENT2)
            static void callback(evutil_socket_t, short, void* data) {
                IoWatcher* self = (IoWatcher*)data;
                self->m_receiver->ioEvent(self);
            }
            #endif

            #if defined(HAVE_LIBEV)
            void start() {
                ev_io_start(m_loop, &m_watcher);
            }
            
            void stop() {
                ev_io_stop(m_loop, &m_watcher);    
            }
            #endif
            
        private:
            #if defined(HAVE_LIBEV)
            ev_io                       m_watcher;
            #endif
            
            IoEventReceiverInterface*   m_receiver;
    };
*/

    class IoReadWatcher: public BaseWatcher
    {
        public:
            IoReadWatcher() : BaseWatcher() {
                m_counter = 0;
                m_receiver = NULL;
            }


            #if defined(HAVE_LIBEV)
            ~IoReadWatcher() {
                //stop();
            }
            #endif
            
            void init(struct event_loop* loop, int sd, IoReadEventReceiverInterface* receiver) {
                
                m_loop = loop;
                m_receiver = receiver;
                
                #if defined(HAVE_LIBEV)
                m_watcher.data = this;
                ev_io* watcher = &m_watcher;
                ev_io_init(watcher, &IoReadWatcher::callback, sd, EV_READ);
                #elif defined(HAVE_LIBEVENT2)
                m_watcher = event_new(m_loop, sd, EV_READ|EV_PERSIST, &IoReadWatcher::callback, this); 
                #endif
            }

            #if defined(HAVE_LIBEV)
            static void callback(struct ev_loop *loop, ev_io *watcher, int revents) {
                IoReadWatcher* self = (IoReadWatcher*)watcher->data;
                self->m_receiver->ioReadEvent(self);
            }
            #elif defined(HAVE_LIBEVENT2)
            static void callback(evutil_socket_t, short, void* data) {
                IoReadWatcher* self = (IoReadWatcher*)data;
                self->m_receiver->ioReadEvent(self);
            }
            #endif

            #if defined(HAVE_LIBEV)
            void start() {

                if(0 < m_counter) {
                    throw "read watcher start error";
                }

                m_counter++;
                //std::cout << "IoReadWatcher::start: " << m_counter << std::endl;
                ev_io_start(m_loop, &m_watcher);
            }
            
            void stop() {

                if(0 == m_counter) {
                    throw "read watcher stop error";
                }

                m_counter--;
                //std::cout << "IoReadWatcher::stop: " << m_counter << std::endl;
                ev_io_stop(m_loop, &m_watcher);    
            }
            #endif
    
        private:

            #if defined(HAVE_LIBEV)
            ev_io                           m_watcher;
            #endif
            IoReadEventReceiverInterface*   m_receiver;

            int                             m_counter;
    };

    class IoWriteWatcher: public BaseWatcher
    {
        public:
            IoWriteWatcher(): BaseWatcher() {
                m_counter = 0;
                m_receiver = NULL;
            }


            #if defined(HAVE_LIBEV)
            ~IoWriteWatcher() {
                //stop();
            }
            #endif
            
            void init(struct event_loop* loop, int sd, IoWriteEventReceiverInterface* receiver) {

                m_loop = loop;
                m_receiver = receiver;  
                
                #if defined(HAVE_LIBEV)
                m_watcher.data = this;
                ev_io* watcher = &m_watcher;
                ev_io_init(watcher, &IoWriteWatcher::callback, sd, EV_WRITE);
                #elif defined(HAVE_LIBEVENT2)
                m_watcher = event_new(m_loop, sd, EV_WRITE|EV_PERSIST, &IoWriteWatcher::callback, this);
                #endif
            }

            #if defined(HAVE_LIBEV)
            static void callback(struct ev_loop *loop, ev_io *watcher, int revents) {
                IoWriteWatcher* self = (IoWriteWatcher*)watcher->data;
                self->m_receiver->ioWriteEvent(self);
            }
            #elif defined(HAVE_LIBEVENT2)
            static void callback(evutil_socket_t, short, void* data) {
                IoWriteWatcher* self = (IoWriteWatcher*)data;
                self->m_receiver->ioWriteEvent(self);
            }
            #endif
    

            #if defined(HAVE_LIBEV)
            void start() {

                if(0 != m_counter) {
                    throw "write watcher start error";
                }

                //std::cout << "IoWriteWatcher::start: " << m_counter << std::endl;

                m_counter++;
                ev_io_start(m_loop, &m_watcher);
            }
            
            void stop() {

                //std::cout << "IoWriteWatcher::stop: " << m_counter << std::endl;

                if(0 == m_counter) {
                    throw "write watcher stop error";
                }

                m_counter--;
                ev_io_stop(m_loop, &m_watcher);    
            }
            #endif
            
        private:

            #if defined(HAVE_LIBEV)
            ev_io                           m_watcher;
            #endif
            IoWriteEventReceiverInterface*  m_receiver;

            int                             m_counter;
    };

}

#endif // _THREADING_IO_WATCHER_H
