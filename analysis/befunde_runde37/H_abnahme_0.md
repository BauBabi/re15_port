# Runde 37 Spur H "zombieaktiv" — Abnahme 0 (unabhaengig)

Datum 2026-10-12. Baum `.claude/worktrees/r37_zombieaktiv`, Zweig `r37/zombieaktiv`, HEAD 6de90398, Merge-Basis 44881ac2.
Gepruefter Diff: `git diff master...HEAD` (drei Punkte; der Zwei-Punkte-Diff zeigt zusaetzlich Master-Dateien, die NACH der
Basis entstanden sind — AUFTRAG/VERTRAG/orchestrator — und keine Aenderung der Spur sind).

## Ergebnis

| Nutzer-Punkt (Wortlaut) | Urteil | Kern-Beleg |
|---|---|---|
| 1 "das soll nicht nur da gelten, das darf generell für die Zombies gelten — schaue einfach nach wie Resident Evil 2 das macht" | **erfuellt** | RE2-Mechanismus selbst disassembliert (Deskriptor 6 = 0x0101 Gang @0x80100ab8-abc, 0 = Stehen @0x801006ac-b8); im Port greift die Abbildung in den Raeumen mit RE1.5-Aktivform (18 laut Zensus; an der exe gemessen 9 davon inkl. Elza 1181 (N3) und die 1070-Kopien in 1030 (N4)), die anderen Formen bleiben -> Mischung wie RE2 |
| 2 "die Zombies sind immer noch zu passiv ... nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen ... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang" | **erfuellt** | echte Tuer 1150 -> 1130 nach der Montage (N1/N2/Z1): alle fuenf Gang ab Bild 6, Griff Bild 896 (steht) / 459 (geht); exe des Nutzers (V1/V2): Stehen 900 Bilder; 1030 (N4) ebenso aktiv |

**bestanden: NEIN** — beide Punkte erfuellt, aber das RE-Gate haelt nicht ganz: **M1** (Zombie-Girl-Beleg zitiert
STAGE1.BIN-Adressen fuer Stage 4) und **M2** (zwei Aussagen im Abschnitt "Fuer den Nutzer" widersprechen der Messung).
Beide sind reine Text-/Zitatkorrekturen, kein Verhalten.


## 0. Bau

`bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (configure)`, `ninja: no work to do`,
`LOCAL-BUILD-OK (build)`; Arbeitsbaum sauber (`git status` leer) -> die exe `re15_port/build/platform/pc/re15_pc.exe`
(12:22) entspricht HEAD.

## 1. RE-Gate — Stichproben selbst disassembliert

Werkzeuge `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (RE2 `info/re2leon/COMMON/BIN/EMZ0.BIN`) und
`re15_disasm.py` (RE1.5 `info/Re1.5/PSX/BIN/STAGE1.BIN`), Sprungziele und Tabellen selbst aufgeloest.

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| RE2 Grundwort 0x0001 = Stehen `@0x801006ac-b8` | `801006ac: addiu v0,zero,1` ... `801006b8: sw v0,4(s2)` (s2 = Entity, a0 @0x80100664) | stimmt |
| RE2 Deskriptor 6 -> 0x0101 `@0x80100ab8-abc` | `80100aa8: lhu v0,270(s2)` (+0x10E), `80100ab0: andi v0,v0,0x3f`, `80100ab4: bne v0,v1(=6),0x80100ac0`, Delay `80100ab8: addiu v0,zero,257`, `80100abc: sw v0,4(s2)` | stimmt |
| RE1.5 grid 0x00 = FUN_80101b64, ENGAGE ohne Abstandsgrenze `@0x80101bf4-c20` | Grid-Tabelle `@0x8011f80c[0] = 0x8010168c`; dort `lbu v0,5(v0)` + Tabelle `0x8011f840` (`addiu at,at,-1984` @0x801016ac) -> `[0] = 0x80101b64`. In 0x80101b64: `jal arc_test(…,0x5f4)` @0x80101b8c, `80101bf4: sll v0,a0,16 / bne -> Ende`, `80101c0c: lhu v0,472(v1) / andi 0x10 / beq -> Ende`, `80101c1c: ori v0,zero,0x201`, `80101c20: sw v0,4(v1)`. Kein Abstandstest vor 0x201. | stimmt — mit Zusatz (s.u.) |
| RE1.5 grid 0x02 = FUN_80101c7c `@0x8011f960[0]` | `@0x8011f80c[2] = 0x80101784`, dort Tabelle `0x8011f960` (`addiu at,at,-1696` @0x801017a4) -> `[0] = 0x80101c7c` | stimmt |
| RE2 Steher wacht < 0x1388 (Kegel 1024 + Sicht) | `801012f0: sltiu v0,s2,0x1388` (s2 = +0x1F0 @0x801012c0), `801012fc: sll v0,s1,16 / bne` (Kegel 1024 @0x801012c8), `80101308: lhu v0,340(s0) / andi 0x800 / beq`, `80101318: addiu v0,zero,257`, `8010131c: sw v0,4(s0)` | stimmt (Kette beginnt @0x801012f0, nicht @0x80101308 wie im Auftrags-Kurztext) |
| Nebenbefund 1: DECISION[0] Block 3 Original committet bei (rand&1)==1 | `801013bc: jal 0x80015fe8`, `801013c4: andi v0,v0,0x1`, `801013c8: beq v0,zero,0x801013d4` (Ueberspringen bei 0), `801013cc: addiu v0,zero,3073`, `801013d0: sw v0,4(s0)`; Port `re2z_decide_stand` Z. 1372-1374: `(re2z_rand() & 1u) == 0u` | Abweichung bestaetigt (vorbestehend, nicht von dieser Spur) |
| RE1.5 grid 0x02 = FUN_80101c7c: ENGAGE nur < 0xFA0 | `80101c8c: jal arc_test(0x5f4)` / `bne -> Ende`, `80101cac: lhu 472 / andi 0x10 / beq`, `80101cc8: sltiu v0,v0,0xfa0 / beq -> Ende`, `80101cd0: ori 0x201`, `80101cd8: sw` (+ 1/8 -> 0x801 @0x80101cdc-f4) | stimmt (Kegel + Sicht + < 4000) |
| RE1.5 grid 0x0d INIT `@0x80100f40-44` | `80100f20: lbu v0,9(a0) / andi 0x1f / bne 0xd`, `80100f34: sb 0x27,148(a0)`, `80100f40: ori v0,zero,0x201`, `80100f44: sw v0,4(v1)`, `80100f54: sb zero,9(v0)` | stimmt |
| Form ROOM1040 main00 `@0x011FC` / ROOM1030 main00 `@0x01DE6` (Datei-Offset) | RDT-Bytes `44 00 16 0d 00 00 00 14 31 9a ...` bzw. `44 00 16 0d 00 00 00 00 ee dc ...` = Sce_em_set Typ 0x16 grid 0x0d | stimmt |
| RE2 Sce_em_set legt pc[4..5] nach +0x10E `@0x8005734c/54` | RE2 PSX.EXE Tabelle `0x800a74c8[0x44] = 0x8005714c`; `80057334: lbu v0,3(v1)` / `8005733c: sb v0,8(s0)`, `8005734c: lhu v0,4(v1)` / `80057354: sh v0,270(s0)` | stimmt |
| RE2 Selbstwecker `@0x80101544-7c`, Wander-Wurf `@0x8010151c-40` | `8010151c: lh v0,346(s0) / bne`, `8010152c: jal rand / andi 1 / bne`, `80101540: sb v0(=2),6(s0)`, danach OHNE Ruecksprung `80101544: lh v0,344(s0)` ... `8010155c: sltiu v0,v0,0x1d4c`, `80101570: andi 0x800`, `8010157c: sw 257,4(s0)`, `8010159c: j 0x801016ac` | stimmt; Nebenbefund 2 (Port faellt nach dem Wurf im selben Takt in P2/P3, ueberspringt den Wecker) bestaetigt (vorbestehend) |
| **Zombie-Girl, Kopf re15_zombieaktiv_r37.h: "Typ 0x13, Stage 4 ROOM4050: Modus-Tabelle @0x80120230[0] = 0x8010b6d4 laedt DECIDE aus @0x80120264[+0x5] (`addiu at,at,612` @0x8010b728, `jalr` @0x8010b738), Eintrag [0] = 0x80101b64"** | Diese Bytes stehen in **STAGE1.BIN** (dort stimmt jede Zeile). Fuer Stage 4 ist das die FALSCHE BINAERDATEI: `STAGE4.BIN` ist 0x1A620 Byte lang (endet @0x8011A620) — @0x80120230/@0x80120264 gibt es dort nicht, @0x80101b64 ist dort Funktionsmitte (`bne a1,zero,0x80101b7c`). Richtig fuer STAGE4.BIN (selbst gesucht): Modus-Eintrag `@0x80119758 -> 0x8010b688`, dort `lbu v0,5(v0)` @0x8010b6cc, `addiu at,at,-26740` @0x8010b6dc (= 0x8011978c), `jalr v0` @0x8010b6ec; `@0x8011978c[0] = 0x80101b18`, und 0x80101b18 ist Wort fuer Wort (70/70, Spruenge relativ) dieselbe Funktion wie STAGE1 FUN_80101b64; auch die Grid-0-Tabelle der Zombies in STAGE4 (`@0x80118d68`) zeigt auf 0x80101b18 | **Aussage stimmt, Zitat falsche Binaerdatei -> Mangel M1** |

**Zusatz zu FUN_80101b64 (nicht im Dossier):** im ENGAGE-Zweig folgt nach `sw 0x201` noch `80101c38: sltiu v0,v0,0x2711` /
`bne -> Ende` / `jal rng` / `andi 0x7` / `bne -> Ende` / `ori v0,zero,0x801` / `sw v0,4(v1)` @0x80101c38-64: ist Leon >= 10001
entfernt, wird der ENGAGE mit 1/8 je Aufruf durch 0x801 ersetzt; dazu der Zaehler +0x9C (`lhu a1,156(a0)` / Dekrement /
`bne a1,zero` / `ori v0,zero,0x101` / `sw` @0x80101ba0-c4) und 0x701 bei dist < 0x7D0 ausserhalb des Kegels 0x2c8
(@0x80101bd4-f0). Die Kernaussage "Sicht im Weitkegel -> ENGAGE ohne Abstandsgrenze" haelt; der Satz "der einzige
Abstandstest gilt dem 1/8-Wander-Wurf" ist richtig, aber der Wurf greift IM Engage-Zweig (Engage wird bei Fernsicht zu 7/8).

## 2. Messungen an der exe (eigene Laeufe, eigene Messkopie `re15_pc_r37h_abn0_<id>.exe`, eigener Auswerter)

Treiber `lauf.sh` (Karte mit `probe_r36_gegnerzahl_karte`, `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`, Eingabeskript ab
Spielbild 60, `RE15_STATE_LOG`, `RE15_EXIT_AT`, `RE15_FRAMEDUMP`, KEIN Software-Renderer) und Auswerter `werte.py`
(je Besuch, je Gegner: Zustandsfolge st/ss1, erstes Bild im Gang (1/1), EIGENER Abstand aus den Lagen Leon/Zombie,
Weg, Leon-HP) liegen in der Sitzungs-Ablage `scratchpad/h_abn/tools/` (nicht eingecheckt).

### N1 — Nutzer-Beispiel, echte Tuer 1150 -> 1130 nach der Montage, Leon steht
Karte `1150 -17150 -11960 0  9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7 7:0xc6-0xca` (= Stand nach der Montage, wie der
Riegel), Skript `W1,A0.2` (Aktion an der Tuer), Leon bleibt danach stehen. debug.log: `CONTINUE: resumed in room 1150`,
`[room] PC loaded room1130.rdt`, `[irons-tod] ROOM1130 Zustand 0: Nachspawn 1130 (Programm 10)`.
```
Besuch 1: F1..F1457, Leon (-5900,16350), HP 100 -> -20 (erstes Bild HP<100: 917)
slot 1 t=16 g=0d Gang ab F6 | Abstand F1:30879 F300:25956 F600:21738 F900:17287 F1200:12275
slot 2 t=10 g=0d Gang ab F6 | Abstand F1:29674 F300:23180 F600:17245 F900:13017 F1200:8391
slot 3 t=10 g=0d Gang ab F6 | Abstand F1:28616 F300:23872 F600:19607 F900:15184 F1200:12417
slot 4 t=11 g=0d Gang ab F6 | Abstand F1:27536 F300:20358 F600:13834 F900:7490  (1/9@986 1/5@1000)
slot 5 t=11 g=0d Gang ab F6 | Abstand F1:28429 F300:19582 F600:11116 F900:981  (Griff 1/3@896)
```
Alle fuenf sind ab dem ersten KI-Takt nach der Blende (F6) im Gang und kommen den ganzen Gang herunter; der erste greift
in F896, Leon (ohne Gegenwehr) stirbt in F1457. Danach laedt `RE15_CONTINUE_TEST` den Spielstand erneut (Schleife bis
zum Abbruch — Folge des Todes, kein Haenger; Lauf per PID beendet). Framedump (Kamera Cut 6 an der Chief-Office-Tuer):
F150..F750 nur Leon im Bild, F900 Zombie greift Leon an der Tuer, F1200 Biss mit Blutlache.
Vorher (v0.8.24, Dossier M1, im Riegel als Gegenprobe gemessen): 1800 Bilder kein Zombie im Gang.

### N2 — wie N1, Leon geht den Gang hinunter (`W1,A0.2,W1,R0.33,U25`, Ende F700)
```
Besuch 1: F1..F700, Leon (-5900,16350) -> (-939,1434), HP 100 -> 40 (erstes Bild HP<100: 480)
slot 1 Gang ab F6 | Abstand F1:30879 F200:22810 F300:13742 F400:9488 F600:5999 F700:4831
slot 2 Gang ab F6 | Abstand F1:29674 F200:20528 F300:11083 F400:6320 F600:2376 (Griff 1/3@683)
slot 3 Gang ab F6 | Abstand F1:28616 F200:20607 F300:11670 F400:7226 F600:3869 F700:2334
slot 4 Gang ab F6 | Abstand F1:27536 F200:17911 F300:8290  F400:3435 (1/9@549)
slot 5 Gang ab F6 | Abstand F1:28429 F200:18160 F300:7684  F400:3145 (Griff 1/3@459)
```
Die fuenf laufen Leon im Gang entgegen; Framedump F400-F520: ein Zombie kommt von links aus der Gangbiegung, greift Leon
in F459 (HP 100 -> 40 bis F700). Vorher (Dossier M1 Lauf w): Wechsel in den Gang erst F672..702 bei Abstand ~6000-7000.

### V1/V2 — Gegenprobe mit der exe des Nutzers, gleiche Karte, gleiche Skripte
exe = Hauptbaum `re15_port/build/platform/pc/re15_pc.exe` (Bau 09:02, vor Runde 37 = ohne Spur H; master hat seit der
Basis 44881ac2 keine Aenderung unter re15_port), nur gelesen; die Messkopie lag im Spurbaum.
```
V1 (steht, F1..F900): alle fuenf t=16/10/10/11/11 g=02 Folge 0/0@1 1/0@6 (Stehen), Abstand F1 27536..30879 -> F900
   27514..29761, Leon-HP 100 -> 100.                                   = Nutzer-Befund "stumpf im Gang stehen"
V2 (geht, F1..F700): Stehen bis Leon auf 5000..7000 heran ist, erst dann Gang (1/1 @672/684/688/694, einer nie);
   Leon-HP 100 -> 100.
```
Gegen N1/N2 (HEAD): Gang ab F6 aus 27500..30900, Griff F896 (steht) bzw. F459 (geht). Der Unterschied liegt genau an
der Spawnform des Nachspawns (g=02 vorher, 0x0d jetzt) — der Fix erklaert den Befund.

### N4 — der weitere Weg nach der Montage: echte Tuer 1040 -> 1030 (Karte 1040 (-20083,-3559) Blick 0, Montage-Bits)
debug.log: `Nachspawn 1040 (Programm 14)`, `[room] PC loaded room1030.rdt`, `Nachspawn 1030 (Programm 11-13)`. 14 Zombies:
6 Raum-Records (0x0d), 5 Kopien aus 1070 (`EM_1070_LISTE`, grid 0x00 -> jetzt abgebildet), 3 Kriecher (0x0d) — ALLE Gang
ab F6; die 1070-Kopien (3816..7622 vor Leon) greifen ab F86, Leon (steht) HP 100 -> 0 bis F450. Framedump F30-F450:
der Pulk kommt den Gang herunter zur Tuer, Griffe/Abstossen an der Tuer. (Vorher laut Dossier M2: die 1070-Kopien wachten
erst in Bild 1..328.)

### Z1 — die GANZE Kette in einem Lauf (ohne Zwischenspeichern): Karte `probe_r35_cut1150_karte 1130 nach10f0 ersteszene`
(1130 vor Irons' Tuer, alle Zombies in 1140/1070 leben) + CONTINUE, `W1,A0.2` -> Tuer -> 1150, danach Tuer Slot 0 1150 -> 1130
ueber `RE15_FIRE_AOT=0@1450#1150` (derselbe Pfad wie das Hineinlaufen, re15_aot_fire_slot), Ende `600#1130`.
debug.log in dieser Reihenfolge: `Szene startet (Programm 0)` -> `Montage 1130 (Programm 1)` -> `Montage 1040 (Programm 2)` ->
`Bilanz 1040: 5 Zombies, durchs Tor 5 ... Spieler-HP 100 -> min 100` -> `Montage 1030 (Programm 3)` -> `Montage 11C0
(Programm 4)` -> `Rueckkehr: Programm 5` -> `Rueckkehr beendet, Steuerung frei` -> `Bilanz Kette: Spieler-HP 100 -> min 100`
-> `[fire-aot] slot=0 at F1450 (Raum 1150)` -> `Nachspawn 1130 (Programm 10)` -> `EXIT_AT: Bild 600 in Raum 1130`.
Letzter Besuch 1130: alle fuenf g=0d Gang ab F6, Abstand F1 27536..30879 -> F600 11116..21738 (identisch zu N1).
Der Montage-SCHRITT 1130 (Besuch 2, F1..F181, Leon geparkt bei (-3050,-2150)): alle fuenf g=02 Folge 0/0@1 1/0@6, Weg 0..18
— wie vorgesehen unveraendert (Framedump Z2 F20..F170, Kamera Cut 0 vor der Briefing-Room-Tuer: die fuenf stehen dort
waehrend der ganzen Einstellung; s. Hinweis H3).

**Urteil Punkt 2 (Nutzer-Beispiel Montage-Zombies im Gang): erfuellt** — gemessen am echten Tuerweg mit der exe; die
Zombies stehen nicht mehr, sie gehen ab Bild 6 auf Leon zu und werden zur Bedrohung (Griff/Biss, Tod ohne Gegenwehr).

### N3 — "generell fuer die Zombies": Zensus an der exe (alle Raeume mit RE1.5 grid 0x00 + Gegenproben)

Datenzensus (Walker `re15_port/tools/aot_sce_census.py` ueber alle 240 RDTs, Skript des Dossiers selbst ausgefuehrt):
419 Zombie-Sce_em_set, grid 0x00 = 72 in 18 Raeumen (1010/1011 1020/1021 1070/1071 1100/1101 1180/1181 11E0/11E1
11F0/11F1 1250/1251 4050/4051), 0x02 = 91 (1090 10B0 10D0 10E0 1140 1220 2020 30D0 30E0 40B0 5120 5140 + Elza),
0x0d = 142 — deckt sich mit dem Dossier. RE2-Zensus (Dossier-Skript, selbst ausgefuehrt): 433 Saetze, Deskriptor 0 = 214,
6 = 140; ROOM20A0 sub00 @0x00A3E `44 00 00 1f 06 00 ...` = Typ 0x1F Deskriptor 6 an (-15699,3739) wie zitiert.

Exe-Laeufe (eigener Auswerter `zensus.py`: Spawn-Satz aus `RE15_SPAWN_DIAG` <-> Aktor ueber die Spawn-Lage, erstes Bild
im Gang). Sprung = Debug-Menue-Sprung (`RE15_DEBUG_JUMP=<raum>@250`, Neues Spiel), Karte = Spielstand im Raum + CONTINUE
(Raumaufbau wie beim Betreten), Bits = die If-Bedingung der aufrechten Form, selbst aus dem SCD gelesen:

| Raum | Weg | Zombies (RE1.5-Form) | Ergebnis |
|---|---|---|---|
| 1010 | Sprung | 2x grid 0x00 | beide Gang ab F6 (einer greift F107) |
| 1180 | Sprung | 3x 0x00 | alle Gang ab F6 |
| 11E0 | Sprung | 4x 0x00 | alle Gang ab F6 |
| 1250 | Sprung | 7x 0x00 | alle Gang ab F6 |
| 1020 | Karte, Bit (3,0xC8) (If @0x01E10 `06 00 6c 00`, Ck @0x01E14 `21 03 c8 00`, aufrechte Form im Else-Zweig) | 4x 0x00 (aufgestanden) | alle Gang ab F0 |
| 1070 | Karte, Bit (4,0xC5) (If @0x015AE `06 00 80 00`, Ck @0x015B2 `21 04 c5 00`) | 5x 0x00 | alle Gang ab F0 |
| 1100 | Karte, Bit (4,0xE8) (If @0x009EE `06 00 6c 00`, Ck @0x009F2 `21 04 e8 00`) | 5x 0x00 | alle Gang ab F0 |
| 11F0 | Karte, Bit (4,0xEE) (If @0x00D5C `06 00 be 02`, Ck @0x00D60 `21 04 ee 00`) | 4x 0x00 | alle Gang ab F0 |
| **1181 (Elza)** | Elza-Karte (`probe_r36_elza_karte`, Figur 4) + CONTINUE | 3x 0x00 | alle Gang ab F0, der nahe aus 7047 nach 300 Bildern auf 1768 |
| 1130 nach der Montage | echte Tuer (N1/N2) | 5x 0x0d (Nachspawn) | alle Gang ab F6 |
| 1020/1070/1100/11F0 Erstbesuch | Sprung | 0x87/0x88 Liegende | 1/7 (Liegen) — unveraendert |
| 10E0 | Sprung | 0x88 + 4x 0x06 (Fresser) | 1/7 bzw. 1/8 — unveraendert |
| 1220 | Sprung | 2x 0x81 | 1/2 — unveraendert |
| 1200 | Sprung | 0x87/0x88/0xA1 | Liegen / 0xA1 Zustand 0 — unveraendert |
| 1030 | Sprung | 0x0d Raum-Records | Gang ab F6 — wie vorher (0x0d war schon Gang) |
| 4050 | Sprung | im Neuspiel-Zustand kein Zombie-Satz (Switch auf Variable 0x0A @0x01E56, Faelle 6/9/0xB/0xE) | nicht an der exe gemessen; Typ 0x13 gehoert der RE2-Familie (`re15_re2z_owns_type` Z. 205-209), Abbildung greift dort ueber denselben Haken |

Damit greift die Abbildung in jedem Raum mit RE1.5-Aktiv-Form (bei Leon UND Elza) ueber den echten Spawn-Weg; die
anderen RE1.5-Formen (Schlaefer 0x02, Liegende, Fresser, Kriecher, Sitzer) bleiben, wie sie waren — der Raum mischt dann
wie RE2 aktive und stehende/liegende Zombies.

**Verhaeltnis zum RE2-Vorbild (Beobachtung, kein Mangel):** in RE2 tragen 140 von 354 aufrechten Zombie-Saetzen
Deskriptor 6 (40 %); im Port gehen jetzt 214 von 305 aufrechten RE1.5-Saetzen ab Spawn (0x00 + 0x0d, 70 %). Die
Abbildung ist eine dokumentierte PORT-ABBILDUNG (Kopf re15_zombieaktiv_r37.h: RE1.5-Klasse "greift auf Sicht ohne
Abstandsgrenze an" -> RE2-Klasse "Gang ab Spawn"), nicht die RE2-Quote. Der Port ist damit etwas aggressiver als RE2;
das liegt in der Richtung des Nutzer-Befunds ("zu passiv"), sollte dem Nutzer aber so gesagt werden.

**Urteil Punkt 1 (generell fuer die Zombies, wie RE2): erfuellt** — der RE2-Mechanismus (Deskriptor 6 = Gang ab Spawn,
Deskriptor 0 = Stehen mit Wecker/Wander) ist am Original belegt (eigene Disassemblierung, s. 1.) und gilt im Port fuer
alle Raeume beider Figuren ueber den Sce_em_set-Weg; an der exe gemessen in 9 Raeumen mit grid 0x00 (8 Leon, 1 Elza), dazu 1130 und 1030 nach der Montage.

### N5 — Gegenprobe RE1.5-KI (`RE15_AI_FLAVOR=re15`), gleicher Weg wie N1, 400 Bilder
```
HEAD:    alle fuenf g=0d Folge 0/0@1 1/2@6 (RE1.5-ENGAGE 0x201), Abstand F1 27536..30879 -> F400 20006..27163
v0.8.24: alle fuenf g=02 Folge 0/0@1 1/0@6 (Stehen), Abstand unveraendert (Weg 0..18)
```
Der Nachspawn 1130 wirkt also auch unter der RE1.5-KI (der Nachspawn ist eine Port-Konstruktion, die Aenderung ist im
Sinn des Nutzers) — der Dossier-Satz "RE1.5-KI im Menue: unveraendert (Original-Verhalten)" stimmt dafuer nicht (-> M2).

## 3. Gates

### 3.1 RE-Gate (Konstanten im Diff)
| Konstante | Beleg im Code | geprueft |
|---|---|---|
| `RE15_R37_RE2_WORT_GANG 0x0101` | `@0x80100ab8-abc` (EMZ0.BIN) | selbst disassembliert, stimmt |
| `RE15_R37_RE2_WORT_STEHEN 0x0001` | `@0x801006ac-b8` | stimmt |
| `RE15_R37_RE15_AKTIV 0x00` | `@0x8011f80c[0]` -> DECIDE `@0x8011f840[0]` = FUN_80101b64 (STAGE1) | stimmt fuer STAGE1; Zombie-Girl-Absatz zitiert fuer Stage 4 STAGE1-Adressen -> **M1** |
| `EM_1130_NACH` grid 0x0d | NUTZER-VORGABE + `@0x80100f40-44` + Form ROOM1040 @0x011FC / ROOM1030 @0x01DE6 | stimmt (Bytes selbst gelesen) |
| Riegel-Schwellen Bild <= 2 / >= 15 je Bild / Bild 7 / Bild 306 / 4500 | RE2-Original-Messung (`H_belege/re2_original_room20a0_takte.txt`: 0x00010101 ab Takt 2, kleinste 100-Takt-Rate 1536) | Datei gelesen, Werte stimmen mit der Tabelle ueberein |

Keine "deferred/tunable/interim/for now/faithful/plausibel/TODO" im Code-Diff, kein `getenv` (kein Env-Schalter als
Abschluss; `re15_r37_zombieaktiv_enable(0)` ist nur aus Sonden/Riegeln erreichbar — grep im Diff und in platform/pc).

### 3.2 Vertrag
* Pfade (`git diff master...HEAD --name-only`): nur `analysis/befunde_runde37/H_*`, `engine/src/zombieaktiv_r37.c`,
  `include/re15_zombieaktiv_r37.h` (neue Dateien nach Namensregel), Haken in `scd_vm.c` (+2: Include + 1 Zeile im
  Sce_em_set), `enemy_ai_re2_zombie.c` (+4: Include, 1 Zeile rng_reset, 2 Zeilen re2z_init), `irons_tod_1150.c` (+14/-2,
  H-Datei laut VERTRAG §1.4: neue Spawnliste + Weiche), Tests nur `tests/unit/probes/r37_zombieaktiv.cmake`,
  `tests/unit/test_r37_zombieaktiv.c`, `tests/integration/test_r37_zombieaktiv.cmake` (mit `cmake_minimum_required(VERSION
  3.16)`), dazu `tests/unit/test_r36_zombieki_sicht.c` (+6, begruendet: der Runde-36-Sichtzensus misst STEHENDE Zombies).
  Kein release/, platform/android/, shared_assets/, keine Test-CMakeLists. Haelt.
* Keine Bank-9-Bits, keine Text-Ids, keine Ereignisse/Slots neu (die Tot-Bits 40-43/46 des Nachspawns sind die der
  bisherigen Liste). Haelt.
* Stil (kein Mangel): `#define EM_1130_NACH` steht als eine 300-Zeichen-Zeile (die Zeilenfortsetzungen der Nachbarliste
  `EM_1130_LISTE` fehlen) — schwer lesbar, Inhalt korrekt (gleiche Typen/Tot-Bits/Lagen/Blick, nur grid 0x02 -> 0x0d).

### 3.3 Tests (gezielt, KEINE volle Suite im Spurbaum)
* Spur-Riegel: `ctest -R "r37_zombieaktiv" --timeout 600 -j 2` -> **4/4 gruen** (unit_r37_zombieaktiv_abbildung 0.07 s,
  _nachspawn1130 0.13 s, _raum1180 0.11 s, integration_r37_zombieaktiv 26.7 s). Die Unit-Riegel einzeln mit Ausgabe gefahren:
  abbildung 6/6 (Tabelle aller 14 RE1.5-Formen mit/ohne Abbildung — nur 0x00 wechselt 1/0 -> 1/1, Gang ab Bild 1; ohne
  Sce_em_set und mit liegengebliebener Marke Steher; Montage-Schritt S1030 Steher), nachspawn1130 4/4 (Gegenprobe v0.8.24:
  5/5 nie im Gang), raum1180 5/5 (Gegenprobe: keiner <= Bild 2). Die Riegel messen das, was der Nutzer meint (Zustand +
  Annaeherung an Leon), mit Gegenprobe gegen den alten Stand.
* Unit/Sonden aller beruehrten Module, breiter als der Bau-Agent (RE2-Zombie, Montage/Irons, Gegnerzahl, Zombie-KI R36,
  alle Raeume mit grid 0x00 inkl. Elza-Nummern, Story, Elza, Spawn/SCD, Girl, Birkin, Hund, Kraehe, 1030/1040/1130, Gore,
  Griff/Biss): `ctest -R "re2|zombie|r35_cut1150|...|gore|grab|griff|biss" -E "^integration_" --timeout 900 -j 4` ->
  **100% tests passed, 0 tests failed out of 228**.
* Integration (echte exe), dieselben Gruppen wie der Bau-Agent + PRI-Masken/Irons-Tisch/Schrift 1150/Dachkiste:
  `ctest -R "^integration_(r35_entladen|r34n_e_dokumente|r35_cut10f0|r35_zgirl$|r36_elza|r36_inventar|r36_pri_schatten|
  r36_story|r35_cut1150|r36_szene1150|r36_gegnerzahl|r30_cut_blitz|r37_zombieaktiv|pri_masken|r30_irons_tisch|
  r34n_g_schrift1150|r36_dachkiste)" --timeout 1500 -j 3` -> **100% tests passed, 0 tests failed out of 35** (2392 s, unter Last parallel
  laufender Agenten; kein Wiederholungslauf noetig)
  Darin integration_r35_cut1150 (Montage-Kette, `Bilanz Kette: Spieler-HP 100 -> min 100`) und integration_r36_szene1150.

## 4. Maengel

**M1 (RE-Gate, Zitat in der falschen Binaerdatei) — `re15_port/include/re15_zombieaktiv_r37.h` Kopf, Absatz Zombie-Girl.**
Dort steht: "Dieselbe Klasse fuer das Zombie-Girl (Typ 0x13, Stage 4 ROOM4050): Modus-Tabelle @0x80120230[0] = 0x8010b6d4 laedt
DECIDE aus @0x80120264[+0x5] (`addiu at,at,612` @0x8010b728, `jalr` @0x8010b738), Eintrag [0] = 0x80101b64". Diese Adressen
gibt es nur in STAGE1.BIN. ROOM4050 laeuft mit STAGE4.BIN (0x1A620 Byte, endet @0x8011A620 — 0x80120230/0x80120264 liegen
ausserhalb, @0x80101b64 ist dort Funktionsmitte `bne a1,zero,0x80101b7c`). Die Aussage selbst stimmt (selbst gemessen):
STAGE4.BIN Modus-Eintrag `@0x80119758 -> 0x8010b688`, `lbu v0,5(v0)` @0x8010b6cc, `addiu at,at,-26740` @0x8010b6dc (=
0x8011978c), `jalr v0` @0x8010b6ec, `@0x8011978c[0] = 0x80101b18`, und 0x80101b18 ist Wort fuer Wort (70/70 Worte,
Spruenge relativ) STAGE1 FUN_80101b64; die Grid-0-Tabelle der Stage-4-Zombies `@0x80118d68` zeigt ebenfalls auf 0x80101b18.
Dieselbe Verwechslung steckt in der Abschlussmeldung ("beim Zombie-Girl derselbe Eintrag @0x80120264[0]").
Nachbesserung: Zitat auf STAGE4.BIN umstellen (oder beide Binaerdateien nennen). Kein Code-Verhalten betroffen.

**M2 (Aussagen an den Nutzer stimmen nicht mit der Messung) — Dossier `H_zombieaktiv.md` Abschnitt "Fuer den Nutzer".**
(a) "RE2 laesst rund ein Drittel seiner Zombies schon beim Erscheinen auf Leon zugehen ... Genau diese Aufteilung gilt jetzt
auch fuer RE1.5" — gemessen (Zensus selbst gefahren): RE2 140 von 433 Zombie-Saetzen (32 %; 140/354 aufrechte = 40 %) gehen
ab Spawn, der Port jetzt 214 von 419 (51 %; 214/305 aufrechte = 70 %). Die Aufteilung folgt den RE1.5-Klassen, nicht der
RE2-Quote; der Port ist aggressiver als RE2. (b) "RE1.5-KI im Menue: unveraendert (Original-Verhalten)" — fuer den Nachspawn
1130 falsch: N5 (`RE15_AI_FLAVOR=re15`) zeigt nachher 0/0@1 1/2@6 (ENGAGE ab F6, Annaeherung 2511..8610 in 400 Bildern),
vorher 1/0 (Stehen, Weg 0). Nachbesserung: beide Saetze an die Messung anpassen (Verhalten selbst ist im Sinn des Nutzers).

## 5. Hinweise (kein Mangel dieser Spur, fuer Orchestrator/Nutzer)

* **H1 — zwei vorbestehende RE2-Abweichungen im Steh-Zweig (selbst bestaetigt, im Dossier OFFEN):** DECISION[0] Block 3
  committet im Original bei (rand&1)==1 (`beq v0,zero,0x801013d4` @0x801013c8), der Port bei ==0 (enemy_ai_re2_zombie.c
  Z. 1372-1374); EXEC[0] P1: nach dem Wander-Wurf (`sb v0,6(s0)` = 2 @0x80101540) laeuft im Original im selben Takt der
  Selbstwecker @0x80101544-a0 und P2 erst im naechsten Takt (`j 0x801016ac` @0x8010159c), der Port springt im selben Takt nach
  P2/P3 (Wurf Z. 1441, Wecker-Sperre `if (e->sub_state_2 == 1)` Z. 1446, P2 Z. 1456). Beides verursacht die Passivitaet nicht (gleiche Wahrscheinlichkeit bzw. ein Takt), verschiebt beim
  Beheben aber den gemeinsamen RE2-Zufallsstrom -> eigener Schritt mit Strom-Messung, wie das Dossier vorschlaegt.
* **H2 — Robustheit der Marke:** re15_r37_re2_gang verlangt Typ UND exakte Spawn-Lage beim INIT; wird ein Zombie zwischen
  Sce_em_set und seinem ersten KI-Takt verschoben, bleibt er stillschweigend Steher. Gemessen trat das in keinem der 10
  geprueften Raeume auf (in 1070 wurde ein Zombie um +638 x versetzt, aber NACH dem INIT — Gang ab F0).
* **H3 — die Montage-EINSTELLUNG 1130 (Cut 0, 181 Bilder) zeigt die fuenf weiter stehend** (Z1/Z2, Absicht der Spur: Leon
  ist dort geparkt). Die AUFTRAG-Lesart ("stehen danach nur im Gang") ist erfuellt; falls der Nutzer auch die Einstellung
  selbst meinte ("wenn wir die ganzen Zombies spawnen ... bleiben einfach nur stumpf alle im Gang stehen"), bleibt das
  sichtbar. Daten fuer eine Entscheidung (Rechnung, nicht gemessen): der Parkplatz liegt 8744..12162 von den fuenf entfernt
  (Z1 Besuch 2); bei den in N1 gemessenen 15..34 Einheiten je Bild braeuchte ein gehender Zombie dafuer >= 257 Bilder, die
  Einstellung dauert 181 (anders als in 1030, wo die 1070-Kopien 4912..8701 vom Parkplatz (-27300,-3650) stehen und die Sperre noetig ist).
* **H4 —** Der Port ist nach der Abbildung aggressiver als RE2 (s. M2a; N4: in 1030 nach der Montage stirbt ein stehender
  Leon nach 450 Bildern, 14 Zombies im Gang) — dem Nutzer so sagen.
* **H5 —** ROOM4050 (Zombie-Girls, grid 0x00) nicht an der exe gemessen (Switch auf Variable 0x0A, im Neuspiel kein
  Zombie-Satz); Typ 0x13 ist RE2-Familie, derselbe Haken.
* **H6 —** `#define EM_1130_NACH` als eine ueberlange Zeile (Stil).
* Release (Bau-Agent): neue Quelldatei zombieaktiv_r37.c (Android-GLOB-Cache neu konfigurieren), +4 Tests (RE15_MIN_TESTS).
