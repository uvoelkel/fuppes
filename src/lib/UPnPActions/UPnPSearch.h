/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPSearch.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _UPNPSEARCH_H
#define _UPNPSEARCH_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "UPnPAction.h"
#include "../Database/SearchCriteria.h"

class CUPnPSearch: public CUPnPBrowseSearchBase
{
        friend class CUPnPActionFactory;

    public:
        CUPnPSearch(std::string message);

        std::string getQuery(bool count = false);

        std::string m_searchCriteria;
        std::string m_searchCriteriaSql;


        Database::SearchCriteria  searchCriteria;

    protected:
        upnp_error_code_t parse();

    private:
        std::string m_sParentIds;


        bool prepareSQL();
};

#endif // _UPNPSEARCH_H
