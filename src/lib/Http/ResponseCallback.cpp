/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ResponseCallback.h"
using namespace Http;


#include <iostream>

BaseResponseCallback::BaseResponseCallback(contentCallback_t callback /*= NULL*/)
{
    m_contentCallback = callback;
}


void BaseResponseCallback::setCallback(contentCallback_t callback)
{
    m_contentCallback = callback;
}
            
fuppes_off_t BaseResponseCallback::getContentSize()
{    
    return 0;
}

fuppes_off_t BaseResponseCallback::getContent(HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof)
{
    if(NULL == m_contentCallback) {
        *eof = true;
        return 0;
    }
    
    return m_contentCallback(session, buffer, offset, eof);
}
