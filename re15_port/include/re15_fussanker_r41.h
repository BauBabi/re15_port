/*
 * RE1.5 Rebuilt — Runde 41 Spur J: Glied-Anker FUN_800369f8 der Executor-Bits +0x1c4 & 1 / & 2.
 *
 * Nutzer-Punkt (AUFTRAG.md 2026-10-23): "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von
 * Sherry in den Schacht nicht." Sherry (Typ 0x4B, em0) kriecht in ROOM3061 sub04 mit RBJ rec1 Clip 1 (75 Bilder) und
 * Clip 2 (60 Bilder, Schleife) und schaltet je Takt das Ankerglied um (Plc_flg SET = @0x80042008 `sh a2,452(v0)`):
 *   @0x012EC SET 1 (22 B.), @0x012F4 SET 2 (22), @0x012FC SET 1 (11); For 2 x [@0x0130E SET 4 (8), @0x01316 SET 6 (26),
 *   @0x0131E SET 5 (24), @0x01326 SET 4 (2)] — 20+22+22+11 = 75 = Clip 1, 8+26+24+2 = 60 = Clip 2.
 * Zensus aller 240 RDTs: Plc_flg mit Bit 1/2 gibt es NUR dort; das Flagbyte von Plc_motion (pc[3]) ist in allen 1359
 * Aufrufen 0.
 *
 * Original (PSX.EXE, selbst disassembliert, Dossier analysis/befunde_runde41/J_stage3szenen_a.md §2.2):
 *   NPC-Executor State 4 @0x80050be8 (Spieler 0x80030660 @0x800306cc..f8 gleich gebaut):
 *     @0x80050c6c `andi v0,v0,0x1` / @0x80050c78 `jal 0x800369f8` a0 = 0, a1 = 0  (Kette 0)
 *     @0x80050c94 `andi v0,v0,0x2` / @0x80050ca0 `jal 0x800369f8` a0 = 0, a1 = 1  (Kette 1, @0x80050ca4 `ori a1,zero,0x1`)
 *     beide NACH der Sub-Verteilung (@0x80050c34 `jalr`), also mit dem Bild, das der Sub gerade gesetzt hat.
 *   FUN_800369f8(a0 = Modus, a1 = Kette):
 *     @0x80036a30 `lw s3,392(v0)` s3 = Entity+0x188 (Part-Feld, Stride 0xAC)
 *     @0x80036a34 `jal 0x80068098` RotMatrix(Entity+0x68) -> Entity-Matrix +0x20 (Translation = +0x34/+0x38/+0x3c)
 *     @0x80036a48 `jal 0x80022da0` E x Part 0 lokal (s3+24) -> sp+16;  @0x80036a58 x Part 1 lokal (s3+196)
 *     @0x80036a60..70 s3 += a1*0x204 + 0x2b0  -> Kette 0: Parts 2,3,4  /  Kette 1: Parts 5,6,7
 *     @0x80036a7c (s3-320), @0x80036a8c (s3-148), @0x80036a9c (s3+24): drei weitere 0x80022da0 = FESTE Kette
 *       E -> 0 -> 1 -> 2 -> 3 -> 4  bzw.  E -> 0 -> 1 -> 5 -> 6 -> 7  mit den LOKALEN Matrizen dieses Bildes
 *     Modus 0 (@0x80036aa4 `bne s2,zero`): @0x80036ab4..ac8 `lw v1,36(sp)` / `lw a0,84(s3)` / `subu` / `subu` / `sw 52(a1)`
 *       Entity.x -= (Kette.x - Part[4|7]+84),  @0x80036ad4..aec dasselbe fuer z mit +92/+60.
 *       Part+84/+92 = Welt-Translation dieses Parts aus dem letzten Zusammensetzen vor dem Zeichnen
 *       (FUN_8001e8c8: Part-Welt = Eltern-Welt x Part lokal ueber 0x80022da0; Haupt-Loop: SCD @0x8001cdec ->
 *       Entities @0x8001ce04 -> Spieler @0x8001ce0c -> Objekte @0x8001ce14 -> ... Zusammensetzen/Zeichnen danach).
 *     Modus != 0 (@0x80036af0..b44) nur Strecke -> +0x8c; von den Executoren nie gerufen (a0 = 0).
 *   0x80022da0 = GTE-CompMatrix: Rotation = Eltern-R x lokal-R (MVMVA sf=1 je Spalte, @0x80022df0/e38/e84),
 *     Translation = TR + (Eltern-R x lokal-t) >> 12 (MVMVA cv=TR @0x80022eec, MAC1..3 -> swc2 @0x80022ef8..f00).
 *
 * Port: der alte Zweig in re15_npc_executor (Bit 1 -> re15_clip_root_motion_delta = Keyframe-Wurzelweg der eigenen Bank,
 * Bit 2 "deferred") ist durch diesen Anker ersetzt. "Letztes Zusammensetzen" = re15_fussanker_r41_merken() am Ende von
 * re15_game_step (nach KI, Spieler, Objekten; vor dem Zeichnen und vor dem SCD-Takt des naechsten Bildes).
 * PORT-WAHL: kein gemerkter Vorbild-Stand (Aktor erst in diesem Bild in State 4, Raumwechsel) -> keine Verschiebung.
 */
#ifndef RE15_FUSSANKER_R41_H
#define RE15_FUSSANKER_R41_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"

#define RE15_FUSSANKER_R41_KETTE_ENDE0   4     /* @0x80036a60..9c Kette 0 endet an Part 4 (s3 = 0x2b0 = 4*0xAC + 0) */
#define RE15_FUSSANKER_R41_KETTE_ENDE1   7     /* Kette 1: s3 = 0x204 + 0x2b0 = 7*0xAC (Parts 5,6,7) */

/* Executor-Bit: FUN_800369f8(0, kette) fuer den Aktor e (State 4). */
void re15_fussanker_r41_anwenden(re15_actor_t *e, int kette);
/* Ende des Spielschritts: Welt-Lage von Part 4 und 7 (Hierarchie wie das Zusammensetzen) je State-4-Aktor merken. */
void re15_fussanker_r41_merken(void);

/* Mess-/Test-Hilfen (gleiche Rechnung wie oben). 1 = ok. */
int re15_fussanker_r41_kette(const re15_actor_t *e, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                             int kette, int32_t out[3]);                 /* feste Kette E-0-1-(2|5)-(3|6)-(4|7) */
int re15_fussanker_r41_teil(const re15_actor_t *e, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                            int part, int32_t out[3]);                    /* Part-Welt ueber die Eltern-Kette */
/* dasselbe mit Welt-Rotation (Part+64 des Originals: Eltern-Welt x lokal ueber 0x80022da0), fuer Super_set (0x48). */
int re15_fussanker_r41_teil_matrix(const re15_actor_t *e, const re15_emd_skeleton_t *sk,
                                   const re15_emd_animation_t *an, int part, int32_t R[9], int32_t t[3]);
/* Kanal des State-4-Subs (Sub 0 +0x180/+0x184 RBJ, 1 +0x84/+0x16c Loco, 2 +0x170/+0x174 eigene Bank, 3 Victim). */
int re15_fussanker_r41_kanal(const re15_actor_t *e, const re15_emd_skeleton_t **sk, const re15_emd_animation_t **an);
/* Testhaken: gemerkten Stand eines Slots setzen/lesen (1 = gueltig). */
int  re15_fussanker_r41_alt(int slot, int kette, int32_t out[3]);
void re15_fussanker_r41_reset(void);

#endif /* RE15_FUSSANKER_R41_H */
