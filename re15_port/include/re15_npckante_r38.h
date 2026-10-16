/* re15_npckante_r38.h — Runde 38, Spur M "npckante" (Dossier analysis/befunde_runde38/M_npckante.md).
 *
 * NUTZER (2026-10-16): "Kendo springt am Ende von ROOM 10B0 nicht die Kante runter, wie im Original, sondern
 * laeuft in die Luft weiter" / "Bei Room 1260 Springt Kendo auch nicht die Kanten runter. Machen Marvin und Ada
 * in Leons Szenario uebrigens auch immer nicht."
 *
 * ORIGINAL (info/Re1.5/PSX.EXE, selbst disassembliert): die Plc_dest-Geh-/Lauf-Subs der NPC-Familie 0x40..0x4d
 * rufen nach jedem Schritt die KANTENSONDE FUN_8002d7d8 (einzige zwei XREFs: @0x80051374 in Sub 4 = 0x80051148,
 * @0x800516c4 in Sub 5 = 0x80051484; kein Overlay ruft sie, Byte-Suche `f6 b5 00 0c` ueber STAGE1..6/DEBUG/TITLE).
 * Ergebnis nach +0x9f (`sb v0,159(v1)` @0x80051388 / @0x800516d8); != 0 -> +0x5 = 0x12 (Sub 4, @0x800513a4) bzw.
 * 0x16 (Sub 5, @0x800516f4), +0x6 = 0. NPC-Tabelle @0x80076ca0: [18] = [22] = 0x80052508 (Kante: Kurs rasten,
 * dann HOCHKLETTERN), [23] = [24] = 0x80052c14 (HINUNTERSPRINGEN). Die Subs 7/8, der Spieler-Walker
 * (0x80030af0/0x80030d28) und Sub 9 rufen die Sonde NICHT.
 */
#ifndef RE15_NPCKANTE_R38_H
#define RE15_NPCKANTE_R38_H

#include <stdint.h>
#include "re15_actor.h"

/* ---- Sonde FUN_8002d7d8 ---------------------------------------------------------------------------------------- */
#define RE15_NPK_SONDE_WEG        800    /* Vektor @0x800109c8 = {800,0,0,0} (lwl/lwr-Kopie @0x8002d7fc-820), gedreht
                                          * um den GERASTETEN Kurs (rot+512)&~1023 @0x8002d82c-838 (FUN_8004f008 =
                                          * RotMatrix 0x80068098 + ApplyMatrixSV 0x800661c0)                          */
#define RE15_NPK_ZELLE_KLETTERN   0x01u  /* Zellwort & 1 @0x8002d880 -> Ergebnis 1 (@0x8002d944 `ori v0,zero,0x1`)  */
#define RE15_NPK_ZELLE_SPRUNG     0x02u  /* Zellwort & 2 @0x8002d9f0 -> room_coll + `slt` @0x8002da14 -> Ergebnis 2   */
#define RE15_NPK_SONDE_MASKE      0x100u /* room_coll a3 = 0x100 @0x8002da08, a1 = 1 @0x8002d9fc                       */

/* ---- Subs (NPC-Tabelle @0x80076ca0) ------------------------------------------------------------------------------ */
#define RE15_NPK_SUB_KANTE_GEHEN  0x12   /* Sub 4: `ori v1,zero,0x12` @0x80051394, `sb v1,5(v0)` @0x800513a4           */
#define RE15_NPK_SUB_KANTE_LAUFEN 0x16   /* Sub 5: `ori v1,zero,0x16` @0x800516e4, `sb v1,5(v0)` @0x800516f4           */
#define RE15_NPK_SUB_SPRUNG_LAUF  0x17   /* aus 0x16: `ori v0,zero,0x17` @0x8005267c                                    */
#define RE15_NPK_SUB_SPRUNG_GEHEN 0x18   /* aus 0x12: `ori v0,zero,0x18` @0x80052684                                    */

/* Kanten-Sub 0x80052508, Phasen +0x6 ueber @0x800112e4 (5 Eintraege, `sltiu v0,v1,0x5` @0x80052520) */
#define RE15_NPK_KANTE_TEMPO      0x4b   /* +0x8c = 75 @0x80052550/54                                                  */
#define RE15_NPK_KANTE_CLIP_RAST  5      /* +0x94 = 5 @0x80052570/74, Kanal +0x170/+0x174 (anim_set @0x800525e8-f0)    */
#define RE15_NPK_KANTE_CLIP_HOCH  2      /* +0x94 = 2 @0x800526a0/a4, Kanal +0x84/+0x16c (anim_set @0x800527f0-f8)      */
#define RE15_NPK_HOCH_BILD_AN     0x11   /* +0x95 == 0x11 -> +0x93 |= 1 @0x800526d8-f0                                  */
#define RE15_NPK_HOCH_BILD_STUFE  0x1d   /* +0x95 == 0x1d @0x80052704 -> Stufe                                          */
#define RE15_NPK_HOCH_BILD_AUS    0x25   /* +0x95 == 0x25 -> +0x93 &= 0xfe @0x800527c8-e0                               */
#define RE15_NPK_HOCH_SCHUB       0x47c  /* +0x8c = 1148 @0x80052710/18, pos_advance(0) @0x80052714                     */
#define RE15_NPK_BAND_HOEHE       1800   /* y -= 1800 @0x80052730, +0x1ba -= 1800 @0x8005274c, +0x82 += 1 @0x80052768;
                                          * Sprung: +0x82 = -(+0x1ba / 1800) per Kehrwert 0x91a2b3c5 @0x80052da4-d0     */

/* Sprung-Sub 0x80052c14, Phasen +0x6 ueber @0x8001131c (7 Eintraege, `sltiu v0,v1,0x7` @0x80052c2c) */
#define RE15_NPK_SPRUNG_CLIP_AB   3      /* +0x94 = 3 @0x80052c6c/70, Kanal +0x84/+0x16c (anim_set @0x80052ca0-a8)      */
#define RE15_NPK_SPRUNG_BILD_AB   0x0d   /* +0x95 == 0xd -> +0x6 = 2 @0x80052cbc-d0                                     */
#define RE15_NPK_SPRUNG_CLIP_LUFT 4      /* +0x94 = 4 @0x80052d08/0c, +0x8f = 0 @0x80052d2c                             */
#define RE15_NPK_SPRUNG_SCHUB     0x320  /* +0x8c = 800 @0x80052d48/50, pos_advance(0) @0x80052d4c                      */
#define RE15_NPK_SPRUNG_HUB       500    /* y += 500 @0x80052d68/6c                                                      */
#define RE15_NPK_FALL_START       90     /* y += 90 + 50*t (t = +0x9c, danach +1) @0x80052e28-5c                         */
#define RE15_NPK_FALL_ZUWACHS     50
#define RE15_NPK_LANDE_ABSTAND    800    /* (+0x1ba - 800) < y -> +0x6 = 4 @0x80052e88-a4                                */
#define RE15_NPK_SPRUNG_CLIP_LAND 5      /* +0x94 = 5 @0x80052ec0/c4, +0x8f = 0 @0x80052ee4, y = +0x1ba @0x80052efc       */

/* Executor-Haken (enemy_ai_common.c): Sonde nach dem Schritt der Subs 4/5. Rueckgabe 1 = Sub gewechselt,
 * der Aufrufer beendet den Tick (Sub 4: nach anim_set @0x80051374ff, Sub 5: vor anim_set @0x800516c4ff). */
int  re15_npckante_nach_schritt(re15_actor_t *e);
/* Executor-Dispatch fuer +0x5 in {0x12, 0x16, 0x17, 0x18}; Rueckgabe 0 = kein Kanten-Sub. */
int  re15_npckante_dispatch(re15_actor_t *e);
/* 1 = einer der vier Kanten-Subs laeuft (Yield-Gate des NPC-Ticks: der Executor ist ihr einziger Vorschieber). */
int  re15_npckante_aktiv(const re15_actor_t *e);
/* Kanal fuer Uhr und Render: 1 = Loco-Paar +0x84/+0x16c, 2 = eigene Bank +0x170/+0x174, 0 = kein Kanten-Sub. */
int  re15_npckante_kanal(const re15_actor_t *e);
/* Alte Lage fuer die SCA-Klemme: nach einem Schub (Kletterstufe @0x80052784-b4, Absprung @0x80052de8-e18) setzt das
 * Original +0x40/+0x42/+0x44 = neue Lage, die Klemme FUN_8003b0a4 sieht den Schub also nicht als Bewegung. */
void re15_npckante_altlage(const re15_actor_t *e, int32_t *ox, int32_t *oz);

/* Sonde selbst (lesend): 0 / 1 (hochklettern) / 2 (hinunterspringen) fuer die aktuelle Lage. */
int  re15_npckante_sonde(const re15_actor_t *e);

/* Messhaken RE15_NPCKANTE_LOG=<datei>: je NPC-Tick eine Zeile (Lage, y, Band, Sub, Clip, Kanal, Sonde). */
void re15_npckante_spur(const re15_actor_t *e);

#endif /* RE15_NPCKANTE_R38_H */
