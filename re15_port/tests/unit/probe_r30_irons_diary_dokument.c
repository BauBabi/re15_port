/* probe_r30_irons_diary_dokument.c — MESSSONDE Runde 30, Thema irons-diary-dokument.
 * Dossier: analysis/befunde_runde30/irons-diary-dokument.md
 *
 * Reine Messung, KEIN Riegel (kein add_test): sie haelt fest, was der Port HEUTE tut,
 * damit der Bau-Agent dieselben Zahlen nach seiner Aenderung wieder misst.
 *
 *   A  FILE-Liste: wie viele Zeilen zeigen einen Namen, wie viele Unterstriche?
 *      Quelle der Namen: DEBUG.BIN, Maske u16[3] @0x800c6c98 (gelesen @0x800c72f0),
 *      Basis-Id u8[3] @0x800c7370.  SOLL nach dem Bau bei leerer Liste: 0 Namen.
 *   B  Leser: haengt der gezeigte Text von der gewaehlten Zeile ab?
 *      Original: nein - der Leser liest immer den Blob @0x800ccd34 (addiu @0x800c7614).
 *   C  Bild-Ebene: (1) Texel mit CLUT-Farbe 0x0000 aber Index != 0 zeichnet die PSX
 *      NICHT (psx-spx "Texture Color 0000h = fully transparent"); der Port prueft nur
 *      Index == 0.  (2) Seitenzaehlung gegen RE2s max_page (@0x800AA144 + doc*4).
 *   D  Prototyp FILE25 aus build/r30_irons-diary-dokument/: Masse, Seitenzahl,
 *      Titel == p00 (RE2-Konvention, Lader @0x8006d484-98), Farben nur aus FILE08.
 */
#include "re15_inv_screen.h"
#include "re15_re2doc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen/re2_files_toc.inc"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
#ifndef R30_PROTOTYP_DIR
#define R30_PROTOTYP_DIR "build/r30_irons-diary-dokument"
#endif

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];
static re15_inv_op_t s_ops2[RE15_INV_MAX_OPS];

static int zeile_von_y(int y)
{
    /* Zeilen bei y = 0x35 + row*16 (@0x800c7300 / @0x800c733c) */
    if (y < 0x35 || (y - 0x35) % 16 != 0) return -1;
    int r = (y - 0x35) / 16;
    return (r >= 0 && r < 10) ? r : -1;
}

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint8_t *datei(const char *pfad, long *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)*n);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f);
    return b;
}

int main(void)
{
    printf("=== r30 irons-diary-dokument: Messung des heutigen Stands ===\n");

    /* ---------------------------------------------------------------- A */
    printf("\n[A] FILE-Liste (substate 2, item_state 1)\n");
    int namen_gesamt = 0, striche_gesamt = 0;
    for (int pg = 0; pg < 3; pg++) {
        re15_inv_screen_t st = g_inv_screen;
        st.substate = 2; st.item_state = 1;
        st.file_page = (uint8_t)pg; st.file_sub = 0; st.file_row = 0;
        int n = re15_inv_screen_build(&st, s_ops, RE15_INV_MAX_OPS);
        int name[10] = {0}, strich[10] = {0};
        for (int i = 0; i < n; i++) {
            const re15_inv_op_t *o = &s_ops[i];
            if (o->kind != RE15_INV_OP_SPRT || o->page != RE15_INV_PAGE_FONT4) continue;
            if (o->x < 0x2c) continue;
            int r = zeile_von_y(o->y);
            if (r < 0) continue;
            if (o->clut == RE15_INV_CLUT_TEXROW0) name[r]++;       /* a3 = 0    @0x800c7320-28 */
            if (o->clut == RE15_INV_CLUT_TEXROW6) strich[r]++;     /* a3 = 0x30 @0x800c7310-1c */
        }
        int nn = 0, ns = 0;
        for (int r = 0; r < 10; r++) { nn += name[r] > 0; ns += strich[r] > 0; }
        printf("  Listenseite %d: %2d Zeilen mit NAMEN, %2d Zeilen mit Unterstrichen (%d Ops)\n",
               pg, nn, ns, n);
        namen_gesamt += nn; striche_gesamt += ns;
    }
    printf("  SUMME vorinstallierte Namen: %d  (Unterstrich-Zeilen: %d)\n",
           namen_gesamt, striche_gesamt);

    /* ---------------------------------------------------------------- B */
    printf("\n[B] Leser (item_state 3): haengt der Text von der Zeile ab?\n");
    {
        re15_inv_screen_t a = g_inv_screen, b = g_inv_screen;
        a.substate = b.substate = 2; a.item_state = b.item_state = 3;
        a.file_reader_page = b.file_reader_page = 1;
        a.file_page = 0; a.file_row = 0;
        b.file_page = 2; b.file_row = 7;
        int na = re15_inv_screen_build(&a, s_ops, RE15_INV_MAX_OPS);
        int nb = re15_inv_screen_build(&b, s_ops2, RE15_INV_MAX_OPS);
        int gleich = (na == nb) && memcmp(s_ops, s_ops2, (size_t)na * sizeof s_ops[0]) == 0;
        printf("  Liste 0/Zeile 0: %d Ops   Liste 2/Zeile 7: %d Ops   -> %s\n", na, nb,
               gleich ? "BITGLEICH (der Leser kennt die Zeile nicht)" : "verschieden");
    }

    /* ---------------------------------------------------------------- C */
    printf("\n[C] Bild-Ebene, Dokument 8 (FILE08)\n");
    re15_re2doc_set_root(RE15_ASSET_RE2_DIR "/FILES");
    {
        int pw = 0, ph = 0, H = re15_re2doc_page_height(8, -1);
        re15_re2doc_size(8, -1, RE15_RE2DOC_PAPER, &pw, &ph);
        long n = 0;
        uint8_t *tim = datei(RE15_ASSET_RE2_DIR "/FILES/FILE08_title_paper.TIM", &n);
        long farbe0_index_ungleich0 = 0, port_sichtbar_schwarz = 0, band = 0;
        if (tim && n > 20) {
            uint32_t clen = rd32(tim + 8);
            const uint8_t *clut = tim + 20;
            const uint8_t *img = tim + 8 + clen + 12;
            for (int v = H; v < ph; v++)
                for (int u = 0; u < pw; u++) {
                    unsigned idx = img[(size_t)v * (size_t)pw + (size_t)u];
                    uint16_t c = rd16(clut + idx * 2);
                    uint8_t r, g, b;
                    band++;
                    if (c == 0 && idx != 0) farbe0_index_ungleich0++;
                    if (re15_re2doc_pixel(8, -1, RE15_RE2DOC_PAPER, u, v, &r, &g, &b) &&
                        c == 0)
                        port_sichtbar_schwarz++;
                }
        }
        printf("  Papier %dx%d, H=%d, Band %ld Texel\n", pw, ph, H, band);
        printf("  Texel mit CLUT-Farbe 0x0000 und Index != 0 (PSX: durchsichtig): %ld\n",
               farbe0_index_ungleich0);
        printf("  davon zeichnet der Port DECKEND SCHWARZ:                        %ld\n",
               port_sichtbar_schwarz);
        free(tim);

        int port_seiten = re15_re2doc_page_count(8);
        int re2_seiten = (int)re2_files_doc[8].max_page + 1;
        printf("  Seitenzahl: Port zaehlt %d (Titel + p00..), RE2 blaettert 0..max_page = %d Seiten\n",
               port_seiten, re2_seiten);
        /* Ist p00 wirklich die Titelseite noch einmal? */
        int w = 0, h = 0, ungleich = 0;
        re15_re2doc_size(8, -1, RE15_RE2DOC_PAGE, &w, &h);
        for (int v = 0; v < h; v++)
            for (int u = 0; u < w; u++) {
                uint8_t r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
                int s1 = re15_re2doc_pixel(8, -1, RE15_RE2DOC_PAGE, u, v, &r1, &g1, &b1);
                int s2 = re15_re2doc_pixel(8, 0, RE15_RE2DOC_PAGE, u, v, &r2, &g2, &b2);
                if (s1 != s2 || (s1 && (r1 != r2 || g1 != g2 || b1 != b2))) ungleich++;
            }
        printf("  FILE08 title_page gegen p00: %d abweichende Pixel -> der Port zeigt den Titel %s\n",
               ungleich, ungleich == 0 ? "ZWEIMAL (Leserseite 0 und 1)" : "einmal");
    }

    /* ---------------------------------------------------------------- D */
    printf("\n[D] Prototyp FILE25 aus %s\n", R30_PROTOTYP_DIR);
    re15_re2doc_set_root(R30_PROTOTYP_DIR);
    {
        int w = 0, h = 0, pw = 0, ph = 0;
        int t = re15_re2doc_size(25, -1, RE15_RE2DOC_PAGE, &w, &h);
        int p = re15_re2doc_size(25, -1, RE15_RE2DOC_PAPER, &pw, &ph);
        if (!t || !p) {
            printf("  FEHLT - erst `python re15_port/tools/re2_doc_satz.py satz ...` laufen lassen\n");
            return 0;
        }
        int seiten = re15_re2doc_page_count(25);
        printf("  Titelseite %dx%d, Papier %dx%d, Port-Seitenzaehlung %d (= Titel + p00..p%02d)\n",
               w, h, pw, ph, seiten, seiten - 2);

        /* Titel == p00 */
        int ungleich = 0;
        for (int v = 0; v < h; v++)
            for (int u = 0; u < w; u++) {
                uint8_t r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
                int s1 = re15_re2doc_pixel(25, -1, RE15_RE2DOC_PAGE, u, v, &r1, &g1, &b1);
                int s2 = re15_re2doc_pixel(25, 0, RE15_RE2DOC_PAGE, u, v, &r2, &g2, &b2);
                if (s1 != s2 || (s1 && (r1 != r2 || g1 != g2 || b1 != b2))) ungleich++;
            }
        printf("  title_page gegen p00: %d abweichende Pixel (SOLL 0)\n", ungleich);

        /* Farben: nur die Eintraege 1..6 und 8 der FILE08-CLUT */
        long n = 0;
        uint8_t *vor = datei(RE15_ASSET_RE2_DIR "/FILES/FILE08_p01_page.TIM", &n);
        long fremd = 0, sichtbar = 0;
        if (vor) {
            uint8_t erl[7][3];
            static const int idx[7] = { 1, 2, 3, 4, 5, 6, 8 };
            for (int i = 0; i < 7; i++) {
                uint16_t c = rd16(vor + 20 + idx[i] * 2);
                erl[i][0] = (uint8_t)((c & 31) << 3);
                erl[i][1] = (uint8_t)(((c >> 5) & 31) << 3);
                erl[i][2] = (uint8_t)(((c >> 10) & 31) << 3);
            }
            for (int pg = -1; pg < seiten - 1; pg++) {
                int hh = re15_re2doc_page_height(25, pg);
                for (int v = 0; v < hh; v++)
                    for (int u = 0; u < w; u++) {
                        uint8_t r, g, b;
                        if (!re15_re2doc_pixel(25, pg, RE15_RE2DOC_PAGE, u, v, &r, &g, &b)) continue;
                        sichtbar++;
                        int ok = 0;
                        for (int i = 0; i < 7; i++)
                            if (erl[i][0] == r && erl[i][1] == g && erl[i][2] == b) ok = 1;
                        if (!ok) fremd++;
                    }
            }
            free(vor);
        }
        printf("  sichtbare Pixel ueber alle Seiten: %ld, davon mit einer Farbe AUSSERHALB der "
               "FILE08-CLUT[1..6,8]: %ld (SOLL 0)\n", sichtbar, fremd);

        /* Papier byte-gleich FILE08? */
        long n1 = 0, n2 = 0;
        uint8_t *a = datei(R30_PROTOTYP_DIR "/FILE25_title_paper.TIM", &n1);
        uint8_t *b = datei(RE15_ASSET_RE2_DIR "/FILES/FILE08_title_paper.TIM", &n2);
        printf("  FILE25_title_paper.TIM %ld B gegen FILE08_title_paper.TIM %ld B: %s\n", n1, n2,
               (a && b && n1 == n2 && memcmp(a, b, (size_t)n1) == 0) ? "BYTE-GLEICH" : "VERSCHIEDEN");
        /* Kopf der Textseite byte-gleich der Vorlage (erste 64 Byte = Kopf + CLUT + Bildkopf)? */
        long n3 = 0, n4 = 0;
        uint8_t *c = datei(R30_PROTOTYP_DIR "/FILE25_p01_page.TIM", &n3);
        uint8_t *d = datei(RE15_ASSET_RE2_DIR "/FILES/FILE08_p01_page.TIM", &n4);
        printf("  FILE25_p01_page.TIM %ld B gegen FILE08_p01_page.TIM %ld B: Kopf+CLUT+Bildkopf "
               "(64 Byte) %s\n", n3, n4,
               (c && d && n3 == n4 && memcmp(c, d, 64) == 0) ? "BYTE-GLEICH" : "VERSCHIEDEN");
        free(a); free(b); free(c); free(d);
    }
    return 0;
}
