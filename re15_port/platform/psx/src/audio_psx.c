/*
 * RE1.5 Rebuilt — Audio backend, PSX.
 *
 * RE2-architecture audio (Content = RE1.5). Two playback paths on the SPU:
 *
 *   SFX  — Se_on (0x36) → one-shot SPU voice from the bundled DOOR00 VAB.
 *   BGM  — RE2 SsSeq: a SEQ (pQES = PPQN + running-status MIDI) drives
 *          note-on/off; each note plays a VAB tone's VAG on a HARDWARE SPU
 *          voice (the SPU decodes ADPCM, runs the ADSR envelope and the
 *          sustain loop itself — we only key voices + write FREQ/VOL/ADSR).
 *
 * PSn00bSDK has NO libsnd/SsSeq, so the sequencer tick (the equivalent of
 * SsSeqCalledTbyT) is hand-written here. It is driven from a 60 Hz vblank
 * counter (VSyncCallback) — NOT the render frame rate — so BGM tempo stays
 * stable when the intro fps dips. All control logic (SEQ/MIDI parse, the
 * stage/room→track table, seq-slot control) is the integer port of the
 * verified PC reference re15_ss (re15_reborn_pc/src/audio_pc.c); the
 * software mixer/envelope/reverb of that reference is replaced by SPU
 * hardware here.
 *
 * Refs: RE2_Quellcode/_SsInitSoundSeq.c, FUN_8007e474.c (MIDI parser),
 * FUN_8005a97c.c (room-BGM play), vab_common.c (VAB soundfont + note2pitch).
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>         /* sprintf — CD path building (codebase idiom) */
#include <psxspu.h>
#include <psxgpu.h>        /* VSyncCallback (60 Hz tick source) */
#include <psxcd.h>         /* async voice CD read (CdSearchFile/CdRead/CdReadSync) */
#include <hwregs_c.h>      /* SPU_CH_* register macros */
#include "re15_audio.h"
#include "re15_scd.h"
#include "re15_vab.h"
#include "re15_cdfs.h"
#include "re15_room.h"     /* g_room_rdt — footstep snd0 VAB sliced from the room RDT */

re15_audio_state_t g_audio;

/* Bundled SFX VAB (DOOR00.vh + DOOR00.vb via psn00bsdk_target_incbin). */
extern const uint8_t  test_vh[];
extern const uint32_t test_vh_size;
extern const uint8_t  test_vb[];
extern const uint32_t test_vb_size;

/* The room's footstep SE bank (snd0: VH header + VB samples + EDT table, the floor
 * sound_type → VAB bank/tone map, FUN_80045630) is sliced out of the parsed RDT
 * (re15_rdt_parse → snd_vh/vb/edt[0], header offsets 0x08-0x10). Phase 2 (2026-06-13):
 * replaces the room1170_snd0_* incbin — pointers alias the resident RDT buffer,
 * byte-true (verified == the old incbin bytes), and any room's snd0 now works. */

/* ── SPU RAM bump allocator ──────────────────────────────────────────────
 * 512 KB total. 0x0000..0x100F reserved (BIOS / capture / dummy block); we
 * hand out 8-byte-aligned regions above 0x1010 for each bank's VB body. */
#define RE15_SPU_BASE  0x1010
static uint32_t s_spu_top = RE15_SPU_BASE;
static uint32_t spu_alloc(uint32_t bytes) {
    uint32_t a = (s_spu_top + 7u) & ~7u;
    s_spu_top = a + bytes;
    return a;
}

/* Upload a VB body to SPU RAM at a freshly-allocated address; return the base. */
static uint32_t spu_upload_vb(const uint8_t *vb, uint32_t vb_sz) {
    uint32_t base = spu_alloc(vb_sz);
    SpuSetTransferMode    (SPU_TRANSFER_BY_DMA);
    SpuSetTransferStartAddr(base);
    SpuWrite              ((const uint32_t *)vb, (size_t)vb_sz);
    SpuIsTransferCompleted(1);
    return base;
}

/* Stream a (possibly large) VB body from CD DIRECTLY to SPU RAM in staging-sized
 * chunks. Room MAIN banks run to ~224KB — far over the 100KB shared staging — so
 * a whole-file load won't fit. This reads N sectors at a time into staging and
 * DMAs each chunk to consecutive SPU addresses (RAM-neutral, any bank size).
 * Allocates the SPU region; returns its base, *out_size = the file byte size (for
 * the VAB sample offsets). Returns 0 on failure. */
static uint32_t ssx_stream_vb_to_spu(const char *path, uint32_t *out_size) {
    re15_audio_voice_flush();
    CdlFILE fp;
    if (!CdSearchFile(&fp, (char *)path) || fp.size <= 0) { *out_size = 0; return 0; }
    int total_sectors = (fp.size + 2047) >> 11;
    int chunk_sectors = RE15_CD_STAGING_SIZE >> 11;          /* 50 sectors / 100KB */
    int base_lba      = CdPosToInt(&fp.pos);
    uint32_t spu_base = spu_alloc((uint32_t)fp.size);
    uint32_t spu_off  = 0;
    for (int done = 0; done < total_sectors; ) {
        int n = total_sectors - done;
        if (n > chunk_sectors) n = chunk_sectors;
        CdlLOC loc;
        CdIntToPos(base_lba + done, &loc);
        CdControl(CdlSetloc, (const uint8_t *)&loc, 0);
        if (!CdRead(n, (uint32_t *)re15_cd_staging, CdlModeSpeed)) { *out_size = 0; return 0; }
        if (CdReadSync(0, 0) < 0)                                  { *out_size = 0; return 0; }
        uint32_t bytes = (uint32_t)(n << 11);
        SpuSetTransferMode    (SPU_TRANSFER_BY_DMA);
        SpuSetTransferStartAddr(spu_base + spu_off);
        SpuWrite              ((const uint32_t *)re15_cd_staging, bytes);
        SpuIsTransferCompleted(1);
        spu_off += bytes;
        done    += n;
    }
    *out_size = (uint32_t)fp.size;
    return spu_base;
}

/* ── SPU voice partition (24 voices; voice 0 left parked on the dummy block) ─
 *   BGM MAIN : 1..12   (12-voice polyphony for the music track)
 *   BGM SUB  : 13..17  (5-voice — the helicopter-rotor / ambience layer)
 *   SFX      : 18..22  (5-voice round-robin)
 *   VOICE    : 23      (1 reserved channel for CD-streamed dialogue) */
#define BGM_MAIN_CH_BASE  1
#define BGM_MAIN_CH_CNT   12
#define BGM_SUB_CH_BASE   13
#define BGM_SUB_CH_CNT    5
#define SFX_CH_BASE       18
#define SFX_CH_CNT        5
#define VOICE_CH          23

static inline void spu_settle(void) {
    /* ~64 CPU cycles: lets the SPU register a KeyOff before we reprogram the
     * voice (the KeyOff→config→KeyOn race wedged the command latch otherwise). */
    for (volatile int i = 0; i < 64; i++) { /* spin */ }
}

/* ════════════════════════════════════════════════════════════════════════
 *  SFX path (Se_on) — bundled DOOR00 VAB
 * ════════════════════════════════════════════════════════════════════════ */
static re15_vab_t s_sfx_vab;
static int        s_sfx_loaded = 0;
static uint32_t   s_sfx_spu_base = 0;
static int        s_sfx_next = 0;     /* round-robin within SFX_CH range */

/* Room footstep bank (snd0). Separate from the Se_on SFX VAB so it can't be
 * disturbed by door-SFX round-robin; its EDT table maps floor sound_type→tone. */
static re15_vab_t     s_foot_vab;
static int            s_foot_loaded  = 0;
static uint32_t       s_foot_spu_base = 0;
static const uint8_t *s_foot_edt     = NULL;

static void load_bundled_vab(void)
{
    if (re15_vab_parse(test_vh, test_vh_size, &s_sfx_vab) != 0) return;
    s_sfx_spu_base = spu_upload_vb(test_vb, (uint32_t)test_vb_size);
    s_sfx_loaded = 1;
}

static void load_footstep_vab(void)
{
    s_foot_loaded = 0;
    if (!g_room_rdt_ok) return;
    const re15_rdt_t *rdt = &g_room_rdt;
    if (!rdt->snd_vh[0] || !rdt->snd_vb[0] || !rdt->snd_edt[0]) return;   /* room has no snd0 bank */
    if (re15_vab_parse(rdt->snd_vh[0], rdt->snd_vh_size[0], &s_foot_vab) != 0) return;
    s_foot_spu_base = spu_upload_vb(rdt->snd_vb[0], (uint32_t)rdt->snd_vb_size[0]);
    s_foot_edt      = rdt->snd_edt[0];
    s_foot_loaded   = 1;
}

/* Key on one SFX voice with `vag_index` (0-based) from an arbitrary bank
 * (vab+spu_base). Pitch/ADSR come from the VAB tone that owns the VAG. */
static void play_sample_from(const re15_vab_t *vab, uint32_t spu_base,
                             int vag_index, int volume, int pitch_override)
{
    if (!vab || vag_index < 0 || vag_index >= vab->vag_count) return;

    int ch = SFX_CH_BASE + s_sfx_next;
    s_sfx_next = (s_sfx_next + 1) % SFX_CH_CNT;

    int spu_vol = (volume * 0x3FFF / 127);
    if (spu_vol > 0x3FFF) spu_vol = 0x3FFF;

    /* Source the SPU envelope from the VAB tone that owns this VAG. */
    const re15_vab_tone_t *tn = NULL;
    if (vab->tones_loaded) {
        for (int p = 0; p < RE15_VAB_TOTAL_TONES; p++) {
            if (vab->tones[p].vag_index == (uint16_t)(vag_index + 1)) {
                tn = &vab->tones[p];
                break;
            }
        }
    }

    SpuSetKey(0, 1u << ch);
    spu_settle();
    /* SFX have no incoming MIDI note → play at the tone's recorded pitch
     * (center_note maps to 1.0× via note2pitch); fall back to 22050 Hz. A
     * pitch_override (>0) forces a fixed SPU rate — the footstep uses 22050 to
     * match the PC mixer (which plays SE at half the 44100 output via its 2×
     * downsample); note2pitch's 0x1000 base = 44100 made it an octave too high. */
    SPU_CH_FREQ (ch) = pitch_override ? pitch_override
                     : tn ? re15_vab_note2pitch(tn->center_note, tn->center_note,
                                                tn->pitch_shift)
                          : getSPUSampleRate(22050);
    SPU_CH_ADDR (ch) = getSPUAddr(spu_base + vab->samples[vag_index].offset);
    SPU_CH_VOL_L(ch) = (int16_t)spu_vol;
    SPU_CH_VOL_R(ch) = (int16_t)spu_vol;
    SPU_CH_ADSR1(ch) = tn ? tn->adsr1 : 0x00FF;
    SPU_CH_ADSR2(ch) = tn ? tn->adsr2 : 0x0000;
    spu_settle();
    SpuSetKey(1, 1u << ch);
}

static void play_sample(int vag_index, int volume)
{
    if (!s_sfx_loaded) return;
    play_sample_from(&s_sfx_vab, s_sfx_spu_base, vag_index, volume, 0);
}

/* Player FOOTSTEP SE (byte-true FUN_80045630): floor sound_type → EDT → snd0
 * tone → VAG → key a voice. `foot` (7=left/4=right) selects nothing for ROOM1170
 * (both feet resolve to the same VAG; the original only differs the SE voice
 * slot via param_2, which our round-robin already gives). The original scales by
 * the per-tone vol × FUN_80045a64 camera-distance attenuation; the player is at
 * the camera so attenuation≈max → play at the tone's recorded volume. */
void re15_audio_footstep(int foot, int sound_type)
{
    (void)foot;
    if (!s_foot_loaded || !s_foot_edt) return;
    int vag = re15_footstep_vag(s_foot_edt, &s_foot_vab, sound_type);
    if (vag < 0) return;
    int vol = s_foot_vab.tones[s_foot_edt[sound_type * 4 + 2] >> 4].vol;
    /* 22050 Hz = the PC mixer's effective SE rate (its 2× downsample of 44100) →
     * matches the user-correct PC footstep pitch (note2pitch's 44100 base was 2× high). */
    play_sample_from(&s_foot_vab, s_foot_spu_base, vag, vol ? vol : 100,
                     getSPUSampleRate(22050));
}

/* Room SE by id (byte-true FUN_800453d0 core) — the combat/room SE bank (snd1). PSX SPU
 * path is a FOLLOW-UP: it needs the snd1 bank transferred to a reserved SPU region (mirror
 * of the snd0 footstep load) before play_sample_from can key it. Stub keeps the target
 * buildable; the PC backend (audio_pc.c) has the real impl. */
void re15_audio_room_se(int se_id)
{
    (void)se_id;   /* TODO(psx): load snd1 to SPU + play_sample_from like re15_audio_footstep */
}

/* Weapon SE by id (byte-true FUN_80045024 bank1 core) — the equipped weapon's ARMS bank. PSX SPU
 * path is a FOLLOW-UP: mirror FUN_80043d8c (load ARMS%02X.EDH/.VB into a reserved SPU region) before
 * play_sample_from can key it. Stub keeps the target buildable; audio_pc.c has the real impl. */
void re15_audio_weapon_se(int se_id)
{
    (void)se_id;   /* TODO(psx): load the equipped weapon's ARMS bank to SPU + play se_id (gunshot=8) */
}

/* Re-prime the weapon SE bank on equip (byte-true FUN_80043d8c). PSX SPU path = follow-up stub. */
void re15_audio_prime_weapon(int weapon_id)
{
    (void)weapon_id;   /* TODO(psx): FUN_80043d8c parity — load ARMS<weapon>.EDH/.VB into the SPU bank */
}

/* ════════════════════════════════════════════════════════════════════════
 *  VOICE path (RE2 dialogue) — SCD Message_on (0x2B) → SCD_AUDIO_VOICE_ON.
 *  RE2 streams dialogue as CD-XA mixed into the SPU; PSn00bSDK has no XA
 *  encoder, so we mirror the same effect with a per-clip CD-load of a small
 *  PSX-ADPCM (VAG) clip into a reserved SPU region, played on a reserved SPU
 *  channel and mixed with the BGM in hardware. Clips are short (~23-49KB, ~2-4s).
 *  The CD read is ASYNC (CdRead without sync) into a dedicated buffer, polled in
 *  re15_audio_tick → no per-line main-loop hitch. (A coincident BG cut-load on the
 *  shared CD drive can abort an in-flight voice read → that clip is just dropped,
 *  no crash; voice lines and cut-changes rarely overlap.) */
#define RE15_VOICE_SPU_SIZE  0xC800   /* reserved SPU region for the current voice clip (≥ largest VAG) */
static uint32_t s_voice_spu_base = 0;
static unsigned char s_voice_buf[RE15_VOICE_SPU_SIZE] __attribute__((aligned(4)));
static int s_voice_loading = 0;       /* a CdRead is in flight */
static int s_voice_bytes   = 0;       /* logical clip size of the in-flight read */

/* Kick off an ASYNC CD read of the dialogue clip for message id `msg_id`
 * (VOICE<NN>.VAG). Missing/oversized clips are silently skipped. The SPU upload
 * + key-on happens in voice_poll() once the read completes. One clip at a time. */
static void voice_play(int msg_id)
{
    if (s_voice_spu_base == 0 || s_voice_loading) return;
    char path[32];
    sprintf(path, "\\VOICE\\VOICE%02d.VAG;1", msg_id);   /* RE2: \PL0\VOICE\ */
    CdlFILE fp;
    if (!CdSearchFile(&fp, path)) return;
    if (fp.size <= 0 || (uint32_t)fp.size > RE15_VOICE_SPU_SIZE) return;
    int sectors = (fp.size + 2047) >> 11;
    if ((sectors << 11) > (int)sizeof s_voice_buf) return;
    CdControl(CdlSetloc, (const uint8_t *)&fp.pos, 0);
    if (!CdRead(sectors, (uint32_t *)s_voice_buf, CdlModeSpeed)) return;  /* async start */
    s_voice_bytes   = fp.size;
    s_voice_loading = 1;
}

/* Poll the in-flight voice read; on completion, DMA to SPU + key the voice. */
static void voice_poll(void)
{
    if (!s_voice_loading) return;
    int rem = CdReadSync(1, 0);          /* non-blocking: >0 remaining, 0 done, <0 error */
    if (rem > 0) return;                 /* still reading */
    s_voice_loading = 0;
    if (rem < 0) return;                 /* error / aborted (e.g. a BG load preempted) → drop */

    SpuSetTransferMode    (SPU_TRANSFER_BY_DMA);
    SpuSetTransferStartAddr(s_voice_spu_base);
    SpuWrite              ((const uint32_t *)s_voice_buf, (size_t)s_voice_bytes);
    SpuIsTransferCompleted(1);

    /* Key the reserved voice channel (prominent, no envelope; 22050 Hz). */
    SpuSetKey(0, 1u << VOICE_CH);
    spu_settle();
    SPU_CH_FREQ (VOICE_CH) = getSPUSampleRate(22050);
    SPU_CH_ADDR (VOICE_CH) = getSPUAddr(s_voice_spu_base);
    SPU_CH_VOL_L(VOICE_CH) = 0x3FFF;
    SPU_CH_VOL_R(VOICE_CH) = 0x3FFF;
    SPU_CH_ADSR1(VOICE_CH) = 0x00FF;
    SPU_CH_ADSR2(VOICE_CH) = 0x0000;
    spu_settle();
    SpuSetKey(1, 1u << VOICE_CH);
}

/* CD serialization barrier (called from re15_cd_load_file before ANY other CD
 * load): if a voice read is in flight, block until it finishes and upload it,
 * so the new load can't collide with it on the single CD drive. This is what
 * lets the async voice coexist with the cut-change BG streaming (re15_bg_load_*)
 * — without it an in-flight voice read corrupts/aborts the cut's BG load and the
 * camera cut appears not to change. No-op when no voice is loading. */
void re15_audio_voice_flush(void)
{
    if (s_voice_loading) {
        CdReadSync(0, 0);   /* block until the in-flight voice read completes */
        voice_poll();       /* → upload + key (CdReadSync(1,0) now returns 0) */
    }
}

/* ════════════════════════════════════════════════════════════════════════
 *  BGM path — RE2 SsSeq on hardware SPU voices
 * ════════════════════════════════════════════════════════════════════════ */
#define SS_CHANNELS  16          /* MIDI channels */
#define SS_SEQ_HDR   15          /* pQES header bytes before the event stream */
#define SS_VOICE_MAX 12          /* >= max of the two layers' channel counts */

typedef struct {
    uint8_t active;
    uint8_t chan, note;          /* MIDI channel + note owning this SPU voice */
} ssx_voice_t;

typedef struct {
    /* soundfont */
    re15_vab_t  vab;
    int         vab_ok;
    uint32_t    vag_spu_addr[RE15_VAB_MAX_SAMPLES];  /* per-VAG SPU RAM addr (0 = none) */
    /* sequence (SEQ bytes live in a persistent buffer; see s_main_seq/s_sub_seq) */
    const uint8_t *seq;
    int         seq_len;
    int         ppqn;
    uint32_t    tempo_us;
    int         cursor;
    uint8_t     rstatus;
    int         pending_dt;       /* MIDI ticks until the next event           */
    uint32_t    accum_q16;        /* accumulated MIDI ticks, 16.16 fixed point */
    int         playing;
    int         loop;
    /* MIDI channel state */
    uint8_t     prog[SS_CHANNELS];
    int         cvol[SS_CHANNELS]; /* CC7 channel volume 0..127 */
    /* SPU voice pool for this layer */
    int         ch_base, ch_count;
    int         mvol_l, mvol_r;    /* per-layer master, 0..0x3FFF (full-scale ceiling) */
    ssx_voice_t voice[SS_VOICE_MAX];
} ssx_seq_t;

static ssx_seq_t s_bgm_main;       /* MAIN<nn> music */
static ssx_seq_t s_bgm_sub;        /* SUB_<nn> secondary / rotor-ambience layer */

/* Persistent SEQ storage (the sequencer reads these continuously while playing). */
static uint8_t s_main_seq[8192];   /* room MAIN SEQs run to ~5.9KB (MAIN16) */
static uint8_t s_sub_seq [2048];

/* ── per-layer voice management ── */
static void ssx_all_keyoff(ssx_seq_t *s) {
    for (int i = 0; i < s->ch_count; i++) {
        if (s->voice[i].active) {
            SpuSetKey(0, 1u << (s->ch_base + i));
            s->voice[i].active = 0;
        }
    }
}

/* allocate a voice slot: first free, else steal the one with the lowest
 * current SPU envelope level (quietest). */
static int ssx_alloc_voice(ssx_seq_t *s) {
    int best = 0, q = 0x7fffffff;
    for (int i = 0; i < s->ch_count; i++) {
        if (!s->voice[i].active) return i;
        int env = (int)SPU_CH_ADSR_VOL(s->ch_base + i);
        if (env < q) { q = env; best = i; }
    }
    return best;
}

static void ssx_note_on(ssx_seq_t *s, int chan, int note, int vel) {
    const re15_vab_tone_t *t = re15_vab_find_tone(&s->vab, s->prog[chan], note);
    if (!t) return;
    int vi = (int)t->vag_index - 1;                  /* vag_index is 1-based */
    if (vi < 0 || vi >= s->vab.vag_count || s->vag_spu_addr[vi] == 0) return;

    /* volume cascade (7-bit): tone vol × velocity × channel volume, panned,
     * then scaled into the layer's SPU full-scale ceiling. */
    int vol = (int)t->vol * vel / 127;
    vol = vol * s->cvol[chan] / 127;                 /* 0..127 */
    int pan = t->pan; if (pan < 0 || pan > 127) pan = 64;
    int vl, vr;
    if (pan >= 56 && pan <= 72) { vl = vol; vr = vol; }
    else { vl = vol * (127 - pan) / 127; vr = vol * pan / 127; }
    int spu_l = vl * s->mvol_l / 127;                /* 0..mvol_l (<= 0x3FFF) */
    int spu_r = vr * s->mvol_r / 127;

    uint16_t pitch = re15_vab_note2pitch(note, t->center_note, t->pitch_shift);

    int slot = ssx_alloc_voice(s);
    int ch   = s->ch_base + slot;

    SpuSetKey(0, 1u << ch);
    spu_settle();
    SPU_CH_FREQ (ch) = pitch;
    SPU_CH_ADDR (ch) = getSPUAddr(s->vag_spu_addr[vi]);
    SPU_CH_VOL_L(ch) = (int16_t)spu_l;
    SPU_CH_VOL_R(ch) = (int16_t)spu_r;
    SPU_CH_ADSR1(ch) = t->adsr1;                     /* SPU runs the ADSR + loop in HW */
    SPU_CH_ADSR2(ch) = t->adsr2;
    spu_settle();
    SpuSetKey(1, 1u << ch);

    s->voice[slot].active = 1;
    s->voice[slot].chan   = (uint8_t)chan;
    s->voice[slot].note   = (uint8_t)note;
}

static void ssx_note_off(ssx_seq_t *s, int chan, int note) {
    for (int i = 0; i < s->ch_count; i++) {
        if (s->voice[i].active && s->voice[i].chan == chan && s->voice[i].note == note) {
            SpuSetKey(0, 1u << (s->ch_base + i));    /* enter the release phase */
            s->voice[i].active = 0;
        }
    }
}

/* variable-length quantity (MIDI delta time). */
static int ssx_vlq(ssx_seq_t *s) {
    int v = 0;
    while (s->cursor < s->seq_len) {
        uint8_t b = s->seq[s->cursor++];
        v = (v << 7) | (b & 0x7f);
        if (!(b & 0x80)) break;
    }
    return v;
}

/* parse + dispatch one event at the cursor (standard MIDI w/ running status). */
static void ssx_fire_event(ssx_seq_t *s) {
    if (s->cursor >= s->seq_len) { s->playing = 0; return; }
    uint8_t st = s->seq[s->cursor];
    if (st & 0x80) { s->rstatus = st; s->cursor++; } else { st = s->rstatus; }
    int typ = st & 0xf0, ch = st & 0x0f;
    if (typ == 0x90) {                               /* note-on (vel 0 = note-off) */
        int n = s->seq[s->cursor++];
        int vel = s->seq[s->cursor++];
        if (vel) ssx_note_on(s, ch, n, vel); else ssx_note_off(s, ch, n);
    } else if (typ == 0x80) {                        /* note-off */
        int n = s->seq[s->cursor++]; s->cursor++; ssx_note_off(s, ch, n);
    } else if (typ == 0xC0) {                        /* program change */
        s->prog[ch] = s->seq[s->cursor++];
    } else if (typ == 0xD0) {                        /* channel pressure — ignore */
        s->cursor++;
    } else if (typ == 0xE0) {                        /* pitch bend — parsed, not applied (TODO) */
        s->cursor += 2;
    } else if (typ == 0xA0 || typ == 0xB0) {         /* aftertouch / control change */
        int d1 = s->seq[s->cursor++];
        int d2 = s->seq[s->cursor++];
        if (typ == 0xB0 && d1 == 7) s->cvol[ch] = d2;   /* CC7 channel volume */
    } else if (st == 0xFF) {                          /* meta */
        int meta = s->seq[s->cursor++];
        int len  = s->seq[s->cursor++];
        if (meta == 0x51 && len == 3)                /* tempo (u24 BE µs/quarter) */
            s->tempo_us = (uint32_t)((s->seq[s->cursor] << 16) |
                          (s->seq[s->cursor+1] << 8) | s->seq[s->cursor+2]);
        s->cursor += len;
        if (meta == 0x2F) {                          /* end of track */
            if (s->loop) { s->cursor = SS_SEQ_HDR; s->rstatus = 0; ssx_all_keyoff(s); }
            else s->playing = 0;
        }
    } else if (st == 0xF0 || st == 0xF7) {           /* sysex — skip */
        s->cursor += ssx_vlq(s);
    } else { s->playing = 0; }                        /* unknown → stop */
}

/* advance the sequence by `vblanks` 60 Hz ticks of elapsed wall time. */
static void ssx_advance(ssx_seq_t *s, uint32_t vblanks) {
    if (!s->playing || !s->seq || s->tempo_us == 0) return;
    /* MIDI ticks per vblank, 16.16:  ppqn * 1e6 / tempo_us / 60 . */
    uint32_t inc = (uint32_t)(((uint64_t)s->ppqn * 1000000ULL * 65536ULL) /
                              ((uint64_t)s->tempo_us * 60ULL));
    s->accum_q16 += inc * vblanks;
    int guard = 4096;
    /* pending_dt is small for these tracks, so (dt << 16) never overflows. */
    while (s->playing && (s->accum_q16 >> 16) >= (uint32_t)s->pending_dt && guard-- > 0) {
        s->accum_q16 -= (uint32_t)s->pending_dt << 16;
        ssx_fire_event(s);
        s->pending_dt = ssx_vlq(s);
    }
}

/* SsVabOpenHead + SsSeqOpen: bind a parsed VH + an already-SPU-uploaded VB
 * (at `spu_base`) + a SEQ into an instance. SEQ is copied into `seq_store`. */
static int ssx_load(ssx_seq_t *s, int ch_base, int ch_count,
                    const uint8_t *vh, int vh_sz, uint32_t spu_base,
                    const uint8_t *seq, int seq_sz,
                    uint8_t *seq_store, int seq_store_cap)
{
    memset(s, 0, sizeof(*s));
    s->ch_base = ch_base; s->ch_count = ch_count;
    if (s->ch_count > SS_VOICE_MAX) s->ch_count = SS_VOICE_MAX;
    if (re15_vab_parse(vh, (size_t)vh_sz, &s->vab) != 0) return -1;
    for (int i = 0; i < s->vab.vag_count; i++)
        s->vag_spu_addr[i] = spu_base + s->vab.samples[i].offset;
    s->vab_ok = 1;

    if (seq_sz <= SS_SEQ_HDR || seq_sz > seq_store_cap ||
        seq[0] != 'p' || seq[1] != 'Q') return -2;
    memcpy(seq_store, seq, (size_t)seq_sz);
    s->seq = seq_store; s->seq_len = seq_sz;
    s->ppqn     = (seq[8] << 8) | seq[9];
    s->tempo_us = (uint32_t)((seq[10] << 16) | (seq[11] << 8) | seq[12]);
    if (s->ppqn <= 0)     s->ppqn = 48;
    if (s->tempo_us == 0) s->tempo_us = 500000;
    return 0;
}

/* SsSeqPlay(id, 0, loop): (re)start playback from the top. */
static void ssx_start(ssx_seq_t *s, int loop) {
    if (!s->vab_ok || !s->seq) { s->playing = 0; return; }
    ssx_all_keyoff(s);
    for (int i = 0; i < SS_CHANNELS; i++) { s->prog[i] = 0; s->cvol[i] = 100; }
    s->cursor = SS_SEQ_HDR; s->rstatus = 0; s->accum_q16 = 0;
    s->pending_dt = ssx_vlq(s);
    s->loop = loop; s->playing = 1;
}

static void ssx_stop(ssx_seq_t *s) {
    s->playing = 0;
    ssx_all_keyoff(s);
}

/* ── canonical RE1.5 stage/room → BGM mapping (ported from PSX.EXE static data;
 * mirrors re15_reborn_pc audio_pc.c). entry = SS_BGMTBL[room + SS_STAGE_OFF[stage]];
 * MAIN slot = entry & 0x3f; SUB slot = (entry>>8)&0x3f (0xff high byte = no sub).
 * STAGE1/ROOM1170 (stage0,room0x17) → MAIN32 + SUB_15. */
static const uint8_t  SS_STAGE_OFF[6] = {0x00, 0x26, 0x32, 0x41, 0x4d, 0x62};
static const uint16_t SS_BGMTBL[106] = {
    0xffff, 0xff1e, 0xff00, 0x4041, 0xff00, 0xff00, 0xffff, 0xff00,
    0xffff, 0x0355, 0xffff, 0xff32, 0xff1f, 0xff1f, 0xff26, 0xff20,
    0xff1f, 0xff1b, 0xff17, 0xff17, 0xff56, 0xff1e, 0xff1d, 0x5572,
    0xff1d, 0xff1d, 0xff08, 0xff1d, 0xff56, 0xff7b, 0xff1d, 0xff78,
    0x5a7a, 0xff1d, 0xff1d, 0xff1d, 0xff28, 0xff00, 0xff11, 0xff24,
    0xff11, 0xff08, 0xff74, 0xff74, 0xff78, 0xff02, 0x042e, 0x042e,
    0xff08, 0x4a51, 0x0c04, 0x0c04, 0x0c04, 0xff22, 0xff04, 0xff04,
    0x4604, 0xff59, 0xffff, 0xff0f, 0xff24, 0xff24, 0xff04, 0x0c04,
    0xff04, 0xff52, 0xff24, 0xffff, 0xff29, 0xff29, 0xff29, 0xffff,
    0xffff, 0xff29, 0xff06, 0xff29, 0xff09, 0xff29, 0xff24, 0xff05,
    0xff29, 0xff29, 0xff1c, 0xff09, 0xff29, 0x5979, 0x576a, 0xff29,
    0xff27, 0xff23, 0xff23, 0xff5a, 0xff5a, 0xff23, 0xff07, 0xff07,
    0xff07, 0xff07, 0xff65, 0xff65, 0xff24, 0x566a, 0xff41, 0xffff,
    0xffff, 0xffff
};
static int ssx_bgm_entry(int stage, int room) {
    if (stage < 0 || stage > 5) return -1;
    int idx = room + SS_STAGE_OFF[stage];
    if (idx < 0 || idx >= (int)(sizeof SS_BGMTBL / sizeof SS_BGMTBL[0])) return -1;
    uint16_t e = SS_BGMTBL[idx];
    return (e == 0xffff) ? -1 : (int)e;
}
static int ssx_main_slot(int stage, int room) {
    int e = ssx_bgm_entry(stage, room);
    return e < 0 ? -1 : (e & 0x3f);
}
static int ssx_sub_slot(int stage, int room) {
    int e = ssx_bgm_entry(stage, room);
    if (e < 0 || ((e >> 8) & 0xff) == 0xff) return -1;
    return (e >> 8) & 0x3f;
}

/* Load one bank (VH/VB/SEQ) from the CD. `name` = "MAIN" or "SUB". CD files are
 * 8.3-uppercase, slot in hex: MAIN32.VH/.VB/.SEQ, SUB15.VH/.VB/.SEQ. Uses the
 * shared CD staging buffer transiently: VH→copy, VB→SPU RAM (uploaded straight
 * from staging), SEQ→persistent store. */
static int ssx_load_track(ssx_seq_t *s, int ch_base, int ch_count,
                          const char *name, int slot,
                          uint8_t *seq_store, int seq_store_cap)
{
    if (slot < 0) return -1;
    char path[32];
    static uint8_t vh_copy[8192];   /* room VH headers run to ~6.7KB (MAIN16) */

    /* VH → vh_copy (must survive the later staging reuse). RE2: \COMMON\SOUND\ */
    sprintf(path, "\\SOUND\\%s%02X.VH;1", name, slot);
    int vhs = re15_cd_load_file(path, re15_cd_staging, RE15_CD_STAGING_SIZE);
    if (vhs <= 0 || vhs > (int)sizeof vh_copy) return -1;
    memcpy(vh_copy, re15_cd_staging, (size_t)vhs);

    /* VB → SPU RAM, streamed in staging-sized chunks (room banks can be ~224KB,
     * over the 100KB staging — see ssx_stream_vb_to_spu). */
    sprintf(path, "\\SOUND\\%s%02X.VB;1", name, slot);
    uint32_t vbs32 = 0;
    uint32_t spu_base = ssx_stream_vb_to_spu(path, &vbs32);
    if (vbs32 == 0) return -1;
    int vbs = (int)vbs32;

    /* SEQ → staging (ssx_load copies it into seq_store). */
    sprintf(path, "\\SOUND\\%s%02X.SEQ;1", name, slot);
    int seqs = re15_cd_load_file(path, re15_cd_staging, RE15_CD_STAGING_SIZE);
    if (seqs <= 0) return -1;

    return ssx_load(s, ch_base, ch_count, vh_copy, vhs, spu_base,
                    re15_cd_staging, seqs, seq_store, seq_store_cap);
}

/* ════════════════════════════════════════════════════════════════════════
 *  60 Hz tick source = the free-running vblank counter (VSync(-1)). No IRQ
 *  callback needed; BGM tempo is independent of the render frame rate.
 * ════════════════════════════════════════════════════════════════════════ */
static uint32_t s_vbl_last = 0;            /* last VSync(-1) sample */

/* Deferred room-BGM request. The CD load (banks → SPU RAM) is a blocking,
 * ~125 KB read; running it in re15_audio_init would stall boot before the
 * first frame is presented. We instead register the request here and perform
 * the load a few ticks into the main loop, so the intro starts drawing first. */
static int s_bgm_req   = 0;
static int s_bgm_defer = 0;
static int s_bgm_stage = 0, s_bgm_room = 0;

/* ════════════════════════════════════════════════════════════════════════
 *  Public API
 * ════════════════════════════════════════════════════════════════════════ */
void re15_audio_init(void)
{
    SpuInit();
    SpuSetCommonMasterVolume(0x3FFF, 0x3FFF);
    SpuSetCommonCDVolume    (0x3FFF, 0x3FFF);
    SpuSetCommonCDReverb    (0);
    SpuSetCommonExtVolume   (0, 0);
    SpuSetCommonExtReverb   (0);

    load_bundled_vab();
    load_footstep_vab();      /* room snd0 → SPU + its EDT table (footstep SE) */

    /* Reserve the dialogue-voice SPU region (one clip at a time, reused). */
    s_voice_spu_base = spu_alloc(RE15_VOICE_SPU_SIZE);

    s_vbl_last = (uint32_t)VSync(-1);

    g_audio.initialized    = 1;
    g_audio.backend_active = 1;
}

#define RE15_AUDIO_MAX_EVENTS_PER_TICK 2

/* The actual room-BGM load (runs once, deferred from the main loop). */
static void do_load_room_bgm(int stage, int room)
{
    /* SPU BGM-region rewind (multi-room): the bump allocator only grows, so
     * reloading a room's banks would overflow SPU RAM. Mark the BGM region start
     * on the FIRST load (it sits above the SFX VAB + the 50KB reserved voice
     * region, both allocated in re15_audio_init) and rewind to it on every
     * reload — the new room's MAIN/SUB banks reuse the previous room's SPU space.
     * Voice + SFX live below the mark, untouched. */
    static uint32_t s_bgm_spu_mark = 0;
    if (s_bgm_spu_mark == 0) s_bgm_spu_mark = s_spu_top;
    else                     s_spu_top = s_bgm_spu_mark;

    /* MAIN: load + play looping. SUB: load but leave STOPPED — the SCD
     * Sce_bgm_control (0x54) opcode keys the rotor layer on/off at cuts. */
    if (ssx_load_track(&s_bgm_main, BGM_MAIN_CH_BASE, BGM_MAIN_CH_CNT,
                       "MAIN", ssx_main_slot(stage, room),
                       s_main_seq, (int)sizeof s_main_seq) == 0) {
        /* per-layer master (0..0x3FFF). Tunable; conservative to leave SPU
         * headroom for up to 12 simultaneous voices without hard clipping. */
        s_bgm_main.mvol_l = s_bgm_main.mvol_r = 0x1400;
        ssx_start(&s_bgm_main, 1);
        g_audio.events_bgm++;
    }
    if (ssx_load_track(&s_bgm_sub, BGM_SUB_CH_BASE, BGM_SUB_CH_CNT,
                       "SUB", ssx_sub_slot(stage, room),
                       s_sub_seq, (int)sizeof s_sub_seq) == 0) {
        s_bgm_sub.mvol_l = s_bgm_sub.mvol_r = 0x1400;
        /* stays playing=0 until seq_ctl(slot 1, op 1). */
    }
    /* re-sample the vblank clock so the deferred-load gap isn't fast-forwarded. */
    s_vbl_last = (uint32_t)VSync(-1);
}

/* DEBUG ISOLATION TOGGLE (2026-06-03): boot hang regression hunt. 0 = the room
 * BGM is never loaded/played (pure isolation: does the rest boot?). Flip to 1
 * once the hang is located. */
#define RE15_BGM_ENABLE 1

void re15_audio_start_room_bgm(int stage, int room)
{
#if RE15_BGM_ENABLE
    /* Multi-room (2026-06-05): re-entrant per-room BGM. Track the resident
     * (stage,room); on a CHANGE, STOP the old room's tracks and load the new
     * room's (SS_BGMTBL → MAIN/SUB slot). If the new room's bank isn't on the
     * disc, ssx_load_track fails gracefully → silence (better than the previous
     * room's track wrongly continuing). Same room → no-op. */
    static int cur_stage = -1, cur_room = -1;
    if (!g_audio.initialized) return;
    if (stage == cur_stage && room == cur_room) return;
    cur_stage = stage; cur_room = room;
    ssx_stop(&s_bgm_main);
    ssx_stop(&s_bgm_sub);
    /* Load NOW (called from main() before scd_vm_init + the loop, and from the
     * room-change consume), so both layers are resident before the SCD can fire
     * Sce_bgm_control (0x54). */
    do_load_room_bgm(stage, room);
    (void)s_bgm_req; (void)s_bgm_defer; (void)s_bgm_stage; (void)s_bgm_room;
#else
    (void)stage; (void)room; (void)s_bgm_defer; (void)s_bgm_stage; (void)s_bgm_room;
#endif
}

/* SsSeq slot control (SCD Sce_bgm_control / 0x54). slot 0 = MAIN, slot 1 = SUB
 * (rotor). op: 1=play/loop, 3=replay → start; 2=stop, 4=pause, 5=decrescendo →
 * stop. The SUB layer is keyed on/off here at the helicopter cuts. */
void re15_audio_seq_ctl(int slot, int op)
{
    if (!g_audio.initialized) return;
    ssx_seq_t *s = (slot == 0) ? &s_bgm_main : (slot == 1) ? &s_bgm_sub : 0;
    if (!s || !s->vab_ok) return;
    switch (op) {
        case 1: case 3: if (!s->playing) ssx_start(s, 1); break;
        case 2: case 4: case 5: ssx_stop(s); break;
        default: break;
    }
}

/* Per-frame rotor (SUB layer) DISTANCE attenuation + STEREO PAN — the L/R volume is
 * computed by the SHARED re15_rotor_compute_pan (rotor_common.c, byte-true
 * FUN_80045a64 + FUN_80045d6c). This backend applies it to the SPU: sets the layer
 * master + rescales the active SUB voices so it tracks continuously. */
void re15_audio_rotor_update(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                             const int32_t heli_pos[3])
{
    if (!g_audio.initialized || !s_bgm_sub.vab_ok || !s_bgm_sub.playing) return;

    int panL, panR;
    re15_rotor_compute_pan(cam_eye, cam_tgt, heli_pos, &panL, &panR);

    int gainL = (0x1400 * panL) / 0x7f;           /* SUB base master 0x1400, scaled */
    int gainR = (0x1400 * panR) / 0x7f;
    s_bgm_sub.mvol_l = gainL;                      /* future note-ons */
    s_bgm_sub.mvol_r = gainR;
    for (int i = 0; i < s_bgm_sub.ch_count; i++)
        if (s_bgm_sub.voice[i].active) {
            SPU_CH_VOL_L(s_bgm_sub.ch_base + i) = (int16_t)gainL;
            SPU_CH_VOL_R(s_bgm_sub.ch_base + i) = (int16_t)gainR;
        }
}
void re15_audio_rotor_silence(void) { if (g_audio.initialized) ssx_stop(&s_bgm_sub); }

void re15_audio_tick(void)
{
    if (!g_audio.initialized) return;

    voice_poll();   /* finish any in-flight async dialogue-clip read → SPU + key */

    /* Perform the deferred room-BGM load once the intro has begun drawing. */
    if (s_bgm_req && --s_bgm_defer <= 0) {
        s_bgm_req = 0;
        do_load_room_bgm(s_bgm_stage, s_bgm_room);
    }

    /* Drain (bounded) the SCD audio event queue. */
    int budget = RE15_AUDIO_MAX_EVENTS_PER_TICK;
    scd_audio_event_t evt;
    while (budget-- > 0 && scd_audio_queue_pop(&evt)) {
        g_audio.events_total++;
        switch ((scd_audio_kind_t)evt.kind) {
            case SCD_AUDIO_SE_ON:
                g_audio.events_se_on++;
                if (s_sfx_loaded && s_sfx_vab.vag_count > 0) {
                    int idx = (evt.sample_id - 1);
                    if (idx < 0) idx = 0;
                    idx %= s_sfx_vab.vag_count;
                    play_sample(idx, evt.volume ? evt.volume : 100);
                }
                break;
            case SCD_AUDIO_SEQ_CTL:
                re15_audio_seq_ctl(evt.bank, evt.volume);  /* bank=slot, volume=op */
                break;
            case SCD_AUDIO_VOICE_ON:
                g_audio.events_xa_on++;          /* dialogue voice (Message_on) */
                voice_play(evt.sample_id);       /* sample_id = message id → VOICE<NN>.VAG */
                break;
            case SCD_AUDIO_BGMTBL_SET: g_audio.events_bgm++;     break;
            case SCD_AUDIO_XA_ON:      g_audio.events_xa_on++;   break;
            case SCD_AUDIO_SE_VOL:     g_audio.events_se_vol++;  break;
            case SCD_AUDIO_NONE:
            default:                   g_audio.events_unknown++; break;
        }
    }

    /* Advance the BGM sequencer(s) by the real elapsed 60 Hz vblanks. */
    uint32_t now = (uint32_t)VSync(-1);
    uint32_t dvb = now - s_vbl_last;
    s_vbl_last = now;
    if (dvb) {
        if (dvb > 16) dvb = 16;          /* clamp a long stall (don't fast-forward the song) */
        ssx_advance(&s_bgm_main, dvb);
        ssx_advance(&s_bgm_sub,  dvb);
    }
}

void re15_audio_shutdown(void)
{
    if (!g_audio.initialized) return;
    SpuSetCommonMasterVolume(0, 0);
    g_audio.backend_active = 0;
}
