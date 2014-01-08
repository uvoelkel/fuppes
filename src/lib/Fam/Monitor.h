/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FAM_MONITOR_H
#define _FAM_MONITOR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/Thread.h"

#include "EventHandler.h"

namespace Fam
{

    class Monitor: public Threading::Thread
    {
        public:
            static Fam::Monitor* createMonitor(Fam::EventHandler& eventHandler, bool forceNull);

            virtual bool addWatch(const std::string path, const bool isRoot = false) = 0;
            virtual bool removeWatch(const std::string path) = 0;

        protected:
            Monitor(Fam::EventHandler& eventHandler) :
                    Threading::Thread::Thread("fam monitor"),
                    m_eventHandler(eventHandler)
            {

            }

            Fam::EventHandler& m_eventHandler;
    };

}

#endif // _FAM_MONITOR_H
