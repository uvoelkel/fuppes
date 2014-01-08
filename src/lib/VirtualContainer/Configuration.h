/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _VIRTUALCONTAINER_CONFIG_H
#define _VIRTUALCONTAINER_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <list>

namespace VirtualContainer
{

    class ConfigEntry
    {
        public:
            std::string     name;
            std::string     cfgfile;
            bool            enabled;
    };


    class Configuration
    {
        public:
            bool enabled;
            std::list<VirtualContainer::ConfigEntry>   virtualContainers;
    };

    typedef std::list<VirtualContainer::ConfigEntry>::iterator   VirtualContainerIter;
}

#endif // _VIRTUALCONTAINER_CONFIG_H
