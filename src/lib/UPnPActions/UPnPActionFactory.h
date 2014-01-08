/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPActionFactory.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
#ifndef _UPNPACTIONFACTORY_H
#define _UPNPACTIONFACTORY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include "UPnPAction.h"
#include "UPnPBrowse.h"
#include "UPnPSearch.h"
#include "../DeviceIdentification/DeviceSettings.h"

class CUPnPActionFactory
{

  public:

   /** builds an UPnP action from a string
    *  @param  p_sContent  the string to build th message from
    *  @return returns the action object on success otherwise NULL
    */
		static CUPnPAction* buildActionFromString(std::string p_sContent, CDeviceSettings* pDeviceSettings, std::string vfolderLayout, bool vfoldersEnabled);

  private:
          
    static bool parseBrowseAction(CUPnPBrowse* pAction, xmlNode* actionNode);		
		static bool parseSearchAction(CUPnPSearch* pAction, xmlNode* actionNode);
		static bool parseSortCriteria(std::string sort, CUPnPBrowseSearchBase* action);
		
		/*static bool parseDestroyObjectAction(CUPnPAction* action);
    static bool parseCreateReferenceAction(CUPnPAction* action, xmlNode* actionNode);*/
};

#endif // _UPNPACTIONFACTORY_H
