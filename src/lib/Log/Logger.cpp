/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Logger.h"

using namespace Log;

Logger::Logger(Threading::threadId_t threadId, const std::string sender, const std::string file, const int line)
{
	m_threadId = threadId;
	m_sender = sender;
	m_file = file;
	m_line = line;
}

Logger::~Logger()
{
}

void Logger::flush()
{
	m_stream.seekp(0, std::ios_base::end);
	if (0 == m_stream.tellp()) {
		return;
	}

    normalizedLog(m_stream.str());
}
