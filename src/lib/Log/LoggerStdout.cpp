/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "LoggerStdout.h"

#include <iostream>

using namespace Log;

LoggerStdout::LoggerStdout(Threading::threadId_t threadId, const std::string sender, const std::string file, const int line) :
	Logger(threadId, sender, file, line)
{
}

void LoggerStdout::normalizedLog(const std::string log)
{
	std::cout << "[0x" << std::hex << m_threadId << std::dec << " :: " << m_sender << "] ";
	std::cout << m_file << " (" << m_line << ")" << std::endl;
    std::cout << log << std::endl;
}
