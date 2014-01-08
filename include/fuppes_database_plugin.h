/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_DATABASE_PLUGIN_H
#define _FUPPES_DATABASE_PLUGIN_H

#include <fuppes_types.h>
#include <fuppes_plugin.h>
#include <fuppes_database_objects.h>
//#include <stdarg.h>

namespace Database
{
    class ConnectionParameters
    {
        public:
            std::string type;
            std::string filename;
            std::string hostname;
            std::string username;
            std::string password;
            std::string dbname;
            bool readonly;
    };

    class AbstractTableManager;
    class AbstractQuery;
    class AbstractQueryBuilder;
    class AbstractPreparedStatement;
    class AbstractObjectManager;
    class AbstractSearchCriteria;
    class AbstractSortCriteria;

    class AbstractConnection
    {
        public:
            virtual ~AbstractConnection()
            {
            }

            virtual bool connect() = 0;
            virtual void close() = 0;

            virtual AbstractTableManager* createTableManager() = 0;
            //virtual AbstractQuery* createQuery() = 0;
            virtual AbstractQueryBuilder* createQueryBuilder() = 0;
            virtual AbstractPreparedStatement* createPreparedStatement() = 0;
            virtual AbstractObjectManager* createObjectManager() = 0;
            virtual AbstractSearchCriteria* createSearchCriteria() = 0;
            virtual AbstractSortCriteria* createSortCriteria() = 0;

            void setParameters(const Database::ConnectionParameters parameters)
            {
                m_parameters = parameters;
            }

            Database::ConnectionParameters getParameters()
            {
                return m_parameters;
            }

        protected:
            Database::ConnectionParameters m_parameters;
    };

    class AbstractTableManager
    {
        public:
            virtual ~AbstractTableManager()
            {
            }

            virtual bool exist() = 0;
            virtual bool isVersion(int version) = 0;
            virtual bool create(int version) = 0;
            virtual bool empty() = 0;

        protected:
            AbstractTableManager(Database::AbstractConnection& connection) :
                    m_connection(connection)
            {
            }

            Database::AbstractConnection& m_connection;
    };

    class AbstractResultRow
    {
        public:
            virtual ~AbstractResultRow()
            {
            }

            virtual void setColumnNull(const std::string column) = 0;
            virtual void setColumnText(const std::string column, const std::string value) = 0;
            virtual void setColumnInt64(const std::string column, const fuppes_int64_t value) = 0;
    };

    class AbstractQueryResult
    {
        public:
            virtual ~AbstractQueryResult()
            {
            }

            virtual AbstractResultRow* addRow() = 0;
    };

    class AbstractQuery
    {
        public:
            virtual ~AbstractQuery()
            {
            }

            /**
             * select
             */
            virtual bool select(const std::string query, Database::AbstractQueryResult& result) = 0;

            /**
             * create/update/delete
             */
            virtual bool execute(const std::string query) = 0;

            /**
             * insert
             *
             * @return  fuppes_off_t    0 on error. else the id of the inserted row
             */
            virtual fuppes_off_t insert(const std::string query) = 0;

        protected:
            AbstractQuery(Database::AbstractConnection& connection) :
                    m_connection(connection)
            {
            }

            Database::AbstractConnection& m_connection;
    };

    class AbstractPreparedStatement
    {
        public:
            virtual ~AbstractPreparedStatement()
            {
            }

            virtual bool prepare(const std::string statement) = 0;
            virtual bool clear() = 0;

            virtual bool bindNull(int index) = 0;
            virtual bool bindText(int index, std::string value) = 0;
            virtual bool bindInt64(int index, fuppes_int64_t value) = 0;

            virtual bool execute() = 0;
            virtual bool select(Database::AbstractQueryResult& result) = 0;

            virtual bool next() = 0;
            virtual bool next(Database::AbstractItem& item) = 0;
            virtual bool assign(Database::AbstractItem& item) = 0;

        protected:
            AbstractPreparedStatement(Database::AbstractConnection& connection) :
                    m_connection(connection)
            {
            }

            Database::AbstractConnection& m_connection;
    };

    enum Tables
    {
        Objects,
        Details,
        ObjectDetails
    };

    class AbstractQueryBuilder
    {
        public:

            enum Operation
            {
                all,
                count,
                distinct,
                max
            };

            virtual ~AbstractQueryBuilder()
            {
            }

            virtual AbstractQueryBuilder& select(Database::Tables from) = 0;
            virtual AbstractQueryBuilder& select(std::string prefix, std::string field, Database::Tables from, Database::AbstractQueryBuilder::Operation op = AbstractQueryBuilder::all) = 0;

            virtual AbstractQueryBuilder& delete_(Database::Tables from) = 0;

            virtual AbstractQueryBuilder& where(std::string prefix, std::string field, std::string op, std::string value) = 0;
            virtual AbstractQueryBuilder& andWhere(std::string prefix, std::string field, std::string op, std::string value) = 0;
            virtual AbstractQueryBuilder& orWhere(std::string prefix, std::string field, std::string op, std::string value) = 0;

            virtual AbstractQueryBuilder& bracketOpen() = 0;
            virtual AbstractQueryBuilder& bracketClose() = 0;

            virtual AbstractQueryBuilder& orderBy(std::string fields) = 0;
            virtual AbstractQueryBuilder& limit(const int offset, const int count) = 0;
            virtual AbstractQueryBuilder& rawSql(std::string sql) = 0;

            virtual bool getQuery(AbstractPreparedStatement& preparedStatement) = 0;

        protected:
            AbstractQueryBuilder(Database::AbstractConnection& connection) :
                    m_connection(connection)
            {
            }

            Database::AbstractConnection& m_connection;
    };

    class AbstractObjectManager
    {
        public:

            /*
            enum FindWhat
            {
                // find all updatable items (UPDATED_AT is NULL or UPDATED_AT < MODIFIED_AT) and DEVICE is NULL and REF_ID = 0
                //UpdatableItems,                 // no params
                //
                //AlbumArtImages,                 // albumArt filenames
                //AudioItems,                     // parentId
                //VideoItemsWithoutThumbnails,    // no params
                //VideoThumbnail,                 // path, filename (without extension)
                //UpdatablePlaylists,             // no params

            };
            */

            virtual ~AbstractObjectManager()
            {
            }

            virtual bool getMaxObjectId(object_id_t& result) = 0;
            virtual bool getAllContainerPaths(Database::AbstractQueryResult& result) = 0;

            virtual bool findOne(const object_id_t objectId, const std::string device, Database::AbstractItem& item, bool withDetails) = 0;
            virtual bool findOneByFilename(const std::string path, const std::string filename, Database::AbstractItem& item, bool withDetails) = 0;

            virtual bool countChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, Database::AbstractQueryResult& result) = 0;
            virtual bool findChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, const std::string orderBy, const int offset, const int limit, Database::AbstractItem& item) = 0;
            virtual bool nextChild(Database::AbstractItem& item) = 0;

            //virtual bool findItems(const Database::AbstractObjectManager::FindWhat what, Database::AbstractItem& item, va_list args) = 0;
            //virtual bool findNext(Database::AbstractItem& item) = 0;

            virtual bool persist(Database::AbstractItem& item) = 0;
            virtual bool persist(Database::AbstractItemDetails& details) = 0;

        protected:
            AbstractObjectManager(Database::AbstractConnection& connection) :
                    m_connection(connection)
            {
            }

            Database::AbstractConnection& m_connection;
    };

    enum SearchCriteriaRelOp
    {
        eq,  // =
        neq, // !=
        lt,  // <
        lte, // <=
        gt,  // >
        gte  // >=
    };

    enum SearchCriteriaStringOp
    {
        contains,
        doesNotContain,
        startsWith,
        derivedFrom
    };

    class AbstractSearchCriteria
    {
        public:
            virtual ~AbstractSearchCriteria()
            {
            }

            virtual const std::string getSearchCapabilities() = 0;

            virtual bool processExistsOp(const std::string property, const std::string value, std::string& result) = 0;
            virtual bool processRelOp(const std::string property, const Database::SearchCriteriaRelOp op, const std::string value, std::string& result) = 0;
            virtual bool processStringOp(const std::string property, const Database::SearchCriteriaStringOp op, const std::string value, std::string& result) = 0;

        protected:
            AbstractSearchCriteria()
            {
            }
    };

    class AbstractSortCriteria
    {
        public:
            virtual ~AbstractSortCriteria()
            {
            }

            virtual const std::string getSortCapabilities() = 0;
            virtual const std::string getSortExtensionCapabilities() = 0;

            virtual bool processField(const std::string direction, const std::string property, std::string& result) = 0;

        protected:
            AbstractSortCriteria()
            {
            }
    };
}

#endif // _FUPPES_DATABASE_PLUGIN_H
