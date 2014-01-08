/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PRESENTATION_PRESENTATION_HANDLER_H
#define _PRESENTATION_PRESENTATION_HANDLER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Fuppes/HttpRequest.h"
#include "../Fuppes/HttpResponse.h"

#include "PresentationPage.h"
#include "../Configuration/FuppesConfig.h"

#include <map>
#include <iostream>

namespace Presentation
{
    struct PresentationFile
    {
        static PresentationFile create(std::string mimeType) 
        {
            PresentationFile file;
            file.mimeType = mimeType;
            return file;
        }

        std::string mimeType;
    };
    
    struct PresentationFiles 
    {
        static std::map<std::string, PresentationFile> create();
        static const std::map<std::string, PresentationFile> m_map;

        static const PresentationFile* get(std::string file) {
            std::map<std::string, PresentationFile>::const_iterator iter;
            if((iter = PresentationFiles::m_map.find(file)) != PresentationFiles::m_map.end()) {
                return &iter->second;
            }
            return NULL;
        }
    };
    

    struct PresentationPages: public std::map<std::string, PresentationPage*>
    {
        static PresentationPages create();
        static const PresentationPages m_pages;

        static PresentationPage* get(std::string name) {
            std::map<std::string, PresentationPage*>::const_iterator iter = PresentationPages::m_pages.find(name);
            if(iter != PresentationPages::m_pages.end()) {
                return (PresentationPage*)iter->second;
            }
            return NULL;
        }

        ~PresentationPages() {
            std::map<std::string, PresentationPage*>::iterator iter;
            for(iter = this->begin(); iter != this->end(); iter++) {
                delete iter->second;
            }
        }

    };
    
    class PresentationHandler
    {
        public:
            PresentationHandler(Fuppes::FuppesConfig& fuppesConfig);
            ~PresentationHandler();

            Fuppes::HttpResponse* handleRequest(Fuppes::HttpRequest* request);
            
        private:
            Fuppes::FuppesConfig&   m_fuppesConfig;
            std::string             m_layout;
    };

}

#endif // _PRESENTATION_PRESENTATION_HANDLER_H
