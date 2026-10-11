# Runde 36 — Spur Q "schluessel": Abnahme 0 (unabhaengig)

Stand: Zweig `r36/schluessel` HEAD 89ac43f2 (Basis master 78a4a748), Baum `.claude/worktrees/r36_schluessel`.
Gebaut mit `bash re15_port/tools/local_build.sh configure` + `build` (LOCAL-BUILD-OK, `ninja: no work to do` =
die exe vom 23:59 entspricht HEAD). Messordner (ausserhalb des Baums): Scratchpad `Q_abn0/`.

**Ergebnis: NICHT BESTANDEN** — Punkt 1 erfuellt, Punkt 2 teilweise (Mangel M1: Prison Key ohne
Wegwerf-Abfrage, bleibt im Inventar). RE-Gate, Vertrag und gezielte Tests (116/116) halten. Details 6-8.

## 0. Gelesen
- AUFTRAG.md Z. 45 / Z. 49 (Wortlaut), Nachtrag 14:00 (Spur Q nur Dossier-Hintergrund), VERTRAG.md 1.1-1.6.
- Dossier `Q_schluessel.md`, `git log master..HEAD` (10 Commits), `git diff 78a4a748..HEAD` (35 Dateien).

## 1. Bau und Riegel der Spur

- `local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `local_build.sh build` -> `ninja: no work to do`,
  `LOCAL-BUILD-OK (build)` (exe 23:59 = HEAD-Stand, letzte Commits nur Riegel/Dossier).
- `ctest -R "^unit_r36_schluessel"` -> `100% tests passed, 0 tests failed out of 4` (raum/tuer/zelle/namen).
  Gelesen: die Riegel fahren die echten Engine-Wege (scd_room_reenter + Installer, re15_aot_scan mit
  Aktionsdruck, scd_event_fire, Item-Modal, Wegwerf-FSM) und pruefen Zustand (Bits, Inventar, Slot-Typ,
  Raumwechsel), nicht nur Konstanten.

## 2. Messungen an der echten exe (eigene Laeufe)

Werkzeug: Messkopie `re15_port/build/platform/pc/re15_pc_Qabn0.exe`, Spielstand mit dem Kartenwerkzeug
`probe_r36_schluessel_karte` + CONTINUE, Tasten `RE15_PAD_AT` (Bildzaehler beginnt je Raum neu bei 0 —
gemessen: Tuer in 10D0 bei F45, naechstes Spielbild ist F1 in 10F0), `RE15_STATE_LOG`, `RE15_FRAMEDUMP`
(RE15_WINDOW_SCALE=1). Laufordner: Scratchpad `Q_abn0/runs/<lauf>/` (debug.log, state.log, fd_*.ppm, sheet.png).
Alle Laeufe gehen ueber den ECHTEN Tuerweg in den Zielraum (DOOR FIRE + Tuersequenz + Raumladen), nicht per Sprung.

### 2.1 Leon: Parking Lot Key in ROOM10F0 (Lauf L1/L3)
Karte `10D0 800 -5600 1024 flag:3:0x32 flag:9:71 flag:3:94` (vor der Funkraum-Tuer, Kartenleser offen,
10F0-Szene gesehen). Druck F45 -> `[aot] DOOR FIRE slot=0 ... spawn=(8400,0,-350)` -> `[room] PC loaded
room10f0.rdt` -> `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei
(8050,-1460,2700), Zone Slot 15` (Installer greift auch auf dem Tuerweg scd_room_reenter).
L3: R F60-70 (rot 2048 -> 3104, +96/Bild gemessen), U F75-160 -> Leon steht an der Konsole (8658,2032);
A F175 -> Aufnahme-Modal mit RE2-Bild (P. Room Key), "Will you take the Parking Lot Key? > Yes No"
(fd_000185..285), A -> `[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)`. Modell danach weg.
**Sichtbarkeit des Weltmodells gemessen** (fd_000165 vor / fd_000345 nach der Aufnahme, gleiche Kamera Cut 0):
Unterschied am Schluesselort = 9 Bildpunkte bei (96..102, 169..170), Werte vorher (5..64, 5..54, 5..54) — ein
dunkler Strich, das rote Emblem ist nicht als Rot erkennbar (hoechster Wert (64,54,54)). Bild:
`runs/L3_aufnahme/zoom_key.png`. Der Schluessel ist im Raum fuer den Spieler praktisch nicht zu sehen
(deckt sich mit Dossier 3.2/6 "9 Bildpunkte").

### 2.2 Leon: Parkplatz-Tuer ROOM11B0 ohne Schluessel (Lauf D1_ohne)
Karte `1180 4900 28800 0 flag:4:0xF3 flag:3:0x82` (Garage, Strom an, Sherry-Szene erledigt). Druck F45 ->
`DOOR FIRE slot=3 ... spawn=(-26483,0,-18094)` -> room11b0 -> `[schluessel] ROOM11B0 Slot 1 (Tuer -> ROOM11C0)
gesperrt: Ereignis 25, Flag (9,93) = 0`. R F60-69 (rot 960), U F75-200: Leon laeuft bis (-25601,-26252) vor die
Tuer. A F215 -> `[schluessel] ROOM11B0: Parkplatz-Tuer verschlossen (kein Parking Lot Key im Inventar)`, kein
DOOR FIRE, state.log msg id=28; Bild: "It's locked." + Seitenpfeil, Seite 2 "The door is marked / "Parking
Lot."" (`runs/D1_ohne/sheet.png`, `zoom_text.png`). Bis Bild 340 in 11B0 (EXIT_AT), kein Raumwechsel.

### 2.3 Leon: Parkplatz-Tuer mit Schluessel (Lauf D2_mit, Ton-Mitschnitt)
Karte wie 2.2 + `flag:9:82 give:0x45:1`, Audio an (`RE15_AUDIO_CAP_SYNC`, `RE15_SE_DEBUG=1`). A F215 ->
`[se] Stimme: se=4 ... pitch=0x4f3 (13641 Hz)` (TUERSE Satz 4 = AUF_KEY, Tonhoehe = Dossier 2.6) und
`[schluessel] ROOM11B0: Parking Lot Key benutzt -> Tuer offen, Flag (9,93) = 1`; Bild "You've used the
Parking Lot Key." (Name gruen), dann "You don't need this key any more. Discard it? > Yes No" (fd_000405),
A F415 = Ja (`[se] se=6` Bestaetigung), A F440-452 -> `[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)` ->
`[room] PC loaded room11c0.rdt` -> `EXIT_AT: Bild 5 in Raum 11c0`. Bild: `runs/D2_mit/sheet.png`.
RE2-Reihenfolge selbst nachdisassembliert (Abschnitt 3): der Schluessel-Zweig springt nach dem Schlossbit
(@0x80051690) direkt in den Epilog @0x800516f8 und laesst den Uebergangs-Block @0x800516cc (sb 1 ->
0x800df348) aus -> kein Durchgang beim Benutzungs-Druck. Der Port bildet das ab.

### 2.4 Elza: Prison Key in ROOM10F1 (Lauf E1, E_sicht_*)
Karte `10D1 800 -5600 1024 elza flag:3:0x32` (`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`). Druck F45
-> `DOOR FIRE slot=0 ... spawn=(8400,0,-350)` -> `room10f1.rdt` -> `[schluessel] ROOM10F1: PRISON KEY (Item
0x42, Bit (9,83)) Prop obj 4 bei (8050,-1460,2700), Zone Slot 15` = dieselbe Stelle wie Leons Schluessel.
Gleiche Eingabe wie L3 -> Modal mit RE2-Bild Cabin Key (Eisenring + Bartschluessel), "Will you take the Prison
Key? > Yes No" -> `[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)` + `Prison Key genommen -> Flag
(4,0xEC) = 1 (Zelle ROOM1211)` (`runs/E1_aufnahme/sheet.png`).
Sichtbarkeit (Spieler am Eintrittspunkt, Bild F40 mit / ohne Bit (9,83), sonst gleich): **10 Bildpunkte**
Unterschied bei (95..103, 168..171), Werte (10..36, 10..34, 10..34) — dunkelgraue Punkte an der hinteren
Konsolenkante (`runs/E_sicht_zoom.png`, `runs/E_sicht_full.png` mit roter Markierung). Ring und Schluessel
sind als Form nicht erkennbar.

### 2.5 Elza: Zelle ROOM1211 (Laeufe Z0/Z1 ueber die Tuer "Prisons" aus 11E1, Z2 Szene)
- Z0 ohne Schluessel: Karte `11E1 9100 5500 0 elza flag:3:0x8B flag:4:0x24`, Druck F45 -> `DOOR FIRE slot=3`
  -> room1211, AOT-Liste (`RE15_AOT_DUMP`): `8 MESSAGE Mitte(-21533,-21654) ... ev=10` (= Original "It's
  locked. It seems that there is someone inside..."), kein EM49, kein Slot 6.
- Z1 mit Prison Key (Karte + `flag:4:0xEC flag:9:83 give:0x42:1`): gleicher Tuerweg -> `[enemy] EM49 loaded`,
  `6 AUTO_EVENT Mitte(-19355,-19136) halb(2212,775) ev=2` (= ROOM1211 sub00 @0x01E22 -> sub02), kein Slot 8.
- Z2 (Karte in 1211 vor dem Ausloeser, U F40-100): Elza laeuft in Slot 6 -> Befreiungsszene sub02 startet
  (Kamera 7/8, John auf der Pritsche, msg 0 "Elza: Hey! You must be John right? / Get up now!",
  `runs/Z2_szene/sheet.png`).

### 2.6 Elza: Roy in ROOM1011 (Lauf R1, ueber die Tuer aus 1021)
Karte `1021 -2950 -16700 1024 elza flag:4:0xF1 flag:4:0xFA flag:3:0x74` (Roy-Dialog gelaufen). Druck F45 ->
`DOOR FIRE slot=1 ... spawn=(3650,0,6900)` -> room1011, `EM47 loaded`; R F60, U F65-130 -> Elza vor Roy
(-1055,7663); A F145 -> `[schluessel] ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)`,
state.log: nur msg id 18 (185 Bilder), **0 Bilder mit id 19/20**; Bild "He has passed out from his injuries." /
"He is holding a slip." (`runs/R1_roy/sheet.png`). Roy gibt den Prison Key nicht mehr.
Selbst gelesen (ROOM1011.RDT): @0x00A52 `2c 05 03 31 .. ff 00 18 06` (Slot 5 sce 3 -> sub06, Rechteck x -2400..
-1400 z 6950..7950), @0x00A78 `46 05 03 31 ff 00 18 07` (-> sub07), @0x00DC0 `46 05 03 31 ff 00 18 06`,
@0x00E2C `22 04 ec 01` Set(4,0xEC), @0x00E30 `22 03 78 01` Set(3,0x78). sub06 wird nur ueber Slot 5 erreicht
(kein Evt_exec sub06 im Raum, census `story_view.py 1011`) -> der Haken in scd_event_fire faengt jeden Weg.
(3,0x78) liest nur ROOM1011 sub00 @0x00A74 (flag_xref.txt) -> kein Folgeschaden durch das fehlende Setzen.

### 2.7 Leon: erster Eintritt in 10F0 MIT Szene, danach Aufnahme (Laeufe S1/S2)
Karte `10D0 1900 -7000 2048 flag:3:0x32 flag:4:0xF7` ((9,71)=0 -> Szene steht aus). Druck F45 -> Tuer ->
`[cut10f0] ROOM10F0: Szene gestartet (Ereignis 20, Faden 10, Flag (9,71)=0)` und im selben Raumaufbau
`[schluessel] ROOM10F0: PARKING LOT KEY ... Zone Slot 15`; Szene laeuft durch (`[cut10f0] Szene zu Ende:
Kartenhinweis ROOM11C0 -> ROOM1150 angefordert`, Hinweis schliesst F2988). Leon danach (4792,11354).
S2: drei Wegstuecke (L/U, gemessen im state.log: (4792,1379) -> (8418,1477) -> Konsole (8522,2032)), A F3285 ->
`[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)`. Szene und Schluessel vertragen sich; der
Schluessel liegt nach der Szene noch und ist aufnehmbar.

### 2.8 Statusschirm / CHECK (Laeufe C45, C42; Harness RE15_INV_CHECK_SHOT)
`[inv] CHECK-Foto Item 0x45: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen` bzw. `Item 0x42`;
Bild `runs/check_beide.png`: RE2-Foto P. Room Key bzw. Cabin Key, Text beginnt "It looks unique." / "I can
access the" (Harness-Bild 140 faengt mitten im Tippen). Glyphen von k_desc_park/k_desc_prison selbst
dekodiert: "It looks unique. / Now I can access / the parking lot." und "I can access the / prison cells /
with this." Icons/Namen im Raster: Dossier-Bild `q_statusschirm_namen_icons.png` (Name "Parking Lot Key" /
"Prison Key").

### 2.9 Leon: Rueckweg nach dem Aufschliessen (Lauf D3_zurueck)
Karte `11C0 -25100 19200 1024 flag:4:0xF3 flag:3:0x82 flag:4:0x40 flag:9:93 flag:9:82`. Druck F45 ->
`DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)` -> room11b0, AOT-Liste `1 DOOR Mitte(-25480,-28190) ... ->
ROOM11C0` (keine `[schluessel] ... gesperrt`-Zeile); Umdrehen + Druck -> `DOOR FIRE slot=1` -> room11c0, und
noch einmal hin und zurueck (Zeilen 80..284 im debug.log). Nach (9,93) bleibt die Tuer eine normale Tuer.

### 2.10 Leon: erster Eintritt 11B0 mit Strom MIT Sherry-Szene (Lauf D4_sherry, nur teilweise)
Karte `1180 4900 28800 0 flag:4:0xF3` ((3,0x82)=0). Tuer -> 11B0: `[schluessel] ... gesperrt` im Raumaufbau,
`EM4B loaded`, Szene sub04 laeuft (`[msg] room=11b0 id=0` "Leon: H-hey!", Leon wird bis (-2447,-21983)
gefuehrt). Den Rueckweg zur Parkplatz-Tuer habe ich in drei Anlaeufen nicht gefunden (Wand bei x -20922 fuer
z -23218..-26269, Hindernis bei x -5413) — der Druck auf die Tuer NACH der Szene ist damit an der exe nicht
gemessen. Statisch: sub04/sub05 enthalten kein Aot_set/Aot_reset (census `story_view.py 11B0`), sub01 fasst
Slot 1 nur ohne Strom an (@0x011F6/@0x01216 selbst gelesen); D1 zeigt, dass der per-Bild-Lauf von sub01 die
Sperre nicht aufhebt. Restrisiko gering.

### 2.11 Elza: Prison Key nach der Befreiung (Lauf Z3_nach_szene)
Karte in 1211 vor dem Ausloeser, `leer give:0x42:1 give:0x15:15`. Szene sub02 laeuft bis msg 9 ("Elza: John.
Wait!", F1799), danach START F2400 -> Statusschirm (fd_002440/002470): **der Prison Key liegt weiter im
Inventar** (Ring-Icon auf Platz 0). Kein "You've used..."-Text, keine Wegwerf-Abfrage; der Platz bleibt bis zur
Item-Box belegt (Dossier 6 nennt das selbst als OFFEN).

### 2.12 Ton im Verschlossen-Fall (Lauf D1_ton, Audio an)
Gleicher Weg wie 2.2 mit `RE15_AUDIO_CAP_SYNC` + `RE15_SE_DEBUG=1`: beim Druck `[se] Stimme: se=0 ... note=66
fine=42 center=85 vol=110 pitch=0x589 (15256 Hz)` = TUERSE Satz 0 ZU_A (Sollzeile in `unit_r30_tuerse_bank`:
note 66, shift 42, center 85, vol 110, pitch 0x589), dann `Parkplatz-Tuer verschlossen`. RE2-Vorbild
@0x800516a0/a4 (Se_on 0x216) -> Port-Wahl "Art M" wie jede mechanische Port-Tuer (Dossier 2.6).

## 3. RE-Gate (selbst disassembliert, richtige Binaerdatei)

| Behauptung | Binaerdatei / Adresse | selbst gelesen | Urteil |
|---|---|---|---|
| RE1.5-Tuer-Handler ohne Schluesseltest | info/Re1.5/PSX.EXE, Tabelle @0x8007469c [2] = 0x800430bc | `ori v0,zero,1 / sw a0,-13912(at)=0x800ac9a8 / sb v0,0x800b5359 / lw/or 0xff000000/sw 0x800aca40 / jr ra` | stimmt |
| RE2 prueft Inventar, Text 5, Ton 0x225, Fortsetzung, Schlossbit | info/re2leon/PSX.EXE @0x80051514.. | @0x800515a8 `lbu a1,15(s2)`, @0x800515b0 `andi 0x80`, @0x80051628 `jal 0x800696cc`, @0x80051634 `bltz s1`, @0x80051640 `addiu a2,zero,5`, @0x80051654/58 `lui a0,0x225 / jal 0x8005ba28`, @0x80051670 `sw v0` (v0 = 0x80051718), @0x80051690 `jal 0x8007730c`, @0x80051698 `j 0x800516f8`; fehlt: @0x800516a0 `lui a0,0x216`, @0x800516b4 `addiu a2,s0,-76` | stimmt; Kommentar "v0 = 0 = kein Durchgang" ist ungenau (v0 ist in allen Zweigen 0) — kein Durchgang, weil der Sprung den Uebergangsblock @0x800516cc (`sb 1,0x800df348` + Freeze) auslaesst. Verhalten richtig. |
| RE2-Texte 5 / 0x13 | RE2 @0x8009F368 Offsettabelle (u16): [5]=281, [0x13]=751; Basis 0x8009EFCC | @0x8009F0E5 `35 4b 51 00 44 3d 52 41 00 51 4f 41 40 00 50 44 41 fc f9 01 ...` "You have used the <Name>."; @0x8009F2BB `fc 25 50 3a 4f 00 48 4b 3f 47 41 40 01 fd 00 30 44 41 ...` "It's locked. / The door is marked "Power Room"." | stimmt |
| Tuer 11B0 + Strom-Bedingung | ROOM11B0.RDT @0x00FAA, @0x011F2, @0x01216 | `3b 01 02 31 00 00 96 97 0c 8b c4 09 ac 0d 41 9d 00 00 74 43 fb 11 00 1c 00..`; `06 00 40 00 21 04 f3 00`; `46 01 01 31 0f 00 ff ff 00 00` | stimmt |
| Roy/Zelle | ROOM1011.RDT @0x00A52/@0x00A78/@0x00DC0/@0x00E2C/@0x00E30, ROOM1211 sub00 | s. 2.6; 1211 per AOT-Liste der exe (2.5) | stimmt |
| Item-Eigenschaften / Schluessel-Klasse | PSX.EXE @0x80074DA8+Id*12; @0x80049124 | 0x36/0x42/0x45/0x47: `01 00 00 00 88 4c 07 80 00 00 00 00`; `sltiu v0,v0,0x22` | stimmt |
| Aktions-Scan / 620 voraus | @0x80042f94; @0x80042bd0 | `j 0x80043028` nach dem ersten Treffer; `ori v0,zero,0x26c` | stimmt |
| Obj_model_set Flags / ITPS-Block / sce-3-Nutzlast | @0x80040998; @0x8001e414; @0x80043100 | `ori v0,v0,0x1`; `ori v0,zero,0x3000`; `lbu a1,3(v0)` | stimmt |
| RE2-Bytes im Port | info/re2leon PL0/RDT/ROOM6150 @0x004FBC (236 B), @0x030B74 (34848 B); ROOM1050 @0x001558 (140 B), @0x00C650 (8736 B); ITEMALL.PIX @0x1BD50/@0x19C80; ITPS.ITP @0x11D000/@0x108000 | md5 der Dateibytes = md5 der Arrays in gen/schluessel_r36.inc (3c9176ee / 0d43d30f / 569def98 / be1f46e9); ITPS-Bloecke weichen von RE2 NUR in +0x0E und +0x218..0x21B ab (= RE1.5-Rechtecke der Bloecke 0x45/0x42: `0000e901` / `40030001`) | stimmt |

Diff-Suche (`git diff 78a4a748..HEAD -- re15_port`, ohne die erzeugte .inc) nach deferred/tunable/interim/
for now/faithful/plausib/TODO/FIXME/vorerst/geschaetzt/approx/Platzhalter: **0 Treffer**; neue getenv-Schalter:
**0**. Jede Konstante in re15_schluessel_r36.h traegt @0x/Datei-Offset/Zensus oder ist als PORT-WAHL (Lage,
Rechteck, Texte, Modellwahl) bzw. VERTRAG (Bits 82/83, IDs 28/29, Ereignis 25) gekennzeichnet; Bit 93 aus der
Reserve mit Begruendung (Dossier 3.4, RE2-Schlossbit @0x80051690).

## 4. Vertrag (VERTRAG.md) und Pfade

- `git diff 78a4a748..HEAD --stat`: 35 Dateien (der `-15` an AUFTRAG.md in `git diff master` kommt vom
  Master-Nachtrag 836023f5 fuer Spur J, nicht von diesem Zweig).
- Bank 9: 82 (Parking Lot Key genommen), 83 (Prison Key genommen) = Zuteilung; **93** aus der Reserve mit
  Begruendung (Dossier 3.4: RE2-Schlossbit getrennt von Besitz, @0x80051690). Zensus der anderen r36-Baeume
  (`git diff 78a4a748` je Baum, Muster (9,93)/_BIT 93/flag(9,93)): **kein** weiterer Nutzer von 93, 82, 83 oder
  Ereignis 25.
- Nachrichten-IDs 28/29 nur in ROOM11B0 (Zuteilung 28..29); keine Sprachdatei `synchro/STAGE1/room11B0/main28|29`
  (vorhanden main00..12) -> keine Fremd-Vertonung. Ereignis 25 (Zuteilung), nur in ROOM11B0.
- Kein Pfad unter release/, platform/android/, shared_assets/PSX/, keine tests/*/CMakeLists.txt; Registrierung
  ueber `tests/unit/probes/r36_schluessel.cmake`. Asset: `shared_assets/RE2/TUERSE.VBS` (geaendert 29976 ->
  42808 B; im Dossier 7 fuers Paket-Gate genannt; Saetze 0..3 laut `unit_r30_tuerse_bank` unveraendert).
- Gemeinsame Dateien: Haken 1-5 Zeilen, Installer-Zeile am Blockende von scd_room_setup.c mit "Runde 36 Spur Q";
  einzig der Prop-Lader in platform/pc/main.c ist ein 6-Zeilen-Block (Muster des r35-Blocks direkt davor) —
  vertretbar.
- Fremde Riegel angepasst (gelesen): `probe_r26_inventar.c` B4 nimmt 0x42/0x45 aus und B4c verlangt dafuer den
  Port-Namen bei sonst bitgleichem Lauf (strenger, nicht weicher); `probe_r35_cut10f0_karte.c` `vor11c0` setzt
  (9,93) (Teil E prueft weiter den Durchgang, die Sperre prueft die eigene Spur); `test_r30_tuer.c` Bank-Teil
  auf 6 Saetze mit exakten Soll-Zeilen fuer AUF_KEY/AUF_HIER erweitert.

## 5. Tests (gezielt, kein Voll-Lauf)

- `ctest -R "^unit_r36_schluessel"` -> 4/4 gruen.
- Gezielter Lauf, -R woertlich (Scratchpad `Q_abn0/regex.txt`):
  `r36_schluessel|r21_discard|r30_tuer|r31_tuer_ton|door|unit_inv_|unit_item_|itembox|pri_inventar_riegel|r26_inventar|r35_inventar1050|item_name_census|android_n1_inventar|sicherung|r35_cut10f0|r22_10f0|r35_inhalt|r34n_e_dokumente|r30_irons_tisch|unit_msg|action_msg_gate|r33_tuer|rolltor|adaruf|r35_cut1150|fenster|elza|keydoor|aot_sce_census|flag_census|itps|prompt|zgirl_tuer|zgirl_selbsttueren|10f0|11b0|1011|1211`
  (`-j 2`, unter Fremdlast anderer Agenten) -> **`100% tests passed, 0 tests failed out of 116`** (745,77 s),
  darunter `integration_r36_schluessel` 87 s, `integration_r35_cut10f0` 246 s, `integration_r35_cut1150` 525 s,
  `integration_elza_vollstart` 100 s, `integration_r35_inventar1050`, `integration_r34n_e_dokumente_bild`,
  `unit_r26_inventar`, `unit_r30_tuerse_bank`. Kein Fenster-Haken rot, kein Nachfahren noetig.
- Riegel je Punkt vorhanden und messend: Tuer zu/auf/Wegwerfen/zurueck/ohne Strom/11B1 (`unit_..._tuer`,
  `integration_..._schluessel` zu/auf), Aufnahme 10F0/10F1 + (4,0xEC) (`_raum`, integration leon/elza), Roy +
  Zelle (`_zelle`), Namen/CHECK/Icon/Bild (`_namen`, integration check45/check42).

## 6. Urteil je Nutzer-Punkt (gegen den Wortlaut)

**Punkt 1 (Leon: Parkplatz-Tuer 11B0 -> 11C0 zu, bis der Parking-Lot-Schluessel in 10F0 gefunden ist; RE2-Schluessel
mit Parkplatz-Optik) — ERFUELLT.**
Gemessen ueber echte Tuerwege: Schluessel liegt in 10F0 (auch nach der Erst-Szene) und ist aufnehmbar (2.1, 2.7);
ohne Schluessel ist die Tuer zu (Text + RE2-Verschlossen-Ton, kein Raumwechsel; 2.2, 2.12); mit Schluessel
"You've used the Parking Lot Key." + RE2-Aufschliess-Ton + Wegwerf-Abfrage, naechster Druck -> ROOM11C0 (2.3);
danach normale Tuer in beide Richtungen (2.9); Elzas 11B1 unberuehrt (Riegel). Modell = RE2 "P. Room Key"
(schwarzer Kunststoffkopf, rotes Emblem): im Aufnahme-Modal, im Raster und im CHECK-Foto klar erkennbar.
Einschraenkung als Hinweis H1 (Weltmodell 9 Bildpunkte).

**Punkt 2 (Elza: Prison Key nicht mehr in 1011, sondern an der Stelle von Leons Schluessel; RE2-Modell) — TEILWEISE.**
Erfuellt: Roy gibt nur noch den Zettel (2.6), der Prison Key liegt in 10F1 an exakt derselben Stelle
(8050,-1460,2700) (2.4), Aufnahme setzt (4,0xEC) wie Roys altes sub06, die Zelle 1211 oeffnet ueber den echten
Tuerweg nur mit Schluessel und die Befreiungsszene laeuft (2.5). Modell = RE2 "Cabin Key" (grosser Eisenring mit
Bartschluessel). NICHT erfuellt: der neue Schluessel-Gegenstand haelt die stehende Nutzer-Regel fuer benutzte
Schluessel nicht ein (Mangel M1) — er bleibt nach der Befreiung dauerhaft im Inventar (2.11).

**Gates:** RE-Gate haelt (Abschnitt 3, alle Stichproben stimmen), Vertrag haelt (Abschnitt 4), Tests gruen
(Abschnitt 5). Wegen M1 -> **nicht bestanden**.

## 7. Maengel

- **M1 Prison Key: keine Wegwerf-Abfrage nach der Benutzung, bleibt dauerhaft im Inventar.**
  Messung: Lauf Z3_nach_szene (2.11) — Befreiungsszene ROOM1211 sub02 laeuft bis msg 9 ("Elza: John. Wait!"),
  danach Statusschirm: Ring-Icon (Item 0x42) weiter auf Platz 0, keine Abfrage im debug.log/Bild.
  Massstab: stehender Nutzerauftrag Runde 21 (`analysis/befunde_2026-09-21/discard-umsetzung.md`, woertlich:
  "bei KEy Items, die verwendet wurden, und nicht mehr im Spiel danach benoetigt werden genauso wie in Resident
  Evil 2 fragst - This Item is not used anymore - drop it? Yes No"); RE2 @0x80051670 (Fortsetzung) / @0x80051834
  (Abfrage). Der Prison Key ist ab dieser Spur ein Schluessel-Gegenstand, wird in der Szene benutzt (Elza
  schliesst die Zelle auf; Spur L haengt dort den Aufschliess-Ton ein) und danach nicht mehr gebraucht: (4,0xEC)
  liest bei Elza nur ROOM1211 sub00 @0x01DEA (der zweite Leser ROOM3070 @0x03408 ist Leons Raum), und sub02
  @0x01F76 setzt (4,0xFC)=1, das sub00 @0x01E1A abfragt — die Zelle loest nie wieder aus (flag_xref.txt).
  Im Port prueft kein Skript den Gegenstand selbst (Keep_Item_ck 0x5E ist ein Ueberspringer, scd_vm.c:5019
  `op_keep_item_ck { t->pc += 4; return 1; }`). Der Parking Lot Key derselben Spur bekommt die Abfrage (2.3), der Prison Key
  nicht — das Dossier fuehrt das selbst unter OFFEN als Frage an den Nutzer, obwohl die Regel steht.
  Nachpruefbar: Lauf Z3 wiederholen (Karte `1211 -19355 -17300 1024 elza flag:3:0x8B flag:4:0x24 flag:4:0xEC
  flag:9:83 leer give:0x42:1`, U F40-100, START F2400) -> Soll: "You don't need this key any more. Discard it?"
  nach dem Ende der Szene, Ja -> 0x42 weg; (4,0xEC)/(4,0xFC) bleiben. Weg ohne neue Nachrichten-ID: dieselbe
  Port-Benutzungsstelle wie 11B0 msg 29 (`re15_schluessel_r36_discard_stelle`), hier (ROOM1211, msg 9) = der vom
  Bau-Agenten selbst vorgeschlagene Ort am Szenenende; mit Spur L abstimmen (gleiche Szene). Riegel dazu
  (Unit: Szene bis msg 9, Abfrage fuer 0x42, Ja -> weg; integration: Lauf wie oben).

## 8. Hinweise (fuer Orchestrator/Nutzer, nicht blockierend)

- **H1 Sichtbarkeit des Weltmodells.** Gemessen 9 Bildpunkte (Leon, (96..102,169..170), dunkel, Rot nicht
  erkennbar) bzw. 10 Bildpunkte (Elza) in der Eintrittskamera Cut 0. Ursache: RE2-Modell 1:1 (152 x 76) und
  10F0-Kameras weit weg (Dossier 3.2: keine Moebelplatte naeher als vz ~2600, Bestwert 12 px). Ohne
  Hinweistext findet ein Spieler den Schluessel nur durch Absuchen. Eine Vergroesserung waere eine Abweichung
  vom RE2-Modell -> Nutzerentscheidung (Dossier 6 fragt bereits).
- **H2** (entfallen, jetzt Mangel M1).
- **H3 Zusammenfuehren:** `shared_assets/RE2/TUERSE.VBS` ist binaer und geteilt — erweitert Spur G dieselbe Bank
  (Piep/Aufschliessen an Kartentueren), muss sie mit `tools/re2_door_se_cut.py` aus beiden Satzlisten neu erzeugt
  werden (Dossier 6). `probe_r35_cut10f0_karte.c` (`vor11c0` setzt (9,93)) und `scd_vm.c`/`scd_room_setup.c`
  (Hakenzeilen am Blockende) sind Textkonflikt-Kandidaten mit J/K/L/P.
- **H4 Dossier-/Kommentar-Ungenauigkeit:** "@0x80051698 v0 = 0 -> kein Durchgang" (Dossier 2.2, Kopf von
  re15_schluessel_r36.h). v0 ist in allen Zweigen 0; kein Durchgang, weil der Sprung nach @0x800516f8 den
  Uebergangsblock @0x800516cc auslaesst. Verhalten richtig, nur die Begruendung.
- **H5 Messluecke dieser Abnahme:** Druck auf die Parkplatz-Tuer NACH der Sherry-Szene (erster Strom-Eintritt) an der
  exe nicht erreicht (2.10, Wegfindung); statisch abgesichert.
- **H6 Beobachtung ausserhalb der Spur (nicht geprueft):** im Messlauf mit Kartenwerkzeug-Elza (character 4) zeigt
  der Statusschirm Leons Ausweis (Z3 fd_002440). Ob das ein Artefakt des Kartenwerkzeugs oder ein Port-Befund ist,
  habe ich nicht untersucht.

Aufraeumen: Messkopie `re15_port/build/platform/pc/re15_pc_Qabn0.exe` nach der Abnahme entfernt; kein Code
geaendert.
