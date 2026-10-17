/*
 * Runde 38 Spur U "wasserzombie" — Zombies im Wasser ziehen sich am Rand hoch (RE1.5).
 *
 * NUTZER (analysis/befunde_runde38/AUFTRAG.md, woertlich): "Die Zombies in Stage2 die sich unten im Wasser befinden und
 * zum Rand des Wassers laufen müssen sich an den Rand hochziehen können, wie im Original Resident Evil 1.5".
 * Dossier: analysis/befunde_runde38/U_wasserzombie.md.
 *
 * DAS ORIGINAL (selbst disassembliert, STAGE2.BIN laedt roh @0x80100000; STAGE1.BIN-Zwilling 0x801033c8 identisch):
 *   Wurzel 0x801002b8 (Typen 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f) -> Zustand 1 0x801010b8 -> Grid-Modus @0x80117910
 *   -> Modus 0/2: DECIDE @0x80117944 / @0x80117a64, ANIMATE @0x80117994 (geteilt).
 *   AUSLOESER = ENGAGE-Decide [2] 0x80101eec (@0x80101f00-44):
 *       c = +0x90 (Kontaktbyte der SCA-Klemme FUN_8003b0a4: (Richtung>>4) + 8 + (Zelle.u1 & 3), @0x8003b4c0-dc)
 *       (c & 3) && ((((c & 0xf0) << 4) - +0x6a + 0x200) & 0xfff) < 0x400  ->  +0x4 = ((c & 1) + 9) << 8 | 1
 *       also Zelle u1 == 1 vor dem Zombie -> 0xA01 (+0x5 = 0x0A). Dasselbe Muster @0x80102df8 (Decide [8]) und
 *       @0x801054f8 (Decide [0x13]) — die einzigen drei Stellen in STAGE2.BIN (Wortsuche `addiu v0,v0,9 / sll v0,v0,8`).
 *   HOCHZIEHEN = ANIMATE [0x0A] 0x8010325c (einzige Tabellenreferenz @0x801179bc):
 *       +0x6 0: +0x6 = 1, +0x94 = 1, +0x95 = 0, +0x8f = 7                                  (@0x801032b0-e0)
 *       +0x6 1: anim_set(+0x170,+0x174,0,0x200); d = FUN_8001aa68((+0x90&0xf0)<<4, 0x10);
 *               +0x6a += d; d == 0 -> +0x6 = 2                                             (@0x801032e4-354)
 *       +0x6 2: +0x6 = 3, +0x94 = 0x16, +0x95 = 0, +0x8f = 7, +0x8c = 0x474,
 *               +0x6a = (+0x90&0xf0)<<4                                                     (@0x80103358-3bc)
 *       +0x6 3: +0x95 == 0x46 -> +0xb0 = 0x474, +0x1ba -= 1800                             (@0x801033cc-fc)
 *               anim_set(...) fertig -> +0x82 += 1, +0x38 -= 1800, +0x4 = 1, +0xb0 = 0,
 *               FUN_800245d8(0) (Vorschub +0x8c entlang +0x6a)                            (@0x80103424-78)
 *   PSX-Y waechst nach unten: -1800 / Band +1 = EIN Band HOCH (Spawn ROOM2020: Band 1 y=-1800, Band 0 y=0).
 *   Clip 0x16 der RE1.5-Aktionsbank (Typ 0x10/0x11/0x12/0x16/0x18 identisch, 159 Bilder): Wurzel f0 (0,-1817,0) ->
 *   f150 (1444,-3278,111) -> f158 (0,-1698,0) = Huefte hebt sich ueber die Kante und wandert 1444 nach vorn.
 *
 * WAS DER PORT VORHER TAT (gemessen, Dossier Abschnitt 1):
 *   RE1.5-KI: re15_enemy_ai_live_edge_fall (enemy_ai_common.c) hatte den Zustand ("EDGE-FALL", falsch gelesen) ohne
 *             den Vorschub FUN_800245d8 und ohne +0x1ba -= 1800 -> der Zombie endete UEBER dem Kanal und fiel bis
 *             y = +1800 (unter den Wasserboden), danach Band 2.
 *   RE2-KI:   das RE2-Gehirn (EMZ0.BIN) kennt kein Hochziehen (Wortsuche: kein +-1800 im Overlay) -> der Zombie lief
 *             dauerhaft gegen die Kanalwand.
 *
 * PORT-ABBILDUNG:
 *   - RE1.5-KI: re15_wasserzombie_hochziehen = ANIMATE [0x0A] byte-true (ersetzt den Aufruf in enemy_ai_common.c).
 *   - RE2-KI (Standard): das RE2-Gehirn hat dafuer nichts — Ziel ist RE1.5 (Nutzer: "wie im Original Resident Evil
 *     1.5"). re15_wasserzombie_re2_dispatch uebernimmt in re15_re2z_tick NUR den Zustands-Dispatch: steht der
 *     RE2-Gang (Zustand 1 / Sub 1, aufrecht) mit u1==1-Kante vorn (dieselbe Pruefung wie @0x80101f00-44), laeuft
 *     dieselbe RE1.5-Maschine; Ende = Wort 1 (wie @0x80103464-68) und der RE2-Executor im selben Bild
 *     (re15_re2z_exec_only, Muster SITZ-IMPORT 10D0). Pose/Uhr aus der RE1.5-Aktionsbank DES EIGENEN TYPS
 *     (re15_wasserzombie_bank_merken aus dem Hybrid-Lader, platform/pc/main.c). PORT-WAHL: Rig-Wechsel RE2 <-> RE1.5
 *     ohne Ueberblendung (anim_prev_valid = 0), weil die beiden Rigs die Knochen verschieden ordnen.
 *   - Schatten: die Zombie-Wurzel zeichnet ihn auf +0x1ba (`lh a1,442(v0)` @0x801004fc -> `jal 0x8001b064`); waehrend
 *     des Hochziehens liegt +0x1ba ab Bild 0x46 schon auf dem Steg -> re15_wasserzombie_schatten_y.
 *   - +0xb0 (= 0x474 / 0): kein Leser — Wortsuche ueber PSX.EXE und STAGE2.BIN findet keine Last auf Offset 176
 *     ausser einem Stapelpuffer (@0x8003a660 `lbu s2,176(v0)`, v0 = sp+...). Nicht abgebildet.
 */
#ifndef RE15_WASSERZOMBIE_R38_H
#define RE15_WASSERZOMBIE_R38_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"

#define RE15_WZ_SUB            0x0Au   /* +0x5 = 0x0A: ((u1&1)+9)<<8|1 @0x80101f30-44, ANIMATE[0x0A] @0x801179bc */
#define RE15_WZ_CLIP_DREH      0x01u   /* +0x94 = 1   (v1 = 1 @0x80103288, `sb v1,148` @0x801032c0) */
#define RE15_WZ_CLIP_KLETTER   0x16u   /* +0x94 = 0x16 (`ori v0,zero,0x16` @0x80103368) */
#define RE15_WZ_FRAC           7       /* +0x8f = 7 (@0x801032dc-e0 / @0x80103388-8c) */
#define RE15_WZ_RATE           0x200   /* anim_set a3 = 0x200 (@0x801032fc / @0x80103418) */
#define RE15_WZ_DREHSCHRITT    0x10    /* FUN_8001aa68 a1 = 0x10 (@0x80103310) */
#define RE15_WZ_TEMPO          0x474   /* +0x8c = 0x474 (`ori v0,zero,0x474` @0x80103398) */
#define RE15_WZ_BODENBILD      0x46    /* +0x95 == 0x46 (`ori v0,zero,0x46` @0x801033d0) */
#define RE15_WZ_BAND           1800    /* `addiu v0,v0,-1800` @0x801033f8 (+0x1ba) und @0x80103454 (+0x38) */

/* Zombie-Wurzel-Typen (0x801002b8 STAGE2 / FUN_80100424 STAGE1) mit derselben 43-Clip-Aktionsbank. */
int  re15_wasserzombie_typ(uint8_t type);

/* Ausloeser @0x80101f00-44: Kontakt u1&3, Kante im +-0x200-Kegel, u1 Bit 0 (= Hochziehen). 1 = ja. */
int  re15_wasserzombie_kante_vorn(const re15_actor_t *e);

/* ANIMATE [0x0A] 0x8010325c — beide KI-Varianten. */
void re15_wasserzombie_hochziehen(re15_actor_t *e);

/* RE2-KI: ersetzt in re15_re2z_tick den Zustands-Dispatch, solange der Zombie hochzieht (oder gerade anfaengt).
 * 1 = Dispatch erledigt (RE2-Tabelle NICHT laufen lassen), 0 = normaler RE2-Dispatch. */
int  re15_wasserzombie_re2_dispatch(int slot, re15_actor_t *e);

/* RE1.5-Aktionsbank je Typ (Hybrid-Lader, PC). Kopiert die Strukturen; keyframe_data muss weiterleben (eb->buf). */
void re15_wasserzombie_bank_merken(uint8_t type, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an);
void re15_wasserzombie_bank_vergessen(void);   /* Tests */

/* Renderer/Uhr/Schuss-Anker: 1 = RE2-KI-Zombie, dessen letzter EIGENER Tick die RE1.5-Maschine lief (Marke); dann
 * stehen in sk und an seine RE1.5-Aktionsbank (motion gueltig). Haengt NICHT am aktuellen Zustandswort: ein fremder
 * Schreiber (RE2-Partner-Domino 0x901 @0x80102E90-98) aendert die Pose im Original erst im eigenen Tick
 * (anim_set rechnet sie dort in den Teileblock +0x188, @0x8001f40c) — Nachbesserung 1, wasserzombie_r38.c. */
int  re15_wasserzombie_bank(const re15_actor_t *e, const re15_emd_skeleton_t **sk, const re15_emd_animation_t **an);

/* Schatten-Hoehe (FUN_8001b064 a1 = +0x1ba @0x801004fc): waehrend des Hochziehens +0x1ba statt y. 1 = gesetzt.
 * RE2-KI: nach demselben Stand wie re15_wasserzombie_bank. */
int  re15_wasserzombie_schatten_y(const re15_actor_t *e, int32_t *y);

/* Testhaken: RE2-Marker ruecksetzen (Raumwechsel im Test). */
void re15_wasserzombie_reset(void);
/* Messschiene: Slot (-1 = noch keins) und Lage des letzten Hochzieh-Endes, VOR Koerperstoss/Klemme jenes Bildes. */
int  re15_wasserzombie_letzte_landung(int32_t *x, int32_t *y, int32_t *z);

#endif /* RE15_WASSERZOMBIE_R38_H */
