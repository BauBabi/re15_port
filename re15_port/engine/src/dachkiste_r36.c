/*
 * RE1.5 Rebuilt — Runde 36 Spur R "dachkiste": Messschrieb des Kisten-Schubs (Diagnose, KEIN Verhalten).
 *
 * RE15_PUSH_LOG=<datei>: je Schubversuch der Schubkette FUN_8002bd44 (aot_common.c re15_prop_push_tick) eine
 * Zeile — Bild, Objekt, Spielerlage, die von FUN_8002cabc(Spieler,obj,1) @0x8002bfa4 gesetzte Ziel-Lage und das
 * Ergebnis von FUN_8003b558(obj,2) @0x8002bfb0 (0 = angenommen @0x8002bfc0, 1 = gesperrt @0x8002c0b0 ->
 * Ruecksetzung @0x8002c0b8-d4). Bei "gesperrt" folgt je getroffener Zelle eine Zeile TREFFER-ZELLE mit
 * Region/Index und Rechteck — das Gegenstueck zum Haltepunkt in FUN_8003da78 (Aufruf @0x8003b79c je Treffer,
 * a0 = Zelle) der Original-Messung (analysis/befunde_runde36/R_belege/orig_1090_push.py), damit Original und
 * Port Versuch fuer Versuch vergleichbar sind (Dossier R_dachkiste.md, Punkt 1-3).
 *
 * Die Zellensuche hier ist dieselbe wie in re15_collision_box_blocked (re15_collision.c), nur mit Ausgabe:
 *   Quadrant       FUN_8003b068 (2-Bit aus sign(pos - ceiling)), Zellenliste des Quadranten
 *   Band           @0x8003b634-644  Bandnibble der Zelle (floor >> 4) == obj+0x82
 *   Klasse         @0x8003b64c-660  (a1 & u0) != 0, a1 = 2 fuer den Schub (@0x8002bfac)
 *   Rechteck       @0x8003b738-768  (u32)(cx - (x0 - r)) < w + 2r, @0x8003b770-794 dito z, r = hx (@0x8003b68c-690)
 *
 * RE15_WEGPLAN=<datei> (Nachbesserung 2): geschlossen geregelter Messlauf — das Gegenstueck zur Original-Messung
 * (orig_1090_push.py: gehe_zu / drehe_auf / schiebe lesen Lage und Blick aus der RAM und steuern den Pad). Ein
 * offenes Skript (RE15_INPUT_SCRIPT) trifft nach mehreren Drehungen und Anlauf-/Auslauf-Animationen die
 * berechneten Stellungen nicht (R_abnahme_1.md, Laeufe R4c/R4d); hier liest der Treiber je Bild Spielerlage,
 * Blick (+0x6A) und Objektlage und erzeugt die Pad-Bits (Haken: platform/pc/src/input_pc.c hinter der
 * Skript-Ersetzung). Reine Mess-Steuerung wie RE15_INPUT_SCRIPT: kein Spiel-Code liest sie, kein Verhalten.
 *   OBJ n                        Objekt-Slot, dessen Lage SCHIEB/Log verfolgen (Default 6)
 *   RAUM hhhh                    warten, bis ROOMhhhh geladen und der Spieler frei ist (player_mode != 2)
 *   WARTE n                      n Spielbilder Pad 0
 *   GEGNERWEG                    alle Gegner-Aktoren (type != 0) freigeben (re15_actor_free) — Messung der
 *                                Schubkette ohne Gegnerkontakt; Gegenstueck am Original: Dispatch-Eintrag
 *                                des Typs auf 0 (FUN_8001a50c ueberspringt Handler 0 @0x8001a568)
 *   ZELLE r i                    Laufzeit-Bytes der SCA-Zelle r[i] ins Log (u0 nach Sca_id_set @0x8004179c)
 *   GEHE x z [tol]               drehen und gehen, bis |Spieler - (x,z)| <= tol (Default 150); Klemme -> weiter
 *   BLICK yaw                    auf yaw drehen (0 = +X Ost, 1024 = -Z Nord, 2048 = -X West, 3072 = +Z Sued)
 *   SCHIEB yaw X|Z >=|<= wert [max]   auf yaw drehen, UP halten, bis Objekt-Koordinate die Bedingung erfuellt,
 *                                das Objekt faellt (y aendert sich), die Schubkette dreimal sperrt oder max Bilder
 *   AKTION                       Aktionstaste (Quadrat) 3 Bilder
 *   ENDE
 * Log: RE15_WEGPLAN_LOG=<datei> (Default wegplan.txt im Arbeitsordner).
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_dachkiste_r36.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_scd.h"

extern scd_vm_t g_scd;

/* FUN_8003b068 — identisch zu quadrant_of in re15_collision.c (dort static). */
static int quadrant(int32_t px, int32_t pz, int16_t ceil_x, int16_t ceil_z)
{
    unsigned zb = (unsigned)(pz - (int32_t)ceil_z) & 0x80000000u;
    unsigned xb = (unsigned)(px - (int32_t)ceil_x) & 0x80000000u;
    return (int)((zb | (xb >> 1)) >> 0x1e);
}

static void treffer_zellen(FILE *f, int32_t cx, int32_t cz, int band, int32_t r, unsigned mask)
{
    const re15_rdt_t *rdt = &g_room_rdt;
    if (!rdt->sca || rdt->sca_count <= 0 || band < 0) return;
    const int q = quadrant(cx, cz, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0;
    for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (band != (e->floor >> 4)) continue;                                     /* @0x8003b634-644 */
        if ((mask & e->u0) == 0) continue;                                         /* @0x8003b64c-660 */
        if ((unsigned)(cx - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r * 2) &&   /* @0x8003b738-768 */
            (unsigned)(cz - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r * 2)) {   /* @0x8003b770-794 */
            fprintf(f, "  TREFFER-ZELLE r%d[%d]: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x\n",
                    q, i - start, (int)e->x, (int)e->x + (int)e->width, (int)e->z, (int)e->z + (int)e->density,
                    (int)e->type, (unsigned)e->u0, (unsigned)e->u1, (unsigned)e->floor);
        }
    }
}

/* Letztes Schub-Ergebnis fuer den Wegplan-Treiber (SCHIEB bricht nach drei gesperrten Schueben ab). */
static uint32_t s_schub_bild = 0;
static int      s_schub_gesperrt = 0;
static int      s_schub_obj = -1;

void re15_dachkiste_schub_log(int prop_idx, int32_t ziel_x, int32_t ziel_z, int gesperrt)
{
    static int init = 0;
    static FILE *f = NULL;
    if (!init) {
        init = 1;
        const char *p = getenv("RE15_PUSH_LOG");
        if (p && *p) f = fopen(p, "w");
    }
    s_schub_bild = g_engine.frame_count;
    s_schub_gesperrt = gesperrt;
    s_schub_obj = prop_idx;
    if (!f || prop_idx < 0 || prop_idx >= RE15_SCD_MAX_PROPS) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(f, "F%u room=%04X p=%d obj=%d SCHUB-%s spieler=(%d,%d) rot=%d lage=(%d,%d) ziel=(%d,%d) band=%d\n",
            g_engine.frame_count, g_current_room_id, prop_idx, (int)g_scd.props[prop_idx].obj_id,
            gesperrt ? "GESPERRT" : "OK", pl->x, pl->z, (int)pl->rot_y,
            g_scd.props[prop_idx].x, g_scd.props[prop_idx].z, ziel_x, ziel_z, (int)g_scd.props[prop_idx].band);
    if (gesperrt)
        treffer_zellen(f, ziel_x + g_scd.props[prop_idx].box_cx, ziel_z + g_scd.props[prop_idx].box_cz,
                       (int)g_scd.props[prop_idx].band, (int32_t)(uint16_t)g_scd.props[prop_idx].box_hx, 2u);
    fflush(f);
}

/* ======================================================================================================
 * RE15_WEGPLAN — geschlossen geregelter Messlauf (Pad-Bits aus Spieler-/Objektlage), Diagnose only.
 * Pad-Bits = PSX-Padwort (platform/pc/src/input_pc.c:18-37, include/re15_player.h RE15_PAD_BIT_*).
 * ====================================================================================================== */
#define WP_UP     0x0010u
#define WP_RIGHT  0x0020u
#define WP_DOWN   0x0040u
#define WP_LEFT   0x0080u
#define WP_SQUARE 0x8000u

enum { WP_OBJ = 1, WP_RAUM, WP_WARTE, WP_GEGNERWEG, WP_ZELLE, WP_GEHE, WP_BLICK, WP_SCHIEB, WP_AKTION, WP_ENDE };

typedef struct {
    int kind;
    int a, b, c, d, e;      /* OBJ n | RAUM id | WARTE n | ZELLE r i | GEHE x z tol | BLICK yaw |
                             * SCHIEB yaw achse(0=X,1=Z) op(0 = >=, 1 = <=) wert max */
} wp_cmd_t;

#define WP_MAX 96
static wp_cmd_t s_wp[WP_MAX];
static int      s_wp_n = 0;
static int      s_wp_i = 0;
static int      s_wp_init = 0;
static FILE    *s_wp_log = NULL;
static int      s_wp_obj = 6;       /* Pool-Slot des verfolgten Objekts (aus OBJ <obj_id> aufgeloest) */
static int      s_wp_obj_id = 6;    /* OBJ n = Obj_model_set-Nummer (pc[1]); der Pool-Slot ist die Installationsreihenfolge */
static int      s_wp_geh_bilder = 0;   /* Bilder mit UP im laufenden GEHE (Auslauf ~ Geschwindigkeit) */
static int32_t  s_wp_geh_dist = 0;     /* Rest im Vorbild (Vorbeilaufen erkennen) */

static void wp_obj_aufloesen(void)
{
    for (int q = 0; q < (int)g_scd.prop_count && q < RE15_SCD_MAX_PROPS; q++)
        if (g_scd.props[q].active && (int)g_scd.props[q].obj_id == s_wp_obj_id) { s_wp_obj = q; return; }
    if (s_wp_obj_id >= 0 && s_wp_obj_id < RE15_SCD_MAX_PROPS) s_wp_obj = s_wp_obj_id;
}
static uint32_t s_wp_bild_zuletzt = 0xffffffffu;
/* Zustand des laufenden Befehls */
static int      s_wp_phase = 0;
static int      s_wp_zaehler = 0;
static int      s_wp_runden = 0;
static int32_t  s_wp_px = 0, s_wp_pz = 0;   /* Spielerlage im Vorbild (Stillstands-/Klemmerkennung) */
static int      s_wp_still = 0;
static int      s_wp_gesperrt_n = 0;
static int32_t  s_wp_obj_y0 = 0;
static uint16_t s_wp_bits = 0;

static int wp_yaw_err(int ziel, int ist)
{
    return ((ziel - ist + 2048) & 4095) - 2048;
}

/* Blick auf einen Punkt: dir(yaw) = (cos, -sin) (0 = +X, 1024 = -Z, 2048 = -X, 3072 = +Z; gemessen Punkt 1.2). */
static int wp_yaw_zu(int32_t dx, int32_t dz)
{
    double a = atan2(-(double)dz, (double)dx);
    int y = (int)lround(a / (2.0 * 3.14159265358979323846) * 4096.0);
    return y & 4095;
}

static void wp_log(const char *fmt, ...);
#include <stdarg.h>
static void wp_log(const char *fmt, ...)
{
    if (!s_wp_log) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(s_wp_log, "F%u room=%04X spieler=(%d,%d) rot=%d mo=%d pm=%d obj%d=(%d,%d,%d) b%d | ",
            g_engine.frame_count, g_current_room_id, pl->x, pl->z, (int)pl->rot_y, (int)pl->motion,
            (int)g_scd.player_mode, s_wp_obj,
            g_scd.props[s_wp_obj].x, g_scd.props[s_wp_obj].y, g_scd.props[s_wp_obj].z, (int)g_scd.props[s_wp_obj].band);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(s_wp_log, fmt, ap);
    va_end(ap);
    fputc('\n', s_wp_log);
    fflush(s_wp_log);
}

static void wp_parse(const char *pfad)
{
    FILE *f = fopen(pfad, "r");
    if (!f) return;
    char zeile[256];
    while (fgets(zeile, sizeof zeile, f) && s_wp_n < WP_MAX) {
        char *h = strchr(zeile, '#');
        if (h) *h = 0;
        char w[32] = {0}, s1[32] = {0}, s2[32] = {0};
        int a = 0, b = 0, c = 0, d = 0, e = 0;
        wp_cmd_t cmd;
        memset(&cmd, 0, sizeof cmd);
        if (sscanf(zeile, "%31s", w) != 1) continue;
        if      (!strcmp(w, "OBJ"))       { cmd.kind = WP_OBJ;   sscanf(zeile, "%*s %d", &a); cmd.a = a; }
        else if (!strcmp(w, "RAUM"))      { cmd.kind = WP_RAUM;  sscanf(zeile, "%*s %x", &a); cmd.a = a; }
        else if (!strcmp(w, "WARTE"))     { cmd.kind = WP_WARTE; sscanf(zeile, "%*s %d", &a); cmd.a = a; }
        else if (!strcmp(w, "GEGNERWEG")) { cmd.kind = WP_GEGNERWEG; }
        else if (!strcmp(w, "ZELLE"))     { cmd.kind = WP_ZELLE; sscanf(zeile, "%*s %d %d", &a, &b); cmd.a = a; cmd.b = b; }
        else if (!strcmp(w, "GEHE"))      { cmd.kind = WP_GEHE; c = 150; sscanf(zeile, "%*s %d %d %d", &a, &b, &c);
                                            cmd.a = a; cmd.b = b; cmd.c = c; }
        else if (!strcmp(w, "BLICK"))     { cmd.kind = WP_BLICK; sscanf(zeile, "%*s %d", &a); cmd.a = a & 4095; }
        else if (!strcmp(w, "SCHIEB"))    { cmd.kind = WP_SCHIEB; e = 900;
                                            sscanf(zeile, "%*s %d %31s %31s %d %d", &a, s1, s2, &d, &e);
                                            cmd.a = a & 4095; cmd.b = (s1[0] == 'Z' || s1[0] == 'z') ? 1 : 0;
                                            cmd.c = (s2[0] == '<') ? 1 : 0; cmd.d = d; cmd.e = e; }
        else if (!strcmp(w, "AKTION"))    { cmd.kind = WP_AKTION; }
        else if (!strcmp(w, "ENDE"))      { cmd.kind = WP_ENDE; }
        else continue;
        s_wp[s_wp_n++] = cmd;
    }
    fclose(f);
}

static void wp_naechster(const char *grund)
{
    wp_log("  -> Befehl %d beendet: %s", s_wp_i, grund);
    s_wp_i++;
    s_wp_phase = 0;
    s_wp_zaehler = 0;
    s_wp_runden = 0;
    s_wp_still = 0;
    s_wp_gesperrt_n = 0;
}

static void wp_start(const wp_cmd_t *c)
{
    switch (c->kind) {
    case WP_OBJ:   wp_log("OBJ %d", c->a); break;
    case WP_RAUM:  wp_log("RAUM %04X warten", c->a); break;
    case WP_WARTE: wp_log("WARTE %d", c->a); break;
    case WP_GEGNERWEG: wp_log("GEGNERWEG"); break;
    case WP_ZELLE: wp_log("ZELLE r%d[%d]", c->a, c->b); break;
    case WP_GEHE:  wp_log("GEHE (%d,%d) tol %d", c->a, c->b, c->c); break;
    case WP_BLICK: wp_log("BLICK %d", c->a); break;
    case WP_SCHIEB: wp_log("SCHIEB blick %d bis obj.%c %s %d (max %d Bilder)", c->a, c->b ? 'z' : 'x',
                           c->c ? "<=" : ">=", c->d, c->e); break;
    case WP_AKTION: wp_log("AKTION"); break;
    default: break;
    }
}

/* Drehen auf ziel: Pad-Bit fuer dieses Bild; 1 = steht (|err| <= 64 und zwei Bilder ruhig). */
static int wp_drehe(int ziel, uint16_t *bits)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int err = wp_yaw_err(ziel, (int)pl->rot_y);
    if (abs(err) > 64) {
        *bits |= (err > 0) ? WP_RIGHT : WP_LEFT;   /* R erhoeht rot_y (gemessen: 3072 -> 4128 = 32) */
        s_wp_zaehler = 0;
        return 0;
    }
    s_wp_zaehler++;
    return s_wp_zaehler >= 2;
}

static int wp_steht_still(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->x == s_wp_px && pl->z == s_wp_pz) s_wp_still++; else s_wp_still = 0;
    s_wp_px = pl->x; s_wp_pz = pl->z;
    return s_wp_still >= 3;
}

static void wp_zelle(int r, int i)
{
    const re15_rdt_t *rdt = &g_room_rdt;
    if (!rdt->sca || r < 0 || r > 4) { wp_log("  ZELLE: keine SCA"); return; }
    int start = 0;
    for (int q = 0; q < r; q++) start += rdt->sca_rgn[q];
    if (i < 0 || start + i >= rdt->sca_count) { wp_log("  ZELLE: Index ausserhalb"); return; }
    const re15_sca_entry_t *e = &rdt->sca[start + i];
    wp_log("  ZELLE r%d[%d] LAUFZEIT: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x", r, i,
           (int)e->x, (int)e->x + (int)e->width, (int)e->z, (int)e->z + (int)e->density,
           (int)e->type, (unsigned)e->u0, (unsigned)e->u1, (unsigned)e->floor);
}

int re15_dachkiste_wegplan_pad(uint16_t *bits)
{
    if (!s_wp_init) {
        s_wp_init = 1;
        const char *p = getenv("RE15_WEGPLAN");
        if (!p || !*p) { s_wp_n = -1; return 0; }
        wp_parse(p);
        const char *l = getenv("RE15_WEGPLAN_LOG");
        s_wp_log = fopen((l && *l) ? l : "wegplan.txt", "w");
        if (s_wp_log) { fprintf(s_wp_log, "RE15_WEGPLAN %s: %d Befehle\n", p, s_wp_n); fflush(s_wp_log); }
    }
    if (s_wp_n <= 0 || s_wp_i >= s_wp_n) return 0;
    const wp_cmd_t *c = &s_wp[s_wp_i];
    if (c->kind == WP_ENDE) return 0;

    /* Nur je NEUEM Spielbild weiterschalten; innerhalb eines Bildes dieselben Bits liefern. */
    const int neu = (g_engine.frame_count != s_wp_bild_zuletzt);
    if (!neu) { if (c->kind != WP_RAUM) *bits = s_wp_bits; return 1; }
    s_wp_bild_zuletzt = g_engine.frame_count;

    /* Vor dem Raum (RAUM) nichts anfassen; danach erst, wenn der Spieler frei ist. */
    if (c->kind == WP_RAUM) {
        if (g_current_room_id != c->a || !g_room_rdt_ok || g_scd.player_mode == 2) return 0;
        if (s_wp_phase == 0) { s_wp_phase = 1; s_wp_zaehler = 0; wp_obj_aufloesen();
                               wp_log("RAUM %04X geladen, Spieler frei (obj %d = Slot %d)", c->a, s_wp_obj_id, s_wp_obj); }
        *bits = 0; s_wp_bits = 0;
        if (++s_wp_zaehler >= 30) wp_naechster("Raum bereit");
        return 1;
    }
    if (g_scd.player_mode == 2) { *bits = 0; s_wp_bits = 0; return 1; }   /* Szene: Pad weg, warten */

    uint16_t out = 0;
    if (s_wp_phase == 0 && s_wp_zaehler == 0 && s_wp_runden == 0) wp_start(c);
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (c->kind) {
    case WP_OBJ:
        s_wp_obj_id = c->a;
        wp_obj_aufloesen();
        wp_naechster("gesetzt");
        break;
    case WP_WARTE:
        if (++s_wp_zaehler >= c->a) wp_naechster("gewartet");
        break;
    case WP_GEGNERWEG: {
        int n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type != 0) {
                wp_log("  Gegner Slot %d Typ 0x%02x (%d,%d) freigegeben", s, (unsigned)g_actors[s].type,
                       g_actors[s].x, g_actors[s].z);
                re15_actor_free(s);
                n++;
            }
        char g[48]; snprintf(g, sizeof g, "%d Gegner weg", n);
        wp_naechster(g);
        break; }
    case WP_ZELLE:
        wp_zelle(c->a, c->b);
        wp_naechster("gelesen");
        break;
    case WP_BLICK:
        if (wp_drehe(c->a, &out)) wp_naechster("Blick steht");
        break;
    case WP_AKTION:
        out = WP_SQUARE;
        if (++s_wp_zaehler >= 3) wp_naechster("gedrueckt");
        break;
    case WP_GEHE: {
        const int32_t dx = c->a - pl->x, dz = c->b - pl->z;
        const int32_t dist = (int32_t)lround(sqrt((double)dx * dx + (double)dz * dz));
        if (s_wp_phase == 0) {                       /* drehen */
            if (dist <= c->c) { wp_naechster("angekommen"); break; }
            if (wp_drehe(wp_yaw_zu(dx, dz), &out)) { s_wp_phase = 1; s_wp_zaehler = 0; s_wp_still = 0;
                                                      s_wp_px = pl->x; s_wp_pz = pl->z;
                                                      s_wp_geh_bilder = 0; s_wp_geh_dist = dist; }
        } else if (s_wp_phase == 1) {                /* gehen; Auslauf nach dem Loslassen ~444 bei voller Fahrt
                                                      * (12 Bilder Anlauf, gemessen Punkt 1.2) -> frueher loslassen */
            const int err = wp_yaw_err(wp_yaw_zu(dx, dz), (int)pl->rot_y);
            const int32_t rest = 40 + 35 * (s_wp_geh_bilder < 12 ? s_wp_geh_bilder : 12);
            const int vorbei = (s_wp_geh_bilder > 2 && dist > s_wp_geh_dist + 10);
            s_wp_geh_dist = dist;
            if (dist > rest && !vorbei && abs(err) <= 300) {
                out = WP_UP;
                s_wp_geh_bilder++;
                if (abs(err) > 96) out |= (err > 0) ? WP_RIGHT : WP_LEFT;
                (void)wp_steht_still();
                if (s_wp_still >= 15) {
                    wp_log("  KLEMME beim Gehen nach (%d,%d): Rest %d", c->a, c->b, dist);
                    wp_naechster("Klemme");
                }
            } else { s_wp_phase = 2; s_wp_zaehler = 0; s_wp_still = 0; }
        } else {                                     /* Auslauf abwarten, dann pruefen */
            if (wp_steht_still()) {
                if (dist <= c->c) wp_naechster("angekommen");
                else if (++s_wp_runden >= 8) { wp_log("  GEHE: nach %d Runden Rest %d", s_wp_runden, dist);
                                               wp_naechster("Runden erschoepft"); }
                else { s_wp_phase = 0; s_wp_zaehler = 0; }
            }
        }
        break; }
    case WP_SCHIEB: {
        const int32_t koord = c->b ? g_scd.props[s_wp_obj].z : g_scd.props[s_wp_obj].x;
        const int erfuellt = c->c ? (koord <= c->d) : (koord >= c->d);
        if (s_wp_phase == 0) {
            if (wp_drehe(c->a, &out)) { s_wp_phase = 1; s_wp_zaehler = 0; s_wp_still = 0; s_wp_gesperrt_n = 0;
                                        s_wp_obj_y0 = g_scd.props[s_wp_obj].y; s_wp_px = pl->x; s_wp_pz = pl->z;
                                        wp_log("  SCHIEB: Blick steht, UP ab jetzt"); }
        } else {
            if (erfuellt) { wp_log("  SCHIEB: Bedingung erfuellt (obj.%c = %d)", c->b ? 'z' : 'x', koord);
                            wp_naechster("Bedingung"); break; }
            if (g_scd.props[s_wp_obj].y != s_wp_obj_y0) { wp_log("  SCHIEB: Objekt-y %d -> %d (Fall)", s_wp_obj_y0,
                                                                 g_scd.props[s_wp_obj].y);
                                                          wp_naechster("Fall"); break; }
            if (s_schub_obj == s_wp_obj && s_schub_bild == g_engine.frame_count - 1) {
                if (s_schub_gesperrt) s_wp_gesperrt_n++; else s_wp_gesperrt_n = 0;
                if (s_wp_gesperrt_n >= 3) { wp_log("  SCHIEB: dreimal gesperrt"); wp_naechster("gesperrt"); break; }
            }
            out = WP_UP;
            if (++s_wp_zaehler >= c->e) { wp_log("  SCHIEB: %d Bilder ohne Bedingung", s_wp_zaehler);
                                          wp_naechster("max Bilder"); break; }
            if (wp_steht_still() && s_wp_still >= 40) { wp_log("  SCHIEB: Spieler steht 40 Bilder (Klemme)");
                                                       wp_naechster("Klemme"); }
        }
        break; }
    default:
        wp_naechster("unbekannt");
        break;
    }
    s_wp_bits = out;
    *bits = out;
    return 1;
}
