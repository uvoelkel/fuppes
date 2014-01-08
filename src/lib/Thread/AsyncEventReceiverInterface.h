/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_ASYNC_EVENT_RECEIVER_INTERFACE_H
#define _THREADING_ASYNC_EVENT_RECEIVER_INTERFACE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace Threading
{
    class AsyncWatcher;
    
    class AsyncEventReceiverInterface
    {        
        public:
            virtual ~AsyncEventReceiverInterface() { }
            virtual void asyncEvent(AsyncWatcher* watcher) = 0;
    };

}

#endif // _THREADING_ASYNC_EVENT_RECEIVER_INTERFACE_H
