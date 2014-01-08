/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONTENTDATABASE_SHAREDOBJECT_H
#define _CONTENTDATABASE_SHAREDOBJECT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace ContentDatabase
{
    class SharedObject
    {
        public:
            enum Type
            {
                unknown = 0,
                directory = 1,
                playlist = 2,
                other = 3,
            };

            SharedObject()
            {
                type = SharedObject::unknown;
            }

            virtual ~SharedObject()
            {
            }

            SharedObject::Type type;
            std::string path;

            std::string name;
            std::string otherType;

    };

}

#endif // _CONTENTDATABASE_SHAREDOBJECT_H
