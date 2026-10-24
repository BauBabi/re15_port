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
#include "re15_audio.h"          /* re15_audio_footstep                                                     */
#include "re15_emd.h"            /* re15_emd_get_frame_entry = *(+0x168)                                    */
#include "re15_enemy.h"          /* re15_enemy_find (Baenke: Container / Loco / Own)                        */
#include "re15_enemy_ai.h"       /* re15_actor_uses_own_bank / re15_actor_uses_loco_bank (die EINE Regel)   */
#include "re15_engine.h"         /* g_engine.frame_count (Mess-Log)                                         */
#include "re15_esp.h"            /* re15_esp_fx_spawn_rows = FUN_80019700                                   */
#include "re15_rdt.h"            /* re15_rdt_floor_sound = FUN_800437d4                                     */
#include "re15_skeleton.h"       /* re15_skel_compute_pose (PORT-WAHL-Ableitung)                            */
#include "re15_scd.h"            /* g_scd.work_vars[0x0A] = DAT_800b0fe4 (gezeigter Cut, Nachbesserung 2)        */
#include "re15_math.h"           /* re15_squareroot0 = SquareRoot0 (Abstand der Messzeile, Nachbesserung 2)      */

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
    s_mess.letzter_ton_orig = -1; s_mess.letzte_lage[0] = s_mess.letzte_lage[1] = -1; s_mess.letzter_abstand = -1;
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
 * Bank, mit der der Sub es ruft - die NPC-Subs {4,5,9} und die Executor-Subs {2,6} laden das Paar +0x170/+0x174
 * (`lw a0,368(v0)` / `lw a1,372(v0)` vor `jal 0x8001f314` @0x800512bc-c0 u.a.) = die OWN-Bank (dir[3]/dir[4],
 * FUN_80022300 @0x800224b8/c8; Port b->anim_own). Die Rueckwaerts-Subs 7/8 laden das LOCO-Paar +0x84/+0x16c
 * (`lw a0,132(v0)` / `lw a1,364(v0)` @0x80051a20-24 / @0x80051c18-1c, Clip 1; der Port zeichnet dort die
 * Container-Bank, deren Clip 1 dieselben Marken 7/22 traegt - Sonde npc_marken.txt). Die Container-Bank (dir[1])
 * traegt in Clip 5 die 20-Bild-Kniebeuge ohne Aufsetzer - DAS war der Bau-Fehler ("kein NPC-Typ traegt Aufsetzer"). */
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

/* FORTBEWEGUNGS-CLIPS (Nachbesserung 1, dritte Fortsetzung): in den ausgelieferten Baenken tragen NUR sie Aufsetzer-
 * Marken - Sonde npc_marken.txt (Bildwort-Bit 0x4000 je Bank und Clip, alle NPC-Typen 0x40/0x42/0x45/0x47/0x49/0x4b/
 * 0x4d, CDEMD0 und CDEMD1): OWN c0 (Laufen, 22 Bilder) + c5 (Gehen, 30), LOCO/Container c0 (34) + c1 (Rueckwaerts,
 * 32; Sub 7/8 posieren LOCO Clip 1, +0x94 = 1 @0x8005196c). Das Original schreitet nur an Marken (`andi 0x4000` vor
 * jedem `jal 0x80045630`, z.B. @0x80051288 / @0x800519ec) - ein anderer Clip ohne Marke (INIT-Pose LOCO c2 @0x8011d978,
 * Gesten, Kante/Schub) gibt dort keinen Schritt. Die PORT-WAHL (NUTZER-VORGABE "alle NPCs") ersetzt deshalb nur
 * FEHLENDE Marken eines Fortbewegungs-Clips: Typ 0x4b (keine Marke in keiner Bank) und 0x47 aus CDEMD1 (OWN c5 ohne
 * Marke, c0 markiert). */
static int fortbewegungs_clip(const re15_emd_animation_t *an, uint8_t type, int clip)
{
    const re15_enemy_bank_t *b = re15_enemy_find(type);
    if (b && b->own_ok && an == &b->anim_own) return clip == 0 || clip == 5;
    return clip == 0 || clip == 1;
}

/* PORT-WAHL (NUTZER-VORGABE Nachtrag 3 / Entscheid E1: Schritte fuer ALLE NPC-Typen) fuer Fortbewegungs-Clips OHNE
 * Marke: Aufsetzer-Bild = das Bild, in dem der Fuss-Teil (4 bzw. 7) nach der Schwungphase erstmals wieder seine
 * Tiefstlage erreicht (Pose-y im obersten Zehntel seiner Spanne; PSX-y: groesser = tiefer). "Nach der Schwungphase"
 * heisst: der Fuss war seit dem letzten Aufsetzer ueber der halben Spanne (Hysterese; ohne sie zaehlte das Zittern
 * eines stehenden Fusses um die Zehntel-Schwelle als weiterer Aufsetzer - gemessen an Roys Clip 5, CDEMD1 0x47:
 * Teil 4 steht in den Bildern 22..50 bei y -224..-230 um die Schwelle -226). Zwei Umlaeufe, gezaehlt im zweiten:
 * der Zustand am Clip-Ende traegt in den Anfang (Schleife). Die FORM ist an den markierten Typen belegt: 0x40-Familie
 * Clip 5 Marken 7/22 -> Regel 7/20, Clip 0 Marken 0/12 -> 0/11; 0x42-Familie Clip 5 Marken 8/22 -> 5/18 (Dossier).
 * Fuss-Code wie in den Original-Daten: Teil 4 landet -> Code 7 (Bit 0x1000 frei), Teil 7 landet -> Code 4. */
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
    (void)type;
    for (int f = 0; f < fc; f++) {
        /* Bild f -> Keyframe wie re15_compute_actor_kf vorwaerts (Slot f < fc; anim_select_common.c), aber OHNE dessen
         * RE15_ANIM_TRACE-Zeile: die Ableitung ist keine Pose des Spiels (gemessen: integration_r37_roy_leon1170 rot,
         * 50 Fremdzeilen Typ 0x47 Clip 2 im Bild 890 = diese Schleife ueber re15_compute_actor_kf). 0x8000-Zwischenbild
         * wie dort (FUN_8001f314 @0x8001f370-f37c -> FUN_8001f8b4): A = erstes echtes Bild rueckwaerts, B = erstes echtes
         * Bild vorwaerts, Gewicht = Wort & 0xfff (Roys Clip 5 traegt solche Bilder: 9/25/41/57, Sonde roy_c5.txt). */
        re15_kf_tween_t tw = {0, 0, 0};
        int slot = f;
        uint32_t w = re15_emd_get_frame_entry(an, clip, slot);
        if (w & 0x8000u) {
            int vor = slot, nach = slot;
            for (int k = 0; k < fc && (re15_emd_get_frame_entry(an, clip, vor) & 0x8000u); k++) vor = (vor + fc - 1) % fc;
            for (int k = 0; k < fc && (re15_emd_get_frame_entry(an, clip, nach) & 0x8000u); k++) nach = (nach + 1) % fc;
            int kf_von = (int)(re15_emd_get_frame_entry(an, clip, vor) & 0xFFFu);
            if (kf_von >= sk->keyframe_count) kf_von = sk->keyframe_count > 0 ? sk->keyframe_count - 1 : 0;
            tw.active = 1; tw.kf_from = kf_von; tw.weight = (int)(w & 0xFFFu);
            slot = nach;
        }
        int kf = (int)(re15_emd_get_frame_entry(an, clip, slot) & 0xFFFu);
        if (kf >= sk->keyframe_count) kf = sk->keyframe_count > 0 ? sk->keyframe_count - 1 : 0;
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        void *save = g_anim_pose_actor;            /* Abfrage: keinen Crossfade-Zustand und keinen anstehenden */
        re15_kf_tween_t save_tw = g_anim_kf_tween; /* Zwischenbild-Kanal des Zeichners veraendern (wie affen_11c0.c) */
        g_anim_pose_actor = NULL;
        g_anim_kf_tween = tw;
        int ok = re15_skel_compute_pose(sk, kf, poses);
        g_anim_pose_actor = save;
        g_anim_kf_tween = save_tw;
        if (ok != 0) return;
        y[0][f] = poses[4].trans[1];
        y[1][f] = poses[7].trans[1];
    }
    for (int p = 0; p < 2; p++) {
        int32_t mx = y[p][0], mn = y[p][0];
        for (int f = 1; f < fc; f++) { if (y[p][f] > mx) mx = y[p][f]; if (y[p][f] < mn) mn = y[p][f]; }
        if (mx == mn) continue;
        int32_t thr   = mx - (mx - mn) / 10;       /* unterstes Zehntel der Fusshoehe = Aufsetzen        */
        int32_t mitte = mx - (mx - mn) / 2;        /* ueber der halben Spanne = Schwungphase (Hysterese)  */
        int scharf = 0;
        for (int k = 0; k < 2 * fc && out->n < 4; k++) {
            int f = k % fc;
            if (y[p][f] < mitte) scharf = 1;
            else if (scharf && y[p][f] >= thr) {
                scharf = 0;
                if (k >= fc) {
                    out->bild[out->n] = (uint8_t)f;
                    out->fuss[out->n] = (uint8_t)(p == 0 ? 7 : 4);
                    out->n++;
                }
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
    if (!fortbewegungs_clip(an, type, clip)) return 0;   /* kein Fortbewegungs-Clip: wie im Original kein Schritt */
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

/* ===================== Nachbesserung 2: Schritt-Ton der NPCs (M1) und seine Lage (M2), Kopf (f) im Header ===================== */

/* EDT-Satz idx der Raumbank snd0 (RDT +0x08; FUN_80045630 `lw a2,8(v0)` @0x80045754 / `lw a2,8(v1)` @0x80045784). */
static const uint8_t *edt_satz(const re15_rdt_t *rdt, int idx)
{
    if (!rdt || !rdt->snd_edt[0] || idx < 0 || (idx + 1) * 4 > rdt->snd_edt_size[0]) return NULL;
    return rdt->snd_edt[0] + (size_t)idx * 4;
}

/* Im ORIGINAL hoerbar: Stimme (e[3] & 0x1f) - 16 + (fuss != 4) liegt in den 8 gepumpten Saetzen (@0x800457a0-b4, FUN_800458d4
 * `ori s4,zero,0x8` @0x800458dc). Ein leerer Satz (00000000) gibt Stimme -16/-15. */
static int ton_orig_hoerbar(const re15_rdt_t *rdt, int idx, int fuss)
{
    const uint8_t *e = edt_satz(rdt, idx);
    if (!e) return 0;
    const int stimme = (e[3] & 0x1f) - RE15_WASSERSPRUNG_R41_STIMME_BASIS + (fuss != 4 ? 1 : 0);
    return stimme >= 0 && stimme < RE15_WASSERSPRUNG_R41_STIMMEN;
}

/* RE2-Retail FUN_8005c040: leerer Satz -> kein Ton (`lw v0,0(s4)` / `beq v0,v1` @0x8005c468-70; RE1.5-Daten: leer = Ton 0 und
 * Kopf 0, dieselbe Pruefung wie re15_footstep_vag in vab_common.c). */
static int satz_belegt(const re15_rdt_t *rdt, int idx)
{
    const uint8_t *e = edt_satz(rdt, idx);
    return e && !(e[2] == 0 && e[3] == 0);
}

/* FUN_80045a64 byte-genau (Kopf (f)). Die Tabellen PAN @0x80074728 (128 B) und ATT @0x800747a8 (128 B) liegen im Original
 * hintereinander; der Pan-Index 0x89 - i (bis 137) liest ueber PAN hinaus in ATT (`lui at,0x8007` / `addiu at,at,18216` /
 * `lbu s1,0(at)` @0x80045c88-94) - deshalb EIN Feld. Bytes = PSX.EXE (re15_disasm.py read 0x80074728 256 --w 1; Riegel
 * unit_r41_wasser20b0_lage vergleicht sie mit der Datei). */
static const uint8_t k_pan_att[256] = {
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, 126,126,126,126,125,125,125,125,124,124,124,124,123,123,122,122,
    121,121,120,120,119,119,118,118,117,117,116,116,116,116,115,115, 114,114,113,113,112,112,111,111,110,110,109,109,108,108,107,107,
    106,106,105,105,104,104,103,103,102,102,101,101,100,100, 99, 99,  98, 98, 97, 97, 96, 96, 95, 95, 94, 94, 93, 93, 92, 92, 91, 91,
     90, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76,  75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 26, 27, 27, 28, 28,
     29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36,  37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44,
     45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52,  53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60,
     61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68,  69, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
};
#define K_ATT 0x80   /* ATT @0x800747a8 = PAN @0x80074728 + 0x80 (`addiu at,at,18344` @0x80045cf4) */

const uint8_t *re15_wassersprung_r41_lage_tabelle(void) { return k_pan_att; }

/* catan @0x800658fc: CORDIC, x = 0x1000 / y = a / z = 0, 12 Stufen (`slti v0,a2,12` @0x800659b4), Schiebung `srav` je Stufe,
 * y >= 0 (`bgez a0` @0x80065930): x += y>>i, y -= x>>i, z += W[i]; sonst umgekehrt; Rueckgabe z nach 12 Stufen (`lw v0,160(sp)`
 * @0x800659c0). W = Tabelle @0x80078c90 (re15_disasm.py read 0x80078c90 12 --w 4). */
static int32_t catan_cordic(int32_t a)
{
    static const int32_t W[12] = { 511, 302, 159, 81, 41, 20, 10, 5, 3, 1, 0, 0 };
    int32_t x = 0x1000, y = a, z = 0;                                   /* `ori v0,zero,0x1000` @0x80065918 */
    for (int i = 0; i < 12; i++) {
        const int32_t xs = x >> i, ys = y >> i;
        if (y >= 0) { x += ys; y -= xs; z += W[i]; }                    /* @0x8006596c-9c */
        else        { x -= ys; y += xs; z -= W[i]; }                    /* @0x80065938-68 */
    }
    return z;
}

/* FUN_80045d6c(x1, z1, x2, z2): Richtung in 4096-teln. dx == 0 -> dz > 0 ? 0xc00 : 0x400 (`blez a0` @0x80045dd8, @0x80045ddc/e0);
 * sonst ((dx >= 0 ? 0x1000 : 0x800) - catan((dz << 12) / dx)) & 0xfff (`sll a0,a0,12` / `div` @0x80045d84-88, @0x80045dc0-d4). */
static int32_t azimut(int32_t x1, int32_t z1, int32_t x2, int32_t z2)
{
    const int32_t dx = x2 - x1, dz = z2 - z1;
    if (dx == 0) return (dz > 0) ? 0xc00 : 0x400;
    const int32_t c = catan_cordic((int32_t)((uint32_t)dz << 12) / dx);
    return ((dx >= 0 ? 0x1000 : 0x800) - c) & 0xfff;
}

int re15_wassersprung_r41_lage(const void *rdt_v, const int32_t pos[3], int lage[2], int *abstand)
{
    const re15_rdt_t *rdt = (const re15_rdt_t *)rdt_v;
    const int cut = (int)g_scd.work_vars[0x0A];          /* DAT_800b0fe4 `lh v0,4068(v0)` @0x80045a70; Satz = *(RDT+0x24) + Cut*0x20 */
    if (!rdt || !rdt->cuts || cut < 0 || cut >= rdt->cut_count || !pos) return 0;
    const re15_camera_cut_t *c = &rdt->cuts[cut];       /* +4/+8/+0xc Auge, +0x10/+0x18 Ziel */
    /* Abstand @0x80045aa0-5c: |dx|^2 + |dz|^2 (32 Bit, mflo), zweimal SquareRoot0, Y-Term Kamera.y - |Kamera.y - y| */
    int32_t dx = c->pos_x - pos[0]; if (dx < 0) dx = -dx;                               /* @0x80045aac-b8 */
    int32_t dz = c->pos_z - pos[2]; if (dz < 0) dz = -dz;                               /* @0x80045acc-d8 */
    const uint32_t sxz = (uint32_t)dx * (uint32_t)dx + (uint32_t)dz * (uint32_t)dz;     /* @0x80045abc-e8 */
    int32_t dy = c->pos_y - pos[1]; if (dy < 0) dy = -dy;                               /* @0x80045af4-b00 */
    const uint32_t r1 = re15_squareroot0(sxz), r2 = re15_squareroot0(sxz);              /* @0x80045b04 / @0x80045b10 */
    const int32_t dyt = c->pos_y - dy;                                                  /* `subu v1,v1,s0` @0x80045b20 */
    const uint32_t r = re15_squareroot0((uint32_t)dyt * (uint32_t)dyt + r1 * r2);      /* @0x80045b24-38 */
    uint32_t di = r / 250u;                                                             /* `0x10624dd3`, `srl 4` @0x80045b3c-4c */
    if (di > 0x7fu) di = 0x7fu;                                                         /* `slti v0,s4,128` @0x80045b50-5c */
    /* Seite @0x80045b70-c88 */
    const int32_t ah = azimut(c->pos_x, c->pos_z, pos[0], pos[2]);                      /* @0x80045b70 */
    const int32_t at = azimut(c->pos_x, c->pos_z, c->target_x, c->target_z);            /* @0x80045b88 */
    const int32_t h = ah - azimut(0, 0, c->pos_x, c->pos_z);                            /* @0x80045ba0 / `subu s0,s0,v0` @0x80045bbc */
    const int32_t t = at - azimut(0, 0, c->pos_x, c->pos_z);                            /* @0x80045bb8 / `subu a0,s1,v0` @0x80045bc0 */
    const uint32_t rel = ((int16_t)h < (int16_t)t) ? (uint32_t)(h + 0x1000 - t) : (uint32_t)(h - t);   /* @0x80045bd4-e8 */
    int voll, volr;
    const int16_t rs = (int16_t)rel;
    if (rs == 0 || rs == 0x1000 || rs == 0x800) {                                      /* @0x80045bf4-c04 */
        voll = volr = 0x89;                                                             /* @0x80045c0c-1c */
    } else {
        const int bit = (rel & 0x800u) != 0;                                            /* `andi v0,a1,0x800` @0x80045c08 */
        int spiegel = bit;                                                              /* a0 = bit, v1 = !bit @0x80045c28-3c */
        if (((rel - 0x401u) & 0xffffu) < 0x7ffu) spiegel = !bit;                        /* @0x80045c40-60 */
        int i = (int)((rel & 0x3ffu) >> 3);                                             /* @0x80045c50-54 */
        if (spiegel) i = 0x89 - i;                                                      /* @0x80045c70-7c */
        const int pan = k_pan_att[(int16_t)i];                                          /* @0x80045c80-94 */
        voll = bit ? 0x89 : pan;                                                        /* DAT_800b2824 `sh a0` @0x80045ce4 */
        volr = bit ? pan : 0x89;                                                        /* DAT_800b2826 `sh a1` @0x80045cd4 */
    }
    lage[0] = (voll - k_pan_att[K_ATT + di]) & 0x7f;                                    /* @0x80045cf0-d2c */
    lage[1] = (volr - k_pan_att[K_ATT + di]) & 0x7f;                                    /* @0x80045d30-44 */
    if (abstand) *abstand = (int)di;
    return 1;
}

/* Lage des GERADE gespielten NPC-Schritts (gesetzt nur um den re15_audio_footstep-Aufruf herum, s. npc_schritt). */
static int s_lage_haengt = 0;
static int s_lage[2] = { 0, 0 };

int re15_wassersprung_r41_lage_nehmen(int lage[2])
{
    if (!s_lage_haengt) return 0;
    lage[0] = s_lage[0]; lage[1] = s_lage[1];
    s_lage_haengt = 0;
    return 1;
}

int re15_wassersprung_r41_lage_pegel(int *vol, const int lage[2], int diag)
{
    const int voll = lage[0], volr = lage[1];
    int pegel, pan;                                         /* SsUtKeyOnV @0x80059d3c: DAT_800b5318 / DAT_800b5319 */
    if (voll == volr)     { pegel = voll; pan = RE15_WASSERSPRUNG_R41_LAGE_MITTE; }   /* @0x80059e08-20            */
    else if (volr < voll) { pegel = voll; pan = (volr << 6) / voll; }                 /* @0x80059e2c-6c            */
    else                  { pegel = volr; pan = 0x7f - (voll << 6) / volr; }          /* @0x80059e78-bc            */
    const int v = *vol * pegel / 0x7f;                      /* SpuVmKeyOnNow: Tastenpegel * Master * Prog * Ton @0x80055d24-80;
                                                             * *vol ist der Port-Pegel des Tons bei Tastenpegel 0x7f */
    int l = v, r = v;
    if (pan < RE15_WASSERSPRUNG_R41_LAGE_MITTE) r = v * pan / RE15_WASSERSPRUNG_R41_PAN_TEILER;          /* @0x80055f58-88 */
    else                                        l = v * (0x7f - pan) / RE15_WASSERSPRUNG_R41_PAN_TEILER; /* @0x80055f8c-... */
    if (diag) fprintf(stderr, "[foot] NPC-Lage voll=%d volr=%d -> Pegel %d Pan %d -> L=%d R=%d (Q15, Ton %d)\n",
                      voll, volr, pegel, pan, l, r, *vol);
    s_mess.letzter_pegel[0] = l; s_mess.letzter_pegel[1] = r;
    *vol = l;
    return r > 0 ? r : 1;                                   /* 0 hiesse im Mischer "Mono" (audio_pc.c vol_r_q15) */
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
    /* Tonindex: a0 + (Boden & 0x7f) @0x80045718-24, NPC-Zweig -8 @0x8004575c (kein Objekt-Override). Nachbesserung 2 (M1,
     * Kopf (f)): im Original nur hoerbar mit Stimme 0..7; sonst (Beta-Luecke) die RE2-Retail-Zeile a0 + (Boden & 0x7f) von
     * FUN_8005c040 (@0x8005c44c / @0x8005c458), leer -> kein Ton (@0x8005c470). Ohne EDT (kein Raum) bleibt die Original-Zeile. */
    const int ton_orig = a0 + (s0 & 0x7f) - RE15_WASSERSPRUNG_R41_NPC_TON;
    int idx = ton_orig, quelle = 0;
    if (rdt && rdt->snd_edt[0] && !ton_orig_hoerbar(rdt, ton_orig, fuss)) {
        idx = a0 + (s0 & 0x7f);
        quelle = satz_belegt(rdt, idx) ? 1 : 2;
    }
    if (quelle == 2 || idx < 0) idx = -1;
    /* M2 (Entscheid A-E4): Lage fuer den Ton = FUN_80045a64(Entity+0x34) @0x80045830-34 */
    const int32_t lpos[3] = { e->x, e->y, e->z };
    int lage[2] = { -1, -1 }, abstand = -1;
    const int mit_lage = re15_wassersprung_r41_lage(rdt, lpos, lage, &abstand);
    if (idx >= 0) {
        s_lage[0] = lage[0]; s_lage[1] = lage[1]; s_lage_haengt = mit_lage;
        re15_audio_footstep(fuss, idx);
        s_lage_haengt = 0;                                   /* nie an einen fremden (Spieler-)Schritt weitergeben */
    }
    s_mess.npc_schritte++;
    s_mess.letzter_ton = idx;
    s_mess.letzter_ton_orig = ton_orig; s_mess.letzte_quelle = (uint8_t)quelle;
    if (quelle == 0) s_mess.npc_ton_orig++; else if (quelle == 1) s_mess.npc_ton_retail++; else s_mess.npc_ton_stumm++;
    s_mess.letzte_lage[0] = lage[0]; s_mess.letzte_lage[1] = lage[1]; s_mess.letzter_abstand = abstand;
    s_mess.letzter_typ = e->type; s_mess.letzter_fuss = (uint8_t)fuss;
    {
        FILE *lf = npc_log();
        if (lf) {
            static const char *const qn[3] = { "orig", "retail", "stumm" };
            fprintf(lf, "F%u slot=%d typ=0x%02x clip=%d bild=%u fuss=%d boden=0x%02x stempel=%d zweig=%s ton=%d plaetze=%d portwahl=%u pos=(%d,%d,%d)"
                        " ton_orig=%d quelle=%s cut=%d abstand=%d lage=%d/%d\n",
                    (unsigned)g_engine.frame_count, (int)(e - g_actors), (unsigned)e->type, (int)e->motion,
                    (unsigned)e->anim_frame, fuss, (unsigned)boden, (int)e->water_y, zweig, idx, n,
                    (unsigned)s_mess.letzter_portwahl, e->x, e->y, e->z,
                    ton_orig, qn[quelle], (int)g_scd.work_vars[0x0A], abstand, lage[0], lage[1]);
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
    else if (!clip_hat_marken(an, clip) && fortbewegungs_clip(an, e->type, clip)) {   /* PORT-WAHL nur Fortbewegung */
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
        re15_actor_t *e = &g_actors[i];
        gezeichnet_t *g = &s_gez[i];
        g->ok = 0;
        /* STEMPEL +0x88 IST EIN PRO-BILD-WERT: der Entity-Takt FUN_8001a50c loescht ihn je Entity NACH dem Handler
         * (`jalr v0` @0x8001a588, `sh zero,136(v0)` @0x8001a59c), der AUTO-Pass FUN_800436a8 (@0x8001ce1c) stempelt neu,
         * wer im WATER-Satz steht. Der Port-Scan (re15_aot_scan) schreibt nur und laeuft im Schritt VOR den KI-Takten
         * (game_step_common.c) - der Bildanfang davor ist die Stelle des Loeschens. Ohne das blieb ein NPC, der einmal
         * durch einen WATER-Satz ging, fuer den Rest des Raums gestempelt (Zweig 0x06032000 auf trockenem Boden). */
        if (e->active) e->water_y = 0;
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
