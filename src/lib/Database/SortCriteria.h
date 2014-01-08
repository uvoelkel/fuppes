/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SORTCRITERIA_H_
#define _SORTCRITERIA_H_

#include <string>

#include <fuppes_database_plugin.h>
#include "../Database/Connection.h"

namespace Database
{

    class SortCriteria
    {
        public:
            SortCriteria();
            ~SortCriteria();

            bool parse(const std::string sortCriteria, std::string& sql);
            bool isValid();

            static const std::string getSortCapabilities();
            static const std::string getSortExtensionCapabilities();

        private:
            bool m_valid;
            Database::AbstractSortCriteria* m_sortCriteria;
            std::string                     m_originalValue;
            std::string                     m_sqlValue;

    };

}

#endif // _SORTCRITERIA_H_
