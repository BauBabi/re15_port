/*
 * re15_montage_fx.h — Praesentations-Schicht der Standbild-Montage (Pre-Intro ROOM1240).
 * Mechanismus + alle Raten aus RE2s OPENING.BIN (Element-Renderer 0x801c1a0c);
 * Ableitung und @0x…-Belege: analysis/preintro_re2/re2-bewegung.md.
 * RE15_MONTAGE_STOCK=1 = byte-true RE1.5-Hartschnitt (Schicht aus).
 *
 * ⛔ ZWEI-EBENEN-MODELL (Korrektur 2026-08-30, Nutzer: "es muss bis in das naechste
 * Standbild zoomen. Bei dir wird das Logo ploetzlich wieder gross."):
 * RE2s Element-Zustand ist ein BITFELD — 71 = einblenden + Zoom (0x47),
 * 75 = AUSBLENDEN + Zoom (0x4B). Das Bewegungs-Bit bleibt beim Umschalten von
 * Ein- auf Ausblenden GESETZT, und die Phasen-Timeline blendet das naechste
 * Standbild ein, WAEHREND das Logo noch zoomt (Phase 2 t=905 {10}->71 einblenden,
 * Phase 3 t=94 {11..14}->7 = naechstes Bild da, erst t=204 {10}->75 ausblenden).
 * ⇒ Jede Ebene traegt ihre EIGENE Bewegung und fuehrt sie ueber den Bildwechsel
 * hinweg fort. Der Zoom wird beim Cut NICHT zurueckgesetzt.
 */
#ifndef RE15_MONTAGE_FX_H
#define RE15_MONTAGE_FX_H

#include <stdint.h>

#define RE15_MONTAGE_CUTS 9      /* ROOM1240: 9 Standbilder (Cut 0..8) */

enum {
    RE15_MFX_NONE = 0,   /* nur Ein-/Ausblenden (Standbild)                      */
    RE15_MFX_PAN,        /* + vertikales Wandern (1 px je 11 Frames, RE2 Bit4)   */
    RE15_MFX_ZOOM_IN,    /* + zentrierter Zoom hinein (2 px je 4 Frames, Bit5)   */
    RE15_MFX_ZOOM_OUT    /* + zentrierter Zoom heraus (RE2 Bit6)                 */
};

void re15_montage_fx_reset(void);
void re15_montage_fx_set_active(int on);   /* Aufrufer gated auf den Montage-Raum */
int  re15_montage_fx_active(void);
void re15_montage_fx_on_cut(int cut, int had_previous);
void re15_montage_fx_tick(void);

int  re15_montage_fx_level_new(void);      /* 0..0x60 Helligkeit aktuelles Bild */
int  re15_montage_fx_level_prev(void);     /* 0..0x60 Helligkeit voriges Bild (Kreuzblende) */

/* Bewegung der AKTUELLEN Ebene */
int  re15_montage_fx_pan_y(void);
int  re15_montage_fx_zoom_px(void);
int  re15_montage_fx_base_zoom(void);
/* Bewegung der AUSBLENDENDEN Ebene — laeuft eigenstaendig WEITER (s.o.) */
int  re15_montage_fx_prev_pan_y(void);
int  re15_montage_fx_prev_zoom_px(void);
int  re15_montage_fx_prev_base_zoom(void);

int  re15_montage_fx_level_max(void);      /* 0x60 (RE2-Klemme @0x801c1bb8-c0) */
int  re15_montage_fx_mode(void);           /* Modus der aktuellen Ebene (Tests) */
int  re15_montage_fx_prev_mode(void);      /* Modus der ausblendenden Ebene      */

int  re15_montage_fx_stock(void);          /* 1 = RE15_MONTAGE_STOCK=1 */
void re15_montage_fx_stock_set(int v);     /* Tests: 0/1 erzwingen, -1 = Umgebung */

#endif /* RE15_MONTAGE_FX_H */
