/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FAM_MONITOR_NULL_H
#define _FAM_MONITOR_NULL_H

namespace Fam
{

    class MonitorNull: public Monitor
    {
        public:
            MonitorNull(Fam::EventHandler& eventHandler) :
                    Monitor(eventHandler)
            {
            }

            virtual ~MonitorNull()
            {
            }

            bool addWatch(const std::string path, const bool isRoot = false)
            {
                return true;
            }

            bool removeWatch(const std::string path)
            {
                return true;
            }

            void run()
            {
            }

    };

}

#endif // _FAM_MONITOR_NULL_H
