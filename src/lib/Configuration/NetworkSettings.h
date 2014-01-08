/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_NETWORK_SETTINGS_H
#define _CONFIGURATION_NETWORK_SETTINGS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
#include <string>

#include "BaseSettings.h"

#include "../Security/Configuration.h"

namespace Configuration
{
    class NetworkHelper
    {
        public:
            static std::string getIpByHostname(const std::string hostname);
            static std::string getIpByInterface(const std::string iface);
    };


    class NetworkSettings: public BaseSettings
    {
        public:
            NetworkSettings();
            void readConfig(ConfigFile& configFile) throw(Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw(Configuration::SetupException);
            void writeConfig(ConfigFile& configFile) throw (Configuration::WriteException);

            std::string     hostname;
            std::string     networkInterface;
            std::string     ipAddress;
            unsigned int    httpPort;
            std::string     baseUrl;

            Security::Configuration   accessControlConfig;
    };
}

/*
class NetworkSettings : public ConfigSettings
{
  public:
    NetworkSettings();
    virtual ~NetworkSettings();
    
    virtual bool Read(void);

    //std::string GetHostname(void) { return m_sHostname; }
    std::string GetIPv4Address(void) { return m_sIP; }
    //std::string GetNetInterface(void) { return m_sNetInterface; }
    void SetNetInterface(std::string p_sNetInterface);
  
    //unsigned int GetHTTPPort() { return m_nHTTPPort; }
    bool SetHTTPPort(unsigned int p_nHTTPPort);
    

    //  allowed ip
    unsigned int AllowedIPCount() { return m_lAllowedIps.size(); }
    //bool IsAllowedIP(std::string p_sIPAddress);  
    bool AddAllowedIP(std::string p_sIPAddress);
    std::string GetAllowedIP(unsigned int p_nIdx) { return m_lAllowedIps[p_nIdx]; }
    bool RemoveAllowedIP(int p_nIndex);  

    unsigned int AllowedMacCount() { return m_lAllowedMacs.size(); }
    //bool IsAllowedMac(std::string mac);  
    //bool AddAllowedMac(std::string mac);
    std::string GetAllowedMac(unsigned int p_nIdx) { return m_lAllowedMacs[p_nIdx]; }
    //bool RemoveAllowedMac(int p_nIndex);  


        
  private:
    virtual void InitVariables(void) { }
    virtual bool InitPostRead(void);
    
    std::string   m_sHostname;
    std::string   m_sIP;
    std::string   m_sNetInterface;    
    unsigned int  m_nHTTPPort;

    std::vector<std::string>  m_lAllowedIps;
    std::vector<std::string>  m_lAllowedMacs;
  
    //bool ResolveHostAndIP();
   // bool ResolveIPByHostname();
    //bool ResolveIPByInterface(std::string p_sInterfaceName);
};
 */

#endif // _CONFIGURATION_NETWORK_SETTINGS_H
