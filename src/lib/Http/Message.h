/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_MESSAGE_H
#define _HTTP_MESSAGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "StatusCode.h"
#include "HttpHeader.h"

namespace Http
{

	class Message
	{
		protected:
			Message();

	};

}

#endif // _HTTP_MESSAGE_H
