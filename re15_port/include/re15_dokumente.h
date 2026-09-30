/*
 * RE1.5 Rebuilt — VIER NEUE DOKUMENTE als Welt-Props mit Aufhebe-Zone und FILE-Leser
 * (Runde 34 Nacht, Spur E). Vorbild in allen Punkten: Irons Diary (include/re15_irons_tisch.h).
 * Dossier mit allen Messungen: analysis/befunde_runde34_nacht/E_dokumente.md
 * (Selbstpruefung der Konstanten: Abschnitt 9.0, re15_port/tools/r34n_e/selbstpruefung.py).
 *
 * ⛔ WARUM ES DIESE DATEI GIBT — NUTZER-VORGABE (Runde 34 Nacht, woertlich, AUFTRAG.md Z. 20-58):
 *   "Dann möchte ich verschiedene Dokumente an verschiedenen Positionen hinterlegen mit
 *    verschiedenen Texten: Einmal auf der sitzenden Leiche in ROOM 1050, vor dem Rolltor. [...]
 *    in ROOM 1000 auf der Bank (Siehe elliot.bmp) [...] in Room 1020 [...] an der Stelle von
 *    Marvins Desk - markiert in marvin.bmp [...] im Room 1010 [...] Ort in etwa markiert in
 *    interrogation.bmp".
 * RE1.5 hat kein Dokument-System (re15_files.h: statische FILE-Tabelle, kein Schreiber) —
 * massgeblich ist RE2 Retail (memory reai-v2-beta-zu-retail); der Aufnahme-Pfad (Leser,
 * Meldung, Abraeumen, FILE-Liste) ist der fertige Runde-30-Weg und bleibt unveraendert.
 * Kein Original-Skript nutzt die Ids 0x49..0x4C (id_zensus.py: 416 Item_aot_set- und 52
 * Aot_reset-Muster in 240 RDTs, 0 Treffer). OHNE Asset-Patch: die RDTs bleiben byte-true.
 *
 * WAS DAS ORIGINAL VORGIBT (selbst nachdisassembliert, info/Re1.5/PSX.EXE; Dossier 9.0):
 *   Aktions-Scan FUN_80042bac: aufsteigend ueber die Zeigertabelle 0x800AC9B0 (Index = Slot,
 *     Aot_set @0x80040548/@0x80040584), im Aktionsmodus nach dem ERSTEN Treffer Ruecksprung
 *     (`j 0x80043028` @0x80042f94) -> je Druck feuert der KLEINSTE treffende Slot.
 *   Pruefpunkt 620 voraus: `ori v0,zero,0x26c` @0x80042bd0.
 *   Dokument-Nr = Item-Id - erste Dokument-Id (RE2 `sltiu v0,a3,0x68` @0x80071bbc,
 *     `addiu a0,a3,-104` @0x80071d04; RE1.5-Basis 0x48 = u8 @0x800c7370).
 */
#ifndef RE15_DOKUMENTE_H
#define RE15_DOKUMENTE_H

#include <stdint.h>

/* ---- gemeinsame Felder (wie Irons Diary, include/re15_irons_tisch.h) ----------------- */

/* Prop-Flags 0x000B = pc[6..7] 0x000A | 1 (@0x80040998), Typ 0, Band 1, Eltern 0x00 = Welt:
 * Zensus 100 von 121 Item-Weltmodellen (RE15_IRONS_PROP_FLAGS/BAND). Kollisionsbox 0
 * (121 von 121). Zone: sat 0x31 (160 von 164 Item_aot_set), floor 0 (152 von 164), Menge 1
 * wie RE2s Dokument-Zone ROOM10E0 @0x01D26 +16 = `01 00` (der Dokument-Pfad liest sie nicht). */
#define RE15_DOK_PROP_FLAGS          0x000B
#define RE15_DOK_PROP_BAND           1
#define RE15_DOK_AOT_SAT             0x31
#define RE15_DOK_AOT_FLOOR           0
#define RE15_DOK_MENGE               1

/* ---- DOKUMENT 1: "Police Officer's Final Diary Entry" — ROOM1050/1051, sitzende Leiche --- */

/* Item-Id / Bit / Slot / obj:
 *   Item 0x49       VERTRAG 1.4 (Dokument-Nr 1 = 0x49 - 0x48, re15_files.c)
 *   Bit 57          VERTRAG 1.1 (freier Block 53..84 der Zone 9, Zensus re15_irons_tisch.h)
 *   Slot 15         VERTRAG 1.2 (ROOM1050/1051 Spur E: 15/16). Belegt sind 1050 0..10 /
 *                   1051 0..12 (scd_dump_room.py, alle Bloecke, Dossier 9.0)
 *   obj 2           erste freie obj_id: nOmodel = 2 (RDT-Byte 2) in 1050 UND 1051, kein
 *                   Obj_model_set mit obj >= 2 (Dossier 9.0); TIM-Slot RE15_TIM_SLOT_PROP(2) = 6 */
#define RE15_DOK1_ITEM               0x49
#define RE15_DOK1_BIT                57
#define RE15_DOK1_SLOT               15
#define RE15_DOK1_OBJ                2
/* LAGE — PORT-WAHL aus Messung, keine Original-Adresse (RE1.5 hat hier kein Objekt; die
 * Leiche ist GEMALT, Hintergrund Cut 3 und Cut 9):
 *   x,z  Triangulation des Hautflecks der Hand auf dem Oberschenkel, Cut 9 (157,67;167,83)
 *        x Cut 3 (136,06;142,39) mit der Engine-Matrix -> (16474, -347, -6592), Strahlabstand
 *        6,0 / 6,0, Rueckprojektion 0,8 / 0,2 px (Dossier 2.4, geom.trianguliere).
 *   y    Schoss -347 minus 13 = halbe Buchdicke (mesh00 bbox y -13..13, RE2 ROOM1150.RDT
 *        @0x029F34 um den Mittelpunkt modelliert) -> Buchmitte -360.
 *   rot  0 = Grundstellung des Modells (PORT-WAHL, keine Original-Adresse: lange Achse z quer
 *        ueber die Oberschenkel; offener Punkt Dossier 8.1, optische Nutzer-Abnahme). */
#define RE15_DOK1_X                  16474
#define RE15_DOK1_Y                  (-360)
#define RE15_DOK1_Z                  (-6592)
#define RE15_DOK1_ROT_Y              0
/* AUFHEBE-RECHTECK (Ecke x,z / Groesse w,d wie Item_aot_set +6/+8/+10/+12) = das Rechteck des
 * Original-Satzes AUF DERSELBEN LEICHE: ROOM1051.RDT main00 @0x00C0E
 *   `2c 0b 03 31 00 00 92 3b ae e3 e8 03 e8 03 ff 00 18 03 00 00`
 *   -> (0x3B92, 0xE3AE, 0x03E8, 0x03E8) = (15250, -7250, 1000, 1000)
 * (Irons-Regel: Rechteck vom Original-Satz auf demselben Gegenstand). Gemessen (Sonde
 * `abdeckung`, Raster 50, 64 Blickrichtungen): ROOM1050 709 Standorte / 8239 Treffer, 0 von
 * einem kleineren Slot abgefangen. ROOM1051: der Leichen-Satz Slot 11 (Waffe) faengt alle,
 * bis sub01 @0x00CB2 `06 00 10 00 21 09 a5 01 46 0b 00…` (Ck(9,165) -> Aot_reset(11, sce 0,
 * sat 0)) ihn stilllegt — dann greift das Tagebuch (Scan ueberspringt sce 0 / sat 0:
 * @0x80042c90 / @0x80042f50). */
#define RE15_DOK1_RECT_X             15250
#define RE15_DOK1_RECT_Z             (-7250)
#define RE15_DOK1_RECT_W             1000
#define RE15_DOK1_RECT_D             1000

/* Legt die Dokumente beim Raumstart an — je Raum nur, wenn das Zone-9-Bit nicht gesetzt ist.
 * Gerufen an BEIDEN Raumstart-Wegen des Ports (Tuer: scd_room_setup.c scd_room_reenter,
 * Boot/CONTINUE: platform/pc/main.c), jeweils NACH dem Init-Lauf von main00. Im Original gibt
 * es nur EINEN Raumlader FUN_800396fc mit zwei Aufrufern (`jal 0x800396fc` @0x8001d5ac LOAD,
 * @0x8001d988 Tuer). Tut in jedem anderen Raum nichts und legt nichts doppelt an. */
void re15_dokumente_install(uint16_t room_id);

/* obj_id des Dokument-Props in diesem Raum (beide Varianten) oder -1. */
int re15_dokumente_obj_id(uint16_t room_id);

/* Dokument-Nummer (1..4) des Raums (beide Varianten) oder 0. */
int re15_dokumente_nr(uint16_t room_id);

/* Hoechster Sortierschluessel (PC-Maler-z) fuer ein Prop: die Tiefen-Klemme des Dokuments,
 * wenn (room, cut, obj) in der Klemmen-Tabelle steht, sonst `irons` UNVERAENDERT (der Wert
 * von re15_irons_tisch_sort_max; -1 = keine Klemme). */
int re15_dokumente_sort_max_mit(int irons, uint16_t room_id, int cut, int obj_id);

/* Eingebackene Engine-Bytes (gen/dokumente_props.inc) fuer den Plattform-Lader: das Modell
 * des Dokuments dieses Raums, NULL in jedem anderen Raum. */
const uint8_t *re15_dokumente_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_dokumente_tim_bytes(uint16_t room_id, int *out_size);

#endif /* RE15_DOKUMENTE_H */
