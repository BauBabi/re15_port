/*
 * re2fx_pc.h — Runde 34 VERTRAG V3 (C0): der PC-Zeichner der RE2-FX-Maschine (include/re2_fx.h).
 *
 * Umsetzung Spur D (BAUPLAN §3.3 C6, bau_d.md §4): RE2 TEX.TIM -> VRAM (768,256) (Lader FUN_80076a40
 * @0x80076a64-a8; 0x800cfbf0 := 28 @0x8002b8cc-d4 -> x = 28*64 - 1024), CLUT-Block (256,480)
 * (@0x80076afc-0c `addiu v0,v0,480`), Billboard FUN_80077924 / FUN_80077ed0 (rueckwaerts) ueber
 * re2fx_quads() (engine/src/re2_fx.c). Die Deklaration re2fx_pc_draw steht woertlich (C0); alles
 * Weitere ist ADDITIV.
 *
 * Einhaengen (platform/pc/main.c, Spur C / Integration — INTEGRATIONSWUNSCH in bau_d.md):
 *   Boot:   re2fx_pc_lade_tex(<shared_assets/RE2/TEX.TIM>, n) nach dem Renderer-Start
 *   Bild:   re2fx_pc_set_ansicht(&cam_view, cx, cy, pc_fx_camf(), has_region, rxs, rzs);
 *           re2fx_pc_draw();      (direkt nach pc_draw_effects, main.c:10516)
 */
#ifndef RE2FX_PC_H
#define RE2FX_PC_H

#include <stdint.h>
#include <stddef.h>
#include "re15_camera.h"

/** Alle sichtbaren Plaetze der RE2-FX-Maschine zeichnen. C0-Stub: nichts. */
void re2fx_pc_draw(void);

/* ---- ADDITIV (Spur D) ------------------------------------------------------------------------ */

/** TIM-Slot der RE2-FX-Seiten. render_pc.c kennt 50 Slots (0..49, RE15_TIM_SLOT_MAX @render_pc.c:204);
 *  Slot 50 braucht RE15_TIM_SLOT_MAX 51 (INTEGRATIONSWUNSCH). Bis dahin laedt/zeichnet nichts. */
#define RE2FX_TIM_SLOT 50

/** RE2 TEX.TIM (shared_assets/RE2/TEX.TIM, 132320 B) in den Slot laden: die Seiten 0x1E (VRAM x 896)
 *  und 0x1F (x 960) nebeneinander (512 x 256 Texel, 4 bpp) mit den 19 CLUT-Zeilen (272, 480..498).
 *  Rueckgabe 0 = geladen, < 0 = Datei unpassend. Die Datei bleibt beim Aufrufer (Zeiger-Leihe). */
int  re2fx_pc_lade_tex(const uint8_t *tim, size_t size);

/** Kamera fuer dieses Bild (dieselben Werte wie pc_draw_effects). */
void re2fx_pc_set_ansicht(const re15_camera_view_t *cam, int cx, int cy, int camf,
                          int has_region, const int16_t rxs[4], const int16_t rzs[4]);

/** Zahl der im letzten re2fx_pc_draw eingereihten Quads (Messschiene). */
int  re2fx_pc_letzte_quads(void);

#endif /* RE2FX_PC_H */
