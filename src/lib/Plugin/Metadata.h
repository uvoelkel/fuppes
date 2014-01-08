/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_METADATA_H
#define _PLUGIN_METADATA_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Plugin.h"

#include <fuppes_database_objects.h>

namespace Plugin
{
    typedef int (*openFile_t)(const char* filename, void** data);
    typedef void (*closeFile_t)(void** data);

    typedef int (*readMetadata_t)(::Database::AbstractItem& item, void** data);
    typedef int (*readImage_t)(char* mimeType, unsigned char** buffer, fuppes_off_t* size, void** data);

    class Metadata: public Plugin
    {
        public:
            Metadata();
            Metadata(Metadata* plugin);

            bool init(LibraryHandle handle);

            bool openFile(const std::string filename)
            {
                if (NULL == m_openFile) {
                    return false;
                }
                return 0 == m_openFile(filename.c_str(), &m_data);
            }

            void closeFile()
            {
                if (NULL == m_closeFile) {
                    return;
                }
                m_closeFile(&m_data);
            }

            bool readMetadata(::Database::AbstractItem& item)
            {
                if (NULL == m_readMetadata) {
                    return false;
                }
                return 0 == m_readMetadata(item, &m_data);
            }

            bool readImage(char* mimeType, unsigned char** buffer, fuppes_off_t* size)
            {
                if (NULL == m_readImage) {
                    return false;
                }
                return 0 == m_readImage(mimeType, buffer, size, &m_data);
            }

            // set out endianess
            // decode interleaved
            // encode interleaved

        private:
            openFile_t m_openFile;
            closeFile_t m_closeFile;

            readImage_t m_readImage;
            readMetadata_t m_readMetadata;
    };
}

#endif // _PLUGIN_METADATA_H
