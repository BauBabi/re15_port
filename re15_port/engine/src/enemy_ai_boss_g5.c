/* ============================================================================================
 * G5-ENDKAMPF-BIRKIN (Typ 0x36, ROOM5090/5091) — BYTE-TRUE PORT DER RE2-G5-KI.
 * --------------------------------------------------------------------------------------------
 * Runde 6 (Nutzer 2026-09-12: "Bei Birkin fehlt immer noch Alles - Animationen, Tentakel
 * die ihn quasi nach vorne ziehen und schlagen, Bewegung - einfach alles von RE2").
 *
 * QUELLE: analysis/befunde_runde6_2026-09-12/birkin-g5-ki.md — Vollzensus des G5-Overlays
 * (CDEMD0.EMS @0x006C0000, 23476 B, gelinkt @0x80100000). Jede Konstante hier traegt ihre
 * @0x-Adresse aus dem Dossier. Dispatch-Architektur des Originals:
 *   Per-Frame-Main 0x801000BC -> Routine-Tabelle @0x801055CC (+0x04):
 *     [0] Ctor, [1] AKTIV (sub0 IDLE / sub1 ZUG / sub2 INTRO / sub3 RUECKZUG / sub4 DEVOUR),
 *     [2] TREFFER 0x801025BC, [3] TOD [T16..T21], [4] Script-Hook.
 *   Parallel die BLOB-MASCHINE (+0x218, Tabelle @0x80105ABC): Puls/Biss/Devour/Tod.
 *
 * EINHAENGUNG: eigener Tick nach dem Gator-Muster — aktiv NUR fuer 0x36 in ROOM5090/5091;
 * ROOM3070/Typ 0x30 bleibt unangetastet auf re15_birkin_ai_tick (enemy_ai_common.c).
 * e->motion traegt DIREKT die RE2-Clip-Indizes 0..10 der EM036-Bank (keine BKMAP mehr).
 *
 * KOORDINATEN-ANKER (dokumentierte Port-Entscheidung, Dossier §7): ROOM5090-Streifen C ist
 * der gekuerzte room7040-Korridor; die absoluten RE2-X-Schwellen laufen auf der u-Achse:
 *   u = 8000 - (x - 1200)      (X0 = sub04-Pos_set-X = 1200 @ROOM5090.RDT 0x12FE;
 *                               s = -1 Richtung Spieler-Kampfmarke 300 @0x1320)
 * Alle RE2-Schwellen woertlich auf u: Rueckzugslinie 4000 (@0x80101934), Zug-/Lunge-Kappe
 * 12000 (@0x80100fd0/@0x80104074), Devour u>=9001 (@0x801008a0), sub0-Entscheidungen
 * 7000/8000/10000/10001/11001 (§2). Yaw: einmalig beim Kampfstart auf die Spielermarke,
 * danach KONSTANT (RE2: einziger Kampf-Schreiber @0x8010044c, Yaw bleibt 0).
 *
 * TENTAKEL: RE2 kommandiert 4 eigene 0x37-Entities (Sender 0x80104E9C/0x80104E5C,
 *   Muster @0x80105674). RE1.5-ROOM5090 spawnt keine 0x37 -> die Kommandos laufen hier
 *   als No-Op-Hook (re15_g5_tentakel_cmd), die "+0x228-Maske = keine Kandidaten"-Zweige
 *   sind byte-true erreichbar (@0x80100c3c-44). EM037-Port = eigener spaeterer Schritt.
 * BLOB-MORPH: der dir[0]-Vertex-Morph ist (noch) nicht gebaut — die GEWICHTE werden hier
 *   trotzdem EXAKT simuliert (alle Schwellen byte-true; die Optik folgt, sobald der Morph
 *   im Renderer ankommt; g5-optik §3.4).
 * DEVOUR-VICTIM-ANIM: EDD-Paar 3 laeuft auf dem 15-Bone-PL0-Rig — DEFERRED (Dossier §7);
 *   der Kill traegt Schaden 500 + Grab-Latch + Yaw-Snap.
 * ============================================================================================ */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re15_skeleton.h"

/* re15_enemy_player_dist / re15_ai_arc_test / re15_engine_rand8 /
 * re15_player_equipped_weapon kommen aus re15_damage.h. Spieler-Schaden laeuft
 * als direkter HP-Abzug (die RE1.5-Schadenstabelle kennt die rohen RE2-Betraege
 * 40/500 nicht; Muster = Birkin-/Gator-Tick). */

/* SE-Hook (Plattform registriert die ENEMSE-Wiedergabe; sound-id 0x27 -> Paar-Zeile 25
 * {0x27,0x00} @PSX.EXE-Datei 0x97C32, flag2000=0 — Mechanik wie der Gator/Bank 17). */
static void (*s_g5_se_fn)(int, int) = 0;
static void (*s_g5_bank_fn)(int) = 0;
void re15_g5_audio_hook(void (*se)(int, int), void (*bank)(int))
{
    s_g5_se_fn = se; s_g5_bank_fn = bank;
    if (bank) bank(25);
}
static void g5_se(int id) { if (s_g5_se_fn) s_g5_se_fn(id, 0); }

/* Tentakel-Kommando-Hook (No-Op ohne 0x37-Entities; Sender-Ziele @0x80104E9C). */
static void g5_tentakel_cmd(int idx, uint32_t wort) { (void)idx; (void)wort; }
static void g5_tentakel_broadcast(uint32_t wort) { (void)wort; }

/* ---- Modul-Zustand (ein Boss je Raum) ------------------------------------------------- */
typedef struct {
    int      aktiv;
    uint8_t  routine;        /* +0x04: 1 AKTIV / 2 TREFFER-Rueckkehr laeuft implizit /
                              * 3 TOD (r0=2 wird inline behandelt: Flinch-Akkumulator) */
    uint8_t  sub;            /* +0x05: 0 idle / 1 zug / 2 intro / 3 rueckzug / 4 devour */
    uint8_t  ph;             /* +0x06 Phase */
    int32_t  timer;          /* +0x158/+0x15A je Zustand */
    int32_t  rampe;          /* sub3-Dreiecksrampe (+0x158) */
    int32_t  flinch_akku;    /* +0x222 Flinch-Akkumulator (Schwelle 15, Zerfall 1/15F) */
    int32_t  flinch_takt;
    uint32_t busy;           /* +0x226 Bit 1 (busy) / Bit 2 (HP<=0) / Bit 8 (Tracking aus) */
    int32_t  gewicht[4];     /* Blob-Morph-Gewichte (part2+0x88+12..24) — exakte Simulation */
    uint8_t  blob;           /* +0x218 Blob-Zustand: 0 Puls / 1 Biss / 3 Vorkampf /
                              * 4 Devour / 5 Tod */
    uint8_t  blob_ph;        /* +0x219 */
    int32_t  blob_speed;     /* Biss-Speed */
    int32_t  blob_timer;
    int32_t  hitbox_b;       /* +0xB4 Biss-Hitbox-Breite 2200..3700 */
    int32_t  biss_cd;        /* +0x229 Cooldown */
    int32_t  blut_serie;     /* +0x21B */
    int32_t  tod_timer;
    int32_t  absink_t;       /* [T20]-Rampe t (Gewicht[2]) */
    uint8_t  gestartet;      /* Kampfstart gesehen (grid==0x13) */
    int32_t  clip_prev;
} g5_state_t;

static g5_state_t s_g5;
static int s_g5_slot = -1;

/* u-Achse (Dossier §7): u = 8000 - (x - 1200). */
static int32_t g5_u(const re15_actor_t *e) { return 8000 - ((int32_t)e->x - 1200); }
static void    g5_setze_u(re15_actor_t *e, int32_t u) { e->x = 1200 - (u - 8000); }

/* Clip setzen (Clip-Wort +0x14C; +0x14E-Blendstaerke traegt der Port als anim_frac-Naeherung:
 * 0x1F = harter Schnitt, 0x07 = weicher Crossfade — FUN_80029614 IR0-Lerp a3*flag/4096). */
static void g5_clip(re15_actor_t *e, int clip, int weich)
{
    if ((int)e->motion != clip) {
        e->motion = (int16_t)clip;
        e->anim_frame = 0;
        e->anim_frac = weich ? 7 : 0;
    }
}

/* Clip-Laenge der EM036-Bank. */
static int g5_clip_len(re15_actor_t *e)
{
    extern int re15_actor_clip_len(const re15_actor_t *a);
    int fc = re15_actor_clip_len(e);
    return fc > 0 ? fc : 1;
}

/* Anim-Schritt: Rueckgabe 1 am Clip-Ende (0x8002959C-Semantik). */
static int g5_anim(re15_actor_t *e)
{
    int fc = g5_clip_len(e);
    if ((int)e->anim_frame + 1 >= fc) { e->anim_frame = 0; return 1; }
    e->anim_frame++;
    return 0;
}

/* Root-Motion des laufenden Clips (0x80015E7C + 0x800152C8): akkumulierte kf-Spur
 * (+6/+8/+10), DIFFERENZIERT, direkt auf die u-ACHSE gebucht — RE2 faehrt den
 * ganzen Kampf mit Yaw 0 und vorwaerts = +X (@0x8010044c einziger Schreiber);
 * die u-Achse IST diese RE2-X-Achse im 5090-Korridor (Dossier 7). Der seitliche
 * sz-Anteil geht auf Welt-z. skala_q12: 4096 = x1, 6144 = x1,5 (sub1-Zug
 * @0x80100fa8-c0). 0x8000-Flag-Skip wie ueberall (FUN_80015db0 @0x80015e18-38). */
static int32_t s_g5_spur_prev_sx, s_g5_spur_prev_sz;
static void g5_root_motion(re15_actor_t *e, int neu_verankert, int32_t skala_q12,
                           int32_t kappe_u_max)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    int16_t sx = 0, sy = 0, sz = 0;
    if (!b || !b->ok) return;
    {
        const re15_emd_animation_t *an = &b->anim;
        int clip = (int)e->motion;
        int fc, fi, fend, kf;
        if (clip < 0 || clip >= an->clip_count) return;
        fc = an->clips[clip].frame_count;
        if (fc <= 0) return;
        fi   = an->clips[clip].first_frame + (int)(e->anim_frame % (uint32_t)fc);
        fend = an->clips[clip].first_frame + fc - 1;
        while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
        kf = (int)(an->frames[fi] & 0xFFFu);
        if (!re15_emd_get_keyframe_speed(&b->skel, kf, &sx, &sy, &sz)) return;
    }
    if (neu_verankert) { s_g5_spur_prev_sx = sx; s_g5_spur_prev_sz = sz; return; }
    {
        int32_t dsx = (int32_t)sx - s_g5_spur_prev_sx;
        int32_t dsz = (int32_t)sz - s_g5_spur_prev_sz;
        s_g5_spur_prev_sx = sx; s_g5_spur_prev_sz = sz;
        if (skala_q12 != 4096) {
            dsx = (int32_t)(((int64_t)dsx * skala_q12) >> 12);
            dsz = (int32_t)(((int64_t)dsz * skala_q12) >> 12);
        }
        {
            int32_t u = g5_u(e) + dsx;                 /* vorwaerts = +u (RE2-X) */
            if (kappe_u_max > 0 && u > kappe_u_max) u = kappe_u_max;
            g5_setze_u(e, u);
            e->z += dsz;
        }
    }
}

/* ---- BLOB-MASCHINE (+0x218, Tabelle @0x80105ABC) --------------------------------------- */
static void g5_blob_tick(re15_actor_t *e, re15_actor_t *pl)
{
    g5_state_t *g = &s_g5;
    int32_t dist = re15_enemy_player_dist(e, pl);
    switch (g->blob) {
    case 0: {   /* KAMPF-PULS + Trigger (0x80103C18): die Masse ATMET dauerhaft. */
        if (g->blob_ph == 0) { g->blob_timer = 180 + (re15_engine_rand8() & 0x1F);
                               g->blob_speed = 6; g->blob_ph = 1; }
        else if (g->blob_ph == 1) {
            g->gewicht[0] += g->blob_speed;
            if (g->blob_speed < 162) g->blob_speed += 6;
            if (g->gewicht[0] > 8000) g->blob_ph = 2;
        } else {
            g->gewicht[0] -= g->blob_speed;
            if (g->blob_speed < 192) g->blob_speed += 6;
            if (g->gewicht[0] < -548) { g->blob_ph = 0; }
        }
        g->gewicht[3] /= 2;
        /* Trigger jeden Frame (@0x80103e08-ec): Biss / zweiter Devour-Pfad. */
        if (dist < 6000 && g->biss_cd == 0 &&
            re15_ai_arc_test(e, pl->x, pl->z, 128) && !(g->busy & 4u)) {
            if (pl->hp >= 0 && g5_u(e) >= 9001) {
                /* DEVOUR-Pfad 2 (@0x80103ea0-ec4): 500 Schaden = sicherer Kill. */
                pl->hp = -1; pl->hit_react |= 1;
                g->sub = 4; g->ph = 0; g->routine = 1;
                g->blob = 4; g->blob_ph = 0;
                return;
            }
            g->blob = 1; g->blob_ph = 0;               /* BISS @0x80103e6c */
        }
        break;
    }
    case 1: {   /* MASSEN-BISS [T22..T26] — Lunge + 40 Schaden @0x8010416c. */
        switch (g->blob_ph) {
        case 0: g->blob_ph = 1; g->blob_speed = 0; break;
        case 1:                                        /* AUSHOLEN @0x80103F58 */
            g->gewicht[0] -= g->blob_speed;
            g->blob_speed += 32;
            g->gewicht[3] += g->blob_speed / 2;
            if (g->gewicht[0] < -4048) g->blob_ph = 2;
            break;
        case 2: {                                      /* ZUSCHNAPPEN @0x80103FE0 */
            g->gewicht[0] += g->blob_speed;
            if (g->gewicht[0] > 0) g->gewicht[0] = 0;
            g->gewicht[3] += g->blob_speed;
            if (g->gewicht[3] >= 5573) {
                g->blob_ph = 3; g5_se(6);              /* @0x80104040-5c */
                g->blob_timer = 15;                    /* +0x21E */
            }
            /* Lunge: u += Speed>>3, Kappe 12000 (@0x80104060-94). */
            {   int32_t du = g->blob_speed >> 3;
                int32_t u = g5_u(e) + du;
                if (u > 12000) u = 12000;
                g5_setze_u(e, u);
            }
            if (g->hitbox_b < 3700) g->hitbox_b += 500;   /* @0x801040b8-e0 */
            /* Treffer: Punkt u+900 vorgehalten, Radius 1500 (@0x801040ec-f4). */
            {   int32_t px = 1200 - ((g5_u(e) + 900) - 8000);
                int64_t dx = (int64_t)pl->x - px, dz = (int64_t)pl->z - e->z;
                if (dx*dx + dz*dz < (int64_t)1500*1500 && pl->hit_react == 0) {
                    pl->hp = (int16_t)(pl->hp - 40);   /* 40 @0x8010416c-70 */
                    if (pl->hp < 0) pl->hp = -1;
                    g5_se(14);
                    pl->hit_react |= 1;
                    g->blut_serie = 10;                /* +0x21B @0x801041a8 */
                }
            }
            g->blob_speed += 256;                      /* @0x801041d8 */
            break;
        }
        case 3:                                        /* Nachwackeln (15 T) */
            g->gewicht[3] += ((g->blob_timer & 1) ? 64 : -192);
            if (--g->blob_timer <= 0) g->blob_ph = 4;
            break;
        default:                                       /* Rueckzug der Masse */
            g->gewicht[3] -= 64;
            if (g->hitbox_b > 2200) g->hitbox_b -= 15;
            if (g->gewicht[3] < 0) {
                g->blob = 0; g->blob_ph = 0;
                g->biss_cd = 5;                        /* +0x229=5 @0x801042a0-ac */
            }
            break;
        }
        break;
    }
    case 3: {   /* VORKAMPF-Puls (0x801044FC) — vereinfachte Puls-Variante mit den
                 * Original-Schwellen 8501/-1048; Konvulsions-/Wellen-Zyklen folgen
                 * mit dem Morph-Renderer (rein optisch, Dossier §4). */
        if (g->blob_ph == 0) { g->blob_speed = 6; g->blob_ph = 1; }
        else if (g->blob_ph == 1) {
            g->gewicht[0] += g->blob_speed;
            if (g->gewicht[0] > 8501) g->blob_ph = 2;
        } else {
            g->gewicht[0] -= g->blob_speed;
            if (g->gewicht[0] < -1048) g->blob_ph = 1;
        }
        break;
    }
    case 4: {   /* DEVOUR-Blob (0x80104848): Kompress, dann Kauen mit SE 14/6. */
        if (g->blob_ph == 0) { g->blob_speed = 1024; g->blob_ph = 1; }
        else if (g->blob_ph == 1) {
            g->gewicht[0] -= g->blob_speed; g->blob_speed += 256;
            if (g->gewicht[0] < -4048) g->blob_ph = 2;
        } else if (g->blob_ph == 2) {
            g->gewicht[0] += 512;
            if (g->gewicht[0] > 10000) { g->blob_ph = 3; g->blob_timer = 0; }
        } else {
            if ((g->blob_timer++ & 15) == 0) g5_se((re15_engine_rand8() & 1) ? 14 : 6);
        }
        break;
    }
    case 5: {   /* TODES-Blob (0x80104B94): Deflate, Zuckungen, Aufblaehen, Kollaps. */
        if (g->blob_ph == 0) {
            g->gewicht[0] -= 128;
            if (g->gewicht[0] < -4048) { g->blob_ph = 1; g->blob_timer = 300; }
        } else if (g->blob_ph == 1) {
            g->gewicht[0] += (int32_t)(re15_engine_rand8() & 0x3F) - 32;
            if (g->gewicht[0] < -5000) g->gewicht[0] = -5000;
            if (g->gewicht[0] > 8000)  g->gewicht[0] = 8000;
            if (--g->blob_timer <= 0) g->blob_ph = 2;
        } else if (g->blob_ph == 2) {
            g->gewicht[0] += 64;
            if (g->gewicht[0] > 12000) g->blob_ph = 3;
        } else if (g->blob_ph == 3) {
            g->gewicht[0] -= 128;
            if (g->gewicht[0] < 0) g->blob_ph = 5;     /* Ende */
        }
        break;
    }
    default: break;
    }
    if (g->biss_cd > 0 && g->blob == 0 && g->blob_ph == 0) g->biss_cd--;
}

/* ---- INTRO-Choreo (sub2, [T0..T14], Dossier §3) ---------------------------------------- */
static void g5_intro_tick(re15_actor_t *e)
{
    g5_state_t *g = &s_g5;
    switch (g->ph) {
    case 0:                                            /* [T0/T-Armierung]: Clip 1, SE 10 */
        memset(g->gewicht, 0, sizeof g->gewicht);
        g5_clip(e, 1, 0); g5_se(10);
        g->ph = 1; g->timer = 0;
        g->blob = 3; g->blob_ph = 0;                   /* Vorkampf-Puls an */
        break;
    case 1:                                            /* [T1] 90 T Tentakel wecken */
        g->timer++;
        if (g->timer == 10 || g->timer == 30 || g->timer == 40) g5_se(9);
        if (g->timer >= 90) { g->ph = 2; g5_se(11); g5_root_motion(e, 1, 4096, 0); }
        break;
    case 2:                                            /* [T2] Clip 1: +7014 Root-Spur */
        g5_root_motion(e, 0, 4096, 0);
        if (g5_anim(e)) g->ph = 3;
        break;
    case 3: g5_clip(e, 3, 0); g5_se(9); g->ph = 4; g->timer = 0; break;   /* [T3] */
    case 4:                                            /* [T4] 90 T */
        g5_anim(e); g->timer++;
        if (g->timer == 30) g5_se(0);
        if (g->timer >= 90) { g->ph = 5; g->timer = 0; g5_se(10); }
        break;
    case 5:                                            /* [T5] 110 T, dann Clip 4 */
        g5_anim(e); g->timer++;
        if (g->timer == 20 || g->timer == 40 || g->timer == 45 || g->timer == 56) g5_se(0);
        if (g->timer >= 110) { g5_clip(e, 4, 0); g->ph = 6; g5_se(11);
                               g5_root_motion(e, 1, 4096, 0); }
        break;
    case 6:                                            /* [T6] Clip 4: +3946 Root-Spur */
        g5_root_motion(e, 0, 4096, 0);
        if (g5_anim(e)) g->ph = 7;
        break;
    case 7: g5_clip(e, 2, 0); g->gewicht[1] = 0; g->ph = 8; g5_se(10); break;  /* [T7] */
    case 8: if (g5_anim(e)) g->ph = 9; break;          /* [T8] Clip 2, 150 F */
    case 9: g5_clip(e, 0, 0); g->ph = 10; g->timer = 0; g5_se(9); break;       /* [T9] */
    case 10:                                           /* [T10] 110 T */
        g5_anim(e); g->timer++;
        if (g->timer >= 110) g->ph = 11;
        break;
    case 11: g5_clip(e, 2, 0); g->ph = 12; g5_se(10); break;                   /* [T11] */
    case 12: if (g5_anim(e)) g->ph = 13; break;        /* [T12] Clip 2 */
    case 13:                                           /* [T13]: Puls an, Timer 60 */
        g5_clip(e, 0, 0); g->ph = 14; g->timer = 60; g5_se(9);
        g->blob = 0; g->blob_ph = 0;
        g5_tentakel_broadcast(0x601);
        break;
    default:                                           /* [T14] -> Kampf (sub1) */
        g5_anim(e);
        if (--g->timer <= 0) { g->sub = 1; g->ph = 0; g5_se(10); }
        break;
    }
}

/* ---- TODES-SEQUENZ ([T16..T21], Dossier §2 r0=3) ---------------------------------------- */
static void g5_tod_tick(re15_actor_t *e)
{
    g5_state_t *g = &s_g5;
    switch (g->ph) {
    case 0:                                            /* [T16]: Clip 6, Blob->Tod */
        e->hp = -1;                                    /* @0x80103060 */
        g5_clip(e, 6, 0); g5_se(10);
        g->blob = 5; g->blob_ph = 0;                   /* +0x218=5 @0x801030e4 */
        g5_se(13);
        g->ph = 1; g->tod_timer = 0;
        break;
    case 1:                                            /* [T17]: Clip 6 -> Clip 7 + 250 T */
        if (g5_anim(e)) { g5_clip(e, 7, 0); g->ph = 2; g->tod_timer = 250; }
        break;
    case 2:                                            /* [T18]: Zucken 6/8, Timer 250 */
        if (g5_anim(e))
            g5_clip(e, ((re15_engine_rand8() & 3) == 0) ? 8 : 6, 0);
        if (--g->tod_timer <= 0) {
            g->ph = 3;
            g->busy |= 8u;                             /* Tracking/Kollision aus */
            g5_clip(e, 10, 0);                         /* Clip-Wort 0x1F000A @0x801034c8 */
        }
        break;
    case 3:                                            /* [T19]: Clip 10 halbe Rate */
        g->tod_timer++;
        if ((g->tod_timer & 1) == 0 && g5_anim(e)) { g->ph = 4; g->absink_t = 0; }
        break;
    case 4: {                                          /* [T20]: ABSINK-RAMPE @0x80103668 */
        int32_t t = g->absink_t;
        int32_t schritt = (t < 1025) ? 16 : (t < 2049) ? 12 : (t < 3073) ? 8 : 4;
        t += schritt;
        g->absink_t = t;
        g->gewicht[2] = t;
        e->y = (int32_t)(((int64_t)t * 2950) >> 12);   /* y=(t*2950)>>12 @0x801037c4-f4:
                                                        * der Kriecher versinkt in die Masse */
        if (t >= 4097) g->ph = 5;
        break;
    }
    default: break;                                    /* [T21] Kadaver-Ruhe */
    }
}

/* ---- HAUPT-TICK ------------------------------------------------------------------------- */
int re15_g5_boss_active(const re15_actor_t *e)
{
    return e && e->type == 0x36 &&
           ((g_current_room_id & 0xFFFEu) == 0x5090u);
}

void re15_g5_boss_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g5_state_t *g = &s_g5;
    int32_t dist;

    if (s_g5_slot != slot || !g->aktiv) {
        memset(g, 0, sizeof *g);
        g->aktiv = 1; g->routine = 1; g->sub = 2; g->ph = 0;   /* Ctor: +0x05=2 @0x8010076C */
        g->hitbox_b = 2200;
        s_g5_slot = slot;
        e->hp = 600;                                   /* @0x801003fc-418 (easy 400: das
                                                        * RE1.5-Spiel kennt kein easy-Bit) */
    }

    /* KAMPFSTART-Armierung (+0x1D4-Bit-0-Analogon): sub04-Member_set(0x0c,0x13). Davor
     * PARK auf der RE2-Parkposition — [T0] unarmiert tut NICHTS (@0x801011d0-dc). */
    if (!g->gestartet) {
        if (e->grid_id == 0x13) {
            g->gestartet = 1;
            /* Yaw EINMALIG auf die Spielermarke (Kampfkorridor -X); danach konstant. */
            e->rot_y = (int16_t)((re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
            /* Intro-Startposition: RE2 [T0] parkt u=-9000 (X=-9000 im room7040-Raum,
             * @0x801011d0-dc); die u-Formel klemmt auf den begehbaren Streifen. */
            {   int32_t x = 1200 - (-9000 - 8000);
                if (x > 18200) x = 18200;
                e->x = x; e->z = -23350;
            }
        } else {
            e->x = -32000; e->z = -32000;              /* wie bisher: off-world parken */
            e->motion = 0; e->anim_frame = 0;
            return;
        }
    }

    dist = re15_enemy_player_dist(e, pl);
    e->dog_dist = (int16_t)dist;

    /* Flinch-Zerfall (Main-Tick §1.2): -1 je 15 Frames. */
    if (g->flinch_akku > 0 && ++g->flinch_takt >= 15) { g->flinch_takt = 0; g->flinch_akku--; }

    /* Blut-Serie des Biss-Treffers (+0x21B). */
    if (g->blut_serie > 0) g->blut_serie--;

    /* TOD-Trigger ((+0x226&6)==4-Analogon): HP <= 0 und nicht busy. */
    if (e->hp <= 0 && g->routine != 3) {
        if (g->busy & 2u) { g->busy |= 4u; }           /* busy: Zug zu Ende spielen */
        else { g->routine = 3; g->ph = 0; }
    }
    if ((g->busy & 4u) && !(g->busy & 2u) && g->routine != 3) { g->routine = 3; g->ph = 0; }

    if (g->routine == 3) {                             /* TOD */
        g5_tod_tick(e);
        g5_blob_tick(e, pl);
        return;
    }

    /* TREFFER-Flinch (r0=2, inline): Akkumulator + Tabelle @0x801056B4; die Waffen-
     * Zuordnung ist die dokumentierte Port-Entscheidung aus §7 (Handfeuer 5,
     * Schrot/Bogen 14, Magnum/Granate 20, Messer 1); Schwelle 15 -> STAGGER Clip 8. */
    if (e->hit_react & 1u) {
        e->hit_react &= (uint8_t)~1u;
        if (!(g->busy & 2u)) {
            int w = re15_player_equipped_weapon();
            int add = (w == 1) ? 1 : (w == 5 || w == 6 || w == 9 || w == 15 || w == 18) ? 20
                    : (w == 8 || w == 13 || w == 12) ? 14 : 5;
            g->flinch_akku += add;
            if (g->flinch_akku >= 15) {                /* STAGGER 0x80102AD0 */
                g->flinch_akku = 0;
                g->sub = 0xF;                          /* Flinch-Pseudo-Sub */
                g->ph = 0;
            }
        }
    }

    if (g->sub == 0xF) {                               /* FLINCH: Clip 8 (50 F), SE 12/9 */
        if (g->ph == 0) { g5_clip(e, 8, 0); g5_se(12); g->ph = 1;
                          g5_root_motion(e, 1, 4096, 0); }
        else {
            if (e->anim_frame == 15) g5_se(9);
            g5_root_motion(e, 0, 4096, 0);
            if (g5_anim(e)) { g->sub = 0; g->ph = 0; }
        }
        g5_blob_tick(e, pl);
        return;
    }

    switch (g->sub) {
    case 2:                                            /* INTRO */
        g5_intro_tick(e);
        break;

    case 0: {                                          /* IDLE/ENTSCHEIDEN (§2 sub0) */
        int32_t u = g5_u(e);
        /* Devour-Check aus dem Stand (Phase A @0x801008a0-44). */
        if (re15_ai_arc_test(e, pl->x, pl->z, 128) && u >= 9001 && dist < 6000 &&
            pl->hit_react == 0 && !(g->busy & 4u) && pl->hp >= 0) {
            pl->hp = -1; pl->hit_react |= 1;           /* 500 @0x80100904 = Kill */
            g->sub = 4; g->ph = 0;
            g->blob = 4; g->blob_ph = 0;
            break;
        }
        if (g->ph == 0) {
            g5_clip(e, 0, 0);                          /* Clip-Wort 0x1F0000 */
            g->timer = 120 + (re15_engine_rand8() & 0x1F);
            g->ph = 1;
            /* Tentakel-Scan: ohne 0x37-Entities sind ALLE 4 "frei" -> sofort sub1
             * (@0x80100c44/0x80100ce0, byte-true erreichbarer Zweig). */
            if (u >= 8000 || dist >= 11001) { g->sub = 1; g->ph = 0; break; }
        }
        if (g5_anim(e) && (re15_engine_rand8() & 3) == 0) g5_se(9);   /* Grollen */
        if (--g->timer <= 0) {
            /* Entscheidung @0x80100c44-ce4 (Reihenfolge = letzte trifft): */
            uint8_t ziel = 3;                                        /* default Rueckzug */
            if ((re15_engine_rand8() & 3) != 0 && u < 10000) ziel = 1;
            if (dist < 10001) ziel = 1;
            if (u >= 9001 && (re15_engine_rand8() & 1) == 0) ziel = 3;
            if (u < 7000 && (re15_engine_rand8() & 1) == 0) ziel = 1;
            g->sub = ziel; g->ph = 0;
        }
        break;
    }

    case 1: {                                          /* TENTAKEL-ZUG (§2 sub1) */
        if (g->ph == 0) {
            g5_clip(e, 5, 0);                          /* Clip-Wort 0x1F0005 @0x80100dd8 */
            g5_tentakel_broadcast(0xB01);
            g5_se(10);                                 /* @0x80100dec */
            g->busy |= 2u;
            g->ph = 1;
            g5_root_motion(e, 1, 6144, 12000);         /* Anker; x1,5 ab jetzt */
        } else if (g->ph == 1) {
            uint32_t f = e->anim_frame;
            if (f == 55) g5_se(9);                     /* @0x80100f18 */
            if (f == 90) g5_se(11);                    /* @0x80100f80 */
            /* Root-Spur Clip 5 x1,5 = +4050 ueber 150 F, Kappe u<12000
             * (@0x80100fa8-c0 / @0x80100fd0-e4). */
            g5_root_motion(e, 0, 6144, 12000);
            if (g5_anim(e)) g->ph = 2;
        } else if (g->ph == 2) {
            g5_clip(e, 2, 0); g5_se(10); g->ph = 3;    /* @0x80101000-0c */
        } else {
            uint32_t f = e->anim_frame;
            if (f == 25) g5_se(9);                     /* @0x80101070-98 */
            g5_anim(e);
            if (f >= 85) {                             /* Wort=1 -> sub0 @0x8010103c-44 */
                g->sub = 0; g->ph = 0;
                g->busy &= ~2u;
            }
        }
        break;
    }

    case 3: {                                          /* RUECKZUG (§2 sub3) */
        if (g->ph == 0) {
            g5_clip(e, 2, 0); g5_se(11);
            g->ph = 1; g->rampe = 0; g->timer = 0;     /* +0x158=0, +0x16A=0 */
            g->busy |= 2u;
        } else {
            /* Dreiecks-Rampe: +1/Tick bis 50 (SE 11), dann -1/Tick bis 0 (SE 10)
             * = exakt 2500 Einheiten rueckwaerts, Kappe u>4000 (@0x801018a8-4c). */
            if (g->timer == 0) {
                g->rampe++;
                if (g->rampe >= 50) { g->timer = 1; g5_se(11); }     /* @0x801018e4 */
            } else {
                g->rampe--;
                if (g->rampe <= 0) { g->rampe = 0; g5_se(10); }      /* @0x80101920 */
            }
            {   int32_t u = g5_u(e) - g->rampe;        /* if (X>4000) X -= Zaehler */
                if (u < 4000) u = 4000;
                if (g5_u(e) > 4000) g5_setze_u(e, u);
            }
            if (g5_anim(e)) {                          /* Clip-2-Ende -> sub0 */
                g->sub = 0; g->ph = 0;
                g->busy &= ~2u;
            }
        }
        break;
    }

    case 4: {                                          /* DEVOUR (§2 sub4) */
        if (g->ph == 0) {
            g5_clip(e, 9, 1);                          /* Clip-Wort 0x70009 (weich) */
            g5_se(6);
            /* Grab-Latch: Spieler einfrieren, Yaw = Boss-Yaw + 2048 (@0x80101ba4-b4).
             * Victim-Anim (EDD-Paar 3, 15-Bone-PL0-Rig) DEFERRED — Dossier §7. */
            pl->rot_y = (int16_t)((e->rot_y + 2048) & 0xfff);
            pl->hit_react |= 1;
            g->ph = 1;
        } else if (g->ph == 1) {
            if (e->anim_frame == 10) g5_se(7);
            if (g5_anim(e)) { g5_clip(e, 2, 1); g5_se(10); g->ph = 2; g->timer = 150; }
        } else {
            g5_anim(e);
            /* Devour-Ende-Gate Gewicht[0]<6000 (@0x80101d04-14); die Gewichte laufen
             * in der Blob-Simulation — der 150er-Timer ist die Ersatzschranke
             * fuer den (noch) fehlenden Morph-Zeitverlauf (dokumentierte Abweichung). */
            if (--g->timer <= 0 && g->gewicht[0] < 6000) { g->blob = 0; g->blob_ph = 0; }
        }
        break;
    }

    default:
        g->sub = 0; g->ph = 0;
        break;
    }

    g5_blob_tick(e, pl);

    /* Mess-Schiene (env-gegated, birkin_dbg.log wie gehabt). */
    if (getenv("RE15_BIRKIN_DBG")) {
        static unsigned n = 0;
        if ((n++ % 15u) == 0u) {
            FILE *bf = fopen("birkin_dbg.log", "a");
            if (bf) {
                fprintf(bf, "g5 tick=%u sub=%u ph=%u clip=%d af=%u u=%d dist=%d hp=%d "
                            "blob=%u/%u gw0=%d akku=%d pos=(%d,%d)\n",
                        n - 1, g->sub, g->ph, (int)e->motion, (unsigned)e->anim_frame,
                        (int)g5_u(e), (int)dist, (int)e->hp, g->blob, g->blob_ph,
                        (int)g->gewicht[0], (int)g->flinch_akku, (int)e->x, (int)e->z);
                fclose(bf);
            }
        }
    }
}
