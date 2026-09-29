# Runde 31 — Stufe 4: Tueren-Anschluss bauen (Dossier)

Zweig `r31/tueren`, Arbeitsbaum `.claude/worktrees/r31_tueren`.
Grundlage: `tueren_03_zuordnung.md` + `tueren_03/zuordnung.json`, `tueren_02_re2.md` §1-§6,
`tueren_01_zensus.md` §9.1, `analysis/tor_1170/09_sequenz.md`.

## Kurz — Antwort an den Nutzer

- **184 Tuerseiten (83 Tueren + die 3 gleichen Seiten der einseitigen Tueren T048/T078/T161) spielen beim
  Durchgehen die RE2-Tuersequenz ihres Archivs mit dessen Ton.** Keine abgedeckte Seite ist weggelassen:
  auch S311 (DOOR2D V4) ist gebaut, weil der Simulator belegt, dass V4 hinauf zeigt (Abschnitt 1.3).
- **Nicht so abgedeckt: 61 von 144 physischen Tueren (115 Tuerseiten)** - Zaehlung und Gruende aus Stufe 3
  (tueren_03_zuordnung.md "Kurz"): 45 aehnlich, 3 einseitig (die andere Seite), 5 Aufzug-Einstiege, 3 keine Tuer,
  1 Tor ROOM1170 (eigene Sequenz, gebaut in v0.8.16), 4 inert/unsichtbar. Diese Tueren laufen wie bisher (RE1.5-Uebergang).
- Griffe: an T027 (Druecker statt Knauf) und T033/T034 (langer Stangengriff statt Knauf) zeichnet die Sequenz den
  Griff des Spender-Archivs (DOOR07 bzw. DOOR04) - PORT-WAHL, RE2 tauscht Griffe nur archivintern.
- Nebenbefund behoben: die zwei Viereck-Tueren ROOM4030/4031 fuehrten ins Leere; sie fuehren jetzt nach ROOM4040/4080.

## Stand

- [x] 0 Viereck-Tueren (40-B-Door_aot_set) — Commit 3398f601, Riegel `unit_r31_viereck`
- [x] 1 Daten (re2_tuer_tabelle.inc, 24 Archive nach shared_assets/RE2/DOOR, tuer_zuordnung.inc)
- [x] 2 Maschine (0x34/0x35/0x3D, 0x8A-0x8C) — 43/43 Archiv-Varianten gleich dem Simulator
- [x] 3 Laeufer + Ton (Tonteil je Archiv, Schliesston ueberlebt den Raumwechsel)
- [x] 4 Griff-Tausch T027/T033/T034
- [x] 5 Anbindung Kreuz-Raum-Tueren (+ Zwischensequenz-Sperre)
- [x] 6 Pruefhaken RE15_TUER_SEITE + Kontaktbogen (184 Seiten, 16 Boegen)
- [x] 7 Paket-Gate shared_assets/RE2/DOOR/*.DO2
- [x] 8 Riegel r31_tueren (4 Tests)
- [x] 9 Echtlauf 5 Tueren (normal, Griff-Tausch, Leiter, Viereck, Aufzug) + Suite

## 0. Viereck-Tueren ROOM4030/4031

**Messung vorher.** `re15_port/tools/engine_tueren.txt` (vom Port selbst erzeugt, `test_map_uebergang.c`)
Zeilen 184/185: `4030 BF0A0 ...` und `4030 C5F00 ...` - der Port las die beiden 40-B-Saetze mit dem
32-B-Schema (Ziel-Stage/Raum aus pc[22]/pc[23] = `be 0a` bzw. `c4 f0`) und stellte Tueren ins Leere auf.
Satzbytes selbst gelesen (ROOM4030.RDT == ROOM4031.RDT an beiden Stellen):

```
@0x47E 3b 01 02 b1 00 00 | 82 a1 6e 9c ae 9d 40 98 fa 97 7c 9d 5e 9d 86 a2 | be 0a 00 00 86 24 00 06 | 03 04 0c 00 | 00..
       Slot 1, sat 0xB1, Band 0, Punkte (-24190,-25490) (-25170,-26560) (-26630,-25220) (-25250,-23930)
       Nutzlast: Lage (2750,0,9350) Richtung 1536, Stage 3 Raum 04 Cut 12 -> ROOM4040 (S225, T116)
@0x4A6 3b 02 02 b1 00 00 | f6 af f0 a1 38 b4 12 9e fc ae a4 98 b0 aa 68 9d | c4 f0 00 00 34 08 00 02 | 03 08 08 00 | 00..
       Slot 2, Punkte (-20490,-24080) (-19400,-25070) (-20740,-26460) (-21840,-25240)
       Nutzlast: Lage (-3900,0,2100) Richtung 512, Stage 3 Raum 08 Cut 8 -> ROOM4080 (S226, T117)
```

**Original (RE1.5 PSX.EXE, selbst disassembliert).**
- Installer `LAB_800405bc`: `80040618 lbu v0,3(v1)` / `80040620 andi v0,v0,0x80` / `80040630 addiu v0,v1,40`
  (sonst `80040634 addiu v0,v1,32`) - sat Bit 0x80 = 40-B-Form.
- Scan `FUN_80042bac`: Objektversatz `80042dc0 sw zero,40(sp)` (pc[5] & 0x80 = 0 bei allen Tueren),
  Viereck-Kopie `80042dd8..80042e5c` (lhu 4/6/8/10/12/14/16/18(s0) + Versatz -> sh 44..58(sp)), Wahl
  `80042f04 andi v0,v1,0x80` -> `80042f10 jal 0x80014368` (a1 = sp+40) statt `80042f20 jal 0x80042b64`;
  Nutzlast `80042f8c jalr v0` mit Delay `80042f90 addiu a0,s0,20` (= pc+22), Rechteckform `80042fb8 addiu a0,s0,12`.
- Trefftest `FUN_80014368` (a0 = Punkt x@0 z@8, a1+4.. = Punkte):
  ```
  80014368 lh t5,4(a1)   8001436c lh t0,6(a1)        ; x0 z0
  80014378 subu t1,v0,t0 8001437c subu t2,v1,t5      ; pz-z0, x1-x0
  80014380 mult t2,t1 -> a0                          ; (x1-x0)(pz-z0)
  80014390 subu a2,v0,t5 80014394 subu a3,v1,t0      ; px-x0, z1-z0
  80014398 mult a3,a2 -> v0                          ; (z1-z0)(px-x0)
  800143ac slt v0,v0,a0  800143b0 bne -> 0
  800143b8 mult t3,t1 / 800143c0 mult t4,a2          ; (x3-x0)(pz-z0) slt (z3-z0)(px-x0)
  800143c8 slt v0,v0,v1  800143cc bne -> 0
  800143d4..800143f8  Bezug auf Ecke 2 (pz-z2, x1-x2, px-x2, z1-z2), 80014400 subu t3,t3,v0 (x3-x2)
  80014408 slt a0,a0,v0  8001440c bne -> 0           ; (x1-x2)(pz-z2) < (z1-z2)(px-x2)
  80014410 subu t4,t4,v1 (z3-z2)
  80014424 slt v0,v0,v1  80014428 beq -> 1 (Delay 8001442c ori v0,zero,1), sonst 80014430 -> 0
  ```
  Feste Umlaufrichtung, 32-Bit-Produkte (mflo), vorzeichenbehafteter Vergleich. Der Port-Test
  `re15_aot_point_in_quad` (Umlaufrichtung frei) ist NICHT dieser - fuer Tuersaetze gibt es jetzt
  `re15_aot_point_in_quad_fun80014368` (aot_common.c), Befehl fuer Befehl.

**Bau.** `scd_vm.c op_door_aot_set`: Viereck-Punkte pc+6..21, Nutzlast ab pc+22 (sonst pc+14), `has_quad`
und die Punkte je Satz gesetzt (auch zurueckgesetzt fuer Rechtecksaetze), Huellrechteck fuer die Port-Stellen,
die mit Mitte/Halbmass rechnen. `aot_common.c`: der Tuer-Vorwaertstest nimmt fuer Viereck-Tueren den neuen Trefftest;
der Kommentar "non-door scan artifacts" ist korrigiert.

**Messung nachher** (`unit_r31_viereck`, Spielschritt-Geruest wie `probe_r30_tuer_verschlossen.c`: Raum booten,
einschwingen, Standplatz vor der Tuer suchen, QUADRAT):

```
ROOM4030 Slot 1: Typ 1 Viereck 1 Punkte gleich Ziel ROOM4040 Lage (2750,0,9350) Richtung 1536 Cut 12
  Durchgang: Standplatz gefunden, Raumwechsel 1 -> ROOM4040 Lage (2750,0,9350) Cut 12
ROOM4030 Slot 2: Typ 1 Viereck 1 Punkte gleich Ziel ROOM4080 Lage (-3900,0,2100) Richtung 512 Cut 8
  Durchgang: Standplatz gefunden, Raumwechsel 1 -> ROOM4080 Lage (-3900,0,2100) Cut 8
ROOM4031 Slot 1 -> ROOM4041, Slot 2 -> ROOM4081 (ebenso)
```
Im Echtlauf (Abschnitt 9) fuehrt S225 nach ROOM4040 mit DOOR25-Sequenz. Suite nach Schritt 0: 406/406.

## 1. Daten

- `engine/src/gen/re2_tuer_tabelle.inc` = `tools/tueren/re2_tuer_tabelle.py --inc` (EXE @0x8009a520, 55 x 12 B,
  je Zeile Adresse + Rohbytes; Pruefung 55/55: Sektor*0x800 + Modellteil == Dateigroesse, Pruefsumme Ton).
- 24 Archive UNVERAENDERT nach `shared_assets/RE2/DOOR/` (`--kopiere-nach ... --nur 04,06,07,09,0A,13,15,16,19,1A,1B,1C,
  1D,1E,23,24,25,26,27,29,2A,2D,2E,31`, sha1 beim Kopieren geprueft; 1,6 MB). `.gitattributes`: `-text`.
- Generator `tools/tueren/tuer_zuordnung_gen.py` -> `engine/src/gen/tuer_zuordnung.inc`: **368 Zeilen = 184 Seiten x
  2 Raumdateien (xxx0/xxx1)**. Schluessel Raum + Rechteck (Mitte/Halbmass mit C-Division wie op_door_aot_set) bzw.
  vier Punkte + Band; je Zeile im Kommentar Seite, Tuer, RDT-Offset(e) + Skript + Slot (nur Herkunft), Archiv, Variante,
  Herkunft der Variante, Griff-Tausch. Zeilenkopf: "PORT-WAHL aus dem Bildvergleich, KEINE Original-Adresse".
  18 Zeilen teilen ihre Flaeche mit einer anderen Seite (gleiche Tuer, Szenario-Saetze mit anderem Ziel, z. B. S117/S118
  ROOM1230) - die Wahl ist dann gleich (der Generator bricht ab, wenn nicht).
- Tabelle des Tors (door_seq_tor1170.c, 4 Eintraege) unveraendert und mit Vorrang.
- Engine: `door_seq_zuordnung.c` (Nachschlagen, Seiten-Zugriff fuer den Pruefhaken, Griff-Tausch-Satz, Archivtabelle);
  auf der PSX nicht gelinkt (`#ifndef RE15_PLATFORM_PSX`), dort liefert die RE2-Tabelle nichts.

### 1.3 DOOR2D: welches Skript zeigt hinauf? (S311 gebaut)

DOOR2D V0/2/4 = Skript 1 (301 Bilder), V1/3/5 = Skript 2 (451). Simulator (`tuerkatalog.VM`): Objekt 2 (Schachtgeruest,
Mesh 2, Flag 0x40, Kinder 3..5) bewegt sich je Bild in **V4 um (-200,+125)** (x = von der Kamera weg, y = nach unten im
Bild) und in **V5 um (+200,-125)** (auf die Kamera zu, nach oben); die Buehne (Objekt 0) steht. Faehrt das Geruest nach
unten durchs Bild, steigt die Kamera: **V4 = hinauf, V5 = hinab** (148 bzw. 238 solcher Schritte, dazwischen Wiederholsprung
um 11800/7375). Das passt zum einzigen RE2-Satz ROOM6170 V5 -> ROOM7000 (hinab zum Zug). S288 (ROOM50B0, hinab) V5 und
S311 (ROOM6000 GATE PLATFORM -> Hangar, hinauf) V4 sind damit belegt; Kontaktbogen 14/15 zeigt beide.

## 2. Maschine (engine/src/door_seq_common.c)

Welche Sonder-Opcodes die zugeordneten Archive brauchen (Simulator ueber alle 43 benutzten Varianten): **nur DOOR2D**
0x34/0x35/0x3D und 0x8A/0x8B/0x8C; Zeichen-Flags 0x2000/0x4000/0x0100 braucht keins (Flags-ODER je Variante gemessen,
hoechstes 0x0ef1) -> nicht gebaut.

- Member_set 0x34 (Handler 0x80055c00: `80055c14 lw a0,340(s0)` Arbeitsobjekt, `80055c18 lbu a1,1(v0)` Feld,
  `80055c1c lh a2,2(v0)` Wert, `80055c30 addiu v0,v0,4`), Member_set2 0x35 (0x80055c50: `80055c7c lh a2,0x800d47ec[var]`,
  `80055c90 addiu v0,v0,3`), Setter `0x80055cb0` (`sltiu v0,a1,0x2c`, Sprungtabelle @0x80011228, Befehl im Delay-Slot):
  Feld 11/12/13 = `sw a2,56/60/64(a0)` @0x80055d30/38/40; dazu die Felder, die der Port als Objektfeld fuehrt
  (0 @0x80055cd8, 7 @0x80055d10, 14-16 @0x80055d48/50/58, 27/28 @0x80055db0/b8, 29 @0x80055dc0); anderes Feld -> Notiz.
- Member_copy 0x3D (0x80055e38: `lb` Variable/Feld @0x80055e4c/50, `addiu v0,v0,3` @0x80055e54, Getter 0x80055f50,
  Tabelle @0x800112f8, Ablage `sh v0,0x800d47ec[..]` @0x80055e70): Feld 11/12/13 = `lw v0,56/60/64(a0)` @0x80055ffc/56008/56014.
- 0x8A/0x8B/0x8C: Handler 0x80059348/0x80059394/0x800593e4 rufen die Vibration (0x8003947c/0x80039514/0x800395b8), Rueckgabe 1,
  Vorschub 6/6/8 (@0x80059378/@0x800593c8/@0x8005941c) - im Port nur der Vorschub, nicht mehr `default:` (das hatte das
  Skript beendet).
- **Pruefung jeder Variante:** `tools/tueren/tuer_maschine_referenz.py` liest aus Skript 0 den Switch auf var 0x0C und
  prueft, dass die Variante als Case (nicht Default) verteilt wird (43/43), und schreibt je Bild eine FNV-Summe ueber alle
  10 Objekte. `unit_r31_maschine`: **43 von 43 Archiv-Varianten Bild fuer Bild gleich**, Se_on-Bilder, Bildzahl,
  Schliesston-Merker gleich, Notizen 0.

## 3. Laeufer + Ton (platform/pc)

- `door_scene_pc.c`: Archiv `shared_assets/RE2/DOOR/DOORxx.DO2` ueber `re15_pc_read_re2`, Aufteilung nach der Tabelle
  @0x8009a520: Modellteil = Datei + Sektor*0x800 (FUN_80015064 @0x800150b0/@0x800150f4), Tonteil = Datei[0..Tonteil)
  (FUN_80014cd0 @0x80014d94); Datei gegen Tabelle geprueft. `re15_door_seq_start(..., variante, bit7 ? 0x80 : 0, tuer_nr)`
  mit var 14 = Payload+13 & 0x80 (@0x80013e84/8c), var 15 = Payload+12 = Archivnummer (@0x80013e90/98; Lader
  @0x80015088 lbu v1,12(v0)). Bit 7 ist fuer alle 184 Seiten 0 (kein DOOR01/05 zugeordnet). DOOR28-Sonderfall nicht gebaut
  (nicht zugeordnet).
- `audio_pc.c re15_audio_re2_tuer_laden/_se`: Tonteil wie TORSE (VH @0x10, Nachspann @0xC30, VB @0xC38; 55/55 laut
  `re2_tuer_ton.py`), RE2-Pegelgesetz (`s_se_pegel_re2`, @0x80083760); gleiche Tonteil-Bytes (Tonfamilie) werden nicht neu
  dekodiert; beim Wechsel der Bank werden Stimmen auf der alten Bank beendet (kein Zeiger auf freigegebenes PCM).
- **Schliesston ueberlebt den Raumwechsel:** RE2 `FUN_80059e54 @0x80059e90 jal 0x800597a4`, dort Stimmen 23..0
  (`800597ac addiu s1,zero,24` / `800597d8 addiu s1,s1,-1`), Key-Off nur fuer SPU-Adressen 0x14441..0x3DC4F
  (`800597bc/c0` s3 = 0xfffebbbf, `800597c8/cc` s2 = 0x2980e, `80059800 addu`, `80059804 sltu`, `80059808 bne`,
  `80059810 jal 0x80079498`); Tuerbank bei 0x3DC50 (`80014f08 lui a2,0x3` / `80014f18 ori a2,a2,0xdc50`, SsVabOpenHeadSticky
  `80014f14`). Port: `re15_audio_load_room_banks` laesst Stimmen, deren PCM aus Tor- oder Tuerbank stammt, samt Prioritaet
  und Vormerkung stehen. `unit_r31_tuer_ton` (echtes audio_pc.c, RE15_AUDIO_CAP_SYNC): DOOR13 Ton 1 -> SE-Stimme 7 an, nach
  `re15_audio_load_room_banks()` an, zwei Bilder spaeter an; Bankwechsel auf DOOR25 beendet sie.
- **Gemessen (echte exe, RE15_AUDIO_CAP_SYNC + RE15_SE_DEBUG, alle 184 Seiten):** 184/184 "Ton geladen", 394 Stimmen,
  0 Gate-Verwerfungen. Je Archiv (Se_on, Door_exit, Tonhoehe): 06/09/13/1C/25 16397 Hz 1+1; 15/1A/23 13641 Hz 1+1;
  19 9226 Hz; 1B 10400+11025 Hz; 1D 10142 Hz; 26/31 6815 Hz; 27 7041 Hz; 0A/1E/24/29/2A/2E 11025 Hz (alle 1+1);
  **16 (Leiter) 4 Se_on, kein Door_exit; 2D Skript-Se_on 0 und 1, kein Door_exit** - wie tueren_02_re2.md Abschnitt 4.
  Ein Audiogeraet gibt es auf dieser Maschine nicht (`SDL_OpenAudioDevice failed: WASAPI ...`) - abgehoert ist nichts.

## 4. Griff-Tausch (PORT-WAHL)

RE2 tauscht Griffe nur archivintern per Bit 7 (DOOR01/05, tueren_02_re2.md 2.5). Fuer die drei Tueren, deren gemalter
Griff eine andere FORM hat als das Archiv, zeichnet der Port das Griff-Objekt des Archivs (Kind des Blatts, Mesh 1) mit dem
Griff-Mesh des Spenders:
- am Anhaengepunkt des ARCHIVS (Objektlage aus dessen Skript: DOOR13 (130,-3364,-3342), DOOR09 (130,-3224,-3372)),
- mit dem TIM des Spenders in TIM-Platz 25 (Beschlagstreifen v 219..255 + dessen CLUT),
- Grund-Drehung des Spendergriffs vorn/hinten (DOOR07 (0,0,0)/(2048,2048,0), DOOR04 (0,0,0)/(0,2048,0)), Zeitverlauf der
  Griffbewegung des Archivs, Ausschlag des Spenders (Knauf 1500 -> Druecker -702; Stange 0) - Werte [SIM] vom Generator in
  `gen/tuer_zuordnung.inc` (`re15_griff_tausche`).
Seiten: S042 (T027, DOOR13 V1 <- DOOR07), S058/S060 (T033), S061/S065 (T034) (DOOR09 <- DOOR04).
Bild: `tueren_belege/t4_griff_tausch.jpg` (angesehen: flacher Druecker rechts, kippt beim Aufziehen; langer
Messing-Stangengriff links bzw. rechts, steht).

## 5. Anbindung

- `aot_common.c tuer_sequenz_anfragen`: Kreuz-Raum-Zweig (vor `re15_room_request_change`) UND Selbst-Tuer-Zweig fragen
  dieselbe Zuordnung (Tor zuerst, dann RE2-Tabelle; Schluessel Raum + Rechteck/Viereck + Band).
- `game_step_common.c`: Sequenz im Spielschritt, also vor `re15_room_apply_pending`; die RE1.5-Einblendung nur, wenn kein
  Raumwechsel ansteht (`!g_room_change.pending`) - sonst folgt sie in main.c nach dem Laden im NEUEN Raum. RE2
  `FUN_80026b7c`: Door_main `@0x80026bfc`, Zielraum `@0x80026e1c`, Warten auf das Tuer-Ende `@0x80026e28..54`, Einblenden danach.
- `main.c`: steht vor `re15_room_apply_pending` noch eine Anfrage (Tuer ausserhalb des Spielschritts gefeuert, z. B.
  RE15_FIRE_AOT), wird sie dort gespielt.
- **Zwischensequenz-Sperre (PORT-WAHL):** feuert eine RE2-Tabellen-Tuer waehrend `player_mode == 2` oder schliessender
  Balken, gibt es keine Sequenz. Der Scan sperrt Tueren in genau dieser Lage (`in_cinematic` in re15_aot_scan), gefeuert hat
  also ein Skript. Gemessen: ROOM4000-Eintrittsszene - eine Figur laeuft per Plc_dest zur Tuer, dann feuert Slot 1 (S216)
  -> ROOM4010; ohne Sperre spielte mitten in der Szene die DOOR29-Sequenz. Log jetzt:
  `[aot] Tuer S216 in einer Zwischensequenz gefeuert -> RE1.5-Uebergang ohne Sequenz`. Das Tor ist davon ausgenommen.
- Nicht angefasst: Tor, verschlossene Tueren (Anfrage nur in aot_fire_door), Null-Rechteck-/Intro-Uebergaenge (nicht in der
  Tabelle), Aufzugskabine (ROOM4020/1080-Ausgaenge sind Null-Rechtecke; `unit_fahrstuhl_4020_etagen` und `_1080_etagen` gruen),
  nicht abgedeckte Tueren. PSX: Tabelle leer, Anfrage verfaellt, Ton-Stubs in audio_psx.c.

## 6. Pruefhaken + Kontaktbogen

- `RE15_TUER_SEITE=<Sxxx[,Syyy]|ALLE>` (main.c, direkt nach dem Start): spielt die Sequenz jeder genannten Seite mit
  Archiv/Variante/Griff-Tausch aus der Tabelle und beendet. `RE15_TUER_BOGEN=<dir>` schreibt je Seite `Sxxx_anfang.ppm`
  (vor der ersten Bewegung, Bild 16..20) und `Sxxx_mitte.ppm` (halbe aufsummierte Drehung von Objekt 0; unter 256
  Drehung die halbe Lage y/z) - Ruecklesen vor dem Present wie RE15_FRAMEDUMP, beschleunigter Renderer.
  `RE15_TUER_SCHNELL=1` ohne VSync-Takt (184 Seiten in 44 s).
- `tools/tueren/tuer_kontaktbogen.py [--erzeugen]` -> `build/r31_tueren/t4/kontaktbogen_01..16.png`, verkleinert
  `tueren_belege/t4_kontaktbogen_01..16.jpg`: je Seite RE1.5-Ausschnitt (T1, groesster Umriss) | Anfang | Mitte + Wahl.
- **Angesehen: Boegen 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 15, 16 (~150 Seiten)**, darin alle 22 Sequenz-Archive
  (06 09 0A 13 15 16 19 1A 1B 1C 1D 1E 23 24 25 26 27 29 2A 2D 2E 31), beide Varianten (V0/V1, V2/V3, V4/V5), alle
  Griff-Tausch-Seiten (S042, S058, S060, S061, S065). Befund: Griffseite im Anfangsbild = gemalte Seite (V0 links, V1 rechts),
  Oeffnungsrichtung wie T2 1.1 (V0 weg, V1 hin; Schiebetueren 19/2A in ihre Richtung; 25 hebt; 16 Leiter; 1E Klappe kippt;
  Doppeltueren 1B/1D beide Fluegel; Schott 26/31 ein-/zweiteilig). Farbabweichungen (DOOR13 dunkler, DOOR19/26 anders)
  sind die in Stufe 3 bewusst zugelassenen (RE2 malt dieselbe Tuer selbst so).

## 7. Paket

`release/make_package.sh check_tree`: jede `shared_assets/RE2/DOOR/*.DO2` des Quellbaums muss im Paket liegen und darf nicht
leer sein (sonst `die`). Gegenprobe an einem Schein-Paketordner: vollstaendig -> "Tuerarchive im Paket: 24"; DOOR13 geleert ->
`DIE: RE2-Asset fehlt/leer im Paket: shared_assets/RE2/DOOR/DOOR13.DO2`. (Kopiert wird shared_assets/RE2 schon heute ganz.)

## 8. Riegel (tests/unit/probes/r31_tueren.cmake)

| Test | prueft |
|---|---|
| `unit_r31_viereck` | Viereck-Saetze: Installation + Trefftest FUN_80014368 + Durchgang nach ROOM4040/4041/4080/4081 |
| `unit_r31_maschine` | 43 benutzte Archiv-Varianten Bild fuer Bild gegen den Simulator, Variante verteilt, Archiv-Dateien gegen @0x8009a520 |
| `unit_r31_zuordnung` | 368 Zeilen treffen ihren echten Door_aot_set (RDT-Offset); jede findet ihre Wahl; Tor unveraendert; Null-Rechteck und S226 nichts, S225 DOOR25; im Spielschritt stellt ROOM1130 S060 (DOOR09 V0, Spender DOOR04) die Anfrage und der Raumwechsel steht danach noch an; eine nicht abgedeckte Tuer ROOM1000 -> ROOM1050 fragt nichts an |
| `unit_r31_tuer_ton` | Schliesston (SE-Stimme 7) ueberlebt `re15_audio_load_room_banks` (echtes audio_pc.c) |

RE15_MIN_TESTS 405 -> 409.

## 9. Echtlauf (echte exe, beschleunigter Renderer)

`tools/tueren/tuer_echtlauf.sh <ziel> <raum> <slot> <zielraum>`: Titel-Vorlauf, `RE15_DEBUG_JUMP=<raum>@120`,
`RE15_FIRE_AOT=<slot>@500#<raum>` (derselbe aot_fire_door wie das Hineinlaufen), `RE15_FRAMEDUMP` fuer alten/neuen Raum,
`RE15_TUER_SERIE` fuer die Sequenzbilder, `RE15_AUDIO_CAP_SYNC` + `RE15_SE_DEBUG=1`. FALLE: `g_engine.frame_count` beginnt beim
Raumwechsel wieder bei 0 (main.c "Frame-Cap des Handoffs") - die FRAMEDUMP-Serie wird darum nur bis zum Feuerbild gelegt und
der Lauf endet am Bild 60 des Zielraums. Ablaufbogen `tools/tueren/tuer_echtlauf_bogen.py` ->
`tueren_belege/t4_echtlauf_bogen.jpg` (angesehen):

| Lauf | Tuer | Ablauf im Bild | Log |
|---|---|---|---|
| S058 | ROOM1120 -> 1130, DOOR09 V1 + Griff DOOR04 | gruener Raum 1120 -> abgedunkelt -> Tuer mit Stangengriff kommt auf -> schwarz -> Flurzimmer 1130 blendet ein (F0 schwarz, F4 dunkel, F8 hell) | Se_on Bild 70 (Stimme 6), Door_exit (Stimme 7), `room1130.rdt` |
| S133 | ROOM2000 -> 1260, DOOR16 V4 (Leiter hinauf) | Kanal 2000 -> Leiter -> 1260 | 4 Se_on (110/165/220/275), kein Door_exit |
| S225 | ROOM4030 -> 4040, DOOR25 V0 (Viereck-Tuer) | Schacht 4030 -> Hubtuer faehrt hoch -> 4040 | Se_on 110, Door_exit |
| S017 | ROOM1050 -> 1030, DOOR13 V0 | Raum 1050 -> Tuer geht auf -> 1030 | Se_on 60, Door_exit |
| S260 | ROOM5000 -> 4020 (Aufzugskabine), DOOR27 V0 | Flur 5000 -> Schiebetuer -> Kabine 4020 blendet ein | Se_on 70, Door_exit; Tuer nur mit Strom-Flag (3,78) aktiv (`Ck(3,0x4e)` vor dem Satz @0x8A4) -> `RE15_SET_FLAG_AT=3:78@110` |

Die Fahrt selbst (Kabinen-Ausgaenge = Null-Rechteck-Tueren, keine Sequenz) ist nicht per exe gefahren; die ganzen Wege
aller drei Etagen pinnen `unit_fahrstuhl_4020_etagen`/`_1080_etagen` (Suite gruen).

## Offen

1. Ton nicht abgehoert (kein Audiogeraet); gemessen sind Stimme, Tonhoehe, Bild und das Ueberleben im Riegel.
2. Die Zwischensequenz-Sperre ist eine PORT-WAHL (RE2 spielt auch per Skript gefeuerte Tueren, dort tragen solche Saetze
   aber meist objektlose Archive). Wer Skript-Tueren MIT Sequenz will: Sperre in `tuer_sequenz_anfragen` entfernen.
3. Griff-Tausch (T027/T033/T034) ist PORT-WAHL (Bewegungsverlauf des Archivs mit dem Ausschlag des Spenders).
4. 54 der 184 Seiten stellt die Engine beim Betreten nicht selbst auf (Unterskripte, T1) - im Echtlauf nicht einzeln gefahren;
   die Tabelle trifft sie ueber Raum + Flaeche + Band, sobald aot_fire_door sie ausloest (unit_r31_zuordnung prueft alle 368
   Zeilen gegen ihre Saetze).
5. PSX: keine Sequenz (Stub), wie beim Tor.
6. Die Leiterregel, T131 (zwei Archive) und die einseitigen Tueren sind Entscheidungen der Stufe 3 (tueren_03_zuordnung.md 3.x).

## Log

- (2026-09-29) Schritt 0 gebaut, gemessen, Suite 406/406 (Commit 3398f601).
- wip-Commit b2ef2c7c: Schritte 1-5.
- Kontaktbogen 184 Seiten, Echtlauf 5 Tueren, Zwischensequenz-Sperre nach dem ROOM4000-Befund, Paket-Gate.
