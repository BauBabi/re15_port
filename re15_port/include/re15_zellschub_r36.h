/*
 * re15_zellschub_r36.h — ZELLEN-AUSSCHIEBUNG der Objekte (Ruhe-Zweig des Objekt-Passes FUN_8002bd44).
 * Runde 36 Spur R "dachkiste", Nachbesserung 3 (Abnahme 2, Maengel P1 und P4).
 *
 * Original (info/Re1.5/PSX.EXE = Auslieferungsstand, @0x80026e4c `08 00 e0 03`; alles selbst
 * disassembliert mit .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x8002bd44 / 0x8003b558 /
 * 0x8003da78 / 0x8003b068; Belege: analysis/befunde_runde36/R_dachkiste.md, Abschnitt "Nachbesserung 3"):
 *
 *   Objekt-Pass FUN_8002bd44, je aktivem Pool-Eintrag (@0x8002bdcc-d8): JEDER Eintrag, der nicht das
 *   Stand-Objekt ist (@0x8002bef8 `bne s2,DAT_800ac788` -> FUN_8002cfd4 -> `j 0x8002c0ec`) und dessen
 *   Schub nicht angenommen UND im Substate 8 gelandet ist (@0x8002c0a8 `j 0x8002c0d8`), laeuft ueber
 *   LAB_8002c0b8:  `ori a1,zero,2` / obj+0x34 = (s16)s4 / obj+0x3C = (s16)s5 (@0x8002c0bc-d4, die
 *   gesicherte Lage) / **`jal 0x8003b558` @0x8002c0d0 — Rueckgabe UNGEPRUEFT**, dann FUN_8002cabc
 *   (Spieler, obj, 0) @0x8002c0e4. Zugaenge zu c0b8: Szenen-Schub Bit 0x200 @0x8002bee4, obj+0 & 0x80
 *   @0x8002bf78-84, Zaehler obj+0x8C != 9 @0x8002bf8c-94, Schub gesperrt @0x8002bfb8 -> @0x8002c0b0,
 *   Substate != 8 @0x8002bfe0-e4. Danach @0x8002c0f0-c10c je Eintrag der u16-Spiegel obj+0x40/+0x42/+0x44
 *   = Lage (`lhu` 52/56/60 -> `sh` 64/66/68).
 *
 *   FUN_8003b558(a0 = obj, a1 = Maske): Sonde = obj+0x78 Box-Mitte (@0x8003b5ac-5d8), Quadrant
 *   FUN_8003b068(&obj.x, Sonde, ceil_x, ceil_z) = 2 Bit aus sign(z+cz-ceil_z) | sign(x+cx-ceil_x)>>1
 *   (@0x8003b068-b0a0), Zellenliste des Quadranten @0x8003b5f4-624. Je Zelle: Band `(s16)(cell+10)>>12`
 *   == obj+0x82 (@0x8003b634-644), `(s8)u0 & (Maske & 0xff)` != 0 (@0x8003b64c-660), obj+0 & 8 ->
 *   uebersprungen (@0x8003b668-674); r = hx (@0x8003b68c), bei hx != hz (@0x8003b690) der Lagecode der
 *   Objektmitte zur Zelle (@0x8003b698-b70c: x 3 = westlich, 2 = innerhalb, 0 = oestlich; z analog) ->
 *   r = hz NUR bei x == 2 und z != 2 (@0x8003b6f8-734 `lhu t0,10(a3)`); Rechteck (u32)(cxw-(x0-r)) <
 *   w+2r (@0x8003b738-768) und (u32)(czw-(z0-r)) < d+2r (@0x8003b770-794); Treffer -> **`jal 0x8003da78`
 *   @0x8003b79c (a0 = Zelle, a1 = obj, a2 = r)**, Ergebnis |= 1 (@0x8003b7a4); die naechste Zelle sieht
 *   die VERSCHOBENE Lage (obj.x/z werden je Zelle neu gelesen @0x8003b73c/@0x8003b774).
 *
 *   FUN_8003da78(a0 = Zelle, a1 = obj, a2 = r): DAT_800b8294 = obj (@0x8003da94); t0 = (x1+r)-cxw+18
 *   (@0x8003da98-ab0), a0 = (x0-r)-cxw-18 (@0x8003daa4/@0x8003dab4-b8), kleinerer Betrag (@0x8003dabc-acc
 *   `slt -a0,t0`); z: a3 = (z1+r)-czw+18 / (z0-r)-czw-18 (@0x8003dad0-b10); Lagecode @0x8003db14-db38:
 *   Bit0 = sign(t0 ^ (obj.x - (s16)obj+0x40)), Bit1 = sign(a3 ^ (obj.z - (s16)obj+0x44)). Code 2
 *   (@0x8003dba0-bc4): |a3|+400 < 801 -> obj.z += a3, sonst kleinerer Betrag; Code 1 (@0x8003db74-98):
 *   |t0|+400 < 801 -> obj.x += t0, sonst kleinerer Betrag; Code 3 (@0x8003dbc8-dc28): |t0| < |a3| ->
 *   obj.x += t0, sonst obj.z += a3; Code 0 (@0x8003dc2c-58): Lage != Spiegel -> obj.x/z = Spiegel
 *   (@0x8003dc98-cb4, Rueckgabe 0), sonst kleinerer Betrag (@0x8003dc60-94).
 *
 * Der Port fuehrt den Spiegel obj+0x40/+0x44 je Prop-Platz hier (g_scd.props[] hat das Feld nicht).
 * Startwert = Lage beim Raumaufbau (PORT-WAHL, belegt: der Installer LAB_80040914 schreibt +0x40/+0x44
 * NICHT — Stores @0x8004095c-80040aa8 nur +0/+8/+9/+12/+52..+60/+104..+108/+116/+120/+130 —, der
 * Raumlader FUN_8003ea7c nullt nur +0; im Original steht dort Altspeicher des Pool-Platzes, s. Dossier).
 * Im Bild 13 eines Falls (obj+5 = 1, obj+6 == 0xd @0x8002c660-6c, Bandwechsel) schreibt FUN_8002c444
 * @0x8002c6ac-b0 den Spiegel selbst = Lage (der Ruhe-Zweig sieht dann dx = dz = 0); in allen anderen
 * Bildern gilt das letzte Bildende. Der Port liest Fall/Zustand/Zaehler aus objfall_r36 (re15_objfall_info).
 * Original gemessen (R_belege/orig_2050_fall): HIT 1 Spiegel = Lage, HIT 2 Spiegel = voriges Bildende.
 */
#ifndef RE15_ZELLSCHUB_R36_H
#define RE15_ZELLSCHUB_R36_H

#include <stdint.h>
#include "re15_rdt.h"

/* Raumaufbau (scd_room_setup.c, Ende des Installer-Blocks): Spiegel aller Plaetze = Lage, Bildmerker 0. */
void re15_zellschub_install(uint16_t room_id);

/* FUN_8003b558(obj, mask) auf die Lage (*x, *z) des Props p: Rueckgabe != 0 = Zelle getroffen; je Treffer
 * wird (*x, *z) wie FUN_8003da78 aus der Zelle geschoben (der Aufrufer entscheidet, ob er die Lage
 * uebernimmt — der Schub-Test @0x8002bfb0 verwirft sie durch die Ruecksetzung @0x8002c0bc-d4). */
int  re15_zellschub_8003b558(const re15_rdt_t *rdt, int p, int32_t *x, int32_t *z, unsigned mask);

/* Ruhe-Zweig LAB_8002c0b8 fuer EIN Objekt: FUN_8003b558(obj, 2) auf g_scd.props[p].x/z mit Ausschiebung. */
void re15_zellschub_ruhe(const re15_rdt_t *rdt, int p);

/* Spiegel @0x8002c0f0-c10c fuer EIN Objekt (nach dem Ruhe-Zweig / Schub des Bildes). */
void re15_zellschub_spiegel(int p);

/* Merker: der Objekt-Pass (re15_prop_push_tick) lief in diesem Bild — Zweige ohne Spieler-Schub
 * (Klettern, Treppe, Szene) rufen stattdessen re15_zellschub_nach_pass. */
void re15_zellschub_pass_lief(void);
void re15_zellschub_nach_pass(const re15_rdt_t *rdt);

/* ---- Mess-/Testzugriff (keine Spiellogik) -------------------------------------------------- */
int  re15_zellschub_spiegel_lesen(int p, int32_t *sx, int32_t *sz);   /* 0 = ok */
/* FUN_8003da78 auf eine freie Zelle/Box (Sonden): wie im Original, Spiegel explizit. */
int  re15_zellschub_8003da78(int32_t cell_x, int32_t cell_w, int32_t cell_z, int32_t cell_d,
                             int32_t box_cx, int32_t box_cz, int32_t r,
                             int32_t spiegel_x, int32_t spiegel_z, int32_t *x, int32_t *z);

#endif /* RE15_ZELLSCHUB_R36_H */
