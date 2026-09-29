/*
 * fx_plattform_pc.h — Runde 34 (Granaten), Spur C (Plattform): die fensterlos pruefbaren Teile
 * des PC-Effektpfads. Dossier: analysis/befunde_runde34_granaten/bau_c.md, Plan BAUPLAN §3.3.
 *
 * Alles hier ist bewusst ohne SDL: die Unit-Sonde probe_r34_plattform linkt diese Datei gegen
 * re15_engine + re15_test_support und prueft Takt, Haken-Bindung, Ton-Weiche, Licht-Latch und
 * die TEX.TIM-Effektseiten, ohne ein Fenster zu oeffnen. main.c ruft die Funktionen an den
 * Stellen, die der Original-Hauptlauf FUN_8001c6e8 vorgibt (Adressen je Funktion).
 */
#ifndef FX_PLATTFORM_PC_H
#define FX_PLATTFORM_PC_H

#include <stdint.h>
#include <stddef.h>
#include "re15_light.h"

/* ===== C1 — ESP-Takt HINTER dem Spielschritt (E10) =========================================
 * Original-Hauptlauf (RE1.5 PSX.EXE, selbst disassembliert):
 *   8001ce04  jal 0x8001a50c      Gegner
 *   8001ce0c  jal 0x80031c44      Spieler inkl. Waffen-FSM (spawnt Muendung/Huelse/Granate)
 *   8001ce2c  jal 0x80019e20      ESP-Tick
 *   8001ce34  jal 0x8001db28      Item-Modal
 *   8001ce60  lbu v0,21336(v0)    Licht-Latch-Leser (0x800b5358)
 * Im Port lief der ESP-Tick im SCD-30-Hz-Zweig VOR re15_game_step — jede im Spielschritt
 * gespawnte Partikel tickte ein Bild zu spaet. Neu: der SCD-Zweig gibt den Takt nur FREI
 * (re15_pc_fx_takt_setzen(1) an der alten Stelle, 0 zu Beginn der Zweig-Kette), und
 * re15_pc_fx_takt() laeuft direkt hinter re15_game_step: erst der RE1.5-ESP-Tick, dann die
 * RE2-FX-Pumpe (RE2: Gegner-Schleife 0x800267c0-0x80026930 vor `jal 0x8001d300` @0x80026980). */
void re15_pc_fx_takt_setzen(int frei);
/** Einmal je freigegebenem Bild: ESP-Tick + RE2-FX-Tick; Freigabe verbraucht. 1 = lief. */
int  re15_pc_fx_takt(void);

#endif /* FX_PLATTFORM_PC_H */
