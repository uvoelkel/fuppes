/*
 * This file is part of fuppes
 *
 * (c) 2008-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef PROCESS_H
#define PROCESS_H

#ifdef HAVE_CONFIG
#include <config.h>
#endif

//#include "Common.h"
//#include "Threading/Thread.h"

#include <map>
#include <list>
#include <string>
#include <signal.h>
#include <unistd.h>
#ifndef WIN32
#include <sys/wait.h>
#else
#include <windows.h>
#endif

namespace Thread
{
    class Process
    {
        public:
    		enum State {
				Idle = 0,
				Starting = 1,
				Running = 2,
				Paused = 3,
				Stopping = 4,
				Stopped = 5,
				Terminated = 6,
				Exited = 7,
				Error = 8
			};

            Process();
            ~Process();

            Thread::Process::State getState() { return m_state; }
            int exitCode() { return m_exitCode; }

            //bool	start(std::string cmd);
            bool start(std::string cmd, std::list<std::string> args);
            bool isRunning();
            bool canPause();
            bool pause();
            bool resume();
            void stop();
            void terminate();
            void waitFor();

            static bool splitCommand(std::string command, std::string& process, std::list<std::string>& args);

#ifndef WIN32
            pid_t pid()
            {
                return m_pid;
            }
#endif

        private:
            //void parseArgs(std::string cmd);

            Process::State	m_state;
            int	m_exitCode;
            char const** m_args;
            int m_numArgs;

#ifndef WIN32
            pid_t m_pid;
#else
            STARTUPINFO m_StartupInfo;
            PROCESS_INFORMATION m_ProcessInformation;
#endif

    };

}

#endif // PROCESS_H
