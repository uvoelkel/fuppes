/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_SORTCRITERIA_H
#define _SQLITE_SORTCRITERIA_H

#include <fuppes_database_plugin.h>

namespace Sqlite
{
    class SortCriteria: public Database::AbstractSortCriteria
    {
        public:
            const std::string getSortCapabilities();
            const std::string getSortExtensionCapabilities();

            bool processField(const std::string direction, const std::string property, std::string& result);
    };
}

#endif // _SQLITE_SORTCRITERIA_H
