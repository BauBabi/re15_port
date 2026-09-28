/* probe_r30_karten-marken.c - Runde 30, Thema F: MESS-WERKZEUG (kein Pin, kein add_test).
 * Dossier: analysis/befunde_runde30/karten-marken.md
 *
 * Misst OHNE Spiellauf, was die Karten-Tabellen und der Spielstand tun:
 *   A  ZENSUS     Zonen / Besucht-Bits / Etagenzeilen / Raeume ohne Zonenzeile
 *   B  MARKEN     Marken, deren zid auf dem EIGENEN Blatt keine Zonenzeile hat
 *                 ("fremde zid"), und was sichtbar wird, wenn NUR ROOM1000-Ost
 *                 betreten ist
 *   C  WAENDE     je Innenwand: auf welchen Kachel-Index faellt sie (4 = gemalte Wand)
 *   D  SPEICHERN  Stage 1 begehen -> capture -> Karte (memcard) -> frischer Zustand
 *                 -> load -> restore -> Vergleich Rechteck fuer Rechteck
 *   E  TUER 1090  wohin projiziert die Engine die Tuer ROOM1090 -> ROOM1100
 *
 * Aufruf: probe_r30_karten-marken [<kartendatei>]
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
#include "re15_inv_screen.h"
#include "re15_map_zones.h"      /* engine/src: dieselben Tabellen, die die Engine fuehrt */

extern int re15_map_zone_bit_test(int zonen_index);
extern int re15_map_zone_bit_kapazitaet(void);
extern int re15_map_floor_row_visited(unsigned room, int zone, int band);

#define NZ ((int)(sizeof s_map_zones  / sizeof s_map_zones[0]))
#define NM ((int)(sizeof s_map_marks  / sizeof s_map_marks[0]))
#define NW ((int)(sizeof s_map_walls  / sizeof s_map_walls[0]))
#define NF ((int)(sizeof s_map_floors / sizeof s_map_floors[0]))
#define SEITEN 13
#define RMAX   32

/* ---- Kartenblatt lesen: headerlos, 256x256 4bpp, unteres Nibble = links ---------- */
static unsigned char g_px[SEITEN][256][256];
static int g_px_ok[SEITEN];
static void blaetter_laden(void)
{
    int p, x, y;
    static unsigned char roh[256 * 128];
    for (p = 0; p < SEITEN; p++) {
        char pfad[700]; FILE *f;
        snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, p + 1);
        f = fopen(pfad, "rb");
        if (!f) continue;
        if (fread(roh, 1, sizeof roh, f) == sizeof roh) {
            for (y = 0; y < 256; y++)
                for (x = 0; x < 256; x++)
                    g_px[p][y][x] = (x & 1) ? (unsigned char)(roh[y * 128 + (x >> 1)] >> 4)
                                            : (unsigned char)(roh[y * 128 + (x >> 1)] & 15);
            g_px_ok[p] = 1;
        }
        fclose(f);
    }
}
/* Kachel-Index des Rechtecks (page,rect) am Schirmpunkt; -1 = ausserhalb. */
static int kachel_index(int page, int rect, int sx, int sy)
{
    int rx, ry, rw, rh, u, v;
    if (page < 0 || page >= SEITEN || !g_px_ok[page]) return -1;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &rx, &ry, &rw, &rh)) return -1;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &u, &v)) return -1;
    if (sx < rx || sx >= rx + rw || sy < ry || sy >= ry + rh) return -1;
    return g_px[page][(v + (sy - ry)) & 255][(u + (sx - rx)) & 255];
}

/* ---- Spieler setzen ---------------------------------------------------------------- */
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

/* Ein Punkt in der Weltbox, fuer den die Engine GENAU diese Zone meldet. */
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

/* Alle Zonen (Haupt-Zeilen, Leon-Variante) dieser Stage auf JEDEM ihrer Baender begehen. */
static int begehe_stage(unsigned stage)
{
    int i, j, n = 0;
    for (i = 0; i < NZ; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        int32_t x = 0, z = 0; int baender[16], nb = 0;
        if (zn->etage || (zn->room & 1)) continue;
        if (((unsigned)zn->room >> 12) != stage) continue;
        if (!punkt_in_zone(zn, &x, &z)) {
            printf("    [!] ROOM%04X Zone %d: kein Punkt, der diese Zone trifft\n",
                   zn->room, zn->idx);
            continue;
        }
        for (j = 0; j < NF; j++) {
            int k, da = 0;
            if (s_map_floors[j].room != zn->room || s_map_floors[j].zone != zn->idx) continue;
            for (k = 0; k < nb; k++) if (baender[k] == s_map_floors[j].band) da = 1;
            if (!da && nb < 16) baender[nb++] = s_map_floors[j].band;
        }
        if (nb == 0) baender[nb++] = 0;
        for (j = 0; j < nb; j++) stehe(zn->room, x, z, baender[j]);
        n++;
    }
    return n;
}

/* ---- Momentaufnahme dessen, was der Zeichner fragt --------------------------------- */
typedef struct {
    unsigned char rect[SEITEN][RMAX];
    unsigned char marke[512];
    unsigned char zone_sicht[512];      /* Haupt: Zonen-Bit, Gast: Etagen-Bit */
    unsigned char etage[256];
    unsigned char bekannt[SEITEN];
    unsigned char bits[32];
} aufnahme_t;

static void aufnehmen(aufnahme_t *a)
{
    int p, r, i;
    memset(a, 0, sizeof *a);
    for (p = 0; p < SEITEN; p++) {
        int cnt = re15_map_rect_count((unsigned)p);
        for (r = 0; r < cnt && r < RMAX; r++)
            a->rect[p][r] = (unsigned char)re15_map_rect_state((unsigned)p, (unsigned)r);
        a->bekannt[p] = (unsigned char)re15_map_page_known((unsigned)p);
    }
    for (i = 0; i < NM && i < 512; i++) {
        int pg, rc, mx, my, kind;
        a->marke[i] = (unsigned char)re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
    }
    for (i = 0; i < NZ && i < 512; i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        a->zone_sicht[i] = (unsigned char)(zn->etage ? re15_map_zone_etage_besucht(zn)
                                                     : re15_map_zone_visited(zn));
    }
    for (i = 0; i < NF && i < 256; i++)
        a->etage[i] = (unsigned char)re15_map_floor_row_visited(s_map_floors[i].room,
                          s_map_floors[i].zone, s_map_floors[i].band);
    re15_map_visited_export(a->bits);
}

static const char *zname(int s)
{
    return s == RE15_MAP_RECT_CURRENT ? "AKTUELL" : s == RE15_MAP_RECT_VISITED ? "BESUCHT"
         : s == RE15_MAP_RECT_UNVISITED ? "unbesucht" : "ohne-Zone";
}

/* Wer liegt auf diesem Rechteck? (fuer die Ausgabe) */
static void rect_bewohner(int page, int rect, char *out, size_t n)
{
    int i; size_t l = 0; out[0] = 0;
    for (i = 0; i < NZ; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        if ((zn->room & 1) || zn->page != page || zn->rect != rect) continue;
        if (l + 24 >= n) break;
        l += (size_t)snprintf(out + l, n - l, "%sROOM%04X/z%d%s", l ? " " : "",
                              zn->room, zn->idx, zn->etage ? "(Gast)" : "");
    }
}

static int vergleiche(const char *titel, const aufnahme_t *a, const aufnahme_t *b)
{
    int p, r, i, verlust = 0, gewinn = 0, mv = 0, mg = 0, ev = 0, eg = 0, bv = 0, bitdiff = 0;
    printf("  --- %s ---\n", titel);
    for (i = 0; i < 32; i++) {
        unsigned char d = (unsigned char)(a->bits[i] ^ b->bits[i]); int k;
        for (k = 0; k < 8; k++) if ((d >> k) & 1) bitdiff++;
    }
    printf("    Zonen-Bits (visited[32]): %d Unterschiede\n", bitdiff);
    for (i = 0; i < NF; i++) {
        if (a->etage[i] && !b->etage[i]) ev++;
        if (!a->etage[i] && b->etage[i]) eg++;
    }
    printf("    Etagen-Bits: %d verloren, %d hinzugekommen (von %d Zeilen)\n", ev, eg, NF);
    for (p = 0; p < SEITEN; p++) {
        int cnt = re15_map_rect_count((unsigned)p);
        for (r = 0; r < cnt && r < RMAX; r++) {
            char wer[200];
            if (a->rect[p][r] == b->rect[p][r]) continue;
            rect_bewohner(p, r, wer, sizeof wer);
            printf("    Blatt %2d Rect %2d: %-9s -> %-9s  [%s]\n", p, r,
                   zname(a->rect[p][r]), zname(b->rect[p][r]), wer);
            if (a->rect[p][r] > b->rect[p][r]) verlust++; else gewinn++;
        }
        if (a->bekannt[p] && !b->bekannt[p]) { bv++;
            printf("    Blatt %2d: war blaetterbar, ist es nicht mehr\n", p); }
    }
    for (i = 0; i < NM; i++) {
        if (a->marke[i] == b->marke[i]) continue;
        printf("    Marke %3d Blatt %2d rect %3d (%3d,%3d) zid %d: %s -> %s\n", i,
               s_map_marks[i].page, s_map_marks[i].rect, s_map_marks[i].mx, s_map_marks[i].my,
               s_map_marks[i].zid, a->marke[i] ? "sichtbar" : "weg", b->marke[i] ? "sichtbar" : "weg");
        if (a->marke[i]) mv++; else mg++;
    }
    printf("    SUMME: Rechtecke verloren %d / gewonnen %d, Marken verloren %d / gewonnen %d, "
           "Blaetter verloren %d\n", verlust, gewinn, mv, mg, bv);
    return verlust + mv + ev;
}

int main(int argc, char **argv)
{
    const char *karte = (argc > 1) ? argv[1] : "r30_karte.mcr";
    int i, j;

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_map_stock_set(0);
    blaetter_laden();

    /* =============================== A ZENSUS =============================== */
    printf("=== A. ZENSUS ===\n");
    {
        int kap = re15_map_zone_bit_kapazitaet(), maxbit = -1, orte = 0, gast = 0, kollision = 0;
        unsigned char belegt[256]; memset(belegt, 0, sizeof belegt);
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *a = &s_map_zones[i];
            int b = re15_map_zone_bit_test(i);
            if (a->etage) gast++;
            if (b > maxbit) maxbit = b;
            if (b < 0 || b >= kap) { printf("  [!] ROOM%04X z%d: Bit %d ausserhalb\n", a->room, a->idx, b); continue; }
            if (!(a->room & 1) && !a->etage) orte++;
            belegt[b] = 1;
            for (j = i + 1; j < NZ; j++) {
                const re15_map_zone_t *c = &s_map_zones[j];
                if (re15_map_zone_bit_test(j) != b) continue;
                if ((c->room & ~1u) == (a->room & ~1u) && c->idx == a->idx) continue;
                printf("  [KOLLISION] Bit %d: ROOM%04X z%d und ROOM%04X z%d\n",
                       b, a->room, a->idx, c->room, c->idx);
                kollision++;
            }
        }
        { int nb = 0; for (i = 0; i < 256; i++) nb += belegt[i];
          printf("  Zonenzeilen %d (davon Gast-Zeilen %d), Orte (Haupt, Leon) %d\n", NZ, gast, orte);
          printf("  Besucht-Bits: %d verschiedene belegt, hoechstes Bit %d, Feld fasst %d, "
                 "Kollisionen %d\n", nb, maxbit, kap, kollision); }
        printf("  Raumliste: RE15_ROOM_COUNT = %d\n", (int)RE15_ROOM_COUNT);
        printf("  Etagenzeilen (s_map_floors): %d -> %d Bits, im Spielstand: 0 Bytes "
               "(re15_savedata_t fuehrt nur visited[32])\n", NF, NF);
        printf("  sizeof(re15_savedata_t) = %u, offsetof(visited) = %u, offsetof(checksum) = %u\n",
               (unsigned)sizeof(re15_savedata_t), (unsigned)offsetof(re15_savedata_t, visited),
               (unsigned)offsetof(re15_savedata_t, checksum));
        printf("  Marken %d, Innenwaende %d\n", NM, NW);
        printf("  Basisraeume OHNE Zonenzeile:");
        for (i = 0; i < (int)RE15_ROOM_COUNT; i++) {
            int da = 0;
            if (re15_room_ids[i] & 1) continue;
            for (j = 0; j < NZ; j++) if (s_map_zones[j].room == re15_room_ids[i]) { da = 1; break; }
            if (!da) printf(" %04X", (unsigned)re15_room_ids[i]);
        }
        printf("\n");
    }

    /* =============================== B MARKEN =============================== */
    printf("=== B. MARKEN MIT FREMDER zid ===\n");
    {
        int fremd = 0;
        for (i = 0; i < NM; i++) {
            const re15_map_mark_t *m = &s_map_marks[i];
            int traeger_blatt = 0, traeger = 0; unsigned traum = 0; int tpage = -1, tidx = -1;
            for (j = 0; j < NZ; j++) {
                if (s_map_zones[j].zid != m->zid) continue;
                if (!traeger) { traum = s_map_zones[j].room; tpage = s_map_zones[j].page;
                                tidx = s_map_zones[j].idx; }
                traeger = 1;
                if (s_map_zones[j].page == m->page) traeger_blatt = 1;
            }
            if (traeger_blatt) continue;
            fremd++;
            printf("  Marke %3d  Blatt %2d rect %3d (%3d,%3d) kind %d zid %3d zid2 %3d  -> zid gehoert "
                   "ROOM%04X/z%d auf Blatt %d\n", i, m->page, m->rect, m->mx, m->my, m->kind,
                   m->zid, m->zid2, traum, tidx, tpage);
        }
        printf("  SUMME fremde zid: %d von %d Marken\n", fremd, NM);

        /* B2: Marken mit gemaltem Rechteck, deren Rechteck KEINER Zeile ihrer eigenen
         * Zone gehoert (die Kachel wurde spaeter einem anderen Raum zugewiesen). */
        { int fr = 0;
          printf("  --- B2: Marke sitzt auf einem Rechteck, das ihrer Zone nicht gehoert ---\n");
          for (i = 0; i < NM; i++) {
              const re15_map_mark_t *m = &s_map_marks[i];
              int eigen = 0, partner = 0; char wer[200];
              if (m->rect == 255) continue;
              for (j = 0; j < NZ; j++) {
                  if (s_map_zones[j].page != m->page || s_map_zones[j].rect != m->rect) continue;
                  if (s_map_zones[j].zid == m->zid) eigen = 1;
                  if (m->zid2 != 255 && s_map_zones[j].zid == m->zid2) partner = 1;
              }
              if (eigen) continue;
              rect_bewohner(m->page, m->rect, wer, sizeof wer);
              printf("  Marke %3d  Blatt %2d rect %3d (%3d,%3d) kind %d zid %3d zid2 %3d auf_partner %d"
                     "  | Rechteck gehoert: [%s]%s\n", i, m->page, m->rect, m->mx, m->my, m->kind,
                     m->zid, m->zid2, m->auf_partner, wer, partner ? "  (Partner wohnt dort)" : "");
              fr++;
          }
          printf("  SUMME B2: %d\n", fr); }

        /* Versuch: NUR ROOM1000-Ostraum (zid 0) betreten, Spieler steht dort. */
        re15_map_visited_reset();
        { const re15_map_zone_t *zn = re15_map_zone_fuer(0x1000, 0, 2); int32_t x = 0, z = 0;
          if (zn && punkt_in_zone(zn, &x, &z)) stehe(0x1000, x, z, 0); }
        printf("  VERSUCH nur ROOM1000/z0 betreten -> sichtbare Marken je Blatt:\n");
        for (i = 0; i < NM; i++) {
            int pg, rc, mx, my, kind;
            if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
            printf("    Marke %3d Blatt %2d rect %3d (%3d,%3d) kind %d zid %d | rect_state(%d,%d)=%s\n",
                   i, pg, rc, mx, my, kind, s_map_marks[i].zid, pg, rc,
                   zname(re15_map_rect_state((unsigned)pg, (unsigned)rc)));
        }
        /* Gegenprobe: alles AUSSER ROOM1000/z0 betreten */
        re15_map_visited_reset();
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *zn = &s_map_zones[i]; int32_t x = 0, z = 0;
            if (zn->etage || (zn->room & 1)) continue;
            if (zn->room == 0x1000 && zn->idx == 0) continue;
            if (punkt_in_zone(zn, &x, &z)) stehe(zn->room, x, z, 0);
        }
        { int n = 0;
          for (i = 0; i < NM; i++) {
              int pg, rc, mx, my, kind;
              if (s_map_marks[i].zid != 0) continue;
              if (re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) n++;
          }
          printf("  GEGENPROBE alles ausser ROOM1000/z0 betreten -> sichtbare zid-0-Marken: %d\n", n); }
    }

    /* =============================== C WAENDE =============================== */
    printf("=== C. INNENWAENDE GEGEN DIE KACHEL ===\n");
    for (i = 0; i < NW; i++) {
        const re15_map_wall_t *w = &s_map_walls[i];
        int senk = (w->x0 == w->x1), a0, a1, s, h[16], aus = 0, n = 0;
        memset(h, 0, sizeof h);
        a0 = senk ? (w->y0 < w->y1 ? w->y0 : w->y1) : (w->x0 < w->x1 ? w->x0 : w->x1);
        a1 = senk ? (w->y0 < w->y1 ? w->y1 : w->y0) : (w->x0 < w->x1 ? w->x1 : w->x0);
        for (s = a0; s <= a1; s++) {
            int k = kachel_index(w->page, w->rect, senk ? w->x0 : s, senk ? s : w->y0);
            n++;
            if (k < 0) aus++; else h[k & 15]++;
        }
        printf("  Wand %2d Blatt %2d rect %2d (%3d,%3d)-(%3d,%3d) zid %2d: %2d px | Index0 %2d  "
               "Index1(Koerper) %2d  Index4(Wand) %2d  sonst %2d  ausserhalb %d\n",
               i, w->page, w->rect, w->x0, w->y0, w->x1, w->y1, w->zid, n, h[0], h[1], h[4],
               n - aus - h[0] - h[1] - h[4], aus);
    }

    /* =============================== D SPEICHERN ============================ */
    printf("=== D. SPEICHERN / LADEN ===\n");
    {
        aufnahme_t vor, nach, nach2; re15_savedata_t sd, zur, alt; uint16_t rr = 0; int n;
        const int32_t SX = -17150, SZ = -11960;       /* ROOM1150, Spawn der Raumliste */

        /* ---- D1: frischer Prozess laedt den Stand --------------------------------- */
        re15_map_visited_reset();
        n = begehe_stage(1);
        stehe(0x1150, SX, SZ, 0);
        aufnehmen(&vor);
        printf("  begangen: %d Orte der Stage 1, Speicherort ROOM1150\n", n);
        re15_savedata_capture(&sd, 1234, 0);
        remove(karte);
        if (re15_memcard_save(karte, 0, &sd, "R30 KARTE") != 0) { printf("  FAIL: Karte schreiben\n"); return 1; }
        /* frischer Prozess = Null-Zustand der beiden Felder */
        re15_map_visited_reset();
        if (re15_memcard_load(karte, 0, &zur) != 0) { printf("  FAIL: Karte lesen\n"); return 1; }
        printf("  Karte: Version %u, Raum %04X, visited[] bytegleich zum Geschriebenen: %s\n",
               (unsigned)zur.version, (unsigned)zur.room,
               memcmp(zur.visited, sd.visited, 32) == 0 ? "ja" : "NEIN");
        if (re15_savedata_restore(&zur, &rr) != 0) { printf("  FAIL: restore\n"); return 1; }
        stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
        aufnehmen(&nach);
        vergleiche("D1 vor dem Speichern  ->  nach dem Laden im FRISCHEN Prozess", &vor, &nach);

        /* ---- D2: dieselbe Sitzung laedt einen AELTEREN Stand ----------------------- */
        re15_map_visited_reset();
        stehe(0x1150, SX, SZ, 0);
        re15_savedata_capture(&alt, 100, 0);            /* alter Stand: nur ROOM1150 */
        { aufnahme_t soll; aufnehmen(&soll);
          begehe_stage(1);                              /* weitergespielt ...            */
          stehe(0x1150, SX, SZ, 0);
          /* ... gestorben, Titel, LOAD GAME: der Lade-Weg ruft KEIN visited_reset */
          if (re15_savedata_restore(&alt, &rr) != 0) { printf("  FAIL: restore alt\n"); return 1; }
          stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
          aufnehmen(&nach2);
          vergleiche("D2 Soll des ALTEN Stands  ->  derselbe Stand, in LAUFENDER Sitzung geladen",
                     &soll, &nach2); }
    }

    /* ---- D3: ein ALT-Stand (v6, Bits = laufende Zonen-Nummer) ------------------------
     * re15_savedata_restore verwirft Besucht-Bits nur bei `in->version < 8`. Die
     * Pruefung laeuft aber NACH re15_savedata_validate, und das stempelt jeden
     * v2..v6-Stand auf RE15_SAVE_VERSION hoch. Gemessen wird, ob die alten Bits
     * trotzdem importiert werden. */
    {
        re15_savedata_v6_t a6; re15_savedata_t puffer; uint16_t rr = 0; unsigned char bits[32];
        const unsigned char *p; uint32_t sum = 0; size_t k; int n = 0, rc;
        memset(&a6, 0, sizeof a6);
        a6.magic = RE15_SAVE_MAGIC; a6.version = 6; a6.room = 0x1120; a6.player_hp = 100;
        a6.visited[1] = 0x80;            /* Bit 15 */
        a6.visited[2] = 0x05;            /* Bits 16, 18 - der Nutzer-Stand aus dem Kommentar */
        p = (const unsigned char *)&a6;
        for (k = 0; k < offsetof(re15_savedata_v6_t, checksum); k++) sum += p[k];
        a6.checksum = sum;
        memset(&puffer, 0, sizeof puffer);
        memcpy(&puffer, &a6, sizeof a6);
        re15_map_visited_reset();
        rc = re15_savedata_restore(&puffer, &rr);
        re15_map_visited_export(bits);
        for (k = 0; k < 32; k++) { int b; for (b = 0; b < 8; b++) n += (bits[k] >> b) & 1; }
        printf("  --- D3 Alt-Stand v6 mit den Bits 15,16,18 (alte Bedeutung) ---\n");
        printf("    restore rc=%d, Raum %04X, importierte Bits: %d  (Soll laut Kommentar "
               "re15_savedata.c:247-257: 0)\n", rc, (unsigned)rr, n);
        re15_map_visited_reset();
    }

    /* =============================== E TUER 1090 ============================ */
    printf("=== E. TUER ROOM1090 -> ROOM1100 (Door slot 1, Band 6) ===\n");
    {
        const re15_map_zone_t *zn = re15_map_zone_fuer(0x1090, 0, 3);
        int rx, ry, rw, rh; int16_t mx = 0, my = 0;
        if (zn && re15_map_rect_geometry(3, zn->rect, &rx, &ry, &rw, &rh)) {
            int a, b;
            re15_map_zone_marker(zn, -5820, -18690, rx, ry, rw, rh, &mx, &my);
            printf("  Gast-Zeile ROOM1090 auf Blatt 3: rect %d (%d,%d) %dx%d, zid %d\n",
                   zn->rect, rx, ry, rw, rh, zn->zid);
            printf("  Trigger-Mitte (-5820,-18690) -> Engine-Projektion (%d,%d)\n", mx, my);
            for (b = -2; b <= 2; b++) {
                printf("    y=%3d:", my + b);
                for (a = -4; a <= 4; a++)
                    printf(" %2d/%2d", kachel_index(3, zn->rect, mx + a, my + b),
                           kachel_index(3, 6, mx + a, my + b));
                printf("   (Index rect%d / rect6=ROOM1100, x=%d..%d)\n", zn->rect, mx - 4, mx + 4);
            }
        } else printf("  keine Gast-Zeile gefunden\n");
        printf("  Marke im Bestand: (188,180) -> Kachel-Index je Rechteck von Blatt 3:");
        for (i = 0; i < re15_map_rect_count(3); i++)
            printf(" r%d=%d", i, kachel_index(3, i, 188, 180));
        printf("\n");
    }
    return 0;
}
