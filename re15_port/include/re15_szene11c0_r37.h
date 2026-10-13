/*
 * RE1.5 Rebuilt — Runde 37 Spur O "szene11c0": Ada/Marvin-Szene ROOM11C0 Cut 13 nach Irons' Tod
 * (Programm 4 k_p_11c0 in engine/src/irons_tod_1150.c). Dossier: analysis/befunde_runde37/O_szene11c0.md.
 *
 * Diese Datei traegt Rechenhilfe, den Port-Takt "Ada folgt Marvin" (Nachtrag 12) und den Mess-Haken; die
 * Choreografie selbst ist das Laufprogramm k_p_11c0 (VERTRAG Runde 37 §1.4: Spur O aendert Programm 4 an Ort und Stelle).
 */
#ifndef RE15_SZENE11C0_R37_H
#define RE15_SZENE11C0_R37_H

#include <stdint.h>

/* Die Figuren des Programms 4: Ada = Aktor 1 (Work_set(2,0), ROOM11C0 sub00 @0x01770 Record-Platz 0), Marvin = Aktor 4
 * (Work_set(2,3); sein Sce_em_set steht hinter den zwei Gorilla-Records sub00 @0x01784/@0x01798, slots_zuweisen). */
#define RE15_S11C0_ADA_AKTOR     1
#define RE15_S11C0_MARVIN_AKTOR  4

/* NACHTRAG 12 (NUTZER-VORGABE, woertlich: "wenn Ada sagt: "Marvin!" kann sie sich gerne den weglaufenden Marvin nach
 * drehen"): Signal-Bit Bank 5 Wort 0 (Raum-Scratch, beim Raum-Init geloescht FUN_8003ecec @0x8003ed74; in ROOM11C0
 * benutzt die RDT nur die Bits 0/1 = Ankunfts-Bits sub05 @0x01C3E / sub06 @0x01C50, Zensus im Dossier). Solange es steht,
 * folgt Ada dem laufenden Marvin:
 *   KOPF  = RE2 Plc_neck Modus 5 (Handler @0x80056e5c: Modus 5 -> Weltpunkt-Bit 0x04 @0x80056f00, +0x170 = Figur,
 *           +0x172 = 0x8000 @0x80056f70-74): die RE2-Kopf-FSM FUN_800177C0 holt bei +0x172 == 0x8000 (`ori v1,zero,0x8000`
 *           @0x800178a0, `bne` @0x800178a4) in JEDEM Bild den Kopf-Ort dieser Figur als Ziel (@0x800178ac..@0x80017940).
 *           RE1.5 hat diesen Modus nicht (Handler @0x80041e98: Modus >= 5 setzt nur 0x80) -> der Port-Takt schreibt den
 *           Kopf-Ort Marvins je Bild in Adas Weltpunkt (+0x160/+0x164), solange ihr Kopf im Weltpunkt-Modus steht.
 *   KOERPER = waehrend Adas Drehen (Plc_dest Modus 9 -> Executor-Sub 9, re15_npc_sub_turn) zeigt der Steuerpunkt
 *           +0x1bc/+0x1be auf Marvins jeweiligen Ort; gedreht wird mit der Rate der Engine (Kegel 0x60 @0x80076c41),
 *           bis sie ihm zugewandt ist (Ankunfts-Bit wie jede Drehung). */
#define RE15_S11C0_SIG_FOLGEN    13

/* Gier (0..4095), die von (vx,vz) auf den Punkt (zx,zz) zeigt = die Peilung, auf die das Drehen der Engine
 * einschwenkt: FUN_8001aac4 (Steuer-Kern, Port re15_enemy_steer_point) nimmt bearing = FUN_8001a6d4(self,
 * ziel) = re15_atan2_q12(dz,dx) - 0x400 (actor_locomotion.c: "+Z-zero-Konvention = Rohwert + 0x400").
 * Dieselbe Form benutzt die Kopf-FSM fuer den Weltpunkt (skeleton_common.c wYaw, FUN_8003790c). */
int re15_s11c0_gier(int32_t vx, int32_t vz, int32_t zx, int32_t zz);

/* Vorzeichenbehafteter kuerzester Winkelabstand b - a in (-2048, 2048]. */
int re15_s11c0_winkel_diff(int a, int b);

/* Port-Takt der Szene, je Bild aus re15_irons_tod_tick (nur ROOM11C0 waehrend Programm 4, VOR den SCD-Faeden):
 * "Ada folgt Marvin" (RE15_S11C0_SIG_FOLGEN) und der Mess-Haken. */
void re15_s11c0_takt(void);

/* MESS-HAKEN RE15_S11C0_LOG=<pfad> (env-gegatet, kein Spielverhalten): eine Zeile je Spielbild, solange
 * Programm 4 in ROOM11C0 laeuft — Untertitel (aktiv/Id), je Figur (A = Aktor 1 Ada, M = Aktor 4 Marvin)
 * Ort, Gier, Clip, Unter-Modus, Kopf-Flags +0x1b8, Kopf-Zielpunkt +0x160/+0x164, Kopf-Akku (part+0x94),
 * die Gier zum Partner und der Kopf-Ort (head_world = Ursprung des Kopf-Blicks der Kopf-FSM, skeleton_common.c).
 * Aufgerufen aus re15_s11c0_takt. */
void re15_s11c0_mess_takt(void);

#endif /* RE15_SZENE11C0_R37_H */
