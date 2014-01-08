/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "../BaseTest.h"


#include <iostream>

/*
#include <sstream>

#include <stdio.h>
using namespace std;
*/




class GetMp3Tail: public Fuppestest::BaseTestCurl
{
    public:
        GetMp3Tail() : BaseTestCurl("get mp3 tail") { }

        void run()
        {
            curl_easy_setopt(m_curl, CURLOPT_URL, this->getUrl("description.xml").c_str());

            CURLcode ret = curl_easy_perform(m_curl);
            std::cout << "CURL RET: " << ret << std::endl;
        }
};


int main(int argc, char* argv[])
{
    Fuppestest::TestApp app(argc, argv);
    
    app.m_tests.push_back(new GetMp3Tail());

    return app.run();
}


