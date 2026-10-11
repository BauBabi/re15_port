# Runde 36 — Spur Q "schluessel": Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Stand: Zweig `r36/schluessel` HEAD 9ca87339 (Basis master 78a4a748), Baum `.claude/worktrees/r36_schluessel`.
Messordner (ausserhalb des Baums): Scratchpad `Q_abn1/` (Laeufe `Q_abn1/runs/<lauf>/`: debug.log, state.log,
discard.log, fd_*.ppm, sheet.png). Messkopie der exe: `re15_pc_Qabn1.exe` (nach der Abnahme entfernt).

**Ergebnis: BESTANDEN** — Punkt 1 erfuellt, Punkt 2 erfuellt (Mangel M1 der Abnahme 0 an der exe behoben, ueber den
echten Tuerweg 11E1 -> 1211 gemessen: Abfrage im Schliess-Bild von msg 9, Ja -> weg, Nein -> bleibt). Die Messluecke
H5 der Abnahme 0 (Parkplatz-Tuer nach der Sherry-Szene) ist geschlossen: verschlossen. RE-Gate, Vertrag und gezielte
Tests (5/5 Unit, 120/120) halten. Keine Maengel; Hinweise H1/H3/H6/H7/H8 nicht blockierend (Abschnitt 8).

## 0. Gelesen
- AUFTRAG.md Z. 45 (Punkt 1) / Z. 49 (Punkt 2) woertlich, Nachtrag 14:00 (Spur Q: nur Dossier-Hintergrund, kein
  leerer Haken), VERTRAG.md 1.1-1.6.
- Dossier `Q_schluessel.md` inkl. Abschnitt "Nachbesserung 1" (N1.0-N1.8), Abnahme 0 (`Q_abnahme_0.md`, Mangel M1,
  Hinweise H1-H6).
- `git log --oneline master..HEAD` (19 Commits, davon 8 Nachbesserung 1: 8423c1f2 .. 9ca87339).
- `git diff 78a4a748..HEAD --stat` (37 Dateien; `git diff master` zeigt zusaetzlich die Sprachdateien
  `synchro/STAGE1/room1011|1021|1031|1041|1141|11B1|1211` als "geloescht" — die kamen erst mit master 565da62b nach
  der Basis, der Zweig fasst sie nicht an).
- Code-Diff der Nachbesserung (`git diff 6af69ce2..HEAD -- re15_port`): `schluessel_r36.c`
  `re15_schluessel_r36_discard_stelle` bekommt den zweiten Zweig (0x1211, msg 9) -> 0x42; `re15_schluessel_r36.h`
  `RE15_R36Q_ZELLE_RAUM 0x1211` / `RE15_R36Q_ZELLE_MSG 9` mit Belegkopf + H4-Kommentar; `item_discard_common.c` nur
  Kommentare; Riegel `test_r36_schluessel.c` Teil `befreiung`, Integrations-Skript `-DNUR=befreiung`, Registrierung
  in `probes/r36_schluessel.cmake`.

## 1. Bau

- `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to
  do`, `LOCAL-BUILD-OK (build)`. exe 02:03:21 (md5 40e375ee); Arbeitsbaum sauber (`git status --short` leer), die
  letzten zwei Commits f268e4f4/9ca87339 aendern nur Dossier + Integrations-Skript -> exe = HEAD-Stand. Die mtime
  von `schluessel_r36.c` (02:03:18) liegt NACH dem Commit 0f363920 (Rueckbau-Probe des Bau-Agenten) und VOR dem
  Link (02:03:21) — der Inhalt ist laut `git status` HEAD, und die Messungen unten zeigen die Stelle wirksam.
- `ctest -R "^unit_r36_schluessel"` -> `100% tests passed, 0 tests failed out of 5` (raum/tuer/zelle/namen/
  befreiung). `test_r36_schluessel befreiung` selbst gefahren: 10 Pruefungen, 0 Fehler (Bytes der Stelle,
  Port-Stellen, Abfrage im Schliess-Bild von msg 9, Vorentscheid bei freiem Belegt-Bit, Ja/Nein, ohne Schluessel
  keine Abfrage).

## 2. Messungen an der echten exe (eigene Laeufe, Messkopie `re15_pc_Qabn1.exe`)

Werkzeug: Kartenwerkzeug `probe_r36_schluessel_karte` + CONTINUE (`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`),
`RE15_PAD_AT` (bildgenaue Flanken; Bildzaehler beginnt je Raum neu), `RE15_STATE_LOG`, `RE15_DISCARD_LOG`,
`RE15_MSG_LOG=1`, `RE15_FRAMEDUMP` mit `RE15_WINDOW_SCALE=1`. Skript `Q_abn1/run_q.sh`.

### 2.1 M1 (Nachbesserung 1): Prison Key nach der Befreiung — ueber den ECHTEN Tuerweg

Anders als der Bau-Agent (Start in 1211 per Karte) starte ich in ROOM11E1 vor der Tuer "Prisons" und gehe in die
Szene hinein. Karte `11E1 9100 5500 0 elza flag:3:0x8B flag:4:0x24 flag:4:0xEC flag:9:83 leer give:0x42:1`
(= Zustand nach der Aufnahme in 10F1: (4,0xEC) + Bit 83 + Prison Key einziger Gegenstand). Tasten
(`Q_abn1/pad_tuer1211.txt`): A F45 (Tuer), U F70-163 (+x, 75 Einheiten/Bild gemessen), R F166-176 (rot 0 -> 1056),
U F180-400 (-z).

| Lauf | Ergebnis (debug.log / state.log / discard.log) |
|---|---|
| P0/P1/P2 (Weg) | `[aot] DOOR FIRE slot=3 ... spawn=(-26400,0,-2200)` -> `[room] PC loaded room1211.rdt` -> `[enemy] EM49 loaded` (John); Elza laeuft bis (-19350,-2200), dreht, laeuft -z; **F~400 Szene sub02 startet** (`pf=01000007` = Pad-Sperre Set(2,7) @0x01F9C, `pst=4` Plc_dest) bei (-20007,-18406) = Slot 6 (sub00 @0x01E22, z -19911..-18361) |
| M1_mess (keine Antwort) | `[msg] room=1211 id=0..9` der Reihe nach; **msg 9 auf F2115** (`abfrage=1 frage=0` vorgemerkt, `belegt=1`), **zu F2214** (99 Bilder) und im SELBEN Bild `frage=8 gegenstand=0x42 text=0/44 frost=1`; Text fertig **F2301** (44/44); bis F2420 `pausepad=1`, Elza steht (-18446,-21834). Bilder F2120-F2190 "Elza: John. Wait!", F2310-F2420 "You don't need this key any more. Discard it? > Yes No" (`runs/M1_mess/sheet_2100.png`, `sheet_2300.png`). Nie `msg_aktiv=1` zusammen mit `frage=8`. |
| M1_ja (A F2320) | F2320 `abfrage=0 gefragt=1 weg=1`; F2322 John `@(0,0)` (Pos_set @0x0228A), **F2323 `pausepad=0`** (Set(2,7)=0 @0x0229A) = Szenenende 3 Bilder nach der Antwort; U F2340-2370 -> Elza laeuft ((-18446,-21834) -> (-19655,-19881)) = frei; START F2400 -> Statusschirm F2440-F2480: **Item-Liste leer** (`runs/M1_ja/sheet_ja2.png`). |
| M1_nein (R F2312, A F2320) | F2312 `wahl=1`; F2320 `weg=0 gefragt=1`; F2323 `pausepad=0`; Elza laeuft danach; Statusschirm F2470: **Ring-Icon (Prison Key) bleibt** in der Item-Liste (`runs/M1_nein/nein_paar.png`). |
| M1_ohne (Schluessel nicht im Inventar, (4,0xEC)+Bit 83 gesetzt = "in der Box") | Szene laeuft gleich, msg 9 F2115-F2214, **0 Zeilen mit `frage=8`**, John (0,0) F2216, Szenenende F2217 — kein Haenger, keine Abfrage (Dossier N1.6 "Randfall Item-Box" bestaetigt). |

Damit ist M1 an der exe behoben: die Abfrage kommt genau am Ende der letzten Szenen-Zeile (Schliess-Bild von msg 9
= RE2 @0x800517f4 Belegt-Schranke), friert die Szene ein (John bleibt bis zur Antwort bei (-19551,-6708), Pad-Sperre
steht), Ja wirft 0x42 weg, Nein behaelt ihn, danach endet die Szene normal und Elza ist frei.
Nebenbefund geprueft und verworfen: das kleine weisse Objekt oben in Bildmitte (Kamera 5) nach dem Szenenende ist
KEIN John bei (0,0,0) — es steht auch im Lauf Z0_ohne (John gar nicht geladen, keine Szene) im Bild
(`runs/Z0_ohne/sheet.png`); es gehoert zum Raum und wird waehrend der Szene (Set(1,0x1B)=1 @0x01FA0) nur nicht
gezeigt. Kein Spur-Q-Thema.

### 2.2 Punkt 1 — Leon (Regressions-Lauf nach der Nachbesserung, echte Tuerwege)

| Lauf | Karte / Tasten | Ergebnis |
|---|---|---|
| L1_aufnahme | `10D0 800 -5600 1024 flag:3:0x32 flag:9:71 flag:3:94`; A F45, R F60-70, U F75-160, A F175, A F300 | `DOOR FIRE slot=0 ... spawn=(8400,0,-350)` -> `room10f0.rdt` -> `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei (8050,-1460,2700), Zone Slot 15`; Leon an der Konsole (8658,2032); Modal mit RE2-Bild P. Room Key (schwarzer Kunststoffkopf, rotes Emblem, Metallbart), "Will you take the Parking Lot Key? > Yes No" (F280); A F300 -> `[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)` (`runs/L1_aufnahme/sheet.png`) |
| L2_ohne (Audio-Mitschnitt) | `1180 4900 28800 0 flag:4:0xF3 flag:3:0x82`; A F45, R F60-69, U F75-200, A F215 | `DOOR FIRE slot=3` -> `room11b0.rdt` -> `[schluessel] ROOM11B0 Slot 1 (Tuer -> ROOM11C0) gesperrt: Ereignis 25, Flag (9,93) = 0`; A F215 -> `[se] Stimme: se=0 ... note=66 fine=42 center=85 vol=110 pitch=0x589` (TUERSE Satz 0 ZU_A) + `Parkplatz-Tuer verschlossen (kein Parking Lot Key im Inventar)`; state.log 126 Bilder `msg id=28`, kein DOOR FIRE bis F340; Bild "It's locked." + Seitenpfeil (`runs/L2_ohne/sheet.png`; der blaue Fleck zwischen "It's" und "locked" ist ein Gegenstand am Boden, steht schon in F200 ohne Text — `runs/L3_mit/sheet.png` F200) |
| L3_mit (Audio-Mitschnitt) | wie L2 + `flag:9:82 give:0x45:1`; A F215, A F310, A F420, A F440-452 | A F215 -> `[se] Stimme: se=4 ... pitch=0x4f3 (13641 Hz)` (Satz 4 AUF_KEY) + `Parking Lot Key benutzt -> Tuer offen, Flag (9,93) = 1`; "You've used the Parking Lot Key." (Name gruen); A F310 schliesst, F311 `frage=8 gegenstand=0x45`, Text fertig F398, A F420 -> `weg=1`; A F440.. -> `DOOR FIRE slot=1 ... spawn=(-25279,0,17268)` -> `room11c0.rdt` -> `EXIT_AT: Bild 5 in Raum 11c0` |

Die Nachbesserung hat den Parking-Lot-Zweig von `re15_schluessel_r36_discard_stelle` nur umgestellt (gleiche
Bedingung (0x11B0, 29) -> 0x45); L3 zeigt ihn an der exe unveraendert wirksam.

### 2.3 Punkt 2 — Elza (echte Tuerwege)

| Lauf | Karte / Tasten | Ergebnis |
|---|---|---|
| E1_aufnahme | `10D1 800 -5600 1024 elza flag:3:0x32`; wie L1, START F330 | `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`; `DOOR FIRE slot=0` -> `room10f1.rdt` -> `[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4 bei (8050,-1460,2700), Zone Slot 15` (= exakt Leons Stelle); Modal mit RE2-Bild Cabin Key (grosser Eisenring + Bartschluessel), "Will you take the Prison Key? > Yes No"; `PRISON KEY genommen: Item 0x42, Bit (9,83)` + `Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)`; Statusschirm: Ring-Icon in der Item-Liste (`runs/E1_aufnahme/sheet.png`) |
| R1_roy | `1021 -2950 -16700 1024 elza flag:4:0xF1 flag:4:0xFA flag:3:0x74`; A F45, R F60, U F65-130, A F145 | `DOOR FIRE slot=1` -> `room1011.rdt`, `EM47 loaded`; `[schluessel] ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)`; `[msg] room=1011 id=18` als EINZIGE Nachricht (245 Bilder), 0 Bilder mit id 19/20; Bild "He has passed out from his injuries." (`runs/R1_roy/sheet.png`) — Roy gibt den Prison Key nicht mehr |
| Z0_ohne | `11E1 9100 5500 0 elza flag:3:0x8B flag:4:0x24` (OHNE (4,0xEC)); Tuer wie 2.1, vor die Zelle, A F490 | `DOOR FIRE slot=3` -> `room1211.rdt`, KEIN `EM49 loaded`; Druck an der Zelle -> msg id 10 (111 Bilder), Bild "It's locked." + Seitenpfeil (`runs/Z0_ohne/sheet.png`) |
| M1_* | s. 2.1 | mit Prison Key: Szene + Abfrage, Ja -> weg |

### 2.4 Messluecke H5 der Abnahme 0 geschlossen: Parkplatz-Tuer NACH der Sherry-Szene (erster Strom-Eintritt)

Karte `1180 4900 28800 0 flag:4:0xF3 flag:4:0x06` ((3,0x82)=0 -> sub01 @0x011AE/@0x011B6 startet sub04, sobald
(4,0x06)=1). Lauf H5_szene: A F45 -> `DOOR FIRE slot=3` -> `room11b0.rdt` -> `[schluessel] ... gesperrt` im
Raumaufbau -> `EM4B loaded` (Sherry) -> `[msg] room=11b0 id=0`; Szene endet F~520, Leon bei (-2447,-21983) im
OST-Teil (sub04 @0x01436 Pos_set (0xf671,0xaa21)). Der Weg zurueck in den West-Teil fuehrt durch die Luecke des
Gitters: SCA (`py/sca_plot.py`, RDT+0x20, 19 Zellen, Partitionen 0/0/5/14/0) Gitter = Zelle 12 x -22170..-21390
z -27641..-16731 + Zelle 17 z -17230..-15230; sub00 @0x0118C `37 03 0c 00` (Sca_id_set Partition 3, Index 12 =
global 17) bei (4,0x06)=1. Gemessen: Leon geht bei z ~-15900 (innerhalb von Zelle 17) ungehindert durch das
Gitter-Nordstueck; bei z -23218..-26269 (wo die Abnahme 0 suchte) ist Zelle 12 Wand.
Lauf H5_w3 (Tasten `Q_abn1/pad_h5.txt`): L F530-546, U F550-583, L F586-596, U F599-641, R F644-654, U F657-863,
R F866-875, U F878-988 (bis (-20546,-16030)), L F991-1000, U F1003-1070 (durch die Luecke bis (-25578,-15758)),
L F1073-1083, U F1086-1228 (bis (-25721,-26252) vor der Parkplatz-Tuer), **A F1240** ->
`[schluessel] ROOM11B0: Parkplatz-Tuer verschlossen (kein Parking Lot Key im Inventar)`, state.log 121 Bilder
`msg id=28`, kein `DOOR FIRE slot=1`; Bild "It's locked." + Seitenpfeil (`runs/H5_w3/sheet.png`).
Statisch dazu selbst gelesen: sub04/sub05/sub09 ohne Aot_set/Aot_reset auf Slot 1; sub01 fasst Slot 1 nur bei
Ck(4,0xF3)==0 an (@0x011F6 -> @0x01216); sub06 setzt nur Slot 5 (@0x0172E). Die Sperre ueberlebt die Szene.

## 3. Tests (gezielt, keine volle Suite)

- `ctest -R "^unit_r36_schluessel"` -> 5/5 gruen (Abschnitt 1).
- Gezielter Lauf `-j 3`, `--timeout 600`, -R woertlich (`Q_abn1/regex.txt`; Auswahl der Abnahme 0 bzw. des
  Bau-Agenten + `unit_scd_key_ck` + `integration_keypad`, die einzigen weiteren Treffer von
  `modal|pickup|discard|item|key|inv` in `ctest -N`):
  `r36_schluessel|r21_discard|r30_tuer|r31_tuer_ton|door|unit_inv_|unit_item_|itembox|pri_inventar_riegel|r26_inventar|r35_inventar1050|item_name_census|android_n1_inventar|sicherung|r35_cut10f0|r22_10f0|r35_inhalt|r34n_e_dokumente|r30_irons_tisch|unit_msg|action_msg_gate|r33_tuer|rolltor|adaruf|r35_cut1150|fenster|elza|keydoor|aot_sce_census|flag_census|itps|prompt|zgirl_tuer|zgirl_selbsttueren|10f0|11b0|1011|1211|unit_scd_key_ck|integration_keypad`
  -> **`100% tests passed, 0 tests failed out of 120`** (519,95 s; parallel zu meinen exe-Laeufen). Darin
  `integration_r36_schluessel` 92,8 s, `integration_r36_schluessel_befreiung` 75,4 s, `integration_r35_cut10f0`
  251,0 s, `integration_r35_cut1150` 519,9 s, `integration_elza_vollstart` 100,3 s, `integration_r35_inventar1050`,
  `integration_r34n_e_dokumente_bild`, `integration_r30_sicherung_laden/_bild`, `unit_r21_discard_wegwerfen`,
  `unit_r26_inventar`, `unit_r30_tuerse_bank`, `integration_door_lock`. Kein Fenster-Haken rot, nichts nachzufahren.
- Riegel je Punkt vorhanden und messend (gelesen): `unit_r36_schluessel_befreiung` faehrt sub02 am echten VM mit
  Spielschritt + Abfrage-FSM in main.c-Reihenfolge und prueft Zustand (Bild der Abfrage relativ zum Schliess-Bild von
  msg 9, Pad-Sperre/John bei der Abfrage, Inventar nach Ja/Nein, Flags bleiben, Szenenende nach der Antwort, ohne
  Schluessel keine Abfrage); `integration_r36_schluessel_befreiung` an der echten exe (discard.log `frage=8
  gegenstand=0x42`, nie `msg_aktiv=1` mit `frage=8`, `weg=1` + `pausepad=0` in F1995). Die uebrigen Riegel wie in
  Abnahme 0 (zu/auf/Wegwerfen/zurueck/ohne Strom/11B1, Aufnahme 10F0/10F1 + (4,0xEC), Roy + Zelle, Namen/CHECK/Icon/Bild).

## 4. RE-Gate (selbst disassembliert, richtige Binaerdatei)

| Behauptung (Nachbesserung 1) | Binaerdatei / Adresse | selbst gelesen | Urteil |
|---|---|---|---|
| RE2-Kette: Besitz vor Text, Fortsetzung, H4 | info/re2leon/PSX.EXE `re2_disasm.py dis 0x80051628 40` | @0x80051628 `jal 0x800696cc`, @0x80051634 `bltz s1,0x800516a0`, @0x80051640 `addiu a2,zero,5`, @0x8005164c `jal 0x8002fe38`, @0x80051654/58 `lui a0,0x225`/`jal 0x8005ba28`, @0x80051664 v0=0x80051718, @0x80051670 `sw v0,-32080(at)`, @0x80051690 `jal 0x8007730c`, @0x80051698 `j 0x800516f8` (Rohbytes `be 45 01 08`), @0x8005169c `addu v0,zero,zero`; Nicht-Treffer-Zweig @0x800516a0 `lui a0,0x216` .. @0x800516c0 `j 0x800516f8` / @0x800516c4 v0=0 | stimmt |
| H4: Uebergangs-Block wird uebersprungen | `dis 0x800516c8 20` | @0x800516c8 `addu v0,zero,zero`, @0x800516cc `addiu v1,zero,1`, @0x800516d4 `sb v1,-3256(at)` = 0x800df348, @0x800516dc..f4 `lw 0x800cfbdc` / `or 0xff000000` / `sw`; Epilog @0x800516f8 | stimmt (Sprung @0x80051698 landet hinter dem Block) |
| Fortsetzung LAB_80051718: Belegt-Schranke, Zaehler, Abfrage 9, Freeze, Nein -> 1 | `dis 0x80051718 100` | @0x800517f0 `andi v0,v0,0x80` / @0x800517f4 `bne v0,zero,0x80051870`, @0x80051810 `addiu v0,v0,-1`, @0x80051824 `bne v0,zero,0x8005185c`, @0x80051830 `addiu a2,zero,9` / @0x80051834 `jal 0x8002fe38` / @0x80051838 `lui a3,0xff00`, @0x80051844 `lui v1,0xff00` / @0x80051850 `sw v0,-1060(at)` (0x800cfbdc); Nein-Zweig @0x800517c4 `sb v1` mit v1=1 (@0x80051754) | stimmt |
| Pad-Sperre Set(2,7) = Bit 0x01000000 | info/Re1.5/PSX.EXE `re15_disasm.py dis 0x800304d0 24` | @0x800304f4 `lw v0,g_pauseflags`, @0x800304f8 `lui v1,0x100`, @0x800304fc `and`, @0x80030514 `andi v0,v0,0xf000` | stimmt |
| Stelle + Szene ROOM1211 | ROOM1211.RDT (Datei-Offsets, `scd_dump_room.py`) | @0x0226A `2b 09 00 00` (einziges Message_on 9 im Raum), @0x02582 `04 00 05 01 21 48 56 3d 16 05 00 00 26 4b 44 4a 57 00 33 3d 45 50 1a 04 01 01 63` ("Elza: John. Wait!"), @0x0259B `01 63`, @0x01F76 `22 04 fc 01`, @0x01F9C `22 02 07 01`, @0x0229A `22 02 07 00`, @0x0228A `32 00 ..` Pos_set John, @0x020F8 `3f 00 00 00 09 0a 3c 00` (Spur-L-Anker) | stimmt |
| (4,0xEC)/(4,0xFC) nach der Befreiung tot | Rohsuche `21|22 04 ec|fc` ueber alle 206 RDTs | Ck(4,0xEC) nur ROOM1211 @0x01DEA und ROOM3070 @0x03408 (Leon); Set(4,0xEC) ROOM1011 @0x00E2C, ROOM3070 @0x0349A; Ck(4,0xFC) nur 1211 @0x01E1A, Set nur 1211 @0x01F76; die Treffer mit Wertbyte 0x43 in 1260/1261/2000/2001/2010/2011/2020/2021 stehen in gleichem Datenkontext `d0 de ee 65 21 04 ec 43 00 cd 0e 44` (kein SCD) | stimmt — der Prison Key wird nach der Szene nie wieder gebraucht |
| Abfrage-Wortlaut | info/Re1.5/PSX/BIN/DEBUG.BIN (RAW @0x800C0000) @0x800C508B | `35 4b 51 00 40 4b 4a 3a 50 00 4a 41 41 40 00 50 44 45 4f 00 47 41 55 00 3d 4a 55 08 49 4b 4e 41 57 00 20 45 4f 3f 3d 4e 40 00 45 50 1b 03 ..` = "You don't need this key any / more. Discard it?" + Ja/Nein | stimmt, = Bild M1_mess F2310 |

Diff-Suche (`git diff 78a4a748..HEAD -- re15_port` ohne die erzeugte .inc) nach deferred/tunable/interim/for now/
faithful/plausib/TODO/FIXME/vorerst/geschaetzt/approx/Platzhalter/placeholder/getenv: **0 Treffer**; neue
getenv-Schalter in der Nachbesserung: **0**. Die zwei neuen Konstanten (`RE15_R36Q_ZELLE_RAUM 0x1211`,
`RE15_R36Q_ZELLE_MSG 9`) tragen @0x0226A/@0x0259B/@0x01F76/@0x01DEA/@0x01E1A/@0x01F9C/@0x0229A im Kopf von
`re15_schluessel_r36.h` und in den Commit-Messages 0f363920/9ca87339; die STELLE ist als PORT-WAHL gekennzeichnet, die
FORM mit RE2-Adressen belegt. Der Fix ERKLAERT den Befund: vorher fehlte (1211, msg 9) in beiden Quellen von
`re15_discard_besitz_vor_nachricht` (erzeugte Tabelle `gen/discard_sites.inc` mit 14 Stellen ohne 0x1211/0x42/0x45 —
selbst gelesen — und `re15_schluessel_r36_discard_stelle`), also keine Vormerkung; nachher steht die Vormerkung ab dem
Oeffnen von msg 9 (M1_mess F2115 `abfrage=1`) und die Abfrage im Schliess-Bild.

Zur Spanne "vorgemerkt" (neue Stelle hat Maske 0; die Tabellen-Stellen oeffnen laut Kommentar in
`item_discard_common.c` `re15_discard_reset` mit 0xFFFF0000 — nicht nachgezaehlt): ueber alle 100 Bilder
F2115..F2214 gemessen `pausepad=1`, `abfrage=1`, Elza unbewegt (-18446,-21834) (awk ueber `runs/M1_mess/discard.log`);
sub02 traegt keinen Tuer-/Raumwechsel-Opcode, die uebrigen Faeden von 1211 (sub01 Col_chg_set, sub04/05 Warte-
schleifen) auch nicht. Die Begruendung im Kopf von `re15_schluessel_r36.h` haelt.

## 5. Vertrag (VERTRAG.md) und Pfade

- Nachbesserung 1 nimmt KEINE neue Ressource: kein Bit, keine Nachrichten-Id, kein Ereignis, kein neuer Haken (nur
  ein zweiter Zweig in der eigenen Funktion `re15_schluessel_r36_discard_stelle`). Bits 82/83 (+93 Reserve mit
  Begruendung), IDs 28/29 in 11B0, Ereignis 25 unveraendert gegenueber Abnahme 0.
- Zensus der anderen r36-Zweige (`git diff 78a4a748...r36/<spur>`): kein anderer Zweig fasst
  `item_discard_common.c`, `gen/discard_sites.inc`, `re15_item_discard.h`, `schluessel_r36.c` oder
  `re15_schluessel_r36.h` an; keiner verwendet (9,82)/(9,83)/(9,93). Spur L (`r36/elza`) ankert in 1211 nur @0x020F8
  (`git diff ...r36/elza` gelesen) — disjunkt zur Stelle msg 9 @0x0226A.
- Pfade der Nachbesserung (`git diff 6af69ce2..HEAD --stat`): Dossier + Belegbild, `item_discard_common.c` (nur
  Kommentarzeilen), `schluessel_r36.c`, `re15_schluessel_r36.h`, `tests/integration/test_r36_schluessel.cmake`,
  `tests/unit/probes/r36_schluessel.cmake`, `tests/unit/test_r36_schluessel.c`. Kein release/, platform/android/,
  shared_assets/PSX/, keine tests/*/CMakeLists.txt. Der Gesamtzweig wie in Abnahme 0 (37 Dateien, Asset nur
  `shared_assets/RE2/TUERSE.VBS`).
- Nebenwirkungen beider Szenarien gemessen: Leon 10F0/11B0/11C0 unveraendert (2.2, 2.4), Elza 10F1/1011/1211 (2.1,
  2.3); Elzas ROOM11B1 und Leons ROOM1210 sind von der neuen Stelle nicht betroffen (Raum-Id 0x1211 exakt, Riegel
  `befreiung` B prueft (1210, msg 9) und (1211, msg 8) als Nicht-Stellen).

## 6. Urteil je Nutzer-Punkt (gegen den Wortlaut, AUFTRAG.md Z. 45 / Z. 49)

**Punkt 1 (Leon: Tuer zum Parking Lot ROOM11C0 bleibt verschlossen, bis ein Parking-Lot-Schluessel in ROOM10F0
gefunden ist; RE2-Schluessel mit Parkplatz-Optik) — ERFUELLT.**
Ueber echte Tuerwege gemessen: der Schluessel liegt in 10F0 und ist aufnehmbar (L1); ohne ihn ist die Tuer zu —
direkt (L2) UND nach der Sherry-Szene im selben Besuch (H5_w3, Luecke der Abnahme 0 geschlossen) — mit RE2-
Verschlossen-Ton und "It's locked. / The door is marked "Parking Lot."", kein Raumwechsel; mit ihm "You've used the
Parking Lot Key." + RE2-Aufschliess-Ton (Satz 4, 0x4f3) + Wegwerf-Abfrage, der naechste Druck fuehrt nach ROOM11C0
(L3). ROOM11C0 hat nur diese eine Tuer (Door_aot_set @0x01712 -> 0x1B), die Sperre gatet den Parkplatz also
vollstaendig. Modell/Icon/Bild = RE2 "P. Room Key" (schwarzer Kunststoffkopf, rotes Emblem, Metallbart): im
Aufnahme-Modal als Autoschluessel erkennbar. Die Nachbesserung hat daran nichts verschlechtert (L3).

**Punkt 2 (Elza: Prison Key nicht mehr in ROOM1011, sondern dort, wo Leons Parking-Lot-Schluessel liegt;
RE2-Modell fuer einen Prison Key) — ERFUELLT.**
Roy gibt nur noch den Zettel (R1: einzige Nachricht msg 18, 0 Bilder msg 19/20); der Prison Key liegt in 10F1 an exakt
Leons Stelle (8050,-1460,2700), Zone Slot 15, und wird ueber das Modal aufgenommen -> Inventar + (9,83) + (4,0xEC)
(E1); ohne ihn bleibt die Zelle zu (Z0, msg 10), mit ihm laeuft ueber den echten Tuerweg die Befreiungsszene (M1_*).
Modell/Icon/Bild = RE2 "Cabin Key" (grosser Eisenring mit Bartschluessel = Schliesser-Schluesselbund). Der Mangel M1
der Abnahme 0 ist behoben und an der exe gemessen: nach "Elza: John. Wait!" fragt das Spiel "You don't need this key
any more. Discard it?", Ja -> Statusschirm leer, Nein -> Schluessel bleibt, die Szene endet in beiden Faellen 3 Bilder
nach der Antwort und Elza ist frei (2.1).

**Gates:** RE-Gate haelt (Abschnitt 4, alle Stichproben stimmen, Fix erklaert den Befund), Vertrag haelt
(Abschnitt 5), Tests gruen (Abschnitt 3: 5/5 Unit, 120/120 gezielt inkl. beider Integrations-Laeufe der Spur).
-> **BESTANDEN.**

## 7. Maengel

Keine.

## 8. Hinweise (fuer Orchestrator/Nutzer, nicht blockierend)

- **H1 Sichtbarkeit des Weltmodells (unveraendert, Nutzerentscheidung).** 9 bzw. 10 Bildpunkte in Cut 0 (Abnahme 0,
  Dossier 3.2/N1.6). Gewicht neu: ROOM11C0 liegt auf Leons Pflichtweg (ablauf_leon.md L19, Ada im Parkplatz,
  Set(3,0x43) sub03 @0x01B02) — der Parking Lot Key ist damit ein Pflicht-Fund, den der Spieler nur durch Absuchen der
  Konsole findet. Eine Vergroesserung waere eine Abweichung vom RE2-Modell 152 x 76.
- **H3 Zusammenfuehren:** `shared_assets/RE2/TUERSE.VBS` (binaer, geteilt) bei einer Erweiterung durch Spur G mit
  `tools/re2_door_se_cut.py` neu erzeugen; Hakenzeilen in `scd_vm.c`/`scd_room_setup.c` sind Textkonflikt-Kandidaten.
- **H6 (nicht Spur Q):** Statusschirm zeigt im Elza-Spiel Leons Ausweis (auch in meinen Laeufen M1_ja/M1_nein/E1,
  `inv_render_pc.c` laedt immer `STPIC_00.TIM`) — an den Eigentuemer des Statusschirms.
- **H7 Elzas Pflichtweg (Folge der Nutzer-Vorgabe, nicht gemessen):** die Befreiung von John (ROOM1211) steht in
  Elzas Pflichtkette (ablauf_elza.md Z. 88: (4,0xEC) -> (3,0x8B) -> (4,0xFC)/(4,0x60) -> (4,0xF0) ...). Der Prison Key
  liegt jetzt in 10F1 hinter dem Kartenleser 10D1 ((3,0x32), Blue Keycard aus 1111 hinter dem Stimmschloss 1101 und
  dem Minidisc-Player 1201) — dieser bisherige Nebenweg wird fuer Elza Pflicht. Ich habe ihn nicht als Durchlauf
  gefahren (alle Laeufe mit Kartenwerkzeug ab dem jeweiligen Vorraum). Empfehlung: einmal Elza-Durchlauf 1201 -> 1101
  -> 1111 -> 10D1 -> 10F1 -> 11E1 -> 1211 an der exe, bevor die Runde ausgeliefert wird.
- **H8 Story-Notiz (nur zur Kenntnis):** in der 10F0-Szene schickt Leon Marvin und Ada voraus "to the parking lot";
  die Tuer dorthin ist fuer Leon danach zu, bis er den Schluessel findet. So bestellt (Nutzer-Vorgabe); falls gewuenscht,
  liesse sich das mit einem Satz erklaeren (z.B. Marvin schliesst hinter sich ab) — keine Aenderung verlangt.
- **Randfall Item-Box (Dossier N1.6) an der exe bestaetigt:** Prison Key nicht im Inventar, (4,0xEC) gesetzt -> Szene
  laeuft, keine Abfrage, kein Haenger (M1_ohne). Gleiches Verhalten wie die RE1.5-Kartenleser im Port.

Aufraeumen: Messkopie `re15_port/build/platform/pc/re15_pc_Qabn1.exe` nach der Abnahme entfernt; kein Code geaendert;
committet wird nur diese Datei.

