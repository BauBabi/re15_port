/* Karte: SPEICHERN UND LADEN VERLIERT NICHTS UND ERFINDET NICHTS.
 *
 * NUTZER-BEFUND 2026-09-27 (Runde 30, Thema F): "Ausserdem glaube ich, das wenn das
 * spiel Gespeichert und dann geladen wird, Teile der Karte die ich bereits
 * freigeschaltet habe verloren gegangen sind...."
 *
 * GEMESSEN am Stand d98e9639 (Sonden probe_r30_karten-marken / _rundlauf,
 * analysis/befunde_runde30/karten-marken.md §2.6 / §2.7):
 *   Rundlauf je Ort   102 Orte, 91 verlustfrei, 11 mit Abweichung:
 *                     verlorene Etagen-Bits 25, Rechtecke 25, Marken 16
 *                     (Zonen-Bits: 0 verloren - die kamen immer durch)
 *   D2 laufende Sitzung   16 Etagen-Bits ZU VIEL nach dem Laden eines aelteren Stands
 *   D3 Alt-Stand v6       3 Bits importiert, Soll 0
 * URSACHE: die Etagen-Bits (s_visited_floor) standen nicht im Spielstand.
 *
 * VORBILD-GRUNDSATZ RE2: alles, was sein Kartenzeichner liest, liegt im gespeicherten
 * Block [0x800D44A4, +0x798) - Lade-Kopie info/re2leon/COMMON/BIN/MEM_CARD.BIN @Datei
 * 0x13D0 (`jal 0x80010778` @0x801C0DF8, `addiu a2,zero,1944` @0x801C0DFC).
 *
 * Der Riegel faehrt den ECHTEN Weg capture -> Karte -> load -> restore, je Ort einzeln.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "re15_room.h"
#include "re15_room_list.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_map_zones.h"      /* engine/src: dieselben Tabellen, die die Engine fuehrt */

#define NZ ((int)(sizeof s_map_zones  / sizeof s_map_zones[0]))
#define NM ((int)(sizeof s_map_marks  / sizeof s_map_marks[0]))
#define NF ((int)(sizeof s_map_floors / sizeof s_map_floors[0]))
#define SEITEN 13
#define RMAX   32
#define KARTE  "r30_speichern_laden.mcr"

extern int re15_map_zone_bit_test(int zonen_index);

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static void stehe(unsigned room, int32_t x, int32_t z, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_current_room_id = room;
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->z = z;
    pl->y = -(int32_t)band * 0x708;                 /* band_from_y = -(y/0x708) */
    re15_collision_set_band(band);
    re15_map_zone_update(room, x, z);
}

static int punkt_in_zone(const re15_map_zone_t *zn, int32_t *px, int32_t *pz)
{
    int gx, gy, ring;
    for (ring = 0; ring <= 8; ring++)
        for (gy = 8 - ring; gy <= 8 + ring; gy++)
            for (gx = 8 - ring; gx <= 8 + ring; gx++) {
                int32_t x, z; const re15_map_zone_t *t;
                if (gx < 1 || gx > 15 || gy < 1 || gy > 15) continue;
                x = zn->wx0 + (int32_t)(((int64_t)(zn->wx1 - zn->wx0) * gx) / 16);
                z = zn->wz0 + (int32_t)(((int64_t)(zn->wz1 - zn->wz0) * gy) / 16);
                t = re15_map_zone_at(zn->room, x, z);
                if (t && t->room == zn->room && t->idx == zn->idx) { *px = x; *pz = z; return 1; }
            }
    return 0;
}

/* Eine Haupt-Zonenzeile auf JEDEM ihrer Baender begehen. Rueckgabe: Zahl der Baender. */
static int begehe_zeile(int i)
{
    const re15_map_zone_t *zn = &s_map_zones[i];
    int32_t x = 0, z = 0; int baender[16], nb = 0, j;
    if (!punkt_in_zone(zn, &x, &z)) return -1;
    for (j = 0; j < NF; j++) {
        int k, da = 0;
        if (s_map_floors[j].room != zn->room || s_map_floors[j].zone != zn->idx) continue;
        for (k = 0; k < nb; k++) if (baender[k] == s_map_floors[j].band) da = 1;
        if (!da && nb < 16) baender[nb++] = s_map_floors[j].band;
    }
    if (nb == 0) baender[nb++] = 0;
    for (j = 0; j < nb; j++) stehe(zn->room, x, z, baender[j]);
    return nb;
}

/* In den neutralen Speicherraum gehen: ROOM1150, fuer ROOM1150 selbst ROOM1130. */
static int in_den_speicherraum(unsigned nicht)
{
    unsigned sraum = (nicht == 0x1150) ? 0x1130 : 0x1150;
    int q; int32_t px = 0, pz = 0;
    for (q = 0; q < NZ; q++)
        if (s_map_zones[q].room == sraum && !s_map_zones[q].etage) {
            if (!punkt_in_zone(&s_map_zones[q], &px, &pz)) return 0;
            stehe(sraum, px, pz, 0);
            return 1;
        }
    return 0;
}

typedef struct {
    unsigned char rect[SEITEN][RMAX];
    unsigned char marke[512];
    unsigned char bits[32];
    unsigned char etage[16];
} aufnahme_t;

static void aufnehmen(aufnahme_t *a)
{
    int p, r, i;
    memset(a, 0, sizeof *a);
    for (p = 0; p < SEITEN; p++) {
        int cnt = re15_map_rect_count((unsigned)p);
        for (r = 0; r < cnt && r < RMAX; r++)
            a->rect[p][r] = (unsigned char)re15_map_rect_state((unsigned)p, (unsigned)r);
    }
    for (i = 0; i < NM && i < 512; i++) {
        int pg, rc, mx, my, kind;
        a->marke[i] = (unsigned char)re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
    }
    re15_map_visited_export(a->bits);
    re15_map_visited_floor_export(a->etage);
}

static int bitzahl(const unsigned char *b, int n)
{
    int k, z = 0, j;
    for (k = 0; k < n; k++) for (j = 0; j < 8; j++) z += (b[k] >> j) & 1;
    return z;
}

/* Zaehlt, was zwischen zwei Aufnahmen verloren ging (vor > nach) und was dazukam. */
typedef struct { int bit_anders, etage_weg, etage_neu, rect_weg, rect_neu, marke_weg, marke_neu; } diff_t;
static void vergleiche(const aufnahme_t *vor, const aufnahme_t *nach, diff_t *d)
{
    int k, b, p, r;
    memset(d, 0, sizeof *d);
    for (k = 0; k < 32; k++) { unsigned char x = (unsigned char)(vor->bits[k] ^ nach->bits[k]);
        for (b = 0; b < 8; b++) d->bit_anders += (x >> b) & 1; }
    for (k = 0; k < 16; k++)
        for (b = 0; b < 8; b++) {
            int v = (vor->etage[k] >> b) & 1, n = (nach->etage[k] >> b) & 1;
            if (v && !n) d->etage_weg++;
            if (!v && n) d->etage_neu++;
        }
    for (p = 0; p < SEITEN; p++) { int cnt = re15_map_rect_count((unsigned)p);
        for (r = 0; r < cnt && r < RMAX; r++) {
            if (vor->rect[p][r] > nach->rect[p][r]) d->rect_weg++;
            else if (vor->rect[p][r] < nach->rect[p][r]) d->rect_neu++; } }
    for (k = 0; k < NM && k < 512; k++) {
        if (vor->marke[k] && !nach->marke[k]) d->marke_weg++;
        if (!vor->marke[k] && nach->marke[k]) d->marke_neu++; }
}

/* capture -> Karte -> (optional: frischer Prozess) -> load -> restore -> wieder hinstellen */
static int rundlauf(int frisch)
{
    re15_savedata_t sd, zur; uint16_t rr = 0;
    re15_savedata_capture(&sd, 1000, 0);
    remove(KARTE);
    if (re15_memcard_save(KARTE, 0, &sd, "R30 KARTE") != 0) return 0;
    if (frisch) re15_map_visited_reset();
    memset(&zur, 0, sizeof zur);
    if (re15_memcard_load(KARTE, 0, &zur) != 0) return 0;
    if (re15_savedata_restore(&zur, &rr) != 0) return 0;
    stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
    return 1;
}

static uint32_t summe(const void *p, size_t n)
{
    const uint8_t *b = (const uint8_t *)p; uint32_t s = 0; size_t i;
    for (i = 0; i < n; i++) s += b[i];
    return s;
}

int main(void)
{
    char t[260];
    int i;

    setvbuf(stdout, NULL, _IONBF, 0);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_map_stock_set(0);

    printf("=== Karte: Speichern und Laden verliert nichts und erfindet nichts ===\n");

    /* =================== 0. STRUKTUR UND SCHLUESSEL ================================= */
    printf("  sizeof(re15_savedata_t) = %u, offsetof visited %u, visited_floor %u, files %u, "
           "checksum %u, Version %u\n", (unsigned)sizeof(re15_savedata_t),
           (unsigned)offsetof(re15_savedata_t, visited),
           (unsigned)offsetof(re15_savedata_t, visited_floor),
           (unsigned)offsetof(re15_savedata_t, files),
           (unsigned)offsetof(re15_savedata_t, checksum), (unsigned)RE15_SAVE_VERSION);
    CHECK("SPEICHER-VERTRAG v9: Version 9", RE15_SAVE_VERSION == 9);
    CHECK("SPEICHER-VERTRAG v9: sizeof 944 (vorher 904)", sizeof(re15_savedata_t) == 944);
    CHECK("SPEICHER-VERTRAG v9: visited_floor[16] bei 900 = dort sass das Pruefwort von v7/v8",
          offsetof(re15_savedata_t, visited_floor) == 900 &&
          sizeof(((re15_savedata_t *)0)->visited_floor) == 16);
    CHECK("SPEICHER-VERTRAG v9: files[24] bei 916, unmittelbar vor checksum bei 940",
          offsetof(re15_savedata_t, files) == 916 &&
          sizeof(((re15_savedata_t *)0)->files) == 24 &&
          offsetof(re15_savedata_t, checksum) == 940);
    CHECK("der Block passt in einen Kartenblock (0x2000 - 0x100 Kopf)",
          sizeof(re15_savedata_t) <= 0x2000 - 0x100);
    {
        int ohne = 0, doppelt = 0, ueber = 0, j, k;
        for (j = 0; j < NF; j++) {
            int bj = re15_map_floor_bit_test(j);
            if (bj < 0) { ohne++;
                printf("     Etagenzeile %d ROOM%04X/z%d Band %d Blatt %d: KEIN Bit\n", j,
                       s_map_floors[j].room, s_map_floors[j].zone, s_map_floors[j].band,
                       s_map_floors[j].page);
                continue; }
            if (bj >= re15_map_floor_bit_kapazitaet()) ueber++;
            for (k = 0; k < j; k++) {
                int selbe = ((s_map_floors[k].room & ~1) == (s_map_floors[j].room & ~1) &&
                             s_map_floors[k].zone == s_map_floors[j].zone &&
                             s_map_floors[k].band == s_map_floors[j].band &&
                             s_map_floors[k].page == s_map_floors[j].page);
                if (!selbe && re15_map_floor_bit_test(k) == bj) { doppelt++;
                    printf("     Etagenzeilen %d und %d teilen Bit %d\n", k, j, bj); }
                if (selbe && re15_map_floor_bit_test(k) != bj) { doppelt++;
                    printf("     Etagenzeilen %d und %d (derselbe Ort) tragen verschiedene "
                           "Bits\n", k, j); }
            }
        }
        snprintf(t, sizeof t, "ABDECKUNG: die Etagen-Tabelle fuehrt Zeilen - %d", NF);
        CHECK(t, NF >= 50);
        snprintf(t, sizeof t, "jede Etagenzeile hat ein Etagen-Bit - %d ohne", ohne);
        CHECK(t, ohne == 0);
        snprintf(t, sizeof t, "kein Bit wird von zwei verschiedenen Etagen geteilt; Leon- und "
                 "Elza-Zeile teilen eins - %d Verstoesse", doppelt);
        CHECK(t, doppelt == 0);
        snprintf(t, sizeof t, "die Bit-Tabelle (%d Zeilen) passt in das Feld (%d Bits)",
                 re15_map_floor_bit_count(), re15_map_floor_bit_kapazitaet());
        CHECK(t, ueber == 0 && re15_map_floor_bit_count() <= re15_map_floor_bit_kapazitaet());
        /* ⛔ NUR ANHAENGEN: die ersten 25 Bits sind seit v9 vergeben und stehen in
         * Spielstaenden. Stichprobe an beiden Enden der Tabelle. */
        CHECK("Bit 0 = ROOM1060 Band 0 Blatt 2, Bit 24 = ROOM50D0 Band 3 Blatt 10 (nur anhaengen)",
              re15_map_floor_bit_test(0) == 0 && s_map_floors[0].room == 0x1060 &&
              s_map_floors[0].band == 0 && s_map_floors[0].page == 2 &&
              re15_map_floor_bit_test(48) == 24 && s_map_floors[48].room == 0x50D0 &&
              s_map_floors[48].band == 3 && s_map_floors[48].page == 10);
    }

    /* =================== F. RUNDLAUF JE ORT ======================================== */
    {
        int orte = 0, ohne_punkt = 0, sauber = 0, mit_etage = 0;
        diff_t ges; memset(&ges, 0, sizeof ges);
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *zn = &s_map_zones[i];
            aufnahme_t vor, nach; diff_t d;
            if (zn->etage || (zn->room & 1)) continue;
            orte++;
            re15_map_visited_reset();
            if (begehe_zeile(i) < 0) { ohne_punkt++; continue; }
            if (!in_den_speicherraum(zn->room)) { printf("  FAIL: Speicherraum\n"); return 1; }
            aufnehmen(&vor);
            if (bitzahl(vor.etage, 16) > 0) mit_etage++;
            if (!rundlauf(1)) { printf("  FAIL: Rundlauf ROOM%04X\n", zn->room); return 1; }
            aufnehmen(&nach);
            vergleiche(&vor, &nach, &d);
            if (d.bit_anders || d.etage_weg || d.etage_neu || d.rect_weg || d.rect_neu ||
                d.marke_weg || d.marke_neu) {
                printf("     ROOM%04X/z%d (Bit %d): Zonen-Bits %d anders, Etagen-Bits -%d/+%d, "
                       "Rechtecke -%d/+%d, Marken -%d/+%d\n", zn->room, zn->idx,
                       re15_map_zone_bit_test(i), d.bit_anders, d.etage_weg, d.etage_neu,
                       d.rect_weg, d.rect_neu, d.marke_weg, d.marke_neu);
            } else sauber++;
            ges.bit_anders += d.bit_anders; ges.etage_weg += d.etage_weg;
            ges.etage_neu += d.etage_neu;   ges.rect_weg += d.rect_weg;
            ges.rect_neu += d.rect_neu;     ges.marke_weg += d.marke_weg;
            ges.marke_neu += d.marke_neu;
        }
        printf("  %d Orte, %d ohne treffbaren Punkt, %d verlustfrei, %d mit Etagenzeile\n",
               orte, ohne_punkt, sauber, mit_etage);
        snprintf(t, sizeof t, "F ABDECKUNG: alle Orte begehbar - %d Orte, %d ohne Punkt",
                 orte, ohne_punkt);
        CHECK(t, orte >= 100 && ohne_punkt == 0);
        snprintf(t, sizeof t, "F ABDECKUNG: Orte mit Etagen-Bits sind dabei (vorher verloren "
                 "genau diese 11) - %d", mit_etage);
        CHECK(t, mit_etage >= 11);
        snprintf(t, sizeof t, "F Zonen-Bits: 0 veraendert - %d", ges.bit_anders);
        CHECK(t, ges.bit_anders == 0);
        snprintf(t, sizeof t, "F Etagen-Bits: 0 verloren (vorher 25), 0 gewonnen - %d / %d",
                 ges.etage_weg, ges.etage_neu);
        CHECK(t, ges.etage_weg == 0 && ges.etage_neu == 0);
        snprintf(t, sizeof t, "F Rechtecke: 0 verloren (vorher 25), 0 gewonnen - %d / %d",
                 ges.rect_weg, ges.rect_neu);
        CHECK(t, ges.rect_weg == 0 && ges.rect_neu == 0);
        snprintf(t, sizeof t, "F Marken: 0 verloren (vorher 16), 0 gewonnen - %d / %d",
                 ges.marke_weg, ges.marke_neu);
        CHECK(t, ges.marke_weg == 0 && ges.marke_neu == 0);
        snprintf(t, sizeof t, "F alle %d Orte verlustfrei (vorher 91 von 102)", orte);
        CHECK(t, sauber == orte);
    }

    /* =================== D2. LADEN IN LAUFENDER SITZUNG ============================ */
    {
        aufnahme_t alt, nach; diff_t d; re15_savedata_t sd, zur; uint16_t rr = 0;
        /* der ALTE Stand: nur der Speicherraum ist besucht */
        re15_map_visited_reset();
        in_den_speicherraum(0);
        aufnehmen(&alt);
        re15_savedata_capture(&sd, 500, 0);
        remove(KARTE);
        CHECK("D2 der alte Stand wird geschrieben",
              re15_memcard_save(KARTE, 0, &sd, "R30 ALT") == 0);
        /* die Sitzung laeuft weiter: ALLES wird begangen */
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *zn = &s_map_zones[i];
            if (zn->etage || (zn->room & 1)) continue;
            begehe_zeile(i);
        }
        {   unsigned char e[16]; re15_map_visited_floor_export(e);
            snprintf(t, sizeof t, "D2 ABDECKUNG: die laufende Sitzung traegt Etagen-Bits - %d",
                     bitzahl(e, 16));
            CHECK(t, bitzahl(e, 16) >= 25); }
        /* ... und dann wird der alte Stand geladen, OHNE Neustart (kein reset) */
        memset(&zur, 0, sizeof zur);
        CHECK("D2 der alte Stand wird gelesen", re15_memcard_load(KARTE, 0, &zur) == 0);
        CHECK("D2 restore", re15_savedata_restore(&zur, &rr) == 0);
        stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
        aufnehmen(&nach);
        vergleiche(&alt, &nach, &d);
        snprintf(t, sizeof t, "D2 kein Etagen-Bit des vorigen Laufs bleibt stehen (vorher 16 zu "
                 "viel) - gewonnen %d, verloren %d", d.etage_neu, d.etage_weg);
        CHECK(t, d.etage_neu == 0 && d.etage_weg == 0);
        snprintf(t, sizeof t, "D2 kein Rechteck und keine Marke erscheint, die der Stand nie "
                 "besucht hat - Rechtecke +%d, Marken +%d, Zonen-Bits %d anders",
                 d.rect_neu, d.marke_neu, d.bit_anders);
        CHECK(t, d.rect_neu == 0 && d.marke_neu == 0 && d.bit_anders == 0);
    }

    /* =================== D3. ALT-STAENDE =========================================== */
    {   /* v6: die Bits bedeuteten die laufende Zonen-Nummer des Generators */
        re15_savedata_v6_t a6; re15_savedata_t sd; uint16_t rr = 0; unsigned char bits[32], e[16];
        int k, leer = 1;
        memset(&a6, 0, sizeof a6);
        a6.magic = RE15_SAVE_MAGIC; a6.version = 6; a6.room = 0x1120; a6.player_hp = 100;
        a6.visited[15 >> 3] |= (uint8_t)(1u << (15 & 7));
        a6.visited[16 >> 3] |= (uint8_t)(1u << (16 & 7));
        a6.visited[18 >> 3] |= (uint8_t)(1u << (18 & 7));
        a6.checksum = summe(&a6, offsetof(re15_savedata_v6_t, checksum));
        memset(&sd, 0, sizeof sd);
        memcpy(&sd, &a6, sizeof a6);
        re15_map_visited_reset();
        CHECK("D3 v6-Stand wird angenommen", re15_savedata_restore(&sd, &rr) == 0 && rr == 0x1120);
        re15_map_visited_export(bits); re15_map_visited_floor_export(e);
        snprintf(t, sizeof t, "D3 v6: KEIN Besucht-Bit alter Bedeutung importiert (vorher 3) - %d",
                 bitzahl(bits, 32));
        CHECK(t, bitzahl(bits, 32) == 0 && bitzahl(e, 16) == 0);
        CHECK("D3 v6: validate hebt auf die aktuelle Version, FILE-Liste leer",
              re15_savedata_validate(&sd) == 0 && sd.version == RE15_SAVE_VERSION);
        for (k = 0; k < (int)sizeof sd.files; k++) if (sd.files[k] != 0xFF) leer = 0;
        CHECK("D3 v6: files = 24 x 0xFF", leer);
    }
    {   /* v7 und v8: dasselbe Layout wie v9 ohne die beiden neuen Felder, Pruefwort @900 */
        int ver;
        for (ver = 7; ver <= 8; ver++) {
            static uint8_t roh[sizeof(re15_savedata_t)];
            re15_savedata_t sd; uint32_t ck; size_t off = offsetof(re15_savedata_t, visited_floor);
            int k, leer = 1, b1060 = -1, b1080 = -1, q, j, e1080 = 0, e1060 = 0, e1060_haupt = 0;
            for (q = 0; q < NZ; q++) {
                if (s_map_zones[q].etage) continue;
                if (s_map_zones[q].room == 0x1060) b1060 = re15_map_zone_bit_test(q);
                if (s_map_zones[q].room == 0x1080) b1080 = re15_map_zone_bit_test(q);
            }
            memset(&sd, 0, sizeof sd);
            sd.magic = RE15_SAVE_MAGIC; sd.version = (uint32_t)ver; sd.room = 0x1150;
            sd.player_hp = 100;
            if (b1060 >= 0) sd.visited[b1060 >> 3] |= (uint8_t)(1u << (b1060 & 7));
            if (b1080 >= 0) sd.visited[b1080 >> 3] |= (uint8_t)(1u << (b1080 & 7));
            /* Alt-Block nachbauen: Pruefwort bei 900 ueber [0,900), dahinter Muell. */
            memset(roh, 0xA5, sizeof roh);
            memcpy(roh, &sd, off);
            ck = summe(roh, off);
            memcpy(roh + off, &ck, sizeof ck);
            memcpy(&sd, roh, sizeof sd);
            snprintf(t, sizeof t, "v%d ABDECKUNG: ROOM1060 und ROOM1080 haben ein Zonen-Bit", ver);
            CHECK(t, b1060 >= 0 && b1080 >= 0);
            snprintf(t, sizeof t, "v%d-Block (Pruefwort bei 900) wird angenommen und gehoben", ver);
            CHECK(t, re15_savedata_validate(&sd) == 0 && sd.version == RE15_SAVE_VERSION);
            snprintf(t, sizeof t, "v%d: frisches Pruefwort stimmt", ver);
            CHECK(t, sd.checksum == re15_savedata_checksum(&sd));
            for (k = 0; k < (int)sizeof sd.files; k++) if (sd.files[k] != 0xFF) leer = 0;
            snprintf(t, sizeof t, "v%d: files = 24 x 0xFF", ver);
            CHECK(t, leer);
            for (j = 0; j < NF; j++) {
                int bj = re15_map_floor_bit_test(j), an;
                if (bj < 0 || (s_map_floors[j].room & 1)) continue;
                an = (sd.visited_floor[bj >> 3] >> (bj & 7)) & 1;
                if (s_map_floors[j].room == 0x1080 && an) e1080++;
                if (s_map_floors[j].room == 0x1060 && an) {
                    e1060++;
                    if (s_map_floors[j].page == 2 && s_map_floors[j].band == 0) e1060_haupt = 1;
                }
            }
            if (ver == 7) {
                snprintf(t, sizeof t, "v7: Besucht-Bits alter Bedeutung verworfen, keine "
                         "Etagen-Bits - %d / %d", bitzahl(sd.visited, 32),
                         bitzahl(sd.visited_floor, 16));
                CHECK(t, bitzahl(sd.visited, 32) == 0 && bitzahl(sd.visited_floor, 16) == 0);
            } else {
                snprintf(t, sizeof t, "v8: die Zonen-Bits bleiben - %d", bitzahl(sd.visited, 32));
                CHECK(t, bitzahl(sd.visited, 32) == 2);
                snprintf(t, sizeof t, "v8 einbaendiger Ort (Fahrstuhl ROOM1080): Etagen-Bit := "
                         "Zonen-Bit, alle 3 Blaetter - %d", e1080);
                CHECK(t, e1080 == 3);
                snprintf(t, sizeof t, "v8 mehrbaendiger Ort (Treppenhaus ROOM1060): NUR die "
                         "Haupt-Zeile (1F) gilt als begangen - %d Bit(s)", e1060);
                CHECK(t, e1060 == 1 && e1060_haupt == 1);
                snprintf(t, sizeof t, "v8: sonst kein Etagen-Bit - gesamt %d",
                         bitzahl(sd.visited_floor, 16));
                CHECK(t, bitzahl(sd.visited_floor, 16) == 4);
            }
            /* Negativ-Kontrolle: ein verfaelschter Alt-Block wird abgewiesen. */
            roh[12] ^= 1;
            memcpy(&sd, roh, sizeof sd);
            snprintf(t, sizeof t, "v%d: verfaelschter Alt-Block wird abgewiesen", ver);
            CHECK(t, re15_savedata_validate(&sd) != 0);
        }
    }
    {   /* v9: ein verfaelschtes Etagen-Feld faellt am Pruefwort auf */
        re15_savedata_t sd;
        re15_map_visited_reset();
        in_den_speicherraum(0);
        re15_savedata_capture(&sd, 1, 0);
        CHECK("v9: ein frischer Stand validiert", re15_savedata_validate(&sd) == 0);
        sd.visited_floor[0] ^= 1;
        CHECK("v9: visited_floor liegt unter dem Pruefwort", re15_savedata_validate(&sd) != 0);
    }

    remove(KARTE);
    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
