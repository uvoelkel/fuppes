/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TRANSCODING_TRANSCODEREXTERNALPROCESS_H
#define _TRANSCODING_TRANSCODEREXTERNALPROCESS_H

#include "../Plugin/AbstractTranscoder.h"
#include "../Thread/Process.h"

namespace Transcoding
{
	class TranscoderExternalProcess: public Plugin::AbstractTranscoder
	{
		public:
			TranscoderExternalProcess();
			~TranscoderExternalProcess();

			bool setup(::Transcoding::AbstractItem& item);
			int transcode();

			bool canPause();

			void teardown();
		private:

			std::string m_cmd;
			std::list<std::string> m_args;

			Thread::Process	m_process;
	};

}
#endif // _TRANSCODING_TRANSCODEREXTERNALPROCESS_H
