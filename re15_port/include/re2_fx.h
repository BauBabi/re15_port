/**
 * @file re2_fx.h
 * @brief Runde 34 VERTRAG V3 (C0, BAUPLAN §3.0): die RE2-FX-MASCHINE (RE2-Retail-Effektpool) —
 *        Schnittstelle fuer den Saeure-/Brand-Aufschlag der Granaten 0x0A/0x0B (E8) und das
 *        Bodenfeuer (Op 48 + Bodenflammen).
 *
 * STAND C0 = NUR STUBS (engine/src/re2_fx.c): nichts wird registriert, gespawnt, getickt oder
 * gezeichnet; beide Funktionszeiger sind NULL. Die Umsetzung gehoert Spur D (O-VB1, O-VB2, C5,
 * C6; Zeichner in platform/pc/src/re2fx_pc.c). Der Zeichner-Zugriff auf die Plaetze wird von
 * Spur D hier ADDITIV ergaenzt — die Deklarationen unten bleiben woertlich stehen.
 *
 * Alle Adressen = RE2-PSX.EXE (info/re2leon/PSX.EXE), sofern nicht anders genannt.
 *   Pool       0x800D8CF0, Schritt 0x7C, 96 Plaetze (Spawner FUN_8001cbe8 `addiu t2,zero,96`)
 *   Spawner    FUN_8001BF10 / FUN_8001cbe8: a0 = Bank<<24 | Sub<<16 | Skala, Sub&7 = Skript,
 *              Sub>>3 = CLUT-Zeile (@0x8001bf1c-c0)
 *   Pumpe      FUN_8001d300 (einziger Aufrufer `jal 0x8001d300` @0x80026980, NACH der
 *              Gegner-Schleife 0x800267c0-0x80026930)
 *   Op-Tabelle @0x8009D868 (`lw v0,-10136(at)` + `jalr`), u.a. [40] 0x80020758, [48] 0x80020F3C,
 *              [49] 0x800215C8
 */
#ifndef RE2_FX_H
#define RE2_FX_H

#include <stdint.h>
#include <stddef.h>

/** Bank-Registrierung der RE2-CORE00.ESP = FUN_8001bca0 (Skripttabelle = Kopf + (2*ca + cb + 2)*4:
 *  `srl a0,v0,16` / `andi v0,v0,0xffff` / `sll v0,v0,1` / `addu` / `addiu v0,v0,2` / `sll v0,v0,2`
 *  @0x8001bd08-1c). raw/size = shared_assets/RE2/CORE00.ESP (8572 B, Ids `03 05 00 01 02 06 07 04`),
 *  vom Aufrufer gehalten (nicht kopiert). Rueckgabe 0 = registriert, < 0 = Fehler.
 *  C0-Stub: registriert nichts, Rueckgabe -1. */
int  re2fx_register_core(const uint8_t *raw, size_t size);

/** Alle Plaetze frei (Raumwechsel / neues Spiel / Tests). C0-Stub: nichts. */
void re2fx_reset(void);

/** Aufschlag-Einstieg (E8, Port-Zuordnung der Uebergabe Granatenplatz -> RE2-FX-Platz): spawnt den
 *  Platz, dessen Op B 48 (Brand, re2_art 1, @0x80020F3C) bzw. 49 (Saeure, re2_art 2, @0x800215C8)
 *  seine Phase 0 im Aufschlagbild laeuft. re2_art = RE2-Art-Byte +0x1B (Id - 9 @0x8001f1a8-b8;
 *  Aufschlag-Op = 47 + Art); q = Granaten-Weltlage (re15_esp_fx_t.wpos), gier = Granaten-Gier
 *  (slot+0x2e). Gebunden an re15_esp_aufschlag_hook (include/re15_esp.h). C0-Stub: nichts. */
void re2fx_aufschlag(int re2_art, const int32_t q[3], int16_t gier);

/** Ein Spielbild der Pumpe FUN_8001d300 (Update-Pass Op A, Draw-Pass mit 0x4000-Befoerderung,
 *  FUN_8001d68c: Weltlage FUN_8001d894, Op B, Physik, Anim). Im Port direkt hinter dem
 *  RE1.5-ESP-Tick (Spur C1). C0-Stub: nichts. */
void re2fx_tick(void);

/** Schadens-Applier des Bodenfeuers (Op 40 @0x80020758 -> `jal 0x800470c0` @0x800207bc).
 *  Signatur = re15_re2_gl_apply (include/re15_damage.h, V2b): p = &Pruefpunkt (s32, y - 100),
 *  gier = Platz +0x22, box = Kopie {-600,0,300,150} von @0x80010910, hitcode 0x2002000A.
 *  Rueckgabe != 0 = Treffer -> Op 50 (@0x800207c4-cc). Vorgabe NULL (= kein Schaden); die
 *  Plattform bindet ihn an re15_re2_gl_apply, sobald Spur B gemergt ist. */
extern int  (*re2fx_applier)(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode);

/** SE-Haken der RE2-FX-Maschine = FUN_8005ba28-Analogon (a0 = code, a1 = Lagezeiger):
 *  Op 49 0x01130001 (`lui a0,0x113` / `ori a0,a0,0x1` @0x80021678-7c, `jal 0x8005ba28` @0x800216ac,
 *  a1 = sp+16), Op 48 0x01120001 (`jal 0x8005ba28` @0x8002102c, a1 = Platz+0x60). Die Toene liegen
 *  in RE1.5 selbst: ARMS10 / ARMS11 Satz 10 (E9). Vorgabe NULL (= stumm); Bindung Plattform. */
extern void (*re2fx_se_hook)(uint32_t code, const int32_t pos[3]);

#endif /* RE2_FX_H */
