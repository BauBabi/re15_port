/* glas1120_pc.h — Runde 35 Spur M: PC-Seite des Fenster-Ereignisses ROOM1120 (glas1120_pc.c). */
#ifndef GLAS1120_PC_H
#define GLAS1120_PC_H

#include <stdint.h>
#include "re15_camera.h"

/* Texturplatz der fuenf Glas-Banken (render_pc.c RE15_TIM_SLOT_MAX 56: 53..55 frei; 55 genommen). */
#define RE15_GLAS_TIM_SLOT 55

/* Einmal beim Start, NACH re2fx_register_core (main.c, hinter dem TEX.TIM-Block). */
void re15_pc_glas1120_init(void);
/* Im Effekt-Zeichenpass direkt hinter re2fx_pc_draw, mit derselben Ansicht. */
void re15_pc_glas1120_draw(const re15_camera_view_t *cam, int cx, int cy, int camf,
                           int has_region, const int16_t rxs[4], const int16_t rzs[4]);
/* Direkt nach dem Hintergrund (Ebene 1), cut = angezeigter Cut. */
void re15_fenster1120_pc_zeichnen(int cut);
/* Glasbruch-Bank (audio_pc.c). */
int  re15_audio_re2_glas_laden(void);
void re15_audio_re2_glas_se(int satz);

/* Pruefhaken. */
int re15_pc_glas1120_esp_rc(void);
int re15_pc_glas1120_tex_ok(void);
int re15_pc_glas1120_letzte_quads(void);
int re15_pc_glas1120_schaden_ops(void);

#endif /* GLAS1120_PC_H */
