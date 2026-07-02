/*
 * RE1.5 Rebuilt — Audio subsystem (Phase 4.6, 2026-05-18).
 *
 * RE2-architecture audio module. Two backends behind a single API:
 *
 *   PSX: SPU hardware (24 voices × ADPCM samples + CD-streamed XA audio)
 *   PC:  SDL2 SDL_OpenAudioDevice + software mixer
 *
 * Both consume the same upstream signal — the scd_audio_event_t queue
 * filled by the SCD VM's audio opcodes (Phase 4.4.3: Se_on, BGM, XA_on,
 * etc.). The audio backend translates SCD events into voice activations
 * or XA stream starts, exactly like RE2 does between the SCD VM and
 * its libsnd/libsd layer.
 *
 * STATUS (updated 2026-07-02): built + PLAYING end-to-end on the PC backend
 * (audio_pc.c) — the old "4.6.1: no playback yet" phasing below is historical.
 * Working: SCD Se_on SFX (ADPCM VAG mixer), player footsteps (room snd0 + EDT),
 * dialogue voice (RE2-style CD-XA stream), BGM (SsSeq synth: MAIN+SUB, ADSR,
 * note2pitch, STUDIO_B SPU reverb), looping rotor ambience.
 * NOT yet wired: COMBAT SFX (gunshot/hit/death) — those come from the overlay
 * SE-play FUN_800453d0 (per-room SE table), not the SCD Se_on path, so the
 * C-driven combat is currently silent.
 * Historical phasing: 4.6.1 foundation · 4.6.2 VAB+ADPCM · 4.6.3 Se_on SFX ·
 * 4.6.4 XA/BGM (all done on PC).
 */
#ifndef RE15_AUDIO_H
#define RE15_AUDIO_H

#include <stdint.h>

/* Audio engine state — exposed for diagnostics (main.c shows event
 * counts in the debug HUD). */
typedef struct {
    uint32_t events_total;        /* lifetime SCD audio events drained  */
    uint32_t events_se_on;        /* of those, kind=Se_on (SFX)         */
    uint32_t events_bgm;          /* kind=BGMTBL_SET                    */
    uint32_t events_xa_on;        /* kind=XA_ON                         */
    uint32_t events_se_vol;       /* kind=SE_VOL                        */
    uint32_t events_unknown;      /* kind not recognized — should be 0  */
    uint8_t  initialized;         /* 1 after re15_audio_init() returned */
    uint8_t  backend_active;      /* PSX: SPU on. PC: SDL audio device on */
} re15_audio_state_t;

extern re15_audio_state_t g_audio;

/* One-time initialization. Call once at boot, AFTER re15_render_init()
 * (some PSX systems share GPU+SPU DMA arbitration) but BEFORE the main
 * loop. */
void re15_audio_init(void);

/* Per-frame tick. Drains the SCD audio event queue and dispatches each
 * event to the backend's playback path (or — in Phase 4.6.1 — just
 * increments the per-kind counters in g_audio). Call once per frame
 * after scd_vm_tick(). */
void re15_audio_tick(void);

/* Start the looping room BGM, resolved via the canonical RE1.5 stage/room→BGM
 * table. Call once at the pre-intro→cinematic handoff (the original plays no
 * BGM during the narrator pre-intro). Idempotent; no-op if not initialized.
 * PSX is a no-op (the SPU/SsSeq path handles room BGM natively). */
void re15_audio_start_room_bgm(int stage, int room);

/* Per-frame: gate the helicopter-rotor (BGM SUB layer) volume by the current
 * cut camera→heli distance, the RE1.5 way (FUN_80045a64 SE distance attenuation).
 * cam_eye = active cut camera eye position, heli_pos = heli prop world pos.
 * PSX backend = no-op. */
void re15_audio_rotor_update(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                             const int32_t heli_pos[3]);
/* Silence the rotor when the heli is gone (gameplay handoff). PSX = no-op. */
void re15_audio_rotor_silence(void);

/* SHARED rotor positional-SE math (rotor_common.c, byte-true FUN_80045a64): compute
 * the rotor L/R volume (0..0x7f) from the camera eye/target + heli position
 * (distance attenuation + stereo azimuth pan, integer ATAN256 LUT — no soft-float).
 * Both ports' re15_audio_rotor_update() call this, then apply volL/volR to their SUB
 * layer (SPU voice vol / SsSeq mvol). Unifies the previously-drifted per-port copies. */
void re15_rotor_compute_pan(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                            const int32_t heli_pos[3], int *out_volL, int *out_volR);

/* SsSeq slot control — the SCD Sce_bgm_control (0x54) opcode (PSX FUN_80044da4).
 * slot = sequence slot (0=MAIN room music, 1=SUB rotor layer), op = control
 * (1=play/loop, 2=stop, 3=replay, 4=pause, 5=decrescendo). This is how ROOM1170's
 * sub02 turns the helicopter rotor (SUB layer) on at the heli-arrival + sky-view
 * cuts and off during Leon's dialogue close-ups — the canonical, 1:1 PSX mechanism.
 * PSX backend = no-op (it drives SsSeqPlay/SsSeqStop natively). */
void re15_audio_seq_ctl(int slot, int op);

/* Shutdown — PC closes SDL_OpenAudioDevice, PSX silences master volume.
 * Safe to call on a never-initialized engine (no-op). */
void re15_audio_shutdown(void);

/* Player FOOTSTEP SE (byte-true FUN_80045630). Called from re15_game_step on a
 * foot-plant (re15_actor_footstep) during walk/run: `foot` = 7 (left) / 4 (right),
 * `sound_type` = the floor.flr region material (re15_rdt_floor_sound). The backend
 * maps sound_type → the snd0 VAB program (via the EDT table) and plays it. WIRED +
 * playing: audio_pc.c load_footstep_vab_pc slices snd0 VH/VB/EDT from the resident
 * RDT (g_room_rdt.snd_*[0]) and re15_audio_footstep mixes the resolved VAG. */
void re15_audio_footstep(int foot, int sound_type);

#endif /* RE15_AUDIO_H */
