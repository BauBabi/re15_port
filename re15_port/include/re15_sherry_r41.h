/*
 * RE1.5 Rebuilt — Runde 41 Spur K "stage3szenen_b": Sherry als SPIELFIGUR (Modell 0x0F, PL0F.PLD) ab ROOM30C1.
 *
 * Belege (Dossier analysis/befunde_runde41/K_stage3szenen_b.md):
 *   ROOM3061 sub02 @0x011F6 `24 10 0f 00` = Save work_vars[0x10] = 0x0F, @0x011FA `47 03` Aot_on 3 -> ROOM30C1 Cut 10.
 *   Raumlader FUN_800396fc @0x80039760-8c: Modell-Nibble DAT_800aca5c != work_vars[0x10] (0x800b0ff0) ->
 *   FUN_800314b0 (Spielermodell neu laden).
 */
#ifndef RE15_SHERRY_R41_H
#define RE15_SHERRY_R41_H
#include <stdint.h>

/* MESSHAKEN RE15_WORKVAR_AT="<idx>:<wert>[,<idx>:<wert>...]@<bild>[#<raum-hex>]" (env-gegatet, kein Spielverhalten):
 * schreibt EINMAL g_scd.work_vars[idx] = wert, sobald das Bild im (optionalen) Raum erreicht ist. Gebraucht, um die
 * Spielfigur Sherry (work_vars[0x10] = 0x0F, wie ROOM3061 @0x011F6) ohne die Kriech-Szene herzustellen; danach nimmt ein
 * echter Tuerweg (RE15_FIRE_AOT=3@..#3061 = die Schacht-Tuer @0x00EF8) den Raumlader wie im Spiel. */
void re15_sherry_r41_messhaken(void);

#endif /* RE15_SHERRY_R41_H */
