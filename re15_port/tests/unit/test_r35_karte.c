/* =============================================================================
 * Runde 35 Spur G "karte" — Mess-Sonde und Riegel fuer die vier Kartenbefunde
 * =============================================================================
 * Nutzer 2026-10-03 (analysis/befunde_runde35/AUFTRAG.md, woertlich):
 *   1. "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."
 *   2. "In ROOM 11F0 taucht nicht auf der Karte auf, wenn man drin ist."
 *      "In ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist."
 *   3. "In ROOM 1230 bekomme ich die Map von ROOM 11E0."
 *   4. "In ROOM 1210 ist der Korridor falsch und so gut wie alle Tueren fehlen"
 * Dossier: analysis/befunde_runde35/G_karte.md.
 *
 * Gemessen wird der ECHTE Pfad, den das Spiel beim Oeffnen der Karte faehrt (wie
 * tests/integration/test_map_raum_live.c): Raum laden (scd_room_reenter), Spieler
 * setzen, Zone nachfuehren, Kartenschirm oeffnen, gezeigte Seite + Zustand jedes
 * Rechtecks + Spieler-Marker auslesen und die echte Op-Liste rastern.
 *
 * Aufruf: test_r35_karte <teil>
 *   messung   Mess-Ausgabe (kein Urteil) fuer alle vier Punkte
 *   fahrstuhl Riegel Punkt 1   b2  Riegel Punkt 2   r1230  Riegel Punkt 3
 *   r1210     Riegel Punkt 4
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_msg.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

static re15_rdt_t s_rdt;
static uint8_t *s_roh;

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    long sz;
    uint8_t *b;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

/* Dieselbe Kette wie test_map_raum_live.c betrete(). */
static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = 0; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid;
    g_room_change.pending = 0;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz;
    re15_collision_reset_band();
    if (band >= 0) re15_collision_set_band(band);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

typedef struct {
    int ok;              /* Raum geladen */
    int zone;            /* 1 = es gibt eine aktuelle Zone */
    int zpage, zrect, zid;
    int page;            /* gezeigte Seite */
    int cur[16];         /* Rechtecke der gezeigten Seite im Zustand CURRENT */
    int ncur;
    int16_t mx, my;      /* Spieler-Marker */
} mess_t;

/* Oeffnet die Karte im Raum rid an (px,pz) und misst. Die Position wird NUR ueber
 * re15_map_zone_update nachgefuehrt (wie game_step_common je Bild), nicht neu geladen. */
static void miss_hier(unsigned rid, int32_t px, int32_t pz, mess_t *m)
{
    const re15_map_zone_t *zn;
    int i, n;
    memset(m, 0, sizeof *m);
    m->ok = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = px;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
    re15_map_zone_update(rid, px, pz);
    zn = re15_map_zone_current();
    if (zn) { m->zone = 1; m->zpage = zn->page; m->zrect = zn->rect; m->zid = zn->zid; }
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    /* wie menu_common.c map_entry(): Seiten-Setzer je Stage (@0x80074c0c), dann die
     * gezeigte Seite */
    re15_inv_map_stage_init((int)((rid >> 12) & 0xfu) - 1, (int)((rid >> 4) & 0xffu));
    g_inv_screen.map_page = re15_inv_map_page_shown();
    m->page = g_inv_screen.map_page;
    n = re15_map_rect_count((unsigned)m->page);
    for (i = 0; i < n && m->ncur < 16; i++)
        if (re15_map_rect_state((unsigned)m->page, (unsigned)i) == RE15_MAP_RECT_CURRENT)
            m->cur[m->ncur++] = i;
    m->mx = m->my = -99;
    re15_inv_map_marker(px, pz, (uint8_t)((rid >> 4) & 0xFF), &m->mx, &m->my);
}

static void miss(unsigned rid, int32_t px, int32_t pz, int band, mess_t *m)
{
    memset(m, 0, sizeof *m);
    if (!betrete(rid, px, pz, band)) return;
    miss_hier(rid, px, pz, m);
}

static void druck(const char *was, unsigned rid, int32_t px, int32_t pz, const mess_t *m)
{
    int i;
    printf("  %-22s ROOM%04X (%6d,%6d): ", was, rid, (int)px, (int)pz);
    if (!m->ok) { printf("RDT fehlt\n"); return; }
    if (m->zone) printf("Zone Blatt %d rect %d zid %d | ", m->zpage, m->zrect, m->zid);
    else         printf("KEINE Zone | ");
    printf("gezeigt Blatt %d | aktuell:", m->page);
    for (i = 0; i < m->ncur; i++) printf(" %d", m->cur[i]);
    if (!m->ncur) printf(" -");
    printf(" | Marker (%d,%d)\n", m->mx, m->my);
}

/* ---- Punkt 1: Fahrstuhl -------------------------------------------------------- */
/* Kabinen-Innenraum aus der SCA von ROOM1080.RDT: Waende x -16750..-15750 / -11550..-10550,
 * z -5150..-4150 / -50..950 -> begehbar x -15750..-11550, z -4150..-50; Ankunft aller drei
 * Etagen-Tueren bei (-13650,-900) (ROOM1040 @0x1096, ROOM10C0 @0xE82, ROOM1120 @0xCB6). */
static const int32_t s_kabine[][2] = {
    { -13650,  -900 }, { -15500, -3900 }, { -11800, -3900 },
    { -15500,  -300 }, { -11800,  -300 }, { -13650, -2100 },
};

/* Etage der Kabine: Bank 3 Bit 54/55/56 (ROOM1040 @0x15D6 `22 03 36 01`,
 * ROOM10C0 @0x0FEE `22 03 37 01`, ROOM1120 @0x0D6C `22 03 38 01`). */
static void kabine_auf(int etage)
{
    re15_game_flag_set(3, 54, etage == 1);
    re15_game_flag_set(3, 55, etage == 2);
    re15_game_flag_set(3, 56, etage == 3);
}

static int teil_messung(void)
{
    mess_t m;
    int e, k;
    printf("=== Punkt 1: Fahrstuhl ROOM1080 ===\n");
    for (e = 1; e <= 3; e++) {
        char was[64];
        if (!betrete(0x1080, s_kabine[0][0], s_kabine[0][1], 0)) { printf("  RDT fehlt\n"); break; }
        kabine_auf(e);
        for (k = 0; k < (int)(sizeof s_kabine / sizeof s_kabine[0]); k++) {
            snprintf(was, sizeof was, "Kabine %dF Punkt %d", e, k);
            miss_hier(0x1080, s_kabine[k][0], s_kabine[k][1], &m);
            druck(was, 0x1080, s_kabine[k][0], s_kabine[k][1], &m);
        }
    }
    printf("=== Punkt 2: ROOM11F0 / ROOM1200 (Blatt 1 = B2) ===\n");
    miss(0x11F0, 250, 250, 0, &m);       druck("11F0 Ankunft", 0x11F0, 250, 250, &m);
    miss(0x11F0, 7000, -12000, 0, &m);   druck("11F0 Mitte", 0x11F0, 7000, -12000, &m);
    miss(0x1200, -20154, -25245, 0, &m); druck("1200 Ankunft", 0x1200, -20154, -25245, &m);
    miss(0x1200, -22000, -15000, 0, &m); druck("1200 Mitte", 0x1200, -22000, -15000, &m);
    miss(0x11E0, -24707, -9442, 0, &m);  druck("11E0 (von 11F0)", 0x11E0, -24707, -9442, &m);
    printf("=== Punkt 3: ROOM1230 / ROOM1180 ===\n");
    miss(0x1230, -8133, 10226, 0, &m);   druck("1230 (von 1190 s4)", 0x1230, -8133, 10226, &m);
    miss(0x1230, 4725, 28650, 0, &m);    druck("1230 (von 11B0)", 0x1230, 4725, 28650, &m);
    miss(0x1230, -4729, -16018, 0, &m);  druck("1230 (von 11D0)", 0x1230, -4729, -16018, &m);
    miss(0x1180, -8133, 10226, 0, &m);   druck("1180 (von 1190 s4)", 0x1180, -8133, 10226, &m);
    miss(0x1180, 4725, 28650, 0, &m);    druck("1180 (von 11B0)", 0x1180, 4725, 28650, &m);
    miss(0x1180, -4729, -16018, 0, &m);  druck("1180 (von 11D0)", 0x1180, -4729, -16018, &m);
    printf("=== Punkt 4: ROOM1210 / ROOM1220 ===\n");
    miss(0x1210, -26400, -2200, 0, &m);  druck("1210 (von 11E0)", 0x1210, -26400, -2200, &m);
    miss(0x1210, -20890, -6560, 0, &m);  druck("1210 (von 1220 s0)", 0x1210, -20890, -6560, &m);
    miss(0x1210, -18100, -25600, 0, &m); druck("1210 (von 1220 s4)", 0x1210, -18100, -25600, &m);
    miss(0x1220, -22400, -6500, 0, &m);  druck("1220 Zelle s1", 0x1220, -22400, -6500, &m);
    miss(0x1220, -16600, -9900, 0, &m);  druck("1220 Zelle s2", 0x1220, -16600, -9900, &m);
    miss(0x1220, -22400, -14000, 0, &m); druck("1220 Zelle s3", 0x1220, -22400, -14000, &m);
    miss(0x1220, -16600, -17900, 0, &m); druck("1220 Zelle s4", 0x1220, -16600, -17900, &m);
    miss(0x1220, -16600, -25050, 0, &m); druck("1220 Zelle s5", 0x1220, -16600, -25050, &m);
    {   /* Tuermarken auf Blatt 1 (sichtbar = re15_map_mark_get liefert 1) */
        int i, n = re15_map_mark_count();
        printf("  Marken Blatt 1 (alle Blaetter aufgedeckt):\n");
        re15_map_debug_reveal_page(1);
        for (i = 0; i < n; i++) {
            int pg, rc, mx, my, kind, za, zb;
            int sicht = re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
            if (pg != 1) continue;
            re15_map_mark_zonen(i, &za, &zb);
            printf("    #%d rect %d (%d,%d) kind %d zid %d/%d sichtbar %d\n",
                   i, rc, mx, my, kind, za, zb, sicht);
        }
    }
    return 0;
}

/* ---- Kachel der Seite: DATA/MAP0<p+1>.PIX (id-Tabelle @0x80074c4c, id 12.. -> MAP01..),
 * 256x256 4bpp, 128 B je Zeile, unteres Nibble = linkes Pixel (gen_map_zones.py page_pix). */
static uint8_t s_pix[256 * 256];
static int s_pix_seite = -1;
static int pix_laden(int seite)
{
    char pfad[600];
    size_t n = 0;
    uint8_t *roh;
    int y, x;
    if (s_pix_seite == seite) return 1;
    snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, seite + 1);
    roh = slurp(pfad, &n);
    if (!roh || n < 256 * 128) { free(roh); return 0; }
    for (y = 0; y < 256; y++)
        for (x = 0; x < 128; x++) {
            s_pix[y * 256 + 2 * x]     = roh[y * 128 + x] & 0xF;
            s_pix[y * 256 + 2 * x + 1] = roh[y * 128 + x] >> 4;
        }
    free(roh);
    s_pix_seite = seite;
    return 1;
}

/* Kachel-Index des Rechtecks `rect` am Bildpunkt (x,y); -1 = ausserhalb des Rechtecks. */
static int texel(int seite, int rect, int x, int y)
{
    int rx, ry, rw, rh, u, v;
    if (!pix_laden(seite)) return -1;
    if (!re15_map_rect_geometry((unsigned)seite, (unsigned)rect, &rx, &ry, &rw, &rh)) return -1;
    if (!re15_map_rect_uv((unsigned)seite, (unsigned)rect, &u, &v)) return -1;
    if (x < rx || x >= rx + rw || y < ry || y >= ry + rh) return -1;
    return s_pix[(v + y - ry) * 256 + (u + x - rx)];
}

/* Genau EIN aktuelles Rechteck, und zwar `soll`. */
static int nur_aktuell(const mess_t *m, int soll)
{
    return m->ncur == 1 && m->cur[0] == soll;
}

/* Marker steht auf der GEMALTEN Flaeche des Rechtecks (Index != 0). */
static int marker_auf_kunst(const mess_t *m, int rect)
{
    int t = texel(m->page, rect, m->mx, m->my);
    return t > 0;
}

/* ---- Riegel Punkt 1: Fahrstuhl ROOM1080 ----------------------------------------- */
static int teil_fahrstuhl(void)
{
    static const int blatt[4] = { 0, 2, 3, 4 }, rect[4] = { 0, 9, 4, 0 };
    /* gemalter Kabinen-Innenraum (Index 1) je Blatt, aus der Kachel uv(168,40) */
    static const int ix0[4] = { 0, 110, 110, 128 }, iy0[4] = { 0, 135, 135, 138 };
    mess_t m, ecke[4];
    int e, k;
    char t[200];
    printf("=== Riegel Punkt 1: Fahrstuhl ROOM1080 ===\n");
    /* (a) DER ECHTE WEG: aus dem Etagenraum durch die Tuer in die Kabine (betrete faehrt
     * scd_room_reenter -> re15_map_zone_update, wie der Raumlade-Punkt im Spiel). Die
     * Etagen-Bits stehen dabei absichtlich auf 1F - der Raum, aus dem man kommt, gewinnt. */
    {
        static const unsigned raum[4] = { 0, 0x1040, 0x10C0, 0x1120 };
        /* Ankunftspunkte an der Fahrstuhltuer der Etage (Gegenrichtung der Tuer-Datensaetze
         * ROOM1080 @0x482/0x4A2/0x4C2) */
        static const int32_t ank[4][2] = { {0,0}, {-21936,-11000}, {1450,7300}, {1300,7300} };
        for (e = 1; e <= 3; e++) {
            if (!betrete(raum[e], ank[e][0], ank[e][1], 0)) { CHECK("RDT der Etage", 0); continue; }
            kabine_auf(1);
            if (!betrete(0x1080, s_kabine[0][0], s_kabine[0][1], 0)) { CHECK("ROOM1080", 0); continue; }
            miss_hier(0x1080, s_kabine[0][0], s_kabine[0][1], &m);
            snprintf(t, sizeof t, "aus ROOM%04X in die Kabine: Blatt %d (soll %d), aktuell nur "
                     "rect %d", raum[e], m.page, blatt[e], rect[e]);
            CHECK(t, m.page == blatt[e] && nur_aktuell(&m, rect[e]));
        }
    }
    for (e = 1; e <= 3; e++) {
        /* (b) Etagen-Bits des Spiels, wenn der Vorraum KEIN Etagenraum ist */
        if (!betrete(0x1070, 0, 0, 0)) { CHECK("ROOM1070", 0); break; }
        if (!betrete(0x1080, s_kabine[0][0], s_kabine[0][1], 0)) { CHECK("ROOM1080", 0); break; }
        kabine_auf(e);
        miss_hier(0x1080, s_kabine[0][0], s_kabine[0][1], &m);
        snprintf(t, sizeof t, "Kabine %dF: gezeigt Blatt %d (soll %d), aktuell nur rect %d",
                 e, m.page, blatt[e], rect[e]);
        CHECK(t, m.page == blatt[e] && nur_aktuell(&m, rect[e]));
        /* vier Ecken des Innenraums: SW, SO, NW, NO (Welt) */
        miss_hier(0x1080, -15500, -3900, &ecke[0]);
        miss_hier(0x1080, -11800, -3900, &ecke[1]);
        miss_hier(0x1080, -15500,  -300, &ecke[2]);
        miss_hier(0x1080, -11800,  -300, &ecke[3]);
        for (k = 0; k < 4; k++) {
            int drin = ecke[k].mx >= ix0[e] && ecke[k].mx <= ix0[e] + 7 &&
                       ecke[k].my >= iy0[e] && ecke[k].my <= iy0[e] + 7;
            snprintf(t, sizeof t, "Kabine %dF Ecke %d: Marker (%d,%d) im gemalten Innenraum "
                     "x%d..%d y%d..%d", e, k, ecke[k].mx, ecke[k].my, ix0[e], ix0[e] + 7,
                     iy0[e], iy0[e] + 7);
            CHECK(t, drin && texel(m.page, rect[e], ecke[k].mx, ecke[k].my) == 1);
        }
        /* 180 Grad (G_karte.md B5): Welt-Ost -> Karte-West, Welt-Nord -> Karte-Sued */
        snprintf(t, sizeof t, "Kabine %dF: Marker folgt dem Spieler, 180 Grad gedreht "
                 "(Ost x %d < West x %d, Nord y %d > Sued y %d)", e, ecke[1].mx, ecke[0].mx,
                 ecke[2].my, ecke[0].my);
        CHECK(t, ecke[1].mx < ecke[0].mx && ecke[3].mx < ecke[2].mx &&
                 ecke[2].my > ecke[0].my && ecke[3].my > ecke[1].my);
    }
    /* Gegenprobe: kein Etagen-Bit, kein Etagen-Vorraum -> wie bisher die erste Zeile (Blatt 2) */
    if (betrete(0x1070, 0, 0, 0) && betrete(0x1080, s_kabine[0][0], s_kabine[0][1], 0)) {
        kabine_auf(0);
        miss_hier(0x1080, s_kabine[0][0], s_kabine[0][1], &m);
        CHECK("ohne Etagen-Bit: Blatt 2 (unveraenderter Rueckfall)", m.page == 2);
    }
    printf(g_fail ? "FEHLER\n" : "OK\n");
    return g_fail;
}

/* ---- Riegel Punkt 2: 11F0 und 1200 auf ihren eigenen Kacheln ---------------------- */
static int teil_b2(void)
{
    static const struct { unsigned rid; int32_t x, z; int rect; const char *was; } P[] = {
        { 0x11F0,    250,    250, 1, "11F0 Ankunft aus 11E0" },
        { 0x11F0,   7000, -12000, 1, "11F0 Mitte" },
        { 0x1200, -20154, -25245, 2, "1200 Ankunft aus 11E0" },
        { 0x1200, -22000, -15000, 2, "1200 Mitte" },
        { 0x11E0, -24707,  -9442, 0, "11E0 Gegenprobe" },
    };
    mess_t m;
    int i;
    char t[200];
    printf("=== Riegel Punkt 2: ROOM11F0 / ROOM1200 ===\n");
    for (i = 0; i < (int)(sizeof P / sizeof P[0]); i++) {
        miss(P[i].rid, P[i].x, P[i].z, 0, &m);
        snprintf(t, sizeof t, "%s: Blatt 1, aktuell NUR rect %d (ist: Blatt %d, %d Rect(s), "
                 "erstes %d)", P[i].was, P[i].rect, m.page, m.ncur, m.ncur ? m.cur[0] : -1);
        CHECK(t, m.page == 1 && nur_aktuell(&m, P[i].rect));
        snprintf(t, sizeof t, "%s: Marker (%d,%d) auf der gemalten Flaeche von rect %d",
                 P[i].was, m.mx, m.my, P[i].rect);
        CHECK(t, marker_auf_kunst(&m, P[i].rect));
    }
    /* Nach dem Besuch aller drei: jede eigene Kachel ist gezeichnet (nicht UNMAPPED) */
    CHECK("rect 1 (11F0) und rect 2 (1200) sind nach dem Besuch BESUCHT/AKTUELL",
          re15_map_rect_state(1, 1) >= RE15_MAP_RECT_VISITED &&
          re15_map_rect_state(1, 2) >= RE15_MAP_RECT_VISITED);
    printf(g_fail ? "FEHLER\n" : "OK\n");
    return g_fail;
}

/* ---- Riegel Punkt 3: 1230 zeigt B1 (Blatt 0) und den Gang ---------------------------- */
static int teil_r1230(void)
{
    static const struct { int32_t x, z; const char *was; } P[] = {
        {  -8133,  10226, "Ankunft aus 1190 (Hauptraum)" },
        {    142,  13001, "Ankunft aus 1190 (Ostkammer)" },
        {   4725,  28650, "Ankunft aus 11B0" },
        {  -4729, -16018, "Ankunft aus 11D0/1160" },
        {   3800,   5000, "Laengsgang Mitte" },
        {   1000,  -5800, "Versatz bei der 10A0-Tuer" },
    };
    static const unsigned R[2] = { 0x1230, 0x1180 };
    mess_t m;
    int i, r;
    char t[220];
    printf("=== Riegel Punkt 3: ROOM1230 (= ROOM1180) ===\n");
    for (r = 0; r < 2; r++)
        for (i = 0; i < (int)(sizeof P / sizeof P[0]); i++) {
            miss(R[r], P[i].x, P[i].z, 0, &m);
            snprintf(t, sizeof t, "ROOM%04X %s: Blatt 0 (B1), aktuell NUR rect 0 (ist Blatt %d, "
                     "%d Rect(s))", R[r], P[i].was, m.page, m.ncur);
            CHECK(t, m.zone && m.page == 0 && nur_aktuell(&m, 0));
            snprintf(t, sizeof t, "ROOM%04X %s: Marker (%d,%d) auf dem gemalten Gang (rect 0)",
                     R[r], P[i].was, m.mx, m.my);
            CHECK(t, marker_auf_kunst(&m, 0));
        }
    /* Gegenprobe: das Blatt von 11E0 (B2) ist es NICHT mehr */
    miss(0x1230, -8133, 10226, 0, &m);
    CHECK("ROOM1230 zeigt NICHT mehr Blatt 1 (B2 = Blatt von 11E0)", m.page != 1);
    /* 10A0 auf B1 (Band 4) = rect 6 */
    miss(0x10A0, 21200, 25500, 4, &m);
    snprintf(t, sizeof t, "ROOM10A0 Band 4 (Ankunft aus 1180/1230): Blatt 0 rect 6 (ist Blatt %d)",
             m.page);
    CHECK(t, m.page == 0 && nur_aktuell(&m, 6));
    printf(g_fail ? "FEHLER\n" : "OK\n");
    return g_fail;
}

/* ---- Riegel Punkt 4: 1210 Korridor + alle Tueren, 1220 Zellen ---------------------- */
static int teil_r1210(void)
{
    static const struct { int32_t x, z; const char *was; } K[] = {
        { -26400,  -2200, "Ankunft aus 11E0" },
        { -20890,  -6560, "Ankunft aus Zelle West 1" },
        { -18100, -25600, "Ankunft aus Zelle Ost 3" },
    };
    /* Spawns aus 1210 in die Zellen (1210.RDT @0x1CE6/0x1D06/0x1D26/0x1D46/0x1D66) */
    static const struct { int32_t x, z; int rect; } Z[] = {
        { -22400,  -6500, 4 }, { -16600,  -9900, 6 }, { -22400, -14000, 5 },
        { -16600, -17900, 7 }, { -16600, -25050, 8 },
    };
    mess_t m;
    int i, k;
    char t[220];
    printf("=== Riegel Punkt 4: ROOM1210 / ROOM1220 ===\n");
    for (i = 0; i < (int)(sizeof K / sizeof K[0]); i++) {
        miss(0x1210, K[i].x, K[i].z, 0, &m);
        snprintf(t, sizeof t, "1210 %s: aktuell NUR rect 3 = T-Korridor (ist %d Rect(s), erstes %d)",
                 K[i].was, m.ncur, m.ncur ? m.cur[0] : -1);
        CHECK(t, m.page == 1 && nur_aktuell(&m, 3));
        snprintf(t, sizeof t, "1210 %s: Marker (%d,%d) auf dem gemalten Korridor", K[i].was,
                 m.mx, m.my);
        CHECK(t, marker_auf_kunst(&m, 3));
    }
    for (i = 0; i < 5; i++) {
        miss(0x1220, Z[i].x, Z[i].z, 0, &m);
        snprintf(t, sizeof t, "1220 Zelle bei (%d,%d): aktuell NUR rect %d (ist %d Rect(s), "
                 "erstes %d), Marker (%d,%d) in der Zelle", Z[i].x, Z[i].z, Z[i].rect, m.ncur,
                 m.ncur ? m.cur[0] : -1, m.mx, m.my);
        CHECK(t, m.page == 1 && nur_aktuell(&m, Z[i].rect) && marker_auf_kunst(&m, Z[i].rect));
    }
    /* TUEREN: jede Tuer-AOT von 1210 (aus dem geladenen Raum, g_aot) hat eine SICHTBARE Marke,
     * die auf einer GEMALTEN WAND (Index 4) liegt und hoechstens 3 px neben der Projektion
     * der Tuer mit der 1210-Zeile @0x800769b8 (177,140,2496,2250). */
    if (betrete(0x1210, -26400, -2200, 0)) {
        int tueren = 0, getroffen = 0, n = re15_map_mark_count();
        re15_map_visited_reset();
        re15_map_zone_update(0x1210, -26400, -2200);     /* nur 1210 besucht */
        for (k = 0; k < RE15_AOT_MAX; k++) {
            const re15_aot_t *a = &g_aot.slots[k];
            int32_t tx, tz;
            int px, py, j, gut = 0;
            if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
            tueren++;
            tx = ((a->x + 32000) * 10 * 2496) >> 20;
            tz = ((a->z + 32000) * 10 * 2250) >> 20;
            px = (tx + 5) / 10 + 177;
            py = -((tz + 5) / 10) + 140;
            for (j = 0; j < n && !gut; j++) {
                int pg, rc, mx, my, kind, w, c;
                if (!re15_map_mark_get(j, &pg, &rc, &mx, &my, &kind)) continue;
                if (pg != 1 || kind >= 4) continue;
                if (abs(mx - px) + abs(my - py) > 3) continue;
                /* Wand der Kachel: rect 3 oder ein Zellen-/Garagen-Rechteck daneben */
                w = 0;
                for (c = 0; c <= 8 && !w; c++) w = (texel(1, c, mx, my) == 4);
                gut = w;
            }
            snprintf(t, sizeof t, "1210 Tuer-AOT %d bei Karte (%d,%d): sichtbare Marke auf "
                     "gemalter Wand <= 3 px", k, px, py);
            CHECK(t, gut);
            getroffen += gut;
        }
        snprintf(t, sizeof t, "1210 fuehrt 6 Tueren (11E0 + 5 Zellen), alle mit Marke: %d/%d",
                 getroffen, tueren);
        CHECK(t, tueren == 6 && getroffen == 6);
    }
    printf(g_fail ? "FEHLER\n" : "OK\n");
    return g_fail;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "messung";
    re15_map_visited_reset();
    re15_inv_map_stage_init(0, 6);
    if (!strcmp(teil, "messung")) return teil_messung();
    if (!strcmp(teil, "fahrstuhl")) return teil_fahrstuhl();
    if (!strcmp(teil, "b2")) return teil_b2();
    if (!strcmp(teil, "r1230")) return teil_r1230();
    if (!strcmp(teil, "r1210")) return teil_r1210();
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
