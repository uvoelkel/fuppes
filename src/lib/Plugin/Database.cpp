/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Database.h"

Plugin::Database::Database() :
        Plugin::Plugin()
{
    m_createConnection = NULL;
}

bool Plugin::Database::init(LibraryHandle handle)
{
    m_createConnection = (createConnection_t)Plugin::Plugin::getProcedure(handle, "fuppes_create_db_connection");
    return (NULL != m_createConnection);
}


::Database::AbstractConnection* Plugin::Database::createConnection()
{
    return m_createConnection(&m_data);
}
