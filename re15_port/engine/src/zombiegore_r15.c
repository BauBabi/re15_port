/*
 * RE1.5 Rebuilt — waffenabhaengige Treffer- und Todes-Handler der Zombie-Familie (STAGE1, Wurzel 0x80100424).
 *
 * Nutzer-Auftrag 2026-10-30: "die fehlenden Gore-Reaktionen auf die unterschiedlichen Waffen". Quelle: eigene
 * Disassembly STAGE1.BIN (Overlay roh @0x80100000), Spezifikation analysis/video_80prozent/zombie_gore_spec.md.
 *
 *   HURT  @0x8011fb90 [+0x5 Waffe * 0x20 + +0x6 Hoehe * 4]:  w5/6 -> 0x80106290, w7/8 Mitte -> 0x80106624,
 *         w19 -> 0x80106048 (w1/3/4 = 0x80105b7c bleibt der vorhandene Port-Stagger).
 *   DEATH @0x8011feac (Wurzel FUN_80106ba4 loest JEDEN Tick neu auf): w7 Mitte -> 0x80106edc (Kopf platzt) bzw.
 *         Spalte 5 -> 0x80107244 (kopflos weiter), w8 Mitte -> 0x80107ee0 (Schrot), w7/8/19 unten -> 0x80108abc
 *         (Beine weg -> Kriecher); alle uebrigen Zellen bleiben der vorhandene Standard-Tod 0x80106c18.
 * Die Teil-Woerter/-Felder schreibt dieses Modul in die RE1.5-Teilschicht (re15_teile_r15.h), der Zeichner liest sie.
 * Blut FUN_80019700(a0, +0x6a, Teil+0x40, a3): a0 = Effekt<<24 | Sub<<16 | Skala, a3 = Teil-lokaler Versatz
 * (Deskriptoren @0x8011fe84 (200,0,0), @0x8011fe9c (100,-200,0), @0x8012016c (200,0,0), @0x8012017c (0,-700,0)).
 *
 * OFFEN (benannt, nicht geraten): die Wandklemme des fliegenden Oberkoerpers (FUN_8003b0a4 @0x80108448ff), die
 * Huefte-Drehung um 0x800 in 0x80107ee0 Phase 2 (Teil 0 +0x64/+0x7c) und Bit 0x200 (Rotation einfrieren) — beide
 * Skelett-seitig; +0x1b8 (Todes-Byte) hat im Port keinen Leser.
 */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_audio.h"
#include "re15_skeleton.h"
#include "re15_teile_r15.h"
#include "re15_scd.h"           /* re15_game_flag_set */
#include "re15_ai_flavor.h"     /* re15_re2z_footlock, re15_ai_re2_for_type, re15_re2z_owns_type */
#include "re15_math.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int  re15_enemy_clip_done_pub(const re15_actor_t *e);   /* enemy_ai_common.c */
void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);

static const int32_t OFS_FE84[3] = { 200, 0, 0 };     /* @0x8011fe84 */
static const int32_t OFS_FE9C[3] = { 100, -200, 0 };  /* @0x8011fe9c */
static const int32_t OFS_016C[3] = { 200, 0, 0 };     /* @0x8012016c */
static const int32_t OFS_017C[3] = { 0, -700, 0 };    /* @0x8012017c */

/* Gore-Teil u8[0x8011f784 + Typ] (Bytes @0x8011f794 fuer 0x10..0x1f). */
static const uint8_t s_gore_teil[16] = { 14, 14, 14, 8, 8, 14, 14, 14, 14, 14, 14, 8, 8, 8, 8, 0 };
static int gore_teil(const re15_actor_t *e) { return (e->type >= 0x10 && e->type <= 0x1f) ? s_gore_teil[e->type - 0x10] : 14; }

/* Familie mit den Handlern 0x80100424 / Zombie-Maedchen 0x13 (gleiche Tabellen @0x8012039c/@0x8012063c), RE1.5-KI. */
int re15_zgore_typ(const re15_actor_t *e)
{
    if (!e || e->type < 0x10 || e->type > 0x1f || e->type == 0x14 || e->type == 0x15 || e->type == 0x17
        || e->type == 0x19 || e->type == 0x1a || e->type == 0x1b) return 0;
    if (re15_ai_re2_for_type(e->type) && re15_re2z_owns_type(e->type)) return 0;   /* RE2-Brain = RE2-Gore */
    return 1;
}

static void fx(re15_actor_t *e, uint32_t a0, int teil, const int32_t ofs[3])
{
    int32_t p[3];
    re15_enemy_part_point(e, teil, ofs, p);
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), (uint8_t)(a0 >> 24), (uint8_t)((a0 >> 16) & 7), (uint16_t)(a0 & 0xffff),
                         p[0], p[1], p[2], (int16_t)e->rot_y);
}
static re15_teil_t *teil(re15_actor_t *e, int k)
{
    int slot = (int)(e - g_actors);
    re15_teil_t *t = re15_teile(slot);
    re15_teile_markieren(slot);
    return (t && k >= 0 && k < RE15_TEILE_MAX) ? &t[k] : NULL;
}
/* +0x94 = 0x80 / +0x98 = 0x20 (sw) + Wort |= 0x80: roter Stumpf. */
static void stumpf(re15_actor_t *e, int k)
{
    re15_teil_t *t = teil(e, k); if (!t) return;
    t->h94[0] = 0x80; t->h94[1] = 0; t->h94[2] = 0x20; t->h94[3] = 0;
    t->w |= 0x80;
}
/* FUN_80108d04(P) @0x80108d04-6c: Blut 0x2000 (a3 @0x8012016c), +0x68 = 0x8f, +0x94..+0x9e = 0, Wort |= 0x4a. */
static void zerplatzen(re15_actor_t *e, int k, int16_t vy, int16_t vy_inc)
{
    fx(e, 0x2000, k, OFS_016C);
    re15_teil_t *t = teil(e, k); if (!t) return;
    t->col = 0x8f;
    memset(t->h94, 0, sizeof t->h94);
    t->h94[1] = (uint16_t)vy; t->h94[3] = (uint16_t)vy_inc;   /* nur der Kopfplatzer: -50 / 3 @0x801070bc-d0 */
    t->w |= 0x4a;
}
static void wort_null(re15_actor_t *e, int k) { re15_teil_t *t = teil(e, k); if (t) t->w = 0; }
static void skala(re15_actor_t *e, int k, int16_t x, int16_t y, int16_t z)
{
    re15_teil_t *t = teil(e, k); if (!t) return;
    t->scale[0] = x; t->scale[1] = y; t->scale[2] = z; t->w |= 0x400;
}
static void kill_flag(re15_actor_t *e)
{
    if (e->em_flag_id != 0xFF) re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);   /* FUN_8004ef90(+0x1c6) */
}
/* FUN_800245d8(ofs) mit +0x8c. */
static void vorschub(re15_actor_t *e, int ofs)
{
    int16_t a = (int16_t)(((int)e->rot_y + ofs) & 0xfff);
    e->x += (int32_t)(((int32_t)re15_cos_q12(a) * e->speed_h) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(a) * e->speed_h) >> 12);
}
static int bit7(const re15_actor_t *e) { return (e->hit_react >> 7) & 1; }
static int fertig(re15_actor_t *e) { return re15_enemy_clip_done_pub(e); }
static void neuer_clip(re15_actor_t *e, uint8_t c, uint8_t blende) { e->motion = c; e->anim_frame = 0; e->anim_frac = blende; }

/* ============================== HURT ============================================================================ */

/* Poise-Tabelle @0x8011fe30[+5] (21 Eintraege, wie stun_step in re15_enemy_ai_live_hurt). */
static void poise(re15_actor_t *e)
{
    static const int8_t stun_step[21] = { 0, -2, -2, -3, -3, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1 };
    e->hit_stun = (int16_t)(e->hit_stun + (e->sub_state_1 < 21 ? stun_step[e->sub_state_1] : 0));
}
static void ausgang_201(re15_actor_t *e, int maske)
{
    re15_ai_set_state_word(e, 0x201u);
    e->hit_react &= (uint8_t)~1u;
    if ((re15_engine_rand8() & maske) == 0) e->sub_state_1 = 8;
    if (re15_ai_facing_aligned(e, &g_actors[RE15_ACTOR_SLOT_PLAYER])) e->sub_state_1 = 7;
}

/* 0x80106290 — Beretta/Glock (w5/6): drei Clips 6 -> 7 -> 8 aus Bank 1, je Phasenwechsel Blut. */
static void z_beretta(re15_actor_t *e, int slot)
{
    int c = e->sub_state_2;
#define BLUT_6290() do { if (c == 1) fx(e, 0x1000, gore_teil(e), OFS_FE84); else if (c == 0) fx(e, 0x2000, 2, OFS_FE84); } while (0)
    switch (e->sub_state_3) {
    case 0:
        e->sub_state_3 = 1; neuer_clip(e, 6, 7);                  /* @0x801062e8-318 */
        BLUT_6290();                                              /* 0x1000 @0x8010637c / 0x2000 @0x801063b0 */
        e->hit_react |= 1; re15_audio_room_se(6);                 /* SE 6 @0x801063d0 */
        poise(e);
        break;
    case 1:
        if (fertig(e)) { e->sub_state_3 = 2; neuer_clip(e, 7, 7); BLUT_6290(); }   /* @0x8010640c, Blut @0x80106530/568 */
        break;
    case 2:
        if (fertig(e)) { e->sub_state_3 = 3; neuer_clip(e, 8, 7); BLUT_6290(); }   /* @0x80106484 */
        break;
    default:
        if (fertig(e)) ausgang_201(e, 7);                         /* @0x80106578-600 */
        break;
    }
#undef BLUT_6290
    re15_re2z_footlock(slot, e);                                  /* FUN_8010939c(0,0) @0x80106608 */
}

/* 0x80106624 — Magnum/Schrot nicht toedlich (Spalte 1): Rueckstoss-Rutschen, kein Poise. */
static void z_rueckstoss(re15_actor_t *e)
{
    if (e->sub_state_3 == 0) {
        e->sub_state_3 = 1;
        e->zg_9f = (int8_t)bit7(e);                               /* +0x9f = +0x93>>7 */
        neuer_clip(e, (uint8_t)(16 - e->zg_9f), 3);               /* Clip 0x10/0x0f, +0x8f 3 */
        e->speed_h = 100; e->zg_9e = 12;                          /* +0x8c = 0x64, +0x9e = 12 */
        e->hit_react |= 1;
        fx(e, 0x2000, 0, OFS_FE84);                               /* @0x801066fc */
        fx(e, 0x2000, gore_teil(e), OFS_FE84);                    /* @0x80106750 */
        if ((re15_engine_rand8() & 1) == 0) re15_audio_room_se(6);   /* @0x80106758-70 */
    } else {
        e->anim_blend_rate = 0x400;                               /* anim_set a3 0x400 @0x80106774 */
        if (fertig(e)) ausgang_201(e, 1);
    }
    vorschub(e, 0x800 - ((int)e->zg_9f << 11));                   /* @0x8010680c */
    e->speed_h = (int16_t)(e->speed_h - e->zg_9e);
    if (e->zg_9e > 0) e->zg_9e--;
}

/* 0x80106048 — MC51 (w19): Zufallsclip, Blut 0x1500 an Teil 7, rueckwaerts rutschen. */
static void z_mc51(re15_actor_t *e, int slot)
{
    static const uint8_t clips[8] = { 6, 8, 6, 7, 6, 8, 8, 7 };   /* @0x8011fe94 */
    switch (e->sub_state_3) {
    case 0:
        e->sub_state_3 = 1; neuer_clip(e, clips[re15_engine_rand8() & 7], 7);
        e->speed_h = 0x0a;
        if (e->sub_state_2 == 1) fx(e, 0x1500, 7, OFS_FE9C);      /* @0x80106120 */
        else if (e->sub_state_2 == 0) fx(e, 0x2000, 2, OFS_FE84); /* @0x80106154 */
        e->hit_react |= 1;
        if ((re15_engine_rand8() & 7) == 0) re15_audio_room_se(6);   /* @0x80106188 */
        poise(e);
        /* faellt durch */
    case 1:
        e->sub_state_3 = (uint8_t)(e->sub_state_3 + fertig(e));
        break;
    default:
        ausgang_201(e, 7);
        break;
    }
    re15_re2z_footlock(slot, e);                                  /* @0x8010626c */
    vorschub(e, 0x800);                                           /* @0x80106274 */
}

int re15_zgore_treffer(re15_actor_t *e, int slot)
{
    if (!re15_zgore_typ(e) || (e->grid_id & 0x80)) return 0;
    int w = e->sub_state_1, c = e->sub_state_2;
    if ((w == 5 || w == 6) && c <= 1)       z_beretta(e, slot);
    else if ((w == 7 || w == 8) && c == 1)  z_rueckstoss(e);
    else if (w == 19 && c <= 1)             z_mc51(e, slot);
    else return 0;
    /* Router-Tor @0x80105b18-68: Poise gebrochen -> Knockdown +0x4 1 / +0x5 0x11 / +0x6 0. */
    if (e->hit_stun < 0 && !(e->grid_id & 0x80)) {
        e->state = (uint8_t)RE15_AI_STATE_ACTIVE; e->sub_state_1 = 0x11; e->sub_state_2 = 0;
    }
    return 1;
}
int re15_zgore_treffer_aktionsbank(const re15_actor_t *a)
{
    if (!a || a->state != 2 || (a->grid_id & 0x80) || !re15_zgore_typ(a)) return 0;
    int w = a->sub_state_1, c = a->sub_state_2;
    return ((w == 5 || w == 6) && c <= 1) || ((w == 7 || w == 8) && c == 1) || (w == 19 && c <= 1);
}

/* ============================== DEATH =========================================================================== */

/* Kopfplatzer-Kern (0x80106edc @0x801070b0-e4 / 0x80107244 @0x801073dc-410): Gore-Teil zerplatzt (vy -50, +3),
 * SE 9, Teil 7 roter Stumpf, Kill-Flag. */
static void kopf_ab(re15_actor_t *e)
{
    zerplatzen(e, gore_teil(e), -50, 3);
    e->hit_react |= 1;
    re15_audio_room_se(9);                                        /* @0x801070e0 */
    stumpf(e, 7);                                                 /* +0x548 0x80 / +0x54c 0x20 @0x80107104-120 */
    kill_flag(e);
}

/* 0x80106edc — Magnum, Mitte. */
static void z_kopfplatzer(re15_actor_t *e, int slot)
{
    if (e->sub_state_3 == 0) {
        if ((re15_engine_rand8() & 1) == 0) { e->sub_state_2 = 5; return; }   /* +0x6 = 5 @0x80106f38-5c -> 0x80107244 */
        e->hit_react |= 1; e->sub_state_3 = 1;
        e->motion = (uint8_t)(13 - 2 * bit7(e)); e->anim_frame = (uint16_t)(re15_engine_rand8() & 3); e->anim_frac = 0xf;
        e->anim_flags &= (uint16_t)~0x04u;
        int g = gore_teil(e);
        fx(e, 0x2000, g, OFS_016C);                               /* @0x8010702c */
        fx(e, 0x2000, g, OFS_016C);                               /* @0x80107084 */
        fx(e, 0x05002800u, g, OFS_016C);                          /* Effekt 5 @0x801070a4 */
        kopf_ab(e);
        return;
    }
    if (e->sub_state_3 == 1) e->sub_state_3 = (uint8_t)(e->sub_state_3 + fertig(e));
    if (e->anim_frac == 0) re15_re2z_footlock(slot, e);           /* Tail +0x8f == 0 */
    if (e->sub_state_3 >= 2) { e->state = (uint8_t)RE15_AI_STATE_CORPSE; e->sub_state_1 = 0; e->sub_state_2 = 0; }
    /* Phase >= 2 ist im Original toter Code (@0x80106f0c/10 genullt); der Leichen-Uebergang folgt dem Port-Standard. */
}

/* 0x80107244 — Spalte 5: kopflos weiterlaufen (nur Magnum), bis zu 5 Halsfontaenen. */
static void z_kopflos(re15_actor_t *e, int slot)
{
    if (e->sub_state_3 == 0) {
        e->hit_react |= 1; e->sub_state_3 = 1;
        e->zg_9e = 0x28; e->zg_9f = (int8_t)((re15_engine_rand8() & 0x3f) + 60);   /* +0x9e 40, +0x9f @0x80107244ff */
        e->motion = e->hurt_clip; e->anim_frame = (uint16_t)(re15_engine_rand8() & 3); e->anim_frac = 7;
        e->anim_flags |= 0x04u;                                   /* Gang-Clip laeuft im Kreis */
        e->zg_9c = 4;                                             /* Fontaenen-Zaehler */
        int g = gore_teil(e);
        fx(e, 0x2000, g, OFS_016C);                               /* @0x80107358 */
        fx(e, 0x05002800u, g, OFS_016C);                          /* @0x801073b4 */
        fx(e, 0x2000, g, OFS_016C);                               /* @0x801073d0 */
        kopf_ab(e);
        e->hit_react &= (uint8_t)~2u;
    }
    if (e->hit_react & 2) {                                       /* weiterer Treffer @0x801074b8-e8 */
        fx(e, 0x2000, 7, OFS_017C);
        e->hit_react &= (uint8_t)~2u;
        if (e->zg_9c-- == 0) e->zg_9f = 0;
    }
    re15_enemy_steer_point(e, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, e->zg_9e);   /* FUN_8001a8f8 @0x80107554 */
    if (e->zg_9f-- == 0) {                                        /* Ende: +6/+7 = 0x101 -> 0x80106edc Phase 1 */
        e->sub_state_2 = 1; e->sub_state_3 = 1;
        neuer_clip(e, 0x0b, 0xf); e->anim_flags &= (uint16_t)~0x04u;
    }
    re15_re2z_footlock(slot, e);                                  /* @0x801075e4-60c */
}

/* 0x80107ee0 — Schrot, Mitte. */
static void z_schrot(re15_actor_t *e, int slot, int granate)
{
    re15_teil_t *t7 = teil(e, 7);
    switch (e->sub_state_3) {
    case 0: {
        e->hit_react |= 1; e->sub_state_3 = 1;
        e->motion = (uint8_t)(13 - 2 * bit7(e)); e->anim_frame = (uint16_t)(re15_engine_rand8() & 3); e->anim_frac = 0xf;
        e->anim_flags &= (uint16_t)~0x04u;
        if (!(e->hit_react & 0x40) && !granate) { e->sub_state_2 = 6; e->sub_state_3 = 0; return; }   /* @0x80107ff4-8008 -> 0x80106c18 */
        stumpf(e, 7); stumpf(e, 0);                               /* @0x8010800c-54 */
        if ((re15_engine_rand8() & 1) && e->type != 0x13) skala(e, 0, 0x5f4, 0x1000, 0x5f4);   /* @0x80108058-98 */
        fx(e, 0x2800, 0, OFS_016C);                               /* @0x801080b8 */
        re15_audio_room_se(9);
        int g = gore_teil(e);
        if ((re15_engine_rand8() & 1) && !granate) {              /* ZERFETZEN @0x801080c8-d4 */
            e->sub_state_3 = 3;
            zerplatzen(e, g, 0, 0);                               /* @0x80108134 */
            if (re15_engine_rand8() & 1) { skala(e, 8, 0x5f4, 0xc18, 0x5f4); zerplatzen(e, 9, 0, 0); }   /* @0x8010818c */
            else { zerplatzen(e, 8, 0, 0); wort_null(e, 9); }     /* @0x8010819c / @0x801081b8 */
            wort_null(e, 10);                                     /* @0x801081d0 */
            if (re15_engine_rand8() & 1) { skala(e, 11, 0x5f4, 0xc18, 0x5f4); zerplatzen(e, 12, 0, 0); }   /* @0x80108224 */
            else { zerplatzen(e, 11, 0, 0); wort_null(e, 12); }   /* @0x80108250 */
            wort_null(e, 13);                                     /* @0x8010826c */
            e->speed_h = 200;
            kill_flag(e);
            return;
        }
        /* OBERKOERPER AB @0x801082d4ff */
        if ((re15_engine_rand8() & 3) == 0) re15_audio_room_se((re15_engine_rand8() & 1) ? 6 : 8);
        if (t7) {
            t7->w |= 0x40;                                        /* @0x80108328 */
            const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            int32_t dx = e->x - pl->x, dz = e->z - pl->z;
            int32_t len = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz)); if (len < 1) len = 1;
            t7->v[0] = (int16_t)(((dx * 4096) / len) >> 3);       /* VectorNormalS (0x80066a1c) dann >>= 3 */
            t7->v[2] = (int16_t)(((dz * 4096) / len) >> 3);
            t7->v[1] = 0x50;                                      /* +0x3a */
            t7->grav = 3; t7->spin_inc = 10; t7->spin = 8;        /* +0x75 Aufschlaege, +0x76 Schwerkraft, +0x82 Drehzaehler */
        }
        fx(e, 0x2800, 7, OFS_016C);                               /* @0x801083cc */
        fx(e, 0x05002800u, 7, OFS_016C);                          /* @0x801083ec */
        fx(e, 0x2000, g, OFS_016C);                               /* @0x80108440 */
        if (granate) {                                            /* GESTALTUNG Video 3 8:10-8:22: grosse Blutlachen */
            int32_t p[3]; re15_enemy_part_point(e, 7, OFS_016C, p);
            re15_esp_fx_splatter(re15_esp_room_bank(), 0, 16, p[0], p[1], p[2], e->y);
            re15_esp_fx_splatter(re15_esp_room_bank(), 0, 16, e->x, e->y - 300, e->z, e->y);
        }
    }   /* faellt in Phase 1 */
    /* fallthrough */
    case 1:                                                       /* Oberkoerper fliegt @0x80108448 */
        if (t7 && t7->fest) {
            t7->t[0] += t7->v[0]; t7->t[2] += t7->v[2];
            t7->v[0] = (int16_t)(t7->v[0] >> 1); t7->v[2] = (int16_t)(t7->v[2] >> 1);
            if (t7->grav == 0) { t7->spin_inc = 0; t7->v[1] = 0; }
            t7->t[1] += t7->v[1];
            t7->v[1] = (int16_t)(t7->v[1] + t7->spin_inc);
            int32_t boden = (int32_t)e->dog_floor_y - 200;
            if (boden < t7->t[1]) {
                t7->v[1] = (int16_t)(-(t7->v[1] >> 2)); t7->t[1] = boden;
                fx(e, 0x2000, 7, OFS_016C);                       /* @0x80108630 */
                if (t7->grav) t7->grav--;
            }
            if (t7->spin != 0) {
                t7->spin--;
                int32_t rz[9] = { re15_cos_q12(0x80), -re15_sin_q12(0x80), 0, re15_sin_q12(0x80), re15_cos_q12(0x80), 0, 0, 0, 0x1000 };
                int32_t r[9];
                for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
                    int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)t7->rot[i*3+k] * rz[k*3+j];
                    r[i*3+j] = (int32_t)(s >> 12); }
                memcpy(t7->rot, r, sizeof r);                     /* @0x801086b8 / @0x801086cc */
                if (t7->spin & 1) fx(e, 0x2000, 7, OFS_016C);     /* @0x80108774 */
            }
        }
        e->sub_state_3 = (uint8_t)(e->sub_state_3 + fertig(e));
        if (e->anim_frac == 0) re15_re2z_footlock(slot, e);
        break;
    case 2: {                                                     /* Kriecher-Uebergang @0x801087b8 */
        static const uint8_t beine[4] = { 1, 2, 4, 5 };
        for (int i = 0; i < 4; i++) { re15_teil_t *b = teil(e, beine[i]); if (b) b->w |= 0x40; }   /* @0x80108828-894 */
        if (t7 && t7->fest) { e->x = t7->t[0]; e->z = t7->t[2]; }  /* Aktor springt zum Oberkoerper */
        if (t7) { t7->w &= (uint16_t)~0x40u; t7->fest = 0; }
        fx(e, 0x2000, 7, OFS_016C);                               /* @0x801088f4 */
        e->grid_id = 0x81;                                        /* @0x80108950 */
        re15_ai_set_state_word(e, 1u);                            /* @0x80108960 */
        e->hp = 0x1e;                                             /* @0x80108970 */
        e->hit_react &= (uint8_t)~1u;
        e->ai_flags |= 0x80;
        break; }
    case 3:                                                       /* Zerfetzt rutschen @0x801089b0 */
        e->sub_state_3 = (uint8_t)(e->sub_state_3 + fertig(e));
        vorschub(e, (((uint8_t)~e->hit_react) >> 7) << 11);
        e->speed_h = (int16_t)(e->speed_h - 20); if (e->speed_h < 0) e->speed_h = 0;
        if (e->anim_frac == 0) re15_re2z_footlock(slot, e);
        break;
    default:                                                      /* @0x80108a74: Leiche */
        e->state = (uint8_t)RE15_AI_STATE_CORPSE; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* 0x80108abc — Magnum/Schrot/MC51 nach unten: Beine weg, weiter als Kriecher (kein Kill-Flag). */
static void z_beine(re15_actor_t *e)
{
    switch (e->sub_state_3) {
    case 0:
        e->hit_react |= 1; e->sub_state_3 = 1;
        e->motion = 0x0b; e->anim_frame = (uint16_t)(re15_engine_rand8() & 3); e->anim_frac = 0xf;
        e->anim_flags &= (uint16_t)~0x04u;
        e->ai_flags |= 0x80;
        zerplatzen(e, 2, 0, 0); zerplatzen(e, 3, 0, 0);           /* @0x80108ba0 / @0x80108bb8 */
        zerplatzen(e, 5, 0, 0); zerplatzen(e, 6, 0, 0);           /* @0x80108bd0 / @0x80108be8 */
        stumpf(e, 1); stumpf(e, 4);                               /* @0x80108c00-c50 */
        if (re15_engine_rand8() & 1) wort_null(e, 4);             /* @0x80108c48-50 */
        re15_audio_room_se((re15_engine_rand8() & 1) ? 6 : 5);    /* @0x80108c6c */
        break;
    case 1:
        e->sub_state_3 = (uint8_t)(e->sub_state_3 + fertig(e));
        break;
    default:
        e->grid_id = 0x81;                                        /* @0x80108cb0 */
        re15_ai_set_state_word(e, 1u);                            /* @0x80108cc0 */
        e->hp = 0x1e;                                             /* @0x80108cd0 */
        e->hit_react &= (uint8_t)~1u;
        break;
    }
}

int re15_zgore_80prozent(void)
{
    static int an = -1;
    if (an < 0) an = getenv("RE15_KEIN_80PROZENT_GORE") ? 0 : 1;
    return an;
}

int re15_zgore_tod(re15_actor_t *e, int slot)
{
    if (!re15_zgore_typ(e) || (e->grid_id & 0x80)) return 0;
    int w = e->sub_state_1, c = e->sub_state_2;
    if ((w == 7 || w == 8 || w == 19) && c == 0) { z_beine(e); return 1; }
    if (w == 7 && c == 1) { z_kopfplatzer(e, slot); return 1; }
    if (w == 7 && c == 5) { z_kopflos(e, slot); return 1; }
    if (w == 8 && c == 1) { z_schrot(e, slot, 0); return 1; }
    /* ⛔ GESTALTUNG (Nutzer-Auftrag "maximales Gore aus der 80%-Version", Video 3 8:10-8:22, BEFUND.md 5.2):
     * Granaten-Tode (+0x5 = 9/10/11 aus FUN_80012d60, Tabelle DAT_8006f430) sind im Auslieferungsstand NULL-Zellen
     * (DEATH[9..11][1] @0x8011feac = 0 -> jalr 0). Das Video zeigt Ober- und Unterkoerper getrennt, der Oberkoerper
     * kriecht weiter, grosse Blutlachen — genau der Oberkoerper-Zweig von 0x80107ee0. Abschaltbar:
     * RE15_KEIN_80PROZENT_GORE=1 (dann Standard-Tod wie bisher). */
    if (w >= 9 && w <= 11 && re15_zgore_80prozent()) { z_schrot(e, slot, 1); return 1; }
    return 0;                                                     /* uebrige Zellen: Standard-Tod 0x80106c18 */
}
int re15_zgore_tod_lokobank(const re15_actor_t *a)
{
    return a && a->state == 3 && re15_zgore_typ(a) && !(a->grid_id & 0x80) && a->sub_state_1 == 7 && a->sub_state_2 == 5;
}

/* MESSHAKEN (kein Verhalten im Normalpfad): RE15_ZGORE_TEST="tick:waffe:hoehe[:krit]" gibt ab dem KI-Tick `tick` dem
 * naechsten RE1.5-Zombie einen toedlichen Treffer mit +0x5 = waffe, +0x6 = hoehe (Bildpruefung des Gores). */
void re15_zgore_messhaken(void)
{
    static int an = -1, tick = 0, ziel = 0, w = 0, c = 0, krit = 0, erledigt = 0;
    if (an < 0) { const char *v = getenv("RE15_ZGORE_TEST"); an = (v && sscanf(v, "%d:%d:%d:%d", &ziel, &w, &c, &krit) >= 3) ? 1 : 0; }
    if (!an || erledigt || ++tick < ziel) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int best = -1; int64_t bd = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || !re15_zgore_typ(e) || e->state == 3 || e->state == 7) continue;
        int64_t dx = e->x - pl->x, dz = e->z - pl->z, d = dx * dx + dz * dz;
        if (best < 0 || d < bd) { best = i; bd = d; }
    }
    if (best < 0) return;
    re15_actor_t *e = &g_actors[best];
    e->hp = -1; e->state = 3; e->sub_state_1 = (uint8_t)w; e->sub_state_2 = (uint8_t)c; e->sub_state_3 = 0;
    e->hit_react = (uint8_t)((e->hit_react & 0x80) | 1 | (krit ? 0x40 : 0));
    erledigt = 1;
    fprintf(stderr, "[zgore] Messhaken: Slot %d Typ 0x%02x Waffe %d Hoehe %d Krit %d\n", best, e->type, w, c, krit);
}
