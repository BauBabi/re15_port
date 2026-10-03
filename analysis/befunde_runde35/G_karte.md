# Runde 35 Spur G "karte" — Dossier

Baum `.claude/worktrees/r35_karte`, Zweig `r35/karte`, Basis master 154a73c1 (geprueft: status leer, log -1 = 154a73c1).

## Punkte (Wortlaut AUFTRAG.md)
1. "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."
2. "In ROOM 11F0 taucht nicht auf der Karte auf, wenn man drin ist." + "In ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist."
3. "In ROOM 1230 bekomme ich die Map von ROOM 11E0."
4. "In ROOM 1210 ist der Korridor falsch und so gut wie alle Türen fehlen"

## Protokoll (fortlaufend)

### Werkzeug
Mess-Sonde `re15_port/tests/unit/test_r35_karte.c` (Ziel `probe_r35_karte messung`, registriert in
`tests/unit/probes/r35_karte.cmake`): faehrt den echten Kartenpfad wie `test_map_raum_live.c`
(RDT laden + `scd_room_reenter`, Spieler setzen, `re15_map_zone_update`, `re15_inv_map_stage_init`
wie `menu_common.c map_entry()`, `re15_inv_map_page_shown`, `re15_map_rect_state` je Rechteck,
`re15_inv_map_marker`). Ausgabe je Punkt: Zone (Blatt/Rect/zid), gezeigtes Blatt, Rechtecke im
Zustand AKTUELL, Spieler-Marker.

### Messung vorher (Stand 154a73c1, gebaut im Baum, 2026-10-03)
```
=== Punkt 1: Fahrstuhl ROOM1080 ===   (Etage der Kabine ueber Bank 3 Bit 54/55/56 gesetzt)
  Kabine 1F/2F/3F, 6 Punkte je Etage, IMMER: Zone Blatt 2 rect 9 | gezeigt Blatt 2 | aktuell 9
  Marker ueber den ganzen Kabinen-Innenraum (x -15500..-11800, z -3900..-300):
     x 113..118 (5 px), y 144..146 (2 px)  -- fuer 1F, 2F und 3F identisch
=== Punkt 2 ===
  11F0 Ankunft (250,250):     Zone Blatt 1 rect 0 zid 32 | aktuell: 0 | Marker (106,111)
  11F0 Mitte (7000,-12000):   Zone Blatt 1 rect 0 zid 32 | aktuell: 0 | Marker (121,137)
  1200 Ankunft (-20154,-25245): Zone Blatt 1 rect 0 zid 33 | aktuell: 0 | Marker (165,135)
  1200 Mitte (-22000,-15000): Zone Blatt 1 rect 0 zid 33 | aktuell: 0 | Marker (159,113)
  11E0 (-24707,-9442):        Zone Blatt 1 rect 0 zid 31 | aktuell: 0
  -> 11E0, 11F0 und 1200 teilen EIN Rechteck (rect 0 = die Garagen-Kachel von 11E0).
=== Punkt 3 ===
  1230 (3 Ankunftspunkte):    KEINE Zone | gezeigt Blatt 1 | aktuell: - | Marker (0,0)
  1180 (dieselben Punkte):    Zone Blatt 0 rect 6 zid 25 | gezeigt Blatt 0 | aktuell: 6
  -> 1230 zeigt Blatt 1 (B2) = das Blatt von 11E0, ohne Hervorhebung, Marker in der Bildecke.
=== Punkt 4 ===
  1210 (3 Ankunftspunkte):    Zone Blatt 1 rect 4 zid 34 | aktuell: 4   (rect 4 = 16x24-Zelle)
  1220 (5 Zellen):            Zone Blatt 1 rect 3 zid 35 | aktuell: 3   (rect 3 = 64x80-Korridor)
  Marken Blatt 1 mit zid 34/35: nur 5 (#11..#15), davon Tuer 1210<->11E0 einmal, Zellentueren 3 von 5.
```
