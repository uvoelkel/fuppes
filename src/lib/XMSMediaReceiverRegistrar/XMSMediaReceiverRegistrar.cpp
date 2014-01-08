/*
 * This file is part of fuppes
 *
 * (c) 2007-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#include "XMSMediaReceiverRegistrar.h"
#include "XMSMediaReceiverRegistrarDescription.cpp"

XMSMediaReceiverRegistrar::XMSMediaReceiverRegistrar():
UPnPService(UPNP_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR, 1, "")
{
}

XMSMediaReceiverRegistrar::~XMSMediaReceiverRegistrar()
{
}

std::string XMSMediaReceiverRegistrar::getServiceDescription()
{
    return sXMSMediaReceiverRegistrarDescription;
}

Fuppes::HttpResponse* XMSMediaReceiverRegistrar::handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request)
{
    std::string sContent = "";

    if(upnpAction->GetActionType() == UPNP_IS_AUTHORIZED) {
	    sContent =
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <u:IsAuthorizedResponse xmlns:u=\"urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1\">"
            "      <Result>1</Result>"
            "    </u:IsAuthorizedResponse>"
            "  </s:Body>"
            "</s:Envelope>";
	}
  
	else if(upnpAction->GetActionType() == UPNP_IS_VALIDATED) {
	    sContent =
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <u:IsValidatedResponse xmlns:u=\"urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1\">"
            "      <Result>1</Result>"
            "    </u:IsValidatedResponse>"
            "  </s:Body>"
            "</s:Envelope>";
	}

	else if(upnpAction->GetActionType() == UPNP_REGISTER_DEVICE) {
	    sContent =
  		    "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <s:Fault>"
            "      <faultcode>s:Client</faultcode>"
            "      <faultstring>UPnPError</faultstring>"
            "      <detail>"
            "        <u:UPnPError xmlns:u=\"urn:schemas-upnp-org:control-1-0\">"
            "          <u:errorCode>501</u:errorCode>"
            "          <u:errorDescription>Action Failed</u:errorDescription>"
            "        </u:UPnPError>"
            "      </detail>"
            "    </s:Fault>"
            "  </s:Body>"
            "</s:Envelope>";
	}

    Fuppes::HttpResponse* response = NULL;
    if(!sContent.empty()) {    
        response = new Fuppes::HttpResponse(Http::OK, "text/xml; charset=\"utf-8\"", sContent);
        //response->getHeader()->setValue("Content-Type", "text/xml; charset=\"utf-8\"");
    }
    else {

        sContent = 
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>"  
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <s:Fault>"
            "      <faultcode>s:Client</faultcode>"
            "      <faultstring>UPnPError</faultstring>"
            "      <detail>"
            "        <UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
            "          <errorCode>401</errorCode>"
            "          <errorDescription>Invalid Action</errorDescription>"
            "        </UPnPError>"
            "      </detail>"
            "    </s:Fault>"
            "  </s:Body>"
            "</s:Envelope>";
    
        response = new Fuppes::HttpResponse(Http::INTERNAL_SERVER_ERROR, "text/xml; charset=\"utf-8\"", sContent);
        //response->getHeader()->setValue("Content-Type", "text/xml; charset=\"utf-8\"");
	}

    return response;
}
