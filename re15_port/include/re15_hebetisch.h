/*
 * RE1.5 Rebuilt — RUHE OBEN des Hebetischs in Irons' Buero (ROOM1150/1151, Prop 0, sub04).
 *
 * Runde 31 (Nutzer, 2026-09-29): "starte mit dem Aufnahme Dialog der items erst wenn das Modell
 * wirklich komplett hochgefahren ist". Dossier: analysis/befunde_runde31/hebetisch.md §2.
 *
 * ⛔ KEINE y-SCHRANKE. Bis Runde 30 gingen Sicherung und Granate bei Plattform-y <= -1100 auf —
 * Bild 134 der Fahrt, mitten im Hub (-1105), danach fuhr sie noch bis -1215 und setzte sich auf
 * -1205. Der Zeitpunkt haengt jetzt am SKRIPT-ZUSTAND von sub04 (ROOM1150.RDT, Datei-Offsets):
 *
 *   0x0FF2 Speed_set 2f 01 f6 ff    / 0x0FF6 For 0d 00 04 00 5b 00   91 x Add_speed (-10) -> -1215
 *   0x100C Speed_set 2f 01 01 00    / 0x1010 For 0d 00 04 00 0a 00   10 x Add_speed (+1)  -> -1205
 *   0x101A Sleep     09 0a 1e 00    30 Bilder      <- RUHE OBEN beginnt
 *   0x101E Work_set  2e 03 00 00
 *   0x1022 Se_on     36 02 0a 00 03 00 00 00 00 00 00 00
 *   0x102E Sleep     09 0a 0a 00    10 Bilder
 *   0x1032 Se_on     36 02 0c 00 03 00 00 00 00 00 00 00
 *   0x103E Speed_set 2f 01 0a 00
 *   0x1042 For       0d 00 04 00 5a 00   90 x Add_speed (+10)       <- Abfahrt
 *
 * Nur Add_speed (0x30) bewegt die Plattform, und zwischen dem letzten des Setzens (@0x1016) und
 * dem ersten der Abfahrt (@0x1048) steht keins. Steht also ein SCD-Thread mit seinem PC im
 * Fenster [@0x101A, @0x1042), dann RUHT die Plattform auf -1205 — "komplett hochgefahren".
 * Das erste Bild darin ist das erste Ruhebild: im Bild davor lief das letzte Add_speed des
 * Setzens, in diesem endet der For (Next @0x1018) und der Sleep @0x101A beginnt (Sleep schiebt den
 * PC um 1, @0x8003f3e8; Sleeping gibt immer 2 = Yield, LAB_8003f428 — scd_vm.c op_sleep/op_sleeping).
 *
 * Das Fenster wird NICHT als Zahl eingetragen, sondern je Raum ueber die 56 Byte @0x1010..@0x1047
 * (For-Setzen bis For-Abfahrt) im geladenen RDT gesucht — Zensus ueber alle 240 RDTs
 * (analysis/befunde_runde31/hebetisch_werkzeug/ruhe_signatur.py): genau ein Treffer in ROOM1150
 * (@0x1010 -> Fenster [0x101A,0x1042)) und in ROOM1151 (@0x0FEE -> [0x0FF8,0x1020)), sonst keiner.
 *
 * ⛔ Dass die Aufnahme HIER aufgeht, ist PORT-WAHL, KEINE ORIGINAL-ADRESSE — das Original hat im
 * Hebetisch keine Beute (Runde 30, sicherung.md §3). Belegt ist der Skript-Zustand, an dem sie
 * haengt, und dass die Aufnahme das Skript anhaelt: FUN_8001db28 Zustand 1 `lui t0,0xff00` @0x8001db98
 * / `or` @0x8001dbb8 / `sw` @0x8001dbc8 (g_pauseflags 0x800aca40 |= 0xFF000000), der SCD-Laeufer
 * FUN_8003f038 springt bei Bit 0x02000000 vorbei (`lui v1,0x200` @0x8003f044, `bne` @0x8003f04c).
 * Der Sleep 30 zaehlt also unter dem Dialog nicht weiter, die Plattform bleibt oben stehen.
 */
#ifndef RE15_HEBETISCH_H
#define RE15_HEBETISCH_H

#include <stdint.h>

/* Beim Registrieren eines Raum-RDT (scd_register_current_rdt): sucht die Signatur im rohen
 * Puffer und merkt sich das Ruhe-Fenster. Ohne Treffer (jeder andere Raum) ist das Fenster leer. */
void re15_hebetisch_raum_scan(const uint8_t *raw, int raw_size);

/* 1 = ein aktiver SCD-Thread hat seinen PC im Ruhe-Fenster [@0x101A, @0x1042): die Plattform ruht
 * oben auf -1205. 0 sonst (auch in jedem anderen Raum). */
int  re15_hebetisch_ruht_oben(void);

/* Datei-Offset (im Raum-RDT) des PC, der im Ruhe-Fenster steht, sonst -1 — fuer Logzeilen und
 * Riegel. Beginn/Ende des Fensters (Datei-Offsets) bzw. -1 ohne Treffer. */
long re15_hebetisch_ruhe_pc_off(void);
long re15_hebetisch_fenster_von(void);
long re15_hebetisch_fenster_bis(void);

/* Mess-Protokoll (nur PC, RE15_HEBETISCH_LOG=<datei>): je Spielbild Plattform-y, Ruhe, PC,
 * Zustand der Aufnahme-FSM. Kein Verhalten. */
void re15_hebetisch_protokoll(void);

#endif /* RE15_HEBETISCH_H */
