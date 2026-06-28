/*
 * RE1.5 Rebuilt — BSS chunk parser (Phase 4.5.6.1, 2026-05-18).
 *
 * Pure C, target-agnostic. Used by both PSX (libpsxpress hardware MDEC
 * path) and PC (software IDCT path). This file only parses the 8-byte
 * chunk header; VLC decoding is bss_vlc.c (Phase 4.5.6.2), IDCT is
 * either PSX hardware or pc-software (Phase 4.5.6.3/4).
 */

#include <stddef.h>
#include <stdint.h>
#include "re15_bss_mdec.h"

/* Read a little-endian uint16 from a byte buffer. We avoid casting the
 * pointer to (uint16_t *) directly because MIPS R3000 traps on unaligned
 * 16-bit loads, and a BSS chunk's payload offset (8) is even, but a
 * future caller might hand us a misaligned pointer. */
static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

int re15_bss_parse_chunk(const uint8_t *buf, size_t size,
                         re15_bss_chunk_t *out_chunk)
{
    if (buf == NULL || out_chunk == NULL) return 0;
    if (size < RE15_BSS_HEADER_SIZE)      return 0;

    out_chunk->run_length_words = read_u16_le(buf + 0);
    out_chunk->id               = read_u16_le(buf + 2);
    out_chunk->quant            = read_u16_le(buf + 4);
    out_chunk->version          = read_u16_le(buf + 6);
    out_chunk->vlc_payload      = buf + RE15_BSS_HEADER_SIZE;
    out_chunk->vlc_payload_size = size - RE15_BSS_HEADER_SIZE;
    return 1;
}

int re15_bss_chunk_has_video(const re15_bss_chunk_t *chunk)
{
    return chunk && chunk->id == RE15_BSS_VLC_ID;
}
