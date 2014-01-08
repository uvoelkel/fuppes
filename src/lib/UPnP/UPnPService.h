/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#ifndef _UPNP_UPNP_SERVICE_H
#define _UPNP_UPNP_SERVICE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "UPnPBase.h"
#include "../UPnPActions/UPnPAction.h"

#include "../Fuppes/HttpRequest.h"
#include "../Fuppes/HttpResponse.h"

class UPnPService: public CUPnPBase
{
    protected:

        /** constructor
         *  @param  nType  the device type
         *  @param  httpServerUrl  URL of the HTTP server
         */
        UPnPService(UPNP_DEVICE_TYPE nType, int version, std::string httpServerUrl):
        CUPnPBase(nType, version, httpServerUrl)
        {
        }

        /** destructor
         */
        virtual ~UPnPService() { };

    public:

        /** returns the service description as string
         *  @return  the service description
         */
        virtual std::string getServiceDescription() = 0;

        virtual Fuppes::HttpResponse* handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request) = 0;

};

#endif // _UPNP_UPNP_SERVICE_H
