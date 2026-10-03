/* re15_karte_fahrstuhl.h — Runde 35 Spur G: auf welchem Kartenblatt steht die Fahrstuhlkabine?
 *
 * Nutzer 2026-10-03: "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."
 * Die Kabine ROOM1080 ist auf drei Blaettern gezeichnet (1F/2F/3F), alle drei Tueren zu ihr
 * tragen Band 0 - die Etagentabelle kann sie also nicht unterscheiden und nahm immer die erste
 * Zeile (Blatt 2 = 1F). Die Etage der Kabine fuehrt das SPIEL selbst: Bank 3 Bit 54/55/56,
 * gesetzt von den Etagenraeumen beim Betreten (ROOM1040 @0x15D6 `22 03 36 01`, ROOM10C0
 * @0x0FEE `22 03 37 01`, ROOM1120 @0x0D6C `22 03 38 01`) und gelesen von ROOM1080 sub10
 * (@0x08AC/0x08BC/0x08CC). Blatt der Etagenraeume aus dem Seiten-Setzer @0x8004b568:
 * 1040 -> 2 (@0x8004b684), 10C0 -> 3 (@0x8004b6f8), 1120 -> 4 (@0x8004b758).
 * Dossier: analysis/befunde_runde35/G_karte.md (B5). */
#ifndef RE15_KARTE_FAHRSTUHL_H
#define RE15_KARTE_FAHRSTUHL_H

/* Blatt, auf dem die Kabine von `room` gerade steht; -1 = kein Fahrstuhl oder kein Bit gesetzt
 * (dann bleibt die Etagenwahl wie bisher). */
int re15_karte_fahrstuhl_blatt(unsigned room);

#endif
