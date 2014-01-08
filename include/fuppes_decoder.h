/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FUPPES_DECODER_H
#define _FUPPES_DECODER_H

typedef enum DecoderCapability_t {
    DC_NONE             = 0,
    DC_TIME_BASED_SEEK  = 1 << 0,
    DC_PAUSE            = 1 << 1,
} DecoderCapability_t;

typedef enum DecoderTarget_t {
    DT_MEMORY           = 0
} DecoderTarget_t;

#endif // _FUPPES_DECODER_H

#ifdef __cplusplus
}
#endif
