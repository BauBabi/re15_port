/*
 * szenenleon_r36.c — Runde 36, Spur J "szenenleon".
 * Dossier: analysis/befunde_runde36/J_szenenleon.md. Konstanten mit Adresse: include/re15_szenenleon_r36.h.
 *
 * Teil 1 — Plc_dest Modus 0x13, der SKRIPT-SCHUB (Nutzer: "In der Cutscene in ROOM 11B0 ... klappt das mit der
 *   Animation und dem gemeinsamen Wegschieben des Containers mit Marvin noch nicht"). Der Port kannte Modus 0x13
 *   nicht (op_plc_dest: "POSE/EVENT, kein Walk"; Marvin fiel auf re15_npc_sub_idle) und Member_set Id 6 auf
 *   einem Objekt war ein No-Op. Original (selbst disassembliert, info/Re1.5/PSX.EXE):
 *     Executor-Sub @0x80052864, Phasen +0x6 ueber die Sprungtabelle @0x800112fc:
 *       [0] @0x800528a4  +0x6=1, +0x94=5, +0x95=0, +0x8f=7, +0x8c=0                 -> faellt in [1]
 *       [1] @0x800528f4  anim_set(+0x170,+0x174,0,0x200) @0x8005294c; Kurs-Rastung @0x80052960-88;
 *                        (Kurs & 0x3e0) == 0 -> +0x6 = 2 (@0x800529a0-b0)
 *       [2] @0x800529b4  +0x6=3, +0x94=0x11, +0x95=0, +0x8f=7                          -> faellt in [3]
 *       [3] @0x800529f4  anim_set(+0x84,+0x16c,0,0x200) @0x80052a08; +0x6 += Clip-Ende; Rastung weiter
 *       [4] @0x80052a74  +0x6=5, +0x94=0x12, +0x95=0, +0x8f=7                          -> faellt in [5]
 *       [5] @0x80052ab4  +0x95==1 -> FUN_80045024(0x02070000) @0x80052ad8; anim_set Loop @0x80052af4;
 *                        Bild-Wort & 0x2000 -> FUN_800369f8(0,0) @0x80052b24; Bit +0x1c3 in Bank 5
 *                        (FUN_8004efe4(0x800b1028,...) @0x80052b44) -> +0x6=6, +0x94=0x11, +0x95=0
 *       [6] @0x80052b84  anim_set(+0x84,+0x16c,1,0x200) @0x80052b98 (RUECKWAERTS); +0x6 += Clip-Ende
 *       [7] @0x80052bc0  FUN_80045024(0x02070000) @0x80052bcc; +0x5 = 6 @0x80052be0, +0x6 = 0 @0x80052bf0
 *   Der Kanal +0x84/+0x16c ist beim SPIELER das COMMON-Paar PL00.EMR/PL00.EDD (0x800acad8/0x800acbc0, einzige
 *   Schreiber @0x80031578/@0x8003154c) — dieselben Clips 0x11/0x12 wie das Kistenschieben LAB_80035810
 *   (player_common.c) —, bei den NPCs die Loco-Bank dir[2]/dir[1] (FUN_80022300 @0x800224a8/@0x80022498).
 *   Das Objekt bewegt NICHT der Sub, sondern der Objekt-Pass: Bit 0x200 im Objekt-Wort 0 (Member_set Id 6,
 *   @0x800411c8 `sw a2,0(a0)`) -> FUN_8002cabc(Spieler, Objekt, 1) @0x8002bed4 = der Spieler schiebt es.
 *
 * Teil 4 — Irons (ROOM1150) nach dem Tod: Kopf-FSM-Flags 0x12 (siehe irons_kopf_tick).
 */
#include "re15_szenenleon_r36.h"
#include "re15_actor.h"
#include "re15_audio.h"
#include "re15_collision.h"
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re15_irons_tod.h"
#include "re15_player.h"
#include "re15_scd.h"
#include "re15_skeleton.h"     /* re15_cos_q12 / re15_sin_q12 */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

extern unsigned g_current_room_id;

#define RE15_SZL_REVERSE 0x80u   /* Port-Richtungsbit (player_common.c RE15_ANIM_REVERSE, anim_select_common.c) */

static void szl_log(const char *was)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[szl-r36] ROOM%04X %s\n", (unsigned)g_current_room_id, was);
#else
    (void)was;
#endif
}

/* ===================================================================================================== */
/*  Gemeinsame Bausteine des Sub 0x13                                                                    */
/* ===================================================================================================== */

/* Kurs-Rastung auf 90 Grad @0x80052960-88 (Phase 1) bzw. @0x80052a38-70 (Phase 3), auf dem ROHEN 16-Bit-
 * Kurswert (`lhu`/`sh` +0x6a): t = (Kurs>>2)&0xff; Kurs & 0x200 ? Kurs+t : Kurs-t. Identisch zur Rastung des
 * Kistenschiebens @0x800358d4-fc (player_common.c push_yaw_snap). */
static void kurs_rasten(re15_actor_t *a)
{
    int yaw = (int)(uint16_t)a->rot_y;
    int t   = (yaw >> 2) & 0xff;
    yaw = (yaw & 0x200) ? (yaw + t) : (yaw - t);
    a->rot_y = (int16_t)(uint16_t)yaw;
}
static int kurs_gerastet(const re15_actor_t *a)
{
    return (((int)(uint16_t)a->rot_y) & 0x3e0) == 0;     /* @0x800529a0 / @0x80052a40 */
}

/* Wurzelbewegung FUN_800369f8(0,0) (@0x80036aac-ae8: die Wurzel-Weltlage wird gegen die Vorlage
 * verschoben). Der Port hat keinen Engine-Pose-Puffer (+0x188) und nimmt dieselbe Groesse aus ihrer Quelle,
 * der Wurzel-Translation der Keyframes, kursgedreht — derselbe Weg wie das Kistenschieben (player_common.c
 * push_root_step, Messbeleg probe_1090_push: PL00 Clip 0x12 Wurzel-X -312 -> +211 = +523 je Zyklus). */
static void wurzel_schritt(re15_actor_t *a, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                           int clip, int slot_now, int slot_prev)
{
    if (!sk || !an || clip < 0 || clip >= an->clip_count) return;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (c->frame_count <= 0 || slot_now >= c->frame_count) return;
    if (slot_prev < 0 || slot_now != slot_prev + 1) return;          /* Clip-Start / Umlauf: neu ankern */
    int kf_n = (int)(re15_emd_get_frame_entry(an, clip, slot_now)  & 0xFFFu);
    int kf_p = (int)(re15_emd_get_frame_entry(an, clip, slot_prev) & 0xFFFu);
    int16_t sx, sy, sz, sx0, sz0;
    if (!re15_emd_get_keyframe_speed(sk, kf_n, &sx,  &sy, &sz))  return;
    if (!re15_emd_get_keyframe_speed(sk, kf_p, &sx0, &sy, &sz0)) return;
    int32_t dx = (int32_t)sx - sx0, dz = (int32_t)sz - sz0;
    if (dx == 0 && dz == 0) return;
    int32_t cs = re15_cos_q12((int)a->rot_y), sn = re15_sin_q12((int)a->rot_y);
    a->x += (int32_t)(( (int64_t)cs * dx + (int64_t)sn * dz) >> 12);
    a->z += (int32_t)((-(int64_t)sn * dx + (int64_t)cs * dz) >> 12);
}

static int clip_laenge(const re15_emd_animation_t *an, int clip)
{
    if (an && clip >= 0 && clip < an->clip_count && an->clips[clip].frame_count > 0)
        return an->clips[clip].frame_count;
    return 0;
}

/* ===================================================================================================== */
/*  SPIELER                                                                                              */
/* ===================================================================================================== */
static int s_sp = -1;          /* +0x6 (DAT_800aca5a) des Sub 0x13; -1 = Spieler nicht in Sub 0x13 */
static int s_sp_vorbild = -1;  /* zuletzt posiertes Bild von Clip 0x12 (Wurzel-Delta) */

int re15_szl_schub_spieler_phase(void) { return s_sp; }

void re15_szl_schub_spieler_ende(void)
{
    if (s_sp >= 0) g_actors[RE15_ACTOR_SLOT_PLAYER].anim_flags &= (uint16_t)~RE15_SZL_REVERSE;
    s_sp = -1;
}

void re15_szl_schub_spieler_start(void)
{
    extern void re15_player_event_reach_end(void);   /* game_step_common.c: ein laufender Sub 6 wird ersetzt
                                                       * (+0x5 := 0x13 @0x80041c18) — ruft auch _ende */
    re15_player_event_reach_end();
    s_sp = 0;
    s_sp_vorbild = -1;
    szl_log("Spieler: Plc_dest Modus 0x13 (Schub-Sub @0x80052864)");
}

/* Clip setzen wie an den Original-Stellen: +0x94 = Clip, +0x95 = 0, +0x8f = 7 (Phasen 0/2/4); Richtung als
 * Port-Bit (anim_set a2 kommt im Original pro Aufruf). */
static void sp_clip(re15_actor_t *p, int clip, int rueckwaerts, int blend)
{
    p->motion = (int16_t)clip;
    p->anim_frame = 0;
    if (blend) p->anim_frac = 7;
    p->motion_init_delay = 0;
    p->anim_freeze = 0;
    if (rueckwaerts) p->anim_flags |= (uint16_t)RE15_SZL_REVERSE;
    else             p->anim_flags &= (uint16_t)~RE15_SZL_REVERSE;
}

int re15_szl_schub_spieler_pl00(void)
{
    return (s_sp >= 2 && s_sp <= 7) ? (int)g_actors[RE15_ACTOR_SLOT_PLAYER].motion : -1;
}
int re15_szl_schub_spieler_wclip(void)
{
    return (s_sp == 0 || s_sp == 1) ? RE15_SZL_CLIP_DREH : -1;
}

void re15_szl_schub_spieler_tick(void)
{
    if (s_sp < 0) return;
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_emd_animation_t *an = re15_player_pl00_anim();
    const re15_emd_skeleton_t  *sk = re15_player_pl00_skel();
    p->walk_active = 0;
    switch (s_sp) {
    case 0:                                                   /* @0x800528a4 */
        s_sp = 1;
        sp_clip(p, RE15_SZL_CLIP_DREH, 0, 1);                 /* +0x94=5 @0x800528c0, +0x8f=7 @0x800528e0 */
        /* +0x8c = 0 @0x800528f0: der Spieler-Port hat kein Skalar-Speed-Feld, der Sub ruft kein pos_advance */
        /* fall through @0x800528f0 -> [1] */
    case 1:                                                   /* @0x800528f4 */
        kurs_rasten(p);                                       /* @0x80052960-88 */
        if (kurs_gerastet(p)) s_sp = 2;                       /* @0x800529a0-b0 */
        break;
    case 2:                                                   /* @0x800529b4 */
        s_sp = 3;
        sp_clip(p, RE15_SZL_CLIP_ANSATZ, 0, 1);
        /* fall through @0x800529f0 -> [3] */
    case 3: {                                                 /* @0x800529f4 */
        int fc = clip_laenge(an, RE15_SZL_CLIP_ANSATZ);
        if (fc > 0 && (int)p->anim_frame >= fc - 1) s_sp = 4; /* +0x6 += anim_set() @0x80052a24/28 */
        if (!kurs_gerastet(p)) kurs_rasten(p);                /* @0x80052a40-70 */
        break;
    }
    case 4:                                                   /* @0x80052a74 */
        s_sp = 5;
        sp_clip(p, RE15_SZL_CLIP_SCHUB, 0, 1);
        s_sp_vorbild = -1;
        /* fall through @0x80052ab0 -> [5] */
    case 5: {                                                 /* @0x80052ab4 */
        int fc = clip_laenge(an, RE15_SZL_CLIP_SCHUB);
        if (fc <= 0) break;
        if ((int)p->anim_frame == RE15_SZL_BILD_SE)           /* @0x80052ac0-c8 */
            re15_audio_room_se_snd0(RE15_SZL_SE_SCHUB);       /* 0x02070000 @0x80052acc/d8 */
        int slot = (int)p->anim_frame;
        if (slot >= fc) { p->anim_frame = 0; slot = 0; }      /* anim_set(a2=0) = Loop */
        uint32_t w = re15_emd_get_frame_entry(an, RE15_SZL_CLIP_SCHUB, slot);
        if (w & RE15_SZL_BIT_WURZEL)                          /* @0x80052b18 */
            wurzel_schritt(p, sk, an, RE15_SZL_CLIP_SCHUB, slot, s_sp_vorbild);   /* @0x80052b24 */
        s_sp_vorbild = slot;
        if (re15_game_flag_get(5, p->walk_flag_bit)) {        /* @0x80052b38-4c */
            s_sp = 6;                                         /* @0x80052b60 */
            sp_clip(p, RE15_SZL_CLIP_ANSATZ, 1, 0);           /* +0x94=0x11 @0x80052b70, +0x95=0 @0x80052b80,
                                                               * KEIN +0x8f-Store; Phase 6 spielt a2=1 */
        }
        break;
    }
    case 6: {                                                 /* @0x80052b84 */
        int fc = clip_laenge(an, RE15_SZL_CLIP_ANSATZ);
        if (fc > 0 && (int)p->anim_frame >= fc - 1) s_sp = 7; /* +0x6 += anim_set() @0x80052bb4-bc */
        break;
    }
    default: {                                                /* [7] @0x80052bc0 */
        extern void re15_player_event_reach_begin(void);       /* game_step_common.c: Sub 6 */
        re15_audio_room_se_snd0(RE15_SZL_SE_SCHUB);           /* 0x02070000 @0x80052bc8/cc */
        s_sp = -1;
        p->anim_flags &= (uint16_t)~RE15_SZL_REVERSE;
        re15_player_event_reach_begin();                      /* +0x5 = 6 @0x80052be0, +0x6 = 0 @0x80052bf0 */
        szl_log("Spieler: Schub-Sub zu Ende -> Sub 6 (Event-Reach)");
        break;
    }
    }
}

/* ===================================================================================================== */
/*  NPC (Executor-Sub 0x13 ueber die NPC-Tabelle 0x80076ca0[19])                                          */
/* ===================================================================================================== */
int re15_szl_schub_npc_kanal(const re15_actor_t *e)
{
    if (!e || e->type < 0x40 || e->type > 0x4d) return 0;
    if (e->state != 4 || e->sub_state_1 != RE15_SZL_MODUS_SCHUB || e->walk_active) return 0;
    return (e->sub_state_2 >= 2) ? 1 : 2;      /* [2..7] Loco +0x84/+0x16c, [0/1] eigene Bank +0x170/+0x174 */
}

static void npc_clip(re15_actor_t *e, int clip, int rueckwaerts, int blend)
{
    e->motion = (int16_t)clip;
    e->anim_frame = 0;
    if (blend) e->anim_frac = 7;
    e->motion_init_delay = 1;                  /* Saat-Tick posiert Bild 0 (enemy_ai_common.c re15_npc_seed_clip) */
    if (rueckwaerts) e->anim_flags |= (uint16_t)RE15_SZL_REVERSE;
    else             e->anim_flags &= (uint16_t)~RE15_SZL_REVERSE;
}

/* Pose-dann-Vorschub wie enemy_ai_common.c re15_npc_anim (FUN_8001f314 posiert +0x95, FUN_8001f3bc erhoeht
 * danach @0x8001f610-63c, Rueckgabe 1 im Tick des letzten Bildes; +0x8f -1 je Aufruf @0x8001f5a8-b4). */
static int npc_anim(re15_actor_t *e, int fc)
{
    if (fc < 1) fc = 1;
    if (e->motion_init_delay > 0) e->motion_init_delay--;
    else e->anim_frame = (e->anim_frame + 1) % fc;
    int fertig = ((int)e->anim_frame + 1 >= fc);
    if (e->anim_frac > 0) e->anim_frac--;
    return fertig;
}

void re15_szl_schub_npc(re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const re15_emd_animation_t *own  = (b && b->own_ok)  ? &b->anim_own  : NULL;
    const re15_emd_animation_t *loco = (b && b->loco_ok) ? &b->anim_loco : NULL;
    const re15_emd_skeleton_t  *lsk  = (b && b->loco_ok) ? &b->skel_loco : NULL;
    switch (e->sub_state_2) {
    case 0:                                                   /* @0x800528a4 */
        e->sub_state_2 = 1;
        npc_clip(e, RE15_SZL_CLIP_DREH, 0, 1);
        e->crow_speed = 0;                                    /* +0x8c = 0 @0x800528f0 */
        /* fall through */
    case 1:                                                   /* @0x800528f4 */
        npc_anim(e, clip_laenge(own, RE15_SZL_CLIP_DREH));    /* anim_set(+0x170,+0x174) @0x8005294c */
        kurs_rasten(e);
        if (kurs_gerastet(e)) e->sub_state_2 = 2;
        break;
    case 2:                                                   /* @0x800529b4 */
        e->sub_state_2 = 3;
        npc_clip(e, RE15_SZL_CLIP_ANSATZ, 0, 1);
        /* fall through */
    case 3:                                                   /* @0x800529f4 */
        if (npc_anim(e, clip_laenge(loco, RE15_SZL_CLIP_ANSATZ))) e->sub_state_2 = 4;
        if (!kurs_gerastet(e)) kurs_rasten(e);
        break;
    case 4:                                                   /* @0x80052a74 */
        e->sub_state_2 = 5;
        npc_clip(e, RE15_SZL_CLIP_SCHUB, 0, 1);
        e->sub_state_3 = 0xFF;                                /* Port-Merker: kein Vorbild fuer das Wurzel-Delta */
        /* fall through */
    case 5: {                                                 /* @0x80052ab4 */
        int fc = clip_laenge(loco, RE15_SZL_CLIP_SCHUB);
        int bild_vorher = (e->sub_state_3 == 0xFF) ? -1 : (int)e->sub_state_3;
        npc_anim(e, fc);                                      /* anim_set Loop @0x80052af4 */
        int slot = (int)e->anim_frame;
        if (slot == RE15_SZL_BILD_SE) re15_audio_room_se_snd0(RE15_SZL_SE_SCHUB);   /* @0x80052ac0-d8 */
        if (loco && (re15_emd_get_frame_entry(loco, RE15_SZL_CLIP_SCHUB, slot) & RE15_SZL_BIT_WURZEL))
            wurzel_schritt(e, lsk, loco, RE15_SZL_CLIP_SCHUB, slot, bild_vorher);  /* @0x80052b24 */
        e->sub_state_3 = (uint8_t)slot;
        if (re15_game_flag_get(5, e->walk_flag_bit)) {        /* @0x80052b38-4c */
            e->sub_state_2 = 6;
            npc_clip(e, RE15_SZL_CLIP_ANSATZ, 1, 0);          /* @0x80052b60-80, kein +0x8f-Store */
            e->sub_state_3 = 0;
        }
        break;
    }
    case 6:                                                   /* @0x80052b84, a2 = 1 */
        if (npc_anim(e, clip_laenge(loco, RE15_SZL_CLIP_ANSATZ))) e->sub_state_2 = 7;
        break;
    default:                                                  /* [7] @0x80052bc0 */
        re15_audio_room_se_snd0(RE15_SZL_SE_SCHUB);           /* @0x80052bcc */
        e->anim_flags &= (uint16_t)~RE15_SZL_REVERSE;
        e->sub_state_1 = 6;                                   /* +0x5 = 6 @0x80052be0 -> Event-Reach */
        e->sub_state_2 = 0;                                   /* +0x6 = 0 @0x80052bf0 */
        e->sub_state_3 = 0;
        break;
    }
}

/* ===================================================================================================== */
/*  Objekt-Pass: Bit 0x200                                                                               */
/* ===================================================================================================== */
int re15_szl_objekt_schub(int p)
{
    if (p < 0 || p >= (int)g_scd.prop_count || !g_scd.props[p].active) return 0;
    if (!(g_scd.props[p].flags & RE15_SZL_OBJ_SCHUB)) return 0;                    /* @0x8002bec0/c4 */
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t nx = g_scd.props[p].x, nz = g_scd.props[p].z;
    (void)re15_collision_push_prop(p, pl->x, pl->z, &nx, &nz);                      /* @0x8002bed4, a2=1 */
    g_scd.props[p].x = nx;                                   /* @0x8002bedc/e0 gesichert, @0x8002c0b8 bleibt */
    g_scd.props[p].z = nz;
    return 1;                                                /* @0x8002bee4 `j 0x8002c0b8`: Zaehler +0x8C,
                                                              * Kontaktbit 0x2000 und Typ-4-Schub entfallen */
}

/* ===================================================================================================== */
/*  Punkt 4: Irons' Kopf nach dem Tod                                                                    */
/* ===================================================================================================== */
/* Die Wurzel 0x8011d140 (Typ 0x45) ruft die Kopf-FSM unbedingt (@0x8011d278 `jal 0x80037358`); deren
 * Standardziel ist der Spieler (Flags 0 = Entity-Tracking, INIT @0x8011c768). Irons' Tod ist Port-Inhalt
 * (Runde 35, NUTZER-VORGABE); "nicht mehr hinter Leon her" hat im Original genau eine Form: Flags 0x12 —
 * das schreibt Sce_em_set fuer einen Aktor ohne Blickziel (`ori v0,zero,0x12` @0x80042614 / `sb v0,440(s0)`
 * @0x80042618) und Plc_neck Modus 0 (caseD @0x80041ed8, Satz ROOM11B0 @0x01646). Die Kopf-FSM laesst dann den
 * Kopf auf die Clip-Pose zurueckgleiten (skeleton_common.c Zweig 0x02/0x10). Gilt, sobald Irons tot daliegt:
 * (9,73) gesetzt UND die Todes-Pose (Motion-Sub 0, Clip 2 = RE15_IT_IRONS_CLIP_TOT) steht — waehrend der
 * Sterbeszene vor dem Armfall (Clips 4/3/6/5) bleibt der Blick auf Leon. */
static void irons_kopf_tick(void)
{
    if ((uint16_t)g_current_room_id != RE15_IT_RAUM_1150) return;
    re15_actor_t *a = &g_actors[RE15_IT_IRONS_SLOT];
    if (!a->active || a->type != RE15_IT_IRONS_TYP) return;
    if (!re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_GESEHEN)) return;
    if (!(a->state == 4 && a->sub_state_1 == 0 && a->motion == RE15_IT_IRONS_CLIP_TOT)) return;
    if (a->neck_flags == RE15_SZL_HALS_FREI) return;
    a->neck_flags = RE15_SZL_HALS_FREI;          /* @0x80042618 */
    a->neck_sweep = 0;
    szl_log("Irons tot: Kopf-FSM +0x1b8 = 0x12 (kein Blickziel mehr)");
}

void re15_szl_tick(void)
{
    irons_kopf_tick();
}
