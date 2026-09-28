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
 * Gegenstand). Runde 31 (Nutzer, 2026-09-29): "packe mir die Granate links in die hochfahrende
 * Box und die Sicherung rechts". Abgeleitet aus der ausgelieferten Geometrie (Fachboden, Kuppel,
 * Deckelweg, Kamera Cut 4: Kopf von include/re15_sicherung.h), Dossier
 * analysis/befunde_runde31/hebetisch.md §1, Werkzeuge hebetisch_werkzeug/r31_suche.py (zulaessige
 * Sitze), r31_raster.py / r31_wahl2.py (Paar Granate-links / Sicherung-rechts), r31_pruef.py:
 *   Granate     gen/granate_prop.inc, Huelle x -77..77 y -55..55 z -55..55 (Laengsachse X); OFFEN
 *               sind die Unterseite (+y) und am Modell-+x-Ende die -z-Kegelflaeche samt
 *               Eckdreieck (in der Hand verdeckt)
 *   Hoehe       die Granate braucht 110; die Kuppel ist innen 102 am Ring, 149 am Scheitel —
 *               links (z < 1260) passt sie nur bis z-Mitte ~1095..1115
 *
 *   POS_Y = -1091  Fachboden -1036 minus halbe Hoehe 55: liegt AUF dem Boden (tiefster Punkt
 *                  genau -1036; die 3 Einheiten Versenkung aus Runde 30 entfallen)
 *   POS_X = -260   } Ergebnis der Suche (Raster 10 in x, 5 in z): Luft unter der geschlossenen
 *   POS_Z = 1140   } Kuppel 5,88, unter den offenen Deckeln 32,70, ganz ueber dem Achteck,
 *                  Abstand zum Sicherungs-Mantel 6,77; z 1075..1205 = LINKER Teil der Oeffnung
 *                  (die 35 links von 1110 liegen unter dem linken Deckel)
 *   ROT_Y = 1792   Nur rot_y 1024..2048 zeigt die offene Ecke (Modell +x/-z) von der Kamera weg
 *                  (Kamera bei Plattform-x +1242); 1792 = bestes Paar der Suche
 * ROOM1150 und ROOM1151 gleich (r31_pruef.py). Schirm-Schwerpunkt LINKS der Sicherung
 * (gemessen: Dossier §1.3). Eingefroren von unit_r31_hebetisch und unit_r30_granate.
 *
 * Runde 30 lag sie mittig (-362,-1088,1260) rot_y 1024 HINTER der quer liegenden Sicherung,
 * 3 tief im Boden (sonst ragte sie aufliegend 1,98 durch die geschlossene Kuppel). */
#define RE15_GRANATE_POS_X  (-260)
#define RE15_GRANATE_POS_Y  (-1091)
#define RE15_GRANATE_POS_Z  (1140)
#define RE15_GRANATE_ROT_Y  (1792)

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
 * Modal (@0x80043334). Zeitpunkt (Runde 31): erst in der RUHE OBEN von sub04 ([@0x101A, @0x1042),
 * include/re15_hebetisch.h); Wiederbewaffnung in der Parklage wie sicherung_1150.c.
 * Rueckgabe 1 = Modal wurde in diesem Bild aufgemacht. */
int  re15_granate_tick(void);

/* Die eingebackenen Engine-Bytes (gen/granate_prop.inc) fuer den Plattform-Lader. */
const uint8_t *re15_granate_md1_bytes(int *out_size);
const uint8_t *re15_granate_tim_bytes(int *out_size);

#endif /* RE15_GRANATE_H */
