/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Database/SearchCriteria.h>
using namespace Database;

class GetSearchCapabilitiesTest: public Test::BaseTest
{
    public:
        GetSearchCapabilitiesTest() :
                BaseTest("getSearchCapabilities")
        {
        }

        void run() throw (Test::Exception)
        {
            std::cout << SearchCriteria::getSearchCapabilities() << std::endl;
        }

};

class SearchCriteriaTest: public Test::BaseTest
{
    public:
        SearchCriteriaTest() :
                BaseTest("search criteria test")
        {
        }

        void run() throw (Test::Exception)
        {
            std::string sql;
            SearchCriteria sc;
            sc.parse(this->getCriteria(), sql);
            std::cout << "===" << std::endl << this->getCriteria() << std::endl << "===" << std::endl << sql << std::endl << "===" << std::endl;
        }

    protected:
        virtual std::string getCriteria() = 0;
};

class SearchCriteriaRelOpTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "dc:title = \"test \\\"dahhummm\\\" [xyz] @(§$)%&a.b.c title\" and dc:title != \"palim\" and upnp:originalTrackNumber != \"3\" and (upnp:originalTrackNumber < \"10\" and upnp:originalTrackNumber <= \"9\") or (upnp:originalTrackNumber > \"12\" and upnp:originalTrackNumber >= \"13\")";
        }

};

class SearchCriteriaExistsOpTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "@refID exists true and upnp:genre exists false or (@refID exists false and (@parentID exists true))";
        }
};

class SearchCriteriaStringOpTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "dc:title startsWith \"Bohemian\" and upnp:artist contains \"Queen\" and upnp:album doesNotContain \"live\"";
        }
};

class SearchCriteriaUpnpClassTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "upnp:class = \"object.item.imageItem.photo\" or upnp:class derivedFrom \"object.item.imageItem\"";
        }
};


class SearchCriteriaJunkTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "%s";
        }
};

class SearchCriteriaAudioItemTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "(upnp:class derivedfrom \"object.item.audioItem\") and ((dc:title contains \"eyes\"))";
        }
};


class SearchCriteriaMusicArtistTest: public SearchCriteriaTest
{
    protected:
        std::string getCriteria()
        {
            return "(upnp:class derivedfrom \"object.container.person.musicArtist\") and ((dc:title contains \"peter\"))";
        }
};





int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_DATABASE);

    app.m_tests.push_back(new GetSearchCapabilitiesTest());
    app.m_tests.push_back(new SearchCriteriaRelOpTest());
    app.m_tests.push_back(new SearchCriteriaExistsOpTest());
    app.m_tests.push_back(new SearchCriteriaStringOpTest());
    app.m_tests.push_back(new SearchCriteriaUpnpClassTest());

    app.m_tests.push_back(new SearchCriteriaJunkTest());
    app.m_tests.push_back(new SearchCriteriaAudioItemTest());
    app.m_tests.push_back(new SearchCriteriaMusicArtistTest());


    return app.run();
}

