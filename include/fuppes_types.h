/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            fuppes_types.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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


#ifndef _FUPPES_TYPES_H
#define _FUPPES_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

#define __STDC_FORMAT_MACROS    1

#include <stdint.h>
#include <sys/types.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef SIZEOF_OFF_T
    typedef off_t fuppes_off_t;
#else
    "off_t unknown"
#endif

// off_t format specifier
#if SIZEOF_LONG_INT == 8
    #define PROff_t   "li"
#elif SIZEOF_LONG_LONG_INT == 8
    #define PROff_t   "lli"
#else
    "todo"
#endif


#ifdef HAVE_INTTYPES_H

    typedef int64_t     fuppes_int64_t;    
    
    typedef int64_t     object_id_t;
    
    // object id format specifier
    #define PRObjectId   PRIx64
#else
    //TODO
    
    //typedef long long int        fuppes_int64_t;
    
    //typedef long long int        object_id_t;
    "no inttypes"
#endif

#ifdef __cplusplus
}
#endif

#endif //_FUPPES_TYPES_H
