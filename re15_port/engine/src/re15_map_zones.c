/*
 * re15_map_zones.c — KARTEN-ZONEN: Marker, Hervorhebung und Besucht-Stand.
 *
 * NUTZER-REPORT 2026-08-30: "Schon im ersten Bereich room 1170 laufen wir oben durch
 * eine Tuer ... nach Durchlauf ist der Marker unten im grossen Rechteck statt oben im
 * kleinen, und das kleine ist nicht hervorgehoben. ... du musst dringend noch an der
 * korrekten Darstellung der Raeume, Treppen, Tueren und Marker arbeiten. Im Original
 * ist hier noch viel kaputt, also orientiere dich da nur grob dran."
 *
 * URSACHE (gemessen): Ein RDT-"Raum" ist nicht immer EIN Ort. ROOM1170 enthaelt zwei
 * raeumlich getrennte Bereiche, verbunden durch eine SELBST-TUER (dest == eigener Raum,
 * Sprung von z=+15250 nach z=-26500 — Tuer-Datensatz im RDT). Das alte Modell
 * "ein Raum = ein Rechteck" konnte das nicht abbilden: Marker und Hervorhebung blieben
 * im Start-Bereich. 26 der 103 Basis-Raeume sind betroffen.
 *
 * ZONEN-MODELL: jeder zusammenhaengende Bereich der Kollisionsgeometrie ist eine Zone
 * mit eigenem Karten-Rechteck und eigener Marker-Abbildung; die aktive Zone folgt der
 * Spielerposition. Weil Zonen auf verschiedenen Seiten liegen duerfen, stimmt damit
 * auch die Etage. Tabelle: re15_map_zones.h (tools/gen_map_zones.py).
 *
 * Das ersetzt die frueheren Tabellen re15_map_rooms.h (Raum->Rechteck) und
 * re15_map_row_fix.h (Marker-Reparatur) — beide Aufgaben erledigt jetzt die Zone.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"
#include "re15_map_zones.h"

#define ZONE_COUNT ((int)(sizeof s_map_zones / sizeof s_map_zones[0]))
/* Toleranz beim Zonen-Test: dieselbe, mit der der Generator Bereiche verschmilzt —
 * der Spieler darf an der Zonen-Kante stehen, ohne "aus der Karte zu fallen". */
#define ZONE_SLACK 1500

static uint8_t s_visited[32];    /* 1 Bit je ZONE (Index = Tabellen-Index / 2) */

void re15_map_visited_reset(void) { memset(s_visited, 0, sizeof s_visited); }
void re15_map_visited_export(uint8_t out[32]) { memcpy(out, s_visited, 32); }
void re15_map_visited_import(const uint8_t in[32]) { memcpy(s_visited, in, 32); }

/* Ein Besucht-Bit je ORT: die globale Zonen-Nummer aus der Tabelle. (Ein aus dem
 * Tabellen-Index abgeleitetes Bit waere falsch — die Eintraege stehen zwar paarweise,
 * aber nicht an durchgehend geraden Positionen.) */
static int zone_bit(int idx) { return s_map_zones[idx].zid; }

/* Die Zone, in der (x,z) im Raum liegt.
 * ⛔ KLEINSTE passende Zone gewinnt: die Bbox eines grossen Bereichs UMSCHLIESST oft
 * die eines kleinen (ROOM1140 Zone 0 x[-10600,16700] z[-25600,1850] enthaelt Zone 1
 * x[-5250,7450] z[-17450,-11350] vollstaendig), obwohl die Kollisionszellen getrennt
 * sind. Ohne diese Regel gewaenne immer der grosse Bereich und der Bereichswechsel
 * waere unsichtbar — genau der Fehler, den der Nutzer gemeldet hat.
 * Ohne jeden Treffer: die erste Zone des Raums (besser als gar keine Karte). */
static int zone_index_at(unsigned room, int32_t x, int32_t z)
{
    int first = -1, best = -1;
    int32_t best_area = 0;
    for (int i = 0; i < ZONE_COUNT; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        if (zn->room != (unsigned short)room) continue;
        if (first < 0) first = i;
        if (x >= (int32_t)zn->wx0 - ZONE_SLACK && x <= (int32_t)zn->wx1 + ZONE_SLACK &&
            z >= (int32_t)zn->wz0 - ZONE_SLACK && z <= (int32_t)zn->wz1 + ZONE_SLACK) {
            int32_t w = (int32_t)zn->wx1 - (int32_t)zn->wx0;
            int32_t d = (int32_t)zn->wz1 - (int32_t)zn->wz0;
            int32_t a = (w / 64) * (d / 64);       /* skaliert, damit int32 reicht */
            if (best < 0 || a < best_area) { best = i; best_area = a; }
        }
    }
    return (best >= 0) ? best : first;
}

const re15_map_zone_t *re15_map_zone_at(unsigned room, int32_t x, int32_t z)
{
    int i = zone_index_at(room, x, z);
    return (i >= 0) ? &s_map_zones[i] : 0;
}

void re15_map_visited_mark_at(unsigned room, int32_t x, int32_t z)
{
    int i = zone_index_at(room, x, z);
    if (i < 0) return;
    int b = zone_bit(i);
    s_visited[b >> 3] |= (uint8_t)(1u << (b & 7));
}

int re15_map_zone_visited(const re15_map_zone_t *zn)
{
    if (!zn) return 0;
    int i = (int)(zn - s_map_zones);
    int b = zone_bit(i);
    return (s_visited[b >> 3] >> (b & 7)) & 1;
}

/* Marker: lineare Abbildung der Zonen-Bbox auf ihr Rechteck. y ist gespiegelt, weil
 * Welt-z nach Sueden waechst, Karten-y aber nach unten. Ergebnis liegt garantiert
 * INNERHALB des Rechtecks — das ist der Kern der Marker-Reparatur. */
int re15_map_zone_marker(const re15_map_zone_t *zn, int32_t x, int32_t z,
                         int rx, int ry, int rw, int rh, int16_t *mx, int16_t *my)
{
    int32_t w, d;
    if (!zn) return 0;
    w = (int32_t)zn->wx1 - (int32_t)zn->wx0;
    d = (int32_t)zn->wz1 - (int32_t)zn->wz0;
    if (w <= 0 || d <= 0) return 0;
    {
        int32_t px = (int32_t)zn->wx0, pz = (int32_t)zn->wz0;
        int32_t fx = x - px, fz = z - pz;
        if (fx < 0) fx = 0; if (fx > w) fx = w;
        if (fz < 0) fz = 0; if (fz > d) fz = d;
        *mx = (int16_t)(rx + (fx * rw) / w);
        /* z waechst nach Sueden -> unten im Bild; die Zonen-Bbox ist so orientiert,
         * dass wz0 der noerdliche Rand ist, also KEINE zusaetzliche Spiegelung. */
        *my = (int16_t)(ry + (fz * rh) / d);
    }
    return 1;
}

/* Rechteck-Zustand fuer den Zeichner: aktuell (Spieler steht in dieser Zone) schlaegt
 * besucht schlaegt unbesucht. Mehrere Zonen duerfen sich ein Rechteck teilen. */
int re15_map_rect_state(unsigned page, unsigned rect_idx)
{
    extern unsigned g_current_room_id;
    int state = RE15_MAP_RECT_UNMAPPED;
    const re15_map_zone_t *cur = re15_map_zone_current();
    for (int i = 0; i < ZONE_COUNT; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        if (zn->page != page || zn->rect != rect_idx) continue;
        if (cur && zn->page == cur->page && zn->rect == cur->rect &&
            zn->room == cur->room) return RE15_MAP_RECT_CURRENT;
        if (state < RE15_MAP_RECT_VISITED && re15_map_zone_visited(zn))
            state = RE15_MAP_RECT_VISITED;
        else if (state < RE15_MAP_RECT_UNVISITED)
            state = RE15_MAP_RECT_UNVISITED;
    }
    (void)g_current_room_id;
    return state;
}

/* Die Zone, in der der Spieler GERADE steht — vom Spiel je Frame nachgefuehrt
 * (re15_map_zone_update), damit der Karten-Zeichner keine Aktor-Kenntnis braucht. */
static const re15_map_zone_t *s_cur_zone;

void re15_map_zone_update(unsigned room, int32_t x, int32_t z)
{
    s_cur_zone = re15_map_zone_at(room, x, z);
    if (s_cur_zone) re15_map_visited_mark_at(room, x, z);
}
const re15_map_zone_t *re15_map_zone_current(void) { return s_cur_zone; }

/* Alt-Schnittstellen, die noch auf Raum-Ebene fragen (Tests/Save-Restore). */
void re15_map_visited_mark(unsigned room_id)
{
    /* Ohne Position: die erste (groesste) Zone des Raums markieren. */
    int i = zone_index_at(room_id, 0x7fffffff, 0x7fffffff);
    if (i < 0) return;
    int b = zone_bit(i);
    s_visited[b >> 3] |= (uint8_t)(1u << (b & 7));
}

int re15_map_visited(unsigned room_id)
{
    for (int i = 0; i < ZONE_COUNT; i++) {
        if (s_map_zones[i].room != (unsigned short)room_id) continue;
        int b = zone_bit(i);
        if ((s_visited[b >> 3] >> (b & 7)) & 1) return 1;
    }
    return 0;
}

static int s_stock_mode = -1;
int re15_map_stock_mode(void)
{
    if (s_stock_mode < 0) {
        const char *e = getenv("RE15_MAP_STOCK");
        s_stock_mode = (e && e[0] == '1') ? 1 : 0;
    }
    return s_stock_mode;
}
void re15_map_stock_set(int v) { s_stock_mode = v; }
