/*
 * RE1.5 Rebuilt — Runde 38 Spur S "affen3": die kleinen Affen (Typ 0x29, EM029) treffen wie im Original.
 *
 * Kopf, Belege und Grenzen: include/re15_affen3_r38.h; Messungen: analysis/befunde_runde38/S_affen3.md.
 * Alle Adressen STAGE3.BIN (Overlay @0x80100000, kein Kopf), ausser FUN_8001xxxx/FUN_8002xxxx = PSX.EXE.
 *
 * Feld-Abbildung (wie bisher in re15_cockroach_ai_tick): +0x5 sub_state_1, +0x6 sub_state_2, +0x7 sub_state_3,
 * +0x94 motion, +0x95 anim_frame, +0x8f anim_frac, +0x9c ai_timer, +0x8c crow_speed, +0x1dc dog_atk_cd,
 * +0x1e2 roach_beh2, +0x1d6 sca_wall_hit (Rueckgabe der Wandklemme FUN_8003b0a4, `sh v0,470` @0x80110bcc).
 */
#include "re15_affen3_r38.h"
#include "re15_affen.h"         /* Pool/Zeichenstand/Fuss-Sperre/Trefferpunkt der Affen-Familie (affen_11c0.c) */
#include "re15_enemy.h"
#include "re15_enemy_ai.h"      /* re15_player_stagger_cmd2, re15_player_knockdown_begin */
#include "re15_damage.h"        /* re15_engine_rand8, re15_ai_arc_test */
#include "re15_audio.h"         /* re15_audio_room_se = FUN_800453d0 */
#include "re15_math.h"          /* re15_squareroot0 = SquareRoot0 0x80065f60 */
#include <string.h>

void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);   /* enemy_ai_common.c = FUN_8001a8f8-Kern */
int  re15_player_aim_active(void);                                                /* player_common.c: aca58 == 0x701 */

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
    if (!re15_affen3_trefferpunkt(e, record, g)) return 0;
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
    e->dog_atk_cd = RE15_AFFEN3_SPERRE_TREFFER;
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

/* ---- A: Entscheider (Tabelle @0x8011ecc4) --------------------------------------------------------- */

/* A[0] 0x80111220 — Ruhe. Alles hinter +0x1dc == 0 (@0x8011122c-34). */
static void a_ruhe(re15_actor_t *e, uint16_t d, int sicht)
{
    if (e->dog_atk_cd != 0) return;
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
    if (pl->hit_react == 0 && a804(e, pl, 0xbb8, 0x180) && e->dog_atk_cd == 0) {   /* @0x801117e8-820 */
        wechsel(e, 5, 0); return;                                                  /* @0x801118dc-8fc */
    }
    if (pl->hit_react != 0) return;                                                /* @0x80111828-834 */
    if (d < 0x1771 && sicht && e->sub_state_3 != 0) {                              /* @0x80111848-878 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;                                    /* @0x80111880-890 */
    }
    if (pl->hit_react != 0) return;                                                /* @0x80111894-8a0 */
    if (d >= 0x1771 && sicht) wechsel(e, 4, 0);                                    /* @0x801118b4-8fc */
}

/* A[4] 0x80111ac0 — Auswahl: schwerer Angriff oder Sprung (Sprung = Sub 7, dessen B[7] faehrt der bisherige Code). */
static void a_auswahl(re15_actor_t *e, const re15_actor_t *pl, uint16_t d, int sicht)
{
    if (pl->hit_react == 0 && a804(e, pl, 0xfa0, 0xc0) && e->dog_atk_cd == 0) {    /* @0x80111ad4-b0c */
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
        if (e->dog_atk_cd == 0) e->dog_atk_cd = RE15_AFFEN3_SPERRE_ENDE;
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
            wechsel(e, (e->dog_atk_cd == 0) ? 5 : 3, 0);              /* @0x80112364-378 */
        } else {
            if (e->dog_atk_cd == 0) e->dog_atk_cd = RE15_AFFEN3_SPERRE_ENDE;   /* @0x80112388-39c */
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

/* ---- Haken -------------------------------------------------------------------------------------- */

void re15_affen3_init(re15_actor_t *e)
{
    if (!e) return;
    e->render_scale_q12 = RE15_AFFEN3_MASSSTAB;   /* +0x0 |= 0x800 @0x80110f44-48, +0x166 = 0x14cc @0x80110f54-58 */
    e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;   /* +0x94 = 0 @0x80110ce4, +0x95 @0x80110cf4, +0x8f @0x80110d04 */
    re15_affen_pool_anim(e);                      /* anim_set @0x80110ed0: der INIT legt den Pool an (Clip 0, Bild 0) */
}

int re15_affen3_aktiv(int slot, int32_t dist, int sicht)
{
    if (slot <= RE15_ACTOR_SLOT_PLAYER || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (e->sub_state_1 > 6) return 0;
    uint16_t d = (uint16_t)dist;                  /* +0x1d4 wird `lhu` gelesen (@0x8011123c u.a.) */
    sicht = (sicht & 1);

    /* A[+0x5]: `lbu v0,5` @0x801110ec, Tabelle 0x8011ecc4, `jalr` @0x8011110c */
    switch (e->sub_state_1) {
    case 0: a_ruhe(e, d, sicht); break;
    case 1: a_schlaf(e, d, sicht); break;
    case 3: a_jagd(e, pl, d, sicht); break;
    case 4: a_auswahl(e, pl, d, sicht); break;
    default: break;                               /* A[2] 0x801115f4, A[5] 0x80111d60, A[6] 0x8011203c = `jr ra` */
    }
    /* B[+0x5]: +0x5 NEU gelesen `lbu v0,5` @0x80111120, Tabelle 0x8011ed04, `jalr` @0x80111140 — derselbe Tick */
    switch (e->sub_state_1) {
    case 0: b_ruhe(e); break;
    case 1: b_schlaf(e); break;
    case 2: b_wach(e); break;
    case 3: b_jagd(e, pl); break;
    case 4: b_anlauf(e, pl, d); break;
    case 5: b_biss(e, pl); break;
    case 6: b_schwer(e, pl); break;
    default: return 0;                            /* A[4] -> 7: B[7] im bisherigen Code, ebenfalls in diesem Tick */
    }
    return 1;
}
