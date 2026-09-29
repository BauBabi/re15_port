# Runde 33 / Thema K — Karte: 2F nach dem Irons-Hinweis wählbar + Zielraum-Markierung

Arbeitsbaum `.claude/worktrees/r33_karte`, Zweig `r33/karte`. Laufend fortgeschrieben.

## Auftrag (wörtlich)

> Nach der Cutscene mit Irons und dem Anzeigen des Communication Room, wo man hin soll, muss man
> hinterher noch in der Lage sein bei der Map zu 2F zu wechseln, um den Raum zu sehen, auch wenn man
> noch nicht auf 2F war. Außerdem muss der Raum irgendwie angezeigt bleiben, wie bei Resident Evil 2
> bei Zielräumen auch.

## Stand

- [ ] 1. Messen im Port (Etagenwahl, Darstellung, Datei:Zeile)
- [ ] 2. RE2 prüfen (Etagenwahl, Zielraum-Darstellung)
- [ ] 3. Bauen
- [ ] 4. Riegel (probes/r33_karte.cmake), RE15_MIN_TESTS
- [ ] 5. Abnahme im echten Spiel (Framedumps)

## Protokoll

---

## 1. MESSUNG im Port (Ist-Zustand, Stand 53b69a1b)

Sonde `re15_port/tests/unit/test_r33_karte.c messen` (gebaut ueber `probes/r33_karte.cmake`).
Aufbau: Weg bis Irons' Buero OHNE 2F — alle Haupt-Zeilen (`etage == 0`) der Blaetter 2 (1F),
4 (3F) und 5 (Dach) besucht, ausser Orten mit einer Zeile auf Blatt 3; Spieler in ROOM1150;
dann der Hinweis so, wie das Spiel ihn zeigt (sub08 setzt (3,94) an seinem Anfang
@0x01110, Anker = Evt_end @0x012EC), START schliesst, danach Statusschirm + L1.
Ausgabe `karte_belege/messung_vorher.txt`:

```
[M0] Weg ohne 2F: 18 Haupt-Zeilen besucht (Blaetter 2/4/5); Ziel = Blatt 3 Rechteck 9
[M1] vor dem Hinweis: flag(3,94)=0  bekannt: 0:0 1:1 2:1 3:0 4:1 5:1 6:0 ...
[M2] nach dem Hinweis: flag(3,94)=1  offen=0  bekannt: 0:0 1:1 2:1 3:0 4:1 5:1 6:0 ...
[M2] Ziel Blatt 3 Rechteck 9: Zustand UNVISITED, Blatt im Besitz 0
[M3] normale Karte: substate 1, Blatt 4
[M3] RUNTER -> Blatt 2 (Ton ja)      <- 2F (Blatt 3) wird UEBERSPRUNGEN
[M3] RUNTER -> Blatt 1 (Ton ja)
[M3] HOCH   -> Blatt 2 (Ton ja)
[M3] HOCH   -> Blatt 4 (Ton ja)      <- und wieder uebersprungen
```

Befund: nach dem Hinweis ist 2F in der normalen Karte NICHT waehlbar, und selbst wenn es das
waere, zoege der Zeichner den Funkraum nicht (unbesucht + Blatt nicht im Besitz). Der Hinweis
hinterlaesst nichts (so gebaut in Runde 30, RE2-treu: `karte-3010.md` §3.7).

Woran es haengt (Datei:Zeile, Stand 53b69a1b):

| Stelle | Inhalt |
|---|---|
| `engine/src/menu_common.c:1537-1552` | HOCH/RUNTER in `map_mode` case 1: Reihenfolge `ORDER[13]`, naechstes Blatt nur `if (re15_map_page_known(ORDER[j]))` (Z. 1546), sonst weiter |
| `engine/src/re15_map_zones.c:919-934` | `re15_map_page_known`: ein Blatt ist bekannt, wenn eine Zone darauf besucht ist ODER ein Etagen-Bit einer Zeile auf ihm steht — sonst nichts |
| `engine/src/re15_inv_screen.c:2597-2598` | Kachel-Schleife: `rs == UNVISITED && !re15_map_owned_page(page) -> continue` (RE2 @0x8006E744) |
| `engine/src/re15_inv_screen.c:2570-2579` | die Zielkachel wird NUR im Hinweis-Schirm (`hint_aktiv`) gezeichnet |

