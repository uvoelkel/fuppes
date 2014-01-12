/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TRANSCODING_TRANSCODER_H
#define _TRANSCODING_TRANSCODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Plugin/Manager.h"
#include "Item.h"
#include "Cache.h"
#include "Configuration.h"

namespace Transcoding
{
	class Transcoder
	{
		public:
			Transcoder(Plugin::Manager& pluginManager, Transcoding::Configuration& configuration);

			bool initializeTranscoding(Transcoding::Item& item);
			bool executeTranscoding(Transcoding::Item& item) const;
			void release(Transcoding::Item& item);

		private:

			bool extractImageFromAudio(Transcoding::Item& item) const;
			bool convertImage(Transcoding::Item& item) const;
			bool scaleImage(Transcoding::Item& item) const;

			Plugin::Manager& m_pluginManager;
			std::string m_tempDir;
			Cache m_cache;
	};
}

#endif // _TRANSCODING_TRANSCODER_H
