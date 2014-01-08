/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_ABSTRACTTRANSCODER_H
#define _PLUGIN_ABSTRACTTRANSCODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_plugin.h>

namespace Plugin
{

	class AbstractTranscoder
	{

		public:
			virtual ~AbstractTranscoder() { }

			virtual bool setup(::Transcoding::AbstractItem& item) = 0;
			virtual int transcode() = 0;

			virtual bool canPause() = 0;

			virtual void teardown() = 0;
	};
}


#endif // _PLUGIN_ABSTRACTTRANSCODER_H
