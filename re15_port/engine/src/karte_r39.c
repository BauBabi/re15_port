/* =============================================================================
 * Runde 39 Spur G "karten" - Korrekturen der Kartenblaetter 1F/2F/3F/B2
 * =============================================================================
 * NUTZER-VORGABE (Wortlaut AUFTRAG.md, Runde 39): "Keine Doppelwaende, Tuer immer genau auf
 * Wand platziert, treppen nicht in der Wand platziert usw." - Form = die rechten Haelften von
 * map1F/2F/3F/B2_correct.png, auf 320x240 zurueckgerechnet (Dossier
 * analysis/befunde_runde39/G_karten.md, Abschnitte M1/M2/R1; Bilder G_karten_bilder/).
 *
 * Die Tuer- und Treppenmarken sitzen in s_map_marks (re15_map_zones.h, "Runde 39 Spur G").
 * Hier stehen nur die Kachel-Korrekturen: welche Texel eines gemalten Rechtecks beim
 * Zeichnen ersetzt werden. Jede Zeile nennt das Rechteck mit seinem Eintrag in der
 * Seiten-Tabelle (Seite p: count/Zeiger @0x80076840 + 8*p, 12 B je Eintrag
 * {s16 x,y,w,h; u8 u,_,v,_}; Blatt 3 = Ersatztabelle s_map_rectfix) und die Kachel-
 * Spalte/-Zeile (Texel = DATA/MAP0<p+1>.PIX, Zeile v*128 + u/2, 4 bpp).
 *
 * GEMESSEN statt geschaetzt: der Klassen-Kompositor (G_karten_werkzeug/edits.py) rastert
 * die echte Op-Liste mit dieser Tabelle und trifft die Soll-Bilder pixelgenau (1F/3F/B2
 * 0 Abweichungen, 2F nur der Spielermarker der Vorlage); robust.py prueft zusaetzlich jedes
 * Rechteck als AKTUELLEN Raum (oben gezeichnet) - keine Wandlinie der Vorlage geht dabei
 * verloren (G_karten.md R1).
 * ========================================================================== */
#include <string.h>
#include <stdint.h>
#include "re15_karte_r39.h"
#include "re15_inv_screen.h"   /* re15_inv_op_t, RE15_INV_*, RE15_KARTE_* */
#include "re15_room.h"         /* re15_map_rect_geometry / re15_map_rect_uv */
#include "re15_map_owned.h"    /* re15_map_owned_page (Traeger-Gatter, Nachbesserung 1) */
#include <stddef.h>

/* neu: 0 = durchsichtig, 1 = Raumkoerper (CLUT-Eintrag 1), 4 = Wandlinie (Eintrag 4)
 *
 * traeger (Nachbesserung 1, Abnahme-Mangel M3): das Rechteck DERSELBEN Seite, dessen gemalte Wandlinie die
 * geloeschte ersetzt (Nachbar-Spalte/-Zeile, Kachel-Index 4 in DATA/MAP0<p+1>.PIX - Werkzeug
 * G_karten_werkzeug/nachbar.py, Dossier G_karten.md "Nachbesserung 1"). Die Korrektur gilt NUR, wenn dieser
 * Traeger im selben Kartenschirm gezeichnet wird - sonst bliebe der Raum offen: gemessen an der echten exe, je
 * Ort allein besucht, verlor z.B. ROOM1070 seine ganze Westwand x196 (34 px), ROOM1010 beide Seitenwaende
 * (G_abnahme_0.md Abschnitt 6). Die Vorlagen zeigen den Zustand "alle Raeume gezeichnet"; ist der Nachbar nicht
 * gezeichnet, gibt es keine Doppelwand, und die eigene Linie bleibt stehen (= Stand vor Runde 39).
 * R39_IMMER: kein Wandverlust moeglich (Loch fuellen; Linie innerhalb DESSELBEN Rechtecks verschoben; Tuerbalken
 * auf der Ersatzlinie, der mit dem Raum selbst gezeichnet wird). */
#define R39_IMMER 255
typedef struct { unsigned char page, rect, c, r, w, h, neu, traeger; } r39_korr_t;

static const r39_korr_t s_korr[] = {
    /* ---- 1F (Blatt 2, MAP03.PIX, Liste @0x8007636C) ---------------------------------- */
    /* r0 (180,69,32,96) uv(0,32) @0x8007636C: Spalte 7 = x187, y145..154 lag Wand an Wand mit
     * der Ostwand von r4 (x186) -> Koerper; x186 bleibt. Traeger r4 (ROOM1030) x186, 10/10 Zeilen. */
    { 2,  0,  7, 76, 1, 10, 1,  4 },
    /* r1 (109,77,56,56) uv(32,32) @0x80076378: Spalte 35 = x144, y109..124 neben der
     * Westwand von r3 (x145) -> Koerper. Traeger r3 (ROOM1070) x145, 16/16. */
    { 2,  1, 35, 32, 1, 16, 1,  3 },
    /* r2 (102,77,24,80) uv(88,48) @0x80076384: Spalte 16 = x118 neben der Westwand von r4/r10
     * (x119): y126..133 und y144..154 -> Koerper (y134..143 deckt r9, Zeile y143 bleibt Wand).
     * Traeger y126..133 r4 (ROOM1030) 8/8, y144..154 r10 (ROOM1060) 11/11. */
    { 2,  2, 16, 49, 1,  8, 1,  4 },
    { 2,  2, 16, 67, 1, 11, 1, 10 },
    /* r3 (145,109,56,40) uv(192,16) @0x80076390: Spalte 51 = x196, y110..143 neben der
     * Westwand des Flurs r0 (x197) -> Koerper. Traeger r0 (ROOM1050) x197, 34/34. */
    { 2,  3, 51,  1, 1, 34, 1,  0 },
    /* r4 (119,125,72,64) uv(184,56) @0x8007639C: Spalte 66 Zeile 20 = (185,145), nach der
     * Schwenk-Tilgung des Renderers durchsichtig (Loch im Boden) -> Koerper. Fuellt ein Loch: immer. */
    { 2,  4, 66, 20, 1,  1, 1, R39_IMMER },
    /* r7 (164,93,16,24) uv(152,16) @0x800763C0 und r8 (164,77,24,24) uv(128,40) @0x800763CC:
     * beide Seitenwaende (Spalten 0/15 = x164/x179, Zeilen 1..15) lagen neben der Ostwand von
     * r1 (x163) bzw. der Westwand von r0/r5/r6 (x180) -> Koerper. Traeger: x163 r1 (ROOM1020) 15/15;
     * x180 bei r7 (y94..108) r6 (ROOM10A0) 15/15, bei r8 y78..87 r0 (ROOM1050), y88..92 r6 (ROOM10A0). */
    { 2,  7,  0,  1, 1, 15, 1,  1 },
    { 2,  7, 15,  1, 1, 15, 1,  6 },
    { 2,  8,  0,  1, 1, 15, 1,  1 },
    { 2,  8, 15,  1, 1, 10, 1,  0 },
    { 2,  8, 15, 11, 1,  5, 1,  6 },
    /* r9 (109,134,16,16) uv(168,40) @0x800763D8: Spalte 9 = x118, y135..142 neben der
     * Westwand von r10 (x119) -> Koerper. Traeger r10 (ROOM1060) x119, 8/8. */
    { 2,  9,  9,  1, 1,  8, 1, 10 },
    /* ---- 2F (Blatt 3, MAP04.PIX, Ersatztabelle s_map_rectfix[5]) ---------------------- */
    /* r5 (146,114,72,32) uv(184,48): (186,121) - der Koerper des Evidence-Raums lag ueber der
     * Wand x186 der kleinen Raeume (Mischfarbe) -> durchsichtig, die Wand darunter steht.
     * Traeger r6 (ROOM1100) - seine Westwand x186 y114..122 (Index 4) liegt unter genau diesem Texel. */
    { 3,  5, 40,  7, 1,  1, 0,  6 },
    /* ---- 3F (Blatt 4, MAP05.PIX, Liste @0x80076468) ---------------------------------- */
    /* r4 (144,80,32,80) uv(0,32) @0x80076498 (Flur): seine Westwand lag eine Spalte NEBEN der
     * Wand der Nachbarn und schien durch deren Koerper (x144 unter r2, x152 unter r1/r5).
     * Soll: eine Linie auf x145 (y81..118) bzw. x153 (y119..157). Die alte Spalte wird
     * durchsichtig, die Nachbarspalte Wand - so steht die Linie auch dann, wenn der Flur der
     * aktuelle Raum ist und oben gezeichnet wird (robust.py). Die Linie bleibt in r4 selbst: immer. */
    { 4,  4,  0,  1, 1, 38, 0, R39_IMMER },
    { 4,  4,  1,  1, 1, 38, 4, R39_IMMER },
    { 4,  4,  8, 40, 1, 38, 0, R39_IMMER },
    { 4,  4,  9, 39, 1, 39, 4, R39_IMMER },
    /* ---- B2 (Blatt 1, MAP02.PIX, Liste @0x800762F4) ---------------------------------- */
    /* r3 (187,70,64,80) uv(96,48) @0x80076318, r4 (187,78,16,24) uv(72,32) @0x80076324,
     * r5 (187,95,16,24) uv(72,32) @0x80076330: Westwand x187 neben der Ostwand des Flurs r0
     * (x186) -> Koerper (r3 nur y72..76, wo vorher die Tuermarke stand).
     * r3 y72..76: auf x186 steht die Tuer 11E0<->1210 (s_map_marks), die mit ROOM1210 gezeichnet wird - immer.
     * Traeger r4: r0 (ROOM11E0) x186 16/16; r5 y96..100 r0 (ROOM11E0), y101..111 r9 (ROOM10A0 B2). */
    { 1,  3,  0,  2, 1,  5, 1, R39_IMMER },
    { 1,  4,  0,  1, 1, 16, 1,  0 },
    { 1,  5,  0,  1, 1,  5, 1,  0 },
    { 1,  5,  0,  6, 1, 11, 1,  9 },
};
#define R39_KORR_N ((int)(sizeof s_korr / sizeof s_korr[0]))

/* Schema-Zellen (s_map_synth_cells, ROOM1000 auf Blatt 2) - Seiten, die laut Vorlage
 * entfallen: Westkanten x207 (Flur-Ostwand x206 steht daneben), Oberkante des Ostraums y89
 * (Suedwand von r5 y88), Unterkante des Nord-WC y138 (Oberkante des Sued-WC y139).
 * traeger wie bei s_korr (Nachbesserung 1, M3): x206 = r0 (ROOM1050) Index 4 auf x206 y89..153 (65/65),
 * y88 = r5 (ROOM1090 Hof) Index 4 auf x207..221 (15/15), y139 = die Schema-Zeichnung des Sued-WC (synth 3,
 * s_map_synth[2]). Gemessen ohne Traeger (je Ort allein besucht): Ostraum 41, Nord-WC 26, Sued-WC 12 Wandpixel weg. */
#define R39_SCHEMA(s) (128 + (s))   /* Traeger = Schema-Zeichnung <s> (zone.synth) derselben Seite */
typedef struct { unsigned char page; short cx, cy, cw, ch; unsigned char seite, traeger; } r39_kante_t;
static const r39_kante_t s_kanten[] = {
    { 2, 207,  89, 15, 33, 0, 5 }, { 2, 207,  89, 15, 33, 2, 0 },
    { 2, 207, 123, 15, 16, 2, 0 }, { 2, 207, 123, 15, 16, 1, R39_SCHEMA(3) },
    { 2, 207, 139, 15, 15, 2, 0 },
};

/* ---- Wird der Traeger gezeichnet? (Nachbesserung 1, M3) -------------------------------------------------------
 * DIESELBEN Gatter wie die Zeichner in re15_inv_screen.c build_status - sonst liefe die Korrektur dem Bild davon:
 * Kachel-Schleife: Hinweis-Kachel und Zielkachel immer (RE2 FUN_8006F1C4 @0x8006F4E8-0x8006F608), sonst Zustand
 * CURRENT/VISITED, UNVISITED nur mit dem Plan des Blattes (RE2 @0x8006E744 / @0x8006E66C), UNMAPPED nur im
 * Stock-Modus; Teilbereiche je Teil. Schema-Durchgang: Besucht-/Etagen-Bit, aktuell, Hinweisziel, Plan (Umriss). */
int re15_map_ebene_kachel(void);   /* re15_inv_screen.c, Messschiene RE15_MAP_EBENE */
int re15_map_ebene_schema(void);

static int kachel_gezeichnet(const re15_inv_screen_t *st, unsigned page, int i)
{
    int rs, n, k;
    if (st->hint_aktiv && st->map_page == st->hint_page && i == st->hint_rect) return 1;
    if (!st->hint_aktiv &&
        ((st->ziel_aktiv && st->map_page == st->ziel_page && i == st->ziel_rect) ||
         (st->ziel2_aktiv && st->map_page == st->ziel2_page && i == st->ziel2_rect))) return 1;
    rs = re15_map_stock_mode() ? RE15_MAP_RECT_UNMAPPED : re15_map_rect_state(page, (unsigned)i);
    if (rs == RE15_MAP_RECT_UNVISITED && !re15_map_owned_page(page)) return 0;
    if (rs == RE15_MAP_RECT_UNMAPPED && !re15_map_stock_mode()) return 0;
    if (!re15_map_ebene_kachel()) return 0;
    n = re15_map_teil_count(page, (unsigned)i);
    if (n <= 0) return 1;
    for (k = 0; k < n; k++) {
        int ts;
        if (!re15_map_teil_get(page, (unsigned)i, k, NULL, NULL, NULL, NULL, &ts)) continue;
        if (ts != RE15_MAP_RECT_UNVISITED || re15_map_owned_page(page)) return 1;
    }
    return 0;
}

static int schema_gezeichnet(const re15_inv_screen_t *st, unsigned page, int synth)
{
    const re15_map_zone_t *cur = re15_map_zone_current();
    int j, nz = re15_map_zone_count();
    if (re15_map_stock_mode() || !re15_map_ebene_schema()) return 0;
    if (re15_karte_r39_schema_ziel(st, page, synth)) return 1;
    if (re15_map_owned_page(page)) return 1;   /* Umriss der unbesuchten Zeichnung (Runde 39) */
    for (j = 0; j < nz; j++) {
        const re15_map_zone_t *zg = re15_map_zone_by_index(j);
        if (!zg || zg->page != page || zg->synth != synth) continue;
        if (zg->etage ? re15_map_zone_etage_besucht(zg) : re15_map_zone_visited(zg)) return 1;
        if (cur && cur->room == zg->room && cur->idx == zg->idx &&
            (!zg->etage || re15_map_zone_etage_besucht(zg)) && re15_karte_r39_etage_hier(zg, page)) return 1;
    }
    return 0;
}

/* Ohne Schirmzustand (st == NULL, Werkzeug-Aufruf) gilt jede Korrektur - der Zustand der Vorlagen. */
static int traeger_gezeichnet(const re15_inv_screen_t *st, unsigned page, unsigned char t)
{
    if (t == R39_IMMER || !st) return 1;
    if (t >= 128) return schema_gezeichnet(st, page, t - 128);
    return kachel_gezeichnet(st, page, t);
}

int re15_karte_r39_kante_weg(const void *inv_screen, unsigned page, int cx, int cy, int cw, int ch, int seite)
{
    unsigned i;
    for (i = 0; i < sizeof s_kanten / sizeof s_kanten[0]; i++) {
        const r39_kante_t *k = &s_kanten[i];
        if (k->page == page && k->cx == cx && k->cy == cy && k->cw == cw && k->ch == ch &&
            k->seite == seite)
            return traeger_gezeichnet((const re15_inv_screen_t *)inv_screen, page, k->traeger);
    }
    return 0;
}

int re15_karte_r39_korrektur_zahl(void) { return R39_KORR_N; }

int re15_karte_r39_korrektur(int i, int *page, int *rect, int *c, int *r, int *w, int *h, int *neu, int *traeger)
{
    if (i < 0 || i >= R39_KORR_N) return 0;
    if (traeger) *traeger = s_korr[i].traeger;
    if (page) *page = s_korr[i].page;
    if (rect) *rect = s_korr[i].rect;
    if (c) *c = s_korr[i].c;
    if (r) *r = s_korr[i].r;
    if (w) *w = s_korr[i].w;
    if (h) *h = s_korr[i].h;
    if (neu) *neu = s_korr[i].neu;
    return 1;
}

#define R39_MAX 128   /* groesste Kachel der Blaetter 1..4: 88x96 */

int re15_karte_r39_kachel_stuecke(const void *inv_screen, unsigned page, unsigned rect,
                                  int rx, int ry, int rw, int rh, int ru, int rv,
                                  int fx, int fy, int fw, int fh,
                                  re15_karte_r39_stueck_t *out, int max)
{
    const re15_inv_screen_t *sst = (const re15_inv_screen_t *)inv_screen;
    static signed char m[R39_MAX][R39_MAX];
    static unsigned char benutzt[R39_MAX][R39_MAX];
    static const signed char codes[3] = { -1, 1, 4 };
    int i, x, y, lx0, ly0, lx1, ly1, n = 0, ci, hat = 0;
    for (i = 0; i < R39_KORR_N; i++)
        if (s_korr[i].page == page && s_korr[i].rect == rect &&
            traeger_gezeichnet(sst, page, s_korr[i].traeger)) { hat = 1; break; }
    if (!hat || rw <= 0 || rh <= 0 || rw > R39_MAX || rh > R39_MAX) return -1;
    for (y = 0; y < rh; y++)
        for (x = 0; x < rw; x++) { m[y][x] = -1; benutzt[y][x] = 0; }
    for (i = 0; i < R39_KORR_N; i++) {
        const r39_korr_t *k = &s_korr[i];
        if (k->page != page || k->rect != rect) continue;
        if (!traeger_gezeichnet(sst, page, k->traeger)) continue;   /* Nachbesserung 1, M3 */
        for (y = k->r; y < k->r + k->h && y < rh; y++)
            for (x = k->c; x < k->c + k->w && x < rw; x++) m[y][x] = (signed char)k->neu;
    }
    /* Fenster (Teilbereiche zeichnen nur einen Ausschnitt derselben Kachel) */
    lx0 = fx - rx; ly0 = fy - ry; lx1 = fx + fw - rx; ly1 = fy + fh - ry;
    if (lx0 < 0) lx0 = 0;
    if (ly0 < 0) ly0 = 0;
    if (lx1 > rw) lx1 = rw;
    if (ly1 > rh) ly1 = rh;
    for (ci = 0; ci < 3; ci++) {
        signed char c = codes[ci];
        for (y = ly0; y < ly1; y++)
            for (x = lx0; x < lx1; x++) {
                int x2, y2, k, voll;
                if (m[y][x] != c || benutzt[y][x]) continue;
                for (x2 = x; x2 < lx1 && m[y][x2] == c && !benutzt[y][x2]; x2++) ;
                for (y2 = y + 1; y2 < ly1; y2++) {
                    for (voll = 1, k = x; k < x2; k++)
                        if (m[y2][k] != c || benutzt[y2][k]) { voll = 0; break; }
                    if (!voll) break;
                }
                for (k = y; k < y2; k++) memset(&benutzt[k][x], 1, (size_t)(x2 - x));
                if (n < max) {
                    out[n].x = (short)(rx + x); out[n].y = (short)(ry + y);
                    out[n].w = (short)(x2 - x); out[n].h = (short)(y2 - y);
                    out[n].u = (unsigned char)(ru + x); out[n].v = (unsigned char)(rv + y);
                    out[n].art = (unsigned char)(c < 0 ? 0 : c);
                    n++;
                }
            }
    }
    return n;
}

int re15_karte_r39_kachel(const void *inv_screen, void *ops_v, int *n, int max, unsigned page, int rect,
                          int clut, int x, int y, int w, int h, int u, int v, int r, int g, int b)
{
    re15_inv_op_t *ops = (re15_inv_op_t *)ops_v;
    re15_karte_r39_stueck_t st[48];
    int rx, ry, rw, rh, ru, rv, ns, k;
    (void)u; (void)v;   /* die Stuecke tragen ihre Kachelecke selbst (Rechteck-Ursprung + Lage) */
    if (!re15_map_rect_geometry(page, (unsigned)rect, &rx, &ry, &rw, &rh) ||
        !re15_map_rect_uv(page, (unsigned)rect, &ru, &rv)) return 0;
    ns = re15_karte_r39_kachel_stuecke(inv_screen, page, (unsigned)rect, rx, ry, rw, rh, ru, rv,
                                       x, y, w, h, st, 48);
    if (ns < 0) return 0;
    for (k = 0; k < ns && *n < max; k++) {
        re15_inv_op_t *q = &ops[*n];
        /* Farbwort: Wandlinie = RE15_KARTE_WAND (TEX.TIM @0x055C, deckend); Koerper = Eintrag 1
         * der gewaehlten Zustandszeile (inv_render_pc.c karten_cluts_bauen): aktuell 0x842D,
         * unbesucht 0x0000 (= durchsichtig, nichts zeichnen), sonst 0x81A4 (TEX.TIM @0x0556). */
        uint16_t wort = (st[k].art == 4) ? RE15_KARTE_WAND
                      : (clut == RE15_INV_CLUT_MAP_AKTUELL) ? RE15_KARTE_AKTUELL
                      : (clut == RE15_INV_CLUT_MAP_UNBESUCHT) ? RE15_KARTE_UNBESUCHT
                      : RE15_KARTE_BESUCHT;
        memset(q, 0, sizeof *q);
        if (st[k].art == 0) {
            q->kind = RE15_INV_OP_SPRT; q->page = RE15_INV_PAGE_MAP4; q->clut = (uint8_t)clut;
            q->abe = 1; q->u = st[k].u; q->v = st[k].v;
            q->r = (uint8_t)r; q->g = (uint8_t)g; q->b = (uint8_t)b;
        } else {
            if (wort == 0) continue;
            /* STP-Koerper mischen wie die Kachel-Texel (abe = 1), die Wand deckt */
            q->kind = RE15_INV_OP_FILL; q->abe = (uint8_t)(st[k].art == 1);
            q->r = (uint8_t)((wort & 31u) << 3);
            q->g = (uint8_t)(((wort >> 5) & 31u) << 3);
            q->b = (uint8_t)(((wort >> 10) & 31u) << 3);
        }
        q->x = st[k].x; q->y = st[k].y; q->w = st[k].w; q->h = st[k].h;
        (*n)++;
    }
    return 1;
}

/* ---- Kartenhinweis-Ziel auf einer Schema-Zeichnung (B1, Runde 39 Spur G) ---------------------------------- */
int re15_karte_r39_ziel_rect(int rect, int synth)
{
    return (rect == 255 && synth > 0) ? RE15_KARTE_R39_SCHEMA_RECT + synth : rect;
}

int re15_karte_r39_schema_ziel(const void *inv_screen, unsigned page, int synth)
{
    const re15_inv_screen_t *st = (const re15_inv_screen_t *)inv_screen;
    const int r = RE15_KARTE_R39_SCHEMA_RECT + synth;
    if (!st || synth <= 0) return 0;
    /* dieselben drei Quellen und dieselbe Vorrangregel wie die Kachel-Schleife (Hinweis vor Ziel) */
    if (st->hint_aktiv)
        return (st->hint_page == page && st->hint_rect == r) ? (st->hint_rot ? 1 : 2) : 0;
    if ((st->ziel_aktiv && st->ziel_page == page && st->ziel_rect == r) ||
        (st->ziel2_aktiv && st->ziel2_page == page && st->ziel2_rect == r))
        return st->ziel_rot ? 1 : 2;
    return 0;
}

int re15_karte_r39_etage_hier(const void *zone, unsigned page)
{
    const re15_map_zone_t *zn = (const re15_map_zone_t *)zone;
    int fp, fr;
    if (!zn || !zn->etage) return 1;
    if (!re15_map_floor_lookup(zn->room, zn->idx, re15_map_player_band(), &fp, &fr)) return 0;
    return (unsigned)fp == page;
}
