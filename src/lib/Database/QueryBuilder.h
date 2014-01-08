/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_QUERYBUILDER_H
#define _DATABASE_QUERYBUILDER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>
#include "Connection.h"
#include "PreparedStatement.h"

namespace Database
{
    class QueryBuilder: public Database::AbstractQueryBuilder
    {
        public:
            QueryBuilder();
            QueryBuilder(Database::Connection* connection);
            QueryBuilder(Database::Connection& connection);
            ~QueryBuilder();

            Database::AbstractQueryBuilder& select(Database::Tables from);
            Database::AbstractQueryBuilder& select(std::string prefix, std::string field, Database::Tables from, Database::AbstractQueryBuilder::Operation op = Database::AbstractQueryBuilder::all);

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
            Database::AbstractQueryBuilder* m_queryBuilder;
    };
}

#endif // _DATABASE_QUERYBUILDER_H
