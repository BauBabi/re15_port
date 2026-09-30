# Spur G2 (ROOM1150 Irons Office, blinkende Schrift) — ABNAHME 1

Stufe: ABNAHME 1 (unabhaengig, nach dem Bau). Prueferin hat den Code nicht geschrieben.
Auftrag: versuchen zu WIDERLEGEN, dass Spur G fertig ist.

Status: ABGESCHLOSSEN 2026-09-30 ~11:50.

## 0. Urteil

**abgenommen** (mit einem kosmetischen Mangel M1, Abschnitt 6).

Widerlegungsversuche, alle gescheitert: Die Leuchtschrift "HEAVEN" hinter Irons' Schreibtisch
(ROOM1150/1151 Cut 2) blinkt an der echten exe auf sechs vom Bauer NICHT benutzten Wegen
(Spielstand des Nutzers, echter Tuerweg 1130 -> 1150 zu Fuss, Wiedereintritt in einer AUS-Phase,
ROOM1151 zu Fuss, Nahansicht/Speichern am Schreibtisch mit Memory Card, Laden des eben
gespeicherten Stands), und zwar **pixelgleich zum Original-Bildspeicher** (409/409 Schriftpunkte
bitgleich im AN-Zustand, <= 2 Stufen im AUS-Zustand) mit **genau 20 Spielbildern je Zustand**
(= 40 VBlanks des Originals, Wanduhr 0,67 s gemessen). Vorher (alte exe, gleicher Nutzerstand)
131/131 Bilder AN. Mechanismus und alle zitierten Adressen selbst disassembliert (FUN_800392d4 ganz,
FUN_800396a8, Handler 0x800428d4, Zeichentest, alle Dirty-Schreiber); Suite selbst gebaut
430/430. Statusschirm/Zielen/Schiessen stoeren den Takt nicht; Neuaufbau genau an den
Dirty-1-Stellen des Originals.

## 1. Stand gelesen (git log, Dossier, Gegenpruefung)

* Zweig `r34n/schrift1170`, HEAD `303094c8` (Dossier 9.7), 32 Commits seit Basis `cf0e68ba`.
  Code-Aenderung (`git diff cf0e68ba..HEAD -- engine platform include`): neu
  `engine/src/masken_gruppen.c` + `include/re15_masken_gruppen.h`; Haken je 1-9 Zeilen in
  `scd_vm.c` (Tabelle 0x45), `room_common.c` Schritt 9, `platform/pc/main.c` pc_cam_present_apply,
  `render_pc.c` Maskenliste, `platform/psx/main.c`, `platform/psx/src/render.c`; dazu Pins/Sonden
  und der Integrations-Riegel. `local_build.sh` weicht von master nur um den Bau-Fix 7d4d11dd ab
  (`git diff master -- re15_port/tools/local_build.sh` leer).
* Dossier `G2_schrift1150.md`: Lesart "HEAVEN"-Leuchtschrift Cut 2 hinter Irons' Schreibtisch;
  Mechanismus sub05 + Opcode 0x45 (Col_chg_set) schaltet sprite.pri-Gruppen 6..11; Original in
  DuckStation an 6 Staenden gemessen (G2_06); Bau + eigene Abnahme §9 (Lade-Weg, Debug-Sprung zu
  Fuss, Inventar, 8 weitere Raeume, DuckStation-Gegenprobe ROOM3000).
* Gegenpruefung `G2_schrift1150.gegenpruefung.md`: "haltbar mit Auflagen", 10 Auflagen; das
  Dossier meldet alle 10 erfuellt bzw. begruendet abgelehnt (Nr. 2 Optionsschirm).
* Nicht vom Bauer abgedeckte Wege (daher hier selbst gefahren, Abschnitt 3/4): echter Tuerweg
  ROOM1130 -> ROOM1150 und zu Fuss in Cut 2; Wiederbetreten von Cut 2 in einer AUS-Phase an der exe;
  Spielstand des NUTZERS (Hauptbaum `re15_card.mcr`, nur gelesen/kopiert); ROOM1151 zu Fuss statt
  per Karte direkt in Cut 2.

## 2. Eigener Bau + Suite-Zeile

`bash re15_port/tools/local_build.sh` im Baum `r34n_schrift`, HEAD `303094c8` (Code-Stand unveraendert
seit dem Bau; `ninja: no work to do`), 10:58-11:10, parallel liefen die Suiten zweier anderer Baeume
(r34n_integration, r34n_adaruf) und exe-Laeufe von r34g_int:

    Total Test time (real) = 741.93 sec
    test OK — 430/430 bestanden
    === LOCAL-BUILD-OK (all) — Tests 430/430

Darin `unit_r34n_g_maskgrp` (0 s) und `integration_r34n_g_schrift1150` (57 s) gruen, kein Ausfall,
auch keiner der bekannten Flatter-Haken. Messlaeufe danach mit einer KOPIE `re15_pc_abn1g2.exe`
(md5 94ccfc7a… == re15_pc.exe).

## 3. Nutzerpunkt an der echten exe (AUFTRAG.md, Nutzer-Korrektur ROOM1150)

Nutzerwortlaut: *"Nein, nicht beim Heliport, sondern in Irons Office room 1150 blinkt die Schrift
eigentlich im Hintergrund."* Gemessen wird das BILD, ohne das Port-Log als Orakel:
eigener Auswerter `re15_port/tools/r34n_g/abn1_blink_eval.py` vergleicht das Schrift-Rechteck jedes
Port-Bilds (RE15_FRAMEDUMP, Skala 3, Mittelpunkt je 3x3-Block, 5 Bit) direkt mit dem
**Original-Bildspeicher** zweier sauberer DuckStation-Staende der Ermittlung (`c2_w12.7.sav` = Masken
an, `c2_w12.sav` = Masken aus; Stub @0x80026e4c == 0x03e00008 geprueft; je zwei weitere Staende
desselben Zustands sind bitgleich, `c2_w10.5`/`c2_w13.5`). Im Original unterscheiden sich AN und AUS
in 545 Punkten, davon 409 um mehr als 2 Stufen ("Schriftpunkte"). Port-Bild = AN, wenn alle 409
Schriftpunkte **bitgleich** zum Original-AN sind; = AUS, wenn alle 409 hoechstens 2 Stufen vom
Original-AUS liegen (BSS-Dekodierung Port/PSX); sonst "?" (anderer Cut).

| Lauf | Weg (echte exe, Kopie) | Ergebnis (Bild gegen Original) | Beleg |
|---|---|---|---|
| U | **Spielstand des NUTZERS** (Hauptbaum `re15_port/build/platform/pc/re15_card.mcr`, nur kopiert; Slot 0 = v8-Stand ROOM1150, camera_cut 2, Pos (-22689,-19693)), CONTINUE + CARD_AUTO | F100..F230: jedes Bild AN (409/409 bitgleich Original-AN) oder AUS (409/409 <= 2 Stufen Original-AUS); Laeufe AN 100-118, AUS 119-138, AN 139-158, AUS 159-178, AN 179-198, AUS 199-218, AN 219-230: **jeder volle Lauf 20 Bilder** | `G_belege/G2_A1_01_nutzerkarte_1150.png`, `G2_A1_eval_U.txt` |
| D1 | **echter Tuerweg**: Titel -> Spiel, `RE15_DEBUG_JUMP=1130@gp`, `RE15_FIRE_AOT=2@30#1130` (ROOM1130 Door_aot_set slot 2 @0x008CE -> Raum 0x15, Eintritts-Cut 0), in 1150 zu Fuss (`U3.84,W0.5,D1.5,W1,U1.5,W4`, Basis Spielbild, Start F100) | Cut 0 -> Cut 1 (F125) -> **Cut 2 bei F204**: AN F204 (Aufbau), AUS F205-224, AN F225-239; zurueck in Cut 1 F240; wieder Cut 2 F345: AN 345-364, AUS 365-384, AN 385-404, AUS 405-424, AN 425-444, AUS 445-464 | `G2_A1_eval_D1.txt` |
| D2 | wie D1, Wiedereintritt spaeter (`W1.8`) | Wiedereintritt in Cut 2 bei **F369 mitten in einer AUS-Phase** (:= 0 war F365): sofort AN (Neuaufbau), das `:= 1` bei F385 aendert nichts, AUS erst beim naechsten `:= 0` F405 -> AN 36 Bilder, dann 20/20 — genau die Original-Regel (Aufbau FUN_800392d4 @0x80021c28 bei jedem Dirty-1-Apply, sub05 laeuft unabhaengig weiter) | `G2_A1_02_tuerweg_wiedereintritt.png`, `G2_A1_eval_D2.txt` |
| E3 | **ROOM1151** (Elza-Raum): Karte `probe_r34n_g_karte re15_card.mcr 1151 1 p=-22900,-15900`, CONTINUE in Cut 1, zu Fuss (`D1.5`) nach Cut 2 | Cut 2 bei F70: AN 70-78, AUS 79-98, AN 99-118, ... AUS 239-258: jeder volle Lauf 20 Bilder, pixelgleich zum Original wie 1150 | `G2_A1_03_raum1151_zu_fuss.png`, `G2_A1_eval_E3.txt` |

Selbst angesehen (G2_A1_01..03): AN = die sechs Buchstaben als unbeleuchtete Lamellen (grau mit
hellen Punkten) im roten Schein, AUS = gleichmaessig rot beleuchtete Lamellen — dasselbe Bildpaar wie
der Original-Bildspeicher (Dossier G2_05). Das Nutzer-Symptom "blinkt bei uns nicht" ist an allen
vier Wegen behoben; der Takt (20 Spielbilder = 20 SCD-Takte je Zustand) entspricht den 40 VBlanks
des Originals (VSync(2), Dossier §3.5).

Nebenbeobachtung Tuerweg (Log D1): der erste sub05-Takt laeuft in der SCD-Init des Raumwechsels
(`F30 ... Gruppe 6 := 0 (0 Treffer, Zahl 0)` noch mit dem Bildzaehler von 1130, VOR `Aufbau Cut 0
... Grund raum`), danach `:= 1` bei 1150-F25. Selbst nachgelesen: im Original laeuft der Init-Durchlauf
aller Faeden ebenfalls in der Raum-Init — FUN_800396fc -> FUN_8003ef6c -> `FUN_8003f0a0()`
(Decompilat: Schleife ueber 10 Faden-Plaetze, Handler bis Rueckgabe != 1), also VOR dem Aufbau im
ersten Present (`DAT_800b5457 = 1` am Ende von FUN_8001d600). Reihenfolge wie im Port — Dossier §8.4
bestaetigt.

## 4. Gegenproben (vorher/nachher, Variante 1151, Wiederbetreten, Laden, Nachbarraeume)

Alle mit demselben Original-Auswerter wie Abschnitt 3 (Bild gegen Original-Bildspeicher).

| Gegenprobe | Weg | Ergebnis | Beleg |
|---|---|---|---|
| **Vorher** (Stand vor dem Bau) | `re15_pc_g2.exe` des Bauers (08:35, vor Commit 20a7761d 09:53) als Kopie, derselbe NUTZER-Spielstand wie Lauf U | F100..F230: **131/131 Bilder AN**, kein einziges AUS — die Schrift blinkt vorher nicht (Nutzerbefund reproduziert); nachher (Lauf U) 20/20 | `G2_A1_eval_UALT_vorher.txt` vs `G2_A1_eval_U.txt` |
| **Falsche Eingaben** | Nutzerkarte, in Cut 2 zielen (R1) F100-147, darin Schuss (R1+Quadrat) F130-132 | Takt unberuehrt: AUS 119-138, AN 139-158, AUS 159-178 | `G2_A1_04_zielen_schiessen_inventar.png`, `G2_A1_eval_FE2.txt` |
| **Statusschirm** (Inventar) in einer AUS-Phase | START F178, Schirm F179-238, zu | nach dem Schirm weiter AUS (F239), `:= 1` erst F240, **kein Aufbau** (mg.log), danach 20/20 — Original Dirty := 2 @0x800466fc (Wert @0x800466dc) -> Sprung @0x80021bd4 ueber @0x80021c28 (selbst disassembliert) | wie oben, `G2_A1_mglog_fe2.txt` |
| **Nahansicht am Schreibtisch** (sub06: `Cut_chg 06`, `Message_on 01 ff ff`, `Evt_next`, `Cut_old`) in einer AUS-Phase | `RE15_FIRE_AOT=3@122#1150` (Aot_set slot 3 sce 3 -> sub06), Meldung per Quadrat weitergeblaettert/geschlossen | Skript steht waehrend der Meldung (keine 0x45 F124..F300), Rueckkehr F301 = Cut_old -> Aufbau -> sofort AN, `:= 1` F315 aendert nichts, AUS ab F335, dann 20/20 — Original: Cut_old = Dirty 1 @0x80040354 -> Aufbau @0x80021c28 | `G2_A1_eval_S6B.txt`, `G2_A1_mglog_s6b.txt` |
| **Speichern** am Schreibtisch (Memory-Card-Ablauf, Runde 33) | `RE15_GIVE_CARD=1`, Untersuchen per FIRE_AOT bzw. per echter Quadrat-Taste, 3x weiterblaettern, JA, Kartenbildschirm (CARD_AUTO) | Lauf SV2 (FIRE_AOT): nach dem Kartenbildschirm zurueck in Cut 2 (Cut_old): AN F403-436, dann 20/20 bis F620 — keine verlorenen Masken nach dem Kartenbildschirm. Lauf SVR (echte Quadrat-Taste F105): Rueckkehr in Cut 2 mit Aufbau F386 (mg.log), gespeichert camera_cut 2 | `G2_A1_eval_SV2.txt`, `G2_A1_mglog_sv2.txt`, `G2_A1_svr_speichern_echter_weg_log.txt` |
| **Laden** des eben gespeicherten Stands | Karte aus dem Speicherlauf mit echter Quadrat-Taste (gespeichert camera_cut 2), CONTINUE Slot 1 | 20/20 wie Lauf U | `G2_A1_eval_LDR_laden_nach_speichern.txt` |
| **Wiederbetreten** von Cut 2 | Lauf D2 (Abschnitt 3) | sofort AN, AUS erst beim naechsten `:= 0` | `G2_A1_02_…png` |
| **Raumvariante** ROOM1151 | Lauf E3 (Abschnitt 3); Daten selbst verglichen: sprite.pri Cut 0..8 (Offsets 0x500..0xBD4, Koepfe gleich), Cut-2-Sektion 0x264 Byte gleich, sub05 1150 @0x10B6 == 1151 @0x1094 (52 Byte), sub00 beider Raeume startet sub05, RDT[7] = 54 in beiden | 20/20, pixelgleich zum Original | `G2_A1_03_…png` |
| **Wanduhr** (Runde-30-Fehlerklasse "Zaehler je Monitorbild") | zwei Laeufe desselben Stands bis Bild 100 bzw. 700 (ohne Bilddumps) | 600 Spielbilder in 20 s (30 - 10 s, +-1 s) = 33,3 ms je Bild -> 20 Bilder = 0,67 s je Zustand; Original 40 VBlanks / 59,83 Hz = 0,669 s. Kein Monitor-Takt (SCD je Bild der 30-Hz-Kappung main.c Z.10840-10852, `scd_vm_tick` Z.5387) | Laeufe takt100/takt700 (Dauerzeilen) |
| **Nachbarverhalten ROOM3000 Zombie-Variante** | Karte ROOM3000 Cut 0, Spawnpunkt des Debug-Sprungs (-25968,-13901), Flag (4,9), `RE15_BEFUND_MARKE=60` | Opcode 0x45 je Bild: Gruppen 1/2/3 := 0 (9 + 20 + 14 = 43/43 Treffer), keine der 43 Leichen-Masken im Bild (Original laut DuckStation-Gegenprobe des Bauers G2_16 ebenso 0/43) | `G2_A1_05_raum3000_befundlog_masken.png` |

Zusatz-Zensus (Abschnitt 5.3): keine Kollision der neuen Sichtbarkeit mit den nachgezeichneten
R15M-Masken in irgendeinem der 2188 Cuts.

⛔ Fehlalarm, selbst aufgeklaert (kein Befund): ein erster Speicherlauf ueber den Messhaken
`RE15_FIRE_AOT` schrieb camera_cut **6** (Nahansicht) in den Spielstand; nach dem Laden klebte die
Kamera auf Cut 6. Ursache ist der MESSHAKEN, nicht das Spiel: `re15_aot_fire_slot`
(aot_common.c Z.855-881) startet das Unterskript direkt, der Latch `re15_savepoint_set_cut`
(aot_common.c Z.1513) sitzt nur im Scan-Pfad der Aktionstaste. Mit der echten Quadrat-Taste
gespeichert: camera_cut **2**, Laden -> Cut 2, blinkt 20/20. (Die alte exe verhaelt sich am Messhaken
gleich — kein Bezug zu Spur G.)

## 5. RE-Gate-Pruefung des Codes (Konstanten, Belege, Stubs)

### 5.1 Selbst disassembliert (re15_disasm.py gegen `info/Re1.5/PSX.EXE`, nicht aus dem Dossier uebernommen)

| Stelle | Instruktionen (selbst gelesen) | Code-Umsetzung | Urteil |
|---|---|---|---|
| Tabelle 0x45 | `table 0x800745bc` -> `0x800428d4` | `s_op_table[0x45] = op_col_chg_set` | stimmt |
| Handler 0x800428d4 | `800428ec lbu a0,1(v0)` / `800428f0 lbu a1,2(v0)` / `800428f4 jal 0x800396a8` / `800428f8 addiu a0,a0,1` / `80042900 ori v0,zero,0x1` / `80042904 addiu v1,v1,3` / `80042908 sw v1,28(s0)` | `re15_mg_setzen(pc[1]+1, pc[2]); pc += 3; return 1;` | stimmt |
| FUN_800396a8 | `800396b8 lbu a3,0(v0)` (RDT[0]) / `800396c0 lw v1,0x800b2584` / `800396c4 beq a3,zero` / `800396cc andi a0,a0,0xff` / `800396d0 lbu v0,1(v1)` / `800396d8 bne v0,a0` / `800396e0 sb a1,0(v1)` / `800396e4 sltu v0,a2,a3` / `800396ec addiu v1,v1,4` | Schleife i < Zahl, Byte1 == g1 -> Byte0 := wert (ganzes Byte) | stimmt |
| FUN_80039590 Test | `800395e8 lbu v0,0(s3)` / `800395f0 andi v0,v0,0x1` / `800395f4 beq v0,zero,0x80039668` | `re15_mg_sichtbar` = Byte0 & 1 | stimmt |
| FUN_800392d4 (ganz, 175 Instr.) | Cut aus `lh 0x800b0fe4` @0x800392e0, Sektion `lw t5,28(v0)` @0x8003931c; NULL `bne v1,-1` @0x8003932c -> `j 0x8003955c` mit `sb zero,0(a0)` @0x80039338; `srl t2,v1,16` @0x80039330 + `sb t2,0(a0)` @0x80039358; Loeschschleife `lbu a3,7(v0)` @0x8003936c, `sb zero,0(s5)` @0x80039378, `andi v0,a3,0xff`/`bne` @0x8003937c-80; Gruppe leer `beq v0,zero,0x80039540` @0x800393c0; je Maske `ori v0,v0,0x1`/`sb` @0x800393e0-e4, `addiu v0,a3,1`/`sb v0,-1(t1)` @0x800393e8-ec, `addiu s5,s5,4` @0x80039494; Innenschleife `andi v0,t2,0xff`/`sltu v0,v0,v1`(u16 Gruppenzahl) @0x8003950c-38 **ohne Ueberspring-Zweig** (jede Maske der Gruppe bekommt einen Record); Aussenschleife `addiu a3,a3,1` @0x80039544, `sltu v0,v0,t8` (t8 = kopf & 0xffff) @0x80039550 | `re15_mg_aufbauen`: gleiche Reihenfolge, Index = laufende Maske, Leer-Gruppen zaehlen mit, NULL -> Zahl 0 ohne Loeschen | stimmt |

Der Record-Index des Originals ist also die laufende Maske in Bau-Reihenfolge; `re15_pri_parse_section`
(pri_common.c Z.84-141) flacht in derselben Reihenfolge ohne Auslassung ab (Abbruch nur bei
Dateiende) — Renderindex i == Record i.

### 5.2 Konstanten im neuen Code und zitierte Haken-Adressen

* `masken_gruppen.c`: jede verhaltensbestimmende Zahl traegt eine selbst nachgepruefte Adresse
  (0xFFFFFFFF @0x80039328-2c, `>> 16 & 0xFF` @0x80039330/@0x80039358, `raw[7]` + 256-Fall
  @0x8003936c-84, `kopf & 0xFFFF` @0x80039344, Gruppe+1 @0x800393e8-ec, Byte0 |= 1 @0x800393e0-e4,
  op1+1 / op2 / pc+3 / Rueckgabe 1 @0x800428f0-04, Byte0 & 1 @0x800395f0-f4). Die einzige
  Port-Konstruktion (`re15_mg_sichtbar(i) = 1` fuer i >= Zahl) ist als solche gekennzeichnet und durch
  den R15M-Zensus (5.3) gedeckt. Uebrige Zahlen sind Grenzwaechter ohne Verhalten (`size < 8u`,
  `g < 256` als Schleifenschranke passend zum 8-Bit-Vergleich @0x8003954c-50).
* Zitierte Dirty-Adressen der Haken-Kommentare selbst disassembliert: `sb v0,21591(at)` (=
  0x800b5457) mit v0 = 1 @0x8001daec (Raumlader), @0x8001d5c8 (Laden), @0x80021514, @0x800402f4
  (Cut_chg), @0x80040354 (Cut_old); mit v0 = 2 @0x800466fc (Wert `ori v0,zero,0x2` im Verzoegerungsplatz
  @0x800466dc) und @0x80026634 (Wert @0x8002661c); FUN_80021bbc @0x80021bc4 `lbu v1,Dirty` /
  @0x80021bc8 `ori v0,zero,0x2` / @0x80021bd4 `beq v1,v0,0x80021df8`; @0x80021c28 `jal 0x800392d4`.
  Alle stimmen.
* Commit-Text 20a7761d traegt dieselben Adressen. Kein "interim/plausibel/tunable".
* Beobachtet, nicht bewertet: vor dem Aufbau-Aufruf steht im Original ein weiteres Gate
  (@0x80021c20 `bne v0,zero,0x80021c34`, DAT_800aca38 & 0x100000, dasselbe Bit wie beim Zeichnen
  @0x8001ce3c-4c). Der Port bildet es weder beim Zeichnen noch beim Aufbau ab. In 89 sauberen
  Savestates ist das Bit nur in den zwei Todes-Staenden gesetzt (`mzd_death_cmd7_youdied.sav`,
  `mzd_stage1_combat_death.sav`, aca38 = 0x44d24000), in keinem der uebrigen 87 (darunter die
  sieben ROOM1150-Staende der Ermittlung `orig_1150_a`/`c2_w*`) — waehrend des Blinkens also nicht
  gesetzt; fuer das Zeichnen vorbestehend, daher kein Mangel dieser Spur.

### 5.3 R15M-Zensus (Port-Konstruktion geprueft)

`re15_port/tools/r34n_g/abn1_r15m_zensus.py` (Beleg `G2_A1_r15m_zensus.txt`): der Port laedt
nachgezeichnete R15M-Masken, sobald `re15_pri_parse_section` 0 liefert — das waere auch bei einer
Nicht-NULL-Sektion mit group_count 0 / decl 0 / > 256 der Fall, bei der `re15_mg_aufbauen` eine
Zahl > 0 setzt und Byte0 loescht (dann waeren R15M-Masken unsichtbar). Ergebnis ueber alle 2188 Cuts
aller RDTs: 1708 NULL, 480 regulaer, **kein** weiterer Fall; 323 Cuts mit R15M, **0 Konflikte**.

### 5.4 Nebenstelle ohne Sichtbarkeitsbit: befund.log (Nutzer-Werkzeug F9)

`platform/pc/main.c` schreibt das immer aktive `befund.log` (Z.5923 ff.): je 15 Bilder die Masken,
die den Figurenkasten beruehren, mit Urteil `DECKT|frei` (Z.6083-6091), und je F9-Marke "alle N
Masken dieses Winkels" (Z.6103-6113) — beides aus `re15_render_pc_debug_pri_rects()` (render_pc.c
Z.2121-2130) = ALLE geladenen Rechtecke, **ohne** `re15_mg_sichtbar()`. Seit dem Bau zeichnet der
Renderer aber nur noch die sichtbaren. Gemessen (Lauf z3000, Marke F60): mg.log `Gruppe 1/2/3 := 0
(9/20/14 Treffer, Zahl 43)` in jedem Bild, im Bild keine der 43 Masken — das befund.log meldet
`masken=43` und listet alle 43 als Masken dieses Winkels (`G2_A1_05_befundlog_auszug.txt`,
`G2_A1_05_raum3000_befundlog_masken.png`). Aus dem Code (nicht im Lauf beobachtet — die Figur
stand dort an keiner Maske, Zeile "keine Maske am Koerper") folgt ausserdem: steht die Figur an
einer ausgeblendeten Maske, lautet das Urteil `DECKT` (Z.6091 `re15_pri_mask_occludes` ohne
Sichtbarkeitspruefung), obwohl nichts verdeckt. Der Kommentar Z.6097-6102 verspricht ausdruecklich
"was der Zeichner WIRKLICH geladen hat". Spielbild unberuehrt -> kosmetisch, aber es verfaelscht das
Messwerkzeug des Nutzers in den acht Raeumen mit Opcode 0x45 (Mangel M1).

## 6. Maengelliste

| Nr | Schwere | Mangel | Beleg |
|---|---|---|---|
| M1 | kosmetisch | Das immer aktive `befund.log` (F9-Werkzeug des Nutzers) kennt das neue Sichtbarkeitsbit nicht: es listet ausgeblendete Masken als Masken des Winkels (gemessen: `masken=43`, alle 43 gelistet, 0 gezeichnet) und wuerde laut Code `DECKT` urteilen, obwohl der Renderer sie nicht zeichnet (betrifft die acht Raeume mit Opcode 0x45; in ROOM3000/3010 Zombie-Variante alle Leichen-Masken). Kein Spielbild betroffen. Vorschlag: im Befund-Block `re15_mg_sichtbar(q)` mitschreiben (z. B. `AUS` statt `DECKT`) — eine Haken-Zeile in main.c. | main.c Z.6083-6091, 6103-6113; render_pc.c Z.2121-2130; Lauf z3000: `G2_A1_05_befundlog_auszug.txt`, `G2_A1_05_raum3000_befundlog_masken.png` |

Keine blocker, keine wesentlichen Maengel. Nicht als Mangel gewertet (Begruendung oben): PSX-Ziel
ungebaut (bekannte Luecke, beide Zeilen gelesen und syntaktisch schluessig — `re15_test_rdt`/
`re15_test_rdt_ok` sind in platform/psx/main.c Z.66-67 deklariert); Optionsschirm SELECT+START im
Port nicht vorhanden (Dossier §9.8/2); Gate aca38 & 0x100000 (5.2); ein Bild Versatz des
Sleep-Rests an der Menue-Grenze (`:= 1` F240 statt F239 nach einem Schirm F179-238; beim Bauer
gleich, F191 statt F190: im Bild des START-Drucks laeuft kein SCD-Takt, der Schirm erscheint erst
im Folgebild) gehoert zur Menue-Einfrier-Logik des Ports (menu_common.c
`re15_menu_gameplay_frozen`), nicht zu dieser Spur, und ist mit 33 ms nicht wahrnehmbar. Ob das
Original im START-Bild ebenfalls keinen SCD-Takt hat (START-Pruefung @0x8001cd24-60 liegt vor dem
SCD-Laeufer @0x8001cdec), ist hier NICHT geprueft — offen fuer die Menue-Spur, nicht fuer G.
