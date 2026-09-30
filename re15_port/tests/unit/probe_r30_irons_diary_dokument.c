/* probe_r30_irons_diary_dokument.c — RIEGEL Runde 30, Thema irons-diary-dokument.
 * Dossier: analysis/befunde_runde30/irons-diary-dokument.md
 *
 * Bis zum Bau war das eine reine Messsonde (kein add_test); sie hielt den Stand VOR der
 * Aenderung fest. Gemessen auf master d98e9639, vor dem ersten Edit:
 *     [A] 21 vorinstallierte Namen (1 + 10 + 10), 9 Unterstrich-Zeilen
 *     [B] Leser Liste 0/Zeile 0 gegen Liste 2/Zeile 7: 394 Ops, BITGLEICH
 *     [C] 7410 Texel mit CLUT-Farbe 0x0000 und Index != 0, davon 7410 deckend schwarz;
 *         Seitenzahl Port 6 gegen RE2 5; Titel zweimal
 * Jetzt ist sie der Riegel: dieselben Groessen, mit Soll.
 *
 *   A  FILE-Liste: bei leerer Liste 0 Zeilen mit Namen, nach re15_files_add(0) genau 1.
 *      Die 21 Namen des Originals (Maske u16[3] @0x800c6c98, Basis-Id u8[3] @0x800c7370,
 *      DEBUG.BIN) bleiben als ARCHIV zaehlbar — der Riegel prueft, dass sie noch 21 sind
 *      (die erzeugte Tabelle wurde also nicht angefasst) und dass das Spiel sie nicht zeigt.
 *   B  Leser: zwei verschiedene Dokumente ergeben verschiedene Anzeigelisten, und ein
 *      Bild-Dokument druckt KEINEN Zeichenstrom (RE1.5 adressierte den Blob fest,
 *      `addiu t1,t1,-13004` @0x800c7614 = 0x800ccd34).
 *   C  Bild-Ebene: 0 deckend gezeichnete Texel der CLUT-Farbe 0x0000 (psx-spx "Color
 *      0000h = Fully-transparent"); Seitenzahl = max_page + 1 = 16; p15 da, p16 nicht
 *      (Nachtrag J, englischer Satz; die deutsche Fassung hatte 18 / p17).
 *   D  die nach shared_assets/RE2/FILES KOPIERTEN Dateien FILE25_*: Masse, Titel == p00
 *      (RE2-Konvention, Lader @0x8006d484-98), Farben nur aus FILE08, Papier byte-gleich.
 *   E  Seite -> Datei und x-Lage der Textseite (re15_inv_file_bild_lage; RE2 Seitenlader
 *      0x8006d444, Ruhelage 25 @0x80076170).
 */
#include "re15_inv_screen.h"
#include "re15_re2doc.h"
#include "re15_files.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen/re2_files_toc.inc"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];
static re15_inv_op_t s_ops2[RE15_INV_MAX_OPS];

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

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

/* Zaehlt ueber alle drei Listenseiten: Zeilen mit Namen, Zeilen mit Unterstrichen,
 * Glyphen der Namen, Glyphen der Titelzeile (clut row 2). */
static void liste_zaehlen(int *namen, int *striche, int *namen_glyphen, int *titel_glyphen,
                          int namen_je_seite[3])
{
    *namen = *striche = *namen_glyphen = 0;
    for (int pg = 0; pg < 3; pg++) {
        re15_inv_screen_t st = g_inv_screen;
        st.substate = 2; st.item_state = 1;
        st.file_page = (uint8_t)pg; st.file_sub = 0; st.file_row = 0;
        st.file_bild = 0;
        int n = re15_inv_screen_build(&st, s_ops, RE15_INV_MAX_OPS);
        int name[10] = {0}, strich[10] = {0}, tg = 0;
        for (int i = 0; i < n; i++) {
            const re15_inv_op_t *o = &s_ops[i];
            if (o->kind != RE15_INV_OP_SPRT || o->page != RE15_INV_PAGE_FONT4) continue;
            if (o->clut == RE15_INV_CLUT_TEXROW2 && o->y == 0x1f) tg++;   /* @0x800c7284-9c */
            if (o->x < 0x2c) continue;
            int r = zeile_von_y(o->y);
            if (r < 0) continue;
            if (o->clut == RE15_INV_CLUT_TEXROW0) { name[r]++; (*namen_glyphen)++; } /* a3 = 0 */
            if (o->clut == RE15_INV_CLUT_TEXROW6) strich[r]++;                       /* a3 = 0x30 */
        }
        int nn = 0, ns = 0;
        for (int r = 0; r < 10; r++) { nn += name[r] > 0; ns += strich[r] > 0; }
        printf("  Listenseite %d: %2d Zeilen mit NAMEN, %2d Zeilen mit Unterstrichen, "
               "Titel %d Glyphen (%d Ops)\n", pg, nn, ns, tg, n);
        *namen += nn; *striche += ns;
        titel_glyphen[pg] = tg;
        namen_je_seite[pg] = nn;
    }
}

int main(void)
{
    printf("=== r30 irons-diary-dokument: RIEGEL ===\n");
    re15_re2doc_set_root(RE15_ASSET_RE2_DIR "/FILES");

    /* ---------------------------------------------------------------- A */
    printf("\n[A] FILE-Liste (substate 2, item_state 1)\n");
    {
        int namen, striche, glyphen, titel[3], je[3];

        /* ARCHIV: die Originaltabelle ist unangetastet und traegt weiter 21 Namen. */
        int archiv = 0;
        for (int pg = 0; pg < 3; pg++) {
            int m = re15_inv_file_archiv_maske(pg);
            for (int r = 0; r < 10; r++) archiv += (m >> r) & 1;
        }
        CHECK(archiv == 21,
              "ARCHIV: die Originalmaske @0x800c6c98 traegt weiter %d Namen (Soll 21 = "
              "1 + 10 + 10) - die erzeugte Tabelle ist nicht angefasst", archiv);
        CHECK(re15_inv_file_archiv_basis(0) == 0x48 && re15_inv_file_archiv_basis(1) == 0x52 &&
              re15_inv_file_archiv_basis(2) == 0x5c,
              "ARCHIV: Basis-Ids @0x800c7370 = %02x %02x %02x (Soll 48 52 5c)",
              re15_inv_file_archiv_basis(0), re15_inv_file_archiv_basis(1),
              re15_inv_file_archiv_basis(2));
        {
            /* "Chris' Diary" @0x800c4e04 - der Name, der unter Id 0x48 stand */
            static const uint8_t chris[13] = { 0x1f,0x44,0x4e,0x45,0x4f,0x3a,0x00,
                                               0x20,0x45,0x3d,0x4e,0x55,0x07 };
            const uint8_t *a = re15_inv_file_archiv_name(0x48);
            CHECK(a && memcmp(a, chris, sizeof chris) == 0,
                  "ARCHIV: unter Id 0x48 steht weiter \"Chris' Diary\" (@0x800c4e04)");
        }

        re15_files_reset();
        printf("  -- leere Liste --\n");
        liste_zaehlen(&namen, &striche, &glyphen, titel, je);
        CHECK(namen == 0,
              "leere Liste: %d Zeilen mit Namen (Soll 0; vor dem Bau 21)", namen);
        CHECK(striche == 30,
              "leere Liste: %d Unterstrich-Zeilen (Soll 30 = 3 Seiten x 10)", striche);
        CHECK(titel[0] == 5 && titel[1] == 5 && titel[2] == 5,
              "alle drei Listenseiten heissen \"Files\" = 5 Glyphen (Titel 0 @0x800c78f0): "
              "%d / %d / %d", titel[0], titel[1], titel[2]);

        int platz = re15_files_add(0);
        printf("  -- nach re15_files_add(0), Platz %d --\n", platz);
        liste_zaehlen(&namen, &striche, &glyphen, titel, je);
        CHECK(platz == 0, "das erste Dokument bekommt Platz 0 (FUN_800692dc @0x800692f8), "
              "ist %d", platz);
        CHECK(namen == 1 && je[0] == 1,
              "nach re15_files_add(0): %d Zeile mit Namen, auf Listenseite 0 (Soll 1)", namen);
        CHECK(striche == 29, "und %d Unterstrich-Zeilen (Soll 29)", striche);
        CHECK(glyphen == 11,
              "der Name \"Irons Diary\" hat %d Glyphen (Soll 11, Leerzeichen ist eine "
              "Glyphe @0x80028fb8)", glyphen);

        /* Platz 10 liegt auf Listenseite 1, Zeile 0: die Aufteilung p*10 + r */
        re15_files_reset();
        for (int i = 0; i < 11; i++) re15_files_add(0);
        liste_zaehlen(&namen, &striche, &glyphen, titel, je);
        CHECK(namen == 11 && je[0] == 10 && je[1] == 1 && je[2] == 0,
              "11 Plaetze belegt -> Seite 0: %d, Seite 1: %d, Seite 2: %d Namen "
              "(Soll 10 / 1 / 0: Zeile r der Seite p zeigt Platz p*10 + r)",
              je[0], je[1], je[2]);

        /* 24 Plaetze, der 25. faellt weg (sltiu v0,a1,0x18 @0x80069308) */
        re15_files_reset();
        int letzter = -1;
        for (int i = 0; i < 24; i++) letzter = re15_files_add(0);
        int voll = re15_files_add(0);
        liste_zaehlen(&namen, &striche, &glyphen, titel, je);
        CHECK(letzter == 23 && re15_files_count() == 24,
              "24 Plaetze: der 24. Eintrag bekommt Platz %d (Soll 23), belegt %d",
              letzter, re15_files_count());
        CHECK(voll == 0 && re15_files_count() == 24,
              "volle Liste: Anhaengen schreibt nichts und gibt %d zurueck (Soll 0, "
              "v0 = letztes sltiu @0x80069308)", voll);
        CHECK(namen == 24 && je[2] == 4 && striche == 6,
              "volle Liste: %d Namen, Seite 2 traegt %d, %d Unterstrich-Zeilen (Soll 24 / "
              "4 / 6 - die Plaetze 24..29 gibt es nicht)", namen, je[2], striche);
        re15_files_reset();
        CHECK(re15_files_count() == 0 && re15_files_get(0) == RE15_FILES_EMPTY &&
              re15_files_get(23) == RE15_FILES_EMPTY,
              "re15_files_reset: alle Plaetze 0xFF (RE2 @0x800682dc-f8)");
    }

    /* ---------------------------------------------------------------- B */
    printf("\n[B] Leser (item_state 3): zwei Dokumente, zwei Anzeigelisten\n");
    {
        const re15_file_doc_t *d = re15_files_doc(0);
        CHECK(d && d->item_id == 0x48 && d->bildsatz == 25 && d->max_page == 15 &&
              d->page_h == 144,
              "Dokument-Tabelle Eintrag 0: Item-Id 0x%02x, Bild-Satz %d, max_page %d, H %d "
              "(Soll 0x48 / 25 / 15 / 144)", d ? d->item_id : 0, d ? d->bildsatz : -1,
              d ? d->max_page : -1, d ? d->page_h : -1);
        /* Runde 34 Nacht (Spur E): 0x49..0x4C sind jetzt die Dokumente 1..4 (re15_files.c);
         * die Grenze der Tabelle liegt bei 0x48 + Anzahl. */
        CHECK(re15_files_doc_from_item(0x48) == 0 && re15_files_doc_from_item(0x47) == -1 &&
              re15_files_doc_from_item(0x48 + re15_files_doc_count()) == -1,
              "Item-Id -> Dokument: 0x48 -> 0, 0x47 -> keines, 0x%02X (Tabellenende) -> keines",
              0x48 + re15_files_doc_count());

        re15_inv_screen_t a = g_inv_screen, b = g_inv_screen, t = g_inv_screen;
        a.substate = b.substate = t.substate = 2;
        a.item_state = b.item_state = t.item_state = 3;
        a.file_reader_page = b.file_reader_page = t.file_reader_page = 0;
        a.file_bild = 1; a.file_bildsatz = 25; a.file_end = 16;       /* Irons Diary */
        b.file_bild = 1; b.file_bildsatz = 8;                          /* RE2 FILE08  */
        b.file_end = (uint8_t)(re2_files_doc[8].max_page + 1);
        t.file_bild = 0;                                               /* alter Textleser */
        int na = re15_inv_screen_build(&a, s_ops, RE15_INV_MAX_OPS);
        int nb = re15_inv_screen_build(&b, s_ops2, RE15_INV_MAX_OPS);
        int gleich = (na == nb) && memcmp(s_ops, s_ops2, (size_t)na * sizeof s_ops[0]) == 0;
        printf("  Irons Diary: %d Ops   FILE08: %d Ops\n", na, nb);
        CHECK(!gleich, "Irons Diary (16 Seiten) und FILE08 (%d Seiten) ergeben VERSCHIEDENE "
              "Anzeigelisten (vor dem Bau: bitgleich, der Leser kannte die Zeile nicht)",
              b.file_end);
        /* Nachbesserung (Gegenpruefung Runde 30): die Anzeigeliste unterscheidet sich nur
         * in der Fusszeile - WELCHES Bild gezeigt wird, entscheidet die Bild-Ebene. Also
         * auch dort pruefen: Bild-Satz/Seite je Dokument (re15_inv_file_bild_lage, RE2
         * Seitenlader 0x8006d444, Titel = Seite -1 @0x8006d484-98) und die gezeigten
         * Pixel der Titel-Textseite beider Saetze. */
        {
            int sa = -1, pa = -9, sb = -1, pb = -9, ta = 0, tb = 0;
            int ra = re15_inv_file_bild_lage(&a, &sa, &pa, &ta);
            int rb = re15_inv_file_bild_lage(&b, &sb, &pb, &tb);
            CHECK(ra == 1 && rb == 1 && sa == 25 && sb == 8 && pa == -1 && pb == -1,
                  "Bild-Ebene: Irons Diary zeigt Satz %d Seite %d, FILE08 Satz %d Seite %d "
                  "(Soll 25/-1 und 8/-1 = je die Titelseite)", sa, pa, sb, pb);
            int wa = 0, ha = 0, wb = 0, hb = 0, diff = 0, beide = 0;
            re15_re2doc_size(sa, pa, RE15_RE2DOC_PAGE, &wa, &ha);
            re15_re2doc_size(sb, pb, RE15_RE2DOC_PAGE, &wb, &hb);
            int w = wa < wb ? wa : wb, h = ha < hb ? ha : hb;
            for (int v = 0; v < h; v++)
                for (int u = 0; u < w; u++) {
                    uint8_t r1, g1, b1, r2, g2, b2;
                    int v1 = re15_re2doc_pixel(sa, pa, RE15_RE2DOC_PAGE, u, v, &r1, &g1, &b1);
                    int v2 = re15_re2doc_pixel(sb, pb, RE15_RE2DOC_PAGE, u, v, &r2, &g2, &b2);
                    if (v1 || v2) beide++;
                    if (v1 != v2 || (v1 && (r1 != r2 || g1 != g2 || b1 != b2))) diff++;
                }
            printf("  Titel-Textseite: Satz %d %dx%d, Satz %d %dx%d, verschiedene Pixel %d "
                   "von %d sichtbaren\n", sa, wa, ha, sb, wb, hb, diff, beide);
            CHECK(w > 0 && h > 0 && diff > 0,
                  "Irons Diary und FILE08 zeigen VERSCHIEDENE Titelseiten: %d Pixel "
                  "verschieden (Soll > 0)", diff);
        }
        int text = 0, fuss = 0, pfeil = 0, sonst = 0;
        for (int i = 0; i < na; i++) {
            const re15_inv_op_t *o = &s_ops[i];
            if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_FONT4 &&
                o->clut == RE15_INV_CLUT_TEXROW0) text++;
            else if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_FONT4 &&
                     o->clut == RE15_INV_CLUT_TEXROW4) fuss++;
            else if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_RE2ST0 &&
                     o->u == 42 && o->x == 282 && o->y == 110) pfeil++;
                     /* Nachschliff pfeil: RE2s rechter Pfeil (@0x80072628-70) statt
                      * RE1.5s TEX4 16x16 - Riegel test_r30_pfeil.c */
            else sonst++;
        }
        printf("  Irons Diary Seite 0: %d Textglyphen, %d Fusszeilen-Glyphen, %d Pfeile, "
               "%d sonstige Ops\n", text, fuss, pfeil, sonst);
        CHECK(text == 0, "Bild-Dokument: KEIN Zeichenstrom (%d Textglyphen, Soll 0)", text);
        CHECK(fuss == 4, "Fusszeile \"1/16\" = %d Glyphen (Soll 4; RE1.5 0x800c7744)", fuss);
        CHECK(pfeil == 1, "Seite 0: nur der rechte Pfeil (%d, Soll 1; RE2 @0x800726e8-f0)",
              pfeil);
        CHECK(sonst == 0,
              "sonst NICHTS auf dem Schirm: %d weitere Ops (Soll 0 - kein Rahmen, keine "
              "Tafeln, kein Hintergrundblatt; RE2 Grund schwarz @0x80071d8c-94)", sonst);
        /* der alte Textleser bleibt als Rueckfall unveraendert. Die Vergleichszahl 394
         * stammt aus der Sonde VOR dem Bau (master d98e9639, [B] Liste 0/Zeile 0) und
         * wurde dort auf Leserseite 1 gemessen (a.file_reader_page = 1) - deshalb hier
         * dieselbe Seite; auf Seite 0 (Titelkarte "Operation Report") sind es weniger. */
        t.file_reader_page = 1;
        int nt = re15_inv_screen_build(&t, s_ops2, RE15_INV_MAX_OPS);
        CHECK(nt == 394, "der Textleser ohne Bild-Dokument ist unveraendert: %d Ops "
              "(Soll 394 auf Seite 1, gemessen vor dem Bau)", nt);
        /* Zustand 8 (Meldung steht): gar keine Op */
        a.item_state = 8;
        CHECK(re15_inv_screen_build(&a, s_ops, RE15_INV_MAX_OPS) == 0,
              "Zustand 8 (Meldung): keine Op - RE2 zeichnet Pfeile nur in den Zustaenden "
              "0 und 1 (`sltiu v0,v0,0x2` @0x80072690) und hat keine Fusszeile");
        /* Blaettern (Zustand 6): Fusszeile ja, Pfeile nein */
        a.item_state = 6; a.file_text_x = 0xc;
        na = re15_inv_screen_build(&a, s_ops, RE15_INV_MAX_OPS);
        pfeil = 0;
        for (int i = 0; i < na; i++)
            if (s_ops[i].kind == RE15_INV_OP_SPRT &&
                (s_ops[i].page == RE15_INV_PAGE_TEX4 || s_ops[i].page == RE15_INV_PAGE_RE2ST0))
                pfeil++;
        CHECK(na == 4 && pfeil == 0,
              "Blaettern: %d Ops, davon %d Pfeile (Soll 4 / 0)", na, pfeil);
    }

    /* ---------------------------------------------------------------- C */
    printf("\n[C] Bild-Ebene: Durchsicht und Seitenzahl\n");
    {
        static const int saetze[2] = { 8, 25 };
        for (int s = 0; s < 2; s++) {
            int doc = saetze[s];
            char pfad[512];
            int pw = 0, ph = 0, H = re15_re2doc_page_height(doc, -1);
            re15_re2doc_size(doc, -1, RE15_RE2DOC_PAPER, &pw, &ph);
            long n = 0;
            snprintf(pfad, sizeof pfad, RE15_ASSET_RE2_DIR "/FILES/FILE%02d_title_paper.TIM", doc);
            uint8_t *tim = datei(pfad, &n);
            long farbe0_index_ungleich0 = 0, port_sichtbar_schwarz = 0, band = 0, sichtbar = 0;
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
                        int vis = re15_re2doc_pixel(doc, -1, RE15_RE2DOC_PAPER, u, v, &r, &g, &b);
                        if (vis) sichtbar++;
                        if (vis && c == 0) port_sichtbar_schwarz++;
                    }
            }
            free(tim);
            printf("  FILE%02d Papier %dx%d, H=%d, Band %ld Texel, sichtbar %ld\n",
                   doc, pw, ph, H, band, sichtbar);
            CHECK(tim != NULL && band == 14336,
                  "FILE%02d: Papierband gelesen (%ld Texel, Soll 14336 = 128 x 112)", doc, band);
            CHECK(farbe0_index_ungleich0 == 7410,
                  "FILE%02d: %ld Texel mit CLUT-Farbe 0x0000 und Index != 0 (Soll 7410 - "
                  "die Messung ist nicht leer)", doc, farbe0_index_ungleich0);
            CHECK(port_sichtbar_schwarz == 0,
                  "FILE%02d: davon zeichnet der Port DECKEND: %ld (Soll 0; vor dem Bau 7410)",
                  doc, port_sichtbar_schwarz);
            CHECK(sichtbar > 1000,
                  "FILE%02d: das Buch selbst bleibt sichtbar (%ld Texel) - die Regel "
                  "loescht nicht alles", doc, sichtbar);
        }
        /* Textseiten des Irons Diary: kein Texel mit Farbe 0x0000 wird gezeichnet */
        long schwarz = 0, sicht = 0;
        for (int pg = -1; pg <= 15; pg++) {
            char pfad[512];
            long n = 0;
            if (pg < 0) snprintf(pfad, sizeof pfad, RE15_ASSET_RE2_DIR "/FILES/FILE25_title_page.TIM");
            else snprintf(pfad, sizeof pfad, RE15_ASSET_RE2_DIR "/FILES/FILE25_p%02d_page.TIM", pg);
            uint8_t *tim = datei(pfad, &n);
            if (!tim) { schwarz = -1; break; }
            uint32_t clen = rd32(tim + 8);
            const uint8_t *clut = tim + 20;
            const uint8_t *img = tim + 8 + clen + 12;
            for (int v = 0; v < 144; v++)
                for (int u = 0; u < 256; u++) {
                    uint8_t by = img[(size_t)v * 128 + (size_t)(u >> 1)];
                    unsigned idx = (u & 1) ? (unsigned)(by >> 4) : (unsigned)(by & 15);
                    uint8_t r, g, b;
                    int vis = re15_re2doc_pixel(25, pg, RE15_RE2DOC_PAGE, u, v, &r, &g, &b);
                    if (vis) sicht++;
                    if (vis && rd16(clut + idx * 2) == 0) schwarz++;
                }
            free(tim);
        }
        CHECK(schwarz == 0 && sicht > 10000,
              "Irons Diary, Titel + 16 Seitendateien: %ld deckende 0x0000-Texel (Soll 0) "
              "bei %ld sichtbaren", schwarz, sicht);

        const re15_file_doc_t *d = re15_files_doc(0);
        int seiten = d ? d->max_page + 1 : -1;
        CHECK(seiten == 16, "Seitenzahl Irons Diary = max_page + 1 = %d (Soll 16)", seiten);
        CHECK(re15_re2doc_size(25, 15, RE15_RE2DOC_PAGE, NULL, NULL) == 1 &&
              re15_re2doc_size(25, 16, RE15_RE2DOC_PAGE, NULL, NULL) == 0,
              "FILE25_p15_page.TIM ist da, FILE25_p16 nicht - max_page 15 deckt sich mit "
              "den Dateien");
        CHECK(re15_files_bildsatz_max_page(8) == (int)re2_files_doc[8].max_page &&
              re2_files_doc[8].max_page == 4,
              "FILE08: max_page %d aus RE2s Record @0x800AA144 (Soll 4; der Port zaehlte "
              "vor dem Bau 6 Seiten statt 5)", (int)re2_files_doc[8].max_page);
    }

    /* ---------------------------------------------------------------- D */
    printf("\n[D] die KOPIERTEN Dateien FILE25 unter %s/FILES\n", RE15_ASSET_RE2_DIR);
    {
        int w = 0, h = 0, pw = 0, ph = 0;
        int t = re15_re2doc_size(25, -1, RE15_RE2DOC_PAGE, &w, &h);
        int p = re15_re2doc_size(25, -1, RE15_RE2DOC_PAPER, &pw, &ph);
        CHECK(t && p, "Titelseite und Papier liegen vor");
        CHECK(w == 256 && h == 144 && pw == 128 && ph == 256,
              "Titelseite %dx%d, Papier %dx%d (Soll 256x144 / 128x256)", w, h, pw, ph);
        int dateien = re15_re2doc_page_count(25);
        CHECK(dateien == 17, "Seitendateien: %d (Soll 17 = Titel + p00..p15)", dateien);

        int ungleich = 0;
        for (int v = 0; v < h; v++)
            for (int u = 0; u < w; u++) {
                uint8_t r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
                int s1 = re15_re2doc_pixel(25, -1, RE15_RE2DOC_PAGE, u, v, &r1, &g1, &b1);
                int s2 = re15_re2doc_pixel(25, 0, RE15_RE2DOC_PAGE, u, v, &r2, &g2, &b2);
                if (s1 != s2 || (s1 && (r1 != r2 || g1 != g2 || b1 != b2))) ungleich++;
            }
        CHECK(ungleich == 0, "title_page gegen p00: %d abweichende Pixel (Soll 0)", ungleich);

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
            for (int pg = -1; pg <= 15; pg++) {
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
        CHECK(vor != NULL && fremd == 0 && sichtbar > 10000,
              "sichtbare Pixel ueber alle Seiten: %ld, davon ausserhalb der "
              "FILE08-CLUT[1..6,8]: %ld (Soll 0)", sichtbar, fremd);

        long n1 = 0, n2 = 0;
        uint8_t *a = datei(RE15_ASSET_RE2_DIR "/FILES/FILE25_title_paper.TIM", &n1);
        uint8_t *b = datei(RE15_ASSET_RE2_DIR "/FILES/FILE08_title_paper.TIM", &n2);
        CHECK(a && b && n1 == n2 && n1 == 33312 && memcmp(a, b, (size_t)n1) == 0,
              "FILE25_title_paper.TIM %ld B byte-gleich FILE08_title_paper.TIM %ld B", n1, n2);
        long n3 = 0, n4 = 0;
        uint8_t *c = datei(RE15_ASSET_RE2_DIR "/FILES/FILE25_p01_page.TIM", &n3);
        uint8_t *d = datei(RE15_ASSET_RE2_DIR "/FILES/FILE08_p01_page.TIM", &n4);
        CHECK(c && d && n3 == n4 && n3 == 18496 && memcmp(c, d, 64) == 0,
              "FILE25_p01_page.TIM %ld B gegen FILE08_p01_page.TIM %ld B: Kopf + CLUT + "
              "Bildkopf (64 Byte) byte-gleich", n3, n4);
        free(a); free(b); free(c); free(d);
    }

    /* ---------------------------------------------------------------- E */
    printf("\n[E] Seite -> Datei, x-Lage der Textseite\n");
    {
        re15_inv_screen_t st = g_inv_screen;
        int set = -9, pg = -9, tx = -9;
        st.substate = 2; st.item_state = 3;
        st.file_bild = 1; st.file_bildsatz = 25; st.file_end = 16;
        st.file_reader_page = 0;
        int r0 = re15_inv_file_bild_lage(&st, &set, &pg, &tx);
        CHECK(r0 == 1 && set == 25 && pg == -1 && tx == 25,
              "Seite 0 -> Titelseite (Datei %d, Soll -1), Satz %d, x = %d (Soll 25, RE2 "
              "`addiu v0,zero,25` @0x80076170)", pg, set, tx);
        st.file_reader_page = 5;
        re15_inv_file_bild_lage(&st, &set, &pg, &tx);
        CHECK(pg == 5, "Seite 5 -> p05 (Datei %d; RE2 Slot erster+1+Seite @0x8006d498)", pg);
        st.file_reader_page = 15;
        re15_inv_file_bild_lage(&st, &set, &pg, &tx);
        CHECK(pg == 15, "Seite 15 -> p15 (Datei %d)", pg);
        st.file_reader_page = 16;                       /* Ende-Stellung */
        re15_inv_file_bild_lage(&st, &set, &pg, &tx);
        CHECK(pg == 15, "Ende-Stellung (Seite 16) zeigt weiter die letzte Seite p15 "
              "(Datei %d; Klemme @0x800c7628-34)", pg);
        st.item_state = 7; st.file_text_x = 320; st.file_reader_page = 0;
        re15_inv_file_bild_lage(&st, &set, &pg, &tx);
        CHECK(tx == 305, "Oeffnen: die Textseite startet bei x = %d (Soll 305 = 25 + "
              "(320 - 0x28); RE2 startet bei 312 @0x80071db0)", tx);
        st.file_text_x = 40;
        re15_inv_file_bild_lage(&st, &set, &pg, &tx);
        CHECK(tx == 25, "Ende der Fahrt: x = %d (Soll 25 - RE1.5s Ruhelage 0x28 faellt auf "
              "RE2s 25)", tx);
        st.item_state = 8;
        CHECK(re15_inv_file_bild_lage(&st, &set, &pg, &tx) == 1 && tx == 25,
              "unter der Meldung (Zustand 8) steht die Seite weiter, x = %d", tx);
        st.item_state = 1;
        CHECK(re15_inv_file_bild_lage(&st, &set, &pg, &tx) == 0,
              "in der Liste (Zustand 1) wird KEIN Dokument gezeichnet");
        st.item_state = 3; st.file_bild = 0;
        CHECK(re15_inv_file_bild_lage(&st, &set, &pg, &tx) == 0,
              "ohne Bild-Dokument wird nichts gezeichnet");
    }

    if (fails) { printf("\nR30 IRONS DIARY DOKUMENT: FAIL (%d)\n", fails); return 1; }
    printf("\nR30 IRONS DIARY DOKUMENT: alle Riegel halten\n");
    return 0;
}
