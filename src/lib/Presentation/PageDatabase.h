/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PRESENTATION_PAGEDATABASE_H
#define _PRESENTATION_PAGEDATABASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PresentationPage.h"

namespace Presentation
{

    class PageDatabase: public Presentation::PresentationPage
    {
        public:
            std::string alias()
            {
                return "database";
            }

            std::string title()
            {
                return "database";
            }

            std::string content();
    };

}

#endif // _PRESENTATION_PAGEDATABASE_H
