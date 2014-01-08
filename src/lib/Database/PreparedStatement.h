/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_PREPAREDSTATEMENT_H
#define _DATABASE_PREPAREDSTATEMENT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>

#include "Connection.h"

namespace Database
{
    class PreparedStatement: public AbstractPreparedStatement
    {
        public:
            PreparedStatement();
            PreparedStatement(Database::Connection& connection);
            PreparedStatement(Database::Connection* connection);
            ~PreparedStatement();

            bool prepare(const std::string statement);
            bool clear();

            bool bindNull(int index);
            bool bindText(int index, std::string value);
            bool bindInt64(int index, fuppes_int64_t value);

            bool execute();
            bool select(Database::AbstractQueryResult& result);

            bool next();
            bool next(Database::AbstractItem& item);
            bool assign(Database::AbstractItem& item);
        private:
            AbstractPreparedStatement*  m_preparedStatement;

    };
}

#endif // _DATABASE_PREPAREDSTATEMENT_H
