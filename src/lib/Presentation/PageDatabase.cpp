/*
 * This file is part of fuppes
 *
 * (c) 2010-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "PageDatabase.h"

using namespace std;
using namespace fuppes;
using namespace Presentation;

string PageDatabase::content()
{
  stringstream result;
  
  result << "<h1>database</h1>" << endl;
  result << "<div class=\"database-status\"></div>" << endl;

  result << "<div id=\"dbcontrols\"></div>" << endl;
  
  return result.str();

}

