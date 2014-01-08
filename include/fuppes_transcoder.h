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

#ifndef _FUPPES_TRANSCODER_H
#define _FUPPES_TRANSCODER_H

typedef enum TranscoderCapability_t {
    TC_NONE             = 0,
    TC_TIME_BASED_SEEK  = 1 << 0,
    TC_PAUSE            = 1 << 1,
} TranscoderCapability_t;

typedef enum TranscoderTarget_t {
    TT_FILE             = 0
} TranscoderTarget_t;

#endif // _FUPPES_TRANSCODER_H

#ifdef __cplusplus
}
#endif
