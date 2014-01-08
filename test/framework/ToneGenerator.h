/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TONEGENERATOR_H
#define _TONEGENERATOR_H

#include <Common/File.h>

class ToneGenerator
{
    public:
        ToneGenerator();
        ~ToneGenerator();

        void generate(const int32_t duration, const std::string filename);

    private:

        void writeHeader(fuppes::File& file);

        double m_pi;

        int32_t m_samplerate;
        int16_t m_bitsPerSample;
        int16_t m_channels;
        double m_frequency;

        int32_t m_bytesPerSec;
        int32_t m_size;

};

#endif // _TONEGENERATOR_H
