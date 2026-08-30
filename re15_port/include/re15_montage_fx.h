/*
 * re15_montage_fx.h — Praesentations-Schicht der Standbild-Montage (Pre-Intro ROOM1240).
 * Mechanismus + alle Raten aus RE2s OPENING.BIN (Element-Renderer 0x801c1a0c);
 * Ableitung und @0x…-Belege: analysis/preintro_re2/re2-bewegung.md.
 * RE15_MONTAGE_STOCK=1 = byte-true RE1.5-Hartschnitt (Schicht aus).
 */
#ifndef RE15_MONTAGE_FX_H
#define RE15_MONTAGE_FX_H

#include <stdint.h>

#define RE15_MONTAGE_CUTS 9      /* ROOM1240: 9 Standbilder (Cut 0..8) */

enum {
    RE15_MFX_NONE = 0,   /* nur Ein-/Ausblenden */
    RE15_MFX_PAN,        /* + vertikales Wandern (1 px je 10 Frames, RE2 Bit4) */
    RE15_MFX_ZOOM_IN,    /* + zentrierter Zoom hinein (2 px je 3 Frames, RE2 Bit6) */
    RE15_MFX_ZOOM_OUT    /* + zentrierter Zoom heraus (RE2 Bit5) */
};

void re15_montage_fx_reset(void);
void re15_montage_fx_set_active(int on);   /* Aufrufer gated auf den Montage-Raum */
int  re15_montage_fx_active(void);
void re15_montage_fx_on_cut(int cut, int had_previous);
void re15_montage_fx_tick(void);

int  re15_montage_fx_level_new(void);      /* 0..0x60 Helligkeit aktuelles Bild */
int  re15_montage_fx_level_prev(void);     /* 0..0x60 Helligkeit voriges Bild (Kreuzblende) */
int  re15_montage_fx_pan_y(void);          /* Wander-Offset in Pixeln */
int  re15_montage_fx_zoom_px(void);        /* Groessenaenderung in Pixeln (proportional) */
int  re15_montage_fx_base_zoom(void);      /* Sockel-Vergroesserung (Pan-Ueberhang, RE2: 80 px) */
int  re15_montage_fx_level_max(void);      /* 0x60 (RE2-Klemme @0x801c1bb8-c0) */

int  re15_montage_fx_stock(void);          /* 1 = RE15_MONTAGE_STOCK=1 */
void re15_montage_fx_stock_set(int v);     /* Tests: 0/1 erzwingen, -1 = Umgebung */

#endif /* RE15_MONTAGE_FX_H */
