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

/* ---- DOKUMENT 2: "Elliot's Diary" — ROOM1000/1001, auf der Bank (elliot.bmp) ------------ */

/*   Item 0x4A       VERTRAG 1.4 (Dokument-Nr 2)
 *   Bit 58          VERTRAG 1.1
 *   Slot 10         erster Slot, der in BEIDEN Varianten frei ist: ROOM1000/1001 belegen 0..9
 *                   (scd_dump_room.py, alle Bloecke; slot_zensus.py) — VERTRAG 1.2 "frei nach Zensus"
 *   obj 2           nOmodel = 2 in 1000 UND 1001 (RDT-Byte 2), kein Obj_model_set obj >= 2;
 *                   TIM-Slot RE15_TIM_SLOT_PROP(2) = 6 */
#define RE15_DOK2_ITEM               0x4A
#define RE15_DOK2_BIT                58
#define RE15_DOK2_SLOT               10
#define RE15_DOK2_OBJ                2
/* LAGE — PORT-WAHL aus Messung, keine Original-Adresse:
 *   x,z  Rueckprojektion der Markenmitte des Nutzerbilds elliot.bmp (Cut 0, Marke x 182..203
 *        y 162..182, Mitte (193,0 ; 172,5), marken.py) mit der ECHTEN Inversen der Engine-Matrix
 *        auf die Sitzflaeche y -385 -> (19226,4 ; -11723,4); vorwaerts (192,98 ; 172,49) (Sonde
 *        `projekt`, Dossier 9.0).
 *   y    Sitzflaeche -385 = Median von fuenf Messungen (Kantenlage Cut 0 -385 / Cut 2 -400 /
 *        Cut 0 Ostkante -325, Deckflaeche Cut 0 -345 / Cut 2 -385; Dossier 3.5) minus 13 = halbe
 *        Buchdicke (mesh03 bbox y -13..13, RE2 ROOM10E0.RDT @0x002320) -> Buchmitte -398.
 *   rot  0 = Grundstellung (PORT-WAHL, keine Original-Adresse: lange Buchachse z = lange Achse
 *        der Bank, SCA-Zelle 7 z 9700 lang; Dossier 8.1). */
#define RE15_DOK2_X                  19226
#define RE15_DOK2_Y                  (-398)
#define RE15_DOK2_Z                  (-11723)
#define RE15_DOK2_ROT_Y              0
/* AUFHEBE-RECHTECK — PORT-WAHL, keine Original-Adresse (auf der Bank liegt im Original nichts):
 * 1000 x 1000 = haeufigste Groesse der Original-Item_aot_set (69 von 162, groessen_zensus.py),
 * mittig auf dem Ursprung — wie ROOM1000s eigene Items (@0x00C66 obj 0 (-4400,-325) <-> Rechteck
 * x[-4850..-3850] z[-800..200]). Gemessen: 134 Standorte / 1000 Treffer, 0 abgefangen (Sonde
 * `abdeckung`); Druck von (18250,-11723) Blick +x -> Leser. */
#define RE15_DOK2_RECT_X             18726
#define RE15_DOK2_RECT_Z             (-12223)
#define RE15_DOK2_RECT_W             1000
#define RE15_DOK2_RECT_D             1000

/* ---- DOKUMENT 3: "Marvin's Notes" — ROOM1020/1021, Marvins Schreibtisch (marvin.bmp) ---- */

/*   Item 0x4B       VERTRAG 1.4 (Dokument-Nr 3)
 *   Bit 59          VERTRAG 1.1
 *   Slot 14         erster Slot, der in BEIDEN Varianten frei ist: ROOM1020 belegt 0..12,
 *                   ROOM1021 0..13 (scd_dump_room.py, alle Bloecke) — VERTRAG 1.2
 *   obj 7           nOmodel = 7 in 1020 UND 1021 (RDT-Byte 2; Dossier 9.0 berichtigt die "6"
 *                   aus 2.2), Obj_model_set nur obj 0..6; TIM-Slot RE15_TIM_SLOT_PROP(7) = 27 */
#define RE15_DOK3_ITEM               0x4B
#define RE15_DOK3_BIT                59
#define RE15_DOK3_SLOT               14
#define RE15_DOK3_OBJ                7
/* LAGE — PORT-WAHL aus Messung, keine Original-Adresse:
 *   x,z  Rueckprojektion der Markenmitte marvin.bmp (Cut 6, Mitte (156,5 ; 105,5)) auf die Platte
 *        y -1410 -> (-9975,9 ; -16434,6), minus Grundriss-Mitte des Blatts (-1, -7) (mesh01 bbox
 *        x -144..142, z -225..211, Fuss y 0; RE2 ROOM2020.RDT @0x003254) -> Ursprung (-9975, -16428);
 *        Blattmitte vorwaerts (156,49 ; 105,48) (Sonde `projekt`, Dossier 9.0).
 *   y    Tischplatte -1410 = Mittel Kantenlage -1405 / Deckflaeche -1415 in Cut 6 (Dossier 3.5);
 *        das Modell hat seinen Fuss bei y 0 -> Ursprung auf der Platte.
 *   rot  0 = Grundstellung (PORT-WAHL; lange Blattachse z = lange Tischachse, SCA-Zelle 2 z 2800
 *        > x 1696; Dossier 8.1). */
#define RE15_DOK3_X                  (-9975)
#define RE15_DOK3_Y                  (-1410)
#define RE15_DOK3_Z                  (-16428)
#define RE15_DOK3_ROT_Y              0
/* AUFHEBE-RECHTECK: x und w = die Tisch-Nachricht DESSELBEN Tischs, ROOM1020.RDT main00 @0x01F0E
 *   `2c 0a 01 31 00 00 a4 d4 4c b9 98 08 e4 0c 03 00 ff ff 00 00` -> x 0xD4A4 = -11100, w 0x0898 = 2200
 * (ein 1000 breites mittiges Rechteck erreicht KEIN Standort — Sonde `abdeckung`: 0, der Tisch ist
 * 1696 breit); z und d = PORT-WAHL 1000 mittig auf -16428 (wie Dok 2). Gemessen mit Umzug der
 * Tisch-Nachricht: 611 Standorte / 5384 Treffer, 0 abgefangen; Druck von (-11500,-16428) Blick +x
 * -> Leser (1020 und 1021). */
#define RE15_DOK3_RECT_X             (-11100)
#define RE15_DOK3_RECT_Z             (-16928)
#define RE15_DOK3_RECT_W             2200
#define RE15_DOK3_RECT_D             1000

/* VORRANG — die Tisch-Nachricht zieht fuer die Liegezeit in einen HOEHEREN Slot.
 * Der Aktions-Scan feuert je Druck den KLEINSTEN treffenden Slot (FUN_80042bac aufsteigend ueber
 * 0x800AC9B0, Ruecksprung nach dem ersten Treffer `j 0x80043028` @0x80042f94). Die Tisch-Nachricht
 * "It's Lieutenant Branagh's desk." (Slot 10 bzw. 11) deckt den GANZEN Tisch x[-11100..-8900]
 * z[-18100..-14800] und finge alle 5384 Treffer des Dokuments ab -> Marvins Notiz waere UNERREICHBAR.
 * ORIGINAL-REGEL "liegt ein Gegenstand im Untersuchen-Rechteck, steht er im kleineren Slot":
 * Zensus aller 240 RDTs 10 von 10 Paaren mit >= 81 % Deckung (zensus_item_nachricht.py), z.B.
 * ROOM10E0 @0x00C4E Item_aot_set Slot 1 / @0x00CCC Aot_set sce 1 Slot 8 mit DEMSELBEN Rechteck
 * (-650,-7350,2600,1000) (selbstpruefung.py). Also zieht die Nachricht, solange Bit (9,59) = 0,
 * von 10 (1020) bzw. 11 (1021) nach 15 — Slot 15 ist in beiden Varianten frei, und kein Skript
 * beruehrt 1020:10 / 1021:11 ausser diesem Aot_set selbst (scd_dump_room.py; kein Work-Var-
 * Vergleich in beiden Raeumen). Ist das Dokument genommen, bleibt sie in ihrem Original-Slot.
 * SATZ-WAECHTER (Muster re15_tuer1120_install): nur der unveraenderte Original-Satz zieht um —
 * aktiv, MESSAGE, sat 0x31, Mitte/halbe Ausdehnung (-10000,-16450) / (1100,1650) = Rechteck
 * (-11100,-18100,2200,3300), Nachricht 3 (1020) bzw. 12 (1021 @0x01F6C `2c 0b 01 31 … 0c 00 ff ff`),
 * Pausenmaske 0xFFFF. Weicht etwas ab, zieht nichts um. */
#define RE15_DOK3_NACHRICHT_ZIEL     15
#define RE15_DOK3_NACHRICHT_1020     10     /* Slot, ROOM1020.RDT @0x01F0E pc[1] = 0x0a */
#define RE15_DOK3_NACHRICHT_1021     11     /* Slot, ROOM1021.RDT @0x01F6C pc[1] = 0x0b */
#define RE15_DOK3_MSG_1020           3      /* Nachricht, @0x01F0E +14 = 03 00 */
#define RE15_DOK3_MSG_1021           12     /* Nachricht, @0x01F6C +14 = 0c 00 */
/* Rechteck des Waechters, beide Varianten: +6 `a4 d4` = -11100, +8 `4c b9` = -18100,
 * +10 `98 08` = 2200, +12 `e4 0c` = 3300 (@0x01F0E bzw. @0x01F6C). */
#define RE15_DOK3_NACHRICHT_RECT_X   (-11100)
#define RE15_DOK3_NACHRICHT_RECT_Z   (-18100)
#define RE15_DOK3_NACHRICHT_RECT_W   2200
#define RE15_DOK3_NACHRICHT_RECT_D   3300

/* TIEFEN-KLEMME — ⛔ PORT-ZUSATZ OHNE ORIGINAL-GEGENSTUECK (wie RE15_IRONS_KLEMME_*).
 * ROOM1020/1021 Cut 3: die ORIGINAL-Maske 35 der Sektion @0xD28 (x 127..166 y 91..114 Tiefe 258 =
 * die Tischplatte; selbstpruefung.py) liegt ueber dem Blatt, das Blatt liegt bei vz 17020..17395 >
 * (258+1)*65536/1023 = 16592 -> ohne Klemme VERDECKT (re15_pri_mask_occludes, otz>>4 @0x8002565c
 * gegen die Maskentiefe @0x80039650-58). Das Original kennt keinen Tiefen-Versatz je Objekt
 * (FUN_8002c18c); RE1.5 hat auf diesem Tisch kein Objekt. Sortierschluessel hoechstens
 * re15_pri_mask_camera_z(258) - 1. Cut 6 (Nutzerbild): Maske 32 Tiefe 156, Blatt vz 9624..9993 <
 * 10058 -> sichtbar ohne Klemme. */
#define RE15_DOK3_KLEMME_CUT         3
#define RE15_DOK3_KLEMME_TIEFE       258

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
