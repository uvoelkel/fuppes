/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FAM_EVENTHANDLER_H
#define _FAM_EVENTHANDLER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace Fam
{
    class EventHandler
    {
        public:
            virtual ~EventHandler()
            {
            }

            virtual void onDirectoryCreateEvent(const std::string path, const std::string name) = 0;
            virtual void onDirectoryDeleteEvent(const std::string path) = 0;
            virtual void onDirectoryMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name) = 0;

            virtual void onFileCreateEvent(const std::string path, const std::string name) = 0;
            virtual void onFileDeleteEvent(const std::string path, const std::string name) = 0;
            virtual void onFileMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name) = 0;
            virtual void onFileModifiedEvent(const std::string path, const std::string name) = 0;
    };

}

#endif // _FAM_EVENTHANDLER_H
