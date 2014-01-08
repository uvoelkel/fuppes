/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            HotPlug.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#include "HotPlug.h"

//using namespace fuppes;

#include <stdio.h>
#include <iostream>
using namespace std;

HotPlugMgr* HotPlugMgr::m_instance = 0;

void HotPlugMgr::init() // static
{
  if(m_instance == 0)
    m_instance = new HotPlugMgr();
}

void HotPlugMgr::uninit() // static
{
  if(m_instance == 0)
    return;
  
  delete m_instance;
  m_instance = 0;
}

HotPlugMgr::HotPlugMgr()
{
  m_hotPlug = 0;

#ifdef HAVE_DBUS
  m_hotPlug = new HotPlugDbus();
  m_hotPlug->setup();
#endif
}

HotPlugMgr::~HotPlugMgr()
{
  if(m_hotPlug)
    delete m_hotPlug;
}




#ifdef HAVE_DBUS

bool HotPlugDbus::setup()
{
  return false;
  log(Log::hotplug, Log::normal) << "creating new dbus connection";

  DBusError error;
  dbus_error_init(&error);

  // connect to the bus
  m_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
  if(m_connection == NULL) {
    error(Log::hotplug, Log::normal) << "error creating dbus connection";
    if(dbus_error_is_set(&error)) {
      error(Log::hotplug, Log::normal) << "dbus error:" << error.message;
    }
    dbus_error_free(&error);
    return false;
  }  
  
  //dbus_connection_set_exit_on_disconnect(m_connection, FALSE);


  // setup message filter
  dbus_bus_add_match(m_connection, 
         "type='signal',interface='org.freedesktop.UDisks'", 
         &error); // see signals from the given interface
   dbus_connection_flush(m_connection);
   if (dbus_error_is_set(&error)) { 
     error(Log::hotplug, Log::normal) << "dbus error:" << error.message;
     dbus_error_free(&error);
     return false; 
   }

  // start event loop
  start();
  return true;
}

HotPlugDbus::HotPlugDbus()
{
  m_connection = NULL;
}

HotPlugDbus::~HotPlugDbus()
{
  close();
  
  if(m_connection != NULL)
    dbus_connection_unref(m_connection);

  /*DBusError error;
  dbus_error_init(&error);
  
  dbus_error_free(&error);*/
}

void printArguments(DBusMessageIter* args)
{
  cout << "printArguments" << endl;
  
  int type;
  const char* str;
  
  do {
    type = dbus_message_iter_get_arg_type(args);

    switch(type) {
    
      case DBUS_TYPE_STRING:
        dbus_message_iter_get_basic(args, &str);
        cout << "string : " << str << endl;
        break;
      case DBUS_TYPE_OBJECT_PATH:
        dbus_message_iter_get_basic(args, &str);
        cout << "object path : " << str << endl;
        break;
      case DBUS_TYPE_VARIANT: {      
          DBusMessageIter iter;
				  dbus_message_iter_recurse(args, &iter);
				  cout << "variant" << endl;
				  printArguments(&iter);
        }
        break;
  		case DBUS_TYPE_ARRAY: {
          cout << "array" << endl;
					int type;
					DBusMessageIter iter;
					dbus_message_iter_recurse(args, &iter);
					while ((type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID) {
						printArguments(&iter);
						dbus_message_iter_next(&iter);
					}
        }
        break;
        
      default:
        cout << "unhandled type: " << type << endl;
        break;
    }
    
  } while(dbus_message_iter_next(args) == TRUE);
}

void HotPlugDbus::run()
{
  DBusMessage* msg;
  DBusMessageIter args;
  
  while(!stopRequested()) {

    // read next message
    dbus_connection_read_write(m_connection, 0);
    msg = dbus_connection_pop_message(m_connection);

    if (NULL == msg) {
       msleep(500);
       continue;
    }

    cout << "DBUS got message" << endl;
    cout << "interface: " << dbus_message_get_interface(msg) << endl;
    cout << "sender: " << dbus_message_get_sender(msg) << endl;
    cout << "member: " << dbus_message_get_member(msg) << endl;

    if (!dbus_message_iter_init(msg, &args)) {
      cout << "Message Has No Parameters" << endl;
    }
    else {
      printArguments(&args);
    }
    
    // if (dbus_message_is_signal(msg, "org.freedesktop.UDisks", "DeviceAdded"))
    
    dbus_message_unref(msg);


    //queryObject("/org/freedesktop/UDisks/devices/sdb");
  }
  
}

void HotPlugDbus::queryObject(std::string object)
{
  DBusMessage* msg;
  DBusMessageIter args;
  DBusPendingCall* pending;

  msg = dbus_message_new_method_call("org.freedesktop.UDisks", // target for the method call
                                    object.c_str(), // object to call on
                                    "org.freedesktop.DBus.Properties", // interface to call on
                                    "Get"); // method name
  if (NULL == msg) { 
    fprintf(stderr, "Message Null\n");
    return;
  }

  // append arguments
  char* interface = "org.freedesktop.UDisks.Device";
  char* param = "DeviceMountPaths";
  dbus_message_iter_init_append(msg, &args);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param);

   
   // send message and get a handle for a reply
   if (!dbus_connection_send_with_reply(m_connection, msg, &pending, -1)) { // -1 is default timeout
      fprintf(stderr, "Out Of Memory!\n"); 
      return;
   }
   if (NULL == pending) { 
      fprintf(stderr, "Pending Call Null\n"); 
      return;
   }
   dbus_connection_flush(m_connection);
   
   printf("Request Sent\n");
   
   // free message
   dbus_message_unref(msg);
   
   // block until we recieve a reply
   dbus_pending_call_block(pending);

  cout << "got reply"  << endl;
  
   // get the reply message
   msg = dbus_pending_call_steal_reply(pending);
   if (NULL == msg) {
      fprintf(stderr, "Reply Null\n"); 
      return; 
   }
   // free the pending message handle
   dbus_pending_call_unref(pending);

  
  // read the parameters
  if (!dbus_message_iter_init(msg, &args))
    fprintf(stderr, "Message has no arguments!\n"); 
  else
    printArguments(&args);   
   
   // free reply 
   dbus_message_unref(msg);   
}
#endif // HAVE_DBUS
