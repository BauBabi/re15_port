/*
 * RE1.5 Rebuilt — Runde 37 Spur O "szene11c0": Ada/Marvin-Szene ROOM11C0 Cut 13 nach Irons' Tod
 * (Programm 4 k_p_11c0 in engine/src/irons_tod_1150.c). Dossier: analysis/befunde_runde37/O_szene11c0.md.
 *
 * Diese Datei traegt nur Rechenhilfe und Mess-Haken; die Choreografie selbst ist das Laufprogramm k_p_11c0
 * (VERTRAG Runde 37 §1.4: Spur O aendert Programm 4 an Ort und Stelle).
 */
#ifndef RE15_SZENE11C0_R37_H
#define RE15_SZENE11C0_R37_H

#include <stdint.h>

/* Gier (0..4095), die von (vx,vz) auf den Punkt (zx,zz) zeigt = die Peilung, auf die das Drehen der Engine
 * einschwenkt: FUN_8001aac4 (Steuer-Kern, Port re15_enemy_steer_point) nimmt bearing = FUN_8001a6d4(self,
 * ziel) = re15_atan2_q12(dz,dx) - 0x400 (actor_locomotion.c: "+Z-zero-Konvention = Rohwert + 0x400").
 * Dieselbe Form benutzt die Kopf-FSM fuer den Weltpunkt (skeleton_common.c wYaw, FUN_8003790c). */
int re15_s11c0_gier(int32_t vx, int32_t vz, int32_t zx, int32_t zz);

/* Vorzeichenbehafteter kuerzester Winkelabstand b - a in (-2048, 2048]. */
int re15_s11c0_winkel_diff(int a, int b);

/* MESS-HAKEN RE15_S11C0_LOG=<pfad> (env-gegatet, kein Spielverhalten): eine Zeile je Spielbild, solange
 * Programm 4 in ROOM11C0 laeuft — Untertitel (aktiv/Id), je Figur (A = Aktor 1 Ada, M = Aktor 4 Marvin)
 * Ort, Gier, Clip, Unter-Modus, Kopf-Flags +0x1b8, Kopf-Zielpunkt +0x160/+0x164, Kopf-Akku (part+0x94)
 * die Gier zum Partner und der Kopf-Ort (head_world = Ursprung des Kopf-Blicks der Kopf-FSM, skeleton_common.c).
 * Aufgerufen aus re15_irons_tod_tick. */
void re15_s11c0_mess_takt(void);

#endif /* RE15_SZENE11C0_R37_H */
