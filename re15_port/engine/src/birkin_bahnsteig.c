/*
 * RE1.5 Rebuilt — Bahnsteig-Birkin: eigene Bewegungsablaeufe der Beta-Form (Kopf: include/re15_birkin_formen.h).
 *
 * ⛔ PORT-ERGAENZUNG, KEIN AUSLIEFERUNGSCODE (Runde 41 Spur R, Nachtrag 38). Modell: tools/birkin_bahnsteig.py
 * (RE2-G4 kind 0x34, umgeformt). Abfolge und Clips stammen aus dem G4-Overlay, CDEMD0.EMS rec 1 (Sektor 3238,
 * 0xB2D4 Byte, gelinkt @0x80100000); alle @0x8010xxxx unten sind Adressen darin.
 *
 * Dispatch im Original: Hauptzustand +0x04 @0x8010af60 {0x8010062c Ctor, 0x80100bbc AKTIV, 0x80106d3c TREFFER,
 * 0x801076ec TOD}; AKTIV verteilt +0x05 auf Entscheiden @0x8010b020[sub] und Ausfuehren @0x8010b068[sub].
 * Dieses Modul fuehrt dieselben Sub-Nummern (e->sub_state_1), damit jede Stelle im Overlay wiederzufinden ist:
 *   0  AUFTRITT   Schwanz der Verwandlungskette 0x80100d88: Clip 10, dann Clip 5
 *   2  HIEB       Clip 3 oder 4 (Seite +0x16a), Doppelschlag, bei Treffer Clip 9
 *   3  GEHEN      Clip 5, die Entscheidungs-Nabe 0x80102370 (Schritt-Ton in den EDD-Flags Bild 43/73)
 *   4  RENNEN     Clip 0, Treffer durch Ueberrennen
 *   8  WEICHEN    Gang rueckwaerts, 40 Bilder (RE2 Clip 2, hier Clip 5, s.u.)
 *   14 BISS       Clip 26
 *   15 GRIFF      Clip 27
 *   20 ZUCKEN     Port-Nummer fuer den Treffer-Zweig: Clip 8 ab Bild 5 (@0x80107224)
 *
 * TAKT: jeder Zustand ruft FUN_8002959c genau einmal je Bild. a3 ist KEIN Takt: die unteren 16 Bit sind das
 * Ueberblendgewicht je +0x14e-Schritt (`andi v0,a3,0xffff` / `mult v0,t3` mit t3 = +0x14e @0x800296b8-bc), Bit 16 die
 * Richtung (`and v0,a3,v0` @0x800295b8-c4); das Bild laeuft immer um 1 (@0x80029b28-34). Siehe bb_anim.
 *
 * NICHT uebernommen (RE2-Phase-1 und Wandlauf, im Dossier R_bahnsteig.md): Clip 2 aus Sub 1/7/8 (Vorderhaende
 * +1019..+1282 unter der Sohle, gemessen; auf ebenem Boden unbrauchbar, Sub 8 faehrt deshalb Clip 5 rueckwaerts),
 * Sub 5/6/16/17 (Sprung an die Wand, Clips
 * 6/7/13/14/16; im Wagen sinnlos), Sub 10 (Fressen am toten Spieler), Sub 11/12/13 (Sprung, Sturz, Klammern).
 *
 * MASSSTAB: das Asset ist um FORM.skala verkleinert. Jede RE2-Laenge unten laeuft deshalb durch BB_L() und traegt
 * ihre @0x-Adresse in der Originalgroesse. Der Zustand, der nicht aus dem Overlay kommt, ist als Gestaltung markiert.
 */
#include "re15_birkin_formen.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"      /* re15_player_stagger_cmd2 */
#include "re15_damage.h"        /* re15_enemy_part_point, re15_engine_rand8, re15_enemy_player_dist */
#include "re15_birkin_opfer.h"  /* re15_bk_opfer_start */
#include "re15_skeleton.h"      /* re15_skel_bone_to_world */
#include "re15_scd.h"           /* g_scd: Kamera des Messhakens */
#include "re15_math.h"          /* re15_squareroot0 */
#include "re15_audio.h"         /* re15_audio_room_se */
#include "re15_esp.h"           /* re15_esp_fx_spawn_ex = FUN_8001bf10-Packung */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);   /* FUN_80015558-Zwilling */
int  re15_r38_birkin_anim(re15_actor_t *e);                                       /* enemy_ai_common.c */
int  re15_r38_birkin_fc(const re15_actor_t *e);
void re15_r38_griff_pin(void);
unsigned re15_re2z_weapon_id(unsigned w);                                         /* enemy_ai_re2_zombie.c: RE1.5-Id -> RE2-Id */

/* FORM.skala aus tools/birkin_bahnsteig.py (Gestaltung, muss mit dem Asset uebereinstimmen): 1.0 = G4-Originalgroesse. */
#define BB_L(x)  ((int32_t)(x))

enum { BB_AUFTRITT = 0, BB_HIEB = 2, BB_GEHEN = 3, BB_RENNEN = 4, BB_WEICHEN = 8, BB_BISS = 14, BB_GRIFF = 15, BB_ZUCKEN = 20 };

/* Clips (Zensus der Schreiber von +0x14c): */
#define BB_C_RENNEN    0     /* @0x80102b10 */
#define BB_C_HIEB      3     /* @0x80101ad0 Clip = 3 + Seite (+0x16a) */
#define BB_C_GEHEN     5     /* @0x801028dc */
#define BB_C_ZUCKEN    8     /* @0x80107224 Wort 0x00070508: Clip 8, Bild 5, Blende 7 */
#define BB_C_NACHHIEB  9     /* @0x8010224c */
#define BB_C_DROHEN   10     /* @0x8010153c */
#define BB_C_BISS     26     /* @0x80104e84 */
#define BB_C_GRIFF    27     /* @0x80105254 */

#define BB_TEIL_RUMPF 10     /* Teil +0x198 + 10*0xac: `addiu a0,a0,1792` = +0x700 = Teil 10 + 0x48 @0x80102ed0 */

/* EDD-Wort eines Bildes (FUN_80016028 @0x80016034-54: Bit 27 = Ton, Id = Wort >> 28; Bit 16 = Trefferbild,
 * `lui v1,1` + `and` @0x80101cb8-bc / @0x80104f58-5c). */
#define BB_FL_TREFFER  0x00010000u
#define BB_FL_TON      0x08000000u

static struct {
    int      slot;           /* Aktor der laufenden Form, -1 = keiner */
    uint8_t  cd_nah;         /* +0x21e Sperre nach Angriff, je Bild -1 @0x80100234-44 */
    uint8_t  cd_griff;       /* +0x23c @0x80100270-80 */
    uint8_t  cd_lauf;        /* +0x232 @0x80100248-58 */
    uint8_t  biss_rest;      /* +0x23a: Bisse bis zum naechsten Griff */
    uint8_t  meter;          /* +0x21d Trefferzaehler, Abbau @0x801001d8-230 */
    uint8_t  meter_uhr;      /* +0x21c */
    uint8_t  seite;          /* +0x16a im HIEB */
    uint8_t  doppelt;        /* +0x15a: Doppelschlag schon gelaufen */
    uint8_t  getroffen;      /* +0x21a & 2 */
    uint8_t  griff_los;      /* Griff angewandt */
    int8_t   lauf;           /* +0x16a im RENNEN: Bit 7 = Ende vorgemerkt, Nibble = Restdurchlaeufe */
    uint16_t abgewandt;      /* +0x238: Bilder ohne Blick auf den Spieler */
    uint16_t uhr;            /* +0x158 */
    uint8_t  abdrehen;       /* +0x15a im RENNEN: Bilder Abdrehen nach dem Treffer */
    uint32_t wort;           /* *+0x178: EDD-Wort des Bildes, das der letzte anim_step gezeichnet hat */
    int32_t  spur_x, spur_z; /* +0x20c/+0x210 letzte Spurwerte (FUN_80015e7c) */
    int32_t  tempo, quer;    /* +0x144 / +0x148 */
    uint8_t  s_state, s_sub, s_ph2, s_ph3;   /* +0x1fc Sicherung (Treffer setzt fort @0x80107180-8c) */
    uint8_t  gesichert;
} s_bb = { .slot = -1 };

static int s_bb_blut = 0;       /* Testhaken: Treffer-Blutspawns seit dem letzten re15_bb_reset */
static void (*s_se_fn)(int, int) = 0;
static void (*s_bank_fn)(int) = 0;

void re15_bb_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank))
{
    s_se_fn = se_fn; s_bank_fn = bank_fn;
}

/* ENEMSE-Bank 24: G4-Spawn ROOM6170 traegt Ton-Id +7 = 0x26, Paar-Tabelle @0x800a7400 Zeile 24 = {0x26,0x00},
 * erste Haelfte (FUN_80052b38, Port-Beleg gen_re2_ems_toc.py TABELLE 3). Wird vor jedem Ton gesetzt, weil der
 * Raum weitere ENEMSE-Baenke fuehrt (G5: Bank 25). */
static void bb_se(int id)
{
    if (s_bank_fn) s_bank_fn(24);
    if (s_se_fn)   s_se_fn(id, 0);
}

void re15_bb_reset(void)
{
    memset(&s_bb, 0, sizeof s_bb);
    s_bb.slot = -1;
    s_bb_blut = 0;
}

int re15_bb_zaehler(void) { return s_bb.meter; }
int re15_bb_blut_zaehler(void) { return s_bb_blut; }

int re15_bb_bank_da(const re15_actor_t *e)
{
    const re15_enemy_bank_t *b = e ? re15_enemy_find(e->type) : NULL;
    return e && e->type == 0x30u && b && b->ok && b->re2_rig && b->skel.bone_count == 27;
}

/* ---- Bausteine ---------------------------------------------------------------------------- */
static void bb_clip(re15_actor_t *e, uint8_t c, uint8_t bild, uint8_t blende)
{
    e->motion = c; e->anim_frame = bild; e->anim_frac = blende;
    s_bb.spur_x = s_bb.spur_z = 0;
    s_bb.tempo = 0;
}

/* FUN_8002959c, a3 wie im Overlay:
 *   Bit 16 = rueckwaerts: Slot (fc - Bild) - 1 (`and v0,a3,v0` @0x800295b8-c4, Spiegel @0x800295cc-e0). Argument JE AUFRUF,
 *     kein Zustand. Port: anim_flags 0x80 (Zeichenpfad re15_actor_anim_reverse); ein Treffer loescht das Bit in den drei
 *     Appliern, darum setzt jeder Aufruf es neu (Abnahme 1 M7/M8).
 *   untere 16 Bit = Ueberblendgewicht je +0x14e (@0x800296b8-bc) -> anim_blend_rate; ohne sie nimmt der Zeichenpfad 0x200,
 *     und ab frac >= 8 klemmt das Gewicht auf 100 % Vor-Pose (Abnahme 1 M6, skeleton_common.c).
 *   Bild +1 (@0x80029b28-34), +0x14e -1 (@0x800299c0-cc), Rueckgabe 1 am Clip-Ende (@0x80029b48-4c).
 * +0x178 zeigt danach auf das Wort des GEZEICHNETEN Bildes, also vor dem +1 (`sw a2,376(a0)` @0x800295f8). */
#define BB_A3_RUECK 0x10000u
static int bb_anim(re15_actor_t *e, uint32_t a3)
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const int rueck = (a3 & BB_A3_RUECK) != 0;
    if (b) {
        int bild = (int)e->anim_frame;
        if (rueck) bild = re15_r38_birkin_fc(e) - 1 - bild;
        s_bb.wort = re15_emd_get_frame_entry(&b->anim, (int)e->motion, bild);
    }
    e->anim_blend_rate = (uint16_t)(a3 & 0xffffu);
    if (rueck) e->anim_flags |= 0x80u; else e->anim_flags &= (uint16_t)~0x80u;
    return re15_r38_birkin_anim(e);
}

/* FUN_80016028: Ton des gezeichneten Bildes, Bit 27 / Wort >> 28 (Aufruf @0x801004ec). */
static void bb_bild_ton(void)
{
    if (s_bb.wort & BB_FL_TON) bb_se((int)(s_bb.wort >> 28));
}

/* FUN_80015614: 1 = Ziel im Kegel +-halb um die Blickrichtung. Peilung wie re15_enemy_steer_point. */
static int bb_kegel(const re15_actor_t *e, int32_t tx, int32_t tz, int halb)
{
    int blick = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff;
    return ((halb + blick - (int)e->rot_y) & 0x0fff) < 2 * halb;
}

/* FUN_80015834 @0x80015874-8f0: Weltpunkt = Teilmatrix * Versatz + Teil.T, Treffer wenn sqrt(dx^2+dz^2) < Radius
 * (`sltu v0,v0,s2` @0x800158f0), Y bleibt ungeprueft. */
static int bb_nah(const re15_actor_t *e, const re15_actor_t *pl, int teil, int32_t ox, int32_t oy, int32_t radius)
{
    const int32_t ofs[3] = { ox, oy, 0 };
    int32_t w[3];
    if (!re15_enemy_part_point(e, teil, ofs, w)) return 0;
    int64_t dx = (int64_t)w[0] - pl->x, dz = (int64_t)w[2] - pl->z;
    return (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz)) < radius;
}

/* FUN_80015e7c (selbst disassembliert, info/re2leon/SLUS_007.48): Spur = kumulierte Keyframe-Werte +6/+10, differenziert.
 * `rueck` ist a3 des Aufrufs: Slot (fc - Bild) - 1 (@0x80015e9c-b0). In Bild 0 startet die Spur bei null (@0x80015f14-1c),
 * rueckwaerts bei letztem + erstem Bild (@0x80015f24-fa4). Ergebnis landet in +0x144/+0x148 (@0x80015fd8-e4), FUN_800152c8
 * addiert es danach mit der Gier auf die Lage. */
static void bb_spur(re15_actor_t *e, int rueck)
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    int16_t sx = 0, sy = 0, sz = 0;
    uint32_t w;
    int bild = (int)e->anim_frame;
    s_bb.tempo = 0; s_bb.quer = 0;
    if (!b || !b->ok) return;
    if (rueck) bild = re15_r38_birkin_fc(e) - 1 - bild;
    w = re15_emd_get_frame_entry(&b->anim, (int)e->motion, bild);
    if (!re15_emd_get_keyframe_speed(&b->skel, (int)(w & 0xFFFu), &sx, &sy, &sz)) return;
    if (e->anim_frame == 0) {
        s_bb.spur_x = s_bb.spur_z = 0;
        if (rueck) {
            int16_t ex = 0, ey = 0, ez = 0;
            (void)re15_emd_get_keyframe_speed(&b->skel, (int)(re15_emd_get_frame_entry(&b->anim, (int)e->motion, 0) & 0xFFFu), &ex, &ey, &ez);
            s_bb.spur_x = (int32_t)sx + ex; s_bb.spur_z = (int32_t)sz + ez;
        }
    }
    s_bb.tempo = (int32_t)sx - s_bb.spur_x;
    s_bb.quer  = (int32_t)sz - s_bb.spur_z;
    s_bb.spur_x = sx; s_bb.spur_z = sz;
}

static void bb_schieben(re15_actor_t *e)
{
    int32_t t[3] = { s_bb.tempo, 0, s_bb.quer }, aus[3];
    re15_skel_bone_to_world(t, e->rot_y, e->x, e->y, e->z, aus);
    e->x = aus[0]; e->z = aus[2];
}

/* Treffer auf den Spieler. Schaden wie die 0x30-Rollen desselben Kampfes: RE2 rechnet mit 200 HP (Hieb 15
 * @0x80101dcc, Biss 20 @0x8010501c), der Port-Spieler mit 100; Hieb/Biss 10 sind die Werte @0x801177f0/@0x80117c78 der
 * RE1.5-KI, Rennen 5 @0x80118b1c. Griff: bb_griff.
 * ⛔ Abnahme 1 M14, Entscheidung: RE2 ist das Vorbild, die Form toetet. FUN_800401d4 liefert 2 fuer den Todeszweig
 * (@0x8004027c-8c, Aufrufer @0x80101ddc / @0x80105028), den Treffer-Clip gibt es dann nicht (@0x80101dfc: cmd 6 statt
 * cmd 2). Die Klemme auf 1 (@0x80117810) gehoert der 0x30-KI und gilt fuer die Form nicht. Tod = hp < 0 wie beim G5
 * (enemy_ai_boss_g5.c); der Detektor in game_step setzt dann cmd 3 (@0x80012ee8), Fressen Sub 10 / cmd 6 ist nicht
 * gebaut. Die Gnaden-Regel @0x80040288-b4 (einmal -1..-14 -> HP 0) hat der Port-Spieler nicht, wie beim G5. */
static void bb_schaden(re15_actor_t *pl, int schaden)
{
    pl->hp = (int16_t)(pl->hp - schaden);
    if (pl->hp < 0) pl->hp = -1;
}

static void bb_schlag(re15_actor_t *pl, int schaden, int ton)
{
    bb_schaden(pl, schaden);
    if (pl->hp >= 0) re15_player_stagger_cmd2(0x08);  /* cmd 2: 0x800cfbfc = ((Seite|2) << 8) | 2 @0x80101de4-f0 */
    pl->hit_react |= 1;
    bb_se(ton);
}

static void bb_zu_gehen(re15_actor_t *e)
{
    e->state = 1; e->sub_state_1 = BB_GEHEN; e->sub_state_2 = 0; e->sub_state_3 = 0;
}

static void bb_setze(re15_actor_t *e, int sub)
{
    e->sub_state_1 = (uint8_t)sub; e->sub_state_2 = 0; e->sub_state_3 = 0;
}

/* ---- Entscheiden: A[3] 0x80102370, Nabe der Bodenphase ------------------------------------- */
static void bb_entscheiden(re15_actor_t *e, re15_actor_t *pl, int32_t dist)
{
    const int k256 = bb_kegel(e, pl->x, pl->z, 256);   /* @0x801023f0 */
    const int k512 = bb_kegel(e, pl->x, pl->z, 512);   /* @0x8010177c */
    const int k824 = bb_kegel(e, pl->x, pl->z, 824);   /* @0x801023d4 */
    const int k1024 = bb_kegel(e, pl->x, pl->z, 1024); /* @0x80101784 */
    int neu = -1;
    if (pl->hit_react == 0 && !re15_bk_opfer_cmd()) {  /* 0x800cfdcb == 0 @0x80102420-2c */
        /* HIEB ist RE2-Phase-1 (A[1] @0x80101780-c8: Kegel 512, dist < 0xfa0, +0x21e == 0). Hier dieselbe Regel
         * in der Nabe, weil Clip 2 entfaellt; Rangfolge durch die Reichweite: Hieb weit, Biss nah, Griff zuletzt. */
        if (k512 && dist < BB_L(0xfa0) && s_bb.cd_nah == 0) neu = BB_HIEB;
        if (k824 && dist < BB_L(0xce4) && s_bb.cd_nah == 0) {          /* @0x80102434-54 */
            neu = BB_BISS;                                             /* sw 0xe01 @0x80102464 */
            if (s_bb.biss_rest) s_bb.biss_rest--;                      /* @0x80102468-6c */
        }
        if (s_bb.cd_griff == 0 && k256 && dist < BB_L(0xed8)           /* @0x80102470-90 */
            && (s_bb.biss_rest == 0 || pl->hp < 15)) {                 /* @0x80102498-b8: RE2 hp < 30 von 200 */
            neu = BB_GRIFF;                                            /* sw 0xf01 @0x801024c4 */
            s_bb.biss_rest = (uint8_t)((re15_engine_rand8() & 3) + 3); /* @0x801024c8-d4 */
        }
    }
    if (!k824) s_bb.abgewandt++;                                       /* +0x238 += 1 @0x80102508-1c */
    if (s_bb.abgewandt >= 0xc9) {                                      /* @0x801026a0 */
        s_bb.abgewandt = 0;
        if (neu < 0) neu = BB_RENNEN;                                  /* 0x401 @0x801026c0 */
    }
    if (dist >= BB_L(0x1f41) && s_bb.cd_lauf == 0 && neu < 0)          /* @0x801026a8 / @0x80102724-48 */
        neu = BB_RENNEN;
    if (neu < 0 && k1024 && dist < BB_L(0x7d0))                        /* Sub 8 aus A[1] @0x801017cc-e0 */
        neu = BB_WEICHEN;
    if (neu >= 0) bb_setze(e, neu);
}

/* ---- Ausfuehren ---------------------------------------------------------------------------- */
static void bb_gehen(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                         /* B[3] @0x801028d4: Clip 5, Blende 15 */
        bb_clip(e, BB_C_GEHEN, 0, 0xf);
        e->sub_state_2 = 1;
        bb_se(0);                                      /* se(0) @0x80102908 */
    }
    bb_spur(e, 0);                                     /* @0x8010291c */
    /* Lenken @0x80102924-68: ab +0x144 >= 21 Rate 4, ab >= 31 zusaetzlich 8 (RE2-Tempo ~30 je Bild; hier skaliert). */
    if (s_bb.tempo >= BB_L(21)) re15_enemy_steer_point(e, pl->x, pl->z, 4);
    if (s_bb.tempo >= BB_L(31)) re15_enemy_steer_point(e, pl->x, pl->z, 8);
    /* Port: RE2 dreht aus dieser Lage ueber den Wandsprung (Sub 5/16). Im Wagen fehlt er, deshalb die Rate aus
     * B[1] @0x801018ec-908: ausserhalb des 824er-Kegels 32 je Bild. */
    if (!bb_kegel(e, pl->x, pl->z, 824)) re15_enemy_steer_point(e, pl->x, pl->z, 32);
    if (bb_anim(e, 0x100)) bb_se(11);                  /* a3 @0x80102a34, Clip-Ende: se(11) @0x80102a40 */
    bb_schieben(e);                                    /* @0x80102a4c */
}

/* B[8] @0x8010419c: Der Spieler steht zu nah, die Form weicht zurueck statt ihn zu schieben. Clip 2 rueckwaerts
 * (root_delta a3 = 1 @0x80104248, anim_step a3 = 0x10080 @0x80104280: rueckwaerts, Gewicht 0x80), Uhr 40 @0x801041f0;
 * RE2 faehrt Clip 2, im Port Clip 5 (s. Kopf). A[8] @0x80104130-58: zwischen 2501 und 4000 wieder der Hieb.
 * Solange +0x14e >= 21 laeuft die Form mit halbem Tempo an (`sltiu v0,v0,0x15` / `sra v0,v0,0x11` @0x80104254-70). */
static void bb_weichen(re15_actor_t *e, re15_actor_t *pl, int32_t dist)
{
    if (e->sub_state_2 == 0) {
        bb_clip(e, BB_C_GEHEN, 0, 0x1f);               /* Blende 31 @0x801041e4 */
        e->sub_state_2 = 1;
        s_bb.uhr = 40;
    }
    if (bb_kegel(e, pl->x, pl->z, 512) && dist >= BB_L(2501) && dist < BB_L(4000) && s_bb.cd_nah == 0) {
        e->anim_flags &= (uint16_t)~0x80u;
        bb_setze(e, BB_HIEB);                          /* 0x201 @0x80104158 */
        return;
    }
    re15_enemy_steer_point(e, pl->x, pl->z, 32);       /* @0x80104204 */
    if (!bb_kegel(e, pl->x, pl->z, 824)) re15_enemy_steer_point(e, pl->x, pl->z, 32);   /* @0x80104218-30 */
    bb_spur(e, 1);                                     /* @0x80104244 */
    if (e->anim_frac >= 0x15) s_bb.tempo >>= 1;        /* @0x80104254-70 */
    (void)bb_anim(e, BB_A3_RUECK | 0x80u);             /* @0x8010427c-84 */
    bb_schieben(e);                                    /* @0x8010428c */
    if (s_bb.uhr-- == 0) {                             /* @0x80104294-a8 */
        e->anim_flags &= (uint16_t)~0x80u;
        bb_zu_gehen(e);
    }
}

/* Doppelschlag @0x8010b0f0: {Bild im Clip, Einstiegsbild im Gegen-Clip} je Seite. */
static const uint8_t k_doppel[2][2] = { { 30, 7 }, { 19, 20 } };

static void bb_hieb(re15_actor_t *e, re15_actor_t *pl, int32_t dist)
{
    switch (e->sub_state_2) {
    case 0:                                            /* T0[10] @0x80101a78 */
        s_bb.seite = (uint8_t)(re15_engine_rand8() & 1);
        if (dist < BB_L(0x9c4)) s_bb.seite = 1;        /* @0x80101a9c-ac */
        bb_clip(e, (uint8_t)(BB_C_HIEB + s_bb.seite), 0, 0xf);   /* 0x000f0003 + Seite @0x80101ab0-d0 */
        e->sub_state_2 = 1;
        s_bb.doppelt = 0; s_bb.getroffen = 0;          /* @0x80101af8-b0c */
        re15_enemy_steer_point(e, pl->x, pl->z, 32);   /* @0x80101ae4 */
        bb_se(0);
        /* faellt in [11] */
    case 1: {                                          /* T0[11] @0x80101b10 */
        int ende;
        bb_spur(e, 0);                                 /* @0x80101b1c */
        ende = bb_anim(e, 0x100);                      /* a3 @0x80101b34 */
        if (ende) {
            if (s_bb.getroffen || pl->hp < 0) {        /* @0x80101b48-6c, `bgez` Spieler-HP @0x80101b60 */
                e->sub_state_2 = 2;
                break;                                 /* j 0x80102350: ohne FUN_800152c8 */
            }
            s_bb.cd_nah = 10; bb_zu_gehen(e);          /* 0x101, +0x21e = 10 @0x80101b74-7c, laeuft dann weiter */
        }
        /* Doppelschlag @0x80101b80-c4: nur einmal, Spieler noch im Kegel und in Reichweite. Der neue Clip steigt mitten
         * ein (k_doppel), deshalb setzt das Original die Spur eigens neu: FUN_80015e7c zweimal hintereinander
         * (@0x80101c4c mit dem neuen Clip-Wort im Delay-Slot @0x80101c50, @0x80101c60), der zweite Aufruf gibt neu - neu = 0,
         * und das Tick-Ende @0x80102348 schiebt um 0. Ohne das nahm der naechste Schritt die ganze aufgelaufene Spur des
         * Einstiegsbilds (Clip 4 Bild 7 = 680 Einheiten, Abnahme 1 M10/M17). */
        if (!s_bb.doppelt && dist < BB_L(0xfa0) && bb_kegel(e, pl->x, pl->z, 512)
            && e->anim_frame == k_doppel[s_bb.seite][0] && pl->hp >= 0) {   /* `bltz v0` @0x80101bf8 */
            uint8_t alt = s_bb.seite;
            s_bb.doppelt = 1;
            s_bb.seite = (uint8_t)(alt ^ 1);           /* @0x80101c20-30 */
            bb_clip(e, (uint8_t)(BB_C_HIEB + s_bb.seite), k_doppel[alt][1], 0xf);
            bb_spur(e, 0);                             /* @0x80101c4c */
            bb_spur(e, 0);                             /* @0x80101c60 */
            re15_enemy_steer_point(e, pl->x, pl->z, 32);   /* @0x80101c78 */
        }
        if (s_bb.wort & BB_FL_TREFFER) {               /* @0x80101cb8-c0 */
            int teil = s_bb.seite ? 13 : 18;           /* Kette 0x80109494 bzw. 0x801093d8 bis Teil 13 / 18 */
            re15_enemy_steer_point(e, pl->x, pl->z, 128);
            if (pl->hit_react == 0 &&
                (bb_nah(e, pl, teil, 0, 0, BB_L(1700)) ||        /* @0x80101d38 */
                 bb_nah(e, pl, teil, 0, BB_L(600), BB_L(1500)))) {   /* @0x80101d50, Versatz (0,600,0) @0x80101ce0 */
                bb_schlag(pl, 10, 3);                  /* se(3) @0x80101e74 */
                s_bb.getroffen = 1;
            }
            if (s_bb.cd_lauf == 0) { bb_se(4); s_bb.cd_lauf = 7; }   /* @0x80102034-40 */
        }
        bb_schieben(e);                                /* @0x80102348 */
        break;
    }
    case 2:                                            /* T0[12] @0x80102244: Clip 9 */
        bb_clip(e, BB_C_NACHHIEB, 0, 0xf);
        e->sub_state_2 = 3;
        /* faellt in [13] */
    default:                                           /* T0[13] @0x80102258 */
        if (e->anim_frac) re15_enemy_steer_point(e, pl->x, pl->z, 32);
        if (e->anim_frame >= 0x1a && (!bb_kegel(e, pl->x, pl->z, 824) || dist >= BB_L(0x1389))) {   /* @0x80102288-c0 */
            s_bb.cd_nah = 5; bb_zu_gehen(e);           /* @0x801022d8-e4 */
            break;
        }
        bb_spur(e, 0);                                 /* @0x801022f4 */
        if (bb_anim(e, 0x100)) { s_bb.cd_nah = 10; bb_zu_gehen(e); }   /* a3 @0x8010230c, @0x80102338-40 */
        bb_schieben(e);                                /* @0x80102348 */
        break;
    }
}

static void bb_biss(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                         /* B[14] @0x80104e7c */
        bb_clip(e, BB_C_BISS, 0, 0xf);
        e->sub_state_2 = 1;
        s_bb.getroffen = 0;
    }
    bb_spur(e, 0);                                     /* @0x80104ea8 */
    if (bb_anim(e, 0x100)) {                           /* a3 @0x80104ec0 */
        s_bb.cd_nah = 15;                              /* 0x301, +0x21e = 15 @0x80104ec8-e0 */
        if (s_bb.getroffen) {                          /* +0x21a & 2 @0x80104ed0-dc */
            s_bb.cd_nah = (uint8_t)((re15_engine_rand8() & 0xf) + 15);   /* @0x80104eec-f4 */
            s_bb.cd_lauf = 90;                         /* @0x80104ef8-f00 */
        }
        bb_zu_gehen(e);
    }
    if (s_bb.wort & BB_FL_TREFFER) {                   /* @0x80104f4c-60 */
        re15_enemy_steer_point(e, pl->x, pl->z, 16);   /* @0x80104f7c */
        if (pl->hit_react == 0 && bb_nah(e, pl, BB_TEIL_RUMPF, BB_L(1400), 0, BB_L(1500))) {   /* @0x80104f88-a4 */
            bb_schlag(pl, 10, 6);                      /* se(6) @0x801050a0 */
            s_bb.getroffen = 1;
        }
    }
    bb_schieben(e);                                    /* FUN_800152c8 am Ende von B[14] */
}

static void bb_griff(re15_actor_t *e, re15_actor_t *pl, int32_t dist)
{
    if (e->sub_state_2 == 0) {                         /* B[15] @0x8010523c */
        int yaw = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0x0fff;
        bb_clip(e, BB_C_GRIFF, 0, 7);                  /* Wort 0x0007001b @0x80105254 */
        e->rot_y = (int16_t)yaw;                       /* steer 2048 = sofort zum Spieler @0x80105268 */
        e->sub_state_2 = 1;
        s_bb.griff_los = 0;
    }
    if (bb_anim(e, 0x200)) {                           /* a3 @0x801053b0; 0x301, alle drei Sperren 150 @0x801053b8-d8 */
        s_bb.cd_nah = s_bb.cd_lauf = s_bb.cd_griff = 150;
        bb_zu_gehen(e);
    }
    /* Griff-Bild 8 @0x80105408-10: der Stoss samt Schuetteln. RE2 packt beim Eintritt (0x800cfbfc = 5 @0x80105294)
     * und fuehrt Leon ueber die Opfer-Spur; der Port setzt den Griff dorthin, wo das Bild die Beruehrung zeigt, und
     * nutzt die Spieler-Maschine der Birkin-Treffer (cmd 6 -> Haken [0] @0x8011defc). Die Spur von Clip 27 ist die
     * Opfer-Spur (FUN_80015cb8 @0x80105398), kein Vortrieb.
     * Schaden RE2 (auf 200 HP): `slti v0,v1,0xab`, sonst 0x46, ab 171 HP so viel, dass 100 bleiben (`addiu a0,v1,-0x64`)
     * @0x801052a8-b4; Port-Spieler 100 HP, alle Zahlen halb. Unter dem Griff-Tor hp < 15 (@0x801024b4) ist er damit wie in
     * RE2 immer toedlich (Abnahme 1 M14). Der Todes-Griff laeuft ueber cmd 3 (s. bb_schlag), RE2-cmd 6 Variante 2
     * @0x801052cc-d0 ist nicht gebaut.
     * Haken [0] ist der Treffer "von vorn" und schiebt rueckwaerts entlang der SPIELER-Gier (FUN_800245d8(0x800)
     * @0x8011b3f0); im 0x30 geht ihm der Gier-Latch @0x80117754-a8 voraus. Der Griff trifft einen freien Spieler mit
     * beliebiger Gier, deshalb stellt ihn der Griff selbst mit dem Gesicht zur Form (Abnahme 1 M15; dieselbe Richtung
     * schreibt RE2 nach Spieler+0x15a = Form-Gier + 0x800 @0x80105358-68). */
    if (!s_bb.griff_los && e->anim_frame == 8) {
        s_bb.griff_los = 1;
        if (pl->hit_react == 0 && dist < BB_L(0xed8)) {
            bb_schaden(pl, (pl->hp < 86) ? 35 : pl->hp - 50);
            pl->hit_react |= 1;
            bb_se(7);
            if (pl->hp >= 0) {
                pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
                if (!re15_bk_opfer_start(e, 6, 0)) re15_r38_griff_pin();
            }
        }
    }
}

/* B[4] @0x80102afc. Nach dem Treffer dreht die Form ab (Abnahme 1 M11): kein Lenken mehr zum Spieler (`andi v0,v0,2` /
 * `bnez` @0x80102b90-94 ueberspringen das steer @0x80102be4), beim Treffer ein Ruck weg vom Spieler (steer a3 = -0x80
 * @0x80102f8c), dann sechs Bilder -0x10 (+0x15a = 6 @0x80102f68, steer @0x80103014, +0x15a -= 1 @0x80103028).
 * Negatives a3 heisst Peilung + 0x800 (FUN_80015558 @0x8001559c-ac). Sonst trug der Koerperschub (FUN_8002aec4, Original)
 * den Spieler bis ans Gangende. */
static void bb_rennen(re15_actor_t *e, re15_actor_t *pl, int32_t dist)
{
    if (e->sub_state_2 == 0) {
        bb_clip(e, BB_C_RENNEN, 0, 7);                 /* Wort 0x00070000 @0x80102b10 */
        e->sub_state_2 = 1;
        s_bb.lauf = 0;
        s_bb.getroffen = 0;                            /* +0x21a &= ~2 @0x80102b58-60 */
        s_bb.abdrehen = 0;                             /* sh zero,0x15a @0x80102b28 */
        re15_enemy_steer_point(e, pl->x, pl->z, 64);   /* a3 0x40 @0x80102b00-24 */
    }
    if (!s_bb.getroffen) re15_enemy_steer_point(e, pl->x, pl->z, 64);   /* @0x80102be4 */
    bb_spur(e, 0);                                     /* @0x80102bf8 */
    {   /* Bremse @0x80102c30-88 / @0x80102cbc-d0: ausserhalb der Kegel 512 und 1024 je 1/3 des Tempos weniger */
        int32_t drittel = s_bb.tempo / 3;
        if (!bb_kegel(e, pl->x, pl->z, 512))  s_bb.tempo -= drittel;
        if (!bb_kegel(e, pl->x, pl->z, 1024)) s_bb.tempo -= drittel;
    }
    if (bb_anim(e, 0x100) && (s_bb.lauf & 0x80)) {     /* a3 @0x80102d08, Clip-Ende @0x80102d0c-4c */
        if ((s_bb.lauf & 0xf) == 0) {                  /* 0x301 @0x80102d58-60 */
            s_bb.cd_lauf = 90;                         /* @0x80102d8c */
            bb_zu_gehen(e);
        } else s_bb.lauf--;                            /* @0x80102db4-c0 */
    }
    s_bb.tempo += BB_L(10);                            /* +0x144 += 10 @0x80102dc4-cc */
    if (s_bb.tempo >= BB_L(201) && pl->hit_react == 0 &&                         /* @0x80102e80-88 */
        bb_nah(e, pl, BB_TEIL_RUMPF, BB_L(1400), 0, BB_L(1500))) {               /* @0x80102eb4-cc */
        bb_schlag(pl, 5, 7);                           /* se(7) @0x80102f98 */
        s_bb.getroffen = 1;                            /* +0x21a |= 2 @0x80102f6c-70 */
        s_bb.abdrehen = 6;                             /* @0x80102f3c / @0x80102f68 */
        re15_enemy_steer_point(e, pl->x, pl->z, -128); /* @0x80102f8c */
    }
    if (dist < BB_L(0x1194) && !(s_bb.lauf & 0x80)) s_bb.lauf = (int8_t)0x81;    /* -127 @0x80102fbc-e4 */
    bb_schieben(e);                                    /* @0x80102fec */
    if (s_bb.abdrehen) {                               /* @0x80102ff4-103028 */
        re15_enemy_steer_point(e, pl->x, pl->z, -16);
        s_bb.abdrehen--;
    }
}

static void bb_auftritt(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                         /* T0[6] @0x80101528 */
        bb_clip(e, BB_C_DROHEN, 0, 7);
        e->sub_state_2 = 1;
    }
    re15_enemy_steer_point(e, pl->x, pl->z, 16);       /* Gestaltung: die Form dreht sich dem Spieler zu */
    bb_spur(e, 0);                                     /* @0x801015bc */
    if (bb_anim(e, 0x200)) bb_zu_gehen(e);             /* a3 @0x801015d4, T0[8] @0x801015f4: Clip 5 */
    bb_schieben(e);
}

/* Treffer-Blut der Form. Sie spielt die Bodenphase (+0x10e = 1 @0x8010154c im Eintritt T0[6]), also Handler 2 @0x80107280
 * der Tabelle @0x8010b110, vier FUN_8001bf10 @0x80107454/94/b8/f8 (Packung a0: Effekt << 24 | Sub << 16 | Skala,
 * a1 = Winkel, a2 = 0x8009db44 = Einheitsmatrix, a3 = Weltpunkt). Anker Teil 10 (+0x714/+0x718/+0x71c @0x80107390-b8,
 * Handler 1 @0x80106d7c nimmt Teil 9). Die Versaetze dreht RotMatrix(0, Peilung Form -> Spieler, 0) @0x801072f8:
 *   Punkt 1 (200,-200,0), Punkt 2 (+0x144 * 4 + 300, 0, 0) @0x80107300-24; Zone 0 (+0x1d2 % 3 == 0 @0x80107328-50):
 *   Punkt 1 (150,800,0), Punkt 2 (+0x144 * 2 + 150, 700, 0) @0x80107358-78.
 * Port: +0x1d2 fuehrt der 0x30 nicht, sein Gegenstueck ist die Trefferhoehe +0x6 des RE1.5-Resolvers (0 unten,
 * re15_damage.c @0x80012438-50). Skala 0x17d0 + (rng & 0xff) * 8 @0x80107430-34; der Rueckstoss von Punkt 1 um
 * (rng & 0x3f) - 0x40 @0x80107464-7c gilt fuer Spawn 2, derselbe Schritt vor Spawn 4 @0x801074c8-e0 trifft wieder Punkt 1
 * und ist tot (Rest im Original). Effekt 0 = Blut in beiden Engines (s. enemy_ai_boss_g5.c). */
static void bb_blut_spawn(uint8_t sub, const int32_t p[3], int16_t winkel)
{
    uint16_t skala = (uint16_t)(0x17d0 + (re15_engine_rand8() & 0xffu) * 8u);
    s_bb_blut++;
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0u, sub, skala, p[0], p[1], p[2], winkel);
}

static void bb_treffer_blut(const re15_actor_t *e, const re15_actor_t *pl, unsigned zone)
{
    static const int32_t null3[3] = { 0, 0, 0 };
    const int16_t winkel = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0x0fff);
    int32_t p1[3] = { 200, -200, 0 }, p2[3] = { s_bb.tempo * 4 + 300, 0, 0 };
    int32_t t[3], a[3], b[3];
    if (zone == 0) {
        p1[0] = 150; p1[1] = 800;
        p2[0] = s_bb.tempo * 2 + 150; p2[1] = 700;
    }
    (void)re15_enemy_part_point(e, BB_TEIL_RUMPF, null3, t);
    re15_skel_bone_to_world(p1, winkel, t[0], t[1], t[2], a);
    re15_skel_bone_to_world(p2, winkel, t[0], t[1], t[2], b);
    bb_blut_spawn(1u, a, winkel);                      /* `lui a0,1` @0x80107438, jal @0x80107454 */
    a[1] -= (int32_t)(re15_engine_rand8() & 0x3fu) - 0x40;
    bb_blut_spawn(0u, a, winkel);                      /* @0x80107494 */
    bb_blut_spawn(0u, b, winkel);                      /* @0x801074b8 */
    (void)re15_engine_rand8();                         /* toter Schritt @0x801074c0 */
    bb_blut_spawn(0u, b, winkel);                      /* @0x801074f8 */
}

/* ---- Wurzel ---------------------------------------------------------------------------------- */
void re15_bb_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t dist;

    if (s_bb.slot != slot) {                           /* Eintritt: Port-Gegenstueck des Ctor-Teils 0x8010062c */
        memset(&s_bb, 0, sizeof s_bb);
        s_bb.slot = slot;
        /* Kasten: Wurzel = Becken, die Masse sitzt davor (Gestaltung; RE2-Kaesten +0x90/+0xb0 @0x80100718-a4
         * tragen x 400 bei Halbmassen 1100/1000). */
        e->hit_offset_x = (int16_t)BB_L(800);
        /* Eigene HP der neuen Form: RE2 schreibt sie im Ctor (+0x156 = 1000/900/1100 je Schwierigkeit @0x8010067c-b8), wie
         * der G5 (@0x801003fc, re15_bf_morph_zu_g5). Port-Mass: die 0x96 der Mutation @0x80118094-98, mit der die 0x30-Waffen
         * rechnen. Ohne das kam die Form unter Dauerfeuer mit dem Mutationsrest an: gemessen HP 30, tot nach 49 Bildern
         * noch im Auftritt, kein einziger Angriff (Lauf a1_leon, Dossier Nachbesserung). */
        e->hp = 150;
        /* Abnahme 1 M1: der Schuss laeuft VOR der KI (game_step_common.c), ein Treffer im Tauschbild steht also schon in
         * Zustand 2 oder 3. Der Auftritt ist dann der gesicherte Zustand (+0x1fc), den der Handler fortsetzt @0x80107180-8c;
         * wer ihn ueberschrieb, verlor case 2 und damit das einzige Armieren des Riegel-Zaehlers (+0x93 Bit 0 blieb stehen). */
        s_bb.s_state = 1; s_bb.s_sub = BB_AUFTRITT; s_bb.s_ph2 = 0; s_bb.s_ph3 = 0;
        s_bb.gesichert = 1;
        if (e->state == 3) e->state = 2;               /* gilt der alten Form; die neue hat noch 150 */
        if (e->state != 2) {
            e->state = 1; bb_setze(e, BB_AUFTRITT);
            /* Riegel ohne laufenden Zaehler loest hier niemand mehr (nur der Vortakt unten ueber +0x1de). */
            if (e->birkin_hurt_cd == 0) e->hit_react = (uint8_t)(e->hit_react & ~1u);
        }
    }
    /* Vortakt 0x80100134 / Port-Gegenstuecke */
    if (s_bb.cd_nah)   s_bb.cd_nah--;                  /* @0x80100234-44 */
    if (s_bb.cd_lauf)  s_bb.cd_lauf--;                 /* @0x80100248-58 */
    if (s_bb.cd_griff) s_bb.cd_griff--;                /* @0x80100270-80 */
    if (s_bb.meter_uhr == 0) {                         /* @0x801001d8-230: alle 16 Bilder Zaehler -1 */
        if (s_bb.meter) { s_bb.meter--; s_bb.meter_uhr = 15; }
    } else s_bb.meter_uhr--;
    if (e->birkin_hurt_cd > 0 && --e->birkin_hurt_cd == 0)   /* 0x30-Riegel +0x1de @0x801163c8-400, damit der */
        e->hit_react = (uint8_t)(e->hit_react & ~1u);        /* Port-Trefferpfad wieder annimmt */

    dist = re15_enemy_player_dist(e, pl);              /* +0x1f0 */
    e->dog_dist = (int16_t)dist;

    switch (e->state) {
    case 2: {                                          /* TREFFER 0x80106d3c: Zaehler, erst ab 12 die Reaktion */
        /* Zuwachs je WAFFE, nicht je KI-Unterphase (Abnahme 1 M12): `lbu v1,5(s2)` @0x80107130 liest die Waffen-Id, die der
         * Applier nach +0x5 schrieb (re15_damage.c, RE2 @0x80041ab4 / @0x80047324); die Unterphase liegt in +0x1fc. Index
         * im Port ueber die RE2-Id, Tabelle 19 Eintraege bis 4 @0x8010b10d. */
        static const uint8_t k_zuwachs[19] = { 8, 1, 3, 3, 3, 10, 15, 5, 8, 10, 8, 8, 3, 2, 20, 2, 2, 20, 4 };   /* @0x8010b0fb */
        const unsigned waffe = re15_re2z_weapon_id(e->sub_state_1);
        const unsigned zone  = e->sub_state_2;
        uint8_t sub = s_bb.gesichert ? s_bb.s_sub : (uint8_t)BB_GEHEN;
        e->birkin_hurt_cd = 9;                         /* +0x1de = 9 @0x8011a0c4 (0x30-Riegel, s.o.) */
        /* Blut und Ton nicht bei +5 == 0x10 / 0xc @0x80107414-1c, Ton nicht bei +5 == 1 @0x80107500-08. Handler 2 spielt
         * se(0xd) @0x8010750c, in Bank 24 ist Id 13 leer (FFFFFFFF); hoerbar ist se(2) des Handlers 1 @0x80107058. */
        if (waffe != 0x10u && waffe != 0x0cu) {
            bb_treffer_blut(e, pl, zone);
            if (waffe != 1u) bb_se(2);
        }
        if (sub != BB_ZUCKEN) {
            s_bb.meter = (uint8_t)(s_bb.meter + (waffe < 19u ? k_zuwachs[waffe] : 8u));   /* @0x80107130-4c */
            if (s_bb.meter_uhr == 0) s_bb.meter_uhr = 15;                                 /* @0x80107160-64 */
        }
        if (sub == BB_ZUCKEN || s_bb.meter < 12) {     /* @0x80107168-8c: weiter wie vor dem Treffer */
            e->state = s_bb.gesichert ? s_bb.s_state : 1;
            e->sub_state_1 = sub;
            e->sub_state_2 = s_bb.gesichert ? s_bb.s_ph2 : 0;
            e->sub_state_3 = s_bb.gesichert ? s_bb.s_ph3 : 0;
        } else {                                       /* @0x80107198-1a4 */
            s_bb.meter = 0; s_bb.meter_uhr = 0;
            e->state = 1;
            bb_setze(e, BB_ZUCKEN);
        }
        break;
    }
    case 3:                                            /* Port: statt Clip 17 @0x80107b3c die Rueckstoss-Szene */
        if (re15_bf_bahnsteig_aktiv()) { re15_bf_rueckstoss_start(slot); return; }
        e->hp = 150; e->state = 1; bb_setze(e, BB_ZUCKEN);   /* RE15_BIRKIN_FORM ausserhalb 5090: nur Sichtpruefung */
        e->birkin_hurt_cd = 9;
        break;
    default:
        e->state = 1;
        break;
    }

    if (e->state == 1) {
        if (e->sub_state_1 == BB_GEHEN && e->sub_state_2 != 0) bb_entscheiden(e, pl, dist);
        switch (e->sub_state_1) {
        case BB_AUFTRITT: bb_auftritt(e, pl); break;
        case BB_HIEB:     bb_hieb(e, pl, dist); break;
        case BB_RENNEN:   bb_rennen(e, pl, dist); break;
        case BB_WEICHEN:  bb_weichen(e, pl, dist); break;
        case BB_BISS:     bb_biss(e, pl); break;
        case BB_GRIFF:    bb_griff(e, pl, dist); break;
        case BB_ZUCKEN:                                /* @0x8010721c-68 */
            if (e->sub_state_2 == 0) { bb_clip(e, BB_C_ZUCKEN, 5, 7); e->sub_state_2 = 1; }
            if (bb_anim(e, 0x200)) bb_zu_gehen(e);     /* a3 @0x80107244; 0x101 @0x8010724c; Bodenphase: Sub 3 */
            break;
        case BB_GEHEN:
        default:
            if (e->sub_state_1 != BB_GEHEN) bb_zu_gehen(e);
            bb_gehen(e, pl);
            break;
        }
        bb_bild_ton();
    }
    s_bb.s_state = e->state; s_bb.s_sub = e->sub_state_1;   /* +0x1fc Sicherung */
    s_bb.s_ph2 = e->sub_state_2; s_bb.s_ph3 = e->sub_state_3;
    s_bb.gesichert = 1;

    /* Mess-Schiene wie im 0x30-Tick (RE15_BIRKIN_DBG=1 -> birkin_dbg.log), je 15 Ticks eine Zeile */
    {
        static int dbg = -1; static unsigned n = 0;
        if (dbg < 0) dbg = getenv("RE15_BIRKIN_DBG") != NULL;
        if (dbg && (n++ % 15u) == 0u) {
            FILE *f = fopen("birkin_dbg.log", "a");
            if (f) {
                fprintf(f, "FORM t=%u st=%u sub=%u ph=%u clip=%u bild=%u pos=(%d,%d,%d) gier=%d dist=%d hp=%d cd=%u/%u/%u biss=%u tempo=%d\n",
                        n - 1u, e->state, e->sub_state_1, e->sub_state_2, e->motion, e->anim_frame, (int)e->x, (int)e->y,
                        (int)e->z, (int)e->rot_y, (int)dist, (int)e->hp, s_bb.cd_nah, s_bb.cd_griff, s_bb.cd_lauf,
                        s_bb.biss_rest, (int)s_bb.tempo);
                fclose(f);
            }
        }
    }
}

/* ---- Messhaken der Sichtpruefung ------------------------------------------------------------- *
 * RE15_BB_POSE="x,z,gier,clip,bild,schnitt": der 0x30 steht fest auf dieser Lage, bild < 0 spielt den Clip ab,
 * schnitt >= 0 haelt die Kamera. Im Normalpfad stumm (kein env = kein Eingriff). Aufruf vor der Gegner-Schleife. */
static int s_bb_pose = -1, s_bb_pw[6];

int re15_bb_pose_aktiv(void)
{
    if (s_bb_pose < 0) {
        const char *v = getenv("RE15_BB_POSE");
        s_bb_pose = (v && sscanf(v, "%d,%d,%d,%d,%d,%d", &s_bb_pw[0], &s_bb_pw[1], &s_bb_pw[2], &s_bb_pw[3],
                                 &s_bb_pw[4], &s_bb_pw[5]) == 6) ? 1 : 0;
    }
    return s_bb_pose;
}

void re15_bb_pose_tick(void)
{
    if (!re15_bb_pose_aktiv()) return;
    {   /* je 150 Ticks alle Aktoren (Messlauf, Raumvergleich) */
        static unsigned t = 0;
        if ((t++ % 150u) == 0u) {
            FILE *f = fopen("bb_pose.log", "a");
            if (f) {
                for (int i = 0; i < RE15_ACTOR_MAX; i++)
                    if (g_actors[i].active)
                        fprintf(f, "t%u slot%d typ 0x%02x st %d/%d pos (%d,%d,%d) gier %d clip %d bild %d grid 0x%02x kein_bild %d\n", t, i,
                                g_actors[i].type, g_actors[i].state, g_actors[i].sub_state_1, (int)g_actors[i].x, (int)g_actors[i].y,
                                (int)g_actors[i].z, (int)g_actors[i].rot_y, g_actors[i].motion, g_actors[i].anim_frame,
                                g_actors[i].grid_id, g_actors[i].no_draw);
                fclose(f);
            }
        }
    }
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x30u) continue;
        e->x = s_bb_pw[0]; e->y = 0; e->z = s_bb_pw[1];
        e->rot_y = (int16_t)(s_bb_pw[2] & 0xfff);
        e->no_draw = 0; e->state = 1;
        e->motion = (uint8_t)s_bb_pw[3]; e->anim_frac = 0;
        if (s_bb_pw[4] >= 0) e->anim_frame = (uint8_t)s_bb_pw[4];
        else (void)re15_r38_birkin_anim(e);
    }
    if (s_bb_pw[5] >= 0 && ((int)g_scd.work_vars[0x0A] != s_bb_pw[5] || g_scd.cam_id != s_bb_pw[5])) {
        g_scd.cam_id = (uint8_t)s_bb_pw[5];
        g_scd.cam_change_pending = 1;
    }
}
