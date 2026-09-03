/*
 * RE1.5 Rebuilt — SLD-Dekompressor + Trailer-Auswertung (plattformunabhaengig).
 *
 * 1:1-Port von FUN_800c47e8 (BIN/DEBUG.BIN Datei-Offset 0x47E8, beim Boot als
 * Datei-Index 7 nach 0x800C0000 geladen). Die vollstaendige Disassembly steht in
 * analysis/esp_masken_2026-09-03/B_sld_vram.md §2; die tragenden Instruktionen
 * sind unten an jeder Verzweigung zitiert.
 *
 * Aufruf-Signatur im Original (aus FUN_80021bbc @0x80021db8 `jal FUN_800c47e8`):
 *   a0 = Zeiger auf das erste Stream-Byte, a1 = entpackte Groesse, a2 = Ziel.
 *
 * Registerrollen der Vorlage: t3 = src-Index, t0 = dst-Index, t4 = flagMask,
 * t5 = flagByte, a3 = aktuelles Byte, t1 = len, t2 = offset.
 *
 * Die Bereichswachen (E_SRC_EOF / E_BACKREF / E_DST_OVF) hat das Original NICHT —
 * es liest bzw. schreibt ungeprueft. Sie sind reine Speicherschutz-Wachen fuer
 * korrupte Dateien und loesen bei keinem der 359 echten Bloecke aus (gemessen
 * ueber alle 6 Stages), aendern also kein Verhalten.
 */
#include "re15_sld.h"

/* Cut-Laengentabelle als erzeugte Kopie der STAGE-Overlays. WARUM eine Kopie: der
 * Trailer braucht nur 2 Byte je (Raum, Cut), die stehen aber in einer bis zu 137 KB
 * grossen Overlay-Datei. Auf dem PC ist das egal (dort wird die Datei gelesen), auf
 * der PSX waere ein 137-KB-CD-Zugriff je Stage-Wechsel fuer 2 Byte nicht vertretbar.
 * Die Kopie ist 120 Zeilen = gut 4 KB Nutzdaten und durch unit_sld_atlas gegen Drift
 * abgesichert (der Test vergleicht sie gegen die Originaldateien). */
#include "gen/sld_lentab.inc"

/* Datei-Offset der Cut-Laengentabelle je Stage. Disasm-belegt (re15_sld.h Kopf):
 * das Stage-Overlay schreibt den Tabellenzeiger nach DAT_800b52c8, Overlay-
 * Ladebasis 0x80100000 ohne 0x800-Header. */
uint32_t re15_sld_table_file_offset(int stage)
{
    switch (stage) {
        case 1: return 0x1EAE4u;   /* @0x8011e97c, v0 = 0x8011EAE4 */
        case 2: return 0x1713Cu;   /* @0x80116fd4, v0 = 0x8011713C */
        case 3: return 0x1D0F8u;   /* @0x8011cfa0, v0 = 0x8011D0F8 */
        case 4: return 0x18590u;   /* @0x80118458, v0 = 0x80118590 */
        case 5: return 0x1DF18u;   /* @0x8011ddc8, v0 = 0x8011DF18 */
        case 6: return 0x01E3Cu;   /* @0x80101e0c, v0 = 0x80101E3C */
        default: return 0u;
    }
}

int re15_sld_used_len(const uint8_t *stage_bin, int stage_bin_size, int stage,
                      int room_index, int cut, uint16_t *out_len)
{
    uint32_t base, off;

    if (!stage_bin || !out_len || stage_bin_size <= 0) return RE15_SLD_E_ARG;
    if (room_index < 0 || cut < 0 || cut >= RE15_SLD_TBL_COLS) return RE15_SLD_E_ARG;

    base = re15_sld_table_file_offset(stage);
    if (base == 0u) return RE15_SLD_E_NOTABLE;

    /* raum*0x20 (`sll v0,v0,5` @0x80021d50) + cut*2 (`sll v1,v1,1` @0x80021d4c) */
    off = base + (uint32_t)room_index * RE15_SLD_TBL_ROW_STRIDE + (uint32_t)cut * 2u;
    if (off + 2u > (uint32_t)stage_bin_size) return RE15_SLD_E_NOTABLE;

    /* `lhu v0,0(v1)` @0x80021d5c — little endian */
    *out_len = (uint16_t)(stage_bin[off] | ((uint16_t)stage_bin[off + 1] << 8));
    return RE15_SLD_OK;
}

int re15_sld_decode(const uint8_t *src, int src_size, int src_pos,
                    uint8_t *dst, int dst_size)
{
    int sp = src_pos;   /* t3 */
    int dp = 0;         /* t0 */
    int mask = 0;       /* t4 */
    int flag = 0;       /* t5 */

    if (!src || !dst || src_size < 0 || src_pos < 0) return RE15_SLD_E_ARG;
    /* `blez a1,0x800c48f0` @0x800c47f0 — Groesse <= 0: sofort zurueck, Ziel unberuehrt. */
    if (dst_size <= 0) return sp;

    /* `slt v0,t0,a1` @0x800c48e4 / `bne v0,zero,LOOP` @0x800c48e8 */
    while (dp < dst_size) {
        int a;

        /* `bne t4,zero,0x800c4810` @0x800c47f8 — nur wenn die Maske leer ist,
         * wird ein neues Flagbyte geholt (`ori t4,zero,0x80` @0x800c4800). */
        if (mask == 0) {
            if (sp >= src_size) return RE15_SLD_E_SRC_EOF;
            flag = src[sp++];
            mask = 0x80;
        }

        if (sp >= src_size) return RE15_SLD_E_SRC_EOF;
        a = src[sp++];                       /* `lbu a3,0(v0)` @0x800c4810 */

        /* `andi v0,a3,0x80` @0x800c4818 / `bne v0,zero,0x800c4834` @0x800c481c */
        if ((a & 0x80) == 0) {
            /* ROH-LITERAL. Der Sprung @0x800c482c geht auf 0x800c48e4, also HINTER
             * jedes `srl t4,t4,1` — hier wird KEIN Flagbit verbraucht. */
            if (dp >= dst_size) return RE15_SLD_E_DST_OVF;
            dst[dp++] = (uint8_t)a;
            continue;
        }

        /* `and v0,t5,t4` @0x800c4834 / `bne v0,zero,0x800c4854` @0x800c4838 */
        if ((flag & mask) == 0) {
            /* GEFLAGGTES LITERAL (Bit 7 bleibt erhalten), `srl t4,t4,1` @0x800c4850 */
            if (dp >= dst_size) return RE15_SLD_E_DST_OVF;
            dst[dp++] = (uint8_t)a;
            mask >>= 1;
            continue;
        }

        /* --- RUECKVERWEIS --- */
        {
            int b2, len, off, base, k;

            if (sp >= src_size) return RE15_SLD_E_SRC_EOF;
            b2 = src[sp++];                  /* `lbu a3,0(v0)` @0x800c4858 */

            /* `sll v0,a3,16` @0x800c4860 / `or v0,v0,v1` @0x800c4864 mit
             * v1 = a<<24 (@0x800c483c) / `sra t2,v0,20` @0x800c4868
             * => off12 = signext12( (a<<4) | (b2>>4) ) */
            off = (((a << 24) | (b2 << 16)) >> 20);   /* arithmetisch, wie sra */

            /* `andi v0,a3,0xf` @0x800c486c / `bne v0,zero,0x800c48a4` @0x800c4870 */
            if ((b2 & 0x0F) != 0) {
                len = (b2 & 0x0F) + 2;       /* `addiu t1,v0,2` @0x800c48a4 */
            } else {
                int b3;
                if (sp >= src_size) return RE15_SLD_E_SRC_EOF;
                b3 = src[sp++];              /* `lbu a3,0(v0)` @0x800c487c */
                len = (b3 & 0x3F) + 3;       /* `addiu t1,v1,3` @0x800c4888 */
                /* `sll v1,t2,2` @0x800c4890 / `or` @0x800c4894 /
                 * `sll v0,v0,16` @0x800c4898 / `sra t2,v0,16` @0x800c48a0
                 * => off = signext16( (off12<<2) | (b3>>6) ) */
                off = (int)((int16_t)(uint16_t)(((off << 2) | (b3 >> 6)) & 0xFFFF));
            }

            mask >>= 1;                      /* `srl t4,t4,1` @0x800c48a8 */

            base = dp + off;                 /* `addu v0,t0,t2` @0x800c48ac (off < 0) */
            if (base < 0)                 return RE15_SLD_E_BACKREF;
            if (dp + len > dst_size)      return RE15_SLD_E_DST_OVF;

            /* `lbu v0,0(a3)` / `sb v0,0(t0)` @0x800c48c4-d8 — BYTEWEISE vorwaerts;
             * Ueberlappung (base + k >= dp) ist ausdruecklich erlaubt und wird vom
             * Format genutzt (Lauflaengen-Effekt). Kein memcpy/memmove! */
            for (k = 0; k < len; k++)
                dst[dp + k] = dst[base + k];
            dp += len;                       /* `addu t0,t2,zero` @0x800c48dc */
        }
    }
    return sp;
}

int re15_sld_atlas_from_chunk(const uint8_t *chunk, int chunk_size,
                              uint16_t used_len,
                              uint8_t *out, int out_cap, int *out_len)
{
    uint32_t present, sld_off, unpacked;
    int      L = (int)used_len;
    int      rv;

    if (!chunk || !out || !out_len || chunk_size <= 0 || out_cap <= 0)
        return RE15_SLD_E_ARG;
    *out_len = 0;

    /* Das Original rechnet `at = 0x80190000 + L` (@0x80021d68) und greift auf
     * -4 / -8 zu; L muss also mindestens 8 sein und in den Chunk passen. */
    if (L < 8 || L > chunk_size) return RE15_SLD_E_USEDLEN;

    /* `lw v0,-4(at)` @0x80021d6c / `beq v0,zero,0x80021df8` @0x80021d74 —
     * der Test ist "!= 0", nicht "== 1". */
    present = (uint32_t)chunk[L - 4]        | ((uint32_t)chunk[L - 3] << 8)
            | ((uint32_t)chunk[L - 2] << 16)| ((uint32_t)chunk[L - 1] << 24);
    if (present == 0u) return RE15_SLD_NO_FOREGROUND;

    /* `lw s0,-8(at)` @0x80021da4 */
    sld_off = (uint32_t)chunk[L - 8]        | ((uint32_t)chunk[L - 7] << 8)
            | ((uint32_t)chunk[L - 6] << 16)| ((uint32_t)chunk[L - 5] << 24);
    if (sld_off + 4u > (uint32_t)chunk_size) return RE15_SLD_E_OFFSET;

    /* Der Block beginnt mit seiner entpackten Groesse (LE32), danach der
     * Flagbyte-Strom — so ruft das Original FUN_800c47e8(a0=blk+4, a1=size). */
    unpacked = (uint32_t)chunk[sld_off]         | ((uint32_t)chunk[sld_off + 1] << 8)
             | ((uint32_t)chunk[sld_off + 2] << 16) | ((uint32_t)chunk[sld_off + 3] << 24);
    if (unpacked == 0u || unpacked > (uint32_t)out_cap) return RE15_SLD_E_SIZE;

    rv = re15_sld_decode(chunk, chunk_size, (int)sld_off + 4, out, (int)unpacked);
    if (rv < 0) return rv;

    *out_len = (int)unpacked;
    return RE15_SLD_OK;
}

int re15_sld_used_len_tab(int stage, int room_index, int cut, uint16_t *out_len)
{
    int i;
    if (!out_len || cut < 0 || cut >= RE15_SLD_TBL_COLS) return RE15_SLD_E_ARG;
    for (i = 0; i < RE15_SLD_LENTAB_COUNT; i++) {
        if (re15_sld_lentab[i].stage == (unsigned char)stage &&
            re15_sld_lentab[i].room  == (unsigned char)room_index) {
            *out_len = re15_sld_lentab[i].len[cut];
            return RE15_SLD_OK;
        }
    }
    return RE15_SLD_E_NOTABLE;
}
