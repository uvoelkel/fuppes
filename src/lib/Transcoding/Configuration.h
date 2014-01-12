/*
 * This file is part of fuppes
 *
 * (c) 2014 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TRANSCODING_CONFIGURATION_H
#define _TRANSCODING_CONFIGURATION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

namespace Transcoding
{
	enum Target
	{
		Invalid = 0,
		Memory = 1,
		File = 2
	};

	class Configuration
	{
		public:
			std::string tempDir;

			Transcoding::Target audioTarget;
			Transcoding::Target imageTarget;
			Transcoding::Target videoTarget;
	};
}

#endif // _TRANSCODING_CONFIGURATION_H
