/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_MANAGER_H
#define _PLUGIN_MANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <map>

#include "Plugin.h"
#include "Database.h"
#include "ItemSource.h"
#include "Metadata.h"

#include "Decoder.h"
#include "Encoder.h"
#include "Transcoder.h"


namespace Plugin
{

    class Manager
    {
        public:
            ~Manager();
            bool load(const std::string path);

            ::Plugin::Database* getDatabasePlugin(const std::string name);

            ::Plugin::ItemSource* createItemSourcePlugin(const std::string name);
            ::Plugin::Metadata* createMetadataPlugin(const std::string name);

            ::Plugin::AbstractDecoder* createDecoderPlugin(const std::string name);
            ::Plugin::AbstractEncoder* createEncoderPlugin(const std::string name);
            ::Plugin::AbstractTranscoder* createTranscoderPlugin(const std::string name);

        private:
            void registerPlugin(const std::string filename);

            std::map<std::string, ::Plugin::Plugin*> m_plugins;

            std::map<std::string, ::Plugin::Database*> m_databasePlugins;
            std::map<std::string, ::Plugin::ItemSource*> m_itemSourcePlugins;
            std::map<std::string, ::Plugin::Metadata*> m_metadataPlugins;

            std::map<std::string, ::Plugin::Decoder*> m_decoderPlugins;
            std::map<std::string, ::Plugin::Encoder*> m_encoderPlugins;
            std::map<std::string, ::Plugin::Transcoder*> m_transcoderPlugins;
    };
}

#endif // _PLUGIN_MANAGER_H
