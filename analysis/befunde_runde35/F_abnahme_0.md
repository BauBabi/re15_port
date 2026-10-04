# Runde 35 Spur F "inhalt" — Unabhaengige Abnahme 0 (2026-10-04)

Baum `.claude/worktrees/r35_inhalt`, Zweig `r35/inhalt`, HEAD 23853e1f, Basis master 154a73c1
(merge-base; `git diff master` ohne `...` zeigt wegen des inzwischen weitergelaufenen master fremde
Loeschungen — massgeblich ist `git diff master...HEAD`). Gemessen an der gebauten exe dieses Baums
(Kopie `re15_port/build/platform/pc/re15_pc_abn0f.exe`, nach der Abnahme wieder geloescht).
Messwerkzeug: Runner `lauf.sh` im Scratchpad (Karte mit `probe_r35_inhalt_karte`, dann
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.log`,
Bilder nur ueber `RE15_FRAMEDUMP`, beschleunigter Renderer). Bild- und Logpfade unten liegen unter
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-...\scratchpad\abn\`
(nicht eingecheckt; jeder Lauf ist mit dem angegebenen Befehl reproduzierbar).

## Ergebnis

**bestanden = NEIN.** P1, P2, P3, P5 erfuellt; **P4 teilweise, P6 teilweise**. Alle Gates halten
(Suite 482/482 selbst gefahren, @0x-Stichproben stimmen, Pfade sauber, 4 Riegel gruen).

| Punkt | Urteil |
|---|---|
| 1 Doppeltueren symmetrisch | erfuellt (Restbefund 1 px + Riegel-Luecke -> M3) |
| 2 Codes gruen | erfuellt |
| 3 Zombies 1010/1220 weiter weg | erfuellt |
| 4 Memory Card im Regal 1010 | **teilweise** (M2) |
| 5 Schrot-Munition auf dem Luefter 1090 | erfuellt |
| 6 RE2-Karten-Weltmodelle | **teilweise** (M1) |

## Gates

### Suite / Bau
* `bash re15_port/tools/local_build.sh configure` -> OK; `... build` -> "ninja: no work to do"
  (exe 04:42, letzter Quellcommit davor); `... test` selbst gefahren:
  `100% tests passed, 0 tests failed out of 482` / `=== LOCAL-BUILD-OK (test) — Tests 482/482`
  (1116 s, unter Last anderer Baeume; die fuenf Fenster-Haken liefen gruen durch).
* `ctest --test-dir re15_port/build -R r35_inhalt -V`: doku 19/19, zombies 22/22, items 25/25,
  tueren 6/6.

### RE-Gate (@0x-Stichproben, selbst disassembliert)
1. **@0x80028974-94** (PSX.EXE, Textmaler FUN_80028868): `andi v1,v0,0x4; sltu v1,zero,v1;
   andi v0,v0,0x3; sll v0,v0,1; addiu v1,v1,480; addu v0,v0,v1; sll v0,v0,6; ... ori s5,v0,0x10`
   = CLUT (x 256, y 480+(N&3)*2+(N&4!=0)). Sprungziel nachgeprueft: Tabelle @0x8001099c[4]
   (Steuerbyte 0x05 -> v1=4) -> 0x80028968 = genau dieser Block. TEX.TIM Kopf `0001 e001 2000 1800`
   (x 256 y 480, 32x24); Zeile 2 @0x94 = (0,184,40) (0,160,32) ... = gruen. Original-Bytes
   ROOM1110.RDT @0x0DB0 `61 05 01 10 0f 0d 0e 05 00`, ROOM1230.RDT @0x172F
   `61 05 01 11 12 0f 0e 05 00` bestaetigt.
2. **@0x80101374 / @0x801020A8** (RE2 EMZ0.BIN, nicht PSX.EXE): @0x80101320 `sltiu v0,s2,0xbb8`,
   @0x80101374 `addiu v0,zero,3073` (=0xC01) `sw v0,4(s0)`; @0x801020A8 `sltiu v0,s2,0xbb8`. Stimmt.
3. **@0x80054CF8 / @0x80054D98** (RE2 PSX.EXE, Item_aot_set, Tabelle @0x800A7600 -> 0x80054CD4):
   `lhu a1,18(s0)` @0x80054CF4, `lbu s2,20(s0)` @0x80054CF8, `sltiu v0,s2,0x20` @0x80054D98. Stimmt.
4. Zusatz **@0x80040644** (RE1.5 Item_aot_set): `lhu a1,18(a2)` @0x80040680, `lbu s1,20(a2)`
   @0x80040684. Stimmt. @0x80073ee4 Bytes `00 60` = Bytepaar [0,96]. Stimmt.
* Diff-Suche (`deferred|tunable|interim|for now|faithful|plausibel|TODO|vorerst|approx`,
  neue `getenv(`) in re15_port/tools: **0 Treffer**. Lagen/Versaetze/Tuer-dz sind als PORT-WAHL /
  NUTZER-VORGABE mit Messregel gekennzeichnet, Mechanik-Konstanten tragen @0x.
* Erklaert der Fix den Befund? P1 ja (Sonde `alt`: getauschte Griffe am 2. Fluegel mit
  rot0[2]=2048, Winkel 49,7/37,1/180 -> neu 0,0-0,2). P2 ja. P3 ja (Vorbedingung im Protokoll:
  Original 1220 Cut 2 Griff in Bild 12). P4/P5 Lage per Rueckprojektion (Marke), Mechanik belegt.

### Pfad-/Vertrags-Gate
* `git diff master...HEAD --name-only`: kein `release/`, kein `platform/android/`, kein
  `shared_assets/PSX/`, keine Aenderung an `tests/unit/CMakeLists.txt` /
  `tests/integration/CMakeLists.txt`. Neue Daten: `shared_assets/RE2/FILES/FILE28_p01_page.TIM`
  (md5 ad9d0f6f...), `FILE29_p01_page.TIM` (f39ce419...) — im Dossier genannt, gleich gross
  (Paket-/Android-Gate-Pin noetig, Orchestrator). Satz reproduzierbar: md5 beider Seiten =
  `F_belege/satz_md5.txt`.
* Bank 9: nur 80/81. AOT-Slots 1010 Slot 10, 1090 Slot 4 (Zensus im Dossier). Keine
  Nachrichten-IDs/Ereignisse belegt.
* Gemeinsame Dateien: scd_room_setup.c +4, scd_vm.c +4, main.c +10 (davon ein 6-Zeilen-Block im
  Prop-Lader — knapp ueber "1-5 Zeilen"). Installer-Zeile steht VOR `re15_dokumente_install` statt
  am Blockende — im Dossier als Vertrags-Abweichung begruendet (Riegel unit_r34n_e_dokumente).
  Hinweis fuer den Orchestrator, kein Abnahme-Mangel.

## Punkt 1 — Doppeltueren (Griffe) symmetrisch: ERFUELLT

Wortlaut: "Einige Doppeltueren ... haben unsymmetrischen Aufbau, z.B. unsymmetrische Tuergriffe."

Messung A (Sonde, Weltgeometrie): `probe_r35_inhalt_tueren 2` / `... 2 alt` / `... 2 mitte`:
P1DK/P1DG/P1DL V3 hinten 49,7 -> 0,1 Grad; P1BD V2 37,1/37,0 -> 0,1; P0CD V0 180/180 -> 0,0;
RE2-Originale unveraendert (DOOR1B V2 2,7/2,6, DOOR1D V3 0,0).

Messung B (echte exe, alle 13 Seiten der Port-Doppeltueren):
```
RE15_TUER_SEITE=S022,S030,S041,S044,S045,S049,S059,S080,S092,S136,S155,S157,S218
RE15_TUER_BOGEN=<dir> RE15_TUER_SCHNELL=1 re15_pc_abn0f.exe
```
-> 13 Sequenzen gespielt (debug.log `[tuer-seite] ...`), Bilder `p1/anfang.png`,
`p1/griffe8x.png`, `p1/lr_mirror.png`. Alle 13 Seiten zeigen gespiegelte Griffe (Druecker nach
aussen, Stangen gleich hoch: S041/S044 P0CD, S136/S155/S157 Riegelstangen, S059 Zugstangen).
Balken-Schwerpunkt (helle Pixel, Spalten 132-142 / 178-186, Zeilen 148-166):
```
S022/S045/S080 (V2): L 155.00  R 155.00
S030/S049/S092 (V3): L 157.00-157.22  R 156.00   <- Rest 1 px
```
Der grobe Befund ist weg. Restbefund: auf den drei V3-Seiten sitzt der linke Druecker ~1 px tiefer
als der rechte (die z-Spiegeldrehung 2048 ist fuer das Spender-Mesh DOOR07 eine Punktspiegelung,
keine Spiegelung; V2 mit y-Drehung ist exakt). Beide Riegel-Masse (Spitze, Huellen-Mitte) sehen
das nicht (0,1 Grad). -> M3 (gering).

Riegel-Abdeckung: Die Sonde ordnet "vorn/hinten" nach dem Vorzeichen der LOKALEN pos[0] des
Griffs. Dadurch werden 6 der 13 Port-Doppeltuer-Seiten nie als Paar gemessen (S022, S041, S045,
S080 = je ein Griff je Fluegel mit pos[0] +130/-130; S136, S157), obwohl die exe dort beide Griffe
von EINER Seite zeigt. S041 (P0CD V1) war vorher unsymmetrisch (Sonde `alt`, Wahl 53: obj 2 spitze
(0,-688,-210), obj 3 (0,688,-210) = eine Stange nach unten, eine nach oben), ist durch die
P0CD-Tabelle jetzt richtig (neu: (0,-688,210)), aber von keinem Riegel gepinnt. -> M3.

## Punkt 2 — Codes in den eigenen Dokumenten gruen: ERFUELLT

* Zensus: Codes nur in `dok3_marvin.txt` ("4312") und `dok4_armory.txt` ("5632")
  (grep Ziffernfolgen in E_texte + irons_diary_en.txt).
* Riegel direkt: `test_r35_inhalt_doku` 19/19 (82 bzw. 104 gruene Kernpixel nur im Code-Kasten,
  0 weisse Kernpixel im Kasten, FILE25..29 sonst 0 gruene Pixel).
* Echte exe, echter Leser (Nutzerkarte re15_card_nutzer_2026-09-27.mcr):
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_DOC=28|29 RE15_PAD_AT=400:S,520:M,580:A,600:A,700:R
  RE15_FRAMEDUMP=780:seite2.ppm RE15_DOC_EXIT_AT=800` -> `p2_docs.png`: Seite 2/3
  "The new code is: **4312**" und "...with the code: **5632**" gruen, Rest weiss, Doppelpunkt weiss
  (wie im Original: `61` vor `05 01`).

## Punkt 3 — Zombies ROOM1010/1220 weiter von der Tuer: ERFUELLT

Echter Tuerweg, echte Spielschleife (RE2-KI = Standard):
* **1010 Cut 0, stehen bleiben**: Karte ROOM1020 (-4000,-18000) rot 0, `A0.5`, START 60 ->
  `[aot] DOOR FIRE slot=1 ... target_cut=0 spawn=(3650,0,6900)` -> Spawn
  `[1 t=10 @(3750,3500,r3072)] [2 t=10 @(205,7248,r0)]` (Tabelle greift). Steuerung ab F6, erster
  Griff `gr=1` in **F107** (Original laut Riegel F48). Lauf `p3_1010_stehen/state.log` Z.168.
* **1010 Cut 0, Flucht** (`START=6`, Skript `[Wn,]A0.2,L0.7,A0.5`, Reaktion 0 / 1 / 2 s):
  0 Griffe, hp 100, zurueck in ROOM1020 nach F33 / F63 / F93; 11 / 9 / 8 Eintritte hintereinander
  (Skript laeuft je Raum neu) — jedes Mal raus.
* **1220 Cut 2** (schlimmster Original-Fall, Original Griff in Bild 12): Karte ROOM1210
  (-18300,-9800) rot 0 -> `DOOR FIRE slot=2 rect=(-17150,-9800,...) target_cut=2` -> Spawn
  `[3 @(-13362,-10345)] [4 @(-14264,-7414)]`; stehen: Griff **F101**; Flucht mit 0 / 1 / 2 s
  Reaktion: 0 Griffe, hp 100, raus nach F33 / F63 / F93 (7 / 6 / 5 Eintritte).
* **1220 Cut 0**: Karte (-20200,-6100) rot 2048 -> Spawn `[1 @(-25398,-6739)] [2 @(-23500,-9700)]`;
  stehen 700 Bilder ohne Griff; Flucht mit 2 s Reaktion: 0 Griffe, 5 Eintritte.
* Riegel: `unit_r35_inhalt_zombies` 22/22 (alle 7 Eintritte + 1221, Flucht-min >= 3000).
Damit hat man bei jedem gemessenen Eintritt "eine Chance, aus dem Raum wieder raus zu drehen",
auch mit 2 s Reaktionszeit. 1011 (Elza) nicht im Lauf gemessen (Spawn bedingt), Tabellenschluessel
deckt dieselben Saetze.

## Punkt 4 — Memory Card im Regal ROOM1010: TEILWEISE

Erfuellt: Lage an der Nutzermarke (Cut 7: Projektion (274,0;164,5), Marke 269..278 x 158..170),
Prop/Zone/Bit (Riegel 25/25), Aufnahme ueber den ECHTEN Weg:
Karte ROOM1020 (-4000,-7250) rot 0, Skript `A0.2,W0.6,U0.25,R0.35,U1.3,R0.35,W0.3,A0.2,W4,A0.2,...,S0.2`
-> `DOOR FIRE slot=0 ... target_cut=4 spawn=(3650,0,-3950)` -> Leon laeuft zu (3147,-1064) rot 64
-> VIERECK -> "Will you take the **Memory Card**?" -> Yes -> Inventar zeigt die Memory Card x3
(`p4_1010_tuer2/sheet.png`). (Mit nur 80 Bildern Wartezeit nach dem Oeffnen nimmt das Modal das
Ja noch nicht an — Original-Eingabesperre, kein Mangel.)

Nicht erfuellt — der Spieler SIEHT die Karte praktisch nicht:
* **Cut 7 (Kamera der Marke) ist im Spiel nicht erreichbar.** `probe_r34n_e_dokumente zonenliste
  1010` und `1011`: keine RVD-Zone mit Ziel Cut 7 (Zone 14 ist nur der Gruppenkopf von Cut 7);
  Tueren nach 1010: ROOM1020 @0x01C82 -> Cut 4, @0x01CA2 -> Cut 0; das SCD von ROOM1010
  (`scd_dump_room.py`) setzt keinen Cut. Echter Weg (oben): `cam=4` auf dem ganzen Weg bis vor
  das Regal (state.log).
* **In Cut 4 ist die Karte ein 10x2-Pixel-Strich**: Framedump mit/ohne Bit 80
  (`RE15_FORCE_CUT=4`, Karte (3650,6900) bzw. `bit:80`): Differenz **11 Pixel**, bbox
  (95..105, 105..107) — die Karte liegt flach auf Brett B, das Cut 4 fast von der Kante sieht
  (`p4_zoom.png`). Zum Vergleich Cut 7: 56 Pixel (268..280, 160..168). Cuts 6/8 (andere Raumhaelfte):
  0 Pixel (kein Durchscheinen — gut).
* Die Dossier-Aussage (OFFEN 3) "das Nutzerbild beweist aber, dass Cut 7 im Spiel erscheint" ist
  nicht gemessen: add_card.bmp (wie Shotgun.bmp) enthaelt keine Spielerfigur, ist also ein
  Hintergrundbild, kein Spielbild; der Zonen-/Tuer-/SCD-Zensus findet keinen Weg nach Cut 7.
-> M2.

## Punkt 5 — Schrot-Munition auf dem rechten Aussenluefter ROOM1090: ERFUELLT

* Cut 2 (Kamera der Marke) ist erreichbar: Zone 4 "von Cut 1 nach Cut 2" (Quad deckt bei
  x -2408 alle z < ca. -14060 ab, also den Standort vor dem Luefter), Zone 25 Cut 9 -> 10
  (= gleiche Kamera).
* Framedump mit/ohne Bit 81 (`RE15_FORCE_CUT=2`, Spieler (-2408,-13800,y -9000)): Differenz
  **161 Pixel**, bbox (99..116, 136..146) — oben auf dem rechten Klimageraet, in der Marke
  (98..117 x 127..146) (`p5_c2_c1.png`, `p5_zoom.png`). Cut 1: 5 Pixel (Fernsicht auf dieselbe
  Stelle, legitim sichtbar), Cuts 0/5/7: 0 Pixel.
* Aussehen: die Kiste ist dunkel — dasselbe Original-Mesh liegt in ROOM1010 Cut 5 im Metallregal
  genauso dunkel (`p5_1010_cut5.png`), also Original-Aussehen, kein Textur-Fehler.
* Aufnahme ueber echten Aktionsdruck: Karte ROOM1090 (-2408,-9500,y -9000) rot 1024,
  `U2.2,W0.5,A0.2,W3,A0.2,W2,S0.2` -> Leon laeuft bis (-2408,-14450) -> "Will you take the
  **Shotgun Shells**?" -> Yes -> Inventar zeigt die gruene Patronenschachtel x3 (Nutzer-Halbierung
  von 7) (`p5_1090_walk/sheet.png`). Riegel 25/25 (auch 1091).

## Punkt 6 — RE2-"Karten Modelle in der Welt zum Einsammeln": TEILWEISE

Erfuellt: 81 Meshes aller 206 Item_aot_set-Platzierungen mit Weltmodell, Galerie
`extracted_re2_items/uebersicht.html`, `kontaktbogen.png`, `karten.png` (Blue Card Key, Lab Card
Key x2), `katalog.csv` — vorhanden, angesehen, Mechanik @0x80054CF8/@0x80054D98 bestaetigt.

Nicht erfuellt: Der Nutzer benutzt "Karte" in DIESEM Auftrag durchgehend fuer den LAGEPLAN
(AUFTRAG.md Z. 25 "auf der Map", Z. 28/31 "taucht nicht auf der Karte auf", Z. 29 "die Map von
ROOM 11E0", Z. 66 "Karte soll aufgehen"). Die Lageplan-Weltmodelle EXISTIEREN in RE2 als
Raum-Objektmodelle (nicht als Item_aot_set) und fehlen in der Ablage. Gemessen (Renderer
`re2_doc_worldmodels.render_clut` auf `info/re2leon/PL0/RDT/<raum>/obj/*.md1` der sieben im Dossier
genannten Lageplan-Raeume, Bogen `p6/sheet.png`):
```
room20B0 model08, room2130 model00   md5 735097e1  zusammengerollter Plan (gleiches Mesh in beiden Polizei-Plan-Raeumen)
room3060 model07, room4040 model02   md5 99dae133  flacher Plan-Bogen (Kanalisation)
room5040 model01, room5060 model02   md5 5cdae635  flacher Plan-Bogen (Fabrik)
room6120 model11                     md5 aa1e4d0c  (Labor, Zuordnung offen)
```
Keiner dieser md5 kommt in `extracted_re2_items/modelle/` vor (0 Treffer), `katalog.csv` enthaelt
kein "map". Das Dossier haelt "Lageplaene sind in RE2 KEINE Items" fest und laesst die Weltmodelle
als OFFEN 1 stehen. -> M1.

## Maengel

**M1 (P6, mittel)** — Lageplan-Weltmodelle fehlen in `extracted_re2_items/`. Belegt vorhanden:
room20B0/obj/model08 + room2130/obj/model00 (md5 735097e1), room3060/obj/model07 +
room4040/obj/model02 (99dae133), room5040/obj/model01 + room5060/obj/model02 (5cdae635),
room6120 (Kandidat model11, aa1e4d0c). Zu tun: je Lageplan-Raum den SCD-Satz finden, der dieses
Objekt setzt und es nach der Plan-Aufnahme (Nachricht + Ja/Nein, Fundstellen im Dossier) ausblendet
(Beleg mit Datei-Offset), die Meshes wie die Item-Meshes schneiden (md1/tim/obj/png) und in
`karten.png` / `uebersicht.html` / `katalog.csv` aufnehmen (oder messend zeigen, dass diese
Objekte NICHT die Plan-Aufnahmen sind).

**M2 (P4, mittel)** — Memory Card in der einzigen erreichbaren Kamera am Regal (Cut 4) nur ein
10x2-Pixel-Strich (11 Pixel Differenz mit/ohne Bit (9,80), bbox 95..105 x 105..107); Cut 7, auf den
die Lage abgestimmt ist, ist im Spiel nicht erreichbar (keine RVD-Zone nach Cut 7 in 1010/1011,
Tueren nur Cut 0/4, SCD ohne Cut-Wechsel; echter Tuerweg 1020->1010 zeigt cam=4 bis vors Regal).
Zu tun: Lage/Ausrichtung so waehlen, dass die Karte in Cut 4 sichtbar ist und in Cut 7 weiter auf
der Marke liegt (z.B. Brett/Ausrichtung — PORT-WAHL mit Messung beider Cuts per Framedump-Diff),
oder einen echten Weg nach Cut 7 belegen. Die Dossier-Aussage "Nutzerbild beweist, dass Cut 7 im
Spiel erscheint" korrigieren.

**M3 (P1, gering)** — (a) Riegel-Luecke: `probe_r35_inhalt_tueren` ordnet Seiten nach dem
Vorzeichen der lokalen pos[0]; 6 von 13 Port-Doppeltuer-Seiten (S022, S041, S045, S080, S136,
S157) werden nie als Paar gemessen, obwohl die exe dort beide Griffe auf einer Seite zeigt. S041
(P0CD V1) war vorher unsymmetrisch und ist nur durch die P0CD-Tabelle repariert — ungepinnt. Seite
nach Welt-/Kameraseite bestimmen (z.B. Vorzeichen der Spitze in Tuernormalen-Richtung) und S041
in den Riegel. (b) Restbefund V3 (S030, S049, S092): linker Druecker ~1 px tiefer als der rechte
(Balken-Schwerpunkt 157,0 vs 156,0; V2-Seiten 155,0/155,0), beide Riegel-Masse blind dafuer —
beheben (echte Spiegelung statt z-Punktspiegelung fuer das Spender-Mesh) oder im Dossier mit Mass
als bekannte Restabweichung fuehren.

## Hinweise (keine Maengel)
* Paket-/Android-Gate: FILE28_p01/FILE29_p01 gleich gross geaendert -> Gate-Pin (Orchestrator).
* Vertrag 1.4: Installer vor `re15_dokumente_install` statt am Blockende (begruendet);
  main.c-Prop-Lader-Haken 6 Zeilen.
* Fuer P6 gibt es naturgemaess keinen ctest (reines Extraktionsprodukt).
