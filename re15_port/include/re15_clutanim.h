/* re15_clutanim.h — Paletten-Animation der Raum-Objekte vom Typ 3 (Monitore ROOM5030/5031, Wasser STAGE2,
 * ROOM11A0/1260). Nutzer 2026-11-15: "Die Monitore im Original laufen, also grieseln, bei uns sind die Bilder
 * statisch."
 *
 * MECHANISMUS (PSX.EXE; Mod-EXE und 40%-EXE an allen Stellen byte-gleich):
 *   Anmeldung FUN_8002b968, einmal je Raum direkt nach dem SCD-Init-Lauf (FUN_8003ef6c: `jal 0x8003f0a0`
 *   @0x8003f018, `jal 0x8002b968` @0x8003f020). Fuer jedes AKTIVE Objekt (`andi v0,v0,0x1` @0x8002b9e0) in
 *   Pool-Reihenfolge (= obj_id, Schranke nOmodel = RDT[2]) wird sein Typ (pool+0x08, `lbu a0,-20(s5)`
 *   @0x8002ba00) in der Tabelle @0x80073c6c (Schritt 3, Ende 0xff) gesucht:
 *       80073c6c: 03 04 07 05 06 08 ff
 *   Treffer bei Eintrag s0 (= s4/3, @0x8002ba38-5c):
 *     - neue CLUT-Zeile GetClut(0x130, 0x1e0 + [0x800b281e]) (@0x8002ba4c-58) in ALLE Dreiecke/Vierecke des
 *       ersten Meshes (@0x8002ba98-bab0 / @0x8002bae8-bb00),
 *     - Quellzeile = CLUT-y des ersten Dreiecks, ueberschrieben vom ersten Viereck (`srl s4,v0,22`
 *       @0x8002ba80 / @0x8002bad4),
 *     - Anim-Eintrag e = 0x800b267c + [0x800b281e]*0x34: +0 = 0, +2 = Takt a = byte[0x80073c6d + s0],
 *       +1 = Zaehler 0, +3 = Laenge b = byte[0x80073c6e + s0] (@0x8002bb2c-bb60; Index s0, NICHT 3*s0 —
 *       Typ 5 liest also 07/05, nicht 06/08),
 *     - zwei DR_MOVE (Puffer 0/1, je 0x18) ab e+4: sx 0, sy Quellzeile, Ziel (0x130, 0x1e0 + [0x800b281e]
 *       NEU gelesen @0x8002bb8c), 16x1 (@0x8002bb64-bba8),
 *     - [0x800b281e] += 1 (@0x8002bbac-b8).
 *   ⛔ UEBERLAUF: Eintrag 8 liegt bei 0x800b281c; sein Byte +2 IST der Zaehler 0x800b281e. Der neunte
 *   angemeldete Eintrag setzt den Zaehler also auf a. Darum ist die Anmeldung hier als BYTE-SPEICHER
 *   nachgebaut und nicht als Feld: ROOM5030 (11 Monitore) endet so bei [0x800b281e] = 7 — im Original
 *   gemessen (DuckStation-RAM, MECHANISMUS.md der Runde).
 *
 *   Je Spielbild FUN_8002bc04 (`jal 0x8002bc04` @0x8002bda0 in FUN_8002bd44, das @0x8001ce14 ohne
 *   Pausen-Gate laeuft; nur nOmodel == 0 ueberspringt, @0x8002bd80):
 *     fuer k < [0x800b281e]:  DR_MOVE[Puffer].sx = (Zaehler / a) << 4   (@0x8002bc4c-bc7c)
 *                             Zaehler++; == b*a -> 0                  (@0x8002bc80-bcb0)
 *                             addPrim in die OT 0x800ac6d8 + Puffer*0x40 (@0x8002bcb4-bd20)
 *   addPrim haengt vorn ein: der Eintrag mit der KLEINEREN Nummer laeuft zuletzt und gewinnt, wenn zwei
 *   Eintraege dieselbe Zeile beschreiben.
 *
 * PORT: kein VRAM. Die Quelle eines Eintrags wird als (Objekt, TIM-CLUT-Zeile) gefuehrt — im Original ist es
 * die verlegte VRAM-Zeile derselben TIM. Eine Zielzeile, die kein getickter Eintrag beschreibt, gilt als
 * 0x0000 (= Texel nicht gezeichnet): so im Original gemessen fuer (304,487)/(304,488) in ROOM5030.
 */
#ifndef RE15_CLUTANIM_H
#define RE15_CLUTANIM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_CLUTANIM_MAX_OBJ 32

/* Ruecksetzen wie FUN_8003ecec (`sb zero,10270(at)` = 0x800b281e @0x8003ed94). */
void re15_clutanim_reset(void);

/* FUN_8002b968 nachgebaut. Je obj_id 0..n_obj-1:
 *   aktiv[i]    = Pool-Flag Bit 0 (Obj_model_set installiert und nicht wieder abgeschaltet)
 *   typ[i]      = pool+0x08 (Obj_model_set pc[2])
 *   quellzeile[i] = CLUT-Zeile der TIM, die das erste Primitiv des ersten Meshes nennt (Viereck vor Dreieck,
 *                 s.o.), -1 = Mesh ohne Primitive. */
void re15_clutanim_anmelden(int n_obj, const uint8_t *aktiv, const uint8_t *typ, const int16_t *quellzeile);

/* Bequemer Weg fuer den Raumaufbau: liest aktiv/typ aus g_scd.props und die Quellzeile aus MD1+TIM der
 * RDT (rdt->prop_md1/prop_tim). Muss DIREKT nach dem SCD-Init-Lauf laufen (@0x8003f020). */
void re15_clutanim_raum_anmelden(const void *rdt);

/* FUN_8002bc04: ein Spielbild. */
void re15_clutanim_tick(void);

/* Abfrage fuer den Zeichner, je obj_id:
 *   0 = nicht animiert (eigene CLUT des Modells unveraendert)
 *   1 = animiert: Farben = TIM von *quell_obj, Zeile *quell_zeile, Unterpalette *unter (Farben 16*unter ..+15)
 *   2 = animiert, aber die Zielzeile schreibt kein getickter Eintrag -> 0x0000 -> nichts zeichnen */
int re15_clutanim_objekt(int obj_id, int *quell_obj, int *quell_zeile, int *unter);

/* Hoechste Unterpalette, die der Eintrag des Objekts erreichen kann ((a*b-1)/a, gekappt auf 15); -1 wenn
 * nicht animiert. Fuer den Texturbau (je Unterpalette ein Streifen). */
int re15_clutanim_max_unter(int obj_id);

/* Messwerte fuer Tests/Protokoll. */
int     re15_clutanim_anzahl(void);                 /* [0x800b281e] */
int     re15_clutanim_eintrag(int k, int *zaehler, int *a, int *b, int *ziel, int *quell_obj, int *quell_zeile,
                              int *sx);             /* ziel = Zeile relativ 0x1e0; sx = zuletzt geschriebener Wert */
int     re15_clutanim_zielzeile(int obj_id);        /* GetClut-Zeile relativ 0x1e0, -1 = nicht angemeldet */

#ifdef __cplusplus
}
#endif

#endif
