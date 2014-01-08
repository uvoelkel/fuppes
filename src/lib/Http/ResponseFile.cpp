/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ResponseFile.h"
using namespace Http;

#include <stdlib.h>

#define FILE_BUFFER_SIZE 1024

//#include <iostream>

BaseResponseFile::BaseResponseFile(fuppes::File* file /*= NULL*/)
{
    m_contentBuffer = NULL;
    m_contentBufferSize = 0;
    
    m_file = file;    
}

BaseResponseFile::BaseResponseFile(std::string filename)
{
    m_file = NULL;
    m_contentBuffer = NULL;
    m_contentBufferSize = 0;
    
    this->setFilename(filename);
}

void BaseResponseFile::setFile(fuppes::File* file)
{
    m_file = file;
}

void BaseResponseFile::setFilename(std::string filename)
{
    m_filename = filename;
    m_file = new fuppes::File(m_filename);
    m_file->open(fuppes::File::Read);
}

BaseResponseFile::~BaseResponseFile()
{
    if(m_filename.length() > 0) {
        m_file->close();
        delete m_file;
    }

    if(NULL != m_contentBuffer) {
        free(m_contentBuffer);
    }
}

fuppes_off_t BaseResponseFile::getContentSize()
{
    if(NULL == m_file)
        return 0;
    
    return m_file->size();
}

fuppes_off_t BaseResponseFile::getContent(HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof)
{
    // allocate the buffer
    if(NULL == m_contentBuffer) {
        m_contentBuffer = (char*)malloc(FILE_BUFFER_SIZE);
        m_contentBufferSize = FILE_BUFFER_SIZE;
    }


    if(offset >= this->getContentSize()) {
        return 0;
    }

    if(!m_file->seek(offset))
        return 0;

    // calc the rest
    if(0 == size || offset + size >= this->getContentSize()) {
        size = this->getContentSize() - offset;
        *eof = true;
    }

    // check if the rest exceeds the buffer size
    fuppes_off_t read = size;
    if(read > m_contentBufferSize) {
        read = m_contentBufferSize;
        *eof = false;
    }
    
    *buffer = &m_contentBuffer[0];
    fuppes_off_t length = m_file->read(m_contentBuffer, read);
    return length;
}

