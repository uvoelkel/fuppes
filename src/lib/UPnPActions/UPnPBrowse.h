/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPBrowse.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
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
 
#ifndef _UPNPBROWSE_H
#define _UPNPBROWSE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "UPnPAction.h"

typedef enum UPNP_BROWSE_FLAG
{
  UPNP_BROWSE_FLAG_UNKNOWN,        
  UPNP_BROWSE_FLAG_DIRECT_CHILDREN,
  UPNP_BROWSE_FLAG_METADATA
}UPNP_BROWSE_FLAG;

class CUPnPBrowse: public CUPnPBrowseSearchBase
{
	friend class CUPnPActionFactory;
	
  public:
    CUPnPBrowse(std::string p_sMessage);
    ~CUPnPBrowse();

		std::string getQuery(bool count = false);
		UPNP_BROWSE_FLAG browseFlag() { return m_nBrowseFlag; }
		
  protected:
          upnp_error_code_t parse();

	private:
		bool	prepareSQL();
		
    UPNP_BROWSE_FLAG m_nBrowseFlag;
    bool             m_bVirtualContainer;
};

#endif // _UPNPBROWSE_H
