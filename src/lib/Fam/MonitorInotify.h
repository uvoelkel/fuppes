/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FAM_MONITOR_INOTIFY_H
#define _FAM_MONITOR_INOTIFY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Monitor.h"

#include "inotify-cxx-0.7.4/inotify-cxx.h"
#include <map>

namespace Fam
{

    class MonitorInotify: public Monitor
    {
        public:
            MonitorInotify(Fam::EventHandler& eventHandler);
            virtual ~MonitorInotify();

            bool addWatch(const std::string path, const bool isRoot = false);
            bool removeWatch(const std::string path);

            void run();
            void stop();

        private:
            Inotify m_inotify;
            std::map<std::string, InotifyWatch*> m_watches;
    };

}

#endif // _FAM_MONITOR_INOTIFY_H
