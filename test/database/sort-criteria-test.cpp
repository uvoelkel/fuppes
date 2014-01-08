/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Database/SortCriteria.h>
using namespace Database;

class GetSortCapabilitiesTest: public Test::BaseTest
{
    public:
        GetSortCapabilitiesTest() :
                BaseTest("getSortCapabilities")
        {
        }

        void run() throw (Test::Exception)
        {
            std::cout << SortCriteria::getSortCapabilities() << std::endl;
        }
};

class GetSortExtensionCapabilitiesTest: public Test::BaseTest
{
    public:
        GetSortExtensionCapabilitiesTest() :
                BaseTest("getSortExtensionCapabilities")
        {
        }

        void run() throw (Test::Exception)
        {
            std::cout << SortCriteria::getSortExtensionCapabilities() << std::endl;
        }
};

class SortCriteriaSingleTest: public Test::BaseTest
{
    public:
        SortCriteriaSingleTest() :
                BaseTest("single sort criteria")
        {
        }

        void run() throw (Test::Exception)
        {
            SortCriteria sc;
            std::string criteria;
            std::string sql;

            criteria = "+dc:title";
            sql = "";

            sc.parse(criteria, sql);
            std::cout << sql << std::endl;
        }
};

class SortCriteriaMultipleTest: public Test::BaseTest
{
    public:
        SortCriteriaMultipleTest() :
                BaseTest("multiple sort criteria")
        {
        }

        void run() throw (Test::Exception)
        {
            SortCriteria sc;
            std::string criteria;
            std::string sql;

            criteria = "+dc:title,-upnp:artist";
            sql = "";

            sc.parse(criteria, sql);
            std::cout << sql << std::endl;
        }
};


class SortCriteriaMultipleTest2: public Test::BaseTest
{
    public:
        SortCriteriaMultipleTest2() :
                BaseTest("multiple sort criteria")
        {
        }

        void run() throw (Test::Exception)
        {
            SortCriteria sc;
            std::string criteria;
            std::string sql;

            criteria = "-upnp:originalTrackNumber,-dc:title,-upnp:artist,+upnp:genre,-upnp:album";
            sql = "";

            sc.parse(criteria, sql);
            std::cout << sql << std::endl;
        }
};

int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_DATABASE);

    app.m_tests.push_back(new GetSortCapabilitiesTest());
    app.m_tests.push_back(new GetSortExtensionCapabilitiesTest());

    app.m_tests.push_back(new SortCriteriaSingleTest());
    app.m_tests.push_back(new SortCriteriaMultipleTest());
    app.m_tests.push_back(new SortCriteriaMultipleTest2());

    return app.run();
}

