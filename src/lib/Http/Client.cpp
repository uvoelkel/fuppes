/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Client.h"

using namespace Http;

Client::Client()
: Threading::EventLoopThread("http client")
{
	m_ioReadWatcher = NULL;
	m_ioWriteWatcher = NULL;
}

bool Client::connect(const std::string ip, const uint16_t port)
{
	m_socket.remoteAddress(ip);
	m_socket.remotePort(port);

	try {
		return m_socket.connect();
	}
	catch (fuppes::Exception &ex) {
		return false;
	}
}

bool Client::send(Http::Request& request)
{


	return false;
}

void Client::run()
{
	m_ioWriteWatcher = new Threading::IoWriteWatcher();
	m_ioWriteWatcher->init(this->getEventLoop()->getLoop(), m_socket.socket(), this);
	m_ioWriteWatcher->start();

	this->startEventLoop();
}

void Client::ioWriteEvent(Threading::IoWriteWatcher* watcher)
{

}

void Client::ioReadEvent(Threading::IoReadWatcher* watcher)
{

}
