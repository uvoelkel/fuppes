/*
 * This file is part of fuppes
 *
 * (c) 2008-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Process.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

using namespace Thread;

#define MAX_ARGS 31

Process::Process()
{
#ifndef WIN32
    m_pid = -1;
#endif
    m_args = NULL;
    m_numArgs = 0;
    m_state = Process::Idle;
    m_exitCode = -2;
}

Process::~Process()
{
    for(int i = 0; i < m_numArgs; i++) {
        free((void*)m_args[i]);
    }
    free(m_args);
}

bool Process::start(std::string cmd, std::list<std::string> args)
{
	m_state = Process::Starting;

#ifndef WIN32
    if(-1 != m_pid) {
    	m_state = Process::Error;
        return false;
    }

    m_pid = fork();

    // child process
    if(m_pid == 0) {

        int argc = 0;
        char* argv[MAX_ARGS + 1];

        string tmp;
        args.push_front(cmd);

        std::list<std::string>::iterator iter;
        for(iter = args.begin(); iter != args.end(); iter++) {

            tmp = *iter;
            argv[argc] = (char*)malloc(tmp.length() + 1 * sizeof(char*));
            strcpy((char*)argv[argc], tmp.c_str());
            argc++;
        }

        argv[argc] = (char*)malloc(sizeof(char*));
        argv[argc] = 0;

        /*for(int i = 0; i < argc; i++) {
            std::cout << "arg " << i << ": " << argv[i] << std::endl;
        }*/

        //close(STDOUT_FILENO);
        close(STDERR_FILENO);

        execv(argv[0], argv); // execvp
        exit(-1);
    }
    // parent process
    else if(m_pid > 0) {
        //m_isRunning = true;
        //ProcessMgr::register_proc(this);
        std::cout << "started process with PID: " << m_pid << std::endl;
        m_state = Process::Running;
        return true;
    }
    // fork() error
    else {
        //m_isRunning = false;
        cout << "fork() failed" << endl;
        m_state = Process::Error;
        return false;
    }

    return false;
#else

    LPCTSTR lpApplicationName = cmd.c_str();

    std::string tmp;
    std::string commandLine = cmd;
    std::list<std::string>::iterator iter;
    for(iter = args.begin(); iter != args.end(); iter++) {

        tmp = *iter;
        /*if(tmp.compare("%in%") == 0)
         tmp = m_inFile;
         else if(tmp.compare("%out%") == 0)
         tmp = m_outFile;*/

        //cmd += (" \"" + StringReplace(tmp, "\\", "/") + "\"");
    }

    std::cout << "STARTING: " << lpApplicationName << " ARGS: " << cmd << std::endl;

    LPTSTR lpCommandLine = strdup(cmd.c_str());

    LPCTSTR lpCurrentDirectory = NULL;

    ZeroMemory(&m_StartupInfo, sizeof(m_StartupInfo));
    m_StartupInfo.cb = sizeof(m_StartupInfo);

    ZeroMemory(&m_ProcessInformation, sizeof(m_ProcessInformation));

    m_StartupInfo.hStdError = GetStdHandle(STD_OUTPUT_HANDLE);
    m_StartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    m_StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

    BOOL ret = CreateProcess(
            NULL, //lpApplicationName,
            lpCommandLine,
            NULL,
            NULL,
            TRUE,
            0,//CREATE_NO_WINDOW | DETACHED_PROCESS, // CREATE_NEW_CONSOLE
            NULL,
            lpCurrentDirectory,
            &m_StartupInfo,
            &m_ProcessInformation);

    //std::cout << lpApplicationName << " " << lpCommandLine <<  " " << ret << std::endl;

    free(lpCommandLine);

    //m_isRunning = (ret != 0);

	m_state = (0 != ret) ? Process::Running : Process::Error;
    return (ret != 0);

#endif
}

bool Process::isRunning()
{

#ifndef WIN32

    if(0 >= m_pid) {
        return false;
    }

    int status = 0;
    pid_t pid = waitpid(m_pid, &status, WNOHANG);
    if(0 == pid) {
        return true;
    }
    else if(-1 == pid) {
    	m_state = Process::Error;
        return false;
    }


    if (WIFEXITED(status)) {
    	m_state = Process::Exited;
    	m_exitCode = WEXITSTATUS(status);
    	return false;
    }
    else if (WIFSIGNALED(status)) {

    	if (SIGTERM == WTERMSIG(status)) {
    		m_state = Process::Stopped;
    	}
    	else if (SIGKILL == WTERMSIG(status)) {
    		m_state = Process::Terminated;
    	}
    	else {
    		m_state = Process::Error;
    	}
		m_exitCode = 0;
		return false;
    }

    return !(WIFEXITED(status) || WIFSIGNALED(status));
#else

    DWORD ExitCode;
    BOOL ret = GetExitCodeProcess(m_ProcessInformation.hProcess, &ExitCode);

    // error
    if(ret == 0) {
        return false;
    }

    //std::cout << "Process::isRunning(): " << ret << " code: " << ExitCode << std::endl;

    return (ExitCode == STILL_ACTIVE);
#endif
}

bool Process::canPause()
{
#ifdef WIN32
	return false;
#else
	return true;
#endif
}

bool Process::pause()
{
    if(!isRunning()) {
        return false;
    }

#ifndef WIN32
    m_state = Process::Paused;
    kill(m_pid, SIGSTOP);
	return true;
#else
	return false;
#endif
}

bool Process::resume()
{
    if(!isRunning()) {
        return false;
    }

#ifndef WIN32
    m_state = Process::Running;
	kill(m_pid, SIGCONT);
	return true;
#else
	return false;
#endif
}


void Process::stop()
{
    if(!isRunning())
        return;

    m_state = Process::Stopping;
#ifndef WIN32
    kill(m_pid, SIGTERM);
#else
#endif
}

void Process::terminate()
{
    if(!isRunning()) {
        return;
    }

    m_state = Process::Terminated;
#ifndef WIN32
    kill(m_pid, SIGKILL);
#else  
    UINT ExitCode = 2;
    /*BOOL ret = */TerminateProcess(m_ProcessInformation.hProcess, ExitCode);

    //std::cout << "Process::terminate(): " << ret << " code: " << ExitCode << std::endl;
#endif
}

void Process::waitFor()
{
    if(!isRunning()) {
        return;
    }

#ifndef WIN32
    int status = 0;
    pid_t pid = waitpid(m_pid, &status, 0);

    if(pid == m_pid) {
        m_pid = -1;
    }

    if (WIFEXITED(status)) {
    	m_state = Process::Exited;
    	m_exitCode = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status)) {

    	if (SIGTERM == WTERMSIG(status)) {
    		m_state = Process::Stopped;
    	}
    	else if (SIGKILL == WTERMSIG(status)) {
    		m_state = Process::Terminated;
    	}
    	else {
    		m_state = Process::Error;
    	}
		m_exitCode = 0;
    }

    /*
     cout << "WAITFOR :: IS RUNNING: " << pid << " : " << m_pid << " STATUS: " << status << endl;

     cout << (WIFEXITED(status) ? "exited" : "not exited") << endl;
     cout << "exitstatus: " << WEXITSTATUS(status) << endl;
     cout << (WIFSIGNALED(status) ? "signaled" : "not signaled") << endl;
     */

    /*while(isRunning()) {
     sleep(100);
     }*/
#else
    WaitForSingleObject(m_ProcessInformation.hProcess, 1000);
#endif

}


bool Process::splitCommand(std::string command, std::string& process, std::list<std::string>& args) // static
{
	size_t spacePos = std::string::npos;
	size_t quotePos = std::string::npos;
	bool quoted = false;

	while (0 < command.length()) {

		spacePos = command.find_first_of(" ");
		quotePos = command.find_first_of("\"");

		if (std::string::npos == spacePos && std::string::npos == quotePos) {
			args.push_back(command);
			command = "";
		}
		else {

			if (spacePos < quotePos && !quoted) {
				if (0 < spacePos) {
					args.push_back(command.substr(0, spacePos));
				}
				command = command.substr(spacePos + 1);
			}
			else if (spacePos > quotePos || (spacePos < quotePos && quoted)) {

				if (std::string::npos == quotePos) {
					return false;
				}

				if (0 < quotePos) {
					args.push_back(command.substr(0, quotePos));
				}
				command = command.substr(quotePos + 1);
				quoted = !quoted;
			}

		}
	}

	if (0 == args.size()) {
		return false;
	}

	process = args.front();
	args.pop_front();
	return true;
}
