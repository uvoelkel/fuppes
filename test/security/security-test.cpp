/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Security/AccessControl.h>
using namespace Security;


void check(AccessControl* accessControl, std::string ip, bool expected)
{
    bool allowed = accessControl->isAllowed(ip);
    std::cout << ip << ": " << (allowed ? "allowed" : "denied") << " " << (allowed == expected ? "[OK]" : "[ERROR]") << std::endl;
}

class SecurityTest: public Test::BaseTest
{
    public:
    SecurityTest() : BaseTest("test access by ip and mac") { }

        void run() throw (Test::Exception)
        {
            Security::Configuration config;
            config.hostIp = serverIp;

            AccessControl accessControl(config);

            accessControl.addIpAce("192.168.0.9");
            accessControl.addMacAce("00:1f:c6:e4:e8:8f");
            accessControl.addIpAce("192.168.0.[40-100]");
            accessControl.addIpAce("192.168.0.[20-30]");
            accessControl.addIpAce("192.*.0.[40-99]");

            check(&accessControl, serverIp, true);
            check(&accessControl, "192.168.0.9", true);
            check(&accessControl, "192.168.0.10", false);
            check(&accessControl, "192.168.0.100", true); //mac
            check(&accessControl, "192.168.0.101", false);

            check(&accessControl, "192.168.0.20", true);
            check(&accessControl, "192.168.0.21", true);
            check(&accessControl, "192.168.0.22", true);

            check(&accessControl, "192.168.0.30", true);
            check(&accessControl, "192.168.0.31", false);

            check(&accessControl, "192.168.0.40", true);
            check(&accessControl, "192.168.0.41", true);
            check(&accessControl, "192.168.0.42", true);

            check(&accessControl, "192.169.0.1", false);
            check(&accessControl, "192.169.0.40", true);
            check(&accessControl, "192.169.0.41", true);

            check(&accessControl, "192.168.0.50", true);
            check(&accessControl, "192.168.0.51", true);

        }
};


int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_NETWORK);

    app.m_tests.push_back(new SecurityTest());

    return app.run();
}

