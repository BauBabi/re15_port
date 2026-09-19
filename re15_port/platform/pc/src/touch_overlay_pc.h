/* =============================================================================================
 * RE1.5 Rebuilt — Touch-Overlay-Controller (On-Screen-Pad) fuer das PC-/Android-Target.
 *
 * WAS: ein halbtransparentes Pad, das am Ende jedes Frames ueber das fertige Bild gezeichnet
 * wird (render_pc.c, unmittelbar vor dem Mess-Readback und dem Present) und Multitouch-Finger
 * (SDL_FINGERDOWN/UP/MOTION) in dieselben PSX-Pad-Bits uebersetzt, die Tastatur und
 * GameController in input_pc.c liefern. Mehrere Finger gleichzeitig = mehrere Bits (Laufen +
 * Zielen + Schiessen).
 *
 * LAYOUT (Fensterkoordinaten, Querformat; u = Bildhoehe/12):
 *   D-Pad links unten (8 Richtungen, Diagonalen = zwei Bits), vier Face-Buttons rechts unten
 *   (Dreieck oben, Kreis rechts, Kreuz unten, Viereck links), L1/R1 oben links/rechts,
 *   SELECT/START unten Mitte, F9-MARKE oben Mitte (setzt die befund.log-Marke wie die
 *   Taste F9, main.c re15_input_debug_fkey(9)).
 *
 * SCHALTER:
 *   Android      : standardmaessig AN; RE15_TOUCH_OVERLAY=0 schaltet ab.
 *   Windows/Linux: standardmaessig AUS; RE15_TOUCH_OVERLAY=1 schaltet an — dann bedient die
 *                  MAUS das Pad (SDL_HINT_MOUSE_TOUCH_EVENTS: jeder Mausklick ist ein Finger),
 *                  damit sich das Overlay ohne Geraet pruefen laesst.
 *   RE15_TOUCH_SELFTEST=1: nach der Initialisierung wird jeder Knopf einmal synthetisch
 *                  gedrueckt und das Pad-Wort gegen den Sollwert geprueft; Ergebnis als
 *                  "[touch] SELFTEST RESULT ok=<n> fail=<m>" nach stderr (= debug.log).
 *
 * Ohne Schalter ist das Modul vollstaendig inert (kein Hint, keine Zeichnung, Bits = 0).
 * ============================================================================================= */
#ifndef RE15_TOUCH_OVERLAY_PC_H
#define RE15_TOUCH_OVERLAY_PC_H

#include <stdint.h>
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* VOR SDL_Init(): setzt auf dem Desktop den Maus->Finger-Hint, wenn das Overlay aktiv ist. */
void re15_touch_pc_preinit(void);

/* Nach SDL_CreateRenderer(): merkt sich den Renderer, laeuft ggf. den Selbsttest. */
void re15_touch_pc_init(SDL_Renderer *r);

/* 1 = Overlay aktiv (zeichnet + liefert Bits). */
int re15_touch_pc_enabled(void);

/* Aus der Event-Schleife (re15_render_begin_frame) fuer JEDES Event aufrufen. */
void re15_touch_pc_event(const SDL_Event *e);

/* PSX-Pad-Bits aus den aktuell aufliegenden Fingern (0 wenn inaktiv). */
uint16_t re15_touch_pc_pad_bits(void);

/* F9-MARKE: 1 genau einmal je Antippen des Marken-Knopfs (Flanke), sonst 0. */
int re15_touch_pc_take_marke(void);

/* Overlay zeichnen (Fensterkoordinaten; logische Groesse wird dabei kurz abgeschaltet). */
void re15_touch_pc_draw(SDL_Renderer *r);

/* Mini-Schrift (5x7, A-Z 0-9 und etwas Interpunktion) in Fensterkoordinaten — wird auch von
 * der Android-Entpack-Anzeige benutzt. Zeichenbreite = 6*scale Pixel. Rueckgabe: Breite. */
int re15_touch_pc_text(SDL_Renderer *r, int x, int y, int scale, const char *s,
                       uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca);

#ifdef __cplusplus
}
#endif
#endif /* RE15_TOUCH_OVERLAY_PC_H */
