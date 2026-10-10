# Runde 36 — Spur K "szene1150" (Irons-Todesszene / Montage)

Baum: `.claude/worktrees/r36_szene1150` (Zweig `r36/szene1150`, Basis master 78a4a748).
Eigentuemer-Modul: `re15_port/engine/src/irons_tod_1150.c` (Spur L r35, Dossier `analysis/befunde_runde35/L_cut1150.md`).

## Auftrag (woertlich, AUFTRAG.md Z. 67-71)

> - Bei der finalen Cutscene mit Irons wo er stirbt, ist das Türknallen noch falsch. Das ist kein Türknallen, das ist einfach nur der Sound von Tür schließen. Suche den Tür Knall Sound von zum Beispiel Resident Evil 2.
> Ich würde bei door Sound, oder Zombies Breaking through Wood suchen ob dort eine Tür Knall Sound/Einbruch Sound zu finden ist.
> - Ebenfalls in der Cutscene wo Marvin und Ada in ROOM 11C0 stehen, ist der Dialog mit Marvin und Ada noch schrott. sie unterhalten sich nicht miteinander, sondern schauen irgendwo hin. Das musst du noch verbessern, das das besser passt. Sie müssen sich schon miteinandern unterhalten.
> - Ebenfalls in der Cutscene kommt das 'Türknallen' Dann die Kamera Ansicht und dann die Zombies aus dem nichts. Das soll so auch nicht sein. Erst 'Türknallen' Dann Zombie Spawnen und dann erst der Kamera Wechsel hin zu den Zombies.

Punkte: K1 = Tuerknall-Ton (RE2), K2 = 11C0-Dialog Marvin/Ada einander zugewandt, K3 = Reihenfolge Knall -> Spawn -> Kamera.

## Fortschritt (fortlaufend)

- [ ] Dossier angelegt (dieser Commit)
- [ ] Bestand gelesen (irons_tod_1150.c, knall_bank.inc, L_cut1150.md §8-10)
- [ ] K1 Messung vorher / RE2-Beleg / Umsetzung / Messung nachher
- [ ] K2 Messung vorher / Beleg / Umsetzung / Messung nachher
- [ ] K3 Messung vorher / Beleg / Umsetzung / Messung nachher
- [ ] Tests (probes/r36_szene1150.cmake)

