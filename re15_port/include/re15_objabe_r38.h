/*
 * RE1.5 Rebuilt — Runde 38 Spur W "rolltor20a0", Nachbesserung 1 (Abnahme-Mangel M1): HALBTRANSPARENTE Raum-Objekte
 * (Obj_model_set-Attribut Bit 0x2) wie im Original — Parameter 4 der Objekt-Zeichner, ABR aus der Texpage, Mischung
 * nur fuer Texel mit STP-Bit.
 *
 * ANLASS: der Original-Polygon-Weg (objpoly_r38.c) zeichnet in ROOM5060/5061 Cut 4 ein Halbviereck der Glasscheibe
 * obj 4 (Attribut 0x0012), das der alte Port-Ausschnitt `_vz < 64` verworfen hatte. Der PC zeichnete es DECKEND dunkel;
 * das Original mischt es zur Haelfte mit dem Hintergrund. Vorbestehend: der PC-Prop-Weg kannte kein ABE (74 Saetze in
 * 152 inhaltsverschiedenen RDTs, Zensus analysis/befunde_runde38/W_belege/zensus_abe.txt).
 *
 * DER ORIGINAL-WEG (PSX.EXE, selbst disassembliert; Dossier W §10.2):
 *   ABE  FUN_8002c18c  @0x8002c2ac `lw v0,12(s1)` (Attribut = Pool+0x0C) / @0x8002c2b4 `andi v0,v0,0x2` /
 *                      @0x8002c2b8 `sltu s2,zero,v0` -> Parameter 4 (a3 = s2) an FUN_80022f0c @0x8002c304,
 *                      FUN_80023098 @0x8002c32c, FUN_800254a0 @0x8002c364, FUN_800256b0 @0x8002c38c; Typ 3 ueber
 *                      FUN_8003e64c @0x8002c2cc (a1 = s2) -> @0x8003e66c / @0x8003e6b8 (a3 = s2).
 *        Zeichner     Code = Grundcode | (ABE << 1): FUN_80022f0c @0x80022f18 `sll a3,a3,1` / @0x80022f1c
 *                      `ori a3,a3,0x34` / @0x80022f38 `sb a3,3(a2)`; FUN_80023098 @0x800230ac/@0x800230b0 (0x3c)/
 *                      @0x800230d0; FUN_800254a0 @0x800254ac/@0x800254b0 (0x34)/@0x800254d4; FUN_800256b0
 *                      @0x800256bc/@0x800256c0 (0x3c)/@0x800256ec -> 0x36 / 0x3E = halbtransparent.
 *   ABR  Texpage-Bits 5-6 jedes Primitivs = MD1-Seite ODER (Attribut & 0xC) << 3 — beim Modell-Bind gebaut:
 *                      FUN_80025940 (GT3) @0x800259bc `lw v0,0(a0)` (a0 = Pool+0x0C) / @0x800259c4 `andi v0,v0,0xc` /
 *                      @0x800259cc `sll t7,v0,19` / @0x80025a14 `or v0,t7,v0` / @0x80025a18 `sw v0,-8(a3)` (Wort 6 =
 *                      u1 v1 Texpage); FUN_80025a98 (GT4) @0x80025b1c / @0x80025b24 / @0x80025b2c / @0x80025b70 /
 *                      @0x80025b74.
 *   FARBE Flach-Zeichner (Attribut & 0x10, @0x8002c2f4 `andi v0,v0,0x10` / @0x8002c2f8 `beq`; Typ 3 immer, FUN_8003e64c
 *                      ruft nur FUN_80022f0c @0x8003e6ac / FUN_80023098 @0x8003e6dc) nehmen obj+0x70 = 0x808080
 *                      (FUN_8002b898) = Textur unveraendert; die Licht-Zeichner NCCT.
 *   GPU  (psx-spx graphicsprocessingunitgpu.md "Semi-transparency"): B = Hintergrund, F = Texel x Farbe / 128;
 *                      ABR 0 = B/2 + F/2, 1 = B + F, 2 = B - F, 3 = B + F/4 (Kanal auf 0..255 geklemmt); "For textured
 *                      primitives using 4-bit or 8-bit textures, bit 15 of each CLUT entry acts as a semi-transparency
 *                      flag ... If the semi-transparency flag is off, the new pixel is written to VRAM as-is";
 *                      Texel 0000h ist ganz durchsichtig.
 * GEMESSEN AM ORIGINAL (DuckStation-GDB, W_belege/orig_abe_*_log.txt): ROOM5060 obj 4 (0x0012) Code 0x3e Texpage 0x0086
 * (ABR 0), obj 5 (0x0006) Code 0x3e Texpage 0x00a6 (ABR 1); ROOM20A0 obj 4-6 Typ 3 (0x001a) Code 0x3e Texpage 0x00c7
 * (ABR 2); ROOM4090 obj 0-2 (0x000e) Code 0x3e/0x36 Texpage 0x00e5 (ABR 3).
 *
 * Diese Datei: die Regeln (engine, auch PSX-tauglich) und die STP-Teilung einer Textur in den DECKENDEN Teil (STP = 0)
 * und den HALBTRANSPARENTEN Teil (STP = 1) — der PC zeichnet ein ABE-Polygon als zwei Durchgaenge mit diesen beiden
 * Texturen (platform/pc/src/objabe_r38_pc.c), weil SDL die Mischung nur je Dreieck, nicht je Texel umschalten kann.
 */
#ifndef RE15_OBJABE_R38_H
#define RE15_OBJABE_R38_H

#include <stdint.h>
#include "re15_tim.h"

#define RE15_OBJABE_ATTR_ABE    0x0002   /* @0x8002c2b4 `andi v0,v0,0x2` */
#define RE15_OBJABE_ATTR_ABR    0x000C   /* @0x800259c4 / @0x80025b24 `andi v0,v0,0xc`, << 19 = Texpage << 3 */
#define RE15_OBJABE_ATTR_FLACH  0x0010   /* @0x8002c2f4 `andi v0,v0,0x10` */
#define RE15_OBJABE_TYP_KACHEL  3        /* @0x8002c2bc `ori v0,zero,0x3` / @0x8002c2c0 `bne` -> FUN_8003e64c */

/* 1 = das Objekt zeichnet halbtransparent (Parameter 4 der Zeichner). */
int re15_objabe_r38_abe(uint16_t attr);
/* ABR 0..3 eines Polygons: Texpage-Bits 5-6 der MD1-Seite ODER (Attribut & 0xC) << 3 (FUN_80025940/FUN_80025a98). */
int re15_objabe_r38_abr(uint16_t attr, uint16_t page);
/* 1 = Flach-Zeichner (Farbe 0x808080 = Textur unveraendert), 0 = Licht-Zeichner (NCCT). */
int re15_objabe_r38_flach(uint16_t attr, int obj_typ);

/* GPU-Mischung eines Kanals (8 Bit, psx-spx): Referenz fuer die Riegel. */
uint8_t re15_objabe_r38_mische(int abr, uint8_t b, uint8_t f);

/* Texel-Klasse aus dem aufgeloesten 16-Bit-Wert: 0 = 0000h (nie gezeichnet), 1 = deckend (STP 0), 2 = halbtransparent
 * (STP 1) — Letzteres nur, wenn das Primitiv ABE traegt; sonst ist es deckend. */
int re15_objabe_r38_texel_klasse(uint16_t raw);

/* Maße der Slot-Textur, wie render_pc.c re15_render_pc_upload_tim_slot sie baut (CLUTs untereinander gestapelt). */
int re15_objabe_r38_textur_mass(const re15_tim_t *tim, int *w, int *h);
/* Die zwei Teil-Texturen einer TIM (ARGB8888, Layout = re15_render_pc_upload_tim_slot):
 *   deckend[i] = Texel mit STP 0 wie re15_tim_texel_argb, STP-1-Texel und 0000h = 0 (nicht gezeichnet)
 *   halb[i]    = Texel mit STP 1 wie re15_tim_texel_argb (Alpha FF), alle anderen 0.
 * Puffer je w*h (re15_objabe_r38_textur_mass). Rueckgabe: Zahl der STP-1-Texel (0 = Teilung unnoetig), -1 = Format
 * nicht unterstuetzt. */
long re15_objabe_r38_teilen(const re15_tim_t *tim, uint32_t *deckend, uint32_t *halb);

/* ---- NUR PC (platform/pc/src/objabe_r38_pc.c) -------------------------------------------------------------------- */
/* Raumwechsel: alle Teil-Texturen verwerfen (Haken am Anfang des Raum-Objekt-Ladens in main.c). */
void re15_objabe_r38_pc_raum_neu(void);
/* Die TIM des Raum-Objekt-Slots `slot` wurde hochgeladen: Teil-Texturen bauen, falls sie STP-1-Texel hat. */
void re15_objabe_r38_pc_textur(const re15_tim_t *tim, int slot);
/* Ein Objekt-Dreieck (Argumente wie re15_render_textured_tri_lit, davor Attribut und MD1-Seite): ohne ABE unveraendert
 * EIN deckendes Dreieck; mit ABE zwei Durchgaenge — STP-0-Texel deckend, STP-1-Texel nach ABR gemischt. */
void re15_objabe_r38_pc_dreieck(uint16_t attr, uint16_t page,
                                int x0, int y0, int u0, int v0,
                                int x1, int y1, int u1, int v1,
                                int x2, int y2, int u2, int v2,
                                int tpage, int clut, int z,
                                uint8_t r0, uint8_t g0, uint8_t b0,
                                uint8_t r1, uint8_t g1, uint8_t b1,
                                uint8_t r2, uint8_t g2, uint8_t b2);
/* Mischmodi der Zeichen-Queue (render_pc.c s_textri_blend) fuer die zwei Durchgaenge. 0..2 belegt render_pc.c. */
#define RE15_OBJABE_BLEND_DECKEND  3     /* deckende Teil-Textur, normal (SDL BLEND, Alpha FF) */
#define RE15_OBJABE_BLEND_HALB(abr) (4 + (abr))   /* halbtransparente Teil-Textur, ABR 0..3 */
/* render_pc.c Flush-Haken: Textur und SDL-Grundmodus (0 BLEND / 1 ADD / 2 B-F) fuer einen Stapel mit Modus
 * `blend` >= 3. NULL = Stapel nicht zeichnen (keine STP-1-Texel). */
#ifdef RE15_PLATFORM_PC
struct SDL_Texture;
struct SDL_Texture *re15_objabe_r38_pc_flush_textur(int slot, int blend, struct SDL_Texture *normal, int *sdl_modus);
#endif

#endif /* RE15_OBJABE_R38_H */
