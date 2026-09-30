# Spur G2 (ROOM1150 Irons Office, blinkende Schrift) — ABNAHME 1

Stufe: ABNAHME 1 (unabhaengig, nach dem Bau). Prueferin hat den Code nicht geschrieben.
Auftrag: versuchen zu WIDERLEGEN, dass Spur G fertig ist.

Status: IN ARBEIT (Geruest angelegt)

## 0. Urteil

(offen)

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

(offen)

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

### 5.2 Konstanten im neuen Code

(folgt)

## 6. Maengelliste

(offen)
