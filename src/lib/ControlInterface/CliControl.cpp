/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            ConfigInterface.cpp
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "CliControl.h"

#include <iostream>
#include <sstream>
using namespace std;

//CliControl* CliControl::m_instance = NULL;
/*
bool CliControl::handleAction(std::string action) // static
{

	if(!m_instance)
		m_instance = new CliControl();

	ControlAction* ctrlAction = getAction(action);
	if(ctrlAction && ctrlAction->exec_single) {

		ControlActionParams params;
		ControlActionParam resultParam;
		
		ControlInterface::ErrorCode error = (m_instance->*(ctrlAction->exec_single))(params, resultParam);
		if(error == ControlInterface::Ok) {
			std::cout << resultParam.name << ": " << resultParam.value << std::endl;
			return true;
		}
	}

	return false;
	
}
*/

