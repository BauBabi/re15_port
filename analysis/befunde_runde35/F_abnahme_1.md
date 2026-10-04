# Runde 35 Spur F "inhalt" — Unabhaengige Abnahme 1 (2026-10-04)

Baum `.claude/worktrees/r35_inhalt`, Zweig `r35/inhalt`, HEAD fc4f89d3 (Nachbesserung 1), Basis
master 154a73c1 (merge-base). `git diff master` ohne `...` zeigt wegen des weitergelaufenen master
(87cc8575, v0.8.22) fremde Loeschungen — massgeblich ist `git diff master...HEAD`.
Letzter Code-Commit 1c42a2c5 (Test), exe 06:39; `local_build.sh configure` + `build` ->
"ninja: no work to do" = gebaute exe entspricht HEAD.
Gemessen an der gebauten exe dieses Baums, Kopie `re15_port/build/platform/pc/re15_pc_abn1f.exe`
(nach der Abnahme geloescht). Runner `lauf.sh` (Karte mit `probe_r35_inhalt_karte`, dann
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.log
RE15_EXIT_AT`), Bilder nur ueber `RE15_FRAMEDUMP` (beschleunigter Renderer, Readback vor Present).
Alle Bild-/Logpfade unten relativ zu
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn1\`
(nicht eingecheckt; jeder Lauf ist mit dem genannten Befehl reproduzierbar).

## Ergebnis

**bestanden = JA.** Alle sechs Punkte erfuellt, alle Gates halten (Suite 482/482 selbst gefahren,
@0x-Stichproben stimmen in der richtigen Binaerdatei, Pfade sauber, 4 Riegel gruen und messend).
Die drei Maengel der Abnahme 0 (M1 P6, M2 P4, M3 P1) sind am gebauten Stand nachgemessen behoben.

| Punkt | Urteil |
|---|---|
| 1 Doppeltueren symmetrisch (Griffe) | erfuellt |
| 2 Codes in den eigenen Dokumenten gruen | erfuellt |
| 3 Zombies 1010/1220 weiter von der Tuer | erfuellt |
| 4 Memory Card im Regal 1010 | erfuellt (M2 aus Abnahme 0 behoben) |
| 5 Schrot-Munition auf dem rechten Aussenluefter 1090 | erfuellt |
| 6 RE2-Karten-Weltmodelle extrahiert und abgelegt | erfuellt (M1 aus Abnahme 0 behoben) |

## Gates

### Suite / Bau
* Das Dossier fuehrt woertlich `=== LOCAL-BUILD-OK (all) — Tests 482/482` (Stand b1553d7c, codegleich
  mit HEAD). Weil der Code seit Abnahme 0 geaendert wurde (tuer_spiegel_r35.c, door_scene_pc.c,
  inhalt_r35.c, Riegel), Suite trotzdem selbst gefahren: `bash re15_port/tools/local_build.sh test`
  -> `100% tests passed, 0 tests failed out of 482` / `=== LOCAL-BUILD-OK (test) — Tests 482/482`
  (1124 s, unter Last anderer Baeume; Log `suite.log`). Die fuenf Fenster-Haken liefen beim ersten
  Mal gruen (boot_bg_pin 17,8 s, dark_start_pin 17,7 s, relatch_pin 22,2 s, save_counter_pin 13,8 s,
  weste_load_pin 5,8 s), ebenso die beruehrten Nachbarn unit_r34n_e_dokumente,
  integration_r34n_e_dokumente_bild, integration_r30_irons_tisch_bild.

`ctest --test-dir re15_port/build -R r35_inhalt`: 4/4 gruen; einzeln mit Ausgabe:
`test_r35_inhalt_doku` 19/19, `test_r35_inhalt_zombies` 22/22 (Original gegen Port im selben
Prozess, Gegenprobe A: Original 1220 Cut 2 gegriffen Bild 12, 1010 Cut 0 Flucht-min 1546),
`test_r35_inhalt_items` 27/27, `probe_r35_inhalt_tueren test` 7/7.

### RE-Gate — Stichproben selbst disassembliert (re2_disasm.py / re15_disasm.py)
1. **@0x800535d4 / @0x800535f4** (RE2 `info/re2leon/PSX.EXE`): `800535c4 lw v0,-7388(v0)` =
   0x800ce324 (RDT-Zeiger), `800535d4 lw v0,72(v0)` (RDT+0x48), `800535dc sw v0,-29508(at)`
   (0x800d8cbc); `800535ec lw v0,-7388(v0)`, `800535f4 lw v0,76(v0)` (RDT+0x4C), `800535fc sw`
   0x800d8cbc. **Stimmt.**
2. **@0x80054040** Switch: Op-Tabelle @0x800A74C8 (Basis aus Eintrag 0x4E @0x800A7600 ->
   0x80054CD4), Eintrag [0x13] @0x800A7514 -> **0x80054020**; dort `lhu t0,2(a3)`, `lbu a2,1(a3)`,
   `80054040 addiu a3,a3,4`; Case `800540cc addiu a3,a3,6`. **Stimmt** (4 Byte, Case 6).
3. **@0x80055290 / @0x80055424 / @0x80055430** Obj_model_set: Eintrag [0x2D] @0x800A757C ->
   **0x80055260**; `80055280 lui t2,0x800d; 80055284 addiu t2,t2,-15896` (t2 = 0x800cc1e8),
   `80055290 lbu t1,1(s2)` (Objekt-Index, t1 danach nicht mehr beschrieben),
   `8005541c lw a0,8508(t2)` (= 0x800ce324 RDT-Zeiger), `80055424 lw v1,48(a0)` (RDT+0x30),
   `80055428 sll v0,t1,3`, `80055430 lw a1,4(v0)` (MD1 des Slots). **Stimmt: Objekt-Index =
   Modell-Slot.** (Nur Kosmetik im Dossier Z. 419: "RDT-Zeiger 0x800d213c" — richtig ist 0x800ce324.)
4. **H 290 @0x80013e34** — richtige Binaerdatei geprueft: in RE1.5 PSX.EXE steht dort ein `nop`
   (Delay-Slot eines `jr v0`), in **RE2 PSX.EXE** `80013e30 jal 0x8008de24` /
   `80013e34 addiu a0,zero,290`; 0x8008de24 = Wort `48c4d000` = `ctc2 a0,$26` (GTE-H). OFX/OFY:
   `80068e80 addiu a0,zero,160` / `80068e84 jal 0x8008de04` / `80068e88 addiu a1,zero,120`. **Stimmt**
   (Konstante stammt aus master: `RE15_DOOR_H` in include/re15_door_seq.h).
5. Daten-Belege M1 direkt in den RDT-Bytes (Auszug): ROOM20B0 @0x03158 `2d 08 .. 20 4e 20 4e 20 4e`
   (Obj_model_set obj 8, geparkt 20000); @0x037BA `2b 00 00 ..` Message_on 0, dann `21 0b 1f 00`
   Ck(11,31,0); @0x037D6 `22 08 53 01` Set(8,0x53,1); @0x037F2 `2e 04 08 00` Work_set(4,8);
   @0x037F6 `32 00 20 4e 20 4e 20 4e` Pos_set(20000,..); @0x03800 `2b 00 11 00` Message_on 17.
   ROOM5040 @0x01D54 `2d 01` (-11599,-2000,-21188), @0x02174 `2e 04 01 00`, @0x02178 `34 0c 00 83`
   Member_set(0x0C,-32000), @0x02188 `22 22 19 01`. ROOM6120 @0x0148A `2d 0b` (-22622,-1459,-18360),
   @0x01538 `22 04 a5 01`, @0x0153C `46 06` Aot_reset 6 — kein Work_set. **Alle Offsets stimmen.**
   Nebenbefund nachgeprueft: room20B0 sub00 = RDT 0x30F4..0x322A (310 Byte), Datei
   `scd/sub00.scd` 14 Byte.
* Diff-Suche (`git diff 154a73c1...HEAD -- re15_port tools`, nur +Zeilen):
  `deferred|tunable|interim|for now|faithful|plausib|TODO|vorerst|approx|geschaetzt` -> **0 Treffer**;
  neue `getenv(` -> **0**.
* Konstanten-Kennzeichnung: Lagen/Drehung der Karte (3748/-1725/-1383, rot 896/128/3584),
  Schrot-Lage, Aufhebe-Rechtecke, Zombie-Ziellagen, P0CD dy/dz 2048, DOOR07-Tausch, obj-3-Versatz =
  als PORT-WAHL / NUTZER-VORGABE mit Messregel gekennzeichnet; Mechanik-Konstanten tragen @0x
  (Flags 0x000B @0x80040998, Etage @0x80042cb4-ccc, Bit-Setzen @0x8001e0d0/d4, Schwelle 0xBB8
  @0x80101374/@0x801020A8, Gruen @0x80028974-94, Switch @0x80054040, RDT+0x48/+0x4C, md1 < 0x20
  @0x80054D98). Die 2048-Werte sind 180-Grad-Drehungen aus den Archiv-Daten (rot0[2] = 2048 der
  Door_model_set) bzw. deren Aufhebung, keine Gefuehlszahlen.
* Erklaert der Fix den Befund? **M2**: Abnahme 0 mass im echten Tuerweg in Cut 4 11 Diff-Pixel
  (flach liegende Karte); jetzt im selben Weg 51 Diff-Pixel (stehende Karte) — Vorbedingung (flach,
  Kamera 14 Grad ueber dem Brett) im Dossier, Wirkung gemessen. **M3**: Abnahme 0 mass auf den
  V3-Seiten L 157,0 / R 156,0 px; jetzt L 157,11/R 157,22 (S030), 157,00/157,25 (S049),
  157,24/157,50 (S092) — der 1-px-Rest ist weg; P0CD-Stangen vorher 98 gegen 153 graue Pixel (linke
  Stange schmaler), jetzt 153/153. **M1**: alle 6 Weltmodelle bytegleich aus der RDT-Modelltabelle
  nachgeschnitten (siehe Punkt 6).

### Pfad-/Vertrags-Gate
* `git diff 154a73c1...HEAD --name-only`: kein `release/`, kein `platform/android/`, kein
  `shared_assets/PSX/`, keine Aenderung an `tests/unit/CMakeLists.txt` /
  `tests/integration/CMakeLists.txt`. Neue/geaenderte Daten nur `shared_assets/RE2/FILES/FILE28_p01_page.TIM`,
  `FILE29_p01_page.TIM` (gleich gross, im Dossier genannt -> Gate-Pin Orchestrator),
  `engine/src/gen/r35_schrot_prop.inc`, `extracted_re2_items/` (Ablage, kein Laufzeit-Asset).
* Bank 9: nur 80/81 (`re15_game_flag_get(9, g->bit)` in inhalt_r35.c; Bits aus der Tabelle).
  AOT-Slots 1010 Slot 10, 1090 Slot 4 (Zensus im Dossier). Keine Nachrichten-IDs, keine Ereignisse.
* Gemeinsame Dateien: scd_room_setup.c +4 (Installer-Zeile + Kommentar + include), scd_vm.c +4,
  main.c +10 (davon 6-Zeilen-Block im Prop-Lader, Muster Dokumente), door_scene_pc.c +20/-1 (steht
  nicht in der Liste VERTRAG 1.4, im Dossier als ~23 Zeilen fuer das Zusammenfuehren genannt).
  Installer steht VOR `re15_dokumente_install` statt am Blockende — begruendet (Riegel
  unit_r34n_e_dokumente), Vertrags-Abweichung im Dossier vermerkt.
* Probe-Merge `git merge-tree --write-tree master HEAD`: **Konflikte nur in den include-Zeilen**
  von scd_room_setup.c (Z. 29: K/L-includes gegen `re15_inhalt_r35.h`) und scd_vm.c (Z. 53: K/L gegen
  `re15_zombie_abstand.h`) — beide Seiten behalten; main.c, door_scene_pc.c mergen sauber.
* Tests je Punkt vorhanden und messend: P1 `unit_r35_inhalt_tueren` (Gegenprobe: am
  Abnahme-0-Stand haette B fuer S041 33,9 Grad und S136/S157 37,1 Grad > 10 Grad gefeuert),
  P2 `unit_r35_inhalt_doku`, P3 `unit_r35_inhalt_zombies`, P4/P5 `unit_r35_inhalt_items`
  (neu V: Cut-4-Flaeche >= 40 px^2). P6 ist ein Extraktionsprodukt ohne Laufzeitwirkung (kein ctest,
  wie in Abnahme 0 vermerkt).

## Punkt 1 — Doppeltueren (Griffe) symmetrisch: ERFUELLT

Wortlaut: "Einige Doppeltueren ... haben unsymmetrischen Aufbau, z.B. unsymmetrische Tuergriffe."

Lauf (alle Port-Doppeltueren + RE2-Referenz S192):
```
RE15_TUER_SEITE=S022,S030,S041,S044,S045,S049,S059,S080,S092,S136,S155,S157,S218,S192
RE15_TUER_BOGEN=p1 RE15_TUER_SCHNELL=1 re15_pc_abn1f.exe
```
debug.log: 14 Sequenzen gespielt (`[tuer-seite] S041 ROOM10C0 DOOR0C V1 Spender FF Port-Archiv P0CD`,
`S136 ROOM2000 DOOR1B V3 Spender 23 Port-Archiv P1B3`, `S030 ROOM1090 DOOR1D V3 Spender 07 ...`).
Vergleich gegen die Bilder der Abnahme 0 (`..\abn\p1\*_anfang.ppm`), Bogen `tueren_a.png`,
`tueren_b.png`, `tueren_c.png` (je Seite: vorher | jetzt | jetzt gespiegelt) und Spiegel-Differenz
um die bestpassende Tuerachse `tueren_spiegeldiff2.png` (Skript `mirdiff2.py`):
```
Seite  Spiegel-Diff-Pixel (>40) vorher -> jetzt     Messung des Griffs
S030   308 -> 222   V3-Druecker Schwerpunkt y L/R 157,11/156,00 -> 157,11/157,22
S049    88 ->   8   157,00/156,00 -> 157,00/157,25
S092   134 ->  40   157,24/156,00 -> 157,24/157,50
S041   270 -> 118   P0CD-Stangen graue Pixel L/R 98/153 -> 153/153
S044   266 -> 122   99/153 -> 153/153
S136   129 -> 102   Riegelstangen spiegelgleich (Bogen tueren_a.png)
S155/S157 16 -> 18  spiegelgleich (vorher und jetzt)
S022/S045/S080      V2-Druecker 155,00/155,00 (unveraendert, waren schon symmetrisch)
S059    30 ->  30   P04B Zugstangen unveraendert symmetrisch
S192   (RE2-Original) 0
```
Der verbleibende Spiegel-Rest der Karten stammt aus Rahmen/Texturkanten und der Fuge (Achse auf
x,5), nicht aus den Griffen (Karten in `tueren_spiegeldiff2.png`: an den Griffen nur 1-px-Kanten).
In den Oeffnungsbildern (`tueren_mitte.png`) stehen die Griffe beider Fluegel ebenfalls gespiegelt;
die asymmetrische Oeffnung der Fluegel selbst hat das RE2-Original S192 genauso (`tueren_s192.png`).
Der kleine Vorsprung an der Blattkante im S030-Oeffnungsbild war schon vorher da und steht im
RE2-Original S192 ebenso (`tueren_s030z.png`) — kein Befund dieser Spur.

## Punkt 2 — Codes in den eigenen Dokumenten gruen: ERFUELLT

* Zensus der eigenen Texte (`analysis/befunde_runde34_nacht/E_texte/*.txt`,
  `analysis/befunde_runde30/irons_diary_en.txt`, Ziffernfolgen >= 3): nur `dok3_marvin.txt`
  "The new code is: 4312" und `dok4_armory.txt` "...with the code: 5632" (sonst nur das Datum
  im Diary).
* Echte exe, echter Leser, Nutzerkarte `re15_card_nutzer_2026-09-27.mcr`:
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_DOC=28|29 RE15_PAD_AT=400:S,520:M,580:A,600:A,700:R
  RE15_FRAMEDUMP=690-790/45:fd_ RE15_DOC_EXIT_AT=800` -> `p2_sheet.png`: Seite 2/3 von
  MARVIN'S NOTES "The new code is: **4312**" und ARMORY NOTICE "...with the code: **5632**" gruen,
  restlicher Text weiss. Gruen-Pixel F780: FILE28 248, FILE29 270 (Seite 1/3 mit nur den Pfeilen:
  83 = die gruenen Blaetter-Pfeile).

## Punkt 3 — Zombies ROOM1010/1220 weiter von der Tuer: ERFUELLT

Echter Tuerweg, echte Spielschleife, RE2-KI (Standard). Flucht = 2 s stehen, dann umdrehen und
VIERECK an der Eintrittstuer (`START=6`, Skript `W2,A0.2,L0.7,A0.5`; das Skript startet in jedem
Raum neu, jeder Eintritt ist ein frischer Raumstart).
```
Lauf                       Eintritt (debug.log)                                  Spawn (state.log F1 im Zielraum)           Ergebnis
1010 Cut 0 stehen          DOOR FIRE slot=1 ... target_cut=0 spawn=(3650,0,6900)  [1 @(3750,3500)] [2 @(205,7248)]          erster Griff gr=1 in F107
1010 Cut 0 Flucht 2 s      17 Eintritte, 16 Mal zurueck nach 1020 (Lauf per Zeitlimit beendet)                              gr=1: 0, hp immer 100
1220 Cut 2 stehen          DOOR FIRE slot=2 ... target_cut=2 spawn=(-16600,0,-9900) [3 @(-13362,-10345)] [4 @(-14264,-7414)] erster Griff F101
1220 Cut 2 Flucht 2 s      17 Eintritte, 17 Mal zurueck                                                                      gr=1: 0, hp 100
1220 Cut 6 stehen          target_cut=6                                                                                      erster Griff F128
1220 Cut 6 Flucht 2 s      9 Eintritte, 8 zurueck                                                                            gr=1: 0, hp 100
1220 Cut 0 stehen          target_cut=0                                                                                      kein Griff in 400 Bildern
1220 Cut 0 Flucht 2 s      9 Eintritte, 8 zurueck                                                                            gr=1: 0, hp 100
```
(Karten: 1010 aus ROOM1020 (-4000,-18000) rot 0; 1220 aus ROOM1210 (-18300,-9800) rot 0 /
(-18300,-17300) rot 0 / (-20200,-6100) rot 2048; Logs `p3_*`.) Original zum Vergleich (Riegel A):
1220 Cut 2 Griff in Bild 12 waehrend des Drehens, 1010 Cut 0 Griff in Bild 48. Damit gibt es bei
jedem gemessenen Eintritt "eine Chance, aus dem Raum wieder raus zu drehen", auch nach 2 s Zoegern.
ROOM1011 (Elza) nicht im Lauf gemessen (Spawn dort bedingt, Dossier OFFEN 4); Tabellenschluessel
Typ + Original-Lage deckt dieselben Saetze.

## Punkt 4 — Memory Card im Regal ROOM1010: ERFUELLT (M2 behoben)

* **Echter Tuerweg 1020 -> 1010** (Karte ROOM1020 (-4000,-7250) rot 0, `START=6`, Skript
  `A0.2,W0.6,U0.25,R0.35,U1.3,R0.35,W0.3,A0.2,W4,A0.2,W1.5,A0.2,W2,S0.2,W3`,
  `RE15_FRAMEDUMP=40-440/10:fd_`, gleicher Lauf mit `bit:80` als Gegenbild): `DOOR FIRE slot=0 ...
  target_cut=4 spawn=(3650,0,-3950)`, `cam=4` auf dem ganzen Weg. Differenz ohne/mit Bit (9,80)
  (`diff.py`, Schwelle 24): **F40..F100 je 41..51 Pixel, bbox (101..107, 99..107)** (F80 0 = Leon
  davor). Bild `p4_sheet1.png`: die Karte steht als graues Rechteck im Regalfach rechts neben Leons
  Kopf, klar erkennbar; vorher (Abnahme 0) 11 Pixel Strich.
* Aufnahme im selben Lauf (`p4_sheet2.png`): F140/F200 Modal mit Memory-Card-Bild, "Will you take
  the **Memory Card**?" -> Yes; F240 Regal leer; F380 Inventar mit Memory Card x3.
* Cut 7 (Kamera der Nutzermarke, im Spiel nicht erreichbar — Dossier/RVD-Zensus): `RE15_FORCE_CUT=7`,
  Spieler (3650,6900) rot 2048, F120 mit/ohne Bit: **70 Pixel, bbox (268..279, 158..172)** — auf der
  Marke (rote Huelle 269..278 x 158..170), Bild `p45_marke.png` (add_card.bmp neben dem Port-Bild).
* Riegel V: Cut 4 47,6 px^2 >= 40, Sichtseite zugewandt 0,99; Cut 7 126,0 px^2.
* Die falsche Dossier-Aussage "Nutzerbild beweist Cut 7" ist durchgestrichen und korrigiert
  (F_inhalt.md OFFEN 3, M2-Abschnitt).

## Punkt 5 — Schrot-Munition auf dem rechten Aussenluefter ROOM1090: ERFUELLT

* **Echter Kamera-Weg ueber den Zonen-Scan** (nicht erzwungen): Karte ROOM1090 (-6500,-9000,-1500)
  rot 1024 (Dach, in Zone 1 "Cut 0 -> 1"), Skript `U4.7,L0.36,U1.8,R0.36,U1.5,W0.5,A0.2,W3,A0.2,W2,S0.2`:
  debug.log `[walk]` cut 0 -> 1 (F60) -> 2 (F330, Zone 4); zweiter Lauf von links
  (`p5_links`, Start (-7500,..)): Cut 2 ab F240 mit Leon am rechten Bildrand — die dunkle Kiste
  steht sichtbar oben auf dem RECHTEN Klimageraet (`p5_links_sheet.png` F240/F260 + Zoom).
  Kommt man genau von unten (x -2414), verdeckt Leon selbst die Kiste, solange er davor steht
  (`p5_echt_sheet2.png`) — normale Verdeckung, kein Befund.
* Aufnahme im Lauf `p5_echt`: Leon (-2414,-14929) rot 1024 -> F400 "Will you take the Shotgun
  Shells?" mit Patronenschachtel-Bild -> Yes -> F600 Inventar mit der Schachtel x3
  (`p5_echt_sheet.png`); F440 Kiste weg.
* Cut 2 erzwungen, Spieler (-2408,-13800,y -9000): mit/ohne Bit (9,81) **161 Pixel, bbox
  (99..115, 136..145)**, Marke 98..117 x 127..146 (`p45_marke.png` neben Shotgun.bmp).
* Hinweis zur Messmethode: ein Kartenstart mitten auf dem Dach laedt Cut 0 und wechselt dort nie
  (aus Cut 0 fuehrt nur Zone 1 bei x -9600..-3500 weiter) — ein Lauf von (-2408,-9500) zeigt Leon
  deshalb gar nicht; das ist ein Artefakt des Kartenstarts, der echte Eintritt aus ROOM10F0
  (Door_aot_set ROOM10F0 @0x00F52 -> 1090 (-13600,-9000,1300) Cut 0) laeuft durch Zone 1.

## Punkt 6 — RE2-"Karten Modelle in der Welt zum Einsammeln": ERFUELLT (M1 behoben)

* `extracted_re2_items/karten.png` angesehen: 3 Card Keys (Blue, Lab x2) + 3 Lageplan-Weltmodelle
  (zusammengerollter Plan karte00, Planbogen karte01 Kanalisation, Planbogen karte02 Fabrik).
  `uebersicht.html`: 180 Bildverweise, 0 fehlend; Abschnitt Karten mit karte00..02.
  `katalog.csv` 3 Lageplan-Zeilen, `lageplaene.csv` 7 Aufnahmen inkl. room6120 "KEIN Weltobjekt".
* Selbst nachgeschnitten aus der RDT-Modelltabelle (RDT+0x30, Eintrag 8 Byte, MD1 bei +4):
```
ROOM20B0 slot 8  MD1 @0x4FDC  == karte00 (1428 B, md5 735097e1)   obj-Datei model08 md5 735097e1
ROOM2130 slot 0  MD1 @0x1D80  == karte00                           model00 735097e1
ROOM3060 slot 7  MD1 @0x45C8  == karte01 (236 B, 99dae133)         model07 99dae133
ROOM4040 slot 2  MD1 @0x2F00  == karte01                           model02 99dae133
ROOM5040 slot 1  MD1 @0x2600  == karte02 (236 B, 5cdae635)         model01 5cdae635
ROOM5060 slot 2  MD1 @0x2484  == karte02                           model02 5cdae635
```
  md5 der abgelegten MD1 = die Werte der Abnahme 0. room6120: Aufnahme-Block blendet nichts aus
  (Bytes oben), obj 11 steht ausserhalb der Aufnahmeflaeche — Widerlegung nachvollzogen.
* Rest (Dossier OFFEN 2): 3 Item-Platzierungen mit md1 >= nOmodel nicht geschnitten (room2080 Id 13,
  roomG040 Id 30/99) — keine Karten (Item-Namen), betrifft den Kartenteil des Wortlauts nicht.

## Maengel

Keine. (Abnahme-0-Maengel: M1 behoben — 6 Lageplan-Weltmodelle bytegleich nachgeschnitten, Offsets
in den RDT-Bytes bestaetigt; M2 behoben — Karte im echten Tuerweg in Cut 4 51 statt 11 Diff-Pixel,
Cut 7 weiter auf der Marke, Dossier korrigiert; M3 behoben — S041 gepinnt, Kameraseiten-Paarung,
V3-Rest 1 px -> <= 0,26 px, P0CD-Stangen 153/153.)

## Hinweise (keine Maengel)
* Zusammenfuehren: Include-Konflikte mit Spur K/L in scd_room_setup.c und scd_vm.c (beide Seiten
  behalten); Installer-Reihenfolge vor `re15_dokumente_install` (begruendet); main.c-Prop-Lader 6
  Zeilen; door_scene_pc.c ~23 Zeilen gegen master.
* Paket-/Android-Gate: FILE28_p01_page.TIM / FILE29_p01_page.TIM gleich gross geaendert -> Gate-Pin
  (Orchestrator). Nachbesserung 1 bringt keine neuen Laufzeit-Assets.
* Dossier Z. 419: "RDT-Zeiger 0x800d213c" -> richtig 0x800ce324 (t2 = 0x800cc1e8 + 0x213C);
  die Aussage selbst (Objekt-Index = Modell-Slot) stimmt.
* Im Inventar liegt weiterhin das Kampfmesser (p4_sheet2.png F380) — Auftrag der Spur E, nicht F.
