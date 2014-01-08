/*
 * This file is part of fuppes
 *
 * (c) 2007-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#ifndef _XMSMEDIARECEIVERREGISTRAR_H
#define _XMSMEDIARECEIVERREGISTRAR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../UPnP/UPnPService.h"

class XMSMediaReceiverRegistrar: public UPnPService
{
    public:
        XMSMediaReceiverRegistrar();
        ~XMSMediaReceiverRegistrar();
		
		std::string getServiceDescription();
        Fuppes::HttpResponse* handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request);

};

#endif // _XMSMEDIARECEIVERREGISTRAR_H
