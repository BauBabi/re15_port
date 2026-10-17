/*
 * npckante_r38.c — Runde 38, Spur M "npckante".
 * Dossier: analysis/befunde_runde38/M_npckante.md. Konstanten mit Adresse: include/re15_npckante_r38.h.
 *
 * Der Port fuehrte die NPC-Plc_dest-Subs 4/5 (enemy_ai_common.c re15_npc_sub_walk) ohne die Kantensonde: der
 * Schritt aendert nur x/z, y bleibt auf dem Startband stehen, und die Absprungzellen (u0 = 0x01, nur fuer den
 * Spieler fest) klemmen einen NPC (Klemm-Maske 4) nicht. Folge: John/Marvin/Ada laufen ueber jede Kante waagrecht
 * in die Luft weiter; an einer Kletterkante (u0 = 0xff) bleiben sie an der Wand haengen. Das Original
 * (selbst disassembliert, info/Re1.5/PSX.EXE):
 *
 *   FUN_8002d7d8 KANTENSONDE (a0 = Entity), Ergebnis -> +0x9f:
 *     @0x8002d7fc-820  Vektor {800,0,0} von @0x800109c8 nach sp+16
 *     @0x8002d82c-844  Kurs q = (s16)((rot_y + 512) & ~1023), FUN_8004f008(q, v, v) = RotMatrix + ApplyMatrixSV
 *     @0x8002d848-86c  Sondenpunkt = (x + v.x, z + v.z)
 *     @0x8002d870-874  w = FUN_8003b7f0(&punkt, 0, +0x82)  = Zellwort (u1 | floor<<8) der ersten Zelle im Band
 *     @0x8002d884      w & 1:  Objektpool 0x800b3f98 (Stride 148, Zahl = RDT-Kopf Byte 2), aktiv (Wort0 & 1):
 *                      FUN_8002da4c(punkt, obj, box.hx >> 1, +0x82 + 1) oder (..., +0x82 - 2) -> 0; sonst -> 1
 *     @0x8002d94c      sonst:  FUN_8002da4c(punkt, obj, ent.box.hx(+0x78)->+6, +0x82) oder (..., +0x82 - 1) -> 0;
 *     @0x8002d9f0-a1c  w & 2:  h = room_coll(&punkt, 1, +0x82, 0x100); ent.y < h -> 2; sonst 0
 *
 *   Sub 0x12/0x16 = 0x80052508 KANTE (Phasen +0x6 ueber @0x800112e4):
 *     [0] @0x80052548  +0x8c=0x4b, +0x6=1, +0x94=5, +0x95=0, +0x8f=7                         -> faellt in [1]
 *     [1] @0x80052598  (Schritt-SE: Bildwort & 0x4000 -> FUN_80045630, s. OFFEN) anim_set(+0x170,+0x174,0,0x200)
 *                      @0x800525f0; Kurs rasten @0x80052604-2c (Bit 0x200 ? +((k>>2)&0xff) : -((k>>2)&0xff));
 *                      (Kurs & 0x3e0) == 0 @0x80052644 -> +0x6 = 2 @0x80052650; dann +0x9f == 2 @0x80052660-68
 *                      -> +0x5 = (+0x5 == 0x12) ? 0x18 : 0x17, +0x6 = 0 @0x80052670-850
 *     [2] @0x80052688  +0x6=3, +0x94=2, +0x95=0, +0x8f=7                                      -> faellt in [3]
 *     [3] @0x800526c8  VOR anim_set: +0x95 == 0x11 -> +0x93 |= 1; +0x95 == 0x1d -> +0x8c = 0x47c, pos_advance(0),
 *                      y -= 1800, +0x1ba -= 1800, +0x82 += 1, +0x40/42/44 = Lage; +0x95 == 0x25 -> +0x93 &= 0xfe;
 *                      anim_set(+0x84,+0x16c,0,0x200) @0x800527f8, +0x6 += Clip-Ende @0x80052814-1c
 *     [4] @0x80052820  +0x5 = (+0x5 == 0x12) ? 4 : 5, +0x6 = 0
 *
 *   Sub 0x17/0x18 = 0x80052c14 SPRUNG (Phasen +0x6 ueber @0x8001131c):
 *     [0] @0x80052c54  +0x6=1, +0x94=3, +0x95=0, +0x8f=7                                      -> faellt in [1]
 *     [1] @0x80052c94  anim_set(+0x84,+0x16c) @0x80052ca8; DANACH +0x95 == 0xd -> +0x6 = 2
 *     [2] @0x80052cd4  +0x93 |= 1, +0x6=3, +0x94=4, +0x95=0, +0x8f=0, +0x9c=0, +0x8c=0x320, pos_advance(0),
 *                      y += 500, +0x1ba = room_coll(&lage, 0, +0x82, 0x100), +0x82 = -(+0x1ba/1800),
 *                      +0x40/42/44 = Lage                                                     -> faellt in [3]
 *     [3] @0x80052e1c  t = +0x9c++; y += 90 + 50*t; anim_set(+0x84,+0x16c) @0x80052e74;
 *                      (+0x1ba - 800) < y -> +0x6 = 4
 *     [4] @0x80052ea8  +0x6=5, +0x94=5, +0x95=0, +0x8f=0, y = +0x1ba, +0x93 &= 0xfe          -> faellt in [5]
 *     [5] @0x80052f50  anim_set(+0x84,+0x16c) @0x80052f64; +0x6 += Clip-Ende
 *     [6] @0x80052f8c  +0x5 = (+0x5 == 0x18) ? 4 : 5, +0x6 = 0
 *
 * POSE-DANN-VORSCHUB (enemy_ai_common.c re15_npc_anim): FUN_8001f314 posiert +0x95 und erhoeht danach. Im Port
 * steht in anim_frame das in DIESEM Tick posierte Bild; ein Original-Test "+0x95 == n VOR anim_set" ist also
 * "das jetzt zu posierende Bild == n" (bild_jetzt), ein Test DANACH ist "posiertes Bild + 1 == n".
 */
#include "re15_npckante_r38.h"
#include "re15_actor.h"
#include "re15_collision.h"
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re15_engine.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_skeleton.h"     /* re15_cos_q12 / re15_sin_q12 */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

static int8_t   s_9f[RE15_ACTOR_MAX];          /* +0x9f = Sondenergebnis (`sb v0,159(v1)` @0x80051388/@0x800516d8) */
static uint16_t s_fall_t[RE15_ACTOR_MAX];      /* +0x9c = Fallzaehler des Sprungs (`sh zero,156` @0x80052d3c)     */
static uint8_t  s_schub[RE15_ACTOR_MAX];       /* 1 = Schub in DIESEM Tick (+0x40/42/44-Schnappschuss, s.u.)      */
static uint8_t  s_nachlauf[RE15_ACTOR_MAX];    /* 1 = Rueckgabe an Sub 4/5 in diesem Tick (Render haelt Loco-Pose) */

static int slot_of(const re15_actor_t *e)
{
    int s = (int)(e - g_actors);
    return (s >= 0 && s < RE15_ACTOR_MAX) ? s : -1;
}

static int ist_npc(const re15_actor_t *e)
{
    return e && e->type >= 0x40 && e->type <= 0x4d;
}

static int ist_kante(uint8_t s) { return s == RE15_NPK_SUB_KANTE_GEHEN || s == RE15_NPK_SUB_KANTE_LAUFEN; }
static int ist_sprung(uint8_t s) { return s == RE15_NPK_SUB_SPRUNG_LAUF || s == RE15_NPK_SUB_SPRUNG_GEHEN; }

int re15_npckante_aktiv(const re15_actor_t *e)
{
    if (!ist_npc(e) || e->state != 4 || e->walk_active) return 0;
    return ist_kante(e->sub_state_1) || ist_sprung(e->sub_state_1);
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Sonde FUN_8002d7d8                                                                                                 */
/* ------------------------------------------------------------------------------------------------------------------ */
static int prop_aktiv(int p)
{
    return p >= 0 && p < (int)RE15_SCD_MAX_PROPS && p < (int)g_scd.prop_count &&
           g_scd.props[p].active && (g_scd.props[p].flags & 1u);   /* Wort0 & 1 @0x8002d8d4 / @0x8002d994 */
}

int re15_npckante_sonde(const re15_actor_t *e)
{
    if (!e || !g_room_rdt.sca || g_room_rdt.sca_count <= 0) return 0;
    /* Kurs rasten @0x8002d82c-844: lhu +0x6a, +512, & -1024, s16 -> RotMatrix-Winkel (Tabellen & 0xfff) */
    int q = (int)(int16_t)((((int)(uint16_t)e->rot_y) + 512) & -1024);
    q &= 0xfff;
    int32_t dx = (int32_t)(((int32_t)re15_cos_q12(q) * RE15_NPK_SONDE_WEG) >> 12);  /* m00*vx      */
    int32_t dz = (int32_t)(((int32_t)-re15_sin_q12(q) * RE15_NPK_SONDE_WEG) >> 12); /* m20*vx = -s */
    int32_t px = e->x + dx, pz = e->z + dz;                                          /* @0x8002d848-86c */
    int band = (int)e->floor;                                                        /* lbu 130 @0x8002d870 */
    uint16_t w = re15_collision_floor_typeword(&g_room_rdt, px, pz, band, 0);        /* @0x8002d874 */
    int p;
    if (w & RE15_NPK_ZELLE_KLETTERN) {                                               /* @0x8002d884 */
        for (p = (int)g_scd.prop_count - 1; p >= 0; p--) {
            if (!prop_aktiv(p)) continue;
            int32_t m = (int32_t)((uint32_t)g_scd.props[p].box_hx >> 1);             /* lhu 6 / srl 1 @0x8002d8f4-900 */
            if (re15_collision_prop_box_hit(p, px, pz, m, (band + 1) & 0xff)) return 0;   /* @0x8002d8fc */
            if (re15_collision_prop_box_hit(p, px, pz, m, (band - 2) & 0xff)) return 0;   /* @0x8002d928 */
        }
        return 1;                                                                    /* @0x8002d944 */
    }
    for (p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (!prop_aktiv(p)) continue;
        int32_t m = (int32_t)e->hit_radius_min;                                      /* ent+0x78 -> +6 @0x8002d9a4-ac */
        if (re15_collision_prop_box_hit(p, px, pz, m, band & 0xff)) return 0;        /* @0x8002d9b0 */
        if (re15_collision_prop_box_hit(p, px, pz, m, (band - 1) & 0xff)) return 0;  /* @0x8002d9d4 */
    }
    if (w & RE15_NPK_ZELLE_SPRUNG) {                                                 /* @0x8002d9f0 */
        int16_t h = re15_collision_room_coll(&g_room_rdt, px, pz, 1, band, RE15_NPK_SONDE_MASKE);   /* @0x8002da04 */
        if (e->y < (int32_t)h) return 2;                                             /* `slt v1,v1,v0` @0x8002da14 */
    }
    return 0;
}

/* Haken in re15_npc_sub_walk (Phase 2): Sub 4 nach anim_set (@0x80051374), Sub 5 vor anim_set (@0x800516c4). */
int re15_npckante_nach_schritt(re15_actor_t *e)
{
    int s = slot_of(e);
    if (s < 0 || !ist_npc(e)) return 0;
    if (e->sub_state_1 != 4 && e->sub_state_1 != 5) return 0;     /* nur 0x80051148/0x80051484 rufen die Sonde */
    int r = re15_npckante_sonde(e);
    s_9f[s] = (int8_t)r;                                           /* `sb v0,159(v1)` — auch 0 wird geschrieben */
    if (r == 0) return 0;
    e->sub_state_1 = (uint8_t)((e->sub_state_1 == 4) ? RE15_NPK_SUB_KANTE_GEHEN : RE15_NPK_SUB_KANTE_LAUFEN);
    e->sub_state_2 = 0;                                            /* `sb zero,6(v0)` @0x800513b4 / @0x80051704 */
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[npckante] ROOM%04X slot %d Typ %02X: Sonde %d bei (%d,%d,%d) Band %d -> Sub 0x%02X\n",
            (unsigned)g_current_room_id, s, e->type, r, (int)e->x, (int)e->y, (int)e->z, (int)e->floor,
            e->sub_state_1);
#endif
    return 1;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Clip-Uhr (Pose-dann-Vorschub, wie re15_npc_anim)                                                                   */
/* ------------------------------------------------------------------------------------------------------------------ */
static int clip_laenge(const re15_actor_t *e, int loco)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const re15_emd_animation_t *an = NULL;
    if (b && loco && b->loco_ok) an = &b->anim_loco;
    if (b && !loco && b->own_ok) an = &b->anim_own;
    int c = (int)e->motion;
    if (an && c >= 0 && c < an->clip_count && an->clips[c].frame_count > 0) return an->clips[c].frame_count;
    return 1;
}

static void clip_start(re15_actor_t *e, int clip, int blend)
{
    e->motion = (int16_t)clip;                 /* +0x94 */
    e->anim_frame = 0;                         /* +0x95 = 0 */
    e->anim_frac = (uint8_t)(blend ? 7 : 0);   /* +0x8f = 7 bzw. 0 */
    e->motion_init_delay = 1;                  /* Saat-Tick posiert Bild 0 (re15_npc_seed_clip) */
}

/* Bild, das anim_set in diesem Tick posieren wird (= Original-+0x95 VOR dem Aufruf). */
static int bild_jetzt(const re15_actor_t *e, int fc)
{
    if (fc < 1) fc = 1;
    return (e->motion_init_delay > 0) ? (int)e->anim_frame : (int)((e->anim_frame + 1) % fc);
}

/* anim_set: posiert, schiebt danach vor; Rueckgabe 1 im Tick des letzten Bildes (@0x8001f610-63c). */
static int clip_schritt(re15_actor_t *e, int loco)
{
    int fc = clip_laenge(e, loco);
    if (e->motion_init_delay > 0) e->motion_init_delay--;
    else e->anim_frame = (e->anim_frame + 1) % fc;
    if (e->anim_frac > 0) e->anim_frac--;
    return ((int)e->anim_frame + 1 >= fc) ? 1 : 0;
}

/* pos_advance(0) = FUN_800245d8: +0x8c entlang rot_y (re15_dog_advance-Zwilling). */
static void schritt(re15_actor_t *e, int32_t tempo)
{
    e->crow_speed = (int16_t)tempo;                                                   /* +0x8c */
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->crow_speed) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->crow_speed) >> 12);
}

static void kurs_rasten(re15_actor_t *e)    /* @0x80052604-2c */
{
    int k = (int)(uint16_t)e->rot_y;
    int t = (k >> 2) & 0xff;
    k = (k & 0x200) ? (k + t) : (k - t);
    e->rot_y = (int16_t)(uint16_t)k;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Sub 0x12/0x16 — 0x80052508                                                                                         */
/* ------------------------------------------------------------------------------------------------------------------ */
static void sub_kante(re15_actor_t *e, int s)
{
    switch (e->sub_state_2) {
    case 0:                                                     /* @0x80052548 */
        e->crow_speed = RE15_NPK_KANTE_TEMPO;                   /* +0x8c = 0x4b @0x80052554 */
        e->sub_state_2 = 1;                                     /* @0x80052564 */
        clip_start(e, RE15_NPK_KANTE_CLIP_RAST, 1);             /* +0x94=5 / +0x95=0 / +0x8f=7 @0x80052570-594 */
        /* fall through */
    case 1:                                                     /* @0x80052598 */
        (void)clip_schritt(e, 0);                               /* anim_set(+0x170,+0x174) @0x800525f0 */
        kurs_rasten(e);                                         /* @0x80052604-2c */
        if ((((int)(uint16_t)e->rot_y) & 0x3e0) == 0) {         /* @0x80052644-48 */
            e->sub_state_2 = 2;                                 /* @0x80052650 */
            if (s_9f[s] == 2) {                                 /* `lb v1,159` @0x80052660-68 */
                e->sub_state_1 = (uint8_t)((e->sub_state_1 == RE15_NPK_SUB_KANTE_GEHEN)
                                           ? RE15_NPK_SUB_SPRUNG_GEHEN : RE15_NPK_SUB_SPRUNG_LAUF);
                e->sub_state_2 = 0;                             /* @0x80052850 */
            }
        }
        break;
    case 2:                                                     /* @0x80052688 */
        e->sub_state_2 = 3;
        clip_start(e, RE15_NPK_KANTE_CLIP_HOCH, 1);             /* +0x94=2 / +0x95=0 / +0x8f=7 @0x800526a0-c4 */
        /* fall through */
    case 3: {                                                   /* @0x800526c8 */
        int b = bild_jetzt(e, clip_laenge(e, 1));
        if (b == RE15_NPK_HOCH_BILD_AN) e->hit_react |= 1u;     /* @0x800526e4-f0 */
        if (b == RE15_NPK_HOCH_BILD_STUFE) {                    /* @0x80052704 */
            schritt(e, RE15_NPK_HOCH_SCHUB);                    /* +0x8c = 0x47c, pos_advance(0) @0x80052710-18 */
            e->y -= RE15_NPK_BAND_HOEHE;                        /* @0x80052730 */
            e->dog_floor_y = (int16_t)(e->dog_floor_y - RE15_NPK_BAND_HOEHE);   /* +0x1ba @0x8005274c */
            e->floor = (uint8_t)(e->floor + 1);                 /* +0x82 @0x80052768 */
            s_schub[s] = 1;                                     /* +0x40/42/44 = Lage @0x80052784-b4 */
        }
        if (b == RE15_NPK_HOCH_BILD_AUS) e->hit_react &= 0xfeu; /* @0x800527d4-e0 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + clip_schritt(e, 1));   /* @0x800527f8-81c */
        break;
    }
    case 4:                                                     /* @0x80052820 */
        e->sub_state_1 = (uint8_t)((e->sub_state_1 == RE15_NPK_SUB_KANTE_GEHEN) ? 4 : 5);   /* @0x8005282c-40 */
        e->sub_state_2 = 0;                                     /* @0x80052850 */
        s_nachlauf[s] = 1;
        break;
    default:                                                    /* `sltiu v0,v1,5` @0x80052520 -> Exit */
        break;
    }
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Sub 0x17/0x18 — 0x80052c14                                                                                         */
/* ------------------------------------------------------------------------------------------------------------------ */
static void sub_sprung(re15_actor_t *e, int s)
{
    switch (e->sub_state_2) {
    case 0:                                                     /* @0x80052c54 */
        e->sub_state_2 = 1;
        clip_start(e, RE15_NPK_SPRUNG_CLIP_AB, 1);              /* +0x94=3 / +0x95=0 / +0x8f=7 @0x80052c6c-90 */
        /* fall through */
    case 1:                                                     /* @0x80052c94 */
        (void)clip_schritt(e, 1);                               /* anim_set(+0x84,+0x16c) @0x80052ca8 */
        if ((int)e->anim_frame + 1 == RE15_NPK_SPRUNG_BILD_AB)  /* +0x95 (nach anim_set) == 0xd @0x80052cbc-c4 */
            e->sub_state_2 = 2;                                 /* @0x80052cd0 */
        break;
    case 2:                                                     /* @0x80052cd4 */
        e->hit_react |= 1u;                                     /* +0x93 |= 1 @0x80052cec */
        e->sub_state_2 = 3;                                     /* @0x80052cfc */
        clip_start(e, RE15_NPK_SPRUNG_CLIP_LUFT, 0);            /* +0x94=4 / +0x95=0 / +0x8f=0 @0x80052d08-2c */
        s_fall_t[s] = 0;                                        /* +0x9c = 0 @0x80052d3c */
        schritt(e, RE15_NPK_SPRUNG_SCHUB);                      /* +0x8c = 0x320, pos_advance(0) @0x80052d48-4c */
        e->y += RE15_NPK_SPRUNG_HUB;                            /* @0x80052d68 */
        {
            int16_t h = re15_collision_room_coll(&g_room_rdt, e->x, e->z, 0, (int)e->floor,
                                                 RE15_NPK_SONDE_MASKE);   /* @0x80052d84 */
            e->dog_floor_y = h;                                 /* +0x1ba @0x80052d98 */
            /* +0x82 = -(+0x1ba / 1800): Kehrwert 0x91a2b3c5, `sra 10`, Vorzeichenkorrektur @0x80052da4-d0 */
            e->floor = (uint8_t)(-((int32_t)h / RE15_NPK_BAND_HOEHE));
        }
        s_schub[s] = 1;                                         /* +0x40/42/44 = Lage @0x80052de8-e18 */
        /* fall through */
    case 3: {                                                   /* @0x80052e1c */
        int32_t t = (int32_t)s_fall_t[s];
        s_fall_t[s] = (uint16_t)(t + 1);                        /* +0x9c += 1 @0x80052e40 */
        e->y += RE15_NPK_FALL_START + RE15_NPK_FALL_ZUWACHS * t;    /* y += 90 + 50*t @0x80052e34-5c */
        (void)clip_schritt(e, 1);                               /* anim_set @0x80052e74 */
        if ((int32_t)e->dog_floor_y - RE15_NPK_LANDE_ABSTAND < e->y)   /* @0x80052e88-94 */
            e->sub_state_2 = 4;                                 /* @0x80052ea4 */
        break;
    }
    case 4:                                                     /* @0x80052ea8 */
        e->sub_state_2 = 5;
        clip_start(e, RE15_NPK_SPRUNG_CLIP_LAND, 0);            /* +0x94=5 / +0x95=0 / +0x8f=0 @0x80052ec4-e4 */
        e->y = (int32_t)e->dog_floor_y;                         /* y = +0x1ba @0x80052efc */
        e->hit_react &= 0xfeu;                                  /* +0x93 &= 0xfe @0x80052f18 */
        /* (@0x80052f28-4c: leere Zaehlschleife ohne Speicherwirkung) — fall through */
    case 5:                                                     /* @0x80052f50 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + clip_schritt(e, 1));   /* @0x80052f64-88 */
        break;
    case 6:                                                     /* @0x80052f8c */
        e->sub_state_1 = (uint8_t)((e->sub_state_1 == RE15_NPK_SUB_SPRUNG_GEHEN) ? 4 : 5);  /* @0x80052f98-ac */
        e->sub_state_2 = 0;                                     /* @0x80052fbc */
        s_nachlauf[s] = 1;
        break;
    default:                                                    /* `sltiu v0,v1,0x7` @0x80052c2c -> Exit */
        break;
    }
}

int re15_npckante_dispatch(re15_actor_t *e)
{
    int s = slot_of(e);
    if (s < 0 || !ist_npc(e)) return 0;
    if (ist_kante(e->sub_state_1))  { sub_kante(e, s);  return 1; }
    if (ist_sprung(e->sub_state_1)) { sub_sprung(e, s); return 1; }
    return 0;
}

int re15_npckante_kanal(const re15_actor_t *e)
{
    int s = slot_of(e);
    if (s < 0 || !ist_npc(e) || e->state != 4 || e->walk_active) return 0;
    if (ist_kante(e->sub_state_1))  return (e->sub_state_2 >= 3) ? 1 : 2;   /* [0..2] Kanal +0x170, [3/4] +0x84 */
    if (ist_sprung(e->sub_state_1)) return (e->sub_state_2 >= 1) ? 1 : 2;   /* Tick der Uebergabe: noch die Rast-Pose */
    /* Uebergabe-Tick an Sub 4/5 ([4] @0x80052820 / [6] @0x80052f8c): kein anim_set in diesem Tick — die Pose ist
     * noch das letzte Loco-Bild (der Port leitet die Bank je Bild neu her, das Original posiert die Locals weiter). */
    if ((e->sub_state_1 == 4 || e->sub_state_1 == 5) && e->sub_state_2 == 0 && s_nachlauf[s] &&
        (e->motion == RE15_NPK_KANTE_CLIP_HOCH || e->motion == RE15_NPK_SPRUNG_CLIP_LAND))
        return 1;
    return 0;
}

void re15_npckante_altlage(const re15_actor_t *e, int32_t *ox, int32_t *oz)
{
    int s = slot_of(e);
    if (s < 0 || !ox || !oz) return;
    if (s_schub[s]) {                                           /* der Schub lief in DIESEM Tick (Dispatch davor) */
        *ox = e->x;                                             /* +0x40 = +0x34 @0x80052784 / @0x80052de8 */
        *oz = e->z;                                             /* +0x44 = +0x3c @0x8005279c / @0x80052e00 */
        s_schub[s] = 0;
    }
    /* Die Rueckgabe-Pose gilt nur bis zum naechsten Tick des Subs 4/5 (dessen INIT setzt +0x6 = 1 und posiert). */
    if (!((e->sub_state_1 == 4 || e->sub_state_1 == 5) && e->sub_state_2 == 0)) s_nachlauf[s] = 0;
}

int re15_npckante_schatten_y(const re15_actor_t *e, int32_t *y)
{
    if (!ist_npc(e) || !y) return 0;
    if (!re15_npckante_aktiv(e) && !(e->fall_1c0 & 0x8000u)) return 0;   /* Fall-Bit @0x8001be6c */
    *y = (int32_t)e->dog_floor_y;                                       /* +0x1ba, `lh a1,442` @0x8011dd78 */
    return 1;
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Messhaken                                                                                                          */
/* ------------------------------------------------------------------------------------------------------------------ */
void re15_npckante_spur(const re15_actor_t *e)
{
#ifdef RE15_PLATFORM_PC
    static FILE *f = NULL; static int init = 0;
    if (!init) { init = 1; const char *p = getenv("RE15_NPCKANTE_LOG"); if (p && *p) f = fopen(p, "w"); }
    if (!f || !ist_npc(e) || !e->active) return;
    int s = slot_of(e);
    int16_t unten = re15_collision_room_coll(&g_room_rdt, e->x, e->z, 0, (int)e->floor, RE15_NPK_SONDE_MASKE);
    /* fall = das Kriterium der Engine-Schwerkraft FUN_8001bd60: Mittelpunkt mindestens den Radius tief in einer
     * Absprungzelle (Bit 1) des eigenen Bandes (FUN_8003b7f0(&+0x34, -r, +0x82) @0x8001bd94-b4) */
    int fall = (re15_collision_floor_typeword(&g_room_rdt, e->x, e->z, (int)e->floor,
                                              -(int32_t)e->hit_radius_min) & RE15_NPK_ZELLE_SPRUNG) ? 1 : 0;
    fprintf(f, "F%u R%04X s%d t%02X x=%d y=%d z=%d b=%d fy=%d st=%d s1=%d s2=%d mo=%d af=%d k=%d sonde=%d boden=%d r=%d"
               " fall=%d f1c0=%04x\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, s, e->type, (int)e->x, (int)e->y, (int)e->z,
            (int)e->floor, (int)e->dog_floor_y, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
            (int)e->anim_frame, re15_npckante_kanal(e), re15_npckante_sonde(e), (int)unten, (int)(uint16_t)e->rot_y,
            fall, (unsigned)e->fall_1c0);
    fflush(f);
#else
    (void)e;
#endif
}
