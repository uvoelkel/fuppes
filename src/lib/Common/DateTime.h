/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATE_TIME_H
#define _DATE_TIME_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <string>

namespace fuppes
{

    class DateTime
    {
        public:
            static DateTime     now();
            
            DateTime();
            DateTime(const time_t time);
            bool                valid();
            std::string         toString();
            int                 toInt();
            //DateTime&					operator=(const DateTime &dateTime);

        private:
            time_t    m_time;
    };

}

#endif // _DATE_TIME_H
