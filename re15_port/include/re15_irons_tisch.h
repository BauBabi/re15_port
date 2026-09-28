/*
 * RE1.5 Rebuilt — IRONS DIARY und MEMORY CARD auf dem Schreibtisch in Irons' Buero
 * (ROOM1150 John-Variante / ROOM1151 Elza-Variante). Runde 30, Thema E2.
 * Dossier mit allen Messungen: analysis/befunde_runde30/irons-diary-welt.md.
 *
 * ⛔ WARUM ES DIESE DATEI GIBT: Nutzer-Auftrag Runde 30 Abschnitt E — "auf seinen Tresen
 * ein Dokument hinterlegen [...] an der Stelle wie in 'Irons items.png' in rot markiert
 * [...] Nach dem Auflesen und Zumachen soll es vom Schreibtisch verschwinden. [...] in
 * blau markiert soll ein Item der Memory Card liegen, das man aufnehmen kann."
 * RE1.5 hat KEINES von beiden auf diesem Tisch: 0 Item_aot_set fuer Id 0x21 und keines
 * mit Id >= 0x48 in allen 240 RDTs (Zensus r30_idw_zensus.py, 1579 SCD-Bloecke,
 * 0 desynchron), der Raum benutzt Bank 9 im SCD nicht. Beides kommt portseitig dazu —
 * OHNE Asset-Patch, die RDTs bleiben byte-true (Muster: include/re15_sicherung.h).
 *
 * WAS DAS ORIGINAL VORGIBT (selbst nachdisassembliert, info/Re1.5/PSX.EXE):
 *   Obj_model_set LAB_80040914 — obj_id = Pool-Index (@0x8004093c-58, Schrittweite 148),
 *     Typ pc[2] -> pool+8 (@0x8004095c), Band pc[4] -> pool+130 (@0x80040974),
 *     Flags = pc[6..7] | 1 (@0x80040990 `lhu v0,6(a2)` / @0x80040998 `ori v0,v0,0x1`).
 *   Item_aot_set @0x80040644 — Kurzform: Zone-9-Bit = +18 (@0x80040680 `lhu a1,18(a2)`),
 *     Prop = +20 (@0x80040684 `lbu s1,20(a2)`), Vorschub 22 (@0x80040688); Bit gesetzt
 *     (`jal 0x8004efe4` @0x800406dc) -> Satz still (@0x800406f4) + Modell weg
 *     (@0x800406f8-718). Der Port bildet das in op_item_aot_set (scd_vm.c) ab; hier
 *     gilt: Bit gesetzt -> gar nicht erst anlegen (dieselbe Regel wie
 *     re15_sicherung_install).
 *   Reichweite des Aktionsdrucks: 620 voraus (@0x80042bd0 `ori v0,zero,0x26c`).
 */
#ifndef RE15_IRONS_TISCH_H
#define RE15_IRONS_TISCH_H

#include <stdint.h>

/* ---- DOKUMENT "Irons Diary" -------------------------------------------------------- */

/* Item-Id = RE1.5s erste FILE-Id (u8 @0x800c7370, DEBUG.BIN Datei 0x07370) + Dokument-Nr 0.
 * Ab dieser Id zweigt der Aufnahme-Pfad in den Leser ab (aot_common.c aot_item_dokument,
 * RE2-Vorbild `sltiu v0,a3,0x68` @0x80071bbc). Dokument 0 = Irons Diary (re15_files.c). */
#define RE15_IRONS_DIARY_ITEM        0x48
/* Menge 1 wie RE2s Dokument-Zone "Secretary's diary B": ROOM10E0.RDT @0x01D26
 * `4e 03 02 31 00 00 a2 b3 50 c9 40 06 40 06 70 00 01 00 69 00 00 00` (+16 = 01 00).
 * Der Dokument-Pfad liest die Menge nicht (re15_menu_request_doc). */
#define RE15_IRONS_DIARY_MENGE       1
/* obj_id / Pool-Index. ROOM1150/1151 belegen 0..3 (nOmodel = 4, RDT-Byte 2), 4 ist die
 * Sicherung (RE15_SICHERUNG_OBJ_ID); 5 und 6 sind frei — gemessen: Sonde
 * probe_r30_irons-diary-welt `pool 1150/1151` (5 Eintraege), kein Obj_model_set mit
 * obj_id 5/6 in beiden RDTs (Walk ueber 317 bzw. 186 Opcodes). TIM-Slot
 * RE15_TIM_SLOT_PROP(5) = 9 (platform/pc/main.c). */
#define RE15_IRONS_DIARY_OBJ_ID      5
/* AOT-Slot. Belegt: 0..6 (ROOM1150) bzw. 0..5 (ROOM1151), 48..63 Kamerazonen. Kein
 * Aot_set/Door_aot_set/Item_aot_set/Aot_reset auf 7/8 in beiden RDTs (derselbe Walk). */
#define RE15_IRONS_DIARY_AOT_SLOT    7
/* Zone-9-Bit "genommen". Zensus ueber alle 240 RDTs: 85 von 256 Bits belegt, freier
 * Block 53..84; portseitig vergeben ist nur 53 (RE15_SICHERUNG_TAKEN_BIT). 54/55 sind
 * die naechsten zwei im selben Block. Bit 0 scheidet aus (das Modal setzt nur
 * `if (s_taken)`, item_modal_common.c). */
#define RE15_IRONS_DIARY_TAKEN_BIT   54

/* LAGE — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE (RE1.5 hat hier kein Objekt, RE2 keinen
 * solchen Tisch). Festgelegt im Auftrag der Runde: "LAGE B" = das Buch liegt genau auf
 * dem GEMALTEN Klemmbrett unter der roten Marke. Gemessen, nicht geschaetzt:
 *   x,z  Mitte des gemalten Klemmbretts, auf die Platte zurueckgelegt (Hauptachsen der
 *        hellen Pixel, r30_idw_klemmbrett.py -> klemmbrett.txt: (-23815,-18277);
 *        Einzelmerkmal-Verfahren (-23804,-18294); Lage B nimmt (-23813,-18280)).
 *   y    Tischplatte y = -1520 (zwei Verfahren: Flaechenkorrelation Cut 2 <-> Cut 6,
 *        NCC-Gipfel -1520; Sehstrahlschnitt -1515..-1523) minus 13 = halbe Buchdicke
 *        (MD1 ROOM10E0 @0x002320 um den Mittelpunkt modelliert, bbox y -13..13).
 *   rot_y 2944  Drehung des gemalten Klemmbretts (2930..2957 je nach Schwelle).
 * Zurueckprojiziert mit der Engine-Matrix (Sonde `projekt`): Cut 2 (153,49 ; 124,44),
 * 2,29 px neben der Markenmitte (152,5 ; 126,5), INNERHALB der Marke x 146..158 /
 * y 120..132; Cut 6 (196,51 ; 123,97). */
#define RE15_IRONS_DIARY_X           (-23813)
#define RE15_IRONS_DIARY_Y           (-1533)
#define RE15_IRONS_DIARY_Z           (-18280)
#define RE15_IRONS_DIARY_ROT_Y       2944

/* AUFHEBE-RECHTECK (Ecke x,z / Groesse w,d, wie Item_aot_set +6/+8/+10/+12).
 * x und w vom Telefon-Satz, den das Original AUF DIESEM TISCH selbst legt:
 * ROOM1150.RDT main00 @0x00DA6 (ROOM1151 dieselbe Stelle)
 *   `2c 03 03 31 00 00 40 a2 c0 ae e8 03 e8 03 ff 00 18 06 00 00`
 *   -> x = 0xA240 = -24000, w = 0x03E8 = 1000.
 * Ein ums Prop ZENTRIERTES Rechteck waere von Lage B aus nicht erreichbar (Sonde
 * `abdeckung`: 0 Standorte), weil der Spieler an der Tischkante x = -22664 steht und sein
 * Pruefpunkt 620 voraus (@0x80042bd0) bei x = -23284 liegt.
 * z und d: ⛔ PORT-WAHL — 1000 tief (haeufigste Groesse, 71 von 164 Item_aot_set),
 * mittig um die z-Lage des Buchs (-18280 - 500). */
#define RE15_IRONS_DIARY_RECT_X      (-24000)
#define RE15_IRONS_DIARY_RECT_Z      (-18780)
#define RE15_IRONS_DIARY_RECT_W      1000
#define RE15_IRONS_DIARY_RECT_D      1000

/* ---- MEMORY CARD (Item 0x21) ------------------------------------------------------- */

/* Item-Id: DEBUG.BIN Offsettabelle @0x495C + 2*0x21 = @0x499E `c2 01` -> Namensblob
 * @0x4BEA `29 41 49 4b 4e 55 00 1f 3d 4e 40 07` = "Memory Card". NICHT 0x20 (@0x499C
 * `af 01` -> "Incendiary Capsule"). Eigenschaften PSX.EXE @0x80074F34
 * `fa 00 00 00 88 4c 07 80 01 00 00 00` (Obergrenze 250, zaehlbar). */
#define RE15_IRONS_KARTE_ITEM        0x21
/* Menge 3. ⛔ RE1.5 IST HIER NICHT MASSGEBLICH: es platziert Item 0x21 NIRGENDS
 * (0 Item_aot_set), hat also keine Menge dafuer. RE2-VORBILD ist das Speicher-Item
 * Ink Ribbon (Id 0x1E): 21 von 21 gefundenen Platzierungen tragen Menge 3, z.B.
 * RE2 ROOM1060.RDT @0x00C24 `4e 02 02 31 00 00 79 98 e5 0c e8 03 e8 03 1e 00 03 00 ...`
 * (+14 Item 0x1E, +16 Menge 3). */
#define RE15_IRONS_KARTE_MENGE       3
#define RE15_IRONS_KARTE_OBJ_ID      6     /* TIM-Slot RE15_TIM_SLOT_PROP(6) = 26; frei s.o. */
#define RE15_IRONS_KARTE_AOT_SLOT    8     /* frei s.o. */
#define RE15_IRONS_KARTE_TAKEN_BIT   55    /* freier Block 53..84, s.o. */

/* LAGE — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE. Die Kartenmitte liegt auf der Mitte der
 * BLAUEN Marke (140,0 ; 126,5), auf die Platte y = -1520 zurueckgelegt mit der echten
 * Inversen der Engine-Matrix: (-23657, -1520, -18649), vorwaerts projiziert
 * (139,97 ; 126,49) = 0,03 px. Das Keycard-Modell hat seinen Ursprung an einer ECKE
 * (Punkte (0,0,0) (0,0,270) (-161,0,270) (-161,0,0), ROOM1110.RDT @0x0013D8); bei
 * rot_y 3072 ist Modell (x,0,z) -> Welt (-z,0,x), die Kartenmitte (-80,5 ; 0 ; 135) liegt
 * also (-135, 0, -80,5) neben dem Ursprung -> Ursprung (-23522, -1520, -18568).
 * rot_y 3072: die lange Kante zeigt vom Betrachter vor dem Tisch weg (Tischachse = Welt-Z,
 * Standlinie x = -22664 ueber z -19400..-17600), Aufdruck fuer ihn aufrecht. */
#define RE15_IRONS_KARTE_X           (-23522)
#define RE15_IRONS_KARTE_Y           (-1520)
#define RE15_IRONS_KARTE_Z           (-18568)
#define RE15_IRONS_KARTE_ROT_Y       3072
/* Rechteck: x/w vom Telefon-Satz @0x00DA6 (s.o.); z/d ⛔ PORT-WAHL: 1000 tief, mittig um
 * die Kartenmitte z = -18649 (-18649 - 500). */
#define RE15_IRONS_KARTE_RECT_X      (-24000)
#define RE15_IRONS_KARTE_RECT_Z      (-19149)
#define RE15_IRONS_KARTE_RECT_W      1000
#define RE15_IRONS_KARTE_RECT_D      1000

/* ---- gemeinsame Felder beider Props / Zonen ----------------------------------------- */

/* Prop-Flags 0x000B = pc[6..7] 0x000A | 1 (@0x80040998). Zensus ueber alle 121
 * Item-Weltmodelle des Spiels (Obj_model_set des Props, das ein Item_aot_set nennt):
 * 100 von 121 tragen Typ 0, Band 1, Eltern 0x00, pc[6..7] = 0x000A; 121 von 121 eine
 * Null-Kollisionsbox. Beispiel Blue Keycard ROOM1110.RDT @0x00BB4
 * `2d 02 00 00 01 00 0a 00 00 00 48 0d ...`. Bit 0x2 schaltet die Objekt-Kollision ab
 * (@0x8002cff4 `andi v0,v0,0x2`, re15_collision.c). */
#define RE15_IRONS_PROP_FLAGS        0x000B
#define RE15_IRONS_PROP_BAND         1
/* Zonen-sat 0x31 (FORWARD + ACTION + Spieler): 160 von 164 ausgelieferten Item_aot_set;
 * floor 0: 152 von 164 (Zensus r30_idw_zensus.py). */
#define RE15_IRONS_AOT_SAT           0x31
#define RE15_IRONS_AOT_FLOOR         0

/* TIEFEN-KLEMME — ⛔ PORT-ZUSATZ OHNE ORIGINAL-GEGENSTUECK.
 * In Cut 2 (dem Cut des Nutzerbilds) liegen ueber beiden Ablagen drei Original-Masken der
 * Tiefe 87 — die Tischplatte selbst:
 *   ROOM1150.RDT (und ROOM1151.RDT byte-gleich) @0x006E8 `38 20 b0 98 57 00 b5 00 18 00 10 00`
 *                                               @0x006F4 `50 20 c8 98 57 00 b5 00 18 00 10 00`
 *                                               @0x00714 `48 30 c0 a8 57 00 b5 00 20 00 18 00`
 * Masken und Objekte teilen EINE Ordnungstabelle (Maske: `lh a0,2(s3)` @0x80039650,
 * `sll a0,a0,2` @0x80039658 -> AddPrim; Objekt: otz >> 4 @0x8002565c / @0x800258dc);
 * einen Tiefen-Versatz je Objekt gibt es nicht (FUN_8002c18c reicht nur Mesh, Farbe und
 * das ABE-Bit weiter). Die Props liegen bei vz 5930..6041 = Bucket 92 -> im Original wie
 * im Port VON DER TISCHMASKE VERDECKT (gemessen im Renderer: 0 von 1485 / 0 von 972
 * Pixeln). Deshalb klemmt der Port NUR diese zwei Props NUR in Cut 2 auf Bucket 87: ihr
 * Sortierschluessel wird hoechstens re15_pri_mask_camera_z(87) - 1 = 5636. Damit sind sie
 * sichtbar, und eine Figur VOR dem Tisch (vz 4328..5852) liegt weiterhin darueber.
 * Cut 6 hat keine Masken (pri_offset 0xBCC -> `ff ff ff ff`). */
#define RE15_IRONS_KLEMME_CUT        2
#define RE15_IRONS_KLEMME_TIEFE      87

/* Legt beide Props + Zonen beim Raumstart an — in ROOM1150/1151, je Gegenstand nur, wenn
 * sein Zone-9-Bit nicht gesetzt ist. Gerufen an BEIDEN Raumstart-Wegen des Ports, jeweils
 * direkt hinter re15_sicherung_install: Tuer (scd_room_setup.c scd_room_reenter) und
 * Boot/CONTINUE (platform/pc/main.c). Im Original gibt es nur EINEN Raumlader FUN_800396fc
 * mit zwei Aufrufern (`jal 0x800396fc` @0x8001d5ac Session-Start/LOAD, @0x8001d988 Tuer).
 * Tut in jedem anderen Raum nichts und legt nichts doppelt an. */
void re15_irons_tisch_install(uint16_t room_id);

/* Hoechster Sortierschluessel (PC-Maler-z) fuer ein Prop — oder -1, wenn nicht zu klemmen.
 * Liefert (int)re15_pri_mask_camera_z(87) - 1 nur fuer obj_id 5/6 in ROOM1150/1151 im
 * Cut 2 (s. TIEFEN-KLEMME). */
int re15_irons_tisch_sort_max(uint16_t room_id, int cut, int obj_id);

/* Eingebackene Engine-Bytes (gen/irons_tisch_props.inc) fuer den Plattform-Lader:
 * obj_id 5 = Dokument (RE2-Buch), 6 = Memory Card. NULL fuer jede andere obj_id. */
const uint8_t *re15_irons_tisch_md1_bytes(int obj_id, int *out_size);
const uint8_t *re15_irons_tisch_tim_bytes(int obj_id, int *out_size);

#endif /* RE15_IRONS_TISCH_H */
