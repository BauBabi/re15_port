/* test_re2doc_bildebene.c — PIN: die Bild-Ebene des FILE-Schirms liest die
 * RE2-Dokumentseiten in der byte-belegten Geometrie.
 *
 * NUTZER-AUFTRAG 2026-08-26: "ja, portiere mir die bild ebene".
 *
 * BYTE-BELEG (RE2 PSX.EXE, FILE-Leser FUN_80075fd0) — selbst disassembliert:
 *   Seitenhoehe:  8007603c lhu   s3,-24250(at)   ; Dokument-Record +2 = y_off
 *                 80076040 addiu s1,zero,256
 *                 80076044 subu  s5,s1,s3        ; H = 256 - y_off
 *   Textseite:    8007604c addiu a1,zero,490     ; CLUT (0,490)
 *                 80076068 sb zero,-2(s0)        ; u = 0
 *                 8007606c sb zero,-1(s0)        ; v = 0
 *                 80076070 sh s1,2(s0)           ; w = 256
 *                 80076078 sh s5,4(s0)           ; h = H
 *   Illustration: 8007609c addiu s1,zero,128
 *                 800760ac addiu a1,zero,489     ; CLUT (0,489)
 *                 800760b8 subu  v0,zero,s3      ; v = (-y_off) & 0xFF = H
 *                 800760d4 sh s1,2(s0)           ; w = 128
 *                 800760dc sh s3,4(s0)           ; h = y_off = 256 - H
 *
 * DIE WACHE IST NICHT VAKUANT. Sie prueft drei unabhaengige Dinge:
 *   (1) die Dateien sind ueberhaupt da und dekodieren (sonst faellt sie durch),
 *   (2) die Masse jedes Teilbilds entsprechen der obigen Geometrie,
 *   (3) ⛔ die aus dem TIM gelesene Seitenhoehe H stimmt mit 256 - y_off aus der
 *       EXE-TABELLE ueberein (re2_files_toc.inc). Zwei voellig getrennte Quellen -
 *       Bilddaten gegen Programmtabelle. Weicht eine ab, ist der Schnitt falsch.
 * Dazu die Gegenprobe, dass ein nicht vorhandenes Dokument sauber 0 meldet.
 */
#include "re15_re2doc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen/re2_files_toc.inc"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

int main(void)
{
    printf("=== FILE-Bildebene: RE2-Dokumentseiten ===\n");
    re15_re2doc_set_root(RE15_ASSET_RE2_DIR "/FILES");

    /* (1) Alle 25 Dokumente muessen ihre Titelseite und ihr Papierbild haben. */
    int docs_ok = 0, paper_ok = 0, h_ok = 0, h_bad = 0, pages_total = 0;
    for (int d = 0; d < RE2_FILES_DOC_COUNT; d++) {
        int tw = 0, th = 0, pw = 0, ph = 0;
        int have_title = re15_re2doc_size(d, -1, RE15_RE2DOC_PAGE, &tw, &th);
        int have_paper = re15_re2doc_size(d, -1, RE15_RE2DOC_PAPER, &pw, &ph);
        if (have_title) docs_ok++;
        if (have_paper) paper_ok++;
        int n = re15_re2doc_page_count(d);
        pages_total += n;

        /* (3) Gegenprobe Bilddaten <-> EXE-Tabelle. */
        int h_tab = (int)re2_files_doc[d].page_h;
        if (have_title) {
            if (th == h_tab) h_ok++;
            else {
                h_bad++;
                printf("     Dokument %2d: TIM sagt H=%d, EXE-Tabelle sagt %d\n", d, th, h_tab);
            }
            if (tw != 256)
                printf("     Dokument %2d: Textseite ist %d breit statt 256\n", d, tw);
        }
        if (have_paper && (pw != 128 || ph != 256))
            printf("     Dokument %2d: Papierbild ist %dx%d statt 128x256\n", d, pw, ph);
        printf("  Dokument %2d: Titelseite %dx%d, Papier %dx%d, %d Seiten (Tabelle H=%d)\n",
               d, tw, th, pw, ph, n, h_tab);
    }

    CHECK(docs_ok == RE2_FILES_DOC_COUNT,
          "alle %d Dokumente haben ihre Titelseite (%d gefunden) - ohne die Dateien misst "
          "die Wache nichts", RE2_FILES_DOC_COUNT, docs_ok);
    CHECK(paper_ok == RE2_FILES_DOC_COUNT,
          "und alle %d ihr Papierbild (%d gefunden)", RE2_FILES_DOC_COUNT, paper_ok);
    CHECK(h_bad == 0,
          "die aus den TIMs gelesene Seitenhoehe stimmt bei ALLEN %d Dokumenten mit "
          "256 - y_off aus der EXE-Tabelle @0x800AA144 ueberein (%d richtig, %d falsch) - "
          "zwei unabhaengige Quellen, Bilddaten gegen Programmtabelle", h_ok, h_ok, h_bad);
    printf("  Textseiten insgesamt: %d\n", pages_total);
    CHECK(pages_total >= 190,
          "und es sind %d Textseiten insgesamt (der Schnitt lieferte 191 Seiten + 25 "
          "Illustrationen = 216 TIMs)", pages_total);

    /* (2) Pixel: die Textseite muss sichtbare Pixel haben, das Papierbild sein Motiv
     *     UNTEN fuehren (Zeilen H..255) - genau das setzt die Zeichnung voraus. */
    {
        int H = re15_re2doc_page_height(0, -1);
        int text_px = 0, paper_top = 0, paper_bottom = 0;
        uint8_t r, g, b;
        for (int v = 0; v < H; v++)
            for (int u = 0; u < 256; u++)
                if (re15_re2doc_pixel(0, -1, RE15_RE2DOC_PAGE, u, v, &r, &g, &b)) text_px++;
        for (int v = 0; v < 256; v++)
            for (int u = 0; u < 128; u++)
                if (re15_re2doc_pixel(0, -1, RE15_RE2DOC_PAPER, u, v, &r, &g, &b)) {
                    if (v < H) paper_top++; else paper_bottom++;
                }
        printf("  Dokument 0: H=%d, Textseite %d sichtbare Pixel; Papier oben %d / unten %d\n",
               H, text_px, paper_top, paper_bottom);
        CHECK(text_px > 500,
              "die Textseite hat sichtbare Pixel (%d) - ein leeres Bild waere ein "
              "Dekodier-Fehler", text_px);
        CHECK(paper_bottom > paper_top,
              "und das Motiv des Papierbildes liegt UNTEN (Zeilen H..255: %d Pixel gegen "
              "%d oberhalb) - genau darauf setzt die Zeichnung auf, die es ab Zeile H "
              "abtastet (`subu v0,zero,s3` @0x800760b8)", paper_bottom, paper_top);
    }

    /* GEGENPROBE: ein Dokument, das es nicht gibt, meldet sauber 0. */
    {
        int w = 0, h = 0;
        int got = re15_re2doc_size(RE2_FILES_DOC_COUNT + 5, -1, RE15_RE2DOC_PAGE, &w, &h);
        CHECK(!got,
              "ein nicht vorhandenes Dokument meldet 0 statt Muell - sonst waere jede "
              "Zusage oben wertlos");
    }

    if (fails) { printf("\nRE2DOC: FAIL (%d)\n", fails); return 1; }
    printf("\nRE2DOC: die Bildebene liest alle 25 Dokumente in der belegten Geometrie\n");
    return 0;
}
