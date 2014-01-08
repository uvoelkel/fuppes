/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "PresentationHandler.h"
using namespace Presentation;

#include "../Common/RegEx.h"
#include "../Common/File.h"
//#include "../SharedConfig.h"

#include "PageStart.h"
#include "PageBrowse.h"
#include "PageConfig.h"
#include "PageDatabase.h"

#include "PagePlaylist.h"

#include <time.h>

const std::map<std::string, PresentationFile> PresentationFiles::m_map = PresentationFiles::create();
std::map<std::string, PresentationFile> PresentationFiles::create() // static
{
    std::map<std::string, PresentationFile> map;

    // device icons
    map["fuppes-icon-48x48.png"] = PresentationFile::create("image/png");
    map["fuppes-icon-48x48.jpg"] = PresentationFile::create("image/jpeg");
    map["fuppes-icon-120x120.png"] = PresentationFile::create("image/png");
    map["fuppes-icon-120x120.jpg"] = PresentationFile::create("image/jpeg");

    // stylesheets
    map["style.css"] = PresentationFile::create("text/css");
    map["grid.css"] = PresentationFile::create("text/css");

    // javascripts
    map["mootools-core-1.4.5-full-nocompat.js"] = PresentationFile::create("text/javascript");
    map["mootools-more-1.4.0.1.js"] = PresentationFile::create("text/javascript");

    map["fuppes.js"] = PresentationFile::create("text/javascript");
    map["fuppes-control.js"] = PresentationFile::create("text/javascript");

    map["fuppes-browse.js"] = PresentationFile::create("text/javascript");
    map["fuppes-config.js"] = PresentationFile::create("text/javascript");
    map["fuppes-database.js"] = PresentationFile::create("text/javascript");

    map["fuppes-playlist.js"] = PresentationFile::create("text/javascript");



    map["jquery-1.10.2.min.js"] = PresentationFile::create("text/javascript");

    map["fuppes.jquery.js"] = PresentationFile::create("text/javascript");
    map["fuppes-database.jquery.js"] = PresentationFile::create("text/javascript");

    map["fuppes-browse.jquery.js"] = PresentationFile::create("text/javascript");

    return map;
}

const PresentationPages PresentationPages::m_pages = PresentationPages::create();
PresentationPages PresentationPages::create() // static
{
    PresentationPages pages;

    pages["index"] = new PageStart();
    pages["browse"] = new PageBrowse();
    pages["config"] = new PageConfig();
    pages["database"] = new PageDatabase();

    pages["playlist"] = new PagePlaylist();

    return pages;
}

PresentationHandler::PresentationHandler(Fuppes::FuppesConfig& fuppesConfig) :
        m_fuppesConfig(fuppesConfig)
{
    std::string layout = m_fuppesConfig.globalSettings.resourcesDir + "layout.html";
    fuppes::File file(layout);
    file.open(fuppes::File::Read); //  | fuppes::File::Text
    fuppes_off_t size = file.size();
    char* buffer = new char[size + 1];
    file.read(buffer, size);
    buffer[size] = '\0';
    file.close();
    m_layout = buffer;
    delete[] buffer;
}

PresentationHandler::~PresentationHandler()
{
}

Fuppes::HttpResponse* PresentationHandler::handleRequest(Fuppes::HttpRequest* request)
{
    std::string name;
    std::string ext;
    std::string uri = request->getUri();

    if ((uri.compare("/") == 0) || (uri.compare("/index.html") == 0)) {
        name = "index";
        ext = "html";
    }
    else if (uri.find("/presentation/") == 0) {

        size_t pos = uri.find_last_of(".");
        if (pos == std::string::npos)
            return NULL;

        ext = uri.substr(pos + 1);
        name = uri.substr(14, pos - 14); // 14 = strlen("/presentation/")
    }

    if (name.empty())
        return NULL;

    Fuppes::HttpResponse* response = NULL;

    //std::cout << "PRESENTATION REQUEST: " << request->getUri() <<  " :: " << name << " . " << ext << std::endl;

    const PresentationFile* file = PresentationFiles::get(name + "." + ext);
    if (NULL != file) {

        std::string filename = m_fuppesConfig.globalSettings.resourcesDir + name + "." + ext;
        if (!fuppes::File::exists(filename)) {
            return NULL;
        }

        response = new Fuppes::HttpResponseFile(Http::OK, file->mimeType, filename);

        char date[30];
        time_t tTime = time(NULL);
        tTime += (60 * 60 * 24);
        strftime(date, 30, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tTime));
        response->getHeader()->setValue("Expires", date);
        return response;
    }

    PresentationPage* page = PresentationPages::get(name);
    if (NULL != page) {
        response = new Fuppes::HttpResponse(Http::OK, "text/html; charset=\"utf-8\"", page->render(m_layout, &m_fuppesConfig));
        return response;
    }

    return response;
}
