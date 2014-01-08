/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_QUERYBUILDER_H
#define _SQLITE_QUERYBUILDER_H

#include <fuppes_database_plugin.h>

#include <sstream>

namespace Sqlite
{
    class QueryBuilder: public Database::AbstractQueryBuilder
    {
        public:
            QueryBuilder(Database::AbstractConnection& connection);

            Database::AbstractQueryBuilder& select(Database::Tables from);
            Database::AbstractQueryBuilder& select(std::string prefix, std::string field, Database::Tables from, Database::AbstractQueryBuilder::Operation op);

            Database::AbstractQueryBuilder& delete_(Database::Tables from);

            Database::AbstractQueryBuilder& where(std::string prefix, std::string field, std::string op, std::string value);
            Database::AbstractQueryBuilder& andWhere(std::string prefix, std::string field, std::string op, std::string value);
            Database::AbstractQueryBuilder& orWhere(std::string prefix, std::string field, std::string op, std::string value);

            Database::AbstractQueryBuilder& bracketOpen();
            Database::AbstractQueryBuilder& bracketClose();

            Database::AbstractQueryBuilder& orderBy(std::string fields);
            Database::AbstractQueryBuilder& limit(const int offset, const int count);
            Database::AbstractQueryBuilder& rawSql(std::string sql);

            bool getQuery(Database::AbstractPreparedStatement& preparedStatement);

        private:
            std::stringstream m_stream;
    };
}

#endif // _SQLITE_QUERYBUILDER_H
