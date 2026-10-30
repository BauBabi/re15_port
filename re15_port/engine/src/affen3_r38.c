/*
 * RE1.5 Rebuilt — Runde 38 Spur S "affen3": die kleinen Affen (Typ 0x29, EM029) treffen wie im Original.
 *
 * Kopf, Belege und Grenzen: include/re15_affen3_r38.h; Messungen: analysis/befunde_runde38/S_affen3.md.
 * Alle Adressen STAGE3.BIN (Overlay @0x80100000, kein Kopf), ausser FUN_8001xxxx/FUN_8002xxxx = PSX.EXE.
 *
 * Feld-Abbildung (wie bisher in re15_cockroach_ai_tick): +0x5 sub_state_1, +0x6 sub_state_2, +0x7 sub_state_3,
 * +0x94 motion, +0x95 anim_frame, +0x8f anim_frac, +0x9c ai_timer, +0x8c crow_speed, +0x1dc dog_blocked_ctr,
 * +0x1e2 roach_beh2, +0x1d6 sca_wall_hit (Rueckgabe der Wandklemme FUN_8003b0a4, `sh v0,470` @0x80110bcc).
 */
#include "re15_affen3_r38.h"
#include "re15_affen.h"         /* Pool/Zeichenstand/Fuss-Sperre/Trefferpunkt der Affen-Familie (affen_11c0.c) */
#include "re15_enemy.h"
#include "re15_enemy_ai.h"      /* re15_player_stagger_cmd2, re15_player_knockdown_begin, re15_player_victim_devour,
                                 * re15_clip_root_motion_abs_pub = FUN_8001ad68 */
#include "re15_damage.h"        /* re15_engine_rand8, re15_ai_arc_test, re15_wound_add = FUN_80037edc,
                                 * re15_enemy_apply_hitbox */
#include "re15_audio.h"         /* re15_audio_room_se = FUN_800453d0, re15_audio_core_se = Se_on(0x04ss0001) */
#include "re15_esp.h"           /* re15_esp_fx_spawn_ex = FUN_80019700 (Blut) */
#include "re15_math.h"          /* re15_squareroot0 = SquareRoot0 0x80065f60 */
#include "re15_player.h"        /* re15_player_pl00_clip_frames (Leons eigene Bank) */
#include "re15_ai_flavor.h"     /* re15_re2z_mash = re15_mash_pressed = FUN_80037024 */
#include <string.h>

void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);   /* enemy_ai_common.c = FUN_8001a8f8-Kern */
int  re15_player_aim_active(void);                                                /* player_common.c: aca58 == 0x701 */
void re15_player_aim_interrupt(void);                                             /* player_common.c */
void re15_player_victim_bone_pos_pub(int bone, int32_t out[3]);                   /* enemy_ai_common.c: Part in der Opfer-Pose */
/* enemy_ai_common.c, Runde 38 Spur S: Spieler-Kommando 5 des Affen-Griffs setzen / loesen (g_player_victim). */
void re15_player_victim_affen3(const re15_actor_t *greifer, int an);

/* EM029 Bank 0 (CDEMD0.EMS Blob 13, dir[1] EDD): Bildzahl je Clip, 32 Clips — dieselbe Tabelle, die
 * re15_cockroach_ai_tick fuehrt (s_roach_clip_len), gegengelesen mit tools/emd_ansichtsblatt.py. */
static const uint8_t s_clip_len[32] =
    { 78,20,15,70,78,39,24,12,25,25,40,40,100,40,70,90,35,20,25,40,40,21,58,30,50,40,70,30,39,20,51,70 };

/* Trefferfenster: Tabelle @0x8011ed48 = {12,13,14,15} (Biss, Schleife 3..0 @0x80111ef4-f24) und
 * @0x8011ed4c = {21,22,23,24,33,34} (schwer, Schleife 5..0 @0x80112238-268). */
static const uint8_t s_fenster_biss[4]   = { 12, 13, 14, 15 };
static const uint8_t s_fenster_schwer[6] = { 21, 22, 23, 24, 33, 34 };

static re15_affen3_treffer_t s_log[RE15_AFFEN3_LOG_N];
static int s_log_n = 0;

void re15_affen3_log_reset(void) { s_log_n = 0; memset(s_log, 0, sizeof s_log); }
int  re15_affen3_log(re15_affen3_treffer_t *out, int max)
{
    int n = (s_log_n < max) ? s_log_n : max;
    if (out && n > 0) memcpy(out, s_log, (size_t)n * sizeof *out);
    return s_log_n;
}

/* ---- Grundbausteine ------------------------------------------------------------------------------ */

/* +0x5 := sub, +0x6 := 0, +0x7 := b7 (die Schreibsaetze `sb v0,5` / `sb zero,6` / `sb ..,7` aller A/B-Ausgaenge). */
static void wechsel(re15_actor_t *e, uint8_t sub, uint8_t b7)
{
    e->sub_state_1 = sub; e->sub_state_2 = 0; e->sub_state_3 = b7;
}

/* Clip-Eintritt: +0x94 := c, +0x95 := 0, +0x8f := 7 (z.B. @0x80111dd8-dfc). */
static void clip_setzen(re15_actor_t *e, uint8_t c)
{
    e->motion = c; e->anim_frame = 0; e->anim_frac = 7;
}

/* anim_set FUN_8001f314(+0x84, +0x16c, 0, 0x200) — a3 = 0x200 an allen Aufrufen dieses Moduls (z.B. @0x80111e18).
 * Wie re15_maggot_anim (Gorilla, gleiche EXE-Routine): Pose des Bildes VOR dem Vorschub in den Pool
 * (FUN_8001f3bc, Ueberblendung mit +0x8f vor dem Abbau), dann +0x95 weiter, +0x8f--, Rueckgabe 1 am Clip-Ende. */
static int anim_set(re15_actor_t *e)
{
    uint8_t c = e->motion; int fc = (c < 32) ? s_clip_len[c] : 1; if (fc < 1) fc = 1;
    re15_affen_pool_anim(e);
    int ende = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    if (e->anim_frac > 0) e->anim_frac--;
    return ende;
}

/* FUN_80115b68(0, a1): Fuss-Sperre an Knochen 14 + 3*a1 (Block a1*0x204 + 0x968 @0x80115ba0-bb0, Kette
 * +0x20 * Rec0 * Rec(12+3a1) * Rec(13+3a1) * Rec(14+3a1) @0x80115b98-bdc, +0x34/+0x3c -= m.t - rec[84/92]
 * @0x80115be4-c20) = Gorilla FUN_8011bf50. */
static void fuss_b68(re15_actor_t *e, int a1) { (void)re15_affen_fusssperre(e, 14 + 3 * a1); }
/* FUN_80115c3c(0, a1): Knochen 6 + 4*a1 (Block a1*688 + 1032 @0x80115c84-ca4, Kette Rec0 * Rec1 * Rec(4+4a1) *
 * Rec(5+4a1) * Rec(6+4a1) @0x80115c6c-cd0) = Gorilla FUN_8011c024. */
static void fuss_c3c(re15_actor_t *e, int a1) { (void)re15_affen_fusssperre(e, 6 + 4 * a1); }

/* FUN_8001a804(r, tol, &Spieler+0x34) < 0: Abstand SquareRoot0(dx*dx+dz*dz) (32-Bit-`lw`, `mflo` @0x8001a82c-60)
 * <= r (`slt s0,s0,s1` @0x8001a870 mit r als s16 @0x8001a868-6c) UND ((tol + atan2 - yaw) & 0xfff) < 2*tol
 * (@0x8001a8b0-c8, dann `or s1,s1,0x80000000`). atan2 = FUN_8001a6d4 = re15_atan2_q12 - 0x400 in der
 * Port-Gierkonvention (wie re15_enemy_steer_point / re15_dog_arc). */
static int a804(const re15_actor_t *e, const re15_actor_t *pl, int32_t r, int tol)
{
    uint32_t dx = (uint32_t)(pl->x - e->x), dz = (uint32_t)(pl->z - e->z);
    int32_t d = (int32_t)re15_squareroot0(dx * dx + dz * dz);
    if ((int32_t)(int16_t)r < d) return 0;
    int ang = (int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400;
    return ((tol + ang - (int)e->rot_y) & 0xfff) < 2 * tol;
}

/* FUN_8001a780(a0 = Spieler) @0x8001a788-a4: ((Spieler.yaw - Affe.yaw + 0x400) & 0xfff) < 0x800. */
static int a780(const re15_actor_t *e, const re15_actor_t *pl)
{
    return ((((int)pl->rot_y - (int)e->rot_y) + 0x400) & 0xfff) < 0x800;
}

int re15_affen3_trefferpunkt(const re15_actor_t *e, int record, int32_t out[3])
{
    return re15_affen_trefferpunkt(e, record, out);   /* a1 = (0x64,0,0) fuer Record 9, sonst (0,0,0) — s. Kopf */
}

int re15_affen3_quadrat(const re15_actor_t *e, const re15_actor_t *pl, int record, int32_t r)
{
    int32_t g[3];
    if (!re15_affen3_trefferpunkt(e, record, g)) {
        /* Ohne EM029-Bank (nur Unit-Harness ohne Modell; im Spiel laedt main.c die Bank je Raum-Aufstellung,
         * sonst gaebe es kein Bild): Ersatzpunkt = Aktor-Wurzel, wie re15_maggot_bone_square beim Gorilla. */
        g[0] = e->x; g[1] = e->y; g[2] = e->z;
    }
    /* @0x8001c080-a0: v0 = (plX - (ptX - r)) & 0xffff; a0 = (r << 16) >> 15 = 2r; `slt v0,a0,v0` -> kein Treffer.
     * Achse Z identisch @0x8001c0a8-c0. */
    uint16_t qx = (uint16_t)((uint16_t)(uint32_t)pl->x - (uint16_t)(uint32_t)g[0] + (uint16_t)r);
    uint16_t qz = (uint16_t)((uint16_t)(uint32_t)pl->z - (uint16_t)(uint32_t)g[2] + (uint16_t)r);
    return qx <= (uint16_t)(2 * r) && qz <= (uint16_t)(2 * r);
}

/* Treffer-Schreibsatz (Biss @0x80111f50-fcc, schwer @0x801122a4-324):
 *   Spieler-HP -= schaden; +0x1dc := 0x2d; SE; aca58 := 2; aca59 := a780 + (2 Biss | 4 schwer); aca5a := 0;
 *   HP < 0 -> aca58 := 3, aca59 := 0; Spieler+0x93 |= 1.
 * Port: aca59 = a780+2 -> re15_player_stagger_cmd2 (Clip 9 / 8, re15_affen_biss_clip), a780+4 ->
 * re15_player_knockdown_begin (Handler [5]/[4]) — dieselben Port-Wege wie der Gorilla (enemy_ai_common.c);
 * HP < 0 faengt der Todes-Detektor von game_step (cmd 3) wie dort. */
static void treffer(re15_actor_t *e, re15_actor_t *pl, int schaden, int se, int niederschlag, int32_t abstand)
{
    pl->hp = (int16_t)(pl->hp - schaden);
    e->dog_blocked_ctr = RE15_AFFEN3_SPERRE_TREFFER;
    re15_audio_room_se(se);
    if (pl->hp >= 0) {
        if (niederschlag) re15_player_knockdown_begin(a780(e, pl));
        else              re15_player_stagger_cmd2(re15_affen_biss_clip(e, pl));
    }
    pl->hit_react |= 1;
    if (s_log_n < RE15_AFFEN3_LOG_N) {
        re15_affen3_treffer_t *t = &s_log[s_log_n++];
        t->slot = (uint8_t)(e - g_actors); t->sub = e->sub_state_1; t->clip = e->motion; t->bild = e->anim_frame;
        t->schaden = (int16_t)schaden; t->hp_nach = pl->hp; t->abstand = (int16_t)abstand;
    }
}

/* ---- Bausteine der Subs 7..15 --------------------------------------------------------------------- */

/* Kasten +0x78 = *(0x8011ec44 + 4*(+0x1e4)) (Boden) bzw. *(0x8011ec48 + 4*(+0x1e4)) (Luft/Decke), +0x1e4 = (grid&8)?4:0
 * (INIT @0x80110e4c-70). Der Port fuehrt den Kasten in hit_radius_min/max/hit_height (re15_actor.h). */
static void kasten_boden(re15_actor_t *e)
{
    uint16_t r = (e->grid_id & 8) ? RE15_AFFEN3_KASTEN_BODEN8_R : RE15_AFFEN3_KASTEN_BODEN_R;   /* @0x8011ec14 / @0x8011ec2c */
    e->hit_radius_min = r; e->hit_radius_max = r; e->hit_height = RE15_AFFEN3_KASTEN_BODEN_H;
}
static void kasten_luft(re15_actor_t *e)
{
    e->hit_radius_min = RE15_AFFEN3_KASTEN_LUFT_R; e->hit_radius_max = RE15_AFFEN3_KASTEN_LUFT_R;   /* @0x8011ec20 / @0x8011ec38 */
    e->hit_height = RE15_AFFEN3_KASTEN_LUFT_H;
}

/* FUN_8001c1a4(hspd, vimp, grav, +0x1ba) @0x8001c1a4-2b4: (hspd,0,0) um die Gier gedreht auf x/z (RotMatrix @0x8001c1f0,
 * ApplyMatrixSV @0x8001c20c); s1 = (s16)(vimp + grav * (u16)+0x9c) (@0x8001c258-74); y -= s1 (@0x8001c278-7c);
 * Boden < y -> y = Boden, Rueckgabe s1 (@0x8001c294-b4), sonst +0x9c++ und 0 (@0x8001c2a0-b0). */
static int ballistisch(re15_actor_t *e, int hspd, int vimp, int grav)
{
    if (hspd) {
        e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * hspd) >> 12);
        e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * hspd) >> 12);
    }
    int32_t s1 = (int32_t)(int16_t)(vimp + grav * (int)(uint16_t)e->ai_timer);
    e->y -= s1;
    if ((int32_t)e->dog_floor_y < e->y) { e->y = e->dog_floor_y; return (int)s1; }
    e->ai_timer = (int16_t)(e->ai_timer + 1);
    return 0;
}

/* FUN_800245d8(0): +0x8c (`lhu v0,140` @0x800245f0) entlang der Gier (wie B[4] oben / re15_dog_advance_ofs(e,0)). */
static void vorschub(re15_actor_t *e)
{
    int32_t sp = (int32_t)(uint16_t)e->crow_speed;
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * sp) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * sp) >> 12);
}

/* FUN_80019700(0x2000, +0x6a, Record 0 + 0x40, &0x8011ec84) — Blut am Rumpf, wenn ein Schuss den Affen im Flug/an der
 * Decke trifft (A[9..14], B[7]/B[8]). Port wie der Gorilla (re15_maggot_gore): Effekt 0 der Raum-Bank am Aktor. */
static void blut(const re15_actor_t *e)
{
    /* a2 = Record 0 + 0x40 = Teil 0, a3 = 0x8011ec84 = Woerter (200, -800, 0) (Teil-lokaler Versatz, @0x8001a248). */
    static const int32_t ofs[3] = { 200, -800, 0 };
    re15_enemy_fx19700((re15_actor_t *)e, 0x2000, 0, ofs);
}

/* A[9]..A[14] (+ B[7]/B[8] Phase 1/2): Schuss im Flug. Der Schaden fasst den Affen hier nicht an, weil +0x93 Bit 0
 * steht (re15_damage.c `hit_react |= 2` + Rekursion @0x8001240c-18); er liest nur das Bit:
 *   +0x93 & 2 und & 0x40 (Magnum / Schrot < 3000, @0x800123b4-b8) -> +0x9c = 0, Zustand `zustand` (2 HURT / 3 DEATH),
 *   +0x5 = 9, +0x6 = 3, +0x7 = 0 (z.B. @0x80112ca0-ce0); sonst +0x93 &= 0xfd und Blut (@0x80112ce4-d34).
 * Rueckgabe 1 = Absturz eingeleitet (das B dieses Ticks laeuft trotzdem, s. aktiv). */
static int luft_treffer(re15_actor_t *e, uint8_t zustand)
{
    uint8_t hr = e->hit_react;
    if (!(hr & 2)) return 0;
    if (hr & 0x40) {
        e->ai_timer = 0;
        e->state = zustand; e->sub_state_1 = 9; e->sub_state_2 = 3; e->sub_state_3 = 0;
        return 1;
    }
    e->hit_react = (uint8_t)(hr & 0xfd);
    blut(e);
    return 0;
}

/* FUN_8001a9cc(&Spieler, tol) @0x8001a9cc-a5c: 0 im Bogen, sonst +tol (Rest <= 0x800) / -tol — re15_ai_arc_test. */
static int16_t bogen(const re15_actor_t *e, const re15_actor_t *pl, int tol)
{
    return (int16_t)re15_ai_arc_test(e, pl->x, pl->z, tol);
}

/* ---- A: Entscheider (Tabelle @0x8011ecc4) --------------------------------------------------------- */

/* A[0] 0x80111220 — Ruhe. Alles hinter +0x1dc == 0 (@0x8011122c-34). */
static void a_ruhe(re15_actor_t *e, uint16_t d, int sicht)
{
    if (e->dog_blocked_ctr != 0) return;
    if (d < 0x1388 && sicht)                   { wechsel(e, 3, 0); return; }   /* @0x80111244-68, @0x801112a4-c0 */
    if (d < 0x1770 && re15_player_aim_active()) { wechsel(e, 4, 0); return; }  /* aca58 == 0x701 @0x80111280-a0 */
    if (d < 0xbb8)                              wechsel(e, 3, 1);               /* @0x801112d8-304 */
}

/* A[1] 0x80111404 — schlafend (grid & 1). KEIN +0x1dc-Tor. */
static void a_schlaf(re15_actor_t *e, uint16_t d, int sicht)
{
    if (d < 0x1388 && sicht)                    { wechsel(e, 2, 0); return; }  /* @0x80111418-458 */
    if (d < 0x1770 && re15_player_aim_active()) { wechsel(e, 2, 1); return; }  /* @0x80111470-4b0, +0x7 = 1 @0x801114f4 */
    if (d < 0xbb8)                               wechsel(e, 2, 2);               /* @0x801114c8-4f4 */
}

/* A[3] 0x801117d8 — Jagd. */
static void a_jagd(re15_actor_t *e, const re15_actor_t *pl, uint16_t d, int sicht)
{
    if (pl->hit_react == 0 && a804(e, pl, 0xbb8, 0x180) && e->dog_blocked_ctr == 0) {   /* @0x801117e8-820 */
        wechsel(e, 5, 0); return;                                                  /* @0x801118dc-8fc */
    }
    if (pl->hit_react != 0) return;                                                /* @0x80111828-834 */
    if (d < 0x1771 && sicht && e->sub_state_3 != 0) {                              /* @0x80111848-878 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;                                    /* @0x80111880-890 */
    }
    if (pl->hit_react != 0) return;                                                /* @0x80111894-8a0 */
    if (d >= 0x1771 && sicht) wechsel(e, 4, 0);                                    /* @0x801118b4-8fc */
}

/* A[4] 0x80111ac0 — Auswahl: schwerer Angriff (Sub 6) oder Sprung (Sub 7, B[7] = b_sprung unten). */
static void a_auswahl(re15_actor_t *e, const re15_actor_t *pl, uint16_t d, int sicht)
{
    if (pl->hit_react == 0 && a804(e, pl, 0xfa0, 0xc0) && e->dog_blocked_ctr == 0) {    /* @0x80111ad4-b0c */
        wechsel(e, 6, 0); return;                                                  /* `sb v0,5` @0x80111b18, @0x80111bd4-bf0 */
    }
    if (!sicht) return;                                                            /* +0x1d0 & 1 @0x80111b28-34 */
    if (e->roach_beh2 != 0) return;                                                /* +0x1e2 @0x80111b3c-44 */
    if (e->sca_wall_hit != 0) return;                                              /* +0x1d6 @0x80111b4c-54 */
    if (d < 0x1771) return;                                                        /* @0x80111b5c-68 */
    if (re15_ai_arc_test(e, pl->x, pl->z, 0x20) != 0) return;                      /* FUN_8001a9cc(&Spieler, 0x20) @0x80111b7c-84 */
    if (!re15_player_aim_active() && (re15_engine_rand8() & 1) == 0) return;       /* aca58 == 0x701 sonst rng & 1 @0x80111b88-ac */
    if (a780(e, pl) != 0) return;                                                  /* @0x80111bb4-bc */
    wechsel(e, 7, 0);                                                              /* @0x80111bc0-bf0 */
}

/* ---- B: Handlungen (Tabelle @0x8011ed04) ---------------------------------------------------------- */

/* B[0] 0x80111310 — Ruhe-Clip 0x16 mit Zeitgeber +0x9c. */
static void b_ruhe(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        e->ai_timer = (int16_t)(re15_engine_rand8() + 59);           /* `addiu v0,v0,59` / `sh v0,156` @0x80111330-44 */
        e->sub_state_2 = 1; clip_setzen(e, 0x16);                     /* @0x80111348-384 */
    }
    uint16_t alt = (uint16_t)e->ai_timer;                             /* `lhu a0,156` @0x80111394 */
    e->ai_timer = (int16_t)(alt - 1);                                 /* @0x8011139c-a4 */
    if (alt == 0) wechsel(e, 3, 1);                                   /* @0x801113a8-d4 */
    (void)anim_set(e);                                                /* @0x801113ec */
}

/* B[1] 0x80111500 — Schlaf-Clip 0, Zeitgeber weckt mit +0x7 = 2. */
static void b_schlaf(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        e->ai_timer = (int16_t)(re15_engine_rand8() + 59);           /* @0x80111520-534 */
        e->sub_state_2 = 1; clip_setzen(e, 0);                        /* @0x80111538-574 */
    }
    uint16_t alt = (uint16_t)e->ai_timer;                             /* @0x80111584 */
    e->ai_timer = (int16_t)(alt - 1);
    if (alt == 0) wechsel(e, 2, 2);                                   /* @0x80111598-5c4 */
    (void)anim_set(e);                                                /* @0x801115dc */
}

/* B[2] 0x801115fc — Aufwachen: Clip nach +0x7 {3,2,1}, SE 3, Fuss-Sperre, Ausgang nach +0x7. */
static void b_wach(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                                        /* @0x80111648-6c8 */
        e->sub_state_2 = 1; e->anim_frame = 0; e->anim_frac = 7;
        if (e->sub_state_3 == 0)      e->motion = 3;
        else if (e->sub_state_3 == 1) e->motion = 2;
        else if (e->sub_state_3 == 2) e->motion = 1;
    }
    if (e->sub_state_2 == 1) {                                        /* 0x801116cc (Phase 0 faellt hinein) */
        if (e->sub_state_3 == 0 && e->anim_frame == 0x1d) re15_audio_room_se(3);   /* +0x95 VOR anim_set @0x801116d8-fc */
        if (anim_set(e)) e->sub_state_2 = 2;                          /* +0x6 += ret @0x80111714-738 */
        fuss_b68(e, 0);                                               /* `jal 0x80115b68` (0,0) @0x80111734 */
        return;
    }
    if (e->sub_state_2 == 2) {                                        /* @0x80111744-7c4 */
        if (e->sub_state_3 == 0)      wechsel(e, 3, 0);
        else if (e->sub_state_3 == 1) wechsel(e, 4, 0);
        else if (e->sub_state_3 == 2) wechsel(e, 3, 1);
    }
}

/* B[3] 0x80111910 — Annaeherung: Clip 5 (sehend) / 4 (blind), Lenken, Fuss-Sperre. */
static void b_jagd(re15_actor_t *e, const re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                                        /* @0x80111930-998 */
        e->sub_state_2 = 1; clip_setzen(e, 4);                        /* Clip 4 @0x8011193c-940 */
        if (e->sub_state_3 == 0) {
            e->motion = 5;                                            /* Delay-Slot `sb v0,148` @0x80111964 */
            (void)re15_engine_rand8();                                /* +0x9e = (rng&0x1f)+2 @0x80111960-978 (sofort ueberschrieben) */
        }
    }
    int lenk = (int)(re15_engine_rand8() & 0x1f) + 6;                 /* +0x9e @0x8011199c-9b4 */
    re15_enemy_steer_point(e, pl->x, pl->z, lenk);                    /* FUN_8001a8f8(&Spieler, (s8)+0x9e) @0x801119c4-d0 */
    (void)anim_set(e);                                                /* @0x801119ec */
    int fenster = (e->sub_state_3 == 0) ? 0x18 : 0x29;                /* `sltiu v0,v0,0x18` @0x80111a18 / 0x29 @0x80111a64 */
    fuss_b68(e, ((int)e->anim_frame < fenster) ? 1 : 0);              /* `jal 0x80115b68` @0x80111a30 / @0x80111a7c */
    if (e->anim_frame == 1 || (int)e->anim_frame == fenster) re15_audio_room_se(8);   /* @0x80111a44-aac */
}

/* B[4] 0x80111c08 — schneller Anlauf: Clip 6, +0x8c 180..211, Gehschritt. */
static void b_anlauf(re15_actor_t *e, const re15_actor_t *pl, uint16_t d)
{
    if (e->sub_state_2 == 0) { e->sub_state_2 = 1; clip_setzen(e, 6); }   /* @0x80111c28-58 */
    e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 180);     /* +0x8c @0x80111c5c-78 */
    int lenk = (int)(re15_engine_rand8() & 0x1f) + 64;                 /* +0x9e @0x80111c74-8c */
    if (d < 0x7d0) lenk += 24;                                          /* @0x80111c9c-bc */
    re15_enemy_steer_point(e, pl->x, pl->z, lenk);                      /* @0x80111ccc-cd8 */
    (void)anim_set(e);                                                  /* @0x80111cf4 */
    if (((int)e->anim_frame % 13) == 1) re15_audio_room_se(8);          /* @0x80111d04-44 */
    /* walker FUN_800245d8(0) @0x80111d48: +0x8c entlang der Gier (wie re15_dog_advance) */
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->crow_speed) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->crow_speed) >> 12);
}

/* B[5] 0x80111d68 — BISS. */
static void b_biss(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 2) {                                        /* @0x80111fd8-2018 */
        if (e->dog_blocked_ctr == 0) e->dog_blocked_ctr = RE15_AFFEN3_SPERRE_ENDE;
        wechsel(e, 3, 0);
        return;
    }
    if (e->sub_state_2 == 0) { e->sub_state_2 = 1; clip_setzen(e, RE15_AFFEN3_BISS_CLIP); }   /* @0x80111dcc-dfc */
    if (e->sub_state_2 != 1) return;
    if (anim_set(e)) e->sub_state_2 = 2;                              /* @0x80111e14-38 */
    fuss_b68(e, 0);                                                   /* @0x80111e34 */
    if (e->anim_frame == 0x0b) re15_audio_room_se(RE15_AFFEN3_BISS_SE_AUSHOL);   /* @0x80111e8c-a0 */
    if (pl->hit_react != 0) return;                                   /* @0x80111ea4-b4 */
    int treffer_box = re15_affen3_quadrat(e, pl, RE15_AFFEN3_BISS_RECORD, RE15_AFFEN3_BISS_R);   /* @0x80111eb8-ec */
    int im_fenster = 0;
    for (int i = 3; i >= 0; i--) if (e->anim_frame == s_fenster_biss[i]) im_fenster = 1;        /* @0x80111ef4-f24 */
    if (im_fenster && treffer_box)
        treffer(e, pl, RE15_AFFEN3_BISS_SCHADEN, RE15_AFFEN3_BISS_SE, 0, re15_enemy_player_dist(e, pl));
}

/* B[6] 0x80112044 — SCHWERER ANGRIFF. */
static void b_schwer(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 2) {                                        /* @0x8011232c-3cc */
        if (pl->hit_react == 0 && a804(e, pl, 0xfa0, 0x180)) {        /* @0x80112334-350 */
            wechsel(e, (e->dog_blocked_ctr == 0) ? 5 : 3, 0);              /* @0x80112364-378 */
        } else {
            if (e->dog_blocked_ctr == 0) e->dog_blocked_ctr = RE15_AFFEN3_SPERRE_ENDE;   /* @0x80112388-39c */
            wechsel(e, 3, 0);
        }
        return;
    }
    if (e->sub_state_2 == 0) { e->sub_state_2 = 1; clip_setzen(e, RE15_AFFEN3_SCHWER_CLIP); }   /* @0x801120ac-dc */
    if (e->sub_state_2 != 1) return;
    if (anim_set(e)) e->sub_state_2 = 2;                              /* @0x801120f4-114 */
    if ((int)e->anim_frame < 6)         fuss_c3c(e, 0);               /* @0x8011212c-138 */
    else if ((int)e->anim_frame < 0x15) fuss_b68(e, 1);               /* `sltiu v0,v1,0x15` / a1 = 1 @0x80112148-15c */
    else                                fuss_b68(e, 0);               /* @0x80112154-15c */
    if (e->anim_frame == 0x14) re15_audio_room_se(RE15_AFFEN3_SCHWER_SE_AUSHOL);   /* @0x80112170-84 */
    if (pl->hit_react != 0) return;                                   /* @0x80112188-98 */
    int h1 = re15_affen3_quadrat(e, pl, RE15_AFFEN3_SCHWER_REC_A, RE15_AFFEN3_SCHWER_R);   /* -> +0x1d8 @0x801121e8 */
    int h2 = re15_affen3_quadrat(e, pl, RE15_AFFEN3_SCHWER_REC_B, RE15_AFFEN3_SCHWER_R);   /* -> +0x1da @0x80112208 */
    int im_fenster = 0;
    for (int i = 5; i >= 0; i--) if (e->anim_frame == s_fenster_schwer[i]) im_fenster = 1;   /* @0x80112238-268 */
    if (im_fenster && (h1 || h2))                                      /* @0x8011226c-29c */
        treffer(e, pl, RE15_AFFEN3_SCHWER_SCHADEN, RE15_AFFEN3_SCHWER_SE, 1, re15_enemy_player_dist(e, pl));
}

/* ---- Subs 7..15 --------------------------------------------------------------------------------- */

/* B[7] 0x801123fc — SPRUNG (Clip 0x14). A[7] 0x801123f4 = `jr ra`. */
static void b_sprung(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                           /* @0x80112450-a8 */
        e->sub_state_2 = 1; clip_setzen(e, 0x14);                     /* +0x94 = 0x14 @0x8011245c-60, +0x8f = 7 @0x80112484 */
        e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 180);   /* `addiu v0,v0,180` / `sh v0,140` @0x80112494-98 */
        e->ai_timer = 0;                                              /* +0x9c @0x801124a8 */
        /* faellt in Phase 1 (@0x801124ac) */
    case 1:
        (void)anim_set(e);                                            /* @0x801124c0 */
        if (e->anim_frame == 0x0a) {                                  /* @0x801124d4-dc */
            e->sub_state_2 = 2;                                       /* @0x801124e8 */
            e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 200);   /* @0x801124e4-fc */
            e->roach_air = 1;                                         /* +0x1e0 @0x8011250c */
            e->hit_react |= 1;                                        /* @0x80112510-2c */
        }
        vorschub(e);                                                  /* jal 0x800245d8 (a0 = 0) @0x80112764 */
        return;
    case 2:                                                           /* @0x80112530 */
        if (luft_treffer(e, 2)) return;                               /* @0x80112530-5d4 (HURT, +0x5 9, +0x6 3) */
        (void)anim_set(e);                                            /* @0x801125ec */
        if (ballistisch(e, e->crow_speed, 600, -60)) {               /* +0x1d8 = 0x258 @0x801125fc, c1a4 @0x8011261c */
            e->sub_state_2 = 3;                                       /* @0x80112638 */
            e->crow_speed = 0x64;                                     /* @0x80112644-4c */
            re15_audio_room_se(1);                                    /* a0 = 1 @0x80112628 -> jal 0x800453d0 @0x80112648 */
            e->roach_air = 0;                                         /* @0x8011265c */
            e->hit_react = 0;                                         /* @0x8011266c */
            return;
        }
        if (e->anim_frame == 0x13 && pl->hp < RE15_AFFEN3_TOD_HP_GRENZE && pl->hit_react == 0   /* @0x8011267c-6b4 */
            && a804(e, pl, 0x1388, 0xc0) && e->dog_blocked_ctr == 0) {    /* @0x801126b8-6e4 */
            wechsel(e, 8, 0);                                         /* @0x801126ec-710 */
            re15_audio_room_se(6);                                    /* a0 = 6 @0x80112708 -> @0x8011270c */
        }
        return;                                                       /* Phase 2 ohne Vorschub (j 0x8011276c) */
    case 3:                                                           /* @0x8011271c */
        if (anim_set(e)) { wechsel(e, 4, 0); return; }                /* @0x80112724-760 */
        vorschub(e);                                                  /* @0x80112764 */
        return;
    default:
        return;
    }
}

/* B[8] 0x80112784 — TODESSPRUNG (Clip 0x15), nur aus B[7] bei Spieler-HP < 50. A[8] 0x8011277c = `jr ra`. */
static void b_todessprung(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                           /* @0x801127ec-848 */
        e->sub_state_2 = 1; clip_setzen(e, RE15_AFFEN3_TOD_CLIP);
        e->roach_air = 1;                                             /* @0x8011282c */
        e->hit_react |= 1;                                            /* @0x8011283c-48 */
        /* faellt in Phase 1 (@0x8011284c) */
    case 1: {
        if (luft_treffer(e, 3)) return;                               /* DEATH, +0x5 9 @0x80112874-8b4 */
        (void)anim_set(e);                                            /* @0x8011291c */
        if (ballistisch(e, e->crow_speed, 600, -60)) {               /* @0x8011292c-4c */
            e->sub_state_2 = 2;                                       /* @0x80112968 */
            e->crow_speed = 0x64;                                     /* @0x80112974-7c */
            re15_audio_room_se(1);                                    /* a0 = 1 @0x80112958 */
            e->roach_air = 0;                                         /* @0x8011298c */
            e->floor = 0;                                             /* +0x82 = 0 `sb zero,130` @0x8011299c */
            e->hit_react = 0;                                         /* @0x801129ac */
            return;
        }
        if (e->anim_frame == 7) re15_audio_room_se(9);               /* @0x801129bc-d0 */
        if (pl->hit_react != 0) return;                              /* @0x801129dc-e4 */
        int h1 = re15_affen3_quadrat(e, pl, 6, 0x320);                /* Pool+1096 @0x80112a34 -> +0x1d8 */
        int h2 = re15_affen3_quadrat(e, pl, 10, 0x320);               /* Pool+1784 @0x80112a54 -> +0x1da */
        int im_fenster = (e->anim_frame == 8 || e->anim_frame == 9 || e->anim_frame == 10);   /* @0x8011ed54 Schleife @0x80112a5c-ab4 */
        if (!im_fenster || !(h1 || h2)) return;                       /* @0x80112ab8-aec */
        pl->hp = (int16_t)(pl->hp - RE15_AFFEN3_TOD_SCHADEN);         /* @0x80112af0-b04 */
        e->dog_blocked_ctr = RE15_AFFEN3_TOD_SPERRE;                       /* @0x80112b08-10 */
        re15_audio_room_se(0);                                        /* a0 = 0 (`addu a0,zero,zero` @0x80112adc) -> @0x80112b0c */
        pl->hit_react |= 1;                                           /* @0x80112b28-58 */
        /* aca58 = 6 (Wort, @0x80112b1c-24), acbfc/acbcc/acbd0 = Affe/+0x178/+0x17c (@0x80112b34-60): der Spieler-Haken
         * cmd 6 0x80116064 ist befehlsgleich zum Gorilla-Finisher 0x8011c3d4 (Kopf) -> derselbe Port-Weg. */
        re15_player_victim_devour(e);
        re15_wound_add(0, 0x32);                                      /* jal 0x80037edc (0, 0x32) @0x80112b64 */
        if (s_log_n < RE15_AFFEN3_LOG_N) {
            re15_affen3_treffer_t *t = &s_log[s_log_n++];
            t->slot = (uint8_t)(e - g_actors); t->sub = e->sub_state_1; t->clip = e->motion; t->bild = e->anim_frame;
            t->schaden = RE15_AFFEN3_TOD_SCHADEN; t->hp_nach = pl->hp; t->abstand = (int16_t)re15_enemy_player_dist(e, pl);
        }
        return; }
    case 2:                                                           /* @0x80112b74 */
        if (anim_set(e)) {                                            /* @0x80112b7c-84 */
            wechsel(e, 4, 0);                                         /* @0x80112b8c-bb8 */
            if (pl->hp < 0) {                                         /* `bgez` @0x80112bc8 */
                wechsel(e, 2, 0); e->sub_state_2 = 1;                 /* +0x5 2, +0x6 1, +0x7 0 @0x80112bd0-bfc */
                e->anim_frame = 0x1d; e->anim_frac = 7; e->motion = 3; /* @0x80112c00-30 */
            }
            return;
        }
        vorschub(e);                                                  /* @0x80112c34 */
        return;
    default:
        return;
    }
}

/* B[9] 0x80112d48 — AN DIE DECKE (Phasen @0x801002d4). */
static void b_decke_hoch(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                           /* @0x80112d88 */
        e->sub_state_2 = 1; clip_setzen(e, 0x0c);                     /* @0x80112d90-dc4 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x80112dd4 / @0x80112de4 */
        e->hit_react |= 1; e->hit_react |= 2;                         /* @0x80112df4-e1c */
        /* faellt in Phase 1 (@0x80112e20) */
    case 1:
        (void)anim_set(e);                                            /* @0x80112e34 */
        if (e->anim_frame == 0x10) {                                  /* @0x80112e48-50 */
            e->sub_state_2 = 2; e->crow_speed = 0;                    /* @0x80112e58-68 */
            e->roach_air = 1;                                         /* @0x80112e78 */
            kasten_luft(e);                                           /* +0x78 = @0x8011ec48[+0x1e4] @0x80112e88-a8 */
            e->hit_react &= 0xfd;                                     /* @0x80112eb8-c8 */
        }
        return;
    case 2:                                                           /* @0x80112ecc */
        (void)anim_set(e);                                            /* @0x80112ee0 */
        (void)ballistisch(e, e->crow_speed, 0x190, -20);             /* @0x80112ef0-efc */
        if (e->y < (int32_t)e->dog_floor_y - RE15_AFFEN3_DECKE_HOEHE) {   /* @0x80112f10-20 */
            e->y = (int32_t)e->dog_floor_y - RE15_AFFEN3_DECKE_HOEHE;    /* @0x80112f28 */
            e->sub_state_2 = 3; e->crow_speed = 0;                    /* @0x80112f38-48 */
        }
        return;
    case 3:                                                           /* @0x80112f4c */
        e->sub_state_2 = 4; e->motion = 0x0c; e->anim_frame = 0x1c; e->anim_frac = 7;   /* @0x80112f58-88 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x80112f98 / @0x80112fa8 */
        /* faellt in Phase 4 (@0x80112fac) */
    case 4:
        if (anim_set(e)) wechsel(e, 12, 0);                           /* @0x80112fc0-ffc */
        return;
    default:
        return;
    }
}

/* B[10] 0x801130f8 — VON DER DECKE (Phasen @0x801002ec); A[10] 0x80113010 (nur +0x6 < 3). */
static void b_decke_runter(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                           /* @0x80113138 */
        e->sub_state_2 = 1; clip_setzen(e, 0x0d);                     /* @0x80113140-174 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x80113184 / @0x80113194 */
        /* faellt in Phase 1 (@0x80113198) */
    case 1:
        (void)anim_set(e);                                            /* @0x801131ac */
        if (e->anim_frame == 0x14) { e->sub_state_2 = 2; e->crow_speed = 0; }   /* @0x801131c0-1d4, @0x80113258 */
        return;
    case 2:                                                           /* @0x801131d8 */
        (void)anim_set(e);                                            /* @0x801131ec */
        if (e->anim_frame == 0x18) e->anim_frame = 0x17;              /* Bild 0x17 halten @0x80113200-214 */
        if (ballistisch(e, e->crow_speed, 0, -70)) {                 /* a1 = 0 @0x8011320c, c1a4 @0x8011322c */
            e->sub_state_2 = 3; e->crow_speed = 0;                    /* @0x80113248-258 */
        }
        return;
    case 3:                                                           /* @0x8011325c */
        e->sub_state_2 = 4; e->motion = 0x0d; e->anim_frame = 0x18; e->anim_frac = 7;   /* @0x80113264-298 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x801132a8 / @0x801132bc */
        re15_audio_room_se(1);                                        /* a0 = 1 @0x801132b4 -> @0x801132b8 */
        e->roach_air = 0;                                             /* @0x801132cc */
        kasten_boden(e);                                              /* +0x78 = @0x8011ec44[+0x1e4] @0x801132dc-fc */
        e->hit_react = 0;                                             /* @0x8011330c */
        /* faellt in Phase 4 (@0x80113310) */
    case 4:
        if (anim_set(e)) wechsel(e, 4, 0);                            /* @0x80113324-360 */
        return;
    default:
        return;
    }
}

/* B[11] 0x80113448 — DREHEN AN DER DECKE (Clip 0x1b); +0x1da merkt den letzten Bogen-Rest ueber die Bilder. */
static void b_decke_drehen(re15_actor_t *e, const re15_actor_t *pl, uint16_t d, int sicht)
{
    if (e->sub_state_2 == 0) {                                        /* @0x80113460-4b8 */
        e->sub_state_2 = 1; clip_setzen(e, 0x1b);
        e->ai_timer = 0x78;                                           /* +0x9c @0x801134a4-a8 */
        e->ai_attack_timer = 1;                                       /* +0x1da = 1 @0x801134b4-b8 (Port-Feld des +0x1da-Platzes) */
    }
    uint16_t b = (uint16_t)e->anim_frame;                            /* +0x1d8 = +0x95 @0x801134c8-d0 */
    if ((uint16_t)(b - 1) < 7 || (uint16_t)(b - 17) < 4) {           /* @0x801134e8-4fc: Bilder 1..7, 17..20 */
        int tol = (int)(int8_t)((re15_engine_rand8() & 0x1f) + 80);  /* +0x9e (`lb`) @0x80113504-52c */
        e->ai_attack_timer = bogen(e, pl, tol);                       /* FUN_8001a9cc -> +0x1da @0x80113538-54c */
        e->rot_y = (int16_t)((uint16_t)e->rot_y + (uint16_t)e->ai_attack_timer);   /* +0x6a += +0x1da (`lhu`/`sh`) @0x8011355c-6c */
    }
    (void)anim_set(e);                                                /* @0x80113584 */
    if (e->ai_attack_timer != 0) return;                              /* `lh v0,474` @0x80113598-5a0 */
    wechsel(e, 12, 0);                                                /* @0x801135a8-5c8 */
    if (!sicht || d >= 0x1389) e->sub_state_1 = 10;                   /* +0x1d0&1 @0x801135d8-e4, +0x1d4 @0x801135ec-5f8 */
}

/* B[12] 0x801136e8 — KRIECHEN AN DER DECKE (Phasen @0x80100304). */
static void b_decke_kriechen(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                           /* @0x80113728 */
        e->sub_state_2 = 1; clip_setzen(e, 0x0f);                     /* @0x80113730-764 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x80113774 / @0x80113784 */
        /* faellt in Phase 1 (@0x80113788) */
    case 1:
        (void)anim_set(e);                                            /* @0x8011379c */
        fuss_c3c(e, 0);                                               /* `jal 0x80115c3c` (0,0) @0x801137a8 */
        if (e->anim_frame == 0x32) wechsel(e, 13, 0);                 /* @0x801137bc-7d0, @0x8011391c-938 */
        return;
    case 2:                                                           /* @0x801137d4 */
        e->sub_state_2 = 3; e->motion = 0x0f; e->anim_frame = 0x32; e->anim_frac = 7;   /* @0x801137dc-810 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x80113820 / @0x80113830 */
        /* faellt in Phase 3 (@0x80113834) */
    case 3: {
        int fertig = anim_set(e);                                     /* @0x80113848 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + fertig);          /* `addu v1,v1,v0` / `sb v1,6` @0x801138f8-900 */
        fuss_c3c(e, 1);                                               /* a1 = 1 @0x80113864, `jal 0x80115c3c` @0x801138fc */
        return; }
    case 5:                                                           /* @0x80113868 */
        e->sub_state_2 = 6; clip_setzen(e, 0x19);                     /* @0x80113870-8a4 */
        e->crow_speed = 0; e->ai_timer = 0;                           /* @0x801138b4 / @0x801138c4 */
        /* faellt in Phase 6 (@0x801138c8) */
    case 6: {
        int fertig = anim_set(e);                                     /* @0x801138dc */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + fertig);          /* @0x801138f8-900 */
        fuss_c3c(e, 0);                                               /* a1 = 0 @0x801138f4 */
        return; }
    case 4:                                                           /* @0x8011390c */
    case 7:                                                           /* @0x8011390c (Tabelle [7]) */
        wechsel(e, 11, 0);                                            /* @0x80113910-938 */
        return;
    default:
        return;
    }
}

/* A[13] 0x8011394c — Entscheider an der Decke. Rueckgabe 1 = Absturz (HURT) eingeleitet. */
static int a_decke(re15_actor_t *e, const re15_actor_t *pl, uint16_t d, int sicht)
{
    if (luft_treffer(e, 2)) return 1;                                 /* @0x80113978-a18 */
    if (pl->hit_react == 0 && d < 0x7d0 && a780(e, pl) != 0 && e->dog_blocked_ctr == 0) {   /* @0x80113a1c-a7c */
        wechsel(e, 15, 0);                                            /* +0x5 15, +0x6 0 @0x80113a80-94, +0x7 0 @0x80113c0c-18 */
        return 0;
    }
    uint8_t b = e->anim_frame;
    if ((b == 0x31 || b == 0x00) && pl->hit_react == 0                /* Tabelle @0x8011ed59 [1..0] = {0x00, 0x31} @0x80113a98-ae0 */
        && a804(e, pl, 0x1388, 0x180) && e->dog_blocked_ctr == 0) {      /* @0x80113af0-b38 */
        wechsel(e, 14, 0);                                            /* `j 0x80113a84` mit v0 = 0xe @0x80113b3c-40 */
        return 0;
    }
    if ((b == 0x00 || b == 0x31 || b == 0x18) && d >= 0xfa1 && !sicht) {   /* @0x8011ed58 [2..0] = {0,0x31,0x18} @0x80113b44-bc8 */
        e->sub_state_1 = 12; e->sub_state_2 = 2;                      /* @0x80113bcc-be0 */
        if ((uint8_t)(b - 1) < 0x19) e->sub_state_2 = 5;              /* @0x80113bf0-c08 */
        e->sub_state_3 = 0;                                           /* @0x80113c0c-18 */
    }
    return 0;
}

/* B[13] 0x80113c3c — JAGD AN DER DECKE (Clip 0x18). */
static void b_decke_jagd(re15_actor_t *e, const re15_actor_t *pl, uint16_t d)
{
    if (e->sub_state_2 == 0) { e->sub_state_2 = 1; clip_setzen(e, 0x18); }   /* @0x80113c54-8c */
    uint16_t b = (uint16_t)e->anim_frame;                            /* +0x1d8 = +0x95 @0x80113c9c-a4 */
    if ((uint16_t)(b - 5) < 8 || (uint16_t)(b - 33) < 5) {           /* Bilder 5..12, 33..37 @0x80113cbc-cd0 */
        e->crow_speed = (int16_t)((re15_engine_rand8() & 0x3f) + 80);   /* @0x80113cd8-cf4 */
        int lenk = (int)(re15_engine_rand8() & 0x30) + 48;            /* +0x9e @0x80113cf0-d08 */
        if (d < 0x7d0) lenk += 24;                                    /* @0x80113d18-38 */
        re15_enemy_steer_point(e, pl->x, pl->z, (int)(int8_t)lenk);  /* FUN_8001a8f8(&Spieler, (s8)+0x9e) @0x80113d48-54 */
    }
    (void)anim_set(e);                                                /* @0x80113d70 */
    fuss_c3c(e, ((int)e->anim_frame < 0x19) ? 1 : 0);                 /* @0x80113d84-da4 */
}

/* B[14] 0x80113e90 — HIEB VON DER DECKE (Clip 0x1a). A[14] 0x80113dbc = nur der Flug-Treffer. */
static void b_decke_hieb(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 2) { wechsel(e, 13, 0); return; }           /* @0x80114274-294 */
    if (e->sub_state_2 == 0) {                                        /* @0x80113f00-44 */
        e->sub_state_2 = 1; clip_setzen(e, RE15_AFFEN3_HIEB_CLIP);
        e->ai_timer = 0;                                              /* @0x80113f44 */
        re15_audio_room_se(9);                                        /* a0 = 9 @0x80113f3c -> @0x80113f40 */
    } else if (e->sub_state_2 != 1) {
        return;
    }
    e->crow_speed = (int16_t)((re15_engine_rand8() & 0x3f) + 100);   /* @0x80113f48-64 */
    int lenk = (int)(re15_engine_rand8() & 0x10) + 64;                /* +0x9e @0x80113f60-78 */
    uint16_t b = (uint16_t)e->anim_frame;                            /* +0x1d8 @0x80113f88-90 */
    if ((uint16_t)(b - 5) < 13 || (uint16_t)(b - 40) < 13)            /* Bilder 5..17, 40..52 @0x80113fa8-bc */
        re15_enemy_steer_point(e, pl->x, pl->z, (int)(int8_t)lenk);  /* @0x80113fc4-fd0 */
    int fertig = anim_set(e);                                         /* @0x80113fec */
    e->sub_state_2 = (uint8_t)(e->sub_state_2 + fertig);              /* @0x80114000-0c */
    fuss_c3c(e, ((int)e->anim_frame < 0x28) ? 1 : 0);                 /* @0x8011401c-3c */
    if (e->anim_frame == 0x28) re15_audio_room_se(9);                /* @0x80114050-64 */
    if (pl->hit_react != 0) return;                                  /* @0x8011406c-74 */
    static const uint8_t f1[6] = { 8, 9, 10, 11, 12, 13 };           /* @0x8011ed5c */
    static const uint8_t f2[6] = { 43, 44, 45, 46, 47, 48 };         /* @0x8011ed64 */
    int h1 = 0, h2 = 0;
    for (int i = 5; i >= 0; i--)                                      /* @0x801140c0-13c: Record 6 r 0x320 nur im Fensterbild */
        if (e->anim_frame == f1[i] && re15_affen3_quadrat(e, pl, 6, 0x320)) h1 = 1;
    for (int i = 5; i >= 0; i--)                                      /* @0x80114148-1c8: Record 10 r 0x320 */
        if (e->anim_frame == f2[i] && re15_affen3_quadrat(e, pl, 10, 0x320)) h2 = 1;
    if (h1 || h2)                                                     /* @0x801141cc-e0 */
        treffer(e, pl, RE15_AFFEN3_HIEB_SCHADEN, RE15_AFFEN3_HIEB_SE, 0, re15_enemy_player_dist(e, pl));   /* @0x801141e4-26c */
}

/* B[15] 0x801142c4 — GRIFF VON DER DECKE (Phasen @0x80100324). A[15] 0x801142bc = `jr ra`. */
static uint8_t s_griff_slot = 0;       /* Affe, der greift (0 = keiner) */
static uint8_t s_griff_affe_bit = 0;   /* Affe +0x0 & 0x1000 (P0 @0x801143c4-c8 .. P6 @0x80114670-7c) */
static uint8_t s_griff_pl_bit = 0;     /* Spieler +0x0 & 0x1000 (P0 @0x801143e4-ec .. Haken 5 @0x80115f68-78) */
static uint8_t s_opfer_ph = 0xff;      /* aca5a des Spieler-Hakens (0xff = kein Griff) */

static void eigene_platzierung(re15_actor_t *e)   /* FUN_8001ad68(self, +0x84, +0x16c) = Wurzel ab Anker +0xa0/+0xa2 */
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (b && b->ok) re15_clip_root_motion_abs_pub(e, &b->skel, &b->anim, (int)e->motion, (int)e->anim_frame);
}

static void b_griff(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:                                                           /* @0x80114304 */
        e->y = (int32_t)e->dog_floor_y + 400;                         /* `addiu v0,v0,400` / `sw v0,56` @0x80114318-1c */
        e->sub_state_2 = 1; e->motion = 0x1c; e->anim_frame = 0; e->anim_frac = 0;   /* @0x8011432c-35c */
        e->anchor_x = pl->x; e->anchor_z = pl->z;                     /* +0xa0/+0xa2 = Spieler-X/Z @0x8011436c-390 */
        pl->anchor_x = pl->x; pl->anchor_z = pl->z;                   /* acaf4/acaf6 @0x801143b0-b8 */
        s_griff_slot = (uint8_t)(e - g_actors);                       /* acbfc = self @0x80114374 */
        s_griff_affe_bit = 1; s_griff_pl_bit = 1;                     /* |= 0x1000 @0x801143c4-c8 / @0x801143e4-ec */
        pl->hit_react |= 1;                                           /* @0x801143f4 / @0x8011441c */
        e->rot_y = pl->rot_y;                                         /* +0x6a = acabe @0x80114424-30 */
        s_opfer_ph = 0;
        re15_player_victim_affen3(e, 1);                              /* aca58 = 5 (Wort) @0x8011440c, acbcc/acbd0 @0x801143fc/@0x80114414 */
        re15_audio_room_se(6);                                        /* a0 = 6 @0x80114428 -> jal @0x801145a4 */
        /* faellt in Phase 1 (@0x801145ac) */
    case 1:
        eigene_platzierung(e);                                        /* @0x801145c4 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + anim_set(e));     /* @0x801145e0-604 */
        return;
    case 2:                                                           /* @0x80114434 */
        e->motion = 0x1d; e->sub_state_2 = 3; e->anim_frame = 0;      /* @0x8011443c-460 */
        e->ai_timer = 0x64;                                           /* +0x9c @0x8011446c-470 */
        e->roach_fade = 0x32;                                         /* +0x9e @0x8011447c-480 (im Griff ungelesen) */
        /* faellt in Phase 3 (@0x80114484) */
    case 3: {
        if (e->anim_frame == 0) re15_audio_room_se(10);              /* @0x80114490-4a4 */
        eigene_platzierung(e);                                        /* @0x801144c0 */
        (void)anim_set(e);                                            /* @0x801144dc */
        int druck = re15_re2z_mash();                                 /* jal 0x80037024 @0x801144e4 (= re15_mash_pressed, Kanten 0xf0f0) */
        e->ai_timer = (int16_t)(e->ai_timer - 1 - 100 * druck);       /* @0x801144ec-514 */
        if (e->ai_timer < 0) { e->sub_state_2 = 4; s_opfer_ph = 4; }  /* @0x80114524-53c (aca5a = 4) */
        return; }
    case 4:                                                           /* @0x80114548 */
        e->motion = 0x1e; e->sub_state_2 = 5; e->anim_frame = 0; e->anim_frac = 0;   /* @0x80114550-584 */
        /* faellt in Phase 5 (@0x80114588) */
    case 5:
        if (e->anim_frame == 0x1d) re15_audio_room_se(5);            /* @0x80114594-5a4 */
        eigene_platzierung(e);                                        /* @0x801145c4 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + anim_set(e));     /* @0x801145e0-604 */
        return;
    case 6:                                                           /* @0x80114608 */
        e->y = (int32_t)e->dog_floor_y;                               /* @0x80114614-1c */
        e->sub_state_2 = 7; e->anim_frame = 0x1d; e->anim_frac = 0; e->motion = 3;   /* @0x80114628-660 */
        re15_audio_room_se(3);                                        /* a0 = 3 @0x80114648 -> @0x8011465c */
        s_griff_affe_bit = 0;                                         /* +0x0 &= ~0x1000 @0x80114670-7c */
        e->roach_air = 0;                                             /* @0x8011468c */
        kasten_boden(e);                                              /* @0x8011469c-6bc */
        e->hit_react = 0;                                             /* @0x801146cc */
        /* faellt in Phase 7 (@0x801146d0) */
    case 7:
        (void)anim_set(e);                                            /* @0x801146e4 */
        if (e->anim_frame == 0x31) e->sub_state_2 = 8;                /* @0x801146f8-70c */
        return;
    case 8:                                                           /* @0x80114710 */
        e->dog_blocked_ctr = 0x3c;                                         /* @0x80114718-1c */
        wechsel(e, 2, 0); e->sub_state_2 = 1;                         /* +0x5 2, +0x6 1, +0x7 0 @0x80114728-74c */
        e->anim_frame = 0x1d; e->anim_frac = 7; e->motion = 3;        /* @0x80114758-77c */
        if (s_griff_slot == (uint8_t)(e - g_actors)) s_griff_slot = 0;
        return;
    default:
        return;
    }
}

/* ---- Spieler-Haken des Griffs: cmd 5 0x80115d30 -> aca59-Tabelle @0x8011ee44 [0] = 0x80115d70, Phasen aca5a
 *      @0x80100364 = {5dac, 5e04, 5e3c, 5e5c, 5ea0, 5ef8, 5f7c, 5fa4, 5fac, 5fd8, 6014}. cur = Spieler. ---------- */
static uint8_t s_opfer_f314 = 0;   /* im Vortick lief FUN_8001f314 auf dem Spieler (+0x8f-Abbau zu Beginn des Ticks) */

int re15_affen3_opfer_phase(void) { return (s_opfer_ph == 0xff) ? 0xff : (int)s_opfer_ph; }
int re15_affen3_opfer_eigenbank(void) { return s_opfer_ph >= 6 && s_opfer_ph <= 9; }
int re15_affen3_griff_paar(const re15_actor_t *e)
{
    return e && e->type == RE15_AFFEN3_TYP && s_griff_slot != 0 && (uint8_t)(e - g_actors) == s_griff_slot
        && s_griff_affe_bit && s_griff_pl_bit;
}

/* anim_set(acbcc, acbd0, a2, 0x200) bzw. (acad8, acbc0, ...) auf dem Spieler: +0x95 weiter, Rueckgabe 1 am Clip-Ende. */
static int opfer_anim(re15_actor_t *pl, int fc)
{
    if (fc < 1) fc = 1;
    int ende = ((int)pl->anim_frame + 1 >= fc);
    pl->anim_frame = (uint8_t)(((int)pl->anim_frame + 1) % fc);
    pl->anim_blend_rate = 0x200;                                      /* a3 = 0x200 an allen Aufrufen */
    s_opfer_f314 = 1;
    return ende;
}

void re15_affen3_opfer_tick(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    if (s_opfer_ph == 0xff || s_opfer_ph > 10) return;
    if (s_opfer_f314 && pl->anim_frac > 0) pl->anim_frac--;          /* FUN_8001f3bc mischt, dann +0x8f-- (@0x8001f5a8-b4) */
    s_opfer_f314 = 0;
    int vfc = (vb && (int)pl->motion < vb->anim_victim.clip_count) ? vb->anim_victim.clips[pl->motion].frame_count : 1;
    switch (s_opfer_ph) {
    case 0:                                                           /* @0x80115dac */
        s_opfer_ph = 1;                                               /* aca5a = 1 @0x80115dc4 */
        pl->motion = 1; pl->anim_frame = 0; pl->anim_frac = 7;        /* acae8 1 / acae9 0 / acae3 7 @0x80115dcc-de0 */
        pl->anim_flags &= (uint16_t)~0x80u;
        re15_audio_core_se(0);                                        /* FUN_80045024(0x04000001, &Spieler+0x34) @0x80115de4 */
        /* aca3c |= 0xc0 @0x80115df8-e00: im Port ohne Gegenstueck (Anzeige-Bits, wie beim Gorilla-Finisher) */
        vfc = (vb && 1 < vb->anim_victim.clip_count) ? vb->anim_victim.clips[1].frame_count : 1;
        /* faellt in [1] (@0x80115e04) */
    case 1:
        if (vb) re15_clip_root_motion_abs_pub(pl, &vb->skel_victim, &vb->anim_victim, (int)pl->motion, (int)pl->anim_frame);   /* FUN_8001ad68 @0x80115e1c */
        if (opfer_anim(pl, vfc)) s_opfer_ph = (uint8_t)(s_opfer_ph + 1);   /* f314 @0x80115fec, aca5a += @0x80115ff4-6008 */
        return;
    case 2:                                                           /* @0x80115e3c */
        s_opfer_ph = 3; pl->motion = 2; pl->anim_frame = 0;           /* aca5a 3, acae8 2, acae9 0 @0x80115e44-58 */
        vfc = (vb && 2 < vb->anim_victim.clip_count) ? vb->anim_victim.clips[2].frame_count : 1;
        /* faellt in [3] (@0x80115e5c) */
    case 3:
        if (vb) re15_clip_root_motion_abs_pub(pl, &vb->skel_victim, &vb->anim_victim, (int)pl->motion, (int)pl->anim_frame);   /* @0x80115e74 */
        (void)opfer_anim(pl, vfc);                                    /* @0x80115e90 — Ende setzt der Affe (aca5a = 4 @0x8011453c) */
        return;
    case 4: {                                                         /* @0x80115ea0 */
        s_opfer_ph = 5; pl->motion = 3; pl->anim_frame = 0; pl->anim_frac = 0;   /* aca5a 5, acae8 3, acae9 0, acae3 0 @0x80115ea4-ee0 */
        int32_t k[3]; re15_player_victim_bone_pos_pub(8, k);         /* a2 = acbdc + 0x5a0 = Part 8 @0x80115ec8/@0x80115eec */
        re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500, k[0], k[1], k[2], (int16_t)pl->rot_y);   /* @0x80115ee8 */
        return; }
    case 5:                                                           /* @0x80115ef8 */
        if (pl->anim_frame == 0x1d) re15_audio_core_se(0);           /* acae9 == 0x1d -> FUN_80045024(0x04000001) @0x80115f00-1c */
        if (vb) re15_clip_root_motion_abs_pub(pl, &vb->skel_victim, &vb->anim_victim, (int)pl->motion, (int)pl->anim_frame);   /* @0x80115f34 */
        if (opfer_anim(pl, vfc)) {                                    /* @0x80115f50-58 */
            s_opfer_ph = 6;                                           /* @0x80115f60-64 */
            s_griff_pl_bit = 0;                                       /* Spieler +0x0 &= ~0x1000 @0x80115f68-78 */
        }
        return;
    case 6:                                                           /* @0x80115f7c */
        s_opfer_ph = 7; pl->motion = 0x10; pl->anim_frame = 0; pl->anim_frac = 0;   /* aca5a 7, acae8 0x10, acae9 0, acae3 0 @0x80115f80-a0 */
        pl->anim_flags &= (uint16_t)~0x80u;                           /* a2 = 0 @0x80115fa8 */
        /* faellt in [7] (@0x80115fdc) */
    case 7:
        if (opfer_anim(pl, re15_player_pl00_clip_frames(0x10))) s_opfer_ph = (uint8_t)(s_opfer_ph + 1);   /* f314(acad8, acbc0, 0) @0x80115fec */
        return;
    case 8:                                                           /* @0x80115fac */
        s_opfer_ph = 9; pl->motion = 0x0b; pl->anim_frame = 0; pl->anim_frac = 7;   /* aca5a 9, acae8 0xb, acae9 0, acae3 7 @0x80115fb0-d4 */
        pl->anim_flags |= 0x80u;                                      /* a2 = 1 (rueckwaerts) @0x80115fd8, wie Gorilla P5 (re15_affen.h (6c)) */
        /* faellt in [9] (@0x80115fdc) */
    case 9:
        if (opfer_anim(pl, re15_player_pl00_clip_frames(0x0b))) s_opfer_ph = (uint8_t)(s_opfer_ph + 1);
        return;
    case 10:                                                          /* @0x80116014 */
        s_opfer_ph = 0xff;                                            /* aca58 = 1 (Wort) @0x80116018-1c */
        pl->hit_react = 0;                                            /* @0x80116030 */
        if (pl->anim_flags & 0x80u) { pl->anim_frame = 0; pl->anim_flags &= (uint16_t)~0x80u; }   /* wie Gorilla P7 */
        /* aca3c &= ~0x80 & ~0x40 @0x80116020-44: Anzeige-Bits, kein Port-Gegenstueck */
        re15_player_victim_affen3(NULL, 0);
        return;
    default:
        return;
    }
}

/* ---- Haken -------------------------------------------------------------------------------------- */

void re15_affen3_init(re15_actor_t *e)
{
    if (!e) return;
    /* Nachbesserung 1: Ereignis-Affen. Nach den Grid-Bit-Bloecken (+0x1e2/+0x1e3/+0x1e4) prueft der INIT die EXAKTEN Werte
     * +0x9 == 0x40 / 0x41 und schreibt das Zustandswort neu (`sw v0,4(a0)` = +0x4..7):
     *   @0x80110e80 `lbu v1,9(a0)` / `ori v0,zero,0x40` / `bne v1,v0,0x80110eac` / `ori v0,zero,0x4` / `sw v0,4(a0)` @0x80110e94
     *   @0x80110ea4 `lbu v1,9(a0)` / `ori v0,zero,0x41` / `bne v1,v0,0x80110ebc` / `ori v0,zero,0x104` / `sw v0,4(a0)` @0x80110eb8
     * (STAGE4.BIN @0x8010c520-558 befehlsgleich). Sce_em_set ruft die Wurzel beim Spawn mit geloeschtem Sperrbit
     * (`andi v0,v1,0xdf` @0x8004256c, `jalr` @0x8004259c): ein gesperrter Affe grid 0x61 sieht hier 0x41 und wartet danach
     * gesperrt in Zustand 4/1 (Original o11 ROOM4040: F18..F56 `4.1.0.0 g61`). Alles danach (anim_set, HP, Kasten) wie sonst. */
    if (e->grid_id == 0x40) { e->state = 4; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }   /* Wort 0x4 */
    if (e->grid_id == 0x41) { e->state = 4; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; }   /* Wort 0x104 */
    e->render_scale_q12 = RE15_AFFEN3_MASSSTAB;   /* +0x0 |= 0x800 @0x80110f44-48, +0x166 = 0x14cc @0x80110f54-58 */
    e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;   /* +0x94 = 0 @0x80110ce4, +0x95 @0x80110cf4, +0x8f @0x80110d04 */
    (void)anim_set(e);                            /* `jal 0x8001f314` (a3 = 0x200) @0x80110ed0: Pool des INIT-Clips 0 */
    e->dog_floor_y = (int16_t)(-(int32_t)e->floor * 1800);   /* +0x1ba: Sce_em_set-Saat FUN_800420a0 `sh v0,442(s0)`
                                                  * @0x80042210 (-(+0x82)*0x708, wie Hund/Gorilla-INIT im Port) */
    kasten_boden(e);                              /* +0x78 = *(0x8011ec44 + 4*(+0x1e4)) @0x80110f68-88 */
    e->roach_air = 0;                             /* +0x1e0 = 0 @0x80110dc4 */
}

/* Zustand 4/5 0x80115734 (Tabelle @0x8011eca4 [4] = [5]; STAGE4 0x80110dd4): `lbu v0,5(v0)` @0x80115744, Unter-Tabelle
 * @0x8011ee34 (`addiu at,at,-4556` @0x80115754) = {0x8011577c, 0x801158e8, 0x80115a5c `jr ra`, 0x80115a64 `jr ra`},
 * `jalr` @0x80115764. Gemessen (Original o11, ROOM4040): Freigabe F57 -> F58 `1.10.2.0 g01`, Fall bis +0x1ba in 18 Bildern. */
void re15_affen3_zustand4(re15_actor_t *e)
{
    if (!e) return;
    if (e->sub_state_1 == 0) {                    /* 0x8011577c — Ereignis-Affe +0x9 == 0x40 */
        switch (e->sub_state_2) {                 /* +0x6 `lbu v1,6(a0)` @0x8011578c, Verteiler @0x80115790-7c0 */
        case 0:                                   /* @0x801157c8 */
            e->sub_state_2 = 1;                   /* `sb v0,6(a0)` (v0 = 1 aus dem Delay @0x801157a8) @0x801157c8 */
            e->motion = 0x1f;                     /* `ori v0,zero,0x1f` / `sb v0,148` @0x801157d4-d8 */
            e->anim_frame = 0;                    /* `sb zero,149` @0x801157e8 */
            e->anim_frac = 7;                     /* `ori v0,zero,0x7` / `sb v0,143` @0x801157f4-f8 */
            e->crow_speed = 0;                    /* +0x8c `sh zero,140` @0x80115808 */
            e->ai_timer = 0;                      /* +0x9c `sh zero,156` @0x80115818 */
            /* faellt in Phase 1 (@0x8011581c) */
        case 1:
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + anim_set(e));   /* `jal 0x8001f314` @0x80115830, `addu`/`sb v1,6` @0x8011584c-54 */
            return;
        case 2:                                   /* @0x80115858 */
            e->grid_id = (uint8_t)(e->grid_id & 0xbf);   /* `andi v0,v0,0xbf` / `sb v0,9(a0)` @0x80115860-64 */
            e->state = 1;                         /* `sb a0,4(v0)` (a0 = 1) @0x80115874 */
            e->sub_state_1 = 2;                   /* `ori v0,zero,0x2` / `sb v0,5(v1)` @0x80115880-84 */
            e->sub_state_2 = 1;                   /* `sb a0,6(v0)` @0x80115894 */
            e->sub_state_3 = 0;                   /* `sb zero,7(v0)` @0x801158a4 */
            e->anim_frame = 0x1d;                 /* `ori v0,zero,0x1d` / `sb v0,149` @0x801158b0-b4 */
            e->anim_frac = 0;                     /* `sb zero,143` @0x801158c4 */
            e->motion = 3;                        /* `ori v0,zero,0x3` / `sb v0,148` @0x801158d0-d4 -> B[2] Phase 1, Clip 3 ab 0x1d */
            return;
        default:                                  /* `j 0x801158d8` */
            return;
        }
    }
    if (e->sub_state_1 == 1) {                    /* 0x801158e8 — Ereignis-Affe +0x9 == 0x41: EIN Bild, dann ACTIVE B[10] Phase 2 */
        e->hit_react |= 1;                        /* `ori v0,v0,0x1` / `sb v0,147` @0x80115900-04 */
        e->hit_react |= 2;                        /* `ori v0,v0,0x2` / `sb v0,147` @0x8011591c-20 */
        e->roach_air = 1;                         /* +0x1e0 `sb a0,480(v0)` @0x80115930 */
        kasten_luft(e);                           /* +0x78 = *(0x8011ec48 + 4*(+0x1e4)) @0x80115940-60 */
        e->grid_id = (uint8_t)(e->grid_id & 0xbf);   /* `andi v0,v0,0xbf` / `sb v0,9(v1)` @0x80115978-7c */
        e->state = 1;                             /* `sb a0,4(v0)` (a0 = 1) @0x8011598c */
        e->sub_state_1 = 10;                      /* `ori v0,zero,0xa` / `sb v0,5(v1)` @0x80115998-9c */
        e->sub_state_2 = 2;                       /* `ori v0,zero,0x2` / `sb v0,6(v1)` @0x801159a8-ac */
        e->sub_state_3 = 0;                       /* `sb zero,7(v0)` @0x801159bc */
        e->motion = 0x0d;                         /* `ori v0,zero,0xd` / `sb v0,148` @0x801159c8-cc */
        e->anim_frame = 0x13;                     /* `ori v0,zero,0x13` / `sb v0,149` @0x801159d8-dc */
        e->anim_frac = 0;                         /* `sb zero,143` @0x801159ec */
        e->crow_speed = 0;                        /* +0x8c `sh zero,140` @0x801159fc */
        e->ai_timer = 0;                          /* +0x9c `sh zero,156` @0x80115a0c */
        (void)anim_set(e);                        /* `jal 0x8001f314` (a3 = 0x200) @0x80115a24 */
        e->anim_frame = 0x14;                     /* `ori v0,zero,0x14` / `sb v0,149` @0x80115a34-38 */
        e->anim_frac = 0;                         /* `sb zero,143` @0x80115a48 */
        return;
    }
    /* +0x5 2 / 3: 0x80115a5c / 0x80115a64 = `jr ra` */
}

void re15_affen3_luft_landung(re15_actor_t *e)
{
    if (!e || !e->roach_air) return;              /* `lbu v0,480` / `beq` @0x801147a0-a8 (DEATH @0x80114fc4-cc) */
    if (ballistisch(e, 0, 0, -50)) {              /* FUN_8001c1a4(0, 0, -50, +0x1ba) @0x801147b8 / @0x80114fdc */
        e->roach_air = 0;                         /* @0x801147d4 / @0x80114ff8 */
        kasten_boden(e);                          /* @0x801147e4-804 / @0x80115008-28 */
        re15_audio_room_se(1);                    /* a0 = 1 @0x801147c4 -> jal 0x800453d0 @0x80114800 */
    }
}

int re15_affen3_aktiv(int slot, int32_t dist, int sicht)
{
    if (slot <= RE15_ACTOR_SLOT_PLAYER || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint16_t d = (uint16_t)dist;                  /* +0x1d4 wird `lhu` gelesen (@0x8011123c u.a.) */
    sicht = (sicht & 1);

    /* A[+0x5]: `lbu v0,5` @0x801110ec, Tabelle 0x8011ecc4, `jalr` @0x8011110c */
    switch (e->sub_state_1) {
    case 0:  a_ruhe(e, d, sicht); break;
    case 1:  a_schlaf(e, d, sicht); break;
    case 3:  a_jagd(e, pl, d, sicht); break;
    case 4:  a_auswahl(e, pl, d, sicht); break;
    case 9:  if (e->sub_state_2 >= 2) (void)luft_treffer(e, 2); break;   /* A[9] 0x80112c60: `sltiu v0,v0,0x2` @0x80112c78 */
    case 10: if (e->sub_state_2 < 3) (void)luft_treffer(e, 2); break;    /* A[10] 0x80113010: `sltiu v0,v0,0x3` @0x80113028 */
    case 11: case 12: case 14: (void)luft_treffer(e, 2); break;          /* A[11] 0x80113374, A[12] 0x80113614, A[14] 0x80113dbc */
    case 13: (void)a_decke(e, pl, d, sicht); break;                      /* A[13] 0x8011394c */
    default: break;                               /* A[2] 0x801115f4, A[5] 0x80111d60, A[6] 0x8011203c, A[7] 0x801123f4,
                                                   * A[8] 0x8011277c, A[15] 0x801142bc = `jr ra` */
    }
    /* B[+0x5]: +0x5 NEU gelesen `lbu v0,5` @0x80111120, Tabelle 0x8011ed04, `jalr` @0x80111140 — derselbe Tick, auch wenn
     * A den Affen gerade in den HURT gestuerzt hat (+0x4 = 2, +0x5 = 9, +0x6 = 3: dann laeuft B[9] Phase 3 noch einmal) */
    switch (e->sub_state_1) {
    case 0:  b_ruhe(e); break;
    case 1:  b_schlaf(e); break;
    case 2:  b_wach(e); break;
    case 3:  b_jagd(e, pl); break;
    case 4:  b_anlauf(e, pl, d); break;
    case 5:  b_biss(e, pl); break;
    case 6:  b_schwer(e, pl); break;
    case 7:  b_sprung(e, pl); break;
    case 8:  b_todessprung(e, pl); break;
    case 9:  b_decke_hoch(e); break;
    case 10: b_decke_runter(e); break;
    case 11: b_decke_drehen(e, pl, d, sicht); break;
    case 12: b_decke_kriechen(e); break;
    case 13: b_decke_jagd(e, pl, d); break;
    case 14: b_decke_hieb(e, pl); break;
    case 15: b_griff(e, pl); break;
    default: break;
    }
    /* Schwanz @0x80111148-1b0 (Zielbaender, Port-Kodierung Bit 1 DOWN / 2 LEVEL / 4 UP wie Gorilla/Hund): word0 &=
     * 0x1fffffff, |= 0x40000000 (LEVEL) @0x80111148-80; FUN_80012974(0xfa0) = Abstand NACH dem B neu (SquareRoot0
     * @0x800129c0) < 0xfa0 -> |= 0x20000000 (DOWN) @0x800129cc-f0; bei +0x1e0: word0 &= 0x1fffffff, FUN_80012a0c(0x1388)
     * -> < 0x1388 |= 0x80000000 (UP) @0x80111184-1b0 / @0x80012a64-88. */
    {
        uint32_t dx = (uint32_t)(pl->x - e->x), dz = (uint32_t)(pl->z - e->z);
        uint32_t nd = (uint32_t)re15_squareroot0(dx * dx + dz * dz);
        e->aim_band = (uint8_t)(2u | ((nd < 0xfa0u) ? 1u : 0u));
        if (e->roach_air) e->aim_band = (uint8_t)((nd < 0x1388u) ? 4u : 0u);
    }
    return 1;
}
