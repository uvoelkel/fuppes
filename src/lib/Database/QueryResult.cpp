/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "QueryResult.h"

using namespace Database;

void ResultRow::setColumnNull(const std::string column)
{

}

void ResultRow::setColumnText(const std::string column, const std::string value)
{
    m_textColumns[column] = value;
}

void ResultRow::setColumnInt64(const std::string column, const fuppes_int64_t value)
{
    m_int64Columns[column] = value;
}

AbstractResultRow* QueryResult::addRow()
{
    ResultRow row;
    m_rows.push_back(row);
    return &m_rows.at(m_rows.size() - 1);
}

fuppes_int64_t ResultRow::getColumnInt64(const std::string column)
{
    if(m_int64Columns.find(column) == m_int64Columns.end()) {
        return -1;
    }
    return m_int64Columns[column];
}

std::string ResultRow::getColumnString(const std::string column)
{
    if(m_textColumns.find(column) == m_textColumns.end()) {
        return "";
    }
    return m_textColumns[column];
}

size_t QueryResult::size()
{
    return m_rows.size();
}

void QueryResult::clear()
{
    m_rows.clear();
}

ResultRow* QueryResult::getRow(size_t index)
{
    if(m_rows.size() <= index) {
        return NULL;
    }
    return &m_rows.at(index);
}
