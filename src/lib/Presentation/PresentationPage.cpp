/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include"PresentationHandler.h"
#include"PresentationPage.h"


using namespace Presentation;

using namespace std;

std::string PresentationPage::render(std::string layout, Fuppes::FuppesConfig* fuppesConfig)
{
    m_fuppesConfig = fuppesConfig;

    layout = StringReplace(layout, "{{ APPVERSION }}", m_fuppesConfig->globalSettings.appVersion);
    layout = StringReplace(layout, "{{ APPNAME }}", m_fuppesConfig->globalSettings.appName);
    layout = StringReplace(layout, "{{ APPFULLNAME }}", m_fuppesConfig->globalSettings.appFullName);

    layout = StringReplace(layout, "{{ TITLE }}", this->title());
    layout = StringReplace(layout, "{{ NAVIGATION }}", this->renderNavigation());
    layout = StringReplace(layout, "{{ CONTENT }}", this->content());


    std::string js;
    if(this->js().length() > 0) {
        js = "<script type=\"text/javascript\" src=\"/presentation/" + this->js() + "\"></script>";
    }
    layout = StringReplace(layout, "{{ JS }}", js);


    return layout;
}

std::string PresentationPage::renderNavigation()
{
    std::stringstream stream;

    stream << "<ul>";

    stream << "<li><a href=\"/" << PresentationPages::get("index")->alias() << ".html\">" << 
        PresentationPages::get("index")->title() << "</a></li>" << endl;

    
    stream << "<li><a href=\"/presentation/" << PresentationPages::get("browse")->alias() << ".html\">" << 
        PresentationPages::get("browse")->title() << "</a></li>" << endl;

    stream << "<li><a href=\"/presentation/" << PresentationPages::get("config")->alias() << ".html\">" << 
        PresentationPages::get("config")->title() << "</a></li>" << endl;

    stream << "<li><a href=\"/presentation/" << PresentationPages::get("database")->alias() << ".html\">" <<
            PresentationPages::get("database")->title() << "</a></li>" << endl;

    
    /*std::map<std::string, PresentationPage*>::const_iterator iter;    
    for(iter = PresentationPages::m_pages.begin(); iter != PresentationPages::m_pages.end(); iter++) {

        if(iter->first.compare("index") == 0) {
            stream << "<li><a href=\"/index.html\">" << iter->second->title() << "</a></li>" << endl;
            continue;
        }
        else {
            stream << "<li><a href=\"/presentation/" << iter->first << ".html\">" << iter->second->title() << "</a></li>" << endl;
        }

    }*/

    stream << "</ul>";
    
    return stream.str();
}
