/*
 * RE1.5 Rebuilt — ZWEI NEUE WELT-ITEMS (Runde 35 Spur F, Punkte 4 und 5):
 *   MEMORY CARD im Regal ROOM1010/1011 (add_card.bmp) und SHOTGUN SHELLS auf dem rechten
 *   Aussenluefter ROOM1090/1091 (Shotgun.bmp). Vorbild in allen Punkten: Irons Diary / Memory
 *   Card (include/re15_irons_tisch.h) und die vier Dokumente (include/re15_dokumente.h).
 * Dossier mit allen Messungen: analysis/befunde_runde35/F_inhalt.md, Punkte 4 und 5.
 *
 * NUTZER-VORGABE (AUFTRAG.md Z. 20 / Z. 38, woertlich):
 *   "Außerdem möchte ich das du in ROOM 1010 eine Memory Card hinzufügst im Regal - siehe add_card.bmp"
 *   "Ich möchte, das du in ROOM 1090 Schrotflinten Munition in die Welt packst, auf den
 *    Außenlüfter. Stelle - siehe Shotgun.bmp."
 * OHNE Asset-Patch: die RDTs bleiben byte-true; Prop + Aufhebe-Zone entstehen portseitig beim
 * Raumstart (beide Raumstart-Wege), wie re15_irons_tisch_install / re15_dokumente_install.
 *
 * WAS DAS ORIGINAL VORGIBT (selbst nachdisassembliert, info/Re1.5/PSX.EXE):
 *   Obj_model_set LAB_80040914 — obj_id = Pool-Index, Typ pc[2] -> pool+8 (@0x8004095c), Band
 *     pc[4] -> pool+130 (@0x80040974), Flags = pc[6..7] | 1 (@0x80040998 `ori v0,v0,0x1`).
 *   Item_aot_set @0x80040644 — Zone-9-Bit = +18 (@0x80040680 `lhu a1,18(a2)`), Prop = +20
 *     (@0x80040684 `lbu s1,20(a2)`); Bit gesetzt -> Satz still + Modell weg (@0x800406dc-718);
 *     hier: Bit gesetzt -> gar nicht erst anlegen.
 *   Aufnahme-Modal FUN_8001db28: Ja -> Zone nullen (@0x8001e090), einfuegen (`jal 0x8004dc4c`
 *     @0x8001e0c4), Zone-9-Bit setzen (`jal 0x8004ef90` @0x8001e0d0, Bank 9 @0x8001e0d4).
 *   Aktions-Pruefpunkt 620 voraus (@0x80042bd0 `ori v0,zero,0x26c`), Scan aufsteigend, je Druck
 *     der KLEINSTE treffende Slot (`j 0x80043028` @0x80042f94).
 */
#ifndef RE15_INHALT_R35_H
#define RE15_INHALT_R35_H

#include <stdint.h>

/* ---- gemeinsame Felder (Zensus der Item-Weltmodelle, re15_irons_tisch.h) ---------------- */
/* Prop-Flags 0x000B = pc[6..7] 0x000A | 1 (@0x80040998), Band 1: 100 von 121 Item-Props. */
#define RE15_R35I_PROP_FLAGS       0x000B
#define RE15_R35I_PROP_BAND        1
/* Zonen-sat 0x31: 160 von 164 Item_aot_set. */
#define RE15_R35I_AOT_SAT          0x31

/* ---- MEMORY CARD ROOM1010/1011 -------------------------------------------------------- */
/* Item 0x21 "Memory Card" (DEBUG.BIN @0x499E `c2 01` -> Namensblob @0x4BEA), Menge wie die
 * Hebetisch-Karte RE15_IRONS_KARTE_MENGE (3 = RE2-Speicher-Item Ink Ribbon, 21 von 21). Modell =
 * dasselbe wie in Irons' Buero (Keycard-MD1 ROOM1110.RDT @0x0013D8 + Karten-TIM,
 * gen/irons_tisch_props.inc ueber re15_irons_tisch_md1_bytes). */
#define RE15_R35I_KARTE_ITEM       0x21
#define RE15_R35I_KARTE_BIT        80        /* VERTRAG 1.1 Spur F */
/* Slot 10: ROOM1010 belegt 0..7, ROOM1011 0..8 (scd_dump_room.py, alle Bloecke), Dokument 4
 * belegt Slot 9 (RE15_DOK4_SLOT). obj 4: nOmodel = 3 in beiden Varianten, Dokument 4 = obj 3;
 * TIM-Slot RE15_TIM_SLOT_PROP(4) = 8. */
#define RE15_R35I_KARTE_SLOT       10
#define RE15_R35I_KARTE_OBJ        4
/* LAGE — PORT-WAHL aus Messung (Dossier Punkt 4), keine Original-Adresse (das Regal ist GEMALT):
 *   Marke add_card.bmp = ROOM1010 Cut 7 (marken.py: Abweichung 3,11 gegen 63,46), 130 rote Pixel
 *   x 269..278 y 158..170, Mitte (274,0 ; 164,5). Sehstrahl Cut 7 tritt bei (3600,-2055,-1708) in
 *   die Regal-Zelle SCA 4 (x 3600..4600, z -2050..1450) ein = Regalfach zwischen Brett A
 *   (Unterkante -2400) und Brett B; Bretthoehen gemessen in Cut 4 (frontale Sicht, Helligkeit je
 *   Hoehe auf der Regalfront x = 3600): Brett B Vorderkante -1725..-1650 -> Oberseite -1725.
 *   Strahl trifft y = -1725 bei (3796,-1725,-1033) = Kartenmitte, vorwaerts Cut 7 (273,96;164,46).
 *   Keycard-Modell: Ursprung an einer Ecke, Mitte = Ursprung + (-80,5 ; 0 ; 135) bei rot 0
 *   -> Ursprung (3877,-1725,-1168). rot 0 = lange Kante (Modell-Z, 270) laengs der Regalachse z
 *   (Zelle 3500 lang) — PORT-WAHL. */
#define RE15_R35I_KARTE_X          3877
#define RE15_R35I_KARTE_Y          (-1725)
#define RE15_R35I_KARTE_Z          (-1168)
#define RE15_R35I_KARTE_ROT_Y      0
/* AUFHEBE-RECHTECK — PORT-WAHL 1000 x 1000 mittig auf der Kartenmitte (3796,-1033) (haeufigste
 * Groesse, 69 von 162 Item_aot_set, groessen_zensus.py). Ueberschneidet keinen Original-Satz von
 * 1010/1011 (naechster: Slot 6 Nachricht x 2300..3300 z -400..600). */
#define RE15_R35I_KARTE_RECT_X     3296
#define RE15_R35I_KARTE_RECT_Z     (-1533)
#define RE15_R35I_KARTE_RECT_W     1000
#define RE15_R35I_KARTE_RECT_D     1000

/* ---- SHOTGUN SHELLS ROOM1090/1091 ----------------------------------------------------- */
/* Item 0x16 "Shotgun Shells" (DEBUG.BIN @0x4988). Menge 7 = haeufigste Original-Menge
 * (Zensus schrot_export.py: 22 von 28 Saetzen, sonst 14). Modell = haeufigstes Original-Mesh
 * (12 von 22 Saetzen mit Modell): ROOM1010.RDT Prop 2, MD1 @0x0015DC 484 B md5 11f139fa...,
 * TIM @0x024F48 17440 B (gen/r35_schrot_prop.inc). */
#define RE15_R35I_SCHROT_ITEM      0x16
#define RE15_R35I_SCHROT_MENGE     7
#define RE15_R35I_SCHROT_BIT       81        /* VERTRAG 1.1 Spur F */
/* Slot 4: ROOM1090 belegt 0..3, ROOM1091 0..2, Kamerazonen ab 39 (Sonde `aots 1090`: "erster
 * freier Slot < 48: 4"); kein Port-Installer in 1090. obj 4: nOmodel = 4 in beiden Varianten. */
#define RE15_R35I_SCHROT_SLOT      4
#define RE15_R35I_SCHROT_OBJ       4
/* LAGE — PORT-WAHL aus Messung (Dossier Punkt 5):
 *   Marke Shotgun.bmp = ROOM1090 Cut 2 (marken.py 7,01; Cut 10 = dieselbe Kamera, 8,41), 400 rote
 *   Pixel x 98..117 y 127..146, Mitte (108,0 ; 137,0). Die zwei Luefter = SCA 32 (x -3400..3400,
 *   z -17400..-15397, Band 5 -> Boden y -9000 wie Obj_model_set obj 1 @0x02190 (y -9000, Band 5));
 *   Bild-rechts = Welt -x (Zeile 0 der Matrix -3875 0 -1334). Oberkante des rechten Luefters in
 *   Bildzeile 146,5 (Helligkeitssprung Wand -> Luefter in allen Spalten 85..135) auf der Front
 *   z -15397 -> y -10761. Kiste (Mesh-Huelle x +-270, y -342..0, z +-103) steht mit der
 *   Vorderkante auf der Front: Mitte z -15500, x aus dem Strahl der Markenmitte -> -2408.
 *   Vorwaerts: Huelle x 99,7..116,2 y 136,7..146,6 (Marke x 98..117). rot 0 = lange Seite
 *   parallel zur Luefterfront — PORT-WAHL. */
#define RE15_R35I_SCHROT_X         (-2408)
#define RE15_R35I_SCHROT_Y         (-10761)
#define RE15_R35I_SCHROT_Z         (-15500)
#define RE15_R35I_SCHROT_ROT_Y     0
/* AUFHEBE-RECHTECK — PORT-WAHL 1000 x 1000 mittig auf (-2408,-15500); Zonen-Etage = Band 5
 * (die Spielerebene auf dem Dach, Original-Satz Slot 3 AUTO band 5 in 1090) — der Aktions-Scan
 * verlangt rec[2] == Spieler-Etage ausser bei Bit 0x80 (aot_common.c, @0x80042cb4-ccc). */
#define RE15_R35I_SCHROT_RECT_X    (-2908)
#define RE15_R35I_SCHROT_RECT_Z    (-16000)
#define RE15_R35I_SCHROT_RECT_W    1000
#define RE15_R35I_SCHROT_RECT_D    1000
#define RE15_R35I_SCHROT_FLOOR     5

/* Legt Prop + Zone beim Raumstart an (nur wenn das Bank-9-Bit fehlt). Gerufen an BEIDEN
 * Raumstart-Wegen (scd_room_setup.c scd_room_reenter, platform/pc/main.c Boot/CONTINUE), nach
 * dem Init-Lauf von main00. Tut in jedem anderen Raum nichts, legt nichts doppelt an. */
void re15_inhalt_r35_install(uint16_t room_id);

/* obj_id des neuen Props in diesem Raum (beide Varianten) oder -1. */
int re15_inhalt_r35_obj_id(uint16_t room_id);

/* Engine-Bytes (MD1/TIM) des neuen Props dieses Raums fuer den Plattform-Lader; NULL sonst. */
const uint8_t *re15_inhalt_r35_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_inhalt_r35_tim_bytes(uint16_t room_id, int *out_size);

#endif /* RE15_INHALT_R35_H */
