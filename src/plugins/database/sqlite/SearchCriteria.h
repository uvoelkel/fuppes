/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_SEARCHCRITERIA_H
#define _SQLITE_SEARCHCRITERIA_H

#include <fuppes_database_plugin.h>

namespace Sqlite
{
    class SearchCriteria: public Database::AbstractSearchCriteria
    {
        public:
            const std::string getSearchCapabilities();

        protected:
            bool processExistsOp(const std::string property, const std::string value, std::string& result);
            bool processRelOp(const std::string property, const Database::SearchCriteriaRelOp op, const std::string value, std::string& result);
            bool processStringOp(const std::string property, const Database::SearchCriteriaStringOp op, const std::string value, std::string& result);


        private:

            enum FieldType {
                Text,
                Number
            };

            inline bool getField(const std::string property, std::string& field, SearchCriteria::FieldType& type);

    };
}

#endif // _SQLITE_SEARCHCRITERIA_H
