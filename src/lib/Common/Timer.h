/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            Timer.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#ifndef _TIMER_H
#define _TIMER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/Thread.h"

namespace fuppes {

class ITimer
{
  public:
    virtual void OnTimer() = 0;
    virtual ~ITimer() {}; 
};

class Timer: public Threading::Thread
{
  public:
    Timer(ITimer* p_OnTimerHandler);
    ~Timer();

    void SetInterval(unsigned int p_nSeconds);
    unsigned int GetInterval() { return m_nInterval; }
    void reset();
 		unsigned int GetCount();
		
		void run();
    
  private:
    unsigned int m_nTickCount;    
		void				 incTicCount();
		
		Threading::Mutex					m_mutex;        
    ITimer*       m_pOnTimerHandler;
    unsigned int  m_nInterval;
};

}

#endif // _TIMER_H
