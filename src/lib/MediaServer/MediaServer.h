/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */
 
#ifndef _MEDIASERVER_H
#define _MEDIASERVER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../UPnP/UPnPDevice.h"

class MediaServer: public CUPnPDevice
{
  public:
    MediaServer(const std::string uuid, IUPnPDevice* onTimerHandler);           
    
};

#endif // _MEDIASERVER_H
