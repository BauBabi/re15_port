/* ============================================================================ *
 *  RE1.5 Rebuilt — EMS enemy-model archive index.
 *
 *  The RE1.5 disc ships every enemy model inside two big EMS archives
 *  (EMD/CDEMD0.EMS + CDEMD1.EMS, ~4.7 MB each) instead of per-type EM<NN>.EMD
 *  files. An EMS is a sequence of EMD blobs, each padded to a 2048-byte CD
 *  sector. This module locates one EMD blob inside an EMS by index or by enemy
 *  type so the loader can hand it straight to re15_emd_parse_container() — no
 *  external split step (the Java RE15MasterExtractor.extractEmsArchives does the
 *  same parse to write the individual .emd files; this is its byte-true port).
 * ============================================================================ */
#ifndef RE15_EMS_H
#define RE15_EMS_H

#include <stddef.h>
#include <stdint.h>

#define RE15_EMS_ALIGN      2048   /* CD sector — each EMD blob is sector-padded */
#define RE15_EMS_FOOTER       36   /* EMS_DIRECTORY_FOOTER_SIZE (Java extractor)  */

/* Locate the `index`-th EMD blob inside an EMS archive. Iterates the
 * sector-aligned blocks exactly like RE15MasterExtractor.parseEmsEntries:
 * each block opens with u32 dir_off (the EMD's own section-directory offset),
 * valid iff dir_off >= 36 && (dir_off & 3) == 0; the block length is
 * dir_off + 36; the next block is align_up(cursor+len, 2048) with any 4-byte
 * zero padding skipped and re-aligned. Stops at the first invalid header.
 * On success writes the blob's byte offset + length and returns 0; returns
 * negative if `index` is past the last blob. */
int re15_ems_get_entry(const uint8_t *ems, size_t ems_size, int index,
                       size_t *out_offset, size_t *out_length);

/* The EMS blob order (RE15MasterExtractor.DEFAULT_EMS_ORDER): index i holds
 * EM<type>. Returns the blob index for an enemy `type`, or -1 if `type` is not
 * one of the 25 archived models. */
int re15_ems_index_for_type(uint8_t type);

#endif /* RE15_EMS_H */
