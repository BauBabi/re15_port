/*
 * RE1.5 Rebuilt — RESIDENT EVIL 2 dog brain (Cerberus, kind 0x20; PORT OPTION, OPTIONS→AI→RE2).
 *
 * WELLE C. Everything here is RE'd from the RE2 (Leon retail) dog module
 *     info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN  == CDEMD0.EMS[0x25B000..+22266] (TOC sector 1206,
 *     EXE type table @0x8009ABF4 rec ModB; module loads RAW @0x80100000, module id word = 6)
 *     disassemble:  .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMD0G_MOD0.BIN
 * Full self-disassembly 2026-08-16 (scratchpad dog_self.asm, 5326 instr; every constant below was
 * re-read from that dump — Adressen im Zeilenkommentar).
 *
 * State table @0x80105438 (self-verified):
 *     [0] 0x801000F4 INIT   [1] 0x801004DC ACTIVE (dispatch +0x5 → 17 substates @0x80105464)
 *     [2] 0x801032A8 HURT   [3] 0x801040DC DEATH  [4] 0x801049E4 jr-ra stub  [5][6] NULL
 *     [7] 0x801049EC CORPSE
 *
 * The RE1.5 dog (root 0x8010d7f8 family, enemy_ai_common.c) stays byte-identical: this brain only
 * runs when re15_ai_flavor()==RE15_AI_FLAVOR_RE2, hooked at the top of re15_dog_ai_tick.
 *
 * PORT MAPPINGS (kein Original-Verhalten erfunden — jede Lücke ist markiert):
 *   - Navigator FUN_8004A808 (steer point +0x1C4/6) und Routen-Helfer FUN_8004AA50 sind nicht
 *     portiert → Steuerziel = Spielerposition (+ die byte-true STALK-Offsets @0x801054A8), Routen-
 *     Byte +0x218 bleibt 0. Auf freier Fläche deckungsgleich (Welle-B-W1-Muster).
 *   - Kollisionskontakt +0x110 Bit 0 → e->ai_contact & 1 (der SCA-Wall-Clamp-Tail in run_all
 *     schreibt ihn NACH dem AI-Tick, also liest die AI byte-true den Vorframe-Kontakt — dieselbe
 *     Phasenlage wie RE2s Frame-Stempel +0x114).
 *   - 0x800CFBF6-Bewegungsbits → pl->motion (RUN=100→0x4 @0x8003D18C, WALK=105→0x2 @0x8003CC80/
 *     @0x8003D6B4; Mapping-Beleg enemy_ai_re2_zombie.c re2z_cfbf6).
 *   - 0x800CFCFE (Spieler-Etage) → pl->floor.
 *   - Partner-Ziel [0x800CFE18] (+0x227-Pfad, Rudel-Latch-Opfer / Partner-Charakter) hat keinen
 *     Port-Produzenten → Ziel ist immer der Spieler; +0x227 bleibt 0 (dokumentiert, Zweig tot).
 *   - Die Latch-Roh-Kette @0x80101E18-EB4 (0x8003947C/0x80039514/0x800395B8) ist PAD-RUMBLE:
 *     beide Funktionen selbst decompiliert (RE2_Quellcode_V2) — sie befüllen die zwei Aktuator-
 *     Ringpuffer DAT_800EAAD8/DAT_800EAC28 (Motor-Envelope (p3-p2)*0x80/p1). Kein Port-Rumble-
 *     Subsystem → dokumentiert übersprungen. Die HÖRBAREN Latch-Sounds kommen byte-true über die
 *     EDD-Frame-Events (0x80016028: Bit 0x8000000 → SE = Wort>>28; ACTIVE-Tail: Bit 0x40000 → SE 8).
 *   - RE2-FX-System 0x8001BF10 (Tabelle @0x801056AC) ist nicht portiert → Blut-Kinds (0/1/2/3/5/7/8)
 *     spawnen das RE1.5-Room-Bank-Blut am Part (re15_enemy_bone_world_pos), Sabber (6) und die
 *     Sonder-Kinds (9/11 Glas) sind stumm (OPEN). Die BUDGET-Mechanik +0x21F ist byte-true.
 */

#include <stdint.h>
#include <stdlib.h>   /* getenv */
#include <stdio.h>
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 / re15_atan2_q12 */
#include "re15_damage.h"     /* re15_ai_arc_test (==FUN_80015614), re15_enemy_bone_world_pos */
#include "re15_enemy_ai.h"   /* re15_ai_set_state_word / victim shims / re15_actor_clip_len */
#include "re15_enemy.h"      /* re15_enemy_find (RE2 bank from the Welle-A loader) */
#include "re15_esp.h"        /* re15_esp_fx_spawn_ex (RE1.5 blood stand-in) */

/* zombie-file shims reused verbatim (enemy_ai_common.c / enemy_ai_re2_zombie.c) */
extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);

/* ---- ownership -------------------------------------------------------------------------- */
int re15_re2_owns_type(unsigned type)
{
    return re15_re2z_owns_type(type) || type == 0x20u
        || type == 0x21u;   /* WELLE D: RE2-Kraehe (EMOVL21_S0.BIN, enemy_ai_re2_crow.c) */
}

/* ---- ENEMSE audio hook ------------------------------------------------------------------ */
/* Dog ENEMSE bank: the ROOM-PAIR table @0x800A7400 (RE2 PSX.EXE file 0x97C00, 73 rows × 2
 * kinds, self-read 2026-08-16) lists kind 0x20 in 13 rows — ALWAYS as the SECOND kind
 * (rows 31/38/44/47/54/57/58/60/61/63/66/68/71) → the dog plays from the SECOND map half
 * (flag2000, se_id+0x10 — FUN_8005bd6c head "(*param_2 & 0x2000)"). Row 31 = {0x00,0x20} is
 * the pure dog bank. EMPIRICAL probe over all 73 decoded EDT maps (same method as the zombie
 * bank 0): every dog-pair bank carries the IDENTICAL dog half — live ids {0,1,2,3,6..14},
 * ids 4/5 = 0xFFFFFFFF (SILENT — the feed-bite SE 4 @0x80101B98 is byte-true silent!).
 * → default bank 31 + flag2000=1; RE15_RE2_DOG_SE_BANK overrides for A/B listening. */
#define RE2DOG_ENEMSE_BANK 31

static void (*s_re2d_se_fn)(int se_id, int flag2000) = 0;
static void (*s_re2d_bank_fn)(int bank) = 0;

void re15_re2dog_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int))
{
    s_re2d_se_fn   = se_fn;
    s_re2d_bank_fn = bank_fn;
    if (s_re2d_bank_fn) {
        const char *ov = getenv("RE15_RE2_DOG_SE_BANK");
        s_re2d_bank_fn(ov ? atoi(ov) : RE2DOG_ENEMSE_BANK);
    }
}
static void re2d_se(int id) { if (s_re2d_se_fn) s_re2d_se_fn(id, 1); }   /* flag2000: zweite Map-Hälfte */

/* ---- room-scoped globals (0x800CFBF4-Analog) --------------------------------------------
 * Bits the dog module reads/writes: 0x20 noise/lunge claim (set @0x8010135C, clear only in the
 * floor-lost helper @0x80105260), 0x40 pack-help signal (set at mash-release @0x8010205C),
 * 0x80 howl claim (set @0x80101950). The ONLY EXE writer is the room-init clear FUN_80052f3c
 * (@0x80052f3c, self-read decompile: DAT_800cfbf4 = 0) → room-scoped, reset here on room load.
 * WELLE D: BIT 0 DESSELBEN Worts ist der Kraehen-Flock-Mutex (Claim @0x801042C4-CC, Frei-Gate
 * @0x8010426C-7C, Release im Krähen-Post-Pass @0x801044E4-F0) → EIN geteiltes Global
 * (exportiert), kein zweites erfinden. */
uint16_t g_re2_room_gflags = 0;
#define s_re2d_gflags g_re2_room_gflags   /* lokale Lesbarkeit; dasselbe 0x800CFBF4-Analog */
void re15_re2dog_room_reset(void) { s_re2d_gflags = 0; }

/* ---- small helpers ---------------------------------------------------------------------- */

/* clip word write `sw (rate<<16)|(frame<<8)|clip, 332(s0)` → port anim fields (the same
 * convention Welle B established: rate half → crossfade seed, a3 of 0x8002959c → blend rate). */
static void re2d_clip(re15_actor_t *e, int clip, int frame, int frac, int blend, int loop)
{
    e->motion     = (int16_t)clip;
    e->anim_frame = (uint16_t)frame;
    e->anim_frac  = (uint8_t)frac;
    e->anim_blend_rate = (uint16_t)blend;
    e->anim_freeze = 0;
    e->anim_flags &= (uint16_t)~(0x80u | 0x04u);
    if (loop) e->anim_flags |= 0x04u;
}

static int re2d_frame_slot(const re15_actor_t *e)          /* +0x14D frame byte analog */
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return (int)e->anim_frame;
    return (int)(e->anim_frame % (uint32_t)fc);
}
/* (kein re2d_clip_done: das 959c-done kommt AUSSCHLIESSLICH aus re2d_advance am Wrap-Tick —
 * ein frame>=fc-1-Test hätte die falsche Semantik, Review-Fix #3) */

/* the dog SELF-ADVANCES its clip (type 0x20 is in re15_type_self_advances_anim, so the global
 * advancer skips it — like the RE1.5 dog). The original advances via 0x8002959c, dessen Kern
 * 0x80029614 SELBST NACHDISASSEMBLIERT ist (Review-Fix #3):
 *   80029b28: lbu v0,333(s2)   ; Frame-Byte +0x14D
 *   80029b30: addiu v0,v0,1 ; 80029b34: sb v0,333(s2)
 *   80029b3c: sltu v0,v0,s3    ; (frame+1) < clip-Laenge ?
 *   80029b40: bne -> weiter (v0=0)   80029b48: sb zero,333(s2)  ; WRAP auf 0
 *   80029b4c: addiu v0,zero,1        ; done NUR am Wrap-Tick
 * und der Crossfade-Zaehler +0x14E verfaellt pro Aufruf:
 *   800299c0: lbu v0,334(s2) ; 800299c8: addiu v0,v0,-1 ; 800299cc: sb v0,334(s2)
 * Port: 1 Frame/Tick, a3 → anim_blend_rate (Welle-B-Konvention); Wrap + frac-Decay exakt wie
 * oben. Loop-Clips laufen dadurch periodisch weiter (EDD-Frame-SEs feuern je Zyklus) und das
 * Lean-Gate frac<7 (@0x80104CBC) wird erreichbar. */
/* WELLE D: als GETEILTER Helfer exportiert (re15_ai_flavor.h) — die Kraehe faehrt DENSELBEN
 * EXE-Treiber 0x8002959c (44 jal im Kraehen-Modul), Kern-Zitate unveraendert die obigen. */
int re15_re2_advance_959c(re15_actor_t *e, int blend)
{
    e->anim_blend_rate = (uint16_t)blend;
    if (e->anim_frac > 0) e->anim_frac--;                  /* +0x14E-Decay @0x800299C0-CC */
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return 1;                                 /* kein Bank -> FSM nie stallen */
    uint32_t nf = e->anim_frame + 1u;                      /* addiu +1 @0x80029B30 */
    if ((int)nf >= fc) { e->anim_frame = 0; return 1; }    /* sb zero,333 + done @0x80029B48-4C */
    e->anim_frame = (uint16_t)nf;
    return 0;
}
static int re2d_advance(re15_actor_t *e, int blend) { return re15_re2_advance_959c(e, blend); }

/* EDD frame word of the CURRENT frame (event bits: 0x40000 SE-8 @0x8010051C-30, 0x8000000 →
 * SE = word>>28 via 0x80016028, 0x30000 blood @0x801027E8-848). Dog bank = pair 1 (27 clips,
 * dir[1]@0xC / dir[2]@0xEBC, 17 bones, byte-parsed EM_TYPE20.EMD). */
static uint32_t re2d_frame_word(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    const re15_emd_animation_t *A = &b->anim;
    if ((int)e->motion >= A->clip_count) return 0;
    const re15_emd_clip_t *c = &A->clips[e->motion];
    if (c->frame_count <= 0) return 0;
    return A->frames[c->first_frame + (e->anim_frame % (uint32_t)c->frame_count)];
}

/* 0x80016028 (self-disasm'd): frame word bit 0x8000000 → SE (word>>28). NO <2 clamp here
 * (unlike the zombie walk path 0x801016c8) — the dog EDD carries the id in the top nibble. */
static void re2d_frame_se_16028(const re15_actor_t *e)
{
    uint32_t fw = re2d_frame_word(e);
    if (fw & 0x08000000u) re2d_se((int)(fw >> 28));
}

/* FUN_800152C8(self, yaw_ofs) — speed vector +0x144/146/148 yaw-rotated onto the position
 * (self-disasm'd @0x800152E4-0x80015334: x += out.x, z += out.z). FUN_80015350 is the 3D
 * variant (@0x80015350-0x800153D8: additionally y += out.y; dog pitch +0x78 is never written
 * → out.y == vy exactly). The dog keeps +0x148 == 0 (INIT @0x80100310, no other writer). */
static void re2d_move(re15_actor_t *e, int yaw_ofs)
{
    int yaw = ((int)e->rot_y + yaw_ofs) & 0xfff;
    int spd = (int)e->speed_h;
    e->x += (int32_t)(((int32_t)re15_cos_q12(yaw) * spd) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(yaw) * spd) >> 12);
}
static void re2d_move3d(re15_actor_t *e, int yaw_ofs)      /* == 0x80015350 */
{
    re2d_move(e, yaw_ofs);
    e->y += (int32_t)e->re2d_vy146;
}

/* 0x80104BEC(step,cap): speed ramp-up; 0x80104C30(step,floor): ramp-down (self-disasm'd). */
static void re2d_speed_up(re15_actor_t *e, int step, int cap)
{
    if ((int)e->speed_h < cap) { e->speed_h = (int16_t)(e->speed_h + step);
                                 if ((int)e->speed_h > cap) e->speed_h = (int16_t)cap; }
}
static void re2d_speed_dn(re15_actor_t *e, int step, int flo)
{
    if ((int)e->speed_h > flo) { e->speed_h = (int16_t)(e->speed_h - step);
                                 if ((int)e->speed_h < flo) e->speed_h = (int16_t)flo; }
}

/* 0x80104C74(self, s): lean clip from tbl @0x801056A8 = {13, 2, 12} (self-read):
 * s==0 → [1]=2 (run), s>0 → [2]=12 (lean R), s<0 → [0]=13 (lean L).
 * Review-Fix #5, selbst nachdisassembliert: der Clip-Write ist der DELAY-SLOT des frac-Gates
 * und feuert auf BEIDEN Branch-Ausgaengen —
 *   80104cac: beq v0,v1,exit        ; Clip unveraendert -> raus
 *   80104cbc: sltiu v0,frac,7
 *   80104cc0: beq v0,zero,exit      ; (Bytes 03 00 40 10 = plain BEQ, kein BEQL)
 *   80104cc4: sb v1,332(a0)         ; DELAY-SLOT: Clip-Wechsel IMMER
 *   80104cc8: addiu v0,zero,15 ; 80104ccc: sb v0,334(a0)   ; Reseed NUR bei frac<7
 * Der Wechsel ist also unbedingt, nur der frac=15-Reseed haengt am Gate. */
static void re2d_lean(re15_actor_t *e, int s)
{
    static const uint8_t lean_tbl[3] = { 13, 2, 12 };      /* @0x801056A8 */
    int clip = lean_tbl[(s == 0) ? 1 : (s > 0 ? 2 : 0)];
    if ((int)e->motion != clip) {                          /* beq v0,v1 @0x80104CAC */
        int reseed = (e->anim_frac < 7);                   /* sltiu 7 @0x80104CBC */
        e->motion = (int16_t)clip;                         /* sb v1,332 @0x80104CC4 (Delay-Slot) */
        if (reseed) e->anim_frac = 15;                     /* sb 15,334 @0x80104CC8-CC */
        if ((int)e->anim_frame >= re15_actor_clip_len(e)) e->anim_frame = 0;
                                                           /* Port-Guard (Lean-Clips 13/2/12 sind
                                                            * alle 18 F — greift praktisch nie) */
    }
}

/* FUN_80015758(A,B,ang,half) — sector test around an arbitrary angle; 0 == inside
 * (clone of enemy_ai_re2_zombie.c re2z_sector — same −0x400 port mesh-yaw convention). */
static int re2d_sector(const re15_actor_t *a, const re15_actor_t *b, int ang, int half)
{
    int bearing = ((int)re15_atan2_q12(b->z - a->z, b->x - a->x) - 0x400) & 0xfff;
    unsigned t  = (unsigned)((bearing - ang + half) & 0xfff);
    return !(t < (unsigned)(half << 1));
}

/* 0x800CFBF6 movement bits from PORT state (Mapping-Beleg enemy_ai_re2_zombie.c re2z_cfbf6). */
static uint16_t re2d_cfbf6(const re15_actor_t *pl)
{
    if (pl->motion == 100) return 0x04u;     /* RUN  @0x8003D18C */
    if (pl->motion == 105) return 0x02u;     /* WALK @0x8003CC80 / @0x8003D6B4 */
    return 0;
}

/* 0x80105070(self, part, fx) — FX spawner with the +0x21F budget gate (@0x80105090-98) and
 * decrement (@0x8010518C-98). Effekt-Tabelle @0x801056AC, 6 B/Eintrag {kind|0x80, sub,
 * u16 spread, u16 param} — alle 60 Bytes selbst gelesen: kinds 0/4/5=Bluttropfen,
 * 1/2/3=Blutspritzer(9), 6=Sabber(0/0x15), 7/8=Gore(0x85), 9=0x84. fx==4 spawns at the PLAYER's
 * part (@0x801050C4-D0 — the latch blood drips ON Leon), everything else at the dog's part.
 * YAW (Review-Fix #16, selbst nachdisassembliert): Spread-Halbweite aus Tabellen-Feld +2
 * (`lhu a1,2(s1)` @0x80105100), Winkel = TARGET-Yaw (`lh a0,118(s0)` @0x80105104, s0 = at!)
 * + (rand & (2*spread−1)) − spread (Maske `sll v1,a1,1; addiu −1; and` @0x8010510C-114,
 * `addu a0,a0,v0` @0x80105118, Zentrierung `subu a1,a0,a1` @0x80105120). Spread==0-Einträge
 * (0/6/7/8/9): Maske −1 → +rand(0..255) unzentriert — byte-true.
 * PORT: RE1.5 room-bank blood at the part position (bone_world_pos); Sabber/9/11 silent (OPEN).
 * One RNG draw (@0x801050D4) is kept — the draw COUNT is behaviour. */
static const uint8_t re2d_fx_tbl[10][6] = {                /* @0x801056AC (60 B selbst gelesen) */
    {   0,    1, 0,0,  0x00,0x10 },   /* 0 Bluttropfen, spread 0      */
    {   9,    0, 0,2,  0x00,0x08 },   /* 1 Spritzer,    spread 0x200  */
    {   9,    1, 0,2,  0x00,0x08 },   /* 2 Spritzer,    spread 0x200  */
    {   9,    2, 0,2,  0x00,0x08 },   /* 3 Spritzer,    spread 0x200  */
    {   0,    1, 0,4,  0x00,0x10 },   /* 4 Tropfen,     spread 0x400  */
    {   0,    1, 0,2,  0x00,0x10 },   /* 5 Tropfen,     spread 0x200  */
    {   0, 0x15, 0,0,  0x00,0x04 },   /* 6 Sabber                     */
    { 0x85,   3, 0,0,  0x00,0x10 },   /* 7 Gore                       */
    { 0x85,   4, 0,0,  0x00,0x10 },   /* 8 Gore                       */
    { 0x84, 0x0F, 0,0,  0x00,0x14 },  /* 9                            */
};
static void re2d_fx(re15_actor_t *e, int part, int fx)
{
    if (e->re2d_budget21f == 0) return;                    /* @0x80105090-98 */
    uint32_t r = re15_re2_rand();                          /* @0x801050D4 yaw spread draw */
    e->re2d_budget21f--;                                   /* @0x8010518C-98 */
    if (fx == 6 || fx == 9 || fx >= 10) return;            /* Sabber/0x84/Glas: kein RE1.5-Pendant
                                                            * (OPEN; fx 11 laege ohnehin hinter dem
                                                            * Tabellenende @0x801056AC+60) */
    const re15_actor_t *at = (fx == 4) ? &g_actors[RE15_ACTOR_SLOT_PLAYER] : e;  /* @0x801050C4-D0 */
    int spread = (int)re2d_fx_tbl[fx][2] | ((int)re2d_fx_tbl[fx][3] << 8);   /* lhu 2(s1) @0x80105100 */
    int off    = (int)(r & (uint32_t)((spread << 1) - 1)) - spread;          /* @0x8010510C-120 */
    int32_t p[3];
    re15_enemy_bone_world_pos(at, part & 0x7f, p);         /* Part-Array +0x198 Stride 0xAC analog */
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500, p[0], p[1], p[2],
                         (int16_t)(((int)at->rot_y + off) & 0xfff));         /* lh 118(s0) @0x80105104 */
}

/* ---- 0x80104B98 — Kopf-/Part-Tracking (Review-Fund #13: bewusst OPEN, hier belegt) ---------
 * Von ~13 Sub-Tails gerufen (u.a. IDLE @0x80100700, STALK @0x80100A94, RUN @0x80100B74,
 * ANGRIFF @0x80101220, HOWL-P3 @0x80101AF0, FEED @0x80101BB0, RETREAT @0x80101CB4,
 * Sub 8/9/11/13/14/15 @0x8010234C/23E8/25E0/28E4/2C5C/2E8C). Selbst disassembliert
 * @0x80104B98-BE8: +0x1C1=2 → jal 0x80017D98(self, yaw +0x76) → +0x1C1=3 → jal erneut →
 * +0x1C1=2 restore. 0x80017D98 (EXE, selbst angelesen): Gate `lbu 448(s1); andi 1`
 * (+0x1C0 & 1 @0x80017DC4-CC), Part-Struct via +0x1C1×0xAC, Ziel *(self+0x1B4) (=Spieler-
 * Block, INIT @0x801002A8), schreibt Part-Winkel (`sh …,152/106`) = NACKEN-/KIEFER-
 * NACHFÜHRUNG der Parts 2/3 zum Spieler. Der Port hat keinen Gegner-Part-Winkel-Kanal
 * (Head-Look existiert nur als NPC-Plc_neck auf Bone 3) → NICHT portiert, OPEN. Die
 * portierten +0x1C0-Bit-1-Schreiber (|=0x2 / &=0xFD) gaten im Original genau dieses Tracking. */

/* ---- FUN_800401d4 (RE2 EXE, decompile self-read RE2_Quellcode_V2/FUN_800401d4.c) ----------
 * a0 = DAMAGE, a1 = mode (dog: 0 @0x80104EC0). HP 0x800CFD4E (PL+0x156) -= dmg. mode 0:
 * HP>=0 → 0 (survived); HP<-14 ODER One-Save-Latch (0x800CFD4C&0x1000) → 0x800CFB74|=0x4000000,
 * PL+0x1D3|=0x80, return 2 (TOD); sonst HP=0 + Latch, return 1 (One-Save). Die Skalierungen
 * (×1.5 bei Flag 0x40/HP>120, ×5/×2 bei Difficulty 3 @0x800D482A) hängen an RE2-only-Globals
 * ohne RE1.5-Pendant — wie in Welle B dokumentiert NICHT übernommen; Roh-Schaden 1:1.
 * Implementierung geteilt mit dem Zombie (EIN One-Save-Latch, re15_re2_player_damage). */
extern int re15_re2_player_damage(re15_actor_t *pl, int dmg);

/* ---- shared per-tick helpers 0x80105234 / 0x80105274 ------------------------------------- */
static void re2d_check_floor_lost(re15_actor_t *e, const re15_actor_t *pl)
{
    /* 0x80105234 (self-disasm'd): player floor != +0x106 → sub 9 + clear noise bit 0x20
     * (@0x8010524C-68 — der EINZIGE Clear des 0xFBF4-Bit-0x20). */
    if (pl->floor != e->floor) {
        e->sub_state_1 = 9; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 9,5 / sh zero,6 */
        s_re2d_gflags &= (uint16_t)~0x20u;                            /* andi 0xffdf @0x80105260 */
    }
}
static void re2d_check_pack_help(re15_actor_t *e)
{
    /* 0x80105274 (self-disasm'd): 0xFBF4 & 0x40 → sub 11 (@0x80105280-90). */
    if (s_re2d_gflags & 0x40u) { e->sub_state_1 = 11; e->sub_state_2 = 0; e->sub_state_3 = 0; }
}

/* drool budget reseed, shared by IDLE tail (@0x80100708-48) and STALK tail (@0x80100AA4-DC):
 * budget==0 → 1/64 (rand&0x3f==0) → second draw → budget = tbl@0x80105420[rand&7]. */
static const uint8_t re2d_budget_tbl[8]  = { 7, 31, 15, 15, 15, 15, 7, 31 };     /* @0x80105420 */
static const uint8_t re2d_wait_tbl[8]    = { 10, 20, 30, 20, 30, 20, 10, 20 };   /* @0x80105428 */
static const uint8_t re2d_howl_tbl[8]    = { 15, 30, 45, 60, 60, 45, 30, 15 };   /* @0x80105430 */
static void re2d_drool_budget(re15_actor_t *e)
{
    if (e->re2d_budget21f != 0) return;
    if ((re15_re2_rand() & 0x3fu) == 0u)                   /* @0x80100718-24 */
        e->re2d_budget21f = re2d_budget_tbl[re15_re2_rand() & 7u];   /* @0x8010072C-48 */
}

/* ---- 0x80104CD8: the attack decision (RUN P1/P3) -----------------------------------------
 * self-disasm'd @0x80104CD8-0x80104DD0. Returns: 1 attack, 0 no, −1 outside 256-sector,
 * −2 target dead / fatigued retreat, −4 target claimed. */
static int re2d_attack_decide(re15_actor_t *e, re15_actor_t *pl)
{
    if (pl->hp < 0) return -2;                             /* lh 342; bltz @0x80104D10-18 */
    if (e->ai_dist >= 4000u) return 0;                     /* slti 4000 @0x80104D40-4C */
    if (pl->floor != e->floor) return 0;                   /* @0x80104D54-60 */
    if (re2d_sector(e, pl, (int)e->rot_y & 0xfff, 256) != 0)   /* 0x80015758(yaw,256) @0x80104D68-7C */
        return -1;
    if (re15_player_is_grabbed()) return -4;               /* PL+0x1D3&0x80 @0x80104D84-90 */
    if (pl->hp < 21) {                                     /* slti 21 @0x80104D98-A4 */
        /* 0x80015910(self,PL)==0 → 1 (Delay-Slot v0=1 @0x80104DB8); sonst Fatigue<600 → −2 */
        if (!re15_ai_facing_aligned(e, pl)) return 1;      /* MAPPING: 15910-Richtungs-Kollaps
                                                            * (Welle-B-Konvention @0x80102714) */
        if ((int16_t)e->re2z_flags21a < 600) return -2;    /* lh 2(s2); slti 600 @0x80104DBC-C8 */
        return 1;                                          /* @0x80104DD0 */
    }
    return 1;                                              /* @0x80104DD0 */
}

/* ---- 0x80104DF0: contact/damage (called from the flight prober) ---------------------------
 * self-disasm'd @0x80104DF0-0x80104FC8. Player path only (a1=1 partner bite 0x80065B9C has no
 * port target — dokumentiert tot). Gates: +0x21E==0, PL nicht geclaimt, Jaw-Nähe 0x800157D4
 * (part 4 + Radius 700/1000 je PL-HP<21 @0x80104E34-48 — PORT: dist bereits über den Sprung-
 * Kontakt approximiert, s.u.), Y > Boden−1800 (@0x80104E84-94), gleiche Etage (@0x80104E9C-A8).
 * Dann +0x21A=0 (@0x80104EB4 Delay-Slot) + FUN_800401D4(20, 0) (@0x80104EB8-C0 — der BISS-
 * SCHADEN 20 STEHT IM MODUL). Rückgabe 2 (tödlich) → LATCH (@0x80104F00-74): +0x21E=2,
 * PL+0x1D3=255, PL-Flags|=0xA, PL-Yaw=Hund+2048, self+0x1D3|=0x80. Sonst → Boden-Biss
 * (@0x80104F7C-88): +0x6=3, +0x21E=1; lebendes Ziel → 0x801051BC (PL state 2 sub dir+2,
 * PL+0x1D3|=0x80 @0x801051DC-200); totes → 0x80105204 (PL +0x4=3). */
static int re2d_contact(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2d_bite21e != 0) return (int)e->re2d_bite21e; /* @0x80104E4C-54 */
    if (re15_player_is_grabbed()) return 0;                /* PL+0x1D3&0x80 @0x80104E5C-68 */
    /* Jaw-Nähe 0x800157D4(PL-Pos, Part-4-Pos, r=700/1000): Port-Näherung über die Wurzel-
     * Distanz — Part 4 (Kiefer) liegt beim gestreckten Sprung ~Actor-Front; MAPPING. */
    {
        int32_t dx = pl->x - e->x, dz = pl->z - e->z;
        int32_t r  = (pl->hp < 21) ? 700 : 1000;           /* @0x80104E34-48 */
        if (dx * dx + dz * dz > r * r) return 0;
    }
    if (!(e->y > (int32_t)e->dog_floor_y - 1800)) return 0;/* @0x80104E84-94 (+0x1C2-Analog) */
    if (pl->floor != e->floor) return 0;                   /* @0x80104E9C-A8 */
    e->re2z_flags21a = 0;                                  /* sh zero,2(s3) @0x80104EB4 (Fatigue-Reset) */
    int r = re15_re2_player_damage(pl, 20);                /* FUN_800401D4(20,0) @0x80104EB8-C0 */
    if (r == 2                                             /* bne v1,2 @0x80104EE0-E4 */
        && !re15_ai_facing_aligned(e, pl)                  /* 0x80015910==0 @0x80104EEC-F8 (MAPPING) */
        && pl->state != 15) {                              /* PL+0x8==15 → kein Latch @0x80104F10-18
                                                            * (Port: kein Zustand 15 → Gate offen) */
        /* LATCH @0x80104F20-74 */
        e->re2d_bite21e = 2;                               /* sb 2,6(s3) @0x80104F24 */
        e->speed_h = 0;                                    /* sh zero,324 @0x80104F28 */
        pl->rot_y = (int16_t)(((int)e->rot_y + 2048) & 0xfff);   /* @0x80104F40-50 */
        e->re2z_self1d3 |= 0x80u;                          /* @0x80104F64-74 */
        re15_re2z_victim_begin(e, pl, 0);                  /* PL+0x1D3=255/Flags|=0xA → Port-Victim-
                                                            * Pin (Welle-B-Shim); Richtung frontal
                                                            * (Yaw=Hund+2048 erzwingt sie) */
        return 2;
    }
    /* Boden-Biss @0x80104F7C-88 */
    e->sub_state_2 = 3; e->sub_state_3 = 0;                /* sh v0,6(s0) v0=3 */
    e->re2d_bite21e = 1;                                   /* sb 1,6(s3) @0x80104F84 */
    if (pl->hp >= 0) {
        /* 0x801051BC(0, dir): PL +0x4=2 (HURT), +0x5=dir+2, Claim @0x801051DC-200. PORT: der
         * Schaden ist schon oben gefallen (800401d4); die Spieler-Hit-Reaktion läuft über die
         * Port-Hit-Chain (hit_react), der Claim über den Grab-Pin unterbleibt (kein Latch). */
        pl->hit_react |= 0x1;
    } else {
        e->anim_flags |= 0;                                /* 0x80105204: PL+0x4=3 — Port-Spieler-
                                                            * Todespfad läuft über player_common */
    }
    return 1;
}

/* flight prober 0x80104FF0 (self-disasm'd): +0x22C (Kreis/Fenster) → nie Latch (@0x80105000-08);
 * sonst Kontakt vs Spieler; Partner-Zweig ([0x800CFBD8]&0x10000000 @0x80105024-34) tot (s.o.). */
static int re2d_flight_probe(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2d_nolatch22c != 0) return 0;
    return re2d_contact(e, pl);
}

/* launch helper 0x80101380(self, a1=speed, a2=vy) — self-disasm'd @0x80101380-C0:
 * +0x158=8 (Windup), +0x144=a1, +0x146=a2, Clip-Wort 0x70014 (Clip 20 Rate 7),
 * +0x21E=0, +0x21F=0, word0=(&0xEFFFFFFF)|0x4000000. */
static void re2d_launch(re15_actor_t *e, int speed, int vy)
{
    e->re2z_t158 = 8;                                      /* sh 8,344 @0x80101398 */
    e->speed_h   = (int16_t)speed;                         /* sh a1,324 @0x801013A0 */
    e->re2d_vy146 = (int16_t)vy;                           /* sh a2,326 @0x801013A4 */
    re2d_clip(e, 20, 0, 7, 0x200, 0);                      /* sw 0x70014,332 @0x801013A8 */
    e->re2d_bite21e = 0; e->re2d_budget21f = 0;            /* sb zero,542/543 @0x801013AC-B0
                                                            * (Review-Fix #11: Zitate lagen 0x1000
                                                            * daneben — 0x801003xx ist INIT-Code) */
}

/* flight core 0x80101488 (self-disasm'd): 3D-Integrator + Gravity 40 + Kontakt-Probe.
 * Rückgabe: −1 Touchdown, 1 Kontakt (=FF0==1 Boden-Biss), 0 weiter (Delay-Slot-Semantik
 * `xori 1; sltiu` @0x801014CC-DC + `addiu s1,-1` @0x801014EC selbst nachvollzogen). */
static int re2d_flight(re15_actor_t *e, re15_actor_t *pl)
{
    re2d_move3d(e, 0);                                     /* jal 0x80015350(0,0) @0x801014B0 */
    e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40);         /* +0x146 += 40 @0x801014B8-C8 */
    int probe = re2d_flight_probe(e, pl);                  /* jal 0x80104FF0 @0x801014C4 */
    int s1 = (probe == 1);
    if (e->re2d_vy146 >= 0 && !s1) return -1;              /* bltz/bne @0x801014D8-E0 → Touchdown */
    if (re2d_frame_slot(e) != 0) re2d_advance(e, 0x200);   /* lbu 333; advance(512) @0x801014F0-508 */
    return s1;                                             /* @0x80101510 */
}

/* landing helper 0x80101574 (self-disasm'd): P(+0x7==0): 3D-Integrator + Gravity, bei
 * y>=Boden(+0x1C2): +0x7=1, y=Boden, +0x219=0, SE 10 (@0x801015C8-600); Frame==8 hält den
 * Advance aus (@0x80101604-0C); P(+0x7==1): move + Ramp-down(64,240) + advance → Rückgabe 1. */
static int re2d_landing(re15_actor_t *e)
{
    if (e->sub_state_3 == 0) {
        re2d_move3d(e, 0);                                 /* 0x80015350(0,0) @0x801015A4 */
        e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40);     /* @0x801015AC-C4 */
        if (e->y >= (int32_t)e->dog_floor_y) {             /* slt @0x801015BC-C0 */
            e->sub_state_3 = 1;                            /* sb 1,7 @0x801015E4 */
            e->re2d_air219 = 0;                            /* sb zero,537 @0x801015EC */
            e->y = (int32_t)e->dog_floor_y;                /* sw a2,60 @0x801015F8 */
            re2d_se(10);                                   /* jal 0x8005bd6c(10) @0x801015FC */
        }
        if (re2d_frame_slot(e) != 8) re2d_advance(e, 0x100);   /* @0x80101604-20 */
        return 0;
    }
    re2d_move(e, 0);                                       /* 0x800152C8(0) @0x80101630 */
    re2d_speed_dn(e, 64, 240);                             /* 0x80104C30(64,240) @0x80101640 */
    return re2d_advance(e, 0x100);                         /* @0x80101654-5C */
}

/* 0x80104088(self, a1): Hitbox-Squash 0↔1 (@0x80104090-D8): a1==0 → +0x98=−500/+0x9E=500,
 * a1==1 → ±1000. Der Port führt die Gegner-Hitbox über hit_radius/atk-pt — dokumentiert NOP. */
static void re2d_hitbox(re15_actor_t *e, int restore) { (void)e; (void)restore; }

/* ============================== ACTIVE substates ========================================== */

/* shared idle-wander helper 0x80100548 (subs 0 und 10; self-disasm'd @0x80100548-0x80100780).
 * Phasen +0x6: 0 Init (+0x144=30, +0x1C0|=0x2, kein Clip-Write), 1 Vorwärtstrott + Kontakt →
 * 1/32 Pause(2, 90f Clip 0) sonst Roam(3), 2 Pause-Timer → 3 + Clip 1, 3 Nav+Steer(16)+Move,
 * Kontakt weg → 1. Rückgabe = Navigator-Ergebnis (Wake) — Port-MAPPING: 0 (kein Navigator).
 * Tail: advance 256 + Drool-Budget + FX(4,6). */
static int re2d_idle_wander(re15_actor_t *e, const re15_actor_t *pl)
{
    int wake = 0;
    switch (e->sub_state_2) {
    case 0:
        e->sub_state_2 = 1;                                /* sb 1,6 @0x801005C0 */
        e->speed_h = 30;                                   /* sh 30,324 @0x801005C4-C8 */
        /* +0x148=0 (`sh zero,328` @0x801005D8) — der Port fuehrt kein +0x148-Feld (immer 0) */
        e->re2d_air219 = 0;                                /* sb zero,537 @0x801005DC */
        e->re2d_wound22d = 0;                              /* sb zero,557 @0x801005E0 (Review-Fix
                                                            * #10: war Selbstzuweisung) */
        break;
    case 1:
        re2d_move(e, 0);                                   /* 0x800152C8(0) @0x80100610 */
        if (e->ai_contact & 1) {                           /* +0x110&1 @0x80100618-24 */
            if ((re15_re2_rand() & 0x1fu) == 0u) {         /* @0x8010062C-38 */
                e->sub_state_2 = 2;                        /* v1=2 @0x80100640 */
                e->re2z_t15a = 90;                         /* sh 90,346 @0x80100660 */
                re2d_clip(e, 0, 0, 0xF, 0x100, 1);         /* 0xF0000 @0x80100664-6C */
            } else e->sub_state_2 = 3;                     /* v1=3 @0x8010063C */
            e->re2d_route218 = 0;                          /* 0x8004AA50 → +0x218 (MAPPING: 0) */
        }
        break;
    case 2:
        if (--e->re2z_t15a == 0) {                         /* @0x80100670-84 */
            e->sub_state_2 = 3;                            /* sb 3,6 @0x80100694 */
            re2d_clip(e, 1, 0, 0xF, 0x100, 1);             /* 0xF0001 @0x8010068C-9C */
        }
        break;
    default:                                               /* 3: Roam @0x801006A0 */
        /* Navigator 0x8004A808(route,1) → s2 (Wake); MAPPING: Steuerziel = Spieler, wake 0 */
        re15_enemy_steer_point(e, pl->x, pl->z, 16);       /* 0x80015558(+0x1C4/6,16) @0x801006C0 */
        re2d_move(e, 0);                                   /* @0x801006CC */
        if (!(e->ai_contact & 1)) e->sub_state_2 = 1;      /* @0x801006D4-E8 */
        break;
    }
    re2d_advance(e, 0x100);                                /* 0x8002959c(256) @0x801006F8 */
    re2d_drool_budget(e);                                  /* @0x80100708-48 */
    re2d_fx(e, 4, 6);                                      /* FX(4,6) Sabber @0x80100750-58 */
    return wake;
}

/* sub 0 IDLE/Schlaf @0x80100784 */
static void re2d_sub0_idle(re15_actor_t *e, re15_actor_t *pl)
{
    int wake = re2d_idle_wander(e, pl);                    /* jal 0x80100548 @0x80100790 */
    if (pl->floor == e->floor) {                           /* 0x800CFCFE vs +0x106 @0x80100798-A8 */
        int go = 0, tgt = 1;
        if (e->ai_dist < 0xfa0u) go = 1;                   /* dist<4000 @0x801007B0-BC */
        else if (wake == 1) go = 1;                        /* Helfer-Wake @0x801007C4-C8 */
        else if (s_re2d_gflags & 0x20u) go = 1;            /* Noise-Bit @0x801007D0-E0 */
        else if (re2d_cfbf6(pl) & 0x1u) { go = 1; tgt = 2; }   /* 0xFBF6&1 @0x801007E8-F8 (toter
                                                                * Zweig: Bit 1 hat keinen Setzer —
                                                                * Zombie-Census, Welle B) */
        if (go) { e->sub_state_1 = (uint8_t)tgt;           /* sb v0,5 @0x80100800 */
                  e->sub_state_2 = 0; e->sub_state_3 = 0; }/* sh zero,6 @0x80100804 */
    }
    re2d_check_pack_help(e);                               /* jal 0x80105274 @0x80100808 */
}

/* sub 1 STALK @0x80100824 */
static const int32_t re2d_off_tbl[8] = { 1024, 0, -512, 0, -1024, 0, 512, 0 };   /* @0x801054A8 */
static void re2d_sub1_stalk(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                             /* P0 @0x80100854 */
        e->sub_state_2 = 1;                                /* sb 1,6 @0x80100860 */
        e->speed_h = 30;                                   /* sh 30,324 @0x80100864-68 */
        e->re2z_dir16a = 0;                                /* Aggro-Meter clear @0x80100878 */
        e->re2d_air219 = 0; e->re2d_wound22d = 0;          /* @0x8010087C-80 */
        e->re2z_self1d3 &= 0x7fu;                          /* @0x8010088C-94 */
        e->re2d_offx228 = (int16_t)re2d_off_tbl[re15_re2_rand() & 7u];   /* @0x801008AC-C4 */
        e->re2d_offz22a = (int16_t)re2d_off_tbl[re15_re2_rand() & 7u];   /* @0x801008C0-E0 */
    }
    /* Ziel: Navigator 0x8004A808(target+off) → +0x1C4/6; MAPPING: Spieler + Offsets */
    int32_t tx = pl->x + e->re2d_offx228, tz = pl->z + e->re2d_offz22a;  /* @0x801008F4-928 */
    int arc = re15_ai_arc_test(e, tx, tz, 16);             /* 0x80015614(...,16) @0x80100938-3C */
    e->rot_y = (int16_t)(((int)e->rot_y + arc) & 0xfff);   /* yaw += arc @0x80100944-54 */
    re2d_move(e, 0);                                       /* 0x800152C8(0) @0x80100950 */

    if (e->ai_dist < 0xbb8u) {                             /* dist<3000 @0x80100958-64 */
        int8_t a = (int8_t)e->re2z_dir16a;
        a = (int8_t)(a + 1);                               /* +1/Tick @0x8010096C-78 */
        if (re2d_cfbf6(pl) & 0x4u) a = (int8_t)((int8_t)e->re2z_dir16a + 21);  /* rennt: +21
                                                            * (addiu v0,v1,21 @0x80100990-94) */
        e->re2z_dir16a = (uint8_t)a;
        if (re2d_sector(e, pl, (int)e->rot_y & 0xfff, 128) == 0    /* @0x80100998-A8 */
            && e->ai_dist >= 0x259u) {                     /* NICHT <601 (sltiu 0x259; bne→skip
                                                            * @0x801009BC-C0; Commit @0x801009C8):
                                                            * die Lunge braucht >=601 Anlauf —
                                                            * Lane-D-Report ("Dist<601") hier nach
                                                            * eigenem Disasm KORRIGIERT */
            e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 3,5 @0x801009C8 */
        }
    } else {                                               /* fern @0x801009D4 */
        if ((int8_t)e->re2z_dir16a > 0)
            e->re2z_dir16a--;                              /* −1/Tick @0x801009DC-E8 */
        if (re2d_sector(e, pl, (int)e->rot_y & 0xfff, 128) == 0   /* @0x801009EC-A00 */
            && (re2d_cfbf6(pl) & 0x4u))                    /* rennt @0x80100A08-18 */
            e->re2z_dir16a = (uint8_t)((int8_t)e->re2z_dir16a + 20);   /* +20 @0x80100A20-2C */
    }
    if ((int8_t)e->re2z_dir16a >= 65                       /* Aggro>=65 @0x80100A30-3C */
        || (re2d_cfbf6(pl) & 0x1u)                         /* @0x80100A44-54 (toter Zweig) */
        || (s_re2d_gflags & 0x20u)) {                      /* Noise @0x80100A5C-6C */
        e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 2,5 @0x80100A74-78 */
        e->re2z_dir16a = 0;                                /* sb zero,362 @0x80100A7C */
    }
    re2d_advance(e, 0x100);                                /* @0x80100A8C */
    re2d_drool_budget(e);                                  /* @0x80100AA4-DC */
    re2d_fx(e, 4, 6);                                      /* Drool FX(4,6) @0x80100AE4-EC */
    re2d_check_floor_lost(e, pl);                          /* jal 0x80105234 @0x80100AF0 */
    re2d_check_pack_help(e);                               /* jal 0x80105274 @0x80100AF8 */
}

/* sub 2 RUN @0x80100B20, Phasen @0x801054C8 {P0 0x80100BA8, P1 0x80100C98, P2 0x80100E4C,
 * P3 0x80100F7C} */
static const uint16_t re2d_packturn_tbl[8] = { 32, 64, 48, 64, 32, 64, 48, 64 };   /* @0x801054D8 */
static void re2d_sub2_run(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: {                                              /* P0 @0x80100BA8 */
        re2d_clip(e, 2, 0, 0xF, 0x100, 1);                 /* 0xF0002 @0x80100BB4-D8 */
        e->re2d_turn224 = 96;                              /* sh 96,548 @0x80100BCC/BE4 */
        e->sub_state_2 = 1;                                /* sb 1,6 @0x80100BF0 */
        e->re2d_nolatch22c = 0; e->re2d_air219 = 0; e->re2d_wound22d = 0;  /* @0x80100BF4-FC */
        e->re2z_self1d3 &= 0x7fu;                          /* @0x80100C00-08 */
        /* 0x8001a5c0(self,0,0x20,0) — zählt ANDERE aktive kind-0x20-Entities (Entity-Tabelle
         * 0x800CFE14, word0&1 + +0x8==0x20, self ausgenommen; selbst disasm'd). Rudel → Turn-
         * Rate aus der Tabelle statt 96 (@0x80100C2C-54). */
        {
            int pack = 0;
            for (int i = 1; i < RE15_ACTOR_MAX; i++)
                if (g_actors[i].active && g_actors[i].type == 0x20 && &g_actors[i] != e) pack++;
            if (pack)
                e->re2d_turn224 = (int16_t)re2d_packturn_tbl[re15_re2_rand() & 7u];
        }
        if (e->re2d_circle22e) {                           /* Kreis-Modus (Spawn 9) @0x80100C58-60 */
            e->sub_state_2 = 3; e->sub_state_3 = 0;        /* sh 3,6 @0x80100C6C */
            e->re2d_route218 = 0;                          /* 0x8004AA50 (MAPPING) @0x80100C68-70 */
            e->re2d_nolatch22c = 1;                        /* sb 1,556 @0x80100C74 */
            e->re2d_stuck230 = 0; e->re2z_t15a = 0;        /* @0x80100C78-7C */
        }
        break;
    }
    case 1: {                                              /* P1 CHASE @0x80100C98 */
        re2d_speed_up(e, 40, 240);                         /* 0x80104BEC(40,240) @0x80100CBC */
        int s2 = 0;
        if (e->re2d_pause21d == 0) {
            /* Navigator (MAPPING: Spieler) + Arc(rate +0x224) @0x80100CEC-D04 */
            s2 = re15_ai_arc_test(e, pl->x, pl->z, (int)e->re2d_turn224);
            if (e->re2d_stuck230 >= 16 && s2 == 0)         /* Unstick @0x80100D08-24 */
                s2 = (int)e->re2d_turn224;
            e->rot_y = (int16_t)(((int)e->rot_y + s2) & 0xfff);   /* @0x80100D28-38 */
        } else {
            e->re2d_pause21d--;                            /* @0x80100D3C-40 */
            if (e->re2d_pause21d == 0 || (e->ai_contact & 1))     /* @0x80100D44-5C */
                { e->re2d_pause21d = 0; e->re2d_rel220 = 0; }     /* @0x80100D64-68 */
        }
        re2d_move(e, 0);                                   /* 0x800152C8(0) @0x80100D70 */
        re2d_lean(e, s2);                                  /* 0x80104C74 @0x80100D80 */
        int d = re2d_attack_decide(e, pl);                 /* 0x80104CD8 @0x80100D88 */
        if (d > 0) {                                       /* bgez @0x80100D9C */
            e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 3,5 @0x80100DD8 */
            break;
        }
        if (d < 0) {
            e->re2z_flags21a = (uint16_t)(e->re2z_flags21a + 10);   /* Fatigue+10 @0x80100DA4-B0 */
            if (d == -2) { e->sub_state_1 = 8; e->sub_state_2 = 0; e->sub_state_3 = 0; break; }
                                                           /* @0x80100DB4-C4 */
            if (d == -4) e->re2d_abort21c = 1;             /* sb 1,4(s1) @0x80100DC8-D4 */
        }
        if ((int16_t)e->re2z_flags21a >= 451) {            /* Fatigue>=451 @0x80100DE4-F0 */
            /* Review-Fix #1, selbst nachdisassembliert: `bne(HP<21) -> 0x80100E10` mit
             * Delay-Slot `addiu v0,zero,1` @0x80100E08; `sh zero,2(s1)` @0x80100E0C liegt NUR
             * auf dem HP>=21-Fallthrough; `sb v0,4(s1)` @0x80100E10 ist Branch-Ziel UND
             * Fallthrough -> Abort feuert IMMER, der Fatigue-Clear nur bei HP>=21. */
            if (pl->hp >= 21) e->re2z_flags21a = 0;        /* sh zero,2(s1) @0x80100E0C */
            e->re2d_abort21c = 1;                          /* sb v0,4(s1) @0x80100E10 (beide Pfade) */
        }
        if (e->re2d_abort21c) {                            /* @0x80100E14-20 */
            e->re2d_abort21c = 0;                          /* sb zero,4(s1) @0x80100E24 */
            e->sub_state_2 = 2; e->sub_state_3 = 0;        /* sh 2,6 @0x80100E28 */
        }
        break;
    }
    case 2: {                                              /* P2 Halbkreis-Abdrehen @0x80100E4C */
        if (e->sub_state_3 == 0) { e->sub_state_3 = 1; e->re2z_t158 = 0; }   /* @0x80100E60-74 */
        int s0 = re15_ai_arc_test(e, pl->x, pl->z, 96);    /* rate 96 @0x80100EA0-A4 (MAPPING nav) */
        re2d_move(e, -(int)e->re2z_t158);                  /* 0x800152C8(−Akku) @0x80100EAC-B8 */
        re2d_lean(e, s0);                                  /* @0x80100EC4 */
        if (e->sub_state_3 == 1) {
            re2d_speed_dn(e, 6, 10);                       /* 0x80104C30(6,10) @0x80100EE0 */
            if ((int)e->speed_h < 11) e->sub_state_3 = 2;  /* slti 11 @0x80100EE8-F4 */
        } else {
            re2d_speed_up(e, 6, 240);                      /* 0x80104BEC(6,240) @0x80100F14 */
        }
        e->rot_y = (int16_t)(((int)e->rot_y + s0) & 0xfff);/* @0x80100F1C-4C */
        e->re2z_t158 = (int16_t)(e->re2z_t158 + s0);       /* Akku @0x80100F20-2C */
        {
            int a = (int)e->re2z_t158; if (a < 0) a = -a;
            if (a >= 2048 || s0 == 0)                      /* @0x80100F38-58 */
                { e->sub_state_2 = 1; e->sub_state_3 = 0; }/* sb 1,6 @0x80100F5C-60 */
        }
        break;
    }
    default: {                                             /* P3 Wegpunkt-Kreisen @0x80100F7C */
        if (e->re2d_atkcd22f) e->re2d_atkcd22f--;          /* @0x80100FB8-CC */
        re2d_speed_up(e, 40, 240);                         /* @0x80100FD4 */
        /* Nav Routen-Modus (a3=1) @0x80100FE8 + Wegpunkt-Advance @0x801010B4-70: OPEN (Route
         * 0x8004AA50/[0x800CE324] nicht portiert) → das Kreisen steuert über die Arcs. */
        int s1p = re15_ai_arc_test(e, pl->x, pl->z, 64);   /* @0x80101000-04 */
        int s3  = re15_ai_arc_test(e, pl->x, pl->z, 128);  /* Ziel-Arc @0x80101018-1C */
        if (e->re2z_t15a != 0) {                           /* Unstick-Fenster @0x80101020-28 */
            e->re2z_t15a--;                                /* @0x80101034-3C */
            s1p = (int8_t)e->re2z_dir16a;                  /* lb 362 @0x80101030 */
        } else if (e->re2d_stuck230 >= 16) {               /* @0x80101040-4C */
            e->re2z_t15a = 30;                             /* sh 30,346 @0x80101054-5C */
            e->re2z_dir16a = (uint8_t)((s1p == 0) ? -(s3 >> 1) : -s1p);   /* @0x80101058-70 */
        }
        e->rot_y = (int16_t)(((int)e->rot_y + s1p) & 0xfff);   /* @0x80101078-88 */
        re2d_move(e, 0);                                   /* @0x80101084 */
        re2d_lean(e, s1p);                                 /* @0x80101094 */
        if (e->re2d_atkcd22f == 0) {                       /* @0x80101174-7C */
            int d = re2d_attack_decide(e, pl);             /* @0x80101184 */
            if (d != 0 || s3 == 0) {                       /* bne/bne @0x8010118C-94 */
                e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* @0x8010119C-A8 */
                e->re2d_atkcd22f = (uint8_t)(re2d_howl_tbl[re15_re2_rand() & 7u] + 30);
                                                           /* 45..90 @0x801011A4-C4 */
            }
        }
        break;
    }
    }
    re2d_advance(e, 0x100);                                /* Tail @0x80100B6C */
    re2d_check_floor_lost(e, pl);                          /* @0x80100B7C */
    re2d_check_pack_help(e);                               /* @0x80100B84 */
}

/* sub 3 ANGRIFF @0x801011EC, Phasen @0x801054E8 {0x8010124C, 0x801013C4, 0x80101534, 0x80101684} */
static void re2d_sub3_attack(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 Absprung @0x8010124C */
        if (e->sub_state_3 == 0) {
            e->sub_state_3 = 1;                            /* sb 1,7 @0x80101280-88 */
            re2d_launch(e, 65, -280);                      /* 0x80101380(65,−280) @0x80101278-84 */
        }
        /* Clip-12/13-Yaw-Steuerung @0x8010129C-C8: nach dem Launch steht Clip 20 — im
         * Auslieferungs-Ablauf toter Code (byte-true mitgeführt, feuert nie). */
        if ((int)e->motion == 12) e->rot_y = (int16_t)(((int)e->rot_y - 32) & 0xfff);
        if ((int)e->motion == 13) e->rot_y = (int16_t)(((int)e->rot_y + 32) & 0xfff);
        re2d_move(e, 0);                                   /* 0x800152C8(0) @0x801012D4 */
        re2d_advance(e, 0x200);                            /* 959c(512) @0x801012E8 */
        if (--e->re2z_t158 == 0) {                         /* Windup 8 @0x801012F0-304 */
            e->sub_state_2 = 1; e->sub_state_3 = 0;        /* sh 1,6 @0x8010131C */
            e->speed_h = 280;                              /* sh 280,324 @0x80101320-24 */
            e->re2d_air219 = 1;                            /* sb 1,537 @0x8010132C */
            e->re2d_launch222 = 1;                         /* sb 1,546 @0x80101330 */
            re2d_se(3);                                    /* jal 0x8005bd6c(3) @0x80101344 */
            s_re2d_gflags |= 0x20u;                        /* 0xFBF4|=0x20 @0x8010134C-60 */
        }
        break;
    case 1: {                                              /* P1 Flug @0x801013C4 */
        int r = re2d_flight(e, pl);                        /* jal 0x80101488 @0x801013E0 */
        if (r > 0) {
            /* Boden-Biss-Treffer: Rumble 0x8003947C(5,0)+0x80039514(5,200,0) @0x801013F0-404
             * (Pad-Aktuator, kein Port-Subsystem — dokumentiert). Phase 3 hat der Kontakt-
             * Handler schon geschrieben (+0x6=3 @0x80104F7C). */
        } else if (r < 0) {                                /* Touchdown @0x80101410 */
            if (e->re2d_bite21e == 2) {                    /* +0x21E==2 @0x80101418-20 */
                e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* LATCH @0x8010142C-34 */
            } else {
                re2d_clip(e, 21, 0, 0xF, 0x100, 0);        /* 0xF0015 @0x80101424/38 */
                e->sub_state_2 = 2; e->sub_state_3 = 0;    /* sh 2,6 @0x80101450 */
                re2d_advance(e, 0x100);                    /* @0x80101454 */
                e->re2z_flags21a = (uint16_t)(e->re2z_flags21a + 20);   /* Fatigue+20 @0x8010145C-68 */
                e->re2d_launch222 = 0;                     /* sb zero,546 @0x80101460 */
            }
        }
        break;
    }
    case 2:                                                /* P2 Landung @0x80101534 */
        if (re2d_landing(e)) {                             /* jal 0x80101574 @0x80101540 */
            e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 2,5 @0x80101550-54 */
            e->re2d_pause21d = 0; e->re2d_bite21e = 0;     /* @0x80101558-5C */
        }
        break;
    default:                                               /* P3 Boden-Biss @0x80101684 */
        switch (e->sub_state_3) {
        case 0:
            e->sub_state_3 = 1;                            /* sb 1,7 @0x801016E0-E4 */
            re2d_clip(e, 22, 0, 0xF, 0x100, 0);            /* 0xF0016 @0x801016EC-F0 */
            e->re2z_t158 = 0;                              /* sh zero,344 @0x801016F8 */
            e->re2d_bite21e = 0;                           /* sb zero,542 @0x801016FC */
            e->re2d_budget21f = 4;                         /* sb 4,543 @0x80101700 */
            e->speed_h = (int16_t)(e->speed_h >> 2);       /* sra 18 @0x80101704-0C */
            break;
        case 1:
            re2d_move3d(e, 2048);                          /* 0x80015350(2048,0) @0x80101714-18 */
            e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40); /* @0x80101720-38 */
            if (e->y >= (int32_t)e->dog_floor_y) {         /* @0x8010172C-34 */
                e->sub_state_3 = 2;                        /* sb 2,7 @0x80101758 */
                e->re2d_air219 = 0;                        /* @0x80101760 */
                e->y = (int32_t)e->dog_floor_y;            /* sw a2,60 @0x8010176C */
                re2d_se(10);                               /* jal 0x8005bd6c(10) @0x80101770 */
            }
            if (re2d_frame_slot(e) < 0x11) re2d_advance(e, 0x100);   /* @0x80101778-98 */
            else e->speed_h = 0;                           /* sh zero,324 @0x801017A8 */
            break;
        default:
            if (re2d_advance(e, 0x100)) {                  /* @0x801017B4-BC */
                e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* sb 2,5 @0x801017C4 */
                e->re2d_pause21d = re2d_budget_tbl[re15_re2_rand() & 7u];    /* @0x801017C8-E4 */
            }
            break;
        }
        if ((int)e->re2z_t158 < 2048) {                    /* Dreh-Biss @0x801017E8-F4 */
            e->rot_y = (int16_t)(((int)e->rot_y + 128) & 0xfff);   /* @0x801017FC-0C */
            e->re2z_t158 = (int16_t)(e->re2z_t158 + 128);  /* @0x80101808-10 */
        }
        break;
    }
    re2d_fx(e, 4, 0);                                      /* Tail FX(4,0) @0x80101228-34 */
}

/* sub 4 HEULEN @0x80101830, Phasen @0x801054F8 {0x8010186C, 0x801019B8, 0x80101A04, 0x80101A58} */
static void re2d_howl_wait_p3(re15_actor_t *e)             /* Phase 3 @0x80101A58 (auch sub-12-Exit) */
{
    if (e->sub_state_3 == 0) {
        e->sub_state_3 = 1;                                /* sb 1,7 @0x80101A84-8C */
        re2d_clip(e, 0, 0, 0x1F, 0x80, 1);                 /* 0x1F0000 @0x80101AA0-A4 */
        e->re2z_t158 = (int16_t)re2d_wait_tbl[re15_re2_rand() & 7u];   /* @0x80101A88-A8 */
    }
    if (--e->re2z_t158 == 0) {                             /* @0x80101AAC-C0 */
        re2d_clip(e, 1, 0, 0xF, 0x100, 1);                 /* 0xF0001 @0x80101AC4-CC */
        e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 1,5 @0x80101AD4-D8 */
    }
    re2d_advance(e, 0x80);                                 /* 959c(128) @0x80101AE8 */
}
static void re2d_sub4_howl(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: {                                              /* P0 @0x8010186C */
        /* Gate: 5000 wenn Ziel-Zustand 0x0201 (state1/sub2 = RENNT — lhu +0x4==513
         * @0x801018B0-C0; für den Spieler ist 513 der RUN-Zustand, Selektor @0x8003C6C0), sonst
         * 3500. Lane-D-Etikett "gepackt" nach eigenem Disasm korrigiert. */
        uint32_t gate = (pl->motion == 100) ? 5000u : 3500u;
        if (e->ai_dist <= gate) {                          /* sltu @0x801018C4-D0 */
            re2d_clip(e, 8, 0, 0xF, 0x100, 0);             /* 0xF0008 @0x801018D4-E8 */
            e->sub_state_2 = 2; e->sub_state_3 = 0;        /* sb 2,6 @0x801018E0 */
        } else if ((re2d_cfbf6(pl) & 0x1u) || (s_re2d_gflags & 0x20u)) {  /* @0x801018EC-914 */
            if (!(s_re2d_gflags & 0x80u)) {                /* Heul-Claim frei @0x8010191C-2C */
                re2d_clip(e, 8, 0, 0xF, 0x100, 0);         /* @0x80101930-40 */
                e->sub_state_2 = 2; e->sub_state_3 = 0;    /* sb 2,6 @0x8010193C */
                s_re2d_gflags |= 0x80u;                    /* Claim @0x80101944-58 */
            } else {
                e->sub_state_2 = 1;                        /* sb 1,6 @0x80101980 */
                e->re2z_t158 = (int16_t)re2d_howl_tbl[re15_re2_rand() & 7u];  /* @0x80101964-84 */
            }
        }
        re2d_advance(e, 0x100);                            /* @0x80101994 */
        break;
    }
    case 1:                                                /* P1 Warten @0x801019B8 */
        if (--e->re2z_t158 == 0) {                         /* @0x801019C0-D4 */
            re2d_clip(e, 8, 0, 0xF, 0x100, 0);             /* @0x801019D8-E8 */
            e->sub_state_2 = 2; e->sub_state_3 = 0;        /* sb 2,6 @0x801019E4 */
        }
        re2d_advance(e, 0x100);                            /* @0x801019EC */
        break;
    case 2:                                                /* P2 Heulen spielt @0x80101A04
                                                            * (der Heul-SE kommt als EDD-Frame-
                                                            * Event Bit 0x40000 → SE 8, ACTIVE-Tail) */
        if (re2d_advance(e, 0x100)) {                      /* @0x80101A14-1C */
            e->sub_state_2 = 3; e->sub_state_3 = 0;        /* sh 3,6 @0x80101A28 */
        }
        break;
    default:                                               /* P3 @0x80101A58 */
        re2d_howl_wait_p3(e);
        break;
    }
}

/* sub 5 Fressen/Knurren @0x80101B14: Clips 3→4 Rate 3, SE 4 beim Übergang (Map-stumm),
 * Ende → +0x4=513 (state 1 sub 2 RUN). */
static void re2d_sub5_feed(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_2 = 1;                                /* sb 1,6 @0x80101B3C */
        re2d_clip(e, 3, 0, 3, 0x400, 0);                   /* 0x30003 @0x80101B34-48 */
        e->re2d_budget21f = 32;                            /* sb 32,543 @0x80101B4C-50 */
    }
    if (re2d_advance(e, 0x400)) {                          /* 959c(1024) @0x80101B5C-60 */
        if ((int)e->motion == 4) {                         /* @0x80101B6C-74 */
            re15_ai_set_state_word(e, 0x201);              /* sw 513,4 @0x80101BA8-AC */
        } else {
            re2d_clip(e, (int)e->motion + 1, 0, 3, 0x400, 0);   /* +0x30001 @0x80101B78-84 */
            if ((int)e->motion == 4) re2d_se(4);           /* jal 0x8005bd6c(4) @0x80101B88-9C
                                                            * (ENEMSE-Map-Eintrag 20 = 0xFFFFFFFF
                                                            * = byte-true STUMM) */
        }
    }
    /* jal 0x80104B98 @0x80101BB0 — Kopf-/Kiefer-Tracking (Review-Fund #13): bewusst OPEN,
     * Beleg-Block bei den Helfern oben (0x80017D98-Zwei-Part-Aufruf, Gate +0x1C0&1). */
    re2d_fx(e, 4, 6);                                      /* @0x80101BB8-C4 */
}

/* sub 6 Rückzug rückwärts @0x80101BDC */
static void re2d_sub6_retreat(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_2 = 1;                                /* sb 1,6 @0x80101C34 */
        e->speed_h = 50;                                   /* sh 50,324 @0x80101C38-3C */
        re2d_clip(e, 9, 0, 3, 0x400, 0);                   /* 0x30009 @0x80101C28-40 */
    }
    if (re2d_frame_slot(e) < 0xF) {                        /* @0x80101C4C-58 */
        if (re2d_frame_slot(e) >= 8) re2d_speed_dn(e, 32, 30);   /* 0x80104C30(32,30) @0x80101C5C-6C */
        re2d_move(e, 2048);                                /* 0x800152C8(2048) @0x80101C78-7C */
        re15_enemy_steer_point(e, pl->x, pl->z, 64);       /* 0x80015558(64) @0x80101C8C-90 */
    }
    if (re2d_advance(e, 0x400))                            /* @0x80101CA0-A8 */
        re15_ai_set_state_word(e, 0x201);                  /* sw 513,4 @0x80101CAC-B0 */
}

/* sub 7 BISS-LATCH @0x80101CDC: +0x6==0 → 0x80101D1C (Clip 23 + Mash), sonst 0x80102124
 * (Clip 24 Hang → Clip 25 → Sub 12). */
static const uint8_t re2d_latch_parts[4] = { 8, 0, 9, 12 };   /* @0x80105508 (self-read) */
static void re2d_sub7_latch(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        if (e->sub_state_3 == 0) {                         /* P0 @0x80101D4C-54 */
            e->sub_state_3 = 1;                            /* sb 1,7 @0x80101D6C */
            e->re2d_rel220 = 0;                            /* sb zero,544 @0x80101D74 */
            e->speed_h = 0;                                /* sh zero,324 @0x80101D78 */
            e->re2z_dir16a = 12;                           /* MASH-Zähler 12 @0x80101D70/7C */
            re2d_clip(e, 23, 0, 0x1F, 0x80, 0);            /* 0x1F0017 @0x80101D5C-84 */
            e->re2d_budget21f = 0;                         /* sb zero,543 @0x80101D94 */
            /* Latch-Globals @0x80101D98-E14: 0x800CFDAC=self, 0x800CFBFC=6 (Grab-Art HUND),
             * 0x800CFD80/84=+0x188/18C, PL+0x1D3|=0x80, PL-Soll-Yaw 0x800CFC6E = Hund+2048.
             * Port: der Kontakt-Handler hat Victim-Pin + Yaw schon gesetzt (re2d_contact). */
            re15_re2z_grab_anchor(e, pl, 23);              /* Review-Fix #12: P0 ankert das Opfer
                                                            * BEREITS (jal 0x80015B94 @0x80101DA0,
                                                            * a3=0 Delay-Slot; selbst disasm'd) —
                                                            * sonst posiert der Victim-Tick den
                                                            * Spieler 1-2 Ticks vom Stale-Anker */
            pl->rot_y = (int16_t)(((int)e->rot_y + 2048) & 0xfff);   /* @0x80101E04-14 */
            /* Rumble-Kette @0x80101E18-EB4 (Kanäle 5/8/20/60, Vol −15 bei 0x800CFB74&0x800):
             * Pad-Aktuator (FUN_8003947C/39514/395B8 selbst decompiliert) — kein Port-Rumble. */
        } else {
            /* P1 @0x80101EC4: Anker 0x80015B94 + Pose 0x80015CB8 jeden Tick */
            re15_re2z_grab_anchor(e, pl, 23);              /* 0x80015B94 @0x80101ED0 */
            re15_re2z_grab_rootmotion(e);                  /* 0x80015CB8 @0x80101EE4 */
            if (re2d_advance(e, 0x80)) {                   /* 959c(128) @0x80101EF8-F00 */
                re2d_clip(e, 24, 0, 0x1F, 0x80, 1);        /* 0x1F0018 Hang-Loop @0x80101F04-08 */
                e->sub_state_2 = 1; e->sub_state_3 = 0;    /* sh 1,6 @0x80101F20 */
                re15_re2z_move_root(e);                    /* 0x80015E7C @0x80101F24 */
                e->re2d_budget21f = 0;                     /* @0x80101F44 */
                /* SPIELER-SCHICKSAL am Biss-Ende (PORT-MAPPING, dokumentiert OPEN): der Latch
                 * entsteht NUR auf dem 800401d4-Rückgabe-2-Pfad (HP<−14 nach dem 20er-Biss,
                 * @0x80104EE0-E4) — der Spieler STIRBT, außer das Break-Free-Signal
                 * 0x800CFB74|=0x4000100 (@0x8010204C-60) kam. Dessen EXE-seitiger Spieler-
                 * Handler (Grab-Art 6) ist nicht RE'd → Port: Mash im 12er-Fenster = One-Save
                 * (HP=0, Throw-off-Choreo), sonst Devour-Kollaps (Zombie-Kill-Muster). */
                if (pl->hp < 0) {
                    if (e->re2d_abort21c) {                /* Mash-Escape (Port-Marker, s.u.) */
                        pl->hp = 0;
                        re15_player_victim_throwoff();
                    } else {
                        pl->state = 7;
                        re15_re2z_victim_devour(e, 0);
                    }
                }
                e->re2d_abort21c = 0;
                return;
            }
            /* Frame-Events Clip 23 @0x80101F48-FF8: Frame 98 → Budget 18 + Part 4 Blend
             * 96/0x101010/Flag 0x80; Frame 102 → Part-Liste {8,0,9,12} dieselbe Behandlung.
             * Die Part-Blend-Wörter sind Render-seitig (Model-Pool +0x98/+0x9C) — im Port
             * nicht modelliert (OPEN); das BUDGET ist verhaltensrelevant und byte-true: */
            if (re2d_frame_slot(e) == 98)  e->re2d_budget21f = 18;   /* sb 18,543 @0x80101F64-68 */
            if (re2d_frame_slot(e) == 102) { /* Part-Blend {8,0,9,12} @0x80101FA8-FF8 (Render-OPEN) */ }
            /* Mash-Fenster @0x80102000-C4 (Review-Fix #4, selbst nachdisassembliert):
             *   `lb v0,362; beq v0,zero,0x80102024` — ctr==0 geht OHNE Dekrement in den
             *   Release-Block (der ab Tick 12 JEDEN Tick laeuft: Gravity + Boden-Klemme);
             *   Dekrement NUR auf den Pfaden ctr!=0 (Delay-Slot @0x80102020, Store @0x801020C4)
             *   oder Mash-Bit (@0x801020A4-C4) — der Zaehler SATURIERT bei 0 ohne Mash.
             *   Bit0 (Spieler-Struggle-Signal, EXE-seitig — Port-MAPPING: re15_mash_pressed)
             *   schaltet zusaetzlich den Release-Block frueher. */
            {
                int mash = re15_re2z_mash();               /* PL+0x8-Bit0-MAPPING */
                if (mash) e->re2d_abort21c = 1;            /* Break-free-Marker (0x4000100-MAPPING) */
                int8_t ctr = (int8_t)e->re2z_dir16a;
                if (ctr == 0 || mash) {
                    if (e->re2d_rel220 == 0) {             /* Einmal-Latch @0x80102024-30 */
                        e->re2d_rel220 = 1;                /* sb 1,8(s4) @0x80102048 */
                        /* 0x800CFB74|=0x4000100 (@0x8010204C-60) = Spieler-Break-Free-Signal;
                         * 0xFBF4|=0x40 = Rudel-Signal @0x80102054-68 */
                        s_re2d_gflags |= 0x40u;
                    }
                    /* Gravity @0x80102070-A0 — laeuft ab ctr==0 dauerhaft: der Hund kommt
                     * waehrend des Latch-Bisses auf den Boden (Klemme @0x80102098-A0) */
                    re2d_move3d(e, 0);
                    e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40);
                    if (e->y >= (int32_t)e->dog_floor_y)
                        { e->y = (int32_t)e->dog_floor_y; e->re2d_vy146 = 0; e->re2d_air219 = 0; }
                }
                if (ctr != 0 || mash)                      /* Saturierung bei 0 (s.o.) */
                    e->re2z_dir16a = (uint8_t)(ctr - 1);   /* @0x80102020 / @0x801020B8-C4 */
                re15_re2z_player_pin();                    /* Victim-Pin hält (Port-Seite) */
            }
            re2d_fx(e, 4, 0);                              /* FX(4,0) @0x801020C8-D4 */
            re2d_fx(e, 8, 4);                              /* FX(8,4) Blut AUF Leon @0x801020D8-E4 */
            re2d_fx(e, 8, 4);                              /* zweiter Spawn @0x801020E8-F4 */
            re2d_frame_se_16028(e);                        /* jal 0x80016028 @0x801020F8 */
        }
        return;
    }
    /* Hang/Abschütteln 0x80102124 */
    if (e->sub_state_3 == 0) {                             /* Clip 24 läuft @0x80102144-4C */
        re15_re2z_move_root(e);                            /* 0x80015E7C @0x80102154 */
        int done = re2d_advance(e, 0x80);                  /* 959c(128) @0x80102168 (Delay-Slot-
                                                            * Erkenntnis: s0 = 959c-Ergebnis!) */
        re2d_move(e, 0);                                   /* 0x800152C8(0) @0x80102178 */
        if (done) {
            re2d_clip(e, 25, 0, 0x1F, 0x80, 0);            /* 0x1F0019 @0x80102184-88 */
            e->sub_state_3 = 1;                            /* sb 1,7 @0x801021A0 */
            e->re2z_t158 = 2;                              /* sh 2,344 @0x801021A8 */
            re15_re2z_move_root(e);                        /* @0x801021AC */
        }
    } else {                                               /* Clip 25 @0x801021D0 */
        re15_re2z_move_root(e);                            /* @0x801021DC */
        if (re2d_advance(e, 0x80)) {                       /* @0x801021F0-F8 */
            if (--e->re2z_t158 == 0) {                     /* @0x80102200-14 */
                e->sub_state_1 = 12; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* sb 12,5 @0x8010221C-20 */
                /* der Spieler ist frei (Throw-off), sofern er lebt */
                if (g_actors[RE15_ACTOR_SLOT_PLAYER].hp >= 0)
                    re15_player_victim_throwoff();
            }
        }
    }
    re2d_frame_se_16028(e);                                /* jal 0x80016028 @0x80102224 */
}

/* sub 8 Break-off-Lauf @0x8010224C */
static void re2d_sub8_breakoff(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_2 = 1;                                /* sb 1,6 @0x80102284 */
        e->re2d_route218 = 0;                              /* 0x8004AA50 @0x80102280-8C (MAPPING) */
        e->re2z_t158 = (int16_t)(re2d_wait_tbl[re15_re2_rand() & 7u] + 30);   /* @0x80102288-AC */
        e->re2z_flags21a = (uint16_t)(e->re2z_flags21a + 300);   /* Fatigue+300 @0x801022A0-B0 */
    }
    int s1 = re15_ai_arc_test(e, pl->x, pl->z, (int)e->re2d_turn224);   /* @0x801022D0-E0 (Nav-MAPPING) */
    e->rot_y = (int16_t)(((int)e->rot_y + s1) & 0xfff);    /* @0x801022EC-FC */
    re2d_move(e, 0);                                       /* @0x801022F8 */
    if (--e->re2z_t158 == 0) {                             /* @0x80102300-14 */
        e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 2,5 @0x8010231C-20 */
        e->re2d_pause21d = 0;                              /* sb zero,541 @0x80102324 */
    }
    re2d_lean(e, s1);                                      /* @0x80102330 */
    re2d_advance(e, 0x100);                                /* @0x80102344 */
    re2d_check_floor_lost(e, pl);                          /* @0x80102354 */
}

/* sub 9 Spieler verloren @0x8010237C */
static void re2d_sub9_lost(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_2 = 1;                                /* sb 1,6 @0x8010239C */
        e->re2z_t158 = 60;                                 /* sh 60,344 @0x801023A0-A4 */
        re2d_clip(e, 0, 0, 0xF, 0x100, 1);                 /* 0xF0000 @0x801023A8-AC */
    }
    if (--e->re2z_t158 == 0) {                             /* @0x801023B0-C4 */
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb zero,5 @0x801023D0-D4 */
        re2d_clip(e, 1, 0, 0xF, 0x100, 1);                 /* 0xF0001 @0x801023C8-D8 */
    }
    re2d_advance(e, 0x100);                                /* @0x801023E0 */
}

/* sub 10 aggressives Idle @0x80102404 — wie sub 0, weckt direkt in RUN */
static void re2d_sub10_aggidle(re15_actor_t *e, re15_actor_t *pl)
{
    (void)re2d_idle_wander(e, pl);                         /* jal 0x80100548 @0x80102410 */
    if (pl->floor == e->floor                              /* @0x80102418-28 */
        && (e->ai_dist < 0xfa0u                            /* @0x80102430-3C */
            || (s_re2d_gflags & 0x20u)                     /* @0x80102444-54 */
            || (re2d_cfbf6(pl) & 0x1u))) {                 /* @0x8010245C-6C */
        e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 2,5 @0x80102474-78 */
    }
}

/* sub 11 Rudel-Hilfe @0x80102490 — rennt zum Latch-Ort (Ziel-MAPPING: Spieler; der Partner-
 * Charakter [0x800CFE18]/0x800CFDE0 existiert im Port nicht) */
static void re2d_sub11_packhelp(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                             /* P0 @0x80102500-08 */
        e->sub_state_2 = 1;                                /* sb 1,6 @0x8010250C */
        e->speed_h = 30;                                   /* sh 30,324 @0x80102510-14 */
        e->re2d_air219 = 0; e->re2d_wound22d = 0;          /* @0x8010251C-20 */
        re2d_clip(e, 1, (int)(re15_re2_rand() & 0x3fu), 0xF, 0x100, 1);   /* @0x80102528-44 */
    } else if (e->sub_state_2 == 1) {
        re15_enemy_steer_point(e, pl->x, pl->z, 16);       /* 0x80015558(16) @0x8010256C-70 */
        re2d_move(e, 0);                                   /* @0x80102578 */
        int outside = re2d_sector(e, pl, (int)e->rot_y & 0xfff, 256);   /* @0x8010258C-90 */
        if ((!outside && e->ai_dist < 0x9c4u) || outside) {/* <2500 stop @0x8010259C-A8; außerhalb:
                                                            * +0xD==255-Gate @0x801025B0-B8 (Port:
                                                            * kein +0xD-Produzent → wie 255 = kein
                                                            * Stop) — nur der Nah-Stop feuert */
            if (!outside && e->ai_dist < 0x9c4u) {
                e->sub_state_2 = 2; e->sub_state_3 = 0;    /* sb 2,6 @0x801025C0 */
                re2d_clip(e, 0, 0, 0xF, 0x100, 1);         /* 0xF0000 @0x801025C4-C8 */
            }
        }
    } /* P2: stehen (nur advance) */
    re2d_advance(e, 0x100);                                /* @0x801025D8 */
}

/* sub 12 Post-Latch @0x80102608: +0x6==0 → Schüttel-Handler 0x80102648, sonst Heul-P3 0x80101A58 */
static void re2d_sub12_postlatch(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 != 0) { re2d_howl_wait_p3(e); return; }   /* jal 0x80101A58 @0x80102630 */
    switch (e->sub_state_3) {
    case 0:
        e->sub_state_3 = 1;                                /* sb 1,7 @0x801026AC */
        e->re2d_air219 = 0; e->re2d_wound22d = 0;          /* @0x801026B0-B8 */
        re2d_clip(e, 14, (int)(re15_re2_rand() & 0x3fu), 0x1F, 0x80, 0);
                                                           /* 0x1F000E + Zufalls-Frame @0x801026B4-E4 */
        re15_re2z_move_root(e);                            /* 0x80015E7C @0x801026E0 */
        re2d_advance(e, 0x80);                             /* @0x801026F4 */
        break;
    case 1:
        if (pl->hp >= 0) {                                 /* 0x800CFD4E @0x80102704-10 */
            uint32_t gate = (pl->motion == 100) ? 5000u : 3500u;   /* 0x800CFBFC==513 @0x80102718-2C
                                                            * (Port-MAPPING wie Heulen: Gate am
                                                            * Renn-Zustand) */
            if (e->ai_dist <= gate) {                      /* @0x80102730-3C */
                e->sub_state_2 = 1; e->sub_state_3 = 0;    /* sh 1,6 @0x801027D8 */
            } else if (re2d_cfbf6(pl) & 0x1u) {            /* toter Zweig @0x80102744-54 */
                if (!(s_re2d_gflags & 0x80u)) {
                    e->sub_state_2 = 1; e->sub_state_3 = 0;/* sh 1,6 @0x80102778 */
                    s_re2d_gflags |= 0x80u;                /* @0x8010277C-90 */
                } else {
                    e->sub_state_3 = 2;                    /* sb v0,7 @0x801027A0 */
                    e->re2z_t158 = (int16_t)re2d_howl_tbl[re15_re2_rand() & 7u];  /* @0x8010279C-B8 */
                }
            }
        }
        break;
    default:                                               /* P2 @0x801027BC */
        if (--e->re2z_t158 == 0) { e->sub_state_2 = 1; e->sub_state_3 = 0; }   /* @0x801027C4-D8 */
        break;
    }
    /* Tail @0x801027DC-884: Frame-Blut-Bits + Frame-SE + Root-Motion */
    {
        uint32_t fw = re2d_frame_word(e);                  /* lw +0x178 @0x801027DC-E4 */
        if (fw & 0x30000u) {                               /* @0x801027E8-F0 */
            e->re2d_budget21f = 1;                         /* sb 1,543 @0x801027FC */
            re2d_fx(e, 4, 0);                              /* FX(4,0) @0x80102800-08 */
            if ((fw & 0x20000u) && (re15_re2_rand() & 3u) == 0u) {   /* @0x8010280C-28 */
                e->re2d_budget21f = 1;                     /* @0x80102834 */
                re2d_fx(e, 4, 1 + (int)(re15_re2_rand() & 1u));      /* FX 1/2 @0x80102838-48 */
            }
        }
    }
    re2d_frame_se_16028(e);                                /* jal 0x80016028 @0x8010284C */
    re15_re2z_move_root(e);                                /* 0x80015E7C @0x80102860 */
    re2d_advance(e, 0x80);                                 /* @0x80102874 */
    re2d_move(e, 0);                                       /* 0x800152C8(0) @0x80102880 */
}

/* sub 13 Zickzack-Flucht @0x801028AC (verwundeter Spawn 7): +0x6==0 Kauern (Clips {11,26}
 * @0x8010550C), +0x6!=0 Flucht (Speed 240, Yaw ±127) */
static const uint8_t re2d_cower_tbl[8] = { 11, 26, 11, 26, 26, 26, 11, 26 };
                                                               /* @0x8010550C — alle 8 Bytes selbst
                                                                * gelesen (Review-Fix #2: [4]/[5]=26,
                                                                * KEINE Periode 4; P(26)=5/8); beide
                                                                * Leser maskieren `andi 0x7`
                                                                * (@0x80102968 / @0x80102A0C) */
static void re2d_sub13_flee(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                             /* Kauern 0x80102900 */
        if (e->sub_state_3 == 0) {
            e->sub_state_3 = 1;                            /* sb 1,7 @0x80102948 */
            re2d_clip(e, re2d_cower_tbl[re15_re2_rand() & 7u], 0, 0xF, 0x100, 0);
                                                           /* 0xF0000+tbl[rand&7] @0x80102968-84 */
            re15_re2z_move_root(e);                        /* @0x80102980 */
            re2d_advance(e, 0x100);                        /* @0x80102994 */
            e->re2z_dir16a = (uint8_t)((re15_re2_rand() & 3u) + 1);   /* Wiederholungen @0x8010299C-AC */
            return;
        }
        re15_re2z_move_root(e);                            /* @0x801029BC */
        if (re2d_advance(e, 0x100)) {                      /* @0x801029D0-D8 */
            if (--e->re2z_dir16a == 0) {                   /* @0x801029E0-F4 */
                e->sub_state_2 = 1; e->sub_state_3 = 0;    /* sh 1,6 @0x801029FC-A00 */
            } else {
                re2d_clip(e, re2d_cower_tbl[re15_re2_rand() & 7u], 0, 0xF, 0x100, 0);  /* andi 0x7
                                                            * @0x80102A0C, lbu @0x80102A18-24 */
            }
        }
        if ((int)e->motion == 11) re2d_move(e, 0);         /* @0x80102A28-3C */
        re2d_frame_se_16028(e);                            /* @0x80102A40 */
        return;
    }
    /* Flucht 0x80102A64 */
    switch (e->sub_state_3) {
    case 0:
        e->sub_state_3 = 1;                                /* sb 1,7 @0x80102AC8 */
        e->speed_h = 240;                                  /* sh 240,324 @0x80102ACC-D4 */
        e->re2z_t158 = (int16_t)re2d_wait_tbl[re15_re2_rand() & 7u];   /* @0x80102AD8-EC */
        e->re2z_t15a = (int16_t)((re15_re2_rand() & 1u) ? 127 : -127); /* @0x80102AF0-04 */
        if (re15_re2_rand() & 1u) e->re2z_t15a = (int16_t)-e->re2z_t15a;   /* @0x80102B00-20 */
        re2d_clip(e, 2, 0, 0xF, 0x100, 1);                 /* 0xF0002 @0x80102B10-2C */
        /* FALLTHRU: P1-Yaw läuft ab @0x80102B30 sofort */
        /* fall through */
    case 1:
        e->rot_y = (int16_t)(((int)e->rot_y + e->re2z_t15a) & 0xfff);   /* @0x80102B30-50 */
        if (--e->re2z_t158 == 0) e->sub_state_3 = 2;       /* @0x80102B38-5C */
        break;
    case 2: {
        e->rot_y = (int16_t)(((int)e->rot_y + e->re2z_t15a) & 0xfff);   /* @0x80102B60-74 */
        int band = ((int)e->rot_y - 1920) & 0xfff;         /* @0x80102B78-7C */
        if (band <= 0x100) {                               /* sltiu 0x101 @0x80102B7C-80 */
            if ((7016 >> (int)(re15_re2_rand() & 0xfu)) & 1) {   /* srav-Bitmuster @0x80102B88-A0 */
                e->sub_state_1 = 15;                       /* sb 15,5 @0x80102BA8 */
                e->sub_state_2 = 0; e->sub_state_3 = 1;    /* sh 256,6 @0x80102BAC-B4 */
            } else {
                e->sub_state_3 = 3;                        /* sb 3,7 @0x80102BB8-BC */
                e->re2z_t15a = 0;                          /* sh zero,346 @0x80102BC4 */
            }
        }
        break;
    }
    default:                                               /* P3 @0x80102BC8 */
        if (e->ai_contact & 1) { e->sub_state_2 = 0; e->sub_state_3 = 0; }   /* +0x110&1 @0x80102BC8-DC */
        break;
    }
    re2d_move(e, 0);                                       /* @0x80102BE4 */
    re2d_lean(e, (int)e->re2z_t15a);                       /* 0x80104C74(+0x15A) @0x80102BEC-F4 */
    re2d_advance(e, 0x100);                                /* @0x80102C04 */
}

/* Fenster-Sprung-Launch-Maschine 0x80102C78 (subs 14/15 Phase 0; +0x7-Phasen) */
static void re2d_windowjump_launch(re15_actor_t *e, int vy)
{
    switch (e->sub_state_3) {
    case 0:                                                /* @0x80102CD4 */
        e->sub_state_3 = 1;
        e->re2z_self1d3 |= 0x80u;                          /* @0x80102CD4-EC */
        e->re2d_air219 = 0; e->re2d_wound22d = 0;          /* @0x80102CDC-E0 */
        break;
    case 1:
        e->sub_state_3 = 2;                                /* sb 2,7 @0x80102D04-0C */
        re2d_launch(e, 65, vy);                            /* 0x80101380(65,−320) @0x80102CF8-08 */
        break;
    default:
        re2d_move(e, 0);                                   /* @0x80102D24 */
        re2d_advance(e, 0x200);                            /* @0x80102D38 */
        if (--e->re2z_t158 == 0) {                         /* @0x80102D40-54 */
            e->sub_state_2 = 1; e->sub_state_3 = 0;        /* sh 1,6 @0x80102D64 */
            e->speed_h = 280;                              /* sh 280,324 @0x80102D68-6C */
            e->re2d_air219 = 1;                            /* sb 1,537 @0x80102D78 */
            re2d_se(3);                                    /* jal 0x8005bd6c(3) @0x80102D74 */
        }
        break;
    }
}

/* sub 14 Fenster-Sprung A @0x80102C28 (Spawn 6), Phasen @0x80105514 */
static void re2d_sub14_windowA(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: re2d_windowjump_launch(e, -320); break;        /* 0x80102C78 (−320 @0x80102D00) */
    case 1: {                                              /* Flug 0x80102D98 */
        e->re2d_nolatch22c = 1;                            /* sb 1,556 @0x80102DB4-C0 */
        if (re2d_flight(e, pl) < 0) {                      /* jal 0x80101488 @0x80102DBC */
            re2d_clip(e, 21, 0, 0xF, 0x100, 0);            /* 0xF0015 @0x80102DC8-CC */
            e->sub_state_2 = 2; e->sub_state_3 = 0;        /* sh 2,6 @0x80102DE4 */
            re2d_advance(e, 0x100);                        /* @0x80102DE8 */
        }
        break;
    }
    default:                                               /* Landung 0x80102E0C */
        if (re2d_landing(e)) {                             /* jal 0x80101574 @0x80102E18 */
            e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb 2,5 @0x80102E2C-30 */
            e->re2d_pause21d = 0; e->re2d_nolatch22c = 0;  /* @0x80102E34-38 */
            e->re2z_self1d3 &= 0x7fu;                      /* @0x80102E28-40 */
        }
        break;
    }
}

/* sub 15 Sprung-Variante B @0x80102E58 (Spawn 8 / aus Sub 13), Phasen @0x80105520 */
static void re2d_sub15_windowB(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: re2d_windowjump_launch(e, -320); break;        /* Tabelle [0]=0x80102C78 */
    case 1: {                                              /* Flug 0x80102EA8 */
        e->re2d_nolatch22c = 1;                            /* sb 1,556 @0x80102EC4-D0 */
        if (re2d_flight(e, pl) < 0) {                      /* @0x80102ECC */
            re2d_clip(e, 21, 0, 0xF, 0x100, 0);            /* 0xF0015 @0x80102ED8-DC */
            e->sub_state_2 = 2; e->sub_state_3 = 0;        /* sh 2,6 @0x80102EF4 */
        }
        if (e->ai_contact & 1) {                           /* Wand im Flug @0x80102F00-0C */
            re2d_clip(e, 22, 0, 0xF, 0x100, 0);            /* 0xF0016 @0x80102F10-14 */
            e->sub_state_2 = 3; e->sub_state_3 = 0;        /* sh 3,6 @0x80102F28-2C */
            e->re2z_t158 = 0;                              /* sh zero,344 @0x80102F40 */
            e->speed_h = (int16_t)(-(int)e->speed_h / 2);  /* Abpraller @0x80102F38-54 */
        }
        break;
    }
    case 2:                                                /* Landung 0x80102F74 */
        if (re2d_landing(e)) {                             /* @0x80102F80 */
            e->sub_state_1 = 13; e->sub_state_2 = 1; e->sub_state_3 = 0;  /* sb 13,5/sh 1,6 @0x80102F8C-9C */
            e->re2z_self1d3 &= 0x7fu;                      /* @0x80102F94-A4 */
        }
        break;
    default: {                                             /* Wand-Abpraller 0x80102FBC */
        if (e->sub_state_3 == 0) {
            re2d_move3d(e, 2048);                          /* 0x80015350(2048,0) @0x80102FE8-F0 */
            e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40); /* @0x80102FF4-0C */
            if (e->y >= (int32_t)e->dog_floor_y) {         /* @0x80103000-08 */
                e->sub_state_3 = 1; e->re2d_air219 = 0;    /* @0x8010302C-34 */
                e->y = (int32_t)e->dog_floor_y;            /* @0x80103040 */
                re2d_se(10);                               /* jal(10) @0x80103044 */
            }
            if (re2d_frame_slot(e) < 0x11) re2d_advance(e, 0x100);   /* @0x8010304C-6C */
            else e->speed_h = 0;                           /* @0x8010307C */
        } else if (re2d_advance(e, 0x100)) {               /* @0x80103080-94 */
            e->sub_state_1 = 13; e->sub_state_2 = 1; e->sub_state_3 = 0;   /* @0x8010309C-A8 */
            e->re2z_self1d3 &= 0x7fu;                      /* @0x80103A0-B0 */
        }
        if ((int)e->re2z_t158 < 1024) {                    /* Spin @0x801030B4-C0 */
            e->rot_y = (int16_t)(((int)e->rot_y + 256) & 0xfff);   /* @0x801030C8-D4 */
            e->re2z_t158 = (int16_t)(e->re2z_t158 + 256);
        }
        break;
    }
    }
}

/* sub 16 Sprung-Variante C @0x801030FC — EINTRITT OFFEN (Skript-Kette, kein Produzent bekannt;
 * Phasen @0x80105530). Port: nicht erreichbar; falls doch → recover in RUN (dokumentiert). */
static void re2d_sub16_open(re15_actor_t *e)
{
    e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
}

/* ================================ HURT (state 2) ========================================== */

/* Generic P0 0x80103344 (Zeile für Zeile self-disasm'd). `death` unterdrückt den SE 1
 * (`bne +0x4,2 → skip` @0x801033BC-CC — DEATH re-nutzt P0). */
static const int16_t re2d_jitter_tbl[8] = { 128, -192, -128, 192, -256, 256, 256, -256 };  /* @0x8010559C */
static void re2d_knockdown_entry(re15_actor_t *e);         /* 0x80103950 (unten) */
static void re2d_hurt_p0(re15_actor_t *e, re15_actor_t *pl)
{
    /* Knock-Richtung: 0x800CFC6E = SPIELER-Yaw (PL+0x76) − self-Yaw + Jitter @0x80103344-A8 */
    e->re2z_t158 = (int16_t)(((((int)pl->rot_y - (int)e->rot_y) & 0xfff)
                              + re2d_jitter_tbl[re15_re2_rand() & 7u]) & 0xffff);
    e->re2z_f10e &= (uint16_t)~0x2000u;                    /* @0x801033AC-B8 */
    if (e->state == 2) re2d_se(1);                         /* SE 1 Jaulen @0x801033D4-DC */
    if (e->re2d_dbl223 != 0 && e->re2d_wound22d == 0) {    /* Doppel-Treffer @0x801033E0-F8 */
        e->re2d_dbl223 |= 0x80u;                           /* ori 0x80 @0x801033FC-400 */
        re2d_knockdown_entry(e);                           /* Review-Fix #6: `jal 0x80103950`
                                                            * @0x8010340C laeuft im SELBEN Tick,
                                                            * via `j 0x801034A8` @0x80103414 am
                                                            * Router (und dessen 3×Tail-FX) VORBEI */
        return;
    }
    if (e->re2d_air219) {                                  /* Luft-Treffer @0x8010341C-28 */
        e->sub_state_2 = 1;                                /* sh 1,6 @0x80103430 */
        e->speed_h = (int16_t)(e->speed_h / 2);            /* sra 17 @0x8010342C-40 */
    } else {
        e->sub_state_2 = 2;                                /* sh 2,6 @0x80103450 */
        e->speed_h = 240;                                  /* sh 240,324 @0x80103454-5C */
        re2d_hitbox(e, 0);                                 /* 0x80104088(0) @0x80103458 */
    }
    re2d_clip(e, 17, 0, 7, 0x200, 0);                      /* 0x70011 @0x80103460-7C */
    e->re2d_rel220 = 0;                                    /* sb zero,8(s2) @0x80103478 */
    e->re2z_t15a = 512;                                    /* sh 512,346 @0x80103480-90 */
    e->re2d_dbl223 = 1;                                    /* sb 1,547 @0x80103488 */
    e->re2d_budget21f = 1;                                 /* sb 1,7(s2) @0x801034A4 */
    re2d_fx(e, 0, 0);                                      /* FX(0,0) @0x801034A0 */
    re2d_advance(e, 0x200);                                /* @0x8010348C */
}

/* P1 Luft-Treffer 0x801034C8: 3D-Fall entlang +0x158, Landung → SE 11 (meist) / SE 10 (2/16
 * gesund) + Phase 2/3 */
static void re2d_hurt_p1(re15_actor_t *e)
{
    re2d_move3d(e, (int)e->re2z_t158);                     /* 0x80015350(+0x158) @0x801034E8-F0 */
    e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40);         /* @0x801034F4-50C */
    if (e->y >= (int32_t)e->dog_floor_y) {                 /* @0x80103504-08 */
        e->y = (int32_t)e->dog_floor_y;                    /* @0x80103524 */
        e->re2d_air219 = 0; e->re2d_rel220 = 1;            /* @0x80103528-30 */
        e->re2z_t15a = 512;                                /* @0x80103518-1C */
        re2d_hitbox(e, 0);                                 /* @0x8010352C */
        int soft = ((1028 >> (int)(re15_re2_rand() & 0xfu)) & 1)   /* Bitmuster @0x80103534-4C */
                   && !(e->re2d_dbl223 & 0x80u)            /* @0x80103554-60 */
                   && e->hp >= 0                           /* @0x80103568-70 */
                   && e->re2d_wound22d == 0;               /* @0x80103578-80 */
        if (soft) { re2d_se(10); e->sub_state_2 = 3; }     /* @0x80103588-98 */
        else      { re2d_se(11); e->sub_state_2 = 2; }     /* @0x8010359C-A8 */
        e->sub_state_3 = 0;
    }
    if (re2d_frame_slot(e) != 3) re2d_advance(e, 0x200);   /* @0x801035AC-C8 */
}

/* P2 Knockback-Rutschen 0x801035E8 → Aufstehen → 513 RUN / 0x501 Fressen-Knurren / 0x601
 * Rückzug / 0x10D01 Flucht (verwundet) */
static void re2d_hurt_p2(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2d_rel220 == 0) {                             /* @0x80103624-2C */
        re2d_move(e, (int)e->re2z_t158);                   /* 0x800152C8(+0x158) @0x80103634-38 */
        re2d_speed_dn(e, 10, 0);                           /* 0x80104C30(10,0) @0x80103648 */
    }
    if (!re2d_advance(e, (int)e->re2z_t15a >= 0x200 ? 0x200 : 0x100))   /* a3=+0x15A @0x80103654-60 */
        return;
    switch (e->sub_state_3) {
    case 0:
        e->sub_state_3 = 1;                                /* sb 1,7 @0x801036AC */
        e->re2d_dbl223 = (uint8_t)((e->re2d_dbl223 & 0xC0u) | 0x2u);   /* @0x801036A8-C4 */
        re2d_clip(e, 18, 0, (int)e->anim_frac, 0x100, 0);  /* Wort rate<<16|18 @0x801036B0-CC */
        break;
    case 1:
        re2d_clip(e, 7, 0, 0xF, 0x100, 0);                 /* 0xF0007 @0x801036D0-DC */
        e->sub_state_3 = 2;                                /* sb 2,7 @0x801036E4 */
        e->re2z_t15a = 256;                                /* sh 256,346 @0x801036E0-EC */
        break;
    default:
        re2d_hitbox(e, 1);                                 /* 0x80104088(1) @0x801036F0-F4 */
        e->re2z_f10e &= (uint16_t)~0x2000u;                /* @0x801036F8-704 */
        e->re2z_self1d3 &= 0x7fu;                          /* @0x80103708-18 */
        e->re2d_dbl223 = 0;                                /* sb zero,547 @0x8010370C */
        if (e->re2d_wound22d) {                            /* @0x80103714 */
            re15_ai_set_state_word(e, 0x10D01);            /* Flucht sub13/+0x6=1 @0x8010371C-24 */
        } else if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0) {   /* @0x80103728-38 */
            re15_ai_set_state_word(e, 0x501);              /* frontal → Knurren @0x80103744-48 */
        } else {
            re15_ai_set_state_word(e, 0x601);              /* Rückzug @0x8010373C-40 */
        }
        break;
    }
}

/* P3 seltene weiche Landung 0x8010376C: Clip 22 ab Frame 15 Rate 3 → 513 RUN */
static void re2d_hurt_p3(re15_actor_t *e)
{
    if (e->sub_state_3 == 0) {
        e->sub_state_3 = 1;                                /* sb 1,7 @0x8010379C */
        re2d_clip(e, 22, 15, 3, 0x400, 0);                 /* 0x30F16 @0x8010378C-A0 */
        e->re2z_self1d3 &= 0x7fu;                          /* @0x801037A4-A8 */
    }
    if (re2d_advance(e, 0x400)) {                          /* 959c(1024) @0x801037B0-B8 */
        re2d_hitbox(e, 1);                                 /* @0x801037C0 */
        e->re2d_dbl223 = 0;                                /* @0x801037CC */
        re15_ai_set_state_word(e, 0x201);                  /* sw 513,4 @0x801037C8-D0 */
    }
}

/* P4 HP-Re-Roll 0x801037E8 (DEATH-Phasen-Tabelle [3] — „stirbt nicht wirklich"; im HURT-Fluss
 * unerreicht): HP = tbl[rand&0xf]+rand&3 (@0x80105340/@0x80105360 je 0x800CFB74&0x20 — Port:
 * Default-Tabelle, das RE2-Global hat keinen Produzenten), +0x151..153=13, +0x1D3|=0x80. */
static const uint16_t re2d_hp_tbl_def[16] =                 /* @0x80105340 (self-read) */
    { 119, 85, 85, 85, 119, 70, 85, 85, 70, 85, 59, 70, 59, 85, 59, 70 };
static void re2d_hurt_p4_reroll(re15_actor_t *e)
{
    uint32_t r1 = re15_re2_rand() & 3u;                    /* @0x8010380C-28 */
    e->hp = (int16_t)(re2d_hp_tbl_def[re15_re2_rand() & 0xfu] + r1);   /* @0x80103850-7C */
    e->re2z_self1d3 |= 0x80u;                              /* @0x80103894-A4 */
}

/* Doppel-Treffer-KNOCKDOWN: Entry 0x80103950 + Phasen @0x801055AC {0x80103950, 0x80103AA4,
 * 0x80103B40, 0x80103C28}; Router-Tick-Tail 3×FX (0/0/5) @0x801038F4-938.
 * Review-Fix #6 (selbst nachdisassembliert @0x801033F8-414): der HURT-P0-Doppel-Treffer ruft
 * den Entry DIREKT — `ori v0,v1,0x80; sb v0,547(s1); jal 0x80103950 @0x8010340C; j 0x801034A8`
 * — also im SELBEN Tick und am Router (und dessen Tail-FX) VORBEI. Der Entry ist deshalb
 * eine eigene Funktion; der Router erreicht Phase 0 nie (Entry setzt +0x6 sofort auf 1/2/3). */
static void re2d_knockdown_entry(re15_actor_t *e)          /* 0x80103950 */
{
    if (e->re2z_prev_sub == 15) {                          /* Sprung-Herkunft, `lbu 5(s1)` — der
                                                            * RE2-Damage-Writer laesst +0x5 stehen,
                                                            * Port: prev_sub-Schnappschuss
                                                            * (@0x80103968-80; +0x5==15/18) */
        e->re2d_budget21f = (uint8_t)(e->re2d_budget21f + 3);   /* @0x80103984-94 */
        if ((re15_re2_rand() & 3u) == 0u) {                /* @0x80103990-9C */
            e->re2d_budget21f++;                           /* @0x801039A4-B4 */
            re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 2 + (int)(re15_re2_rand() & 1u));
                                                           /* FX 2/3 @0x801039B8-D0 */
        }
    } else {
        e->re2d_budget21f = 1;                             /* sb 1,543 @0x801039DC */
        re2d_fx(e, 0, 0);                                  /* @0x801039E0-EC */
        re2d_se(1);                                        /* SE 1 @0x801039F0-F8 */
    }
    if ((e->re2d_dbl223 & 0x7fu) == 1) {                   /* erster Doppel-Treffer @0x801039FC-08 */
        e->sub_state_2 = e->re2d_air219 ? 1 : 2;           /* @0x80103A10-30 */
        e->sub_state_3 = 0;
    } else {
        e->sub_state_2 = 3; e->sub_state_3 = 1;            /* sh 259,6 @0x80103A34 */
    }
    if (!(e->re2d_dbl223 & 0x40u)) {                       /* Clip-Latch @0x80103A38-44 */
        re2d_clip(e, 18, 0, (int)e->anim_frac, 0x100, 0);  /* rate<<16|18 @0x80103A50-64 */
        re2d_se(1);                                        /* @0x80103A60 */
        e->re2d_dbl223 |= 0x40u;                           /* @0x80103A68-74 */
    }
    e->re2z_dir16a = 0; e->re2d_rel220 = 1;                /* @0x80103A7C-80 */
    e->re2z_t15a = 0;                                      /* sh zero,346 @0x80103A84 */
}

static void re2d_knockdown(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                /* defensiv — im Original unerreichbar
                                                            * (Entry setzt +0x6 sofort weiter) */
        re2d_knockdown_entry(e);
        break;
    case 1:                                                /* Luft 0x80103AA4 */
        if (e->sub_state_3 == 0) {
            e->sub_state_3 = 1;                            /* @0x80103AD0-D8 */
            re2d_move(e, (int)e->re2z_t158 + 2 * (int)(re15_re2_rand() & 0xffu) - 256);  /* @0x80103ADC-F4 */
            e->rot_y = (int16_t)(((int)e->rot_y - 128 + (int)(re15_re2_rand() & 0xffu)) & 0xfff);
                                                           /* @0x80103AF8-B10 */
        }
        re2d_hurt_p1(e);                                   /* jal 0x801034C8 @0x80103B1C */
        break;
    case 2:                                                /* Boden-Rutschen 0x80103B40 */
        if (e->re2d_rel220 == 0) {                         /* @0x80103B64-6C */
            re2d_move(e, (int)e->re2z_t158);               /* @0x80103B74-78 */
            re2d_speed_dn(e, 10, 0);                       /* @0x80103B88 */
        }
        if (e->sub_state_3 == 0) {
            e->sub_state_3 = 1;                            /* @0x80103BA0-A8 */
            e->rot_y = (int16_t)(((int)e->rot_y - 128 + (int)(re15_re2_rand() & 0xffu)) & 0xfff);
                                                           /* @0x80103BAC-BC */
        }
        if (re2d_advance(e, (int)e->re2z_t15a >= 0x200 ? 0x200 : 0x100)) {   /* @0x80103BCC-D4 */
            e->sub_state_2 = 3; e->sub_state_3 = 0;        /* sh 3,6 @0x80103BDC */
            e->re2d_dbl223 = (uint8_t)((e->re2d_dbl223 & 0xC0u) | 0x2u);   /* @0x80103BE0-F0 */
            e->re2z_dir16a = 0;                            /* @0x80103BF8 */
            re2d_clip(e, 18, 0, (int)e->anim_frac, 0x100, 0);   /* @0x80103BF4-C04 */
        }
        break;
    default:                                               /* Erholung 0x80103C28 → generic P2 */
        if ((int8_t)e->re2z_dir16a == 0) {                 /* Einmal @0x80103C44-4C */
            e->re2z_dir16a = 1;                            /* @0x80103C5C */
            if (e->re2z_prev_sub != 12) {                  /* `lbu 5(s0)` @0x80103C54-64 — +0x5 =
                                                            * Herkunfts-Substate (prev_sub, s.o.) */
                e->speed_h = 40;                           /* sh 40,324 @0x80103C68-74 */
                re2d_move(e, (int)e->re2z_t158);           /* @0x80103C6C-70 */
                e->rot_y = (int16_t)(((int)e->rot_y - 128 + (int)(re15_re2_rand() & 0xffu)) & 0xfff);
                                                           /* @0x80103C78-90 */
            }
        }
        re2d_hurt_p2(e, pl);                               /* jal 0x801035E8 @0x80103C98-A0 */
        if (e->sub_state_3 >= 2)                           /* @0x80103CA4-B0 */
            e->re2d_dbl223 &= (uint8_t)~0x40u;             /* @0x80103CB8-C4 */
        break;
    }
    /* Tick-Tail 0x801038F4-938: 2×FX(rand&0xf, 0) + FX(rand&0xf, 5) */
    re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 0);
    re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 0);
    re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 5);
}

/* HURT-Router 0x801032A8 + Zeilen-Tabelle @0x80105538. Sonder-Zeilen (self-disasm'd):
 * [0] 0x8010321C Schlaf-Treffer (Fallen+Landen, dann Zeile 2), [4]/[15] 0x80103FF4 (Heulen/
 * Sprung: Luft-Kick @0x80104030 — word0&0x18000000 && !+0x219 → vy=−140/Speed 240),
 * [9] 0x80103CE4 (Gore-FX budget 2−hits/3, FX 8), [10]/[16] 0x80103D9C (budget 4−hits/3 + Claim),
 * [11] 0x80103E60 (FX 9 + Part-Feld — Render-OPEN), [14] 0x80103F00 (Glas-FX 11 — OPEN). */
static void re2d_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2d_dbl223 & 0x80u) { re2d_knockdown(e, pl); return; }   /* Router @0x801032B0-BC */
    if (e->sub_state_2 == 0) {                             /* frischer Treffer → Zeilen-Entry */
        uint8_t row = e->re2z_prev_sub;                    /* +0x5 = Herkunfts-Substate (Port-
                                                            * Schnappschuss; take_damage über-
                                                            * schreibt +0x5 — Welle-B-Muster) */
        switch (row) {
        case 9:                                            /* [9] 0x80103CE4 */
        {
            /* +0x1D2 = zone + 3*bracket, PRO TREFFER GESETZT (fix_1d2_spec, Welle-D-Nachtrag):
             * Applier FUN_800470C0 {1 @0x80047294-98 / 0 @0x800472D4 / 2 @0x8004730C, +3*s6
             * @0x80047310-30, s6 = Hitcode>>28 @0x80047114} + Hitscan-Applier @0x80041A8C-9C.
             * PORT-PRODUZENT: re15_re2_stamp_1d2 (re15_damage.c) — ZONE aus der Aim-Elevation
             * (deklarierte Naeherung), BRACKET = 0 (OPEN: Hitcode-Produzenten waffen-/pfad-
             * spezifisch, RE1.5-Mapping nicht RE'd). Das /3 hier extrahiert das BRACKET →
             * mit Bracket 0 laufen die Budgets auf Maximum (= RE2-Nah-Treffer-Verhalten). */
            int n = 2 - (int)(e->re2z_hits1d2 / 3u);       /* @0x80103D00-28 */
            e->re2d_budget21f = (uint8_t)((n > 0) ? n : 0);
            for (int i = 0; i < n; i++)
                re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 8);   /* FX 8 @0x80103D38-5C */
            break;
        }
        case 10: case 16:                                  /* [10]/[16] 0x80103D9C */
        {
            int n = 4 - (int)(e->re2z_hits1d2 / 3u);       /* @0x80103DB8-E0 (/3 = BRACKET;
                                                            * Produzent re15_re2_stamp_1d2,
                                                            * s. Zeile-9-Block oben) */
            e->re2d_budget21f = (uint8_t)((n > 0) ? n : 0);
            for (int i = 0; i < n; i++)
                re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 8);
            e->re2z_self1d3 |= 0x80u;                      /* @0x80103E28-3C */
            break;
        }
        case 11:                                           /* [11] 0x80103E60 */
            e->re2d_budget21f = 1;                         /* @0x80103EC8 */
            re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 9);  /* FX 9 (0x84 — OPEN, stumm) @0x80103E80-C4 */
            break;
        case 14:                                           /* [14] 0x80103F00: Glas-Schauer */
            e->re2d_budget21f = 8;                         /* sb 8,543 @0x80103F34-3C */
            re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 11); /* FX 11 (OPEN) @0x80103F40-4C */
            break;
        case 4: case 15:                                   /* [4]/[15] 0x80103FF4 → 0x80104030 */
            if (e->re2d_air219 == 0) {                     /* @0x80104038-54 (word0&0x18000000-
                                                            * MAPPING: Sprung-Zustand = +0x219) */
                e->re2d_air219 = 1;                        /* sb 1,537 @0x8010405C */
                e->re2d_vy146 = -140;                      /* sh −140,326 @0x80104060-64 */
                e->speed_h = 240;                          /* sh 240,324 @0x80104068-6C */
            }
            break;
        case 0:                                            /* [0] 0x8010321C: Schlaf-Treffer */
            if (!re2d_landing(e)) return;                  /* jal 0x80101574 @0x80103258 */
            e->re2d_pause21d = 0; e->re2d_nolatch22c = 0;  /* @0x80103274-78 */
            e->re2z_self1d3 &= 0x7fu;                      /* @0x80103268-80 */
            e->floor = pl->floor;                          /* +0x106 = 0x800CFCFE @0x80103284-90 */
            break;
        default: break;                                    /* [1..3]/[5..8]/[12]/[13] → generisch */
        }
        re2d_hurt_p0(e, pl);                               /* generisch 0x80103308→Phase 0 */
        if (e->re2d_dbl223 & 0x80u) return;                /* Doppel-Treffer → Router nächster Tick */
        return;
    }
    switch (e->sub_state_2) {                              /* Phasen @0x80105588 */
    case 1: re2d_hurt_p1(e); break;                        /* 0x801034C8 */
    case 2: re2d_hurt_p2(e, pl); break;                    /* 0x801035E8 */
    case 3: re2d_hurt_p3(e); break;                        /* 0x8010376C */
    default: re2d_hurt_p4_reroll(e);                       /* 0x801037E8 */
             re15_ai_set_state_word(e, 0x201); break;      /* (unerreicht im HURT — Absicherung) */
    }
}

/* ================================ DEATH (state 3) ========================================== */

/* Kern 0x80104178: EXE 0x80018FB0 (Kill-Buchhaltung — kein Port-Pendant), P0-Reuse (Knock-
 * Richtung + Clip 17), SE 7 einmalig über +0x231 (@0x801041B8-D4; die Gore-Variante 0x80104694
 * SETZT +0x231 vor dem Kern und unterdrückt so den Schrei @0x801046E8). */
static void re2d_death(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                             /* frischer Kill → Kern/Variante */
        uint8_t row = e->re2z_prev_sub;
        /* Grab-Abbruch, falls der Hund im Latch stirbt (Port-Infrastruktur; RE2-DEATH [7]/[8]
         * = 0x801042B0 fährt die Phasen mit Gore-FX-Tail @0x801042E4-31C): */
        if (row == 7 && re15_player_victim_state() == 1) re15_player_victim_throwoff();
        int gore = (row == 0 || row == 5 || row == 6 || row == 9);   /* Tabelle @0x801055CC →
                                                                      * 0x80104610 */
        if (gore && !(row == 9 && e->re2z_hits1d2 < 3)) {  /* 0x80104694-Gate @0x801046A8-C4:
                                                            * `<3` = BRACKET 0 (Nah-Treffer) →
                                                            * Zeile-9-Gore nur bei Fern-Brackets.
                                                            * Produzent re15_re2_stamp_1d2 setzt
                                                            * zone+3*0 (Bracket OPEN) → wie im
                                                            * RE2-Nah-Fall bleibt die Gore-
                                                            * Variante hier unterdrueckt; Fern-
                                                            * Bracket = offene Folge-Lane. */
            e->re2d_se231 = 1;                             /* sb 1,561 @0x801046E8 (kein SE 7) */
            /* Part-Scatter 0x80104440 (7 Parts @0x80105680 {2,3,4,7,8,9,10}, Flags|0x4A,
             * Vel 800/−150/10/−100) = GIB-Dismember — Render-seitig OPEN, dokumentiert. */
            if (row == 9) {                                /* @0x801046F4-758 */
                e->re2d_budget21f = 1;
                re2d_fx(e, (int)(re15_re2_rand() & 0xfu), 7);   /* Gore FX 7 @0x80104748-50 */
                e->re2d_budget21f = 18;                    /* sb 18,543 @0x80104754-58 */
            }
        }
        if (e->re2d_se231 == 0) { re2d_se(7); }            /* SE 7 Todesschrei @0x801041C8-CC */
        e->re2d_se231 = 1;                                 /* Latch */
        {
            uint8_t d223 = e->re2d_dbl223;
            e->re2d_dbl223 = 0;                            /* sb zero,547 @0x801041AC */
            re2d_hurt_p0(e, pl);                           /* jal 0x80103344 @0x801041B0 */
            if ((d223 & 0x3fu) == 2) e->speed_h = 0;       /* @0x801041D8-E0 */
        }
        if (e->sub_state_2 == 0) e->sub_state_2 = 2;       /* Kern erzwingt die Phasen-Kette */
        return;
    }
    switch (e->sub_state_2) {                              /* Phasen @0x80105668 */
    case 1: re2d_hurt_p1(e); break;                        /* Luft 0x801034C8 */
    case 2:                                                /* Rutschen 0x80104200 → CORPSE */
        if (e->re2d_rel220 == 0) {                         /* @0x8010421C-24 */
            re2d_move(e, (int)e->re2z_t158);               /* @0x8010422C-30 */
            re2d_speed_dn(e, 10, 0);                       /* @0x80104240 */
        }
        if (re2d_advance(e, (int)e->re2z_t15a >= 0x200 ? 0x200 : 0x100)) {   /* @0x80104254-5C */
            if (e->sub_state_3 == 0) {
                e->sub_state_3 = 1;                        /* sb 1,7 @0x8010427C */
                re2d_clip(e, 18, 0, (int)e->anim_frac, 0x100, 0);   /* rate<<16|18 @0x80104274-8C */
            } else {
                re15_ai_set_state_word(e, 0x7);            /* sw 7,4 @0x80104290 → CORPSE */
            }
        }
        break;
    default:                                               /* [3] 0x801037E8 HP-Re-Roll („spielt
                                                            * tot") — byte-true mitgeführt */
        re2d_hurt_p4_reroll(e);
        re15_ai_set_state_word(e, 0x7);                    /* Port-Absicherung: kein Wiedergänger-
                                                            * Produzent → Corpse (dokumentiert) */
        break;
    }
}

/* ================================ CORPSE (state 7) ========================================= */
/* 0x801049EC: sub0 → +0x5=1 + word0|=0x2; 0x80104A2C Blut-Lache: Phase 0 → Modell-Farbwörter
 * (&0xFF000000)|0x00BFBF10 + Timer 90 (@0x80104A50-88); Phase 1: Textur-U/V += 8/Frame
 * (@0x80104A8C-A0) bis Timer 0 → Phase 2. Farbe/Scroll sind Render-seitig (Modell-Pool
 * +0x16C) — im Port OPEN dokumentiert; die Timer-Maschine läuft byte-true. */
static void re2d_corpse(re15_actor_t *e)
{
    if (e->sub_state_1 != 1) {
        e->sub_state_1 = 1;                                /* sb 1,5 @0x80104A08 */
        e->hp = -1;                                        /* Port-Konvention Corpse (Damage-
                                                            * Resolver-Gate; RE2 lässt HP<0) */
        e->re2d_budget21f = 0;
        e->re2z_dir16a = 0;                                /* +0x221-Analoge */
    }
    if (e->re2z_dir16a == 0) {                             /* Phase 0 @0x80104A4C-88 */
        e->re2z_dir16a = 1;
        e->re2z_t15a = 90;                                 /* sb 90,363 @0x80104A80-88 */
    } else if (e->re2z_dir16a == 1) {                      /* Phase 1 @0x80104A8C-C0 */
        if (--e->re2z_t15a == 0) e->re2z_dir16a = 2;       /* @0x80104AA4-C0 */
    }
    /* Liegepose hält (der globale Advancer überspringt state 7) */
}

/* ================================ INIT (state 0) =========================================== */

static const uint8_t re2d_spawn_tbl[9] = { 0, 1, 2, 4, 10, 12, 14, 13, 15 };   /* @0x80105458 */

static void re2d_init(re15_actor_t *e)
{
    re15_ai_set_state_word(e, 0x1);                        /* sw 1,4 @0x8010012C (INIT schreibt
                                                            * state=1 ZUERST) */
    e->re2z_t158 = 0; e->re2z_t15a = 0;
    e->re2z_flags21a = 0; e->re2z_dir16a = 0;
    e->re2z_self1d3 = 0; e->re2z_hits1d2 = 0; e->re2z_prev_sub = 0;
    e->re2d_vy146 = 0; e->re2d_turn224 = 0;
    e->re2d_offx228 = 0; e->re2d_offz22a = 0;
    e->re2d_route218 = 0; e->re2d_air219 = 0; e->re2d_abort21c = 0;
    e->re2d_pause21d = 0; e->re2d_bite21e = 0; e->re2d_budget21f = 0;
    e->re2d_rel220 = 0; e->re2d_launch222 = 0; e->re2d_dbl223 = 0;
    e->re2d_nolatch22c = 0; e->re2d_wound22d = 0; e->re2d_circle22e = 0;
    e->re2d_atkcd22f = 0; e->re2d_stuck230 = 0; e->re2d_se231 = 0; e->re2d_cd232 = 0;
    /* HP-Roll @0x80100150-234: 2 Draws (rand&3 + rand&0xf); Tabellen-Auswahl über
     * 0x800CFB74-Bits 0x40/0x20 (@0x80105340/60/B0/D0) — das RE2-Global hat keinen Port-
     * Produzenten → Default-Tabelle @0x80105340 (beide Bits 0). Spanne 119+3. */
    {
        uint32_t r1 = re15_re2_rand() & 3u;                /* @0x801001D0-E8 */
        e->hp = (int16_t)(re2d_hp_tbl_def[re15_re2_rand() & 0xfu] + r1);   /* @0x80100210-234 */
    }
    /* Hitbox @0x80100284-C4: +0x94=500, +0x98=−1000, +0x9E=1000, 600er-Familie — der Port
     * führt die Trefferhöhe über hit_radius/atk_pt (Damage-System); dokumentiert. Die
     * 2-Part-Schleife @0x801002D0-F8 (Parts 2..3: +0x9C=32/+0xA0=384/+0xA2=128 — Lane-D sagte
     * „4 Parts", der Loop läuft a0=2..3, selbst nachgelesen) ist Part-Hitbox-Metadaten. */
    e->dog_floor_y = (int16_t)e->y;                        /* +0x1C2-Analog: Spawn-Boden */
    e->speed_h = 0;                                        /* +0x144/146/148 = 0 @0x80100308-310 */
    e->root_prev_kf = -1;
    e->sca_mask = 4;
    /* Spawn-Param +0x10E: RE1.5-Räume tragen kein RE2-+0x10E → Port-MAPPING: alle RE1.5-Hunde-
     * Spawns = Raw 0 → Start-Substate tbl[0]=0 (IDLE) + Clip 1 mit Zufalls-Frame 0..63
     * (@0x801003A8-C8). Die Fenster-/Skript-Varianten (Raw 6/7/8/9/0x2003) haben keinen
     * RE1.5-Produzenten (dokumentiert OPEN). */
    e->sub_state_1 = re2d_spawn_tbl[0];                    /* lbu tbl[+0x10E&0x7F] @0x80100300-328 */
    e->sub_state_2 = 0; e->sub_state_3 = 0;
    re2d_clip(e, 1, (int)(re15_re2_rand() & 0x3fu), 0xF, 0x100, 1);   /* 0xF0001|frame @0x801003A8-C8 */
    /* Callback-Install 0x80104ACC → 0x800CE480 (@0x801004A8-B4): Skript-/Event-Hook (Clip-0-
     * Pose gespiegelt + Boden-Pin) — Konsument OFFEN, im Port nicht installiert. */
}

/* ================================ root tick ================================================ */

int re15_re2dog_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e->active) return 0;

    /* Root-Prolog @0x80100004-50: Freeze-Gate 0x800CFBDC&0x20000000 übernimmt der Port-Caller
     * (re15_dog_ai_tick wird bei s_ai_paused gar nicht erreicht); +0x1D3 low-7 Dekrement
     * (@0x80100028-3C) + +0x232 Dekrement (@0x80100040-50): */
    if (e->re2z_self1d3 & 0x7fu)
        e->re2z_self1d3 = (uint8_t)((e->re2z_self1d3 & 0x80u) | ((e->re2z_self1d3 & 0x7fu) - 1u));
    if (e->re2d_cd232) e->re2d_cd232--;

    if (e->state == 1) { e->re2z_prev_sub = e->sub_state_1; e->re2z_prev_hp = e->hp; }
    /* Treffer-Erkennung: das geteilte take_damage schreibt +0x4=2/3 und überschreibt +0x5/+0x6
     * (re15_damage.c:767-773) — der RE2-Damage-Writer lässt +0x5 stehen und nullt die Phase.
     * MAPPING: bei neuem Treffer Phase auf 0 zurücksetzen, Zeile = prev_sub. */
    if ((e->state == 2 || e->state == 3) && e->hp != e->re2z_prev_hp) {
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->re2z_prev_hp = e->hp;
    }

    int32_t ox = e->x, oz = e->z;
    switch (e->state) {                                    /* Tabelle @0x80105438 */
    case 0: re2d_init(e); break;                           /* 0x801000F4 */
    case 1:
        switch (e->sub_state_1) {                          /* Tabelle @0x80105464 */
        case 0:  re2d_sub0_idle(e, pl); break;             /* 0x80100784 */
        case 1:  re2d_sub1_stalk(e, pl); break;            /* 0x80100824 */
        case 2:  re2d_sub2_run(e, pl); break;              /* 0x80100B20 */
        case 3:  re2d_sub3_attack(e, pl); break;           /* 0x801011EC */
        case 4:  re2d_sub4_howl(e, pl); break;             /* 0x80101830 */
        case 5:  re2d_sub5_feed(e); break;                 /* 0x80101B14 */
        case 6:  re2d_sub6_retreat(e, pl); break;          /* 0x80101BDC */
        case 7:  re2d_sub7_latch(e, pl); break;            /* 0x80101CDC */
        case 8:  re2d_sub8_breakoff(e, pl); break;         /* 0x8010224C */
        case 9:  re2d_sub9_lost(e); break;                 /* 0x8010237C */
        case 10: re2d_sub10_aggidle(e, pl); break;         /* 0x80102404 */
        case 11: re2d_sub11_packhelp(e, pl); break;        /* 0x80102490 */
        case 12: re2d_sub12_postlatch(e, pl); break;       /* 0x80102608 */
        case 13: re2d_sub13_flee(e); break;                /* 0x801028AC */
        case 14: re2d_sub14_windowA(e, pl); break;         /* 0x80102C28 */
        case 15: re2d_sub15_windowB(e, pl); break;         /* 0x80102E58 */
        default: re2d_sub16_open(e); break;                /* 0x801030FC (Eintritt OFFEN) */
        }
        /* ACTIVE-Tail @0x80100510-30: EDD-Frame-Event-Bit 0x40000 → SE 8 */
        if (re2d_frame_word(e) & 0x40000u) re2d_se(8);
        break;
    case 2: re2d_hurt(e, pl); break;                       /* 0x801032A8 */
    case 3: re2d_death(e, pl); break;                      /* 0x801040DC */
    case 7: re2d_corpse(e); break;                         /* 0x801049EC */
    default: re15_ai_set_state_word(e, 0x201); break;      /* [4] jr-ra-Stub / [5][6] NULL */
    }

    /* Root-Epilog @0x80100090-D8: Stuck-Zähler +0x230 — Pos unverändert → ++; bewegt UND
     * +0xD==255 → 0 (der +0xD!=255-Zweig inkrementiert auch bewegt; +0xD = Partner-Index,
     * kein Port-Produzent → wie 255 behandelt, selbst disasm'd + dokumentiert). */
    if (e->x == ox && e->z == oz) { if (e->re2d_stuck230 < 255) e->re2d_stuck230++; }
    else e->re2d_stuck230 = 0;

    if (getenv("RE15_RE2_TRACE")) {                        /* Smoke-/Messlauf-Diagnose (wie der
                                                            * Zombie-Trace in decide_walk) */
        static uint32_t s_last[RE15_ACTOR_MAX];
        uint32_t sig = ((uint32_t)e->state << 16) | ((uint32_t)e->sub_state_1 << 8) | e->sub_state_2;
        if (sig != s_last[slot]) {
            fprintf(stderr, "[re2dog] slot %d state=%d sub=%d/%d clip=%d hp=%d dist=%u spd=%d\n",
                    slot, e->state, e->sub_state_1, e->sub_state_2,
                    (int)e->motion, (int)e->hp, (unsigned)e->ai_dist, (int)e->speed_h);
            s_last[slot] = sig;
        }
    }
    return 1;
}
