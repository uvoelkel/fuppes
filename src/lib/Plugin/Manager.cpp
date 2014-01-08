/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Manager.h"

#include "../Common/Directory.h"
#include "../Common/File.h"

//using namespace Plugin;

Plugin::Manager::~Manager()
{
    std::map<std::string, Plugin*>::iterator iter;
    for (iter = m_plugins.begin(); iter != m_plugins.end(); iter++) {
        iter->second->unload();
        delete iter->second;
    }
    m_plugins.clear();
}

bool Plugin::Manager::load(const std::string path)
{
    fuppes::Directory dir(path);
    if (!dir.open()) {
        return false;
    }

    fuppes::DirEntryList entryList = dir.dirEntryList();
    for (unsigned int i = 0; i < entryList.size(); i++) {

        if (fuppes::DirEntry::File == entryList.at(i).type()) {
            registerPlugin(entryList.at(i).absolutePath());
        }
    }
    dir.close();
    return true;
}

void Plugin::Manager::registerPlugin(const std::string filename)
{
    if (!fuppes::File::exists(filename)) {
        return;
    }

    LibraryHandle handle = Plugin::loadLibrary(filename);
    if (NULL == handle) {
        return;
    }

    register_t registerProc = (register_t)Plugin::getProcedure(handle, "fuppes_register_plugin");
    unregister_t unregisterProc = (unregister_t)Plugin::getProcedure(handle, "fuppes_unregister_plugin");
    if (NULL == registerProc || NULL == unregisterProc) {
        Plugin::closeLibrary(handle);
        return;
    }

    plugin_info info;
    info.type = ptUnknown;
    info.name[0] = '\0';
    info.author[0] = '\0';
    info.user_data = NULL;
    registerProc(&info);

    Plugin* plugin;
    switch (info.type) {
        case ptDatabase:
            plugin = new Database();
            break;
        case ptMetadata:
            plugin = new Metadata();
            break;
        case ptItemSource:
            plugin = new ItemSource();
            break;
        case ptDecoder:
            plugin = new Decoder();
            break;
        case ptEncoder:
            plugin = new Encoder();
            break;
        case ptTranscoder:
            plugin = new Transcoder();
            break;
        default:
            plugin = NULL;
            break;
    }

    if (!plugin->load(handle, registerProc, unregisterProc, &info)) {
        delete plugin;
        return;
    }

    m_plugins[plugin->getName()] = plugin;

    switch (info.type) {

        case ptDatabase:
            m_databasePlugins[plugin->getName()] = (Database*)plugin;
            break;

        case ptItemSource:
            m_itemSourcePlugins[plugin->getName()] = (ItemSource*)plugin;
            break;
        case ptMetadata:
            m_metadataPlugins[plugin->getName()] = (Metadata*)plugin;
            break;

        case ptDecoder:
            m_decoderPlugins[plugin->getName()] = (Decoder*)plugin;
            break;
        case ptEncoder:
            m_encoderPlugins[plugin->getName()] = (Encoder*)plugin;
            break;
        case ptTranscoder:
            m_transcoderPlugins[plugin->getName()] = (Transcoder*)plugin;
            break;

        default:
            break;
    }


}

Plugin::Database* Plugin::Manager::getDatabasePlugin(const std::string name)
{
    if (m_databasePlugins.find(name) == m_databasePlugins.end()) {
        return NULL;
    }
    return m_databasePlugins[name];
}

Plugin::ItemSource* Plugin::Manager::createItemSourcePlugin(const std::string name)
{
    if (m_itemSourcePlugins.find(name) == m_itemSourcePlugins.end()) {
        return NULL;
    }
    return new ItemSource(m_itemSourcePlugins[name]);
}

Plugin::Metadata* Plugin::Manager::createMetadataPlugin(const std::string name)
{
    if (m_metadataPlugins.find(name) == m_metadataPlugins.end()) {
        return NULL;
    }
    return new Metadata(m_metadataPlugins[name]);
}


Plugin::AbstractDecoder* Plugin::Manager::createDecoderPlugin(const std::string name)
{
    if (m_decoderPlugins.find(name) == m_decoderPlugins.end()) {
        return NULL;
    }
    return new Decoder(m_decoderPlugins[name]);
}

Plugin::AbstractEncoder* Plugin::Manager::createEncoderPlugin(const std::string name)
{
    if (m_encoderPlugins.find(name) == m_encoderPlugins.end()) {
        return NULL;
    }
    return new Encoder(m_encoderPlugins[name]);
}

Plugin::AbstractTranscoder* Plugin::Manager::createTranscoderPlugin(const std::string name)
{
    if (m_transcoderPlugins.find(name) == m_transcoderPlugins.end()) {
        return NULL;
    }
    return new Transcoder(m_transcoderPlugins[name]);
}
