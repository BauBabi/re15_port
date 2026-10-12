# Runde 37 Spur L "re2zombie" — Dossier

Baum `.claude/worktrees/r37_re2zombie`, Zweig `r37/re2zombie`, Basis e399ee3f (= abgenommener Stand der Spur H 9c2a825d +
AUFTRAG/VERTRAG master d805da87).

## Wortlaut des Nutzers (AUFTRAG.md, Nachtrag 2 + Ergaenzung) und Auftrag L

> Ein weiteres Problem das ich immernoch habe - die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1
> Leon, wenn wir die ganzen Zombies spawnen nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So
> sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang.

> das soll nicht nur da gelten, das darf generell für die Zombies gelten

> schaue einfach nach wie Resident Evil 2 das macht

Orchestrator-Auftrag L (AUFTRAG.md): (a) die drei Produzenten des Arme-hoch-Gangs `+0x4 = 0x201` (EXEC[2]) im
Gang-Executor (@0x80101D68-DBC Timer, @0x80101EC4-F10 Nahbereich, @0x80101F14-F60 Spieler rennt) byte-true scharf, NACHDEM
der Nahkampf-Zusammenbruch (test_re2_zombie_abc 3523 -> 0 mit (1)+(2)) an der Ursache behoben ist; (b) DECISION[0]
Block 3 committet bei (rand & 1) == 1 wie @0x801013c8; (c) EXEC[0] P1: Selbstwecker im selben Takt, P2 erst im naechsten
wie @0x80101540-a0 / @0x8010159c. Dazu Zensus der uebrigen OFFEN-Marken (Bewegung/Angriff).

## Stand

- [ ] Schritt 0: Lektuere (AUFTRAG/VERTRAG, H-Dossier OFFEN, H_abnahme_0 H1, Code-OFFEN-Block, test_re2_zombie_abc)
- [ ] Schritt 1: Messung vorher (Zusammenbruch mit (1)+(2) reproduzieren, Bild fuer Bild; RE2-Original gegenmessen)
- [ ] Schritt 2: RE-Belege (EMZ0.BIN selbst disassembliert)
- [ ] Schritt 3: Ursache beheben, dann (1)(2)(3) + (b) + (c) scharf
- [ ] Schritt 4: Zensus OFFEN-Marken
- [ ] Schritt 5: Strom-Messung + verschobene Riegel
- [ ] Schritt 6: Messung nachher (exe), Tests

## Schritt 0 — Lektuere (Befunde)

* Die Zahl "3523 -> 0" im OFFEN-Block (enemy_ai_re2_zombie.c) stammt aus 2026-08-22, also VOR Runde 28: damals traf das
  Messer ueber die RE2-Applier-Sub-Box 3000..3400 weit. Seit Runde 28 gilt der RE1.5-Nahkampf-Kegel R = 1500
  (@0x8006E5A0[1] = 1100 + Radius 400), die Positiv-Kontrolle ist `k.hits > 250` (gemessen 364). Der Zusammenbruch muss
  daher am HEUTIGEN Stand neu gemessen werden.
* test_re2_zombie_abc zaehlt `hits` = Bilder, in denen die HP eines Zombies (RE2-Familie) sinken, ueber 64 Seeds x 900
  Bilder, Spieler steht mit R1 (Zielen) 2200..3400 vom Zombie-Schwerpunkt in ROOM1140 und schlaegt alle 7..17 Bilder
  (SQUARE-Flanke); `pl->hp = 100` in jedem Bild (der Spieler stirbt nie). Parameter `nah` ist ungenutzt (`(void)nah`).
