/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "DateTime.h"
#include <time.h>

using namespace fuppes;

DateTime DateTime::now() // static
{
    DateTime result;
    result.m_time = time(NULL);
    return result;
}

DateTime::DateTime()
{
    m_time = -1;
}

DateTime::DateTime(const time_t time)
{
    m_time = time;
}

bool DateTime::valid()
{
    return (m_time != -1);
}

std::string DateTime::toString() 
{ 
    return ctime(&m_time);
}

int DateTime::toInt() 
{ 
    return m_time;
}

/*
DateTime& DateTime::operator=(const DateTime &dateTime) 
{
  if (this == &dateTime)
    return *this;

	m_time = dateTime.m_time;
  return *this;
}
*/
