/*
 * RE1.5 Rebuilt — Audio backend, PC (started Phase 4.6.1; NOW FULLY PLAYING).
 *
 * SDL2 audio device (44.1 kHz, S16 stereo) + a software mixer running in the
 * SDL callback thread (mixer state guarded by SDL_LockAudioDevice). What is
 * ACTUALLY built + playing here (the "4.6.1 no playback" note below was long
 * outdated — this backend renders sound end-to-end):
 *   - SCD Se_on SFX: ADPCM-decoded VAGs mixed via play_sample_pc (re15_audio_tick).
 *   - Player FOOTSTEPS: re15_audio_footstep plays the room snd0 bank via its EDT.
 *   - Dialogue VOICE: RE2-style re15_xa CD-XA stream (re15_voice_play, from synchro WAVs).
 *   - BGM: a full SsSeq software synth (MAIN+SUB layers, SEQp→MIDI, VAB soundfont,
 *     real PSX SPU ADSR envelopes + note2pitch + STUDIO_B SPU reverb).
 *   - Looping room ambience (helicopter rotor) with distance/pan.
 *
 * NOT YET wired: COMBAT SFX (gunshot / hit / zombie death groan). Those come from
 * the OVERLAY SE-play FUN_800453d0 (the per-room SE table @DAT_800ac778+0x14), a
 * DIFFERENT path than the SCD Se_on above — the C-driven combat (re15_enemy_*_fx,
 * player fire) emits no audio yet. Porting it = FUN_800453d0's SE-id→VAG+voice-param
 * logic + the combat SE bank. (Unverifiable by ear in the headless/agent env.)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL.h>
#include "re15_audio.h"
#include "re15_scd.h"
#include "re15_vab.h"
#include "re15_room.h"     /* g_room_rdt — footstep snd0 VAB sliced from the room RDT */

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);

re15_audio_state_t g_audio;

static SDL_AudioDeviceID s_audio_dev = 0;

/* ===== Software ADPCM mixer =============================================
 *
 * The audio callback runs on a separate thread spawned by SDL. We protect
 * mixer state with SDL_LockAudioDevice / SDL_UnlockAudioDevice (these
 * suspend the callback while held — clearer than a separate mutex).
 *
 * Each VAG is decoded to PCM at boot and kept in main RAM. When an
 * Se_on event arrives, we activate a free slot in s_active[]. The
 * callback walks active slots and mixes them into the output buffer.
 *
 * Sample rate: VAGs are typically 22050 Hz on PSX but we play them
 * back at the device rate (44100 Hz) without resampling, so they
 * sound ~2× faster than original. Acceptable for the "is it audible"
 * milestone; Phase 4.6.4+ adds proper pitch handling. */

/* 4 simultaneous voices is plenty for SFX-style audio; reducing from 8
 * keeps the cumulative clipping/click load down when several Se_on
 * events fire in quick succession. RE1.5 in-game audio is essentially
 * monophonic per channel anyway — multiple events should map to
 * different SPU channels via the tone-table mapping (Phase 4.7+). */
#define MIXER_MAX_ACTIVE_SAMPLES   4

/* Output device rate (Hz). Voice clips are resampled to this at load. */
#define RE15_AUDIO_RATE 44100

/* Linear fade-out length applied at the tail of every sample to suppress
 * the discontinuity click that occurs when the last decoded PCM value
 * (often non-zero) is followed by silence. 32 samples at 44.1 kHz =
 * 0.73 ms — imperceptible as a fade but kills the click. */
#define MIXER_TAIL_FADE_SAMPLES   32

typedef struct {
    int16_t *pcm;          /* points into s_decoded_vag[]                */
    int      pcm_len;      /* total int16 mono samples                   */
    int      pos;          /* current playback cursor (whole samples)    */
    int      subpos;       /* 0/1 — frame-half counter for 2× downsample */
    int      active;       /* 1 = playing, 0 = free slot                 */
    int      volume_q15;   /* per-voice volume in 0..0x4000 (Q15)        */
} active_sample_t;

static int16_t *s_decoded_vag    [RE15_VAB_MAX_SAMPLES];
static int      s_decoded_vag_len[RE15_VAB_MAX_SAMPLES];
static int      s_vag_count   = 0;
static int      s_vab_loaded  = 0;

static active_sample_t s_active[MIXER_MAX_ACTIVE_SAMPLES];
static int             s_next_slot = 0;

/* Room footstep bank (snd0) — separate from the Se_on test VAB. Its EDT table
 * (snd0.edt) maps a floor sound_type → VAB tone → VAG (byte-true FUN_80045630). */
static int16_t *s_foot_decoded    [RE15_VAB_MAX_SAMPLES];
static int      s_foot_decoded_len[RE15_VAB_MAX_SAMPLES];
static re15_vab_t s_foot_vab;
static uint8_t    s_foot_edt[128];
static int        s_foot_loaded = 0;

/* Room SE bank (snd1, RDT +0x14/+0x18/+0x1c) — the per-room sound-effect bank that the
 * overlay SE-play FUN_800453d0 uses (DAT_800ac778+0x14 = snd1's EDT/SE-table). This is
 * where the COMBAT SEs live (zombie death groan, etc.). Same EDT→program→tone→VAG lookup
 * as the footstep bank, so re15_audio_room_se reuses re15_footstep_vag on snd1. */
static int16_t   *s_se_decoded    [RE15_VAB_MAX_SAMPLES];
static int        s_se_decoded_len[RE15_VAB_MAX_SAMPLES];
static re15_vab_t s_se_vab;
static const uint8_t *s_se_edt   = NULL;
static int        s_se_loaded    = 0;

/* ===== RE2-style dialogue-voice subsystem (RE 2026-05-30) ================
 * Modelled on RE2's CD-XA voice path: the script selects a voice by
 * (file, channel) → CdlSetfilter, then CdlReadS streams its sectors into the
 * SPU CD-input mix (gated by CD_initvol's CD_VOL_L/R=0x3FFF). RE2's trigger is
 * opcode Xa_on(0x59); ROOM1170's SCD has NO Xa_on, so — per RE1.5's content —
 * the clip is keyed by the Message_on id (the r<room>_m<id>.xa file-naming
 * convention). We therefore trigger from Message_on but keep RE2's subsystem
 * SHAPE: a `re15_xa` streaming façade (filter / read_s / eos / cd_vol) and a
 * `re15_voice` manager (the Xa_on-handler role: (room,voice)→clip). On this
 * SDL/PC backend the "CD-XA stream" is one pre-decoded clip mixed through a
 * single monophonic channel — the data model stays RE2-shaped (a future real
 * CD-XA backend drops in under the same re15_xa interface).
 * Ref: RE2_Quellcode/CD_initvol.c, CdControl(CdlSetfilter/CdlReadS). */
#define VOICE_MAX_MSG 64

/* re15_xa — the CD-XA streaming primitives (== libcd CdlSetfilter/CdlReadS). */
typedef struct { uint8_t file, channel; } re15_xa_filter_t;   /* == CdlFILTER */
typedef struct {
    re15_xa_filter_t filter;       /* selected (file,channel) — CdlSetfilter   */
    const int16_t   *pcm;          /* PC: the bound clip = "the live stream"   */
    int              pcm_len;      /* clip length in mono samples              */
    int              pos, subpos;  /* cursor + 22050→44100 half-step           */
    uint16_t         cd_vol_l;     /* mirrors CD_VOL_L (CD_initvol = 0x3FFF)   */
    uint16_t         cd_vol_r;     /* mirrors CD_VOL_R                         */
    int              active;       /* mirrors a CdlReadS in progress           */
} re15_xa_stream_t;
static re15_xa_stream_t s_xa;      /* the single streaming voice channel       */

/* re15_voice — decoded-clip cache, keyed by voice id (RE2 voice-table role). */
typedef struct { int16_t *pcm; int len; int tried; } re15_voice_clip_t;
static re15_voice_clip_t s_voice_clip[VOICE_MAX_MSG];

/* forward decls (defined below; used by re15_audio_init / _tick above them) */
static void re15_xa_init(void);
static void re15_voice_play(uint16_t room, int voice_id);

/* ===== RE2-style BGM: SsSeq software sequencer + VAB-soundfont synth ======
 * Mirrors RE2's room-music path (RE 2026-05-30, FUN_8005a97c): load the MAIN
 * bank's VAB (VH+VB) + SEQ, SsVabOpenHead/SsVabTransBody/SsSeqOpen/SsSeqPlay,
 * then SsSeqCalledTbyT pumps the sequence each frame. On this SDL backend the
 * SPU is a software synth: the SEQp event stream (standard MIDI after the
 * 15-byte pQES header) drives note-on/off; each note plays a VAB tone's VAG
 * (ADPCM→PCM) at note2pitch, enveloped, looped, mixed into the output.
 * Content = RE1.5 SOUND/BGM/MAIN32. Ref: RE2_Quellcode/FUN_8005a97c.c,
 * FUN_8007e474.c, _SsInitSoundSeq.c, vab_common.c (the soundfont backend). */
#define SS_MAX_VOICES 24
#define SS_CHANNELS   16
#define SS_SEQ_HDR    15            /* pQES header bytes before the event stream */

typedef struct {
    int             active;
    const int16_t  *pcm;
    int             pcm_len;
    int             loop_start;     /* sample index to wrap to, or -1 = one-shot */
    uint32_t        phase;          /* Q16 resample cursor */
    uint32_t        step;           /* Q16 step per output frame (= pitch<<4) */
    int             vol_l, vol_r;   /* Q15 */
    int             env_level;      /* current PSX-SPU ADSR level 0..0x7fff */
    int             env_phase;      /* 0=attack 1=decay 2=sustain 3=release */
    int             env_count;      /* output samples until the next envelope step */
    uint16_t        adsr1, adsr2;   /* SPU ADSR registers (from the VAB tone) */
    uint8_t         chan, note;
} ss_voice_t;

/* One independent SsSeq instance = one VAB soundfont + one SEQ being played.
 * RE2 plays the room's MAIN and SUB tracks SIMULTANEOUSLY (FUN_80044564 +
 * FUN_80044774), so we run two instances and sum them. For ROOM1170 that is
 * MAIN32 (the music) + SUB_15 (the secondary/rotor-ambience layer). */
typedef struct {
    re15_vab_t     vab;
    int            vab_ok;
    int16_t       *vag_pcm [RE15_VAB_MAX_SAMPLES];
    int            vag_len [RE15_VAB_MAX_SAMPLES];
    int            vag_loop[RE15_VAB_MAX_SAMPLES];  /* loop-start sample, -1 = one-shot */
    const uint8_t *seq;             /* SEQ event bytes (owned copy)        */
    int            seq_len;
    int            ppqn;            /* ticks per quarter note              */
    uint32_t       tempo_us;        /* microseconds per quarter note       */
    int            cursor;          /* current read offset in seq          */
    uint8_t        rstatus;         /* MIDI running status                 */
    int            pending_dt;      /* ticks until the next event fires    */
    double         accum;           /* accumulated ticks (fractional)      */
    int            playing;
    int            loop;
    uint8_t        prog[SS_CHANNELS];
    int            cvol[SS_CHANNELS];      /* CC7 channel volume 0..127     */
    int            bend_q16[SS_CHANNELS];  /* pitch-bend mult (Q16, 0x10000=none) */
    ss_voice_t     voice[SS_MAX_VOICES];
    int            mvol;            /* per-layer master volume (Q15)       */
    int            mvol_l, mvol_r;  /* per-layer L/R master (rotor pan; =mvol if no pan) */
} ss_seq_t;

static ss_seq_t s_ss_main;          /* MAIN<nn> music track                */
static ss_seq_t s_ss_sub;           /* SUB_<nn> secondary/ambience track   */
static int      s_ss_rate = 44100;
static int      s_ss_sub_base_mvol = 0x1a00;  /* SUB layer base vol (rotor gating scales this) */

/* (RE1.5 SE pan + distance-attenuation tables moved to the SHARED rotor_common.c,
 * along with the integer azimuth + pan math — see re15_rotor_compute_pan. This port
 * previously kept its own SEPARATE SE_PAN/SE_ATTEN[128] + an atan2(double) azimuth
 * with a MIRRORED L/R pan and a 127-clamp; all three drifted from the byte-true PSX
 * path and are now unified.) */

/* Looping room ambience (the helicopter rotor). RE: the rotor is a looping VAG
 * in the room's snd0 SE bank (RDT +0x0C/+0x10), keyed on by the helicopter
 * object (FUN_80036718 → FUN_80045630(2,0)), NOT by the SCD. The reborn engine
 * never loaded the room SE banks, so it was silent. We load snd0, decode the
 * rotor VAG, and loop it continuously from the cinematic start. */
typedef struct {
    int16_t *pcm; int len; int loop_start;   /* decoded VAG + loop point */
    uint32_t pos, step;                       /* Q16 play cursor + rate   */
    int vol;                                  /* Q15 */
    int active;
} amb_voice_t;
static amb_voice_t s_amb;

static void ss_mix(ss_seq_t *s, int16_t *out, int frames);  /* one layer → out (additive) */
static void re15_ss_render_bgm(int16_t *out, int frames);   /* both layers + reverb → out */
static void re15_amb_mix(int16_t *out, int frames);         /* looping rotor ambience → out */
static void re15_bgm_play_room(int stage, int room); /* load+play MAIN+SUB layers */
static int  re15_bgm_for_room(int stage, int room);   /* stage/room → MAIN slot   */

/* Audio callback — mixes active sample slots into the output buffer.
 * Format: signed 16-bit stereo at the device sample rate. SDL guarantees
 * `len` is a whole number of audio frames (we asked for 1024 samples ×
 * 2 channels × 2 bytes = 4096 bytes per chunk). */
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
    (void)userdata;
    SDL_memset(stream, 0, (size_t)len);

    int16_t *out      = (int16_t *)stream;
    int      frames   = len / 4;                /* 2 ch × 2 bytes = 4 B/frame */

    /* For each active sample, mix into both output channels. Volume
     * scaling is Q15. Clip to int16 range.
     *
     * Sample-rate match: PSX VAGs are typically 22050 Hz; SDL plays at
     * 44100 Hz. Advance the source cursor every OTHER frame (subpos
     * toggles) to halve effective playback rate — VAG sounds at its
     * natural pitch instead of 2× fast. Phase 4.6.4+ will pull real
     * per-sample rates from the VAB tone table for accurate pitch. */
    for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++) {
        active_sample_t *s = &s_active[i];
        if (!s->active || s->pcm == NULL) continue;

        for (int f = 0; f < frames; f++) {
            if (s->pos >= s->pcm_len) {
                s->active = 0;
                break;
            }
            int32_t v = ((int32_t)s->pcm[s->pos] * s->volume_q15) >> 15;

            /* Tail fade-out: ramp the last MIXER_TAIL_FADE_SAMPLES PCM
             * samples to zero. Otherwise the abrupt amplitude drop at
             * sample end produces an audible click ("klopfen" with
             * multiple overlapping voices each ending periodically). */
            int rem = s->pcm_len - s->pos;
            if (rem < MIXER_TAIL_FADE_SAMPLES) {
                v = (v * rem) / MIXER_TAIL_FADE_SAMPLES;
            }

            s->subpos ^= 1;
            if (s->subpos == 0) s->pos++;   /* advance every other audio frame */

            int32_t L = (int32_t)out[f * 2 + 0] + v;
            int32_t R = (int32_t)out[f * 2 + 1] + v;
            if (L >  32767) L =  32767; else if (L < -32768) L = -32768;
            if (R >  32767) R =  32767; else if (R < -32768) R = -32768;
            out[f * 2 + 0] = (int16_t)L;
            out[f * 2 + 1] = (int16_t)R;
        }
    }

    /* Mix the RE2-style XA voice stream into the output (the PC analogue of the
     * SPU CD-input mix). cd_vol_l/r mirror RE2's CD_VOL_L/R (0x3FFF = full, set
     * by re15_xa_init ≈ CD_initvol). Clips are pre-resampled to the device rate
     * in re15_voice_load_clip (the synchro WAVs mix 32000 + 22050 Hz sources),
     * so playback is 1:1 here — no fixed half-step. */
    if (s_xa.active && s_xa.pcm) {
        for (int f = 0; f < frames; f++) {
            if (s_xa.pos >= s_xa.pcm_len) { s_xa.active = 0; break; }
            int32_t smp = s_xa.pcm[s_xa.pos];
            int rem = s_xa.pcm_len - s_xa.pos;
            if (rem < MIXER_TAIL_FADE_SAMPLES) smp = (smp * rem) / MIXER_TAIL_FADE_SAMPLES;
            s_xa.pos++;
            int32_t L = (int32_t)out[f * 2 + 0] + ((smp * (int32_t)s_xa.cd_vol_l) >> 14);
            int32_t R = (int32_t)out[f * 2 + 1] + ((smp * (int32_t)s_xa.cd_vol_r) >> 14);
            if (L >  32767) L =  32767; else if (L < -32768) L = -32768;
            if (R >  32767) R =  32767; else if (R < -32768) R = -32768;
            out[f * 2 + 0] = (int16_t)L;
            out[f * 2 + 1] = (int16_t)R;
        }
    }

    /* RE2-style BGM: MAIN + SUB SsSeq layers + reverb send. */
    re15_ss_render_bgm(out, frames);
    /* Looping room ambience (helicopter rotor) — dry, after the BGM reverb. */
    re15_amb_mix(out, frames);
}

/* Try to load the bundled test VAB from disk. The PC asset path uses
 * the same several-candidate search pattern as bg_pc to be robust
 * against CWD differences (run from build/, from repo root, etc.). */
static int load_bundled_vab_pc(void)
{
    static const char *vh_candidates[] = {
        "../../re15_reborn/assets/test.vh",
        "../re15_reborn/assets/test.vh",
        "../../../re15_reborn/assets/test.vh",
        "psx_dev/re15_reborn/assets/test.vh",
        NULL
    };
    static const char *vb_candidates[] = {
        "../../re15_reborn/assets/test.vb",
        "../re15_reborn/assets/test.vb",
        "../../../re15_reborn/assets/test.vb",
        "psx_dev/re15_reborn/assets/test.vb",
        NULL
    };

    uint8_t *vh = NULL;
    uint8_t *vb = NULL;
    int      vh_sz = 0, vb_sz = 0;

    for (int i = 0; vh_candidates[i] && !vh; i++) {
        vh = re15_asset_read_file(vh_candidates[i], &vh_sz);
    }
    for (int i = 0; vb_candidates[i] && !vb; i++) {
        vb = re15_asset_read_file(vb_candidates[i], &vb_sz);
    }
    if (!vh || !vb) {
        free(vh); free(vb);
        return -1;
    }

    re15_vab_t vab;
    if (re15_vab_parse(vh, (size_t)vh_sz, &vab) != 0) {
        free(vh); free(vb);
        return -2;
    }

    /* Decode every VAG to 16-bit PCM up front so the audio callback
     * doesn't have to do ADPCM work in the audio thread. */
    s_vag_count = vab.vag_count;
    for (int i = 0; i < vab.vag_count; i++) {
        uint32_t off = vab.samples[i].offset;
        uint32_t sz  = vab.samples[i].size;
        if (off + sz > (uint32_t)vb_sz) continue;

        size_t pcm_cap = (sz / 16) * 28;
        int16_t *pcm   = (int16_t *) malloc(pcm_cap * sizeof(int16_t));
        if (!pcm) continue;

        int wrote = re15_vag_adpcm_decode(vb + off, sz, pcm, pcm_cap);
        s_decoded_vag[i]     = pcm;
        s_decoded_vag_len[i] = wrote;
    }

    free(vh);
    free(vb);
    s_vab_loaded = 1;
    return 0;
}

/* Load the room footstep bank (snd0.vh/vb) + its EDT table (snd0.edt), decode
 * every VAG to PCM up front. Mirrors load_bundled_vab_pc for a separate bank. */
static int load_footstep_vab_pc(void)
{
    /* Globalization Phase 3-A (2026-06-13): the room footstep bank (snd0 VH+VB+EDT) is
     * SLICED from the parsed RDT (g_room_rdt.snd_*[0], header offsets 0x08-0x10) — was
     * separate room####_snd0.{vh,vb,edt} files. Pointers alias the resident RDT buffer
     * (byte-true == the old files; no free). Parity with the PSX footstep loader. */
    if (!g_room_rdt_ok) return -1;
    const uint8_t *vh  = g_room_rdt.snd_vh[0];
    const uint8_t *vb  = g_room_rdt.snd_vb[0];
    const uint8_t *edt = g_room_rdt.snd_edt[0];
    int vh_sz = g_room_rdt.snd_vh_size[0], vb_sz = g_room_rdt.snd_vb_size[0],
        edt_sz = g_room_rdt.snd_edt_size[0];
    if (!vh || !vb || !edt || re15_vab_parse(vh, (size_t)vh_sz, &s_foot_vab) != 0)
        return -1;
    for (int i = 0; i < s_foot_vab.vag_count; i++) {
        uint32_t off = s_foot_vab.samples[i].offset, sz = s_foot_vab.samples[i].size;
        if (off + sz > (uint32_t)vb_sz) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_foot_decoded[i]     = pcm;
        s_foot_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    memcpy(s_foot_edt, edt, (edt_sz < (int)sizeof s_foot_edt) ? edt_sz : (int)sizeof s_foot_edt);
    s_foot_loaded = 1;
    return 0;
}

/* Load + decode the room SE bank (snd1, RDT +0x14/+0x18/+0x1c) — the combat/room SE bank
 * FUN_800453d0 plays from. Sliced from the resident RDT (g_room_rdt.snd_*[1]); the EDT is
 * the SE-parameter table (se_id*4 records) that re15_audio_room_se indexes. Mirrors
 * load_footstep_vab_pc for bank index 1. */
static int load_room_se_vab_pc(void)
{
    if (!g_room_rdt_ok) return -1;
    const uint8_t *vh  = g_room_rdt.snd_vh[1];
    const uint8_t *vb  = g_room_rdt.snd_vb[1];
    const uint8_t *edt = g_room_rdt.snd_edt[1];
    int vb_sz = g_room_rdt.snd_vb_size[1], vh_sz = g_room_rdt.snd_vh_size[1];
    if (!vh || !vb || !edt || re15_vab_parse(vh, (size_t)vh_sz, &s_se_vab) != 0)
        return -1;
    for (int i = 0; i < s_se_vab.vag_count; i++) {
        uint32_t off = s_se_vab.samples[i].offset, sz = s_se_vab.samples[i].size;
        if (off + sz > (uint32_t)vb_sz) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_se_decoded[i]     = pcm;
        s_se_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    s_se_edt    = edt;
    s_se_loaded = 1;
    return 0;
}

/* Play a room SE by id (byte-true FUN_800453d0 core, PC path). The per-room SE table
 * (snd1 EDT, DAT_800ac778+0x14) maps se_id → program+tone → VAG (identical to the footstep
 * lookup, re15_footstep_vag). We resolve the VAG and mix it into the SE voices. The exact
 * pitch (record byte0) + SPU voice/pan (byte3) are FAITHFUL-LINE (the VAG selection + play
 * is byte-true; the fine voice params are the deferred SPU-driver detail). Used by the C-driven
 * combat (e.g. the zombie death groan, FUN_80107cb0 frame 7 -> func_0x800453d0(rng&1?8:5)). */
void re15_audio_room_se(int se_id)
{
    if (!g_audio.initialized || !s_se_loaded || se_id < 0 || se_id >= 0x19) return;
    int vag = re15_footstep_vag(s_se_edt, &s_se_vab, se_id);   /* EDT record -> program/tone -> VAG */
    if (vag < 0 || vag >= RE15_VAB_MAX_SAMPLES || !s_se_decoded[vag]) return;

    int tvol = 100;                                            /* default; per-tone vol is the refinement */
    int vol  = (tvol * 0x4000 / 127) >> 1;
    if (vol > 0x4000) vol = 0x4000; if (vol < 0) vol = 0;

    SDL_LockAudioDevice(s_audio_dev);
    int slot = -1;
    for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++)
        if (!s_active[i].active) { slot = i; break; }
    if (slot < 0) { slot = s_next_slot; s_next_slot = (s_next_slot + 1) % MIXER_MAX_ACTIVE_SAMPLES; }
    s_active[slot].pcm        = s_se_decoded[vag];
    s_active[slot].pcm_len    = s_se_decoded_len[vag];
    s_active[slot].pos        = 0;
    s_active[slot].subpos     = 0;
    s_active[slot].volume_q15 = vol;
    s_active[slot].active     = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* Triggered from re15_audio_tick when a SCD Se_on event arrives. */
static void play_sample_pc(int vag_index, int scd_volume)
{
    if (!s_vab_loaded) return;
    if (vag_index < 0 || vag_index >= s_vag_count) return;
    if (!s_decoded_vag[vag_index]) return;

    /* Map 0..127 SCD volume → 0..0x4000 Q15, then halve to keep
     * headroom for overlapping voices. */
    int vol = (scd_volume * 0x4000 / 127) >> 1;
    if (vol > 0x4000) vol = 0x4000;
    if (vol <    0)   vol = 0;

    /* Phase 4.6.3 demo: pick the first FREE slot. If all are busy, kill
     * the oldest one (s_next_slot — the round-robin head). This avoids
     * the "klopfen" the user heard at event 9+ when 8 simultaneous
     * 1.4-second samples played by 0.5-second-spaced Se_on events all
     * overlapped and clipped against each other. RE1.5 audio is monophonic
     * per channel anyway. */
    SDL_LockAudioDevice(s_audio_dev);
    int slot = -1;
    for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++) {
        if (!s_active[i].active) { slot = i; break; }
    }
    if (slot < 0) {
        slot = s_next_slot;
        s_next_slot = (s_next_slot + 1) % MIXER_MAX_ACTIVE_SAMPLES;
    }
    s_active[slot].pcm        = s_decoded_vag[vag_index];
    s_active[slot].pcm_len    = s_decoded_vag_len[vag_index];
    s_active[slot].pos        = 0;
    s_active[slot].subpos     = 0;
    s_active[slot].volume_q15 = vol;
    s_active[slot].active     = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

static void re15_bgm_dump_wav(const char *path, int stage, int room, int seconds);

void re15_audio_init(void)
{
    /* Diagnostic: RE15_BGM_DUMP=<wav> renders the room's MAIN BGM offline (exact
     * synth path) for A/B vs the PSX capture, then exits. No SDL device needed. */
    const char *dump = getenv("RE15_BGM_DUMP");
    if (dump) {
        const char *sec = getenv("RE15_BGM_DUMP_SECS");
        re15_bgm_dump_wav(dump, 0, 0x17, sec ? atoi(sec) : 40);
        exit(0);
    }

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "[audio] SDL_InitSubSystem(AUDIO) failed: %s\n",
                SDL_GetError());
        return;
    }

    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof want);
    want.freq     = 44100;       /* matches PSX SPU max sample rate */
    want.format   = AUDIO_S16SYS;
    want.channels = 2;
    want.samples  = 1024;        /* ~23 ms latency at 44.1 kHz */
    want.callback = audio_callback;

    s_audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have,
                                       SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (s_audio_dev == 0) {
        fprintf(stderr, "[audio] SDL_OpenAudioDevice failed: %s\n",
                SDL_GetError());
        return;
    }

    /* Unpause — callback fires immediately. */
    SDL_PauseAudioDevice(s_audio_dev, 0);

    /* Phase 4.6.3: load bundled VAB so Se_on events have something to play. */
    load_bundled_vab_pc();
    load_footstep_vab_pc();   /* room snd0 + its EDT table (footstep SE) */
    load_room_se_vab_pc();    /* room snd1 + its SE table (combat/room SE, FUN_800453d0) */

    /* RE2-style XA voice: gate the CD-input into the SPU mix (== CD_initvol). */
    re15_xa_init();

    /* NOTE: room BGM is NOT started here. In the original the helipad MAIN32
     * track only begins when the room cinematic starts — the pre-intro narrator
     * (sub11) plays over silence. The main loop calls re15_audio_start_room_bgm()
     * at the cinematic handoff (sub00 spawn). */

    g_audio.initialized    = 1;
    g_audio.backend_active = 1;
}

/* ── re15_xa: CD-XA streaming primitives (== libcd CdlSetfilter/CdlReadS) ── */

/* == CD_initvol(): gate the CD-XA stream into the SPU mix at full volume. */
static void re15_xa_init(void)
{
    s_xa.cd_vol_l = 0x3FFF;   /* CD_VOL_L = 0x3FFF (RE2_Quellcode/CD_initvol.c) */
    s_xa.cd_vol_r = 0x3FFF;   /* CD_VOL_R = 0x3FFF */
    s_xa.pcm = NULL;
    s_xa.active = 0;
}

/* == CdControl(CdlSetfilter, {file,channel}) — pick one interleaved channel. */
static void re15_xa_set_filter(uint8_t file, uint8_t channel)
{
    s_xa.filter.file = file;
    s_xa.filter.channel = channel;
}

/* == CdlSetloc + CdlReadS — bind the selected clip and start streaming it
 * (one voice at a time; a new read_s re-seeks the single stream). */
static void re15_xa_read_s(const int16_t *pcm, int pcm_len)
{
    SDL_LockAudioDevice(s_audio_dev);
    s_xa.pcm     = pcm;
    s_xa.pcm_len = pcm_len;
    s_xa.pos     = 0;
    s_xa.subpos  = 0;
    s_xa.active  = (pcm && pcm_len > 0) ? 1 : 0;
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* ── re15_voice: SCD-facing voice manager (RE2 Xa_on(0x59)-handler role) ──── */

/* Parse a RIFF/WAVE blob: walk chunks for `fmt ` (rate/channels/bits) and
 * `data`. NOT a fixed 44-byte skip — the synchro WAVs vary: main00-04 are
 * 32000 Hz with an extended header (data well past byte 44), main05-16 are
 * 22050 Hz / 44-byte. Returns the data pointer + fills rate/ch/bits/bytes. */
static const uint8_t *wav_find_data(const uint8_t *b, int sz,
                                    int *rate, int *ch, int *bits, int *data_bytes)
{
    *rate = 22050; *ch = 1; *bits = 16; *data_bytes = 0;
    if (sz < 44 || memcmp(b, "RIFF", 4) != 0 || memcmp(b + 8, "WAVE", 4) != 0) return NULL;
    int p = 12;
    const uint8_t *data = NULL;
    while (p + 8 <= sz) {
        uint32_t csz = (uint32_t)b[p+4] | ((uint32_t)b[p+5]<<8) |
                       ((uint32_t)b[p+6]<<16) | ((uint32_t)b[p+7]<<24);
        const uint8_t *body = b + p + 8;
        if (memcmp(b + p, "fmt ", 4) == 0 && p + 8 + 16 <= sz) {
            *ch   = body[2] | (body[3] << 8);
            *rate = body[4] | (body[5]<<8) | (body[6]<<16) | (body[7]<<24);
            *bits = body[14] | (body[15] << 8);
        } else if (memcmp(b + p, "data", 4) == 0) {
            data = body;
            /* UNSIGNED clamp: main01.wav has a corrupt data size 0xFFFFFFFF
             * (FFmpeg streaming-mux artifact). Casting to (int) gave -1, which
             * skipped the clamp → the clip was rejected (the "2nd voiceover"
             * that never played). Compare against the bytes actually available. */
            uint32_t avail = (uint32_t)(sz - (int)(body - b));
            *data_bytes = (int)(csz > avail ? avail : csz);
            break;
        }
        p += 8 + (int)csz + ((int)csz & 1);   /* chunks are word-aligned */
    }
    return data;
}

/* Load the room's decoded clip for a voice id and cache it, resampled to the
 * mixer rate (44100). RE1.5 keys voice by the Message_on id (the r<room>_m<id>.xa
 * naming); on PC the bytes come from synchro/STAGE1/room1170/main<id>.wav.
 * Returns 1 if a clip exists for this id. */
static int re15_voice_load_clip(int voice_id)
{
    if (voice_id < 0 || voice_id >= VOICE_MAX_MSG) return 0;
    if (s_voice_clip[voice_id].tried) return s_voice_clip[voice_id].pcm != NULL;
    s_voice_clip[voice_id].tried = 1;
    /* TODO multi-room: the RE2 voice table keys on room+channel; only ROOM1170
     * is implemented, so the path is fixed here. */
    static const char *dirs[] = {
        "../../../../synchro/STAGE1/room1170/",
        "../../../synchro/STAGE1/room1170/",
        "../../synchro/STAGE1/room1170/",
        "synchro/STAGE1/room1170/",
        NULL
    };
    uint8_t *wav = NULL; int wsz = 0; char path[160];
    for (int i = 0; dirs[i] && !wav; i++) {
        snprintf(path, sizeof path, "%smain%02d.wav", dirs[i], voice_id);
        wav = re15_asset_read_file(path, &wsz);
    }
    int rate, ch, bits, dbytes;
    const uint8_t *data = wav_find_data(wav, wsz, &rate, &ch, &bits, &dbytes);
    if (!data || dbytes < 2 || bits != 16 || ch < 1) { free(wav); return 0; }

    if (rate <= 0) rate = 22050;
    int src_n = (dbytes / 2) / ch;                       /* source frames (mono-ize) */
    const int16_t *src = (const int16_t *)data;
    /* linear resample src_rate → 44100 (and downmix to mono if stereo).
     * int64_t throughout: on Windows `long` is 32-bit and src_n*44100 overflows. */
    int64_t out_n = (int64_t)src_n * RE15_AUDIO_RATE / rate;
    if (out_n < 1) out_n = 1;
    int16_t *pcm = (int16_t *)malloc((size_t)out_n * sizeof(int16_t));
    if (!pcm) { free(wav); return 0; }
    for (int64_t i = 0; i < out_n; i++) {
        int64_t sp = i * (int64_t)rate * 65536 / RE15_AUDIO_RATE;   /* Q16 src pos */
        int64_t si = sp >> 16; int frac = (int)(sp & 0xffff);
        int64_t s0i = si * ch, s1i = (si + 1 < src_n ? (si + 1) : si) * ch;
        int32_t a, b2;
        if (ch == 1) { a = src[s0i]; b2 = src[s1i]; }
        else { a = (src[s0i] + src[s0i+1]) / 2; b2 = (src[s1i] + src[s1i+1]) / 2; }
        pcm[i] = (int16_t)(a + (((b2 - a) * frac) >> 16));
    }
    free(wav);

    /* Level-floor: the synchro VO is our own AI-TTS production (RE1.5 has no
     * English voice), assembled from two pipelines at inconsistent levels —
     * some clips (e.g. main00/02/03) sit at ~15% peak while others are ~50%, so
     * the quiet lines are inaudible next to Leon's. Lift only the under-level
     * clips to a common floor (cap the gain so near-silent clips don't blow up
     * their noise floor). Non-destructive; clips already at level are untouched. */
    {
        int32_t peak = 1;
        for (int64_t i = 0; i < out_n; i++) {
            int32_t a = pcm[i] < 0 ? -pcm[i] : pcm[i];
            if (a > peak) peak = a;
        }
        const int32_t target = 14745;            /* ~45% full-scale, matches the loud clips */
        if (peak < target) {
            int gain_q8 = target * 256 / peak;    /* Q8 */
            if (gain_q8 > 4 * 256) gain_q8 = 4 * 256;   /* cap at 4× */
            for (int64_t i = 0; i < out_n; i++) {
                int32_t s = (pcm[i] * gain_q8) >> 8;
                if (s >  32767) s =  32767; else if (s < -32768) s = -32768;
                pcm[i] = (int16_t)s;
            }
        }
    }

    s_voice_clip[voice_id].pcm = pcm;
    s_voice_clip[voice_id].len = (int)out_n;
    fprintf(stderr, "[voice] clip loaded: main%02d.wav (%d Hz x%d → %ld @%d Hz)\n",
            voice_id, rate, ch, (long)out_n, RE15_AUDIO_RATE);
    return 1;
}

/* The RE2 Xa_on(0x59)-handler equivalent: resolve (room, voice) → (XA file,
 * channel) then stream it. Triggered from Message_on (ROOM1170 has no Xa_on). */
static void re15_voice_play(uint16_t room, int voice_id)
{
    if (!g_audio.initialized) return;
    if (!re15_voice_load_clip(voice_id)) return;    /* no clip for this id → silent */
    /* RE2 addresses a voice by (file, channel); model file = room low byte,
     * channel = voice id (the r<room>_m<id> key). */
    re15_xa_set_filter((uint8_t)(room & 0xFF), (uint8_t)voice_id);
    re15_xa_read_s(s_voice_clip[voice_id].pcm, s_voice_clip[voice_id].len);
}

/* ── re15_ss implementation: VAB soundfont + SEQp software synth ─────────── */

/* Decode every VAG of the loaded VAB to PCM + find its loop-start sample
 * (PSX VAG flag byte+1 bit 0x04 on a 16-byte ADPCM block = loop start). */
static void ss_decode_vab(ss_seq_t *s, const uint8_t *vb, int vb_sz)
{
    for (int i = 0; i < s->vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s->vab.samples[i].offset, sz = s->vab.samples[i].size;
        s->vag_pcm[i] = NULL; s->vag_len[i] = 0; s->vag_loop[i] = -1;
        if (sz == 0 || off >= (uint32_t)vb_sz) continue;
        /* CLAMP rather than skip when the VAB size-table slightly overruns the VB
         * (e.g. SUB_15: table says 41504 B but the VB body is 41500 — 4 B short
         * from extraction/rounding). Skipping dropped the whole VAG → SUB_15 (the
         * helicopter-rotor layer) rendered SILENT. Clamp to the bytes present. */
        if (off + sz > (uint32_t)vb_sz) sz = (uint32_t)vb_sz - off;
        sz &= ~15u;                              /* whole 16-byte ADPCM blocks */
        if (sz == 0) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s->vag_pcm[i] = pcm;
        s->vag_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
        /* PSX VAG loop semantics (psx-spx): flag bit2 (0x04)=loop-start (the
         * repeat address); the END block's bit0 (0x01)=loop-end. The voice only
         * KEEPS looping if bit1 (0x02, repeat) is also set there — bit0 without
         * bit1 = play-once-then-mute. So a VAG is one-shot UNLESS some block
         * carries the repeat flag. (Old code looped on the mere presence of a
         * 0x04 start → one-shot samples like VAG0 repeated forever = the
         * "elements repeat / break" artefact.) */
        int loop_start = -1, repeats = 0;
        for (uint32_t b = 0; b + 16 <= sz; b += 16) {
            uint8_t fl = vb[off + b + 1];
            if ((fl & 0x04) && loop_start < 0) loop_start = (int)(b / 16) * 28;
            if (fl & 0x02) repeats = 1;             /* sustaining loop */
        }
        s->vag_loop[i] = repeats ? (loop_start >= 0 ? loop_start : 0) : -1;
    }
}

static void ss_all_voices_off(ss_seq_t *s) {
    for (int i = 0; i < SS_MAX_VOICES; i++) s->voice[i].active = 0;
}

/* PSX SPU ADSR envelope, advanced one output sample (psx-spx "Envelope
 * Operation"). Replaces the old fixed 23 ms attack/release approximation —
 * MAIN32's release shifts (11/14) are 93 ms / 740 ms, so the fixed ramp cut
 * sustained notes ~30× too fast ("bricht abrupt ab"). Decodes adsr1/adsr2:
 *   adsr1: Sl[0-3] Dr[4-7] AtkStep[8-9] AtkShift[10-14] AtkMode[15]
 *   adsr2: RelShift[0-4] RelMode[5] SusStep[6-7] SusShift[8-12] SusDir[14] SusMode[15]
 * Returns the current level (0..0x7fff); deactivates the voice when release hits 0. */
static int ss_env_update(ss_voice_t *v) {
    if (v->env_count > 0) { v->env_count--; return v->env_level; }

    int shift, mag, mode, inc;            /* base step magnitude + direction */
    switch (v->env_phase) {
        case 0:  /* attack — rise to 0x7fff */
            shift = (v->adsr1 >> 10) & 0x1f; mag = 7 - ((v->adsr1 >> 8) & 3);
            mode = (v->adsr1 >> 15) & 1; inc = 1; break;
        case 1:  /* decay — exp fall to the sustain level */
            shift = (v->adsr1 >> 4) & 0xf;  mag = 8; mode = 1; inc = 0; break;
        case 2:  /* sustain — hold / SusDir-driven drift */
            shift = (v->adsr2 >> 8) & 0x1f; mag = 8 - ((v->adsr2 >> 6) & 3);
            mode = (v->adsr2 >> 15) & 1; inc = ((v->adsr2 >> 14) & 1) ? 0 : 1;
            if (shift == 0) { v->env_count = 0x7fffffff; return v->env_level; } /* static hold */
            break;
        default: /* release — fall to 0 */
            shift = v->adsr2 & 0x1f; mag = 8; mode = (v->adsr2 >> 5) & 1; inc = 0; break;
    }

    int cycles = 1 << (shift > 11 ? shift - 11 : 0);
    int step   = mag << (shift < 11 ? 11 - shift : 0);
    if (mode && inc && v->env_level > 0x6000) cycles <<= 2;       /* exp attack slows near top */
    if (mode && !inc) step = (int)(((int64_t)step * v->env_level) >> 15); /* exp decay/release */
    if (step < 1) step = 1;

    v->env_level += inc ? step : -step;
    if (v->env_level > 0x7fff) v->env_level = 0x7fff;
    if (v->env_level < 0)      v->env_level = 0;
    v->env_count = cycles - 1;

    if (v->env_phase == 0 && v->env_level >= 0x7fff) v->env_phase = 1;   /* → decay */
    else if (v->env_phase == 1) {
        int sl = ((v->adsr1 & 0xf) + 1) << 11;                  /* sustain level */
        if (sl > 0x7fff) sl = 0x7fff;
        if (v->env_level <= sl) { v->env_level = sl; v->env_phase = 2; } /* → sustain */
    } else if (v->env_phase == 3 && v->env_level <= 0) v->active = 0;     /* release done */
    return v->env_level;
}

/* Fade-release every voice (used at the loop point so the wrap is seamless
 * instead of an abrupt cut). Clears notes left hanging at end-of-track. */
static void ss_all_voices_release(ss_seq_t *s) {
    for (int i = 0; i < SS_MAX_VOICES; i++)
        if (s->voice[i].active && s->voice[i].env_phase != 3) {
            s->voice[i].env_phase = 3; s->voice[i].env_count = 0;
        }
}

/* note-on: channel program + note → VAB tone → allocate a synth voice. */
static void ss_note_on(ss_seq_t *s, int chan, int note, int vel)
{
    const re15_vab_tone_t *t = re15_vab_find_tone(&s->vab, s->prog[chan], note);
    if (!t) return;
    int vi = (int)t->vag_index - 1;                 /* vag_index is 1-based */
    if (vi < 0 || vi >= s->vab.vag_count || !s->vag_pcm[vi] || s->vag_len[vi] <= 0) return;
    int slot = -1, q = 0x7fffffff;
    for (int i = 0; i < SS_MAX_VOICES; i++) {
        if (!s->voice[i].active) { slot = i; break; }
        if (s->voice[i].env_level < q) { q = s->voice[i].env_level; slot = i; }  /* steal quietest */
    }
    ss_voice_t *v = &s->voice[slot];
    v->pcm = s->vag_pcm[vi]; v->pcm_len = s->vag_len[vi]; v->loop_start = s->vag_loop[vi];
    v->phase = 0;
    uint16_t pitch = re15_vab_note2pitch(note, t->center_note, t->pitch_shift);
    v->step = (uint32_t)pitch << 4;                 /* Q16: pitch 0x1000 = 1.0 */
    int vol = (int)t->vol * vel / 127;
    vol = vol * s->cvol[chan] / 127;
    int q15 = vol * 0x7fff / 127;
    int pan = t->pan; if (pan < 0) pan = 64; if (pan > 127) pan = 127;
    if (pan >= 56 && pan <= 72) { v->vol_l = q15; v->vol_r = q15; }
    else { v->vol_l = q15 * (127 - pan) / 127; v->vol_r = q15 * pan / 127; }
    v->adsr1 = t->adsr1; v->adsr2 = t->adsr2;       /* real SPU ADSR from the VAB tone */
    v->env_level = 0; v->env_phase = 0; v->env_count = 0;   /* start in attack */
    v->chan = (uint8_t)chan; v->note = (uint8_t)note; v->active = 1;
}

static void ss_note_off(ss_seq_t *s, int chan, int note) {
    for (int i = 0; i < SS_MAX_VOICES; i++)
        if (s->voice[i].active && s->voice[i].chan == chan &&
            s->voice[i].note == note && s->voice[i].env_phase != 3) {
            s->voice[i].env_phase = 3; s->voice[i].env_count = 0;   /* → release */
        }
}

static int ss_vlq(ss_seq_t *s) {                     /* variable-length quantity */
    int v = 0;
    while (s->cursor < s->seq_len) {
        uint8_t b = s->seq[s->cursor++];
        v = (v << 7) | (b & 0x7f);
        if (!(b & 0x80)) break;
    }
    return v;
}

/* parse + dispatch one event at the cursor (standard MIDI w/ running status). */
static void ss_fire_event(ss_seq_t *s) {
    if (s->cursor >= s->seq_len) { s->playing = 0; return; }
    uint8_t st = s->seq[s->cursor];
    if (st & 0x80) { s->rstatus = st; s->cursor++; } else { st = s->rstatus; }
    int typ = st & 0xf0, ch = st & 0x0f;
    if (typ == 0x90) {
        int n = s->seq[s->cursor++];
        int vel = s->seq[s->cursor++];
        if (vel) ss_note_on(s, ch, n, vel); else ss_note_off(s, ch, n);
    } else if (typ == 0x80) {
        int n = s->seq[s->cursor++]; s->cursor++; ss_note_off(s, ch, n);
    } else if (typ == 0xC0) {            /* program change (1 byte) */
        s->prog[ch] = s->seq[s->cursor++];
    } else if (typ == 0xD0) {            /* channel pressure (1 byte) — ignored */
        s->cursor++;
    } else if (typ == 0xE0) {            /* pitch bend (2 bytes, 14-bit, ±2 semitones) */
        int lo = s->seq[s->cursor++];
        int hi = s->seq[s->cursor++];
        int bend = (lo & 0x7f) | ((hi & 0x7f) << 7);     /* 0..0x3fff, center 0x2000 */
        double semis = ((double)(bend - 0x2000) / 8192.0) * 2.0;  /* range ±2 */
        s->bend_q16[ch] = (int)(pow(2.0, semis / 12.0) * 65536.0 + 0.5);
    } else if (typ == 0xA0 || typ == 0xB0) {
        int d1 = s->seq[s->cursor++];
        int d2 = s->seq[s->cursor++];
        if (typ == 0xB0 && d1 == 7) s->cvol[ch] = d2;     /* CC7 channel volume */
    } else if (st == 0xFF) {
        int meta = s->seq[s->cursor++];
        int len  = s->seq[s->cursor++];
        if (meta == 0x51 && len == 3)
            s->tempo_us = (uint32_t)((s->seq[s->cursor] << 16) |
                          (s->seq[s->cursor+1] << 8) | s->seq[s->cursor+2]);
        s->cursor += len;
        if (meta == 0x2F) {                          /* end of track */
            if (s->loop) { s->cursor = SS_SEQ_HDR; s->rstatus = 0; ss_all_voices_release(s); }
            else s->playing = 0;
        }
    } else if (st == 0xF0 || st == 0xF7) {
        s->cursor += ss_vlq(s);                      /* sysex — skip */
    } else { s->playing = 0; }                        /* unknown → stop */
}

/* advance the sequence by `frames` output samples of elapsed time. */
static void ss_advance(ss_seq_t *s, int frames) {
    if (!s->playing || !s->seq) return;
    s->accum += (double)frames * (double)s->ppqn * 1e6 /
                ((double)s->tempo_us * (double)s_ss_rate);
    int guard = 8192;
    while (s->playing && s->accum >= (double)s->pending_dt && guard-- > 0) {
        s->accum -= (double)s->pending_dt;
        ss_fire_event(s);
        s->pending_dt = ss_vlq(s);
    }
}

static void ss_mix(ss_seq_t *s, int16_t *out, int frames) {
    if (!s->vab_ok) return;
    ss_advance(s, frames);
    for (int i = 0; i < SS_MAX_VOICES; i++) {
        ss_voice_t *v = &s->voice[i];
        if (!v->active || !v->pcm) continue;
        /* per-channel pitch-bend applied to the base step (Q16 × Q16 → Q16). */
        uint32_t eff_step = (uint32_t)(((uint64_t)v->step *
                              (uint32_t)s->bend_q16[v->chan]) >> 16);
        for (int f = 0; f < frames; f++) {
            int idx = (int)(v->phase >> 16);
            if (idx >= v->pcm_len) {
                if (v->loop_start >= 0 && v->loop_start < v->pcm_len) {
                    v->phase = ((uint32_t)v->loop_start << 16) | (v->phase & 0xffff);
                    idx = (int)(v->phase >> 16);
                } else { v->active = 0; break; }
            }
            int env = ss_env_update(v);             /* real PSX SPU ADSR */
            if (!v->active) break;                  /* release reached 0 */
            /* linear interpolation (kills the nearest-neighbour aliasing/"blechern"). */
            int frac = (int)(v->phase & 0xffff);
            int nxt = idx + 1;
            if (nxt >= v->pcm_len)
                nxt = (v->loop_start >= 0 && v->loop_start < v->pcm_len) ? v->loop_start : idx;
            int32_t s0 = v->pcm[idx], s1 = v->pcm[nxt];
            int32_t smp = s0 + (((s1 - s0) * frac) >> 16);
            smp = (smp * env) >> 15;
            v->phase += eff_step;
            /* per-layer L/R master (rotor pan via re15_audio_rotor_update; =mvol for MAIN). */
            int32_t sl = (smp * s->mvol_l) >> 15;
            int32_t sr = (smp * s->mvol_r) >> 15;
            int32_t L = (int32_t)out[f*2+0] + ((sl * v->vol_l) >> 15);
            int32_t R = (int32_t)out[f*2+1] + ((sr * v->vol_r) >> 15);
            if (L >  32767) L =  32767; else if (L < -32768) L = -32768;
            if (R >  32767) R =  32767; else if (R < -32768) R = -32768;
            out[f*2+0] = (int16_t)L; out[f*2+1] = (int16_t)R;
        }
    }
}

/* ── PSX SPU hardware reverb (the REAL one — replaces the old Schroeder hack) ─
 * RE: ROOM1170 BGM enables SPU reverb via FUN_800438c8 → SsUtSetReverbType(3)
 * = STUDIO_B + SsUtReverbOn + SsUtSetReverbDepth(0x40,0x40). The STUDIO_B preset
 * (32 regs below) was dumped byte-exact from PSX.EXE 0x80077274 + 3*0x44 and
 * matches the documented psx-spx STUDIO_B table. Algorithm = psx-spx "SPU Reverb
 * Formula" (same/diff-side IIR reflections then 4-tap comb then 2 all-pass), run
 * at 22050 Hz over a 0x4840-byte ring; the m../d.. addresses are in 8-byte units. */
#define REV_AREA   0x4840           /* STUDIO_B reverb work-area size (bytes) */
#define REV_NSAMP  (REV_AREA/2)
/* STUDIO_B preset (16-bit signed Q15 volumes / 8-byte-unit addresses): */
enum { dAPF1=0x00b1, dAPF2=0x007f };
static const int vIIR=0x70f0, vCOMB1=0x4fa8, vCOMB2=(int16_t)0xbce0, vCOMB3=0x4510,
                 vCOMB4=(int16_t)0xbef0, vWALL=(int16_t)0xb4c0, vAPF1=0x5280, vAPF2=0x4ec0,
                 vLIN=(int16_t)0x8000, vRIN=(int16_t)0x8000;
enum { mLSAME=0x0904, mRSAME=0x076b, mLCOMB1=0x0824, mRCOMB1=0x065f, mLCOMB2=0x07a2,
       mRCOMB2=0x0616, dLSAME=0x076c, dRSAME=0x05ed, mLDIFF=0x05ec, mRDIFF=0x042e,
       mLCOMB3=0x050f, mRCOMB3=0x0305, mLCOMB4=0x0462, mRCOMB4=0x02b7, dLDIFF=0x042f,
       dRDIFF=0x0265, mLAPF1=0x0264, mRAPF1=0x01b2, mLAPF2=0x0100, mRAPF2=0x0080 };
static int16_t s_rev_buf[REV_NSAMP];
static int     s_rev_addr;          /* byte write head, wraps [0,REV_AREA) */
static int     s_rev_vlout = 0x4000, s_rev_vrout = 0x4000;  /* reverb output vol = SsUtSetReverbDepth(0x40)→0x40<<8 */

static inline int rv_i(int reg)        { int b=(s_rev_addr+reg*8)%REV_AREA; return b>>1; }
static inline int rv_im1(int reg)      { int b=(s_rev_addr+reg*8-2+REV_AREA)%REV_AREA; return b>>1; }
#define RB(reg)   s_rev_buf[rv_i(reg)]
#define RBm1(reg) s_rev_buf[rv_im1(reg)]
static inline int16_t rv_sat(int v){ return v>32767?32767:(v<-32768?-32768:(int16_t)v); }

/* One reverb tick (22050 Hz). lin/rin = BGM input; returns L/R reverb output. */
static void ss_spu_reverb(int Lin0, int Rin0, int *outL, int *outR) {
    int Lin = ((int16_t)vLIN * Lin0) >> 15;
    int Rin = ((int16_t)vRIN * Rin0) >> 15;
    s_rev_buf[rv_i(mLSAME)] = rv_sat((((Lin + (RB(dLSAME)*(int16_t)vWALL>>15) - RBm1(mLSAME)) * (int16_t)vIIR)>>15) + RBm1(mLSAME));
    s_rev_buf[rv_i(mRSAME)] = rv_sat((((Rin + (RB(dRSAME)*(int16_t)vWALL>>15) - RBm1(mRSAME)) * (int16_t)vIIR)>>15) + RBm1(mRSAME));
    s_rev_buf[rv_i(mLDIFF)] = rv_sat((((Lin + (RB(dRDIFF)*(int16_t)vWALL>>15) - RBm1(mLDIFF)) * (int16_t)vIIR)>>15) + RBm1(mLDIFF));
    s_rev_buf[rv_i(mRDIFF)] = rv_sat((((Rin + (RB(dLDIFF)*(int16_t)vWALL>>15) - RBm1(mRDIFF)) * (int16_t)vIIR)>>15) + RBm1(mRDIFF));
    int Lo = ((int16_t)vCOMB1*RB(mLCOMB1) + (int16_t)vCOMB2*RB(mLCOMB2) + (int16_t)vCOMB3*RB(mLCOMB3) + (int16_t)vCOMB4*RB(mLCOMB4)) >> 15;
    int Ro = ((int16_t)vCOMB1*RB(mRCOMB1) + (int16_t)vCOMB2*RB(mRCOMB2) + (int16_t)vCOMB3*RB(mRCOMB3) + (int16_t)vCOMB4*RB(mRCOMB4)) >> 15;
    Lo = Lo - ((int16_t)vAPF1*RB(mLAPF1-dAPF1)>>15); s_rev_buf[rv_i(mLAPF1)] = rv_sat(Lo); Lo = (Lo*(int16_t)vAPF1>>15) + RB(mLAPF1-dAPF1);
    Ro = Ro - ((int16_t)vAPF1*RB(mRAPF1-dAPF1)>>15); s_rev_buf[rv_i(mRAPF1)] = rv_sat(Ro); Ro = (Ro*(int16_t)vAPF1>>15) + RB(mRAPF1-dAPF1);
    Lo = Lo - ((int16_t)vAPF2*RB(mLAPF2-dAPF2)>>15); s_rev_buf[rv_i(mLAPF2)] = rv_sat(Lo); Lo = (Lo*(int16_t)vAPF2>>15) + RB(mLAPF2-dAPF2);
    Ro = Ro - ((int16_t)vAPF2*RB(mRAPF2-dAPF2)>>15); s_rev_buf[rv_i(mRAPF2)] = rv_sat(Ro); Ro = (Ro*(int16_t)vAPF2>>15) + RB(mRAPF2-dAPF2);
    *outL = (Lo * s_rev_vlout) >> 15;
    *outR = (Ro * s_rev_vrout) >> 15;
    s_rev_addr = (s_rev_addr + 2) % REV_AREA;        /* advance ring 1 sample/tick */
}

/* Render both BGM layers (+ SPU reverb) additively into `out` (44100). The
 * reverb processes at 22050 Hz (every 2nd output frame), output held across the
 * pair — matching the SPU's reverb sample rate. */
static void re15_ss_render_bgm(int16_t *out, int frames) {
    if (!s_ss_main.vab_ok && !s_ss_sub.vab_ok) return;
    static int16_t bgm[2048 * 2];
    if (frames > 2048) frames = 2048;
    memset(bgm, 0, (size_t)frames * 2 * sizeof(int16_t));
    ss_mix(&s_ss_main, bgm, frames);
    ss_mix(&s_ss_sub,  bgm, frames);
    static int s_rev_held_l = 0, s_rev_held_r = 0, s_rev_phase = 0;
    for (int f = 0; f < frames; f++) {
        int dryL = bgm[f*2+0], dryR = bgm[f*2+1];
        if (s_rev_phase == 0) ss_spu_reverb(dryL, dryR, &s_rev_held_l, &s_rev_held_r);
        s_rev_phase ^= 1;
        int L = (int)out[f*2+0] + dryL + s_rev_held_l;
        int R = (int)out[f*2+1] + dryR + s_rev_held_r;
        if (L >  32767) L =  32767; else if (L < -32768) L = -32768;
        if (R >  32767) R =  32767; else if (R < -32768) R = -32768;
        out[f*2+0] = (int16_t)L; out[f*2+1] = (int16_t)R;
    }
}

/* == SsVabOpenHead + SsVabTransBody + SsSeqOpen: bind a MAIN bank (VH+VB+SEQ). */
static int re15_ss_load(ss_seq_t *s, const uint8_t *vh, int vh_sz,
                        const uint8_t *vb, int vb_sz, const uint8_t *seq, int seq_sz)
{
    if (re15_vab_parse(vh, (size_t)vh_sz, &s->vab) != 0) return -1;
    ss_decode_vab(s, vb, vb_sz);
    s->vab_ok = 1;
    if (seq_sz <= SS_SEQ_HDR || seq[0] != 'p' || seq[1] != 'Q') return -2;
    uint8_t *sc = (uint8_t *)malloc((size_t)seq_sz);
    if (!sc) return -3;
    memcpy(sc, seq, (size_t)seq_sz);
    s->seq = sc; s->seq_len = seq_sz;
    s->ppqn     = (seq[8] << 8) | seq[9];
    s->tempo_us = (uint32_t)((seq[10] << 16) | (seq[11] << 8) | seq[12]);
    if (s->ppqn <= 0)     s->ppqn = 48;
    if (s->tempo_us == 0) s->tempo_us = 500000;
    fprintf(stderr, "[bgm] loaded: %d VAGs, SEQ %dB, PPQN=%d tempo=%uus\n",
            s->vab.vag_count, seq_sz, s->ppqn, s->tempo_us);
    return 0;
}

/* == SsSeqPlay(id, 0, loop). */
static void re15_ss_play(ss_seq_t *s, int loop) {
    SDL_LockAudioDevice(s_audio_dev);
    ss_all_voices_off(s);
    for (int i = 0; i < SS_CHANNELS; i++) {
        s->prog[i] = 0; s->cvol[i] = 100; s->bend_q16[i] = 0x10000;
    }
    s->cursor = SS_SEQ_HDR; s->rstatus = 0; s->accum = 0;
    s->pending_dt = ss_vlq(s);
    s->loop = loop; s->playing = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* Canonical RE1.5 stage/room → BGM mapping (ported from PSX.EXE static data).
 * RE: room-load FUN_800396fc → BGM selector FUN_800443ec computes
 *   entry = UNK_80074828[room + DAT_800748fc[stage]]   (stage/room 0-based),
 * then loader FUN_80044564 takes  main_slot = entry & 0x3f  and resolves the CD
 * file via DAT_8007498c[slot] (= 0xc9 + slot, i.e. the slot IS the MAIN number).
 * The high byte feeds the SUB layer (FUN_80044774): sub_slot = (entry>>8)&0x3f.
 * 0xff high byte / 0xffff entry = "no track". Tables verified against
 * info/Re1.5/PSX.EXE 2026-05-30. STAGE1/ROOM1170 (stage0,room0x17) → MAIN32. */
static const uint8_t  SS_STAGE_OFF[6] = {0x00, 0x26, 0x32, 0x41, 0x4d, 0x62};
static const uint16_t SS_BGMTBL[106] = {  /* UNK_80074828 @ PSX.EXE 0x80074828 */
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

/* == FUN_800443ec + FUN_80044564/80044774: resolve the MAIN/SUB BGM slots for a
 * room. main = entry&0x3f; sub = (entry>>8)&0x3f (0xff high byte = no sub). -1 none. */
static int ss_bgm_entry(int stage, int room) {
    if (stage < 0 || stage > 5) return -1;
    int idx = room + SS_STAGE_OFF[stage];
    if (idx < 0 || idx >= (int)(sizeof SS_BGMTBL / sizeof SS_BGMTBL[0])) return -1;
    uint16_t e = SS_BGMTBL[idx];
    return (e == 0xffff) ? -1 : (int)e;
}
static int re15_bgm_for_room(int stage, int room) {            /* MAIN slot */
    int e = ss_bgm_entry(stage, room);
    return e < 0 ? -1 : (e & 0x3f);
}
static int re15_bgm_sub_for_room(int stage, int room) {        /* SUB slot */
    int e = ss_bgm_entry(stage, room);
    if (e < 0 || ((e >> 8) & 0xff) == 0xff) return -1;
    return (e >> 8) & 0x3f;
}

/* Load one bank (VH/VB/SEQ) into the given instance. `name` = "MAIN" or "SUB_"
 * (dirs are hex-named: MAIN32/MAIN32.vh, SUB_15/SUB_15.vh). Returns 0 on success. */
static int re15_bgm_load_track(ss_seq_t *s, const char *name, int slot) {
    if (slot < 0) return -1;
    static const char *dirs[] = {
        "../../../../extracted/PSX/SOUND/BGM/",
        "../../../extracted/PSX/SOUND/BGM/",
        "../../extracted/PSX/SOUND/BGM/",
        "extracted/PSX/SOUND/BGM/",
        NULL
    };
    uint8_t *vh=NULL,*vb=NULL,*seq=NULL; int vhs=0,vbs=0,seqs=0; char p[200];
    for (int i = 0; dirs[i] && !vh; i++) {
        snprintf(p,sizeof p,"%s%s%02X/%s%02X.vh",dirs[i],name,slot,name,slot); vh=re15_asset_read_file(p,&vhs);
        if (!vh) continue;
        snprintf(p,sizeof p,"%s%s%02X/%s%02X.vb",dirs[i],name,slot,name,slot); vb=re15_asset_read_file(p,&vbs);
        snprintf(p,sizeof p,"%s%s%02X/%s%02X.seq",dirs[i],name,slot,name,slot); seq=re15_asset_read_file(p,&seqs);
    }
    int rc = -1;
    if (vh && vb && seq) { rc = re15_ss_load(s, vh, vhs, vb, vbs, seq, seqs);
                           s->mvol = 0x1a00; s->mvol_l = 0x1a00; s->mvol_r = 0x1a00; }
    else fprintf(stderr, "[bgm] %s%02X not found\n", name, slot);
    free(vh); free(vb); free(seq);   /* VAGs decoded + SEQ copied; raw no longer needed */
    return rc;
}

/* Init the play state for a loaded instance (no SDL lock — caller holds it / offline). */
static void ss_start(ss_seq_t *s, int loop) {
    if (!s->vab_ok || !s->seq) { s->playing = 0; return; }
    ss_all_voices_off(s);
    for (int i = 0; i < SS_CHANNELS; i++) { s->prog[i]=0; s->cvol[i]=100; s->bend_q16[i]=0x10000; }
    s->cursor = SS_SEQ_HDR; s->rstatus = 0; s->accum = 0;
    s->pending_dt = ss_vlq(s);
    s->loop = loop; s->playing = 1;
}

/* Load the helicopter rotor into the looping ambient voice.
 * RE-determined + acoustically verified (2026-05-30): the rotor is the room's
 * snd0 SE-bank VAG#2 = snd0_00001.wav — it matches the measured PSX ground truth
 * (intrinsic ~11-12.5 Hz blade-pass modulation + 225 Hz tonal; the 11 Hz is in
 * the waveform, not the loop rate). Its VAG loops (block1 0x04 start, last block
 * 0x07 repeat). Reached via snd0.edt SE[29]/[30] → tone1/2, keyed by the heli EM
 * behavior DAT_80073c70[0x40] (dispatcher FUN_8002bd44) — NOT by an SCD Se_on.
 * (Prior wrong guesses: snd0_00004 = crash rumble; CORE00_00002 = wrong bank.)
 * Env overrides for A/B: RE15_ROTOR_WAV, RE15_ROTOR_RATE, RE15_ROTOR_VOL. */
static void re15_amb_load_rotor(int stage, int room) {
    (void)stage; (void)room;
    (void)stage; (void)room;
    const char *e;
    /* Rotor source STILL unconfirmed. STAGE1-overlay RE shows the stage SE banks
     * are snd0 (Leon footsteps/weapons), snd1 (CROWS — the room enemies, type
     * 0x21), CORE (effects), bank0 (common). The heli is the single type-0x47 EM;
     * its sound is a CORE/common SE — exact one not yet user-confirmed. OFF by
     * default (no wrong sound); A/B any WAV via RE15_ROTOR_WAV. */
    const char *rel = getenv("RE15_ROTOR_WAV");
    if (!rel) { s_amb.pcm = NULL; s_amb.active = 0; return; }
    static const char *roots[] = {
        "../../../../extracted/", "../../../extracted/", "../../extracted/", "extracted/", NULL
    };
    uint8_t *wav = NULL; int wsz = 0; char p[256];
    for (int i = 0; roots[i] && !wav; i++) {
        snprintf(p, sizeof p, "%s%s", roots[i], rel);
        wav = re15_asset_read_file(p, &wsz);
    }
    if (!wav) { fprintf(stderr, "[amb] rotor wav not found (%s)\n", rel); return; }

    int rate, ch, bits, dbytes;
    const uint8_t *data = wav_find_data(wav, wsz, &rate, &ch, &bits, &dbytes);
    if (!data || dbytes < 2 || bits != 16 || ch < 1) { free(wav); return; }
    if ((e = getenv("RE15_ROTOR_RATE"))) rate = atoi(e);
    if (rate <= 0) rate = 22050;
    int src_n = (dbytes / 2) / ch;
    const int16_t *src = (const int16_t *)data;
    /* resample to the device rate so it loops 1:1 (int64 — Windows long is 32-bit). */
    int64_t out_n = (int64_t)src_n * RE15_AUDIO_RATE / rate;
    if (out_n < 2) { free(wav); return; }
    int16_t *pcm = (int16_t *)malloc((size_t)out_n * sizeof(int16_t));
    if (!pcm) { free(wav); return; }
    for (int64_t i = 0; i < out_n; i++) {
        int64_t sp = i * (int64_t)rate * 65536 / RE15_AUDIO_RATE;
        int64_t si = sp >> 16; int frac = (int)(sp & 0xffff);
        int64_t a = src[si * ch];
        int64_t b = src[(si + 1 < src_n ? si + 1 : si) * ch];
        pcm[i] = (int16_t)(a + (((b - a) * frac) >> 16));
    }
    free(wav);
    s_amb.pcm = pcm; s_amb.len = (int)out_n; s_amb.loop_start = 0;  /* loop whole buffer */
    s_amb.step = 0x10000;                                           /* already at device rate */
    s_amb.vol = (e = getenv("RE15_ROTOR_VOL")) ? atoi(e) : 0x1600;
    s_amb.pos = 0; s_amb.active = 0;
    fprintf(stderr, "[amb] rotor loaded: %s (%d Hz → %ld @%d)\n", rel, rate, (long)out_n, RE15_AUDIO_RATE);
}

static void re15_amb_mix(int16_t *out, int frames) {
    if (!s_amb.active || !s_amb.pcm || s_amb.len <= 0) return;
    for (int f = 0; f < frames; f++) {
        int idx = (int)(s_amb.pos >> 16);
        if (idx >= s_amb.len) {                       /* loop */
            s_amb.pos = ((uint32_t)s_amb.loop_start << 16) | (s_amb.pos & 0xffff);
            idx = (int)(s_amb.pos >> 16);
            if (idx >= s_amb.len) { s_amb.active = 0; break; }
        }
        int frac = (int)(s_amb.pos & 0xffff);
        int nxt = idx + 1; if (nxt >= s_amb.len) nxt = s_amb.loop_start;
        int32_t s0 = s_amb.pcm[idx], s1 = s_amb.pcm[nxt];
        int32_t smp = s0 + (((s1 - s0) * frac) >> 16);
        smp = (smp * s_amb.vol) >> 15;
        s_amb.pos += s_amb.step;
        int32_t L = (int32_t)out[f*2+0] + smp;
        int32_t R = (int32_t)out[f*2+1] + smp;
        if (L >  32767) L =  32767; else if (L < -32768) L = -32768;
        if (R >  32767) R =  32767; else if (R < -32768) R = -32768;
        out[f*2+0] = (int16_t)L; out[f*2+1] = (int16_t)R;
    }
}

/* re15_bgm: room-music manager (RE2 FUN_8005a97c). Loads + loops MAIN, SUB,
 * and the looping room ambience (helicopter rotor from the snd0 SE bank). */
static void re15_bgm_play_room(int stage, int room) {
    int main_ok = re15_bgm_load_track(&s_ss_main, "MAIN", re15_bgm_for_room(stage, room)) == 0;
    int sub_ok  = re15_bgm_load_track(&s_ss_sub,  "SUB_", re15_bgm_sub_for_room(stage, room)) == 0;
    re15_amb_load_rotor(stage, room);
    SDL_LockAudioDevice(s_audio_dev);
    if (main_ok) ss_start(&s_ss_main, 1);
    if (sub_ok) {
        ss_start(&s_ss_sub, 1);
        /* The SUB layer (helicopter rotor, BGM seq slot 1) is gated 1:1 by the SCD
         * Sce_bgm_control (0x54) opcode — start it MUTED so it is silent until
         * sub02's first SsSeqPlay (op=1) at the heli-arrival cut. Otherwise it would
         * drone through the pre-arrival open. The sequence keeps looping; we toggle
         * its master vol on PLAY/STOP so resumes are seamless (no restart pop). */
        s_ss_sub.mvol = s_ss_sub.mvol_l = s_ss_sub.mvol_r = 0;
    }
    if (s_amb.pcm) s_amb.active = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* Offline render: synth the room's MAIN+SUB layers (+reverb) to a 16-bit stereo
 * WAV via the EXACT render path, for A/B vs the PSX capture. RE15_BGM_DUMP. */
static void re15_bgm_dump_wav(const char *path, int stage, int room, int seconds) {
    int main_ok = re15_bgm_load_track(&s_ss_main, "MAIN", re15_bgm_for_room(stage, room)) == 0;
    int sub_ok  = re15_bgm_load_track(&s_ss_sub,  "SUB_", re15_bgm_sub_for_room(stage, room)) == 0;
    re15_amb_load_rotor(stage, room);
    if (!main_ok && !sub_ok && !s_amb.pcm) { fprintf(stderr,"[bgm] dump: load failed\n"); return; }
    int subonly = getenv("RE15_BGM_SUBONLY") != NULL;   /* isolate the SUB (rotor) layer */
    int mainonly = getenv("RE15_BGM_MAINONLY") != NULL;
    if (main_ok && !subonly)  ss_start(&s_ss_main, 1);
    if (sub_ok && !mainonly)  ss_start(&s_ss_sub, 1);
    if (s_amb.pcm) s_amb.active = 1;

    FILE *f = fopen(path, "wb");
    if (!f) { fprintf(stderr,"[bgm] dump: cannot open %s\n", path); return; }
    long total = (long)seconds * RE15_AUDIO_RATE, data_bytes = total * 4;
    uint8_t h[44]; uint32_t v;
    memcpy(h, "RIFF", 4); v = 36 + data_bytes; memcpy(h+4,&v,4);
    memcpy(h+8, "WAVEfmt ", 8); v = 16; memcpy(h+16,&v,4);
    h[20]=1; h[21]=0; h[22]=2; h[23]=0;                     /* PCM, 2 ch */
    v = RE15_AUDIO_RATE; memcpy(h+24,&v,4);
    v = RE15_AUDIO_RATE*4; memcpy(h+28,&v,4);
    h[32]=4; h[33]=0; h[34]=16; h[35]=0;                    /* block align, bits */
    memcpy(h+36,"data",4); v = data_bytes; memcpy(h+40,&v,4);
    fwrite(h, 1, 44, f);
    int16_t buf[512*2];
    while (total > 0) {
        int n = total < 512 ? (int)total : 512;
        memset(buf, 0, (size_t)n*2*sizeof(int16_t));
        re15_ss_render_bgm(buf, n);
        re15_amb_mix(buf, n);                       /* + the looping rotor */
        fwrite(buf, sizeof(int16_t), (size_t)n*2, f);
        total -= n;
    }
    fclose(f);
    fprintf(stderr, "[bgm] dumped %ds (MAIN+SUB) to %s\n", seconds, path);
}

/* Public: start the looping room BGM (MAIN+SUB). Called once at the pre-intro→
 * cinematic handoff so nothing plays under the narrator pre-intro. Idempotent. */
void re15_audio_start_room_bgm(int stage, int room)
{
    static int started = 0;
    if (!g_audio.initialized || started) return;
    started = 1;
    re15_bgm_play_room(stage, room);
}

/* Gate the helicopter-rotor (SUB layer) volume + PAN by the current cut camera→heli
 * geometry. The L/R volume (0..0x7f) is computed by the SHARED re15_rotor_compute_pan
 * (rotor_common.c, byte-true FUN_80045a64); this backend just scales the SsSeq SUB
 * layer master by it. (Was an independent atan2(double) + mirrored-pan + 127-clamp
 * copy — now unified with the PSX.) */
void re15_audio_rotor_update(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                             const int32_t heli_pos[3])
{
    if (!g_audio.initialized) return;
    int volL, volR;
    re15_rotor_compute_pan(cam_eye, cam_tgt, heli_pos, &volL, &volR);
    s_ss_sub.mvol_l = (s_ss_sub_base_mvol * volL) / 0x7f;
    s_ss_sub.mvol_r = (s_ss_sub_base_mvol * volR) / 0x7f;
    s_ss_sub.mvol   = (s_ss_sub.mvol_l + s_ss_sub.mvol_r) / 2;
}

/* Silence the rotor (SUB layer) — the heli is an intro element; once the
 * cinematic hands off to gameplay (player_mode≠2) the heli has flown off and the
 * SE is keyed off in the original (distance attenuation alone never reaches 0). */
void re15_audio_rotor_silence(void)
{
    if (!g_audio.initialized) return;
    s_ss_sub.mvol_l = s_ss_sub.mvol_r = s_ss_sub.mvol = 0;
}

/* SsSeq slot control (SCD Sce_bgm_control / 0x54 → PSX FUN_80044da4). slot 0 =
 * MAIN room music, slot 1 = SUB helicopter-rotor layer. We toggle the layer's
 * master volume (a looping mute/unmute, sounding identical to SsSeqStop/Play for a
 * sustained drone, with no restart pop). This is the canonical, 1:1 PSX rotor
 * on/off: ROOM1170's sub02 plays slot 1 at the heli-arrival + sky-view cuts and
 * stops it during Leon's dialogue close-ups. */
void re15_audio_seq_ctl(int slot, int op)
{
    if (!g_audio.initialized) return;
    ss_seq_t *s = (slot == 0) ? &s_ss_main : (slot == 1) ? &s_ss_sub : NULL;
    if (!s) return;
    int base = (slot == 1) ? s_ss_sub_base_mvol : 0x1a00; /* MAIN inits to 0x1a00 */
    SDL_LockAudioDevice(s_audio_dev);
    switch (op) {
        case 1:   /* SsSeqSetVol + SsSeqPlay (loop) → audible */
        case 3:   /* SsSeqReplay → audible (restart-from-top; mute-toggle ok for drone) */
            s->mvol = s->mvol_l = s->mvol_r = base;
            break;
        case 2:   /* SsSeqStop → silent */
        case 4:   /* SsSeqPause → silent */
        case 5:   /* SsSeqSetDecrescendo → fade to silent (we cut for now) */
            s->mvol = s->mvol_l = s->mvol_r = 0;
            break;
        default: break;
    }
    SDL_UnlockAudioDevice(s_audio_dev);
}

void re15_audio_tick(void)
{
    if (!g_audio.initialized) return;

    scd_audio_event_t evt;
    while (scd_audio_queue_pop(&evt)) {
        g_audio.events_total++;
        switch ((scd_audio_kind_t)evt.kind) {
            case SCD_AUDIO_SE_ON:
                g_audio.events_se_on++;
                if (s_vab_loaded && s_vag_count > 0) {
                    int idx = (evt.sample_id - 1);
                    if (idx < 0) idx = 0;
                    idx %= s_vag_count;
                    play_sample_pc(idx, evt.volume ? evt.volume : 100);
                }
                break;
            case SCD_AUDIO_VOICE_ON:
                /* RE2-style voice: route through the re15_voice manager →
                 * re15_xa stream. Room fixed to 0x1170 (only room implemented). */
                re15_voice_play(0x1170, evt.sample_id);
                break;
            case SCD_AUDIO_SEQ_CTL:
                /* 0x54 SsSeq slot control — the canonical rotor on/off. */
                re15_audio_seq_ctl(evt.bank, evt.volume);
                break;
            case SCD_AUDIO_BGMTBL_SET: g_audio.events_bgm++;     break;
            case SCD_AUDIO_XA_ON:      g_audio.events_xa_on++;   break;
            case SCD_AUDIO_SE_VOL:     g_audio.events_se_vol++;  break;
            case SCD_AUDIO_NONE:       /* fallthrough */
            default:                   g_audio.events_unknown++; break;
        }
    }
}

/* Player footstep SE — STUB (footstep chain step 3, 2026-06-09). Trigger
 * (re15_actor_footstep, the 0x4000 foot-plant flag) + FLR material lookup are
 * wired in re15_game_step; real snd0/snd1 VAB playback (FUN_80045630) is next.
 * Log the foot-plant so the trigger is verifiable (a step every ~half second
 * while Leon walks, sound_type=29 concrete on the ROOM1170 floor). */
/* Player FOOTSTEP SE (byte-true FUN_80045630): floor sound_type → EDT → snd0
 * tone → VAG → activate a mixer voice. `foot` (7=left/4=right) resolves to the
 * same VAG for ROOM1170 (the original only differs the SE voice slot). Volume =
 * the tone's recorded vol (the original further scales by camera-distance
 * attenuation; the player is at the camera → ≈max). */
void re15_audio_footstep(int foot, int sound_type)
{
    (void)foot;
    if (!s_foot_loaded) return;
    int vag = re15_footstep_vag(s_foot_edt, &s_foot_vab, sound_type);
    if (vag < 0 || !s_foot_decoded[vag]) return;

    int tvol = s_foot_vab.tones[s_foot_edt[sound_type * 4 + 2] >> 4].vol;
    if (!tvol) tvol = 100;
    int vol = (tvol * 0x4000 / 127) >> 1;
    if (vol > 0x4000) vol = 0x4000;
    if (vol < 0) vol = 0;

    SDL_LockAudioDevice(s_audio_dev);
    int slot = -1;
    for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++)
        if (!s_active[i].active) { slot = i; break; }
    if (slot < 0) { slot = s_next_slot; s_next_slot = (s_next_slot + 1) % MIXER_MAX_ACTIVE_SAMPLES; }
    s_active[slot].pcm        = s_foot_decoded[vag];
    s_active[slot].pcm_len    = s_foot_decoded_len[vag];
    s_active[slot].pos        = 0;
    s_active[slot].subpos     = 0;
    s_active[slot].volume_q15 = vol;
    s_active[slot].active     = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

void re15_audio_shutdown(void)
{
    if (s_audio_dev) {
        SDL_PauseAudioDevice(s_audio_dev, 1);
        SDL_CloseAudioDevice(s_audio_dev);
        s_audio_dev = 0;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    g_audio.backend_active = 0;
}
