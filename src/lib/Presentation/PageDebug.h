/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PageDebug.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _PAGEDBUG_H
#define _PAGEDBUG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "PresentationPage.h"

class PageDebug: public Presentation::PresentationPage
{
  public:
    std::string alias() { return "debug"; }
    std::string title() { return "debug"; }

    std::string content();
};


#endif // _PAGEDBUG_H
