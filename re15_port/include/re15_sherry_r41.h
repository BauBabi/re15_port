/*
 * RE1.5 Rebuilt — Runde 41 Spur K "stage3szenen_b": Sherry als SPIELFIGUR (Modell 0x0F, PL0F.PLD) ab ROOM30C1.
 * Dossier: analysis/befunde_runde41/K_stage3szenen_b.md (K1.3-K1.5). Alle Belege selbst disassembliert (PSX.EXE):
 *
 *   ROOM3061 sub02 @0x011F6 `24 10 0f 00` = Save work_vars[0x10] = 0x0F, @0x011FA `47 03` Aot_on 3 -> ROOM30C1 Cut 10;
 *   ROOM3091 sub07 @0x01D9C/@0x01DA8 `24 10 04/05 00` = zurueck auf Elza.
 *   Raumlader FUN_800396fc @0x80039760-8c: Nibble(DAT_800aca5c) != work_vars[0x10] -> `jal 0x800314b0` @0x80039788.
 *   FUN_800314b0 bindet ALLE VIER PLD-Teile: EDD @0x8003154c (+0x16c), EMR @0x80031578 (+0x84), MD1 @0x800315a0,
 *     TIM @0x800315b0; Skelett-Aufbau @0x80031614/@0x80031624; Waffe @0x80031690 = DEBUG.BIN-Tabelle @0x800c00d4
 *     [Modell], **Modell 0x0F -> Waffe 0 @0x800316b8/@0x800316c4**; Wunden FUN_80037c1c @0x800316c8; CORE-Bank
 *     FUN_800440c4(Modell) @0x800316e8; Waffenlader FUN_80036b68 @0x800316f0.
 *   FUN_80036b68: Datei = u16 @0x800741e8[Modell] + Waffe (`4c x4, 61 x4, 4c x7, 76 00` -> 0x0F = 118 = PL0FW00.PLW);
 *     W-Bank EDD @0x80036be4 (+0x174) / EMR @0x80036c04 (+0x170); **Waffe 0 -> kein Hand-/Waffen-Mesh
 *     (`beq v1,zero,0x80036da0` @0x80036c08)**.
 *   Zielen nur mit Waffe != 0: Stehen @0x80032014, Gehen @0x80032430, Rennen @0x80032720, Drehen @0x80032a80,
 *     Rueckwaerts @0x80032cb4 (`lbu DAT_800aca5d` + beq/bne zero vor `ori v0,zero,0x701`).
 *   Bewegung je Modell [Tempo, Drehung] (Index DAT_800aca5c*2): GEHEN @0x80073ea4, RUECK @0x80073ec4, DREHEN
 *     @0x80073ee4, RENNEN @0x80073f24 — Zeile 15 (Sherry) s. sherry_r41.c.
 */
#ifndef RE15_SHERRY_R41_H
#define RE15_SHERRY_R41_H
#include <stdint.h>

#define RE15_SHERRY_R41_MODELL 0x0F   /* DAT_800aca5c-Nibble der Spielfigur Sherry (@0x80073f70[15] = PL0F.PLD) */

/* MESSHAKEN RE15_WORKVAR_AT="<idx>:<wert>[,<idx>:<wert>...]@<bild>[#<raum-hex>]" (env-gegatet, kein Spielverhalten):
 * schreibt EINMAL g_scd.work_vars[idx] = wert, sobald das Bild im (optionalen) Raum erreicht ist. Gebraucht, um die
 * Spielfigur Sherry (work_vars[0x10] = 0x0F, wie ROOM3061 @0x011F6) ohne die Kriech-Szene herzustellen; danach nimmt ein
 * echter Tuerweg (RE15_FIRE_AOT=3@..#3061 = die Schacht-Tuer @0x00EF8) den Raumlader wie im Spiel. */
void re15_sherry_r41_messhaken(void);

/* Gebundenes Spielermodell (Nibble von DAT_800aca5c nach dem Raumlader, `sb v0,0(s0)` @0x8003978c); -1 = unbekannt. */
int  re15_sherry_r41_modell(void);
/* Boot/Laden: Modell setzen, ohne Waffen-Logik. */
void re15_sherry_r41_modell_init(int modell);
/* Modellwechsel (Raumlader -> FUN_800314b0): Modell setzen + Waffe wie @0x800316c4 (0x0F -> 0). Beim Verlassen von 0x0F
 * PORT-WAHL: die vorher ausgeruestete Waffe kommt zurueck (wie der Westen-Weg des Ports; Original setzt hier die
 * DEBUG.BIN-Tabelle @0x800c00d4[Modell] = 1, Messer — Dossier K1 OFFEN). */
void re15_sherry_r41_modell_setzen(int modell);
/* 1, solange Sherry die Figur ist: Ausruesten gesperrt (RE2-Retail: Sherry fuehrt keine Waffe; das Original wuerde
 * Datei 118+Waffe laden = keine PLW, Beta-Luecke). */
int  re15_sherry_r41_waffe_gesperrt(void);

/* Bewegungs-Tabellen je Modell (Tempo/Drehung). tab: 0 GEHEN @0x80073ea4, 1 RUECK @0x80073ec4, 2 DREHEN @0x80073ee4,
 * 3 RENNEN @0x80073f24; feld: 0 Tempo, 1 Drehung. Liefert fuer Modell 0x0F den Tabellenwert, sonst `standard`
 * (Leon/Elza bleiben auf den bisherigen Port-Werten — Abweichung Elza 70/65/210 als OFFEN im Dossier). */
int  re15_sherry_r41_tabelle(int tab, int feld, int standard);

#endif /* RE15_SHERRY_R41_H */
