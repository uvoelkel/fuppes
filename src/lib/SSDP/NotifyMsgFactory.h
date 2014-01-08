/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#ifndef _SSDP_NOTIFYMSGFACTORY_H
#define _SSDP_NOTIFYMSGFACTORY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

namespace Ssdp
{

typedef enum {
  MESSAGE_TYPE_UNKNOWN                        =  0,
  MESSAGE_TYPE_USN                            =  1,
  MESSAGE_TYPE_ROOT_DEVICE                    =  2,
  MESSAGE_TYPE_CONNECTION_MANAGER             =  3,
  MESSAGE_TYPE_CONTENT_DIRECTORY              =  4,
  MESSAGE_TYPE_MEDIA_SERVER                   =  5,
  MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR  =  6
} MESSAGE_TYPE;

class NotifyMsgFactory
{
    public:		
        NotifyMsgFactory(std::string httpServerURL, std::string serverSignature, std::string uuid); 

		std::string msearch();	
	    std::string notify_alive(MESSAGE_TYPE);	
	    std::string notify_bye_bye(MESSAGE_TYPE);
        std::string GetMSearchResponse(MESSAGE_TYPE);		

    private:
	    std::string     m_httpServerURL;
        std::string     m_serverSignature;
        std::string     m_uuid;

        static std::string type_to_string(MESSAGE_TYPE, std::string uuid);
};

}

#endif // _SSDP_NOTIFYMSGFACTORY_H
