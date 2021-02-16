/**
 * (c) Copyright 2021 tripulse
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef RKPI2
#define RKPI2

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of samples captured/played-back in a second.
 */
typedef enum {
    rkpi2_rate_8000hz,
    rkpi2_rate_11025hz,
    rkpi2_rate_12000hz,
    rkpi2_rate_16000hz,
    rkpi2_rate_22050hz,
    rkpi2_rate_24000hz,
    rkpi2_rate_32000hz,
    rkpi2_rate_44100hz,
    rkpi2_rate_48000hz,
    rkpi2_rate_64000hz,
    rkpi2_rate_88200hz,
    rkpi2_rate_96000hz,
    rkpi2_rate_176400hz,
    rkpi2_rate_192000hz
} rkpi2_samplerate_t;

/**
 * @brief Data type of samples digitally in memory.
 */
typedef enum {
    rkpi2_fmt_signed8,
    rkpi2_fmt_signed16,
    rkpi2_fmt_float32,
    rkpi2_fmt_float64
} rkpi2_samplefmt_t;

/**
 * @brief Byte order of each sample values.
 */
typedef enum {
    rkpi2_big_endian,
    rkpi2_little_endian
} rkpi2_endianness_t;

/**
 * @brief Header structure to hold information about the
 * contained PCM data in a RKPI2 file.
 */
typedef struct {
    rkpi2_endianness_t endian;
    rkpi2_samplefmt_t format;
    rkpi2_samplerate_t rate;
    unsigned int channels : 3;
} rkpi2_header_t;

/**
 * @brief Error values returned from the parser routine.
 */
typedef enum {
    rkpi2_no_error,
    rkpi2_invalid_signature_error,
    rkpi2_reserved_rate_use_error
} rkpi2_error_t;

/**
 * @brief Parse RKPI2 header bitstream into a structure.
 * @param hdr Structure to fill in successfuly parsed info.
 * @param rawhdr Raw header bitstream of atleast 2-bytes.
 */
rkpi2_error_t rkpi2_parse_header(rkpi2_header_t* hdr, const unsigned char rawhdr[2]) {
    if(rawhdr[0] >> 2 != 0x32)
        return rkpi2_invalid_signature_error;
    
    hdr->endian = (rawhdr[0] >> 1) & 1;
    hdr->format = (rawhdr[0] & 1) << 1 | rawhdr[1] >> 7;
    
    if((hdr->rate = (rawhdr[1] >> 3) & 0xF) > 13)
        return rkpi2_reserved_rate_use_error;
    
    hdr->channels = (rawhdr[1] & 7) + 1;
    return rkpi2_no_error;
}

/**
 * @brief Code RKPI2 header structure to its bitstream form.
 * @param rawhdr Character array to hold atleast 2-bytes of bitstream.
 * @param hdr Structure to convert into its bitstream form. 
 */
void rkpi2_code_header(unsigned char rawhdr[2], const rkpi2_header_t hdr) {
    rawhdr[0] = 0xc8 | 
                hdr.endian << 1 | 
                hdr.format >> 1;
    
    rawhdr[1] = (hdr.format & 1) << 7 | 
                hdr.rate << 3 | 
                (hdr.channels - 1);
}

#ifdef __cplusplus
}
#endif
#endif