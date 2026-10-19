# Runde 39 Spur P "johnruf1041" - Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r39_johnruf1041`, Zweig `r39/johnruf1041`, HEAD 20df7045, Basis b1309110
(= r39/dining1141 c7253b42 + master 02dc7917). Eigener Diff der Spur = `git diff b1309110 HEAD` (16 Dateien, +2056/-2);
`git diff master` zeigt zusaetzlich Spur J (dining_r39) und die inzwischen in master liegende Spur K - nicht Gegenstand.
Massstab: AUFTRAG.md Nachtrag 10 (Wortlaut), VERTRAG.md, Lesart des Orchestrators ("ROOM 1040" = ROOM1041,
"nicht erst in ROOM 1041" = 1141). Zeit: 2026-10-19 ab ~02:30.

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, "ninja: no work to do".
- exe `re15_port/build/platform/pc/re15_pc.exe` 01:27:58 liegt NACH dem letzten Code-Commit 4699f778 (01:27:36);
  alle spaeteren Commits sind Dossier/Riegel; `git status` leer. 81 `johnruf`-Zeichenketten in der exe.


## 1. Tests (gezielt, keine volle Suite)
- `ctest --test-dir re15_port/build -R "r39_johnruf1041" --timeout 900 --output-on-failure -j 2`
  -> "100% tests passed, 0 tests failed out of 10" (integration_r39_johnruf1041_a 71,09 s, _b 282,90 s; Laeufe
  `re15_port/build/r39_johnruf1041_wd/lauf_a_77d47d61`, `lauf_b_0f84bdd8` = MEINE Laeufe, unten ausgewertet).
- Unit-Gruppen der beruehrten Module: `ctest ... -R "^unit_(scd_|map_|karte|r30_hinweis|r30_karte|r33_karte|r35_|r36_karte|
  r38_tagebuch|r38_dining1141|r39_dining1141|r38_elzatoene|r39_stimmen|r39_funk10f1|r39_inventar|synchro|voice|se_voice|xa_|
  rotor_bgm|r39_johnruf)" -j 4` -> "100% tests passed, 0 tests failed out of 182".
- Integration der beruehrten Module: `ctest ... -R "integration_r35_cut10f0|integration_r39_dining1141|integration_r38_dining1141|
  integration_r38_elzatoene|integration_r38_tagebuch|integration_r39_elza_karte|integration_r39_stimmen|integration_r39_funk10f1"
  --timeout 1800 -j 3` -> "100% tests passed, 0 tests failed out of 23" (1a).

### 1a. Integrationsgruppe einzeln (alle Passed)
integration_r35_cut10f0 239.86 s; integration_r39_dining1141_d 356.05 s; integration_r38_dining1141_w 182.53 s; integration_r38_dining1141_z 124.88 s; integration_r38_dining1141_r 197.73 s; integration_r38_elzatoene_gunshot1041 83.78 s; integration_r39_stimmen_11b0 108.33 s; integration_r38_dining1141_s 679.11 s; integration_r38_elzatoene_szene1021 78.34 s; integration_r39_stimmen_1171 62.20 s; integration_r39_stimmen_1150 61.45 s; integration_r38_dining1141_l 51.52 s; integration_r38_tagebuch_lesen 60.85 s; integration_r39_elza_karte 51.32 s; integration_r39_stimmen_1050 40.36 s; integration_r38_dining1141_a 0.24 s; integration_r38_dining1141_d 0.19 s; integration_r38_tagebuch_leon 35.76 s; integration_r38_tagebuch_karte 35.30 s; integration_r39_funk10f1_zweit 21.19 s; integration_r39_funk10f1_leon 10.40 s; integration_r39_funk10f1_echt 74.53 s; integration_r39_dining1141_kampf 313.54 s;

## 2. RE-Gate (Stichproben SELBST disassembliert / Bytes gelesen)
| Zitat im Diff | selbst gemessen | Ergebnis |
|---|---|---|
| RE2-Verteiler @0x800a74c8[0x5F] -> 0x80058090 (Xa_vol) | `re2_disasm.py table 0x800a7640 3` (info/re2leon/PSX.EXE): `[1] 0x800a7644 -> 0x80058090`; `dis 0x80058090`: `lbu v1,1(v0)` @0x80058098, `sltiu v0,v1,0x80` @0x800580a0, `sw v1,23376(at)` = 0x800d5b50 @0x800580b0, `addiu v0,v0,2` @0x800580bc, `addiu v0,zero,1` @0x800580c8 | stimmt |
| Standard 120: FUN_80052f3c @0x80053014/@0x80053038, FUN_800128c4 @0x80012958/@0x8001296c | `addiu v0,zero,120` @0x80053014, `sw v0,23376(at)` (0x800d5b50) @0x80053038; `addiu v0,zero,120` @0x80012958, `sw v0,2128(s0)` @0x8001296c | stimmt |
| Wirkung: FUN_80013ac8 Stereo L=R=vol / Mono vol>>1, DsMix @0x80013b34 | `lbu v1,23387(v1)` (0x800d5b5b) @0x80013ad0, Stereo `sb a0` @0x80013ae4/@0x80013af4, Mono `srl v0,a0,1` @0x80013b08, `jal 0x8008bebc` @0x80013b34; Leser FUN_80013b4c `lw s0,23376(s0)` (0x800d5b50) @0x80013b60 rampt bis zu diesem Wert | stimmt (Lautstaerke linear in vol) |
| RE2 ROOM11D0.RDT @0x01F7A `5f 46`, @0x01F84 `5f 32`; ROOM1020.RDT @0x01960 `5f 64` | `xxd`: 11D0 @0x01F7A `5f 46 59 00 0b 00`, @0x01F84 `5f 32 59 00 09 00`; 1020 @0x01960 `5f 64 59 00 0b 00` | stimmt (Xa_vol + Xa_on) |
| RE2 ROOM1060 sub05 +0x2E `09 0a 1e 00` (Sleep 30), +0x58 `09 0a 5a 00` (Sleep 90) | `xxd info/re2leon/PL0/RDT/room1060/scd/sub05.scd`: +0x2E `09 0a 1e 00`, +0x58 `09 0a 5a 00` | stimmt (Form-Leihe aus einer Se_on/Sleep-Szene, als solche gekennzeichnet) |
| RE2 Opcode 0x84 @0x800591C4, Anforderung @0x800591e8 | Tabelle [0x84] @0x800a76d8 -> 0x800591c4; `sb v0,-3256(at)` (0x800df348 = 1) @0x800591e8 | stimmt |
| ROOM1041 sub11 @0x021EA..@0x0223C (54 B Original im Programm) | `xxd -s 0x021EA -l 84 ROOM1041.RDT`: `22 03 ad 01 | 46 08 .. | 22 02 07 01 | 22 01 1b 01 | 2b 03 00 00 | 2e 01 00 00 | 09 0a 19 00 | 40 00 09 20 3c ab 04 42 | 11 00 08 00 | 02 00 | 12 04 | 21 05 20 00 | 09 0a 19 00 | 2b 02 00 00 | 09 0a 64 00 | 22 02 07 00 | 22 01 1b 00 | 3c 01 | 2e 01 00 00 | 42 00 | 01 00` = Listing von gen/johnruf_r39_programme.inc; Plc_dest-Ziel ersetzt (`3e b3 38 cd` = (-19650,-13000)), msg 2 -> 40 | stimmt |
| Treppentuer ROOM1041 main00 @0x010D8 | `3b 02 02 31 00 00 50 b0 ec c8 dc 05 98 08 ...` x=-20400 z=-14100 w=1500 d=2200 -> Mitte (-19650,-13000), Ziel Raum 06 Cut 7 | stimmt |
| ROOM1021 sub02 @0x01FD8 / @0x01FE0; ROOM1141 @0x00AFE / @0x00C06 / @0x00C0C / @0x00C18 / @0x00C1C | 1021: `22 04 60 00` @0x01FD8, `22 04 f0 01` @0x01FE0; 1141: `21 04 f0 01` @0x00AFE, `54 00 02 00 00 00` @0x00C06, `22 04 f2 00` @0x00C0C, `22 04 f0 00` @0x00C18, `22 04 f2 01` @0x00C1C | stimmt |
Rate-Signale: `git diff b1309110 HEAD -- re15_port | grep -iE "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx"`
-> 0 Treffer. getenv im Diff nur `RE15_SE_DEBUG` / `RE15_BGM_CTL_DEBUG` (Protokollzeilen), kein Verhaltensschalter.
Konstanten ohne Adresse sind gekennzeichnet: Rufwahl help03/help02 (PORT-WAHL, Spracherkennung Dossier 3.2), Plc_dest-Ziel
(PORT-WAHL, Ziel = Tuer @0x010D8), Xa_vol 100 statt 70 in 1131 (beide RE2-Werte derselben Stimme 0x0B, Wahl GEMESSEN
Dossier 4.2), Bank-9-Bit 97 / Ids (VERTRAG). **Gate haelt.**

## 3. Messung je Nutzer-Punkt (selbst, an der exe dieses Baums)
Messordner (nicht committet): Scratchpad `abn0/` dieser Abnahme-Sitzung. Eigene Laeufe:
- **voll** = ECHTER Weg von VOR der John-Szene: Spielstand ROOM1041 (-23550,16700) Blick 0, Flags 4:0x60 3:0xC9 4:0xF1 3:0xAD
  4:5 4:4 (wie integration_r38_elzatoene_szene1021 + Rolladen offen wie Riegel a), CONTINUE, `RE15_FIRE_AOT=3@40#1041` (Tuer
  nach 1021), Wegplan: in 1021 ins Ausloesefeld AOT 5 -> echte Szene -> zurueck zur Tuer slot 2 -> ROOM1041, dort 700 Bilder
  OHNE Eingabe und OHNE `GEGNERWEG`; eigene exe-Kopie `re15_pc_abn0p_voll.exe` (nach dem Lauf geloescht), Elza per
  `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `RE15_SE_DEBUG/RE15_BGM_CTL_DEBUG/RE15_MSG_LOG/RE15_STATE_LOG/
  RE15_AUDIO_CAP_SYNC/RE15_FRAMEDUMP=0-720/30` (RE15_WINDOW_SCALE=1). Skript `tools/mess.cmake`.
- **a/b** = meine Laeufe der Riegel integration_r39_johnruf1041_a/_b (`re15_port/build/r39_johnruf1041_wd/lauf_a_77d47d61`,
  `lauf_b_0f84bdd8`), selbst ausgewertet.
- Pegel UNABHAENGIG vom Werkzeug der Spur: eigenes numpy-Skript (`tools/pegel_abn.py`: Mitschnitt int16 Stereo 44,1 kHz,
  1470 Werte/Bild, Referenz = WAV linear auf 44,1 kHz interpoliert OHNE Port-Lader) -> Verhaeltnis derselben Datei in
  verschiedenen Raeumen (vom Lader-Modell unabhaengig).

### 3.1 Punkt 1a "Hilferufe ... erstmalig ... leise im ROOM 1040 [=1041], nachdem John verschwunden ist"
Lauf voll, debug.log (Auszug, Reihenfolge):
```
[save] CONTINUE: resumed in room 1041            (kein [johnruf] - (4,0xF0)=0, John-Szene noch nicht gelaufen)
[room] PC loaded room1021.rdt
[msgtext] room=1021 id=0 "John: No way, this can't be... Roy, this is a joke, right?"  ... id=8 "John: Please... I... I'd like to be alone now..."
[room] PC loaded room1041.rdt
[johnruf] F2333 ROOM1041: John verschwunden ((4,0xF0)=1), Bit 97=0 -> RUF1041 gestartet (Faden 10)
[johnruf] F36 Xa_vol(50) ROOM1041 -> CD_VOL 6826
[johnruf] F36 ROOM1041 Ruf help03 (Sprach-Id 55, Xa_vol 50 -> CD_VOL 6826)   [voice] Start ROOM1041 Id 55 CD_VOL 6826 capTick 3090
[johnruf] F98 ROOM1041 Ruf help02 (Sprach-Id 54, Xa_vol 50 -> CD_VOL 6826)   [voice] Start ROOM1041 Id 54 CD_VOL 6826 capTick 3152
```
=> (4,0xF0) setzt die ECHTE 1021-Szene (sub02 @0x01FE0), der erste Eintritt in 1041 danach startet die Rufe; vorher (erster
Aufenthalt in 1041) kein Ruf. Pegel:
| Zeile | Werkzeug der Spur (g, Soll) | eigenes Skript: g roh | Verhaeltnis zum selben Ruf in 1141 (Xa 120, Lauf b) |
|---|---|---|---|
| help03 1041 (voll, Takt 3090) | 0,4179 / 0,4167 (1,003) | 0,4881 (rho 0,953) | 0,4881 / 1,1688 = **0,418** (Soll 50/120 = 0,417) |
| help02 1041 (voll, Takt 3152) | 0,4201 / 0,4167 (1,008) | 0,4780 (rho 0,525) | 0,4780 / 1,1452 = **0,417** |
| main40 1041 (voll, Takt 3242) | 1,0021 / 1,0000 | 0,4975 (rho 0,32*) | main50 in 1141 (Spur J, 120): 0,4933 -> gleich laut |
(*) Elzas Datei wird von der Stufe 2 der Spur A dynamisch bearbeitet; ohne Port-Lader korreliert die Rohdatei schlecht, das
Verhaeltnis zur gleichen Datei in 1141 (1,01) zeigt aber denselben Pegel.
Hoerbarkeit (eigenes Skript, Stimme g*Referenz gegen Rest im selben Fenster): help03 +9,9 dB ueber dem Rest; help02 Bild fuer
Bild: "Please" (+7..+18) 7..58 dB ueber dem Rest, "help" (+40..+45) **2,4..8 dB UNTER** dem Rest (Zombie-Stoehnen se=4/se=5,
vol 127, im selben Fenster, debug.log `[se] Stimme: se=5 ... vol=127`, `se=4 ...`). Siehe Hinweis H1.
**Urteil: erfuellt** (leise = 41,7 % = -7,6 dB, RE2-Wert Xa_vol 50 @0x01F84; erstmalig; nach Johns Verschwinden).

### 3.2 Punkt 1b "dann soll bereits von Elza kommen: Oh, no, this is John! He is in Trouble, I have to help him!"
Lauf voll: `[johnruf] F188 Xa_vol(120)`, `[msgtext] room=1041 id=40 "Elza: Oh, no, this is John! He is in trouble, I have to help
him!"`, `[voice] clip loaded: main40.wav`, `[voice] Start ROOM1041 Id 40 CD_VOL 16383 capTick 3242` (Pegel 1,002). Reihenfolge
help03 < help02 < msg 40 im debug.log. Bild `abn0/voll/fd_000210.png`, `fd_000300.png`: Textkasten zweizeilig, "Elza:" gruen
(Farbe 1 wie ROOM1041 msg 2), Elza zur Treppentuer gedreht (state.txt Gier 923, Soll 924). Schreibung "trouble" klein
(Nachtrag 6). **Urteil: erfuellt.**

### 3.3 Punkt 1c "Kopiere mir auch das voicefile stattdessen dort hin"
`synchro/STAGE1/room1041/main40.wav` liegt (Orchestrator-Kopie 02dc7917 in der Basis), sha256 a2a0b166b1da1613...68b531 =
`room1141/main50.wav` (bytegleich); im Lauf gespielt (`clip loaded: main40.wav`). Keine Nutzer-WAV unter den von P belegten
Ids (room1041 hat nur main02/main40, Ordner room1131 existiert nicht). **Urteil: erfuellt.**

### 3.4 Punkt 1d "Dann soll die Karte aufgehen und den ROOM 1141 markieren"
Lauf voll: `F370 Kartenhinweis angefordert (Eintrag 5: ROOM1041 -> ROOM1141), Faden parkt` (= msg 40 + 182 Bilder, main40 6,0 s
ausgeredet), `[hint] F380 begin`, Blinken rot/umriss, `F496 Zeit um (Hinweis 5, 3 Blinkperioden)`, `F510 Karte zu`. Bilder
`abn0/voll/fd_000390.png` (rot gefuellt) und `fd_000450.png` / Riegel-Abzug `lauf_a_77d47d61/karte.bmp` (Umriss): Schirm
"POLICE STATION 3F", eine Raumkachel blinkt. Zonentabelle `engine/src/re15_map_zones.h:172` `{ 0x1141, ..., 4, 6, ...}` =
Blatt 4 = 3F (dort auch 1121/1131/1151). Ohne Lageplan 3F zeigt der Schirm nur das Ziel (RE2-Form, wie die Hinweise der
Runden 35 K / 38 G). **Urteil: erfuellt.**

### 3.5 Punkt 1e "Danach soll wieder die gleiche Panische finale Musik kommen wie bei Leon zum Ende ..."
Lauf voll: `F510 Karte zu: Faden laeuft weiter (Set(9,97)=1 ...)`, `Panik-Musikfenster auf in ROOM1041 (Bit 97 = 1)`,
`[bgm] stage=0 room=04 entry=FF01 -> MAIN01(flag 0)`, MAIN00 blendet aus (`Ausblendung fertig`), `[bgm] loaded: 9 VAGs, SEQ
7788B, PPQN=48 tempo=555555us`. Leons Fenster (meine Laeufe integration_r35_cut10f0, `r35_cut10f0_wd/lauf_ab9fd425_d/e/f2/g`):
`[cut10f0] MAIN01-Fenster auf ...` + `entry=FF01 -> MAIN01` + **dieselbe** Zeile `[bgm] loaded: 9 VAGs, SEQ 7788B, PPQN=48
tempo=555555us`. Nutzer Runde 35 (AUFTRAG r35 Z. 92): "Bis Leon dann den Parking Lot erreicht hat, soll durchweg MAIN01 ...".
Weiterlauf: Lauf a `room=06 entry=FF01 -> MAIN01 ... [unveraendert, laeuft durch]` (1061), Lauf b nach dem Laden in 1121
`entry=FF01 -> MAIN01`, 1131 `[unveraendert, laeuft durch]`. In 1141 (Gorilla-Raum) wie Leons 11C0 die eigene Musik MAIN16.
**Urteil: erfuellt.**

### 3.6 Punkt 1f "Wenn wir uns den Flur 1131 naeheren, koennte das please help von John auch noch mal kommen, aber schon etwas lauter"
Lauf b (Spielstand ROOM1121 mit Bit 97, echte Tuer -> 1131): `[johnruf] F83 ROOM1131 in der Panikphase -> RUF1131 gestartet`,
`F36 Xa_vol(100) ROOM1131 -> CD_VOL 13652`, `Ruf help02 (Sprach-Id 54 ...)`, `[voice] Start ROOM1131 Id 54 CD_VOL 13652 capTick
649`, `F126 Xa_vol(120)`; genau 1 Zeile "ROOM1131 Ruf help" im ganzen Lauf. Pegel: Werkzeug der Spur g 0,8331 (Soll 0,8333);
eigenes Skript g 0,9576 gegen 1,1452 fuer denselben Ruf in 1141 -> **0,836** (Soll 100/120 = 0,833), also doppelt so laut wie
in 1041 (0,417; +6 dB); Stimme +8,1 dB ueber dem Rest im Fenster (MAIN01). "das please help": eigene Gegenprobe Windows
System.Speech en-US (Zwangswahl, verworfene Alternativen): help01 "hello please help" 0,40, help02 "please"/"help" 0,23,
help03 "help" 0,33, help05 "help" 0,03 -> help02 = "Please help!" (wie Dossier 3.2), derselbe Ruf wie in 1041 ("noch mal").
Lesart "naehern" = beim Betreten des Flurs 1131 (30 Bilder danach, Elza laeuft frei weiter) - vertretbar.
**Urteil: erfuellt.**

### 3.7 Punkt 1g "Im ROOM 1141 soll alles so bleiben wie bisher, nur eben ohne die 'Oh, no, this is John! ...' Cutscene"
- P-Anteil (1141 unveraendert): Lauf b `room=14 entry=FF56 -> MAIN16(flag 1)`, HAEMMERN39 `Sce_bgm_control slot=0 op=1`
  angewandt (keine Zeile "in der Panikphase NICHT angewandt"), Rufe Standard (`[voice] Start ROOM1141 Id 55 CD_VOL 16383`,
  Pegel 1,0007), MAIN01 blendet beim Eintritt aus; integration_r39_dining1141_d/_kampf und integration_r38_dining1141_w/z/r/s/l/a/d
  gruen (1a). P aendert keine 1141-Datei (dining_r39.c / gen/dining_r39_programme.inc nicht im Diff b1309110..HEAD).
- "ohne die Cutscene" ist laut Orchestrator (AUFTRAG Nachtrag 10) Aufgabe der **Spur J**. In DIESEM Baum spricht Elza den Satz
  in 1141 noch (Lauf b Z. 359 `[msgtext] room=1141 id=50 "Elza: Oh, no, this is John! ..."`) - also bis zur Integration
  zweimal (1041 und 1141). Spur J hat msg 50 in r39/dining1141 entfernt (11d366a9, 6c2a0547; J_abnahme_2.md liegt);
  `git merge-tree --write-tree r39/johnruf1041 r39/dining1141` = konfliktfrei. Pruefen kann man das Zusammenspiel erst im
  Integrationsbaum.
**Urteil (P-Anteil): erfuellt; J-Anteil hier nicht pruefbar (Integration).**

### 3.8 Punkt 2 "Die panische Musik hoert auf, sobald man John gerettet hat. Der Rest bleibt normal."
Lauf b: Kampfende `[bgm] Sce_bgm_control slot=0 op=2 ... capTick=5430` (= Original sub01 @0x00C06 `54 00 02 00 00 00`, MAIN16
Stop), im selben Bild `[johnruf] F4039 ROOM1141 John gerettet ((4,0xF0)=0, (4,0xF2)=0): Bit 97 = 0, Panikmusik aus`,
`Panik-Musikfenster zu in ROOM1141 (Bit 97 = 0)`; danach Johns Zeilen 51/52, Dachtuer -> ROOM1171 `entry=5572 -> MAIN32(flag 1)`
(Tabelle), nach Z. 1200 keine `[johnruf]`-Zeile, kein `entry=FF01`. Mechanik: Bit 97 faellt nur, wenn F0 (geloescht @0x00C18 beim
Szenenbeginn) UND F2 (gesetzt @0x00C1C im selben Skriptschritt, geloescht @0x00C0C bei beiden Gorillas tot) null sind - kein
Zwischenbild moeglich (beide Set in einem VM-Schritt ohne Abgabe). Lesart: MAIN01 laeuft bis zum Gorilla-Raum, dort wie Leons
11C0 die Raummusik MAIN16 ("1141 wie bisher"), die das Original bei der Rettung stoppt; die Panikphase (Bit 97, MAIN01 in
allen anderen STAGE1-Raeumen, Kartenziel) endet mit der Rettung. Spur J hat gemessen, dass der Nutzer MAIN16 selbst
"Panik Musik" nennt (J_dining1141.md 1.5) - beide Lesarten sind damit erfuellt. Die Kampfende-Ausloesung im Riegel b ist der
Messhaken `RE15_R38F_GORILLAS_TOT` (Kill-Flags); P liest nur die Original-Flags, unabhaengig davon, wie die Gorillas sterben.
**Urteil: erfuellt.**

## 4. Vertrag (VERTRAG.md)
- Bank 9: nur Bit 97 (1.1 "P = 97"), in `k_port[]` eingetragen (test_r36_kartenbits.c +2 Zeilen); unit_r36_kartenbits gruen
  (Kollisionspruefung gegen die Original-Item-Taken-Bits). Bit 97 im Spielstand (Lauf b: Laden mit 9:97 -> Fenster auf).
- Nachrichten-Ids: ROOM1041 Id 40 (zugeteilt, Text + main40.wav). Zusaetzlich nur INNERHALB der eigenen Programme (PC-Schranke)
  Sprach-Ids 53..57 (1041/1131, ohne Text -> room1141/help01..05) und Warte-Zeile 59 (1041). Keine Kollision: ROOM1041.RDT hat
  Ids 0..3, keine Nutzer-WAV room1041/main53..59, kein Ordner room1131 -> Hinweis H4 (formal, VERTRAG 1.2 nennt fuer 1041 nur 40).
- Pfade (`git diff b1309110 HEAD --name-only`): analysis/.../P_johnruf1041.md, engine/src/{game_step_common.c, gen/johnruf_r39_
  programme.inc, johnruf_r39.c, map_hint_common.c, scd_room_setup.c, scd_vm.c}, include/re15_johnruf_r39.h, platform/pc/{main.c,
  src/audio_pc.c}, tests/integration/test_r39_johnruf1041.cmake, tests/unit/{probes/r39_johnruf1041.cmake, test_r36_kartenbits.c,
  test_r38_tagebuch.c, test_r39_johnruf1041.c}, tools/r39_johnruf/programme_bauen.py. Kein release/, platform/android/,
  shared_assets/PSX/, synchro/, keine Test-CMakeLists. **haelt.**
- Gemeinsame Dateien, je Haken 1-5 Zeilen mit "Runde 39 Spur P": scd_room_setup.c +2, scd_vm.c +5, main.c +2, game_step_common.c
  +1, map_hint_common.c +5, audio_pc.c +10 (4 Haken). Ausserhalb der 1.3-Liste fuer P: audio_pc.c (dort A/B), game_step_common.c,
  map_hint_common.c (Karten-Code G) - im Dossier 6 offengelegt, fuer Musik/Karte unvermeidlich -> Hinweis H5.
- Riegel je Punkt: 8 Unit (texte/programme/stimme/xa/ablauf/musik/ruf1131/leon) + 2 Integration (a: 1041-Saetze + Pegel + FB_SHOT,
  b: Laden/1131/1141/Rettung/1171), messend (Folge, Pegel +-10 %, Gier +-32, Bildabzug > 100 kB). Parallel-fest (exe-Kopie
  `re15_pc_r39p_<teil>_<zufall>`, eigener Laufordner, RE15_*-Umgebung nicht geerbt), Linux-fest (`m` gelinkt, cmake_minimum_required,
  keine .exe-/Backslash-Pfade, kein popen).
- Zusammenfuehrung (git merge-tree, ohne Eingriff): P+J konfliktfrei; P+master (Spur K) und P+G/Q/R/O nur triviale Konflikte an den
  vereinbarten Blockenden (Include-/Installer-Zeilen, beide behalten) -> Hinweis H3.

## 5. Nebenwirkungen
- Leon: unit_r39_johnruf1041_leon (ROOM1040 mit denselben Flags: kein Programm, Weiche -1), integration_r35_cut10f0 (MAIN01-
  Fenster Leons, 239,86 s), integration_r38_tagebuch_leon, integration_r39_funk10f1_leon gruen.
- Sprachpegel aller anderen Zeilen: CD_VOL = 0x3FFF*Xa_vol/120 = 0x3FFF bei 120 = Initwert (`audio_pc.c:2043` einziger anderer
  Schreiber) -> unveraendert; integration_r39_stimmen_1050/1150/11b0/1171 gruen; Xa_vol je Raumaufbau zurueck auf 120.
- Opcode 0x5F im RDT-Bytecode: unveraendert op_unknown (pc+1), unit_r39_johnruf1041_xa + unit_scd_* gruen.
- 1041-Schuss-Szene (sub11, vor Roys Dialog): integration_r38_elzatoene_gunshot1041 gruen; RUF1041 braucht (4,0xF0)=1, sub11 (4,0xF1)=0
  -> schliessen sich aus.
- 1141 (Spur J): siehe 3.7, alle 1141-Riegel gruen.

## 6. Hinweise (kein Mangel, fuer Orchestrator/Nutzer)
- **H1 Zombies im Gang 1041 waehrend der neuen Szene.** In meinem echten Lauf (Rolladen offen wie der Riegel a) stehen beim Eintritt
  3 Zombies (Typ 0x16, ROOM1041 main00 @0x01224.., Toetungsbits je Zombie) in ~5000 Einheiten. Die Szene (Rufe + Satz ~370 Bilder
  Spielzeit, Karte eingefroren) haelt sie nicht an: Zombie #1 5016 -> 2572 bis zur Karte, Steuerung zurueck bei F510 mit 2534,
  ohne Eingabe Biss bei F590 (+80 Bilder = 2,7 s, Energie 100 -> 80) und F628 (-> 60) (state.txt Lauf voll). Bilder
  `abn0/voll/fd_000300.png` (Zombie direkt neben Elza waehrend "Oh, no, this is John!"), `fd_000510/570/600.png`. Das Stoehnen
  (se=4/se=5, vol 127) liegt im zweiten Wort des leisen "Please help!" 2,4..8 dB UEBER der Stimme. Der Riegel a raeumt die Gegner
  mit `GEGNERWEG` direkt nach der Szene ab und sieht das nicht. Bewertung: Form wie das Original - ROOM1041 sub11 (@0x021EA..
  @0x0223C, gleiche Pad-Sperre) haelt die Zombies ebenfalls nicht an (kein `Member_set(12,..)`, Message_on mit Flag 0, d.h. kein
  Freeze-Bit 0x2000), und die Port-Regel der Runde 38 B (Szenenschutz: kein Angriff, solange die Szene haelt) greift (0 Treffer
  waehrend der Szene). Die P-Szene ist aber ~2x so lang wie sub11. Wenn der Nutzer das nicht will: Original-Form zum Anhalten
  waere `Member_set(12,0x30)` / `(12,0x10)` je Gegner (Ziel per Work_set(2,n)), selbst gelesen in ROOM1141 sub02 `34 0c 30 00`
  @0x00E70/@0x00E78 (anhalten) und `34 0c 10 00` @0x00E40/@0x00E54/@0x00F64/@0x00F74 (freigeben), ebenso ROOM11C0 @0x01A58;
  ROOM1041 sub11 hat KEINEN solchen Befehl (scd_dump_room.py: nur Message_on @0x02200/@0x02224). Entscheidung beim Nutzer.
- **H2** Der lautere Ruf in 1131 kommt einmal je Panikphase und Sitzung (Merker im Speicher); nach dem Laden eines Spielstands aus
  der Phase noch einmal (Dossier 6, offengelegt).
- **H3** Integration: triviale Konflikte in scd_room_setup.c (mit master/Q/R/O), scd_vm.c/main.c (R/O), map_hint_common.c (G) -
  beide Zeilen behalten. In diesem Baum spricht Elza ihren Satz in 1141 noch (Spur J entfernt ihn), erst die Integration zeigt
  den Endzustand "nur in 1041".
- **H4** Sprach-/Warte-Ids 53..57/59 in 1041 und 53..57 in 1131 sind nicht ausdruecklich zugeteilt (formal); eigene Aufnahmen unter
  room1041/main53..59.wav bzw. room1131/main53..57.wav wuerden dort nicht gespielt (Dossier 7 sagt das dem Nutzer).
- **H5** Haken in audio_pc.c / game_step_common.c / map_hint_common.c ausserhalb der 1.3-Liste fuer P (klein, kommentiert).

## 7. Maengel
Keine. (H1 ist eine im Original-Raumskript angelegte Eigenschaft der Szene, keine Abweichung von Wortlaut, Beleg oder Vertrag;
sie gehoert aber vor den Nutzer.)

## 8. Ergebnis
| Punkt | Urteil |
|---|---|
| 1a Rufe leise in 1041 nach Johns Verschwinden, erstmalig | erfuellt |
| 1b Elzas Satz in 1041 | erfuellt |
| 1c Sprachdatei nach room1041 kopiert | erfuellt |
| 1d Karte geht auf, ROOM1141 markiert | erfuellt |
| 1e danach Leons Panikmusik (MAIN01) | erfuellt |
| 1f 1131: "Please help!" noch mal, lauter | erfuellt |
| 1g 1141 wie bisher (P-Anteil; "ohne Elzas Reaktion" = Spur J) | erfuellt |
| 2 Panikmusik endet mit Johns Rettung, Rest normal | erfuellt |
Gates: Bau gruen, Riegel 10/10 + Unit 182/182 + Integration 23/23, RE-Gate (9 Zitate selbst gelesen) haelt, Vertrag haelt
(formale Hinweise H4/H5). **Abnahme 0: bestanden.**
