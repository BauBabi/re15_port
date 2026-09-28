# Runde 31 (2026-09-29) — Auftrag des Nutzers, wörtlich

Stand vor der Runde: `r30/integration` 7cb74897 (v0.8.16 wird parallel von der Runde-30-Sitzung
gebaut und ausgeliefert, Suite 405). Drei eigene Arbeitsbäume, alle auf 7cb74897:
`.claude/worktrees/r31_tueren` (Zweig `r31/tueren`), `r31_hebetisch` (`r31/hebetisch`),
`r31_generator` (`r31/generator`).

> Während die andere Session für den Bau noch läuft, gehe hier schon mal 2 Dinge an:
> vergleiche mir die Türen aus resident evil 1.5 mit den Türen aus resident evil 2 und baue
> mir für die gleichen Türen die tür animationen. binde den korrekten sound ein. Wenn
> Türgriffe falsch sind, korrigieren sie. Sage mir wie viele Türen nicht so abgedeckt werden
> können. Außerde baue mir danach room 1170 die items im model an, und packe mir die Granate
> links in die hochfahrende Box und die Sicherung rechts. Außerdem starte mit dem Aufnahme
> Dialog der items erst wenn das Modell wirklich komplett hochgefahren ist. gleiches in room
> 11f0 bei den generator rätsel. warte erst bis der zeiger final auf 80 steht, bevor du mit ok
> das abnimmst, das Licht anschaltest etc.

## Lesart

- **T — Türen.** Jede Tür, die RE1.5 aufstellt, gegen die 55 RE2-Türarchive (`info/re2leon/COMMON/DOOR/DOORxx.DO2`)
  vergleichen. Wo es DIESELBE Tür ist (gleiche Gestaltung im Hintergrundbild), die RE2-Türsequenz mit
  dem Ton DIESES Archivs abspielen (die Maschine gibt es seit v0.8.16: `engine/src/door_seq_common.c`,
  bisher nur für das Tor ROOM1170). Türgriffe = Seite UND Form des Griffs; wo die RE2-Sequenz sie anders
  zeigt als das RE1.5-Bild, korrigieren. Am Ende die Zahl der Türen nennen, die so NICHT abgedeckt werden.
  Einordnung (Memory `reai-v2-beta-zu-retail`): RE1.5 hat die Türmaschine, aber nur DOOR00 mit dem
  einzigen Skript `Evt_end`, und alle 649 echten Türsätze tragen Archiv 0 / Variante 0 → unfertig,
  Ziel ist RE2 Retail.
- **H — Hebetisch.** „room 1170 … hochfahrende Box" = wie in Runde 30 (AUFTRAG H, vom Nutzer bestätigt:
  „ich meine ROOM 1150 mit den Hebetisch") der Hebetisch in Irons' Büro ROOM1150/1151, sub04.
  Granate (Item 0x09, obj 7) LINKS, Sicherung (Item 0x40, obj 4) RECHTS im Fach, beide sichtbar
  mitfahrend; der Aufnahme-Dialog erst, wenn die Plattform VOLLSTÄNDIG oben ist.
- **G — Generator ROOM11F0.** Die Abnahme des Rätsels (Bestätigung, Licht, Kamerawechsel usw.) erst,
  wenn der Zeiger sichtbar auf 80 STEHT.
