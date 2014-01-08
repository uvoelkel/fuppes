/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            Timer.cpp
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
 

#include "Timer.h"
#include "Common.h"

using namespace fuppes;

#include <string>
#include <iostream>
using namespace std;

Timer::Timer(ITimer* p_OnTimerHandler)
:Threading::Thread("Timer")
{
  m_pOnTimerHandler = p_OnTimerHandler;
  m_nTickCount      = 0;
}

Timer::~Timer()
{
  close();
}

void Timer::SetInterval(unsigned int p_nSeconds)
{
  m_nInterval = p_nSeconds;
}

void Timer::reset()
{
	m_mutex.lock();
  m_nTickCount = 0;
	m_mutex.unlock();
}

unsigned int Timer::GetCount()
{
  return (m_nInterval - (m_nTickCount / 10));
}

void Timer::incTicCount() {
  m_mutex.lock();
	m_nTickCount++;
	m_mutex.unlock();
}

void Timer::run() {	

	while(!this->stopRequested()&& ((this->m_nTickCount / 10) <= this->GetInterval())) {

		this->incTicCount();
    msleep(100); //fuppesSleep(100);
    
    if(!this->stopRequested() && ((this->m_nTickCount / 10) >= (this->GetInterval() - 1))) {
			if(m_pOnTimerHandler != NULL) {
		    m_pOnTimerHandler->OnTimer(); 
			}
			reset();
    }
  }

}
