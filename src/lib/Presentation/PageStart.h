/*
 * This file is part of fuppes
 *
 * (c) 2010-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PRESENTATION_PAGESTART_H
#define _PRESENTATION_PAGESTART_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PresentationPage.h"

namespace Presentation
{

    class PageStart: public Presentation::PresentationPage
    {
        public:
            std::string alias()
            {
                return "index";
            }
            std::string title()
            {
                return "start";
            }

            virtual std::string content();

        private:
            std::string buildObjectStatusTable();
    };

}

#endif // _PRESENTATION_PAGESTART_H
