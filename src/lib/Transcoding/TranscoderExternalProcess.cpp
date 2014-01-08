/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "TranscoderExternalProcess.h"

using namespace Transcoding;

TranscoderExternalProcess::TranscoderExternalProcess()
{

}

TranscoderExternalProcess::~TranscoderExternalProcess()
{

}

bool TranscoderExternalProcess::setup(::Transcoding::AbstractItem& item)
{
	m_args.empty();

	if (!Thread::Process::splitCommand(item.getTranscoderCommand(), m_cmd, m_args)) {
		return false;
	}

	std::list<std::string>::iterator iter;
	for (iter = m_args.begin(); iter != m_args.end(); iter++) {

		if (0 == iter->compare("%in%") || 0 == iter->compare("%src%")) {
			*iter = item.getSourcePath();
		}
		else if (0 == iter->compare("%out%") || 0 == iter->compare("%dst%")) {
			*iter = item.getTargetPath();
		}
	}

	return true;
}

int TranscoderExternalProcess::transcode()
{
	if (Thread::Process::Idle == m_process.getState()) {
		if (!m_process.start(m_cmd, m_args)) {
			return -1;
		}
	}

	return (m_process.isRunning() ? 1 : 0);
}

bool TranscoderExternalProcess::canPause()
{
	return m_process.canPause();
}


void TranscoderExternalProcess::teardown()
{
	if (m_process.isRunning()) {
		m_process.terminate();
		m_process.waitFor();
	}
}
