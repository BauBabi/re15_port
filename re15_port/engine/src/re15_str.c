/*
 * re15_str.c — PSX STR/MDEC streaming-movie demux + decode. See re15_str.h for
 * the byte-true container layout (derived from CAPCOM.STR). The MDEC codec is
 * the same one the room backgrounds use, so this reuses the port's byte-true
 * BSS pipeline unchanged — this file is purely the STR container demux.
 */
#include "re15_str.h"
#include "re15_bss.h"
#include <stdlib.h>
#include <string.h>

#define STR_SECTOR        2336   /* CD-XA Mode2/Form1 sector (no sync/header)  */
#define STR_SUBHDR        8      /* CD-XA subheader                            */
#define STR_FRAME_HDR     32     /* STR frame header (user +0)                 */
#define STR_PAYLOAD_OFF   (STR_SUBHDR + STR_FRAME_HDR)   /* = 40 (0x28)        */
#define STR_PAYLOAD_LEN   2016   /* 2048 user data − 32 STR header (Form1)     */
#define STR_VIDEO_MAGIC   0x0160 /* STR frame header magic (video sector)      */
#define STR_MAX_FRAME_SEC 16     /* sane upper bound on sectors/frame          */

/* STR frame-header field offsets, relative to the sector start (user +0 = +8). */
#define STR_OFF_MAGIC     (STR_SUBHDR + 0x00)
#define STR_OFF_CUR       (STR_SUBHDR + 0x04)
#define STR_OFF_CNT       (STR_SUBHDR + 0x06)
#define STR_OFF_WIDTH     (STR_SUBHDR + 0x10)
#define STR_OFF_HEIGHT    (STR_SUBHDR + 0x12)

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

int re15_str_open(re15_str_t *str, const uint8_t *data, size_t size)
{
    if (!str || !data) return -1;
    memset(str, 0, sizeof(*str));
    str->data = data;
    str->size = size;

    int frames = 0, w = 0, h = 0;
    for (size_t off = 0; off + STR_PAYLOAD_OFF <= size; off += STR_SECTOR) {
        const uint8_t *sec = data + off;
        if (rd16(sec + STR_OFF_MAGIC) != STR_VIDEO_MAGIC) continue;  /* audio/pad */
        if (rd16(sec + STR_OFF_CUR) == 0) {                          /* frame start */
            frames++;
            if (!w) {
                /* byte-true: FUN_8002a2a8.c:62-63 rounds each dim up to the 16px
                 * macroblock grid (w=(raw+8)&0xfff0). For 320x240 this is a no-op. */
                w = (rd16(sec + STR_OFF_WIDTH)  + 8) & 0xfff0;
                h = (rd16(sec + STR_OFF_HEIGHT) + 8) & 0xfff0;
            }
        }
    }
    str->num_frames = frames;
    str->width      = w;
    str->height     = h;
    return (frames > 0 && w > 0 && h > 0) ? 0 : -2;
}

int re15_str_decode(const re15_str_t *str, int frame_index, uint32_t *rgba_out)
{
    if (!str || !rgba_out || frame_index < 0 || frame_index >= str->num_frames)
        return -1;

    /* 1. Locate the frame_index-th frame (its cur==0 sector). */
    size_t start = 0;
    int fi = -1, found = 0;
    for (size_t off = 0; off + STR_PAYLOAD_OFF <= str->size; off += STR_SECTOR) {
        const uint8_t *sec = str->data + off;
        if (rd16(sec + STR_OFF_MAGIC) != STR_VIDEO_MAGIC) continue;
        if (rd16(sec + STR_OFF_CUR) == 0 && ++fi == frame_index) {
            start = off; found = 1; break;
        }
    }
    if (!found) return -2;

    uint16_t cnt = rd16(str->data + start + STR_OFF_CNT);
    if (cnt == 0 || cnt > STR_MAX_FRAME_SEC) return -3;

    /* 2. Reassemble the MDEC bitstream: concatenate each video sector's
     *    2016-byte payload (offset +0x28) in cur order, skipping interleaved
     *    audio sectors, until cnt sectors are collected or the next frame
     *    (cur==0) begins. */
    uint8_t *buf = (uint8_t *) malloc((size_t)cnt * STR_PAYLOAD_LEN);
    if (!buf) return -4;
    size_t blen = 0;
    int got = 0;
    for (size_t off = start; off + STR_PAYLOAD_OFF <= str->size && got < cnt; off += STR_SECTOR) {
        const uint8_t *sec = str->data + off;
        if (rd16(sec + STR_OFF_MAGIC) != STR_VIDEO_MAGIC) continue;   /* skip audio */
        if (got > 0 && rd16(sec + STR_OFF_CUR) == 0) break;           /* next frame */
        size_t avail = STR_PAYLOAD_LEN;
        if (off + STR_PAYLOAD_OFF + avail > str->size)
            avail = str->size - off - STR_PAYLOAD_OFF;
        memcpy(buf + blen, sec + STR_PAYLOAD_OFF, avail);
        blen += avail;
        got++;
    }

    /* 3. Decode via the byte-true BSS MDEC pipeline (identical codec). */
    int rv = -5;
    re15_bss_chunk_t chunk;
    if (re15_bss_parse_chunk(buf, blen, &chunk) && re15_bss_chunk_has_video(&chunk)) {
        size_t   vlc_cap = ((size_t)chunk.run_length_words + 2) * 4;
        int16_t *coeffs  = (int16_t *) malloc(vlc_cap * sizeof(int16_t));
        uint8_t *rgb     = (uint8_t *) malloc((size_t)str->width * str->height * 3);
        if (coeffs && rgb) {
            int vw = re15_bss_vlc_decode(chunk.vlc_payload, chunk.vlc_payload_size,
                                         chunk.run_length_words, chunk.quant,
                                         chunk.version, coeffs, vlc_cap);
            if (vw >= 0 &&
                re15_bss_mdec_decode(coeffs, (size_t)vw, str->width, str->height, rgb) == 0) {
                int n = str->width * str->height;
                for (int i = 0; i < n; i++) {
                    uint8_t r = rgb[i * 3 + 0], g = rgb[i * 3 + 1], b = rgb[i * 3 + 2];
                    rgba_out[i] = ((uint32_t)r << 24) | ((uint32_t)g << 16)
                                | ((uint32_t)b <<  8) | 0xFFu;
                }
                rv = 0;
            }
        }
        free(coeffs);
        free(rgb);
    }
    free(buf);
    return rv;
}
