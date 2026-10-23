/*
 * wassersprung_r41.c — Runde 41 Spur A "wasser20b0", Punkt 2: Landungs-Spritzer des Spielers (Sprung von der
 * Kante ins Wasser) und die Schritte der NPCs (Wasser-Effekt + Schritt-Ton). Belege (Adressen) im Kopf von
 * include/re15_wassersprung_r41.h, Dossier analysis/befunde_runde41/A_wasser20b0.md (Punkt 2, Nachbesserung 1).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_wassersprung_r41.h"
#include "re15_wasser_r38.h"     /* Codes des Schritts, re15_wasser_r38_anker (Part-Weltmatrix * (0,100,0)) */
#include "re15_actor.h"
#include "re15_anim_select.h"    /* re15_compute_actor_kf (PORT-WAHL-Ableitung aus der Pose)                  */
#include "re15_audio.h"          /* re15_audio_footstep                                                     */
#include "re15_emd.h"            /* re15_emd_get_frame_entry = *(+0x168)                                    */
#include "re15_enemy.h"          /* re15_enemy_find (Baenke: Container / Loco / Own)                        */
#include "re15_enemy_ai.h"       /* re15_actor_uses_own_bank / re15_actor_uses_loco_bank (die EINE Regel)   */
#include "re15_engine.h"         /* g_engine.frame_count (Mess-Log)                                         */
#include "re15_esp.h"            /* re15_esp_fx_spawn_rows = FUN_80019700                                   */
#include "re15_rdt.h"            /* re15_rdt_floor_sound = FUN_800437d4                                     */
#include "re15_skeleton.h"       /* re15_skel_compute_pose (PORT-WAHL-Ableitung)                            */

static re15_wassersprung_r41_mess_t s_mess;

/* Folge-Plaetze der NPC-Schritte (Sub 3, Flags 0x07: Matrix je Takt neu @0x80019f54-a4). */
#define FOLGE_MAX 8
static struct { int platz; int aktor; int part; } s_folge[FOLGE_MAX];
static int s_init = 0;

/* GEZEICHNETER ZUSTAND je Aktor (Nachbesserung 1, Fortsetzung 2026-10-24) = das Port-Gegenstueck zu *(+0x168) und den
 * Part-Matrizen +0x188 des Originals. anim_set FUN_8001f314 setzt +0x168 auf das Bildwort des Bilds, das es POSIERT
 * (`sw a2,360(t0)` @0x8001f36c, Vorschub erst danach @0x8001f618-1c); die Part-Matrizen (Stride 0xac) entstehen beim
 * Zeichnen dieses Bilds. Die Schritt-Tests der NPC-Subs lesen beide im NAECHSTEN Takt - auch wenn der Sub davor schon
 * +0x94/+0x95 neu gesetzt (Sub 5 Phase 1 ausgerichtet: +0x94 = 0 / +0x95 = 0 @0x8005157c-8c vor dem Test @0x800515e4;
 * Phase 0 jedes Subs), gedreht (Kurs Sub 4/5 Phase 1 `jal 0x8001aac4` @0x80051264 / @0x800515d0, Sub 9 @0x80051e44) oder geschoben hat
 * (Sub 7/8: pos_advance @0x800519c8 / @0x80051bc0 vor dem Test @0x800519dc / @0x80051bd4). Der Port haelt deshalb am
 * Bildanfang (game_step_common.c, vor allen KI-Takten) je Aktor Bank, Clip, Bild und Lage des gezeichneten Bilds fest. */
typedef struct {
    uint32_t gen;
    uint8_t  ok, type;
    int16_t  motion, rot_y;
    int32_t  bild, x, y, z;
    const void *anim, *skel;
} gezeichnet_t;
static gezeichnet_t s_gez[RE15_ACTOR_MAX];
static uint32_t s_gen = 0;

static const gezeichnet_t *gezeichnet(const re15_actor_t *e)
{
    if (!e || e < &g_actors[0] || e >= &g_actors[RE15_ACTOR_MAX] || s_gen == 0) return NULL;
    const gezeichnet_t *g = &s_gez[e - g_actors];
    if (!g->ok || g->gen != s_gen || g->type != e->type || !g->anim) return NULL;
    return g;
}

static void init(void)
{
    if (s_init) return;
    s_init = 1;
    for (int i = 0; i < FOLGE_MAX; i++) { s_folge[i].platz = -1; s_folge[i].aktor = -1; s_folge[i].part = -1; }
}

const re15_wassersprung_r41_mess_t *re15_wassersprung_r41_mess(void) { return &s_mess; }
void re15_wassersprung_r41_reset(void)
{
    memset(&s_mess, 0, sizeof s_mess);
    memset(s_gez, 0, sizeof s_gez);
    s_gen = 0;
    s_mess.letzter_ton = -1;
    for (int i = 0; i < FOLGE_MAX; i++) { s_folge[i].platz = -1; s_folge[i].aktor = -1; s_folge[i].part = -1; }
    s_init = 1;
}

/* Mess-Log (Debug-Harness, env RE15_NPCSCHRITT_LOG=<datei>): eine Zeile je NPC-Schritt. Kein Verhalten. */
static FILE *npc_log(void)
{
    static int init_log = 0;
    static FILE *f = NULL;
    if (!init_log) {
        init_log = 1;
        const char *p = getenv("RE15_NPCSCHRITT_LOG");
        if (p && *p) f = fopen(p, "w");
    }
    return f;
}

static int erster_freier_platz(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++)
        if (!re15_esp_fx_get(i)) return i;
    return -1;
}

/* FUN_80019700(code, gier, Matrix, Versatz) mit bereits aufgeloester Weltlage. */
static int spawn(uint32_t code, int16_t gier, const int32_t w[3])
{
    const re15_esp_t *bank = re15_esp_room_bank();
    const uint8_t  id    = (uint8_t)(code >> 24);
    const uint8_t  sub   = (uint8_t)((code >> 16) & 0xffu);
    const uint16_t skala = (uint16_t)(code & 0xffffu);
    if (!bank || re15_esp_find_id(bank, id) < 0) return 0;
    /* Boden INT32_MAX: Zeilen B = 0 (keine Routine 12) -> keine Port-Bodenklemme (wie wasser_r38). */
    int n = re15_esp_fx_spawn_rows(bank, id, sub, skala, w[0], w[1], w[2], INT32_MAX, gier);
    if (n > 0) s_mess.plaetze += (unsigned)n;
    s_mess.letzter_anker[0] = w[0]; s_mess.letzter_anker[1] = w[1]; s_mess.letzter_anker[2] = w[2];
    return n;
}

/* ---- (a) Landung @0x80038730-d0 ------------------------------------------------------------------- */
int re15_wassersprung_r41_landung(const re15_actor_t *p, int16_t stempel, int32_t t_y)
{
    init();
    if (!p) return 0;
    /* `slt v1,v1,a0` @0x8003874c: Stempel (Spieler+0x88) < y (Spieler+0x38 = Bodenhoehe); `beq v1,zero` @0x80038758 */
    if (!((int32_t)stempel < p->y)) { s_mess.landung_trocken++; return 0; }
    /* a2 = Spieler+0x20 (Entity-Matrix, T = zuletzt gezeichnete Lage), a3 = (0,0,0) @0x8007428c */
    const int32_t w[3] = { p->x, t_y, p->z };
    int n = 0, k = 0;
    for (int s1 = 4; s1 >= 1; s1--) {                                  /* `ori s1,zero,0x4` @0x8003875c, --@0x800387d0 */
        int16_t s0 = (int16_t)((s1 - 2) << 8);                         /* `addiu s0,s0,-2` / `sll s0,s0,8` @0x80038788-90 */
        int16_t g1 = (int16_t)(p->rot_y + s0);                         /* `lh a1,106(v0)` + s0 @0x8003878c-98            */
        int16_t g2 = (int16_t)(g1 + 2048);                             /* `addiu a1,a1,2048` @0x800387c0                  */
        if (k < RE15_WASSERSPRUNG_R41_PLAETZE) s_mess.letzte_gier[k++] = g1;
        if (k < RE15_WASSERSPRUNG_R41_PLAETZE) s_mess.letzte_gier[k++] = g2;
        n += spawn(RE15_WASSERSPRUNG_R41_CODE, g1, w);                 /* jal 0x80019700 @0x80038794 */
        n += spawn(RE15_WASSERSPRUNG_R41_CODE, g2, w);                 /* jal 0x80019700 @0x800387bc */
    }
    s_mess.landungen++;
    return RE15_WASSERSPRUNG_R41_PLAETZE;
}

/* ---- (b) NPC-Schritt = FUN_80045630(a0, fuss) fuer Typ >= 0x40 ---------------------------------- */

/* Die Bank des Kanals, den der laufende Sub spielt: anim_set (f314) setzt +0x168 auf das Bildwort GENAU der
 * Bank, mit der der Sub es ruft - die NPC-Geh-Subs {4,5,7,8} und die Executor-Subs {2,6,9} laden das Paar
 * +0x170/+0x174 (`lw a0,368(v0)` / `lw a1,372(v0)` vor `jal 0x8001f314` @0x800512bc-c0 u.a.) = die OWN-Bank
 * (dir[3]/dir[4], FUN_80022300 @0x800224b8/c8; Port b->anim_own). Die Container-Bank (dir[1]) traegt dagegen in
 * Clip 5 die 20-Bild-Kniebeuge ohne Aufsetzer - DAS war der Bau-Fehler ("kein NPC-Typ traegt Aufsetzer"). */
const void *re15_wassersprung_r41_npc_bank(const re15_actor_t *e, const void **out_skel)
{
    if (out_skel) *out_skel = NULL;
    if (!e) return NULL;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return NULL;
    if (re15_actor_uses_own_bank(e) && b->own_ok) { if (out_skel) *out_skel = &b->skel_own; return &b->anim_own; }
    if (re15_actor_uses_loco_bank(e) && b->loco_ok) { if (out_skel) *out_skel = &b->skel_loco; return &b->anim_loco; }
    if (out_skel) *out_skel = &b->skel;
    return &b->anim;
}

/* Traegt der Clip ueberhaupt ein Aufsetzer-Bit (0x4000)? Ausgelieferte OWN-Baenke: 0x40/0x45/0x49 Clip 5 Bilder
 * 7 (Wort 0x6xxx) + 22 (0x5xxx), Clip 0 Bilder 0 + 12; 0x42/0x4d Clip 5 Bilder 8 (0x4xxx) + 22 (0x7xxx), Clip 0
 * Bilder 0 + 11; 0x4b (Kind): KEIN Clip mit Marke (Dossier Nachbesserung 1, Sonde npc_own.txt). */
static int clip_hat_marken(const re15_emd_animation_t *an, int clip)
{
    int fc = an->clips[clip].frame_count;
    for (int f = 0; f < fc; f++)
        if (re15_emd_get_frame_entry(an, clip, f) & 0x4000u) return 1;
    return 0;
}

/* PORT-WAHL (NUTZER-VORGABE Nachtrag 3 / Entscheid E1: Schritte fuer ALLE NPC-Typen) fuer Clips OHNE Marke:
 * Aufsetzer-Bild = das Bild, in dem der Fuss-Teil (4 bzw. 7) nach der Schwungphase erstmals wieder seine
 * Tiefstlage erreicht (Pose-y im obersten Zehntel seiner Spanne; PSX-y: groesser = tiefer). Die FORM ist an den
 * markierten Typen belegt: 0x40-Familie Clip 5 Marken 7/22 -> Regel 7/20, Clip 0 Marken 0/12 -> 0/11; 0x42-
 * Familie Clip 5 Marken 8/22 -> 5/18 (Dossier). Fuss-Code wie in den Original-Daten: Teil 4 landet -> Code 7
 * (Bit 0x1000 frei), Teil 7 landet -> Code 4. */
typedef struct { const void *anim; int clip; int fc; int n; uint8_t bild[4]; uint8_t fuss[4]; } portwahl_t;
#define PORTWAHL_MAX 32
static portwahl_t s_portwahl[PORTWAHL_MAX];
static int s_portwahl_n = 0;

static void portwahl_ableiten(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk, uint8_t type, int clip,
                              portwahl_t *out)
{
    int fc = an->clips[clip].frame_count;
    static int32_t y[2][256];
    memset(out, 0, sizeof *out);
    out->anim = an; out->clip = clip; out->fc = fc;
    if (!sk || fc <= 0 || fc > 256 || sk->bone_count <= 7) return;
    for (int f = 0; f < fc; f++) {
        re15_actor_t a; memset(&a, 0, sizeof a);
        a.active = 1; a.type = type; a.motion = (int16_t)clip; a.anim_frame = (uint16_t)f;
        int kf = re15_compute_actor_kf(an, sk, &a, -1, (uint32_t)f);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        void *save = g_anim_pose_actor;
        g_anim_pose_actor = NULL;
        int ok = re15_skel_compute_pose(sk, kf, poses);
        g_anim_pose_actor = save;
        if (ok != 0) return;
        y[0][f] = poses[4].trans[1];
        y[1][f] = poses[7].trans[1];
    }
    for (int p = 0; p < 2; p++) {
        int32_t mx = y[p][0], mn = y[p][0];
        for (int f = 1; f < fc; f++) { if (y[p][f] > mx) mx = y[p][f]; if (y[p][f] < mn) mn = y[p][f]; }
        int32_t thr = mx - (mx - mn) / 10;
        for (int f = 0; f < fc && out->n < 4; f++) {
            int32_t prev = y[p][(f + fc - 1) % fc];
            if (y[p][f] >= thr && prev < thr) {
                out->bild[out->n] = (uint8_t)f;
                out->fuss[out->n] = (uint8_t)(p == 0 ? 7 : 4);
                out->n++;
            }
        }
    }
}

static const portwahl_t *portwahl(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk, uint8_t type, int clip)
{
    int fc = an->clips[clip].frame_count;
    for (int i = 0; i < s_portwahl_n; i++)
        if (s_portwahl[i].anim == an && s_portwahl[i].clip == clip && s_portwahl[i].fc == fc) return &s_portwahl[i];
    if (s_portwahl_n >= PORTWAHL_MAX) s_portwahl_n = 0;     /* Ring: Baenke wechseln mit dem Raum */
    portwahl_t *pw = &s_portwahl[s_portwahl_n++];
    portwahl_ableiten(an, sk, type, clip, pw);
    return pw;
}

int re15_wassersprung_r41_aufsetzer(const void *anim_v, const void *skel_v, uint8_t type, int clip,
                                    int bilder[4], int fuesse[4], int *portwahl_out)
{
    const re15_emd_animation_t *an = (const re15_emd_animation_t *)anim_v;
    const re15_emd_skeleton_t  *sk = (const re15_emd_skeleton_t *)skel_v;
    if (portwahl_out) *portwahl_out = 0;
    if (!an || clip < 0 || clip >= an->clip_count) return 0;
    int n = 0, fc = an->clips[clip].frame_count;
    if (clip_hat_marken(an, clip)) {
        for (int f = 0; f < fc && n < 4; f++) {
            uint32_t w = re15_emd_get_frame_entry(an, clip, f);
            if (w & 0x4000u) { bilder[n] = f; fuesse[n] = (w & 0x1000u) ? 4 : 7; n++; }
        }
        return n;
    }
    const portwahl_t *pw = portwahl(an, sk, type, clip);
    if (portwahl_out) *portwahl_out = 1;
    for (int i = 0; i < pw->n; i++) { bilder[i] = pw->bild[i]; fuesse[i] = pw->fuss[i]; }
    return pw->n;
}

int re15_wassersprung_r41_regel(const void *anim_v, const void *skel_v, uint8_t type, int clip, int bilder[4], int fuesse[4])
{
    const re15_emd_animation_t *an = (const re15_emd_animation_t *)anim_v;
    if (!an || clip < 0 || clip >= an->clip_count) return 0;
    portwahl_t pw;
    portwahl_ableiten(an, (const re15_emd_skeleton_t *)skel_v, type, clip, &pw);
    for (int i = 0; i < pw.n; i++) { bilder[i] = pw.bild[i]; fuesse[i] = pw.fuss[i]; }
    return pw.n;
}

static void folge_merken(int platz, int aktor, int part)
{
    init();
    int frei = -1;
    for (int i = 0; i < FOLGE_MAX; i++) {
        if (s_folge[i].platz == platz) { s_folge[i].aktor = aktor; s_folge[i].part = part; return; }
        if (frei < 0 && s_folge[i].platz < 0) frei = i;
    }
    if (frei < 0) frei = 0;
    s_folge[frei].platz = platz; s_folge[frei].aktor = aktor; s_folge[frei].part = part;
}

static int npc_anker(const re15_actor_t *e, int part, int32_t out[3])
{
    const gezeichnet_t *g = gezeichnet(e);
    if (g && g->skel) {   /* Part[fuss] des gezeichneten Bilds: `lw a2,392(a1)` (+0x188) + fuss*0xac @0x8004567c-a8 (s. Kopf s_gez) */
        re15_actor_t v = *e;
        v.x = g->x; v.y = g->y; v.z = g->z; v.rot_y = g->rot_y; v.motion = g->motion; v.anim_frame = g->bild;
        return re15_wasser_r38_anker(g->skel, g->anim, &v, -1, (uint32_t)g->bild, part, out);
    }
    const void *sk = NULL;
    const void *an = re15_wassersprung_r41_npc_bank(e, &sk);
    if (an && sk)
        return re15_wasser_r38_anker(sk, an, e, -1, e->anim_frame, part, out);
    out[0] = e->x; out[1] = e->y; out[2] = e->z;
    return -1;
}

int re15_wassersprung_r41_npc_schritt(re15_actor_t *e, int a0, int fuss, const void *rdt_v)
{
    init();
    if (!e || e->type < 0x40) return -1;
    const re15_rdt_t *rdt = (const re15_rdt_t *)rdt_v;
    /* Bodentyp an der Lage +0x34/+0x3c (`jal 0x800437d4` @0x8004566c) */
    const int boden = rdt ? re15_rdt_floor_sound(rdt, e->x, e->z) : 0;
    int s0 = boden, part = -1;
    uint32_t code = 0; int16_t gier = 0;
    const char *zweig = "trocken";
    if (boden & RE15_WASSER_R38_BIT_WASSER) {                     /* `andi v0,s0,0x80` @0x800456ac          */
        code = RE15_WASSER_R38_CODE_BODEN; part = fuss; gier = e->rot_y;   /* @0x800456b8-d0                 */
        s_mess.npc_boden++; zweig = "boden";
    } else if (e->water_y != 0) {                                 /* `lh v0,136(a1)` / beq @0x800456dc-e4   */
        s0 = boden - RE15_WASSER_R38_WATSATZ;                     /* `addiu s0,a0,-3` @0x800456ec           */
        code = RE15_WASSER_R38_CODE_STEMPEL; part = fuss - 1; gier = (int16_t)RE15_WASSER_R38_GIER_STEMPEL;
        s_mess.npc_stempel++; zweig = "stempel";
    }
    int n = 0;
    if (code) {
        int32_t w[3];
        (void)npc_anker(e, part, w);
        int platz = erster_freier_platz();
        n = spawn(code, gier, w);
        if (n > 0 && platz >= 0 && code == RE15_WASSER_R38_CODE_STEMPEL)
            folge_merken(platz, (int)(e - g_actors), part);
    }
    /* Tonindex: a0 + (Boden & 0x7f) @0x80045718-24, NPC-Zweig -8 @0x8004575c (kein Objekt-Override) */
    int idx = a0 + (s0 & 0x7f) - RE15_WASSERSPRUNG_R41_NPC_TON;
    if (idx >= 0) re15_audio_footstep(fuss, idx);
    s_mess.npc_schritte++;
    s_mess.letzter_ton = idx;
    s_mess.letzter_typ = e->type; s_mess.letzter_fuss = (uint8_t)fuss;
    {
        FILE *lf = npc_log();
        if (lf) {
            fprintf(lf, "F%u slot=%d typ=0x%02x clip=%d bild=%u fuss=%d boden=0x%02x stempel=%d zweig=%s ton=%d plaetze=%d portwahl=%u pos=(%d,%d,%d)\n",
                    (unsigned)g_engine.frame_count, (int)(e - g_actors), (unsigned)e->type, (int)e->motion,
                    (unsigned)e->anim_frame, fuss, (unsigned)boden, (int)e->water_y, zweig, idx, n,
                    (unsigned)s_mess.letzter_portwahl, e->x, e->y, e->z);
            fflush(lf);
        }
    }
    return idx;
}

int re15_wassersprung_r41_npc_takt(re15_actor_t *e, int a0)
{
    if (!e || e->type < 0x40) return 0;
    const void *sk_v = NULL;
    const re15_emd_animation_t *an;
    int clip;
    int32_t bild_roh;
    const gezeichnet_t *g = gezeichnet(e);
    if (g) {   /* *(+0x168) = Bildwort des GEZEICHNETEN Bilds (s. Kopf s_gez) - unabhaengig von +0x94/+0x95 dieses Takts */
        an = (const re15_emd_animation_t *)g->anim; sk_v = g->skel; clip = (int)g->motion; bild_roh = g->bild;
    } else {   /* ohne Bildanfang (Riegel, die den Sub direkt rufen): der Aktor steht noch im gezeichneten Zustand */
        an = (const re15_emd_animation_t *)re15_wassersprung_r41_npc_bank(e, &sk_v); clip = (int)e->motion; bild_roh = e->anim_frame;
    }
    if (!an || an->clip_count <= 0) return 0;
    if (clip < 0 || clip >= an->clip_count) return 0;
    int fc = an->clips[clip].frame_count;
    if (fc <= 0) return 0;
    /* Bildwort *(+0x168) (`lw v0,360(v0)` / `lw v1,0(v0)` / `andi 0x4000` @0x80051278-88), Fuss 7 - 3*((Wort>>12)&1)
     * @0x80051294-ac. Ohne Schnappschuss gilt: der Haken laeuft VOR dem Bildvorschub des Takts (POSE-DANN-VORSCHUB,
     * enemy_ai_common.c re15_npc_anim) -> e->anim_frame = das zuletzt posierte Bild. */
    uint32_t bild = (uint32_t)bild_roh % (uint32_t)fc;
    uint32_t wort = re15_emd_get_frame_entry(an, clip, (int)bild);
    int fuss = 0; unsigned pw_treffer = 0;
    if (wort & 0x4000u) fuss = (wort & 0x1000u) ? 4 : 7;
    else if (!clip_hat_marken(an, clip)) {
        const portwahl_t *pw = portwahl(an, (const re15_emd_skeleton_t *)sk_v, e->type, clip);
        for (int i = 0; i < pw->n; i++) if (pw->bild[i] == bild) { fuss = pw->fuss[i]; pw_treffer = 1; break; }
    }
    if (!fuss) return 0;
    s_mess.letzter_portwahl = pw_treffer;
    if (pw_treffer) s_mess.portwahl_schritte++;
    s_mess.letzter_clip = (uint8_t)clip; s_mess.letztes_bild = (uint8_t)bild;
    extern re15_rdt_t g_room_rdt; extern int g_room_rdt_ok;
    (void)re15_wassersprung_r41_npc_schritt(e, a0, fuss, g_room_rdt_ok ? &g_room_rdt : NULL);
    return 1;
}

void re15_wassersprung_r41_bildanfang(void)
{
    init();
    if (++s_gen == 0) s_gen = 1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {        /* Zustand des gezeichneten Bilds je NPC (s. Kopf s_gez) */
        const re15_actor_t *e = &g_actors[i];
        gezeichnet_t *g = &s_gez[i];
        g->ok = 0;
        if (!e->active || e->type < 0x40) continue;
        const void *sk = NULL;
        const void *an = re15_wassersprung_r41_npc_bank(e, &sk);
        if (!an) continue;
        g->gen = s_gen; g->ok = 1; g->type = e->type; g->motion = e->motion; g->rot_y = e->rot_y;
        g->bild = e->anim_frame; g->x = e->x; g->y = e->y; g->z = e->z; g->anim = an; g->skel = sk;
    }
    for (int i = 0; i < FOLGE_MAX; i++) {
        if (s_folge[i].platz < 0) continue;
        re15_esp_fx_t *f = (re15_esp_fx_t *)re15_esp_fx_get(s_folge[i].platz);
        if (!f || f->effect_id != 0x06 || f->sub_index != 0x03 || !(f->flags & 0x04)) {
            if (!f || f->effect_id != 0x06 || f->sub_index != 0x03) { s_folge[i].platz = -1; s_folge[i].aktor = -1; }
            continue;
        }
        if (s_folge[i].aktor <= 0 || s_folge[i].aktor >= RE15_ACTOR_MAX) continue;
        const re15_actor_t *e = &g_actors[s_folge[i].aktor];
        if (!e->active) { s_folge[i].platz = -1; continue; }
        int32_t a[3];
        if (npc_anker(e, s_folge[i].part, a) == 0) { f->x = a[0]; f->y = a[1]; f->z = a[2]; }
    }
}
