/*
 * RE1.5 Rebuilt — VAB parser + ADPCM decoder (Phase 4.6.2, 2026-05-18).
 *
 * Pure C, target-agnostic. Used by both PSX (re15_audio_psx upload path)
 * and PC (re15_audio_pc software mixer). Ports the Java VabFile +
 * PsxAdpcmDecoder; see those for the algorithmic origin (revengi /
 * Sony libsd documentation).
 *
 * VH layout (offset 0):
 *   +0  u32   form ("pBAV" = 0x70424156)
 *   +4  u32   version
 *   +8  u32   bank id
 *   +12 u32   total bank size (VH + VB)
 *   +16 u16   reserved
 *   +18 u16   program count
 *   +20 u16   total tone count
 *   +22 u16   vag (sample) count
 *   +24 u8    master volume
 *   +25 u8    master pan
 *   +26..31    reserved/attr
 *   +32        program table (128 × 16 bytes)
 *   +2080      tone table (program_count × 16 × 32 bytes)
 *   +(2080 + prog_count*512)  VAG size table (256 × u16, sizes in units of 8 bytes)
 *
 * The VAG size table maps logical VAG index → byte size in the body.
 * A zero entry means "no sample at this index". The samples appear in
 * the VB body in order (skipping zero-sized entries).
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "re15_vab.h"

/* ===== little-endian helpers ============================================ */

static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] <<  8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

/* ===== VAB header parser ================================================ */

int re15_vab_parse(const uint8_t *vh_data, size_t vh_size, re15_vab_t *out)
{
    if (vh_data == NULL || out == NULL)       return -4;
    if (vh_size < RE15_VAB_HEADER_SIZE)        return -1;

    memset(out, 0, sizeof(*out));

    out->form = read_u32_le(vh_data + 0);
    if (out->form != RE15_VAB_SIGNATURE)       return -2;

    out->version       = read_u32_le(vh_data + 4);
    out->bank_id       = read_u32_le(vh_data + 8);
    out->file_size     = read_u32_le(vh_data + 12);
    out->program_count = (int)read_u16_le(vh_data + 18);
    /* Clamp to the tone-table capacity (tones[PROGRAM_COUNT*TONES_PER_PROGRAM]).
     * A VH claiming > 128 programs would overflow tones[] in the loop below. */
    if (out->program_count < 0) out->program_count = 0;
    if (out->program_count > RE15_VAB_PROGRAM_COUNT) out->program_count = RE15_VAB_PROGRAM_COUNT;
    out->tone_count    = (int)read_u16_le(vh_data + 20);
    out->vag_count     = (int)read_u16_le(vh_data + 22);
    out->master_volume = vh_data[24];
    out->master_pan    = vh_data[25];

    /* Clamp program_count against what's actually present in the file,
     * mirroring the Java parser's defensive trim. The tone table follows
     * the program table; we figure out how many program tone segments
     * fit and the VAG size table starts immediately after. */
    int program_table_size = RE15_VAB_PROGRAM_COUNT * RE15_VAB_PROGRAM_ENTRY_SIZE;
    int tone_segment_size  = RE15_VAB_TONES_PER_PROGRAM * RE15_VAB_TONE_ENTRY_SIZE;
    int tone_base          = RE15_VAB_HEADER_SIZE + program_table_size;

    if ((size_t)tone_base > vh_size)           return -3;

    int available_tone_bytes = (int)(vh_size - (size_t)tone_base);
    int max_tone_segments    = available_tone_bytes / tone_segment_size;
    int tone_segments        = (out->program_count < max_tone_segments)
                                ? out->program_count : max_tone_segments;
    if (tone_segments < 0) tone_segments = 0;

    int size_table_offset = tone_base + tone_segments * tone_segment_size;
    if ((size_t)(size_table_offset + RE15_VAB_MAX_SAMPLES * 2) > vh_size) {
        /* Clamp to available bytes — partial size tables happen on the
         * last segment of some banks. */
        return -3;
    }

    /* Walk the VAG size table and compute per-sample offsets in the VB body.
     * Entries are u16 word-counts where 1 word = 8 bytes (PSX ADPCM frame
     * pair: 8 bytes = half an ADPCM frame? actually a sub-frame; Sony
     * stores in 8-byte units historically). Multiply by 8 for byte size. */
    int produced = 0;
    uint32_t running_offset = 0;
    int expected_count = (out->vag_count > 0 &&
                          out->vag_count <= RE15_VAB_MAX_SAMPLES)
                          ? out->vag_count : 0;

    for (int i = 0; i < RE15_VAB_MAX_SAMPLES; i++) {
        uint16_t words = read_u16_le(vh_data + size_table_offset + i * 2);
        uint32_t bytes = (uint32_t)words << 3;     /* * 8 */
        if (bytes == 0) continue;

        out->samples[produced].offset = running_offset;
        out->samples[produced].size   = bytes;
        running_offset += bytes;
        produced++;

        if (expected_count > 0 && produced >= expected_count) break;
        if (produced >= RE15_VAB_MAX_SAMPLES) break;
    }

    out->vag_count       = produced;
    out->vb_total_bytes  = (int)running_offset;

    /* BO-round 2026-05-29: parse the per-tone attributes (32-byte entries) —
     * the canonical source of pitch (center_note/shift) + SPU ADSR, replacing
     * audio_psx.c's hardcoded 22050 Hz + dummy ADSR. */
    for (int p = 0; p < tone_segments; p++) {
        for (int t = 0; t < RE15_VAB_TONES_PER_PROGRAM; t++) {
            int off = tone_base + p * tone_segment_size + t * RE15_VAB_TONE_ENTRY_SIZE;
            if ((size_t)(off + RE15_VAB_TONE_ENTRY_SIZE) > vh_size) break;
            re15_vab_tone_t *tn = &out->tones[p * RE15_VAB_TONES_PER_PROGRAM + t];
            tn->vol         = vh_data[off + 2];   /* +1 is `mode`; vol is +2 (SsUtGetVagAtr.c) */
            tn->pan         = vh_data[off + 3];
            tn->center_note = vh_data[off + 4];
            tn->pitch_shift = vh_data[off + 5];
            tn->min_note    = vh_data[off + 6];
            tn->max_note    = vh_data[off + 7];
            tn->adsr1       = read_u16_le(vh_data + off + 0x10);
            tn->adsr2       = read_u16_le(vh_data + off + 0x12);
            tn->vag_index   = read_u16_le(vh_data + off + 0x16);
        }
    }
    out->tones_loaded = (tone_segments > 0) ? 1 : 0;
    return 0;
}

/* ===== footstep EDT resolver (byte-true FUN_80045630 L50-57) ============= */
/* Map a floor.flr sound_type to a 0-based sample index in `vab` (snd0), via
 * the room's 128-byte EDT table (snd0.edt, DAT_800ac778+8 in the original).
 *
 *   iVar8 = EDT + sound_type*4               (L50)
 *   bank  = *(iVar8+1) & 0x7f                 (L53 — snd0 = bank 0)
 *   tone  = *(iVar8+2) >> 4                   (L55 — tone index in the VAB)
 * The tone descriptor's vag field (1-based, program 0) selects the VAG; our
 * samples[] is the compacted non-zero VAG list, and the raw VAG table's index 0
 * is always the reserved zero entry, so raw 1-based N → samples[N-1].
 *
 * Returns the 0-based sample index, or -1 if the EDT slot is empty / the tone
 * lives in another bank (snd1, not loaded for the footstep path). */
int re15_footstep_vag(const uint8_t *edt, const re15_vab_t *vab, int sound_type)
{
    if (edt == NULL || vab == NULL) return -1;
    if (sound_type < 0 || sound_type > 0x7f) return -1;

    const uint8_t *e = edt + (size_t)sound_type * 4;
    if (e[2] == 0 && e[3] == 0) return -1;        /* empty EDT slot          */

    /* e[1]&0x7f ist ein PROGRAMM-Index in DEMSELBEN VAB, KEIN Bank-Selektor:
     * FUN_80045630 @0x800457dc `sll v1,s5,0x9` = prog*0x200 (= 16 Tones*32B) +
     * 0x820 Header in die Tone-Tabelle. [BYTE_TRUE_AUDIT #37] */
    int prog = e[1] & 0x7f;
    if (prog >= RE15_VAB_PROGRAM_COUNT) return -1;

    int tone = e[2] >> 4;
    if (tone < 0 || tone >= RE15_VAB_TONES_PER_PROGRAM) return -1;

    int vag1 = vab->tones[prog * RE15_VAB_TONES_PER_PROGRAM + tone].vag_index;  /* program prog, 1-based */
    if (vag1 <= 0 || vag1 > vab->vag_count) return -1;
    return vag1 - 1;
}

/* PSX note2pitch LUT (DAT_80077520) — one octave of 2^(x/12) in Q12 (0x1000=1.0×),
 * 12 semitones × 16 fine steps = 192 entries. Byte-extracted from the binary. */
static const uint16_t s_pitch_lut[12][16] = {
    { 0x1000, 0x100E, 0x101D, 0x102C, 0x103B, 0x104A, 0x1059, 0x1068, 0x1078, 0x1087, 0x1096, 0x10A5, 0x10B5, 0x10C4, 0x10D4, 0x10E3 },
    { 0x10F3, 0x1103, 0x1113, 0x1122, 0x1132, 0x1142, 0x1152, 0x1162, 0x1172, 0x1182, 0x1193, 0x11A3, 0x11B3, 0x11C4, 0x11D4, 0x11E5 },
    { 0x11F5, 0x1206, 0x1216, 0x1227, 0x1238, 0x1249, 0x125A, 0x126B, 0x127C, 0x128D, 0x129E, 0x12AF, 0x12C1, 0x12D2, 0x12E3, 0x12F5 },
    { 0x1306, 0x1318, 0x132A, 0x133C, 0x134D, 0x135F, 0x1371, 0x1383, 0x1395, 0x13A7, 0x13BA, 0x13CC, 0x13DE, 0x13F1, 0x1403, 0x1416 },
    { 0x1428, 0x143B, 0x144E, 0x1460, 0x1473, 0x1486, 0x1499, 0x14AC, 0x14BF, 0x14D3, 0x14E6, 0x14F9, 0x150D, 0x1520, 0x1534, 0x1547 },
    { 0x155B, 0x156F, 0x1583, 0x1597, 0x15AB, 0x15BF, 0x15D3, 0x15E7, 0x15FB, 0x1610, 0x1624, 0x1638, 0x164D, 0x1662, 0x1676, 0x168B },
    { 0x16A0, 0x16B5, 0x16CA, 0x16DF, 0x16F4, 0x170A, 0x171F, 0x1734, 0x174A, 0x175F, 0x1775, 0x178B, 0x17A1, 0x17B6, 0x17CC, 0x17E2 },
    { 0x17F9, 0x180F, 0x1825, 0x183B, 0x1852, 0x1868, 0x187F, 0x1896, 0x18AC, 0x18C3, 0x18DA, 0x18F1, 0x1908, 0x191F, 0x1937, 0x194E },
    { 0x1965, 0x197D, 0x1995, 0x19AC, 0x19C4, 0x19DC, 0x19F4, 0x1A0C, 0x1A24, 0x1A3C, 0x1A55, 0x1A6D, 0x1A85, 0x1A9E, 0x1AB7, 0x1ACF },
    { 0x1AE8, 0x1B01, 0x1B1A, 0x1B33, 0x1B4C, 0x1B66, 0x1B7F, 0x1B98, 0x1BB2, 0x1BCC, 0x1BE5, 0x1BFF, 0x1C19, 0x1C33, 0x1C4D, 0x1C67 },
    { 0x1C82, 0x1C9C, 0x1CB7, 0x1CD1, 0x1CEC, 0x1D07, 0x1D22, 0x1D3D, 0x1D58, 0x1D73, 0x1D8E, 0x1DA9, 0x1DC5, 0x1DE0, 0x1DFC, 0x1E18 },
    { 0x1E34, 0x1E50, 0x1E6C, 0x1E88, 0x1EA4, 0x1EC1, 0x1EDD, 0x1EFA, 0x1F16, 0x1F33, 0x1F50, 0x1F6D, 0x1F8A, 0x1FA7, 0x1FC5, 0x1FE2 },
};

uint16_t re15_vab_note2pitch(int midi_note, int center_note, int pitch_shift)
{
    /* 16 fine steps per semitone; pitch_shift (0..127) → 0..15 fine index. */
    int fine16 = (pitch_shift * 16) / 128;
    int total  = (midi_note - center_note) * 16 + fine16;  /* 1/16-semitone steps */
    int octave = total / 192;
    int idx    = total - octave * 192;
    if (idx < 0) { idx += 192; octave -= 1; }              /* floor for negatives */
    uint32_t base = s_pitch_lut[idx / 16][idx % 16];       /* Q12 within one octave */
    uint32_t p    = (octave >= 0) ? (base << octave) : (base >> (-octave));
    if (p > 0x3FFF) p = 0x3FFF;                            /* SPU_CH_FREQ ceiling */
    return (uint16_t)p;
}

const re15_vab_tone_t *re15_vab_find_tone(const re15_vab_t *vab, int program, int note)
{
    if (!vab || !vab->tones_loaded) return NULL;
    if (program < 0 || program >= RE15_VAB_PROGRAM_COUNT) return NULL;
    for (int t = 0; t < RE15_VAB_TONES_PER_PROGRAM; t++) {
        const re15_vab_tone_t *tn = &vab->tones[program * RE15_VAB_TONES_PER_PROGRAM + t];
        if (tn->vag_index == 0) continue;                  /* empty slot */
        if (note >= tn->min_note && note <= tn->max_note) return tn;
    }
    return NULL;
}

/* ===== PSX ADPCM (VAG) decoder ========================================= */

/* Predictor filter coefficients — pairs of (coef0, coef1) for the 4 filter
 * modes used by PSX VAG. Filter index 0 = no prediction. Coefficients are
 * Q6 fixed-point; the predictor adds `(state1*c0 + state2*c1 + 32) >> 6`.
 *
 * These are documented in psx-spx (SPU chapter) and match Sony's libspu
 * VAG decoder + revengi's depack_adpcm.c. */
static const int8_t s_filter_coef0[5] = {  0, 60, 115,  98, 122 };
static const int8_t s_filter_coef1[5] = {  0,  0, -52, -55, -60 };

static int sat16(int v)
{
    if (v < -32768) return -32768;
    if (v >  32767) return  32767;
    return v;
}

int re15_vag_adpcm_decode(const uint8_t *adpcm_data, size_t adpcm_size,
                          int16_t *pcm_out, size_t pcm_capacity)
{
    if (adpcm_data == NULL || pcm_out == NULL) return 0;
    if (adpcm_size < 16)                       return 0;

    size_t frames = adpcm_size / 16;
    int    state1 = 0;
    int    state2 = 0;
    size_t pcm_pos = 0;

    for (size_t f = 0; f < frames; f++) {
        const uint8_t *frame = adpcm_data + f * 16;
        int header = frame[0];
        /* frame[1] (Flags) wird nicht mehr gelesen — Loop-Start-Bit ist kein Reset [#20] */

        int filter = (header >> 4) & 0x0F;
        int shift  = header & 0x0F;
        if (filter > 4) filter = 4;
        int c0 = s_filter_coef0[filter];
        int c1 = s_filter_coef1[filter];

        for (int i = 0; i < 14; i++) {
            int byte = frame[2 + i];

            /* Each byte contains two 4-bit samples: low nibble first. */
            for (int half = 0; half < 2; half++) {
                int nibble = (half == 0) ? (byte & 0x0F) : (byte >> 4);

                /* Sign-extend the 4-bit value via the trick: bit 3 set → subtract 16. */
                int sample = nibble;
                if (sample & 0x08) sample -= 16;

                int value  = sample << 12;
                value    >>= shift;
                value     += (state1 * c0 + state2 * c1 + 32) >> 6;
                value      = sat16(value);

                state2 = state1;
                state1 = value;

                if (pcm_pos >= pcm_capacity) return (int)pcm_pos;
                pcm_out[pcm_pos++] = (int16_t)value;
            }
        }

        /* byte-true: Flag-Bit 0x04 (Loop Start) ist NUR eine Adress-Kopie ins Repeat-
         * Register (psx-spx soundprocessingunitspu: "Copy current address"; pcsx-redux
         * spu.cc:618 setzt nur pLoop) — KEIN Predictor-Reset. state1/state2 laufen
         * kontinuierlich ueber die Block-Grenze; der fruehere Reset erzeugte eine
         * falsche ADPCM-Diskontinuitaet. [#20] */
    }

    return (int)pcm_pos;
}
