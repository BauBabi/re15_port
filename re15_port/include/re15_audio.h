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
 * COMBAT SFX come from the overlay SE-play FUN_800453d0 (per-room snd1 SE table),
 * NOT the SCD Se_on path — re15_audio_room_se is that path (PC). WIRED: the zombie
 * death groan (FUN_80107cb0 frame 7). NOT yet wired: gunshot/hit SEs (same snd1 path).
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

/* OPTIONS SOUND screen: set the output mode. mono != 0 collapses the final stereo mix to mono
 * (byte-true FUN_80043c00 CD-audio cross-mix matrix + mono flag). */
void re15_audio_set_mono(int mono);

/* Player FOOTSTEP SE (byte-true FUN_80045630). Called from re15_game_step on a
 * foot-plant (re15_actor_footstep) during walk/run: `foot` = 7 (left) / 4 (right),
 * `sound_type` = the floor.flr region material (re15_rdt_floor_sound). The backend
 * maps sound_type → the snd0 VAB program (via the EDT table) and plays it. WIRED +
 * playing: audio_pc.c load_footstep_vab_pc slices snd0 VH/VB/EDT from the resident
 * RDT (g_room_rdt.snd_*[0]) and re15_audio_footstep mixes the resolved VAG. */
void re15_audio_footstep(int foot, int sound_type);

/* Die beiden RAUM-Sound-Baenke (snd0 = Schritte, snd1 = Raum/Combat-SE) an den GERADE
 * geladenen Raum binden. Gegenstueck zu FUN_80043eac @0x80043eac und FUN_80043fb0
 * @0x80043fb0, die der Raumlader FUN_800396fc pro Raum aufruft (VH/VB aus dem frisch
 * geladenen RDT, mit SsVabClose der Vorgaengerbank).
 * MUSS nach dem RDT-Parse laufen und bei JEDEM Raumwechsel erneut: die Baenke werden aus
 * dem RDT geschnitten. Lief das frueher in re15_audio_init(), war g_room_rdt_ok noch 0 und
 * die Schritt-Bank wurde nie geladen. */
void re15_audio_load_room_banks(void);

/* Room SE by id (byte-true FUN_800453d0 core): play the per-room snd1 SE bank's sound for
 * `se_id` (0..0x18). This is the COMBAT/room SE path (zombie death groan, etc.) — distinct
 * from the SCD Se_on path in re15_audio_tick. The snd1 bank + its SE table (EDT) are loaded
 * from the resident RDT. Called C-side (e.g. the zombie death FSM frame 7). */
void re15_audio_room_se(int se_id);

/* Room SE from the snd0 bank (byte-true FUN_80045024 bank 2 = *(DAT_800ac778+8) = the RDT snd0
 * bank, same EDT->prog/tone->VAG lookup as room_se on snd1). Bank 2 of the Se_on selector. */
void re15_audio_room_se_snd0(int se_id);

/* Byte-true Se_on VAB-bank selector (FUN_80045024 @0x80045028: bank = arg>>24, dispatch
 * @0x80010e70, `sltiu 0x6` = banks 0..5). Maps the SCD Se_on bank byte to the port's loaded VAB:
 *   0 -> SKIP   (resident in-RAM blob @0x801fdd00 — NOT loaded in the port; the original skips a
 *                bank whose DAT_800b21ec[bank] handle == -1 @0x8004506c, so SKIP is byte-faithful)
 *   1 -> WEAPON (ARMS, the equipped weapon @0x801fcd00 -> re15_audio_weapon_se)
 *   2 -> SND0   (RDT snd0 bank -> re15_audio_room_se_snd0)
 *   3 -> SND1   (RDT snd1 SE bank, the FUN_800453d0 path -> re15_audio_room_se)
 *   4 -> CORE   (resident CORE @0x801fbd00 -> re15_audio_core_se)
 *   5 -> SND0   (caseD_5 @0x80045130 laedt DAT_800ac778 und liest `lw a0,0x8` @0x8004513c
 *                = RDT snd0 — bank 5 == bank 2, NICHT snd1 (das ist caseD_3 mit +0x14).
 *                Korrigiert 2026-08-02, Dossier analysis/rolltor_sound.md D6; z.B. der
 *                EXE-Callsite FUN_80035538 case 5 = 0x0207xxxx gehoert auf snd0.)
 *   >=6 -> SKIP  (invalid: `sltiu v0,v1,0x6; beq v0,zero,ret` @0x80045094). */
typedef enum {
    RE15_SE_BANK_SKIP = 0,
    RE15_SE_BANK_WEAPON,
    RE15_SE_BANK_SND0,
    RE15_SE_BANK_SND1,
    RE15_SE_BANK_CORE
} re15_se_bank_kind_t;

static inline re15_se_bank_kind_t re15_audio_se_bank_kind(unsigned bank)
{
    switch (bank) {
        case 1:  return RE15_SE_BANK_WEAPON;
        case 2:  return RE15_SE_BANK_SND0;
        case 3:  return RE15_SE_BANK_SND1;
        case 4:  return RE15_SE_BANK_CORE;
        case 5:  return RE15_SE_BANK_SND0;   /* @0x8004513c lw a0,0x8 = snd0 (D6) */
        default: return RE15_SE_BANK_SKIP;   /* 0 (blob, not resident) + >=6 (invalid) */
    }
}

/* WEAPON SE by id (byte-true FUN_80045024 bank1 core): play the equipped weapon's resident ARMS SE
 * bank (SOUND/ARMS%02X.EDH/.VB, bank selector 1) sound for `se_id`. This is the PLAYER-FIRE path —
 * the GUNSHOT is se_id 8 (FUN_80035538/FUN_80011f50 -> FUN_80045024(0x1080001)). The bank is loaded
 * per equipped weapon (FUN_80043d8c); the port primes weapon 1 = ARMS01 (the ROOM1140 briefing
 * handgun, savestate-confirmed). Called C-side at the player fire in game_step. PSX = follow-up stub. */
void re15_audio_weapon_se(int se_id);

/* CORE-bank SE (byte-true FUN_80045024 bank selector 4 = the resident CORE00.EDH table
 * @0x801fbd00; Se_on(0x40NN0001) = record NN). Used by the devour-collapse SEs. */
void re15_audio_core_se(int se_id);

/* Re-prime the resident weapon SE bank (bank1) to `weapon_id`'s ARMS bank (byte-true FUN_80043d8c:
 * the equip-commit + room-init both re-load the equipped weapon's ARMS bank). Called by the
 * weapon-select menu on EQUIP so re15_audio_weapon_se then plays the newly-equipped weapon's SEs.
 * PC = load_weapon_se_vab_pc(weapon_id); PSX = follow-up stub. */
void re15_audio_prime_weapon(int weapon_id);

/* ===== RE2-Flavor: ENEMSE-Gegner-SE-Bank (WELLE A, PORT-OPTION, PC-only) ==========
 * RE2s globale, kind-getriebene Gegner-SE-Bank (COMMON/SOUND/ENEMSE.VBS) — der
 * RE1.5-Kontrast ist die RDT-snd1-Bank (re15_audio_room_se). Byte-Belege:
 *   - Bank-TOC @0x800a7b1c (EDT+VBD-Records; Loader FUN_8005a09c, file-id 0x161,
 *     Tags "ENEM EDT"/"ENEM VBD"; VH = EDT-Base + u32@[edt_size-8], SsVabOpenHeadSticky)
 *   - Trigger FUN_8005bd6c(se_id, actor): Actor-Flag 0x2000 -> se_id += 0x10
 *     (zweite Map-Haelfte = zweiter kind des Raum-Paars @0x800a7400); Map-Eintrag-
 *     Semantik in re2_ems.h (re2_enemse_decode_entry).
 * Bank-Wahl: das Original mappt das Raum-Gegner-Paar {DAT_800d8cd0/d1} ueber die
 * Paar-Tabelle @0x800a7400 auf den Bankindex (FUN_80052b38) — die kind-Basis dieser
 * Tabelle ist OFFEN (Lane I), darum waehlt der Port die Bank EXPLIZIT. Ohne gewaehlte
 * Bank spielt re15_audio_re2_enemy_se nichts (einmalige stderr-Notiz). */

/* Bank fuer die folgenden RE2-SEs waehlen (0..RE2_ENEMSE_BANK_COUNT-1). Lazy: die
 * eigentlichen EDT/VBD-Reads passieren beim ERSTEN re15_audio_re2_enemy_se. */
void re15_audio_re2_enemy_bank(int bank);

/* RE2-Gegner-SE (byte-true FUN_8005bd6c-Dekodierung): `flag2000` = Actor-Flag 0x2000
 * (vertauschtes Raum-Paar -> +0x10 in die zweite Map-Haelfte). PC-only; PSX: no-op. */
void re15_audio_re2_enemy_se(int se_id, int flag2000);

#endif /* RE15_AUDIO_H */
