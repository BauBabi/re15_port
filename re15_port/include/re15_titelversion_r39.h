#ifndef RE15_TITELVERSION_R39_H
#define RE15_TITELVERSION_R39_H
/*
 * Runde 39 Spur T "titelversion" — Version im Titelbild statt "Sample / Ver.2025.01.25".
 * Dossier analysis/befunde_runde39/T_titelversion.md.
 *
 * Nutzer (AUFTRAG.md, Nachtrag 14, woertlich): "bei dieser und zukuenftig neuen Versionen passe mir bitte die Version im
 * Titelscreen an. Entferne das bestehende."  -> NUTZER-VORGABE; kein Original-Verhalten (die Beschriftung stammt nicht
 * aus dem RE1.5-Code, sie ist in DATA/TITLEU.TIM eingebrannt).
 *
 * BELEGE (Datei-Messung DATA/TITLEU.TIM, 320x240 16 bpp, Pixel (x,y) bei Datei-Byte 0x14 + 2*(y*320+x), Dossier 1.1/1.2):
 *  - Beschriftung = die grauen (r=g=b) 8-zusammenhaengenden Komponenten im Kasten x 188..295, y 4..35, deren Maximum
 *    >= 20 ist (Glyphenkerne 25): 929 Pixel, bbox x198..283 y11..33. Die einzigen anderen Komponenten im Kasten sind
 *    Staub der Hintergrund-Ader (Maximum 1 bzw. 2) und bleiben. Kasten-Rand: 0 Nicht-Schwarz-Pixel.
 *  - Hintergrund darunter: der 1-Pixel-Ring um die Maske (608 Pixel) ist zu 100 % 0x8000 -> Entfernen = 0x8000.
 *  - Schrift der Zeile "Ver.2025.01.25": Pixelschrift, Raster 1,12 px, Quadrat 1,52 px, oberste Schrift-Zeile ab
 *    y 24,88, Wert = round(25 x Deckung), Schriftfarbe (25,25,25), STP 0. Tintenkante rechts der Zeile: 283,60 px
 *    ("5" bei x277: Tinte [277,60; 283,60)). Abstand zwischen den Tinten zweier Zeichen 0,72 px (1 Raster - 0,40),
 *    zwischen "Ver." und "2025" 2,74 px (Tinte "." bis 224,10, "2" ab 226,84).
 * Glyphen: gen/titelversion_r39_glyphen.inc (Generator tools/r39_titelversion/glyphen_bauen.py, Herkunft je Glyphe).
 *
 * VERSION: RE15_VERSION (Compile-Definition nur fuer titelversion_r39.c, cmake/re15_version_r39.cmake): Release-Bau
 * "vX.Y.Z" (release/release_alles.sh -> build_*.sh -> CMake/Gradle), lokaler Bau "vX.Y.Z dev" (letzter Tag + " dev").
 * Anzeige: "Ver.X.Y.Z" bzw. "Ver.X.Y.Z dev" — alles hinter X.Y.Z (" dev", "-5-gabc") wird als " dev" gezeigt.
 */
#include <stdint.h>

#define RE15_R39T_KASTEN_X0     188     /* Suchkasten der alten Beschriftung, einschliesslich */
#define RE15_R39T_KASTEN_X1     295
#define RE15_R39T_KASTEN_Y0     4
#define RE15_R39T_KASTEN_Y1     35
#define RE15_R39T_KERN          20      /* Komponente gehoert zur Beschriftung, wenn ihr Maximum >= 20 ist (Kerne 25) */
#define RE15_R39T_HINTERGRUND   0x8000u /* Ring um die Maske: 608 von 608 Pixeln 0x8000 */
#define RE15_R39T_ZEILE0        24      /* Kacheln: Bildzeilen 24..33 */
#define RE15_R39T_ZEILEN        10
#define RE15_R39T_RECHTS25      7090    /* Tintenkante rechts 283,60 px in 1/25 px */
#define RE15_R39T_ABSTAND25     18      /* 0,72 px zwischen den Tinten zweier Zeichen */
#define RE15_R39T_WORT25        69      /* 2,74 px: "Ver." -> Versionsnummer und vor " dev" */
#define RE15_R39T_LINKS         RE15_R39T_KASTEN_X0   /* keine Kachel links davon (sonst wird nichts gezeichnet) */
#define RE15_R39T_TEXT_MAX      48

enum { RE15_R39T_AUSGESCHNITTEN = 1, RE15_R39T_ERGAENZT = 2 };

typedef struct {
    char           zeichen;
    uint8_t        breite;     /* Kachelspalten */
    uint16_t       tinte_l;    /* Tintenkanten in 1/25 px relativ zur ersten Kachelspalte */
    uint16_t       tinte_r;
    uint8_t        quelle;     /* RE15_R39T_AUSGESCHNITTEN / RE15_R39T_ERGAENZT */
    const uint8_t *werte;      /* breite x RE15_R39T_ZEILEN, Werte 0..25 (5 bit, Grau) */
} re15_r39t_glyphe_t;

/* Die einkompilierte Version (RE15_VERSION), nie NULL. */
const char *re15_titelversion_r39_version(void);

/* Anzeigetext aus einer Version: "v0.8.27" -> "Ver.0.8.27", "v0.8.26 dev"/"v0.8.26-5-gabc" -> "Ver.0.8.26 dev",
 * ohne Ziffernfolge -> "Ver.dev". Rueckgabe: Laenge, -1 bei zu kleinem Puffer. */
int re15_titelversion_r39_text(const char *version, char *out, int n);

/* Glyphe eines Zeichens, NULL wenn die Tabelle es nicht hat. */
const re15_r39t_glyphe_t *re15_titelversion_r39_glyphe(char c);
int re15_titelversion_r39_glyphen_zahl(void);
const re15_r39t_glyphe_t *re15_titelversion_r39_glyphe_nr(int i);

/* Layout eines Anzeigetextes ("Ver.0.8.27"), rechtsbuendig an der Tintenkante 283,60: je Zeichen (ausser Leerzeichen)
 * die erste Kachelspalte. Rueckgabe: Zahl der Kacheln, -1 bei unbekanntem Zeichen / zu vielen Zeichen. */
int re15_titelversion_r39_layout(const char *anzeige, char *zeichen, int *spalte, int max);

/* Maske der alten Beschriftung (siehe BELEGE). maske = w*h Bytes (darf NULL sein). Rueckgabe: Zahl der Pixel. */
int re15_titelversion_r39_maske(const uint16_t *px, int w, int h, uint8_t *maske);

/* Titelbild umschreiben (im Speicher, TITLEU.TIM bleibt bytegleich): alte Beschriftung -> 0x8000, dann
 * "Ver." + Version zeichnen. Rueckgabe: Zahl der entfernten Pixel; -1 bei falschem Bild (nicht 320x240). */
int re15_titelversion_r39_anwenden(uint16_t *px, int w, int h, const char *version);

/* Haken fuer render_pc.c (Titelbild-Umwandlung): Kopie von px mit re15_titelversion_r39_anwenden(..., einkompilierte
 * Version). Rueckgabe: die Kopie (gueltig bis zum naechsten Aufruf) oder px selbst, wenn das Bild nicht 320x240 ist. */
const uint16_t *re15_titelversion_r39_bild(const uint16_t *px, int w, int h);

#endif /* RE15_TITELVERSION_R39_H */
