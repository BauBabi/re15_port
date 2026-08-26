/* re15_re2doc.h — die BILD-EBENE des FILE-Schirms (RE2-Dokumentseiten).
 *
 * ⛔ WAS DAS IST UND WAS NICHT
 * RE1.5s FILE-Schirm ist REINER TEXT: der Leser druckt eine Zeichenkette aus dem
 * 7-Seiten-Blob @0x800ccd34 (DEBUG.BIN) und zeichnet Fusszeile plus Pfeile
 * (re15_inv_screen.c, emit_file_reader). Eine Bild-Ebene existiert dort nicht.
 * RE2 dagegen zeichnet je Seite ZWEI Sprites. Diese Datei bringt genau diese zwei
 * Sprites in den Port — als PORT-ERWEITERUNG, nicht als byte-true Nachbau von RE1.5.
 * Ohne zugewiesenes Dokument aendert sich am FILE-Schirm NICHTS.
 *
 * ⛔ DIE GEOMETRIE IST BYTE-BELEGT (RE2 PSX.EXE, FILE-Leser FUN_80075fd0):
 *   Seitenhoehe H:
 *     8007603c  lhu   s3,-24250(at)   ; Dokument-Record +2 = y_off  (@0x800AA144 + doc*4)
 *     80076040  addiu s1,zero,256
 *     80076044  subu  s5,s1,s3        ; s5 = H = 256 - y_off
 *   Sprite 1 — TEXTSEITE (4bpp):
 *     8007604c  addiu a1,zero,490     ; GetClut(0, 490)
 *     80076050  addiu v0,zero,102     ; 0x66 = SPRT
 *     80076068  sb    zero,-2(s0)     ; u = 0
 *     8007606c  sb    zero,-1(s0)     ; v = 0
 *     80076070  sh    s1,2(s0)        ; w = 256
 *     80076078  sh    s5,4(s0)        ; h = H
 *   Sprite 2 — ILLUSTRATION (8bpp):
 *     8007609c  addiu s1,zero,128
 *     800760ac  addiu a1,zero,489     ; GetClut(0, 489)
 *     800760b8  subu  v0,zero,s3      ; v0 = -y_off; als BYTE geschrieben ist das
 *     800760d0  sb    v0,-1(s0)       ; v = (-y_off) & 0xFF = 256 - y_off = H
 *     800760d4  sh    s1,2(s0)        ; w = 128
 *     800760dc  sh    s3,4(s0)        ; h = y_off = 256 - H
 *   Also: Text oben auf den Zeilen 0..H-1 ueber die volle Breite 256, darunter die
 *   128 breite Illustration auf den Zeilen H..255 — beide in eine 256x256-Flaeche.
 *
 * DIE DATEN liegen als Einzel-TIMs in shared_assets/RE2/FILES/ (geschnitten von
 * re15_port/tools/re2_files_cut.py nach den drei EXE-Tabellen @0x800A9AD0 /
 * @0x800AA144 / @0x800A94B4). Namen:
 *     FILE%02d_title_paper.TIM   die Illustration des Dokuments (8bpp, 128x256)
 *     FILE%02d_title_page.TIM    die Titelseite               (4bpp, 256xH)
 *     FILE%02d_p%02d_page.TIM    Folgeseite p                 (4bpp, 256xH)
 * H wird NICHT aus einer Tabelle geholt, sondern aus dem TIM-Kopf gelesen — damit kann
 * es nicht von den Bilddaten abweichen, und eigene Seiten duerfen eine eigene Hoehe haben.
 *
 * FUER EIGENE DOKUMENTE: dieselben Namen, dasselbe Format. Textseite 4bpp 256 breit,
 * Illustration 8bpp 128x256 mit dem Motiv UNTEN buendig (die oberen 256-H Zeilen bleiben
 * Index 0 = durchsichtig). Vorlagen als PNG: re15_port/tools/re2_files_png.py.
 */
#ifndef RE15_RE2DOC_H
#define RE15_RE2DOC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Die Seiten-Rolle. Die Titelseite ist die Seite mit der Illustration. */
typedef enum {
    RE15_RE2DOC_PAGE  = 0,   /* Textseite, 4bpp, 256 x H            */
    RE15_RE2DOC_PAPER = 1    /* Illustration, 8bpp, 128 x 256       */
} re15_re2doc_kind_t;

/* Wurzelverzeichnis der Dokumentdateien setzen (…/RE2/FILES). Ohne das sucht der Lader
 * ueber den normalen Asset-Pfad. Darf mehrfach gerufen werden. */
void re15_re2doc_set_root(const char *dir);

/* Groesse eines Teilbilds. page < 0 = Titelseite. Rueckgabe 1 = vorhanden. */
int re15_re2doc_size(int doc, int page, re15_re2doc_kind_t kind, int *w, int *h);

/* Ein Pixel. Rueckgabe 0 = durchsichtig oder ausserhalb (CLUT-Index 0 zeichnet die GPU
 * nicht), 1 = sichtbar, dann sind r/g/b gesetzt. */
int re15_re2doc_pixel(int doc, int page, re15_re2doc_kind_t kind,
                      int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);

/* Seitenhoehe H des Dokuments (aus dem TIM-Kopf der Seite). 0 = unbekannt. */
int re15_re2doc_page_height(int doc, int page);

/* Wie viele Textseiten hat das Dokument? Zaehlt die vorhandenen Dateien. */
int re15_re2doc_page_count(int doc);

/* Welches Dokument soll der FILE-Leser zeigen? -1 = keines (dann bleibt der Schirm
 * unveraendert beim byte-true Textleser von RE1.5). */
void re15_re2doc_select(int doc);
int  re15_re2doc_selected(void);

#ifdef __cplusplus
}
#endif

#endif /* RE15_RE2DOC_H */
