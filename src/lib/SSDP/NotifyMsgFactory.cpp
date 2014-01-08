/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#include "NotifyMsgFactory.h"

#include <sstream>
#include <time.h>

using namespace std;
using namespace Ssdp;

NotifyMsgFactory::NotifyMsgFactory(std::string httpServerURL, std::string serverSignature, std::string uuid)
{
    m_httpServerURL     = httpServerURL;
    m_serverSignature   = serverSignature;
    m_uuid              = uuid;
}

std::string NotifyMsgFactory::msearch()
{
    stringstream result;

    result << "M-SEARCH * HTTP/1.1\r\n";
    result << "MX: 10\r\n";
    result << "ST: ssdp:all\r\n";
    result << "HOST: 239.255.255.250:1900\r\n"; // UPnP multicast address
    result << "MAN: \"ssdp:discover\"\r\n\r\n";
	
    return result.str();
}

std::string NotifyMsgFactory::notify_alive(MESSAGE_TYPE a_type)
{
    stringstream result;

    result << "NOTIFY * HTTP/1.1\r\n";
    result << "HOST: 239.255.255.250:1900\r\n";
    result << "CACHE-CONTROL: max-age=1800\r\n";
    result << "LOCATION: http://" << m_httpServerURL << "/description.xml\r\n";
    result << "NT: " << type_to_string(a_type, m_uuid) << "\r\n";
    result << "NTS: ssdp:alive\r\n";
    result << "SERVER: " << m_serverSignature << "\r\n";  

    result << "USN: uuid:" << m_uuid;
    if(a_type == MESSAGE_TYPE_USN) {
	    result << "\r\n";
    }
    else {
      result << "::" << type_to_string(a_type, m_uuid) << "\r\n";
    }
	
    result << "\r\n";

    return result.str();
}

std::string NotifyMsgFactory::notify_bye_bye(MESSAGE_TYPE a_type)
{
    stringstream result;

    result << "NOTIFY * HTTP/1.1\r\n";
    result << "HOST: 239.255.255.250:1900\r\n";
    result << "NTS: ssdp:byebye\r\n";

    result << "USN: uuid:" << m_uuid;
    if(a_type == MESSAGE_TYPE_USN) {
        result << "\r\n";
        result << "NT: " << m_uuid << "\r\n";
    }
    else {
        result << "::" << type_to_string(a_type, m_uuid) << "\r\n";	
        result << "NT: " << type_to_string(a_type, m_uuid) << "\r\n";
    }  
    result << "\r\n";      

    return result.str();	
}

std::string NotifyMsgFactory::GetMSearchResponse(MESSAGE_TYPE p_MessageType)
{	
    stringstream result;

    char   szTime[30];
    time_t tTime = time(NULL);
    strftime(szTime, 30,"%a, %d %b %Y %H:%M:%S GMT" , gmtime(&tTime));   

    result << "HTTP/1.1 200 OK\r\n";
    result << "CACHE-CONTROL: max-age=1800\r\n";
    result << "DATE: " << szTime << "\r\n";
    result << "EXT: \r\n";
    result << "LOCATION: http://" << m_httpServerURL << "/description.xml\r\n";
    result << "SERVER: " << m_serverSignature << "\r\n";  
    result << "ST: " << type_to_string(p_MessageType, m_uuid) << "\r\n";  
    result << "USN: uuid:" << m_uuid;
    if(p_MessageType == MESSAGE_TYPE_USN)
        result << "\r\n";
    else
        result << "::" << type_to_string(p_MessageType, m_uuid) << "\r\n";

    result << "Content-Length: 0\r\n";
    result << "\r\n";

    return result.str();
}

std::string NotifyMsgFactory::type_to_string(MESSAGE_TYPE a_type, std::string uuid) // static
{
	/* Convert message type to string */
    string result;

    switch(a_type)
    {
        case MESSAGE_TYPE_USN:
            result = "uuid:" + uuid;
            break;

        case MESSAGE_TYPE_ROOT_DEVICE:
            result = "upnp:rootdevice";
            break;

        case MESSAGE_TYPE_CONNECTION_MANAGER:
            result = "urn:schemas-upnp-org:service:ConnectionManager:1";
            break;

        case MESSAGE_TYPE_CONTENT_DIRECTORY:
            result = "urn:schemas-upnp-org:service:ContentDirectory:1";
            break;

        case MESSAGE_TYPE_MEDIA_SERVER:
            result = "urn:schemas-upnp-org:device:MediaServer:1";
            break;

        case MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR:
            result = "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1";
            break;

        default:	 
            break;
    }
	
    return result;
}
