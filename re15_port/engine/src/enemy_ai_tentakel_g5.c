/* ============================================================================================
 * G5-TENTAKEL (Typ 0x37, EM037) — die vier Arme des Endkampf-Birkin.
 * --------------------------------------------------------------------------------------------
 * Nutzer 2026-09-13: "Birkin hat immer noch - keine Tentakel". Im Original sind sie KEINE
 * Teile des Boss-Modells, sondern vier eigene Entities, die G5 per Routine-Wort kommandiert.
 * QUELLE: analysis/befunde_runde7_2026-09-13/em037-tentakel.md (Vollzensus des EM037-Overlays,
 * CDEMD0.EMS Sektor 0xE04, gelinkt @0x80100000) - jede Zahl hier traegt ihre @0x-Adresse.
 * PHASE 2 (2026-09-19, analysis/befunde_2026-09-19/birkin-g5.md 2.3/4.3 + Skeptiker #5/#8):
 * sub9 (Speer), sub10 (Wedeln) und sub11 (Zug) sind jetzt ECHTE Zustaende (Phasentabellen
 * @0x80100074 / @0x8010009C / @0x801000B4), mit Ankermodus, Per-Part-Einrollwinkeln (+0x6C),
 * Entity-RotX/RotZ, Spitzensonde gegen die Raumgrenzen und der Kind-Kollision (4 Segmente).
 *
 * ARCHITEKTUR DES ORIGINALS (uebernommen):
 *   - Per-Frame-Main 0x80100178; Routine-Tabelle @0x80105688; 16 Sub-Zustaende @0x801056F0.
 *   - G5 schreibt ein GANZES Wort nach +0x04 (Sender 0x80104E9C @0x80104E9C-B0):
 *         wort = (ph << 16) | (sub << 8) | routine
 *     0x301 Peitsche, 0xD01 Spiess, 0xB01/0x70B01/0x90B01 Zug, 0x901 Speer, 0xA01/0x40A01
 *     Wedeln, 0x601 Einziehen, 0xE01 Tod, +0x06-Treiber fuer den Austritt (sub8).
 *   - ANKER: die Tentakel haengen JEDEN FRAME an G5s Part 2 (der MASSE, nicht am Kriecher),
 *     Tabelle @0x80105668; Modus +0x219: 0 = voller Anker, 1 = frei, 2 = halber Anker
 *     (@0x801001E4-388). Der Anker schreibt NUR die Position (ApplyMatrixSV auf part2+0x48
 *     @0x80100348 + Translation @0x80100354-388), nie den Yaw.
 *   - HP = -1 @0x80100530: die Tentakel sind unverwundbar.
 *   - WINKELKETTE: Entity (+0x74/+0x76/+0x78) -> RotMatrix (FUN_80104BA4 @0x80104bc4), dann
 *     Part-Weltmatrizen als Kette E*L0, W0*L1, W1*L2, W2*L3 (@0x80104bd8-c14); die Part-
 *     Lokalmatrizen aus den Part-Winkeln +0x68/6A/6C (FUN_80104C30 @0x80104c30). Im Port
 *     addiert der Bone-Winkel-Haken (re15_g5_skin.h) die +0x6C-Zuschlaege auf den Keyframe.
 *   - KIND-ENTITY (+0x10E = 8, 0x80104F64): 4 Kollisionssegmente (Ctor @0x80100674-c4:
 *     Segment 0 r=800, 1..3 r=600) an part3 / part2 / part1 / Mitte(part1,part2)
 *     (@0x80104f94-5074), FUN_80034D0C gegen den Spieler, Ergebnis +0x220 |= 6*r
 *     (@0x80105078-a4). Der Port fuehrt die Segmente im Elternteil (ein Entity weniger).
 *
 * PORT-ENTSCHEIDUNGEN (dokumentiert, nicht geraten):
 *   1. SPAWN: ROOM5090 kennt keine 0x37 (sub00 hat nur 0x4D und 0x30) - das Modul spawnt die
 *      vier Aktoren beim Kampfstart selbst. Im RE2-Raum stehen sie ebenfalls nicht in der RDT
 *      (room7040 parkt alle fuenf auf (-32000,0,-32000)); die Platzierung macht auch dort der
 *      Code ueber die Ankertabelle (em037-tentakel.md 4.2/4.4).
 *   2. STRECKUNG: scaleX (part0+0x8C) wirkt im Original ueber FUN_80019CD0 (Flag 0x400) als
 *      Skalierung UM DEN PART-0-RAHMEN auf die Weltraum-Vertices des Schlauchs; der Zeichner
 *      (main.c) holt den Wert ueber re15_g5_tentakel_scale_x und streckt den geskinnten
 *      Schlauch entlang lokal X (re15_g5_skin.h). Kein uniformer render_scale mehr.
 *   3. SPIELER-ZYLINDER fuer die Kind-Kollision: RE2-Spieler-Init r=450 (`li v1,0x1c2`
 *      @0x8003bdc0 -> +0x9A/9C/90/92 @0x8003bdc4-d0), Hoehe 1530 (+0x9E @0x8003bdec),
 *      ein Segment (+0x1E8=1 @0x8003bddc). Von FUN_80034D0C ist der Kreis-/Hoehentest und
 *      der radiale Push-out portiert; der Vorzeichen-Dreh-Zweig ueber +0x46/+0x44 des Ziels
 *      (Zeilen 45-70 des Decompiles) und die 0x100000-Klemme (Z.71-86) NICHT (dokumentiert).
 *   4. ZUG-GRIFF (sub11 ph3 @0x80102d4c-dec): Schaden 15 (FUN_800401d4(15,1) @0x80102d78)
 *      + Treffer-Latch; die Opfer-Animation (Griff-Modus 5/0x105.. auf dem RE2-PL0-Rig)
 *      bleibt DEFERRED (Dossier 4.3.6).
 * ========================================================================================== */
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
#include "re15_anim_select.h"
#include "re15_math.h"
#include "re15_g5_skin.h"

#define TENT_N 4

/* Ankertabelle @0x80105668 (4 x 8 B: s16 x, y, z, pad). Zwei je Seite, je einer hoch/tief. */
static const int16_t s_tent_anker[TENT_N][3] = {
    {  965, -2243, -1933 },   /* vorn, hoch, links        */
    { 1615,  -706, -2440 },   /* weiter vorn, tief, links */
    { 1409,  -683,  2495 },   /* weiter vorn, tief, rechts*/
    {  957, -2426,  1958 },   /* vorn, hoch, rechts       */
};

/* Austritts-Clips je Index (Byte-Tabelle @0x80100048): 14 / 14 / 16 / 15. */
static const uint8_t s_tent_clip_austritt[TENT_N] = { 14, 14, 16, 15 };

/* Peitschen-Fenster (Tabelle @0x80105730, zwei Varianten a 4 s16):
 * Variante 0 (Clip 7) {30,55,60,70}, Variante 1 (Clip 8) {38,50,52,60}. */
static const int16_t s_tent_fenster[2][4] = {
    { 30, 55, 60, 70 },
    { 38, 50, 52, 60 },
};

/* RotX je Index beim Peitschen-Start (u16-Tabelle @0x80105738). */
static const int16_t s_tent_peitsch_rotx[TENT_N] = { 896, 1088, 2976, 3200 };

/* Rotationstabelle @0x80105740 (4 x 12 B, selbst gelesen: `00 03 00 00 20 00 | 00 09 00 00
 * 80 00` ...): je Index {sub10: rx, ry, rz | sub9: rx, ry, rz}; sub10 ph0 liest +0/+2/+4
 * (@0x801027a8-ec), sub9 ph0 liest +6/+8/+10 (@0x801020b8-fc). */
static const int16_t s_tent_rot10[TENT_N][3] = { {768,0,32}, {1536,0,32}, {2560,0,32}, {3584,0,32} };
static const int16_t s_tent_rot9 [TENT_N][3] = { {2304,0,128}, {2816,0,128}, {64,0,-64}, {1920,0,128} };
/* sub11 ph0 @0x80102a88-aec: Zeile = idx + 4*(rng&3), 8 B {rx, ry, rz, pad} @0x80105770
 * (selbst gelesen `80 07 00 00 80 00 00 00` = (1920,0,128) ...). */
static const int16_t s_tent_rot11[16][3] = {
    {1920,0,128}, {  64,0,-64}, {  64,0,-64}, {1536,0,128},
    {2048,0,128}, {2560,0,128}, { 896,0,128}, {2048,0,128},
    {2944,0,128}, {3072,0,128}, { 896,0,128}, {1280,0,128},
    {2304,0,128}, {2816,0,128}, {  64,0,-64}, {1920,0,128},
};

typedef struct {
    int      slot;          /* Aktor-Slot (-1 = nicht gespawnt)                    */
    uint8_t  sub, ph;       /* +0x05 / +0x06                                        */
    uint8_t  ankermodus;    /* +0x219: 0 voll / 1 frei / 2 halb                     */
    uint8_t  variante;      /* +0x16B (Peitschen-Variante)                          */
    int32_t  timer;         /* +0x158                                               */
    int32_t  t;             /* Phasenzaehler (sub8 ph4/ph6)                         */
    int32_t  scale_x;       /* part0+0x8C: 0..4096                                  */
    int32_t  rate;          /* +0x15A (s16)                                         */
    int8_t   dir;           /* +0x16A                                               */
    int16_t  part_rz[4];    /* +0x6C je Part: Zuschlag auf den Keyframe-Winkel      */
    int16_t  z21a, z21c, z21e; /* +0x21A/+0x21C/+0x21E (Zug-Laengenformel)          */
    uint16_t kontakt;       /* +0x220: Bits 2|4 = Kind-Kollision, Bit 1 = Zug haelt */
    int      getroffen;     /* Treffer-Sperre je Angriff                            */
} tent_t;

static tent_t s_tent[TENT_N];
static int    s_tent_bereit = 0;
static uint8_t s_tent_maske = 0;       /* G5 +0x228: Bit i+4 = beschaeftigt */

/* SE-Hook (dieselbe ENEMSE-Bank wie G5, Paar-Zeile 25). */
static void (*s_tent_se_fn)(int, int) = 0;
void re15_tentakel_audio_hook(void (*se)(int, int)) { s_tent_se_fn = se; }
static void tent_se(int id) { if (s_tent_se_fn) s_tent_se_fn(id, 0); }

uint8_t re15_g5_tentakel_maske(void) { return s_tent_maske; }

/* ---- Kommando-Eingang: G5s Sender 0x80104E9C schreibt ein ganzes Wort nach +0x04 ---- */
void re15_g5_tentakel_cmd(int idx, uint32_t wort)
{
    tent_t *t;
    if (idx < 0 || idx >= TENT_N) return;
    t = &s_tent[idx];
    t->sub = (uint8_t)((wort >> 8) & 0xFFu);
    t->ph  = (uint8_t)((wort >> 16) & 0xFFu);
    t->timer = 0; t->t = 0; t->getroffen = 0;
}

void re15_g5_tentakel_broadcast(uint32_t wort)
{
    int i;
    for (i = 0; i < TENT_N; i++) re15_g5_tentakel_cmd(i, wort);
}

/* ---- Spawn beim Kampfstart (Port-Entscheidung 1) ---------------------------------------- */
void re15_g5_tentakel_spawn(const re15_actor_t *g5)
{
    int i;
    if (s_tent_bereit) return;
    for (i = 0; i < TENT_N; i++) {
        int slot = re15_actor_alloc(0x37u);
        memset(&s_tent[i], 0, sizeof s_tent[i]);
        s_tent[i].slot = slot;
        s_tent[i].sub = 8; s_tent[i].ph = 0;      /* Ctor startet in sub8 (Austritt) */
        if (slot >= 0) {
            re15_actor_t *e = &g_actors[slot];
            e->type = 0x37u; e->active = 1; e->flags = 1;
            e->hp = -1;                            /* unverwundbar @0x80100530 */
            e->motion = (int16_t)s_tent_clip_austritt[i];
            e->anim_frame = 0;
            e->rot_x = 0; e->rot_z = 0;
            e->rot_y = g5 ? g5->rot_y : 0;
            e->render_scale_q12 = 0;               /* Streckung laeuft ueber das Skinning */
            e->grid_id = 0;
            re15_g5_bone_add_clear(slot);
        }
    }
    s_tent_maske = 0;
    s_tent_bereit = 1;
}

void re15_g5_tentakel_reset(void)
{
    int i;
    for (i = 0; i < TENT_N; i++) {
        int slot = s_tent[i].slot;
        /* ⛔ NUR EINEN ECHTEN ARM ABSCHALTEN (Nutzer-Befund 2026-09-13: "es taucht immer
         * noch kein Birkin auf, + ich haenge in der cutscene fest").
         *
         * s_tent[] ist als `static` NULL-initialisiert - `slot` ist vor dem ersten Spawn
         * also 0, und Slot 0 ist per Definition der SPIELER (re15_actor.h:23
         * RE15_ACTOR_SLOT_PLAYER). Die alte Schleife hat beim ersten Reset viermal
         * `g_actors[0].active = 0` geschrieben und damit Leon abgeschaltet: der Raum fror
         * ein (befund.log 2026-09-13: Cut 12 bzw. 15, Position konstant ueber 110-575
         * Messzeilen), und weil der Boss-Tick den toten Spieler-Aktor las, erschien auch
         * Birkin nicht. Aufgefallen ist es erst, als der Reset ueberhaupt gerufen wurde -
         * bis zum selben Tag stand er nur im Test (das war der Runde-8-Fix).
         *
         * Zwei Riegel statt einem: Slot 0 ist NIE ein Arm, und der Aktor muss auch
         * wirklich noch der 0x37-Arm sein - sonst gehoert der Platz inzwischen jemand
         * anderem und das Abschalten waere genau der Fehler, den der Reset verhindern
         * soll. */
        if (slot > 0 && slot < RE15_ACTOR_MAX && g_actors[slot].type == 0x37u) {
            g_actors[slot].active = 0;
            re15_g5_bone_add_clear(slot);
        }
        s_tent[i].slot = -1;
    }
    s_tent_bereit = 0; s_tent_maske = 0;
}

/* ---- Anker: Position aus G5s Part-2-(Blob-)Matrix (@0x801001E4-388) --------------------- */
static void tent_anker(tent_t *t, int idx, const re15_actor_t *g5, re15_actor_t *e)
{
    int32_t v[3];
    int32_t cs, sn;
    if (t->ankermodus == 1u) return;               /* frei - keine Nachfuehrung */
    v[0] = s_tent_anker[idx][0];
    v[1] = s_tent_anker[idx][1];
    v[2] = s_tent_anker[idx][2];
    if (t->ankermodus == 2u) {                     /* halber Anker @0x80100290-33C */
        v[1] = s_tent_anker[idx][1] / 2 - ((idx == 1 || idx == 2) ? 1500 : 1300);
        v[2] = s_tent_anker[idx][2] / 2;
    }
    /* Part 2 = die Masse: Bind (1800,4500,0) am WURZELKNOCHEN des Kriechers
     * (main.c-Komposition, g5-optik.md @0x80100670-740). Die Weltlage ist also
     * WURZELPOSE + Rot(yaw)*(Bind + Anker) - und die Wurzel haengt 3,1-4,5 m
     * UEBER dem Entity-Ursprung (alle EM036-Keyframes tragen rootY -4536..-3105).
     * ⛔ GEMESSEN 2026-09-13 (Nutzer-Marke R5090 F550): ohne diese Wurzelhoehe
     * landeten die vier Arme bei y = +2057..+3816, also 2-4 m UNTER dem Boden.
     * re15_enemy_bone_world_pos liefert die gerenderte Wurzel samt rootY. */
    {
        int32_t w[3];
        re15_enemy_bone_world_pos(g5, 0, w);
        v[0] += 1800; v[1] += 4500;
        cs = re15_cos_q12((int)g5->rot_y);
        sn = re15_sin_q12((int)g5->rot_y);
        e->x = w[0] + (int32_t)(((int64_t)cs * v[0] + (int64_t)sn * v[2]) >> 12);
        e->z = w[2] + (int32_t)((-(int64_t)sn * v[0] + (int64_t)cs * v[2]) >> 12);
        e->y = w[1] + v[1];
    }
}

/* ---- Posen / Weltpositionen der Kette ------------------------------------------------------
 * Pose im QUERY-Modus (Crossfade unangetastet), aber MIT dem Bone-Winkel-Haken (die
 * +0x6C-Zuschlaege gehoeren zur Kette). Entity-Matrix = RotMatrix(rx, ry, rz) wie
 * FUN_80104BA4 @0x80104bc4 (`jal 0x8008e1f4` auf +0x74). */
static int tent_posen(const re15_actor_t *e, re15_skel_pose_t poses[RE15_EMD_MAX_BONES],
                      int32_t E[9])
{
    re15_enemy_bank_t *b = re15_enemy_find(0x37u);
    int kf, rv;
    void *sav;
    if (!b || !b->ok || b->skel.bone_count < 4) return -1;
    kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
    sav = g_anim_pose_actor;
    g_anim_pose_actor = NULL;
    g_anim_query_actor = (void *)e;
    rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_query_actor = NULL;
    g_anim_pose_actor = sav;
    if (rv != 0) return -1;
    re15_skel_euler_matrix((int)e->rot_x, (int)e->rot_y, (int)e->rot_z, E);
    return 0;
}

static void tent_welt(const re15_actor_t *e, const int32_t E[9], const int32_t p[3], int32_t out[3])
{
    out[0] = e->x + (int32_t)(((int64_t)E[0]*p[0] + (int64_t)E[1]*p[1] + (int64_t)E[2]*p[2]) >> 12);
    out[1] = e->y + (int32_t)(((int64_t)E[3]*p[0] + (int64_t)E[4]*p[1] + (int64_t)E[5]*p[2]) >> 12);
    out[2] = e->z + (int32_t)(((int64_t)E[6]*p[0] + (int64_t)E[7]*p[1] + (int64_t)E[8]*p[2]) >> 12);
}

/* Spitzensonde (@0x80102264-2cc / @0x80102c70-cd4): FUN_8008DBA4(part3+0x48, (1500,0,0)) +
 * part3-Welt-Y/Z. Rueckgabe 0 ok. */
static int tent_spitze_welt(const re15_actor_t *e, int32_t out[3])
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t E[9], l[3], m[3], w3[3];
    if (tent_posen(e, poses, E) != 0) { out[0] = e->x; out[1] = e->y; out[2] = e->z; return -1; }
    /* R3*(1500,0,0) im Modellraum, dann Entity-Rotation, plus part3-Welt-Translation. */
    l[0] = (poses[3].rot[0] * 1500) >> 12;
    l[1] = (poses[3].rot[3] * 1500) >> 12;
    l[2] = (poses[3].rot[6] * 1500) >> 12;
    m[0] = poses[3].trans[0] + l[0]; m[1] = poses[3].trans[1] + l[1]; m[2] = poses[3].trans[2] + l[2];
    tent_welt(e, E, m, w3);
    out[0] = w3[0]; out[1] = w3[1]; out[2] = w3[2];
    return 0;
}

/* Diagnose/Test: Weltposition der Spitze eines Tentakels (-1 = kein Tentakel). */
int re15_g5_tentakel_spitze(int idx, int32_t out[3])
{
    if (idx < 0 || idx >= TENT_N) return -1;
    if (s_tent[idx].slot < 0 || s_tent[idx].slot >= RE15_ACTOR_MAX) return -1;
    return tent_spitze_welt(&g_actors[s_tent[idx].slot], out);
}

/* Zeichner-Schnittstelle: Streckung part0+0x8C dieses Arms (4096 = 1.0; 0 = eingezogen). */
int32_t re15_g5_tentakel_scale_x(int slot)
{
    int i;
    for (i = 0; i < TENT_N; i++)
        if (s_tent[i].slot == slot) return s_tent[i].scale_x;
    return 0x1000;
}

/* Diagnose: Zustand eines Arms. */
int re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus, int32_t *scale_x,
                             uint16_t *kontakt)
{
    if (idx < 0 || idx >= TENT_N) return -1;
    if (sub) *sub = s_tent[idx].sub;
    if (ph) *ph = s_tent[idx].ph;
    if (ankermodus) *ankermodus = s_tent[idx].ankermodus;
    if (scale_x) *scale_x = s_tent[idx].scale_x;
    if (kontakt) *kontakt = s_tent[idx].kontakt;
    return 0;
}

/* ---- Kind-Kollision (0x80104F64 + FUN_80034D0C) --------------------------------------------
 * Segmente: 0 = part3-Welt (r 800), 1 = part2 (600), 2 = part1 (600), 3 = Mitte(part1,part2)
 * (600) (@0x80104f94-5074, Ctor @0x80100674-c4). Spieler r 450 / h 1530 (Port-Entscheidung 3).
 * Rueckgabe = FUN_80034D0C-Ergebnis (1 = Kontakt, Spieler herausgeschoben). */
static int tent_kind_kollision(re15_actor_t *e, re15_actor_t *pl)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t E[9], w1[3], w2[3], w3[3];
    int32_t seg[4][3]; uint16_t r[4] = { 800, 600, 600, 600 };
    int k, hit = 0;
    if (tent_posen(e, poses, E) != 0) return 0;
    tent_welt(e, E, poses[1].trans, w1);
    tent_welt(e, E, poses[2].trans, w2);
    tent_welt(e, E, poses[3].trans, w3);
    memcpy(seg[0], w3, sizeof w3);
    memcpy(seg[1], w2, sizeof w2);
    memcpy(seg[2], w1, sizeof w1);
    seg[3][0] = w1[0] + ((int16_t)(w2[0] - w1[0]) >> 1);   /* sll 16 / sra 17 @0x80105024-38 */
    seg[3][1] = w1[1] + ((int16_t)(w2[1] - w1[1]) >> 1);
    seg[3][2] = w1[2] + ((int16_t)(w2[2] - w1[2]) >> 1);
    for (k = 0; k < 4; k++) {
        int32_t rs = (int32_t)r[k] + 450;                  /* r_a + r_b */
        int32_t dx = pl->x - seg[k][0];
        int32_t dz = pl->z - seg[k][2];
        int32_t dy, hs, dist, over;
        if ((uint32_t)(dx + rs) > (uint32_t)(rs * 2)) continue;     /* Broadphase X */
        if ((uint32_t)(dz + rs) > (uint32_t)(rs * 2)) continue;     /* Broadphase Z */
        dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        over = rs - dist;
        if (over <= 0) continue;
        dy = pl->y - seg[k][1];
        hs = 0 + 1530;                                     /* Segment-Hoehe 0 (nicht gesetzt) + Spieler */
        if (!(-hs < dy && dy < hs)) continue;
        {   /* radialer Push-out (Decompile Z.38-44), ohne Dreh-Zweig/Klemme (s. Kopf). */
            int32_t d1 = dist + 1;
            int32_t px = (dx * over) / d1, pz = (dz * over) / d1;
            pl->x += px; pl->z += pz;
        }
        hit = 1;
    }
    return hit;
}

static int tent_anim(re15_actor_t *e, tent_t *t)
{
    extern int re15_actor_clip_len(const re15_actor_t *a);
    int fc = re15_actor_clip_len(e);
    /* Der Anim-Schritt (0x8002959c) schreibt die Part-Winkel neu -> die +0x6C-Zuschlaege
     * der Einroll-Phasen sind damit weg. */
    memset(t->part_rz, 0, sizeof t->part_rz);
    if (fc <= 0) fc = 1;
    if ((int)e->anim_frame + 1 >= fc) { e->anim_frame = 0; return 1; }
    e->anim_frame++;
    return 0;
}

static void tent_clip(re15_actor_t *e, tent_t *t, int clip, int frame)
{
    extern int re15_actor_clip_len(const re15_actor_t *a);
    int fc;
    e->motion = (int16_t)clip;
    e->anim_frame = 0;
    fc = re15_actor_clip_len(e);
    if (fc > 0 && frame > 0) e->anim_frame = (uint32_t)(frame % fc);
    memset(t->part_rz, 0, sizeof t->part_rz);
}

/* Gesamtwinkel-Klemme (@0x80102458-4fc / @0x80102ef4-f98): (RotZ + Sum part.rz) & 0xfff
 * in [2049, 4095] -> letzten Schritt zuruecknehmen, rate = 0, dir = 0. */
static void tent_einroll_schritt(re15_actor_t *e, tent_t *t)
{
    int32_t r = t->rate;
    t->part_rz[0] = (int16_t)(t->part_rz[0] + r);           /* part0.rz += 15A */
    t->part_rz[1] = (int16_t)(t->part_rz[1] - r);           /* part1.rz -= 15A */
    t->part_rz[2] = (int16_t)(t->part_rz[2] - r);           /* part2.rz -= 15A */
    t->part_rz[3] = (int16_t)(t->part_rz[3] - (((int16_t)r) >> 1)); /* part3.rz -= 15A>>1 */
    t->rate = (int16_t)(t->rate - 2 * t->dir);              /* 15A -= 2*16A */
    if ((int16_t)t->rate == 0) t->timer = 0;
    {
        int32_t sum = ((int32_t)e->rot_z + t->part_rz[0] + t->part_rz[1] + t->part_rz[2] +
                       t->part_rz[3]) & 0xfff;
        if ((uint32_t)(sum - 2049) < 0x7ffu) {
            r = t->rate;
            t->part_rz[0] = (int16_t)(t->part_rz[0] - r);
            t->part_rz[1] = (int16_t)(t->part_rz[1] + r);
            t->part_rz[2] = (int16_t)(t->part_rz[2] + r);
            t->part_rz[3] = (int16_t)(t->part_rz[3] + (((int16_t)r) >> 1));
            t->rate = 0; t->dir = 0;
        }
    }
}

/* Ausroll-/Zitter-Folge (sub9 ph7 @0x80102508-268c, sub11 ph7 @0x80103010-31c8):
 * f_jit0/1 = Zitterfenster, f_out = ab wann ausrollen, f_end = Phasenende. */
static void tent_ausrollen(re15_actor_t *e, tent_t *t, int f_jit0, int f_jit1, int f_out)
{
    int32_t tt = t->timer;
    if (tt < 10) {                                          /* @0x8010250c-70 */
        t->part_rz[0] = (int16_t)(t->part_rz[0] - 2 * tt);
        t->part_rz[1] = (int16_t)(t->part_rz[1] + 2 * tt);
        t->part_rz[2] = (int16_t)(t->part_rz[2] + 2 * tt);
        t->part_rz[3] = (int16_t)(t->part_rz[3] + tt);
    }
    if ((uint32_t)(tt - f_jit0) < (uint32_t)(f_jit1 - f_jit0 + 1)) {   /* Zittern +-(rng&0xF) */
        int32_t s = 1 - 2 * (tt & 1);
        e->x += (int32_t)(re15_engine_rand8() & 0xF) * s;
        e->y += (int32_t)(re15_engine_rand8() & 0xF) * s;
        e->z += (int32_t)(re15_engine_rand8() & 0xF) * s;
    }
    if (tt >= f_out) {                                      /* @0x8010262c-58 */
        t->part_rz[0] = (int16_t)(t->part_rz[0] + 4);
        t->part_rz[1] = (int16_t)(t->part_rz[1] - 4);
        t->part_rz[2] = (int16_t)(t->part_rz[2] - 4);
        t->part_rz[3] = (int16_t)(t->part_rz[3] - 2);
    }
}

/* Zug-Laengenformel (@0x80102fd0-3004 / @0x80103184-b8): scaleX = 21C * (((21A - X) << 8)
 * >> 13) >> 8, mit Rundung gegen -unendlich korrigiert (bgez/addiu 8191). */
static void tent_zug_laenge(const re15_actor_t *e, tent_t *t)
{
    int32_t v = ((int32_t)t->z21a - e->x) << 8;
    if (v < 0) v += 8191;
    v >>= 13;
    t->scale_x = (int16_t)(((int32_t)t->z21c * v) >> 8);
}

/* Spitzensonde gegen die Raumgrenzen (@0x801022a4-340 / @0x80102cb8-d48): nur bei 16A == 0. */
static void tent_sonde(re15_actor_t *e, tent_t *t)
{
    int32_t tip[3];
    if (tent_spitze_welt(e, tip) != 0) return;
    if (t->dir != 0) return;
    if (tip[1] > 0)      t->ph = 4;   /* blez -> Boden */
    if (tip[1] < -5000)  t->ph = 4;
    if (tip[2] < -27000) t->ph = 4;
    if (tip[2] >= -19799) t->ph = 4;
}

static void tent_treffer(re15_actor_t *pl, tent_t *t, int schaden)
{
    t->getroffen = 1;
    pl->hp = (int16_t)(pl->hp - schaden);
    if (pl->hp < 0) pl->hp = -1;
    pl->hit_react |= 1;
    tent_se(3);                                             /* @0x80101790 */
}

/* ---- Ein Tentakel je Tick ---------------------------------------------------------------- */
static void tent_tick(int idx, const re15_actor_t *g5, re15_actor_t *pl)
{
    tent_t *t = &s_tent[idx];
    re15_actor_t *e;
    int k;
    if (t->slot < 0 || t->slot >= RE15_ACTOR_MAX) return;
    e = &g_actors[t->slot];
    if (!e->active) return;

    /* KIND-KOLLISION jeden Frame VOR dem Zustand (Main 0x801001a0-c8: das Kind laeuft als
     * eigener Entity-Tick; +0x220 &= ~6, dann |= 6*r @0x80105078-a4). Nur mit sichtbarer
     * Streckung (scale 0 = Modell in der Masse). */
    t->kontakt &= (uint16_t)~6u;
    if (t->scale_x > 0 && tent_kind_kollision(e, pl)) t->kontakt |= 6u;

    switch (t->sub) {
    case 0:                                        /* IDLE 0x80100A44 */
        if (t->ph == 0) {
            tent_clip(e, t, 0, 0);
            t->ph = 1;
            t->timer = 60 + (re15_engine_rand8() & 0x3F);   /* @0x80100A5C-9C */
            t->scale_x = 0;                                  /* Modell aus (parts.flags &= ~1) */
        }
        break;

    case 8:                                        /* AUSTRITT/BEBEN @0x80101D38ff */
        switch (t->ph) {
        case 0:
            tent_clip(e, t, s_tent_clip_austritt[idx], 0);
            t->scale_x = 0;
            e->rot_x = 0; e->rot_z = 0; e->rot_y = g5->rot_y;
            if (idx == 1) t->ankermodus = 2;       /* @0x80101D60-78 */
            break;                                  /* G5 treibt +0x06 weiter */
        case 1:
            tent_se(0); t->ph = 2; t->ankermodus = 1;   /* @0x80101D90 */
            break;
        case 2:                                     /* @0x80101DAC: 16 Frames heraus */
            t->scale_x += 256;
            if (t->scale_x > 4096) t->scale_x = 4096;
            if (tent_anim(e, t)) t->ph = 3;
            break;
        case 3:
            tent_clip(e, t, 0, 0);                 /* Clip-Wort 0x00070000 */
            t->timer = 0; t->ph = 4; tent_se(2);
            break;
        case 4:                                     /* @0x80101E24 */
            if (t->t < 5)               e->x -= t->t * 10;
            else if (t->t <= 34)        e->x += 10;
            if (t->t == 36) t->ph = 5;
            t->t++;
            tent_anim(e, t);
            break;
        case 5: t->timer = 0; break;               /* warten - G5 treibt */
        case 6: {                                   /* Zittern @0x80101E94 */
            int32_t z = (int32_t)(re15_engine_rand8() & 0xF) * (1 - 2 * (t->t & 1));
            if (t->t < 20) e->x -= t->t;
            if (t->t >= 16 && t->t <= 179) { e->x += z; e->y += z; e->z += z; }
            if (t->t >= 161) e->x += 5;
            if (t->t == 180) t->ph = 7;
            t->t++;
            tent_anim(e, t);
            break;
        }
        case 8:                                     /* Einzug @0x80101FA8 */
            t->scale_x -= 64;
            e->rot_y = (int16_t)((e->rot_y + ((idx < 2) ? -16 : 16)) & 0xfff);
            if (t->scale_x < 2048) { t->scale_x = 0; tent_se(0); t->ph = 9; }
            tent_anim(e, t);
            break;
        default: break;                             /* 7/9: warten bzw. verborgen */
        }
        break;

    case 3:                                        /* PEITSCHE (0x301) @0x80101404ff */
        switch (t->ph) {
        case 0:
            tent_clip(e, t, 0, 0);
            e->rot_z = -576;                        /* @0x8010143c-40 (+0x78) */
            e->rot_y = g5->rot_y;                   /* sh zero,118 @0x80101444: Yaw 0 = G5-Achse */
            e->rot_x = s_tent_peitsch_rotx[idx];    /* @0x80101454-5c */
            t->scale_x = 0; t->ph = 1;
            s_tent_maske |= (uint8_t)(1u << (idx + 4));   /* +0x228 belegt */
            tent_se(0);
            break;
        case 1:
            t->scale_x += 64;
            if (t->scale_x >= 2025 && e->rot_z < 32) e->rot_z = (int16_t)(e->rot_z + 32);
            if (t->scale_x >= 3073) {               /* @0x801014B8-D4 */
                t->variante = (uint8_t)(re15_engine_rand8() & 1u);
                tent_clip(e, t, 7 + t->variante, 0);
                t->ph = 2;
            }
            tent_anim(e, t);
            break;
        case 2:
        case 3: {
            const int16_t *F = s_tent_fenster[t->variante & 1u];
            int fr = (int)e->anim_frame;
            if (t->ph == 2) {
                t->scale_x += 64;
                if (t->scale_x >= 4097) {           /* @0x80101544-68 */
                    t->scale_x = 4096; t->ph = 3;
                    t->timer = (re15_engine_rand8() & 0xF) + 10;
                }
            } else if (tent_anim(e, t)) {           /* @0x801015D8-E8 */
                tent_clip(e, t, 0, 0); t->ph = 4;
                break;
            }
            if (fr > F[0] && fr < F[1]) e->rot_z = (int16_t)(e->rot_z + 8);
            if (fr > F[2] && fr < F[3]) {           /* TREFFER-FENSTER @0x80101640-94 */
                e->rot_z = (int16_t)(e->rot_z + 4);
                /* `+0x220 & 4` @0x80101684-94 = die Kind-Kollision dieses Bildes. */
                if (!t->getroffen && pl->hit_react == 0 && g5->hp >= 0 && (t->kontakt & 4u))
                    tent_treffer(pl, t, 15);                 /* 15 @0x801016D0 */
            }
            if (t->ph == 2) tent_anim(e, t);
            break;
        }
        default:                                    /* ph4: einziehen @0x801017BC */
            t->scale_x -= 64;
            if (t->scale_x < 0) {
                t->scale_x = 0; t->sub = 0; t->ph = 0;
                s_tent_maske &= (uint8_t)~(1u << (idx + 4));
            }
            tent_anim(e, t);
            break;
        }
        break;

    case 13:                                       /* SPIESS (0xD01) @0x801035BCff */
        switch (t->ph) {
        case 0:
            tent_clip(e, t, (idx == 0 || idx == 3) ? 23 : 21, 0);
            e->rot_x = 0; e->rot_z = 0; e->rot_y = g5->rot_y;
            t->scale_x = 0; t->ph = 1;
            s_tent_maske |= (uint8_t)(1u << (idx + 4));
            tent_se(0);
            break;
        case 1:
            t->scale_x += 128;
            if (t->scale_x > 4096) t->scale_x = 4096;
            {   int fr = (int)e->anim_frame;
                /* `+0x220 & 4` @0x801036f4-fc im Fenster 27..31. */
                if (fr > 26 && fr < 32 && !t->getroffen && pl->hit_react == 0 &&
                    g5->hp >= 0 && (t->kontakt & 4u))
                    tent_treffer(pl, t, 15);                 /* 15 @0x80103730 */
            }
            if (tent_anim(e, t)) t->ph = 5;
            break;
        default:                                    /* ph5: fertig @0x801038E0 */
            t->scale_x -= 96;
            if (t->scale_x < 0) {
                t->scale_x = 0; t->sub = 0; t->ph = 0;
                s_tent_maske &= (uint8_t)~(1u << (idx + 4));
            }
            tent_anim(e, t);
            break;
        }
        break;

    case 9:                                        /* SPEER (0x901) @0x8010204cff, Tabelle @0x80100074 */
        switch (t->ph) {
        case 0:                                     /* @0x8010209c */
            tent_clip(e, t, 0, 0);                 /* sw zero,332 */
            e->rot_x = s_tent_rot9[idx][0];        /* @0x801020b8-c0 (+0x74) */
            e->rot_y = (int16_t)((g5->rot_y + s_tent_rot9[idx][1]) & 0xfff);   /* +0x76 = 0 */
            e->rot_z = s_tent_rot9[idx][2];        /* @0x801020fc/120 (+0x78) */
            t->rate = 64;                          /* @0x80102100-04 */
            t->ankermodus = 2;                     /* @0x8010210c */
            t->dir = 0; t->timer = 0;              /* @0x80102114-18 */
            t->ph = 1;
            t->scale_x = 0;                        /* @0x8010212c */
            tent_se(0);                            /* @0x8010213c */
            break;
        case 1:                                     /* @0x80102144 */
            tent_anim(e, t);
            t->scale_x += t->timer; t->timer += 8;
            if (t->scale_x >= 3073) { t->ph = 2; tent_clip(e, t, 2, 0); }   /* 0x1F0002 */
            e->rot_z = (int16_t)(e->rot_z - t->rate); t->rate = (int16_t)(t->rate - 4);
            break;
        case 2:                                     /* @0x801021c0 */
            tent_anim(e, t);
            t->scale_x += t->timer; t->timer += 16;
            if (t->scale_x >= 4097) { t->scale_x = 4096; t->ph = 3; tent_se(15); }
            /* FALLTHROUGH in den ph3-Koerper (@0x80102224) */
            e->rot_z = (int16_t)(e->rot_z - t->rate);
            if (t->ph == 3) t->rate = (int16_t)(t->rate - 4);
            tent_sonde(e, t);
            break;
        case 3:                                     /* @0x80102224 */
            e->rot_z = (int16_t)(e->rot_z - t->rate);
            t->rate = (int16_t)(t->rate - 4);
            tent_sonde(e, t);
            break;
        case 4:                                     /* @0x80102344 */
            tent_se(2);
            t->rate = 64; t->timer = 60; t->ph = 5; t->dir = 1;
            t->ankermodus = 1;                     /* @0x80102370 */
            t->kontakt = 7;                        /* sb v0=7,546 (+0x222) */
            break;
        case 5: {                                   /* @0x8010237c: Einrollen */
            int32_t was = t->timer;
            t->timer--;
            if (was == 0) {
                t->ph = 6; tent_clip(e, t, 0, 0);   /* 0x3F0000 */
                t->timer = 8;
                t->z21c = (int16_t)t->scale_x;
                /* +0x21A = part3.WeltX - part0.WeltX, +0x21E = part0.WeltX: nur Diagnose in
                 * sub9 (kein Leser im Speer) -> nicht gefuehrt. */
            }
            tent_einroll_schritt(e, t);
            break;
        }
        case 6:                                     /* @0x80102500: halten, timer = 0 */
            t->timer = 0;
            break;
        case 7:                                     /* @0x80102508: ausrollen/zittern, 8..174, >=166 */
            tent_ausrollen(e, t, 8, 174, 166);
            if (t->timer == 180) t->ph = 8;
            t->timer++;
            break;
        case 9:                                     /* @0x80102690 (Tabelle [9]): einziehen */
            tent_anim(e, t);
            t->scale_x -= t->timer; t->timer += 4;
            e->rot_z = (int16_t)(e->rot_z + 8);
            if (t->scale_x < 0) { t->scale_x = 0; t->ph = 10; tent_se(0); }
            break;
        default:                                    /* ph8 (@0x801026f4 = leer), ph10 = fertig */
            break;
        }
        break;

    case 10:                                       /* WEDELN (0xA01) @0x8010271cff, Tabelle @0x8010009C */
        switch (t->ph) {
        case 0:                                     /* @0x8010276c */
            tent_clip(e, t, 0, 0);
            t->ph = 1;
            e->rot_x = s_tent_rot10[idx][0];       /* @0x801027a8-b0 */
            e->rot_y = (int16_t)((g5->rot_y + s_tent_rot10[idx][1]) & 0xfff);
            e->rot_z = s_tent_rot10[idx][2];       /* @0x801027ec-fc */
            t->ankermodus = 0;                     /* sb zero,537 @0x801027f4 */
            tent_se(0);
            break;
        case 1:                                     /* @0x80102800 */
            tent_anim(e, t);
            t->scale_x += 64;
            if ((int16_t)t->scale_x >= 2049) {
                t->ph = 2;
                tent_clip(e, t, 9 + (idx % 3), 0); /* 0x1F0009 + idx%3 @0x8010283c-74 */
            }
            break;
        case 2:                                     /* @0x80102878 */
            tent_anim(e, t);
            t->scale_x += 64;
            if ((int16_t)t->scale_x >= 4097) {
                t->scale_x = 4096; t->ph = 3;
                t->timer = (re15_engine_rand8() & 0xF) + 30;   /* @0x801029fc-a0c */
            }
            break;
        case 3: {                                   /* @0x801028c0 */
            int32_t was;
            tent_anim(e, t);
            was = t->timer; t->timer--;
            if (was == 0) {
                unsigned a = re15_engine_rand8() & 0xFFu;      /* @0x801028e8: Startbild */
                unsigned b = re15_engine_rand8() & 0xFFu;      /* @0x801028f0: Clip       */
                tent_clip(e, t, 9 + (int)(b % 3u), (int)(a % 90u));   /* 0x3F0009 + b%3, Bild a%90 */
                t->timer = (re15_engine_rand8() & 0xF) + 60;   /* @0x8010296c-7c */
            }
            break;
        }
        case 4:                                     /* @0x80102980: einmal Peitsche 7/8 */
            t->ph = 5;
            tent_clip(e, t, 7 + (re15_engine_rand8() & 1), 0);   /* 0x1F0007 + rng&1 */
            /* FALLTHROUGH (@0x801029a0) */
        case 5:                                     /* @0x801029a0 */
            if (tent_anim(e, t)) {
                t->ph = 3;
                tent_clip(e, t, 9 + (int)((re15_engine_rand8() & 0xFFu) % 3u), 0);
                t->timer = (re15_engine_rand8() & 0xF) + 30;
            }
            break;
        default: break;
        }
        break;

    case 11: {                                     /* ZUG (0xB01/0x70B01/0x90B01) @0x80102a38ff, Tabelle @0x801000B4 */
        int fall = 0;
        switch (t->ph) {
        case 0: {                                   /* @0x80102a88 */
            int row = idx + 4 * (int)(re15_engine_rand8() & 3u);
            tent_clip(e, t, 0, 0);
            e->rot_x = s_tent_rot11[row][0];       /* @0x80102aac/c0 */
            e->rot_y = (int16_t)((g5->rot_y + s_tent_rot11[row][1]) & 0xfff);
            e->rot_z = s_tent_rot11[row][2];       /* @0x80102aec/b0c */
            t->rate = 64; t->dir = 0; t->timer = 0;
            t->ankermodus = 0;                     /* sb zero,537 @0x80102b04 */
            t->ph = 1;
            t->scale_x = 0;
            tent_se(0);
            /* Die Zugstraenge bleiben "frei" (Maske GELOESCHT @0x80102B28-4C): sie ziehen G5
             * heran, blockieren aber keinen Angriffsplatz. */
            s_tent_maske &= (uint8_t)~(1u << (idx + 4));
            fall = 1;                              /* faellt in ph1 durch (@0x80102b50) */
        }
        /* FALLTHROUGH */
        case 1:                                     /* @0x80102b50 */
            (void)fall;
            tent_anim(e, t);
            t->scale_x += t->timer; t->timer += 8;
            if (t->scale_x >= 3073) { t->ph = 2; tent_clip(e, t, 2, 0); }
            e->rot_z = (int16_t)(e->rot_z - t->rate); t->rate = (int16_t)(t->rate - 4);
            break;
        case 2:                                     /* @0x80102bcc */
            tent_anim(e, t);
            t->scale_x += t->timer; t->timer += 16;
            if (t->scale_x >= 4097) { t->scale_x = 4096; t->ph = 3; tent_se(15); }
            /* FALLTHROUGH in den ph3-Koerper */
            e->rot_z = (int16_t)(e->rot_z - t->rate);
            if (t->ph == 3) t->rate = (int16_t)(t->rate - 4);
            tent_sonde(e, t);
            if (t->ph == 3) goto zug_griff;
            break;
        case 3:                                     /* @0x80102c30 */
            e->rot_z = (int16_t)(e->rot_z - t->rate);
            t->rate = (int16_t)(t->rate - 4);
            tent_sonde(e, t);
        zug_griff:
            /* GRIFF-Fenster @0x80102d4c-dec: `+0x220 & 2` und Spieler-Latch 0 -> Schaden 15
             * (FUN_800401d4(15,1) @0x80102d78); Griff-Modus/Opfer-Anim DEFERRED. */
            if ((t->kontakt & 2u) && !t->getroffen && pl->hit_react == 0 && g5->hp >= 0)
                tent_treffer(pl, t, 15);
            break;
        case 4:                                     /* @0x80102df4 */
            tent_se(2);
            t->rate = 64; t->timer = 60; t->ph = 5; t->dir = 1;
            t->kontakt = (uint16_t)(7u | 0x100u);  /* +0x222 = 7, +0x223 = 1 */
            break;
        case 5: {                                   /* @0x80102e2c */
            int32_t was = t->timer;
            t->timer--;
            if (was == 0) {
                t->ph = 6; tent_clip(e, t, 0, 0);   /* 0x3F0000 */
                t->timer = 8;
                t->z21a = (int16_t)(e->x + 8192);   /* lw 56 / addiu 8192 @0x80102e50-60 */
                t->z21c = (int16_t)t->scale_x;      /* @0x80102e64-6c */
            }
            tent_einroll_schritt(e, t);
            break;
        }
        case 6:                                     /* @0x80102f9c: Zug haelt, Laengenformel */
            t->kontakt |= 1u;                      /* +0x220 |= 1 */
            tent_zug_laenge(e, t);
            t->timer = 0;
            break;
        case 7:                                     /* @0x80103010: ausrollen 6..79, >=81, ==90 */
            tent_ausrollen(e, t, 6, 79, 81);
            if (t->timer == 90) t->ph = 8;
            tent_zug_laenge(e, t);
            t->timer++;
            break;
        case 8:                                     /* @0x801031cc */
            t->dir = 0;
            tent_clip(e, t, 0, 0);                 /* 0x3F0000 */
            tent_zug_laenge(e, t);
            break;
        case 9:                                     /* @0x80103210: loslassen, wieder strecken */
            e->rot_z = (int16_t)(e->rot_z - 1);
            t->scale_x += 128;
            if ((int16_t)t->scale_x > t->z21c) {
                t->scale_x = t->z21c;
                t->ph = 10;
                t->kontakt &= (uint16_t)~1u;
                tent_se(0);
            }
            tent_anim(e, t);                       /* @0x8010328c-9c */
            break;
        case 10:                                    /* @0x801032a8: einziehen -> sub0 */
            tent_anim(e, t);
            t->scale_x -= t->timer; t->timer += 1;
            e->rot_z = (int16_t)(e->rot_z - 1);
            if (t->scale_x < 0) { t->scale_x = 0; t->sub = 0; t->ph = 0; }   /* sw v0=1,4 */
            break;
        default: break;
        }
        break;
    }

    case 6:                                        /* EINZIEHEN (0x601) @0x80101B80 */
        if (t->ph == 0) { tent_clip(e, t, 0, 0); t->ph = 1; }
        else {
            t->scale_x -= 64;
            if (t->scale_x < 0) { t->scale_x = 0; t->sub = 0; t->ph = 0; }
            tent_anim(e, t);
        }
        break;

    case 14:                                       /* TOD (0xE01) @0x801039B0 */
        if (t->ph == 0) {
            s_tent_maske |= (uint8_t)(1u << (idx + 4));   /* bleibt fuer immer gesetzt */
            t->ankermodus = 1;                     /* @0x80103b1c */
            t->ph = 1;
        }
        if (t->scale_x > 0) {
            t->scale_x -= 32;
            if (t->scale_x < 0) t->scale_x = 0;
        }
        tent_anim(e, t);
        break;

    default:                                       /* 12: ueber Timer laufen lassen */
        if (t->scale_x < 4096) t->scale_x += 96;
        if (tent_anim(e, t)) { t->sub = 0; t->ph = 0; }
        break;
    }

    /* Anker (Position an der Masse) + die Part-Winkel-Zuschlaege an den Pose-Builder. */
    tent_anker(t, idx, g5, e);
    for (k = 0; k < 4; k++) re15_g5_bone_add_set(t->slot, k, 0, t->part_rz[k]);
    e->render_scale_q12 = 0;                       /* Streckung ueber das Skinning (s. Kopf) */
    /* Eingezogen (scale 0) = nicht zeichnen (parts.flags &= ~1 im Original): der Zeichner
     * fragt re15_g5_tentakel_scale_x. */
    e->no_draw = (uint8_t)(t->scale_x <= 0);
}

/* ---- Haupteinstieg: G5 ruft das je Frame nach seinem eigenen Tick ------------------------ */
void re15_g5_tentakel_tick(const re15_actor_t *g5)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int i;
    if (!s_tent_bereit || !g5) return;
    for (i = 0; i < TENT_N; i++) tent_tick(i, g5, pl);
}
