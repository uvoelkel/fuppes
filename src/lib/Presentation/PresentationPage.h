/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PRESENTATION_PRESENTATION_PAGE_H
#define _PRESENTATION_PRESENTATION_PAGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Configuration/FuppesConfig.h"

#include <string>
#include <sstream>

namespace Presentation
{

    class PresentationPage
    {
        public:
            std::string render(std::string layout, Fuppes::FuppesConfig* fuppesConfig);

            virtual ~PresentationPage() { }
            
        protected:
            virtual std::string alias() = 0;
            virtual std::string title() = 0;
            virtual std::string js() { return ""; }
            virtual std::string content() = 0;

            Fuppes::FuppesConfig* m_fuppesConfig;

        private:
            std::string renderNavigation();
    };

}
    
#endif // _PRESENTATION_PRESENTATION_PAGE_H
