/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#include "MediaServer.h"

MediaServer::MediaServer(const std::string uuid, IUPnPDevice* onTimerHandler)
:CUPnPDevice(UPNP_DEVICE_MEDIA_SERVER, "", onTimerHandler)
{
  m_sUUID  			     = uuid;
  m_sPresentationURL = "index.html";      
}
