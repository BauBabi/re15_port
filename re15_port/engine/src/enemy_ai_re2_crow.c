/*
 * RE1.5 Rebuilt — RESIDENT EVIL 2 crow brain (kind 0x21; PORT OPTION, OPTIONS→AI→RE2).
 *
 * WELLE D. Everything here is RE'd from the RE2 (Leon retail) crow module
 *     info/re2leon/COMMON/BIN/EMOVL21_S0.BIN  == CDEMD0.EMS[Sektor 0x528] (byte-identisch,
 *     Lane K; TOC: EXE @0x8009adf4 Typ 0x21 = Code 0x4A88 @0x51E/0x528, TIM 0x8220 @0x532,
 *     EMD 0x7E28 @0x543 — alle Werte selbst nachgelesen). Modul laedt RAW @0x80100000 (Slot 0).
 *     disassemble:  .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMOVL21_S0.BIN
 * Full self-disassembly 2026-08-16 (scratchpad crow_self.asm, 4770 instr; JEDE Konstante unten
 * wurde aus diesem eigenen Dump re-gelesen — Adressen im Zeilenkommentar).
 *
 * State table @0x80104908 (self-verified):
 *     [0] 0x801002FC INIT   [1] 0x801004E4 ACTIVE (DEC @0x80104928 + EXEC @0x80104964 auf +0x5,
 *                                15 Substates; ALT-Tabellen @0x801049A0/DC fuer +0x10E&0x80 sind
 *                                INHALTSGLEICH — selbst gedumpt)
 *     [2]=[3] 0x801028BC HURT (Zeilen-Dispatch @0x80104A18 auf +0x5)
 *     [4] 0x801034DC SKRIPT-PERCH (eigene Tabelle @0x80104A64)   [5][6] NULL
 *     [7] 0x80103950 CORPSE (Tabelle @0x80104A70 auf +0x5)
 *
 * Der RE1.5-Default (Kraehe 0x21, Flug-Brain FUN_80112020, byte-true) bleibt byte-identisch:
 * dieses Brain laeuft NUR bei re15_ai_flavor()==RE15_AI_FLAVOR_RE2, Hook am Kopf von
 * re15_crow_ai_tick (enemy_ai_common.c).
 *
 * PORT MAPPINGS (kein Original-Verhalten erfunden — jede Luecke ist markiert):
 *   - Navigator FUN_8004A808 (Steuerpunkt +0x1C4/+0x1C6) + Routen-Helfer FUN_8004AA50 sind nicht
 *     portiert → Steuerziel = Spielerposition, Routen-Byte +0x21F bleibt 0; die +0x220/+0x21F-
 *     Buchhaltung (RNG-Draws!) laeuft byte-true (Welle-B/C-Muster).
 *   - LOS-Ray 0x80050858(self,PL,0x8400,0) → re15_re2_los_clear (RE1.5-FUN_8003dcc4-Ray, alle
 *     4 Regionen pro Tick). POLARITAET SELBST BELEGT: Root @0x801001C8-E8 `beq v0,zero ->
 *     ori 0x2` — ret==0 setzt +0x22A-Bit 2, Konsumenten (Sub-11-DEC @0x80101D60-6C, Sub-13-DEC
 *     @0x8010221C-28, Arbitrierung @0x80104284-90) verlangen Bit 2 fuer den Angriff = "Sicht
 *     frei". Mode 0x8400 selbst nicht RE'd (OFFEN).
 *   - +0xD (Body-Push-Stempel): EXE FUN_800355c4 setzt +0xD=0xFF pro Frame, der Part-Push
 *     FUN_80034d0c stempelt beim Kontakt +0xD = Pusher-Id (Spieler = Listenkopf 0) — beide
 *     selbst decompile-gelesen. `+0xD==0` = "der Spieler hat mich diesen Frame gepusht".
 *     Port-Produzent = re15_body_push(player, …) im Root-Tail (Vorframe-Phase, wie der Hund
 *     seinen +0x110-Kontakt liest).
 *   - Wand-System: Root-Tail FUN_8003567c schreibt +0x110 (Kontakt) + +0x114 (Zellen-Record);
 *     der Crash-Prober 0x80104094 zieht die Wand-EBENE via 0x80050110 und crasht nur im
 *     Anflug-Fenster (plane<<10)+1920..2176 (@0x80104138-58). Port: SCA-Klemme
 *     (re15_collision_constrain_enemy, wie die RE1.5-Kraehe) liefert Kontakt + eine
 *     Frontal-Metrik (Restweg² < Sollweg²/4) als deklariertes MAPPING fuer das Ebenen-Fenster.
 *   - STRIKE-Spielerschaden: Sub 12 P0 schreibt NUR den Spieler-State (2, Sub 1/0)
 *     (@0x8010201C-34) — die Schadenshoehe liegt im EXE-State-2-Handler des Spielers, der
 *     nicht RE'd ist (kein FUN_800401d4-Caller dafuer; einziger weiterer EXE-Caller
 *     FUN_80047664 ist ein AoE-Helfer, selbst gelesen). Port: Hit-Guard/Flinch OHNE
 *     HP-Verlust — OFFEN, kein erfundener Wert. Die GRAB-Pecks tragen die byte-zitierten
 *     5 HP (0x800401d4(5, aliveflag) @0x8010265C-64).
 *   - Rumble/Screenshake 0x8003947C/0x80039514 = Pad-Aktuator-Ringpuffer (Welle-C-Decompile)
 *     → dokumentiert uebersprungen.
 *   - SE-Volume 0x8010472C (350/100/50/10 → +0x90/92/9A/9C) hat keinen Port-Kanal → NOP mit
 *     Beleg (byte-relevant nur als Wand-Radius-Quelle, s. Root-Tail-Kommentar).
 *   - FX 0x8001BF10: Federn 0xC000800 (±512, @0x80104678) / Federburst 0x12800 (±128,
 *     @0x8010459C) → RE1.5-ESP-Splatter-Stand-in (dieselbe dokumentierte Annaeherung wie die
 *     RE1.5-GIB-Lane); Grab-Blut 0x11000 @ y+500 → RE1.5-Room-Bank-Blut (Hunde-Muster).
 *   - Part-Scatter des GIB (13 Zeilen à 172 B ab [+0x198], Wurf 0x80102EF4 / Wirbel 0x80102F34)
 *     ist Render-seitig nicht modelliert → crow_hide-Mesh-Wipe (RE1.5-GIB-Muster) + Stand-in-FX.
 *   - Modell-Tint 0x80016fe4([+0x16C], 0xBFBF10) (@0x80103A08-20) ist Render-seitig OFFEN
 *     (kein Port-Modell-Tint-Kanal; Welle-C-Praezedenz Hunde-Leiche).
 */

#include <stdint.h>
#include <stdlib.h>   /* getenv */
#include <stdio.h>
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 / re15_atan2_q12 */
#include "re15_damage.h"     /* re15_ai_arc_test (==FUN_80015614), re15_enemy_player_dist */
#include "re15_enemy_ai.h"   /* re15_body_push, victim shims, re15_actor_clip_len */
#include "re15_enemy.h"      /* re15_enemy_find (RE2-Bank des Welle-A-Loaders) */
#include "re15_esp.h"        /* re15_esp_fx_spawn_ex / re15_esp_fx_splatter (Stand-ins) */
#include "re15_collision.h"  /* re15_collision_constrain_enemy (SCA-Wand-Klemme) */
#include "re15_room.h"       /* g_room_rdt / g_room_rdt_ok */

extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);

#define RE15_BODY_R_PLAYER  450   /* PSX.EXE hitbox @file 0x64694: 0x1c2 (enemy_ai_common.c) */

/* ---- ENEMSE audio hook ------------------------------------------------------------------ */
/* Kraehen-Bank: ROOM-Paar-Tabelle @0x800A7400 (RE2 PSX.EXE file 0x97C00, 73 Zeilen à 2 kinds,
 * selbst gelesen 2026-08-16): kind 0x21 steht in GENAU EINER Zeile — 21 = {0x21, 0x00} = reine
 * Kraehen-Bank, ERSTE Haelfte (flag2000=0, FUN_8005bd6c-Kopf). EDT-Probe Bank 21 (TOC
 * @0x800A7B1C selbst dekodiert): ids 0..14 live, 15+ leer; SEs 1..6 alle belegt
 * (1=0x2230000 Fluegelschlag, 2=0x2330000 Picken, 3=0x3420000 Kreischen, 4=0x3520000
 * Strike-Impact, 5=0x3640000 Thud, 6=0x3720000 Kraechzen). */
#define RE2CROW_ENEMSE_BANK 21

static void (*s_re2c_se_fn)(int se_id, int flag2000) = 0;
static void (*s_re2c_bank_fn)(int bank) = 0;

/* Bank-Wahl bleibt EMPIRISCH (deklarierte PORT-NAEHERUNG) — der byte-true Mechanismus
 * FUN_80052b38 liest die RE2-RAUMDATEN (Spawn-Record +7), nicht den Gegner-kind; Belege +
 * Refutation im Kopf von enemy_ai_re2_zombie.c (RE2Z_ENEMSE_BANK). Stuetze fuer 21: eigener
 * jal-0x8005bd6c-Scan ueber EMOVL21_S0.BIN (2026-08-17) findet die ids {1,2,3,4,5,6} — Bank 21
 * (erste Haelfte) hat 0..14 live, deckt sie also vollstaendig. */
void re15_re2crow_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int))
{
    s_re2c_se_fn   = se_fn;
    s_re2c_bank_fn = bank_fn;
    if (s_re2c_bank_fn) {
        const char *ov = getenv("RE15_RE2_CROW_SE_BANK");
        s_re2c_bank_fn(ov ? atoi(ov) : RE2CROW_ENEMSE_BANK);
    }
}
static void re2c_se(int id) { if (s_re2c_se_fn) s_re2c_se_fn(id, 0); }   /* erste Map-Haelfte */

/* ---- kleine Helfer ------------------------------------------------------------------------ */

/* 0x80104078(self, a1, a2): +0x4=a1, +0x5=a2, +0x6/+0x7=0 (`sb a1,4; sb a2,5; sh zero,6`
 * @0x80104078-84, selbst disasm'd). */
static void re2c_state(re15_actor_t *e, int st, int sub)
{
    e->state = (uint8_t)st; e->sub_state_1 = (uint8_t)sub;
    e->sub_state_2 = 0; e->sub_state_3 = 0;
}
/* 0x80104088(self, a1): +0x5=a1, +0x6/+0x7=0 (@0x80104088-90). */
static void re2c_sub(re15_actor_t *e, int sub)
{
    e->sub_state_1 = (uint8_t)sub; e->sub_state_2 = 0; e->sub_state_3 = 0;
}

/* Anim-Wort +0x14C = rate<<16|frame<<8|clip (Welle-B-Konvention: rate → frac-Seed,
 * 0x8002959c-a3 → blend). Alle Kraehen-Writes tragen rate 7 (lui 0x7). */
static void re2c_clip(re15_actor_t *e, int clip, int frame)
{
    e->motion     = (int16_t)clip;
    e->anim_frame = (uint16_t)frame;
    e->anim_frac  = 7;
    e->anim_blend_rate = 0x200;
    e->anim_freeze = 0;
    e->anim_flags = (uint16_t)((e->anim_flags & ~0x80u) | 0x04u);
}

static int re2c_frame_slot(const re15_actor_t *e)          /* +0x14D-Frame-Byte-Analog */
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return (int)e->anim_frame;
    return (int)(e->anim_frame % (uint32_t)fc);
}

static int re2c_adv(re15_actor_t *e) { return re15_re2_advance_959c(e, 0x200); }
/* a3=0x10200 (Idle-P9/P13 @0x801009E4-EC/@0x80100AF0-F8): Bit 0x10000-Semantik OFFEN
 * (Lane K §5) — Port behandelt es als blend 0x200. */
static int re2c_adv_flag(re15_actor_t *e) { return re15_re2_advance_959c(e, 0x200); }

/* FUN_800152C8(self, yaw_ofs) / FUN_80015350(self,0,0) — Bewegungs-Paar (Welle-C-Zitate
 * @0x800152E4-334 / @0x80015350-3D8; Kraehe haelt +0x148==0, INIT @0x80100344). */
static void re2c_move(re15_actor_t *e, int yaw_ofs)
{
    int yaw = ((int)e->rot_y + yaw_ofs) & 0xfff;
    int spd = (int)e->speed_h;
    e->x += (int32_t)(((int32_t)re15_cos_q12(yaw) * spd) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(yaw) * spd) >> 12);
}
static void re2c_move3d(re15_actor_t *e)                   /* == 0x80015350(0,0) */
{
    re2c_move(e, 0);
    e->y += (int32_t)e->re2d_vy146;
}

/* 0x8010472C(self, vol): +0x9A/+0x9C/+0x90/+0x92 = vol (sh @0x8010472C-3C). Kein Port-
 * SE-Volume-Kanal → dokumentierter NOP (Aufrufstellen bleiben byte-sichtbar). +0x90 ist
 * zugleich der Wand-Radius von FUN_8003567c (param[0x24]) — s. Root-Tail-MAPPING. */
static void re2c_vol(re15_actor_t *e, int vol) { (void)e; (void)vol; }

/* 0x80104678(self, n): n Federn, Offsets ±512 (sll24/sra22 @0x801046A4-E0), FX 0xC000800,
 * a1 = rand<<4 (@0x801046F4). Port: RE1.5-ESP-Splatter-Stand-in (RE1.5-GIB-Praezedenz);
 * die 4 RNG-Draws pro Feder bleiben (Draw-Count = Verhalten am geteilten Strom). */
static void re2c_feather(re15_actor_t *e, int n)
{
    for (int i = 0; i < n; i++) {
        int32_t ox = ((int32_t)(int8_t)(re15_re2_rand() & 0xffu)) << 2;
        int32_t oy = ((int32_t)(int8_t)(re15_re2_rand() & 0xffu)) << 2;
        int32_t oz = ((int32_t)(int8_t)(re15_re2_rand() & 0xffu)) << 2;
        (void)re15_re2_rand();                             /* a1 = rand<<4 @0x801046F0-F4 */
        re15_esp_fx_splatter(re15_esp_room_bank(), 0, 1,
                             e->x + ox, e->y + oy, e->z + oz, (int32_t)e->dog_floor_y);
    }
}
/* 0x8010459C(self): EIN Federburst, Offsets ±128 (sll24/sra24 @0x801045B0-EC), FX 0x12800,
 * a1 = rand<<4. 4 Draws. */
static void re2c_featherburst(re15_actor_t *e)
{
    int32_t ox = (int32_t)(int8_t)(re15_re2_rand() & 0xffu);
    int32_t oy = (int32_t)(int8_t)(re15_re2_rand() & 0xffu);
    int32_t oz = (int32_t)(int8_t)(re15_re2_rand() & 0xffu);
    (void)re15_re2_rand();
    re15_esp_fx_splatter(re15_esp_room_bank(), 0, 1,
                         e->x + ox, e->y + oy, e->z + oz, (int32_t)e->dog_floor_y);
}

/* 0x8010452C(mask): BROADCAST — alle Typ-0x21-Entities der Aktivliste AB EINTRAG 1 (Basis
 * 0x800CFE1C = Liste+4, der Spieler-Kopf wird uebersprungen), NUR wenn NICHT selbst geclaimt
 * (`andi a1,0x4; bne` @0x8010456C-70): +0x22A|=mask, +0x220=0, +0x21F=0 (@0x80104574-80). */
static void re2c_broadcast(unsigned mask)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *o = &g_actors[i];
        if (!o->active || o->type != 0x21) continue;       /* lbu +0x8 == 33 @0x80104554-5C */
        if (o->re2c_flags22a & 0x4u) continue;             /* Claimer ausgenommen @0x8010456C-70 */
        o->re2c_flags22a |= (uint16_t)mask;                /* or; sh 554 @0x80104574-78 */
        o->re2d_rel220 = 0; o->re2d_budget21f = 0;         /* sb zero,544/543 @0x8010457C-80 */
    }
}

/* Spieler-Boden-Y 0x800CFDBA (PL+0x1C2) — Port-MAPPING: pl->y (der Port-Spieler steht am
 * Boden; Welle-C-Muster fuer +0x1C2-Analoga). */
static int32_t re2c_pl_floor_y(const re15_actor_t *pl) { return pl->y; }

/* 0x800CFBF6-Bewegungsbits (Mapping-Beleg enemy_ai_re2_zombie.c re2z_cfbf6). */
static uint16_t re2c_cfbf6(const re15_actor_t *pl)
{
    if (pl->motion == 100) return 0x04u;     /* RUN  @0x8003D18C */
    if (pl->motion == 105) return 0x02u;     /* WALK @0x8003CC80 / @0x8003D6B4 */
    return 0;
}

/* ============================ Wand-Crash-Prober 0x80104094 ================================= */
/* Kontakt-Gate +0x110&1 (@0x801040AC-B8, Port: crow_wall Bit0 aus dem Root-Tail).
 * Streak +0x21D: frisch (==0) && speed>=231 (@0x801040D0-D8) → Ebenen-Fenster
 * (plane<<10)+1920..2176 (@0x80104138-58; plane 2 wrap-identisch @0x80104120-28) → Sub 10.
 * Port-MAPPING Ebene: crow_wall Bit1 = Frontal-Metrik (s. Root-Tail). Streak>=11
 * (@0x8010416C-74): rand&0x7f==0 → Sub 7 sonst Sub 8 (@0x8010417C-9C). speed>=101 → −4
 * (@0x801041A4-BC); +0x21D++ (@0x801041C0-D0); kein Kontakt → +0x21D=0 (@0x801041D4). */
static void re2c_wall_probe(re15_actor_t *e)
{
    if (!(e->crow_wall & 1)) { e->re2d_pause21d = 0; return; }
    if (e->re2d_pause21d == 0 && (int)e->speed_h >= 231 && (e->crow_wall & 2))
        re2c_sub(e, 10);
    if (e->re2d_pause21d >= 11) {
        if ((re15_re2_rand() & 0x7fu) == 0u) re2c_sub(e, 7);   /* @0x80104184-94 */
        else                                 re2c_sub(e, 8);   /* @0x80104198-9C */
    }
    if ((int)e->speed_h >= 101) e->speed_h = (int16_t)(e->speed_h - 4);
    e->re2d_pause21d++;
}

/* ====================== Arbitrierung 0x801041F0 + Listener ================================= */
/* Nur EINE Kraehe greift an: Flock-Mutex = g_re2_room_gflags Bit 0 (0x800CFBF4). */
static void re2c_attack_arbiter(re15_actor_t *e, re15_actor_t *pl)
{
    if (pl->hp < 0) return;                                /* bltz @0x80104204 */
    /* Grab-Join: +0x22A-Bits 8|2 zusammen (lw +0x228; and 0xA0000 @0x8010420C-18) →
     * Sub 13 + Cooldown-Clear + Bits 8|0x40 loeschen (andi 0xffb7 @0x80104228-34). */
    if ((e->re2c_flags22a & 0xAu) == 0xAu) {
        re2c_sub(e, 13);                                   /* @0x80104220-24 */
        e->re2d_abort21c = 0;                              /* sb zero,540 @0x8010422C */
        e->re2c_flags22a &= (uint16_t)~0x48u;
        return;
    }
    if (e->re2d_abort21c) { e->re2d_abort21c--; return; }  /* Cooldown @0x8010423C-50 */
    /* Fenster dist ∈ [901, 901+0x189A] (addiu −901; sltiu 0x189B @0x80104254-60) + Mutex frei
     * (@0x8010426C-7C) + LOS Bit 2 (@0x80104284-90) + Kegel(Spieler, 256)==0 (@0x801042A8-B0). */
    if (e->ai_dist - 901u < 0x189Bu
        && !(g_re2_room_gflags & 0x1u)
        && (e->re2c_flags22a & 0x2u)
        && re15_ai_arc_test(e, pl->x, pl->z, 256) == 0) {
        g_re2_room_gflags |= 0x1u;                         /* Claim @0x801042C4-CC */
        e->re2c_flags22a  |= 0x4u;                         /* @0x801042D0-E0 */
        if ((re15_re2_rand() & 0xfu) == 0u) re2c_sub(e, 11);   /* 1/16 Anlauf @0x801042E4-F4 */
        else                                re2c_sub(e, 13);   /* sonst Hover @0x80104300-04 */
        return;
    }
    /* Direkt-STRIKE bei Spieler-Body-Kontakt (+0xD==0 @0x80104310-18, Port: crow_contact):
     * Streak +0x21E frisch (@0x80104320-28) && speed>=231 (@0x80104330-38) && Mutex frei
     * (@0x80104344-54) && Kegel 256 (@0x8010436C-74) → Sub 12 (@0x8010437C-80; OHNE Claim).
     * Lane-K-Korrektur: das Gate ist KEIN Dist<901-Fenster, sondern der Body-Push-Stempel. */
    if (e->crow_contact) {
        if (e->re2d_bite21e == 0) {
            if ((int)e->speed_h >= 231 && !(g_re2_room_gflags & 0x1u)
                && re15_ai_arc_test(e, pl->x, pl->z, 256) == 0)
                re2c_sub(e, 12);
        }
        if (e->re2d_bite21e >= 11) {                       /* sltiu 0xb @0x8010432C/8C-90 */
            re2c_sub(e, 8);                                /* @0x80104398-9C */
            e->re2d_rel220    = (uint8_t)(re15_re2_rand() & 0x7fu);   /* @0x801043A0-B0 */
            e->re2d_budget21f = 0;                         /* 0x8004AA50 (MAPPING) @0x801043AC-B4 */
        }
        if ((int)e->speed_h >= 101) e->speed_h = (int16_t)(e->speed_h - 4);   /* @0x801043B8-D0 */
        e->re2d_bite21e++;                                 /* @0x801043D4-E4 */
    } else e->re2d_bite21e = 0;                            /* @0x801043E8 */
}

/* 0x80104400 — Abbruch-Listener: +0x22A&0x10 → Vol 350, Nav-Reseed rand&0x9D (!) + 0x8004AA50,
 * Sub 4, Cooldown +0x21C=30, Bits 0x58 loeschen (andi 0xffa7 @0x80104458-64). */
static void re2c_abort_listener(re15_actor_t *e)
{
    if (!(e->re2c_flags22a & 0x10u)) return;
    re2c_vol(e, 350);                                      /* @0x80104424-28 */
    e->re2d_rel220    = (uint8_t)(re15_re2_rand() & 0x9du);/* andi 0x9d @0x80104434-3C */
    e->re2d_budget21f = 0;                                 /* aa50 (MAPPING) @0x80104438-48 */
    re2c_sub(e, 4);                                        /* @0x80104440-50 */
    e->re2d_abort21c  = 30;                                /* sb 30,540 @0x8010444C-54 */
    e->re2c_flags22a &= (uint16_t)~0x58u;
}

/* 0x8010447C — Spieler tot + Pacifier abgelaufen: dist < 0x708 (1800) → Sub 7 landen. */
static void re2c_dead_player_lander(re15_actor_t *e)
{
    if (e->ai_dist < 0x708u) re2c_sub(e, 7);               /* sltiu 0x708 @0x80104484-9C */
}

/* geteilter DEC-Schwanz der Flugzustaende (Sub 4/5/6): Wand-Prober + Arbitrierung/Listener
 * bzw. Toter-Spieler-Lander (@0x80101150-98 / @0x8010133C-84 / @0x801014DC-24). */
static void re2c_flight_dec_tail(re15_actor_t *e, re15_actor_t *pl)
{
    re2c_wall_probe(e);
    if (pl->hp < 0 && e->re2c_pac221 <= 0) re2c_dead_player_lander(e);
    else { re2c_attack_arbiter(e, pl); re2c_abort_listener(e); }
}

/* ============================== ACTIVE substates =========================================== */

/* -- Sub 0 SITZEN/IDLE — DEC 0x801006C0 ---------------------------------------------------- */
static void re2c_dec0_idle(re15_actor_t *e, re15_actor_t *pl)
{
    int wake = 0;
    if (pl->hp < 0 && e->re2c_pac221 <= 0) {               /* bgez/bgtz @0x801006D0-E0 */
        if (e->ai_dist < 0x708u) return;                   /* tot+nah: NIE wecken @0x801006E8-F4 */
    }
    if (e->ai_dist < 0x709u) wake = 1;                     /* dist<=1800 sltiu 0x709 @0x80100704 */
    if ((re2c_cfbf6(pl) & 0x4u) && e->ai_dist < 0x1519u)   /* RENNT + dist<5401 @0x80100714-34 */
        wake = 1;
    if (re2c_cfbf6(pl) & 0x1u) wake = 1;                   /* toter Zweig (Bit 1 ohne Setzer,
                                                            * Zombie-Census Welle B) @0x80100738-50 */
    if (e->re2c_flags22a & 0x1u) {                         /* Wake-Timer-Arm @0x80100754-60 */
        uint16_t t = (uint16_t)e->re2z_t158;
        e->re2z_t158 = (int16_t)(t - 1);                   /* Dec IMMER (Delay-Slot @0x80100778) */
        if (t == 0) wake = 1;                              /* Wake am 0-Tick (Wrap 0xFFFF) @0x8010077C */
    }
    if (wake) re2c_sub(e, 1);                              /* 0x80104088(self,1) @0x80100788-8C */
}

/* -- Sub 0 EXEC 0x801007A0: die 15-Phasen-Idle-Maschine (Header-Tabelle @0x80100004) -------- */
static void re2c_exec0_idle(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 @0x801007EC */
        e->re2d_air219 = (uint8_t)(re15_re2_rand() % 100u);/* %100 (magic 0x51EB851F) @0x80100A8C-B8 */
        e->sub_state_2 = 7;                                /* sb 7,6 @0x80100808 */
        break;
    case 1: {                                              /* P1 @0x8010080C */
        uint32_t r = re15_re2_rand() % 5u;                 /* magic 0x66666667 @0x80100814-4C */
        e->re2d_air219 = 5;                                /* sb 5,537 @0x80100824 */
        e->sub_state_2 = 10;                               /* sb 10,6 @0x8010082C */
        re2c_clip(e, 0, (int)r);                           /* (r%5)<<8|0x70000 @0x8010084C-5C */
        break;
    }
    case 2:                                                /* P2 @0x80100860 */
        re2c_clip(e, 1, 0); e->sub_state_2 = 8; break;     /* 0x70001; sb 8,6 @0x80100868-74 */
    case 3:                                                /* P3 @0x80100878 */
        re2c_clip(e, 1, 0); e->sub_state_2 = 9; break;     /* @0x80100880-8C */
    case 4:                                                /* P4 @0x80100890 */
        re2c_clip(e, 1, 0); e->sub_state_2 = 11; break;    /* @0x80100898-A4 */
    case 5: {                                              /* P5 @0x801008A8 */
        uint32_t r = re15_re2_rand() % 7u;                 /* magic 0x92492493 @0x801008B0-E8 */
        e->sub_state_2 = 8;                                /* sb 8,6 @0x801008C4-C8 */
        re2c_clip(e, 2, (int)r);                           /* (r%7)<<8|0x70002 @0x801008EC-F8 */
        break;
    }
    case 6: {                                              /* P6 HOP-Start @0x801008FC */
        re2c_clip(e, 0xA, 0);                              /* 0x7000A @0x80100900-08 */
        uint32_t r = re15_re2_rand() % 5u;                 /* @0x8010090C-4C */
        e->re2d_turn224 = (int16_t)e->x;                   /* Snapshot sh a0,548 @0x8010092C */
        e->re2c_snap226 = (int16_t)e->z;                   /* sh a1,550 @0x80100930 */
        e->sub_state_2 = 13;                               /* sb 13,6 @0x80100924 */
        e->re2d_air219 = (uint8_t)(r + 15);                /* +15 @0x8010094C-54 */
        break;
    }
    case 7: {                                              /* P7 Timer/Wuerfel @0x80100958 */
        if (e->re2d_air219 == 0) {
            uint32_t r = re15_re2_rand() % 7u;             /* @0x80100968-98 */
            e->re2d_route218 = (uint8_t)r;                 /* sb v0,536 @0x8010099C */
            if (r == 6) {                                  /* @0x801009A8-AC */
                if ((re15_re2_rand() & 0xfu) == 0u)        /* 1/16 @0x801009B4-C0 */
                    e->sub_state_2 = 6;                    /* Hop @0x80100ACC */
                else e->sub_state_2 = 0;                   /* @0x801009C8-CC */
            } else e->sub_state_2 = (uint8_t)r;            /* @0x801009D0-D4 */
        }
        e->re2d_air219--;                                  /* Dec IMMER (Wrap) @0x80100AD0-E0 */
        break;
    }
    case 8:                                                /* P8 @0x80100A34 */
        if (re2c_adv(e)) e->sub_state_2 = 0;               /* @0x80100A44-58 */
        break;
    case 9:                                                /* P9 @0x801009D8 (a3=0x10200) */
        if (re2c_adv_flag(e)) e->sub_state_2 = 0;
        break;
    case 10: {                                             /* P10 @0x801009F0 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* Dec/Wrap (Delay-Slot @0x80100A00) */
        if (t == 0) {
            if (!(e->re2c_flags22a & 0x20u)                /* Kraechz-Sperre @0x80100A04-10 */
                && (re15_re2_rand() & 0x1u))               /* 50% @0x80100A18-24 */
                re2c_se(6);                                /* SE 6 @0x80100A2C */
        }
        if (re2c_adv(e)) e->sub_state_2 = 0;               /* @0x80100A44-58 */
        break;
    }
    case 11:                                               /* P11 @0x80100A5C */
        if (re2c_adv(e)) {
            e->sub_state_2 = 12;                           /* sb 12,6 @0x80100A74-7C */
            e->re2d_air219 = (uint8_t)(re15_re2_rand() % 100u);   /* @0x80100A78-B8 */
        }
        break;
    case 12:                                               /* P12 @0x80100ABC */
        if (e->re2d_air219 == 0) e->sub_state_2 = 3;       /* @0x80100AC4-CC */
        e->re2d_air219--;                                  /* Dec IMMER @0x80100AD0-E0 */
        break;
    case 13: {                                             /* P13 Hop hin @0x80100AE4 */
        re2c_adv_flag(e);
        re15_re2z_move_root(e);                            /* Hop-Mover 0x80103FE0 (Keyframe-
                                                            * Root-Delta) @0x80100AFC-B00 */
        int f = re2c_frame_slot(e);
        if (f == (int)e->re2d_air219) {                    /* Frame == Ziel @0x80100B04-10 */
            e->anim_frame = (uint16_t)(39 - f);            /* (39-f)<<8|0x7000A @0x80100B1C-30 */
            e->sub_state_2 = 14;                           /* sb 14,6 @0x80100B2C-38 */
        }
        break;
    }
    default:                                               /* P14 Hop zurueck @0x80100B3C */
        if (re2c_adv(e)) {
            e->sub_state_2 = 0;                            /* sb zero,6 @0x80100B60 */
            e->x = (int32_t)e->re2d_turn224;               /* sw v0,56 @0x80100B64 (lh-Restore) */
            e->z = (int32_t)e->re2c_snap226;               /* sw v1,64 @0x80100B6C */
        } else re15_re2z_move_root(e);                     /* @0x80100B70-74 */
        break;
    }
}

/* -- Sub 1 ALARM EXEC 0x80100B9C (DEC 0x80100B94 = jr ra) ----------------------------------- */
static void re2c_exec1_alarm(re15_actor_t *e)
{
    /* Review-Fix #3, selbst nachdisassembliert 0x80100B9C-C28: der P0-Block @0x80100BCC-D8
     * ist nur `jal rand; andi 0x7; sb v0,537` — es gibt KEINEN Write auf +0x6 in der ganzen
     * Funktion (0x80104088 nullt sub2 beim Eintritt). Das Original wuerfelt also in JEDEM
     * Alarm-Tick rand&7 NEU und exitet erst bei einem frischen 0-Wurf (geometrisch, p=1/8,
     * genau 1 Draw/Tick); der Delay-Slot-Dec @0x80100BEC wird vom naechsten Re-Roll
     * ueberschrieben (wirkungslos, aber byte-true mitgefuehrt). Die fruehere sub2=1-Latch
     * war erfunden. */
    if (e->sub_state_2 <= 1) {
        if (e->sub_state_2 == 0)
            e->re2d_air219 = (uint8_t)(re15_re2_rand() & 0x7u);   /* @0x80100BCC-D8 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* Delay-Slot-Dec @0x80100BEC */
        if (t == 0) {                                      /* ALT==0-Test @0x80100BDC-E8 */
            if (e->re2z_f10e & 0x40u) re2c_sub(e, 3);      /* +0x10E&0x40 @0x80100BF0-C08 */
            else                      re2c_sub(e, 2);      /* @0x80100C0C-14 */
        }
    }
}

/* -- Sub 2 ABHEBEN EXEC 0x80100C34 (Phasen-Tabelle @0x80100044; DEC 0x80100C2C = jr ra) ------
 * Ph0..4 = Setup-Zeilen, Ph5 = Timer/Steer/Move-Schwanz @0x80100DCC (Setups fallen im selben
 * Tick hinein). vy-Folge SELBST GELESEN: −10 (P0) → −80 (P1) → −60 (P2) → −100 (P3) → −80 (P4)
 * — Lane-K-Reihenfolge (−10/−60/−80/−100) korrigiert. */
static void re2c_exec2_takeoff(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: {                                              /* P0 @0x80100C84 */
        int s1 = (int)(re15_re2_rand() & 0x7u);            /* Startframe @0x80100C90 */
        if (re15_re2_rand() & 0x1u) re2c_se(1);            /* Fluegelschlag @0x80100C94-A4 */
        if ((re15_re2_rand() & 0x7u) == 0u) re2c_feather(e, 1);   /* 1/8 @0x80100CA8-C4 */
        re2c_clip(e, 7, s1);                               /* (s1<<8)|0x70007 @0x80100CC8-D8 */
        e->re2d_pause21d = 0;                              /* sb zero,541 @0x80100CE0 */
        e->speed_h = (int16_t)(100 + (re15_re2_rand() & 0x1fu));  /* @0x80100CDC-EC */
        e->re2d_vy146 = -10;                               /* sh −10,326 @0x80100CF0-F4 */
        e->re2d_air219 = (uint8_t)(9 - s1);                /* @0x80100CF8-D00 */
        e->re2d_route218 = 1;                              /* sb 1,536 @0x80100D04-08 */
        e->re2c_flags22a |= 0x1u;                          /* @0x80100D0C-20 */
        e->sub_state_2 = 5;                                /* sb 5,6 @0x80100D14 */
        break;
    }
    case 1:                                                /* P1 @0x80100D24 */
        e->re2d_vy146 = -80; e->re2d_air219 = 6; e->re2d_route218 = 2;
        e->sub_state_2 = 5; break;                         /* @0x80100D24-3C, @0x80100DC4-C8 */
    case 2:                                                /* P2 @0x80100D40 */
        if (re15_re2_rand() & 0x1u) re2c_se(1);            /* @0x80100D48-58 */
        e->re2d_vy146 = -60; e->re2d_air219 = 4; e->re2d_route218 = 3;
        e->sub_state_2 = 5; break;                         /* @0x80100D5C-74 */
    case 3:                                                /* P3 @0x80100D78 */
        e->re2d_vy146 = -100; e->re2d_air219 = 9; e->re2d_route218 = 4;
        e->sub_state_2 = 5; break;                         /* @0x80100D78-90 */
    case 4:                                                /* P4 @0x80100D94 */
        if ((re15_re2_rand() & 0x7u) == 0u) re2c_feather(e, 1);   /* @0x80100D9C-B0 */
        e->re2d_vy146 = -80; e->re2d_air219 = 255;
        e->sub_state_2 = 5; break;                         /* @0x80100DB4-C8 */
    default: break;                                        /* P5: nur Schwanz */
    }
    /* Ph5-Schwanz @0x80100DCC: Timer, 50%-Steer-Jink, Move, Accel, advance→Sub 4 */
    e->re2d_air219--;                                      /* Dec IMMER @0x80100DCC-D8 */
    if (e->re2d_air219 == 0) e->sub_state_2 = e->re2d_route218;   /* @0x80100DE0-F0 */
    if (re15_re2_rand() & 0x1u)                            /* @0x80100DF4-600 */
        re15_enemy_steer_point(e, pl->x - e->x, pl->z - e->z, 32);/* DELTA als Punkt (authored
                                                            * Jink, byte-true) @0x80100E08-2C */
    else
        re15_enemy_steer_point(e, pl->x, pl->z, 32);       /* @0x80100E30-48 */
    re2c_move3d(e);                                        /* 0x80015350 @0x80100E4C-58 */
    if ((int)e->speed_h < 300) e->speed_h++;               /* +1 bis 300 @0x80100E5C-74 */
    if (re2c_adv(e)) re2c_sub(e, 4);                       /* done → Sub 4 @0x80100E84-98 */
}

/* -- Sub 3 SKRIPT-ABFLUG EXEC 0x80100EC4 (DEC 0x80100EBC = jr ra) --------------------------- */
static void re2c_exec3_script_leave(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 3) { re2c_sub(e, 4); return; }   /* P3: Dispatch-Ziel der Phasen-
                                                            * Weiche (`beq v1,3` @0x80100F18 →
                                                            * jal 0x80104088 a1=4 @0x80101088-8C)
                                                            * — erst im Folge-Tick */
    switch (e->sub_state_2) {
    case 0:                                                /* P0 @0x80100F28 */
        e->re2z_f10e &= (uint16_t)~0x40u;                  /* andi 0xffbf @0x80100F28-38 */
        if (re15_re2_rand() & 0x1u) re2c_se(1);            /* @0x80100F3C-4C */
        re2c_clip(e, 6, (int)(re15_re2_rand() & 0x7u));    /* (r&7)<<8|0x70006 @0x80100F50-6C */
        e->re2d_pause21d = 0;                              /* @0x80100F74 */
        e->speed_h = (int16_t)(60 + (re15_re2_rand() & 0x7u));   /* @0x80100F70-80 */
        e->re2d_air219 = 12;                               /* sb 12,537 @0x80100F84-88 */
        e->re2d_route218 = 1;                              /* sb 1,536 @0x80100F8C-90 */
        e->re2d_vy146 = 0;                                 /* sh zero,326 @0x80100F9C */
        e->re2z_flags21a = 0;                              /* Steer-Rate-Byte +0x21A=0 @0x80100FA0 */
        e->re2c_flags22a |= 0x1u;                          /* @0x80100F94-FB0 */
        e->sub_state_2 = 2;                                /* sb 2,6 @0x80100FA4 */
        break;
    case 1:                                                /* P1 @0x80100FB4 */
        e->flags &= (uint8_t)~0x8u;                        /* word0&=~8 @0x80100FB4-C8 */
        e->re2z_self1d3 &= 0x7fu;                          /* @0x80100FBC-D0 */
        e->speed_h = (int16_t)(80 + (re15_re2_rand() & 0x7u));   /* @0x80100FCC-DC */
        e->re2d_vy146 = 80;                                /* sh 80,326 @0x80100FE0-E4 */
        e->re2d_air219 = 255;                              /* @0x80100FE8-EC */
        e->re2z_flags21a = 32;                             /* Steer-Rate 32 @0x80100FF0-F4 */
        e->sub_state_2 = 2;                                /* @0x80100FF8-FC */
        break;
    default: break;
    }
    /* P2-Schwanz @0x80101000 */
    re15_enemy_steer_point(e, pl->x, pl->z, (int)(e->re2z_flags21a & 0xffu));  /* rate=+0x21A */
    re2c_move3d(e);                                        /* @0x8010101C-28 */
    if ((int)e->speed_h < 300) e->speed_h++;               /* @0x8010102C-44 */
    if (re2c_adv(e)) e->sub_state_2 = 3;                   /* @0x80101048-64 */
    {   /* Review-Fix #11, selbst nachdisassembliert @0x80101068-84: ALT==0-Test (`bne v1,zero
         * → exit`), Dec im Delay-Slot IMMER; bei ALT==0 ueberschreibt +0x218 auch ein frisch
         * gesetztes sub2=3. Der P3-Dispatch (jal 0x80104088 a1=4 @0x80101088-8C) laeuft erst
         * im FOLGE-Tick ueber die Phasen-Weiche (@0x80100F18) — s. Funktions-Kopf. */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);
        if (t == 0) e->sub_state_2 = e->re2d_route218;
    }
}

/* -- Sub 4 KREISEN — DEC 0x801010AC ---------------------------------------------------------- */
static void re2c_dec4_circle(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->ai_dist >= 0x1C21u) {                           /* dist>=7201 sltiu @0x801010C4-C8 */
        if (re15_re2_rand() & 0x1u) re2c_sub(e, 7);        /* @0x801010D0-E8 */
        else                        re2c_sub(e, 8);        /* @0x801010EC-F4 */
    }
    /* KEIN else: der Fern-Zweig FAELLT in den 1/16-Block (jal 0x80104088 @0x801010F0 →
     * sequentiell jal rand @0x801010F8, kein Sprung dazwischen — Review-Fix #1, selbst
     * nachdisassembliert). Der 1/16-Roll laeuft also IMMER (Draws!) und ueberschreibt in
     * 1/16 der Fern-Exits die Sub-7/8-Wahl wieder mit Sub 5/6 (0x80104088 = nackter Setter). */
    if ((re15_re2_rand() & 0xfu) == 0u) {                  /* 1/16 @0x801010F8-104 */
        e->re2z_t158 = (int16_t)(re15_re2_rand() & 0x3fu); /* Dauer @0x8010110C-18 */
        if (re2c_pl_floor_y(pl) - 3600 < e->y) re2c_sub(e, 5);   /* zu tief → steigen @0x8010111C-3C */
        else                                   re2c_sub(e, 6);   /* @0x80101140-4C */
    }
    re2c_flight_dec_tail(e, pl);                           /* @0x80101150-98 */
}
/* -- Sub 4 EXEC 0x801011B0 -------------------------------------------------------------------- */
static void re2c_exec4_circle(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 @0x801011F0 */
        if (re15_re2_rand() & 0x1u) {
            re2c_clip(e, 3, 0);                            /* 0x70003 @0x80101200-08 */
            e->re2d_route218 = 2;                          /* Accel-Step @0x80101210/3C-40 */
        } else {
            re2c_clip(e, 4, 0);                            /* 0x70004 @0x80101214-1C */
            if ((re15_re2_rand() & 0x7u) == 0u) re2c_feather(e, 1);   /* @0x80101220-34 */
            e->re2d_route218 = 3;                          /* @0x80101228-40 */
        }
        e->re2d_air219 = (uint8_t)(re15_re2_rand() & 0x1fu);   /* @0x8010123C-48 */
        e->re2d_vy146 = 0;                                 /* @0x80101250 */
        e->sub_state_2 = 1;                                /* @0x80101254 */
        /* FALLTHRU: P1 laeuft im selben Tick (@0x80101258 fallthrough) */
        /* fall through */
    default:                                               /* P1 @0x8010125C */
        /* Steer auf den NAVIGATOR-Punkt +0x1C4/+0x1C6 (lh 452/454) — MAPPING: Spieler */
        re15_enemy_steer_point(e, pl->x, pl->z, 32);       /* rate 32 @0x8010125C-68 */
        re2c_move3d(e);                                    /* @0x8010126C-78 */
        if ((int)e->speed_h < 300)
            e->speed_h = (int16_t)(e->speed_h + (int)e->re2d_route218);   /* +2/+3 @0x8010127C-A0 */
        re2c_adv(e);                                       /* @0x801012A4-B0 */
        {   /* Review-Fix #2, selbst nachdisassembliert @0x801012B4-C8: `lbu v1,537; addiu
             * v0,v1,255; bne v1,zero,0x801012cc; sb v0,537 (Delay-Slot, IMMER); sb zero,6`
             * — der Re-Roll feuert bei ALT==0 (vor dem Dekrement), nicht bei NEU==0. */
            uint8_t t = e->re2d_air219;
            e->re2d_air219 = (uint8_t)(t - 1);
            if (t == 0) e->sub_state_2 = 0;
        }
        break;
    }
}

/* -- Sub 5 STEIGEN — DEC 0x801012E8 / EXEC 0x8010139C ---------------------------------------- */
static void re2c_dec5_climb(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->y < re2c_pl_floor_y(pl) - 4800) re2c_sub(e, 4); /* Band @0x80101300-18 */
    /* KEIN else (Review-Fix #10): der Band-Zweig FAELLT in den Timer-Block @0x8010131C —
     * das Dekrement (sh im bne-Delay-Slot @0x8010132C) laeuft IMMER, ein 0-Timer ruft
     * sub(4) harmlos doppelt (selbst nachdisassembliert). */
    {
        uint16_t t = (uint16_t)e->re2z_t158;
        e->re2z_t158 = (int16_t)(t - 1);                   /* @0x8010131C-2C */
        if (t == 0) re2c_sub(e, 4);                        /* @0x80101330-38 */
    }
    re2c_flight_dec_tail(e, pl);                           /* @0x8010133C-84 */
}
static void re2c_exec5_climb(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        re2c_clip(e, 4, 0);                                /* 0x70004 @0x801013DC-E4 */
        e->re2d_vy146 = -40;                               /* @0x801013E8-EC */
        e->re2d_air219 = 0;                                /* @0x801013F4 */
        e->sub_state_2 = 1;                                /* @0x801013F0-F8 */
    }
    if (e->re2d_air219 == 0) e->re2d_air219 = 9;           /* Flap-Reload 9 @0x80101400-14 */
    else e->re2d_air219--;
    re15_enemy_steer_point(e, pl->x, pl->z, 32);           /* +0x1C4/6 (MAPPING) @0x80101418-28 */
    re2c_move3d(e);                                        /* @0x8010142C-38 */
    if ((int)e->speed_h < 300) e->speed_h = (int16_t)(e->speed_h + 2);   /* @0x8010143C-54 */
    re2c_adv(e);                                           /* @0x80101458-68 */
}

/* -- Sub 6 SINKEN — DEC 0x80101488 / EXEC 0x8010153C ------------------------------------------ */
static void re2c_dec6_descend(re15_actor_t *e, re15_actor_t *pl)
{
    if (re2c_pl_floor_y(pl) - 1800 < e->y) re2c_sub(e, 4); /* Band @0x801014A0-B8 */
    /* KEIN else (Review-Fix #10, wie dec5): Timer-Block @0x801014BC laeuft UNBEDINGT. */
    {
        uint16_t t = (uint16_t)e->re2z_t158;
        e->re2z_t158 = (int16_t)(t - 1);                   /* @0x801014BC-CC */
        if (t == 0) re2c_sub(e, 4);                        /* @0x801014D0-D8 */
    }
    re2c_flight_dec_tail(e, pl);                           /* @0x801014DC-24 */
}
static void re2c_exec6_descend(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        re2c_clip(e, 3, 0);                                /* 0x70003 @0x8010157C-84 */
        e->re2d_vy146 = 40;                                /* @0x80101588-8C */
        e->re2d_air219 = 0; e->sub_state_2 = 1;            /* @0x80101590-98 */
    }
    if (e->re2d_air219 == 0) e->re2d_air219 = 15;          /* Reload 15 @0x801015A0-B4 */
    else e->re2d_air219--;
    re15_enemy_steer_point(e, pl->x, pl->z, 32);           /* @0x801015B8-C8 */
    re2c_move3d(e);                                        /* @0x801015CC-D8 */
    if ((int)e->speed_h < 300) e->speed_h++;               /* +1 @0x801015DC-F4 */
    re2c_adv(e);                                           /* @0x801015F8-608 */
}

/* -- Sub 7 LANDEN EXEC 0x80101630 (Tabelle @0x8010005C, 7 Phasen; DEC 0x80101628 = jr ra) ---- */
static void re2c_exec7_land(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 Sturzflug @0x8010167C */
        re2c_clip(e, 3, 0);                                /* 0x70003 @0x8010167C-84 */
        e->speed_h = 50;                                   /* @0x80101688-8C */
        e->re2d_vy146 = 120;                               /* @0x80101690-94 */
        e->re2d_pause21d = 0; e->re2d_air219 = 0;          /* @0x8010169C-A0 */
        e->sub_state_2 = 1;                                /* @0x80101698-A4 — FAELLT in P1
                                                            * @0x801016A8 (Review-Fix #5, kein
                                                            * Sprung nach dem sb) */
        /* fall through */
    case 1: {                                              /* P1 @0x801016A8 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* Wrap-Dec @0x801016B0-B8 */
        if (t == 0) {
            if (re15_re2_rand() & 0x1u) re2c_se(1);        /* @0x801016BC-D4 */
            e->re2d_air219 = 15;                           /* Reload @0x801016D8-DC */
        }
        re2c_move3d(e);                                    /* @0x801016E0-704 */
        if (e->y - ((int32_t)e->dog_floor_y - 250) >= -299)/* Flare-Gate slti −299 @0x80101708-1C */
            e->sub_state_2 = 2;                            /* @0x80101724-28 */
        re2c_adv(e);                                       /* @0x8010172C-38 */
        break;
    }
    case 2:                                                /* P2 Flare @0x80101744 */
        re2c_clip(e, 0xA, 0);                              /* 0x7000A @0x80101744-50 */
        if (re15_re2_rand() & 0x1u) re2c_se(1);            /* @0x8010174C-64 */
        e->speed_h = 50; e->re2d_vy146 = 15;               /* @0x80101768-74 */
        e->re2d_air219 = 15; e->re2d_route218 = 3;         /* @0x80101778-84 */
        e->sub_state_2 = 5;                                /* @0x80101788-90 */
        goto p5_tail;
    case 3:                                                /* P3 @0x80101794 */
        if (re15_re2_rand() & 0x1u) re2c_se(1);            /* @0x80101794-AC */
        e->re2d_air219 = 5; e->re2d_route218 = 4;          /* @0x801017B0-BC */
        e->sub_state_2 = 5;                                /* @0x801017C4 */
        goto p5_tail;
    case 4:                                                /* P4 Aufsetzen @0x801017C8 */
        e->speed_h = 10;                                   /* @0x801017C8-CC */
        e->re2d_air219 = 255;                              /* @0x801017D0-D4 */
        e->re2d_vy146 = 0;                                 /* @0x801017E0 */
        e->y = (int32_t)e->dog_floor_y - 250;              /* SNAP @0x801017D8-EC */
        e->sub_state_2 = 5;                                /* @0x801017DC-E4 */
        goto p5_tail;
    case 5:
    p5_tail: {                                             /* P5 @0x801017F0 */
        e->re2d_air219--;                                  /* Dec IMMER @0x801017F0-FC */
        if (e->re2d_air219 == 0) e->sub_state_2 = e->re2d_route218;   /* @0x80101800-14 */
        re2c_move3d(e);                                    /* @0x80101818-38 */
        if ((int32_t)e->dog_floor_y - 250 < e->y)          /* Boden-Klemme @0x80101840-58 */
            e->y = (int32_t)e->dog_floor_y - 250;
        if (re2c_adv(e)) {                                 /* @0x8010185C-6C */
            re2c_clip(e, 8, 0);                            /* Falten 0x70008 @0x80101870-8C */
            e->speed_h = 10;                               /* @0x80101878-7C */
            e->sub_state_2 = 6;                            /* @0x80101880-84 */
        }
        break;
    }
    default: {                                             /* P6 Falten @0x80101890 */
        re15_enemy_steer_point(e, pl->x, pl->z, 96);       /* +0x1C4/6 (MAPPING) rate 96 @0x80101894-A0 */
        re2c_move3d(e);                                    /* @0x801018A4-B0 */
        e->y = (int32_t)e->dog_floor_y - 250;              /* @0x801018C0-D0 */
        if (re2c_adv(e)) {
            /* SELBST GELESEN (Lane-K-Korrektur "Spieler ruhig → Sub 0"): Spieler LEBT ODER
             * Pacifier laeuft → +0x158=rand*2 und SOFORT wieder Sub 2 (Abheben); NUR
             * tot+pacified → Sub 0 sitzen (@0x801018DC-924). */
            if (pl->hp >= 0 || e->re2c_pac221 > 0) {
                e->re2z_t158 = (int16_t)(re15_re2_rand() << 1);   /* sll 1 @0x80101908-14 */
                re2c_sub(e, 2);                            /* @0x80101918-24 */
            } else re2c_sub(e, 0);                         /* @0x80101900-04/20 */
        }
        break;
    }
    }
}

/* -- Sub 8 TRUDELN — DEC 0x80101944 (nur Listener) / EXEC 0x80101964 -------------------------- */
static void re2c_exec8_tumble(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                             /* P0 @0x801019A4 */
        if ((re15_re2_rand() & 0x7u) == 0u) re2c_feather(e, 1);   /* @0x801019A4-C0 */
        re2c_clip(e, 4, 0);                                /* 0x70004 @0x801019C4-D0 */
        e->re2d_air219 = (uint8_t)(re15_re2_rand() & 0x3fu);      /* @0x801019CC-D8 */
        e->re2z_flags21a = 0;                              /* +0x21A @0x801019E0 */
        e->re2d_pause21d = 0;                              /* @0x801019E4 */
        e->re2d_vy146 = 0;                                 /* @0x801019E8 */
        e->sub_state_2 = 1;                                /* @0x801019DC-EC */
    }
    {
        uint8_t t = (uint8_t)e->re2z_flags21a;
        e->re2z_flags21a = (uint16_t)((uint8_t)(t - 1));   /* Wrap-Dec @0x801019F0-A00 */
        if (t == 0) e->re2z_flags21a = 14;                 /* Reload 14 @0x80101A04-08 */
    }
    re15_enemy_steer_point(e, pl->x, pl->z, 96);           /* +0x1C4/6 (MAPPING) @0x80101A0C-1C */
    e->rot_y = (int16_t)(((int)e->rot_y + (int8_t)e->re2d_route218) & 0xfff);
                                                           /* Yaw += (s8)+0x218 @0x80101A2C-44 */
    re2c_move3d(e);                                        /* @0x80101A40 */
    re2c_adv(e);                                           /* @0x80101A48-58 */
    if (re15_ai_arc_test(e, pl->x, pl->z, 96) == 0)        /* Kegel 96 auf den Steuerpunkt
                                                            * (+0x1C4/6, MAPPING Spieler) @0x80101A5C-70 */
        re2c_sub(e, 4);                                    /* @0x80101A90-94 */
    else {
        /* Review-Fix #6, selbst nachdisassembliert @0x80101A78-94: `lbu v1,537; addiu
         * v0,v1,255; bne v1,zero,0x80101a98; sb v0,537 (Delay-Slot); jal 0x80104088 a1=4`
         * — Exit bei ALT==0 (vor dem Dekrement), Dec laeuft immer. */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);
        if (t == 0) re2c_sub(e, 4);
    }
}

/* -- Sub 9 SETZEN — EXEC 0x80101ABC (DEC 0x80101AB4 = jr ra) ---------------------------------- */
static void re2c_exec9_settle(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        re2c_clip(e, 8, 0);                                /* 0x70008 @0x80101AFC-B04 */
        e->sub_state_2 = 1;                                /* @0x80101B08-0C */
    }
    re15_enemy_steer_point(e, pl->x, pl->z, 96);           /* @0x80101B14-20 */
    if (re2c_adv(e)) {                                     /* @0x80101B24-38 */
        e->re2z_t158 = (int16_t)(re15_re2_rand() & 0xfu);  /* Wake-Timer @0x80101B40-4C */
        re2c_sub(e, 0);                                    /* @0x80101B50-58 */
    }
}

/* -- Sub 10 WAND-CRASH — EXEC 0x80101B80 (Tabelle @0x8010007C; DEC 0x80101B78 = jr ra) -------- */
static void re2c_exec10_wallcrash(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 @0x80101BCC */
        re2c_se(3);                                        /* Kreischen @0x80101BCC-D4 */
        re2c_clip(e, 9, 0);                                /* 0x70009 @0x80101BD8-E0 */
        e->re2d_vy146 = 0; e->speed_h = 0;                 /* @0x80101BE8-EC */
        e->re2d_turn224 = 0;                               /* Grav-Akku @0x80101BF0 */
        e->re2d_pause21d = 0;                              /* @0x80101BF4 */
        e->sub_state_2 = 1;                                /* @0x80101BE4-F8 — FAELLT in P1
                                                            * @0x80101BFC (Review-Fix #5) */
        /* fall through */
    case 1:                                                /* P1 @0x80101BFC */
        if (re2c_adv(e)) e->sub_state_2 = 2;               /* @0x80101C08-18 */
        /* fall through */
    case 2: {                                              /* Fall @0x80101C1C (P1+P2) */
        e->re2d_turn224 = (int16_t)(e->re2d_turn224 + 20); /* +20/Tick @0x80101C1C-2C */
        e->y += (int32_t)e->re2d_turn224;                  /* @0x80101C30-3C */
        if ((int32_t)e->dog_floor_y - 250 < e->y) {        /* Boden @0x80101C5C-6C */
            re2c_se(5);                                    /* Thud @0x80101C70-78 */
            e->y = (int32_t)e->dog_floor_y - 250;          /* @0x80101C84 */
            re2c_clip(e, 0xB, 0);                          /* 0x7000B @0x80101C7C-8C */
            e->re2d_air219 = (uint8_t)(re15_re2_rand() & 0xfu);   /* @0x80101C88-94 */
            /* +0x78 (Pitch) = 0 @0x80101C9C — Render-Kanal nicht modelliert (doc) */
            e->sub_state_2 = 3;                            /* @0x80101C98-A4 */
        }
        break;
    }
    case 3: {                                              /* P3 Liegen @0x80101CA8 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x80101CA8-B8 */
        if (t == 0) e->sub_state_2 = 4;                    /* @0x80101CBC-C0 */
        re2c_adv(e);                                       /* @0x80101CC4-D4 */
        if (e->anim_frame) e->anim_frame--;                /* Frame-Hold (lbu 333; −1) @0x80101CD8-E8 */
        break;
    }
    default:                                               /* P4 Aufstehen @0x80101CEC */
        if (re2c_adv(e)) re2c_state(e, 1, 9);              /* 0x80104078(1,9) @0x80101D00-10 */
        break;
    }
}

/* -- Sub 11 ANGRIFFS-ANLAUF — DEC 0x80101D30 -------------------------------------------------- */
static void re2c_dec11_attackrun(re15_actor_t *e, re15_actor_t *pl)
{
    if (re15_ai_arc_test(e, pl->x, pl->z, 96) != 0         /* Kegel 96 @0x80101D3C-58 */
        || !(e->re2c_flags22a & 0x2u))                     /* LOS weg @0x80101D60-6C */
        re2c_sub(e, 4);                                    /* @0x80101D74-78 */
    re2c_abort_listener(e);                                /* @0x80101D7C-80 */
}
/* -- Sub 11 EXEC 0x80101D98 -------------------------------------------------------------------- */
static void re2c_exec11_attackrun(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                             /* P0 @0x80101DD8 */
        re2c_clip(e, 4, 0);                                /* 0x70004 @0x80101DD8-E0 */
        {
            int32_t tgt = re2c_pl_floor_y(pl) - 2700;      /* Angriffshoehe @0x80101DE4-F0 */
            e->re2d_turn224 = (int16_t)tgt;                /* sh v1,548 @0x80101E04 */
            e->re2d_route218 = (uint8_t)((tgt - e->y >= 0) ? 1 : 0);   /* @0x80101DF4-E14 */
        }
        e->re2d_vy146 = 0;                                 /* @0x80101E1C */
        e->re2d_air219 = 0;                                /* @0x80101E20 */
        e->sub_state_2 = 1;                                /* @0x80101E18-24 */
    }
    {
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* Wrap-Dec @0x80101E28-38 */
        if (t == 0) {
            re2c_se(1);                                    /* @0x80101E3C-44 */
            e->re2z_flags21a = 14;                         /* +0x21A=14 @0x80101E48-4C */
        }
    }
    re15_enemy_steer_point(e, pl->x, pl->z, 96);           /* SPIELER rate 96 @0x80101E50-68 */
    if ((int)e->speed_h < 300) e->speed_h = (int16_t)(e->speed_h + 5);   /* +5 @0x80101E6C-84 */
    re2c_adv(e);                                           /* @0x80101E88-98 */
    /* Review-Fix #7, selbst nachdisassembliert @0x80101EAC-E4: reine ±40-Schritte OHNE
     * Klemme auf +0x224 (`slt; beq skip; addiu ±40; sw` — keine weitere Instruktion bis zum
     * Kontakt-Check @0x80101EE8). y ueberschiesst das Ziel um bis zu 39 Einheiten und bleibt
     * dort (die slt-Bedingung greift im Folgetick nicht mehr). Die fruehere Klemme war erfunden. */
    if (e->re2d_route218 == 0) {                           /* steigen @0x80101E9C-A4 */
        if ((int32_t)e->re2d_turn224 < e->y) e->y -= 40;   /* @0x80101EAC-C8 */
    } else {                                               /* sinken @0x80101ECC-E4 */
        if (e->y < (int32_t)e->re2d_turn224) e->y += 40;
    }
    if (!e->crow_contact) {                                /* +0xD!=0 (kein Push) @0x80101EE8-F0 */
        re2c_move3d(e);                                    /* @0x80101F68-70 */
    } else {                                               /* Spieler-Kontakt: Commit-Fenster */
        int32_t d = (int32_t)e->re2d_turn224 - e->y;       /* @0x80101EF8-F04 */
        if (d + 359 >= 0 && d + 359 < 0x2CF) {             /* Fenster ±359 @0x80101F08-10 */
            if ((int)e->speed_h >= 231) re2c_sub(e, 12);   /* STRIKE @0x80101F18-30 */
            else {
                e->re2d_rel220    = (uint8_t)(re15_re2_rand() & 0x7fu);   /* @0x80101F3C-4C */
                e->re2d_budget21f = 0;                     /* aa50 (MAPPING) @0x80101F48-5C */
                re2c_sub(e, 8);                            /* Trudeln @0x80101F50-58 */
            }
        }
    }
}

/* -- Sub 12 STRIKE — EXEC 0x80101FB0 (Tabelle @0x80100094; DEC 0x80101F90 = nur Listener) ----- */
static void re2c_dec12_strike(re15_actor_t *e)
{
    re2c_abort_listener(e);                                /* @0x80101F98 */
}
static void re2c_exec12_strike(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 Kontakt @0x80102000 */
        /* 0x80015910(self, PL) @0x8010200C-14: ret!=0 → Spieler-State (2, Sub 1) Voll-Treffer,
         * ret==0 → (2, Sub 0) Streifer (@0x8010201C-34). Port-Mapping 15910 = Richtungs-
         * Kollaps re15_ai_facing_aligned (Welle-B-Konvention). SCHADENSHOEHE: liegt im
         * EXE-Spieler-State-2-Handler — nicht RE'd, OFFEN (Datei-Kopf): Port setzt nur den
         * Hit-Guard (+0x93|=1-Analog), KEIN erfundener HP-Verlust. */
        (void)re15_ai_facing_aligned(e, pl);               /* Draw-/Aufruf-Paritaet */
        pl->hit_react |= 1;
        /* 0x8003947C(4,0) + 0x80039514(10,150,0) @0x80102038-50: Pad-Aktuator (doc-skip) */
        re2c_se(4);                                        /* Strike-Impact @0x80102054-5C */
        re2c_clip(e, 9, 0);                                /* 0x70009 @0x80102060-68 */
        e->re2d_vy146 = 0; e->speed_h = 0;                 /* @0x80102070-74 */
        e->re2d_bite21e = 0;                               /* sb zero,542 @0x80102078 */
        e->re2d_turn224 = 0;                               /* Grav-Akku @0x8010207C */
        e->sub_state_2 = 1;                                /* @0x8010206C-80 — FAELLT in P1
                                                            * @0x80102084 (Review-Fix #5) */
        /* fall through */
    case 1:                                                /* P1 @0x80102084 */
        if (re2c_adv(e)) e->sub_state_2 = 2;               /* @0x80102090-A0 */
        /* fall through */
    case 2: {                                              /* Fall @0x801020A4 */
        e->re2d_turn224 = (int16_t)(e->re2d_turn224 + 20); /* @0x801020A4-B4 */
        e->y += (int32_t)e->re2d_turn224;                  /* @0x801020B8-C4 */
        if ((int32_t)e->dog_floor_y - 250 < e->y) {        /* @0x801020E4-F4 */
            re2c_se(5);                                    /* @0x801020F8-100 */
            e->y = (int32_t)e->dog_floor_y - 250;          /* @0x8010210C */
            re2c_clip(e, 0xB, 0);                          /* @0x80102104-14 */
            e->re2d_air219 = (uint8_t)(re15_re2_rand() & 0xfu);   /* @0x80102110-1C */
            e->sub_state_2 = 3;                            /* @0x80102120-28 */
        }
        break;
    }
    case 3: {                                              /* P3 Liegen @0x8010212C */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x80102130-3C */
        if (t == 0) e->sub_state_2 = 4;                    /* @0x80102140-44 */
        re2c_adv(e);                                       /* @0x80102148-58 */
        if (e->anim_frame) e->anim_frame--;                /* Frame-Hold @0x8010215C-6C */
        break;
    }
    default:                                               /* P4 @0x80102170 */
        if (re2c_adv(e)) {
            if (re15_ai_arc_test(e, pl->x, pl->z, 1024) == 0)   /* Kegel 1024 (+0x1C4/6-MAPPING)
                                                                 * @0x8010218C-9C */
                re2c_sub(e, 9);                            /* innen → Setzen @0x801021A4-C8 */
            else {
                e->re2z_t158 = (int16_t)(re15_re2_rand() & 0xfu);   /* @0x801021AC-B8 */
                re2c_sub(e, 0);                            /* @0x801021BC-C8 */
            }
        }
        break;
    }
}

/* -- Sub 13 HOVER-HARASS — DEC 0x801021EC ------------------------------------------------------ */
static void re2c_dec13_hover(re15_actor_t *e, re15_actor_t *pl)
{
    if (re15_ai_arc_test(e, pl->x, pl->z, 192) != 0        /* Kegel 192 @0x801021F8-210 */
        || !(e->re2c_flags22a & 0x2u)) {                   /* LOS @0x8010221C-28 */
        re2c_vol(e, 350);                                  /* @0x80102230-34 */
        re2c_sub(e, 4);                                    /* @0x80102238-40 */
    }
    re2c_abort_listener(e);                                /* @0x80102244 */
}
/* -- Sub 13 EXEC 0x80102260 --------------------------------------------------------------------- */
static void re2c_exec13_hover(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                             /* P0 @0x801022A0 */
        re2c_clip(e, 4, 0);                                /* 0x70004 @0x801022A0-A8 */
        {
            int32_t tgt = re2c_pl_floor_y(pl) - 2880;      /* @0x801022AC-B8 */
            e->re2d_turn224 = (int16_t)tgt;                /* sh v1,548 @0x801022CC */
            e->re2d_route218 = (uint8_t)((tgt - e->y >= 0) ? 1 : 0);   /* @0x801022BC-DC */
        }
        e->speed_h = 0; e->re2d_vy146 = 0;                 /* @0x801022E4-E8 */
        /* +0x74/+0x78 (Roll/Pitch) = 0 @0x801022F0-F4 — Render-Kanal (doc) */
        e->re2d_air219 = 0;                                /* @0x801022F8 */
        e->sub_state_2 = 1;                                /* @0x801022E0-FC */
    }
    {
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* Wrap-Dec @0x80102300-10 */
        if (t == 0) { re2c_se(1); e->re2z_flags21a = 14; } /* @0x80102314-24 */
    }
    if (e->re2c_flags22a & 0x4u) {                         /* geclaimter Angreifer @0x80102328-34 */
        re15_enemy_steer_point(e, pl->x, pl->z, 96);       /* rate 96 @0x8010233C-50 */
        if ((int)e->speed_h < 300) e->speed_h = (int16_t)(e->speed_h + 4);   /* +4 @0x80102354-70 */
    } else {
        re15_enemy_steer_point(e, pl->x, pl->z, 384);      /* rate 384 @0x80102374-88 */
        if ((int)e->speed_h < 300) e->speed_h = (int16_t)(e->speed_h + 12);  /* +12 @0x8010238C-A4 */
    }
    re2c_adv(e);                                           /* @0x801023A8-B8 */
    {
        int32_t d = (int32_t)e->re2d_turn224 - e->y;       /* Hoehen-Klemme ±60 @0x801023BC-F0 */
        if (!(d + 60 >= 0 && d + 60 < 0x79)) {
            if (d < -60) e->y -= 60;
            else if (d >= 61) e->y += 60;
        }
    }
    if (e->ai_dist < 0x384u) re2c_vol(e, 100);             /* dist<900 → Vol 100 @0x801023F4-40C */
    if (e->ai_dist < 0x28Au) {                             /* dist<650 @0x80102410-1C */
        int32_t d = (int32_t)e->re2d_turn224 - e->y;       /* Hoehenfenster ±99 @0x80102424-38 */
        if (d + 99 >= 0 && d + 99 < 0xC7)
            re2c_sub(e, 14);                               /* GRAB @0x80102444-48 */
    } else re2c_move3d(e);                                 /* @0x80102454-5C */
}

/* -- Sub 14 GRAB — EXEC 0x8010249C (Tabelle @0x801000AC; DEC 0x8010247C = nur Listener) ------- */
static void re2c_grab_release(re15_actor_t *e, re15_actor_t *pl)
{
    /* @0x80102848-98: Nav-Reseed + Sub 4; geclaimt → Vol 350 + Broadcast 16 + PL+0x6=3
     * (0x800CFBFE = Victim-FSM-Phase 3 = Release) */
    e->re2d_rel220    = (uint8_t)(re15_re2_rand() & 0x7fu);/* @0x80102848-58 */
    e->re2d_budget21f = 0;                                 /* aa50 (MAPPING) @0x80102854-68 */
    re2c_sub(e, 4);                                        /* @0x8010285C-64 */
    if (e->re2c_flags22a & 0x4u) {                         /* @0x8010286C-78 */
        re2c_vol(e, 350);                                  /* @0x80102880-84 */
        re2c_broadcast(16);                                /* @0x80102888-8C */
        if (pl->hp >= 0 && re15_player_victim_state() == 1)
            re15_player_victim_throwoff();                 /* PL+0x6=3 @0x80102890-98 (Port:
                                                            * Victim-Release-Phase; toter Spieler
                                                            * laeuft ueber den Port-Todespfad) */
    }
}
static void re2c_dec14_grab(re15_actor_t *e)
{
    re2c_abort_listener(e);                                /* @0x80102484 */
}
static void re2c_exec14_grab(re15_actor_t *e, re15_actor_t *pl)
{
    /* Prolog @0x801024C4-D8: +0x218 = (PL-HP > 0) ? 1 : 0 — der 0x800401d4-MODE jedes Pecks */
    e->re2d_route218 = (uint8_t)((pl->hp > 0) ? 1 : 0);
    /* Review-Fix #4, selbst nachdisassembliert: P0 endet @0x801025C8 `sb v0,6` und FAELLT in
     * den P1-Body @0x801025CC; P1 endet @0x801026BC `sb 2,6` und FAELLT in den P2-Body
     * @0x801026C0 — der Grab-Eintritts-Tick fuehrt P0+P1+P2 aus (erster 5-HP-Peck inkl. SE/FX/
     * Draws sofort, erste Mash-Dekrementierung im selben Tick). P2/P3/P4 springen mit
     * `j 0x8010289C` raus (kein weiterer Fallthrough). Deshalb Kaskade statt switch. */
    if (e->sub_state_2 == 0) {                             /* P0 @0x80102508 */
        if (pl->hp < 0) { re2c_grab_release(e, pl); goto grab_out; }   /* @0x80102510-18 */
        e->speed_h = 0; e->re2d_vy146 = 0;                 /* @0x80102524-28 */
        /* +0x148/+0x74/+0x78 = 0 @0x8010252C-3C (Render-Kanaele doc) */
        if (e->re2c_flags22a & 0x4u) {                     /* geclaimt @0x80102534-38 */
            re2c_broadcast(8);                             /* @0x80102540-44 */
            re2c_broadcast(64);                            /* @0x80102548-4C */
            /* PL → (5,0) Opfer-FSM @0x80102550-58; Anker 0x800CFDAC/0x800CFD80/84 =
             * self / +0x188 / +0x18C @0x80102560-7C (EXE-Render-Zeiger, doc). Port:
             * Victim-Shim (Clips 0→1-Loop→2 — Hook 0x80104740, Tabelle @0x80100124,
             * selbst gelesen: Ph0 Clip 0 @0x801047B8, Hold Clip 1 @0x801047D8-EC,
             * Release Clip 2 @0x80102844-50, Ende → PL (1,0) @0x80104870-80). */
            re15_re2z_victim_begin(e, pl, 0);              /* Front-Variante (aca59=0-Analog) */
        }
        e->y = (int32_t)e->re2d_turn224;                   /* y-Snap auf Harass-Hoehe @0x80102580-88 */
        re15_enemy_steer_point(e, pl->x, pl->z, 2048);     /* Snap-Turn @0x8010258C-A0 */
        re2c_clip(e, 5, 0);                                /* Krallen 0x70005 @0x801025A4-AC */
        e->re2d_air219 = 100;                              /* GRAB-Timeout 100 @0x801025B0-B4 */
        e->speed_h = 300;                                  /* @0x801025B8-BC */
        e->re2z_flags21a = 0;                              /* +0x21A @0x801025C4 */
        e->sub_state_2 = 1;                                /* @0x801025C0-C8, faellt in P1 */
    }
    if (e->sub_state_2 == 1) {                             /* P1 PECK @0x801025CC */
        if (e->re2c_flags22a & 0x4u) {                     /* @0x801025CC-D8 */
            /* Shake(6,0)+Rumble(4,150,1) @0x801025DC-F4: Pad-Aktuator (doc-skip) */
            /* Blut-FX 0x11000 @ (x, y+500, z), a1=+0x76 (@0x801025F8-262C) → Room-Bank-Blut */
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                                 e->x, e->y + 500, e->z, e->rot_y);
            if ((re15_re2_rand() & 0x3u) == 0u) re2c_feather(e, 1);   /* 1/4 @0x80102630-4C */
            re2c_se(2);                                    /* Picken @0x80102650-58 */
            {
                int r = re15_re2_player_damage_mode(pl, 5, (int)e->re2d_route218);
                                                           /* 0x800401d4(5, +0x218) @0x8010265C-64 */
                if (r == 1) { re2c_grab_release(e, pl); goto grab_out; }   /* @0x80102668-70 */
                if (r == 2) {                              /* Tod @0x80102674-78 */
                    re2c_broadcast(128);                   /* @0x80102680-84 */
                    re2c_grab_release(e, pl); goto grab_out;   /* @0x80102688 */
                }
            }
        } else {
            if ((re15_re2_rand() & 0x7u) == 0u) re2c_feather(e, 1);   /* @0x80102690-AC */
        }
        e->re2c_grab21b = 4;                               /* sb 4,539 @0x801026B0-B4 */
        e->sub_state_2 = 2;                                /* @0x801026B8-BC, faellt in P2 */
    }
    if (e->sub_state_2 == 2) {                             /* P2 Halten/Mash @0x801026C0 */
        re2c_adv(e);                                       /* @0x801026C0-D0 */
        int mash = re15_re2z_mash();                       /* 0x8001598C @0x801026D4 */
        e->re2d_air219 = (uint8_t)(e->re2d_air219 - (uint8_t)(3 * mash + 1));
                                                           /* +0x219 -= 3*mash+1 @0x801026DC-F0 */
        if ((int8_t)e->re2d_air219 < 0) { re2c_grab_release(e, pl); goto grab_out; }   /* @0x801026F4-F8 */
        if (e->re2c_grab21b) e->re2c_grab21b--;            /* @0x80102700-10 */
        else {
            e->re2d_turn224 = 150;                         /* Lift-Betrag @0x80102714-1C */
            {
                uint8_t v = (uint8_t)((re15_re2_rand() & 0x7u) + 2);   /* @0x80102718-2C */
                e->re2z_flags21a = v; e->re2c_grab21b = v;
            }
            e->sub_state_2 = 3;                            /* @0x80102730-38, j 0x8010289C */
        }
    }
    else if (e->sub_state_2 == 3) {                        /* P3 Hochreissen @0x8010273C */
        re2c_adv(e);
        int mash = re15_re2z_mash();
        e->re2d_air219 = (uint8_t)(e->re2d_air219 - (uint8_t)(3 * mash + 1));   /* @0x80102758-6C */
        if ((int8_t)e->re2d_air219 < 0) { re2c_grab_release(e, pl); goto grab_out; }
        e->y -= (int32_t)e->re2d_turn224;                  /* @0x8010277C-8C */
        e->re2d_turn224 = (int16_t)(e->re2d_turn224 - 15); /* @0x80102790-9C */
        {
            uint8_t t = (uint8_t)e->re2z_flags21a;
            e->re2z_flags21a = (uint16_t)((uint8_t)(t - 1));   /* @0x80102794-A8 */
            if (t == 0) e->sub_state_2 = 4;                /* @0x801027AC-B4, j 0x8010289C */
        }
    }
    else {                                                 /* P4 Fallenlassen @0x801027B8 */
        re2c_adv(e);
        int mash = re15_re2z_mash();
        e->re2d_air219 = (uint8_t)(e->re2d_air219 - (uint8_t)(3 * mash + 1));   /* @0x801027D4-E8 */
        if ((int8_t)e->re2d_air219 < 0) { re2c_grab_release(e, pl); goto grab_out; }
        e->re2d_turn224 = (int16_t)(e->re2d_turn224 + 15); /* @0x801027F8-808 */
        e->y += (int32_t)e->re2d_turn224;                  /* @0x8010280C-1C */
        {
            uint8_t t = e->re2c_grab21b;
            e->re2c_grab21b = (uint8_t)(t - 1);            /* @0x80102818-24 */
            if (t == 0) {
                re2c_clip(e, 5, 0);                        /* 0x70005 @0x80102828-30 */
                e->sub_state_2 = 1;                        /* @0x80102834-38, j 0x8010289C
                                                            * (KEINE Rueck-Kaskade in P1 im
                                                            * selben Tick — expliziter Sprung) */
                e->re2z_flags21a = 7;                      /* @0x8010283C-44 */
            }
        }
    }
grab_out:
    /* Victim-Pin haelt pro Frame, solange der Claimer den Spieler haelt (run_all cleart
     * s_player_grabbed jeden Frame — Welle-B-Muster). */
    if ((e->re2c_flags22a & 0x4u) && e->state == 1 && e->sub_state_1 == 14
        && re15_player_victim_state() == 1)
        re15_re2z_player_pin();
}

/* ================================ HURT (state 2/3) ========================================== */

/* Zeilen-Dispatch @0x80104A18 auf +0x5 (selbst gedumpt):
 *   0..4, 13..16, 18 → FLUG-TREFFER 0x80102934
 *   5, 6, 9, 10, 11, 17 → GIB 0x80102CA0
 *   7, 8 → LAUNCH-TREFFER 0x8010327C     12 → BODEN-TREFFER 0x80102FB4 */

/* FLUG-TREFFER 0x80102934 (Phasen-Tabelle @0x801000C4) — mit RECOVERY bei HP>=0! */
static void re2c_hurt_flight(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: {                                              /* P0 @0x80102980 */
        re2c_se(3);                                        /* @0x80102980-88 */
        re2c_clip(e, 9, 0);                                /* 0x70009 @0x8010298C-94 */
        e->re2d_vy146 = 0; e->speed_h = 0;                 /* @0x80102998-9C */
        e->re2d_pause21d = 0;                              /* @0x801029A0 */
        e->re2d_turn224 = 0;                               /* Grav-Akku @0x801029A4 */
        /* Knock-Richtung: 0x800154AC(PL, self) − Yaw → +0x226 (@0x801029A8-D4) */
        {
            int bear = ((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff;
            e->re2c_snap226 = (int16_t)(bear - (int)e->rot_y);
        }
        e->speed_h = 100;                                  /* sh 100,324 auf [0x800CE330]=SELF
                                                            * (Current-Entity-Global — Lane-K-
                                                            * "Attacker-Write" aufgeloest)
                                                            * @0x801029D8-E8 */
        e->re2d_offx228 = (int16_t)((re15_re2_rand() & 0x1u) ? 200 : -200);   /* Spin @0x801029E4-FC */
        re2c_featherburst(e);                              /* 0x8010459C @0x80102A00-04 */
        re2c_feather(e, (int)(re15_re2_rand() & 0x1u) | 0x2);   /* 2..3 Federn @0x80102A08-1C */
        e->re2d_air219 = 3; e->re2z_flags21a = 3;          /* @0x80102A20-28 */
        e->sub_state_2 = 1;                                /* @0x80102A2C-30 — FAELLT in P1
                                                            * @0x80102A34 (Review-Fix #5) */
    }
        /* fall through */
    case 1:                                                /* P1 Spiralsturz @0x80102A34 */
        if (re2c_adv(e)) e->sub_state_2 = 2;               /* @0x80102A40-50 */
        /* fall through */
    case 2: {                                              /* Sturz @0x80102A54 */
        {
            uint8_t t = e->re2d_air219;
            e->re2d_air219 = (uint8_t)(t - 1);             /* @0x80102A54-64 */
            if (t == 0) re2c_feather(e, 1);                /* @0x80102A68-70 */
        }
        {
            uint8_t t = (uint8_t)e->re2z_flags21a;
            e->re2z_flags21a = (uint16_t)((uint8_t)(t - 1));   /* @0x80102A74-84 */
            if (t == 0) re2c_feather(e, 1);                /* @0x80102A88-90 */
        }
        e->re2d_turn224 = (int16_t)(e->re2d_turn224 + 20); /* Gravitation @0x80102A94-A4 */
        e->y += (int32_t)e->re2d_turn224;                  /* @0x80102AA8-B4 */
        re2c_move(e, (int)e->re2c_snap226);                /* 0x800152C8(self, +0x226) @0x80102A98-C4 */
        /* Spiral-Spin ±200 Clamp ±2047 auf +0x78 (PITCH, lh/sh 120 @0x80102AC8-B10) —
         * Render-Kanal nicht modelliert (doc; Barrel-Roll rein visuell) */
        if ((int32_t)e->dog_floor_y - 250 < e->y) {        /* Boden @0x80102B38-48 */
            re2c_se(5);                                    /* @0x80102B4C-54 */
            e->y = (int32_t)e->dog_floor_y - 250;          /* @0x80102B60 */
            re2c_clip(e, 0xB, 0);                          /* 0x7000B @0x80102B58-68 */
            e->re2d_air219 = (uint8_t)(re15_re2_rand() & 0xfu);   /* @0x80102B64-74 */
            if (e->hp < 0) {                               /* lh 342 @0x80102B6C-78 */
                e->re2d_air219 = 11;                       /* @0x80102B84 */
                /* 0x80018FB0 Kill-Buchhaltung (EXE) @0x80102B88 — kein Port-Pendant (doc) */
                e->sub_state_2 = 5;                        /* @0x80102B90-98 */
            } else e->sub_state_2 = 3;                     /* RECOVERY-Pfad @0x80102B9C-A4 */
        }
        break;
    }
    case 3: {                                              /* P3 Liegen @0x80102BA8 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x80102BA8-B8 */
        if (t == 0) e->sub_state_2 = 4;                    /* @0x80102BBC-C0 */
        re2c_adv(e);                                       /* @0x80102BC4-D4 */
        if (e->anim_frame) e->anim_frame--;                /* Frame-Hold @0x80102BD8-E8 */
        break;
    }
    case 4:                                                /* P4 ERHOLUNG @0x80102BEC — die Kraehe
                                                            * fliegt bei HP>=0 wieder! */
        if (re2c_adv(e)) {
            if (re15_ai_arc_test(e, pl->x, pl->z, 1024) != 0)   /* @0x80102C08-18 */
                re2c_state(e, 1, 9);                       /* aussen → Setzen @0x80102C20-28/7C */
            else {
                e->re2z_t158 = (int16_t)(re15_re2_rand() & 0xfu);   /* @0x80102C2C-38 */
                re2c_state(e, 1, 0);                       /* innen → Idle @0x80102C3C-44/78 */
            }
        }
        break;
    default: {                                             /* P5 tot @0x80102C48 */
        re2c_adv(e);                                       /* @0x80102C48-58 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x80102C5C-6C */
        if (t == 0) re2c_state(e, 7, 0);                   /* CORPSE @0x80102C70-7C */
        break;
    }
    }
}

/* GIB 0x80102CA0: 13 Part-Zeilen ([+0x198]+i*172) Wurf 0x80102EF4 / Wirbel 0x80102F34 nach
 * rand-Maske (Bit1→Zeilen 0..2, Bit2→3..5, Bit4→6..8, Zeilen 9/10 IMMER Wurf, Bit8→11,
 * Bit16→12; @0x80102CF4-E90); Wurf = Speed 800/vy −100 (0x80102EF4). Part-Mover nicht
 * modelliert → crow_hide + Stand-in-FX (Datei-Kopf). */
/* Wirbel-Helfer 0x80102F34 (Review-Fix #8, selbst nachdisassembliert): zieht EXAKT 3 Draws
 * (jal 0x80015fe8 @0x80102F54, @0x80102F68 [&0x7f+100], @0x80102F88 [<<4]); der Wurf-Helfer
 * 0x80102EF4 ist RNG-frei (nur Stores, Speed 800/vy −100). Der Part-Mover selbst ist
 * Render-OPEN — aber die DRAW-ZAHL ist Verhalten am geteilten RE2-Strom. */
static void re2c_gib_swirl_draws(void)
{
    (void)re15_re2_rand(); (void)re15_re2_rand(); (void)re15_re2_rand();
}
static void re2c_hurt_gib(re15_actor_t *e, re15_actor_t *pl)
{
    e->re2d_vy146 = 0;                                     /* sh zero,326 @0x80102CB8 */
    (void)re15_atan2_q12(e->z - pl->z, e->x - pl->x);      /* Bearing-Draw 0x800154AC @0x80102CD4 */
    {   /* Scatter-Maske @0x80102CDC; pro Mask-Bit-CLEAR WIRBELN die Zeilen (beq → 0x80102F34):
         * Bit1→Zeilen 1,2 (Zeile 0 nur bei SET geworfen), Bit2→3..5, Bit4→6..8, Zeilen 9/10
         * IMMER Wurf, Bit8→11, Bit16→12 (@0x80102CF4-E90 selbst gelesen). */
        uint32_t m = re15_re2_rand();
        if (!(m & 0x01u)) { re2c_gib_swirl_draws(); re2c_gib_swirl_draws(); }               /* 1,2   */
        if (!(m & 0x02u)) { re2c_gib_swirl_draws(); re2c_gib_swirl_draws(); re2c_gib_swirl_draws(); }  /* 3..5 */
        if (!(m & 0x04u)) { re2c_gib_swirl_draws(); re2c_gib_swirl_draws(); re2c_gib_swirl_draws(); }  /* 6..8 */
        if (!(m & 0x08u)) re2c_gib_swirl_draws();                                           /* 11    */
        if (!(m & 0x10u)) re2c_gib_swirl_draws();                                           /* 12    */
    }
    e->crow_hide = 1;                                      /* Part-Scatter-Stand-in (RE1.5-GIB-Muster) */
    re15_esp_fx_splatter(re15_esp_room_bank(), 0, 13, e->x, e->y, e->z,
                         (int32_t)e->dog_floor_y);         /* 13 Zeilen → 13 Kinder-Stand-in */
    re2c_featherburst(e); re2c_featherburst(e); re2c_featherburst(e);   /* 3× @0x80102E90-A4 */
    re2c_feather(e, (int)(re15_re2_rand() & 0x3u) | 0x8);  /* 8..11 Federn @0x80102EA8-BC */
    /* 0x80018FB0 @0x80102EC0 (EXE-Kill, doc) */
    e->hp = -1;                                            /* Port-Konvention (Corpse-Gate) */
    re2c_state(e, 7, 1);                                   /* CORPSE Sub 1 (KEINE Lache) @0x80102EC8-D4 */
}

/* BODEN-TREFFER 0x80102FB4 (Zeile 12; Tabelle @0x801000DC) — SOFORT-KILL + Rutscher/Wand-Splat. */
static void re2c_hurt_ground(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 @0x80103000 */
        /* 0x80018FB0 @0x80103000 (doc) */
        re2c_se(3);                                        /* @0x80103008-10 */
        re2c_vol(e, 100);                                  /* @0x80103014-1C */
        e->hp = -1;                                        /* sh −1,342 @0x80103024-2C */
        re2c_featherburst(e);                              /* @0x80103028 */
        re2c_feather(e, (int)(re15_re2_rand() & 0x3u) + 3);/* 3..6 @0x80103030-44 */
        re2c_clip(e, 9, 0);                                /* @0x80103048-54 */
        e->speed_h = (int16_t)(e->speed_h >> 1);           /* sra 17 @0x8010304C-74 */
        e->sub_state_2 = 1;                                /* @0x80103058-5C — FAELLT (nach der
                                                            * Halbierung) in P1 @0x8010307C
                                                            * (Review-Fix #5) */
        /* fall through */
    case 1:                                                /* P1 @0x8010307C */
        if (re2c_adv(e)) e->sub_state_2 = 2;               /* @0x80103088-98 */
        /* fall through */
    case 2: {                                              /* Roh-Vektor-Rutscher @0x8010309C */
        e->x += (int32_t)e->speed_h;                       /* +0x144 UNROTIERT @0x8010309C-C0
                                                            * (byte-true Skalar/Vektor-Aliasing) */
        e->y += (int32_t)e->re2d_vy146;                    /* @0x801030A4-B8 */
        e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 20);     /* @0x801030BC-CC */
        if ((int32_t)e->dog_floor_y - 250 < e->y) {        /* @0x801030D0-E8 */
            e->re2d_air219 = 8;                            /* @0x801030EC-F0 */
            e->y = (int32_t)e->dog_floor_y - 250;          /* @0x801030F8 */
            e->sub_state_2 = 3;                            /* @0x801030F4-100 */
        }
        if (e->crow_wall & 1) {                            /* +0x110&1 @0x80103104-10 */
            /* Wand-Splat: Roll/Pitch-Zufall (word0&0x4000-Fork @0x80103118-7C, Render-doc),
             * Clip 4 Frame 3 (@0x80103190-A0) → CORPSE Sub 2 */
            (void)re15_re2_rand(); (void)re15_re2_rand();  /* Winkel-Draws @0x8010312C-70 */
            e->motion = 4; e->anim_frame = 3;              /* sw 4,332; sb 3,333 @0x80103190-A0 */
            re2c_adv(e);                                   /* @0x8010319C */
            re2c_state(e, 7, 2);                           /* @0x801031A4-AC */
        }
        break;
    }
    case 3: {                                              /* P3 Auslauf @0x801031B4 */
        e->x += (int32_t)e->speed_h;                       /* @0x801031B4-C4 */
        e->speed_h = (int16_t)(e->speed_h >> 1);           /* Halbierung @0x801031CC-E4 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x801031E8-200 */
        if (t == 0) {
            re2c_clip(e, 0xB, 0);                          /* @0x80103204-0C */
            e->re2d_air219 = 11;                           /* @0x80103210-14 */
            e->sub_state_2 = 4;                            /* @0x80103218-20 */
        }
        break;
    }
    default: {                                             /* P4 @0x80103224 */
        re2c_adv(e);                                       /* @0x80103224-34 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x80103238-48 */
        if (t == 0) re2c_state(e, 7, 0);                   /* @0x8010324C-58 */
        break;
    }
    }
}

/* LAUNCH-TREFFER 0x8010327C (Zeilen 7/8; Tabelle @0x801000F4) — Sofort-Kill, Start mit
 * Speed 600 in SPIELER-Blickrichtung (0x800CFC6E), Wand-Bounce Yaw+2048. */
static void re2c_hurt_launch(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                /* P0 @0x801032C8 */
        /* 0x80018FB0 @0x801032C8 (doc) */
        re2c_se(3);                                        /* @0x801032D0-D8 */
        re2c_vol(e, 10);                                   /* @0x801032DC-E4 */
        e->hp = -1;                                        /* @0x801032EC-F4 */
        re2c_featherburst(e);                              /* @0x801032F0 */
        re2c_feather(e, (int)(re15_re2_rand() & 0x3u) + 5);/* 5..8 @0x801032F8-30C */
        e->speed_h = 600;                                  /* @0x80103320-24 */
        re2c_clip(e, 9, 0);                                /* @0x80103310-2C */
        e->rot_y = pl->rot_y;                              /* Spieler-Yaw 0x800CFC6E @0x80103318-34 */
        e->sub_state_2 = 1;                                /* @0x80103328-30 — FAELLT in P1
                                                            * @0x80103338 (Review-Fix #5) */
        /* fall through */
    case 1:                                                /* P1 Flug @0x80103338 */
        if (re2c_adv(e)) e->sub_state_2 = 2;               /* @0x80103344-54 */
        /* fall through */
    case 2: {
        re2c_move(e, 0);                                   /* 0x800152C8(0) @0x80103358-64 */
        e->y += (int32_t)e->re2d_vy146;                    /* @0x80103368-7C */
        e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 20);     /* @0x80103380-94 */
        if ((int32_t)e->dog_floor_y - 250 < e->y) {        /* @0x8010338C-90 */
            re2c_se(5);                                    /* @0x80103398-A0 */
            e->re2d_air219 = 16;                           /* @0x801033A4-A8 */
            e->y = (int32_t)e->dog_floor_y - 250;          /* @0x801033B0 */
            e->re2d_vy146 = 0;                             /* @0x801033B4 */
            e->sub_state_2 = 3;                            /* @0x801033AC-B8 */
        }
        if (e->crow_wall & 1) {                            /* Wand @0x801033BC-C8 */
            re2c_se(3);                                    /* @0x801033CC-D4 */
            re2c_featherburst(e);                          /* @0x801033D8 */
            e->speed_h = 20;                               /* @0x801033E0-E4 */
            e->re2d_vy146 = 0;                             /* @0x801033F0 */
            e->sub_state_2 = 4;                            /* @0x801033EC-F4 */
            e->rot_y = (int16_t)(((int)e->rot_y + 2048) & 0xfff);   /* Bounce @0x801033E8-400 */
        }
        break;
    }
    case 3: {                                              /* P3 Auslauf @0x80103404 */
        re2c_move(e, 0);                                   /* @0x80103404-10 */
        e->speed_h = (int16_t)(e->speed_h >> 1);           /* @0x80103414-24 */
        uint8_t t = e->re2d_air219;
        e->re2d_air219 = (uint8_t)(t - 1);                 /* @0x80103418-30 */
        if (t == 0) re2c_state(e, 7, 3);                   /* CORPSE Sub 3 @0x80103434-40 */
        break;
    }
    default:                                               /* P4 Wand-Abfall @0x80103444 */
        e->y += (int32_t)e->re2d_vy146;                    /* @0x80103454-64 */
        e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 20);     /* @0x80103458-6C */
        re2c_move(e, 0);                                   /* @0x80103444-68 */
        if ((int32_t)e->dog_floor_y - 250 < e->y) {        /* @0x80103470-80 */
            re2c_vol(e, 350);                              /* @0x80103484-8C */
            re2c_se(5);                                    /* @0x80103490-98 */
            e->y = (int32_t)e->dog_floor_y - 250;          /* @0x801034A8 */
            e->re2d_vy146 = 0;                             /* @0x801034AC */
            re2c_state(e, 7, 3);                           /* @0x8010349C-B4 */
        }
        break;
    }
}

/* HURT-Router 0x801028BC: Navigator-Draw (MAPPING doc) + Zeilen-Dispatch auf +0x5.
 * ⚠️ ROW-SEMANTIK (Welle-D-Nachtrag, selbst belegt): +0x5 wird von BEIDEN RE2-Waffen-Appliern
 * VOR dem HURT-Tick ueberschrieben (Projektil `sb s5,5` = RE2-WAFFEN-Low-Byte @0x80047324/74,
 * Hitscan `sb hi16+1,5` @0x80041AB4) — die 19 Zeilen @0x80104A18 sind also TREFFER-CODES
 * (GIB bei den Projektil-Ids 9/10/11/17 = GL-Runden/Rocket usw.), NICHT der Herkunfts-Sub.
 * Der Port hat kein RE2-Waffenarsenal und keinen Hitscan-Code-Producer → deklariertes
 * MAPPING: Herkunfts-Sub (re2z_prev_sub) als Zeile; das RE1.5-Waffe→RE2-Zeile-Mapping ist
 * eine offene Folge-Lane (RE15_RE2_AI.md OFFEN — betrifft ebenso den Welle-C-Hunde-Router). */
static void re2c_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    uint8_t row = e->re2z_prev_sub;                        /* MAPPING, s.o. */
    switch (row) {
    case 5: case 6: case 9: case 10: case 11: case 17:
        re2c_hurt_gib(e, pl); break;                       /* @0x80104A2C-30/3C-44/5C */
    case 7: case 8:
        re2c_hurt_launch(e, pl); break;                    /* @0x80104A34-38 */
    case 12:
        re2c_hurt_ground(e); break;                        /* @0x80104A48 */
    default:
        re2c_hurt_flight(e, pl); break;                    /* @0x80104A18-28/4C-58/60 */
    }
}

/* ================================ CORPSE (state 7) ========================================== */
/* Tabelle @0x80104A70 auf +0x5: [0] 0x8010398C Normal (Lache+Zucken), [1] 0x80103C0C GIB,
 * [2] 0x80103C44 Wand-Splat, [3] 0x80103EB0 Launch, [4] 0x8010477C (tote Zeile — kein
 * Schreiber setzt Corpse-Sub 4; selbst geprueft: nur (7,0)/(7,1)/(7,2)/(7,3) existieren). */
static void re2c_corpse(re15_actor_t *e)
{
    switch (e->sub_state_1) {
    case 0:                                                /* NORMAL 0x8010398C */
        switch (e->sub_state_2) {
        case 0: {                                          /* Ph0 @0x801039E8 */
            re2c_clip(e, 0xB, 0);                          /* 0x7000B @0x801039C8/E8 */
            e->re2z_self1d3 = 128;                         /* sb 128,467 @0x801039F0-F4 */
            e->flags |= 0x2;                               /* word0|=2 @0x801039EC-A04 */
            {
                uint8_t v = (uint8_t)((re15_re2_rand() & 0x1fu) + 25);   /* @0x80103A00-1C */
                e->re2d_air219 = v;                        /* sb v,537 @0x80103A1C */
                e->re2z_t158   = (int16_t)v;               /* sh v,344 (Delay-Slot) @0x80103A24 */
            }
            /* Modell-Tint 0x80016FE4([+0x16C], 0xBFBF10) @0x80103A08-20 — Render-OFFEN (doc) */
            e->crow_shadow_w = 400; e->crow_shadow_h = 400;/* Lache-Basis [+0x16C]+4/+6
                                                            * @0x80103A28-3C */
            e->re2z_t15a = 60;                             /* Grow-Ticks @0x80103A40-44 */
            e->sub_state_2 = 1;                            /* @0x80103A48-4C — FAELLT in Ph1
                                                            * @0x80103A50 (Review-Fix #5) */
        }
            /* fall through */
        case 1: {                                          /* Ph1 Warten+Lache @0x80103A50 */
            uint8_t t = e->re2d_air219;
            e->re2d_air219 = (uint8_t)(t - 1);             /* @0x80103A50-60 */
            if (t == 0) {                                  /* Zuck-Entscheid @0x80103A64-B4 */
                if ((int)e->re2z_t158 >= 91) {             /* slti 91 @0x80103A6C-70 */
                    e->anim_frame = 11;                    /* sb 11,333 @0x80103A74-78 */
                    e->re2d_air219 = 6;                    /* @0x80103A80/AC */
                } else if (re15_re2_rand() & 0x1u) {       /* @0x80103A84-90 */
                    e->anim_frame = 0;                     /* @0x80103A9C */
                    e->re2d_air219 = 12;                   /* @0x80103A94-AC */
                } else {
                    e->anim_frame = 11;                    /* @0x80103AA0-A4 */
                    e->re2d_air219 = 7;                    /* @0x80103AA8-AC */
                }
                e->sub_state_2 = 2;                        /* @0x80103AB0-B4 */
            }
            if (e->re2z_t15a != 0) {                       /* Lache-Grower @0x80103AB8-FC */
                e->re2z_t15a--;
                e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + 10);
                e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + 10);
            }
            break;
        }
        case 2: {                                          /* Ph2 ZUCKEN @0x80103B00 */
            int run = 1;
            if ((int)e->re2z_t158 >= 91) {                 /* @0x80103B00-0C */
                uint8_t g = (uint8_t)(e->re2z_flags21a + 1);
                e->re2z_flags21a = g;                      /* +0x21A-Toggle @0x80103B14-28 */
                run = !(g & 1);                            /* nur jeder 2. Tick */
            }
            if (run) {
                if (re2c_frame_slot(e) == 0                /* @0x80103B34-3C */
                    && (re15_re2_rand() & 0x1u)            /* @0x80103B44-50 */
                    && (int)e->re2z_t158 < 90)             /* @0x80103B58-64 */
                    re2c_se(3);                            /* Zuck-Kreischen @0x80103B68-70 */
                re2c_adv(e);                               /* @0x80103B78-84 */
                e->re2d_air219--;                          /* @0x80103B88-94 */
            }
            if (e->re2d_air219 == 0) {                     /* @0x80103B98-A0 */
                uint16_t n = (uint16_t)(e->re2z_t158 + 10 + (re15_re2_rand() & 0xfu));
                e->re2z_t158 = (int16_t)n;                 /* Eskalation @0x80103BA8-C8 */
                if ((int16_t)n >= 151) e->sub_state_2 = 3; /* Ende @0x80103BD0-E0 */
                else { e->re2d_air219 = (uint8_t)n; e->sub_state_2 = 1; }   /* @0x80103BE4-EC */
            }
            break;
        }
        default: break;                                    /* Ph3 terminal */
        }
        break;
    case 1:                                                /* GIB-Leiche 0x80103C0C */
        if (e->sub_state_2 == 0) {
            e->re2z_self1d3 = 128;                         /* @0x80103C18-1C */
            e->flags |= 0x2;                               /* @0x80103C20-2C */
            e->crow_shadow_w = 0; e->crow_shadow_h = 0;    /* Schatten-Disable [+0x16C]+0xE=0
                                                            * @0x80103C34 (Port: 0-Groesse) */
            e->crow_tint = 0;
            e->sub_state_2 = 1;                            /* @0x80103C30-38 */
        }
        break;
    case 2:                                                /* WAND-SPLAT 0x80103C44 */
        switch (e->sub_state_2) {
        case 0: {                                          /* Ph0 @0x80103C84 */
            e->re2z_self1d3 = 128;                         /* @0x80103C84-88 */
            e->flags |= 0x2;                               /* @0x80103C8C-A0 */
            {
                uint8_t v = (uint8_t)((re15_re2_rand() & 0x1fu) + 25);   /* @0x80103C9C-B4 */
                e->re2d_air219 = v; e->re2z_t158 = (int16_t)v;
            }
            e->crow_shadow_w = 0; e->crow_shadow_h = 0;    /* Disable @0x80103CBC */
            e->crow_tint = 0;
            e->re2z_t15a = 60;                             /* @0x80103CB8-C0 */
            e->sub_state_2 = 1;                            /* @0x80103C94/C4 — FAELLT in Ph1
                                                            * @0x80103CC8 (Review-Fix #5) */
        }
            /* fall through */
        case 1: {                                          /* Ph1 @0x80103CC8 */
            uint8_t t = e->re2d_air219;
            e->re2d_air219 = (uint8_t)(t - 1);             /* @0x80103CC8-D8 */
            if (t == 0) {
                re2c_clip(e, 4, 0);                        /* Flatter-Zucken 0x70004 @0x80103CDC-F0 */
                if ((int)e->re2z_t158 >= 121)              /* slti 121 @0x80103CE8-EC */
                    e->re2d_air219 = (uint8_t)((re15_re2_rand() & 0x1u) * 5 + 10);   /* @0x80103CF4-28 */
                else
                    e->re2d_air219 = (uint8_t)((re15_re2_rand() & 0x7u) * 5 + 5);    /* @0x80103D10-28 */
                e->sub_state_2 = 2;                        /* @0x80103D2C-30 */
            }
            break;
        }
        case 2: {                                          /* Ph2 @0x80103D34 */
            int run = 1;
            if ((int)e->re2z_t158 >= 101) {                /* @0x80103D34-44 */
                uint8_t g = (uint8_t)(e->re2z_flags21a + 1);
                e->re2z_flags21a = g;                      /* @0x80103D48-5C */
                run = !(g & 1);
            }
            if (run) {
                re2c_adv(e);                               /* @0x80103D68-6C */
                if ((int)e->re2z_t158 < 100) e->anim_frame++;   /* Doppel-Rate @0x80103D70-90 */
                e->re2d_air219--;                          /* @0x80103D94-A0 */
            }
            if (e->re2d_air219 == 0) {                     /* @0x80103DA4-AC */
                uint16_t n = (uint16_t)(e->re2z_t158 + 10 + (re15_re2_rand() & 0xfu));
                e->re2z_t158 = (int16_t)n;                 /* @0x80103DB4-D0 */
                if ((int16_t)n >= 151) { e->sub_state_2 = 3; e->re2d_vy146 = 0; }   /* @0x80103DD8-F0 */
                else { e->re2d_air219 = (uint8_t)n; e->sub_state_2 = 1; }           /* @0x80103DF4-FC */
            }
            break;
        }
        case 3:                                            /* Ph3 Abfallen @0x80103E00 */
            e->y += (int32_t)e->re2d_vy146;                /* @0x80103E00-14 */
            e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 20); /* @0x80103E18-2C */
            if ((int32_t)e->dog_floor_y - 250 < e->y) {    /* @0x80103E24-28 */
                re2c_vol(e, 350);                          /* @0x80103E30-38 */
                re2c_se(5);                                /* @0x80103E3C-44 */
                re2c_clip(e, 0xB, 0);                      /* @0x80103E48-50 */
                e->re2d_air219 = 11;                       /* @0x80103E54-58 */
                e->y = (int32_t)e->dog_floor_y - 250;      /* @0x80103E6C */
                e->sub_state_2 = 4;                        /* @0x80103E5C/94 */
            }
            break;
        case 4: {                                          /* Ph4 @0x80103E70 */
            re2c_adv(e);                                   /* @0x80103E70-78 */
            uint8_t t = e->re2d_air219;
            e->re2d_air219 = (uint8_t)(t - 1);             /* @0x80103E7C-8C */
            if (t == 0) e->sub_state_2 = 5;                /* @0x80103E90-94 */
            break;
        }
        default: break;                                    /* Ph5 terminal */
        }
        break;
    case 3:                                                /* LAUNCH-Leiche 0x80103EB0 */
        if (e->sub_state_2 == 0) {                         /* Ph0 @0x80103EE8 */
            /* Tint 0xBFBF10 @0x80103EEC-F18 (doc) */
            e->re2z_self1d3 = 128;                         /* @0x80103F04-08 */
            re2c_clip(e, 0xB, 0);                          /* @0x80103EE8-F0C */
            e->flags |= 0x2;                               /* @0x80103F10-18 */
            e->crow_shadow_w = 400; e->crow_shadow_h = 400;/* @0x80103F1C-30 */
            e->re2z_t15a = 60;                             /* @0x80103F34-38 */
            e->re2d_air219 = 11;                           /* @0x80103F3C-40 */
            e->sub_state_2 = 1;                            /* @0x80103F44-48 — FAELLT in Ph1
                                                            * @0x80103F4C (Review-Fix #5) */
        }
        if (e->sub_state_2 == 1) {                         /* Ph1 @0x80103F4C */
            if (e->re2d_air219 != 0) {
                e->re2d_air219--;                          /* @0x80103F50-5C */
                re2c_adv(e);                               /* @0x80103F60-70 */
            }
            if (e->re2z_t15a != 0) {                       /* Grower @0x80103F74-B8 */
                e->re2z_t15a--;
                e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + 10);
                e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + 10);
            } else e->sub_state_2 = 2;                     /* @0x80103FBC-C0 */
        }
        break;
    default: break;                                        /* Sub 4: tote Zeile @0x80104A80 */
    }
}

/* ================================ INIT (state 0) ============================================ */
static void re2c_init(re15_actor_t *e)
{
    re15_ai_set_state_word(e, 0x1);                        /* sw 1,4 @0x8010032C — WORD-Write
                                                            * cleart +0x5/+0x6/+0x7 */
    e->hp = 10;                                            /* sh 10,342 @0x80100324/348 */
    e->speed_h = 0; e->re2d_vy146 = 0;                     /* +0x144/146/148 @0x8010033C-44 */
    /* Block-Clear +0x218..+0x234 (sw zero @0x8010034C-368) + +0x1D4..+0x1DA (@0x8010036C-78): */
    e->re2d_route218 = 0; e->re2d_air219 = 0; e->re2z_flags21a = 0;
    e->re2c_grab21b = 0; e->re2d_abort21c = 0; e->re2d_pause21d = 0;
    e->re2d_bite21e = 0; e->re2d_budget21f = 0; e->re2d_rel220 = 0;
    e->re2c_pac221 = 0; e->re2d_turn224 = 0; e->re2c_snap226 = 0;
    e->re2d_offx228 = 0; e->re2c_flags22a = 0;
    e->re2z_t158 = 0; e->re2z_t15a = 0;
    e->re2z_self1d3 = 0; e->re2z_prev_sub = 0; e->re2z_prev_hp = 10;
    e->crow_hide = 0; e->crow_contact = 0; e->crow_wall = 0;
    /* Anim-Wort = 0 (Clip 0 Frame 0) + EIN Advancer-Tick (sw zero,332; jal 0x8002959c a3=512
     * @0x8010037C-80): */
    re2c_clip(e, 0, 0);
    re2c_adv(e);
    re2c_vol(e, 350);                                      /* 0x8010472C(350) @0x80100388/3D8 */
    /* Hitbox-Record [+0x198]: {96, 96, 512, 312} @0x801003A0-B4, Offsets 0 @0x801003BC-C0.
     * Port: Radius 96/96 + Hoehe 512 in die Damage-Felder; der vierte Wert 312 ([rec]+506)
     * ist im Record-Layout nicht aufgeloest (OFFEN, doc). */
    e->hit_radius_min = 96; e->hit_radius_max = 96; e->hit_height = 512;
    e->hit_offset_x = 0; e->hit_offset_y = 0; e->hit_offset_z = 0;
    e->target_z = -350;                                    /* +0x98=−350 @0x801003B8-C4 (Render-
                                                            * Pitch-Kanal OFFEN, doc) */
    /* +0x9E=530 (@0x801003C8/DC), +0x1E8=1 (@0x801003CC), +0x94/+0x96=0 (@0x801003D0-D4),
     * +0x1EE=300 (@0x801003F4-FC): Leser offen (Lane K §5) — nicht modelliert (doc). */
    /* Boden-Probe 0x8004FBA0(&pos, 250, 1024, 0) → +0x1C2 (@0x801003E0-414). MAPPING:
     * Spawn-Y als Boden (Welle-C-Hunde-Muster; die EXE-Probe ist nicht portiert). */
    e->dog_floor_y = (int16_t)e->y;
    /* Schatten-Init 0x80016480(+0x16C, 0, 0xC800C8, 0) = 200×200 (@0x80100400-418): */
    e->crow_shadow_w = 200; e->crow_shadow_h = 200;
    e->crow_tint = 128;                                    /* PORT-Praesentations-Shim: der
                                                            * RE2-Schattenrenderer ist EXE-seitig
                                                            * (Farbe OFFEN); ohne Tint zeichnete
                                                            * der Port-Schatten schwarz/leer */
    e->crow_pool = 0;
    /* word0 |= 0x8000000 @0x80100420-438 (Engine-Bit, doc). */
    /* +0x10E&0x4000 → State 4 (Skript-Perch, +0x5=+0x10E&0xf, +0x22A|=1 @0x8010043C-8C):
     * OFFEN — kein RE1.5-Spawn-Produzent fuer +0x10E (re2z_f10e bleibt 0; Spec der drei
     * State-4-Subs inkl. Wegpunkte (−12500,−12852)→(−3919,−12288) @0x801036FC-704/@0x8010376C-70
     * in RE15_RE2_AI.md dokumentiert). +0x10E&0x40 → +0x1D3=128 + word0|=8 (@0x80100490-B0):
     * ebenso ohne Produzent. Victim-Hook-Install 0x80104740→0x800CE384 (@0x801004B4-C0):
     * Port = Victim-Map keyt auf Typ 0x21 (enemy_ai_common.c re15_victim_clip_map). */
    e->root_prev_kf = -1;
    e->sca_mask = 4;
}

/* ================================ root tick ================================================= */

int re15_re2crow_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e->active) return 0;

    /* Root-Prolog @0x80100148-174: Pause-Gate uebernimmt der Hook (enemy_ai_common.c);
     * +0x1D3 low-7-Dekrement: */
    if (e->re2z_self1d3 & 0x7fu)
        e->re2z_self1d3 = (uint8_t)((e->re2z_self1d3 & 0x80u) | ((e->re2z_self1d3 & 0x7fu) - 1u));

    /* LOS-Ray 0x80050858(self, PL, 0x8400, 0) @0x80100178-1C4: ret==0 → Bit 2 SETZEN
     * (@0x801001C8-E8 — Polaritaet selbst belegt, s. Datei-Kopf). */
    if (re15_re2_los_clear(e, pl)) e->re2c_flags22a |= 0x2u;
    else                           e->re2c_flags22a &= (uint16_t)~0x2u;

    /* dist<900 → +0x98=−350 sonst 0 (@0x801001EC-208; Render-Pitch OFFEN, doc): */
    e->target_z = (int16_t)((e->ai_dist < 0x384u) ? -350 : 0);

    if (e->state == 1) { e->re2z_prev_sub = e->sub_state_1; e->re2z_prev_hp = e->hp; }
    /* Treffer-Erkennung (Welle-C-Muster): der Port-Damage-Writer schreibt +0x4=2/3 und
     * ueberschreibt +0x5/+0x6 — bei neuem Treffer Phase nullen, Zeile = prev_sub. */
    if ((e->state == 2 || e->state == 3) && e->hp != e->re2z_prev_hp) {
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->re2z_prev_hp = e->hp;
    }

    int32_t ox = e->x, oz = e->z;

    switch (e->state) {                                    /* Tabelle @0x80104908 */
    case 0: re2c_init(e); break;                           /* 0x801002FC */
    case 1: {                                              /* ACTIVE 0x801004E4 */
        /* Nav-Reseed 1/32 @0x80100508-540: +0x220=rand&0x7f, +0x21F=0x8004AA50() (MAPPING 0) */
        if (e->re2d_rel220 == 0 && (re15_re2_rand() & 0x1fu) == 0u) {
            e->re2d_rel220    = (uint8_t)(re15_re2_rand() & 0x7fu);
            e->re2d_budget21f = 0;
        }
        /* Navigator-Modus @0x80100544-5F4 (0x8004A808-MAPPING: Steuerziel = Spieler; die
         * Buchhaltung laeuft byte-true): */
        if (pl->hp < 0) {
            int8_t p = e->re2c_pac221;
            if (p != 0) {
                if (e->re2d_rel220 != 0) e->re2d_rel220--;  /* Routen-Tick @0x801005C8-E0 */
                e->re2c_pac221 = (int8_t)(p - 1);           /* Pacifier-Dec @0x80100574-80 */
            }
        } else {
            if (!(e->re2c_flags22a & 0x40u) && e->re2d_rel220 != 0)
                e->re2d_rel220--;                           /* @0x80100584-A8/5C8-E0 */
        }
        /* DEC → EXEC Doppel-Dispatch (@0x801005F8-69C; ALT-Tabellen inhaltsgleich). Das
         * Original liest +0x5 fuer den EXEC NEU (`lbu v0,5` @0x80100654 UND @0x8010067C) —
         * ein DEC-Commit laeuft also NOCH IM SELBEN TICK in den NEUEN Executor. */
        switch (e->sub_state_1) {                          /* DEC @0x80104928 */
        case 0:  re2c_dec0_idle(e, pl); break;
        case 4:  re2c_dec4_circle(e, pl); break;
        case 5:  re2c_dec5_climb(e, pl); break;
        case 6:  re2c_dec6_descend(e, pl); break;
        case 8:  re2c_abort_listener(e); break;            /* DEC[8] 0x80101944 */
        case 11: re2c_dec11_attackrun(e, pl); break;
        case 12: re2c_dec12_strike(e); break;
        case 13: re2c_dec13_hover(e, pl); break;
        case 14: re2c_dec14_grab(e); break;
        default: break;                                    /* 1/2/3/7/9/10: jr-ra-DECs */
        }
        switch (e->sub_state_1) {                          /* EXEC @0x80104964 (frisch gelesen) */
        case 0:  re2c_exec0_idle(e); break;
        case 1:  re2c_exec1_alarm(e); break;
        case 2:  re2c_exec2_takeoff(e, pl); break;
        case 3:  re2c_exec3_script_leave(e, pl); break;
        case 4:  re2c_exec4_circle(e, pl); break;
        case 5:  re2c_exec5_climb(e, pl); break;
        case 6:  re2c_exec6_descend(e, pl); break;
        case 7:  re2c_exec7_land(e, pl); break;
        case 8:  re2c_exec8_tumble(e, pl); break;
        case 9:  re2c_exec9_settle(e, pl); break;
        case 10: re2c_exec10_wallcrash(e); break;
        case 11: re2c_exec11_attackrun(e, pl); break;
        case 12: re2c_exec12_strike(e, pl); break;
        case 13: re2c_exec13_hover(e, pl); break;
        default: re2c_exec14_grab(e, pl); break;
        }
        break;
    }
    case 2: case 3:                                        /* HURT 0x801028BC ([2]==[3]) */
        re2c_hurt(e, pl);
        break;
    case 4:
        /* SKRIPT-PERCH 0x801034DC (Tabelle @0x80104A64) — OFFEN: kein RE1.5-Spawn erreicht
         * State 4 (+0x10E&0x4000 ohne Produzent, s. INIT-Kommentar). Defensive: ACTIVE-Idle. */
        re2c_state(e, 1, 0);
        break;
    case 7:                                                /* CORPSE 0x80103950 */
        re2c_corpse(e);
        break;
    default:
        re2c_state(e, 1, 0);                               /* [5][6] NULL-Zeilen */
        break;
    }

    /* Root-Tail @0x80100234-254:
     * (1) Schatten-Helfer 0x8010026C (selbst disasm'd): state!=7 → alt = +0x1C2 − y;
     *     alt−250>0 → [+0x16C]+4/+6 = 360000/((alt+1550)>>1) (lui 0x5 ori 0x7e40 = 0x57E40,
     *     div @0x80100294-2D8), sonst 400/400 (0x1900190 @0x801002E4-F0). */
    if (e->state != 7) {
        int32_t alt = (int32_t)e->dog_floor_y - e->y;
        if (alt - 250 > 0) {
            int32_t w = 360000 / ((alt + 1550) >> 1);
            e->crow_shadow_w = (uint16_t)w; e->crow_shadow_h = (uint16_t)w;
        } else { e->crow_shadow_w = 400; e->crow_shadow_h = 400; }
    }
    /* (2) Post-Pass 0x801044B0: Mutex-Release wenn geclaimt aber +0x5 nicht in 11..14
     *     (`lbu v0,5; addiu −11; sltiu 0x4; bne → skip` @0x801044C4-D4 — WOERTLICH auf dem
     *     AKTUELLEN +0x5, ohne State-Check; Review-Fix #9 entfernte den erfundenen state==1-
     *     Zusatz). +0x5-Semantik nach einem Treffer SELBST BELEGT (Welle-D-Nachtrag): BEIDE
     *     RE2-Waffen-Applier ueberschreiben +0x5 — Projektil/AoE `sb s5,5(s1)` (= Waffen-
     *     Low-Byte) @0x80047324/@0x80047574, Hitscan `sb (hi16(param)+1),5` @0x80041AB4.
     *     Ein Hitscan-/Standard-Treffer setzt +0x5 also auf einen kleinen Code ∉ 11..14 →
     *     das Original released den Mutex im ERSTEN Post-Pass nach dem Treffer; gehalten
     *     wird er nur bei den Projektil-Waffen-Ids 11..14. Der Port-Writer schreibt die
     *     RE1.5-Waffen-Id (0..10) nach +0x5 → identische Mechanik fuer das gesamte RE1.5-
     *     Arsenal (RE2-Waffen 11..14 existieren im Port nicht; s. RE15_RE2_AI.md OFFEN). */
    if ((e->re2c_flags22a & 0x4u)
        && !(e->sub_state_1 >= 11 && e->sub_state_1 <= 14)) {
        g_re2_room_gflags &= (uint16_t)~0x1u;              /* @0x801044E4-F0 */
        e->re2c_flags22a  &= (uint16_t)~0x4u;              /* @0x801044F4-500 */
    }
    /* Pacify-Bit 0x80 → +0x221=120 (@0x80104504-20). */
    if (e->re2c_flags22a & 0x80u) {
        e->re2c_flags22a &= (uint16_t)~0x80u;              /* @0x80104510-18 */
        e->re2c_pac221 = 120;                              /* sb 120,545 @0x8010451C-20 */
    }
    /* (3) Wand-Pass FUN_8003567C → +0x110/+0x114 (Radius = +0x90, Vol-gekoppelt — MAPPING:
     *     Port-SCA-Klemme mit Radius 200 wie die RE1.5-Kraehe; Bit 1 = Frontal-Metrik als
     *     deklariertes MAPPING fuer das 0x80050110-Ebenen-Fenster des Crash-Probers). */
    if (g_room_rdt_ok) {
        int32_t nx = e->x, nz = e->z;
        re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, 200, e->y, 4u);
        if (nx != e->x || nz != e->z) {
            int64_t in2  = (int64_t)(e->x - ox) * (e->x - ox) + (int64_t)(e->z - oz) * (e->z - oz);
            int64_t out2 = (int64_t)(nx - ox) * (nx - ox) + (int64_t)(nz - oz) * (nz - oz);
            e->crow_wall = (uint8_t)((out2 * 4 < in2) ? 3 : 1);
            e->x = nx; e->z = nz;
        } else e->crow_wall = 0;
    } else e->crow_wall = 0;
    /* (4) Body-Push-Stempel +0xD (FUN_800355c4/FUN_80034d0c, s. Datei-Kopf): Spieler-Kontakt
     *     dieses Frames — Vorframe-Phase wie beim Hund. */
    e->crow_contact = (uint8_t)(re15_body_push(pl, RE15_BODY_R_PLAYER, e,
                                               (int32_t)e->hit_radius_min) ? 1 : 0);
    /* (5) RE1.5-INTERFACE-Shim (nicht RE2): aim_band-Stempel, damit der RE1.5-Schuss-Resolver
     *     die fliegende Kraehe im richtigen Hoehenband trifft (re15_damage.c keyt fuer Typ
     *     0x21 auf aim_band; Formel = RE1.5-ACTIVE-Tail @0x80112560-C8 — dokumentierter
     *     Port-Anschluss, kein RE2-Verhalten). */
    {
        int32_t vert = pl->y - e->y;
        if (vert >= 4001)     e->aim_band = (uint8_t)((e->ai_dist < 6000u) ? 4 : 0);
        else if (vert < 800)  e->aim_band = 1;
        else                  e->aim_band = 2;
    }

    if (getenv("RE15_RE2_TRACE")) {
        static uint32_t s_last[RE15_ACTOR_MAX];
        uint32_t sig = ((uint32_t)e->state << 16) | ((uint32_t)e->sub_state_1 << 8) | e->sub_state_2;
        if (sig != s_last[slot]) {
            fprintf(stderr, "[re2crow] slot %d state=%d sub=%d/%d clip=%d hp=%d dist=%u spd=%d y=%d fl=%04x\n",
                    slot, e->state, e->sub_state_1, e->sub_state_2,
                    (int)e->motion, (int)e->hp, (unsigned)e->ai_dist, (int)e->speed_h,
                    (int)e->y, (unsigned)e->re2c_flags22a);
            s_last[slot] = sig;
        }
    }
    return 1;
}
