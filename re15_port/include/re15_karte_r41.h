/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Punkt 2: Karte B1 — der Marker sprang vor der VORDERTUER der
 * "Waffen Armory" (Nutzer "room 1090" = ROOM1190 Ostkammer, Runde 36 M R0) in den HINTEREN Teil (Halle).
 *
 * GEMESSEN (probe_r41_kleinkram_b_karte mess, Stand 3309246f; VM im Init-Lauf getickt, Tuer-AOTs vorhanden):
 *   Gang ROOM1230 vor der Ostkammer-Tuer (Slot 4 @0x00CE6, Mitte (96,12161)): (96,13500) -> Marker (146,106),
 *   (96,13300) -> (141,109), (96,13100) -> (137,110), (96,13078) -> (137,111) — statt auf der Ostkammer-Marke
 *   (146,109) wandert er 9 px nach Westen zur Hallen-Marke (127,115). Vor der Hallen-Tuer (Slot 2 @0x00B54) stimmt
 *   er ((127,112..114)).
 * URSACHE (re15_inv_screen.c tuer_anziehen): das Symbol einer Tuer wurde als "Marke, deren andere Zone zu IRGENDEINER
 *   Zone des Zielraums gehoert" gesucht; ROOM1190 hat zwei Zonen (Halle zid 26, Ostkammer zid 110;
 *   re15_map_zones.h), und s_map_marks fuehrt die Hallen-Marke (Zeile "Gang <-> 1190 Halle") vor der Ostkammer-Marke.
 *   Beide Gang-Tueren nach 1190 zogen also auf die Hallen-Marke.
 * BELEG, WELCHE Zone eine Tuer meint: der Tuer-Datensatz selbst. Door_aot_set traegt die Ankunft im Zielraum
 *   (Bytes +14..+19 = next x/y/z, re15_aot.h door_params spawn_x/y/z): ROOM1230 Slot 4 @0x00CE6
 *   `3b 4c 00 00 b3 d0` = (19515, 0, -12109) -> Ostkammer (x 13998..21360, z -25472..-11432, zid 110);
 *   ROOM1230/1180 Slot 2 @0x00B54/@0x00A54 `66 0c 00 00 52 d4` = (3174, 0, -11182) -> Halle (zid 26).
 *   Die Zone der Ankunft ist dieselbe Zuordnung, die der Raumaufbau nach dem Durchgang trifft
 *   (scd_room_setup.c re15_map_zone_update mit der Spawn-Lage) — der Zug zielt jetzt auf genau diese Zone.
 * PORT-WAHL ohne Original-Vorbild: RE1.5 fuehrt keine Tuermarken (Zeichner FUN_800473f8 malt Rechtecke ohne
 *   Zustand), RE2 zieht den Marker nicht; der Zug ist eine Port-Ergaenzung (Runde 30), hier nur praezisiert.
 */
#ifndef RE15_KARTE_R41_H
#define RE15_KARTE_R41_H

#include <stdint.h>

/* Zonen-Nummer (zid) der Zone, in der die Ankunft der Tuer in AOT-Slot `aot_slot` im Zielraum `ziel_room` liegt;
 * -1, wenn der Slot keine Tuer ist oder der Zielraum dort keine Zone hat (dann sucht der Rufer wie bisher). */
int re15_karte_r41_tuer_zielzone(int aot_slot, unsigned ziel_room);

#endif /* RE15_KARTE_R41_H */
