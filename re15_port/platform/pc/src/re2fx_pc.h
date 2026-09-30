/*
 * re2fx_pc.h — Runde 34 VERTRAG V3 (C0): der PC-Zeichner der RE2-FX-Maschine (include/re2_fx.h).
 *
 * STAND C0 = STUB (re2fx_pc.c): zeichnet nichts, und NIEMAND ruft ihn (keine Verhaltensaenderung).
 * Umsetzung = Spur D (BAUPLAN §3.3 C6): RE2 TEX.TIM -> VRAM (768,256) (Lader FUN_80076a40
 * @0x80076a64-a8; 0x800cfbf0 := 28 @0x8002b8cc-d4 -> x = 28*64 - 1024), CLUT-Block (256,480)
 * (@0x80076afc-0c `addiu v0,v0,480`), Billboard FUN_80077924 / FUN_80077ed0 (rueckwaerts).
 * Einhaengen in den Bildaufbau (Aufruf aus platform/pc/main.c): Spur C/D.
 */
#ifndef RE2FX_PC_H
#define RE2FX_PC_H

/** Alle sichtbaren Plaetze der RE2-FX-Maschine zeichnen. C0-Stub: nichts. */
void re2fx_pc_draw(void);

#endif /* RE2FX_PC_H */
