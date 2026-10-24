/*
 * RE1.5 Rebuilt — Runde 38 Spur R "spinnen" (Stage-2-Spinnen). Kopf: include/re15_spinnen_r38.h,
 * Dossier: analysis/befunde_runde38/R_spinnen.md (Messungen 1.x, Belege 2.x).
 *
 * Drei Teile:
 *   1. SCHACHT-AUSSTIEG — STAGE2.BIN Zustand 4 Unterzustand 1 (grid 0x41), byte-true aus den Bytes
 *      @0x80114ec8 / Phasentabelle @0x8010027c; im Original per DuckStation-GDB Bild fuer Bild
 *      nachgemessen (R_belege/orig_2060_schacht_gift.txt, orig_20a0_schacht.txt).
 *   2. GIFT — Effekt 0x0d der Raum-ESP (Spucken @0x80112770), ESP-Routinen 24 @0x80017f50 und
 *      25 @0x80017fa4, Gift-Wirkung des Spieler-Kommandos 1 @0x80031e44-ecc.
 *   3. (in enemy_ai_re2_spider.c, kleine Haken) RE2-Minispinnen: Boden des Elternteils, Biss-
 *      Reichweite mit dem RE2-Spieler-Radius.
 */
#include "re15_spinnen_r38.h"
#include "re15_zustand_r39.h"   /* Runde 39 Spur F: RE2-Gift-Takt */
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_esp.h"
#include "re15_enemy.h"        /* re15_enemy_find — die geladene Bank des Typs (RE1.5 bzw. RE2-Hybrid) */
#include "re15_skeleton.h"     /* re15_skel_compute_pose / re15_skel_bone_to_world / sin/cos */
#include "re15_anim_select.h"  /* re15_compute_actor_kf */
#include "re15_damage.h"       /* re15_resolve_attack (FUN_80012d60), re15_hitbox_test, rand8 */
#include "re15_collision.h"    /* re15_collision_room_coll (FUN_8001c6e8) */
#include "re15_rdt.h"
#include "re15_esp_brocken.h"   /* re15_esp_fx_zeile_weiter = FUN_800174e4 */
#include "re15_scd.h"            /* g_scd.props = Obj_model_set-Pool 0x800b3f98 (Teil 4) */
#include "re15_ai_flavor.h"      /* re15_ai_re2_for_type / re15_re2spider_owns (Teil 4) */
#include "re15_math.h"          /* re15_squareroot0 = FUN_8008d2f4 (Teil 3b) */
#include "re15_enemy_ai.h"      /* RE15_AI_STATE_CORPSE (Teil 3b) */
#include "re15_engine.h"         /* g_engine.frame_count (Protokollzeile des Szenenschutzes) */
#include "re15_szenenschutz_r38.h" /* Runde 38 Spur B: re15_szenenschutz_haelt (Routine 25, Nachbesserung 1 Mangel 3) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================================
 * Teil 1 — SCHACHT-AUSSTIEG
 * ============================================================================================
 * EM025-Cliplaengen, aus der Datei gelesen: CDEMD0.EMS Blob 10 (DEFAULT_EMS_ORDER[10] = 0x25,
 * re15_ems.c), Verzeichnis @Blob+0x1f47c, dir[1] = Bank-0-EDD @Blob+0xc, Offset0 0x44 = 17 Clips,
 * u16 Bildzahl je 4-Byte-Kopf. Die Zustands-Pruefungen `lbu +0x95 / ori 0x1f / bne`
 * (@0x80114fe0-ec, @0x801151cc-d8) brauchen genau diese Laengen (Clip 0x10 und 0x0d = 32). */
const uint8_t re15_spinnen_r38_em025_clip_len[17] =
    { 16, 8, 16, 48, 126, 68, 5, 32, 21, 17, 5, 10, 45, 32, 32, 32, 32 };

/* RE2-KI (Hybrid: RE2-Skelett/-Animation): RE1.5-Clip -> keyframe-gleicher RE2-EM25-Clip.
 * Bild fuer Bild verglichen (EDD-Bildliste + EMR-Keyframe-Bytes, RE2 = shared_assets/RE2/CDEMD0.EMS
 * Sektor 1871, 16 Clips {16,8,16,31,21,6,10,17,5,29,55,42,206,68,32,32}):
 *   RE1.5 0x10 (32) == RE2 15 (32): 32/32 Keyframes gleich
 *   RE1.5 0x0d (32) == RE2 14 (32): 32/32
 *   RE1.5 2    (16) == RE2 2  (16): 16/16
 *   RE1.5 0    (16) == RE2 0  (16): 16/16
 * Dossier 3.1. Die Laengen sind paarweise gleich -> die RE1.5-Bildpruefungen gelten unveraendert. */
static int16_t lauer_clip_anzeige(int re2, uint8_t re15_clip)
{
    if (!re2) return (int16_t)re15_clip;
    if (re15_clip == 0x10) return 15;
    if (re15_clip == 0x0d) return 14;
    return (int16_t)re15_clip;                       /* 0 und 2 sind identisch nummeriert */
}
static uint8_t lauer_clip_re15(int re2, int16_t motion)
{
    if (!re2) return (uint8_t)motion;
    if (motion == 15) return 0x10;
    if (motion == 14) return 0x0d;
    return (uint8_t)motion;
}

/* Clip setzen wie `sb clip,148 / sb zero,149 / sb 7|0,143` (+0x94/+0x95/+0x8f). */
static void lauer_clip(re15_actor_t *e, int re2, uint8_t re15_clip, uint8_t frac)
{
    e->motion     = lauer_clip_anzeige(re2, re15_clip);
    e->anim_frame = 0;
    e->anim_frac  = frac;
}

/* anim_set FUN_8001f314(+0x84,+0x16c,0,0x200)-Zwilling: Bild nach dem Posieren +1 (POST-inc),
 * Umbruch an der EM025-Cliplaenge, +0x8f-Ueberblendzaehler -1. Rueckgabe 1 im Umbruchbild. */
static int lauer_anim(re15_actor_t *e, int re2)
{
    uint8_t c = lauer_clip_re15(re2, e->motion);
    int fc = (c < 17) ? re15_spinnen_r38_em025_clip_len[c] : 1;
    if (e->anim_frac > 0) e->anim_frac--;
    e->anim_frame++;
    if ((int)e->anim_frame >= fc) { e->anim_frame = 0; return 1; }
    return 0;
}

/* FUN_800245d8(a0)-Zwilling: Vektor (+0x8c,0,0) um RotMatrixY(+0x6a + a0) drehen, X/Z addieren
 * (`lhu v0,140(v0)` @0x800245f0, `lh v0,106(v0); addu a0,v0,a0` @0x80024658-64; MVMVA sf=1 ->
 * dx = (cos*v) >> 12, dz = (-sin*v) >> 12, Zeilen wie actor_locomotion.c). */
static void lauer_schub(re15_actor_t *e, int16_t speed, int a0)
{
    e->crow_speed = speed;                            /* +0x8c */
    int a = ((int)(uint16_t)e->rot_y + a0) & 0xfff;
    int32_t c = re15_cos_q12(a), s = re15_sin_q12(a);
    e->x += (int32_t)((c * (int32_t)speed) >> 12);
    e->z += (int32_t)((-s * (int32_t)speed) >> 12);
}

/* +0x78-Kaesten der Spinne (STAGE2.BIN @0x80118dd4.., 6 x s16 = Versatz x/y/z, Halbmasse x/y/z):
 *   A @0x80118dd4 = {0,-900,0, 1000,900,1000}  (*(0x80118e04), Normalkasten)
 *   C @0x80118dec = {0,-1440,0, 200,1440,200}  (*(0x80118e0c), waehrend des Ausstiegs)
 * Port-Felder wie re15_enemy_apply_hitbox (re15_damage.c): Radius = Halbmass x, Hoehe = Halbmass y,
 * Versatz y = -Hoehe. */
static void lauer_kasten(re15_actor_t *e, int kasten_c)
{
    uint16_t r = kasten_c ? 200 : 1000, h = kasten_c ? 1440 : 900;
    e->hit_radius_min = r; e->hit_radius_max = r; e->hit_height = h;
    e->hit_offset_x = 0; e->hit_offset_y = (int16_t)(-(int32_t)h); e->hit_offset_z = 0;
}

/* Die neun Phasen @0x80114ec8 (Gate `sltiu v0,v1,0x9` @0x80114ee0, Tabelle @0x8010027c).
 * Rueckgabe 1, wenn Phase 8 das Zustandswort auf ACTIVE gesetzt hat (gelandet). */
static int lauer_lauf(re15_actor_t *e, int re2)
{
    switch (e->sub_state_3) {
    case 0:                                                    /* @0x80114f08 */
        if (e->grid_id == 0x41) return 0;                      /* `ori v0,zero,0x41 / beq` @0x80114f18-1c:
                                                                * warten, bis das Skript grid aendert
                                                                * (ROOM2060 sub02 `34 0c 42 00`) */
        e->sub_state_3 = 1;                                    /* `sb v0(1),7(a0)` @0x80114f24 */
        lauer_clip(e, re2, 0x10, 7);                           /* +0x94 = 0x10 @0x80114f34, +0x95 = 0
                                                                * @0x80114f44, +0x8f = 7 @0x80114f54 */
        lauer_kasten(e, 1);                                    /* +0x78 = *(0x80118e0c) @0x80114f64-6c */
        e->hit_react |= 3u;                                    /* +0x93 |= 3 @0x80114f84-88 */
        /* faellt durch (@0x80114f8c folgt ohne Sprung) */
        /* fallthrough */
    case 1:                                                    /* @0x80114f8c */
        if (e->anim_frame < 0x0b)                              /* `sltiu v0,v0,0xb` @0x80114fa0 */
            lauer_schub(e, 0xc8, 0);                           /* +0x8c = 200 @0x80114fac, FUN_800245d8(0)
                                                                * @0x80114fb0 */
        (void)lauer_anim(e, re2);                              /* anim_set @0x80114fcc */
        if (e->anim_frame == 0x1f) e->sub_state_3 = 2;         /* `ori v0,zero,0x1f / bne` @0x80114fe4-f4 */
        return 0;
    case 2:                                                    /* @0x80114ff8 */
        e->sub_state_3 = 3;                                    /* @0x80115004 */
        e->y += 1000;                                          /* `addiu v0,v0,1000` @0x8011501c */
        lauer_schub(e, 0x12c, 0x800);                          /* +0x8c = 300 @0x80115034, FUN_800245d8(0x800)
                                                                * @0x80115030 (rueckwaerts) */
        e->dog_floor_y = 0;                                    /* +0x1ba = 0 `sh zero,442(v0)` @0x80115044 */
        e->rot_z = (int16_t)(e->rot_z + 1024);                 /* +0x6c += 1024 @0x8011505c-64 */
        (void)lauer_anim(e, re2);                              /* `j 0x80115314` -> anim_set @0x80115328 */
        return 0;
    case 3:                                                    /* @0x80115068 */
        lauer_clip(e, re2, 2, 7);                              /* +0x94 = 2 @0x80115074, +0x95 = 0, +0x8f = 7 */
        e->sub_state_3 = 4;                                    /* @0x80115084 */
        /* fallthrough */
    case 4:                                                    /* @0x801150a8 */
        (void)lauer_anim(e, re2);                              /* anim_set @0x801150bc */
        e->y += 100 + (int32_t)(re15_engine_rand8() & 0xfu);   /* rng @0x801150c4, `andi 0xf / addiu 100`
                                                                * @0x801150dc-e8 */
        if ((int32_t)e->dog_floor_y - 1800 < e->y)             /* `addiu -1800 / slt` @0x80115100-04 */
            e->sub_state_3 = 5;                                /* @0x80115114 */
        return 0;
    case 5:                                                    /* @0x80115118 */
        e->sub_state_3 = 6;                                    /* @0x80115124 */
        lauer_clip(e, re2, 0x0d, 7);                           /* +0x94 = 0xd @0x80115134, +0x95 = 0, +0x8f = 7 */
        /* fallthrough */
    case 6:                                                    /* @0x80115158 */
        (void)lauer_anim(e, re2);                              /* anim_set @0x8011516c */
        if (e->y < (int32_t)e->dog_floor_y - 500)              /* `addiu -500 / slt` @0x80115188-8c */
            e->y += 100 + (int32_t)(re15_engine_rand8() & 0xfu);   /* @0x80115198-bc */
        if (e->anim_frame == 0x1f) e->sub_state_3 = 7;         /* @0x801151d0-e0 */
        return 0;
    case 7: {                                                  /* @0x801151e4 */
        e->sub_state_3 = 8;                                    /* @0x801151f0 */
        e->anim_frac = 0;                                      /* +0x8f = 0 @0x80115200 */
        e->rot_z = (int16_t)(e->rot_z - 1024);                 /* +0x6c -= 1024 @0x80115210-1c */
        lauer_kasten(e, 0);                                    /* +0x78 = *(0x80118e04) @0x8011522c-34 */
        e->floor = 0;                                          /* +0x82 = 0 @0x80115244 */
        e->grid_id = 5;                                        /* +0x9 = 5 @0x80115250-54 */
        e->y = (int32_t)e->dog_floor_y;                        /* y = (s16)+0x1ba @0x80115264-6c */
        unsigned q = ((unsigned)(uint16_t)e->rot_y & 0xc00u) >> 10;   /* `lhu 106 / andi 0xc00 / srl 10`
                                                                * @0x8011527c-88 */
        if      (q == 0) e->x += 1488;                         /* @0x801152d0-e0 */
        else if (q == 1) e->z -= 1488;                         /* @0x801152e4-ec */
        else if (q == 2) e->x -= 1488;                         /* @0x801152f0-00 */
        else             e->z += 1488;                         /* @0x80115304-10 */
        (void)lauer_anim(e, re2);                              /* anim_set @0x80115328 */
        return 0;
    }
    case 8:                                                    /* @0x80115338 */
        e->hit_react = 0;                                      /* +0x93 = 0 @0x80115344 */
        lauer_clip(e, re2, 0, 0);                              /* +0x94 = 0 @0x80115354, +0x95 = 0,
                                                                * +0x8f = 0 @0x80115374 */
        (void)lauer_anim(e, re2);                              /* anim_set @0x8011538c */
        e->state = 1; e->sub_state_1 = 0;                      /* `sh v0(1),4(v1)` @0x801153a0 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;                /* `sh zero,6(v0)` @0x801153b0 */
        return 1;
    default:
        return 0;                                              /* Gate >= 9 @0x80114ee4 -> Epilog */
    }
}

void re15_spinnen_r38_lauer(re15_actor_t *e)
{
    if (!e) return;
    (void)lauer_lauf(e, 0);
}

/* RE2-KI. Schachtspinnen (Sce_em_set grid 0x41 in ROOM2060/2061/20A0/20A1) haben im RE2-Gehirn
 * kein Gegenstueck: der RE2-INIT liest +0x10E (Port: grid) ueber die Sprungtabelle @0x80100004 —
 * Eintrag 1 ist der leere Epilog @0x8010058C, die Spinne bliebe im Bodenmodus auf der Schachthoehe
 * stehen (gemessen: y -3600 ueber dem Boden, Dossier 1.2). PORT-WAHL: die Schacht-Choreografie ist
 * RE1.5-Raumdaten + RE1.5-Gegnercode; sie laeuft byte-true wie im RE1.5-Modus (mit den keyframe-
 * gleichen RE2-Clips), danach uebernimmt das RE2-Gehirn mit seinem INIT auf dem Boden:
 *   +0x10E := 0 (= Boden-Deskriptor, INIT-Tabelle Eintrag 0 @0x80100460), +0x1C2 (Port dog_floor_y)
 *   = die RE1.5-Bodenhoehe nach der Landung (+0x1ba = 0 @0x80115044). */
/* Je Aktor-Platz: hat in DIESEM Bild RE1.5-Spinnencode (INIT-Zweig grid 0x41 oder die Phasen) die
 * Spinne im RE2-Modus bewegt? Dann gilt fuer sie in diesem Bild auch der RE1.5-Objekt-Durchgang
 * (Teil 4, re15_spinnen_r38_objekt_pass liest und loescht die Marke). */
static uint8_t s_lauer_lief[RE15_ACTOR_MAX];

int re15_spinnen_r38_lauer_re2(re15_actor_t *e)
{
    if (!e || e->type != 0x25) return 0;
    {   int slot = (int)(e - g_actors);
        if (slot >= 0 && slot < RE15_ACTOR_MAX
            && ((e->state == 0 && e->grid_id == 0x41) || (e->state == 4 && e->sub_state_1 == 1)))
            s_lauer_lief[slot] = 1;
    }
    if (e->state == 0 && e->grid_id == 0x41) {
        /* RE1.5-INIT-Zweig fuer grid 0x41 (0x80110b6c): Clip 0x10 @0x80110b90, +0x95 = 0 @0x80110ba0,
         * +0x93 = 0 @0x80110bc0; grid & 0xf = 1 < 2 -> rot_x = rot_z = 0, y = -1800 * +0x82
         * (@0x80110c28-74); Zustandswort 0x104 (Zustand 4, +0x5 = 1) @0x80110d14; anim_set einmal
         * @0x80110d60 (Halteposen-Bild 1, gemessen `c16/1` im Original). */
        e->state = 4; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->rot_x = 0; e->rot_z = 0;
        e->y = -1800 * (int32_t)e->floor;
        e->hit_react = 0;
        e->aim_band = 0;                                       /* Runde 38 Integration: word0 &= 0x1fffffff
                                                                * (`lui v1,0x1fff` / `and` / `sw v0,0(a0)` @0x80110d38-48) */
        lauer_clip(e, 1, 0x10, 0);
        (void)lauer_anim(e, 1);
        return 1;
    }
    if (e->state == 4 && e->sub_state_1 == 1) {
        if (lauer_lauf(e, 1)) {
            /* gelandet -> RE2-INIT auf dem Boden (s.o.) */
            e->grid_id   = 0;                                  /* -> +0x10E = 0 (Port-Abbildung in
                                                                * re15_adult_spider_ai_tick) */
            e->re2z_f10e = 0;
            e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        }
        return 1;
    }
    return 0;
}

/* ============================================================================================
 * Teil 5 (Runde 38 Integration) — DIE SPINNE IM SCHACHT IST KEIN ZIEL
 * ============================================================================================
 * Original, selbst disassembliert (PSX.EXE bzw. STAGE2.BIN roh @0x80100000):
 *   Resolver FUN_80011f50: Spieler-word0 = (word0 & 0x1fffffff) | (acaec << 16) @0x80011fb0-e8; je Kandidat
 *     `lw v0,0(s0)` @0x800120d0, `lw v1,-13740(v1)` (0x800aca54) @0x800120d8, `and v0,v0,v1` @0x800120e0,
 *     `lui v1,0xe000` @0x800120e4, `and v0,v0,v1` @0x800120e8, `beq v0,zero,0x80012128` @0x800120ec -> naechster.
 *   Spinne INIT 0x80110b6c: `sw v0(1),4(v1)` @0x80110b80, grid 0x41 -> Zustandswort 0x104 (`ori v0,zero,0x104` /
 *     `sw v0,4(a0)` @0x80110d10-14), Band weg: `lui v1,0x1fff` @0x80110d38 / `lw v0,0(a0)` / `ori v1,v1,0xffff` /
 *     `and v0,v0,v1` / `sw v0,0(a0)` @0x80110d48.
 *   Zustand 4 (Tabelle @0x80118e24 [4..6] = 0x80114e18, Ende 0x801153d0): KEIN Store auf +0..+3 (Vollscan; die
 *     Aufrufe 0x800245d8 / 0x8001f314 / 0x8001af20 schreiben word0 ebenfalls nicht).
 *   ACTIVE 0x80110e50 (Schwanz): `lui v1,0x1fff` / `and` / `sw v0,0(a0)` @0x80110f94-a4, `lbu v1,9(v0)` @0x80110fb4,
 *     `sltiu v0,v1,0x2` @0x80110fbc, `bne v0,zero,0x80110fd0` @0x80110fc0, `ori v0,zero,0x5` / `bne v1,v0,0x80110fd8`
 *     @0x80110fc4-c8, `jal 0x80012aa4` / `ori a0,zero,0xfa0` @0x80110fd0-d4.
 * Gemessen im Original (DuckStation-GDB, MZD-Disc, ROOM2060, R_belege/orig_2060_zielhilfe_messer.txt): Wort 0 der
 *   Schachtspinne 0x00000801 (Band 0) im Warten (grid 0x41) und Kriechen (grid 0x42, +0x93 = 03), nach Phase 7
 *   (grid 5) und Phase 8 (Zustand 1, +0x93 = 00) weiter Band 0, ab dem ersten ACTIVE-Bild 0x60000801 (LEVEL|DOWN).
 * PORT: aim_band (re15_actor.h: "word0-Bits 29-31: Elevation-Band fuer den Schuss-Resolver") traegt dieses Band fuer
 *   die RE1.5-Spinne; der Resolver-Zweig Typ 0x25 (re15_damage.c) fuehrt die BITAUSWAHL weiter ueber sein bisheriges
 *   Modell (LEVEL + RE2-Fenster) und nimmt hier nur das LEERE Band des Originals: kein Kandidat. */
int re15_spinnen_r38_band_leer(const re15_actor_t *e)
{
    if (!e || e->type != 0x25u) return 0;
    /* Zustand 0: Sce_em_set-Satz vor dem INIT (word0 = 1 bzw. 0x2001 @0x8004228c-b0, Bandbits leer) — unter der RE2-KI
     * auch die Uebergabe nach Phase 8 (spinnen_r38.c lauer_re2: Zustand 0 bis zum RE2-INIT). Zustand 4: INIT-Zweig grid
     * 0x40/0x41 (@0x80110cec-d14) und die Schacht-Choreografie, beide ohne Stempel. */
    if (e->state == 0 || e->state == 4) return 1;
    /* RE1.5-Gehirn: das Band ist der ACTIVE-Stempel (re15_spinnen_r38_band_stempel); leer bis zum ersten ACTIVE-Schwanz
     * nach INIT/Phase 8 und wann immer FUN_80012aa4(0xfa0) nichts setzt. */
    if (!re15_ai_re2_for_type(0x25u)) return e->aim_band == 0;
    /* RE2-Gehirn nach seinem INIT: RE2s Kandidatenfilter hat kein Hoehenband (FUN_800470C0 Gates @0x8004712C/38/48/58,
     * FUN_800410CC ueber die Haltungsklasse) — Port-Abbildung wie fuer jeden RE2-eigenen Typ. */
    return 0;
}

void re15_spinnen_r38_band_stempel(re15_actor_t *e, const re15_actor_t *pl)
{
    if (!e || !pl) return;
    e->aim_band = 0;                                           /* word0 &= 0x1fffffff @0x80110f94-a4 */
    if (e->grid_id < 2u || e->grid_id == 5u)                   /* `sltiu v0,v1,0x2` @0x80110fbc / `ori v0,zero,0x5` +
                                                                * `bne v1,v0` @0x80110fc4-c8 (ganzes Byte +0x9) */
        e->aim_band = re15_band_stamp_aa4_aim(pl, e, 0xfa0);   /* `jal 0x80012aa4` / `ori a0,zero,0xfa0` @0x80110fd0-d4 */
}

int re15_spinnen_r38_schacht_re15(const re15_actor_t *e)
{
    return e && e->type == 0x25u && e->state == 4;            /* Zustand 4 = 0x80114e18 (Tabelle @0x80118e24 [4]) */
}

/* ============================================================================================
 * Teil 2 — GIFT
 * ============================================================================================ */
static uint8_t  s_gift_zaehler;                       /* Spieler+0x1d3 = 0x800acc27 (@0x80031e60) */
static unsigned s_gift_treffer;
static uint8_t  s_gift_besitzer[RE15_ESP_FX_MAX];     /* Spinnen-Slot + 1 je Effektplatz (Gate A),
                                                      * 0 = kein Besitzer */

void re15_spinnen_r38_reset(void)
{
    s_gift_zaehler = 0;
    s_gift_treffer = 0;
    memset(s_gift_besitzer, 0, sizeof s_gift_besitzer);
}
uint8_t  re15_spinnen_r38_gift_zaehler(void) { return s_gift_zaehler; }
unsigned re15_spinnen_r38_gift_treffer(void) { return s_gift_treffer; }

/* Weltlage des Teil-0-Ankers + lokaler Versatz. Original: FUN_80019700 a2 = *(+0x188)+0x40 = die
 * Part-0-Weltmatrix (Kommentar re15_esp.h FOLLOW), a3 = Versatz; Weltlage = R*Versatz + T
 * (Hauptlauf @0x8001a248-2a4). Port: Pose von Bone 0 aus der geladenen Bank im aktuellen
 * Clipbild (re15_skel_compute_pose: rot = Q12-Modellmatrix, trans = Modell-Lage), dann
 * Modell -> Welt mit der Gier (re15_skel_bone_to_world). Ohne Bank (Unit-Test ohne Modell):
 * Wurzel + Gier*Versatz — als Rueckfall deklariert, nicht als Original. */
static void gift_anker(const re15_actor_t *e, const int32_t ofs[3], int32_t out[3])
{
    int32_t v[3] = { ofs[0], ofs[1], ofs[2] };
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (b && b->ok && b->skel.bone_count > 0) {
        int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        void *save = g_anim_pose_actor;
        g_anim_pose_actor = NULL;                       /* Abfrage: Ueberblendung nicht anfassen */
        int rv = re15_skel_compute_pose(&b->skel, kf, poses);
        g_anim_pose_actor = save;
        if (rv == 0) {
            for (int i = 0; i < 3; i++) {
                int64_t s = (int64_t)poses[0].rot[i * 3 + 0] * ofs[0]
                          + (int64_t)poses[0].rot[i * 3 + 1] * ofs[1]
                          + (int64_t)poses[0].rot[i * 3 + 2] * ofs[2];
                v[i] = (int32_t)(s >> 12) + poses[0].trans[i];
            }
        }
    }
    re15_skel_bone_to_world(v, e->rot_y, e->x, e->y, e->z, out);
}

static int gift_platz_index(const re15_esp_fx_t *f)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++)
        if (re15_esp_fx_get(i) == f) return i;
    return -1;
}

/* Spucken. RE1.5 Unterzustand 7 @0x801126d4, Phase 0:
 *   func_0x80019700(0x0d002000, +0x6a, *(+0x188)+0x40, &DAT_80118ee8)  `jal 0x80019700` @0x80112770
 *   -> Effekt 0x0d (`a0>>24`), sub 0, Skala 0x2000; Versatz DAT_80118ee8 = (500,0,0) (STAGE2.BIN,
 *   `read 0x80118ee8 --w 2`: 500,0,0).
 * RE2 Modus 0 Unterzustand 8 @0x80100F88 P0: FUN_801058A4(self, +0x222+8, +0x76+0x21A) @0x8010101C;
 *   dort Versatz (1000,0,-100) (`addiu v0,zero,1000 / sh 16(sp)`, `sh zero,18(sp)`, `addiu v0,
 *   zero,-100 / sh 20(sp)` @0x80105910-24) am Teil 0 (+0x198-Pool + 72), Gier a1 = +0x76 + +0x21A.
 *   PORT-WAHL (Hybrid wie die RE1.5-Modelle im RE2-Modus): gespawnt wird der Gift-Effekt der
 *   RE1.5-RAUMDATEN (0x0d) — der RE2-Effekt 0x1D liegt nur in den RE2-Raum-ESPs. */
void re15_spinnen_r38_spucken(re15_actor_t *e, int re2)
{
    if (!e) return;
    const re15_esp_t *bank = re15_esp_room_bank();
    if (!bank || re15_esp_find_id(bank, 0x0d) < 0) return;   /* Effekt nicht registriert: kein Spawn */
    static const int32_t ofs15[3] = { 500, 0, 0 };            /* DAT_80118ee8 */
    static const int32_t ofs2[3]  = { 1000, 0, -100 };        /* @0x80105910-24 */
    int32_t w[3];
    gift_anker(e, re2 ? ofs2 : ofs15, w);
    int16_t gier = re2 ? (int16_t)(e->rot_y + e->re2s_yaw21a) : e->rot_y;
    uint8_t vorher[RE15_ESP_FX_MAX];
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        vorher[i] = (re15_esp_fx_get(i) != NULL);
        if (!vorher[i]) s_gift_besitzer[i] = 0;               /* freier Platz: kein (veralteter) Absender */
    }
    /* Boden: das Original klemmt im Tick nicht (FUN_80019e20 ohne Bodenpruefung); den Boden
     * bestimmt Routine 25 selbst ueber FUN_8001c6e8 -> keine Port-Klemme (INT32_MAX). */
    (void)re15_esp_fx_spawn_rows(bank, 0x0d, 0x00, 0x2000, w[0], w[1], w[2], INT32_MAX, gier);
    uint8_t slot1 = (uint8_t)((e - g_actors) + 1);
    for (int i = 0; i < RE15_ESP_FX_MAX; i++)
        if (!vorher[i] && re15_esp_fx_get(i) != NULL) s_gift_besitzer[i] = slot1;
}

static uint16_t r16(const re15_esp_fx_t *f, int o) { return (uint16_t)(f->row[o] | (f->row[o + 1] << 8)); }
static void     w16(re15_esp_fx_t *f, int o, uint16_t v) { f->row[o] = (uint8_t)v; f->row[o + 1] = (uint8_t)(v >> 8); }

extern int re15_esp_kind_r38(const re15_esp_t *bank, uint32_t code, int16_t gier, const int32_t p[3]);

/* Routine 25 @0x80017fa4 (selbst disassembliert):
 *   +0x04 += 30, +0x06 += 30                              @0x80017fbc-d8 (Klumpen waechst)
 *   P = (s16) +0x28/+0x2a/+0x2c                           @0x80017fd0-f8
 *   s0 = FUN_8001c6e8(&P, 0, 8, 0x100)                    @0x80017ff4 (`addu a1,zero,zero`, `ori a2,zero,0x8`,
 *                                                          `ori a3,zero,0x100`)
 *   v0 = FUN_80012d60(500, &P, 0)                         @0x80018008 (`ori a0,zero,0x1f4`, `addu a2,zero,zero`)
 *   v0 != 0 ODER P.y >= (s16)s0 -> AUFSCHLAG:              @0x80018010-28
 *     FUN_800199d4(0x030c0000 | (+0x72 + 500), +0x2e, 0x80072d4c, &P)   @0x80018030-54
 *     FUN_80045024(0x03030001, &P)                        @0x8001805c-64
 *     +0x00 = +0x02 = 0, +0x08..+0x14 = 0, +0x6e = 0x0d    @0x80018078-98
 * GIFT (Beta -> Retail): der Spielerzweig von FUN_80012d60 wuerfelt das Gift-Bit nur scheinbar
 * (@0x80012e78-eb4); gemessen ist a0 = 0x0A an beiden `jal 0x8001af20` -> nie Gift (Dossier 1.4).
 * RE2 vergiftet bei JEDEM Gift-Treffer am Spieler: Op 32 @0x800200b0 — Treffertest FUN_80047664,
 * Ziel-Art < 0x10 (`sltiu v0,v0,0x10` @0x800200f8), `andi v0,v0,0x8` @0x80020118 (Spinnen-sub 8..11),
 * `sh v0(5400),-0x1f0(at)` @0x80020130 = 0x800cfe10 (Gift). Port: dasselbe Ereignis (Klumpen trifft
 * den Spielerkasten) setzt das RE1.5-Gift-Bit 0x2 von 0x800acaec (= status_flags), dessen Wirkung
 * (re15_spinnen_r38_gift_tick) und Heilung (Gegengift 0x23 / Blau 0x26, @0x8004af8c) RE1.5 sind.
 * NUTZER-VORGABE (AUFTRAG Z. 69: "das Gift ist das einzige das die Charaktere vergiften kann"). */
static void routine25(re15_esp_fx_t *f)
{
    w16(f, 0x04, (uint16_t)(r16(f, 0x04) + 30u));             /* @0x80017fbc-c8 */
    w16(f, 0x06, (uint16_t)(r16(f, 0x06) + 30u));             /* @0x80017fcc-d8 */
    int32_t P[3] = { f->wpos[0], f->wpos[1], f->wpos[2] };    /* `lh a2,40` / `lh 42` / `lh 44` */
    int16_t boden = re15_collision_room_coll(g_room_rdt_ok ? &g_room_rdt : NULL,
                                             P[0], P[2], 0, 8, 0x100u);
    re15_attack_box_t box;
    box.x = P[0]; box.y = P[1]; box.z = P[2]; box.radius = 500;   /* `ori a0,zero,0x1f4` @0x80017ffc */
    int idx = gift_platz_index(f);
    int besitzer = (idx >= 0) ? (int)s_gift_besitzer[idx] - 1 : -1;
    /* Gift NUR fuer den Klumpen einer SPINNE: RE2 Op 32 vergiftet nur bei Spinnen-sub 8..11 (`andi v0,v0,0x8`
     * @0x80020118); Effekt 0x0d steht in 41 Raum-ESPs (auch ROOM1260/4001/50x0, Birkins Hieb-Effekt
     * 0x0d002000 @0x801146d4-ec) — dort bleibt Routine 25 der reine Original-Treffer (10 HP, Wuerfel tot). */
    /* Fortsetzung 2: Gift nur, wenn der Treffer den Spieler auch ERREICHT. RE2 Op 32 vergiftet nur, wenn der
     * Applier FUN_80047664 den Spieler zurueckgibt (`jal 0x80047664` @0x800200e0, `beq v0,zero` @0x800200e8);
     * der ueberspringt ihn bei +0x1D3 != 0 (Schutz-/Griffzaehler, `lbu v0,467(s0) / bne` @0x800476ec-f4) und
     * HP < 0 (`lh v0,342(s0) / bltz` @0x800476fc-704). Port: dieselben Tore wie der Spielerzweig von
     * FUN_80012d60 (+0x93 & 1 `hit-once` @0x80012e24-30) und der Babybiss (+0x1D3 & 0x80 =
     * re15_player_is_grabbed @0x80100E84) — VOR dem Resolver gelesen, der +0x93 selbst setzt. */
    re15_actor_t *spl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Nachbesserung 1, Mangel 3 (Runde 38 Spur B, NUTZER-VORGABE "nicht angreifbar, solange die Szene ihn haelt"):
     * Routine 25 laeuft im ESP-Takt AUSSERHALB von re15_enemy_ai_run_all, Spur Bs Sicht (je geschuetztem Gegner-Tick)
     * erfasst sie nicht. Haelt die Szene den Spieler (re15_szenenschutz_haelt), sieht der Klumpen ihn durch DASSELBE
     * Original-Tor wie Spur Bs Gegner: +0x93 Bit 0 — gelesen vom Resolver FUN_80012d60 (`lbu v0,player.hit_react`
     * @0x80012e24 / `andi v0,v0,0x1` / `bne` @0x80012e30: kein HP-Abzug, kein Kommando 2) und vom Gift-Tor `frei`
     * unten (RE2-Gegenstueck FUN_80047664 +0x1D3 @0x800476ec-f4). Danach steht wieder der echte Wert (wie
     * szenenschutz_r38.c abschliessen). Jeder Klumpen ist ein FREIER Angriff: gespuckt wird nur im Zustand 1 (RE1.5
     * Unterzustand 7 @0x80112770, RE2 Unterzustand 8 @0x8010101C), kein SCD-Skript spawnt Effekt 0x0d (Zensus Abnahme 0,
     * alle ROOM*.RDT) -> keine "Teil der Szene"-Ausnahme. */
    const uint8_t hr_echt = spl->hit_react;
    const int szene = re15_szenenschutz_haelt();
    if (szene) spl->hit_react = (uint8_t)(hr_echt | 0x1u);
    const int frei = !(spl->hit_react & 0x1u) && !re15_player_is_grabbed() && spl->hp >= 0;
    if (szene && re15_hitbox_test(spl, &box))
        fprintf(stderr, "[szenenschutz] F%u Giftklumpen abgefangen (Absender Slot %d, Spieler HP %d)\n",
                (unsigned)g_engine.frame_count, besitzer, (int)spl->hp);
    if (besitzer > 0 && besitzer < RE15_ACTOR_MAX && g_actors[besitzer].type == 0x25 && frei &&
        re15_hitbox_test(&g_actors[RE15_ACTOR_SLOT_PLAYER], &box)) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags |= 0x2u;   /* Gift (s.o.) */
        s_gift_treffer++;
        { extern void re15_zustand_r39_gift_setzen(void);          /* Runde 39 Spur F: Takt-Zaehler := 30 wie */
          re15_zustand_r39_gift_setzen(); }                        /* RE2 Op 32 @0x80020138-40 (0x800cfe12) */
    }
    { extern int re15_giftkugel_r41_treffer(re15_actor_t *, const re15_attack_box_t *, int);   /* Runde 41 Spur D: RE2-Giftkugel */
      (void)re15_giftkugel_r41_treffer(spl, &box, besitzer); }   /* 20 HP @0x800200dc, Kommando 2/3 wie FUN_80047664; setzt +0x93 -> der Spielerzweig unten zieht nicht noch einmal ab */
    int treffer = re15_resolve_attack(&box, 0, besitzer);     /* FUN_80012d60(500,&P,0) @0x80018008 */
    if (szene) spl->hit_react = hr_echt;                      /* echter +0x93 zurueck (die Sicht gilt nur hier) */
    if (treffer == 0 && P[1] < (int32_t)boden) return;        /* `slt v1,v1,v0 / bne` @0x80018024-28 */
    /* AUFSCHLAG */
    (void)re15_esp_kind_r38(f->bank, 0x030c0000u | (uint16_t)(f->scale16 + 500u), f->param, P);
    if (re15_esp_se_hook) re15_esp_se_hook(0x03030001u, P);   /* `lui a0,0x303 / ori 0x1` @0x8001805c-60 */
    w16(f, 0x00, 0); w16(f, 0x02, 0);                         /* @0x8001807c / @0x80018078 */
    w16(f, 0x08, 0); w16(f, 0x0a, 0); w16(f, 0x0c, 0);       /* @0x80018094/90/8c */
    w16(f, 0x10, 0); w16(f, 0x12, 0); w16(f, 0x14, 0);       /* @0x80018088/84/80 */
    f->accel_x = f->accel_y = f->accel_z = 0;                 /* Port-Physikfelder = dieselben Bytes */
    f->drift_x = f->drift_y = f->drift_z = 0;
    f->frame = (int16_t)(0x0d - 1);                           /* +0x6e := 0x0d @0x80018098; Port-Konvention
                                                               * wie Routine 5/10 (-1: der Anim-Schritt
                                                               * landet AUF dem Satz) */
    f->timer = 0;
    if (idx >= 0) s_gift_besitzer[idx] = 0;
}

/* Routine 24 @0x80017f50: row[0x0e] != 0 -> -1 (`lhu 14 / addiu -1 / sh 14` @0x80017f70-80), sonst
 * Vorschub FUN_800174e4 (@0x80017f84); danach IMMER Routine 25 (`jal 0x80017fa4` @0x80017f8c). */
static void routine24(re15_esp_fx_t *f)
{
    uint16_t c = r16(f, 0x0e);
    if (c != 0) w16(f, 0x0e, (uint16_t)(c - 1u));
    else        re15_esp_fx_zeile_weiter(f);
    routine25(f);
}

int re15_spinnen_r38_routine_a(re15_esp_fx_t *f)
{
    if (!f || !f->rows_base) return 0;
    uint16_t a = r16(f, 0x00);
    if (a == 24) { routine24(f); return 1; }                  /* 0x80071d40[24] = 0x80017f50 */
    if (a == 25) { routine25(f); return 1; }                  /* 0x80071d40[25] = 0x80017fa4 */
    return 0;
}

/* Gift-Wirkung, Spieler-Kommando 1 (Handler 0x80031de8 = 0x80073f90[1]), selbst disassembliert:
 *   80031e44 lhu a0,0x800acaec ; andi v0,a0,0x7 ; beq -> Ende        (kein Gift)
 *   80031e5c lbu v1,0x800acc27 ; addiu v0,v1,255 ; sb v0             (Zaehler - 1)
 *   80031e74 bne v1,zero -> Ende ; Delay `ori v0,zero,0x78`           (lief noch)
 *   80031e7c sb 0x78 ; andi v1,a0,0x4 ; beq ; Delay `ori v0,zero,0x8` ; sb 8   (0x78 bzw. 8)
 *   80031e98 lhu HP ; addiu -2 ; sh ; sll 16 ; bgez -> Ende            (HP - 2)
 *   80031ebc bne v1,zero -> Ende ; Delay `ori v0,zero,0x1` ; sh 1      (nie toedlich ohne Bit 0x4)
 * Periode also 121 Bilder (Zaehler 0x78 .. 0, dann wieder Abzug).
 * Im ganzen Bereich 0x80031e44-0x80031ec8 stehen NUR diese Stores (0x800acc27 @0x80031e70/80/94, HP @0x80031eac/ec8):
 * kein `jal` (kein SE), kein Store auf das Kommandowort 0x800aca58 oder +0x93 — der Gift-Takt ist KEIN Treffer
 * (eine Treffer-Reaktion schreibt im Original nur ein Kommando-2-Schreiber, z.B. `sb v0(=2),4(s1)` @0x80012ebc im
 * Resolver FUN_80012d60). RE2 ebenso (@0x8003c12c-180: nur 0x800cfe12 und HP +0x156).
 * Rueckgabe (Nachbesserung 1): der HP-Abzug dieses Takts (0 ohne Abzug), damit der Aufrufer ihn aus dem Port-Detektor
 * "HP gefallen -> Treffer" (game_step_common.c s_prev_hp) herausnimmt. */
int re15_spinnen_r38_gift_tick(re15_actor_t *pl)
{
    if (!pl) return 0;
    uint16_t a0 = pl->status_flags;
    if ((a0 & 7u) == 0) return 0;                             /* @0x80031e50-54 */
    uint8_t v1 = s_gift_zaehler;
    s_gift_zaehler = (uint8_t)(v1 + 255u);                    /* @0x80031e68-70 */
    if (v1 != 0) return 0;                                    /* @0x80031e74 */
    s_gift_zaehler = (a0 & 4u) ? 8u : 0x78u;                  /* @0x80031e78-94 */
    const int16_t vor = pl->hp;
    int16_t hp = (int16_t)(pl->hp - 2);                       /* @0x80031e9c-ac */
    pl->hp = hp;
    if (hp < 0 && !(a0 & 4u)) pl->hp = 1;                     /* @0x80031eb4-c8 */
    return (int)vor - (int)pl->hp;
}

/* Kommando 0 (Handler 0x800318f8 = Tabelle 0x80073f90[0]; laeuft nach jedem Kommando-Nullen: Raumwechsel und
 * Inventar-Ende `sb zero,0x800aca58` @0x8001cbdc, Spieler-Laden `sw zero,0x800aca58` @0x80031518) nullt den
 * Gift-Zaehler bedingungslos: `sb zero,-13273(at)` @0x8003196c (0x800acc27, zwischen +0x93 @0x80031964 und
 * 0x800acc0c @0x80031974; einziger weiterer Schreiber ausser dem Takt selbst, ghidra1_V2 XREF[5]). Der erste Takt
 * danach kommt deshalb sofort (@0x80031e74). Port: re15_player_cmd_zero (game_step_common.c). Abnahme 0 Hinweis 1. */
void re15_spinnen_r38_gift_cmd0(void) { s_gift_zaehler = 0; }

/* ============================================================================================
 * Teil 3 — RE2-MINISPINNEN: das ANIMATIONSPAAR der Babybank (EM026)
 * ============================================================================================
 * GEMESSEN (Port, Dossier 4.3): die Babys stehen logisch auf dem Boden (y = +0x1C2), werden aber 1166..1797
 * DARUEBER gezeichnet (Framedump ROOM2030: eine Kette brauner Klumpen auf Gelaenderhoehe neben Leon). Die
 * Port-Bank nahm fuer kind 0x26 nach der Regel "Paar mit den meisten Clips" (re2_ems.c re2_emd_parse_bank)
 * PAAR 3 (EMD dir[5]/dir[6], 3 Clips 14/36/20 Bilder, 80-Byte-Keyframes): Wurzel (0,-1797,0) in Keyframe 0,
 * -1797..-1585 ueber alle 70 Keyframes, Vertex-Y -2072..-1166 (Sonde, Dossier 2.6).
 * ORIGINAL (RE2 Retail, selbst disassembliert):
 *   Binder PSX.EXE  8001abb0  sw v1,380(s1)   ; dir[1] (EDD Paar 1) -> +0x17C
 *                   8001abc0  sw v1,264(s1)   ; dir[2] (EMR Paar 1) -> +0x108
 *                   8001abd0/abe0/abf0/ac00   ; dir[3..6] -> +0x184/+0x180/+0x18C/+0x188
 *   EMS26-Wurzel    8010005c  lw a1,264(s0)   ; +0x108
 *                   80100060  lw a2,380(s0)   ; +0x17C  -> Zustandstabelle @0x80101084 (jalr @0x80100078)
 *   alle fuenf `jal 0x8002959c` (Animations-Treiber) im Modul bekommen genau diese a1/a2 (@0x801004ec-f4,
 *   @0x80100854-5c, ...); Vollscan EMS26.BIN (1086 Worte): KEIN anderer Zugriff auf +0x108/+0x17C und keiner
 *   auf +0x180..+0x18C. Einziger Clip-Schreiber: `lui v0,0xf` @0x8010011c / `sw v0,332(s0)` @0x80100120
 *   (+0x14C = 0x000F0000 = Clip 0).
 * => das RE2-Baby spielt IMMER Paar 1 Clip 0 (EDD: 2 Bilder, Keyframes 0/1, Wurzel (0,0,0), Vertex-Y -275..0
 *    = auf dem Boden). Paar 3 ist die Opfer-Bank (Wurzel-Y -1797 = Brusthoehe des Spielers) und wird vom
 *    Baby nie abgespielt. */
void re15_spinnen_r38_re2_bank(int kind, re15_enemy_bank_t *eb)
{
    if (!eb || kind != 0x26 || !eb->loco_ok) return;
    eb->skel = eb->skel_loco;                                  /* +0x108 = dir[2] @0x8001abc0 */
    eb->anim = eb->anim_loco;                                  /* +0x17C = dir[1] @0x8001abb0 */
}

/* ============================================================================================
 * Teil 3b - RE2-MINISPINNEN: der WURZEL-SCHWANZ des Babys (Schub + Wandpass)
 * ============================================================================================
 * GEMESSEN (Fortsetzung 2026-10-17, echte exe, ROOM2030, RE15_SPINNEN_LOG mit der UNABHAENGIGEN
 * Bodengroesse bu = hoechste SCA-Bodenflaeche auf/unter der Figur): 9 von 9 Babys verlassen den Steg,
 * in 14485 von 20025 Baby-Bildern liegt ihre Lage-Y (-5400) UEBER dem Boden unter ihnen (0); sie
 * laufen durch Gelaender und Waende (Lagen bis (42446,-5400,50409), weit ausserhalb des Raums) und
 * schweben ueber der unteren Ebene. Der Port rief fuer das RE2-Baby weder Schub noch Wandklemme (Zweig
 * 0x26 in re15_enemy_ai_run_all: "does NOT translate -> no wall-clamp" = der RE1.5-Feuer-Emitter).
 * ORIGINAL (RE2 EMS26.BIN, Wurzel @0x8010001C, selbst disassembliert):
 *   80100094  lw   a0,0(s0)             ; word0
 *   801000ac  beq  a0,zero,0x801000c8   ; geloescht -> fertig
 *   801000b4  jal  0x80035530           ; SCHUB aus allen anderen Entities (a0 = self)
 *   801000c0  jal  0x8003567c           ; WANDPASS
 *   801000c4  addiu a1,zero,1024        ; Maske 0x400
 *
 * SCHUB FUN_80035530 (RE2 PSX.EXE): FUN_80035408(self, self+56) (Kugeln aus der Lage), +0xD = 255, dann je
 *   Eintrag der Entity-Liste ab 0x800cfe14 (`addiu s0,s0,-492` @0x80035560) ausser self (`beq s1,a0`
 *   @0x80035574) mit word0 & 1 (@0x8003557c-88): `jal 0x80034d0c` (a0 = andere, a1 = self) @0x80035590.
 * FUN_80034d0c(A, B) verschiebt B (self) aus der Kugel von A (je EINE Kugel: +0x1E8 = 1 am Spieler
 *   `sw v0,488(s2)` @0x8003bddc und am Baby @0x80100164):
 *   80034d48-50  (A.w0 | B.w0) & 2 -> nichts;  80034d58-60  (A.w0 & B.w0) & 0x1000 -> nichts
 *   80034d68-6c  B.w0 & 4 -> nichts
 *   80034de8     R = A.r (+0x9A) + B.r;  dx = B.x - A.x (@0x80034dec), dz analog (@0x80034e10)
 *   80034df4-fc / 80034e14-1c  `sltu (2R),(d+R)` -> zu weit
 *   80034e38     dist = SquareRoot0(dx*dx + dz*dz) (`jal 0x8008d2f4`); pen = R - dist (@0x80034e44), <= 0 -> nichts
 *   80034e50-78  dy = (B.y + B.ofs) - (A.y + A.ofs) (ofs = +0x98), hs = A.h + B.h (h = +0x9E): -hs < dy < hs
 *   80034e7c-f00 a3 = dx*pen / (dist+1), a2 = dz*pen / (dist+1)
 *   80034ec4-f10 Vor-Bild-Hoehe (B+0x46 + ofs) ausserhalb -hs..hs -> Kreuzungsausstoss je Achse
 *                (@0x80034f18-fac / @0x80034fb0-35044): A zwischen Vor- und Jetzt-Lage ->
 *                a = (+-2*A.r) - (-(d*pen)/(dist+1))
 *   80035048-a8  A.w0 & 0x100000 -> Kappe +-100 (kein Port-Bit: alle Port-Figuren 0)
 *   800350b8-cc  `sw v0,56(s1)` / `sw v1,64(s1)`: B.x += a3, B.z += a2
 * Kugeln (Radius +0x9A, Versatz +0x98, Halbhoehe +0x9E), alle selbst gelesen:
 *   Spieler 450 / -1530 / 1530: `addiu v1,zero,450 / sh v1,154(s2)` @0x8003bdc0-c4, `addiu v0,zero,-1530 /
 *     sh v0,152(s2)` @0x8003bde0-e4, `addiu v0,zero,1530 / sh v0,158(s2)` @0x8003bde8-ec
 *   Baby 10 / -10 / 10: `addiu v0,zero,-10 / sh v0,152(s0)` @0x80100168-6c, `addiu v0,zero,10 / sh v0,154(s0)
 *     / sh v0,158(s0)` @0x80100170-78
 *   Adult (RE2-Gehirn) 1000 / +0x98 / +0x9E: alle drei Adult-Tabellen @0x801063A0/@0x801063C0/@0x801063E0
 *     tragen +0x9A = 1000; +0x98/+0x9E fuehrt der Port (re2_hit_b98/re2_hit_h9e)
 * word0 & 2: Adult im Tod `ori v0,v0,0x42` @0x80103df4 / @0x801044a0 (Port: Zustand 3 DEATH / 7 CORPSE);
 *   Baby im Fall `ori v1,v1,0x2` @0x8010078c, geloescht bei der Landung `addiu v1,zero,-3 / and` @0x801007dc-ec
 *   (Port: Zustand 1 Unterzustand 2 = FALL).
 * PORT-ZUORDNUNG: die Entity-Liste = g_actors (Spieler-Platz zuerst, dann die Plaetze); andere Figurentypen
 *   (keine RE2-Kugel im Port) mit ihrem RE1.5-Kasten (hit_radius_min / hit_offset_y / hit_height), wenn sie einen
 *   tragen und keine Leiche sind - dieselbe Zuordnung wie re15_enemy_body_push_tail; der tote Spieler (hp < 0)
 *   schiebt nicht (wie re15_body_push_player).
 *
 * WANDPASS FUN_8003567C (RE2 PSX.EXE): `sw zero,272(s0)` @0x8003569c (+0x110 = 0); word0 & 8 -> fertig
 *   (`andi v0,v1,0x8 / bne` @0x80035694-98); +0x1E4 != 0 -> fertig (@0x800356a0-a8); sonst
 *   FUN_8004c1bc(self+0x84, `lhu a1,144(s0)` = +0x90, `sllv a2,1,+0x106` = Bandbit, 0x400|0x01000000)
 *   `jal 0x8004c1bc` @0x800356d0 -> +0x110. Radius +0x90 = 10: Baby-INIT `addiu v0,zero,10`
 *   @0x80100170 / `sh v0,144(s0)` @0x80100180. Band +0x106 = (-Y)/1800 (@0x80100080-b0).
 * PORT-ZUORDNUNG (dieselbe wie Hund/Kraehe/Adult, enemy_ai_common.c "EIN Klemmpfad fuer BEIDE Flavors"):
 *   FUN_8004c1bc liest RE2-SCA-Daten, die der Port nicht hat; die Klemme laeuft auf den RE1.5-Raumdaten
 *   ueber re15_collision_constrain_enemy (Band aus Y, Solid-Maske 4 = RE2 0x400). +0x1E4 hat am Baby
 *   keinen Schreiber im Port (memset beim Allokieren) -> 0. */
typedef struct { int32_t r, ofs, h; } r38_kugel_t;

static const r38_kugel_t k_kugel_spieler = { 450, -1530, 1530 };   /* @0x8003bdc0-ec */
static const r38_kugel_t k_kugel_baby    = {  10,   -10,   10 };   /* @0x80100168-78 */
#define R38_ADULT_KUGEL_R 1000                                       /* +0x9A @0x801063A0/C0/E0 */

/* Kugel und word0 & 2 der anderen Figur A (s.o.). Rueckgabe 0 = A schiebt nicht. */
static int kugel_von(const re15_actor_t *A, int slot, r38_kugel_t *k)
{
    if (slot == RE15_ACTOR_SLOT_PLAYER) { *k = k_kugel_spieler; return 1; }
    if (A->type == 0x26 && re15_re2spider_baby_owns(A)) {
        if (A->state == 1 && A->sub_state_1 == 2) return 0;          /* FALL: word0 |= 2 @0x8010078c */
        *k = k_kugel_baby; return 1;
    }
    if (A->type == 0x25 && re15_ai_re2_for_type(A->type) && re15_re2spider_owns(A)) {
        if (A->state == 3 || A->state == 7) return 0;                /* word0 |= 0x42 @0x80103df4/@0x801044a0 */
        k->r = R38_ADULT_KUGEL_R; k->ofs = A->re2_hit_b98; k->h = A->re2_hit_h9e; return 1;
    }
    if (A->hit_radius_min == 0 || A->state == (uint8_t)RE15_AI_STATE_CORPSE) return 0;   /* PORT-ZUORDNUNG */
    k->r = (int32_t)A->hit_radius_min; k->ofs = A->hit_offset_y; k->h = (int32_t)A->hit_height;
    return 1;
}

/* Kreuzungsausstoss einer Achse (@0x80034f18-fac fuer x, @0x80034fb0-35044 fuer z). */
static int32_t kreuzung(int32_t a, int32_t d, int32_t pen, int32_t dist, int32_t ar,
                        int32_t vor_b, int32_t ax, int32_t bx)
{
    if (!((vor_b < ax && ax < bx) || (ax < vor_b && bx < ax))) return a;   /* `slt` @0x80034f24-5c */
    int32_t t = (int32_t)(0u - (uint32_t)(d * pen)) / (dist + 1);         /* `subu v0,zero,t2 / div` */
    int32_t v = (t > 0) ? (ar << 1) : -(ar << 1);                         /* `bgtz` / `sll v0,s7,1` / `subu` */
    return v - t;                                                         /* `subu a3,v0,a3` @0x80034fac */
}

static void kugel_schub(const re15_actor_t *A, r38_kugel_t ka, re15_actor_t *B, r38_kugel_t kb,
                        int32_t vor_x, int32_t vor_y, int32_t vor_z)
{
    const int32_t R  = ka.r + kb.r;                                       /* @0x80034de8 */
    const int32_t dx = B->x - A->x;                                       /* @0x80034dec */
    if ((uint32_t)(R << 1) < (uint32_t)(dx + R)) return;                  /* @0x80034df4-fc */
    const int32_t dz = B->z - A->z;                                       /* @0x80034e10 */
    if ((uint32_t)(R << 1) < (uint32_t)(dz + R)) return;                  /* @0x80034e14-1c */
    const int32_t dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));   /* @0x80034e38 */
    const int32_t pen  = R - dist;                                        /* @0x80034e44 */
    if (pen <= 0) return;                                                 /* `blez` @0x80034e48 */
    const int32_t hs = ka.h + kb.h;                                       /* @0x80034e64 */
    const int32_t ay = A->y + ka.ofs;
    const int32_t dy = (B->y + kb.ofs) - ay;                              /* @0x80034e54-60 */
    if (!(-hs < dy) || !(dy < hs)) return;                                /* @0x80034e6c-78 */
    int32_t a3 = (dx * pen) / (dist + 1);                                 /* @0x80034e7c-eb4 */
    int32_t a2 = (dz * pen) / (dist + 1);                                 /* @0x80034ec0-f00 */
    const int32_t vdy = (vor_y + kb.ofs) - ay;                            /* `lh 20(s2)` + `lh 70(s1)` @0x80034ec4-d4 */
    if (!(-hs < vdy && vdy < hs)) {                                       /* @0x80034f04-10 */
        a3 = kreuzung(a3, dx, pen, dist, ka.r, vor_x, A->x, B->x);         /* +0x44 `lh a0,68(s1)` */
        a2 = kreuzung(a2, dz, pen, dist, ka.r, vor_z, A->z, B->z);         /* +0x48 `lh a0,72(s1)` */
    }
    B->x += a3;                                                           /* `sw v0,56(s1)` @0x800350c8 */
    B->z += a2;                                                           /* `sw v1,64(s1)` @0x800350cc */
}

void re15_spinnen_r38_baby_schwanz(re15_actor_t *e, int32_t ox, int32_t oy, int32_t oz)
{
    if (!e || !e->active) return;                             /* `beq a0,zero,0x801000c8` @0x801000ac */
    /* SCHUB FUN_80035530 @0x801000b4 - B.w0 & 2 (eigener Fall @0x8010078c) sperrt jedes Paar (@0x80034d48-50) */
    if (!(e->state == 1 && e->sub_state_1 == 2)) {
        for (int s = RE15_ACTOR_SLOT_PLAYER; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *A = &g_actors[s];
            if (A == e || !A->active) continue;               /* `beq s1,a0` @0x80035574, word0 & 1 @0x8003557c-88 */
            if (s == RE15_ACTOR_SLOT_PLAYER && A->hp < 0) continue;
            r38_kugel_t ka;
            if (!kugel_von(A, s, &ka)) continue;
            kugel_schub(A, ka, e, k_kugel_baby, ox, oy, oz);  /* `jal 0x80034d0c` @0x80035590 */
        }
    }
    /* WANDPASS FUN_8003567C(self, 0x400) @0x801000c0 */
    if (e->flags & 0x8u) return;                              /* word0 & 8 @0x80035694-98 */
    if (!g_room_rdt_ok) return;
    int32_t nx = e->x, nz = e->z;
    re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                   10,                        /* +0x90 = 10 @0x80100170/@0x80100180 */
                                   e->y, 4u);                 /* Band (-Y)/1800, Maske 0x400 -> 4 */
    e->x = nx; e->z = nz;
}

/* ============================================================================================
 * Teil 4 — OBJEKT-AUSSCHIEBUNG (der Aktor-Durchgang des Objekt-Ticks FUN_8002bd44)
 * ============================================================================================
 * GEMESSEN (DuckStation-GDB, R_belege/orig_2060_klemme.txt + orig_2060_watch.txt): die Schachtspinne
 * von ROOM2060 wird beim Herauskriechen NACH ihrer Wurzel 0x801109e4 verschoben — Bild 66 -> 67
 * z 5400 -> 5525, Bild 67 -> 68 x 5800 -> 5975 (alle sechs Aufrufe der Wurzel lassen x/z stehen). Der
 * Schreib-Wachpunkt auf +0x3c trifft pc 0x8002cde0 mit ra 0x8002be48 = FUN_8002cabc, gerufen aus dem
 * Objekt-Tick FUN_8002bd44 (Bild-Reihenfolge @0x8001ce04 Gegner -> @0x8001ce0c Spieler -> @0x8001ce14
 * Objekte):
 *   8002bd78  lbu  s3,2(v0)            ; Objektzahl = RDT-Kopf +2 (nOmodel)
 *   8002bdb0-c8                        ; s2 = 0x800b3f98 + 148*(s3-1): Objekte von HINTEN nach vorn
 *   8002bdcc-d8  lw v0,0(s2) / andi 1  ; nur aktive
 *   8002be14  lbu  s1,0(0x800aca4e)    ; Aktorzahl, s0 = 0x800acc2c (Stride 0x1f4)
 *   8002be24-30  lw v0,0(s0) / andi 1  ; nur aktive Aktoren, KEIN Typ-Filter
 *   8002be40  jal  0x8002cabc          ; a0 = Aktor, a1 = Objekt, a2 = 0 (Delay `addu a2,zero,zero`)
 * Das Objekt ist das SCHACHTGITTER obj 0 (5775,-4860,5325), Obj_model_set mit NULL-Kasten. FUN_8002cabc
 * hat keine Null-Kasten-Ausnahme: t5 = Objekt-hx + Aktor-hx = 0 + 200 (Kasten C) macht das Gitter zum
 * Punkt-Hindernis. Nachgerechnet: Bild 67 dz = 5325-5400 = -75, dx = -25 -> |dx*200| < |dz*200| ->
 * z = 5325 + 200 = 5525; Bild 68 dz = 0 -> x = 5775 + 200 = 5975 (beides gemessen). ROOM20A0
 * (orig_20a0_schacht.txt): Gitter obj 2 (22532,-6610,-27050) -> z -27250, x 22732, ebenso.
 *
 * FUN_8002cabc mit a2 = 0, selbst disassembliert (PSX.EXE @0x8002cabc-cdf0):
 *   8002cad0-dc  Aktor-Wort0 & 0x40 -> 0          8002cae4-f0  Objekt-Wort0 & 2 -> 0
 *   8002caf8-cb38 t8 = (objbox.cx + obj.x) - (akt+0x7c.x + akt.x), t7 analog z
 *   8002cb3c-4c  t5 = objbox.hx + aktbox.hx, t4 = objbox.hz + aktbox.hx (Aktor +6 fuer BEIDE Achsen)
 *   8002cb50-70  `sltu (2*t5),(t8+t5)` / `sltu (2*t4),(t7+t4)` -> keine Ueberlappung -> 0
 *   8002cb84-c8  t6 = (objbox.cy + obj.y) - (aktbox.cy + akt.y), a1 = aktbox.hy + objbox.hy, Y-Test
 *   8002cbcc-cc38 Aktor+0xa := Objekt-Index, Objekt+0xa := Aktor-Index (Buchhaltung; KEIN Leser im
 *                Spinnen-Code: Vollscan lb/lbu/sb mit Versatz 10/11 ueber 0x801109e4-0x801158a0 = 0)
 *   8002cc3c-70  Vor-Hoehe aus den Spiegeln +0x42 (lh) von Objekt und Aktor: lag der Aktor VORHER
 *                nicht in der Hoehe des Objekts -> Ausschiebung in Y (@0x8002ccb4-e4), sonst x/z
 *   8002cce8-d0c |t8*t4| gegen |t7*t5|: `slt` -> x (@0x8002cd50-7c), sonst z (@0x8002cdb8-dc)
 * Die Lage-Spiegel +0x40/+0x42/+0x44: Sce_em_set setzt sie auf die Spawnlage (@0x800421a0-b8), danach
 * schreibt sie die Zeichen-Schleife je Bild NACH dem Objekt-Tick (`sh` @0x8001d124/@0x8001d13c/
 * @0x8001d154; gemessen: Wachpunkt auf +0x40 trifft pc 0x8001d128 mit der schon geklemmten Lage).
 * PORT-ZUORDNUNG Objekt-Spiegel +0x42: geschrieben am Ende jedes Objekt-Durchgangs (`sh v1,0x42(s2)`
 * @0x8002c0f8) = die Objekt-Hoehe; der Port fuehrt keinen Objekt-Spiegel und nimmt die Objekt-Hoehe
 * selbst (gleich fuer jedes Objekt, das seine Hoehe nicht aendert — die Schachtgitter und Kisten). */
static int32_t mflo_abs(int32_t a, int32_t b)
{
    int32_t p = (int32_t)(uint32_t)((int64_t)a * (int64_t)b);   /* `mult` / `mflo` (untere 32 Bit) */
    return (p < 0) ? (int32_t)(0u - (uint32_t)p) : p;          /* `bgez` / `negu` @0x8002ccf0-08 */
}

static void objekt_schub(re15_actor_t *e, int i)
{
    if (g_scd.props[i].flags & 0x2u) return;                     /* @0x8002cae4-f0 */
    const int32_t  ox  = g_scd.props[i].x, oy = g_scd.props[i].y, oz = g_scd.props[i].z;
    const int32_t  ocx = g_scd.props[i].box_cx, ocy = g_scd.props[i].box_cy, ocz = g_scd.props[i].box_cz;
    const uint32_t ohx = (uint16_t)g_scd.props[i].box_hx, ohy = (uint16_t)g_scd.props[i].box_hy;
    const uint32_t ohz = (uint16_t)g_scd.props[i].box_hz;
    /* Aktor-Kasten +0x78 der Spinne: A {0,-900,0,1000,900,1000} @0x80118dd4 bzw. C {0,-1440,0,200,1440,200}
     * @0x80118dec — im Port re15_enemy_apply_hitbox / lauer_kasten (hit_offset_*, hit_radius_min = +6,
     * hit_height = +8). Der gedrehte Versatz +0x7c (FUN_8002b498 dreht Kasten-x/z um +0x6a) ist fuer beide
     * Kaesten 0, weil ihr x/z-Versatz 0 ist. */
    const int32_t  ecx = e->hit_offset_x, ecy = e->hit_offset_y, ecz = e->hit_offset_z;
    const uint32_t ehx = (uint16_t)e->hit_radius_min, ehy = (uint16_t)e->hit_height;
    const int32_t  t8 = (ocx + ox) - e->x;                       /* @0x8002cb0c-1c */
    const int32_t  t7 = (ocz + oz) - e->z;                       /* @0x8002cb20-38 */
    const uint32_t t5 = ohx + ehx;                               /* @0x8002cb3c-48 */
    const uint32_t t4 = ohz + ehx;                               /* @0x8002cb44-4c */
    if ((t5 << 1) < (uint32_t)(t8 + (int32_t)t5)) return;        /* @0x8002cb50-5c */
    if ((t4 << 1) < (uint32_t)(t7 + (int32_t)t4)) return;        /* @0x8002cb64-70 */
    const int32_t  t6 = (ocy + oy) - (ecy + e->y);               /* @0x8002cb84-a4 */
    const uint32_t a1 = ehy + ohy;                               /* @0x8002cb9c-a8 */
    if ((a1 << 1) < (uint32_t)(t6 + (int32_t)a1)) return;        /* @0x8002cbac-c8 */
    /* Vor-Hoehe @0x8002cc3c-70 (Objekt-Spiegel = Objekt-Hoehe, s. PORT-ZUORDNUNG oben) */
    const int32_t vor = ((int32_t)(int16_t)(uint16_t)oy + ocy)
                      - ((int32_t)(int16_t)e->pos_s_y + ecy) + (int32_t)a1;
    if ((a1 << 1) < (uint32_t)vor) {                             /* vorher nicht ueberlappt -> Y */
        const int32_t v = ocy + oy - ecy;                        /* @0x8002ccb4-c4 */
        e->y = (t6 >= 0) ? v - 1 - (int32_t)a1                   /* @0x8002ccd8-e4 */
                         : v + 1 + (int32_t)a1;                  /* @0x8002ccc8-d4 */
        return;
    }
    const int32_t kx = mflo_abs(t8, (int32_t)t4);                /* `mult t8,t4` @0x8002cc74 */
    const int32_t kz = mflo_abs(t7, (int32_t)t5);                /* `mult t7,t5` @0x8002ccec */
    if (kz < kx) {                                               /* `slt v0,v0,v1` @0x8002cd0c -> x */
        const int32_t v = ocx + ox - ecx;                        /* @0x8002cd50-64 */
        e->x = (t8 >= 0) ? v - (int32_t)t5 : v + (int32_t)t5;    /* @0x8002cd60-7c */
    } else {                                                     /* z @0x8002cd80 */
        const int32_t v = ocz + oz - ecz;                        /* @0x8002cdb8-cc */
        e->z = (t7 >= 0) ? v - (int32_t)t4 : v + (int32_t)t4;    /* @0x8002cdc8-dc */
    }
}

void re15_spinnen_r38_objekt_pass(re15_actor_t *e)
{
    if (!e || !e->active || e->type != 0x25) return;
    const int slot = (int)(e - g_actors);
    const int lief = (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_lauer_lief[slot] : 0;
    if (slot >= 0 && slot < RE15_ACTOR_MAX) s_lauer_lief[slot] = 0;
    /* RE1.5-Spinnencode bewegt die Spinne (RE1.5-KI, oder die RE1.5-Schacht-Choreografie im RE2-Modus)
     * -> der RE1.5-Objekt-Durchgang gilt. Das RE2-Gehirn nach der Landung: OFFEN (kein RE2-Beleg fuer
     * einen Objekt-Durchgang der RE2-Spinne gelesen; Dossier 6). */
    const int re2_gehirn = re15_ai_re2_for_type(e->type) && re15_re2spider_owns(e);
    if ((!re2_gehirn || lief) && g_room_rdt_ok && !(e->flags & 0x40u)) {   /* Aktor-Wort0 & 0x40 @0x8002cad0-dc */
        const int n_obj = (int)g_room_rdt.nOmodel;                      /* @0x8002bd78 */
        for (int oid = n_obj - 1; oid >= 0; oid--) {                     /* von hinten @0x8002bdb0-c8 */
            for (int i = 0; i < (int)g_scd.prop_count && i < RE15_SCD_MAX_PROPS; i++) {
                if (g_scd.props[i].obj_id != (uint8_t)oid || !g_scd.props[i].active) continue;   /* @0x8002bdcc-d8 */
                objekt_schub(e, i);
            }
        }
    }
    /* Lage-Spiegel nach dem Objekt-Tick (Zeichen-Schleife `sh` @0x8001d124/@0x8001d13c/@0x8001d154) */
    e->pos_s_x = (uint16_t)e->x;
    e->pos_s_y = (uint16_t)e->y;
    e->pos_s_z = (uint16_t)e->z;
}

/* ---- MESSSCHIENE RE15_SPINNEN_LOG ------------------------------------------------------------
 * Reiner Messhaken (env-gegated, kein Spielverhalten): je Bild und Spinne eine Zeile. Die GUI-exe
 * hat kein nutzbares stderr, deshalb eine Datei (Muster RE15_STATE_LOG, platform/pc/main.c). */
/* Messgroesse "bu": die hoechste SCA-Bodenflaeche AUF ODER UNTER der Figur — FUN_8001c6e8 mit dem Startband
 * aus der eigenen Hoehe ((-y + 1799) / 1800, Baender start-1 .. 0), damit eine Ebene UEBER der Figur (Steg,
 * Decke) nicht als ihr Boden zaehlt. Reine Messgroesse der Schiene (kein Spielverhalten). */
int16_t re15_spinnen_r38_boden_unter(const re15_actor_t *a)
{
    if (!a) return 0;
    int32_t sb = (-a->y + 1799) / 1800;                       /* Band b traegt die Oberkante -(b+1)*1800 */
    if (sb < 0) sb = 0;
    if (sb > 16) sb = 16;                                     /* Bandnibble 0..15 (`& 0xf002` @FUN_8001c6e8) */
    return re15_collision_room_coll(g_room_rdt_ok ? &g_room_rdt : NULL, a->x, a->z, 0, (int)sb, 0x100u);
}

void re15_spinnen_r38_log_frame(unsigned frame)
{
    static FILE *s_fp = NULL;
    static int   s_init = 0;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_SPINNEN_LOG");
        if (p && *p) s_fp = fopen(p, "w");
    }
    if (!s_fp) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* bo = SCA-Bodenhoehe unter der Figur, unabhaengig von ihrem eigenen Bodenfeld +0x1ba/+0x1C2:
     * FUN_8001c6e8(&P, 0, 8, 0x100) — derselbe Aufruf wie Routine 25 (@0x80017fe0-f4). Gegen
     * die SELBSTBESTAETIGENDE Pruefung "y == +0x1ba" (Fortsetzung 2026-10-17). */
    const re15_rdt_t *rdt = g_room_rdt_ok ? &g_room_rdt : NULL;
    /* Nachbesserung 1: pmo = Spieler-Clip +0x94, phf = Treffer-Reaktion (cmd-2-Flinch) laeuft — misst, ob ein Gift-Takt
     * eine Reaktion ausloest (darf er nicht, @0x80031e44-ec8). Hinter der PL-Klammer, damit alte Auswerter weiterlaufen. */
    fprintf(s_fp, "F%u R%04X PL(%d,%d,%d hp=%d st=%d ps=%04x fl=%d gz=%u bo=%d bu=%d) pmo=%d phf=%d fx=%d", frame,
            (unsigned)g_current_room_id, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->hp,
            (int)pl->state, (unsigned)pl->status_flags, (int)pl->floor,
            (unsigned)re15_zustand_r39_gift_zaehler(),   /* Runde 39 Spur F: gz = RE2-Takt-Zaehler 0x800cfe12 */
            (int)re15_collision_room_coll(rdt, pl->x, pl->z, 0, 8, 0x100u), (int)re15_spinnen_r38_boden_unter(pl),
            (int)pl->motion, re15_player_hit_flinch_active(), re15_esp_fx_count());
    /* Runde 38 Integration (versteckte Schachtspinne): pr = Spieler-Blick +0x6a (Port rot_y), zh = der Platz, den die
     * Zielhilfe FUN_8003703c mit dem Radius des Waffe-Hebens (`ori a0,zero,0x7530` @0x80032fac) JETZT waehlen wuerde
     * (re15_player_aim_target, reine Abfrage), -1 = kein Ziel. Hinter fx=, damit die bisherigen Auswerter
     * (ana_bo.py, test_r38_spinnen pruef_log) unveraendert lesen. Messgroesse, kein Spielverhalten. */
    {   extern int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);
        int32_t tx = 0, tz = 0; int zh = -1;
        if (re15_player_aim_target(0x7530, &tx, &tz))
            for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX && zh < 0; i++)
                if (g_actors[i].active && g_actors[i].x == tx && g_actors[i].z == tz) zh = i;
        fprintf(s_fp, " pr=%d zh=%d", (int)pl->rot_y, zh); }
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (!e->active || (e->type != 0x25 && e->type != 0x26)) continue;
        fprintf(s_fp, " [%d t=%02x st=%d s=%d.%d.%d g=%02x fl=%d m=%d gy=%d bo=%d bu=%d mo=%d af=%d"
                      " @(%d,%d,%d) r=(%d,%d,%d) hp=%d h93=%02x]",
                i, e->type, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                e->grid_id, e->floor, e->re2s_mode222, (int)e->dog_floor_y,
                (int)re15_collision_room_coll(rdt, e->x, e->z, 0, 8, 0x100u), (int)re15_spinnen_r38_boden_unter(e),
                e->motion,
                (int)e->anim_frame, (int)e->x, (int)e->y, (int)e->z,
                (int)e->rot_x, (int)e->rot_y, (int)e->rot_z, (int)e->hp, e->hit_react);
    }
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || !f->rows_base || f->effect_id != 0x0d) continue;
        fprintf(s_fp, " {fx%d A%u fl%02x w%u p%d,%d,%d an%d}", i,
                (unsigned)(f->row[0] | (f->row[1] << 8)), f->flags,
                (unsigned)(f->row[4] | (f->row[5] << 8)), f->wpos[0], f->wpos[1], f->wpos[2],
                (int)f->frame);
    }
    fputc('\n', s_fp);
    fflush(s_fp);
}
