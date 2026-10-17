/*
 * RE1.5 Rebuilt — Runde 38 Spur H "elzatoene": Toene in Elzas Szenen ROOM1041 und ROOM1021.
 *
 * Auftrag (woertlich: analysis/befunde_runde38/AUFTRAG.md, Belege: analysis/befunde_runde38/H_elzatoene.md):
 *   P1 ROOM1041  "bei *Gunshot* fehlt der Sound fuer einen Gunshot" -> der Platzhaltertext msg 3 "*gunshot*" wird zum
 *                SCHUSS-TON, das Textfenster entfaellt.
 *   P2 ROOM1021  "Wenn Roy ... auf John zulaeuft soll er schon irgendwie ein Zombie stoehnen machen" -> Zombie-Stoehnen
 *                der Raumbank snd1, wenn Roys Gang beginnt.
 *   P3 ROOM1021  "Wenn John ... auf Roy schiesst, muss auch ein Schusssound kommen" -> der Schuss-Satz der Mini-Bank
 *                ELZASE.VBS (Runde 36 P4: RE2 ROOM40A0 Satz 0x08 = Schuss der PARTNERIN Ada auf den Alligator, Se_on
 *                Art 2 @0x02896; eine Aufnahme, die unter keiner RE1.5-Waffe vorkommt) ist jetzt der RE2-Skript-
 *                Pistolenschuss aus dem GEGNERFELD, ROOM4040 Satz 0x0A (Annette, Se_on Art 3 Index 1 @0x014E4) — Johns
 *                Art: RE1.5 Work_set 2 = Gegnerfeld (@0x80040d60) = RE2 Art 3 (@0x80055988/@0x800564b8). Die Welle ist
 *                die Aufnahme des RE1.5-Super-Redhawk ARMS07 SE 0 (Korrelation 0,98). Werkzeug tools/r36_l/
 *                re2_elza_se_cut.py, gen/re2_elza_bank.inc; Herleitung analysis/befunde_runde38/H_elzatoene.md §8.
 *                Die Anker/Zeitform der Runde 36 (@0x020CE/@0x020D6 +1) bleiben in engine/src/elza_r36.c.
 *
 * MECHANIK (Muster engine/src/elza_r36.c): ANKER = Datei-Offset + 8-Byte-Signatur im rohen RDT, einmal je Raum gesucht
 * (Haken scd_vm.c scd_register_current_rdt). Ein Anker vor einem Opcode meldet sich ueber den Opcode-Haken; der
 * Text-Anker 1041 ersetzt den Message_on (Haken in op_message_on, Muster re15_leiche_message_on: pc += 4, weiter).
 * Die Raum-Id ist der Szenario-Schalter (ROOM1041/1021 werden nur bei Elza geladen: FUN_800396fc @0x800397e4 `srl a0,a0,31`
 * + @0x800397ec `addu` = Leon-RDT + Elza-Bit).
 */
#ifndef RE15_ELZATOENE_R38_H
#define RE15_ELZATOENE_R38_H

#include <stdint.h>

#define RE15_ELZATOENE_RAUM_1021  0x1021u
#define RE15_ELZATOENE_RAUM_1041  0x1041u

/* ---- P1 ROOM1041 sub11 (@0x021EA..0x0223C; AOT 8 main00 @0x01178 sce 3 auto, nur Ck(4,0xF1)==0 @0x0116C und
 * Ck(3,0xAD)==0 @0x01174) -------------------------------------------------------------------------------------
 * @0x02200 `2b 03 00 00` Message_on 3 (Maske 0, kein Einfrieren) = msg 3 @0x022FB "*gunshot*" — der einzige
 * Geraeusch-Platzhalter in 1229 RE1.5- und 2315 RE2-Nachrichten. Folge @0x02204 `2e 01 00 00` Work_set(1,0).
 * Ton im selben Takt wie der Platzhalter (Verzug 0): an seiner Stelle zeigt das Original den Text. */
#define RE15_ELZATOENE_1041_OFF       0x02200u
#define RE15_ELZATOENE_1041_MSG       3

/* ---- P2 ROOM1021 sub02 -------------------------------------------------------------------------------------
 * @0x020AE `2e 02 01 00` Work_set(2,1) (Roy = Gegner 1, Sce_em_set Slot 1 @0x01E0A), @0x020B2 `40 00 04 22 4e ef 42 e6`
 * Plc_dest -> (-4274,-6590) = Johns Platz: Roys Gang beginnt. Original-Form (STAGE1.BIN FUN_801021f8, Engage-Gang):
 * ein gehender Zombie stoehnt beim EINTRITT in den Gang (Phase +0x6 == 0 @0x80102210/@0x80102218), nie waehrend des
 * Gangs: @0x80102270 jal rng / @0x80102278 andi 3 / @0x8010227c bne (1/4) / @0x80102284 jal rng / @0x8010228c andi 1 /
 * @0x80102290 beq / @0x80102294 `ori a0,zero,0x5` / @0x80102298 `ori a0,zero,0x4` / @0x8010229c jal 0x800453d0.
 * NUTZER-VORGABE "soll stoehnen": der 1/4-Wurf entfaellt. PORT-WAHL des Muenzwurfs 4/5: SE 4 — SE 5 ist schon Roys
 * Sterbe-Ton (FUN_80106c18 @0x80106da8, Runde 36 P5). ROOM1021 snd1 (EDT @0x145A8): SE 4 = `00 00 5b 15` -> VAG 5
 * (10304 B, 11025 Hz, 1,41 s), dieselbe Welle als SE 4 in 26 Zombie-Raeumen der Bank-Familie 1000..1251. */
#define RE15_ELZATOENE_1021_OFF       0x020B2u
#define RE15_ELZATOENE_ROY_STOEHN_SE  4

/* Aktionen (Pruefhaken). */
#define RE15_ELZATOENE_AKT_SCHUSS_1041   1
#define RE15_ELZATOENE_AKT_ROY_STOEHNEN  2
#define RE15_ELZATOENE_AKT_N             3

/* HAKEN scd_vm.c scd_register_current_rdt: Anker des Raums im rohen RDT suchen (Signaturpruefung). */
void re15_elzatoene_r38_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
/* HAKEN scd_vm.c Opcode-Schleife: vor jedem ausgefuehrten Opcode (Wachposten g_re15_elzatoene_anker_n). */
extern int g_re15_elzatoene_anker_n;
void re15_elzatoene_r38_pc(const unsigned char *pc);
/* HAKEN scd_vm.c op_message_on (hinter dem Stimmen-Riegel): 1 = diese Nachricht ist ein Ton -> pc += 4, weiter. */
int  re15_elzatoene_r38_message_on(const unsigned char *pc);

/* Pruefhaken. */
int      re15_elzatoene_r38_zaehler(int aktion);
int      re15_elzatoene_r38_letzter_takt(int aktion);   /* VM-Takt (g_scd.tick_count) der letzten Ausfuehrung */
unsigned re15_elzatoene_r38_anker_off(int i);
void     re15_elzatoene_r38_reset(void);

#endif /* RE15_ELZATOENE_R38_H */
