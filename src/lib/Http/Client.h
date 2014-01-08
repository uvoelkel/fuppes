/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_HTTP_CLIENT_H
#define _HTTP_HTTP_CLIENT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/EventLoopThread.h"
#include "../Thread/IoWatcher.h"
#include "../Thread/IoEventReceiverInterface.h"

#include "Request.h"
#include "../Common/Socket.h"

namespace Http
{

	class Client: public Threading::EventLoopThread, public Threading::IoWriteEventReceiverInterface, public Threading::IoReadEventReceiverInterface
	{
		public:
			Client();

			bool connect(const std::string ip, const uint16_t port);
			bool send(Http::Request& request);

			void run();
			void ioWriteEvent(Threading::IoWriteWatcher* watcher);
			void ioReadEvent(Threading::IoReadWatcher* watcher);

		protected:
			fuppes::TCPSocket			m_socket;

			Threading::IoWriteWatcher*	m_ioWriteWatcher;
			Threading::IoReadWatcher*	m_ioReadWatcher;
	};

}


#endif // _HTTP_HTTP_CLIENT_H
