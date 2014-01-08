/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _UPNP_UPNPOBJECT_H
#define _UPNP_UPNPOBJECT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <stdlib.h>
#include <map>

#include <fuppes_types.h>

#include "UPnPObjectTypes.h"

namespace UPnP
{
    class UPnPObject
    {
        public:
            enum TranscodeType_t
            {
                TRANSCODE_NONE,             // no transcoding
                TRANSCODE_RENAME,           // just rename a file (e.g. serve vdr files as vob)
                TRANSCODE_EXTRACT,          // extract data (e.g. embedded image) from file
                TRANSCODE_ONESHOT,          // transcode in one shot (e.g. convert/scale an image)
                TRANSCODE_EXTRACT_ONESHOT,  // extract and transcode (e.g. get an embedded image and scale it)
                TRANSCODE_THREADED          // full transcoding over multiple requests (e.g. audio or video conversion)
            };

            UPnPObject(object_id_t objectId, OBJECT_TYPE objectType, std::string path, std::string extension, std::string mimeType) {
                m_objectId = objectId;
                m_objectType = objectType;
                m_originalPath = path;
                m_originalExtension = extension;
                m_originalMimeType = mimeType;
                m_transcodeType = TRANSCODE_NONE;
            }

            virtual ~UPnPObject() {

            }

            object_id_t getObjectId() {
                return m_objectId;
            }

            OBJECT_TYPE getObjectType() {
                return m_objectType;
            }

            void setObjectType(OBJECT_TYPE objectType) {
                m_objectType = objectType;
            }

            TranscodeType_t getTranscodeType() {
                return m_transcodeType;
            }

            void setTranscodeType(TranscodeType_t transcodeType) {
                m_transcodeType = transcodeType;
            }



            std::string getOriginalPath() {
                return m_originalPath;
            }

            void setPath(std::string path) {
                m_targetPath = path;
            }
            
            std::string getPath() {
                if(!m_targetPath.empty())
                    return m_targetPath;
                else
                    return m_originalPath;
            }
            

            std::string getOriginalExtension() {
                return m_originalExtension;
            }

            void setExtension(std::string extension) {
                m_targetExtension = extension;
            }
            
            std::string getExtension() {
                if(!m_targetExtension.empty())
                    return m_targetExtension;
                else
                    return m_originalExtension;
            }

            std::string getOriginalMimeType() {
                return m_originalMimeType;
            }

            void setMimeType(std::string mimeType) {
                m_targetMimeType = mimeType;
            }

            std::string getMimeType() {
                if(!m_targetMimeType.empty())
                    return m_targetMimeType;
                else
                    return m_originalMimeType;
            }

            void setOriginal(std::string key, std::string value) {
                m_originalValues[key] = value;
                m_targetValues[key] = value;
            }
            
            void set(std::string key, std::string value) {
                m_targetValues[key] = value;
            }

            std::string get(std::string key, std::string defaultValue = "") {
                std::map<std::string, std::string>::iterator iter;
                iter = m_targetValues.find(key);
                if(m_targetValues.end() == iter)
                    return defaultValue;  
                return m_targetValues[key];
            }

            int getAsInt(std::string key) {
                std::string value = get(key, "0");
                return atoi(value.c_str());
            }

            unsigned int getAsUInt(std::string key) {
                std::string value = get(key, "0");
                return strtoul(value.c_str(), NULL, 0);
            }


            void setDlna4thField(std::string dlna4thField) {
                m_dlna4thField = dlna4thField;
            }

            std::string getDlna4thField() {
                return m_dlna4thField;
            }
            
        private:
            object_id_t         m_objectId;
            OBJECT_TYPE         m_objectType;
            TranscodeType_t     m_transcodeType;

            std::string     m_originalPath;
            std::string     m_targetPath;
            
            std::string     m_originalExtension;
            std::string     m_targetExtension;

            std::string     m_originalMimeType;
            std::string     m_targetMimeType;

            std::map<std::string, std::string>  m_originalValues;
            std::map<std::string, std::string>  m_targetValues;


            std::string     m_dlna4thField;
    };

}

#endif // _UPNP_UPNPOBJECT_H
