/***************************************************************************
 *            ConnectionManager.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2006-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
 
#ifndef _CONNECTIONMANAGER_H
#define _CONNECTIONMANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../UPnP/UPnPService.h"
#include <string>

#include "../Fuppes/HttpRequest.h"
#include "../Fuppes/HttpResponse.h"


typedef enum CM_ERROR {
	ERR_NONE 						= 0,
	ERR_INVALID_ACTION	= 401,
	ERR_INVALID_ARGS		= 402,
	ERR_INVALID_VAR			= 404,
	ERR_ACTION_FAILED		= 501
} CM_ERROR;

class CConnectionManagerCore
{
	public:
		static void init();
    static void uninit();
	
	private:
		static CConnectionManagerCore* m_instance;
};

class CConnectionManager: public UPnPService
{
	public:
    CConnectionManager();
  
    std::string 	getServiceDescription();

	Fuppes::HttpResponse* handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request);

	private:
		CM_ERROR	getProtocolInfo(CUPnPAction* action, std::string& result);
		CM_ERROR	prepareForConnection(CUPnPAction* action, std::string* result);
		CM_ERROR	connectionComplete(CUPnPAction* action, std::string* result);
		CM_ERROR	getCurrentConnectionIds(CUPnPAction* action, std::string& result);
		CM_ERROR	getCurrentConnectionInfo(CUPnPAction* action, std::string& result);
};

#endif // _CONNECTIONMANAGER_H
