/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "AccessControl.h"
using namespace Security;

#include "../Common/Common.h"
#include "MacAddressTable.h"
#include "../Common/RegEx.h"



#include <iostream>

/*
std::string AccessControl::m_hostIp = "";
AccessControl* AccessControl::m_instance = NULL;

void AccessControl::init() // static
{
    if(NULL != m_instance)
        return;
    m_instance = new AccessControl();

    MacAddressTable::init();
}

void AccessControl::uninit() // static
{
    delete m_instance;
    m_instance = NULL;

    MacAddressTable::uninit();
}
*/


AccessControl::AccessControl(Security::Configuration config)
{
    m_hostIp = config.hostIp;

    std::list<std::string>::iterator iter;

    for(iter = config.allowedIps.begin(); iter != config.allowedIps.end(); iter++) {
        this->addIpAce(*iter);
    }

    for(iter = config.allowedMacs.begin(); iter != config.allowedMacs.end(); iter++) {
		this->addMacAce(*iter);
    }
}

AccessControl::~AccessControl()
{
    std::map<std::string, AccessControlList*>::iterator iter;
    for(iter = m_acls.begin(); iter != m_acls.end(); iter++) {
       delete iter->second;
    }
    m_acls.clear();
}

bool AccessControl::isAllowed(std::string ip, int port /*= 0*/, std::string method /*= "*"*/, std::string uri /*= "*"*/)
{
    // if we got no acls everyone is allowed to access
    if(0 == m_acls.size())
        return true;
    
    // the host is always allowed to access
    if(ip == m_hostIp)
        return true;
    
    Threading::MutexLocker locker(&m_mutex);

    // find the acl for the uri
    std::map<std::string, AccessControlList*>::iterator iterUri;
    iterUri = m_acls.find(uri);
    if(m_acls.end() == iterUri) {
        
        if(uri.compare("*") != 0)
            iterUri = m_acls.find("*");

        if(m_acls.end() == iterUri)
            return false;
    }
    AccessControlList* acl = iterUri->second;


    // get the mac address for the ip
    std::string mac;
    m_macAddressTable.mac(ip, mac);

    // ip address chunks
    std::string ipChunks[4];
    bool splitted = false;
    
    // check the aces
    AccessControlEntry* ace;
    std::list<AccessControlEntry*>::iterator iter;
    for(iter = acl->aces.begin(); iter != acl->aces.end(); iter++) {
        ace = *iter;

        // check the port
        if(ace->port > 0 && ace->port != port)
            continue;

        // check the method
        if(ace->method.compare(method) != 0)
            continue;

        // check ip
        if(ace->ip.length() > 0) {

            // check for exact match
            if(ace->ip.compare(ip) == 0)
                return true;

            // check if the ace contains wildcards. if not continue
            if(ace->ip.find("*") == std::string::npos && ace->ip.find("[") == std::string::npos)
                continue;

            std::string pattern = "([0-9|\\[|\\]|\\*|-]+).([0-9|\\[|\\]|\\*|-]+).([0-9|\\[|\\]|\\*|-]+).([0-9|\\[|\\]|\\*|-]+)";
            std::string aceChunks[4];
            
            // split the ip
            if(!splitted) {
                RegEx rxIp(pattern.c_str());
		        if(rxIp.Search(ip.c_str())) {
                    ipChunks[0] = rxIp.Match(1);
                    ipChunks[1] = rxIp.Match(2);
                    ipChunks[2] = rxIp.Match(3);
                    ipChunks[3] = rxIp.Match(4);

                    //std::cout << "IP: " << ipChunks[0] << " . " << ipChunks[1] << " . " << ipChunks[2] << " . " << ipChunks[3] << std::endl;
                    splitted = true;
                }
                else {
                    return false;
                }
            }

            // split the ace ip
            RegEx rxIp(pattern.c_str());
            if(rxIp.Search(ace->ip.c_str())) {
                aceChunks[0] = rxIp.Match(1);
                aceChunks[1] = rxIp.Match(2);
                aceChunks[2] = rxIp.Match(3);
                aceChunks[3] = rxIp.Match(4);

                //std::cout << "ACE: " << aceChunks[0] << " . " << aceChunks[1] << " . " << aceChunks[2] << " . " << aceChunks[3] << std::endl;
            }
            else {
                //std::cout << "NOMATCH" << std::endl;
                continue;
            }

            // compare chunks
            bool match = true;
            for(int i = 0; i < 4; i++) {

                //std::cout << i << " " <<  ipChunks[i] << " == " << aceChunks[i] << std::endl;

                // * wildcard or exact match
                if(aceChunks[i].compare("*") == 0 || ipChunks[i].compare(aceChunks[i]) == 0) {
                    //std::cout << i << " exact or *" << std::endl;
                    continue;
                }

                // check for range
                if(aceChunks[i].find("[") == std::string::npos) {
                    //std::cout << i << " nomatch" << std::endl;
                    match = false;
                    break;
                }

                aceChunks[i] = StringReplace(aceChunks[i], "[", "");
                aceChunks[i] = StringReplace(aceChunks[i], "]", "");
                size_t pos = aceChunks[i].find("-");
                if(std::string::npos == pos)
                    return false;

                std::string from = aceChunks[i].substr(0, pos);
                std::string to = aceChunks[i].substr(pos + 1);

                //std::cout << "RANGE1: " << from << " - " << to << "!" << std::endl;

                if(from.compare("*") == 0)
                    from = "0";
                if(to.compare("*") == 0)
                    to = "255";

                //std::cout << "RANGE2: " << from << " - " << to << "!" << std::endl;
                
                int start = atoi(from.c_str());
                int end = atoi(to.c_str());

                int chunk = atoi(ipChunks[i].c_str());

                if(chunk < start || chunk > end) {
                    match = false;
                    break;
                }
                
                //std::cout << "RANGE3: " << start << " - " << end << "!" << std::endl;
            }

            if(match)
                return true;
            else
                continue;
        }

        // check mac
        if(ace->mac.length() > 0) {
            if(ace->mac.compare(mac) == 0)
                return true;
            else
                continue;
        }
        
    }

    return false;

/*

    if (AllowedIPCount() == 0) return true;           // if no allowed ip is set all addresses are allowed


  for(unsigned int i = 0; i < AllowedIPCount(); i++) { 
		string pattern = GetAllowedIP(i) + "$";
		pattern = StringReplace(pattern, ".*", ".[0-255]");  // 192.*.0.* => 192.[0-255].0.[0-255]
		pattern = StringReplace(pattern, "*", "255"); // 192.168.0.[10-*] => 192.168.0.[10-255]

		RegEx rxIp(pattern.c_str());
		if(rxIp.Search(p_sIPAddress.c_str()))
		  return true;
  }
  
  return false;  
    */

    
    return true;
}

void AccessControl::addIpAce(std::string ip, int port /*= 0*/, std::string method /*= "*"*/, std::string uri /*= "*"*/) // static
{
    std::map<std::string, AccessControlList*>::iterator iter;
    AccessControlList* acl;
    
    Threading::MutexLocker locker(&m_mutex);
    iter = m_acls.find(uri);
    if(m_acls.end() != iter) {
        acl = iter->second;
    }
    else {
        acl = new AccessControlList();
        m_acls[uri] = acl;
    }

    AccessControlEntry* ace = new AccessControlEntry();
    ace->ip = ip;
    ace->port = port;
    ace->method = method;

    acl->aces.push_back(ace);
    
}

void AccessControl::addMacAce(std::string mac, int port /*= 0*/, std::string method /*= "*"*/, std::string uri /*= "*"*/) // static
{
    std::map<std::string, AccessControlList*>::iterator iter;
    AccessControlList* acl;
    
    Threading::MutexLocker locker(&m_mutex);
    iter = m_acls.find(uri);
    if(m_acls.end() != iter) {
        acl = iter->second;
    }
    else {
        acl = new AccessControlList();
        m_acls[uri] = acl;
    }

    AccessControlEntry* ace = new AccessControlEntry();
    ace->mac = mac;
    ace->port = port;
    ace->method = method;

    acl->aces.push_back(ace);
}
