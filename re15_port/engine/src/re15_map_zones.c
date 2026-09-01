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
#include "re15_collision.h"
#include "re15_actor.h"   /* Spielerposition fuer die Ersatz-Zonenbestimmung */
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
/* Abstand eines Punktes zur Zonen-Bbox (0 = drin). Quadriert, aber auf 1/64
 * skaliert, damit int32 auch fuer die grossen Weltkoordinaten reicht. */
static int32_t zone_dist2(const re15_map_zone_t *zn, int32_t x, int32_t z)
{
    int32_t dx = 0, dz = 0;
    if (x < (int32_t)zn->wx0) dx = (int32_t)zn->wx0 - x;
    else if (x > (int32_t)zn->wx1) dx = x - (int32_t)zn->wx1;
    if (z < (int32_t)zn->wz0) dz = (int32_t)zn->wz0 - z;
    else if (z > (int32_t)zn->wz1) dz = z - (int32_t)zn->wz1;
    dx /= 64; dz /= 64;
    return dx * dx + dz * dz;
}

static int zone_index_at(unsigned room, int32_t x, int32_t z)
{
    int best = -1, near = -1;
    int32_t best_area = 0, near_d = 0;
    for (int i = 0; i < ZONE_COUNT; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        if (zn->room != (unsigned short)room) continue;
        {   /* ⛔ NAECHSTGELEGENE statt der ERSTEN Zone als Rueckfall (Nutzer
             * 2026-08-31: "Der positionsmarker ist immer noch falsch im kleinen
             * Bereich ... wenn ich die Tuer wieder zurueck gehe ... auch wieder
             * falsch im grossen Bereich").
             * URSACHE: Die Bereichs-Bboxen decken die unregelmaessigen Grundrisse
             * nicht luecklos ab — zwischen den beiden Bereichen von ROOM1170 liegt
             * ein Streifen, der in KEINER Bbox liegt. Dort griff bisher pauschal die
             * ERSTE Zone des Raums (bei 1170 der grosse Bereich), der Marker sprang
             * also ins falsche Rechteck. Jetzt gewinnt der Bereich, dessen Bbox am
             * naechsten liegt. */
            int32_t d = zone_dist2(zn, x, z);
            if (near < 0 || d < near_d) { near = i; near_d = d; }
        }
        if (x >= (int32_t)zn->wx0 - ZONE_SLACK && x <= (int32_t)zn->wx1 + ZONE_SLACK &&
            z >= (int32_t)zn->wz0 - ZONE_SLACK && z <= (int32_t)zn->wz1 + ZONE_SLACK) {
            int32_t w = (int32_t)zn->wx1 - (int32_t)zn->wx0;
            int32_t d = (int32_t)zn->wz1 - (int32_t)zn->wz0;
            int32_t a = (w / 64) * (d / 64);       /* skaliert, damit int32 reicht */
            if (best < 0 || a < best_area) { best = i; best_area = a; }
        }
    }
    return (best >= 0) ? best : near;
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
    /* ---- GEEICHTE PROJEKTION: die Formel des Originals ------------------------
     * FUN_800473f8 @0x8004741c-0x80047528:
     *     mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
     *     my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
     * Das Original fuehrt dafuer eine Zeile je Raum @0x800768b0, hat aber nur 13 der
     * 38 Zeilen geeicht - der Rest ist der Stub {0,0,1,1}. Der Generator bestimmt die
     * fehlenden Zeilen: MASSSTAB aus der gezeichneten Flaeche der Kachel (das
     * Rechteck IST die Zeichnung des Raums), VERSATZ aus den GEMALTEN Tuersymbolen.
     * Beides ist bestimmt, kein freier Vierparameter-Fit.
     * Ohne Eichung (sx == 0) bleibt es bei der linearen Bbox-Streckung darunter. */
    if (zn->sx && zn->sy) {
        int32_t t  = (((int32_t)x + 32000) * 10 * (int32_t)zn->sx) >> 20;
        int32_t t2 = (((int32_t)z + 32000) * 10 * (int32_t)zn->sy) >> 20;
        int32_t px = (t + 5) / 10 + zn->ox;
        int32_t py = -((t2 + 5) / 10) + zn->oy;
        if (px < rx) px = rx;
        if (px > rx + rw - 1) px = rx + rw - 1;
        if (py < ry) py = ry;
        if (py > ry + rh - 1) py = ry + rh - 1;
        *mx = (int16_t)px; *my = (int16_t)py;
        return 1;
    }
    w = (int32_t)zn->wx1 - (int32_t)zn->wx0;
    d = (int32_t)zn->wz1 - (int32_t)zn->wz0;
    if (w <= 0 || d <= 0) return 0;
    {
        int32_t px = (int32_t)zn->wx0, pz = (int32_t)zn->wz0;
        int32_t fx = x - px, fz = z - pz;
        if (fx < 0) fx = 0; if (fx > w) fx = w;
        if (fz < 0) fz = 0; if (fz > d) fz = d;
        *mx = (int16_t)(rx + (fx * rw) / w);
        /* ⛔ Z WIRD GESPIEGELT — belegt an der Original-Markerformel (FUN_800473f8):
         * dort steht nach der Skalierung ein `t2 = -t2` (die y-Haelfte negiert das
         * Ergebnis, die x-Haelfte nicht). Wachsendes z wandert also im Bild nach OBEN.
         * Meine Projektion hatte das NICHT — der Kommentar hier behauptete sogar
         * ausdruecklich das Gegenteil ("KEINE zusaetzliche Spiegelung"). Folge: der
         * Marker lief senkrecht verkehrt herum (Nutzer 2026-08-31: "Jetzt laeuft er im
         * grossen Rechteck gleich vom Anfang an in die falsche Richtung"). */
        *my = (int16_t)(ry + rh - 1 - (fz * rh) / d);
    }
    return 1;
}

/* Rechteck-Zustand fuer den Zeichner: aktuell (Spieler steht in dieser Zone) schlaegt
 * besucht schlaegt unbesucht. Mehrere Zonen duerfen sich ein Rechteck teilen. */
static int re15_map_player_band(void);

int re15_map_rect_state(unsigned page, unsigned rect_idx)
{
    extern unsigned g_current_room_id;
    int state = RE15_MAP_RECT_UNMAPPED;
    const re15_map_zone_t *cur = re15_map_zone_current();
    /* ⛔ DIESELBE ERSATZ-BESTIMMUNG WIE DER MARKER (Nutzer 2026-08-31: "Der
     * Spielermarker steht im nirgendwo auf der Karte", 2. Abschnitt von 1170).
     * URSACHE: der Marker faellt auf re15_map_zone_at zurueck, wenn die laufend
     * nachgefuehrte Zone noch nicht steht (z.B. direkt nach einem Bereichswechsel,
     * bevor der naechste Spielschritt lief) — diese Funktion tat das NICHT. Dann
     * wurde der Marker gezeichnet, sein Rechteck aber als unbesucht behandelt und
     * gar nicht gemalt: der Marker schwebte im Schwarzen. Jetzt benutzen beide
     * denselben Weg. */
    if (!cur) cur = re15_map_zone_at(g_current_room_id,
                                     g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                     g_actors[RE15_ACTOR_SLOT_PLAYER].z);
    for (int i = 0; i < ZONE_COUNT; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        int fp, fr, hat_etage, passt;
        /* ---- ZWEIT-ZEICHNUNG EINER ZONE AUF EINEM ANDEREN ETAGENBLATT ----------
         * Eine Zone kann auf mehreren Blaettern gezeichnet sein (ROOM1170s zweiter
         * Bereich: Dach-Blatt Seite 5 Rect 0 und 3F-Blatt Seite 4 Rect 3 - dieselbe
         * Zeichnung, 22 von 1152 Pixeln Unterschied). Das Zweit-Rechteck gehoert
         * KEINER eigenen Zone; ohne diesen Zweig blieb es "unbekannt" und wurde
         * dauerhaft grau gemalt, statt den Zustand seiner Zone zu erben. */
        hat_etage = re15_map_floor_lookup(zn->room, zn->idx,
                                          re15_map_player_band(), &fp, &fr);
        passt = (zn->page == page && zn->rect == rect_idx) ||
                (hat_etage && (int)page == fp && (int)rect_idx == fr);
        if (!passt) continue;
        if (cur && zn->room == cur->room && zn->idx == cur->idx) {
            /* Auf einer Etagen-Umschaltung ist NUR das Rechteck dieser Etage
             * aktuell - sonst leuchteten beide Zeichnungen zugleich rot. */
            if (hat_etage) {
                if ((int)page == fp && (int)rect_idx == fr) return RE15_MAP_RECT_CURRENT;
            } else if (zn->page == cur->page && zn->rect == cur->rect) {
                return RE15_MAP_RECT_CURRENT;
            }
        }
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

/* ---- MARKEN (Tueren/Treppen) ------------------------------------------------
 * Nutzer-Report: "die [Tuer] ist auf der Karte nicht eingezeichnet ... auserdem
 * muesste links im kleinen rechteck die Treppe eingezeichnet sein. Wenn das Symbol/
 * der marker fuer Treppen fehlt in re 1.5, musst du ihn halt aus resident Evil 2
 * uebernehmen." RE1.5 zeichnet auf seinen Karten-Seiten weder Tueren noch Treppen
 * (die Rect-Grafik ist eine reine Flaeche); RE2 dagegen setzt gelbe Tuerpunkte in
 * die Grafik und eigene 8x8-Marker fuer Uebergaenge (Tabelle @0x800a9b1c, Icons ab
 * v=0xf0 der Karten-Seite). Weil RE1.5 diese Icons nicht mitliefert, zeichnet der
 * Port sie aus Linien — Tuer als kurzer Strich, Treppe als Leiter-Symbol, in RE2s
 * Tuer-Gelb. Positionen sind vorberechnet (tools/gen_map_zones.py). */
#define MARK_COUNT ((int)(sizeof s_map_marks / sizeof s_map_marks[0]))

int re15_map_mark_count(void) { return MARK_COUNT; }

#define FLOOR_COUNT ((int)(sizeof s_map_floors / sizeof s_map_floors[0]))

/* ⛔ DIE ETAGE KOMMT AUS DER SPIELER-Y, NICHT AUS actor.floor.
 * `g_actors[SLOT_PLAYER].floor` (das +0x82 des Originals) wird im Port NUR beim Laden
 * eines Spielstands geschrieben (re15_savedata.c) - im Spiel pflegt der Treppenlauf
 * statt dessen das GLOBALE Kollisionsband (stair_common.c: re15_collision_set_band aus
 * der Spieler-Y). Wer die Etage an actor.floor haengt, bekommt einen Wert, der sich nie
 * aendert: die Karte blieb deshalb beim Treppenlauf auf der Startetage stehen
 * (Nutzer 2026-09-01: "beim Treppe runterlaufen schaltet er die Karte nicht nach 3F um"
 * und "im Treppenhaus ganz oben bei 3F zeigt die Karte immer noch 1F").
 * Die Y ist die verlaessliche Quelle - der Spielstand-Loader leitet das Band genauso ab
 * (re15_collision_band_from_y). */
static int re15_map_player_band(void)
{
    return re15_collision_band_from_y(g_actors[RE15_ACTOR_SLOT_PLAYER].y);
}

/* ETAGE: liefert fuer (Raum, Band) das Blatt und das Rechteck, auf dem der Raum auf
 * DIESER Etage gezeichnet ist. 1 = gefunden.
 *
 * Nutzer 2026-08-31/09-01: "wenn ich im Treppenhaus oben bin, bin ich auf Ebene 3F ...
 * bei und im Treppenhaus bin ich IMMER auf Ebene 1F laut Karte." Das Treppenhaus ist
 * ROOM1060; es liegt auf Seite 2, deren Titelbild woertlich "POLICE STATION 1F" heisst.
 * Das Original hat keinen Etagenbegriff INNERHALB eines Raums (Seiten-Setzer
 * @0x8004b568 liest nur die Raumnummer; das Spieler-Band DAT_800acad6 kommt im
 * Kartencode nicht vor) - die Umschaltung ist eine PORT-ERGAENZUNG, aber aus den Daten
 * abgeleitet: jede Tuer traegt ihr Band und ihren Zielraum, der Zielraum seine Seite.
 * Fuer ROOM1060: Band 8 -> ROOM1120 (Seite 4 "3F"), Band 4 -> ROOM10C0 (Seite 3 "2F"),
 * Band 0 -> ROOM1040 (Seite 2 "1F"). Tabelle: tools/gen_map_zones.py. */
int re15_map_floor_lookup(unsigned room, int zone, int band, int *page, int *rect)
{
    int i, best = -1, bestd = 0, n = 0;
    for (i = 0; i < FLOOR_COUNT; i++) {
        int d;
        if (s_map_floors[i].room != room) continue;
        if ((int)s_map_floors[i].zone != zone) continue;
        n++;
        d = (int)s_map_floors[i].band - band;
        if (d < 0) d = -d;
        if (best < 0 || d < bestd) { best = i; bestd = d; }
    }
    /* ⛔ NAEHERN NUR BEI MEHREREN ETAGEN. Ein Raum mit nur EINEM Eintrag spannt keine
     * Etagen - dort darf die Tabelle nicht als Auffangregel wirken, sonst zieht sie
     * jedes Band auf dieselbe Seite/Rect. ROOM1170 hat genau einen Eintrag (Band 4);
     * ohne diese Bedingung landete sein zweiter Bereich auf Seite 5 Rect 1 statt
     * Rect 0 - der Pin test_map_re2_system hat das gefangen. */
    if (best < 0 || (n < 2 && bestd != 0)) return 0;
    /* Kein exaktes Band? Dann die NAECHSTE Etage. Auf einem Treppen-Zwischenpodest
     * steht der Spieler auf einem Band, zu dem keine Tuer und damit keine Etage
     * gehoert (ROOM1060 fuehrt 0/2/4/6/8, Etagen gibt es nur zu 0, 4 und 8). Ohne
     * diese Naeherung faellt die Karte dort auf die Raum-Vorgabe zurueck und zeigt
     * mitten im Treppenhaus wieder 1F. */
    if (page) *page = s_map_floors[best].page;
    if (rect) *rect = s_map_floors[best].rect;
    return 1;
}

/* 1, wenn der Spieler auf dieser Kartenseite schon mindestens eine Zone gesehen hat.
 * Riegel fuer das Ebenen-Blaettern: man soll nur Blaetter durchsehen koennen, die man
 * kennt - dieselbe Bedingung, die RE2 an seine Nachbar-Etagen legt. */
int re15_map_page_known(unsigned page)
{
    int i;
    for (i = 0; i < ZONE_COUNT; i++)
        if (s_map_zones[i].page == page && re15_map_zone_visited(&s_map_zones[i]))
            return 1;
    return 0;
}

int re15_map_mark_get(int i, int *page, int *rect, int *mx, int *my, int *kind)
{
    if (i < 0 || i >= MARK_COUNT) return 0;
    {
        const re15_map_mark_t *m = &s_map_marks[i];
        *page = m->page; *rect = m->rect;
        *mx = m->mx; *my = m->my; *kind = m->kind;
        /* Nur zeigen, was der Spieler schon gesehen hat: die Marke gehoert zur Zone
         * mit dieser Nummer — ist sie unbesucht, wird auch das Rechteck nicht
         * gezeichnet, dann waere die Marke ein Hinweis auf Unbekanntes. */
        /* Sichtbar, sobald EINE der beiden Zonen besucht ist. Ein Durchgang
         * zwischen zwei Raeumen wird als EIN Datensatz gefuehrt; zid2 haelt die
         * Zone der anderen Seite (255 = keine). Sonst waere die Tuer unsichtbar,
         * wenn der Spieler zuerst den anderen Raum betritt. */
        if ((s_visited[m->zid >> 3] >> (m->zid & 7)) & 1) return 1;
        if (m->zid2 != 255 &&
            ((s_visited[m->zid2 >> 3] >> (m->zid2 & 7)) & 1)) return 1;
        return 0;
    }
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
