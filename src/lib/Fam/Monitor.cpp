/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Monitor.h"

#include "MonitorNull.h"

#ifdef HAVE_INOTIFY
#include "MonitorInotify.h"
#endif

#ifdef WIN32

#endif

using namespace Fam;

Fam::Monitor* Monitor::createMonitor(Fam::EventHandler& eventHandler, bool forceNull) // static
{
    if (forceNull) {
        return new MonitorNull(eventHandler);
    }

    Monitor* monitor = NULL;

#ifdef HAVE_INOTIFY
    monitor = new MonitorInotify(eventHandler);
#endif

#ifdef WIN32
    monitor = NULL;
#endif

    if (NULL == monitor) {
        monitor = new MonitorNull(eventHandler);
    }

    return monitor;
}
