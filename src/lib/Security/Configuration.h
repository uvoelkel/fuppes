/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SECURITY_CONFIG_H
#define _SECURITY_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <list>
#include <string>

namespace Security
{
    class Configuration
    {
        public:
            std::string             hostIp;

            std::list<std::string>  allowedIps;
            std::list<std::string>  allowedMacs;
    };
    
};

#endif // _SECURITY_CONFIG_H
