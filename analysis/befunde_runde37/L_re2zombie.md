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

- [x] Schritt 0: Lektuere (AUFTRAG/VERTRAG, H-Dossier OFFEN, H_abnahme_0 H1, Code-OFFEN-Block, test_re2_zombie_abc)
- [x] Schritt 1a: Nahkampf-Zusammenbruch reproduziert und AUFGEKLAERT (Messaufbau-Artefakt, s. M1)
- [ ] Schritt 1b: RE2-Original gegenmessen (DuckStation-GDB, jeder Takt)
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

## Messung vorher

### M0 — heutiger Stand (Basis e399ee3f), Messer-/Pistolen-Sweep von test_re2_zombie_abc mit Produzenten-Maske

Sonde `test_r37_re2zombie mess <maske> 64 <waffe>` (Aufbau wortgleich zu test_re2_zombie_abc run_sweep: ROOM1140, echte
Sce_em_set-Spawns, RE2-Baenke, echter re15_game_step, Spieler steht mit R1 2200..3400 vom Zombie-Schwerpunkt und schlaegt
alle 7..17 Bilder, 64 Seeds x 900 Bilder). Die Produzenten haengen an einem Sonden-Schalter
(include/re15_re2zombie_r37.h), Maske Bit0 = (1) Timer, Bit1 = (2) Nahbereich, Bit2 = (3) Rennen.

| Maske | Waffe | Treffer | Tode | Griff-Bilder | Bilder s1=2 | 0x201-Wechsel (1)/(2)/(3) |
|---|---|---|---|---|---|---|
| 0 (= Basis) | Messer | 368 | 4 | 26146 | 171 | 0/0/0 |
| 1 | Messer | 347 | 4 | 26796 | 1646 | 83/0/0 |
| 2 | Messer | 383 | 2 | 27598 | 6951 | 0/375/0 |
| 3 | Messer | 367 | 2 | 27560 | 7938 | 60/359/0 |
| 7 | Messer | 367 | 2 | 27560 | 7938 | 60/359/0 |
| 0 | Pistole | 1795 | 96 | 16004 | 3351 | 0/0/0 |
| 3 / 7 | Pistole | 1776 | 90 | 17458 | 8346 | 19/192/0 |

=> Am heutigen Stand bricht der Nahkampf mit (1)+(2) NICHT zusammen (368 -> 367, Schranke `k.hits > 250`). (3) feuert im
Sweep nie, weil der Spieler steht (0x800CFBF6 & 4 = Rennen, Port: Spieler-Motion 100).

### M1 — der historische Zusammenbruch "3523 -> 0" (Commit 187fd244, 2026-08-23) reproduziert und aufgeklaert

Quelle des Satzes: Commit 187fd244 "der zweite Gang des Zombies hatte keine Wurzelbewegung" (Bisektion "nur (1) -> 3756, nur
(2) -> 3589, (1)+(2) -> 0"). Nachgebaut: `git archive 187fd244 re15_port` in den Scratch-Ordner, (1) und (2) per Mess-Env
nach dem Wortlaut des OFFEN-Blocks eingesetzt (+0x15A-Seed (rand>>3)+100 im P0, Dekrement/Reseed/rand&3/dist<0x1388;
dist<0x7D0 && arc512 == 0 && Spieler unbeansprucht), test_re2_zombie_abc dieses Stands gebaut und gefahren:

| 187fd244 + | Pistole | Messer |
|---|---|---|
| nichts | 1712 | 3751 |
| (1) | 1691 | 3589 |
| (2) | FAIL (C) 2 Vorfaelle | **0** |
| (1)+(2) | FAIL | **0** |

(Die Einzelwerte weichen von der Notiz ab — (2) allein bricht hier schon zusammen; die Notiz hatte die Bisektion vermutlich
vertauscht: "nur (2) -> 3589" ist hier exakt der Wert von "nur (1)".)

**Mechanismus (gemessen, Instrumentierung `DBG` in der Scratch-Kopie):**
* Pistolen-Sweep Seed 58, Bild 497: der Spieler wird gegriffen (Opfer-Zustand 1, Lage (9,-20082)); im selben Bild oeffnet
  sich das Item-Get-Modal (`re15_item_modal_active()` 0 -> 1): die SQUARE-Flanke des Schlagtakts ist ohne R1-Zielhaltung (im
  Griff) die Aktionstaste und nimmt den Gegenstand am Griffplatz auf.
* test_re2_zombie_abc frame() hatte in 187fd244 KEIN Leerfahren des Modals. re15_game_step kehrt bei offenem Modal sofort
  zurueck (vor re15_enemy_ai_run_all) — der ganze Spielschritt steht fuer den Rest des Prozesses still, das Modal ist
  globaler Zustand und wird vom Seed-Ruecksetzen (re15_actor_init/re15_enemy_reset/re15_player_cmd_reset) nicht geschlossen.
* Ab Pistolen-Seed 59 und im GESAMTEN Messer-Sweep (Seeds 0..63): `modal=1`, `re15_player_is_grabbed() = 1` (der zuletzt
  abgeleitete Wert von s_player_grabbed bleibt stehen, weil run_all nie mehr laeuft), Spieler-Motion 0 statt Zielhaltung 215
  -> **0 Messer-Treffer**. Der Messer-Sweep laeuft nach dem Pistolen-Sweep im SELBEN Prozess — deshalb traf es nur den
  Nahkampf; die Pistole hatte bis Seed 58 schon 1569 Treffer gesammelt (Schranke > 500 gruen).
* Gegenprobe im selben Stand 187fd244 mit Modal-Leerfahren (Sonde test_r37_re2zombie, frame() mit Drain, Pistole dann
  Messer im selben Prozess): Maske 2 -> Messer **3500** (Maske 0: 3751). Einziger Unterschied = das Leerfahren.
* Das Leerfahren kam mit Commit **182db277** (2026-09-14, "die 'Unsterblichkeit' aus Runde 12 war mein Messaufbau, nicht der
  Port") in test_re2_zombie_abc frame(). Seitdem ist der Zusammenbruch weg (M0).

**Ursache = Messaufbau, nicht die KI:** Die Produzenten bringen mehr Zombies in Griffnaehe (s1=2: 800 -> 13234 Bilder im
Messer-Sweep 187fd244), dadurch trat das Modal-Einfrieren (ein Griff an einem Gegenstand) frueher und in einem Seed VOR dem
Messer-Sweep auf. Der Verdacht des OFFEN-Blocks ("Anmarschweg endet ausserhalb des +-0x400-Kegels, weil EXEC[2] nur mit +-16
steuert") ist damit widerlegt: EXEC[2] steuert im Original genau mit +-16 (`jal 0x80015558` a3 = 16 @0x80102354-58 / a3 = -16
@0x8010236c-70, s.u.), und der Nahkampf traf mit Drain auch mit (2) (3500).
Die Ursache ist an der Stelle behoben, an der sie lag (182db277, Testaufbau). Fuer Spur L bleibt: die Produzenten nach dem
Original einsetzen und den Nahkampf am heutigen Stand nachmessen (M0: keine Verschlechterung).
