/*
 * RE1.5 Rebuilt — VAB (PSX audio bank) parser + ADPCM decoder
 * (Phase 4.6.2, 2026-05-18).
 *
 * VAB = "VAB" Sony audio container. Two files per bank:
 *   .VH (header)  — 32-byte main header + 128-entry program table +
 *                   per-program tone tables + 256-entry VAG size table
 *   .VB (body)    — concatenated ADPCM sample data, each sample sized
 *                   per the VH size table
 *
 * For the engine in 4.6.2 we expose:
 *   1. Parse the VH and extract VAG size + offset table.
 *   2. Software ADPCM → 16-bit PCM decode (PC only; on PSX the hardware
 *      SPU plays ADPCM directly without decode).
 *
 * Phase 4.6.3 will add:
 *   - PSX: SpuWrite to upload VB body to SPU RAM, voice key-on per sample.
 *   - PC: enqueue decoded PCM into the SDL audio mixer.
 *
 * Tone/program-graph parsing is deferred — SCD's Se_on opcodes pass
 * (bank, sample_id) tuples and that's enough to look up VAG by index.
 */
#ifndef RE15_VAB_H
#define RE15_VAB_H

#include <stdint.h>
#include <stddef.h>

/* "VABp" little-endian — file bytes are 0x70 ('p'), 0x42 ('B'),
 * 0x41 ('A'), 0x56 ('V') which a LE u32 read interprets as 0x56414270. */
#define RE15_VAB_SIGNATURE   0x56414270u
#define RE15_VAB_HEADER_SIZE 32
#define RE15_VAB_PROGRAM_COUNT      128
#define RE15_VAB_PROGRAM_ENTRY_SIZE 16
#define RE15_VAB_TONE_ENTRY_SIZE    32
#define RE15_VAB_TONES_PER_PROGRAM  16
#define RE15_VAB_MAX_SAMPLES        256

/* VAG (compressed sample) entry within the bank's VB body. */
typedef struct {
    uint32_t offset;       /* byte offset into VB body */
    uint32_t size;         /* size in bytes (VH stores as units of 8) */
} re15_vab_sample_t;

/* BO-round 2026-05-29: per-tone attributes from the 32-byte VH tone entry.
 * Source of canonical pitch + ADSR (replaces audio_psx.c's 22050/dummy hacks). */
typedef struct {
    uint8_t  vol;          /* +2  tone volume 0..127 (SsUtGetVagAtr.c; +1 is `mode`) */
    uint8_t  pan;          /* +3  tone pan 0..127 (64=center)   */
    uint8_t  center_note;  /* +4  MIDI note of the recorded pitch */
    uint8_t  pitch_shift;  /* +5  fine-tune (1/128 semitone)    */
    uint8_t  min_note;     /* +6  lowest MIDI note for this tone */
    uint8_t  max_note;     /* +7  highest MIDI note             */
    uint16_t adsr1;        /* +0x10 SPU ADSR1 register value    */
    uint16_t adsr2;        /* +0x12 SPU ADSR2 register value    */
    uint16_t vag_index;    /* +0x16 1-based VAG index in VB body */
} re15_vab_tone_t;

#define RE15_VAB_TOTAL_TONES (RE15_VAB_PROGRAM_COUNT * RE15_VAB_TONES_PER_PROGRAM) /* 2048 */

/* Parsed VAB header — minimal fields the engine needs. */
typedef struct {
    uint32_t          form;             /* must == RE15_VAB_SIGNATURE   */
    uint32_t          version;
    uint32_t          bank_id;
    uint32_t          file_size;        /* total bank size (VH + VB)    */
    int               program_count;    /* 1..128                       */
    int               tone_count;       /* total tones across programs  */
    int               vag_count;        /* number of actual samples     */
    uint8_t           master_volume;
    uint8_t           master_pan;
    re15_vab_sample_t samples[RE15_VAB_MAX_SAMPLES];
    int               vb_total_bytes;   /* sum of all sample sizes      */
    re15_vab_tone_t   tones[RE15_VAB_TOTAL_TONES]; /* [prog*16 + tone]   */
    int               tones_loaded;     /* 1 if the tone table parsed   */
} re15_vab_t;

/* Canonical SPU pitch from a tone's center-note/shift + the played MIDI note
 * (PSX note2pitch2 + the DAT_80077520 12×16 LUT). Returns the SPU_CH_FREQ value
 * (0x1000 = 1.0×). Replaces the hardcoded 22050 Hz. */
uint16_t re15_vab_note2pitch(int midi_note, int center_note, int pitch_shift);

/* Find the first tone in `program` whose [min_note,max_note] contains `note`.
 * Returns NULL if none / tones not loaded. */
const re15_vab_tone_t *re15_vab_find_tone(const re15_vab_t *vab,
                                          int program, int note);

/* Footstep EDT resolver (byte-true FUN_80045630): map a floor.flr sound_type
 * to a 0-based sample index in `vab` (snd0) via the room's 128-byte EDT table
 * (snd0.edt). Returns -1 if the slot is empty or points at another bank. */
int re15_footstep_vag(const uint8_t *edt, const re15_vab_t *vab, int sound_type);

/* Parse a VH (header) blob. Computes sample offsets + sizes by walking
 * the VAG size table. Returns 0 on success, negative on error:
 *   -1: too small (< 32 bytes)
 *   -2: bad signature (not "pBAV")
 *   -3: corrupt/oversized field
 *   -4: NULL pointer */
int re15_vab_parse(const uint8_t *vh_data, size_t vh_size, re15_vab_t *out);

/* Decode PSX ADPCM (a.k.a. VAG format) data into signed 16-bit PCM.
 * Each ADPCM frame = 16 bytes → 28 PCM samples (mono).
 *
 *   adpcm_data:   input ADPCM bytes (multiple of 16)
 *   adpcm_size:   byte length of input
 *   pcm_out:      output PCM buffer (must have capacity ≥ adpcm_size/16*28)
 *   pcm_capacity: maximum int16 samples to write
 *
 * Returns number of PCM samples written (≥ 0). Used by the PC backend
 * to decode samples ahead-of-time for the SDL mixer; PSX uses the SPU
 * hardware ADPCM decoder and never calls this.
 *
 * Ported from src/main/java/de/re15/extractors/audio/PsxAdpcmDecoder.java
 * — byte-equivalent to the Java reference. */
int re15_vag_adpcm_decode(const uint8_t *adpcm_data, size_t adpcm_size,
                          int16_t *pcm_out, size_t pcm_capacity);

#endif /* RE15_VAB_H */
