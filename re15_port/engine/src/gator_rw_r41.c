/*
 * gator_rw_r41.c - Runde 41 Spur G2-RE2 (Neuaufbau ab Nachbesserung 5, bc5fb7c9c): Alligator mit dem Kanister im Maul
 * (RE2-Stufe 7) in ROOM2090 - RUECKWAERTS SETZEN, wo die Drehung nicht passt, und KURZ TAUCHEN nur als Notloesung.
 * Dossier: analysis/befunde_runde41/G2_gatorfix.md Abschnitt "G2-RE2 (Neuaufbau ab NB5)".
 *
 * Grundlage ist RE2 (Nutzer 2026-11-03: "nimm Resident Evil 2 als Grundlage"). Die Verfolgung selbst bleibt die von NB5
 * (enemy_ai_boss_gator.c GBP_CHASE: RE2-Clip 0/1 nach der +0x1F0-Hysterese, authored Wurzelversatz, gator_re2_r41.c
 * re15_gr2_chase_clip). Dieses Modul greift NUR ein, wenn
 *   (a) die Drehung nicht passt: die Bewegung des Takts wird in 8 von 12 Bildern ueberwiegend von den Klemmen (Kopf-Klemme,
 *       Wurzel-Klemme) zurueckgenommen ("Drueckt sich fest") -> ein Durchgang des RE2-Rueckzugs-Clips 10 (Blend 3,
 *       `addiu a1,zero,10 / addiu a2,zero,3 / jal 0x8001a330` @0x80102c40-48 im EM23-Overlay, nach Clip-Ende `addiu v1,zero,257 /
 *       sw v1,4(s0)` @0x80102c54-5c zurueck in Routine 1 Sub 1 = Schwimmen), Wurzel mit dem AUTHORED Rueckwaerts-Versatz des
 *       Clips (gemessen NB6: 30 Bilder, -35..+6 je Bild, Summe -700), dabei Kopf zum Ziel drehen;
 *   (b) er wirklich haengt: die Wurzel bleibt 150 Bilder in einem Kasten unter 2500, der Kopf ist weiter als 4500 von der Figur,
 *       kein Biss, und ein Rueckwaerts-Durchgang in diesem Fenster hat ihn nicht befreit -> kurz abtauchen, unsichtbar an eine
 *       freie Stelle (gator_re2_r41.c re15_gr2_rw_freie_stelle) und wieder auftauchen.
 * Ein Zustand "zuruecksetzen, um zu wenden" existiert in keinem Original (Dossier NB6 "Original-Pruefung Rueckwaerts": RE2 fuehrt
 * Sub 4 nur nach einem Treffer, beim Tragen fuehrt jeder Treffer in R1; RE1.5 Typ 0x23 hat nur den Rueckstoss) - Ausloeser,
 * Dauer, Drehrate, Tauch-Schwelle und Tauchablauf sind PORT-WAHL; das Mittel (Clip 10, Blend 3) ist RE2.
 * Abtauchen/Auftauchen hat RE2 fuer den Kampf nicht (nur das Intro: Sce_em_set y = +3000 @RDT 0x25A0) - Darstellung PORT-WAHL wie
 * r41/gatorfix NB7 (Wasserlinien-Schnitt im Zeichner, Mesh und Kanister untergetaucht aus, Wellenring Effekt 6 Sub 5).
 *
 * Pruefhaken re15_gr2_rw_alt_setzen(1) = Stand NB5 (bc5fb7c9c): nichts von diesem Modul, Klemme/Kuerzung nur Aussenwaende,
 * Belagerung mit dem Kanister = NB5-Stadion. Kein Spielweg, kein Env-Schalter.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_gator_re2_r41.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_esp.h"
#include "re15_damage.h"
#include "re15_scd.h"
#include "re15_skeleton.h"     /* re15_cos_q12 / re15_sin_q12 */
#include "re15_math.h"         /* re15_squareroot0 */

extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);
extern void re15_ai_advance(re15_actor_t *e, int32_t speed);
extern int  re15_gr2_chase_clip(int slot, re15_actor_t *e, int32_t dist, int32_t *vortrieb);

/* ---- PORT-WAHL (Gestaltung; keine Original-Werte - RE2 setzt beim Tragen nicht zurueck und taucht nicht) ---------------------- */
#define RW_DRUECK_N       16     /* Fenster "drueckt sich fest" (Bilder) */
#define RW_DRUECK_WEG    400     /* ... in dem die Takte zusammen mindestens 400 vorwaerts wollten (ein Paddel-Halbzyklus), ... */
#define RW_DRUECK_NETTO  400     /* ... die Wurzel aber netto keine 400 und kein Viertel davon vorankam (Gleiten an der Wand bleibt darueber) */
#define RW_RUECK_DREH   0x20     /* Drehrate beim Zuruecksetzen je Bild (NB6 Messung 2 Rueckwaerts-Baustein: +-32 je Bild) */
#define RW_PAUSE          45     /* Bilder Vorwaerts-Schwimmen zwischen zwei Rueckwaerts-Durchgaengen (kein Dauer-Rangieren) */
#define RW_HAENG_BILDER  150     /* Haenge-Fenster: 5 s */
#define RW_HAENG_KASTEN 2500     /* Wurzel bleibt im Fenster in einem Kasten unter 2500 x 2500 (Kreis der Verfolgung R 2000 = 4000 gross) ... */
#define RW_HAENG_NETTO   600     /* ... und kam in den letzten 60 Bildern netto keine 600 voran (sonst schwimmt er gerade los) */
#define RW_NAH          4500     /* Kopf (Bone 6) naeher als 4500 an der Figur = "am Spieler", kein Haengen */
#define RW_SINK_BILDER    20     /* Abtauchen: 20 Bilder */
#define RW_SINK_TIEFE   2000     /* ... bis die Wurzel 2000 unter der Schwimmhoehe liegt: Koerperhoehe 1939 (NB7: Schwimm-Pose y -1793..146 zur Wurzel) */
#define RW_AUF_BILDER     20     /* Auftauchen: 20 Bilder */
#define RW_UNTER_TEMPO   250     /* untergetaucht: Weg zur freien Stelle je Bild ... */
#define RW_UNTER_MIN      10     /* ... mindestens 10, hoechstens 40 Bilder */
#define RW_UNTER_MAX      40
#define RW_UNTER_DREH   0x60     /* Gier untergetaucht je Bild (Port-Wenderate 0x60, GIER-WACHE) */
#define RW_WASSER_Y    (-1200)   /* Schwimmhoehe = GB_WATER_Y (0x23-KI INIT @0x8010c65c) */
#define RW_WASSERLINIE  (-1054)  /* tiefster Scheitel der Schwimm-Pose auf der Schwimmhoehe (NB7 gemessen: -1200 + 146) - darunter schneidet der Zeichner */
#define RW_SPIEGEL     (-1620)   /* Wasserspiegel = sce-8-Wasserzonen p1 @RDT 0x09d6/0x09ea/0x09fe (enemy_ai_boss_gator.c Kopf) */
#define RW_RING_SUB        5     /* Wellenring = Wasserboden-Effekt 0x06050D00: Effekt 6 Sub 5 (`lui a0,0x605 / ori a0,a0,0xd00` @0x800456b8/bc) */
#define RW_RING_SKALA 0x3000     /* PORT-WAHL (NB7): Skala des Landungs-Spritzers 0x06033000 (@0x8003876c-70) - der Koerper ist ~12 300 lang */
#define RW_RING_KAM     2500     /* PORT-WAHL (NB8): kein Ring naeher als 2500 vor der laufenden Kamera */

enum { RW_NORMAL = 0, RW_RUECK, RW_SINKEN, RW_UNTER, RW_AUF };

typedef struct {
    uint8_t  modus;
    int16_t  bild, unter_n, pause;
    int32_t  dw[RW_DRUECK_N], dx[RW_DRUECK_N], dz[RW_DRUECK_N]; int dn; uint8_t fest;   /* Takt-Weg und Lage der letzten Bilder, 1 = drueckt sich fest */
    int32_t  hx[RW_HAENG_BILDER], hz[RW_HAENG_BILDER]; int hn;
    int32_t  seit_rueck, seit_biss;
    int32_t  sx, sz, zx, zz; int16_t zgier;
    int32_t  tx, tz;                     /* Steuerziel des letzten Verfolgungs-Takts (Richtung fuer die freie Stelle) */
    uint16_t r_min, r_max;
    re15_gr2_rw_stat_t st;
} rw_t;
static rw_t s_rw[RE15_ACTOR_MAX];
static int s_rw_alt = 0;
void re15_gr2_rw_alt_setzen(int an) { s_rw_alt = an; }
int  re15_gr2_rw_alt(void) { return s_rw_alt; }
/* G2-RE2 NB2 Vorschlag 4 (Dossier G2_gatorfix.md "G2-RE2 NB2 (Vorschlag 4)"): rw_an = die G2-RE2-Bausteine, die den Kanister als Koerper behandeln (Kollisions-Antwort,
 * erweiterte Kopf-Klemme, Westkanal-Umweg, Vorstoss/Rueckzug, Ueberquerungs-Profil, keine Belagerung) - nur noch mit dem Pruefhaken re15_gr2_v4_alt_setzen(1).
 * rw_traegt = dieses Modul selbst (Rueckwaerts / Tauchen als Rueckfall, wenn er auch ohne den Kanister haengt) und das Rammen in der Ueberquerung ("Rampe: rammen
 * wie gehabt"): getragen, nicht der NB5-Pruefhaken. */
int  re15_gr2_rw_an(void) { return !s_rw_alt && re15_gr2_traegt_lenkung(); }
int  re15_gr2_rw_traegt(void) { return !s_rw_alt && re15_gr2_traegt(); }
static int s_rw_tauch_aus = 0;           /* Pruefhaken der Riegel (Gegenprobe): 1 = Notloesung Tauchen aus */
void re15_gr2_rw_tauchen_aus_setzen(int an) { s_rw_tauch_aus = an; }
static int s_rw_rueck_aus = 0;           /* Pruefhaken der Riegel (Gegenprobe): 1 = Rueckwaerts aus */
void re15_gr2_rw_rueck_aus_setzen(int an) { s_rw_rueck_aus = an; }

void re15_gr2_rw_reset(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_gr2_rw_stat_t st = s_rw[slot].st;
    memset(&s_rw[slot], 0, sizeof s_rw[slot]);
    s_rw[slot].st = st;                    /* Zaehler laufen ueber den Kampf (Messhaken) */
    s_rw[slot].seit_rueck = 1 << 20; s_rw[slot].seit_biss = 1 << 20;
}
void re15_gr2_rw_stat(int slot, re15_gr2_rw_stat_t *s) { if (slot >= 0 && slot < RE15_ACTOR_MAX && s) *s = s_rw[slot].st; }
void re15_gr2_rw_stat_null(void) { for (int i = 0; i < RE15_ACTOR_MAX; i++) memset(&s_rw[i].st, 0, sizeof s_rw[i].st); }
int  re15_gr2_rw_modus(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_rw[slot].modus : 0; }
int  re15_gr2_rw_taucht(int slot) { const int m = re15_gr2_rw_modus(slot); return m == RW_SINKEN || m == RW_UNTER || m == RW_AUF; }

static void rw_log(const char *fmt, int slot, const re15_actor_t *e)
{
    re15_gr2_log("%s F%u slot %d pos=(%d,%d) gier=%d clip=%d", fmt, (unsigned)g_engine.frame_count, slot, e->x, e->z, (int)(e->rot_y & 0xfff), (int)e->motion);
}

/* Wellenring an Wurzel, Mitte und Kopf auf dem Wasserspiegel (PORT-WAHL wie NB7 ga_ringe); Ringe dicht vor der Linse entfallen */
static void rw_ringe(const re15_actor_t *e)
{
    const int g = e->rot_y & 0xfff;
    const int32_t fx = re15_cos_q12(g), fz = -re15_sin_q12(g);   /* vorwaerts (cos g, -sin g) wie gb_koerper_kontakt */
    static const int32_t a[3] = { 0, 2000, 4000 };
    for (int i = 0; i < 3; i++) {
        const int32_t rx = e->x + (int32_t)(((int64_t)fx * a[i]) >> 12), rz = e->z + (int32_t)(((int64_t)fz * a[i]) >> 12);
        const int32_t p[3] = { rx, RW_SPIEGEL, rz };
        if (re15_gr2_kamera_tiefe((int)g_scd.cam_id, p) < RW_RING_KAM) continue;
        (void)re15_esp_fx_spawn_rows(re15_esp_room_bank(), 6, RW_RING_SUB, RW_RING_SKALA, rx, RW_SPIEGEL, rz, INT32_MAX, 0);
    }
}

static void rw_tauchen_start(int slot, re15_actor_t *e, const re15_actor_t *pl)
{
    rw_t *w = &s_rw[slot];
    int32_t x = e->x, z = e->z; int gier = e->rot_y & 0xfff;
    if (!re15_gr2_rw_freie_stelle(e, pl, w->tx, w->tz, &x, &z, &gier)) {
        rw_log("G2RE2 TAUCHEN keine freie Stelle - kein Tauchgang", slot, e);
        w->hn = 0;
        return;
    }
    w->modus = RW_SINKEN; w->bild = 0; w->zx = x; w->zz = z; w->zgier = (int16_t)gier;
    w->r_min = e->hit_radius_min; w->r_max = e->hit_radius_max;
    e->hit_radius_min = 0; e->hit_radius_max = 0;            /* untergetaucht kein Ziel (wie gb_untertauchen, Port B9) */
    w->st.tauchgaenge++;
    re15_gr2_log("G2RE2 TAUCHEN an F%u slot %d pos=(%d,%d) -> freie Stelle (%d,%d) gier %d", (unsigned)g_engine.frame_count, slot, e->x, e->z, (int)x, (int)z, gier);
    rw_ringe(e);
}

/* Je Takt in GBP_CHASE mit dem Kanister (neuer Stand), nachdem das Steuerziel (tx,tz) feststeht und VOR Lenkung/Vortrieb der NB5-Verfolgung.
 * 1 = dieses Bild gehoert dem Modul (Rueckwaerts/Tauchgang), die Verfolgung laeuft nicht. */
int re15_gr2_rw_takt(int slot, re15_actor_t *e, const re15_actor_t *pl, int32_t tx, int32_t tz, int32_t dist)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !e || !re15_gr2_rw_traegt()) return 0;
    rw_t *w = &s_rw[slot];
    w->st.takte++;
    w->tx = tx; w->tz = tz;
    if (w->modus == RW_NORMAL) {
        if (!s_rw_rueck_aus && w->fest && w->pause == 0) {
            w->modus = RW_RUECK; w->bild = 0; w->fest = 0; w->dn = 0;
            w->st.rueck_n++;
            re15_gr2_log("G2RE2 RUECKWAERTS an F%u slot %d pos=(%d,%d) gier=%d ziel=(%d,%d) (drueckt sich fest)", (unsigned)g_engine.frame_count,
                         slot, e->x, e->z, (int)(e->rot_y & 0xfff), (int)tx, (int)tz);
        } else if (w->pause > 0) w->pause--;
    }
    if (w->modus == RW_RUECK) {
        int32_t v = 0;
        const int ende = re15_gr2_rueck_clip(slot, e, &v);
        re15_enemy_steer_point(e, tx, tz, RW_RUECK_DREH);
        re15_ai_advance(e, v);
        e->y = RW_WASSER_Y;
        w->st.rueck_bilder++;
        w->seit_rueck = 0;
        if (ende) {
            w->modus = RW_NORMAL; w->pause = RW_PAUSE;
            rw_log("G2RE2 RUECKWAERTS aus", slot, e);
        }
        return 1;
    }
    if (w->modus == RW_SINKEN) {
        (void)re15_gr2_chase_clip(slot, e, dist, NULL);       /* Clip laeuft weiter, kein Vortrieb */
        w->bild++;
        e->y = RW_WASSER_Y + (int32_t)((int64_t)RW_SINK_TIEFE * w->bild / RW_SINK_BILDER);
        w->st.tauch_bilder++;
        if (w->bild >= RW_SINK_BILDER) {
            w->modus = RW_UNTER; w->bild = 0; w->sx = e->x; w->sz = e->z;
            const int64_t dx = (int64_t)w->zx - e->x, dz = (int64_t)w->zz - e->z;
            int32_t n = (int32_t)(re15_squareroot0((uint32_t)(dx * dx + dz * dz > 0xffffffffLL ? 0xffffffffLL : dx * dx + dz * dz)) / RW_UNTER_TEMPO);
            w->unter_n = (int16_t)(n < RW_UNTER_MIN ? RW_UNTER_MIN : n > RW_UNTER_MAX ? RW_UNTER_MAX : n);
        }
        return 1;
    }
    if (w->modus == RW_UNTER) {
        w->bild++;
        e->x = w->sx + (int32_t)((int64_t)(w->zx - w->sx) * w->bild / w->unter_n);
        e->z = w->sz + (int32_t)((int64_t)(w->zz - w->sz) * w->bild / w->unter_n);
        e->anchor_x = e->x; e->anchor_z = e->z;
        {   const int d = ((w->zgier - (int)e->rot_y + 0x800) & 0xfff) - 0x800;
            const int s = d > RW_UNTER_DREH ? RW_UNTER_DREH : d < -RW_UNTER_DREH ? -RW_UNTER_DREH : d;
            e->rot_y = (int16_t)((e->rot_y + s) & 0xfff); }
        e->y = RW_WASSER_Y + RW_SINK_TIEFE;
        w->st.tauch_bilder++; w->st.unter_bilder++;
        if (w->bild >= w->unter_n) {
            w->modus = RW_AUF; w->bild = 0;
            rw_ringe(e);
            rw_log("G2RE2 AUFTAUCHEN", slot, e);
        }
        return 1;
    }
    if (w->modus == RW_AUF) {
        (void)re15_gr2_chase_clip(slot, e, dist, NULL);
        w->bild++;
        e->y = RW_WASSER_Y + (int32_t)((int64_t)RW_SINK_TIEFE * (RW_AUF_BILDER - w->bild) / RW_AUF_BILDER);
        w->st.tauch_bilder++;
        if (w->bild >= RW_AUF_BILDER) {
            e->y = RW_WASSER_Y;
            e->hit_radius_min = w->r_min; e->hit_radius_max = w->r_max;
            w->modus = RW_NORMAL; w->pause = RW_PAUSE; w->hn = 0; w->dn = 0; w->fest = 0;
            rw_log("G2RE2 TAUCHEN aus", slot, e);
        }
        return 1;
    }
    return 0;
}

/* Je Takt am Takt-Ende (re15_gator_boss_tick nach den Klemmen): ox/oz = Lage vor dem Takt, takt_x/z = nach dem Takt vor den Klemmen.
 * phase_biss = 1 waehrend eines Bisses (GBP_LUNGE). verfolgt = 1 in GBP_CHASE (sonst ruhen die Fenster). */
void re15_gr2_rw_nachtakt(int slot, re15_actor_t *e, const re15_actor_t *pl, int32_t ox, int32_t oz, int32_t takt_x, int32_t takt_z, int verfolgt, int phase_biss)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !e) return;
    rw_t *w = &s_rw[slot];
    if (!re15_gr2_rw_traegt()) {
        if (w->modus >= RW_SINKEN && w->r_min) { e->hit_radius_min = w->r_min; e->hit_radius_max = w->r_max; }
        if (w->modus != RW_NORMAL) { w->modus = RW_NORMAL; e->y = RW_WASSER_Y; }
        w->hn = 0; w->dn = 0; w->fest = 0;
        return;
    }
    if (phase_biss) w->seit_biss = 0; else if (w->seit_biss < (1 << 20)) w->seit_biss++;
    if (w->modus != RW_RUECK && w->seit_rueck < (1 << 20)) w->seit_rueck++;
    /* G2-RE2 NB2 Vorschlag 4: ein Rueckwaerts-Durchgang, den ein Phasenwechsel (Biss, Belagerung, Ueberquerung) unterbricht, ist zu Ende - sonst hielt der
     * Modus ausserhalb der Verfolgung an (gemessen Riegel re2, Lage Rampe-x2318: 100 % "rueckwaerts" in der Belagerung). Pruefhaken v4_alt wie f942f56c0. */
    if (!verfolgt && w->modus == RW_RUECK && !re15_gr2_v4_alt()) { w->modus = RW_NORMAL; w->pause = RW_PAUSE; }
    if (!verfolgt || w->modus != RW_NORMAL) { w->dn = 0; w->fest = 0; if (!verfolgt) w->hn = 0; return; }
    /* (a) drueckt sich fest: die Takte wollten in RW_DRUECK_N Bildern zusammen >= RW_DRUECK_WEG, die Wurzel kam netto < RW_DRUECK_NETTO und < 1/4 davon voran
     * (die Klemmen / die Kollisions-Antwort nahmen die Bewegung zurueck). Gleiten an einer Wand (NB5, netto weiter) loest nichts aus. */
    {
        const int64_t ix = (int64_t)takt_x - ox, iz = (int64_t)takt_z - oz;
        const int i = w->dn % RW_DRUECK_N;
        w->dw[i] = (int32_t)re15_squareroot0((uint32_t)(ix * ix + iz * iz > 0xffffffffLL ? 0xffffffffLL : ix * ix + iz * iz));
        const int alt = (w->dn + 1) % RW_DRUECK_N;            /* Lage vor RW_DRUECK_N Bildern (sobald das Fenster voll ist) */
        int32_t weg = 0; for (int k = 0; k < RW_DRUECK_N; k++) weg += w->dw[k];
        w->fest = 0;
        if (w->dn >= RW_DRUECK_N) {
            const int64_t nx = (int64_t)e->x - w->dx[alt], nz = (int64_t)e->z - w->dz[alt];
            const int32_t netto = (int32_t)re15_squareroot0((uint32_t)(nx * nx + nz * nz > 0xffffffffLL ? 0xffffffffLL : nx * nx + nz * nz));
            w->fest = (uint8_t)(weg >= RW_DRUECK_WEG && netto < RW_DRUECK_NETTO && 4 * netto < weg);
            /* G2-RE2 NB2 Vorschlag 4 (Auftrag "Rueckwaerts und Tauchen nur als Rueckfall, falls er auch ohne Kanister haengt"): ohne Kopf-/Kanister-Klemme loest
             * "drueckt sich fest" nicht mehr aus - das 16-Bilder-Fenster schlug sonst beim Wenden und Beissen an der Figur an (gemessen Riegel re2 Lage Mitte-Nord:
             * 8 % rueckwaerts im Angriff); der Rueckfall beginnt erst mit dem Haenge-Fenster (b) (Rueckwaerts zuerst, dann Tauchen). */
            if (!re15_gr2_v4_alt()) w->fest = 0;
        }
        w->dx[i] = e->x; w->dz[i] = e->z; w->dn++;
    }
    /* (b) Haenge-Fenster */
    w->hx[w->hn % RW_HAENG_BILDER] = e->x; w->hz[w->hn % RW_HAENG_BILDER] = e->z; w->hn++;
    if (w->hn >= RW_HAENG_BILDER && w->seit_biss >= RW_HAENG_BILDER && pl) {
        int32_t x0 = w->hx[0], x1 = x0, z0 = w->hz[0], z1 = z0;
        for (int i = 1; i < RW_HAENG_BILDER; i++) {
            if (w->hx[i] < x0) x0 = w->hx[i]; if (w->hx[i] > x1) x1 = w->hx[i];
            if (w->hz[i] < z0) z0 = w->hz[i]; if (w->hz[i] > z1) z1 = w->hz[i];
        }
        int32_t kopf[3] = { e->x, e->y, e->z };
        (void)re15_gr2_bone_welt_pub(e, 6, kopf);
        const int64_t kx = (int64_t)kopf[0] - pl->x, kz = (int64_t)kopf[2] - pl->z;
        const int i60 = (w->hn - 60) % RW_HAENG_BILDER;
        const int64_t n6x = (int64_t)e->x - w->hx[i60], n6z = (int64_t)e->z - w->hz[i60];
        if (x1 - x0 < RW_HAENG_KASTEN && z1 - z0 < RW_HAENG_KASTEN && kx * kx + kz * kz > (int64_t)RW_NAH * RW_NAH
            && n6x * n6x + n6z * n6z < (int64_t)RW_HAENG_NETTO * RW_HAENG_NETTO) {
            w->st.haenger++;
            re15_gr2_log("G2RE2 HAENGT F%u slot %d: Wurzel %d Bilder in (%d..%d, %d..%d), Kopf %d von der Figur, Rueckwaerts vor %d Bildern",
                         (unsigned)g_engine.frame_count, slot, RW_HAENG_BILDER, (int)x0, (int)x1, (int)z0, (int)z1,
                         (int)re15_squareroot0((uint32_t)(kx * kx + kz * kz > 0xffffffffLL ? 0xffffffffLL : kx * kx + kz * kz)), (int)w->seit_rueck);
            if (w->seit_rueck > RW_HAENG_BILDER && !s_rw_rueck_aus) {   /* erst (a): noch kein Rueckwaerts in diesem Fenster */
                w->fest = 1; w->pause = 0;
            } else if (!s_rw_tauch_aus) {
                rw_tauchen_start(slot, e, pl);                          /* (b) Notloesung */
            }
            w->hn = 0;
        }
    }
}

/* Zeichner-Haken (PC): untergetaucht kein Mesh/Kanister; beim Ab-/Auftauchen Schnitt an der Wasserlinie */
static int rw_slot(const re15_actor_t *e)
{
    const int i = (int)(e - g_actors);
    return (e && i >= 0 && i < RE15_ACTOR_MAX && re15_gr2_rw_traegt()) ? i : -1;
}
int re15_gr2_rw_modell_aus(const re15_actor_t *e)
{
    const int i = rw_slot(e);
    return i >= 0 && s_rw[i].modus == RW_UNTER;
}
int re15_gr2_rw_wasserlinie(const re15_actor_t *e, int32_t *y_wl)
{
    const int i = rw_slot(e);
    if (i < 0 || (s_rw[i].modus != RW_SINKEN && s_rw[i].modus != RW_UNTER && s_rw[i].modus != RW_AUF)) return 0;
    if (y_wl) *y_wl = RW_WASSERLINIE;
    return 1;
}
