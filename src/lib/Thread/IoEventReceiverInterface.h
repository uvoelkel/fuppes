/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _THREADING_IO_EVENT_RECEIVER_INTERFACE_H
#define _THREADING_IO_EVENT_RECEIVER_INTERFACE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace Threading
{
    class IoWatcher;
    class IoReadWatcher;
    class IoWriteWatcher;
    
    class IoEventReceiverInterface
    {        
        public:
            virtual ~IoEventReceiverInterface() { }
            virtual void ioEvent(IoWatcher* watcher) = 0;
    };

    class IoReadEventReceiverInterface
    {        
        public:
            virtual ~IoReadEventReceiverInterface() { }
            virtual void ioReadEvent(IoReadWatcher* watcher) = 0;
    };

    class IoWriteEventReceiverInterface
    {        
        public:
            virtual ~IoWriteEventReceiverInterface() { }
            virtual void ioWriteEvent(IoWriteWatcher* watcher) = 0;
    };

}

#endif // _THREADING_IO_EVENT_RECEIVER_INTERFACE_H
