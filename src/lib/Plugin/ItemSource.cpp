/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ItemSource.h"

using namespace Plugin;

ItemSource::ItemSource() :
        Plugin::Plugin()
{
    m_sourceOpen = NULL;
    m_sourceNext = NULL;
    m_sourceClose = NULL;
}

ItemSource::ItemSource(ItemSource* plugin) :
        Plugin(plugin)
{
    m_sourceOpen = plugin->m_sourceOpen;
    m_sourceNext = plugin->m_sourceNext;
    m_sourceClose = plugin->m_sourceClose;
}

bool ItemSource::init(LibraryHandle handle)
{
    m_sourceOpen = (sourceOpen_t)Plugin::Plugin::getProcedure(handle, "fuppes_source_open");
    m_sourceNext = (sourceNext_t)Plugin::Plugin::getProcedure(handle, "fuppes_source_next");
    m_sourceClose = (sourceClose_t)Plugin::Plugin::getProcedure(handle, "fuppes_source_close");

    return true;
}

bool ItemSource::open(const std::string source)
{
    if (NULL == m_sourceOpen) {
        return false;
    }
    return 0 == m_sourceOpen(source.c_str(), &m_data);
}

bool ItemSource::getNext(Database::AbstractItem& item)
{
    if (NULL == m_sourceNext) {
        return false;
    }
    return 0 == m_sourceNext(item, &m_data);
}

void ItemSource::close()
{
    if (NULL == m_sourceClose) {
        return;
    }
    m_sourceClose(&m_data);
}
