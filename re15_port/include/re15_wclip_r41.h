/*
 * RE1.5 Rebuilt — Runde 41 Spur J: Spieler-Plc_motion(2,c) = W-Bank-Clip c in JEDEM Raum.
 *
 * Nutzer-Punkte (AUFTRAG.md 2026-10-23): "die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck
 * gewechselt wird zu Leon ist falsch in ROOM 3060" und "Am Ende von dieser Cutscene [ROOM3061] hat Elza auch erneut eine
 * falsche Animation". Beide Szenen enden mit Plc_motion(2,6) auf dem SPIELER:
 *   ROOM3060 sub03 @0x010BA `2e 01 00` Work_set Spieler, @0x010BE `3f 02 06 00`, Sleep 40 @0x010C2
 *   ROOM3061 sub02 @0x011DA `2e 01 00` Work_set Spieler, @0x011DE `3f 02 06 00`, Sleep 5  @0x011E2
 * (Zensus aller 240 RDTs: sonst kein Spieler-Plc_motion mit pc[1] = 2.)
 * Original (PSX.EXE, selbst disassembliert):
 *   Plc_motion-Handler @0x80041b90: @0x80041ba8 `sb a1,148(v0)` +0x94 = Clip, @0x80041bb0 `sb v1,4(v0)` +0x4 = 4,
 *     @0x80041bb4 `sb zero,6(v0)` +0x6 = 0, @0x80041bc4 `sb a2,5(v0)` +0x5 = pc[1] = 2 — auch fuer den Spieler.
 *   Spieler-Executor 0x80030660: @0x8003068c `lbu v0,-13735(v0)` (+0x5 = 0x800aca59), @0x8003069c Tabelle 0x80073e30,
 *     Eintrag [2] = 0x80050f00.
 *   Sub 0x80050f00: Phase 0 -> 1 (@0x80050f34 +0x6 = 1, @0x80050f44 +0x8f = 7, @0x80050f54 +0x95 = 0),
 *     Phase 1: @0x80050f88 `lw a0,368(v0)` (+0x170) / @0x80050f90 `lw a1,372(v0)` (+0x174) / @0x80050f94 anim_set;
 *     Clip-Ende -> @0x80050fec Phase 2 = HALTEN (kein anim_set mehr), Schleife nur mit +0x1c4 & 4 (@0x80051004/10).
 *   Kanal +0x170/+0x174 des Spielers = 0x800acbc4/0x800acbc8 = die W-Bank: Waffen-Lader FUN_80036b68
 *     @0x80036c04 `sw v0,-13372(at)` / @0x80036be4 `sw v0,-13368(at)` (PLW-Datei = Tabelle @0x800741e8[Modell] + Waffe:
 *     Modell 0..3 und 8..14 -> 76 = PL00Wxx, 4..7 -> 97 = PL04Wxx, 15 -> 118 = PL0FW00).
 * GEMESSEN am Original (DuckStation, MZD-Disc, Savestate-Reihe o3060_s2, Dossier §1): 6,5 s nach dem Sprung
 *   cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x170 = 0x801d7b64, +0x174 = 0x801d7708 (PLW-Puffer @0x801d7700), Marvin steht
 *   mit erhobenem Arm; der Port spielte PL00-Clip 6 (50 Bilder) = Marvin KNIET (Lauf m3060_vor).
 * Ende der Phase: anderer Plc-Befehl (Kommandowort/Clip ersetzt) oder Raumwechsel (Original o3060_s2_t04 in ROOM3020:
 *   cmd 0 / Sub 0 — der Executor 0x80030660 laeuft nicht mehr; gemessen ohne Pruefung: 1 Bild Leon mit W-Clip 6).
 * Bild-Zaehler: FUN_8001f3bc zaehlt +0x95 NACH dem Setzen der Pose hoch (@0x8001f618/@0x8001f61c, Ende -> 0 @0x8001f63c),
 *   +0x168 zeigt auf das gezeichnete Bild (@0x8001f36c) — Phase 2 mit +0x95 = 0 zeigt also Bild 9 = das letzte.
 * Bis Runde 41 galt die W-Bank-Regel nur in ROOM1141 (Runde 39 Spur J, dining_r39.c, dort mit Port-Wahl Messer-DRAW);
 * dieses Modul ist die allgemeine Regel fuer alle uebrigen Raeume (ohne Messer-Ersatz: das Original spielt Clip 6 der
 * W-Bank, gleich welche Waffe). Eingehaengt ueber die vorhandenen dining_r39-Haken (Beginn, Zeichner, Halten).
 */
#ifndef RE15_WCLIP_R41_H
#define RE15_WCLIP_R41_H

#define RE15_WCLIP_R41_SUB   2   /* Plc_motion pc[1] = 2 -> 0x80073e30[2] = 0x80050f00 (Kanal +0x170/+0x174 = W-Bank) */

/* Aus op_plc_motion (Spieler-Zweig, ueber re15_dining_r39_w_beginn): merkt den W-Bank-Clip bei pc[1] == 2. */
void re15_wclip_r41_beginn(int entity, int clip);
/* Der zu zeichnende W-Bank-Clip oder -1 (Zeichner main.c ueber re15_dining_r39_w_clip). */
int  re15_wclip_r41_clip(void);
/* Je Spielbild (ueber re15_dining_r39_tick): Phase 2 = letztes Bild halten (@0x80050fec); endet, sobald ein anderer
 * Plc-Befehl Kommandowort oder Clip ersetzt (Plc_motion @0x80041ba8/@0x80041bb0, Plc_dest @0x80041c14, Plc_ret). */
void re15_wclip_r41_halten(void);

#endif /* RE15_WCLIP_R41_H */
