/*
 * This file is part of fuppes
 *
 * (c) 2010-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "PageStart.h"

using namespace std;
using namespace fuppes;
using namespace Presentation;

string PageStart::content()
{
  stringstream result;
  
  result << "<h1>system information</h1>" << endl;
  
  result << "<p>" << endl;
  result << "<div class=\"grid_3\">Version</div><div class=\"grid_9\">" << m_fuppesConfig->globalSettings.appVersion << "</div>" << endl;
  result << "<div class=\"grid_3\">Hostname</div><div class=\"grid_9\">" << m_fuppesConfig->networkSettings.hostname << "</div>" << endl;
  result << "<div class=\"grid_3\">OS</div><div class=\"grid_9\">" << m_fuppesConfig->globalSettings.osName << " " << m_fuppesConfig->globalSettings.osVersion << "</div>" << endl;
  result << "<div class=\"clear\"></div>";
  result << "</p>" << endl;
  
  return result.str();

  result << "<p>" << endl;
  result << "<div class=\"grid_3\">build at</div><div class=\"grid_9\">" << __DATE__ << " - " << __TIME__ << "</div>" << endl;
  result << "<div class=\"grid_3\">build with</div><div class=\"grid_9\">" << __VERSION__ << "</div>" << endl;

  #if defined(HAVE_LIBEVENT2)
  result << "<div class=\"grid_3\">eventlib</div><div class=\"grid_9\">libevent2</div>" << endl;
  #elif defined(HAVE_LIBEV)
  result << "<div class=\"grid_3\">eventlib</div><div class=\"grid_9\">libev</div>" << endl;
  #endif
  result << "<div class=\"clear\"></div>";
  result << "</p>" << endl;
  
  // uptime
  fuppes::DateTime now = fuppes::DateTime::now();
  int uptime = now.toInt() - m_fuppesConfig->globalSettings.startTime.toInt();


  int days;
  int hours;
  int minutes;
  int seconds;

  seconds = uptime % 60;
  minutes = uptime / 60;
  hours = minutes / 60;
  minutes = minutes % 60;
  days = hours / 24;
  hours = hours % 24;
  
  result << "<p>" << endl;
  result << "<div class=\"grid_3\">uptime</div><div class=\"grid_9\">" << days << " days " << hours << " hours " << minutes << " minutes " << seconds << " seconds" << "</div>" << endl;
  result << "<div class=\"clear\"></div>";
  result << "</p>" << endl;

  
  /*result << "<h1>remote devices</h1>";
  result << BuildFuppesDeviceList(CSharedConfig::Shared()->GetFuppesInstance(0));*/



  
  result << "<p>" << endl;
  //result << "cfgfile: " << CSharedConfig::Shared()->filename() << "<br />" << endl;
  result << "<div class=\"grid_3\">cfgfile</div><div class=\"grid_9\">" << m_fuppesConfig->configFilename << "</div>" << endl;
  result << "<div class=\"grid_3\">dbfile</div><div class=\"grid_9\">" << m_fuppesConfig->databaseSettings.filename << "</div>" << endl;
  result << "<div class=\"grid_3\">thumbnaildir</div><div class=\"grid_9\">" << m_fuppesConfig->contentDatabaseSettings.getThumbnailDirectory() << "</div>" << endl;
  result << "<div class=\"clear\"></div>";
  result << "</p>" << endl;
/*
  result << "<p>" << endl;
  result << "<a href=\"http://sourceforge.net/projects/fuppes/\">http://sourceforge.net/projects/fuppes/</a><br />" << endl;
  result << "</p>" << endl;
  
  
  result << "<h1>database status</h1>" << endl;  
  result << buildObjectStatusTable() << endl;

  result << "<ul>";
  result << "<li><a href=\"javascript:fuppesCtrl('DatabaseRebuild');\">rebuild database</a></li>";
  result << "<li><a href=\"javascript:fuppesCtrl('DatabaseUpdate');\">update database</a></li>";
  result << "<li><a href=\"javascript:fuppesCtrl('VfolderUpdate');\">update virtual folders</a></li>";
  result << "</ul>";

*/
  
  return result.str();
}


string PageStart::buildObjectStatusTable()
{
  //OBJECT_TYPE nType = OBJECT_TYPE_UNKNOWN;
  stringstream result;

  /*
	SQLQuery qry;
  string sql = qry.build(SQL_GET_OBJECT_TYPE_COUNT, 0);
	qry.select(sql);
  
  result << 
    "<table rules=\"all\" style=\"font-size: 10pt; border-style: solid; border-width: 1px; border-color: #000000;\" cellspacing=\"0\" width=\"400\">" << endl <<
      "<thead>" << endl <<
        "<tr>" << endl <<        
          "<th>Type</th>" << 
          "<th>Count</th>" << endl <<
        "</tr>" << endl <<
      "</thead>" << endl << 
      "<tbody>" << endl;  


  while(!qry.eof()) {
    nType = (OBJECT_TYPE)qry.result()->asInt("TYPE");
    
    result << "<tr>" << endl;
    result << "<td>" << CFileDetails::Shared()->GetObjectTypeAsStr(nType) << "</td>" << endl;
    result << "<td>" << qry.result()->asString("VALUE") << "</td>" << endl;    
    result << "</tr>" << endl;
    
    qry.next();
  }
    
  result <<
      "</tbody>" << endl <<   
    "</table>" << endl;
*/

  return result.str();

}

