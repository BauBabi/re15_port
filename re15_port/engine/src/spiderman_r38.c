/* spiderman_r38.c — Runde 38 Spur T "spiderman": Typ 0x2B (RE15Editor "SPIDER-MAN") — Schacht-Austritt
 * und die drei Griffe byte-true (Belege und Messungen: analysis/befunde_runde38/T_spiderman.md).
 * Kopf und Adressuebersicht: include/re15_spiderman_r38.h. Alle Adressen STAGE4.BIN (RAW @0x80100000).
 *
 * Gerufen aus enemy_ai_common.c re15_tyrant_ai_tick (Zustand 4, +0x5 = 5/8/9/10/15) und aus der
 * generischen Opfer-Maschine (re15_victim_clip_map / re15_player_victim_tick). */
#include <stdint.h>
#include <stdlib.h>
#include "re15_spiderman_r38.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"      /* re15_re2z_mash = re15_mash_pressed (FUN_80037024-Zwilling) */
#include "re15_audio.h"          /* re15_audio_room_se = FUN_800453d0 (snd1) */
#include "re15_collision.h"
#include "re15_room.h"
#include "re15_skeleton.h"       /* re15_cos_q12 / re15_sin_q12 */

/* Schmale Zugaenge in enemy_ai_common.c (Runde 38 Spur T, dort neben re15_player_victim_latch). */
void re15_r38_griff_pin(void);
void re15_r38_opfer_latch(const re15_actor_t *e, re15_actor_t *pl, int variante);
void re15_r38_opfer_variante(uint8_t v);
int  re15_r38_birkin_anim(re15_actor_t *e);
int  re15_r38_birkin_fc(const re15_actor_t *e);

static unsigned s_latches = 0;
unsigned re15_r38_sm_latches(void) { return s_latches; }

/* ------------------------------------------------------------------------------------------------
 * (1) AUSTRITT — Zustand 4, Untertabelle @0x8011A270: [0] 0x8011502C (grid 0x40), [1] 0x801152EC (grid 0x41).
 * Beide Maschinen sind gleich gebaut, sie unterscheiden sich nur im Freigabe-Bild (P2):
 *   [1] `ori v0,zero,0x21` @0x80115508   [0] `ori v0,zero,0x32` @0x80115248.
 * ------------------------------------------------------------------------------------------------ */
int re15_r38_sm_austritt(re15_actor_t *e)
{
    if (!e || e->sub_state_1 > 1) return 0;                  /* nur [0]/[1]; [2] (grid 0x43) bleibt alt */
    const int frei = (e->sub_state_1 == 1) ? 0x21 : 0x32;   /* @0x80115508 / @0x80115248 */
    switch (e->sub_state_2) {                                /* +0x6 */
    case 0:                                                  /* P0 @0x80115338 / @0x80115078 */
        e->flags |= 0x02;                                    /* word0 |= 2    @0x80115340 / @0x80115080 */
        e->flags |= 0x40;                                    /* word0 |= 0x40 @0x8011535C / @0x8011509C */
        e->motion = (e->grid_id == 0x41) ? 0x1D : 0x1C;      /* Clip 0x1C, grid 0x41 -> 0x1D @0x80115380-A8 */
        e->anim_frame = 0;                                   /* +0x95 = 0 @0x801153B8 */
        e->anim_frac  = 0;                                   /* +0x8F = 0 @0x801153C8 */
        e->sub_state_2 = 1;                                  /* +0x6 = 1  @0x801153D8 */
        (void)re15_r38_birkin_anim(e);                       /* anim_set  @0x801153F0 */
        e->no_draw = 1;                                      /* 26 Teile word0 &= ~1, Schleife @0x801153F8-448 */
        /* faellt in P1 (0x8011544C folgt direkt) */
        /* fallthrough */
    case 1:                                                  /* P1 @0x8011544C: kein anim_set */
        if (e->grid_id != 0x42) return 1;                    /* `ori v0,zero,0x42 / bne` @0x8011545C-60 */
        e->anim_frame = 0;                                   /* @0x80115468 */
        e->anim_frac  = 7;                                   /* @0x80115478 */
        e->sub_state_2 = 2;                                  /* @0x80115488 */
        /* fallthrough */
    default: {                                               /* P2 @0x8011548C */
        if (e->anim_frame == 6) e->no_draw = 0;              /* Bild 6: Teile |= 1 @0x8011549C-F4 */
        if (e->anim_frame == frei) {                         /* Bild 0x21 / 0x32 */
            e->flags = (uint8_t)(e->flags & ~0x02u);         /* @0x80115514-20 */
            e->flags = (uint8_t)(e->flags & ~0x40u);         /* @0x80115530-3C */
            e->grid_id = 0;                                  /* `sb zero,9` @0x80115564 */
        }
        if (re15_r38_birkin_anim(e)) {                       /* anim_set @0x8011557C, fertig -> */
            e->state = 1; e->sub_state_1 = 1;                /* +0x4-Wort = 0x101 @0x80115588-98 */
            e->sub_state_2 = 0; e->sub_state_3 = 0;
        }
        return 1;
    }
    }
}

int re15_r38_sm_versteckt(const re15_actor_t *e)
{
    return e && e->type == RE15_R38_SM_TYP && (e->grid_id & 0x60) != 0;   /* @0x8003710c */
}

int re15_r38_sm_schatten_aus(const re15_actor_t *e)
{
    /* +0xBC/+0xBE = 0 in P0 (@0x80115370-74) bis zum Freigabe-Bild (0x3E8/0x44C @0x80115548-54) — dieselben
     * Grenzen wie word0 & 0x40 (gesetzt @0x8011535C, geloescht @0x80115530-3C). */
    return e && e->type == RE15_R38_SM_TYP && e->state == 4 && (e->flags & 0x40) != 0;
}

/* ------------------------------------------------------------------------------------------------
 * Gemeinsame Helfer der Griffe
 * ------------------------------------------------------------------------------------------------ */
/* FUN_8001ad68(e, +0x84, +0x16C): Lage = Anker + RotY * Wurzel(Clip, Bild) aus der Hauptbank. */
static void sm_platzieren(re15_actor_t *e)
{
    re15_enemy_bank_t *gb = re15_enemy_find(e->type);
    if (gb && gb->ok)
        re15_clip_root_motion_abs_pub(e, &gb->skel, &gb->anim, (int)e->motion, (int)e->anim_frame);
}

/* Einmal-Ende eines Clips: das letzte Bild halten (wie der Port-Todeszustand 0x2B, Runde 34 B8). */
static int sm_anim_halten(re15_actor_t *e)
{
    int fertig = re15_r38_birkin_anim(e);
    if (fertig) e->anim_frame = (uint8_t)(re15_r38_birkin_fc(e) - 1);
    return fertig;
}

static void sm_gierschnapp(re15_actor_t *e, const re15_actor_t *pl)
{
    /* a8f8(Spielerlage, 0x800) — mit Klemme 0x800 schnappt der Blick ganz auf den Spieler. */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
}

/* ------------------------------------------------------------------------------------------------
 * (2a) GRIFF 1 — B[5] vorn 0x80112A64 (Phasen @0x8010026C) / B[9] hinten 0x80112F58 (Phasen @0x8010028C).
 * ------------------------------------------------------------------------------------------------ */
void re15_r38_sm_griff1(re15_actor_t *e, re15_actor_t *pl, int hinten)
{
    const uint8_t c_zu   = hinten ? 0x12 : 0x0E;   /* P0 @0x80112FB8 / @0x80112AC0 */
    const uint8_t c_halt = hinten ? 0x13 : 0x0F;   /* P2 @0x8011309C / @0x80112BA4 */
    const uint8_t c_los  = hinten ? 0x14 : 0x10;   /* P4 @0x801131B4 / @0x80112CD8 */
    switch (e->sub_state_2) {
    case 0: {                                                /* P0 @0x80112AA8 / @0x80112FA0 */
        e->sub_state_2 = 1;                                  /* +0x6 = 1 @0x80112AB4 / @0x80112FAC */
        e->motion = c_zu; e->anim_frame = 0; e->anim_frac = 7;   /* +0x94/+0x95/+0x8F */
        /* DER GEMEINSAME ANKER: FUN_8001ac38(Spieler) @0x80112B08 / @0x80113000 — Anker des Spiderman =
         * Lage - RotY(Blick VOR dem Schnapp) * Wurzel(Clip, Bild 0) aus +0x84/+0x16C (@0x8001acfc/@0x8001ad18),
         * dann KOPIE auf den Spieler (@0x8001ad28-48). Ohne diese Kopie platzierte die Opfer-Maschine den
         * Spieler vom Anker eines frueheren Griffs aus (Messung vorher: 14 600 Einheiten weit). */
        {
            re15_enemy_bank_t *gb = re15_enemy_find(e->type);
            if (gb && gb->ok) re15_clip_anchor_set_pub(e, &gb->skel, &gb->anim, (int)c_zu, 0);
            else              { e->anchor_x = e->x; e->anchor_z = e->z; }
            pl->anchor_x = e->anchor_x;
            pl->anchor_z = e->anchor_z;
        }
        e->birkin_grab = 1;                                  /* word0 |= 0x1000 @0x80112B24 / @0x8011301C */
        re15_r38_griff_pin();                                /* aca58 = 5 @0x80112B34 / @0x8011302C */
        pl->hit_react |= 1;                                  /* +0x93 |= 1 @0x80112B80 / @0x80113078 */
        sm_gierschnapp(e, pl);                               /* a8f8(.., 0x800) @0x80112B8C / @0x80113084 */
        sm_platzieren(e);                                    /* j 0x80112D44: ad68 @0x80112D5C */
        /* Spieler-Hook A P0 (0x80115D3C) laeuft im Original erst im Spieler-Tick, also NACH Schnapp und
         * Platzierung des Spiderman: Blick a8f8(Greiferlage, 0x800 - aca59<<12) @0x80115D88 (+0x800 vorn
         * @0x80115DA4-B8), Se_on(0x04000001) @0x80115DC8. Deshalb steht der Latch hier, nicht davor. */
        re15_r38_opfer_latch(e, pl, hinten ? 1 : 0);         /* aca59 = 0 @0x80112B3C / = 1 @0x80113034 */
        s_latches++;
        if (re15_r38_birkin_anim(e)) e->sub_state_2++;       /* anim_set, +0x6 += fertig */
        return;
    }
    case 1:                                                  /* P1 @0x80112D44 / @0x80113220 */
        re15_r38_griff_pin();
        sm_platzieren(e);
        if (re15_r38_birkin_anim(e)) e->sub_state_2++;
        return;
    case 2:                                                  /* P2 @0x80112B9C / @0x80113094 */
        e->motion = c_halt;                                  /* +0x94 (Bild bleibt) */
        e->sub_state_2 = 3;
        e->ai_timer = 100;                                   /* +0x9C = 0x64 @0x80112BC8 / @0x801130C0 */
        e->grab_kill_ctr = 0x32;                             /* +0x9E = 0x32 @0x80112BD8 / @0x801130D0 */
        /* fallthrough: P3 folgt direkt */
    case 3: {                                                /* P3 @0x80112BDC / @0x801130D4 */
        re15_r38_griff_pin();
        sm_platzieren(e);                                    /* ad68 @0x80112BF4 / @0x801130EC */
        (void)re15_r38_birkin_anim(e);                       /* anim_set (Halten = Schleife) */
        if (!hinten) {
            int alt = e->grab_kill_ctr;                      /* `lbu a0,158` @0x80112C24 */
            e->grab_kill_ctr = (int16_t)((alt - 1) & 0xff);  /* `sb v0,158` @0x80112C34 (u8) */
            if (alt == 0 || pl->hp < 0) {                    /* @0x80112C30 / `bgez` @0x80112C44 */
                e->sub_state_1 = 8; e->sub_state_2 = 0;      /* -> KOLLAPS @0x80112C58 / @0x80112C68 */
                return;
            }
        }
        /* Tasten: +0x9C -= 1 + 100 * FUN_80037024() (`sll/addu/sll/addu/sll` = *100 @0x80112C74-90 / @0x80113118-34) */
        e->ai_timer = (int16_t)(e->ai_timer - 1 - 100 * re15_re2z_mash());
        if (e->ai_timer < 0) {                               /* `bgez` @0x80112CB4 / @0x80113158 */
            e->sub_state_2 = 4;                              /* +0x6 = 4 @0x80112CBC / @0x80113160 */
            re15_player_victim_throwoff();                   /* aca5a = 4 @0x80112CC4 / @0x80113168 */
        }
        if (hinten) {
            int alt = e->grab_kill_ctr;                      /* `lbu a0,158` @0x80113178 */
            e->grab_kill_ctr = (int16_t)((alt - 1) & 0xff);  /* @0x80113188 */
            if (alt == 0) {                                  /* `bne a0,zero` @0x80113184 */
                e->sub_state_1 = 10; e->sub_state_2 = 0;     /* -> B[10] @0x80113198 / @0x801131A8 */
            }
        }
        return;
    }
    case 4:                                                  /* P4 @0x80112CD0 / @0x801131AC */
        e->motion = c_los; e->sub_state_2 = 5;
        e->anim_frame = 0; e->anim_frac = 7;                 /* +0x95 = 0, +0x8F = 7 */
        /* fallthrough: P5 folgt direkt */
    case 5:                                                  /* P5 @0x80112D20 / @0x801131FC */
        if (e->anim_frame == 0x18) re15_audio_room_se(0);    /* @0x80112D30-3C / @0x8011320C-18 */
        sm_platzieren(e);                                    /* ad68 @0x80112D5C / @0x80113238 */
        if (re15_r38_birkin_anim(e)) e->sub_state_2 = 6;
        return;
    default:                                                 /* P6 @0x80112DA0 / @0x8011327C */
        e->birkin_hurt_cd = 0x3C;                            /* +0x1DE = 0x3C @0x80112DAC / @0x80113288 */
        e->sub_state_1 = 3; e->sub_state_2 = 0;              /* +0x5 = 3, +0x6 = 0 */
        e->birkin_grab = 0;                                  /* word0 &= ~0x1000 @0x80112DE0-F0 */
        pl->hit_react = (uint8_t)(pl->hit_react & 0xFE);     /* +0x93 &= 0xFE @0x80112DFC-E00 */
        return;
    }
}

/* ------------------------------------------------------------------------------------------------
 * (2b) KOLLAPS — B[8] vorn 0x80112E20 (Clip 0x11, aca59 = 0) / B[10] hinten 0x80113300 (Clip 0x15, aca59 = 1).
 * Kein neuer Anker: der des Griffs 1 gilt weiter (Platzierung ad68 @0x80112F00 / @0x801133E0).
 * ------------------------------------------------------------------------------------------------ */
void re15_r38_sm_kollaps(re15_actor_t *e, re15_actor_t *pl, int hinten)
{
    (void)pl;
    if (e->sub_state_2 == 0) {                               /* P0 @0x80112E50 / @0x80113330 */
        e->sub_state_2 = 1;
        e->motion = hinten ? 0x15 : 0x11;                    /* @0x8011333C / @0x80112E5C */
        e->anim_frame = 0; e->anim_frac = 7;                 /* +0x95 = 0, +0x8F = 7 */
        re15_r38_griff_pin();
        re15_player_victim_devour(e);                        /* aca58 = 6 @0x80112EA4 / @0x80113384 */
        re15_r38_opfer_variante(hinten ? 1 : 0);             /* aca59 = 0 @0x80112EAC / = 1 @0x8011338C */
        re15_audio_room_se(8);                               /* 0x800453d0(8) @0x80112EE0 / @0x801133C0 */
    }
    if (e->sub_state_2 == 1) {                               /* P1 @0x80112EE8 / @0x801133C8 */
        re15_r38_griff_pin();
        sm_platzieren(e);
        if (sm_anim_halten(e)) e->sub_state_2 = 2;
    }
    /* P >= 2: nichts (@0x80112F40 / @0x80113420) — der Spieler liegt, cmd 7 setzt Hook B. */
}

/* ------------------------------------------------------------------------------------------------
 * (2c) GRIFF 2 — B[15] 0x8011411C (Phasen @0x80100324). EINMALIGE Spieler-Lage in P0, danach nie wieder.
 * ------------------------------------------------------------------------------------------------ */
void re15_r38_sm_griff2(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: {                                                /* P0 @0x80114160 */
        e->sub_state_2 = 1;                                  /* +0x6 = 1 @0x8011416C */
        e->motion = 0x18; e->anim_frame = 0; e->anim_frac = 7;   /* @0x8011417C / @0x8011418C / @0x8011419C */
        /* Lage merken (sh +0xA0/+0xA2 @0x801141BC / @0x801141D4), +0x8C = 0x66C (@0x801141E0-E8),
         * pos_advance(0x800) @0x801141E4 = (0x66C,0,0) gedreht um RotY(Blick + 0x800), */
        int16_t ax = (int16_t)e->x, az = (int16_t)e->z;
        e->crow_speed = 0x66C;
        int w = ((int)e->rot_y + 0x800) & 0xfff;
        int32_t vx = (int32_t)((int64_t)re15_cos_q12(w) * 0x66C >> 12);
        int32_t vz = (int32_t)((-(int64_t)re15_sin_q12(w)) * 0x66C >> 12);
        /* ... Spieler X/Z = die vorgeschobene Lage (@0x80114204 / @0x80114214). Die Wandklemme des naechsten
         * Spieler-Ticks (FUN_8003b0a4 @0x80031d70, Bezug = Lage vor der Versetzung) laeuft hier gleich mit. */
        {
            int32_t ox = pl->x, oz = pl->z, nx = e->x + vx, nz = e->z + vz;
            if (g_room_rdt_ok) {
                re15_collision_ensure_band(pl->y);
                re15_collision_constrain(&g_room_rdt, ox, oz, &nx, &nz);
                if (re15_r38_griff_durch_wand(ox, oz, nx, nz)) { nx = ox; nz = oz; }   /* NUTZER-VORGABE (3) unten */
            }
            pl->x = nx; pl->z = nz;
        }
        e->x = (int32_t)ax; e->z = (int32_t)az;              /* `lh` +0xA0/+0xA2 zurueck @0x80114218-38 */
        e->birkin_grab = 1;                                  /* word0 |= 0x1000 @0x80114250-54 */
        re15_r38_griff_pin();                                /* aca58 = 5 @0x80114264 */
        {
            int16_t blick = pl->rot_y;                       /* Hook A [2]/[3] 0x80115FF0: KEIN a8f8 */
            int v = re15_ai_facing_aligned(e, pl) + 2;       /* aca59 = a780(Spieler) + 2 @0x8011426C-84 */
            re15_r38_opfer_latch(e, pl, v);
            pl->rot_y = blick;
        }
        pl->hit_react |= 1;                                  /* +0x93 |= 1 @0x801142AC-B4 */
        s_latches++;
        if (re15_r38_birkin_anim(e)) e->sub_state_2++;       /* j 0x801143D8: anim_set, +0x6 += fertig */
        return;
    }
    case 1:                                                  /* P1 @0x801143D8 */
        re15_r38_griff_pin();
        if (re15_r38_birkin_anim(e)) e->sub_state_2++;
        return;
    case 2:                                                  /* P2 @0x801142C8 */
        e->motion = 0x19; e->sub_state_2 = 3;                /* @0x801142D4 / @0x801142E4 */
        e->ai_timer = 100;                                   /* +0x9C = 0x64 @0x801142F4 */
        /* fallthrough: P3 folgt direkt */
    case 3:                                                  /* P3 @0x801142F8 */
        re15_r38_griff_pin();
        (void)re15_r38_birkin_anim(e);                       /* anim_set @0x8011430C */
        if (pl->hp < 0) {                                    /* `bgez` @0x80114324 */
            pl->hp = 0;                                      /* `sh zero` @0x8011432C */
        } else {
            e->ai_timer = (int16_t)(e->ai_timer - 1 - 8 * re15_re2z_mash());   /* `sll v0,v0,3` @0x80114358 */
            if (e->ai_timer >= 0) return;                    /* `bgez` @0x8011437C */
        }
        e->sub_state_2 = 4;                                  /* +0x6 = 4 @0x80114384 */
        re15_player_victim_throwoff();                       /* aca5a = 4 @0x8011438C */
        return;
    case 4:                                                  /* P4 @0x80114398 */
        e->motion = 0x1A; e->sub_state_2 = 5;                /* @0x801143A4 / @0x801143B4 */
        e->anim_frame = 0; e->anim_frac = 7;                 /* @0x801143C4 / @0x801143D4 */
        /* fallthrough: P5 = 0x801143D8 */
    case 5:
        if (re15_r38_birkin_anim(e)) e->sub_state_2 = 6;
        return;
    default:                                                 /* P6 @0x80114414 */
        e->birkin_hurt_cd = 0x1E;                            /* +0x1DE = 0x1E @0x80114420 */
        e->sub_state_1 = 0x0D; e->sub_state_2 = 0;           /* +0x5 = 0xD @0x80114430, +0x6 = 0 @0x80114440 */
        e->birkin_grab = 0;                                  /* word0 &= ~0x1000 @0x80114454-64 */
        pl->hit_react = (uint8_t)(pl->hit_react & 0xFE);     /* @0x80114470-74 */
        return;
    }
}

/* ------------------------------------------------------------------------------------------------
 * Spieler-Opfer-Maschinen [0x2B]: Hook A 0x80115CC0 (Tabelle @0x8011A298 nach aca59), Hook B 0x801161DC
 * (Tabelle @0x8011A2B8: [0] 0x8011621C, [1] 0x801165F4).
 *   aca59 0/1: Zugriff aca59*4 (`sll v0,v0,2` @0x80115D7C), Halten +1 (`addiu v0,v0,1` @0x80115E5C),
 *              Loesen +2 (`addiu v0,v0,2` @0x80115ED8), Kollaps +3 (`addiu v0,v0,3` @0x8011628C / @0x80116664).
 *   aca59 2/3: Clip 8 (@0x80116044), 9 (@0x801160C8), 0xA (@0x80116108) — ohne Kollaps (Griff 2 toetet nicht).
 * ------------------------------------------------------------------------------------------------ */
int re15_r38_sm_opfer_clips(uint8_t typ, uint8_t v, uint8_t *c_intro, uint8_t *c_hold,
                            uint8_t *c_release, uint8_t *c_collapse)
{
    if (typ != RE15_R38_SM_TYP) return 0;
    if (v >= 2) {
        *c_intro = 8; *c_hold = 9; *c_release = 0x0A; *c_collapse = 0x0A;
    } else {
        *c_intro = (uint8_t)(v * 4); *c_hold = (uint8_t)(v * 4 + 1);
        *c_release = (uint8_t)(v * 4 + 2); *c_collapse = (uint8_t)(v * 4 + 3);
    }
    return 1;
}

int re15_r38_sm_griff2_opfer(uint8_t typ, uint8_t v)
{
    return typ == RE15_R38_SM_TYP && v >= 2;
}

/* ------------------------------------------------------------------------------------------------
 * (3) KEIN GRIFF DURCH EINE WAND — NUTZER-VORGABE "Sowas darf GARNICHT passieren, bei keinen Gegner."
 * Die Spieler-Wandklemme beider Originale ist ein PUNKT-Test gegen die um den Radius aufgeblasene Zelle:
 *   RE1.5 FUN_8003b0a4 (`(u32)(x - (zelle.x - r)) < (u32)(zelle.w + 2r)`, Port push_rect), RE2 FUN_8004c1bc
 *   (`subu v1,s4,v1 / addu v0,v0,a0 / sltu` @0x8004c370-80 und @0x8004c38c-9c, Lage + Radius gegen x/z/w/d).
 * Ein Sprung, der eine Wand GANZ ueberquert, landet hinter ihr ausserhalb jeder Zelle — die Klemme sieht nichts.
 * Im Original kommt das nicht vor; im Port setzen Opfer-Platzierungen mit grossem Wurzelversatz (RE2-Kriecher
 * ~2000 in ROOM1220, Runde 37 J_bissarm.md OFFEN 1: F13 von (-21086,-5496) nach (-22318,-5344) durch Zelle 7)
 * den Spieler hinter eine duenne Wand. FORM (keine neue Zahl): die Strecke vom letzten begehbaren Standpunkt zur
 * neuen Lage darf das INNERE keiner soliden Rechteckzelle (Typ 1) des Spielerbandes schneiden — Maske 1 = die
 * Spieler-Maske von re15_collision_constrain (`collision_constrain_impl(.., PR, 1u, ..)`), Band = das
 * Klemmband. Schneidet sie, bleibt der Standpunkt — derselbe Rueckfall, den re15_victim_place schon nimmt. */
int re15_r38_griff_durch_wand(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca || g_room_rdt.sca_count <= 0) return 0;
    if (x0 == x1 && z0 == z1) return 0;
    const int band = re15_collision_debug_band();
    if (band < 0) return 0;
    const double p0[2] = { (double)x0, (double)z0 };
    const double d[2]  = { (double)x1 - (double)x0, (double)z1 - (double)z0 };
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if (band != (c->floor >> 4) || (c->u0 & 1u) == 0 || c->type != 1) continue;
        const double r0[2] = { (double)(int32_t)c->x, (double)(int32_t)c->z };
        const double r1[2] = { r0[0] + (double)c->width, r0[1] + (double)c->density };
        double lo = 0.0, hi = 1.0; int drin = 1;
        for (int a = 0; a < 2 && drin; a++) {
            if (d[a] == 0.0) { if (p0[a] <= r0[a] || p0[a] >= r1[a]) drin = 0; continue; }
            double t0 = (r0[a] - p0[a]) / d[a], t1 = (r1[a] - p0[a]) / d[a];
            if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
            if (t0 > lo) lo = t0;
            if (t1 < hi) hi = t1;
            if (!(lo < hi)) drin = 0;                        /* offenes Inneres: Beruehren zaehlt nicht */
        }
        if (drin) return 1;
    }
    return 0;
}
