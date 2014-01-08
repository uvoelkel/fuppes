/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef WIN32
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#include "NetworkSettings.h"

#include "../Common/Common.h"
#include "../Common/RegEx.h"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN     256
#endif

std::string Configuration::NetworkHelper::getIpByHostname(const std::string hostname) // static
{
    std::string ipAddress;

    in_addr* addr;
    struct hostent* host;

    host = gethostbyname(hostname.c_str());
    if(NULL != host) {
      addr = (struct in_addr*)host->h_addr;
      ipAddress = inet_ntoa(*addr);
    }

    return ipAddress;
}

std::string Configuration::NetworkHelper::getIpByInterface(const std::string iface) // static
{
    std::string ipAddress;

    #ifdef WIN32
    return ipAddress;
    #else

    struct ifreq        ifa;
    struct sockaddr_in  *saddr;
    int                 fd;
    char                str[INET_ADDRSTRLEN + 1];

    strcpy (ifa.ifr_name, iface.c_str());
    if(((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) || ioctl(fd, SIOCGIFADDR, &ifa)) {
        return ipAddress;
    }
    saddr = (struct sockaddr_in*)&(ifa.ifr_addr);
    ipAddress = inet_ntop(AF_INET, &(saddr->sin_addr), str, INET_ADDRSTRLEN);
    close(fd);
    return ipAddress;

    #endif
}



Configuration::NetworkSettings::NetworkSettings()
{
    // ./configure --enable-default-http-port=PORT
    #ifdef DEFAULT_HTTP_PORT
    httpPort = DEFAULT_HTTP_PORT;
    #else
    httpPort = 0;
    #endif
}

void Configuration::NetworkSettings::readConfig(ConfigFile& configFile) throw (Configuration::ReadException)
{
    networkInterface = configFile.getStringValue("/fuppes_config/network/interface/text()");

    std::string port = configFile.getStringValue("/fuppes_config/network/http_port/text()");
    if(0 < port.length()) {
        httpPort = atoi(port.c_str());
    }

    EntryList aces = configFile.getEntries("/fuppes_config/network/allowed_ips/child::*");
    EntryListIterator aceIter;
    for (aceIter = aces.begin(); aceIter != aces.end(); aceIter++) {
        if(0 == aceIter->key.compare("ip")) {
            accessControlConfig.allowedIps.push_back(aceIter->value);
        }
        else if(0 == aceIter->key.compare("mac")) {
            accessControlConfig.allowedMacs.push_back(aceIter->value);
        }
    }


}

void Configuration::NetworkSettings::setup(PathFinder& /*pathFinder*/) throw (Configuration::SetupException)
{
    // get the hostname
    char name[MAXHOSTNAMELEN + 1];
    memset(name, 0, MAXHOSTNAMELEN + 1);
    if(0 != gethostname(name, MAXHOSTNAMELEN)) {
        throw Configuration::SetupException("can't resolve hostname");
    }
    hostname = name;

    // check the http port
    if(httpPort > 0 && httpPort <= 1024) {
        throw Configuration::SetupException("please set port to \"0\" or a number greater \"1024\"");
    }

    // check if the networkInterface is actually an ip
    RegEx rxIP("\\d+\\.\\d+\\.\\d+\\.\\d");
    if(rxIP.Search(networkInterface.c_str())) {
        ipAddress = networkInterface;
    }
    // otherwise try to get the ip from the interface name
    else if(!networkInterface.empty()) {
        ipAddress = NetworkHelper::getIpByInterface(networkInterface);
    }

    // still no ip. so let's try to get it by the hostname
    if(ipAddress.empty()) {
        ipAddress = NetworkHelper::getIpByHostname(hostname);
    }

    if(ipAddress.empty() || 0 == ipAddress.compare("127.0.0.1")) {
        throw Configuration::SetupException("no ip address detected");
    }

    accessControlConfig.hostIp = ipAddress;
}

void Configuration::NetworkSettings::writeConfig(ConfigFile& configFile) throw (Configuration::WriteException)
{
    xmlNodePtr node = configFile.getNode("/fuppes_config/network/interface");
    configFile.setNodeText(node, networkInterface);

    std::stringstream port;
    port << httpPort;
    node = configFile.getNode("/fuppes_config/network/http_port");
    configFile.setNodeText(node, port.str());
}
