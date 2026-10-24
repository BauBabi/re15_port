/*
 * RE1.5 Rebuilt — Runde 41 Spur L "spinnenanim": Zeichnen der Adultspinne (Typ 0x25) wie das Original.
 * Kopf und Belege: include/re15_spinnenanim_r41.h, Dossier analysis/befunde_runde41/L_spinnenanim.md.
 *
 * Original-Messung (DuckStation-GDB, MZD-Disc, ROOM2060, Spinne e0, je Takt die Figur-Matrix +0x20 und die
 * Weltmatrix von Teil 0): Takt 62..133 M = 0.8 x RotMatrix(0,1024,0) bzw. 0.8 x RotMatrix(0,1024,1024)
 * (Eintraege 3276 = 0xccc, z.B. Takt 93: [0,0,3276, 3276,0,0, 0,3276,0]); Dossier 1.3.
 */
#include "re15_spinnenanim_r41.h"
#include "re15_skeleton.h"   /* re15_skel_euler_matrix = RotMatrix-Zwilling (mat3_from_euler, Riegel test_rotmatrix) */
#include "re15_engine.h"     /* g_engine.frame_count = Spieltakt (main.c zaehlt ihn am Ende des Takts hoch) */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SPINNE_TYP 0x25u

void re15_spinnenanim_r41_init(re15_actor_t *e)
{
    if (!e || e->type != SPINNE_TYP) return;
    /* `lw v0,0(v1)` @0x80110de0 / `ori v0,v0,0x800` @0x80110de8 / `sw v0,0(v1)` @0x80110dec (Flag 0x800) und
     * `ori v0,zero,0xccc` @0x80110df8 / `sh v0,358(v1)` @0x80110dfc (+0x166). Port: render_scale_q12 != 0
     * == Flag gesetzt (re15_actor.h). Einziger Schreiber von +0x166 im Spinnen-Code 0x801109e4-0x801158a0
     * (Zensus aller `sh rt,358(rs)` in STAGE2.BIN: nur @0x80110dfc im Spinnen-Bereich) -> der Wert gilt das
     * ganze Leben der Spinne; der Haken darf daher in jedem Takt laufen (idempotent). */
    e->render_scale_q12 = RE15_SPINNENANIM_R41_MASSSTAB;
}

int re15_spinnenanim_r41_volle_drehung(const re15_actor_t *e)
{
    /* FUN_8001e8c8: `addiu a0,a1,104` @0x8001e8f0 / `jal 0x80068098` (RotMatrix) @0x8001e8f4 /
     * `addiu a1,a1,32` @0x8001e8f8 — fuer JEDE gezeichnete Figur; Teil 0 haengt an Figur+0x20
     * (`lw s0,108(s2)` @0x8001ea0c, Live-RAM: part0 +0x6c == Figur+0x20). Port-Haken nur fuer die Spinne
     * (der Zeichner nimmt fuer alle anderen Typen weiter RotMatrix(0, Gier, 0), Dossier 6). */
    return (e && e->type == SPINNE_TYP) ? 1 : 0;
}

void re15_spinnenanim_r41_figur_matrix(const re15_actor_t *e, int32_t m[9])
{
    if (!m) return;
    if (!e) { memset(m, 0, 9 * sizeof m[0]); m[0] = m[4] = m[8] = 0x1000; return; }
    re15_skel_euler_matrix((int)e->rot_x, (int)e->rot_y, (int)e->rot_z, m);   /* RotMatrix @0x8001e8f4 */
    if (e->render_scale_q12) {
        /* ScaleMatrix @0x8001e940 (RE_15_Quellcode_V2/ScaleMatrix.c: m[i][j] = (m[i][j] * v[j]) >> 12,
         * v = (+0x166, +0x166, +0x166) @0x8001e91c-938) — gleich der Port-Formel in main.c. */
        for (int k = 0; k < 9; k++)
            m[k] = (m[k] * (int32_t)e->render_scale_q12) >> 12;
    }
}

/* ---- Gezeichnetes Bild (FUN_8001f314 / FUN_8001f3bc) ---------------------------------------------------- */
typedef struct {
    uint8_t  gueltig;
    uint8_t  schacht;     /* beim anim_set im Schacht-Ablauf (Zustand 4, +0x5 = 1)? */
    int16_t  clip;        /* Port-motion (RE2-Nummer im RE2-Modus) */
    int32_t  bild;        /* +0x95 VOR dem Hochzaehlen = das posierte Bild */
    uint8_t  frac;        /* +0x8f VOR dem Herunterzaehlen = die Ueberblendung des posierten Bilds */
    uint8_t  phase;       /* +0x7 beim anim_set (8 = Landetakt @0x80115338) */
    uint32_t takt;        /* g_engine.frame_count beim anim_set */
} merker_t;

static merker_t s_merker[RE15_ACTOR_MAX];

static int platz_von(const re15_actor_t *e)
{
    if (!e) return -1;
    ptrdiff_t s = e - g_actors;
    return (s >= 0 && s < RE15_ACTOR_MAX) ? (int)s : -1;
}

void re15_spinnenanim_r41_reset(void)
{
    memset(s_merker, 0, sizeof s_merker);
}

void re15_spinnenanim_r41_vor_anim(const re15_actor_t *e)
{
    int s = platz_von(e);
    if (s < 0 || e->type != SPINNE_TYP) return;
    merker_t *m = &s_merker[s];
    m->gueltig = 1;
    m->schacht = (e->state == 4 && e->sub_state_1 == 1) ? 1 : 0;   /* Zustandswort 0x104 @0x80110d14 */
    m->clip    = e->motion;
    m->bild    = e->anim_frame;                                    /* `lbu v0,149(t0)` @0x8001f35c */
    m->frac    = e->anim_frac;                                     /* `lbu s0,143(v1)` @0x8001f408 */
    m->phase   = e->sub_state_3;
    m->takt    = g_engine.frame_count;
}

int re15_spinnenanim_r41_zeichenbild(const re15_actor_t *e, int *bild, int *frac)
{
    int s = platz_von(e);
    if (s < 0 || e->type != SPINNE_TYP) return 0;
    const merker_t *m = &s_merker[s];
    if (!m->gueltig || !m->schacht || m->clip != e->motion) return 0;
    /* Gilt, solange die Spinne im Schacht-Ablauf steht (Phase 0 ruft kein anim_set: die INIT-Pose bleibt
     * stehen, @0x80114f14-1c), und im Landetakt (Phase 8 posiert Clip 0 Bild 0 und setzt danach das
     * Zustandswort, @0x80115394-b0) — nur in DIESEM Takt. Danach animiert das Gehirn des Modus selbst. */
    if (!(e->state == 4 && e->sub_state_1 == 1) && !(m->phase == 8 && m->takt == g_engine.frame_count)) return 0;
    if (bild) *bild = m->bild;
    if (frac) *frac = m->frac;
    return 1;
}

/* Spielstand-Ueberblendzaehler waehrend der Pose-Rechnung (je Platz; 0xffff = nichts getauscht). */
static uint16_t s_frac_spiel[RE15_ACTOR_MAX];
static uint8_t  s_frac_init = 0;

void re15_spinnenanim_r41_zeichnen_beginn(const re15_actor_t *ec, uint32_t *ncur, int fps)
{
    /* Der Zeichner haelt die Figur const; der Ueberblendzaehler wird nur fuer die Pose-Rechnung getauscht und in
     * ..._zeichnen_ende zurueckgestellt (wie g_anim_pose_actor = npc im Zeichner). */
    re15_actor_t *e = (re15_actor_t *)(uintptr_t)ec;
    if (!s_frac_init) { for (int i = 0; i < RE15_ACTOR_MAX; i++) s_frac_spiel[i] = 0xffffu; s_frac_init = 1; }
    int s = platz_von(e);
    if (s < 0) return;
    int bild = 0, frac = 0;
    if (!re15_spinnenanim_r41_zeichenbild(e, &bild, &frac)) return;
    if (ncur) *ncur = (fps == 30) ? (uint32_t)bild : ((uint32_t)bild >> 1);   /* main.c-Regel 30/60 Bilder */
    s_frac_spiel[s] = e->anim_frac;
    e->anim_frac = (uint8_t)frac;
}

void re15_spinnenanim_r41_zeichnen_ende(const re15_actor_t *ec)
{
    re15_actor_t *e = (re15_actor_t *)(uintptr_t)ec;
    int s = platz_von(e);
    if (s < 0 || !s_frac_init || s_frac_spiel[s] == 0xffffu) return;
    e->anim_frac = (uint8_t)s_frac_spiel[s];
    s_frac_spiel[s] = 0xffffu;
}

/* ---- Messschiene RE15_SPINNENANIM_LOG (env-gegatet, kein Spielverhalten) --------------------------------------
 * main.c ruft das NACH dem Aufbau seiner Figur-Matrix (nyaw inkl. Massstab) je gezeichneter Figur; geschrieben wird
 * nur fuer Typ 0x25: "L41 F<takt> s<platz> st<zustand>/<+0x5>/<+0x7> c<clip> b<gezeichnetes Bild> f<Ueberblendung>
 * M<9 Eintraege der Matrix, die der Zeichner WIRKLICH benutzt>". Auswerter: test_r41_spinnenanim pruef_log. */
#include <stdio.h>
#include <stdlib.h>
void re15_spinnenanim_r41_log_matrix(const re15_actor_t *e, const int32_t m[9])
{
    static FILE *s_fp = NULL;
    static int s_init = 0;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_SPINNENANIM_LOG");
        if (p && *p) s_fp = fopen(p, "w");
    }
    if (!s_fp || !e || !m || e->type != SPINNE_TYP) return;
    int bild = (int)e->anim_frame, frac = (int)e->anim_frac;
    (void)re15_spinnenanim_r41_zeichenbild(e, &bild, &frac);
    fprintf(s_fp, "L41 F%u s%d st%d/%d/%d c%d b%d f%d M%d,%d,%d,%d,%d,%d,%d,%d,%d\n", (unsigned)g_engine.frame_count,
            platz_von(e), (int)e->state, (int)e->sub_state_1, (int)e->sub_state_3, (int)e->motion, bild, frac,
            (int)m[0], (int)m[1], (int)m[2], (int)m[3], (int)m[4], (int)m[5], (int)m[6], (int)m[7], (int)m[8]);
    fflush(s_fp);
}

/* Pose des letzten Zeichnens fuer Weltanker (Gift-Anker spinnen_r38.c): das Original liest *(+0x188)+0x40 = die
 * Teil-0-WELTmatrix, die NUR der Zeichner schreibt (FUN_8001e9ec -> FUN_80022da0 @0x8001ea24) — also die Pose des
 * VORIGEN Takts. Der Merker eines anim_set-Zwillings aus dem vorigen Takt ist genau diese Pose (Clip, Bild vor dem
 * Vorschub). Rueckgabe 0 = kein Merker aus dem vorigen Takt (z.B. RE2-Gehirn nach der Landung) -> Aufrufer nimmt
 * seinen bisherigen Weg. Die Ueberblendung (+0x8f) des gezeichneten Bilds rechnet der Anker nicht nach. */
int re15_spinnenanim_r41_letzte_pose(const re15_actor_t *e, int *clip, int *bild)
{
    int s = platz_von(e);
    if (s < 0 || e->type != SPINNE_TYP) return 0;
    const merker_t *m = &s_merker[s];
    if (!m->gueltig || m->takt + 1u != g_engine.frame_count) return 0;
    if (clip) *clip = m->clip;
    if (bild) *bild = m->bild;
    return 1;
}
