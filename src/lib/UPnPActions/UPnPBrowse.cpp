/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPBrowse.cpp
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
 
#include "UPnPBrowse.h"
#include "../Common/Common.h"

CUPnPBrowse::CUPnPBrowse(std::string p_sMessage):
  CUPnPBrowseSearchBase(UPNP_SERVICE_CONTENT_DIRECTORY, UPNP_BROWSE, p_sMessage)
{
}                                     

CUPnPBrowse::~CUPnPBrowse()
{
}

upnp_error_code_t CUPnPBrowse::parse()
{
    return upnp_200_ok;
}

std::string CUPnPBrowse::getQuery(bool count /*= false*/) {
	if(!prepareSQL()) {
		return "";
	}

	if(count) 
		return m_queryCount;
	else
		return m_query;
}

bool CUPnPBrowse::prepareSQL()
{
	if(m_query.length() > 0 && m_queryCount.length() > 0) {
		return true;
	}
	
	return false;
}
