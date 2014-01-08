/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ToneGenerator.h"

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <climits>

ToneGenerator::ToneGenerator()
{
    m_pi = std::atan(1.0) * 4;

    m_samplerate = 44100;
    m_bitsPerSample = 16;
    m_channels = 2;

    m_frequency = 440;

    m_size = 0;
    m_bytesPerSec = 0;
}

ToneGenerator::~ToneGenerator()
{
}

#define WRITE_U32(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);\
                          *((buf)+2) = (unsigned char)(((x)>>16)&0xff);\
                          *((buf)+3) = (unsigned char)(((x)>>24)&0xff);

#define WRITE_U16(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);

void ToneGenerator::generate(const int32_t duration, const std::string filename)
{
    fuppes::File file(filename);
    if (!file.open(fuppes::File::Write | fuppes::File::Truncate)) {
        std::cerr << "error opening file: " << filename << std::endl;
        return;
    }

    m_bytesPerSec = m_channels * m_samplerate * (m_bitsPerSample / 8);
    m_size = duration * m_bytesPerSec;

    writeHeader(file);


    char* buffer = new char[sizeof(int16_t) * m_channels];

    for (int32_t s = 0; s < m_samplerate * duration; s += m_channels) {

        double time = (double)s / (double)m_samplerate;
        double data = std::sin(2 * m_pi * m_frequency * time) * SHRT_MAX;

        for (int16_t channel = 0; channel < m_channels; channel++) {
            WRITE_U16(buffer + (channel * sizeof(int16_t)), (int16_t)data);
        }
        file.write(buffer, sizeof(buffer));
    }

    file.close();
    delete buffer;

}

void ToneGenerator::writeHeader(fuppes::File& file)
{
    unsigned char header[45];
    memset(header, 0, sizeof(header));

    memcpy(header, "RIFF", 4);
    WRITE_U32(header + 4, m_size - 8);
    memcpy(header + 8, "WAVE", 4);
    memcpy(header + 12, "fmt ", 4);
    WRITE_U32(header + 16, 16);
    WRITE_U16(header + 20, 1);

    WRITE_U16(header + 22, m_channels);
    WRITE_U32(header + 24, m_samplerate);
    WRITE_U32(header + 28, m_bytesPerSec);
    WRITE_U16(header + 32, m_channels * (m_bitsPerSample / 8));
    WRITE_U16(header + 34, m_bitsPerSample);
    memcpy(header + 36, "data", 4);
    WRITE_U32(header + 40, m_size - 44);

    file.write((const char*)header, 44);
}
