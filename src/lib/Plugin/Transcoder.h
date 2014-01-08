/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_TRANSCODER_H
#define _PLUGIN_TRANSCODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Plugin.h"
#include "AbstractTranscoder.h"

namespace Plugin
{

	typedef int (*transcodeSetup_t)(::Transcoding::AbstractItem& item, void** data);

    class Transcoder: public Plugin, public AbstractTranscoder
    {
        public:
            Transcoder();
            Transcoder(Transcoder* plugin);

            bool init(LibraryHandle handle);

            bool setup(::Transcoding::AbstractItem& item);
            int transcode();

            bool canPause();

            void teardown();

            // set out endianess
            // decode interleaved
            // encode interleaved

        private:
            transcodeSetup_t		m_transcodeSetup;
    };
}

#endif // _PLUGIN_TRANSCODER_H
