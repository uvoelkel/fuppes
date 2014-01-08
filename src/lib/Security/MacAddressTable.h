/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SECURITY_MAC_ADDRESS_TABLE_H
#define _SECURITY_MAC_ADDRESS_TABLE_H

#include <string>
#include <map>
#include "../Thread/Mutex.h"

namespace Security
{

    class MacAddressTable
    {
        public:
            //static void  init();
            //static void  uninit();
            bool  mac(std::string ip, std::string& mac);
            void  clear();

        private:
            //static MacAddressTable*               instance();

            //static MacAddressTable*               m_instance;
            std::map<std::string, std::string>    m_map;
            Threading::Mutex                      m_mutex;  

            std::string      getMac(std::string ip);
    };

}

#endif // _SECURITY_MAC_ADDRESS_TABLE_H
