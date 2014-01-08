/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONTENTDATABASE_CONFIGURATION_H
#define _CONTENTDATABASE_CONFIGURATION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
#include <string>

#include "SharedObject.h"

namespace ContentDatabase
{
    typedef std::vector<SharedObject> SharedObjects_t;

    class Configuration
    {
        public:
            virtual ~Configuration()
            {
            }

            virtual bool isReadonly() = 0;
            virtual const std::string getLocalCharset() = 0;
            virtual const std::string getThumbnailDirectory() = 0;


            virtual SharedObjects_t getSharedObjects() = 0;
    };
    
}

#endif // _CONTENTDATABASE_CONFIGURATION_H
