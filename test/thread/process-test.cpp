#include <BaseTest.h>

#include <Thread/Process.h>

static std::list<std::string> testArgs;


class ProcessTest: public Test::BaseTest
{
	protected:
		Thread::Process proc;

		ProcessTest(std::string name) : Test::BaseTest(name) { }

		void wait(int seconds)
		{
            for(int i = 0; i < seconds; i++) {
                msleep(1000);
                std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;
                if (!proc.isRunning()) {
                    break;
                }
            }
		}
};

class ExitingProcess: public ProcessTest
{
    public:
        ExitingProcess() : ProcessTest("run a process that exits on itself after a while") { }

        void run() throw (Test::Exception)
        {
            // create and start the process
            std::cout << "starting process" << std::endl;
            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;

            std::list<std::string> args = testArgs;
            args.push_front("exit");
            proc.start("dummy-process", args);

            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;
            std::cout << "process started. state: " << proc.getState() << std::endl;


            // wait for the process to exit
            while(proc.isRunning()) {
                std::cout << "waiting for process. state: " << proc.getState() << std::endl;
                this->wait(1);
            }

            std::cout << "process exited. code: " << proc.exitCode() << " state: " << proc.getState() << std::endl;
        }
};


class NotExitingProcessStop: public ProcessTest
{
    public:
        NotExitingProcessStop() : ProcessTest("run a process that does not exit on itself and stop it") { }

        void run() throw (Test::Exception)
        {
            // create and start the process
            std::cout << "starting process" << std::endl;
            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;

            std::list<std::string> args = testArgs;
            args.push_front("loop");
            proc.start("dummy-process", args);

            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;
            std::cout << "process started. state: " << proc.getState() << std::endl;

            // let the process run for a few seconds
            this->wait(3);

            // stop and wait for the process
            if(proc.isRunning()) {
                std::cout << "stopping process. state: " << proc.getState() << std::endl;
                proc.stop();
                std::cout << "waiting for process. state: " << proc.getState() << std::endl;
                proc.waitFor();
            }

            std::cout << "process exited. code: " << proc.exitCode() << " state: " << proc.getState() << std::endl;
        }
};


class NotExitingProcessTerminate: public ProcessTest
{
    public:
        NotExitingProcessTerminate() : ProcessTest("run a process that does not exit on itself and terminate it") { }

        void run() throw (Test::Exception)
        {
            // create and start the process
            std::cout << "starting process" << std::endl;
            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;


            std::list<std::string> args = testArgs;
            args.push_front("loop");
            proc.start("dummy-process", args);

            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;
            std::cout << "process started. state: " << proc.getState() << std::endl;


            // let the process run for a few seconds
            this->wait(3);

            // stop and wait for the process
            if(proc.isRunning()) {
                std::cout << "terminating process. state: " << proc.getState() << std::endl;
                proc.terminate();
                std::cout << "waiting for process. state: " << proc.getState() << std::endl;
                proc.waitFor();
            }

            std::cout << "process exited. code: " << proc.exitCode() << " state: " << proc.getState() << std::endl;
        }
};


class PauseableProcess: public ProcessTest
{
    public:
		PauseableProcess() : ProcessTest("run a process. pause for a while and resume") { }

        void run() throw (Test::Exception)
        {
            // create and start the process
            std::cout << "starting process" << std::endl;
            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;
            if (!proc.canPause()) {
            	std::cout << "process is not pauseable" << std::endl;
            	return;
            }


            std::list<std::string> args = testArgs;
            args.push_front("loop");
            proc.start("dummy-process", args);

            std::cout << "is running: " << (proc.isRunning() ? "yes" : "no") << std::endl;

            std::cout << "process started" << std::endl;


            // let the process run for a few seconds
            this->wait(3);


            // pause the process and wait a few seconds
            proc.pause();
            std::cout << "process paused" << std::endl;
            this->wait(3);


            // resume the process and wait a few seconds
            proc.resume();
            std::cout << "process resumed" << std::endl;
            this->wait(3);

            // stop and wait for the process
            if(proc.isRunning()) {
                std::cout << "stopping process" << std::endl;
                proc.stop();
                std::cout << "waiting for process" << std::endl;
                proc.waitFor();
            }

            std::cout << "process exited" << std::endl;
        }
};



class SplitCommandTest: public Test::BaseTest
{
	protected:
		SplitCommandTest() : Test::BaseTest("split single command string into argument list.") { }

		std::string input;

        void run() throw (Test::Exception)
        {
        	std::string cmd;
			std::list<std::string> args;

			if (!Thread::Process::splitCommand(input, cmd, args)) {
				throw Test::Exception("error splitting input: " + input);
			}

			std::cout << "INPUT: " << input << std::endl;
			std::cout << "COMMAND: " << cmd << std::endl;
			std::cout << "ARGS:";
			std::list<std::string>::iterator iter;
			for (iter = args.begin(); iter != args.end(); iter++) {
				std::cout << " " << *iter;
			}
			std::cout << std::endl;
        }
};


class SplitSimpleTest: public SplitCommandTest
{
	public:
		void setup() throw (Test::SetupException) {
			input = " dummy-process   test foo   bar   abc  -a   b     -c d  ";
		}
};

class SplitQuotedCmdTest: public SplitCommandTest
{
	public:
		void setup() throw (Test::SetupException) {
			input = " \"C:\\Program Files\\dummy-process.exe\" foo    bar   test";
		}
};

class SplitQuotedArgsTest: public SplitCommandTest
{
	public:
		void setup() throw (Test::SetupException) {
			input = " dummy-process   test \"C:\\Program Files\\\"   abc  -a   b     -c d";
		}
};


class SplitFailEmptyTest: public SplitCommandTest
{
	public:
		void setup() throw (Test::SetupException) {
			input = "   ";
		}
};


class SplitFailQuotedTest: public SplitCommandTest
{
	public:
		void setup() throw (Test::SetupException) {
			input = " dummy-process   test C:\\Program Files\\\"   abc  -a   b     -c d";
		}
};



int main(int argc, char* argv[])
{
    testArgs.push_back("test");
    testArgs.push_back("C:\\Program Files\\Palim Palim\\");
    testArgs.push_back("C:/Program Files/Test Dir/");
    testArgs.push_back("dingens kirchen");


    Test::App app(argc, argv);

    /*
    app.m_tests.push_back(new SplitSimpleTest());
    app.m_tests.push_back(new SplitQuotedCmdTest());
    app.m_tests.push_back(new SplitQuotedArgsTest());
    app.m_tests.push_back(new SplitFailEmptyTest());
    app.m_tests.push_back(new SplitFailQuotedTest());
    return app.run();
*/
    app.m_tests.push_back(new ExitingProcess());
    app.m_tests.push_back(new NotExitingProcessStop());
    app.m_tests.push_back(new NotExitingProcessTerminate());
    //app.m_tests.push_back(new PauseableProcess());

    return app.run();
}
