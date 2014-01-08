/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SECURITY_ACCESS_CONTROL_H
#define _SECURITY_ACCESS_CONTROL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <map>
#include <list>
#include <string>

#include "Configuration.h"

#include "MacAddressTable.h"
#include "../Thread/Mutex.h"

namespace Security
{
    struct AccessControlEntry
    {
        std::string     ip;
        std::string     mac;
        int             port;
        std::string     method;
    };
    
    struct AccessControlList
    {
        std::string     uri;
        
        std::list<AccessControlEntry*>   aces;

        ~AccessControlList() {
            std::list<AccessControlEntry*>::iterator iter;
            for(iter = aces.begin(); iter != aces.end(); iter++) {
                delete *iter;
            }
            aces.clear();
        }
    };
    
    class AccessControl
    {
        public:            
            AccessControl(Security::Configuration config);
            ~AccessControl();
            
            bool isAllowed(std::string ip, int port = 0, std::string method = "*", std::string uri = "*");

            void addIpAce(std::string ip, int port = 0, std::string method = "*", std::string uri = "*");
            void addMacAce(std::string mac, int port = 0, std::string method = "*", std::string uri = "*");

            MacAddressTable* getMacAddressTable() { return &m_macAddressTable; }
            
        private:                        
            std::string                                 m_hostIp;
            MacAddressTable                             m_macAddressTable;

            Threading::Mutex                            m_mutex;
            std::map<std::string, AccessControlList*>   m_acls;
    };
};

#endif // _SECURITY_ACCESS_CONTROL_H
