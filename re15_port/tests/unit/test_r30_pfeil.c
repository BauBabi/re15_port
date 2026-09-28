/* test_r30_pfeil.c — RIEGEL Runde 30, Nachschliff Spur pfeil: die Blaetter-Pfeile des
 * Lesers fuer Bild-Dokumente (Irons Diary) nach RE2.
 * Dossier: analysis/befunde_runde30/nachschliff-pfeil.md
 *
 * ANLASS (Gegenpruefung Runde 30): RE1.5s linker Pfeil (x = 0x14 - off, 16x16, DEBUG.BIN
 * @0x800c7554-70) lag mit off = 0 auf der ersten Glyphen-Spalte der RE2-Textseite
 * (Textseite bei x = 25, RE2 `addiu v0,zero,25` @0x80076170) - am Framedump des
 * Integrationsstands cac33993 12 der 17 Textseiten plus die Ende-Stellung, 6..16
 * Glyphen-Pixel je Seite.
 *
 * NACHTRAG J (englischer Satz): das Diary hat jetzt max_page 15 = Titel + p01..p15, Ende-
 *         Stellung Seite 16 (re15_files.c, gemessen am Satz). Die Seitenzahl kommt unten aus
 *         DIARY_LETZTE / DIARY_ENDE; Teil A prueft sie gegen die Dokument-Tabelle. Die Zahlen
 *         der Negativ-Kontrolle haengen am TEXT der Seiten und sind neu gemessen (s. B).
 *
 * TEIL A  ANZEIGELISTE je Seite (Titel, p01..p15, Ende-Stellung) und Wipp-Stellung 0/1:
 *         genau RE2s Sprites (info/re2leon/PSX.EXE FUN_80075fd0 / FUN_800724b4):
 *           Seite < letzte: Pfeil rechts (282 + 3*b, 110) 12x13 uv (42,12) CLUT (256,492)
 *                           (@0x80072628-70, u/w/h/v @0x80076104-44)
 *           letzte Seite + Ende-Stellung: Ende-Marke (280,110) 42x14 uv (56,12) CLUT
 *                           (256,490), Helligkeit 48 / in der Ende-Stellung 128
 *                           (@0x800725c4-624, @0x80072678)
 *           Seite != 0 und nicht Ende-Stellung: Pfeil links (12 - 3*b, 110) uv (28,12)
 *                           (@0x800726b8-f8)
 *         alle Code 0x66 (abe 1, @0x80076120), keine RE1.5-Pfeile (TEX4 16x16) mehr.
 * TEIL B  UEBERDECKUNG (der eigentliche Riegel): jedes gezeichnete Pfeil-Pixel (CLUT-Farbe
 *         != 0 aus RE2s ST0.TIM) gegen jedes sichtbare Pixel der Textseite bei (25,30):
 *         Soll 0 auf allen 17 Stellungen x 2 Wipp-Stellungen.
 *         NEGATIV-KONTROLLE: dieselbe Rechnung mit den Pfeilen, die der Port VORHER fuer
 *         Bild-Dokumente zeichnete (RE1.5-Emitter, off 0/4, TEX.TIM): Soll 79 Pixel auf
 *         10 Stellungen (englischer Satz, r30_pfeil_ueberdeckung.py gegen die kopierten
 *         FILE25: p01 13, p02 7, p03 7, p05 3, p07 6, p09 7, p11 14, p12 6, p13 13, p14 3,
 *         Ende 0 - p15 traegt nur Zeile 0). Deutsche Fassung vorher: 126 auf 13, p01 8,
 *         p02 13, Ende 7 (wie am Framedump des Integrationsstands cac33993).
 * TEIL C  WIPP-TAKT im laufenden Automaten (menu_common.c):
 *         C1 Aufnahme-Leser (re15_menu_request_doc): nach der Ankunft 41 Bilder b = 0
 *            (Ankunftsbild + 40), dann 39 / 39 (Schwelle 0x51 @0x80072800, Start c = 2
 *            @0x80072aa0-b0, c +-2 @0x800727f4/@0x80072818, Umkehr c < 10 @0x800727dc).
 *         C1b Blaettern, waehrend b = 1 steht: Ankunft mit b = 0, wieder 41 Bilder b = 0.
 *         C2 Ende-Stellung zaehlt nicht (RE2 Zustand 1 @0x80072918); LINKS daraus setzt
 *            b = 0, c = 2 (@0x80072944-48) -> wieder 41 Bilder b = 0.
 *         C3 Leser aus der FILE-Liste: 26, dann 24 / 24 (Schwelle 0x33 @0x8006d0e8).
 * TEIL D  RE1.5-Textleser (file_bild = 0) unveraendert: Pfeile x = 0x14 - off / 0x11c + off,
 *         y 0x70, 16x16, TEX4 (@0x800c7554-a8).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_inv_screen.h"
#include "re15_re2doc.h"
#include "re15_tim.h"
#include "re15_menu.h"
#include "re15_files.h"
#include "re15_fade.h"
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_inventory.h"  /* re15_inv_set_equipped_slot */

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];

/* Irons Diary, englischer Satz (Nachtrag J): letzte Seite = max_page 15 (re15_files.c,
 * Port-Wahl, keine Original-Adresse - gemessen am Satz FILE25_satz.txt), Ende-Stellung =
 * max_page + 1 (RE2 `lhu a0,-24252(at)` @0x800727c8, +1). */
#define DIARY_LETZTE 15
#define DIARY_ENDE   (DIARY_LETZTE + 1)

/* ------------------------------------------------------------ Texel der Quellen */
static uint8_t  st0_tex[72][256];      /* RE2 ST0.TIM zweites TIM, 4bpp-Indizes */
static uint16_t st0_clut[3][16];       /* Datei-Zeilen 0..2 (VRAM 490..492)      */
static uint8_t  tex_tex[256][256];     /* RE1.5 TEX.TIM Spalten 0..255           */
static uint16_t tex_clut[8][16];       /* Selektor s = TEX.TIM CLUT-Zeile 8 + s  */

static uint8_t *datei(const char *pfad, int *sz)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t *b;
    long n;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
    b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    *sz = (int)n;
    return b;
}

static uint32_t le32(const uint8_t *b, int o)
{
    return (uint32_t)b[o] | (uint32_t)b[o + 1] << 8 | (uint32_t)b[o + 2] << 16 |
           (uint32_t)b[o + 3] << 24;
}

static int quellen_laden(void)
{
    int sz = 0, off, y, u, k;
    re15_tim_t t;
    uint8_t *b = datei(RE15_ASSET_RE2_DIR "/ST0.TIM", &sz);
    CHECK(b != NULL && sz == 77536, "shared_assets/RE2/ST0.TIM da, %d Byte (Soll 77536 = "
          "info/re2leon/COMMON/DATA/ST0.TIM)", sz);
    if (!b) return 0;
    off = 8 + (int)le32(b, 8);           /* erstes TIM: CLUT-Block */
    off += (int)le32(b, off);            /* + Bild-Block */
    CHECK(off == 0x10820, "zweites TIM @0x%x (Soll 0x10820; RE2 laedt 0x80198000 + 0x10820 "
          "@0x80068580-84)", off);
    if (re15_tim_parse(b + off, sz - off, &t) != 0 || t.bpp != 4) { free(b); return 0; }
    CHECK(t.width == 256 && t.height == 72 && t.clut_entries == 16 * 21,
          "Blatt 2: %dx%d Texel, %d CLUT-Eintraege (Soll 256x72, 16x21)", t.width, t.height,
          t.clut_entries);
    for (y = 0; y < 72; y++)
        for (u = 0; u < 256; u++) {
            uint8_t bb = ((const uint8_t *)t.pixels)[y * (t.width / 2) + (u >> 1)];
            st0_tex[y][u] = (u & 1) ? (bb >> 4) : (bb & 15);
        }
    for (k = 0; k < 3; k++) memcpy(st0_clut[k], t.clut + k * 16, sizeof st0_clut[k]);
    CHECK(st0_clut[2][5] == 0x1386 && st0_clut[2][6] == 0x09c3 && st0_clut[2][7] == 0x0060,
          "Pfeil-CLUT (256,492) = Datei-Zeile 2: 5 %04x 6 %04x 7 %04x (Soll 1386 09c3 0060 = "
          "gruen; Lader-Wort 0x0a1b @0x80068588)", st0_clut[2][5], st0_clut[2][6],
          st0_clut[2][7]);
    free(b);

    b = datei(RE15_ASSET_PSX_DIR "/DATA/TEX.TIM", &sz);
    if (!b || re15_tim_parse(b, sz, &t) != 0 || t.bpp != 4) { free(b); return 0; }
    for (y = 0; y < 256 && y < t.height; y++)
        for (u = 0; u < 256; u++) {
            uint8_t bb = ((const uint8_t *)t.pixels)[y * (t.width / 2) + (u >> 1)];
            tex_tex[y][u] = (u & 1) ? (bb >> 4) : (bb & 15);
        }
    for (k = 0; k < 8; k++) memcpy(tex_clut[k], t.clut + (8 + k) * 32, sizeof tex_clut[k]);
    free(b);
    re15_re2doc_set_root(RE15_ASSET_RE2_DIR "/FILES");
    return 1;
}

/* Pixel eines Pfeil-Sprites, das gezeichnet wird (CLUT-Farbe != 0). Rueckgabe: Anzahl;
 * *auf_glyphen = davon auf sichtbaren Pixeln der Textseite (Satz, Datei-Seite) bei (25,30). */
static int sprite_pixel(const re15_inv_op_t *o, int satz, int dseite, int *auf_glyphen)
{
    int n = 0;
    for (int dy = 0; dy < o->h; dy++)
        for (int dx = 0; dx < o->w; dx++) {
            int u = o->u + dx, v = o->v + dy, idx;
            uint16_t c;
            if (o->page == RE15_INV_PAGE_RE2ST0) {
                if (v >= 72 || u >= 256) continue;
                idx = st0_tex[v][u];
                c = st0_clut[o->clut == RE15_INV_CLUT_RE2ST0_Z0 ? 0 : 2][idx];
            } else {
                idx = tex_tex[v & 255][u & 255];
                c = tex_clut[o->clut & 7][idx];
            }
            if (c == 0) continue;
            n++;
            {
                uint8_t r, g, b;
                int x = o->x + dx, y = o->y + dy;
                if (re15_re2doc_pixel(satz, dseite, RE15_RE2DOC_PAGE, x - 25, y - 30, &r, &g, &b))
                    (*auf_glyphen)++;
            }
        }
    return n;
}

static int ist_pfeil_re15(const re15_inv_op_t *o)
{
    return o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_TEX4 && o->w == 16 &&
           o->h == 16 && o->u == 0x70 && (o->v == 0x38 || o->v == 0x48);
}

/* ------------------------------------------------------------------ TEIL A + B */
static void teil_ab(void)
{
    re15_inv_screen_t t;
    int seiten_ok = 0, falsch = 0, re15_rest = 0, sum_ueber = 0, sum_pixel = 0;
    int neg_sum = 0, neg_seiten = 0, neg_p01 = -1, neg_p02 = -1, neg_ende = -1;
    printf("\n[A/B] Anzeigeliste und Ueberdeckung, %d Stellungen x 2 Wipp-Stellungen\n",
           DIARY_ENDE + 1);
    {
        const re15_file_doc_t *d = re15_files_doc(0);
        CHECK(d && d->max_page == DIARY_LETZTE,
              "A  Dokument-Tabelle: Irons Diary max_page %d (Soll %d, englischer Satz)",
              d ? d->max_page : -1, DIARY_LETZTE);
    }
    memset(&t, 0, sizeof t);
    t.substate = 2;
    t.item_state = 3;
    t.file_bild = 1; t.file_bildsatz = 25; t.file_end = DIARY_ENDE;     /* Irons Diary */
    for (int pg = 0; pg <= DIARY_ENDE; pg++) {
        int seite_neg = 0;
        for (int b = 0; b <= 1; b++) {
            int satz = -1, dseite = -9, tx = 0, n, ok = 1, rechts = 0, links = 0, marke = 0;
            t.file_reader_page = (uint8_t)pg;
            t.file_re2_wippe = (uint8_t)b;
            n = re15_inv_screen_build(&t, s_ops, RE15_INV_MAX_OPS);
            re15_inv_file_bild_lage(&t, &satz, &dseite, &tx);
            for (int i = 0; i < n; i++) {
                const re15_inv_op_t *o = &s_ops[i];
                if (ist_pfeil_re15(o)) {
                    /* jeder Pfeil im Bild-Leser zaehlt fuer B, auch ein RE1.5-Pfeil */
                    int auf = 0;
                    re15_rest++;
                    sum_pixel += sprite_pixel(o, satz, dseite, &auf);
                    sum_ueber += auf;
                    continue;
                }
                if (!(o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_RE2ST0)) continue;
                if (o->abe != 1 || o->y != 110) ok = 0;
                if (o->u == 42) {
                    rechts++;
                    if (!(pg < DIARY_LETZTE && o->x == 282 + 3 * b && o->w == 12 && o->h == 13 &&
                          o->v == 12 && o->clut == RE15_INV_CLUT_RE2ST0_Z2 &&
                          o->r == 128 && o->g == 128 && o->b == 128)) ok = 0;
                } else if (o->u == 28) {
                    links++;
                    if (!(pg != 0 && pg != DIARY_ENDE && o->x == 12 - 3 * b && o->w == 12 &&
                          o->h == 13 && o->v == 12 && o->clut == RE15_INV_CLUT_RE2ST0_Z2 &&
                          o->r == 128)) ok = 0;
                } else if (o->u == 56) {
                    marke++;
                    if (!(pg >= DIARY_LETZTE && o->x == 280 && o->w == 42 && o->h == 14 &&
                          o->v == 12 && o->clut == RE15_INV_CLUT_RE2ST0_Z0 &&
                          o->r == (pg == DIARY_ENDE ? 128 : 48) && o->g == o->r && o->b == o->r))
                        ok = 0;
                } else ok = 0;
                {
                    int auf = 0;
                    sum_pixel += sprite_pixel(o, satz, dseite, &auf);
                    sum_ueber += auf;
                }
            }
            if (rechts != (pg < DIARY_LETZTE) || marke != (pg >= DIARY_LETZTE) ||
                links != (pg != 0 && pg != DIARY_ENDE))
                ok = 0;
            if (ok) seiten_ok++; else {
                falsch++;
                printf("  Stellung Seite %d Wippe %d: rechts %d links %d Marke %d - FALSCH\n",
                       pg, b, rechts, links, marke);
            }
            /* NEGATIV-KONTROLLE: RE1.5s Emitter (file_bild = 0), off = 4 * b, gegen
             * dieselbe RE2-Textseite */
            {
                re15_inv_screen_t alt = t;
                int m;
                alt.file_bild = 0;
                alt.file_bob_off = (uint16_t)(4 * b);
                m = re15_inv_screen_build(&alt, s_ops, RE15_INV_MAX_OPS);
                for (int i = 0; i < m; i++)
                    if (ist_pfeil_re15(&s_ops[i])) {
                        int auf = 0;
                        sprite_pixel(&s_ops[i], satz, dseite, &auf);
                        neg_sum += auf;
                        seite_neg += auf;
                        if (b == 0 && pg == 1)  neg_p01 = (neg_p01 < 0 ? 0 : neg_p01) + auf;
                        if (b == 0 && pg == 2)  neg_p02 = (neg_p02 < 0 ? 0 : neg_p02) + auf;
                        if (b == 0 && pg == DIARY_ENDE)
                            neg_ende = (neg_ende < 0 ? 0 : neg_ende) + auf;
                    }
            }
        }
        if (seite_neg) neg_seiten++;
    }
    CHECK(falsch == 0 && seiten_ok == 2 * (DIARY_ENDE + 1),
          "A  RE2-Sprites in allen %d von %d Stellungen (Lage, uv, Groesse, CLUT, Helligkeit)",
          seiten_ok, 2 * (DIARY_ENDE + 1));
    CHECK(re15_rest == 0, "A  keine RE1.5-Pfeile (TEX4 16x16) mehr im Bild-Leser: %d", re15_rest);
    CHECK(sum_pixel > 0, "B  gezeichnete Pfeil-/Marken-Pixel gesamt %d (> 0, sonst waere der "
          "Riegel leer)", sum_pixel);
    CHECK(sum_ueber == 0, "B  Pfeil-Pixel auf Glyphen-Pixeln der Textseite: %d (Soll 0)",
          sum_ueber);
    printf("  NEGATIV-KONTROLLE (RE1.5-Pfeile an RE1.5-Lage): %d Pixel auf Glyphen, auf %d "
           "Stellungen; p01 %d, p02 %d, Ende %d\n", neg_sum, neg_seiten, neg_p01, neg_p02,
           neg_ende);
    /* Ende 0: die Ende-Stellung zeigt p15, und p15 traegt nur Zeile 0 (y 30..45) - der
     * linke RE1.5-Pfeil liegt bei y 0x70..0x7f; neg_ende wird trotzdem GEMESSEN (>= 0), d. h.
     * der Pfeil wurde gezeichnet und traf nichts. */
    CHECK(neg_sum == 79 && neg_seiten == 10 && neg_p01 == 13 && neg_p02 == 7 && neg_ende == 0,
          "B  Negativ-Kontrolle: die alte Lage ueberdeckt %d Glyphen-Pixel auf %d Stellungen "
          "(Soll 79 auf 10 Textseiten; p01 13, p02 7, Ende 0 - englischer Satz, "
          "r30_pfeil_ueberdeckung.py)", neg_sum, neg_seiten);
}

/* ------------------------------------------------------------------ TEIL C */
static void frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}
static void fframe(uint16_t p) { frame(p, p); }

/* Folge der gezeichneten Wipp-Stellung ab dem Ankunftsbild (Index 0), n Bilder. Liefert
 * die ersten 4 Lauflaengen. */
static void laeufe(int n, int out[4])
{
    int last = g_inv_screen.file_re2_wippe, len = 1, k = 0;
    for (int i = 1; i < n && k < 4; i++) {
        frame(0, 0);
        if (g_inv_screen.file_re2_wippe == last) len++;
        else { out[k++] = len; len = 1; last = g_inv_screen.file_re2_wippe; }
    }
    while (k < 4) out[k++] = -1;
}

static int bis_zustand3(void)
{
    int n = 0;
    while (n < 400 && !(re15_menu_phase() == 1 && g_inv_screen.item_state == 3)) {
        frame(0, 0); n++;
    }
    return n;
}

static void teil_c(void)
{
    int l[4], n;
    printf("\n[C] Wipp-Takt im Automaten\n");

    /* C1 Aufnahme-Leser */
    re15_files_reset();
    re15_menu_request_doc(0, 0, -1, -1);
    n = bis_zustand3();
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_bild == 1 &&
          g_inv_screen.file_re2_wippe == 0 && re15_menu_doc_active(),
          "C1 Aufnahme-Leser liest nach %d Bildern, Ankunftsbild Wippe 0", n);
    laeufe(200, l);
    printf("  Aufnahme-Leser: Laeufe %d / %d / %d / %d\n", l[0], l[1], l[2], l[3]);
    CHECK(l[0] == 41 && l[1] == 39 && l[2] == 39 && l[3] == 39,
          "C1 Aufnahme-Leser: 41 Bilder b = 0 (Ankunft + 40), dann 39 / 39 / 39 "
          "(Schwelle 0x51 @0x80072800, Umkehr c < 10 @0x800727dc, Start c = 2 @0x80072aa0)");
    /* C1b Blaettern startet die Wippe neu (RE2 @0x80072a9c-b0: jede Ankunft b = 0, c = 2),
     * geblaettert, WAEHREND b = 1 steht; dabei die Lage in beiden Stellungen am laufenden
     * Automaten. */
    {
        int x_l[2] = { -1, -1 }, x_r[2] = { -1, -1 }, k0 = 0, nullen = 0, an = -1;
        while (k0 < 200 && g_inv_screen.file_re2_wippe != 1) { frame(0, 0); k0++; }
        fframe(RE15_PAD_BIT_RIGHT);               /* auf Seite 1 (linker Pfeil da) */
        bis_zustand3();
        an = g_inv_screen.file_re2_wippe;
        for (int i = 0; i < 90; i++) {
            int m = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
            int b = g_inv_screen.file_re2_wippe;
            if (b == 0 && nullen == i) nullen++;
            for (int k = 0; k < m; k++)
                if (s_ops[k].kind == RE15_INV_OP_SPRT && s_ops[k].page == RE15_INV_PAGE_RE2ST0) {
                    if (s_ops[k].u == 28) x_l[b] = s_ops[k].x;
                    if (s_ops[k].u == 42) x_r[b] = s_ops[k].x;
                }
            frame(0, 0);
        }
        CHECK(an == 0 && nullen == 41,
              "C1b Blaettern bei b = 1: Ankunft mit b = %d, dann %d Bilder b = 0 (Soll 0 / 41; "
              "Neustart @0x80072aa0-b0)", an, nullen);
        CHECK(x_l[0] == 12 && x_l[1] == 9 && x_r[0] == 282 && x_r[1] == 285,
              "C1 am Automaten: links x %d / %d, rechts x %d / %d (Soll 12/9, 282/285; "
              "@0x800726c4-d4, @0x80072660-68)", x_l[0], x_l[1], x_r[0], x_r[1]);
    }

    /* C2 Ende-Stellung zaehlt nicht; LINKS daraus startet neu */
    for (int p = 1; p < DIARY_LETZTE; p++) { fframe(RE15_PAD_BIT_RIGHT); bis_zustand3(); }
    CHECK(g_inv_screen.file_reader_page == DIARY_LETZTE, "C2 letzte Seite %d erreicht (%d)",
          DIARY_LETZTE, g_inv_screen.file_reader_page);
    for (int i = 0; i < 45; i++) frame(0, 0);      /* Wippe steht jetzt auf 1 */
    {
        int b0 = g_inv_screen.file_re2_wippe, gleich = 1;
        fframe(RE15_PAD_BIT_RIGHT);               /* -> Ende-Stellung, kein Blaettern */
        for (int i = 0; i < 120; i++) {
            frame(0, 0);
            if (g_inv_screen.file_re2_wippe != b0) gleich = 0;
        }
        CHECK(g_inv_screen.file_reader_page == DIARY_ENDE && b0 == 1 && gleich,
              "C2 Ende-Stellung: 120 Bilder ohne Zaehlung, Wippe bleibt %d (RE2 Zustand 1 "
              "@0x80072918 zaehlt nicht)", b0);
        fframe(RE15_PAD_BIT_LEFT);
        CHECK(g_inv_screen.file_reader_page == DIARY_LETZTE && g_inv_screen.file_re2_wippe == 0,
              "C2 LINKS aus der Ende-Stellung: Seite %d, Wippe 0 (@0x80072944-48)",
              DIARY_LETZTE);
        laeufe(120, l);
        CHECK(l[0] == 41 && l[1] == 39,
              "C2 danach wieder 41 / 39 (%d / %d) - Zaehler auf 2 gesetzt", l[0], l[1]);
    }

    /* schliessen: KREUZ -> Meldung -> bestaetigen -> Menue zu -> Spiel */
    fframe(RE15_PAD_BIT_CROSS);
    {
        uint8_t mid = 0; int rev = 0, total = re15_menu_doc_msg_total(), k = 0;
        while (k < 400 && re15_menu_doc_msg(&mid, &rev) && rev < total) { frame(0, 0); k++; }
        fframe(RE15_PAD_BIT_SQUARE);
        k = 0;
        while (k < 400 && (re15_menu_is_open() || re15_menu_stage() != 0)) { frame(0, 0); k++; }
    }
    CHECK(!re15_menu_is_open() && !re15_menu_doc_active() && re15_files_get(0) == 0,
          "C  Aufnahme-Leser geschlossen, Irons Diary auf Platz 0");

    /* C3 Leser aus der FILE-Liste (Weg wie test_inv_fsm.c F1..F6) */
    re15_inv_set_equipped_slot(0x80);
    re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    re15_inv_screen_sync_equip();
    fframe(RE15_PAD_BIT_R1);                      /* FILE-Reiter */
    for (int i = 0; i < 31; i++) frame(0, 0);     /* Rutsche -> Liste */
    fframe(RE15_PAD_BIT_SQUARE);                  /* Zeilenwahl */
    fframe(RE15_PAD_BIT_SQUARE);                  /* Zeile 0 oeffnen */
    n = bis_zustand3();
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_bild == 1 &&
          !re15_menu_doc_active() && g_inv_screen.file_re2_wippe == 0,
          "C3 Leser aus der Liste liest nach %d Bildern, Ankunftsbild Wippe 0", n);
    laeufe(200, l);
    printf("  Listen-Leser: Laeufe %d / %d / %d / %d\n", l[0], l[1], l[2], l[3]);
    CHECK(l[0] == 26 && l[1] == 24 && l[2] == 24 && l[3] == 24,
          "C3 Listen-Leser: 26 Bilder b = 0 (Ankunft + 25), dann 24 / 24 / 24 "
          "(Schwelle 0x33 @0x8006d0e8, Start @0x8006d290-9c)");
    fframe(RE15_PAD_BIT_CROSS);                   /* zurueck in die Liste */
    CHECK(g_inv_screen.item_state == 1 && g_inv_screen.file_bild == 0,
          "C3 KREUZ: zurueck in der Liste");
}

/* ------------------------------------------------------------------ TEIL D */
static void teil_d(void)
{
    re15_inv_screen_t t;
    int ok = 1, n_l = 0, n_r = 0;
    printf("\n[D] RE1.5-Textleser unveraendert\n");
    memset(&t, 0, sizeof t);
    t.substate = 2; t.item_state = 3; t.file_bild = 0; t.file_reader_page = 1;
    for (int off = 0; off <= 4; off += 4) {
        int n;
        t.file_bob_off = (uint16_t)off;
        n = re15_inv_screen_build(&t, s_ops, RE15_INV_MAX_OPS);
        for (int i = 0; i < n; i++) {
            const re15_inv_op_t *o = &s_ops[i];
            if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_RE2ST0) ok = 0;
            if (!ist_pfeil_re15(o)) continue;
            if (o->v == 0x38) { n_l++; if (o->x != 0x14 - off || o->y != 0x70 || o->clut != 7) ok = 0; }
            if (o->v == 0x48) { n_r++; if (o->x != 0x11c + off || o->y != 0x70 || o->clut != 7) ok = 0; }
        }
    }
    CHECK(ok && n_l == 2 && n_r == 2,
          "D  Textleser Seite 1: RE1.5-Pfeile links (0x14-off,0x70) / rechts (0x11c+off,0x70) "
          "clut UI7, off 0/4 (@0x800c7554-a8), keine RE2-Sprites");
}

int main(void)
{
    printf("=== r30 Nachschliff pfeil: RE2-Pfeile im Leser fuer Bild-Dokumente ===\n");
    if (!quellen_laden()) { printf("Quellen fehlen\n"); return 1; }
    teil_ab();
    teil_c();
    teil_d();
    if (fails) { printf("\nR30 PFEIL: FAIL (%d)\n", fails); return 1; }
    printf("\nR30 PFEIL: alle Riegel halten\n");
    return 0;
}
