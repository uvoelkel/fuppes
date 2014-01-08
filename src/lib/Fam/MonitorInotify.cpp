/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "MonitorInotify.h"

using namespace Fam;

MonitorInotify::MonitorInotify(Fam::EventHandler& eventHandler) :
        Monitor(eventHandler)
{
}

MonitorInotify::~MonitorInotify()
{
    m_inotify.Close();

    std::map<std::string, InotifyWatch*>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); iter++) {
        delete iter->second;
    }
    m_watches.clear();
}

bool MonitorInotify::addWatch(const std::string path, const bool isRoot /*= false*/)
{
    InotifyWatch* watch = new InotifyWatch(path, IN_CREATE | IN_DELETE | IN_MOVE | IN_CLOSE_WRITE);

    //std::cout << "MonitorInotify::addWatch " << path << std::endl;

    try {
        m_inotify.Add(watch);
    }
    catch (InotifyException &ex) {

        //std::cout << "FAM: " << ex.GetMessage() << std::endl;

        delete watch;
        return false;
    }

    m_watches[path] = watch;
    return true;
}

bool MonitorInotify::removeWatch(const std::string path)
{
    InotifyWatch* watch = m_inotify.FindWatch(path);
    if (NULL == watch) {
        return false;
    }

    try {
        m_inotify.Remove(watch);
    }
    catch (InotifyException &ex) {
        //std::cout << "FAM: " << ex.GetMessage() << std::endl;
        return false;
    }

    m_watches.erase(path);
    delete watch;
    return true;
}

void MonitorInotify::run()
{
    InotifyEvent inotifyEvent;
    InotifyEvent inotifyPeek;

    m_inotify.SetNonBlock(true);

    while (!this->stopRequested()) {

        //std::cout << "FAM LOOP" << std::endl;

        // wait for events
        try {
            m_inotify.WaitForEvents();
        }
        catch (InotifyException &ex) {
            //std::cout << "FAM: " << ex.GetMessage() << std::endl;
            //Log::log_(Log::fam, Log::normal, __FILE__, __LINE__, "exception \"%s\"", ex.GetMessage().c_str());
        }

        if (this->stopRequested()) {
            break;
        }

        if (0 == m_inotify.GetEventCount()) {
            msleep(1000);
            continue;
        }

        //std::cout << "GOT EVENTS" << std::endl;

        while (m_inotify.GetEvent(&inotifyEvent)) {

            if (inotifyEvent.IsType(IN_IGNORED)) {
                continue;
            }

            /*std::string types;
            inotifyEvent.DumpTypes(types);
            std::cout << "inotify event: " << "cookie: " << inotifyEvent.GetCookie() << " types: " << types << std::endl;*/

            if (inotifyEvent.IsType(IN_CREATE) || inotifyEvent.IsType(IN_MOVED_TO)) {

                if (inotifyEvent.IsType(IN_ISDIR)) {
                    m_eventHandler.onDirectoryCreateEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName());
                }
                else {
                    m_eventHandler.onFileCreateEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName());
                }
            }
            else if (inotifyEvent.IsType(IN_DELETE)) {

                if (inotifyEvent.IsType(IN_ISDIR)) {
                    m_eventHandler.onDirectoryDeleteEvent(inotifyEvent.GetWatch()->GetPath() + inotifyEvent.GetName() + "/");
                }
                else {
                    m_eventHandler.onFileDeleteEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName());
                }
            }
            else if (inotifyEvent.IsType(IN_MOVED_FROM)) {

                // if the next event in the queue (if any) is a IN_MOVED_TO event with the same cookie as the current
                // the directory/file was renamed or moved inside the shared directories
                if (m_inotify.PeekEvent(inotifyPeek) && inotifyPeek.IsType(IN_MOVED_TO) && inotifyPeek.GetCookie() == inotifyEvent.GetCookie()) {
                    m_inotify.GetEvent(inotifyPeek);

                    if (inotifyEvent.IsType(IN_ISDIR)) {
                        m_eventHandler.onDirectoryMoveEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName(), inotifyPeek.GetWatch()->GetPath(), inotifyPeek.GetName());
                    }
                    else {
                        m_eventHandler.onFileMoveEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName(), inotifyPeek.GetWatch()->GetPath(), inotifyPeek.GetName());
                    }
                }
                // otherwise the directory/file is moved outside of the watched paths
                else {
                    if (inotifyEvent.IsType(IN_ISDIR)) {
                        m_eventHandler.onDirectoryDeleteEvent(inotifyEvent.GetWatch()->GetPath() + inotifyEvent.GetName() + "/");
                    }
                    else {
                        m_eventHandler.onFileDeleteEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName());
                    }
                }
            }
            else if (inotifyEvent.IsType(IN_CLOSE_WRITE)) {

                if (!inotifyEvent.IsType(IN_ISDIR)) {
                    m_eventHandler.onFileModifiedEvent(inotifyEvent.GetWatch()->GetPath(), inotifyEvent.GetName());
                }
            }

        }
    }

    //std::cout << "LOOP EXIT" << std::endl;
}

void MonitorInotify::stop()
{

    Monitor::stop();

}

