/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            ControlInterface.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010-2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _CONTROLINTERFACE_H
#define _CONTROLINTERFACE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <map>
#include <list>

#include "../Configuration/FuppesConfig.h"
#include "../ContentDatabase/ContentDatabase.h"

struct ControlAction;
struct ControlActionParam;

typedef std::list<ControlActionParam> ControlActionParams;
typedef std::list<ControlActionParam>::const_iterator ControlActionParamsIter;

namespace Fuppes
{
    class Core;
}

class ControlInterface
{
    public:
        enum ErrorCode
        {
        	// Success
            Ok = 0,
            RestartRequired = 1,
            Unchanged = 2,

            // Errors
            Error = 3,
            MissingInputParam = 4,
            MissingAttribute = 5,
            NotAllowed = 6
        };

        //static void init(CContentDatabase* contentDb);
        //static void uninit();

        ControlInterface(Fuppes::Core* fuppes, Fuppes::FuppesConfig& fuppesConfig, ContentDatabase::ContentDatabase& contentDatabase);
        virtual ~ControlInterface();

        //static FUPPES_CONTROL_ACTION getActionFromString(std::string action);
        ControlAction* getAction(std::string action);

    protected:

    private:
        Fuppes::Core* m_fuppes;
        Fuppes::FuppesConfig& m_fuppesConfig;
        ContentDatabase::ContentDatabase& m_contentDatabase;

        std::map<std::string, ControlAction*> m_actionMap;
        //static std::map<std::string, FUPPES_CONTROL_ACTION>   m_actionTypeMap;

        // database
        ControlInterface::ErrorCode execGetDatabaseStatus(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execDatabaseRebuild(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execDatabaseUpdate(const ControlActionParams params, ControlActionParam &result);
        //ControlInterface::ErrorCode execVfolderRebuild(const ControlActionParams params, ControlActionParam &result);

        ControlInterface::ErrorCode execGetDir(const ControlActionParams params, ControlActionParams &result);

        ControlInterface::ErrorCode execGetSharedObjects(const ControlActionParams params, ControlActionParams &result);
        ControlInterface::ErrorCode execGetSharedObjectTypes(const ControlActionParams params, ControlActionParams &result);
        ControlInterface::ErrorCode execAddSharedObject(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execDelSharedObject(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execModSharedObject(const ControlActionParams params, ControlActionParam &result);

        // system status and info
        ControlInterface::ErrorCode execGetVersion(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execGetHostname(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execGetUptime(const ControlActionParams params, ControlActionParam &result);


        // configuration
        ControlInterface::ErrorCode execGetConfigFileName(const ControlActionParams params, ControlActionParam &result);

        ControlInterface::ErrorCode execGetInterface(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execSetInterface(const ControlActionParams params, ControlActionParam &result);

        ControlInterface::ErrorCode execGetHttpPort(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execSetHttpPort(const ControlActionParams params, ControlActionParam &result);

        ControlInterface::ErrorCode execGetCharset(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execSetCharset(const ControlActionParams params, ControlActionParam &result);

        ControlInterface::ErrorCode execGetTempDir(const ControlActionParams params, ControlActionParam &result);
		ControlInterface::ErrorCode execSetTempDir(const ControlActionParams params, ControlActionParam &result);

        ControlInterface::ErrorCode execGetUuid(const ControlActionParams params, ControlActionParam &result);
		ControlInterface::ErrorCode execSetUuid(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execGetIsFixedUuid(const ControlActionParams params, ControlActionParam &result);
		ControlInterface::ErrorCode execSetIsFixedUuid(const ControlActionParams params, ControlActionParam &result);


        // virtual container
        ControlInterface::ErrorCode execGetVirtualContainers(const ControlActionParams params, ControlActionParam &result);


        // transcoding
        ControlInterface::ErrorCode execGetTranscodingCacheState(const ControlActionParams params, ControlActionParam &result);
        ControlInterface::ErrorCode execEmptyTranscodingCache(const ControlActionParams params, ControlActionParam &result);


        /*
         // get log level
         FUPPES_CTRL_GET_LOG_LEVEL,
         // set log level
         FUPPES_CTRL_SET_LOG_LEVEL,
         // add log subsystem
         FUPPES_CTRL_ADD_LOG_SUBSYSTEM,
         // delete log subsystem
         FUPPES_CTRL_DEL_LOG_SUBSYSTEM,
         */

};

struct ControlActionParam
{
        std::string name;
        std::string value;

        std::map<std::string, std::string> attributes;
        bool attribute(const std::string key, std::string &value) const
        {

            std::map<std::string, std::string>::const_iterator iter = attributes.begin();
            for (; iter != attributes.end(); iter++) {
                if (iter->first == key) {
                    value = iter->second;
                    return true;
                }
            }
            return false;
        }

        ControlActionParams children;
};

typedef ControlInterface::ErrorCode (ControlInterface::*execute_ptr_single)(const ControlActionParams params, ControlActionParam &result);
typedef ControlInterface::ErrorCode (ControlInterface::*execute_ptr_multi)(const ControlActionParams params, ControlActionParams &result);

struct ControlAction
{
        ControlAction(std::string a, execute_ptr_single e)
        {
            action = a;
            exec_single = e;
            exec_multi = NULL;
        }

        ControlAction(std::string a, execute_ptr_multi e)
        {
            action = a;
            exec_single = NULL;
            exec_multi = e;
        }

        /*
         ControlAction& operator=(const ControlAction& ctrlaction) {
         action = ctrlaction.action;
         exec_single = ctrlaction.exec_single;
         exec_multi = ctrlaction.exec_multi;
         return *this;
         }
         */

        //FUPPES_CONTROL_ACTION   action;
        std::string action;
        execute_ptr_single exec_single;
        execute_ptr_multi exec_multi;
};

#endif // _CONTROLINTERFACE_H
