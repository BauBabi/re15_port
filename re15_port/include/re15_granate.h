/*
 * RE1.5 Rebuilt — die HANDGRANATE (Item 0x09 "Hand Grenade") im Hebetisch von Irons' Buero.
 *
 * Runde 30, Nachtrag K (Nutzer, 2026-09-28): "Ausserdem moechte ich im hochfahrenden model in
 * irons office eine granate mit hochfahren haben." Vorbild ist die Sicherung derselben Fahrt
 * (include/re15_sicherung.h, Abschnitt H). Dossier: analysis/befunde_runde30/nachtrag-granate.md.
 *
 * ⛔ DAS ORIGINAL HAT HIER KEINE GRANATE. Wie bei der Sicherung ist das eine Port-Ergaenzung:
 * Prop, Sitz, Menge und Modal sind Port-Wahl. Belegt sind Item, Name, Bild, Icon, Weltmodell und
 * die Aufnahme-Mechanik (dieselbe wie bei der Sicherung).
 *
 * ⛔ DIE GRANATE IST IM PORT NOCH NICHT WIE IM ORIGINAL BENUTZBAR (Dossier §2). Ausruesten,
 * Wurfanimation (W09 Clip 7, 35 Bilder) und Munitionsabzug laufen. Der Wurf selbst fehlt: die
 * ESP-Routinen 30 @0x8001843c (Wurf-Init), 29 @0x80018320 (Flug/Abprall) und 31 @0x8001854c
 * (Explosion, `jal 0x80012d60` @0x800185b8) sind in re15_esp.c nicht portiert. Den Schaden
 * liefert die Port-Bruecke ENT[9].resolve (game_step_common.c) SOFORT beim Abzug.
 */
#ifndef RE15_GRANATE_H
#define RE15_GRANATE_H

#include <stdint.h>

/* Item-Id "Hand Grenade". Namensleser FUN_80028840 (`lhu v1,0x800c495c(id*2)` @0x80028854,
 * Name = 0x800c4a28 + Offset @0x80028864): Eintrag 0x09 @0x800C496E = 0x005B -> Name
 * @0x800C4A83 "Hand Grenade" (DEBUG.BIN Datei 0x04A83). Einzige Granate mit Wurf: FSM-Gate
 * `ori v0,zero,0x9` @0x80033688 / `bne v1,v0` @0x8003368c vor dem Spawn 0x040D1000. */
#define RE15_GRANATE_ITEM      0x09

/* Menge je Aufnahme — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE. Das Original platziert die Hand
 * Grenade nirgends: 0 von 164 Item_aot_set in 206 RDTs (Zensus nachtrag-granate_werkzeug/
 * zensus.py). 1, weil der Nutzer "eine Granate" schreibt, das Weltmodell eine zeigt und ein
 * Wurf genau eine verbraucht (Entlade-Handler 0x80033B38: `jal 0x8004eae4` @0x80033b40,
 * gemessen Magazin 5 -> 4 -> 3). */
#define RE15_GRANATE_MENGE     1

/* Zone-9-Bit "Granate genommen". Aus dem Zensus, nicht gewaehlt: in keinem der 206 RDTs belegt
 * (Item_aot_set-Nutzlast oder Ck/Set mit bank=9), portseitig sind 53 (Sicherung), 54 (Diary)
 * und 55 (Memory Card) vergeben — 56 ist das naechste im freien Block 53..84. */
#define RE15_GRANATE_TAKEN_BIT 56

/* obj_id des zusaetzlichen Props. ROOM1150/1151: 0..3 Raum (nOmodel=4), 4 Sicherung,
 * 5/6 Diary/Memory Card -> 7. Textur-Slot RE15_TIM_SLOT_PROP(7) = 26 + 1 = 27, belegt nur in
 * Raeumen mit nOmodel >= 8 (render_pc.c, Slot-Tabelle). */
#define RE15_GRANATE_OBJ_ID    7

/* SITZ UND DREHUNG — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE (das Original hat im Hebetisch keinen
 * Gegenstand). Abgeleitet aus der ausgelieferten Geometrie, Rechnung nachtrag-granate_werkzeug/
 * sitz.py (Ausgabe nachtrag-granate_belege/sitz.txt), Plattform-Koordinaten (+Y nach unten):
 *   Fachboden   ROOM1150 Prop 0 Punkte @0x121AC..@0x1221C, alle y = -1036 (ROOM1151 dieselben
 *               Werte @0x14224..@0x14294), x -485..-74, z 875..1645
 *   Sicherung   (-280,-1062,1260) rot_y 1024, Rohr 406 x D52 -> x -306..-254, z 1057..1463
 *   Kuppel      Prop 1 MD1 @0x138D4 / Prop 2 @0x13B88: Ring y -1138, Scheitel (-280,-1185,1260);
 *               offen (Deckelweg +-150, For @0x0FC0) gibt sie z 1110..1410 frei
 *   Granate     gen/granate_prop.inc, Huelle x -77..77 y -55..55 z -55..55 (Laengsachse X)
 *
 *   POS_Z = 1260   Naht der Deckelhaelften = hoechste Stelle der Kuppel; z 1183..1337 liegt ganz
 *                  in der Oeffnung 1110..1410
 *   ROT_Y          Viertelkreis wie die Sicherung: Laengsachse entlang z, parallel zu ihr
 *
 * ⛔ BEIDE SCHRANKEN ZUGLEICH GEHEN BEI AUFLIEGENDER GRANATE NICHT (sitz_suche.py, Kanten in 8
 * Stuecke geteilt, x-Mitte -368..-353, z-Mitte 1250..1270, Gierwinkel +-48, Rollen +-160): keine
 * Lage haelt die Sicherung frei UND bleibt unter der GESCHLOSSENEN Kuppel. Bestes aufliegendes
 * Paar: x -361 Abstand 0,02 / Luft -1,98; x -365 Abstand 4,02 / Luft -3,39. GEMESSEN im Spiel
 * (Framedump MIT/OHNE, x -365 y -1091): bei geschlossener Kuppel F238/F240 je 2 Punkte der
 * Granate durch die Schale (x211..212 y148 bei 320x240).
 * Deshalb liegt sie 3 Einheiten TIEF im Fachboden. Das geht, weil sie unten offen ist: die in
 * der Hand verdeckte Handflaechen-Seite hat keine Flaechen, im Boden steckt also nur deren Rand.
 *   POS_X = -362   Abstand zur Sicherung 1,07 (Punkt-genau gegen den Zylinder r 26)
 *   POS_Y = -1088  Fachboden -1036 minus halbe Hoehe 55 plus 3: Luft unter der geschlossenen
 *                  Kuppel +0,67, tiefster Punkt y -1033 = 3 unter dem Boden
 * Beides in ROOM1150 und ROOM1151 gleich (sitz_suche.py --sitz -362 -1088). */
#define RE15_GRANATE_POS_X  (-362)
#define RE15_GRANATE_POS_Y  (-1088)
#define RE15_GRANATE_POS_Z  (1260)
#define RE15_GRANATE_ROT_Y  (1024)

/* Legt das Prop beim Raumstart an — nur in ROOM1150/1151, nur wenn die Granate noch nicht
 * genommen ist. Gerufen an BEIDEN Raumstart-Wegen des Ports wie die Sicherung (Tuer:
 * scd_room_setup.c scd_room_reenter; Boot/CONTINUE: platform/pc/main.c), weil das Original
 * genau EINEN Raumlader hat (FUN_800396fc, `jal 0x800396fc` @0x8001d5ac LOAD und @0x8001d988
 * Tuer). */
void re15_granate_install(uint16_t room_id);

/* Pro Gameplay-Bild, NACH re15_sicherung_tick: steht der Hebetisch oben UND hat die Sicherung
 * ihr Modal in dieser Fahrt schon gehabt (oder ist nicht mehr anzubieten), geht das Modal der
 * Granate auf. Reihenfolge und Sperren (Auftrag: erst Sicherung, dann Granate; jede mit eigenem
 * Flag; "No" bei einer laesst die andere unberuehrt; je Fahrt hoechstens ein Modal je
 * Gegenstand) — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE. Die Einzelregel je Gegenstand ist die der
 * Sicherung: abgelehnte Aufnahme bleibt scharf (RE1.5 nur der Ja-Zweig nullt die Zone
 * @0x8001e090, "No" @0x8001e0ec; RE2 @0x800720cc / @0x80072298), ein Ausloesen = hoechstens ein
 * Modal (@0x80043334). Fenster (-5000 .. -1100] und Wiederbewaffnung in der Parklage wie
 * sicherung_1150.c. Rueckgabe 1 = Modal wurde in diesem Bild aufgemacht. */
int  re15_granate_tick(void);

/* Die eingebackenen Engine-Bytes (gen/granate_prop.inc) fuer den Plattform-Lader. */
const uint8_t *re15_granate_md1_bytes(int *out_size);
const uint8_t *re15_granate_tim_bytes(int *out_size);

#endif /* RE15_GRANATE_H */
