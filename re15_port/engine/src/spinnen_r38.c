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
int re15_spinnen_r38_lauer_re2(re15_actor_t *e)
{
    if (!e || e->type != 0x25) return 0;
    if (e->state == 0 && e->grid_id == 0x41) {
        /* RE1.5-INIT-Zweig fuer grid 0x41 (0x80110b6c): Clip 0x10 @0x80110b90, +0x95 = 0 @0x80110ba0,
         * +0x93 = 0 @0x80110bc0; grid & 0xf = 1 < 2 -> rot_x = rot_z = 0, y = -1800 * +0x82
         * (@0x80110c28-74); Zustandswort 0x104 (Zustand 4, +0x5 = 1) @0x80110d14; anim_set einmal
         * @0x80110d60 (Halteposen-Bild 1, gemessen `c16/1` im Original). */
        e->state = 4; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->rot_x = 0; e->rot_z = 0;
        e->y = -1800 * (int32_t)e->floor;
        e->hit_react = 0;
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
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) vorher[i] = (re15_esp_fx_get(i) != NULL);
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
    if (re15_hitbox_test(&g_actors[RE15_ACTOR_SLOT_PLAYER], &box)) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags |= 0x2u;   /* Gift (s.o.) */
        s_gift_treffer++;
    }
    int treffer = re15_resolve_attack(&box, 0, besitzer);     /* FUN_80012d60(500,&P,0) @0x80018008 */
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
 * Periode also 121 Bilder (Zaehler 0x78 .. 0, dann wieder Abzug). */
void re15_spinnen_r38_gift_tick(re15_actor_t *pl)
{
    if (!pl) return;
    uint16_t a0 = pl->status_flags;
    if ((a0 & 7u) == 0) return;                               /* @0x80031e50-54 */
    uint8_t v1 = s_gift_zaehler;
    s_gift_zaehler = (uint8_t)(v1 + 255u);                    /* @0x80031e68-70 */
    if (v1 != 0) return;                                      /* @0x80031e74 */
    s_gift_zaehler = (a0 & 4u) ? 8u : 0x78u;                  /* @0x80031e78-94 */
    int16_t hp = (int16_t)(pl->hp - 2);                       /* @0x80031e9c-ac */
    pl->hp = hp;
    if (hp < 0 && !(a0 & 4u)) pl->hp = 1;                     /* @0x80031eb4-c8 */
}

/* ---- MESSSCHIENE RE15_SPINNEN_LOG ------------------------------------------------------------
 * Reiner Messhaken (env-gegated, kein Spielverhalten): je Bild und Spinne eine Zeile. Die GUI-exe
 * hat kein nutzbares stderr, deshalb eine Datei (Muster RE15_STATE_LOG, platform/pc/main.c). */
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
    fprintf(s_fp, "F%u R%04X PL(%d,%d,%d hp=%d st=%d ps=%04x fl=%d gz=%u) fx=%d", frame,
            (unsigned)g_current_room_id, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->hp,
            (int)pl->state, (unsigned)pl->status_flags, (int)pl->floor, (unsigned)s_gift_zaehler,
            re15_esp_fx_count());
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (!e->active || (e->type != 0x25 && e->type != 0x26)) continue;
        fprintf(s_fp, " [%d t=%02x st=%d s=%d.%d.%d g=%02x fl=%d m=%d gy=%d mo=%d af=%d"
                      " @(%d,%d,%d) r=(%d,%d,%d) hp=%d h93=%02x]",
                i, e->type, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                e->grid_id, e->floor, e->re2s_mode222, (int)e->dog_floor_y, e->motion,
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
