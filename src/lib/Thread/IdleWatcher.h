/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_IDLE_WATCHER_H
#define _THREADING_IDLE_WATCHER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseWatcher.h"
#include "IdleEventReceiverInterface.h"


namespace Threading
{

    class IdleWatcher: public BaseWatcher
    {
        public:

            #if defined(HAVE_LIBEV)
            ~IdleWatcher() {
                stop();
            }
            #endif
            
            void init(IdleEventReceiverInterface* receiver) {
                m_watcher.data = this;
                ev_idle* watcher = &m_watcher;
                ev_idle_init(watcher, &IdleWatcher::callback);

                m_receiver = receiver;
            }

            static void callback(struct ev_loop *loop, ev_idle *watcher, int revents) {
                
                IdleWatcher* self = (IdleWatcher*)watcher->data;
                
                self->m_receiver->idleEvent(self);
            }
    
        private:
            void start(struct ev_loop* loop) {
                ev_idle_start(loop, &m_watcher);
            }

            void stop(struct ev_loop* loop) {
                ev_idle_stop(loop, &m_watcher);
            }

            ev_idle                       m_watcher;
            IdleEventReceiverInterface*   m_receiver;
    };

}

#endif // _THREADING_IDLE_WATCHER_H
