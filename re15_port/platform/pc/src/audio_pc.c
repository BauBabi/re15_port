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
#include "re15_room.h"
#include "re15_esp.h"     /* re15_esp_shell_clink_hook */     /* g_room_rdt — footstep snd0 VAB sliced from the room RDT */
#include "re2_ems.h"      /* WELLE A: RE2-ENEMSE-Bank-TOC + SE-Map-Dekodierung (PC-only) */

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
    uint32_t step_q16;     /* Q16 Abspielschritt pro 44100-Hz-Ausgabeframe: PSX-Pitch<<4
                            * (SPU-Pitch 0x1000 == 44100 Hz; note2pitch2 @SsUtKeyOnV
                            * @0x8004522c). 0 = Altverhalten 0x8000 (= konstant -12 HT). */
    uint32_t pos_frac;     /* Q16-Restakkumulator des Cursors                            */
    int      active;       /* 1 = playing, 0 = free slot                 */
    int      volume_q15;   /* per-voice LEFT volume in 0..0x4000 (Q15)   */
    int      vol_r_q15;    /* per-voice RIGHT volume (Q15). Tone-Pan tone[+3]: die
                            * SPU-Voice-Maschine liest vol UND pan aus den Tone-Attributen
                            * selbst (SsUtKeyOnV.c: DAT_800b5321/22 = tone[+2]/[+3]) —
                            * pan 0 = hart links, 0x40 = Mitte (beide voll), 0x7f = hart
                            * rechts. Lineares LIBSND-vm-Modell; das CORE11-Announcer-
                            * Paar (pan 0/127) ist damit exakt. */
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

/* Resident WEAPON SE bank (bank1, byte-true FUN_80043d8c parity): the equipped weapon's
 * SOUND/ARMS%02X.EDH + .VB (bank selector 1 in FUN_80045024). The .EDH is [EDT SE-records @0]
 * [VAB VH "pBAV" @pBAV_off][8-byte trailer]; pBAV_off = the u32 at edh[size-8] (= the EDT prefix
 * size, per-file: ARMS01=0x28). The GUNSHOT muzzle = SE index 8 (FUN_80035538 / FUN_80011f50 ->
 * FUN_80045024(0x1080001)). ROOM1140 briefing handgun = weapon 1 = ARMS01 (savestate-confirmed
 * across all STAGE1 saves: DAT_800aca5d==1, 0x801fcd00 == ARMS01.EDH); idx 8 -> prog0/tone4 -> VAG 4.
 * Same EDT->prog/tone->VAG lookup as snd0/snd1, so re15_audio_weapon_se reuses re15_footstep_vag.
 * See RE15_COMBAT_SE_SUBSYSTEM.md §3. */
static int16_t   *s_weap_decoded    [RE15_VAB_MAX_SAMPLES];
static int        s_weap_decoded_len[RE15_VAB_MAX_SAMPLES];
static re15_vab_t s_weap_vab;
static uint8_t   *s_weap_edt   = NULL;   /* owns the EDH buffer; the EDT prefix lives at [0..pBAV_off) */
static int        s_weap_edt_count = 0;  /* EDT record count = pBAV_off/4 (se_id bound) */
static int        s_weap_loaded = 0;
static int        s_weap_id     = -1;

/* The resident CORE SE bank (FUN_80045024 bank selector 4, resident @0x801fbd00 — RAM-matched to
 * SOUND/CORE00.EDH in the STAGE1 briefing save; all CORE0N share the EDT, the diffs are the runtime
 * master-vol bytes Se_on writes into the table). Se_on(0x40NN0001) = CORE EDT record NN. Used by the
 * devour-collapse SEs (FUN_8010a6f8: record 1 at entry, record 3 = the frame-0x37 blood). */
static int16_t   *s_core_decoded    [RE15_VAB_MAX_SAMPLES];
static int        s_core_decoded_len[RE15_VAB_MAX_SAMPLES];
static re15_vab_t s_core_vab;
static uint8_t   *s_core_edt   = NULL;
static int        s_core_edt_count = 0;
static int        s_core_loaded = 0;

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

/* FE-3 opening-movie (CD-XA) audio: the decoded STR soundtrack, mixed like the
 * s_xa voice but STEREO and pre-resampled to the device rate. Separate from
 * s_xa because a movie is stereo + full-length and must not disturb the mono
 * SCD voice channel. Owns its resampled PCM buffer. */
static struct {
    int16_t *pcm;      /* interleaved stereo S16 at the device rate (owned)     */
    int      frames;   /* stereo frame count                                    */
    int      pos;      /* playback cursor (device-rate frames) — the movie clock*/
    int      active;
    int      cd_vol;  /* CdMix CD->SPU per-channel gain / 128; byte-true movie
                       * steady state CdlATV = {0x64,0,0x64,0} = 100/128 (FUN_8002acac
                       * stereo branch; FUN_8002ac84(100)). 'out = in*cd_vol>>7'.     */
} s_fmv_audio;
static int s_dev_freq = 44100;     /* actual device rate (from SDL have.freq)   */
static FILE *s_audio_cap = NULL;   /* RE15_AUDIO_CAP=<raw>: dump mixed output (verify) */
static int   s_audio_cap_sync = 0; /* RE15_AUDIO_CAP_SYNC: frame-locked render, no SDL device */
static long  s_cap_ticks      = 0; /* gerenderte Spielframes in diesem Modus (== PCM/1470)     */
static long  s_cap_limit      = 0; /* RE15_AUDIO_CAP_FRAMES: danach Datei schliessen + exit(0)  */
static int   s_audio_mono = 0;     /* OPTIONS SOUND: 1 = mono (collapse final mix), 0 = stereo */
void re15_audio_set_mono(int mono) { s_audio_mono = mono ? 1 : 0; }

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
    uint8_t         pbmin, pbmax;   /* per-TONE pitch-bend ranges (VagAtr +0xc/+0xd) — SpuVmPBVoice
                                     * scales the bend PER VOICE by these; 0 = pitch pinned.
                                     * [audit wf_1db9c802 BGM-PITCHBEND-PBMINMAX] */
    int             bend_q16;       /* per-voice bend multiplier (Q16, 0x10000 = none) */
} ss_voice_t;

/* One independent SsSeq instance = one VAB soundfont + one SEQ being played.
 * RE2 plays the room's MAIN and SUB tracks SIMULTANEOUSLY (FUN_80044564 +
 * FUN_80044774), so we run two instances and sum them. For ROOM1170 that is
 * MAIN32 (the music) + SUB_15 (the secondary/rotor-ambience layer). */
typedef struct ss_seq_s {
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
    int            bend_deflect[SS_CHANNELS]; /* pitch-bend MSB-64 (byte-true _SsSetPitchBend
                                            * @0x8005ef04 reads ONE 7-bit byte; the per-voice
                                            * pbmin/pbmax scale it — wf_1db9c802 PBMINMAX) */
    uint8_t        bank[SS_CHANNELS];      /* CC0 bank select (byte-true _SsSetControlChange
                                            * case 0 -> seq+0x4c: rebinds the tone lookup to the
                                            * bank's VAB — MAIN id 5 / SUB id 6. wf_1db9c802
                                            * BGM-CC0-VAB-BANKCHANGE) */
    /* Sony in-track loop (CC99 NRPN 0x14 start / 0x1e jump, CC6 count — byte-true
     * _SsContNrpn2/_SsContDataEntry; notes SUSTAIN across the jump. wf_1db9c802 LOOP-NRPN) */
    int            loop_cursor;     /* saved read offset (after the CC99-20 event), -1 = unset */
    int            loop_armed;      /* CC99-20 seen (CC6 then sets the count) */
    int            loop_count;      /* remaining jumps; -1 or 0x7f = infinite */
    int            skip_next_dt;    /* force the post-jump delta to 0 (Sony forces delta 0) */
    int            vab_id;          /* SsVabOpenHead id: MAIN = 5, SUB = 6 (FUN_80044564/44774) */
    struct ss_seq_s *tone_src;      /* tone/VAG source instance (SUB SEQ1 shares the SUB bank) */
    ss_voice_t     voice[SS_MAX_VOICES];
    int            mvol;            /* per-layer master volume (Q15)       */
    int            mvol_l, mvol_r;  /* per-layer L/R master (rotor pan; =mvol if no pan) */
} ss_seq_t;

static ss_seq_t s_ss_main;          /* MAIN<nn> music track                */
static ss_seq_t s_ss_sub;           /* SUB_<nn> secondary/ambience track (SEQ0) */
static ss_seq_t s_ss_sub2;          /* SUB_<nn> SECOND sequence (SEQ1) — a real third SsSeq handle
                                     * (FUN_80044774 opens BOTH: @0x8004494c SEQ0 + @0x8004498c
                                     * SEQ1 at seq_base + *(file_end-8); the SCD 0x54 slot 2
                                     * addresses it. Shares the SUB VAB via tone_src.
                                     * [audit wf_1db9c802 BGM-SUB-SECOND-SEQ] */
static int      s_ss_rate = 44100;
/* BGM LAYER BASE MASTER — the SAME value for all three SsSeq slots, byte-true.
 * FUN_80044774 gives each opened sequence its own SsSeqSetVol from the slot's vol halfword
 * (SEQ0: `lh a1,DAT_800b52b8` @0x80044960 -> `jal 0x8005ab5c` @0x80044970;
 *  SEQ#2: `lh a1,DAT_800b52c0` @0x800449a0 -> `jal 0x8005ab5c` @0x800449b0), and the SCD 0x54 op1
 * re-applies it (`lh a1,DAT_800b52b0+slot*8` @0x80044e00-10 -> `jal 0x8005ab5c` @0x80044e24).
 * MEASURED in the original mid-intro savestate stage_saves/orig_1170_gp.sav (and gp2):
 *   vol@0x800b52b0 (MAIN) = 127, vol@0x800b52b8 (SUB SEQ0) = 127, vol@0x800b52c0 (SUB SEQ#2) = 127,
 *   handles @0x800b52ae/b6/be = 0/1/2 (all three sequences OPEN during the cinematic).
 * i.e. the SUB layer is NOT quieter than MAIN and carries no positional scaling. */
#define SS_BASE_MVOL 0x1a00                   /* port-side Q15 master, shared by MAIN/SUB/SUB2 */
static int      s_ss_sub_base_mvol = SS_BASE_MVOL;  /* == MAIN base (orig: both SetVol 127) */

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
     * Sample-rate: jede Voice traegt ihren Q16-Schritt aus dem Tone-Pitch
     * (SsUtKeyOnV @0x8004522c: note=tone[+6], fine=tone[+5] -> note2pitch2 ->
     * SpuVmKeyOnNow; PSX-Pitch 0x1000 == 44100 Hz -> step = pitch<<4 bei
     * 44100-Hz-Ausgabe). Der alte feste 2x-Downsample (= konstant Pitch 0x800,
     * -12 HT) machte z.B. aus dem 2,5-s-Rolltor-Rumpeln (-39,4 HT, Tone
     * note66/center107/shift105, ROOM1130 snd0 @0x1dd4) einen 0,47-s-Blip.
     * (Dossier analysis/rolltor_sound.md D1, CONFIRMED) */
    for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++) {
        active_sample_t *s = &s_active[i];
        if (!s->active || s->pcm == NULL) continue;
        uint32_t step = s->step_q16 ? s->step_q16 : 0x8000u;

        for (int f = 0; f < frames; f++) {
            if (s->pos >= s->pcm_len) {
                s->active = 0;
                break;
            }
            int vr_q15 = s->vol_r_q15 ? s->vol_r_q15 : s->volume_q15;   /* 0 = legacy mono */
            int32_t vl = ((int32_t)s->pcm[s->pos] * s->volume_q15) >> 15;
            int32_t vr = ((int32_t)s->pcm[s->pos] * vr_q15) >> 15;

            /* Tail fade-out: ramp the last MIXER_TAIL_FADE_SAMPLES PCM
             * samples to zero. Otherwise the abrupt amplitude drop at
             * sample end produces an audible click ("klopfen" with
             * multiple overlapping voices each ending periodically). */
            int rem = s->pcm_len - s->pos;
            if (rem < MIXER_TAIL_FADE_SAMPLES) {
                vl = (vl * rem) / MIXER_TAIL_FADE_SAMPLES;
                vr = (vr * rem) / MIXER_TAIL_FADE_SAMPLES;
            }

            s->pos_frac += step;
            s->pos      += (int)(s->pos_frac >> 16);
            s->pos_frac &= 0xffffu;

            int32_t L = (int32_t)out[f * 2 + 0] + vl;
            int32_t R = (int32_t)out[f * 2 + 1] + vr;
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

    /* FE-3: the opening-movie CD-XA soundtrack (stereo, device-rate). Mixed with
     * the CdMix gain (cd_vol), which the caller ramps for the byte-true fade. */
    if (s_fmv_audio.active && s_fmv_audio.pcm) {
        for (int f = 0; f < frames; f++) {
            if (s_fmv_audio.pos >= s_fmv_audio.frames) { s_fmv_audio.active = 0; break; }
            int32_t l = s_fmv_audio.pcm[s_fmv_audio.pos * 2 + 0];
            int32_t r = s_fmv_audio.pcm[s_fmv_audio.pos * 2 + 1];
            s_fmv_audio.pos++;
            int32_t L = (int32_t)out[f * 2 + 0] + ((l * (int32_t)s_fmv_audio.cd_vol) >> 7);
            int32_t R = (int32_t)out[f * 2 + 1] + ((r * (int32_t)s_fmv_audio.cd_vol) >> 7);
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

    /* OPTIONS SOUND = Mono: collapse the final stereo mix to mono. Byte-true FUN_80043c00 installs the
     * CD-audio cross-mix matrix {0x3f,0x3f,0x3f,0x3f} (= (L+R)/2 to both channels) + software flag
     * 0x800b2678 for mono; here we apply the equivalent to the whole final mix. */
    if (s_audio_mono) {
        for (int f = 0; f < frames; f++) {
            int32_t m = ((int32_t)out[f * 2 + 0] + (int32_t)out[f * 2 + 1]) / 2;
            out[f * 2 + 0] = (int16_t)m; out[f * 2 + 1] = (int16_t)m;
        }
    }

    if (s_audio_cap) fwrite(stream, 1, (size_t)len, s_audio_cap);   /* RE15_AUDIO_CAP verify hook */
}

/* Try to load the bundled test VAB from disk. The PC asset path uses
 * the same several-candidate search pattern as bg_pc to be robust
 * against CWD differences (run from build/, from repo root, etc.). */
static int load_bundled_vab_pc(void)
{
    /* Asset-Pfad-Konsolidierung (2026-07-02): der frühe Bring-up-Test-VAB (test.vh/.vb) ist
     * retired — die alten re15_reborn-Pfade sind entfernt. Diese Kandidaten zeigen nur noch auf
     * die EINE Wurzel shared_assets/PSX; die Dateien existieren dort nicht (Loader bleibt inert),
     * das echte Spiel-Audio läuft über den SOUND/-Pfad weiter unten. */
    static const char *vh_candidates[] = {
        "shared_assets/PSX/DATA/TEST.VH", "../shared_assets/PSX/DATA/TEST.VH",
        NULL
    };
    static const char *vb_candidates[] = {
        "shared_assets/PSX/DATA/TEST.VB", "../shared_assets/PSX/DATA/TEST.VB",
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
    if (!g_room_rdt_ok) { fprintf(stderr, "[foot] kein RDT geparst\n"); return -1; }
    const uint8_t *vh  = g_room_rdt.snd_vh[0];
    const uint8_t *vb  = g_room_rdt.snd_vb[0];
    const uint8_t *edt = g_room_rdt.snd_edt[0];
    int vh_sz = g_room_rdt.snd_vh_size[0], vb_sz = g_room_rdt.snd_vb_size[0],
        edt_sz = g_room_rdt.snd_edt_size[0];
    if (!vh || !vb || !edt || re15_vab_parse(vh, (size_t)vh_sz, &s_foot_vab) != 0) {
        fprintf(stderr, "[foot] Bank snd0 nicht ladbar: vh=%p(%d) vb=%p(%d) edt=%p(%d)\n",
                (const void *)vh, vh_sz, (const void *)vb, vb_sz, (const void *)edt, edt_sz);
        return -1;
    }
    fprintf(stderr, "[foot] Bank snd0 geladen: %d VAGs, vh=%dB vb=%dB edt=%dB\n",
            s_foot_vab.vag_count, vh_sz, vb_sz, edt_sz);
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

/* == FUN_80043eac @0x80043eac + FUN_80043fb0 @0x80043fb0, gerufen aus dem Raumlader
 * FUN_800396fc. Beide schliessen erst die Vorgaengerbank (SsVabClose) und oeffnen dann die
 * Bank des NEUEN Raums aus dem frisch geladenen RDT. Der Port hat kein SPU-RAM, also ist das
 * Gegenstueck zu SsVabClose das Freigeben der dekodierten PCM-Blöcke. */
static void free_room_bank_pcm(void)
{
    for (int i = 0; i < RE15_VAB_MAX_SAMPLES; i++) {
        free(s_foot_decoded[i]); s_foot_decoded[i] = NULL; s_foot_decoded_len[i] = 0;
        free(s_se_decoded[i]);   s_se_decoded[i]   = NULL; s_se_decoded_len[i]   = 0;
    }
    s_foot_loaded = 0;
    s_se_loaded   = 0;
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

/* Activate one mixer voice per resolved SE LAYER (byte-true FUN_80045024 @0x8004516c /
 * FUN_800453d0 @0x8004548c: EDT record byte3 bits 5-7 = extra consecutive tones keyed on with
 * the base tone — the handgun gunshot is TWO layered VAGs. audit wf_1db9c802 AUD-EDT-LAYER-B3HI.
 * PER-TONE PITCH + VOLUME (Dossier analysis/rolltor_sound.md D1/D2, CONFIRMED):
 *  - Pitch: SsUtKeyOnV(voice, vabId, prog, tone, note=tone[+6], fine=tone[+5], voll, volr)
 *    @0x8004522c (lbu 0x6(s1) @0x800451fc / lbu 0x5(s1) @0x80045210) -> note2pitch2 ->
 *    SpuVmKeyOnNow(1, pitch). Exakter Algorithmus: re15_vab_note2pitch2 (fine+shift ADDIEREN).
 *  - Volume non-positional: voll = volr = tone[+2] (LAB_800451dc lbu 0x2(s1) -> DAT_800b2824/26);
 *    Positional-Pfad (flags!=0 -> FUN_80045a64-Panning @0x800451c0-cc) bleibt OFFEN (D3). */
static void se_play_layers(const uint8_t *edt, const re15_vab_t *vab,
                           int16_t *const *decoded, const int *decoded_len, int se_id)
{
    int vags[8], tones[8];
    int n = re15_edt_resolve_layers_ex(edt, vab, se_id, vags, tones, 8);
    if (n <= 0) return;
    SDL_LockAudioDevice(s_audio_dev);
    for (int k = 0; k < n; k++) {
        int vag = vags[k];
        if (vag < 0 || vag >= RE15_VAB_MAX_SAMPLES || !decoded[vag]) continue;
        const re15_vab_tone_t *t = &vab->tones[tones[k]];
        int vol = ((t->vol ? t->vol : 100) * 0x4000 / 127) >> 1;   /* tone[+2]; >>1 = Mixer-Headroom */
        /* Tone-Pan tone[+3] (title_fade_voice.md §2.3): die Voice-Maschine liest vol+pan aus den
         * Tone-Attributen (SsUtKeyOnV.c DAT_800b5321/22 = tone[+2]/[+3]); pan 0 = hart L,
         * 0x40 = Mitte (beide voll), 0x7f = hart R — das CORE11-Announcer-Paar ist tone1 pan 0 /
         * tone2 pan 127 (EDH-Bytes @0x40+0x820, byte-geparst). Lineares vm-Modell. */
        int pan = t->pan;
        int vl = vol, vr = vol;
        if (pan < 0x40)      vr = vol * pan / 0x40;
        else if (pan > 0x40) vl = vol * (0x7f - pan) / 0x3f;
        uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                              t->center_note, t->pitch_shift);
        {   static int se_dbg = -1;
            if (se_dbg < 0) se_dbg = getenv("RE15_SE_DEBUG") ? 1 : 0;
            if (se_dbg)
                fprintf(stderr, "[se] voice: se=%d layer=%d vag=%d note=%u fine=%u center=%u "
                                "vol=%u pitch=0x%03x (%d Hz)\n",
                        se_id, k, vag, t->min_note, t->pitch_shift, t->center_note,
                        t->vol, pitch, (int)((44100u * pitch) >> 12));
        }
        int slot = -1;
        for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++)
            if (!s_active[i].active) { slot = i; break; }
        if (slot < 0) { slot = s_next_slot; s_next_slot = (s_next_slot + 1) % MIXER_MAX_ACTIVE_SAMPLES; }
        s_active[slot].pcm        = decoded[vag];
        s_active[slot].pcm_len    = decoded_len[vag];
        s_active[slot].pos        = 0;
        s_active[slot].subpos     = 0;
        s_active[slot].step_q16   = (uint32_t)pitch << 4;   /* 0x1000 == 44100 Hz Ausgabe */
        s_active[slot].pos_frac   = 0;
        s_active[slot].volume_q15 = vl;
        s_active[slot].vol_r_q15  = vr;
        s_active[slot].active     = 1;
    }
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* Play a room SE by id (byte-true FUN_800453d0 core, PC path). The per-room SE table
 * (snd1 EDT, DAT_800ac778+0x14) maps se_id → program+tone(+layers) → VAG(s). The exact
 * pitch (record byte0) + SPU voice/pan (byte3 low bits) are FAITHFUL-LINE. Used by the
 * C-driven combat (e.g. the zombie death groan, FUN_80107cb0 frame 7 -> 800453d0(rng&1?8:5)). */
void re15_audio_room_se(int se_id)
{
    if (!g_audio.initialized || !s_se_loaded || se_id < 0 || se_id >= 0x19) return;
    se_play_layers(s_se_edt, &s_se_vab, s_se_decoded, s_se_decoded_len, se_id);
}

/* Play a room SE from the snd0 bank (byte-true FUN_80045024 bank 2). Same EDT->prog/tone->VAG
 * layering as room_se (snd1), on the resident snd0 bank the footsteps also use. */
void re15_audio_room_se_snd0(int se_id)
{
    if (!g_audio.initialized || !s_foot_loaded || se_id < 0) return;
    se_play_layers(s_foot_edt, &s_foot_vab, s_foot_decoded, s_foot_decoded_len, se_id);
}

/* Load + decode the resident WEAPON SE bank (bank1) for `weapon_id` from SOUND/ARMS%02X.EDH + .VB
 * (byte-true FUN_80043d8c parity: it loads the equipped-weapon DAT_800aca5d's ARMS bank into
 * 0x801fcd00). The .EDH = [EDT prefix @0][VH "pBAV" @pBAV_off][8-byte trailer]; pBAV_off = the u32
 * at edh[size-8]. We parse the VH from pBAV_off, decode the VAGs from the .VB, and keep the EDH
 * (the EDT prefix @0) for the se_id lookup. Idempotent per weapon; frees the previous bank on change. */
static int load_weapon_se_vab_pc(int weapon_id)
{
    if (weapon_id < 0 || weapon_id > 0xff) return -1;
    if (s_weap_loaded && s_weap_id == weapon_id) return 0;
    for (int i = 0; i < RE15_VAB_MAX_SAMPLES; i++) {
        free(s_weap_decoded[i]); s_weap_decoded[i] = NULL; s_weap_decoded_len[i] = 0;
    }
    free(s_weap_edt); s_weap_edt = NULL; s_weap_edt_count = 0; s_weap_loaded = 0; s_weap_id = -1;

    static const char *dirs[] = { "shared_assets/PSX/SOUND/", "SOUND/", "PSX/SOUND/",
                                  "../shared_assets/PSX/SOUND/", NULL };
    char path[256];
    uint8_t *edh = NULL, *vb = NULL; int edh_sz = 0, vb_sz = 0;
    for (int i = 0; dirs[i] && !edh; i++) {
        snprintf(path, sizeof path, "%sARMS%02X.EDH", dirs[i], weapon_id);
        edh = re15_asset_read_file(path, &edh_sz);
    }
    for (int i = 0; dirs[i] && !vb; i++) {
        snprintf(path, sizeof path, "%sARMS%02X.VB", dirs[i], weapon_id);
        vb = re15_asset_read_file(path, &vb_sz);
    }
    if (!edh || !vb || edh_sz < 8) { free(edh); free(vb); return -1; }
    /* pBAV (VH) offset = the trailer u32 at edh[size-8] (= the EDT-prefix byte size). */
    uint32_t pbav = (uint32_t)edh[edh_sz-8] | ((uint32_t)edh[edh_sz-7] << 8) |
                    ((uint32_t)edh[edh_sz-6] << 16) | ((uint32_t)edh[edh_sz-5] << 24);
    if (pbav + 0x20u > (uint32_t)edh_sz ||
        re15_vab_parse(edh + pbav, (size_t)edh_sz - pbav, &s_weap_vab) != 0) {
        free(edh); free(vb); return -1;
    }
    for (int i = 0; i < s_weap_vab.vag_count; i++) {
        uint32_t off = s_weap_vab.samples[i].offset, sz = s_weap_vab.samples[i].size;
        if (off + sz > (uint32_t)vb_sz) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_weap_decoded[i]     = pcm;
        s_weap_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    free(vb);                          /* the VAGs are decoded; only the EDH (EDT prefix) is kept */
    s_weap_edt       = edh;
    s_weap_edt_count = (int)(pbav / 4);
    s_weap_loaded    = 1;
    s_weap_id        = weapon_id;
    return 0;
}

/* Load + decode a resident CORE SE bank (bank4, SOUND/CORE%02X.EDH + .VB — same EDH layout as the
 * ARMS banks). Byte-true FUN_800440c4(idx): laedt die CD-Files u16[0x80073a88][idx] (EDH) +
 * u16[0x80073ab0][idx] (VB) nach 0x801fbd00 (CORE00..CORE13, hex-benannt). Callsites im Original:
 * Title-Init idx 0x11 (@0x80102704-08 in TITLE.BIN), Game-Start idx DAT_800aca5c (0 Leon / 4 Elza,
 * @0x800316d8-e8 in FUN_800314b0). analysis/title_fade_voice.md §2.3. */
static int s_core_idx = -1;
static int16_t *s_core_old_decoded[RE15_VAB_MAX_SAMPLES];   /* Vorgaenger-Generation: bleibt alloziert,
                                                             * bis die NAECHSTE prime laeuft — eine noch
                                                             * spielende Voice (der 3.96-s-Announcer
                                                             * ueber Fade+Select+Game-Start) liest ihre
                                                             * PCM-Puffer weiter (kein Stop-Call im
                                                             * Original-Pfad) */
static uint8_t *s_core_old_edt = NULL;
static int load_core_se_vab_pc(int idx)
{
    if (s_core_loaded && s_core_idx == idx) return 0;
    static const char *dirs[] = { "shared_assets/PSX/SOUND/", "SOUND/", "PSX/SOUND/",
                                  "../shared_assets/PSX/SOUND/", NULL };
    char path[256];
    uint8_t *edh = NULL, *vb = NULL; int edh_sz = 0, vb_sz = 0;
    for (int i = 0; dirs[i] && !edh; i++) {
        snprintf(path, sizeof path, "%sCORE%02X.EDH", dirs[i], idx);
        edh = re15_asset_read_file(path, &edh_sz);
    }
    for (int i = 0; dirs[i] && !vb; i++) {
        snprintf(path, sizeof path, "%sCORE%02X.VB", dirs[i], idx);
        vb = re15_asset_read_file(path, &vb_sz);
    }
    if (!edh || !vb || edh_sz < 8) { free(edh); free(vb); return -1; }
    uint32_t pbav = (uint32_t)edh[edh_sz-8] | ((uint32_t)edh[edh_sz-7] << 8) |
                    ((uint32_t)edh[edh_sz-6] << 16) | ((uint32_t)edh[edh_sz-5] << 24);
    re15_vab_t nvab;
    if (pbav + 0x20u > (uint32_t)edh_sz ||
        re15_vab_parse(edh + pbav, (size_t)edh_sz - pbav, &nvab) != 0) {
        free(edh); free(vb); return -1;
    }
    /* Generation rotieren: die VOR-vorherige Bank ist sicher still -> freigeben; die aktuelle wird
     * zur alten (Voices darauf spielen aus). Danach erst die neue einhaengen. */
    for (int i = 0; i < RE15_VAB_MAX_SAMPLES; i++) {
        free(s_core_old_decoded[i]);
        s_core_old_decoded[i] = s_core_decoded[i];
        s_core_decoded[i] = NULL; s_core_decoded_len[i] = 0;
    }
    free(s_core_old_edt); s_core_old_edt = (uint8_t *)s_core_edt;
    s_core_vab = nvab;
    for (int i = 0; i < s_core_vab.vag_count; i++) {
        uint32_t off = s_core_vab.samples[i].offset, sz = s_core_vab.samples[i].size;
        if (off + sz > (uint32_t)vb_sz) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_core_decoded[i]     = pcm;
        s_core_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    free(vb);
    s_core_edt       = edh;
    s_core_edt_count = (int)(pbav / 4);
    s_core_loaded    = 1;
    s_core_idx       = idx;
    return 0;
}

/* CORE-Bank explizit laden (byte-true FUN_800440c4-Callsites, s.o.). */
void re15_audio_prime_core(int idx)
{
    if (!g_audio.initialized) return;
    load_core_se_vab_pc(idx);
}

/* Play a CORE-bank SE by EDT record index (byte-true FUN_80045024 bank4: Se_on(0x40NN0001) -> record
 * NN of the resident CORE table @0x801fbd00 -> program+tone -> VAG, same lookup as the room/weapon
 * banks). FAITHFUL-LINE: per-tone volume/pan deferred, like the other SE paths. */
void re15_audio_core_se(int se_id)
{
    if (!g_audio.initialized) return;
    if (!s_core_loaded && load_core_se_vab_pc(0) != 0) return;   /* Default CORE00 (Leon-Ingame) */
    if (se_id < 0 || se_id >= s_core_edt_count) return;
    se_play_layers(s_core_edt, &s_core_vab, s_core_decoded, s_core_decoded_len, se_id);
}

/* Play a WEAPON SE by id (byte-true FUN_80045024 bank1 core, PC path). The equipped weapon's ARMS
 * EDT (bank1) maps se_id -> program+tone -> VAG (identical to re15_footstep_vag). The GUNSHOT is
 * se_id 8. FAITHFUL-LINE: pitch (record byte0 VAB-ID override) + SPU voice/pan (byte3) deferred;
 * the VAG selection + positional-less play is byte-true. Wired at the player fire in game_step. */
void re15_audio_weapon_se(int se_id)
{
    if (!g_audio.initialized || !s_weap_loaded || se_id < 0 || se_id >= s_weap_edt_count) return;
    /* LAYERED (byte-true): ARMS record 0 (the gunshot) = 00 00 13 30 -> byte3 0x30 = 1 extra tone,
     * so the original keys VAG2 AND VAG3 simultaneously; every ARMS bank layers record 0. */
    se_play_layers(s_weap_edt, &s_weap_vab, s_weap_decoded, s_weap_decoded_len, se_id);
}

/* Re-prime bank1 to `weapon_id`'s ARMS bank (byte-true FUN_80043d8c parity) — the weapon-select
 * menu calls this on EQUIP so the next re15_audio_weapon_se plays the newly-equipped weapon's SEs. */
void re15_audio_prime_weapon(int weapon_id)
{
    load_weapon_se_vab_pc(weapon_id);
}

/* ===== 5. Bank-Slot: RE2-Flavor ENEMSE (WELLE A, PORT-OPTION) ====================
 * RE2s globale Gegner-SE-Bank aus shared_assets/RE2/ENEMSE.VBS — Muster wie s_core_vab.
 * Byte-Belege: Bank-TOC @0x800a7b1c (re2_enemse_toc_entry, gen/re2_ems_toc.inc);
 * EDT-Record = [SE-Map @0 .. vh_off) [VAB-VH "pBAV" @vh_off] [Trailer], vh_off =
 * u32 @edt[edt_size-8] (FUN_8005a09c: DAT_800d75ac = base + *(int*)(base-8+size);
 * Bank 0/11 real: vh_off=0x80, Bytes dort 70 42 41 56 = "pBAV" — selbst verifiziert).
 * VBD-Record = der VB-Body (Original: FUN_800132b0 in die SPU; hier ADPCM->PCM). */
static int16_t   *s_re2se_decoded    [RE15_VAB_MAX_SAMPLES];
static int        s_re2se_decoded_len[RE15_VAB_MAX_SAMPLES];
static re15_vab_t s_re2se_vab;
static uint8_t   *s_re2se_edt = NULL;       /* eigener EDT-Record-Puffer (SE-Map @0)   */
static int        s_re2se_map_count = 0;    /* vh_off/4 = SE-Map-Eintraege (Bank11: 32) */
static int        s_re2se_loaded = 0;
static int        s_re2se_bank_sel  = -1;   /* gewaehlte Bank (re15_audio_re2_enemy_bank) */
static int        s_re2se_bank_cur  = -1;   /* tatsaechlich geladene Bank               */

/* ENEMSE.VBS lokalisieren (Nutzer-Entscheidung: shared_assets/RE2/; env-Override). */
static uint8_t *read_re2_enemse_vbs(int *out_sz)
{
    char path[300];
    uint8_t *b = NULL;
    const char *envroot = getenv("RE15_RE2_ASSET_ROOT");
    if (envroot && envroot[0]) {
        snprintf(path, sizeof path, "%s/ENEMSE.VBS", envroot);
        b = re15_asset_read_file(path, out_sz);
    }
#ifdef RE15_ASSET_ROOT_DEFAULT
    if (!b) {
        snprintf(path, sizeof path, "%s/../RE2/ENEMSE.VBS", RE15_ASSET_ROOT_DEFAULT);
        b = re15_asset_read_file(path, out_sz);
    }
#endif
    if (!b) {
        static const char *roots[] = { "shared_assets/RE2/", "../shared_assets/RE2/",
                                       "../../shared_assets/RE2/", "../../../shared_assets/RE2/", NULL };
        for (int i = 0; roots[i] && !b; i++) {
            snprintf(path, sizeof path, "%sENEMSE.VBS", roots[i]);
            b = re15_asset_read_file(path, out_sz);
        }
    }
    return b;
}

static int load_re2_enemy_se_pc(int bank)
{
    if (s_re2se_loaded && s_re2se_bank_cur == bank) return 0;
    re2_enemse_rec_t rec;
    if (re2_enemse_toc_entry(bank, &rec) != 0) return -1;
    int vbs_sz = 0;
    uint8_t *vbs = read_re2_enemse_vbs(&vbs_sz);
    if (!vbs) {
        static int warned = 0;
        if (!warned) { warned = 1;
            fprintf(stderr, "[re2se] shared_assets/RE2/ENEMSE.VBS fehlt -> RE2-Gegner-SEs stumm\n"); }
        return -1;
    }
    if (rec.edt_off + rec.edt_size > (uint32_t)vbs_sz ||
        rec.vbd_off + rec.vbd_size > (uint32_t)vbs_sz || rec.edt_size < 12) {
        free(vbs); return -1;
    }
    /* Vorgaenger-Bank freigeben (Original: SsVabClose @FUN_8005a09c-Kopf). */
    for (int i = 0; i < RE15_VAB_MAX_SAMPLES; i++) {
        free(s_re2se_decoded[i]); s_re2se_decoded[i] = NULL; s_re2se_decoded_len[i] = 0;
    }
    free(s_re2se_edt); s_re2se_edt = NULL; s_re2se_map_count = 0;
    s_re2se_loaded = 0; s_re2se_bank_cur = -1;

    uint8_t *edt = (uint8_t *)malloc(rec.edt_size);
    if (!edt) { free(vbs); return -1; }
    memcpy(edt, vbs + rec.edt_off, rec.edt_size);
    /* VH-Offset = Trailer-u32 @[size-8] (FUN_8005a09c, s.o.). */
    uint32_t vh_off = (uint32_t)edt[rec.edt_size-8]        | ((uint32_t)edt[rec.edt_size-7] << 8)
                    | ((uint32_t)edt[rec.edt_size-6] << 16) | ((uint32_t)edt[rec.edt_size-5] << 24);
    if (vh_off + 0x20u > rec.edt_size ||
        re15_vab_parse(edt + vh_off, (size_t)rec.edt_size - vh_off, &s_re2se_vab) != 0) {
        free(edt); free(vbs); return -1;
    }
    const uint8_t *vb = vbs + rec.vbd_off;
    for (int i = 0; i < s_re2se_vab.vag_count; i++) {
        uint32_t off = s_re2se_vab.samples[i].offset, sz = s_re2se_vab.samples[i].size;
        if (off + sz > rec.vbd_size) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_re2se_decoded[i]     = pcm;
        s_re2se_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    free(vbs);                       /* VAGs dekodiert; nur der EDT-Record (SE-Map+VH) bleibt */
    s_re2se_edt       = edt;
    s_re2se_map_count = (int)(vh_off / 4);
    s_re2se_loaded    = 1;
    s_re2se_bank_cur  = bank;
    fprintf(stderr, "[re2se] ENEMSE Bank %d geladen: %d VAGs, Map %d Eintraege\n",
            bank, s_re2se_vab.vag_count, s_re2se_map_count);
    return 0;
}

void re15_audio_re2_enemy_bank(int bank)
{
    s_re2se_bank_sel = bank;         /* lazy: geladen beim ersten re15_audio_re2_enemy_se */
}

/* RE2-Gegner-SE (byte-true FUN_8005bd6c-Dekodierung, PC-Wiedergabe wie se_play_layers):
 *   - flag2000 -> se_id += 0x10 ("(*param_2 & 0x2000) != 0" am FUN_8005bd6c-Kopf)
 *   - Map-Eintrag @EDT[se_id*4]; 0xFFFFFFFF = stumm ("*(int*)pbVar13 != -1")
 *   - Tone-Adresse VH+0x820 + prog*0x200 + tone*0x20 == re15_vab-Tone [prog*16+tone]
 *   - b3>>5 = ZUSAETZLICHE konsekutive Tones (Schleife am FUN_8005bd6c-Ende: tone+1 je Layer)
 * Nicht uebernommen (dokumentiert): das Kanal-Prioritaets-Gate FUN_8005c92c (b2&0xF) und die
 * SPU-Kanal-Buchhaltung DAT_800d4f18.. — der PC-Mixer hat freie Voices statt fester Kanaele.
 * VAB-Override (b0 bit7) verweist auf eine ANDERE Laufzeit-VAB-Handle-Nummer; der Port hat
 * nur die ENEMSE-VH resident und spielt dann ersatzweise aus ihr (einmalige stderr-Notiz). */
void re15_audio_re2_enemy_se(int se_id, int flag2000)
{
    if (!g_audio.initialized) return;
    if (!s_re2se_loaded) {
        if (s_re2se_bank_sel < 0) {
            static int warned = 0;
            if (!warned) { warned = 1;
                fprintf(stderr, "[re2se] keine ENEMSE-Bank gewaehlt (re15_audio_re2_enemy_bank) -> stumm\n"); }
            return;
        }
        if (load_re2_enemy_se_pc(s_re2se_bank_sel) != 0) return;
    }
    if (flag2000) se_id += 0x10;                       /* zweite Map-Haelfte (Raum-Paar) */
    if (se_id < 0 || se_id >= s_re2se_map_count) return;
    uint32_t entry = (uint32_t)s_re2se_edt[se_id*4]        | ((uint32_t)s_re2se_edt[se_id*4+1] << 8)
                   | ((uint32_t)s_re2se_edt[se_id*4+2] << 16) | ((uint32_t)s_re2se_edt[se_id*4+3] << 24);
    re2_enemse_se_t se;
    re2_enemse_decode_entry(entry, &se);
    if (se.silent) return;
    if (se.vab_override >= 0) {
        static int warned = 0;
        if (!warned) { warned = 1;
            fprintf(stderr, "[re2se] Map-Eintrag mit VAB-Override %d -> spiele aus der ENEMSE-VH\n",
                    se.vab_override); }
    }
    SDL_LockAudioDevice(s_audio_dev);
    for (int k = 0; k <= se.extra; k++) {              /* Basis-Tone + b3>>5 Extra-Layer */
        int tone_idx = se.prog * RE15_VAB_TONES_PER_PROGRAM + se.tone + k;
        if (tone_idx >= RE15_VAB_TOTAL_TONES) break;
        const re15_vab_tone_t *t = &s_re2se_vab.tones[tone_idx];
        int vag = (int)t->vag_index - 1;               /* VH-vag_index ist 1-basiert */
        if (vag < 0 || vag >= RE15_VAB_MAX_SAMPLES || !s_re2se_decoded[vag]) continue;
        int vol = ((t->vol ? t->vol : 100) * 0x4000 / 127) >> 1;   /* wie se_play_layers */
        int pan = t->pan, vl = vol, vr = vol;
        if (pan < 0x40)      vr = vol * pan / 0x40;
        else if (pan > 0x40) vl = vol * (0x7f - pan) / 0x3f;
        uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                              t->center_note, t->pitch_shift);
        int slot = -1;
        for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++)
            if (!s_active[i].active) { slot = i; break; }
        if (slot < 0) { slot = s_next_slot; s_next_slot = (s_next_slot + 1) % MIXER_MAX_ACTIVE_SAMPLES; }
        s_active[slot].pcm        = s_re2se_decoded[vag];
        s_active[slot].pcm_len    = s_re2se_decoded_len[vag];
        s_active[slot].pos        = 0;
        s_active[slot].subpos     = 0;
        s_active[slot].step_q16   = (uint32_t)pitch << 4;
        s_active[slot].pos_frac   = 0;
        s_active[slot].volume_q15 = vl;
        s_active[slot].vol_r_q15  = vr;
        s_active[slot].active     = 1;
    }
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
    s_active[slot].step_q16   = 0;   /* Legacy-Pfad ohne Tone-Kontext -> 0x8000 (Altverhalten) */
    s_active[slot].pos_frac   = 0;
    s_active[slot].volume_q15 = vol;
    s_active[slot].vol_r_q15  = 0;   /* legacy mono (Slot-Reuse: kein Pan-Rest) */
    s_active[slot].active     = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

static void re15_bgm_dump_wav(const char *path, int stage, int room, int seconds);

/* Shell-clink SE (row-VM B=12 first floor contact - FUN_80045024(0x01020001) = ARMS record 2). */
static void pc_shell_clink(void) { re15_audio_weapon_se(2); }
static void pc_bang(void)        { re15_audio_weapon_se(0); }   /* R9: FUN_80045024(0x01000001) */

void re15_audio_init(void)
{
    re15_esp_shell_clink_hook = pc_shell_clink;   /* the row-VM shell bounce SE */
    re15_esp_bang_hook        = pc_bang;          /* the row-VM muzzle bang (tick 2) */
    /* Diagnostic: RE15_BGM_DUMP=<wav> renders the room's MAIN BGM offline (exact
     * synth path) for A/B vs the PSX capture, then exits. No SDL device needed. */
    const char *dump = getenv("RE15_BGM_DUMP");
    if (dump) {
        const char *sec = getenv("RE15_BGM_DUMP_SECS");
        re15_bgm_dump_wav(dump, 0, 0x17, sec ? atoi(sec) : 40);
        exit(0);
    }

    if (g_audio.initialized) return;   /* idempotent: the FE-3 movie inits audio early */

    /* Headless / no-audio-device escape: RE15_NOAUDIO skips the SDL audio device entirely. Needed
     * because SDL_OpenAudioDevice(NULL,...) can BLOCK when the session has no usable audio endpoint
     * (e.g. a disconnected RDP session), which would hang the whole boot before the game loop. All
     * re15_audio_* calls no-op while g_audio stays uninitialised. */
    if (getenv("RE15_NOAUDIO")) {
        fprintf(stderr, "[audio] RE15_NOAUDIO set -> audio disabled\n");
        return;
    }

    /* DETERMINISTISCHE MESS-AUFNAHME (RE15_AUDIO_CAP_SYNC=<raw>): kein SDL-Geraet, statt dessen
     * rendert re15_audio_tick() pro SPIELFRAME exakt RE15_AUDIO_RATE/30 Stereo-Frames durch
     * GENAU DIESELBE Mix-Kette (audio_callback) und schreibt sie in die Datei. Notwendig, weil
     * RE15_AUDIO_CAP am SDL-Callback haengt und damit ECHTZEIT-getaktet ist: die Zuordnung
     * Spielframe -> PCM-Offset ist dort nicht reproduzierbar, ein Mess-Fenster "ROOM1170-Frame
     * F441..F1218" also nicht sauber ausschneidbar. Mit CAP_SYNC gilt Offset = Frame * 1470
     * Stereo-Frames, und derselbe Lauf liefert bit-gleiche Werte.
     * Reine Diagnose (wie RE15_BGM_DUMP / RE15_AUDIO_CAP), kein Spielpfad. */
    { const char *sync = getenv("RE15_AUDIO_CAP_SYNC");
      if (sync && *sync) {
          s_audio_cap = fopen(sync, "wb");
          s_audio_cap_sync = 1;
          { const char *lim = getenv("RE15_AUDIO_CAP_FRAMES");
            if (lim && *lim) s_cap_limit = atol(lim); }
          s_dev_freq = RE15_AUDIO_RATE;
          s_audio_dev = 0;                 /* SDL_LockAudioDevice(0) ist ein No-op */
          load_bundled_vab_pc();
          load_weapon_se_vab_pc(1);
          re15_xa_init();
          g_audio.initialized = 1;
          g_audio.backend_active = 1;
          fprintf(stderr, "[audio] RE15_AUDIO_CAP_SYNC -> %s (%d Frames/Tick, kein SDL-Geraet)\n",
                  sync, RE15_AUDIO_RATE / 30);
          return;
      } }

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

    s_dev_freq = (have.freq > 0) ? have.freq : 44100;   /* actual device rate */
    { const char *cap = getenv("RE15_AUDIO_CAP"); if (cap && *cap) s_audio_cap = fopen(cap, "wb"); }

    /* Unpause — callback fires immediately. */
    SDL_PauseAudioDevice(s_audio_dev, 0);

    /* Phase 4.6.3: load bundled VAB so Se_on events have something to play. */
    load_bundled_vab_pc();
    /* Die RAUM-Baenke (snd0/snd1) werden hier NICHT mehr geladen: re15_audio_init laeuft schon
     * vor dem Opening-Movie, also BEVOR das erste RDT geparst ist (main.c) — g_room_rdt_ok war 0,
     * load_footstep_vab_pc gab sofort -1 zurueck und s_foot_loaded blieb 0. Ergebnis: der
     * Schritt-Sound hat NIE gespielt (gemeldet 2026-08-01, gemessen als "[foot] kein RDT geparst").
     * Im Original haengen beide Baenke am RAUMLADER FUN_800396fc: FUN_80043eac (Bank 2, VH aus
     * RDT+0xc, VB aus RDT+0x10) und FUN_80043fb0 (Bank 3, VB aus RDT+0x1c) — pro Raum, mit
     * SsVabClose der Vorgaengerbank. Der Port macht das jetzt in re15_audio_load_room_banks(). */
    load_weapon_se_vab_pc(1); /* bank1: the briefing handgun ARMS01 (weapon 1, savestate-confirmed);
                               * gunshot = re15_audio_weapon_se(8). FUN_80043d8c loads the equipped
                               * weapon's ARMS bank — per-room/on-equip reload is a follow-up. */

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

/* ── FE-3 opening-movie audio: play the decoded CD-XA soundtrack ──────────────
 * src = interleaved stereo S16 at src_rate (37800 for CAPCOM.STR). We resample
 * to the device rate ONCE here (linear — adequate, and matches the voice path
 * which pre-resamples at load rather than in the callback) into an owned buffer.
 * cd_vol = the CdMix CD->SPU gain (0x3FFF full, == re15_xa_init / CD_initvol).
 * The natural fade-in/out is baked into the ADPCM data itself. */
void re15_fmv_audio_start(const int16_t *src, int src_frames, int src_rate, int cd_vol)
{
    if (!src || src_frames <= 0 || src_rate <= 0) return;
    /* out_frames = src_frames * dev / src (round up so the tail is not clipped). */
    long long of = (long long)src_frames * s_dev_freq;
    int out_frames = (int)((of + src_rate - 1) / src_rate);
    int16_t *buf = (int16_t *) malloc((size_t)out_frames * 2 * sizeof(int16_t));
    if (!buf) return;
    /* linear resample per channel; step in Q16 to avoid float drift. */
    uint64_t step = ((uint64_t)src_rate << 16) / (uint64_t)s_dev_freq;
    uint64_t acc  = 0;
    for (int i = 0; i < out_frames; i++) {
        int   si = (int)(acc >> 16);
        int   fr = (int)(acc & 0xFFFF);
        int   s1 = (si < src_frames) ? si : src_frames - 1;
        int   s2 = (si + 1 < src_frames) ? si + 1 : src_frames - 1;
        for (int ch = 0; ch < 2; ch++) {
            int a = src[s1 * 2 + ch], b = src[s2 * 2 + ch];
            buf[i * 2 + ch] = (int16_t)(a + (((b - a) * fr) >> 16));
        }
        acc += step;
    }
    SDL_LockAudioDevice(s_audio_dev);
    if (s_fmv_audio.pcm) free(s_fmv_audio.pcm);
    s_fmv_audio.pcm     = buf;
    s_fmv_audio.frames  = out_frames;
    s_fmv_audio.pos     = 0;
    s_fmv_audio.cd_vol = cd_vol;
    s_fmv_audio.active  = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

void re15_fmv_audio_stop(void)
{
    SDL_LockAudioDevice(s_audio_dev);
    s_fmv_audio.active = 0;
    if (s_fmv_audio.pcm) { free(s_fmv_audio.pcm); s_fmv_audio.pcm = NULL; }
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* Movie clock: seconds of audio played so far (device-rate cursor / dev freq).
 * -1 when not playing. The FMV video loop follows this so audio is the master. */
double re15_fmv_audio_time(void)
{
    if (!s_fmv_audio.active || s_dev_freq <= 0) return -1.0;
    return (double)s_fmv_audio.pos / (double)s_dev_freq;
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
 * mixer rate (44100). RE1.5 keys voice by (room, Message_on id) — the r<room>_m<id>.xa
 * naming; on PC the bytes come from synchro/STAGE<n>/room<id>/main<id>.wav.
 * The cache is keyed per ROOM (invalidated on room change) so ROOM1150 (Irons),
 * ROOM1240 (intro) and ROOM10D0 get THEIR lines instead of ROOM1170's (the old
 * hardcode served room1170 clips — or silence — everywhere). Returns 1 if a clip
 * exists for this (room, id). */
static uint16_t s_voice_room = 0;      /* the room the clip cache belongs to */

static int re15_voice_load_clip(uint16_t room, int voice_id)
{
    if (voice_id < 0 || voice_id >= VOICE_MAX_MSG) return 0;
    if (s_voice_room != room) {        /* room changed -> drop the whole cache */
        for (int i = 0; i < VOICE_MAX_MSG; i++) {
            free(s_voice_clip[i].pcm);
            s_voice_clip[i].pcm = NULL; s_voice_clip[i].len = 0; s_voice_clip[i].tried = 0;
        }
        s_voice_room = room;
    }
    if (s_voice_clip[voice_id].tried) return s_voice_clip[voice_id].pcm != NULL;
    s_voice_clip[voice_id].tried = 1;
    /* room id packs (stage+1)<<12 | room<<4 | variant: 0x1170 -> STAGE1/room1170. */
    char reldir[64];
    snprintf(reldir, sizeof reldir, "synchro/STAGE%u/room%04X/",
             (unsigned)(room >> 12), (unsigned)room);
    static const char *prefix[] = { "../../../../", "../../../", "../../", "", NULL };
    uint8_t *wav = NULL; int wsz = 0; char path[200];
    for (int i = 0; prefix[i] && !wav; i++) {
        snprintf(path, sizeof path, "%s%smain%02d.wav", prefix[i], reldir, voice_id);
        wav = re15_asset_read_file(path, &wsz);
    }
    /* the synchro dirs are lower-case "room1170" on disk; retry lower-hex if the
     * upper-hex probe missed (case-sensitive mounts). */
    if (!wav) {
        snprintf(reldir, sizeof reldir, "synchro/STAGE%u/room%04x/",
                 (unsigned)(room >> 12), (unsigned)room);
        for (int i = 0; prefix[i] && !wav; i++) {
            snprintf(path, sizeof path, "%s%smain%02d.wav", prefix[i], reldir, voice_id);
            wav = re15_asset_read_file(path, &wsz);
        }
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
    if (!re15_voice_load_clip(room, voice_id)) return;   /* no clip for this (room, id) → silent */
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
        /* Defensiver Clamp — er soll NICHT mehr greifen, darum die Warnung.
         * Aus dem SOUND/*.BGM-Container gelesen (Trailer-Offsets, FUN_80044564 @0x80044564 /
         * FUN_80044774 @0x80044774) deckt sich Summe(VAG) exakt mit dem VB: game-weit
         * 89/89 Container, und bei MAIN32 sind 0 von 5344 ADPCM-Bloecken ungueltig
         * (gegen 90.5% ungueltige Flag-Bytes, wenn der VB 4 Byte zu frueh beginnt).
         * Greift der Clamp doch, stimmt die Container-Aufteilung nicht.
         * (Der frueher hier vermerkte Fall "SUB_15: Tabelle 41504 B, Koerper 41500 B" stammt
         * aus dem alten extracted/-Dreidatei-Pfad, der nie aufloeste — nicht aus diesem.) */
        if (off + sz > (uint32_t)vb_sz) {
            fprintf(stderr, "[bgm] WARNUNG: VAG %d ragt aus dem VB (off=%u sz=%u vb=%d) — "
                            "Container-Aufteilung pruefen\n", i, off, sz, vb_sz);
            sz = (uint32_t)vb_sz - off;
        }
        sz &= ~15u;                              /* whole 16-byte ADPCM blocks */
        if (sz == 0) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s->vag_pcm[i] = pcm;
        s->vag_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
        /* PSX VAG loop semantics (psx-spx soundprocessingunitspu.md:103-141; audit wf_1db9c802
         * AUD-ADPCM-END-FLAG): the voice loops IFF the END block ITSELF (first block with bit
         * 0x01) also carries bit 0x02 (Code 3 = End+Repeat); Code 1 (bit0 without bit1) =
         * End+Mute -> one-shot. The loop start = the LAST block with bit 0x04 at/before the END
         * block (default block 0). The old scan looked at EVERY block: one-shot VAGs are followed
         * by a dummy looper block (flags 0x07 — bits 0x02|0x04!) inside the extent, so 4174/4175
         * VAGs were classified as looping and a held BGM note re-looped a one-shot sample where
         * hardware mutes at the Code-1 END. (The decoder now also STOPS at the END block, so the
         * looper block is never part of the PCM.) */
        int loop_start = 0, repeats = 0;
        for (uint32_t b = 0; b + 16 <= sz; b += 16) {
            uint8_t fl = vb[off + b + 1];
            if (fl & 0x04) loop_start = (int)(b / 16) * 28;   /* last 0x04 so far */
            if (fl & 0x01) {                                   /* the END block decides */
                repeats = (fl & 0x02) ? 1 : 0;
                break;
            }
        }
        s->vag_loop[i] = repeats ? loop_start : -1;
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
/* Per-voice pitch-bend multiplier (byte-true SpuVmPBVoice @0x80057d70): deflect = the bend
 * MSB - 64; up (>=0) scales by the tone's pbmax/63 (@0x80057e44), down by pbmin/64
 * (@0x80057ea8 >>6); range 0 pins the pitch to the base note. */
static int ss_bend_q16(int deflect, int pbmin, int pbmax)
{
    if (deflect == 0) return 0x10000;
    double semis = (deflect >= 0) ? (double)deflect * (double)pbmax / 63.0
                                  : (double)deflect * (double)pbmin / 64.0;
    if (semis == 0.0) return 0x10000;
    return (int)(pow(2.0, semis / 12.0) * 65536.0 + 0.5);
}

static void ss_note_on(ss_seq_t *s, int chan, int note, int vel)
{
    /* TONE SOURCE: CC0 bank select rebinds the lookup to the MAIN bank (byte-true
     * _SsSetControlChange case 0 -> seq+0x4c, consumed by _SsNoteOn `lh a1,76(s0)` as the
     * key-on vab id — MAIN=5). SUB SEQ1 shares the SUB bank via tone_src. */
    ss_seq_t *src = s->tone_src ? s->tone_src : s;
    if (s->bank[chan] == 5 && src != &s_ss_main && s_ss_main.vab_ok) src = &s_ss_main;

    /* ALL matching tones are keyed, one voice each (byte-true SpuVmKeyOn @0x800585e4-866c —
     * stacked/chorus instruments; the port used to key only the first match). */
    const re15_vab_tone_t *tn[8];
    int nt = re15_vab_match_tones(&src->vab, s->prog[chan], note, tn, 8);
    for (int k = 0; k < nt; k++) {
        const re15_vab_tone_t *t = tn[k];
        int vi = (int)t->vag_index - 1;             /* vag_index is 1-based */
        if (vi < 0 || vi >= src->vab.vag_count || !src->vag_pcm[vi] || src->vag_len[vi] <= 0)
            continue;
        int slot = -1, q = 0x7fffffff;
        for (int i = 0; i < SS_MAX_VOICES; i++) {
            if (!s->voice[i].active) { slot = i; break; }
            if (s->voice[i].env_level < q) { q = s->voice[i].env_level; slot = i; }  /* steal quietest */
        }
        ss_voice_t *v = &s->voice[slot];
        v->pcm = src->vag_pcm[vi]; v->pcm_len = src->vag_len[vi]; v->loop_start = src->vag_loop[vi];
        v->phase = 0;
        uint16_t pitch = re15_vab_note2pitch(note, t->center_note, t->pitch_shift);
        v->step = (uint32_t)pitch << 4;             /* Q16: pitch 0x1000 = 1.0 */
        /* volume chain incl. the RUNTIME-writable program mvol + VAB master vol (the 0x54
         * Sce_bgm_control payload writes these — FUN_80044da4; note-ons re-read them.
         * wf_1db9c802 SEQCTL-VOLPAN). */
        int vol = (int)t->vol * vel / 127;
        vol = vol * s->cvol[chan] / 127;
        vol = vol * (int)src->vab.prog_mvol[s->prog[chan]] / 127;
        vol = vol * (int)src->vab.master_volume / 127;
        int q15 = vol * 0x7fff / 127;
        int pan = t->pan; if (pan < 0) pan = 64; if (pan > 127) pan = 127;
        if (pan >= 56 && pan <= 72) { v->vol_l = q15; v->vol_r = q15; }
        else { v->vol_l = q15 * (127 - pan) / 127; v->vol_r = q15 * pan / 127; }
        v->adsr1 = t->adsr1; v->adsr2 = t->adsr2;   /* real SPU ADSR from the VAB tone */
        v->env_level = 0; v->env_phase = 0; v->env_count = 0;   /* start in attack */
        v->pbmin = t->pbmin; v->pbmax = t->pbmax;
        v->bend_q16 = ss_bend_q16(s->bend_deflect[chan], t->pbmin, t->pbmax);
        v->chan = (uint8_t)chan; v->note = (uint8_t)note; v->active = 1;
    }
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
    } else if (typ == 0xE0) {            /* pitch bend — value = the MSB ONLY (byte-true
                                          * _SsSetPitchBend @0x8005ef04 reads one 7-bit byte,
                                          * center 64; the LSB is stream-skipped). Per-voice
                                          * scaling by the tone's pbmin/pbmax. */
        s->cursor++;                     /* LSB: consumed, ignored */
        int msb = s->seq[s->cursor++] & 0x7f;
        s->bend_deflect[ch] = msb - 64;
        for (int i = 0; i < SS_MAX_VOICES; i++)
            if (s->voice[i].active && s->voice[i].chan == ch)
                s->voice[i].bend_q16 = ss_bend_q16(s->bend_deflect[ch],
                                                   s->voice[i].pbmin, s->voice[i].pbmax);
    } else if (typ == 0xA0 || typ == 0xB0) {
        int d1 = s->seq[s->cursor++];
        int d2 = s->seq[s->cursor++];
        if (typ == 0xB0) {
            if (d1 == 7)  s->cvol[ch] = d2;           /* CC7 channel volume */
            else if (d1 == 0) s->bank[ch] = (uint8_t)d2;  /* CC0 bank select (-> seq+0x4c) */
            else if (d1 == 99) {                      /* Sony NRPN loop markers (_SsContNrpn2) */
                if (d2 == 0x14) {                     /* LOOP START: save the post-event cursor */
                    s->loop_cursor = s->cursor;
                    s->loop_armed  = 1;
                } else if (d2 == 0x1e && s->loop_cursor >= 0) {   /* LOOP JUMP */
                    int jump = 1;
                    if (s->loop_count >= 0 && s->loop_count != 0x7f) {
                        if (s->loop_count > 0) s->loop_count--;
                        else jump = 0;                /* finite count exhausted */
                    }
                    if (jump) {
                        s->cursor       = s->loop_cursor;   /* notes SUSTAIN across the jump */
                        s->skip_next_dt = 1;                /* Sony forces the post-jump delta 0 */
                    }
                }
            } else if (d1 == 6 && s->loop_armed) {    /* CC6 Data Entry = loop count (0x7f inf) */
                s->loop_count = d2;
                s->loop_armed = 0;
            }
        }
    } else if (st == 0xFF) {
        int meta = s->seq[s->cursor++];
        int len  = s->seq[s->cursor++];
        if (meta == 0x51 && len == 3)
            s->tempo_us = (uint32_t)((s->seq[s->cursor] << 16) |
                          (s->seq[s->cursor+1] << 8) | s->seq[s->cursor+2]);
        s->cursor += len;
        if (meta == 0x2F) {                          /* end of track */
            /* whole-file wrap = the fallback only (the in-track CC99 loop is the normal path);
             * byte-true _SsGetMetaEvent's loop branch restarts WITHOUT keying voices off. */
            if (s->loop) { s->cursor = SS_SEQ_HDR; s->rstatus = 0; }
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
        if (s->skip_next_dt) { s->pending_dt = 0; s->skip_next_dt = 0; }   /* post-loop-jump delta 0 */
    }
}

static void ss_mix(ss_seq_t *s, int16_t *out, int frames) {
    if (!s->vab_ok) return;
    ss_advance(s, frames);
    for (int i = 0; i < SS_MAX_VOICES; i++) {
        ss_voice_t *v = &s->voice[i];
        if (!v->active || !v->pcm) continue;
        /* per-VOICE pitch-bend applied to the base step (Q16 × Q16 → Q16) — the bend deflect is
         * scaled by each voice's own tone pbmin/pbmax (SpuVmPBVoice). */
        uint32_t eff_step = (uint32_t)(((uint64_t)v->step *
                              (uint32_t)(v->bend_q16 ? v->bend_q16 : 0x10000)) >> 16);
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
/* ── BGM-Ausblendung beim Track-Wechsel ──────────────────────────────────────
 * Byte-true FUN_80044ab8 @0x80044ab8 (Frame-Tick aus FUN_800458d4), angestossen von
 * FUN_800449f4 @0x800449f4 (DAT_800b5218 = 1, DAT_800b5360 = 0x3c) aus FUN_800443ec.
 *
 *   state 1: einmal   v -= v/n fuer alle 16 Stimmen (SsUtGetDetVVol/SsUtSetDetVVol,
 *                     Clamp >= 0), dann state = 2, n -= 1
 *   state 2: dieselbe Schleife, n -= 1, bei n == 1 -> state = 3
 *   state 3: n = 0, state = 0, und SsSeqStop fuer jeden der DREI Slots mit Flag != 0
 *            (@0x80044b8c-@0x80044bb0)
 *
 * n laeuft von 0x3c=60 abwaerts, also teleskopiert das Produkt: v_k = v0 * (60-k)/60.
 * Das ist eine LINEARE Rampe ueber 59 Frames auf 1/60, im 60. Frame der harte Stop.
 * Nachgerechnet gegen die Formel: k=15 -> 0.75, k=30 -> 0.50, k=45 -> 0.25.
 *
 * Der Port hat keine SPU-Stimmen-Detune-Volumes, also faehrt derselbe Integer-Ausdruck
 * hier auf einem Skalar ueber den ganzen BGM-Mix — gleiche Rampe, gleiche Frame-Zahl. */
#define BGM_FADE_FRAMES 0x3c              /* FUN_800443ec: FUN_800449f4(0x3c) */
static int s_bgm_fade_state = 0;          /* DAT_800b5218 */
static int s_bgm_fade_n     = 0;          /* DAT_800b5360 */
static int s_bgm_fade_vol   = 0x8000;     /* Skalar-Ersatz der 16 Stimmen-Volumes, 1.0 = 0x8000 */

static void re15_ss_render_bgm(int16_t *out, int frames) {
    if (!s_ss_main.vab_ok && !s_ss_sub.vab_ok) return;
    static int16_t bgm[2048 * 2];
    if (frames > 2048) frames = 2048;
    memset(bgm, 0, (size_t)frames * 2 * sizeof(int16_t));
    ss_mix(&s_ss_main, bgm, frames);
    ss_mix(&s_ss_sub,  bgm, frames);
    ss_mix(&s_ss_sub2, bgm, frames);   /* the SUB bank's SECOND sequence (SCD slot 2) */
    const int fade = s_bgm_fade_vol;   /* Stimmen-Volume-Rampe der Ausblendung */
    static int s_rev_held_l = 0, s_rev_held_r = 0, s_rev_phase = 0;
    for (int f = 0; f < frames; f++) {
        int dryL = bgm[f*2+0], dryR = bgm[f*2+1];
        if (fade != 0x8000) {          /* == SsUtSetDetVVol auf allen BGM-Stimmen */
            dryL = (dryL * fade) >> 15;
            dryR = (dryR * fade) >> 15;
        }
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
        s->prog[i] = 0; s->cvol[i] = 100;
        s->bend_deflect[i] = 0;                       /* bend MSB-64, per-voice scaled */
        s->bank[i] = (uint8_t)s->vab_id;              /* CC0 default = the own bank id */
    }
    s->loop_cursor = -1; s->loop_armed = 0; s->loop_count = 0x7f; s->skip_next_dt = 0;
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
static uint32_t bgm_u32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* SUB-Container fuehren eine ZWEITE Sequenz; hier geparkt fuer re15_bgm_play_room. */
static uint8_t *s_sub_seq2     = NULL;
static int      s_sub_seq2_len = 0;

static int re15_bgm_load_track(ss_seq_t *s, const char *name, int slot) {
    if (slot < 0) return -1;
    /* AUSGELIEFERTES FORMAT: EIN Container je Track unter SOUND/, nicht drei Dateien unter einem
     * extracted/-Baum (der alte Pfad suchte MAIN%02X/MAIN%02X.{vh,vb,seq} unter
     * "extracted/PSX/SOUND/BGM/" — ein extracted/-Verzeichnis existiert im Repo NIRGENDS, die BGM
     * hat deshalb nie gespielt).
     *
     * DIE AUFTEILUNG STEHT IN EINEM TRAILER AM DATEIENDE — sie wird NICHT gerechnet.
     *
     * MAIN, FUN_80044564 @0x80044564:
     *     piVar5 = (int *)(base + size + -8);  iVar6 = *piVar5;          // u32 @size-8  = VH-Offset
     *     piVar5 = piVar5 + -1;  puVar7 = base + *piVar5;                // u32 @size-12 = VB-Offset
     *     FUN_8004ee38(&DAT_801f5500, base, *piVar5);                    // memcpy [0 .. VB) = SEQ+VH
     *     SsVabOpenHeadSticky(&DAT_801f5500 + iVar6, 5, 0x42fc0);        // VH  = Puffer + VH-Offset
     *     SsVabTransBody(puVar7, ...);                                   // VB  = Ladepuffer + VB-Offset
     *     SsSeqOpen((ulong *)&DAT_801f5500, ...);                        // SEQ = [0 .. VH-Offset)
     *
     * SUB, FUN_80044774 @0x80044774 — ein Eintrag mehr, alles um ein u32 verschoben:
     *     iVar3  = *(int *)(iVar5 + -8);      // u32 @size-8  = Offset der ZWEITEN Sequenz
     *     iVar6  = *(int *)(iVar5 + -0xc);    // u32 @size-12 = VH-Offset
     *     puVar7 = base + *(int *)(iVar5 + -0x10);   // u32 @size-16 = VB-Offset
     *     SsSeqOpen(&DAT_801eed00, ...);              SsSeqPlay(...);    // SEQ #1 @0
     *     SsSeqOpen(&DAT_801eed00 + iVar3, ...);      SsSeqPlay(...);    // SEQ #2 @Trailer-Offset
     *
     * WARUM DAS ZAEHLT: zwischen VH-Ende und VB liegen 4 Byte Pad. Die alte Rechnung
     * vb = pBAV + (fsize - vb_total_bytes) verschluckt sie und beginnt den VB 4 Byte ZU FRUEH —
     * ab dem zweiten 16-Byte-ADPCM-Block wird Datenmitte als Shift/Filter-Kopfbyte gelesen.
     * Zensus mit re15_port/tools/bgm_container_probe.py ueber alle ausgelieferten Container:
     * 89 echte (61 MAIN + 28 SUB), 0 Magic-Fehler, 89/89 fsize == VH-Formel + Summe(VAG),
     * 89/89 Differenz Trailer-VB minus gerechnetem VB = exakt +4. Dazu 39 Platzhalter < 0x40 B
     * (z.B. MAIN3C.BGM = 4 Byte) — daher die sz-Wache unten. */
    char path[256];
    int sz = 0;
    snprintf(path, sizeof path, "SOUND/%s%02X.BGM", name, slot);
    uint8_t *blob = re15_asset_read_file(path, &sz);
    if (!blob || sz < 0x40) { free(blob); fprintf(stderr, "[bgm] %s%02X.BGM nicht lesbar\n", name, slot); return -1; }

    const int is_sub = (name[0] == 'S');
    uint32_t vh_off, vb_off, seq1_len, seq2_off = 0;
    if (is_sub) {                                   /* FUN_80044774 */
        seq2_off = bgm_u32(blob + sz - 8);
        vh_off   = bgm_u32(blob + sz - 12);
        vb_off   = bgm_u32(blob + sz - 16);
        seq1_len = seq2_off;
    } else {                                        /* FUN_80044564 */
        vh_off   = bgm_u32(blob + sz - 8);
        vb_off   = bgm_u32(blob + sz - 12);
        seq1_len = vh_off;
    }

    /* Der Trailer muss zu dem passen, was in der Datei steht — sonst ist es kein Container
     * dieser Bauart und wir fallen auf die pBAV-Suche zurueck, statt Muell zu dekodieren. */
    int trailer_ok =
        seq1_len > (uint32_t)SS_SEQ_HDR && vh_off > 0 && vh_off < vb_off && vb_off < (uint32_t)sz &&
        seq1_len <= vh_off && (!is_sub || (seq2_off < vh_off && seq2_off >= (uint32_t)SS_SEQ_HDR)) &&
        blob[0] == 'p' && blob[1] == 'Q' && blob[2] == 'E' && blob[3] == 'S' &&
        blob[vh_off] == 'p' && blob[vh_off+1] == 'B' && blob[vh_off+2] == 'A' && blob[vh_off+3] == 'V' &&
        (!is_sub || (blob[seq2_off] == 'p' && blob[seq2_off+1] == 'Q' &&
                     blob[seq2_off+2] == 'E' && blob[seq2_off+3] == 'S'));

    if (!trailer_ok) {
        int scan = -1;
        for (int i = 0; i + 4 <= sz; i++)
            if (blob[i]=='p' && blob[i+1]=='B' && blob[i+2]=='A' && blob[i+3]=='V') { scan = i; break; }
        if (scan <= 0) { fprintf(stderr, "[bgm] %s%02X.BGM: kein pBAV\n", name, slot); free(blob); return -1; }
        fprintf(stderr, "[bgm] %s%02X.BGM: Trailer unplausibel (vh=%u vb=%u seq2=%u sz=%d)"
                        " — Rueckfall auf pBAV-Suche @%d\n", name, slot, vh_off, vb_off, seq2_off, sz, scan);
        vh_off = (uint32_t)scan; seq1_len = (uint32_t)scan; seq2_off = 0;
        vb_off = 0;                                  /* unten aus dem VH-Kopf nachgezogen */
    }

    re15_vab_t probe;
    size_t vh_span = (vb_off > vh_off) ? (size_t)(vb_off - vh_off) : (size_t)(sz - (int)vh_off);
    if (re15_vab_parse(blob + vh_off, vh_span, &probe) != 0) {
        fprintf(stderr, "[bgm] %s%02X.BGM: VAB-Kopf unlesbar\n", name, slot); free(blob); return -1;
    }
    if (vb_off == 0) {                               /* nur im Rueckfall: alte Rechnung */
        int vh_sz = (int)probe.file_size - probe.vb_total_bytes;
        if (vh_sz <= 0 || (int)vh_off + vh_sz > sz) vh_sz = sz - (int)vh_off;
        vb_off = vh_off + (uint32_t)vh_sz;
    }

    /* SsVabTransBody uebertraegt genau die Summe der VAG-Groessen (SsVabOpenHead legt sie in
     * DAT_800bbda8[vabid] ab) — nicht "bis Dateiende": dahinter liegt der Trailer. */
    int vbs = probe.vb_total_bytes;
    if (vbs <= 0 || (int)vb_off + vbs > sz) vbs = sz - (int)vb_off;

    /* SEQ #2 des SUB-Containers fuer den Aufrufer sichern (FUN_80044774 oeffnet BEIDE). */
    free(s_sub_seq2); s_sub_seq2 = NULL; s_sub_seq2_len = 0;
    if (is_sub && seq2_off > 0 && vh_off > seq2_off) {
        int len2 = (int)(vh_off - seq2_off);
        uint8_t *c2 = (uint8_t *)malloc((size_t)len2);
        if (c2) { memcpy(c2, blob + seq2_off, (size_t)len2); s_sub_seq2 = c2; s_sub_seq2_len = len2; }
    }

    int rc = re15_ss_load(s, blob + vh_off, (int)vh_span, blob + vb_off, vbs, blob, (int)seq1_len);
    if (rc == 0) { s->mvol = 0x1a00; s->mvol_l = 0x1a00; s->mvol_r = 0x1a00; }
    else fprintf(stderr, "[bgm] %s%02X.BGM: ss_load fehlgeschlagen\n", name, slot);
    free(blob);   /* VAGs dekodiert + SEQ kopiert */
    return rc;
}


/* Init the play state for a loaded instance (no SDL lock — caller holds it / offline).
 * An instance with tone_src (SUB SEQ1) has no own VAB — the source's counts. */
static void ss_start(ss_seq_t *s, int loop) {
    if ((!s->vab_ok && !s->tone_src) || !s->seq) { s->playing = 0; return; }
    ss_all_voices_off(s);
    for (int i = 0; i < SS_CHANNELS; i++) {
        s->prog[i] = 0; s->cvol[i] = 100;
        s->bend_deflect[i] = 0;
        s->bank[i] = (uint8_t)s->vab_id;
    }
    s->loop_cursor = -1; s->loop_armed = 0; s->loop_count = 0x7f; s->skip_next_dt = 0;
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
 * Env overrides for A/B: RE15_ROTOR_WAV, RE15_ROTOR_RATE, RE15_ROTOR_VOL.
 *
 * ⚠ WIDERLEGT (2026-08-17) — ALLES OBERHALB DIESER ZEILE IST EINE ALTE HYPOTHESE, KEIN BELEG.
 * Der Rotor ist SUB_15.BGM SEQ0: FUN_80044774 oeffnet die Sequenz und spielt sie
 * (@0x8004494c / SsSeqSetVol @0x80044960-74 / SsSeqPlay @0x80044980), der SCD-0x54-op1 dreht sie
 * auf (@0x80044e00-44). FUN_8002bd44 ist der generische Objekt-/AI-Dispatch-Loop des Haupt-Updates
 * (@0x8001ce14), kein SE-Keyer. Diese amb_-Schleife ist damit KEIN Spiel-Pfad mehr, sondern nur
 * noch ein manuelles A/B-Werkzeug — sie bleibt per Default AUS (ohne RE15_ROTOR_WAV kein Ton). */
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

/* Bank freigeben, BEVOR neu geladen wird. Ohne das leckt jeder Raumwechsel die dekodierten
 * VAGs + die SEQ-Kopie — der Nachlade-Pfad ist im Port bis 2026-08-01 nie gefahren worden. */
static void ss_free_bank(ss_seq_t *s) {
    for (int i = 0; i < RE15_VAB_MAX_SAMPLES; i++) {
        free(s->vag_pcm[i]); s->vag_pcm[i] = NULL; s->vag_len[i] = 0; s->vag_loop[i] = -1;
    }
    free((void *)(uintptr_t)s->seq); s->seq = NULL; s->seq_len = 0;
    s->vab_ok = 0; s->playing = 0;
    memset(&s->vab, 0, sizeof s->vab);
}

/* ALLE Lade-/Start-Helfer unten setzen voraus, dass der Aufrufer SDL_LockAudioDevice haelt:
 * ss_free_bank gibt PCM frei, aus dem der Mix-Callback liest, und ss_start lockt selbst nicht. */

/* == FUN_80044564 @0x80044564: MAIN-Bank laden + starten. Flag = main_b>>6. */
static int re15_bgm_load_main(uint8_t main_b) {
    ss_free_bank(&s_ss_main);
    if (main_b == 0xff) return -1;                       /* @0x80044598: *DAT_800b5570 == 0xff */
    if (re15_bgm_load_track(&s_ss_main, "MAIN", main_b & 0x3f) != 0) return -1;
    s_ss_main.vab_id = 5;                                /* SsVabOpenHeadSticky(.., 5, 0x42fc0) */
    ss_start(&s_ss_main, 1);
    /* FUN_800444b0 @0x800444b0 spielt einen Slot NUR, wenn sein Flag == 0 ist (DAT_800b52ad).
     * FUN_80044210 setzt das Flag auf main_b>>6 (@0x800442D0). Flag != 0 = geladen, aber stumm,
     * bis das Skript ihn per Sce_bgm_control (0x54) aufdreht. */
    if ((main_b >> 6) != 0) s_ss_main.mvol = s_ss_main.mvol_l = s_ss_main.mvol_r = 0;
    return 0;
}

/* == FUN_80044774 @0x80044774: SUB-Bank laden + BEIDE Sequenzen starten.
 * Flags: SUB-A = (sub_b>>6)&1 (DAT_800b52b5), SUB-B = sub_b>>7 (DAT_800b52bd) — @0x8004439c/@0x800443a4. */
static int re15_bgm_load_sub(uint8_t sub_b) {
    ss_free_bank(&s_ss_sub);
    ss_free_bank(&s_ss_sub2);
    s_ss_sub2.tone_src = NULL;
    if (sub_b == 0xff) return -1;                        /* @0x800447c8: DAT_800b5570[1] == -1 */
    if (re15_bgm_load_track(&s_ss_sub, "SUB_", sub_b & 0x3f) != 0) return -1;
    s_ss_sub.vab_id = 6;                                 /* SsVabOpenHeadSticky(.., 6, ..+DAT_800bbdec) */
    ss_start(&s_ss_sub, 1);
    if (((sub_b >> 6) & 1) != 0) s_ss_sub.mvol = s_ss_sub.mvol_l = s_ss_sub.mvol_r = 0;

    /* SEQ#2 — die zweite Sequenz des Containers, ein echtes drittes SsSeq-Handle. FUN_80044774
     * oeffnet BEIDE: SsSeqOpen(&DAT_801eed00, ..) und SsSeqOpen(&DAT_801eed00 + iVar3, ..), je
     * gefolgt von SsSeqSetVol(.,0,0) + SsSeqPlay. Der SCD-0x54-Slot 2 adressiert sie.
     * Grenze aus dem Trailer (s_sub_seq2) statt aus einer pQES-Suche im Blob.
     * Teilt sich die SUB-VAB ueber tone_src. */
    if (s_sub_seq2 && s_sub_seq2_len > SS_SEQ_HDR) {
        uint8_t *sc = (uint8_t *)malloc((size_t)s_sub_seq2_len);
        if (sc) {
            memcpy(sc, s_sub_seq2, (size_t)s_sub_seq2_len);
            s_ss_sub2.seq = sc; s_ss_sub2.seq_len = s_sub_seq2_len;
            s_ss_sub2.ppqn     = (sc[8] << 8) | sc[9];
            s_ss_sub2.tempo_us = (uint32_t)((sc[10] << 16) | (sc[11] << 8) | sc[12]);
            if (s_ss_sub2.ppqn <= 0)     s_ss_sub2.ppqn = 48;
            if (s_ss_sub2.tempo_us == 0) s_ss_sub2.tempo_us = 500000;
            s_ss_sub2.vab_ok = 0; s_ss_sub2.tone_src = &s_ss_sub; s_ss_sub2.vab_id = 6;
            /* BASIS-mvol — fehlte komplett (Nutzer-Report "Helikopter-Rotor unhoerbar in ROOM1170",
             * 2026-08-17): s_ss_sub2 laeuft NICHT durch re15_bgm_load_track (das setzt mvol
             * @audio_pc.c ~L1900), also blieb sein Master statisch 0 -> SEQ#2 war PERMANENT stumm
             * (gemessene RMS im Narrator-Fenster exakt 0.0). Im Original bekommt SEQ#2 dieselbe
             * SsSeqSetVol-Behandlung wie SEQ0: `lh a1,DAT_800b52c0` @0x800449a0 + `jal 0x8005ab5c`
             * @0x800449b0 + `jal 0x80060030` (SsSeqPlay) @0x800449c0, mit vol@0x800b52c0 = 127
             * (Savestate stage_saves/orig_1170_gp.sav) = derselbe Wert wie MAIN/SEQ0.
             * Und SEQ#2 laeuft ab RAUM-EINTRITT durch: Flag B = 0 -> der Auto-Play-Gate
             * FUN_800444b0 laesst ihn spielen, schon unterm Narrator (Savestate:
             * DAT_800b52bd == 0, DAT_800b52b5 == 1). ZITAT KORRIGIERT (2026-08-17, Review-Fund
             * F2): die frueher hier genannte Adresse @0x80044500 gehoert zum SEQ0-Flag
             * DAT_800b52b5, NICHT zu DAT_800b52bd. Die Bytes (selbst disassembliert):
             *   800444f0: lbu v1,21173(v1)        ; DAT_800b52b5  (SEQ0-Flag)
             *   800444f8: beq v1,0xff -> 0x80044554  ; 0xff = kein SUB -> auch SEQ#2 faellt aus
             *   80044500: bne v1,zero,0x80044524  ; <- 52b5-Gate (mit 52b5==1 GENOMMEN)
             *   80044524: lui v0,0x800b
             *   80044528: lbu v0,21181(v0)        ; DAT_800b52bd  (SEQ#2-Flag)  <-- das Gate
             *   80044530: bne v0,zero,0x80044554  ; Flag != 0 -> SEQ#2 NICHT auto-replayen
             *   8004453c: lb  a0,21182(a0)        ; DAT_800b52be = SEQ#2-Handle
             *   80044540: jal 0x8005acec          ; SsSeqReplay
             *   80044550: sb  v0,21180(at)        ; DAT_800b52bc = 1
             * Der Beleg fuer "SEQ#2 spielt ab Raum-Eintritt" liegt also in @0x80044528/@0x80044530
             * /@0x80044540 — plus der Vorbedingung @0x800444f8 (DAT_800b52b5 != 0xff).
             *
             * ⚠ DATEN-BEFUND zu SUB_15 (ROOM1170), selbst geparst 2026-08-17 — SEQ#2 dieses
             * Containers ist STUMM BY DATA, nicht durch einen Port-Bug:
             *   SUB_15.BGM size 44808; Trailer u32[size-8] = 152 = SEQ#2-Offset, u32[size-12] = 176.
             *   SEQ#2 = Bytes 152..176 (24 B): "pQES" 00000001 | ppqn 0x0030 | tempo 0x07a120 |
             *   rhythm 0x0402 | 00 b0 0a 40 (delta0, CC10 Pan=0x40) | 00 ff 2f 00 (End of Track).
             *   Also EIN Pan-Controller, dann Track-Ende — keine Note. Der Rotor ist SEQ0 (@0,
             *   152 B, ppqn 0x01e0), den das Skript per Sce_bgm_control(1,1) aufdreht.
             * Das Basis-mvol hier ist trotzdem byte-true richtig (SsSeqSetVol @0x800449a0-b4 mit
             * vol 127) und gilt fuer JEDEN SUB-Container — es macht aber SUB_15 nicht hoerbar. */
            s_ss_sub2.mvol = s_ss_sub2.mvol_l = s_ss_sub2.mvol_r = SS_BASE_MVOL;  /* @0x800449a0-b4 */
            ss_start(&s_ss_sub2, 1);
            if ((sub_b >> 7) != 0) s_ss_sub2.mvol = s_ss_sub2.mvol_l = s_ss_sub2.mvol_r = 0;
        }
    }
    return 0;
}

/* Nur die SUB-Sequenzen stoppen — FUN_80044210 @0x8004432C/@0x80044354 (SsSeqStop fuer beide),
 * der Pfad "nur der SUB-Track hat sich geaendert". MAIN laeuft dabei unangetastet weiter. */
static void re15_bgm_stop_subs(void) {
    s_ss_sub.playing  = 0; ss_all_voices_off(&s_ss_sub);
    s_ss_sub2.playing = 0; ss_all_voices_off(&s_ss_sub2);
}

/* Der aufgeschobene Track-Wechsel: im Original blockiert FUN_80044210 die Ausblendung aus
 * (@0x8004428C-@0x800442B0: while (DAT_800b5218) FUN_80029ac8(1)) und laedt DANACH. Der Port
 * darf die Hauptschleife nicht anhalten, also merkt er sich das Ziel und laedt, sobald die
 * State-Machine auf 0 steht — hoerbar dieselbe Reihenfolge: alt ausblenden, Stop, neu starten. */
static int     s_bgm_pending      = 0;
static uint8_t s_bgm_pending_main = 0xff, s_bgm_pending_sub = 0xff;
static int     s_bgm_pending_stage = 0, s_bgm_pending_room = 0;

static void re15_bgm_commit_pending(void) {
    if (!s_bgm_pending) return;
    s_bgm_pending = 0;
    re15_amb_load_rotor(s_bgm_pending_stage, s_bgm_pending_room);
    SDL_LockAudioDevice(s_audio_dev);
    re15_bgm_load_main(s_bgm_pending_main);
    re15_bgm_load_sub (s_bgm_pending_sub);
    if (s_amb.pcm) s_amb.active = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* == FUN_800449f4 @0x800449f4: Ausblendung anstossen. */
static void re15_bgm_fade_start(void) {
    s_bgm_fade_state = 1;
    s_bgm_fade_n     = BGM_FADE_FRAMES;
    s_bgm_fade_vol   = 0x8000;
}

/* == FUN_80044ab8 @0x80044ab8: ein Frame der Ausblendung. */
static void re15_bgm_fade_tick(void) {
    if (s_bgm_fade_state == 0) return;
    if (s_bgm_fade_state == 1 || s_bgm_fade_state == 2) {
        if (s_bgm_fade_n > 0) {                       /* v -= v/n, Clamp >= 0 */
            int v = s_bgm_fade_vol - s_bgm_fade_vol / s_bgm_fade_n;
            s_bgm_fade_vol = (v < 0) ? 0 : v;
        }
        if (getenv("RE15_BGM_FADE_DEBUG")) {
            int k = BGM_FADE_FRAMES - s_bgm_fade_n + 1;   /* Frames, die bereits gedaempft haben */
            fprintf(stderr, "[bgm] fade k=%2d vol=%5d soll=%5d\n", k, s_bgm_fade_vol,
                    (0x8000 * (BGM_FADE_FRAMES - k)) / BGM_FADE_FRAMES);
        }
        if (s_bgm_fade_state == 1) {
            s_bgm_fade_state = 2;
            s_bgm_fade_n--;
        } else {
            s_bgm_fade_n--;
            if (s_bgm_fade_n == 1) s_bgm_fade_state = 3;   /* @0x80044b64 */
        }
        return;
    }
    /* state 3: n = 0, state = 0, SsSeqStop fuer alle drei Slots (@0x80044b8c-@0x80044bb0). */
    if (getenv("RE15_BGM_FADE_DEBUG"))
        fprintf(stderr, "[bgm] Ausblendung fertig: Restlautstaerke %d/32768 (Soll 546 = 1/60)\n",
                s_bgm_fade_vol);
    s_bgm_fade_n     = 0;
    s_bgm_fade_state = 0;
    SDL_LockAudioDevice(s_audio_dev);
    s_ss_main.playing = 0; ss_all_voices_off(&s_ss_main);
    re15_bgm_stop_subs();
    s_bgm_fade_vol = 0x8000;      /* die neuen Stimmen starten wieder auf voller Rampe */
    SDL_UnlockAudioDevice(s_audio_dev);
    re15_bgm_commit_pending();    /* == das, worauf FUN_80044210 gewartet hat */
}

/* re15_bgm: room-music manager. Laedt MAIN + SUB + die geloopte Raum-Ambience (Rotor). */
static void re15_bgm_play_room(int stage, int room) {
    int e = ss_bgm_entry(stage, room);
    uint8_t main_b = (e < 0) ? 0xff : (uint8_t)(e & 0xff);
    uint8_t sub_b  = (e < 0) ? 0xff : (uint8_t)((e >> 8) & 0xff);
    re15_amb_load_rotor(stage, room);
    SDL_LockAudioDevice(s_audio_dev);
    re15_bgm_load_main(main_b);
    re15_bgm_load_sub(sub_b);
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
    /* BYTE-TRUE LATCH: pro TRACK-ID, nicht pro Aufruf und nicht pro Raum.
     *
     * VORHER: 'static int started' war ein prozess-globaler ONE-SHOT — der erste erfolgreiche Aufruf
     * gewann fuer die ganze Session, jeder spaetere war ein No-op. Der PC-Port spielte also GENAU
     * EINE BGM pro Programmlauf; der Raumwechsel-Aufruf in room_common.c war ein toter Call.
     *
     * DAS ORIGINAL (FUN_80044210): es vergleicht die unteren 6 Bit der Track-ID gegen den Cache
     * 0x800B2B44 (main) / 0x800B2B45 (sub) — 'andi a0,s0,0x3f / andi v1,v1,0x3f / beq' @0x80044278,
     * @0x8004427C, @0x80044280 fuer main und @0x800442FC, @0x80044300, @0x80044304 fuer sub. Sind
     * BEIDE gleich, springt es nach @0x800443B0 und tut GAR NICHTS: kein Stop, kein Reload, kein
     * Play — die Musik laeuft einfach weiter. Der Cache wird danach unbedingt aufgefrischt
     * (@0x800443C8 / @0x800443D0).
     *
     * Deshalb genuegt hier KEIN Vergleich auf (stage,room): zwei verschiedene Raeume mit demselben
     * Stueck wuerden die Musik neu starten, wo das Original sie durchlaufen laesst. Verglichen wird
     * die aufgeloeste Track-ID.
     *
     * !! VORERST ZURUECKGENOMMEN (2026-08-01). Der Latch unten ist wieder der alte One-Shot, WEIL
     * die byte-true Fassung eine Regression ausgeloest hat: Nutzer-Report "BGM und Soundeffekte
     * scheinen nicht zu funktionieren. Lediglich die Voiceover funktionieren was sound betrifft."
     *
     * URSACHE DER REGRESSION (Diagnose, noch nicht am Log bestaetigt): der One-Shot machte den
     * Raumwechsel-Aufruf in room_common.c zu einem TOTEN Call — re15_bgm_play_room lief damit exakt
     * EINMAL pro Programmlauf. Mit dem Track-Latch laeuft es bei jedem Wechsel erneut, und dieser
     * NACHLADE-Pfad ist im Port nie gefahren worden: re15_ss_load schreibt in dieselben ss_seq_t
     * (s_ss_main/s_ss_sub), die VAB-ids 5/6 werden erneut geoeffnet, und ss_start laeuft unter
     * SDL_LockAudioDevice. Dass auch die SOUNDEFFEKTE weg sind, passt dazu — die haengen an
     * derselben VAB-Maschinerie, waehrend die Voiceover ueber einen anderen Pfad laufen.
     *
     * ABBAU-PFAD IST JETZT DA (2026-08-01), damit ist der Latch wieder scharf:
     *   - ss_free_bank() gibt die alte Bank frei, bevor neu geladen wird (der Nachlade-Pfad
     *     leckte vorher VAG-PCM + SEQ-Kopie und war nie gefahren worden),
     *   - re15_bgm_stop_subs() = SsSeqStop fuer beide Sub-Sequenzen @0x8004432C/@0x80044354,
     *   - alles unter SDL_LockAudioDevice, weil der Mix-Callback aus genau diesem PCM liest.
     *
     * NICHT NACHGEBAUT: die Fadeout-Wartschleife des Main-Pfads (@0x8004428C-@0x800442B0:
     * while (DAT_800b5218) FUN_80029ac8(1)). Das Gegenstueck dazu ist FUN_800449f4(0x3c)
     * @0x800449f4 — SsSeqSetDecrescendo(handle, 0, 60) auf alle laufenden Handles, angestossen
     * schon in FUN_800443ec @0x800443ec ganz am Anfang des Raumladers. Der Port hat kein
     * Decrescendo; hier wird stattdessen hart umgeschaltet. Das ist eine BEKANNTE, benannte
     * Luecke (60-Frame-Ausblendung beim Track-Wechsel), kein stiller Ersatz. */
    if (!g_audio.initialized) return;

    /* == FUN_80044210 @0x80044210. Tabellen-Eintrag: LOW byte = MAIN, HIGH byte = SUB;
     * je die unteren 6 Bit sind der Slot, die oberen 2 sind Replay-Flags. */
    int e = ss_bgm_entry(stage, room);
    uint8_t main_b = (e < 0) ? 0xff : (uint8_t)(e & 0xff);
    uint8_t sub_b  = (e < 0) ? 0xff : (uint8_t)((e >> 8) & 0xff);

    /* DAT_800b2b44 / DAT_800b2b45 — die Caches. Sie werden AUSSCHLIESSLICH am Ende von
     * FUN_80044210 geschrieben (@0x800443B8/@0x800443D0) und nirgends initialisiert, liegen
     * also im BSS und stehen beim Boot auf 0. Genau das bilden wir ab (nicht ein -1-Sentinel,
     * der "beim ersten Aufruf immer laden" bedeuten wuerde).
     * FOLGE, wie im Original: startet man DIREKT in einem Raum, dessen MAIN-Byte 0x00 ist
     * (Tabelleneintraege 0xff00), gilt der Track als "schon geladen" und es laeuft nichts an.
     * Ueber eine Tuer erreicht man solche Raeume immer mit einem abweichenden Cache. */
    static int cache_main = 0, cache_sub = 0;
    int main_same = ((main_b & 0x3f) == (cache_main & 0x3f));
    int sub_same  = ((sub_b  & 0x3f) == (cache_sub  & 0x3f));

    if (main_same && sub_same) {
        /* @0x80044280 -> LAB_800443b0: GAR NICHTS. Kein Stop, kein Reload, kein Play —
         * dieselbe Musik laeuft ueber den Raumwechsel hinweg durch. */
    } else if (main_same) {
        /* Nur der SUB-Track hat gewechselt: beide Sub-Sequenzen stoppen, SUB neu laden.
         * MAIN wird NICHT angefasst (@0x80044320-@0x80044368). */
        SDL_LockAudioDevice(s_audio_dev);
        re15_bgm_stop_subs();
        re15_bgm_load_sub(sub_b);
        SDL_UnlockAudioDevice(s_audio_dev);
    } else {
        /* MAIN hat gewechselt. Im Original passiert das in ZWEI Etappen derselben Funktion
         * FUN_800396fc: erst FUN_800443ec @0x800443ec ganz am Anfang (SsUtKeyOffV 0x10..0x17 und,
         * weil der MAIN-Track abweicht, FUN_800449f4(0x3c) = Ausblendung ueber 60 Frames), dann
         * am Ende FUN_80044210, das auf das Ende der Ausblendung WARTET und erst danach
         * FUN_80044564 + FUN_80044774 laedt (@0x800442C0-@0x800442E0).
         * Der Port stoesst hier die Ausblendung an und merkt sich das Ziel; geladen wird in
         * re15_bgm_fade_tick, sobald die State-Machine durch ist. */
        s_bgm_pending       = 1;
        s_bgm_pending_main  = main_b;
        s_bgm_pending_sub   = sub_b;
        s_bgm_pending_stage = stage;
        s_bgm_pending_room  = room;
        re15_bgm_fade_start();
    }

    /* @0x800443B8-@0x800443D0: der Cache wird UNBEDINGT aufgefrischt, auch im Nichts-Fall. */
    cache_main = main_b; cache_sub = sub_b;
    fprintf(stderr, "[bgm] stage=%d room=%02X entry=%04X -> MAIN%02X(flag %d) SUB%s(flags %d/%d)%s\n",
            stage, room, (e < 0) ? 0xffff : e, main_b & 0x3f, main_b >> 6,
            (sub_b == 0xff) ? "--" : "", (sub_b >> 6) & 1, sub_b >> 7,
            (main_same && sub_same) ? "  [unveraendert, laeuft durch]" : "");
}

/* ⛔ PORT-ERFINDUNG ENTFERNT (Nutzer-Report "Helikopter-Rotor unhoerbar in ROOM1170", 2026-08-17).
 *
 * Diese Funktion skalierte den SsSeq-SUB-Layer (s_ss_sub.mvol_l/r) JEDEN FRAME mit der
 * SE-Distanz-/Pan-Mathematik FUN_80045a64 — dafuer gibt es KEINEN Beleg, im Gegenteil:
 * `jal 0x80045a64` hat in der ganzen PSX.EXE exakt VIER Aufrufer — @0x800451cc, @0x8004527c,
 * @0x800454e8, @0x80045830 (selbst gescannt: Instruktionswort 0x0C011699, 4-Byte-aligned) — und
 * ALLE VIER liegen im SE-Pfad und uebergeben eine Entity-Position (@0x800454e4/@0x8004582c
 * `lw a0,g_entity(cur)` + `addiu a0,a0,52` @0x800454ec/@0x80045830-Delay, bzw. `lw a0,32(sp)`
 * @0x800451c8/@0x80045278). NIEMAND wendet sie auf ein SsSeq-/BGM-Volumen an. Die BGM-Lautstaerke
 * kommt ausschliesslich aus SsSeqSetVol mit dem Slot-vol-Halbwort (@0x80044960/@0x800449a0 beim
 * Laden, @0x80044e00-24 beim SCD-0x54-op1), und das ist im Original-Savestate fuer ALLE DREI Slots
 * 127 (0x800b52b0/b8/c0, stage_saves/orig_1170_gp.sav) — also unskaliert.
 * Nachgerechnet lag die erfundene Skalierung bei 44..96 von 127 je Intro-Cut (-2.5..-9 dB) und
 * begrub den Rotor unter dem gleichzeitig laufenden MAIN32-Score.
 *
 * Die Funktion bleibt als NO-OP stehen, weil der gemeinsame Treiber re15_rotor_drive
 * (engine/src/game_step_common.c) sie noch ruft; rotor_common.c/re15_rotor_compute_pan bleiben
 * fuer den ECHTEN SE-Pfad (FUN_80045a64-Replikat) korrekt und unveraendert. */
void re15_audio_rotor_update(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                             const int32_t heli_pos[3])
{
    (void)cam_eye; (void)cam_tgt; (void)heli_pos;
    /* kein Positions-Gate auf dem BGM-Layer — siehe 4-Caller-Census oben. */
}

/* ⛔ EBENFALLS BELEGLOS: das harte Nullen des SUB-Layers ausserhalb des Skript-Modus.
 * Im Original steuert AUSSCHLIESSLICH der SCD-0x54-Status den Slot (op1 = SetVol(vol)+SsSeqPlay
 * @0x80044e00-44, op2 = SsSeqStop @0x80044e50-84), und SEQ#2 laeuft ueber den Auto-Play-Gate
 * FUN_800444b0 sogar unabhaengig davon weiter, bis der Raum-BGM-Wechsel (FUN_80044210) ihn stoppt.
 * NO-OP, damit der bestehende PSX-/PC-Aufrufpfad kompiliert. */
void re15_audio_rotor_silence(void)
{
    /* kein player_mode-Mute — der Slot-Status kommt aus Sce_bgm_control (@0x80044e00-84). */
}

/* SsSeq slot control (SCD Sce_bgm_control / 0x54 → PSX FUN_80044da4). slot 0 =
 * MAIN room music, slot 1 = SUB helicopter-rotor layer. We toggle the layer's
 * master volume (a looping mute/unmute, sounding identical to SsSeqStop/Play for a
 * sustained drone, with no restart pop). This is the canonical, 1:1 PSX rotor
 * on/off: ROOM1170's sub02 plays slot 1 at the heli-arrival + sky-view cuts and
 * stops it during Leon's dialogue close-ups. */
/* Full Sce_bgm_control (0x54) semantics (byte-true @0x800429b4-e8 packs FIVE operand bytes;
 * FUN_80044da4 consumes: slot = pc[1] indexes THREE seq handles (0 MAIN / 1 SUB SEQ0 / 2 SUB
 * SEQ1, @0x800b52ae+slot*8), op = pc[2]; pc[4]!=0 writes vol-1 into the slot's in-RAM VAB
 * master mvol (part pc[3]==0, VabHdr+0x18) or ProgAtr[pc[3]-1].mvol (+1, stride 16); pc[5]
 * likewise for pan (+0x19 / +4). Note-ons re-read those — audit wf_1db9c802
 * SCD-BGMCTL-OPERAND-PAYLOAD + AUDIO-SEQCTL-VOLPAN-FIELDS. */
static void ss_seq_ctl_ex(int slot, int op, int part, int vol, int pan)
{
    ss_seq_t *s = (slot == 0) ? &s_ss_main : (slot == 1) ? &s_ss_sub
                : (slot == 2) ? &s_ss_sub2 : NULL;
    if (!s) return;
    int base = (slot >= 1) ? s_ss_sub_base_mvol : 0x1a00; /* MAIN inits to 0x1a00 */
    fprintf(stderr, "[bgm] Sce_bgm_control slot=%d op=%d (part=%d vol=%d pan=%d) capTick=%ld\n",
            slot, op, part, vol, pan, s_cap_ticks);
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
    /* the vol/pan payload writes hit the slot's VAB header/program attrs (SUB SEQ1 shares the
     * SUB bank via tone_src); ROOM1090 mutes MAIN program 0 mid-script this way (vol byte 1 ->
     * write 0), ROOM11F0/11F1 scale the master. */
    re15_vab_t *vab = s->tone_src ? &s->tone_src->vab : &s->vab;
    if (vol) {
        if (part == 0) vab->master_volume = (uint8_t)(vol - 1);
        else if (part - 1 < RE15_VAB_PROGRAM_COUNT) vab->prog_mvol[part - 1] = (uint8_t)(vol - 1);
    }
    if (pan) {
        if (part == 0) vab->master_pan = (uint8_t)(pan - 1);
        else if (part - 1 < RE15_VAB_PROGRAM_COUNT) vab->prog_mpan[part - 1] = (uint8_t)(pan - 1);
    }
    SDL_UnlockAudioDevice(s_audio_dev);
}

void re15_audio_seq_ctl(int slot, int op)
{
    if (!g_audio.initialized) return;
    ss_seq_ctl_ex(slot, op, 0, 0, 0);
}

void re15_audio_tick(void)
{
    if (!g_audio.initialized) return;

    /* RE15_AUDIO_CAP_SYNC: ein Spielframe = RE15_AUDIO_RATE/30 Stereo-Frames, gerendert durch
     * dieselbe Kette wie der SDL-Callback (der in diesem Modus gar nicht laeuft). audio_callback
     * schreibt selbst nach s_audio_cap. */
    if (s_audio_cap_sync) {
        static int16_t capbuf[(RE15_AUDIO_RATE / 30) * 2];
        audio_callback(NULL, (Uint8 *)capbuf, (int)sizeof capbuf);
        s_cap_ticks++;
        if (s_cap_limit > 0 && s_cap_ticks >= s_cap_limit) {
            if (s_audio_cap) { fflush(s_audio_cap); fclose(s_audio_cap); s_audio_cap = NULL; }
            fprintf(stderr, "[audio] CAP_SYNC: %ld Frames geschrieben -> exit\n", s_cap_ticks);
            fflush(stderr);
            exit(0);
        }
    }

    /* == FUN_80044ab8 aus dem Frame-Tick FUN_800458d4: die BGM-Ausblendung weiterdrehen und,
     * wenn sie durch ist, den aufgeschobenen Track-Wechsel ausfuehren. */
    re15_bgm_fade_tick();

    scd_audio_event_t evt;
    while (scd_audio_queue_pop(&evt)) {
        g_audio.events_total++;
        switch ((scd_audio_kind_t)evt.kind) {
            case SCD_AUDIO_SE_ON:
                g_audio.events_se_on++;
                if (getenv("RE15_SE_DEBUG")) {
                    static const char *kn[] = { "SKIP", "WEAPON", "SND0", "SND1", "CORE" };
                    int k = (int)re15_audio_se_bank_kind(evt.bank);
                    fprintf(stderr, "[se] Se_on bank=%u id=%u -> %s%s\n", evt.bank, evt.sample_id,
                            (k >= 0 && k < 5) ? kn[k] : "?",
                            (re15_audio_se_bank_kind(evt.bank) == RE15_SE_BANK_SKIP)
                                ? "  << VERWORFEN (Bank nicht resident)" : "");
                }
                /* Byte-true FUN_80045024: the Se_on bank byte (evt.bank = the opcode's pc[1]) picks
                 * the VAB bank; evt.sample_id (pc[2]) is the record index (bank_base + record*4 =
                 * @0x80045140, NO -1). Route to the matching resident/room bank; a bank the port has
                 * not loaded is skipped exactly as the original skips DAT_800b21ec[bank] == -1. (Was:
                 * ignore the bank and play (sample_id-1) from a bring-up TEST VAB — a divergence.) */
                switch (re15_audio_se_bank_kind(evt.bank)) {
                    case RE15_SE_BANK_WEAPON: re15_audio_weapon_se(evt.sample_id);   break;
                    case RE15_SE_BANK_SND0:   re15_audio_room_se_snd0(evt.sample_id);break;
                    case RE15_SE_BANK_SND1:   re15_audio_room_se(evt.sample_id);     break;
                    case RE15_SE_BANK_CORE:   re15_audio_core_se(evt.sample_id);     break;
                    case RE15_SE_BANK_SKIP:   default: break;   /* bank 0/>=6 not resident -> skip */
                }
                break;
            case SCD_AUDIO_VOICE_ON:
                /* RE2-style voice: route through the re15_voice manager → re15_xa stream,
                 * keyed on the CURRENT room (synchro has clips for 1170/1150/1240/10D0 —
                 * the old 0x1170 hardcode served room1170 lines everywhere). */
                re15_voice_play((uint16_t)g_current_room_id, evt.sample_id);
                break;
            case SCD_AUDIO_SEQ_CTL:
                /* 0x54 SsSeq slot control + the vol/pan payload (part=sample_id, vol=raw_w0,
                 * pan=pan — the FIVE operand bytes FUN_80044da4 consumes). */
                ss_seq_ctl_ex(evt.bank, evt.volume, evt.sample_id, (int)evt.raw_w0, evt.pan);
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
    static int diag = -1;
    if (diag < 0) diag = getenv("RE15_FOOT_DEBUG") ? 1 : 0;
    if (!s_foot_loaded) {
        if (diag) fprintf(stderr, "[foot] SE unterdrueckt: Bank snd0 nicht geladen\n");
        return;
    }
    int vag = re15_footstep_vag(s_foot_edt, &s_foot_vab, sound_type);
    if (vag < 0 || !s_foot_decoded[vag]) {
        if (diag) fprintf(stderr, "[foot] SE unterdrueckt: type=%d -> vag=%d (dekodiert=%d)\n",
                          sound_type, vag, (vag >= 0) ? (s_foot_decoded[vag] != NULL) : 0);
        return;
    }
    if (diag) fprintf(stderr, "[foot] SE foot=%d type=%d vag=%d\n", foot, sound_type, vag);

    /* Tone volume with the PROGRAM stride (byte-true FUN_80045630 @0x800457dc walks
     * VH + prog*0x200 + 0x820 + tone*0x20 — the old read omitted prog*16, so any prog!=0
     * record (ROOM11A0 recs 1-4/6/8/9 = prog 1) read a foreign tone's volume.
     * [audit wf_1db9c802 AUD-FOOT-VOL-STRIDE] The 0x80 water bit is masked like the resolver. */
    int st   = sound_type & 0x7f;
    int prog = s_foot_edt[st * 4 + 1] & 0x7f;
    int tone = s_foot_edt[st * 4 + 2] >> 4;
    int tvol = 0;
    uint32_t step = 0;
    if (prog < RE15_VAB_PROGRAM_COUNT && tone < RE15_VAB_TONES_PER_PROGRAM) {
        const re15_vab_tone_t *t = &s_foot_vab.tones[prog * RE15_VAB_TONES_PER_PROGRAM + tone];
        tvol = t->vol;
        /* Tone-Pitch auch fuer Footsteps (derselbe SsUtKeyOnV-Pfad @0x8004522c; Dossier D1 —
         * bisher klangen nur Tones mit note ~ center-12 zufaellig richtig). */
        step = (uint32_t)re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                              t->center_note, t->pitch_shift) << 4;
    }
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
    s_active[slot].step_q16   = step;
    s_active[slot].pos_frac   = 0;
    s_active[slot].volume_q15 = vol;
    s_active[slot].vol_r_q15  = 0;   /* legacy mono (Slot-Reuse: kein Pan-Rest) */
    s_active[slot].active     = 1;
    SDL_UnlockAudioDevice(s_audio_dev);
}

/* Raum-Sound-Baenke (snd0 Schritte + snd1 Raum/Combat-SE) fuer den GERADE geladenen Raum
 * binden. Gegenstueck zu FUN_80043eac/FUN_80043fb0 im Raumlader FUN_800396fc — muss nach dem
 * RDT-Parse und bei JEDEM Raumwechsel laufen, weil beide Baenke aus dem RDT geschnitten werden. */
void re15_audio_load_room_banks(void)
{
    if (!g_audio.initialized) return;
    SDL_LockAudioDevice(s_audio_dev);
    for (int i = 0; i < MIXER_MAX_ACTIVE_SAMPLES; i++) s_active[i].active = 0;  /* nichts spielt mehr aus der alten Bank */
    free_room_bank_pcm();
    SDL_UnlockAudioDevice(s_audio_dev);
    load_footstep_vab_pc();   /* room snd0 + EDT (Schritt-SE) */
    load_room_se_vab_pc();    /* room snd1 + SE-Tabelle (FUN_800453d0) */
}

void re15_audio_shutdown(void)
{
    if (s_audio_dev) {
        SDL_PauseAudioDevice(s_audio_dev, 1);
        SDL_CloseAudioDevice(s_audio_dev);
        s_audio_dev = 0;
    }
    if (s_audio_cap) { fflush(s_audio_cap); fclose(s_audio_cap); s_audio_cap = NULL; }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    g_audio.backend_active = 0;
}
