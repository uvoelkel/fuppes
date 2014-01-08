/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_QUERYRESULT_H
#define _DATABASE_QUERYRESULT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>
#include <map>
#include <vector>

namespace Database
{
    class ResultRow: public AbstractResultRow
    {
        public:
            fuppes_int64_t  getColumnInt64(const std::string column);
            std::string     getColumnString(const std::string column);

        private:
            void setColumnNull(const std::string column);
            void setColumnText(const std::string column, const std::string value);
            void setColumnInt64(const std::string column, const fuppes_int64_t value);

            std::map<std::string, std::string> m_textColumns;
            std::map<std::string, fuppes_int64_t> m_int64Columns;
    };

    class QueryResult: public AbstractQueryResult
    {
        public:
            size_t  size();
            void    clear();
            Database::ResultRow*    getRow(size_t index);

        private:
            AbstractResultRow* addRow();
            std::vector<Database::ResultRow>  m_rows;
    };
}

#endif // _DATABASE_QUERYRESULT_H
