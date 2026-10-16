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
- [x] Schritt 1b: RE2-Original gegengemessen (DuckStation-GDB, jeder Takt, M2)
- [x] Schritt 2: RE-Belege (EMZ0.BIN selbst disassembliert, R1-R4)
- [x] Schritt 3: (1)(2)(3) + (b) + (c) scharf; dazu DECISION[12] (Zensus-Fund)
- [x] Schritt 4: Zensus OFFEN-Marken
- [x] Schritt 5: Strom-Messung + verschobene Riegel
- [x] Schritt 6: Messung nachher (Harness + exe Leon/Elza), Tests
- [x] Fortsetzung 2026-10-13: Stand gegen den Auftrag geprueft ((a)(b)(c) Code gegen Disassembly, (c) inkl. Moan-Takt),
      breitere Testauswahl -> vier weitere verschobene Riegel gefunden und mit Mechanismus angepasst (5b), Abschluss
- [x] Nachbesserung 1 (2026-10-14, nach Abnahme 0): M1 = Nutzer-Satz zum Messer an die exe-Messung angepasst (reine
      Textkorrektur, kein Code), s. Abschnitt "Nachbesserung 1"
- [ ] Nachbesserung 2 (begonnen 2026-10-14, am Monatslimit abgebrochen, Rest 7f581fde UNGEPRUEFT = nur die |dx|+|dz|-
      Kennzeichnung in M2/N1 und diese Zeile; FORTGESETZT 2026-10-16 nach dem Merge von master 44721dca und der Freigabe
      des Orchestrators): M1 = Erholungsleiter P3 im CODE korrigiert (Inversion @0x80105B3C + Ein-Angreifer-Riegel
      @0x80105AA0-B0 + 0x800152C8 @0x80105B98), Texte richtiggestellt; M2 = Messer-Aussage phasenrobust, nach dem Fix an der
      exe neu gemessen. Abschnitt "Nachbesserung 2"

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

### M2 — RE2-Original gegengemessen (DuckStation-GDB, JEDER Takt, `L_belege/re2_gdb_0x201.py`)

Spielstand SLUS-00748_6.sav (ROOM20A0, sechs Zombies Deskriptor 6 wie Spur H R4), Haltepunkt EXE-Gegnerschleife @0x800265E0
(`sw v0,496(s0)`), 900 Takte je Zombie (5400 Proben, 79 s), Leon steht. settings.ini und resume.sav gesichert und
zurueckgeschrieben. Rohdaten `L_belege/re2_original_room20a0_jeder_takt.txt.gz`, Auswertung `L_belege/re2_0x201_ausw.py`
(Peilung = -atan2(dz,dx) in 4096-Teilung, empirisch: 3508 von 4188 Gang-Takten zeigen innerhalb +-128 auf das Steuerziel).

| Zombie | Takt | von | +0x1F0 im Takt | Spieler im Kegel | Produzent | danach |
|---|---|---|---|---|---|---|
| 8014777c | 421 | 0x00010101 | 1999 | +-512 ja (Peilung 1119, Gier 1119) | (2) Nahbereich | 16 Takte 0x201, dann GRIFF 0x301 bei +0x1F0 1180 |
| 80156d64 | 773 | 0x00010101 | 1987 | ja (963/961) | (2) | 4 Takte, dann 0x101 (Spieler beansprucht) |
| 8016634c | 696 | 0x00010101 | 3565 | ja (1271/1273) | (1) Timer: +0x15A 1 -> 125 | 80 Takte 0x201, dann GRIFF bei 1152 |

Weg je Takt als Summe |dx|+|dz| (Zaehler in `re2_0x201_ausw.py`): Gang 0x101 32,2 (4213 Takte), Arme-hoch-Gang 0x201 44,1
(94 Takte); als Wegstrecke (euklidisch, gleiche Takt-Auswahl, Nachbesserung 2) 25,6 / 35,6. Alle Zustandswechsel der 5400 Proben:
1/1->1/2 3x, 1/2->1/3 2x, 1/2->1/1 1x, 1/3->1/5 2x, 1/5->1/1 3x, 1/1->1/9 5x (Rueckstoss-Domino eines abgeworfenen Greifers,
`sw 0x901` @0x80102E90 — im Port vorhanden, enemy_ai_re2_zombie.c re2z_exec_grab), 1/9->1/5 5x.
=> **So macht es RE2:** ein gehender Zombie hebt die Arme (0x201), sobald Leon im ersten Takt unter 2000 vor ihm steht
(Produzent (2)) oder wenn sein +0x15A-Timer unter 5000 ablaeuft und 1/4 faellt (Produzent (1)); aus dem Arme-hoch-Gang greift er
im ersten Takt unter 1200. Produzent (3) (Leon rennt) kommt im Lauf nicht vor (Leon steht) — statisch belegt (R1).

## RE-Belege (EMZ0.BIN = RE2-Zombie-Overlay @0x80100000, re2_disasm.py, selbst disassembliert)

### R1 — EXEC[1] (Gang, 0x80101A40, Tabelle 0x8010C8CC[1]): die drei Produzenten von +0x4 = 0x201
```
(1) 80101d68 lhu v0,346(s1) / 80101d70 addiu v0,v0,-1 / 80101d74 sh v0,346(s1) / 80101d78 sll v0,v0,16
    80101d7c bne v0,zero,0x80101dc0 / 80101d84 jal 0x80015fe8 / 80101d8c sra v0,v0,3 / 80101d90 addiu v0,v0,100
    80101d94 jal 0x80015fe8 / 80101d98 sh v0,346(s1) (Delay-Slot) / 80101d9c andi v0,v0,0x3 / 80101da0 bne -> 0x80101dc0
    80101da8 lw v0,496(s1) / 80101db0 sltiu v0,v0,0x1388 / 80101db4 beq -> 0x80101dc0 / 80101db8 addiu v0,zero,513 / 80101dbc sw v0,4(s1)
(2) 80101ec4 lw v0,496(s1) / 80101ecc sltiu v0,v0,0x7d0 / 80101ed0 beq -> 0x80101f14
    80101ed8-e4 a1/a2 = 0x800cfc30/0x800cfc38 (Spieler x/z) / 80101ee8 jal 0x80015614 / 80101eec addiu a3,zero,512
    80101ef0 bne v0,zero -> 0x80101f14 / 80101efc lbu 0x800cfdcb / 80101f04 andi 0x80 / 80101f08 bne -> 0x80101f14
    80101f0c addiu v0,zero,513 / 80101f10 sw v0,4(s1)
(3) 80101f18 lhu 0x800cfbf6 / 80101f20 andi v0,v0,0x4 / 80101f24 beq -> Epilog 0x80101f64
    80101f2c lw v0,496(s1) / 80101f34 sltiu v0,v0,0x9c4 / 80101f38 beq -> Epilog
    80101f50 jal 0x80015614 / 80101f54 addiu a3,zero,1324 / 80101f58 bne -> Epilog / 80101f5c addiu v0,zero,513 / 80101f60 sw v0,4(s1)
```
Kegeltest 0x80015614 (RE2 PSX.EXE, selbst gelesen @0x80015614-94): Rueckgabe 0, wenn ((Peilung - Gier + h) & 0xfff) < 2h, sonst
+-h = Port re15_ai_arc_test. Steuerung 0x80015558 (@0x80015558-610): negative Rate = weg vom Ziel (Peilung + 2048) = Port
re15_enemy_steer_point. Reihenfolge im Takt: (1) vor dem Brand-/Saeure-DoT @0x80101DC0-EC0, (2)/(3) danach; kein Rueckkehrweg
zwischen (2) und (3), jeder Produzent schreibt nur das Wort (0x201 -> +0x6 = 0: EXEC[2] P0 laeuft im naechsten Takt).
EXEC[2] (0x80102260, Arme-hoch-Gang) selbst nachgeprueft (@0x80102260-5E0): P0 Clip 0xF0004 + walkclip, +0x158 = (r&0x3f)+180,
Startbild (r&0x1f), +0x15A = (r&0x1f)+45; P1 Steuerung +-16 @0x80102354/@0x8010236c, Wurzelbewegung @0x801023dc/@0x8010246c,
Ausstieg @0x80102550-cc — deckt sich Zeile fuer Zeile mit re2z_exec_bump (seit 187fd244). Der Verdacht "steuert nur +-16 statt
8+16" ist damit KEIN Fehler: das Original steuert im Arme-hoch-Gang genau mit +-16.

### R2 — DECISION[0] Block 3 (0x80101294, Tabelle 0x8010C88C[0])
```
801013bc jal 0x80015fe8 / 801013c4 andi v0,v0,0x1 / 801013c8 beq v0,zero,0x801013d4 / 801013cc addiu v0,zero,3073 / 801013d0 sw v0,4(s0)
```
= Commit 0xC01 bei (rand & 1) == 1. Block 2 dagegen `andi v0,v0,0x1` @0x80101368 / `bne v0,zero,0x80101380` @0x8010136c = Commit bei
== 0. Der Port hatte beide Bloecke auf == 0.

### R3 — EXEC[0] P1 (0x801013F4): Wander-Wurf und Selbstwecker im SELBEN Takt
```
8010151c lh v0,346(s0) / 80101524 bne v0,zero,0x80101544 / 8010152c jal 0x80015fe8 / 80101534 andi v0,v0,0x1
80101538 bne v0,zero,0x80101544 / 8010153c addiu v0,zero,2 / 80101540 sb v0,6(s0)            ; +0x6 = 2, KEIN Sprung
80101544 lh v0,344(s0) / 8010154c bne v0,zero,0x80101590                                    ; Selbstwecker
80101554 lw v0,496(s0) / 8010155c sltiu v0,v0,0x1d4c / 80101560 beq / 80101568 lhu v0,340(s0) / 80101570 andi 0x800 / 80101574 beq
80101578 addiu v0,zero,257 / 8010157c sw v0,4(s0)                                           ; ganzes Wort -> +0x6 = 0
80101580 lh v0,344(s0) / 80101588 beq v0,zero,0x801016ac                                    ; Epilog
80101590 lhu v0,344(s0) / 80101598 addiu v0,v0,-1 / 8010159c j 0x801016ac / 801015a0 sh v0,344(s0)
```
P2 (@0x801015a4) nur ueber den Phasen-Dispatch @0x80101410-50 (+0x6 == 2), also im naechsten Takt.
Fortsetzung 2026-10-13, P1 ab 0x80101494 nachgelesen (re2_disasm EMZ0.BIN, 68 Instr.): auch der MOAN-Takt erreicht den
Wecker — `bne v1,zero,0x80101500` @0x801014b4 (+0x15A vor dem Dekrement != 0), sonst Moan-Block @0x801014bc-fc
(Reseed `addiu v0,v0,300` / `sh v0,346` @0x801014f8-fc), Durchfall nach @0x80101500 (`lbu v0,334` / `bne` / `jal 0x80016200`
@0x80101500-18), dann @0x8010151c `lh v0,346` (nach dem Reseed != 0) -> `bne` -> 0x80101544 = Wecker. Der Port
(re2z_exec_stand) laeuft nach beiden Zweigen in den Wecker — deckt sich. `jal 0x80016200(self,0,1)` @0x80101514 (bei
+0x14E == 0) ist der Lage-Nachzieher auf Part 1 (0x80016200 selbst gelesen: Part-Matrix, a1 = 0 -> +0x38/+0x40 -= Versatz
@0x800162a8-d8) — Lage-Korrektur der Pose, keine Entscheidung (Zensus).

### R4 — DECISION[12] (0x8010460C) ist KEIN Stub (Zensus-Fund, s. Schritt 4)
`table 0x8010c88c 16`: [0] 0x80101294, [1] 0x80101714, [2] 0x80101f7c, [3..11] und [13..15] je `jr ra`/`nop` (0x801025e4,
0x80103170, 0x80103180, 0x8010394c, 0x80103778, 0x80103b6c, 0x80103e40, 0x80104174, 0x80104394, 0x80104920, 0x80104d6c,
0x80104e4c), aber **[12] = 0x8010460c = eine echte Entscheidung**:
```
8010462c lbu v0,6(s0) / 80104634 bne v0,v1(3) -> 0x801046a0          ; nur Phase 3 (Stoss, Clip 0x1B)
80104630 lw a0,496(s0) / 8010463c sltiu v0,a0,0x514 / 80104640 beq   ; +0x1F0 < 1300
80104648-58 lbu 0x800cfdcb / andi 0x80 / bne                          ; Spieler nicht beansprucht
80104660-6c lbu v0,333(s0) / sltiu v0,v0,0xb / beq                    ; Stoss-Bild +0x14D < 11
80104674-84 lbu v1,262(s0) / lbu 0x800cfcfe / bne                     ; gleiche Etage
8010468c-9c lbu 0x800cfc00 / xori 0xf / sltu a1,zero,v0               ; Spieler+0x8 != 15
801046a8-d8 +0x21A & 0x20 == 0 && 0x80015758(self,PL,+0x76+256,256) == 0 -> addiu v0,zero,769 / sw v0,4(s0) ; GRIFF
801046dc-e8 PL+0x1D3 |= 0x80
801046ec-1c +0x21A & 0x40 == 0 && 0x80015758(self,PL,+0x76-256,256) == 0 -> 769 / sw ; GRIFF
80104720-2c PL+0x1D3 |= 0x80
```
Der Port fuehrte [12] unter `default: break` — ein Ausfall-Stoss (0xC01, DECISION[0] Bloecke 2/3, DECISION[1] D/E, DECISION[2] C)
endete IMMER in 0x101 und nie im Griff. Das betrifft "greifen und beissen wie in RE2" direkt.

## Umsetzung

Datei `re15_port/engine/src/enemy_ai_re2_zombie.c` (VERTRAG Ausnahme Spur L), Kommentar "Runde 37 Spur L" + @0x je Zeile:
* Produzent (1) in re2z_exec_walk (+0x15A-Block, Wuerfe waren schon da): `+0x4 = 0x201` @0x80101DB8-BC.
* Produzenten (2)/(3) nach dem DoT-Block: dist < 0x7D0 && arc512 == 0 && !re15_player_is_grabbed() / cfbf6 & 4 (Port: Spieler-
  Motion 100 = RUN, re2z_cfbf6) && dist < 0x9C4 && arc1324 == 0 -> 0x201.
* (b) re2z_decide_stand Block 3: Commit bei (rand & 1) != 0.
* (c) re2z_exec_stand P1: der Wecker laeuft in jedem P1-Takt (auch im Wurf-Takt) und endet immer im Epilog; P2 im naechsten Takt.
* DECISION[12] re2z_decide_lunge + `case 12` in re2z_active.
* Sonden-Schalter (NUR Gegenproben): `re15_re2z_r37_produzenten(maske)` in NEU `re15_port/include/re15_re2zombie_r37.h`,
  Bits (1)/(2)/(3)/(b)/(c)/D12, Standard RE15_R37L_ALLES = Original; Maske 0 = exakt der Stand bis Runde 37 (gemessen: Messer-
  Sweep Maske 0 = 368 Treffer / 26146 Griff-Bilder / 171 Bilder s1=2 = Basis-Build e399ee3f bitgleich). Dazu Zaehler je
  Produzent, Produzent des letzten Wechsels je Platz, `re15_re2z_rng_peek()` (Wurf vorhersehen ohne den Strom zu bewegen).
* Stale-Kommentar korrigiert: Kopf OFFEN-Block "BEWUSST NICHT SCHARF" ersetzt (Ursache aufgeklaert, s. M1).
Keine neuen Assets, keine Sprachdateien, keine Bank-9-Bits, keine Text-Ids, keine neue .c-Datei (nur ein neuer Header).

## Schritt 4 — Zensus der OFFEN-Marken der RE2-Zombie-KI (Bewegung/Angriff)

| Marke (enemy_ai_re2_zombie.c) | Adresse | wirkt auf Aktivitaet? | Ergebnis |
|---|---|---|---|
| 0x201-Produzenten (1)(2)(3) | @0x80101D68-DBC / EC4-F10 / F14-F60 | JA (Annaeherung mit erhobenen Armen, Griff) | geschlossen (R1) |
| DECISION[0] Block 3 | @0x801013c8 | ja (Ausfall aus dem Stand) | geschlossen (R2) |
| EXEC[0] P1 Wecker/Wurf | @0x80101540-a0, @0x8010159c | ja (Aufwachen/Wandern) | geschlossen (R3) |
| DECISION[12] `default: break` ("stubs / not RE'd") | @0x8010460C | JA (Griff aus dem Ausfall) | geschlossen (R4) |
| DECISION[3..11],[13..15] | 0x801025e4 ... 0x80104e4c | nein | `jr ra`-Stubs (je Eintrag geprueft) |
| Kriecher-Test P1 (~Z.3187 "OFFEN ... NICHT genommen") | @0x80104530-B8 | ja | ERLEDIGT seit Welle 5 (re2z_exec_eleven, `re15_re2z_enter_crawler`); Kopfkommentar war veraltet |
| Kriecher mit +0x5 > 2 (~Z.3670) | Griff-Wurf `sw 0x501` @0x80102D24-2C | nein (Folgezustand NACH einem Griff eines Kriechers; Annaeherung/Griff selbst byte-true) | bleibt OFFEN mit Adresse (im Original unerreichbar: Tabelle 0x8010C918 hat 3 Worte) |
| Steuerziel = RE2-Navigator FUN_8004A808 "nicht portiert" (~Z.290) | @0x80100354 | ja (Weg um Hindernisse) | Port nimmt den RE1.5-Navigator FUN_80039e7c (re15_nav_update_steer) + zonenlos-Form @0x8004a90c-924 (Runde 36 F) — RE1.5-Raumdaten tragen RE1.5-Zonen; kein RE2-Zwilling moeglich. Kommentar veraltet ("nimmt die Spielerlage"). Befund N3 (Tasche 1030) gehoert hierher |
| Gait-Draws +0x16A/+0x14D "NOT ported" (~Z.393) | @0x80101AD4-E8 / @0x80101B04-0C | ja (Strom) | ERLEDIGT (B3-Fix: Draw 3/4 in re2z_exec_walk P0); Kommentar veraltet |
| EXEC[2] Drei-Bild-Zweig "NICHT modelliert" (~Z.1827) | @0x801023F8-468 | nein (Kadenz) | ERLEDIGT (B4-Fix re2z_fat_cadence_tick(e,0)); Kommentar veraltet |
| fill_gates Block A / C / K, PL+0x8 == 15 | @0x8010200C / +0x1D4 / @0x80102208 / @0x80102108 | nein | inert mit Beleg (kein Schreiber im Zombie-Overlay bzw. RE1.5-Raumdaten / Spieler-HP nie -32768 / Spieler-Id nie 15) |
| Liege-Wecker (EXEC[7] "WAKE not RE'd") | @0x801038D8/0x80103900 | ja (Liegende) | PORT-MAPPING mit RE1.5-Beleg (Nibble 7/8 ohne Wecker, DECIDE-Stub 0x801039F4; Skript-Wecker 0x89/0x8A) — RE1.5-Raumdaten, bleibt |
| Fresser-Wecker (EXEC[8] "conditions not RE'd") | @0x80103D94 | ja (Fresser) | PORT-MAPPING RE1.5-Fresser dist < 0xFA0 (STAGE1.BIN @0x80103990-98), RE2-Weg Bank-B @0x80104F0C ohne Produzent — bleibt |
| Knockdown P7 "Liegezeit 90 MAPPED" (~Z.2262) | @0x80103628 | nein | ERLEDIGT (Review #6: Clip laeuft aus); Kommentar veraltet |
| EXEC[11] gerade Takte +0x144 >> 2 (~Z.3296) | @0x801044D0-E0 | nein (Ausrollen beim Zusammenbruch) | bleibt OFFEN mit Adresse |
| EXEC[10] Haemmern / EXEC[15] Bank B | 0x8010417C / 0x80104E54 | nein | inert (+0x1D4-Bits bzw. kein Produzent) |
| Gore/Teile-Physik (Wand/Boden/Skalierung), Rumble, Praesentation | @0x80028CAC u.a. | nein | Praesentation, bleibt |
| EXEC[0] P1 `jal 0x80016200(self,0,1)` bei +0x14E == 0 ("benannt, nicht portiert") | @0x80101500-18 | nein (Lage-Nachzieher auf Part 1, @0x800162a8-d8; kein Zustand, kein Wurf) | bleibt, mit Adresse (Fortsetzung) |
| Wurzel-Re-Anker beim Clipwechsel (Port: Delta 0) gegen e7c Bild 0 = root(kf0) | @0x80015F04-1C | ja (Weg im ersten Bild eines Clips mit root(kf0) != 0, z.B. RIP-Clip 4 (-277,-26)) | vorbestehend, nicht L (enemy_ai_common.c, Spur H); OFFEN mit Adresse (5b) |
| Treffer-Handler P3: Leiter-Ende umgekehrt + Ein-Angreifer-Riegel fehlt (Nachbesserung 2) | @0x80105B30-98, @0x80105AA0-B0 | JA (Erholung: 0x10101 statt 0x101 -> EXEC[1]-P0; +0x15A 300..555 statt 100..131 = Timer-Produzent (1) nach Treffern) | GESCHLOSSEN (Freigabe 2026-10-16, R5) |
| Treffer-Handler P0 faellt im selben Takt nach P1 durch (Port: `break`) | @0x80105714-3C | gering (Reaktion +1 Takt) | OFFEN, nicht freigegeben (case 0) |
| Treffer-Handler P1 halbiert den Wurzel-Vektor | @0x80105770-94 | gering (Weg in P1) | OFFEN, nicht freigegeben (case 1) |
| Treffer-Handler P2-Schwanz: Griff 0x301 aus der Reaktion (+0x1F0 < 0x514, Sektor 288) | @0x80105980-A4C | JA (Griff waehrend der Trefferreaktion) | OFFEN, nicht freigegeben (case 2); offen: Port-Zwilling Spieler+0x220 Bit 0 (@0x801059C0-D8) |
| Treffer-Handler: kein Bildvorschub in P0/P3 (Port-Advancer rueckt jeden Takt vor) | @0x80105790 / @0x801058C0 (nur P1/P2) | nein (Bild) | OFFEN, nicht freigegeben (player_common.c) |

## Schritt 5 — Zufallsstrom und verschobene Riegel

### Strom (Wuerfe je Bild, re15_re2_rand_draws, Sonde test_r37_re2zombie mess, 64 Seeds x 900 Bilder, ROOM1140)
| Lauf | vorher (Basis-Build e399ee3f-Verhalten) | nachher (Spur L) |
|---|---|---|
| Messer-Sweep | 14003 Wuerfe = 218,8 je Seed = 0,243 je Bild | 15045 = 235,1 je Seed = 0,261 je Bild |
| Pistolen-Sweep | 13538 = 211,5 je Seed = 0,235 je Bild | 13831 = 216,1 je Seed = 0,240 je Bild |
Mehrwuerfe = EXEC[2] P0 (+0x158, Moan, Startbild, +0x15A: 3..4 Wuerfe je Eintritt @0x801022a8-334) und EXEC[2] P1 bei
Wandkontakt (@0x801023ac-cc). (b) aendert die Wurfzahl nicht (nur die Richtung je Wurf), (c) nur den Takt der P2-Wuerfe.

### Verschobene Riegel (gleiche -R-Auswahl, Basis-Build e399ee3f im Scratch-Ordner gegen Spur L, `ctest -V`, Vergleich
`L_belege/riegel_vergleich.py`): 163 Tests, 140 mit bitgleicher Ausgabe. Geaenderte Ausgabe:

| Riegel | alt | neu | Begruendung |
|---|---|---|---|
| unit_re2_zombie_abc | Pistole 1795 / 96 Tode / DOWN 96..324 (250); Messer 368 | Pistole 1775 / 90 / 94..323 (254); Messer 373 | Strom + Arme-hoch-Gang; Schranken > 500 / > 250 gruen; Nahkampf NICHT schlechter |
| unit_r36_zombieki_eintritt | KLEMME 0; Taschenfall-Gegenprobe 450 Bilder | KLEMME 1 (1030<-1050 hin Slot 1); Gegenprobe 30 Bilder -> ROT | **angepasst**, s.u. |
| unit_r36_zombieki_wecker | 10B0/1220 Weck-Zeilen "Bild 2 sub 1" | "Bild 2 sub 2" | Leon 800 vor dem Steher: DECISION[0] 0x101, EXEC[1] im selben Takt (Dispatcher liest +0x5 neu @0x801011d0), Produzent (2) (800 < 0x7D0, Kegel) -> 0x201 — byte-true; Riegel prueft "reagiert in Bild <= 2" (gruen) |
| unit_r36_zombieki_sicht | 1010 Slot 1/2 Zaehler 74/2 bzw. 4/0 | 60/0 bzw. 4/0 | Strom; Riegel gruen |
| unit_r36_zombieki_szenen | erneuter Angriff F340/299/148/307 | F349/276/145/319 | Strom; gruen |
| unit_r35_inhalt_zombies | 1220 Slot 2 Griff Bild 12 | Bild 15 | Strom; gruen |
| unit_r35_zgirl_ki_re2 | Griff ab Bild 155 | 154 | Strom; gruen |
| unit_re2z_bump_walk | EXEC[2] ab Bild 120 (gesetzt), 9 Bilder, dist 1492 -> 1200 | ab Bild 100 (Produzent (2) bei 1962), 21 Bilder, 1962 -> 1214, Ausgang Griff | der Gang mit erhobenen Armen entsteht jetzt von selbst; gruen |
| unit_re2_room1140_ab | B RE2 Slot 4/5 Wechsel 0/0 | 2/112 | 0x101 <-> 0x201 zaehlen als Wechsel; gruen |
| unit_re2_lyer_1140 | Ende 1/1/1 Clip 0 | Ende 1/12/3 Clip 27 (Ausfall) | Strom; Riegel prueft das Aufstehen (Bild 82, gruen) |
| r28_messer_dauerschlag | A: 50 Schlaege / 17 Treffer / Tod F1095; B Griff 3 | 48 / 17 / F1056; B Griff 4 | Strom; gruen |
| unit_1030_crawl_live | RE2 z end -10392 | -9756 | Strom; gruen |
| unit_re2_kfjump, unit_re2_zombie_teardeath, unit_r36_blut_pin_kriecher, unit_re2z_bandlock_pin, unit_re2z_grab_core_se (699 -> 702 Zuege), unit_re2z_rise_hittable (683/19 -> 708/16) | Messwerte | Messwerte verschoben | Strom; alle gruen |
(Fuenf Tests liefen im Scratch-Basisbaum mangels Dateien ausserhalb re15_port rot — unit_r30_irons_diary_ablauf,
unit_r34_re2fx_knochen, unit_r36_szene1150_knall_re2, unit_re15_re2z_gore_se, unit_re2_crow_se_bank —; im Spurbaum vorher
und nachher gruen, nicht vergleichbar.)

**unit_r36_zombieki_eintritt — Anpassung mit Beleg (Runde 36 Spur F, test_r36_zombieki_eintritt.c):**
1. KLEMME 1030<-1050 "hin" Slot 1 (Typ 0x16, grid 0x0D, geht ab Spawn): GEMESSEN mit dem Basis-Build (gleicher Lauf
   `spur 1030 1050 2 1 1 1200 30`): derselbe Zombie steht schon VORHER ab F330 an derselben Wand (-6230,-24838), Leon (-6170,
   -22303) auf der anderen Seite der Tresen-/Wandzelle (Raster `raster 1030 1 125`: solide z -24734..-22484, x -8117..-3600),
   870 Bilder lang — der Zensus zaehlte ihn nicht, weil die KLEMME eine "Reaktion" (Zustandswechsel) verlangt und ein ab Spawn
   gehender Zombie vorher nie wechselte. Jetzt wechselt er in F230 per Produzent (1) in den Arme-hoch-Gang und in F270 per
   DECISION[2] Block C in den Ausfall — der Zensus sieht den vorbestehenden Stillstand. Ursache des Stillstands: das
   Steuerziel (-4984,-23860) des Navigators liegt IN der soliden Zelle (OFFEN, s.u.). Anpassung: jeder KLEMME-Fall wird mit
   dem Stand bis Runde 37 (Sonden-Maske 0) gegengelaufen; steht der Zombie dort ebenfalls >= 300 Bilder aufrecht still
   (Leon > 1500, ohne Reaktions-Bedingung), zaehlt er als "vorbestehend" (ausgegeben, nicht gezaehlt). Gemessen: 600 Bilder.
2. Taschenfall-Gegenprobe (Wurf-Klemme aus): der Rueckstoss in die Tasche haengt an den Wuerfen des Griffs; mit Spur L 30
   statt >= 300 Bilder. Anpassung: das Paar Gegenprobe/Fix laeuft im Strom bis Runde 37 (Maske 0, Befund 450 Bilder, Fix
   0 Bilder), zusaetzlich neuer Check "Taschenfall mit Wurf-Klemme UND Spur L": keine KLEMME (30 < 300), Leon erreicht (92).
Nach der Anpassung: 11/11 gruen.

### 5b — Fortsetzung 2026-10-13: drei weitere verschobene Riegel (nicht in der -R-Auswahl der Liste oben)

Die gezielte Pruefung der Fortsetzung (breitere -R-Auswahl, s. Tests) fand drei rote Riegel der Runde 36 Spur C, die
in der 163er-Auswahl oben fehlten: `unit_r36_blut_pin_eben` / `_tief` / `_rip` (probe_r36_blut, Blut-Anker des
RE2-Zombies). Basis-Build e399ee3f (Scratch) im selben Lauf: alle drei gruen. Mechanismus je Fall GEMESSEN
(Sonde voruebergehend instrumentiert: Lage/Gier/Motion/Bild/Bank/Blend vor und nach dem Spielschritt, Anker vor dem
Schritt, Abstand zur gezeichneten Matrix T-1 und T; Instrumentierung danach zurueckgenommen):

| Riegel | alt (Basis) | neu (Spur L) | Mechanismus |
|---|---|---|---|
| blut_pin_eben | AUF F330/F342 d = 24 / 6, OK | AUF F318 d = 46 (> 40), 3 FAILURES | Strom: der Zombie liegt auf der ANDEREN Seite (+0x16A = 1, Clip 9 statt 8). Der Aufsteh-Treffer @0x80107AEC spawnt in P0, der Port-Anker rechnet (Port-Konvention "Pose des LAUFENDEN Bilds", re15_schuss_r36.h) die reine Keyframe-Pose Bild 63 (Anker vor dem Schritt Bild 62: y -1973, Spawn y -2020 = Kopfweg eines Bilds ~47); danach setzt P0 das Blend-Byte `sb 15,334` @0x80107B3C, die gezeichnete Pose friert auf dem Vorbild ein (wp = 15 x 0x200 -> 0x1000). d = Kopfweg eines Bilds im Aufsteh-Clip 9 |
| blut_pin_tief | AUF F174 d = 32, OK | AUF F162 d = 67 (> 40), 3 FAILURES | dasselbe, Clip 9 Bild 10 -> 11 |
| blut_pin_rip | F66 d = 0, OK (Tod aus 1/1/1) | F66 d = 278 (Kopf + RIP-P1), 3 FAILURES | Tod aus dem ARME-HOCH-GANG 1/2/1 (Produzent (2)): Loco-Clip 4 -> Haupt-Clip 4, Motion bleibt 4. RIP P0 spawnt (@0x80108E28, @0x80109054) und faellt nach P1 durch (@0x80109078): `jal 0x80015E7C` @0x80109084 + 0x800152C8 @0x801090B0. e7c im Bild 0: `lbu v0,333` / `bne` / `sw zero,524` / `sh zero,528` @0x80015F04-1C -> Momentaufnahme 0 -> +0x144 = root(kf0) = (-277,-26) (gemessen r144 -277, snap (-277,-26)) -> der Zombie rueckt NACH dem Spawn 278 weiter (Lage F66 (-1487,-19963) -> Schub -450 -> (-2193,-19785)). Aus 1/1/1 (Motion 0 -> 4) verankert der Port neu (Delta 0, re15_clip_root_motion_delta fr_prev < 0) |

Bewertung: (a) ist die Port-Konvention des Blut-Ankers der Spur C (Original: Anker = Record+72 des Vorbilds,
re15_schuss_r36.h) — Spur L wuerfelt nur einen Aufsteh-Clip mit groesserem Kopfweg je Bild; (b) ist der
e7c-Bild-0-Wurzelschritt des Originals (@0x80015F14-1C, aus JEDEM Vorzustand), den der Port bisher nur dann nicht
faehrt, wenn sich die Motion-Nummer aendert (Port-Re-Anker) — der Weg aus 0x201 ist hier der byte-treuere; die
Sonde (G) misst den Spawn gegen die Pose NACH dieser Bewegung im Handler (wie beim MG-Schub @0x8010810C-6C, dort
Schranke 100). Keiner der drei Faelle ist ein Fehler im Code der Spur L.
Anpassung: probe_r36_blut laeuft im RE2-Zombie-Strom bis Runde 37 (`re15_re2z_r37_produzenten(0u)` im Kopf von main,
Kommentar mit beiden Mechanismen). GEMESSEN: alle neun Varianten (eben/tief/mg/rip/arm/burst/glied/kriecher/kriecher5)
liefern danach eine zum Basis-Build e399ee3f BITGLEICHE Ausgabe (cmp je Variante), alle `probe_r36_blut: OK`.
Beide Mechanismen stehen unter OFFEN (Spur C/G: Anker-Zeitpunkt; Port-Re-Anker beim Clipwechsel gegen e7c).

Vierter Riegel aus dem breiten Unit-Lauf (alle Nicht-Integrations-Tests, s. Tests): `probe_10d0_situp_handoff`
(probe_10d0_situp_re2 8, 10D0-Sitzer -> RE2-Gehirn). Alt: "Uebergabe-Tick t88: clip=0 af=16 frac=15 b8dy=-2527", OK.
Neu: "FAIL: der Sitzer erreicht den RE2-WALK nie". Mechanismus: die Sonde suchte den ersten Tick mit +0x5 == 1; EXEC[1]
P0 faellt aber im selben Tick nach P1 durch (`sw v0,332` @0x80101a8c, P0 endet @0x80101b20 ohne Sprung, P1 ab
0x80101b24 — selbst disassembliert) und dort schreibt Produzent (2) @0x80101EC4-F10 sofort 0x201 (Leon steht 1694 vor dem
Sitzer, < 0x7D0, im Kegel). Der Tick endet mit +0x5 = 2. Anpassung: Uebergabe = erster Tick mit +0x5 in {1, 2} (Ausgabe
nennt s1). Gemessen danach: "Uebergabe-Tick t88: s1=2 clip=0 (RE2-Bank hat 31 Clips) af=16 frac=15 b8dy=-2527",
`probe_10d0_situp_re2: OK` — Tick, Clip, Bild, Blend und Brusthoehe bitgleich zum Basis-Build, nur s1 = 2 statt 1.

## Messung nachher

### N1 — Port-Harness (echte Raumeintritte, echte Spielschleife; Sonde `test_r37_re2zombie gang`)
| Lauf | Wechsel nach 0x201 | +0x1F0 im Wechsel-Takt | danach | Weg je Bild als |dx|+|dz| 0x101 / 0x201 | Gegenprobe Maske 0 |
|---|---|---|---|---|---|
| ROOM1130 nach der Montage, Leon steht an der Tuer (900 Bilder) | 1 (Produzent (2), Slot 5 Typ 0x11, F864) | 1993 | Griff F883 (+0x1F0 1193) | 31,0 / 53,8 | kein 0x201, erster Griff F891 |
| ROOM1180 Normalspiel, Leon am Eintritt aus 1160 (900) | 2 (beide (2), Slot 3 Typ 0x10, F284/F803) | 1995 / 1991 | Griff F312 / F837 (+0x1F0 1193 / 1180) | 26,0 / 33,5 | kein 0x201 |
| ROOM1180, Leon RENNT geradeaus (Gier 0/1024/2048/3072, 300 Bilder) | je 1-2 durch (3) | 2495 (< 0x9C4) | — | | — |
RE2-Original (M2): Wechsel bei 1999/1987 (2) bzw. 3565 (1), Griff bei 1180/1152, Weg (|dx|+|dz|) 32,2 / 44,1 (Wegstrecke
euklidisch 25,6 / 35,6).

### N2 — Nahkampf (Aufbau test_re2_zombie_abc, Messer, 64 Seeds x 900 Bilder)
Stand bis Runde 37 (Maske 0): 368 Treffer; Spur L: 369 (Sonde) bzw. 373 (test_re2_zombie_abc im Prozess nach der Pistole).
Pistole 1795 -> 1775 (90 statt 96 Tode). Die Trefferpruefung wird nicht schlechter (Gesamt-Treffer im Sweep).
⛔ Diese Metrik zaehlt Treffer ueber den ganzen Lauf, NICHT je Anmarsch. Je Anmarsch an der exe (Abnahme 1 Abschnitt 2,
18 Paare Spur L / Basis mit gleicher Eingabe ueber alle 15 Phasen des 0,5-s-Schlagtakts): Anmarsch 1 L 7 / Basis 4,
Anmarsch 2 L 31 / Basis 33, alle Platz-5-Treffer bis Bild 1400 43 / 41, Griffe 35 / 35 — im Mittel gleich viele, je
Schlagphase einer mehr oder weniger (die "6 statt 10" der Abnahme 0 waren EINE Schlagphase). Nach dem P3-Fix der
Nachbesserung 2 neu gemessen: Abschnitt "Nachbesserung 2", M2.

### N3 — ECHTE EXE (RE2-KI = Standard; Leon UND Elza; Spielstand per Kartenwerkzeug + CONTINUE + Eingabeskript, RE15_STATE_LOG)
Treiber `L_belege/exe_lauf.sh` (eigene Messkopie `re15_pc_r37l_mess_<lauf>.exe`, nur eigene PID beendet), Auswerter
`L_belege/ausw_state.py` (je Raumbesuch: Wechsel ss1 -> 2 mit +0x1F0, was danach kommt, Ausfall ss1=12 und Griff aus dem
Ausfall, Spieler-Griffe). Montage-Karte = `9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7 7:0xc6-0xca` (wie Spur H N1/N4).

| Lauf | Weg | Wechsel nach 0x201 (+0x1F0) | danach | Ausfall / Griff aus Ausfall (D12) | Spieler gegriffen |
|---|---|---|---|---|---|
| E1 Leon 1130 nach Irons' Tod, steht | Karte 1150 + Tuer | 1 (1991) | Griff | 0 / 0 | 1x (F888) |
| E2 Leon 1130, geht den Gang hinunter | dto. `R0.33,U25` | 3 (1945, 985, 1987) | 1 Griff, 2x zurueck in den Gang | 1 / 1 | 2x (F453) |
| E3 Leon 1130, RENNT den Gang hinunter | dto. `R0.33,UX25` | 4 (2332, 2374, 2482 bei rennendem Leon = Produzent (3), < 0x9C4; 596) | 1 Griff, 2 Ausfaelle | 2 / 1 | 3x (F325) |
| E4 Leon 1030 nach der Montage | Karte 1040 + Tuer | 3 (1962, 1221, 709) | 2 Griffe | 0 / 0 | 3x (F76) |
| E5 Leon 1040 nach der Montage | Karte 1040 | 2 (1987, 1999) | 2 Griffe | 0 / 0 | 2x (F224) |
| E6 Leon 1010 (Normalspiel, Eintritt aus 1020) | Karte 1010 | 5 (1995, 849, 951, 1978, 1991) | 3 Griffe | 0 / 0 | 3x (F93) |
| E7 Leon 1100 (aufrechte Form, Bit (4,0xE8)) | Karte 1100 | 2 (849, 1991) | 2 Griffe | 0 / 0 | 3x (F82) |
| E8 **Elza** 1181 | Elza-Karte (probe_r36_elza_karte) | 1 (1995) | Griff | 0 / 0 | 1x (F311) |
| E11 Leon 1130 mit MESSER an der Tuer (R1 + Schlaege) | Karte 1150 + Tuer | — | — | — | **3 Messertreffer** am Fetten (Typ 0x11, HP 250 -> 247 -> 244 -> 241, Reaktion 2/1) |
BEISSEN (Spieler-HP im selben Besuch): E1 100 -> 40 ab F909, E2 -> -20 ab F474, E3 -> -20 ab F346, E4 -> -20 ab F97, E5 -> 40
ab F245, E6 -> -20 ab F114, E7 -> -20 ab F103, E8 (Elza) -> 60 ab F332 — die Griffe enden im Biss wie in RE2.
Weg je Bild im Arme-hoch-Gang 25..58 (Typ/Bank), im Gang 20..31 (RE2-Original 44,1 / 32,2).
Bild (Framedump, exe, Fensterskalierung 1): `L_belege/exe_1130_arme_hoch_F864_F872_F880.png` — Chief-Office-Tuer, Bild 864
der fette Zombie im Gang (Arme unten), Bild 872/880 im Arme-hoch-Gang (Clip 6 = Loco 4 + walkclip 2) kurz vor dem Griff (F888).
(ROOM1011 Elza: im Kartenstand laeuft Roys Szene, keine Zombies — kein Messlauf.)

## Tests

Neu (re15_port/tests/unit/probes/r37_re2zombie.cmake, Quelle tests/unit/test_r37_re2zombie.c, tests/integration/test_r37_re2zombie.cmake):
| Test | misst | Ergebnis |
|---|---|---|
| unit_r37_re2zombie_prod1 | (1) je Wurf (48 Strom-Lagen): 0x201 genau bei (r1&3)==0 und +0x1F0 0x1387; Reseed (r0>>3)+100; 2 Wuerfe; Gegenprobe 0x1388 | 4/4 |
| unit_r37_re2zombie_prod2 | (2) +0x1F0 0x7CF geradeaus -> 0x201; Gegenproben hinten / 0x7D0 / beansprucht / Maske ohne (2) | 5/5 |
| unit_r37_re2zombie_prod3 | (3) rennend seitlich (im +-1324-, nicht im +-512-Kegel), 0x9C3 -> 0x201; Gegenproben gehen / 0x9C4 / hinten | 5/5 |
| unit_r37_re2zombie_b | Block 3 je Wurf (32 Lagen): Commit genau bei (rand&1)==1; ein Wurf; Gegenprobe Stand bis Runde 37 umgekehrt | 3/3 |
| unit_r37_re2zombie_c | Wurf-Takt: +0x6 = 2 bleibt, +0x158 10 -> 9, ein Wurf; P2 im naechsten Takt; Wecker schlaegt den Wurf; Gegenproben | 6/6 |
| unit_r37_re2zombie_d12 | DECISION[12]: Griff aus dem Stoss bei 0x513 / Bild 5; Gegenproben 0x514 / Bild 11 / hinten / Phase 1 / Stand bis Runde 37 | 6/6 |
| unit_r37_re2zombie_nahkampf | Messer-Sweep: > 250 und >= 95 % vom Stand bis Runde 37 (373 gegen 368) | 2/2 |
| unit_r37_re2zombie_raum1130 | Nutzer-Beispiel: Wechsel vor Leon (+0x1F0 1993), Griff aus 0x201; Gegenprobe Maske 0 keiner | 4/4 |
| unit_r37_re2zombie_raum1180 | Normalspiel: dasselbe (2 Wechsel, 2 Griffe) | 4/4 |
| integration_r37_re2zombie | echte exe, Karte 1150 + CONTINUE + Tuer -> 1130, Leon steht 1100 Bilder: Wechsel bei +0x1F0 1991 < 0x7D0, Griff aus 0x201 | gruen (52 s) |
Angepasst (mit Beleg, s. Schritt 5): unit_r36_zombieki_eintritt (Gegenlauf "vorbestehend", Taschenfall-Paar im alten Strom),
integration_r34_granaten (Fenster 220 -> 360), integration_r35_granate (EXIT_AT 260 -> 320, Leichen-Fenster X+80 -> X+180):
Zeile 9 waehlt per Wurf Bersten oder Zerreissen (`beq s0,zero` @0x80108d10, 1 von 4 Zerreissen); im Strom nach Spur L zerreisst
Gegner 3 (Leiche ab Bild 258 = X+139: P1 Clip 4, P5/P6 (rand&0x1f)+15 @0x80109214-1C, P3 Clip 1), vorher bersten beide (165).
Gemessen mit 360: "Gegner 2 Leiche ab Bild 165, Gegner 3 Leiche ab Bild 258" — kein Haenger.
Fortsetzung (5b): probe_r36_blut (Sonden-Maske 0, alle neun Varianten bitgleich zur Basis), probe_10d0_situp_re2
(Uebergabe-Tick +0x5 in {1,2}).

### Gezielte Laeufe der Fortsetzung (2026-10-13, Baum r37_re2zombie, Stand nach 4c9e56e9; keine volle Suite)
Bau: `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`.
1. Unit-Auswahl (alle Module/Raeume des Diffs + RE2-Strom-Nutzer), `-E "^integration_" -j 4`:
   `-R "re2|zomb|zgirl|granat|lyer|messer|crawl|kriech|gegnerzahl|1140|1130|1180|enemy|biss|griff|grab|bump|kfjump|teardeath|blut|bandlock|rise|r37_|irons|1150|dog|crow|spider|affen|situp|10d0"`
   -> **`100% tests passed, 0 tests failed out of 200`** (vorher, vor 5b: `98% tests passed, 3 tests failed out of 198` =
   blut_pin eben/tief/rip).
2. Gegenprobe ALLER uebrigen Nicht-Integrations-Tests (`-E "^integration_|<Ausdruck 1 ohne situp|10d0>" -j 4`, 424 Tests):
   `99% tests passed, 1 tests failed out of 424` — einziger roter = probe_10d0_situp_handoff (vor der Anpassung 5b;
   danach in Lauf 1 enthalten und gruen).
3. Integration (seriell), `-R "^integration_.*(<Ausdruck 1 ohne situp|10d0>)"`: **`100% tests passed, 0 tests failed out of 16`**
   (u.a. integration_r37_re2zombie 51,6 s, integration_r37_zombieaktiv, integration_r34_granaten, integration_r35_granate,
   integration_r36_gegnerzahl, integration_r35_zgirl/_5090, integration_r36_szene1150, integration_r35_cut1150).
4. Integration (seriell), alle uebrigen Skripte, die Zombies/Gegner/state.log anfassen (grep der tests/integration/*.cmake):
   `-R "^integration_.*(r34n_e_dokumente|r35_cut10f0|r35_inventar1050|r35_redhawk|r35_werfer|r36_dachkiste|r36_elza|r36_karte|r36_kleidung_spielweg|r36_pri_schatten|r36_schuss|r36_szenenleon)"`
   -> **`100% tests passed, 0 tests failed out of 20`**.

## Nachbesserung 1 (2026-10-14, nach Abnahme 0 `L_abnahme_0.md`)

Die Abnahme 0 hat alle vier Punkte als erfuellt bewertet und einen Mangel gemeldet (Abnahme-M1; nicht zu verwechseln mit
der Messung M1 oben = historischer Nahkampf-Zusammenbruch).

### Abnahme-M1 — Satz "Nahkampf/Messer: unveraendert wirksam (gemessen 368 -> 373 Treffer im Messer-Sweep)"

**Ursache.** Der Satz im Abschnitt "Fuer den Nutzer" stuetzte sich nur auf den Harness-Sweep N2 (Aufbau test_re2_zombie_abc:
64 Seeds x 900 Bilder, Spieler steht mit R1 2200..3400 vom Zombie-Schwerpunkt und schlaegt alle 7..17 Bilder). Diese Metrik
zaehlt Treffer ueber den ganzen Lauf und belegt nur, dass die TREFFERPRUEFUNG nicht schlechter wird. Was der Nutzer an der exe
erlebt, ist die Zahl der Schlaege je ANMARSCH, bevor ein Zombie greift. Die exe-Messung N3/E11 (3 Treffer) hatte kein
Basis-Paar mit gleicher Eingabe; der Vergleich fehlte, der Satz verallgemeinerte die Harness-Metrik.

**Messung vorher** (Abnahme 0, Abschnitt 3.6, selbst von der Abnahme gemessen; NICHT wiederholt): Leon steht mit Messer an
der Tuer 1130 (`RE15_EQUIP=1`), R1 + Schlag alle 0,5 s, vier Paare Spur L / Basis mit identischer Eingabe, erster anrueckender
Zombie Platz 5 (Typ 0x11, HP 250, 3 je Messertreffer):

| Skript-Versatz | Spur L: HP / Treffer vor dem Griff | Basis: HP / Treffer |
|---|---|---|
| W20 (k_L_20 / k_B_20) | 247 / 1 | 244 / 2 |
| W21.3 | 244 / 2 | 244 / 2 |
| W22 (a5 / b5) | 247 / 1 | 244 / 2 |
| W23.7 | 244 / 2 | 238 / 4 |
| Summe | **6** | **10** |

Bildfolge k_L_20 / k_B_20 (state.log der Abnahme): Spur L F1180 Arme hoch (+0x1F0 1995), F1186 Messertreffer (2/1), nach der
Erholung F1208 sofort wieder 1/2 (+0x1F0 1487 < 0x7D0, im Kegel), Griff F1214 bei 1173, der naechste Schlag (F1222) kommt zu
spaet. Basis: Erholung F1193 in den langsamen Gang 1/1, zweiter Treffer F1201 bei 1419, Griff erst F1224 bei 851.

**Beleg (Mechanismus, RE2 EMZ0.BIN) — ⛔ in Nachbesserung 2 KORRIGIERT (Abnahme 1, M1).** Die erste Fassung beschrieb
das Leiter-Ende falsch ("sonst 0x101 @0x80105B38/@0x80105B40"), nannte den Port-Weg "P0 nach P1" RE2 und folgerte "weniger
Schlaege". Richtig (vollstaendige Disassembly mit Bytes: Abschnitt "Nachbesserung 2", R5):
1. Treffer-Erholung, Haupt-Handler 0x80105438 P3 @0x80105A50: 0xB01 (`addiu v0,zero,2817` @0x80105ACC) nur bei Spieler vorn
   (+-512 @0x80105A7C-84), +0x1F0 < 0xFA0 (@0x80105A94), Ein-Angreifer-Riegel 0x800CFDCB & 0x80 frei (@0x80105AA0-B0) und
   (rand & 0xf) == 0 (@0x80105AB8-C4); 0xC01 (`addiu v0,zero,3073` @0x80105B08) bei Spieler ausserhalb +-1024 und rand & 1;
   0x201 (`addiu v0,zero,513` @0x80105B24) bei rand & 1; sonst das ENDE `lbu a0,536(s1)` / `lbu v1,332(s1)` /
   `addiu v0,zero,257` / `bne a0,v1,0x80105ba0` / `sw v0,4(s1)` (Delay-Slot) @0x80105B30-40: 0x101 bleibt NUR bei UNGLEICHEM
   Clip stehen. Bei GLEICHEM Clip (+0x218 == +0x14C, der Normalfall: P0 zwingt den Gang-Clip @0x801054C8-D4) schreibt das
   Original 0x10101 (`lui v0,0x1` / `ori v0,v0,0x101` / `sw` @0x80105B44-50) mit +0x16B = (r&0xf)<<1, +0x158 = 30,
   +0x16A = (r&0x1f)+30, +0x15A = r + 300 (@0x80105B54-80) und wendet den +0x144-Vektor noch einmal an
   (`jal 0x800152c8` @0x80105B98).
2. "Faellt die Leiter auf 0x101, laeuft im ersten Gang-Takt EXEC[1] P0 nach P1 durch" war der PORT-Weg (umgekehrte
   Bedingung in re2z_hit_main case 3, vorbestehend seit d39b815b), NICHT RE2. Im Original geht es mit 0x10101 direkt in
   EXEC[1] P1: kein Clip-Neustart auf ein Zufallsbild, +0x15A = 300..555 statt (rand>>3)+100 = 100..131. Produzent (2)
   (`sltiu v0,v0,0x7d0` @0x80101ECC, Kegel a3 = 512 @0x80101EEC, `sw 0x201` @0x80101F0C-10) laeuft in BEIDEN Wegen im
   ersten Gang-Takt (P1-Rumpf); in der Basis war (2) aus, der Zombie blieb im 0x101-Gang (k_B_20 F1193).
3. Der Arme-hoch-Gang schliesst schneller: RE2-Original Wegstrecke 35,6 gegen 25,6 je Takt (als Summe |dx|+|dz| 44,1 / 32,2;
   M2, `re2_0x201_ausw.py`). Der Griff hat in beiden Gaengen dieselbe Schwelle +0x1F0 < 0x4B0 (DECISION[1] `sltiu 0x4b0`
   @0x801018F4, DECISION[2] `sltiu 0x4b0` @0x80102114, Commit 0x301 @0x80102174-78 / @0x801021B8-BC). Daraus folgt NICHT
   "weniger Schlaege": die Trefferreaktion haelt den Zombie je Schlag an — an der exe ueber alle 15 Schlagphasen im Mittel
   gleich viele Treffer je Anmarsch (Abnahme 1, 18 Paare; M2).
Die Trefferpruefung des Messers aendert Spur L nicht. "byte-true RE2" gilt fuer die Produzenten (1)(2)(3), (b), (c) und
DECISION[12] (Abnahme 0/1) und seit Nachbesserung 2 fuer das Leiter-Ende P3 (Code-Fix, Freigabe 2026-10-16).

**Aenderung.** Nur Text, kein Code, keine Konstante:
* "Fuer den Nutzer", 4. Punkt: neu formuliert (Messer trifft wie vorher; je Anmarsch weniger Schlaege, an der exe 6 statt
  10 in vier Laeufen; RE2-Verhalten mit Beleg 44 / 32 je Takt).
* N2: Schlusssatz "Der Nahkampf wird nicht schlechter" auf "Die Trefferpruefung wird nicht schlechter (Gesamt-Treffer im
  Sweep)" eingeschraenkt, mit Verweis auf die exe-Messung je Anmarsch.
* OFFEN: beim Nachlesen der Erholungsleiter gefunden — der Ausfall-Zweig P3 prueft im Original den Ein-Angreifer-Riegel
  0x800CFDCB & 0x80 (@0x80105AA0-B0) vor dem Wurf, der Port nicht (vorbestehend, ausserhalb des Vertrags der Spur L, mit
  Adresse und Vorschlag eingetragen).

**Messung nachher.** Neuer Text gegen die Abnahme-Messung 3.6 geprueft (Summe 6 / 10, je Paar wie in der Tabelle oben).
Code unveraendert gegenueber dem abgenommenen Stand ab2bf2b9 (`git diff ab2bf2b9 -- re15_port` leer); Bau
`bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
Gezielte Tests (Baum r37_re2zombie, Stand fe329139 = Code ab2bf2b9; keine volle Suite):
1. `ctest --test-dir re15_port/build -R "r37_re2zombie|re2_zombie_abc|blut_pin|10d0_situp|zombieki_eintritt|r37_zombieaktiv" -E "^integration_" -j 4 --timeout 600`
   -> **`100% tests passed, 0 tests failed out of 24`** (alle Riegel der Spur L, Nahkampf-Positivkontrolle
   unit_re2_zombie_abc / unit_r37_re2zombie_nahkampf, die in 5 / 5b angepassten Riegel blut_pin x9, probe_10d0_situp_handoff,
   unit_r36_zombieki_eintritt, dazu die Riegel der Spur H r37_zombieaktiv).
2. `ctest --test-dir re15_port/build -R "^integration_r37_(re2zombie|zombieaktiv)$" --timeout 600` (seriell)
   -> **`100% tests passed, 0 tests failed out of 2`** (integration_r37_re2zombie 51,50 s, integration_r37_zombieaktiv 25,11 s).

## Nachbesserung 2 (2026-10-14 begonnen, 2026-10-16 fortgesetzt; nach Abnahme 1 `L_abnahme_1.md`)

### Grundlage der Fortsetzung
* Rest 7f581fde (UNGEPRUEFT) geprueft: nur Text — die Stand-Zeile ("reine Textkorrektur, kein Code"; durch die Freigabe
  ueberholt, ersetzt) und die Kennzeichnung der Weg-Zahlen als Summe |dx|+|dz| (M2-Tabelle, N1). Die dort genannte
  Wegstrecke 25,6 / 35,6 selbst aus den Rohdaten nachgerechnet: `re2_0x201_ausw.py` gibt jetzt beide Masse aus —
  "Weg je Takt s1=1: |dx|+|dz| 32.2, Wegstrecke 25.6 (4213 Takte)", "s1=2: |dx|+|dz| 44.1, Wegstrecke 35.6 (94 Takte)".
* Merge 44721dca (master v0.8.25 + Runde-38-Auftrag): `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`,
  `build` -> `=== LOCAL-BUILD-OK (build)` (851 Schritte). Gezielte Unit-Auswahl VOR dem Fix (Ausdruck wie Abnahme 1, s. Tests):
  `100% tests passed, 0 tests failed out of 287`.
* Freigabe (AUFTRAG Runde 38, "gehe die 2 Punkte an"; VERTRAG Runde 38 Fortsetzung L): die Inversion in re2z_hit_main
  case 3 und der fehlende Ein-Angreifer-Riegel @0x80105AA0-B0 werden KORRIGIERT. Runde 38 Spur U (wasserzombie) haengt
  parallel einen Haken in dieselbe Datei — geaendert sind nur case 3, ein neuer Helfer neben den Runde-37-Helfern und die
  Sonden-Zaehler.

### M1 — Erholungsleiter P3 (Text UND Code)

**Ursache.** (a) Text: der Beleg der Nachbesserung 1 las nur bis zum `sw` im Delay-Slot @0x80105B40 und uebersah den
`bne a0,v1` @0x80105B3C davor. (b) Code (vorbestehend seit d39b815b): re2z_hit_main case 3 hatte die Bedingung des Leiter-
Endes umgekehrt (`motion != walkclip` -> 0x10101) und keinen Ein-Angreifer-Riegel vor dem 0xB01-Wurf; der Port fuhr deshalb
im Normalfall 0x101 -> EXEC[1] P0 (Clip-Neustart auf Zufallsbild, 5 Wuerfe, +0x15A = (rand>>3)+100 = 100..131).

**Messung vorher.** Abnahme 1 Abschnitt 4 (an beiden exe, nicht wiederholt): `L8_messer_W21.3` F1228 `st=2 ss1=1 ss2=3 mo=2`
-> F1229 `st=1 ss1=1 ss2=0` (0x101); `B7_messer_W20` F1234 -> F1235 `ss2=0` -> F1236 `ss2=1 af 22 -> 18` (P0-Zufallsbild).
Eigene Kalibrierung des Messer-Aufbaus am gemergten Stand VOR dem Fix (`L_belege/messer_laeufe.sh` / `messer_ausw.py`,
Zaehlregel der Abnahme 1): w = 20.000 -> "anm1 0 anm2 2 alle 2 griffe 2 | Treffer F1183,F1213 | Griffe F888,F1236" =
Abnahme 1 (L 0 / 2, F888, F1236) bitgleich.

**R5 — Beleg (EMZ0.BIN, `re2_disasm.py --bin EMZ0.BIN dis 0x80105A50 90`, selbst disassembliert; EMOVL10_S0.BIN und
EMZ0_d1.BIN im Bereich 0x80105A50-BA0 bytegleich, geprueft):**
```
80105a7c jal 0x80015614 / 80105a80 addiu a3,zero,512 / 80105a84 bne v0,zero,0x80105ad8     ; Spieler vorn +-512
80105a8c lw v0,496(s1) / 80105a94 sltiu v0,v0,0xfa0 / 80105a98 beq v0,zero,0x80105ad8      ; +0x1F0 < 4000
80105aa0 lui v0,0x800d / 80105aa4 lbu v0,-565(v0) / 80105aac andi v0,v0,0x80 / 80105ab0 bne v0,zero,0x80105ad8
                                                                                          ; 0x800CFDCB & 0x80 frei
80105ab8 jal 0x80015fe8 / 80105ac0 andi v0,v0,0xf / 80105ac4 bne v0,zero,0x80105ad8
80105acc addiu v0,zero,2817 / 80105ad0 j 0x80105ba0 / 80105ad4 sw v0,4(s1)                 ; 0xB01
80105ae8 jal 0x80015614 / 80105aec addiu a3,zero,1024 / 80105af0 beq v0,zero,0x80105b14     ; ausserhalb +-1024
80105af8 jal 0x80015fe8 / 80105b00 andi v0,v0,0x1 / 80105b04 beq v0,zero,0x80105b14
80105b08 addiu v0,zero,3073 / 80105b0c j 0x80105ba0 / 80105b10 sw v0,4(s1)                 ; 0xC01
80105b14 jal 0x80015fe8 / 80105b1c andi v0,v0,0x1 / 80105b20 beq v0,zero,0x80105b30
80105b24 addiu v0,zero,513 / 80105b28 j 0x80105ba0 / 80105b2c sw v0,4(s1)                  ; 0x201
80105b30 lbu a0,536(s1) / 80105b34 lbu v1,332(s1) / 80105b38 addiu v0,zero,257
80105b3c bne a0,v1,0x80105ba0 / 80105b40 sw v0,4(s1)                                       ; UNGLEICH: 0x101, Epilog
80105b44 lui v0,0x1 / 80105b48 ori v0,v0,0x101 / 80105b4c jal 0x80015fe8 / 80105b50 sw v0,4(s1)   ; GLEICH: 0x10101
80105b54 andi v0,v0,0xf / 80105b58 sll v0,v0,1 / 80105b5c sb v0,363(s1)                    ; +0x16B = (r1&0xf)<<1
80105b60 addiu v0,zero,30 / 80105b64 jal 0x80015fe8 / 80105b68 sh v0,344(s1)                ; +0x158 = 30
80105b6c andi v0,v0,0x1f / 80105b70 addiu v0,v0,30 / 80105b74 jal 0x80015fe8 / 80105b78 sb v0,362(s1) ; +0x16A
80105b7c addiu v0,v0,300 / 80105b80 sh v0,346(s1)                                          ; +0x15A = r3 + 300
80105b84 lbu v1,536(s1) / 80105b88 lbu v0,332(s1) / 80105b90 bne v1,v0,0x80105ba0
80105b98 jal 0x800152c8 / 80105b9c addu a1,zero,zero                                       ; Vektor +0x144 noch einmal
Rohbytes 80105aa0: 0d 80 02 3c cb fd 42 90 00 00 00 00 80 00 42 30 09 00 40 14 ...
         80105b30: 18 02 24 92 4c 01 23 92 01 01 02 24 18 00 83 14 04 00 22 ae 01 00 02 3c 01 01 42 34 fa 57 00 0c
         80105b50: 04 00 22 ae 0f 00 42 30 40 10 02 00 6b 01 22 a2 1e 00 02 24 fa 57 00 0c 58 01 22 a6 1f 00 42 30
         80105b70: 1e 00 42 24 fa 57 00 0c 6a 01 22 a2 2c 01 42 24 5a 01 22 a6 18 02 23 92 4c 01 22 92 00 00 00 00
         80105b90: 03 00 62 14 21 20 20 02 b2 54 00 0c 21 28 00 00
```
* RE2-rand @0x80015FE8 liefert 0..255 (`andi v0,v0,0xff` @0x80016014) -> +0x15A = 300..555.
* Normalfall GLEICH: im ganzen Handler (0x80105438-BB8) schreibt nur P0 +0x14C (`sw v0,332(s1)` @0x801054D4, nach
  `beq v1,v0` @0x801054C8 = Gang-Clip erzwingen); Scan aller `332(`/`536(`-Zugriffe des Handlers selbst gemacht.
* 0x800152C8 (RE2 PSX.EXE, selbst disassembliert): `lhu v0,118(s1)` / `addu v0,v0,a1` (+0x76 + a1) @0x800152E4-F0,
  RotMatrixY `jal 0x8008e1f4` @0x800152F8, ApplyMatrixSV auf &self+0x144 (`addiu a1,s1,324` / `jal 0x8008dba4`
  @0x80015304-08), +0x38 += out.x @0x80015314-20, +0x40 += out.z @0x80015324-34. Der Vektor ist der zuletzt von e7c
  abgelegte (+0x144 `sh v1,324(t0)` @0x80015FD8, +0x148 `sh a1,328(t0)` @0x80015FE4); P3 ruft KEIN e7c (Scan: e7c nur in
  P0 @0x80105590/A8, P1 @0x8010575C, P2 @0x801058AC) -> es ist der Vektor des letzten P2-Takts.
* Port-Zwilling des Riegels: re15_player_is_grabbed() — derselbe wie Produzent (2) @0x80101EFC-08, DECISION[12]
  @0x80104648-58 und re15_re2z_fill_gates (`g->pl_1d3`, "The port's equivalent latch is s_player_grabbed").

**Aenderung** (Kommentar "Runde 37 Spur L, Nachbesserung 2" + @0x je Zeile):
* `engine/src/enemy_ai_re2_zombie.c` re2z_hit_main case 3: Riegel vor dem 0xB01-Wurf (@0x80105AA0-B0); Leiter-Ende
  GLEICH -> 0x10101 + drei Wuerfe (+0x16B, +0x158 = 30, +0x16A, +0x15A = r + 300) + 0x800152C8 (@0x80105B44-98), UNGLEICH
  -> 0x101 (@0x80105B38-40). Neuer Helfer `r2z_r37_152c8` (barer 0x800152C8(self,0), Drehung wie re15_clip_root_motion_delta).
* Ein Feld im Original, zwei im Port: +0x158 = 30 (@0x80105B68) ist im Gang der ZEILEN-TIMER — EXEC[1] mit +0x6 == 1
  springt an P0 vorbei (`beq v1,v0,0x80101b24` @0x80101A64; P0 = Gang-Seeds @0x80101A74-B20) und P1 liest `lhu v1,344(s1)`
  @0x80101B24 (Dekrement/Zeilenwechsel @0x80101B2C-90). Der Port fuehrt diesen Timer als re2z_gaittmr und erkennt "Gang
  betreten" am Tag re2z_gaitinit (re2z_gait_tick: neues Tag -> Gang-Init mit 2 Wuerfen). P3 setzt deshalb im
  Gleichheitsfall zusaetzlich re2z_gaittmr = 30 und das Tag 0x80 | 1 (die Gang-Seeds SIND gesetzt) — sonst lief im ersten
  P1-Takt eine zweite Gang-Init, die das Original nicht hat. re2z_t158 = 30 bleibt (dasselbe Original-Feld; jeder andere
  Executor saet +0x158 in seinem P0 neu).
* `include/re15_actor.h`: neues Feld `re2z_root148` (+0x148, eine Zeile + Kommentar) neben re2z_root144;
  `engine/src/enemy_ai_common.c` re15_clip_root_motion_delta: +0x148 wie +0x144 ablegen (2 Zeilen, @0x80015FE4).
* `include/re15_re2zombie_r37.h`: Sonden-Bit `RE15_R37L_P3` (0x40, RE15_R37L_ALLES = 0x7F; Maske 0 bleibt der Stand bis
  Runde 37 BITGLEICH, also auch die alte umgekehrte Bedingung — fuer probe_r36_blut / unit_r36_zombieki_eintritt) und
  Zaehler der Leiter-Ausgaenge `re15_re2z_r37_p3_ausgang(k)` (nur Sonden).
* Test `unit_r37_re2zombie_p3` (probes/r37_re2zombie.cmake, test_r37_re2zombie.c `pin_p3`).

**Messung nachher (Riegel, Strom-Lage je Wurf vorhergesehen mit re15_re2z_rng_peek):** `test_r37_re2zombie p3` (zweiter
Bau-Ordner build_l2 mit dem Fix) -> `test_r37_re2zombie p3: 8/8`:
* gleicher Clip, Spieler 6000 vorn (kein 0xB01-/0xC01-Wurf): 16 x 0x10101 mit +0x16B = (r1&0xf)<<1, +0x158 = 30,
  +0x16A = (r2&0x1f)+30, +0x15A = r3+300 und genau 4 Wuerfen; 16 x 0x201 bei r0&1 mit 1 Wurf;
* 0x800152C8 @0x80105B98: der Vektor (37,-5) wird bei Gier 1024 einmal gedreht angewandt -> Lage (-5,-37); beim
  0x201-Ausgang keine Bewegung;
* Folgetakt nach 0x10101: EXEC[1] P1 ohne P0 — Bild bleibt, Gang-Zeilen-Timer 30 -> 29 mit der P3-Zeile, +0x15A - 1;
* Gegenprobe anderer Clip: 0x101, 1 Wurf, keine Bewegung, +0x15A unberuehrt (16 Lagen);
* Riegel: Spieler frei 3000 vorn -> 0xB01 (EXEC[11], Sturz nach vorn mit Clip 0x0A) genau bei r0&0xf == 0 (8 Lagen); Spieler
  beansprucht -> nie 0xB01, kein 0xB01-Wurf (45 Lagen);
* Gegenprobe Stand bis Runde 37 (Bit P3 aus): gleicher Clip -> 0x101 und im naechsten Takt EXEC[1] P0 (+0x15A 99..130),
  beansprucht -> 0xB01 moeglich (16 / 8 Lagen).

**Messung nachher (Harness, Aufbau test_re2_zombie_abc: ROOM1140, 64 Seeds x 900 Bilder; `test_r37_re2zombie mess <maske> 64
<waffe>`; Maske 0x3F = Spur L ohne den P3-Fix = Stand vor Nachbesserung 2, reproduziert die frueheren Summen 369 / 1775):**

| Sweep | vorher (0x3F) | nachher (0x7F) |
|---|---|---|
| Messer: Treffer / Tode / Griff-Bilder | 369 / 2 / 27473 | 362 / 2 / 27609 |
| Messer: P3-Ausgaenge 0xB01 / 0xC01 / 0x201 / 0x10101 / 0x101 | 5 / 1 / 47 / **0** / **61** | 2 / 1 / 57 / **54** / **0** |
| Messer: Wuerfe | 15305 | 15325 |
| Pistole: Treffer / Tode / Griff-Bilder | 1775 / 90 / 17472 | 1826 / 90 / 16520 |
| Pistole: P3-Ausgaenge | 7 / 3 / 86 / **0** / **82** | 1 / 2 / 96 / **80** / **0** |
| Pistole: Wuerfe | 14118 | 14280 |

Vorher endete JEDE Leiter, die nicht in 0xB01/0xC01/0x201 ging, in 0x101 (die umgekehrte Bedingung trifft im Normalfall nie
den 0x10101-Zweig), nachher in 0x10101 — wie das Original. Messer-Positivkontrolle 362 (Schranke > 250; >= 95 % von 368).

**Zufallsstrom und verschobene Riegel.** Je Erholung, die nicht in 0xB01/0xC01/0x201 endet, zieht das Original 3 Wuerfe in P3
(@0x80105B4C/64/74) und faehrt danach EXEC[1] P1; der Port zog bisher 0 in P3 und 5 im EXEC[1]-P0 des Folgetakts (Gang-Seeds
@0x80101A88/98/C4, Startbild @0x80101AEC, +0x15A @0x80101B10) — also 2 Wuerfe weniger je Erholung, dazu kein Clip-Neustart,
+0x15A 300..555 statt 100..131 (der Timer-Produzent (1) kommt nach Treffern spaeter) und der eine 0x800152C8-Schritt. Wo der
Spieler beansprucht ist, entfaellt zusaetzlich der 0xB01-Wurf. Gleiche -R-Auswahl wie Abnahme 1 (s. Tests), Baum vor dem Fix
(build, Merge-Stand) gegen den Fix (build_l2), `ctest -V`, Vergleich `L_belege/riegel_vergleich.py`: 287 gemeinsame Tests,
282 mit bitgleicher Ausgabe, alle gruen; geaendert:

| Riegel | alt | neu | Begruendung |
|---|---|---|---|
| unit_re2_zombie_abc | Pistole 1775 Treffer / 90 Tode / DOWN 94..323 ueber 254 Fenster; Messer 373 | Pistole 1826 / 90 / 96..322 ueber 250; Messer 361 | Strom + Erholung 0x10101 (Harness-Tabelle oben); Schranken > 500 / > 250 gruen |
| unit_r37_re2zombie_nahkampf | 373 gegen 368 | 361 gegen 368 (>= 95 %) | dasselbe; gruen |
| unit_r36_zombieki_szenen | schuss3 erneuter Angriff F145, weg 912; schuss6 F319, weg 817 | F148, weg 804; F321, weg 807 | Zeitlinie `test_r36_zombieki_szenen schuss3` beider Baeume: die erste Erholung (Haupt-Handler P3) endet alt in F66 `st=1/1/0` (0x101, Folgetakt EXEC[1] P0), neu `st=1/1/1` (0x10101; Lage 1185 statt 1162 = 0x800152C8-Schritt + Clip laeuft weiter); danach anderer Strom: die zweite Erholung (Stagger-Handler P2 @0x80106010, NICHT case 3; Clip 4 = Stagger-Clip 0x00030004 @0x80105C18-38) wuerfelt alt 0x201, neu 0x101; Griff 3 Bilder spaeter. Riegel prueft "erneuter Angriff" (gruen) |
| unit_re2_weapon_rows | Diagnose-Clips w=3 0->0, w=4 0->4, w=7 0->0, w=12 0->0, w=20 0->1 | 0->4, 0->0, 0->1, 0->4, 0->0 | die Waffen laufen nacheinander im SELBEN Strom; ab der ersten Erholung (w=0..2) verschoben. Clip-Indizes sind laut Testkopf Diagnose und bewusst nicht gepinnt; Zeilen/Handler unveraendert, gruen |
| unit_re2z_bandlock_pin | Liege-Bit in 606 aufrechten Bildern (0,97 %, 5/64 Seeds) | 331 (0,54 %, 3/64) | Strom; gruen |
| unit_r37_re2zombie_p3 | — | neu, 8/8 | Riegel dieses Fixes |

**Messung nachher an der EXE (M1 + M2): Messer-Paarlaeufe wie Abnahme 1 Abschnitt 2** (`L_belege/messer_laeufe.sh`,
Zaehlregel `messer_ausw.py`, Erholungs-Ausgaenge `erholung_ausw.py`; Montage-Karte, echte Tuer 1150 -> 1130, RE15_EQUIP=1,
Skript `W1,A0.2,W2,W<w>` + 50x `M0.4,MA0.1`, RE15_EXIT_AT=1400#1130; "vor" = exe des Merge-Stands VOR dem Fix, "nach" =
exe mit dem Fix; Basis = Spalte der Abnahme 1, nicht wiederholt):

| w | vor: Anm. 1 / Anm. 2 (Griffe) | nach: Anm. 1 / Anm. 2 (Griffe) | P3-Ausgaenge vor -> nach |
|---|---|---|---|
| 20.000 | 0 / 2 (F888, F1236) | 0 / 2 (F888, F1236) | 2x 0x201 -> gleich |
| 20.033 / .067 / .100 / .133 | je 0 / 1 (F888, F1213..1214) | je 0 / 1 (gleich) | je 1x 0x201 -> gleich |
| 20.167 | 0 / 4+ (F888, kein 2. Griff) | **0 / 2 (F888, F1216)** | 0x201, 0x101, 0xB01 -> 0x201, **0x10101** |
| 20.200 / .233 / .267 | je 0 / 3 (F888, F1272..1274) | je 0 / 3 (gleich) | 0x101 + 2x 0x201 -> **3x 0x10101** |
| 20.300 | 1 / 2 (F904, F1230) | 1 / 2 (gleich) | 0x101 + 2x 0x201 -> 0x10101 + 2x 0x201 |
| 20.333 / .367 / .400 | je 1 / 1 | je 1 / 1 (gleich) | 0x201 + 0xB01 -> gleich |
| 20.433 / .467 | je 1 / 2 | je 1 / 2 (gleich) | 3x 0x201 -> gleich |
| 21.3 | 1 / 2 (F904, F1230) | 1 / 2 (gleich) | 0x101 + 2x 0x201 -> 0x10101 + 2x 0x201 |
| 22.2 / 23.7 | je 0 / 3 (F888, F1272) | je 0 / 3 (gleich) | 0x101 + 2x 0x201 -> 3x 0x10101 |
| **Summe** | Anm. 1 **7**; Anm. 2 (16 Paare wie Abnahme 1) **31**; alle Treffer bis F1400 **43**; Griffe **35** | Anm. 1 **7**; Anm. 2 **31**; alle **41**; Griffe **36** | 30x 0x201 / 8x 0x101 / 4x 0xB01 -> 20x 0x201 / **18x 0x10101** / 3x 0xB01 |

* Kalibrierung: "vor" ist in ALLEN 18 Laeufen zeichengleich zur Spalte "Spur L" der Abnahme 1 (7 / 31 / 43 / 35) — Aufbau und
  Zaehler stimmen, der Merge von master hat diesen Ablauf nicht veraendert. Basis (Abnahme 1): 4 / 33 / 41 / 35.
* Wirkung des Fixes an der exe: jede Erholung, die das Leiter-Ende erreicht, geht jetzt 0x10101 (vorher 0x101 -> EXEC[1]-P0
  mit Zufalls-Startbild); weil Leon unter 2000 im Kegel steht, schaltet Produzent (2) im ersten Gang-Takt ohnehin in den
  Arme-hoch-Gang — deshalb bleiben Treffer- und Griffbilder in 17 von 18 Laeufen gleich. Einzige Aenderung: w = 20.167 —
  vor dem Fix fuehrte der andere Strom nach der 0x101-Erholung (F1211) zu einem Ausfall 0xB01 und KEINEM zweiten Griff bis
  F1400 (4+ Treffer), nach dem Fix greift der Zombie nach dem zweiten Treffer (F1216, 2 Treffer).
* Nahkampf nicht schlechter: je Anmarsch gleich (7 / 31 wie vor dem Fix), alle Treffer 41 = Basis.

### M2 — Messer-Aussage (Text)

**Ursache.** Die Aussage "je Anmarsch WENIGER Schlaege, 6 statt 10" (Fuer den Nutzer Punkt 4, N2-Zusatz, Commit d3f7417e)
verallgemeinerte die vier Laeufe der Abnahme 0 (EINE Schlagphase des 0,5-s-Takts); dazu war "44 statt 32 Einheiten je Takt"
die Summe |dx|+|dz|, keine Wegstrecke.

**Messung.** Abnahme 1 (18 Paare ueber alle 15 Schlagphasen, nicht wiederholt): Anm. 1 L 7 / Basis 4, Anm. 2 L 31 / Basis 33,
alle 43 / 41, Griffe 35 / 35. Nach dem M1-Code-Fix neu gemessen (Tabelle oben): 7 / 31 / 41 / 36. Wegstrecke je Takt aus den
RE2-Rohdaten nachgerechnet: 35,6 (0x201) / 25,6 (0x101), als |dx|+|dz| 44,1 / 32,2.

**Aenderung (Text).** "Fuer den Nutzer" Punkt 4 auf das phasenrobuste Ergebnis umgestellt (im Mittel gleich viele Treffer
je Anmarsch, je Schlagtakt einer mehr oder weniger; Arme-hoch-Gang schneller, die Trefferreaktion haelt je Schlag an),
"weniger Schlaege" gestrichen; N2-Zusatz und Nachbesserung-1-Beleg Punkt 3 ebenso; Weg-Zahlen ueberall als Wegstrecke mit
|dx|+|dz| in Klammern (M2-Tabelle, N1, R1-Kommentar in enemy_ai_re2_zombie.c). Die Commit-Nachricht d3f7417e bleibt als
Verlauf stehen; dieser Abschnitt ersetzt ihre Aussage "weniger Schlaege".

## OFFEN

* **Navigator-Tasche ROOM1030 (vorbestehend, nicht durch L):** Zombie Slot 1 (Typ 0x16, grid 0x0D) steht im "hin"-Zensus
  (Leon geht von der Tuer aus 1050 auf ihn zu und bleibt an der Tresen-/Wandzelle haengen) an der Wand (-6230,-24838); das
  Steuerziel des Navigators (-4984,-23860) liegt IN der soliden Zelle (Raster `test_r36_zombieki_eintritt raster 1030 1 125`:
  solide z -24734..-22484, x -8117..-3600). Basis-Build: 870 Bilder Stillstand an derselben Stelle. Der Navigator
  (re15_nav_update_steer = FUN_80039e7c-Zwilling, nav_zone_common.c) ist nicht Teil der Spur L; Vorschlag fuer eine eigene Spur:
  Zonen-/Verbindungspunkt von ROOM1030 gegen das Original pruefen (RE1.5 STAGE1/ROOM1030.RDT BLK-Daten, FUN_80039e7c).
* Kriecher mit +0x5 > 2 nach dem Griff-Wurf `sw 0x501` @0x80102D24-2C (im Original unerreichbar, Vorbedingung nicht gefunden) —
  betrifft nicht die Annaeherung/den Griff (Zensus).
* EXEC[11] gerade Takte `+0x144 >> 2` @0x801044D0-E0 (Ausrollen beim Zusammenbruch) — keine Aktivitaet (Zensus).
* Der Nahkampf-Zusammenbruch ist aufgeklaert (M1), nichts offen.
* **Blut-Anker im Aufsteh-Treffer (Spur C/G, vorbestehend, durch L nur haeufiger getroffen):** der Port rechnet den Anker
  im Handler aus der Pose des LAUFENDEN Bilds (re15_schuss_r36.h, Port-Konvention); P0 @0x80107AEC setzt danach das
  Blend-Byte `sb 15,334` @0x80107B3C, die gezeichnete Pose friert auf dem Vorbild ein. Das Blut liegt dann um den Kopfweg
  eines Bilds neben dem gezeichneten Kopf (gemessen 46 / 67 im Aufsteh-Clip 9). Original: Anker = Record+72 des Vorbilds
  (FUN_80027434 @0x8002751C-DC) = genau die eingefrorene Pose. Vorschlag fuer Spur C/G: Anker im Aufsteh-Treffer aus der
  Pose des Vorbilds.
* **Wurzel-Re-Anker beim Clipwechsel (enemy_ai_common.c re15_clip_root_motion_delta, fr_prev < 0 -> Delta 0):** das
  Original kennt keinen Clipwechsel-Sonderfall, e7c setzt im Bild 0 die Momentaufnahme auf 0 (@0x80015F04-1C), das erste
  Bild eines Clips faehrt also root(kf0) (RIP-Clip 4: (-277,-26)). Der Port faehrt das nur, wenn die Motion-Nummer gleich
  bleibt (z.B. Tod aus dem Arme-hoch-Gang, Loco-Clip 4 -> Haupt-Clip 4); bei anderer Nummer Delta 0. Vorbestehend
  (Runde 12), nicht Teil der Spur L; betrifft jeden Clip mit root(kf0) != 0.
* ~~Treffer-Erholung P3: Ausfall-Zweig ohne Ein-Angreifer-Riegel (@0x80105AA0-B0) und umgekehrtes Leiter-Ende
  (@0x80105B3C)~~ — GESCHLOSSEN in Nachbesserung 2 (Freigabe 2026-10-16), Abschnitt "Nachbesserung 2" M1.
* **Treffer-Handler 0x80105438, Phasen P0-P2 (beim Disassemblieren der Nachbesserung 2 gefunden; vorbestehend, NICHT
  freigegeben — die Freigabe 2026-10-16 umfasst nur case 3):**
  1. P0 faellt im Original im SELBEN Takt in P1 durch: P0 endet mit der Steuerung `jal 0x80015558` (a3 = 16) @0x8010571C-20
     und dem Zeile-14-Funken @0x80105724-38, die naechste Instruktion @0x8010573C ist der P1-Einstieg (Dispatch
     `beq v1,a0,0x8010573c` @0x80105480) — kein Sprung dazwischen. Port re2z_hit_main case 0 endet mit `break`: die
     Reaktion dauert im Port einen Takt laenger (Original P0+P1, 3x P1, 17x P2, P3 = 22 Takte; Port 23). Wirkt auf die
     Aktivitaet: gering (ein Takt je Treffer).
  2. P1 halbiert den Wurzel-Vektor vor der Anwendung: `lhu v0,324(s1)` @0x80105770 / `lhu v1,328(s1)` @0x80105778 /
     `sll 16` + `sra 17` @0x8010577C-88 / `sh v0,324(s1)` @0x8010578C / `sh v1,328(s1)` @0x80105794 (danach 0x800152C8
     @0x80105B98). Port re2z_hit_move wendet in P1 den vollen Gang-Weg an. Wirkt: ja, gering (Weg in 4 Takten je Treffer).
  3. Der P2-Schwanz (im Code als OPEN gefuehrt): in JEDEM P2-Takt Griff 0x301 (`sw` @0x80105A10 / @0x80105A4C) bei
     +0x1F0 < 0x514 (@0x80105980), 0x800CFDCB & 0x80 frei (@0x8010598C-A0), gleicher Etage (+0x106 gegen 0x800CFCFE
     @0x801059A8-B8), `*(*(u32*)0x800CFE18) & 1 == 0` (@0x801059C0-D8; 0x800CFE18 = Spieler+0x220, ein Zeiger — Bedeutung
     nicht ermittelt) und Sektor 0x80015758(self+0x38, Spieler+0x38, +0x76 +-256, 288) (@0x80105A00 / @0x80105A38; a1 =
     0x800CFDCB - 411 = 0x800CFC30), je gesperrt durch +0x21A & 0x20 / 0x40. Wirkt auf die Aktivitaet: JA — in RE2 kann ein
     Zombie, der aus unter 1300 getroffen wird, WAEHREND der Trefferreaktion zupacken; im Port nicht (der Nahkampf ist im
     Port dadurch leichter als in RE2). Offen fuer die Umsetzung: der Port-Zwilling von Spieler+0x220 / Bit 0.
  4. Bildvorschub: der Handler rueckt den Clip nur in P1 (`jal 0x8002959c` @0x80105790) und P2 (@0x801058C0) vor, NICHT in
     P0 und P3; der Port-Advancer re15_actors_anim_advance (player_common.c) rueckt jeden RE2-Zombie in jedem Takt vor
     (+1 Bild in P0, +1 in P3). Wirkt: nein (Bild/Weg um ein Bild verschoben).
  Vorschlag: eigener Punkt "RE2-Trefferreaktion Haupt-Handler P0-P2" mit Strom-Riegel-Liste (jeder der vier verschiebt den
  gemeinsamen Zufallsstrom bzw. den Weg in jedem Messer-/Pistolen-Lauf).

* **Werkzeug-Befund (nicht Spur L, fuer den Orchestrator):** `local_build.sh build` beendet vor dem Link alle
  `re15_pc`-Prozesse, deren Pfad mit dem Bauordner BEGINNT (`$_.Path.StartsWith('$_bw', ...)`, local_build.sh Z.326) —
  ohne Pfadtrenner. Ein Bau in `re15_port/build` beendet damit auch die exe in `re15_port/build_l2` bzw. im empfohlenen
  `re15_port/build_bisect`. Gemessen 2026-10-16: Bau-Log angelegt 16:56:46,44, die exe-Lauf-Ausgabe von
  integration_r34_granaten (build_l2, `g9_re2/debug.log`) endet 16:56:46,83 bei F231 mit `exit=-1` (= Stop-Process -Force);
  einzeln wiederholt gruen (s. Nachbesserung 2, Integration). Vorschlag: `StartsWith('$_bw\')` (mit Pfadtrenner).

## Fuer den Nutzer

* **Was jetzt anders ist (RE2-KI, Standard; Leon und Elza; jeder Raum):** wie in RE2 heben gehende Zombies die Arme und kommen
  schneller auf Leon zu, sobald er knapp vor ihnen steht (unter ~2 m im Blickfeld), gelegentlich auch schon auf ~5 m (Zufall
  1 zu 4 alle 3-4 Sekunden), und wenn Leon an ihnen VORBEIRENNT (unter ~2,5 m, im weiten Blickfeld). Aus diesem Gang greifen sie
  zu. Im RE2-Original nachgemessen (Leon steht im Gang ROOM20A0): Arme hoch bei 2,0 m bzw. 3,6 m, Griff bei 1,2 m.
* Ein Zombie, der aus dem Stand oder im Gang nach Leon schnappt (Ausfall), kann ihn jetzt dabei auch packen — wie in RE2.
* Stehende Zombies: zwei kleine Korrekturen (welcher Zufallswurf den Ausfall ausloest, und dass ein Zombie, der gerade zu
  wandern beginnt, im selben Moment noch aufwachen kann) — kaum sichtbar.
* Nahkampf/Messer: das Messer trifft wie vorher. An der exe gemessen (Leon steht mit dem Messer an der Tuer im Gang 1130,
  ein Schlag alle 0,5 s, 18 Laeufe ueber alle Schlagphasen): je Anmarsch im Mittel GLEICH viele Treffer wie vor dieser
  Aenderung (erster Anmarsch 7, zweiter 31; vor Spur L 4 / 33), je nach Schlagtakt einer mehr oder weniger. Wie in RE2 kommen
  die Zombies mit erhobenen Armen schneller heran (RE2-Original: 35,6 statt 25,6 Einheiten Weg je Takt), dafuer haelt sie jeder
  Treffer kurz an. Der alte Grund, warum diese RE2-Regeln seit August aus waren ("Messer trifft nicht mehr"), war ein Fehler im
  Messaufbau, nicht im Spiel.
* Nach einem Treffer (Nachbesserung 2): der Zombie setzt seinen Gang wie in RE2 fort, statt die Gehanimation neu zu starten,
  und die Gelegenheit, schon auf einige Meter von selbst die Arme zu heben (1 zu 4), kommt nach einem Treffer erst nach 10-18
  statt nach 3-4 Sekunden (steht Leon knapp vor ihm, sofort, wie vorher). Ist Leon schon von einem anderen Zombie gepackt,
  laesst sich ein getroffener Zombie nicht mehr nach vorn zu Boden fallen (wie in RE2).
* Keine neuen Assets (Paket-Gate unveraendert), keine Sprachdateien, keine Bank-9-Bits, keine Text-Ids. Neue Datei nur
  `re15_port/include/re15_re2zombie_r37.h` (Header, keine neue .c — Android-GLOB unberuehrt); +9 Unit-Tests, +1 Integration
  (RE15_MIN_TESTS +10).
* Entscheidungsfrage: keine.
* Fuer die Integration: Riegel-Liste = Schritt 5 (Tabelle + Granaten-Riegel unter Tests) + 5b, je mit alt/neu. DECISION[12] (re2z_decide_lunge,
  `case 12` in re2z_active) ist ein neuer Griff-EINSTIEG; der Griff-/Biss-Code selbst (Spur J) ist unberuehrt. Geaenderte
  fremde Testdateien: test_r36_zombieki_eintritt.c, test_r34_granaten.cmake, test_r35_granate.cmake, probe_r36_blut.c,
  probe_10d0_situp_re2.c (je mit Beleg im Dossier).
