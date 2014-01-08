/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "MacAddressTable.h"
#include "../Common/File.h"
#include "../Common/RegEx.h"

#ifdef WIN32
#include <stdio.h>
#include <iphlpapi.h>
#endif

using namespace Security;

#include <iostream>
using namespace std;

#define MAC_UNKNOWN "00:00:00:00:00:00"

/*
MacAddressTable* MacAddressTable::m_instance = NULL;

void MacAddressTable::init() // static
{
    if(m_instance == NULL)
        m_instance = new MacAddressTable();
}

MacAddressTable* MacAddressTable::instance() // static
{
    init();
    return m_instance;
}

void MacAddressTable::uninit() // static
{
    if(m_instance == NULL)
        return;

    clear();
    delete m_instance;
}
*/


bool MacAddressTable::mac(std::string ip, std::string& mac)
{
    Threading::MutexLocker locker(&m_mutex);

    std::map<std::string, std::string>::iterator it;
    it = m_map.find(ip);
    if(it != m_map.end()) {
        mac = it->second;
        return true;
    }

    mac = getMac(ip);
    if(mac != MAC_UNKNOWN) {
        m_map[ip] = mac;
        return true;
    }
    return false;
}

void MacAddressTable::clear()
{
    Threading::MutexLocker locker(&m_mutex);
    m_map.clear();
}


std::string MacAddressTable::getMac(std::string ip)
{
#ifdef WIN32
  PMIB_IPNETTABLE pIpNetTable = NULL;
  ULONG dwSize = 0;
  DWORD ret;

  // get the required size and alloc the buffer
  ret = GetIpNetTable(NULL, &dwSize, false);
  pIpNetTable = (MIB_IPNETTABLE*)malloc(dwSize);
  // get the table
  ret = GetIpNetTable(pIpNetTable, &dwSize, false);

  // successfull but no dat
  if(ret == ERROR_NO_DATA) {
    free(pIpNetTable);
    return MAC_UNKNOWN;
  }

  // error
  if(ret != NO_ERROR) {
    std::cout << "MacAddressTable::getMac: error" << std::endl;
    if(ret == ERROR_INSUFFICIENT_BUFFER)
      std::cout << "ERROR_INSUFFICIENT_BUFFER" << std::endl;
    else if(ret == ERROR_INVALID_PARAMETER)
      std::cout << "ERROR_INVALID_PARAMETER" << std::endl;

    free(pIpNetTable);
    return MAC_UNKNOWN;
  }

  for(DWORD i = 0; i < pIpNetTable->dwNumEntries; i++) {

    std::string addr = inet_ntoa(*(struct in_addr *)&pIpNetTable->table[i].dwAddr);
    if(addr != ip || pIpNetTable->table[i].dwPhysAddrLen != 6)
      continue;

    char mac[18];
    memset(mac, 0, sizeof(mac));

    sprintf(mac,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
	    pIpNetTable->table[i].bPhysAddr[0],
		  pIpNetTable->table[i].bPhysAddr[1],
      pIpNetTable->table[i].bPhysAddr[2],
      pIpNetTable->table[i].bPhysAddr[3],
      pIpNetTable->table[i].bPhysAddr[4],
      pIpNetTable->table[i].bPhysAddr[5]);

    free(pIpNetTable);
    return mac;
  }

  free(pIpNetTable);
  return MAC_UNKNOWN;

#else

  fuppes::File file("/proc/net/arp");
  if(file.open(fuppes::File::Read)) {
    std::string line;
    RegEx rx("([\\d|\\.]+) +0x\\d +0x\\d +([\\d|\\w|:]+)");
    while(file.getline(line)) {

      if(!rx.search(line))
        continue;

      if(rx.match(1) == ip) {
        file.close();
        return rx.match(2);
      }
    }
    file.close();
  }

  return MAC_UNKNOWN;
#endif  
}

