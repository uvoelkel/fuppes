/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SEARCHCRITERIA_H_
#define _SEARCHCRITERIA_H_

#include <string>
#include <list>
#include <sstream>

#include <fuppes_database_plugin.h>
#include "../Database/Connection.h"

namespace Database
{

    class SearchCriteria
    {
        public:
            SearchCriteria();
            ~SearchCriteria();

            bool parse(const std::string searchCriteria, std::string& sql);

            static const std::string getSearchCapabilities();

            bool isValid();

        private:
            bool m_valid;
            Database::AbstractSearchCriteria* m_searchCriteria;

            std::string unescape(const std::string escapedQuote);
            std::string replaceUpnpClass(const std::string value, const bool derived);

    };

}

#endif // _SEARCHCRITERIA_H_
