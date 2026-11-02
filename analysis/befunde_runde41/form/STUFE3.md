# Stufe 3 Bahnsteig-Form: Bewegung, Angriffe, Einbau (laufendes Dossier)

Auftrag (Orchestrator, nach E19): J bekommt Gerippe-Pruefung, eigene Clips nach dem Muster RE2-G4, das G4-Verhalten aus Spur R auf die
neuen Clips, Einbau in 5080 und 5090 unter eigenem Asset-Namen, Tests, Sichtbelege. Bindend: `NUTZER_ENTSCHEIDE.md` E1-E19.
Arbeitsbaum `.claude/worktrees/r41_form`, Zweig `r41/form`. Dieses Dossier wird fortlaufend geschrieben; wer nach einem Abbruch
weitermacht, liest zuerst Abschnitt 0.

## 0. Stand und naechster Schritt

| Schritt | Stand |
|---|---|
| 1 Sichern + `git merge master` | erledigt: 8423920d4 (Formstand, `.gitignore` im Ordner), f89159cf4 (Merge, keine Konflikte) |
| 2 Gerippe pruefen | erledigt (Abschnitt 2): alle Teile haben Knochen, keine Aenderung an Form/Textur noetig |
| 3 Clips | gebaut (Abschnitt 3), Asset `stufe3/asset/BAHNSTEIG_FORM.EMD` = `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` |
| 4 Angriffsmuster | Tick auf die neuen Clips umgestellt, Flucht 5080, Sturz 5090 (Abschnitt 4) |
| 5 Einbau, Tests | Lader, Paket-Gate, Tests angepasst (Abschnitt 5). Mit dem Endstand (Bau `re15_port/build_s3`): unit_birkin_bahnsteig, unit_birkin_szenen gruen, integration_r41_birkin_pistole gruen (Leon + Elza: keine Mutation, Form-HP 150 bis 0, Rueckstoss, Sturz, G5, G5 faellt) |
| 6 Sichtbelege | erledigt: `stufe3/kontakt/` (00_uebersicht + 12 Clips), `stufe3/spiel/` (7 Blaetter, echtes Fenster) |
| Voller ctest | Lauf in `re15_port/build` (Stand Commit 0e10882f9, Asset waehrend des Laufs getauscht): siehe Abschnitt 8 |

Commits dieser Stufe: 8423920d4 (Sichern), f89159cf4 (Merge master), a7f9922d4 / 79aefb203 (Generator WIP), 0e10882f9 (Einbau,
Tests), 072d1d3f7 (Sichtbelege, Pistolen-Test nachgezogen), 6200b3d35 (Asset-Nachschliff, Uebergabepunkt G5).

Generator dieser Stufe: `stufe3/gen/` (`s3_rig.py` Gerippe/Posen/IK, `s3_clips.py` Asset, `s3_angriffe.py` Clips, `s3_g4.py`
G4-Clipdaten, `s3_pruef.py`/`s3_dreh.py`/`s3_reichweite.py` Zahlenpruefung, `s3_testkopf.py` Testreferenz, `s3_kontakt.py`/
`s3_uebersicht.py`/`s3_blatt.py` Kontaktblaetter, `s3_spielfolge.sh`/`s3_folge_blatt.py` Spielbilder). Zwischenbilder unter
`stufe3/roh/` (nicht versioniert, nicht fuer den Nutzer).

## 1. Bestand vor dieser Stufe (gemessen im Quelltext nach dem Merge)

**5080/5081 (Bahnsteig).** RDT sub00 @0x00746 `44 00 30 33 00 00 00 ff 4c b9 20 ea c8 00 ..` = Typ 0x30, grid 0x33, Lage
(-18100,-5600,200), solange Flag (3,48) = 0 (@0x0072A). grid 0x33 friert die Wurzel ein (`re15_birkin_root`, Bit 0x20), bis sub03
nach der Generator-Folge Pos_set (-18100,-5600,-17800) @0x00822, Speed_set/Add_speed (Fall auf y 0) und Member_set(0x0C,0x13)
@0x0083E faehrt. Port heute: Original-EM30 kaempft (`re15_birkin_ai_tick`), Tod -> Todesmaschine Phase 2 setzt Flag (5,0x1c)
(@0x8011aecc STAGE5), sub01 @0x0078A sieht das, setzt (3,48)/(3,49), Musik, sub04. **Die Bahnsteig-Form kommt in 5080 nicht vor.**

**5090/5091 (Zug).** sub00 @0x0124A Typ 0x30 grid 0x33, sub04 Member_set(0x0C,0x13) @0x0130A. Port heute: EM30 kaempft, toedlicher
Treffer -> Mutation (Sub 6, Clip 0x14) -> `re15_bf_mutation_fertig` laedt die Bank neu -> RE2/BAHNSTEIG_G4.EMD (G4 umgeformt,
27 Knochen) -> `re15_bb_tick` (engine/src/birkin_bahnsteig.c, Spur R) -> beim Tod Rueckstoss-Szene (`re15_bf_rueckstoss_start`,
birkin_formen.c) -> G5 (Typ 0x36). Belegt in R_bahnsteig.md 8.1 (Leon 5090, Elza 5091 gleich).

**Soll nach E5/E6/E17 (Nutzer):** 5080 Riesenform, Kampf endet mit Flucht; 5090 Riesenform, wird aus dem Zug geschossen; dann G5.
Die Mutation zur Riesenform liegt nach E6/E17 am Ende des letzten Sichel-Form-Kampfs (Spur SF, Baum r41_sichel), also VOR dem Bahnsteig.
Folge fuer diese Stufe: in 5080 und 5090 traegt der 0x30 die neue Form ab der Raumladung; die EM30-Runde samt Mutation in 5090 entfaellt.

## 2. Gerippe J

Gelesen aus `varianten/J_dicker/FORM.EMD` (`stufe3/gen/s3_lesen.py`): 32 Knochen (Port-Grenze RE15_EMD_MAX_BONES = 32, voll), 32 Meshes,
Mesh i an Knochen i, Eltern vor dem Kind.

| Knochen | Eltern | Rolle |
|---|---|---|
| 0 | - | Huefte, Wurzel |
| 1-6 | 0,1,..,5 | Schwanz, Kette von sechs Gliedern nach -X (Versaetze 592..678) |
| 7, 10 | 0, 7 | Wirbel, Brust (= BB_TEIL_RUMPF 10) |
| 8/24/25, 9/26/27 | 0/8/24, 0/9/26 | Bein links/rechts: Oberschenkel, Unterschenkel, Fuss (kantig, Sohle) |
| 11/12/13 | 10/11/12 | Arm links: Oberarm, Unterarm, Hand (= Teil 13, Hieb Seite 1); Klauen 14, 15, 28, 30 |
| 16/17/18 | 10/16/17 | Arm rechts: Oberarm, Unterarm, Hand (= Teil 18, Hieb Seite 0); Klauen 19, 20, 29, 31 |
| 21/22/23 | 10/21/22 | Hals, Kopf, Kiefer |

Alle Teile, die eine Bewegung braucht, haben schon einen eigenen Knochen; Teil 10/13/18 sitzen dort, wo der Tick sie erwartet. Neue Knochen
gingen ohnehin nicht (32 von 32). Posen von J: Clip 0 = Bild 1, Clip 1 = Bild 2, Clip 2 = Ruhe (`form.json`).

**Bewegungspruefung.** Die Form ist starr je Knochen (das EMD kennt keine Gewichte, Mesh i haengt an Knochen i): eine Drehung verzerrt nie
ein Glied, sie kann nur an Gelenken Spalten oder Ueberlappungen oeffnen. Die Posen Bild 1 und Bild 2 von J drehen Beine, Arme, Kopf und
Schwanz schon weit aus der Ruhe (Unterschenkel bis 100 Grad, Fuss bis 160 Grad) und bestehen die Formpruefung; die Clips bleiben in diesem
Bereich. An Gerippe, Mesh und Textur ist nichts geaendert (MD1 und TIM bytegleich mit J, Test). Zahlenpruefung je Clip:
`stufe3/gen/s3_pruef.py` (tiefster Punkt je Bild, Gleiten stehender Glieder in der Welt, groesste Drehung je Bild), `s3_dreh.py`
(Spruenge je Knochen), `s3_reichweite.py` (Hiebe gegen die Trefferprobe). Sichtpruefung: Kontaktblaetter, Abschnitt 6.

## 3. Clips (Asset BAHNSTEIG_FORM.EMD, 28 Clips, 760 Bilder)

Generator: `stufe3/gen/s3_clips.py` (ohne Argumente, schreibt `stufe3/asset/`, deterministisch: md5 zweier Laeufe gleich), Posen und
Angriffe `s3_angriffe.py`, Gerippe/IK `s3_rig.py`. Danach `cp stufe3/asset/BAHNSTEIG_FORM.* re15_port/shared_assets/RE2/` und
`python stufe3/gen/s3_testkopf.py` (Testreferenz `re15_port/tests/unit/form_j_posen.h`). `form_emd.py rundlauf` auf dem Asset:
byte-gleich, `FE.pruefe` 0 Fehler, 0 Warnungen.

**Aus G4 (Daten, kein Code):** RE2 CDEMD0.EMS kind 0x34 rec 3 (Sektor 3310, Datei 0x677000; EDD Paar 1 ab 0x67b960, EMR Paar 1 ab
0x67d05c). Je Tick-Clip unveraendert: Bildzahl, EDD-Bits 16..31 (Treffer Bit 16, Ton Bit 27 + Id) und Spur (Keyframe +6/+10). Der Test
vergleicht jedes Bild (Clip 10 ausgenommen, s.u.). Damit gelten im Tick unveraendert: Doppelschlag k_doppel @0x8010b0f0, Trefferbilder
@0x80101cb8 / @0x80104f58, Griff Bild 8 @0x80105408, Zucken ab Bild 5 @0x80107224, Lenkschwellen 21/31 @0x80102924-68, Ueberrennen 201
@0x80102e80, Gang 2666 Einheiten je Durchlauf.

**PORT-WAHL:** alle Posen und Zeitpunkte in den Clips, Fussarbeit, Schwanzwelle, die neuen Clips 2, 11, 12 samt Laenge, Spur und Toenen
(Ton-Ids aus der G4-Bank 24), die Platzhalter 1, 6, 7, 13-25 (je ein Bild Ruhepose, vom Tick nie gespielt).

| Clip | Rolle | Bilder | Treffer | Ton (Bild: Id) | Herkunft |
|---|---|---|---|---|---|
| 0 | Rennen (Sprunggalopp) | 18 | - | 17: 1 | G4 Clip 0; **Bild 11 = Pose Bild 1** (Rueckansicht Bahnsteig) |
| 2 | Stehen/Atmen | 60 | - | - | PORT-WAHL; Bild 0 = Ruhe von J |
| 3 | Hieb rechts (Teil 18) | 60 | 26-30 | 25: 1, 32: 0 | G4 Clip 3 |
| 4 | Hieb links (Teil 13) | 50 | 14-17 | 13: 1, 19: 0 | G4 Clip 4 |
| 5 | Gehen (auch Weichen rueckwaerts) | 89 | - | 43: 0, 73: 0 | G4 Clip 5 |
| 8 | Zucken/Treffer | 30 | - | - | G4 Clip 8 |
| 9 | Nachhieb (beidhaendig auf den Boden, Bruellen) | 63 | - | 1: 0, 24: 6 | G4 Clip 9 |
| 10 | Drohen/Auftritt | 126 | - | 30: 11, 51: 10, 74: 1, 79: 0, 81: 12, 113: 11, 114: 1 | G4 Clip 10; **Bild 45-52 = Pose Bild 2** |
| 11 | Flucht (Ende 5080): Gebruell, Galopp, Sprung nach oben | 57 | - | 5: 10, 37: 1 | PORT-WAHL |
| 12 | Sturz aus dem Zug (Ende 5090) | 56 | - | 2: 10 | PORT-WAHL; **Bild 0 = Pose Bild 2** |
| 26 | Biss | 35 | 16-30 | 19: 5, 27: 5 | G4 Clip 26 |
| 27 | Griff (Packen Bild 8, Schleudern, Bruellen) | 100 | - | 14: 8, 36: 9, 60: 9, 82: 7 | G4 Clip 27 |

Die Posen von J stehen bytegleich im Asset (Wurzel + 32 Winkel; Test `pose_gleich`): Ruhe = Clip 2 Bild 0, Bild 1 = Clip 0 Bild 11
(auch zweimal in der Flucht, die den Galopp wiederholt), Bild 2 = Clip 10 Bild 45-52 und Clip 12 Bild 0. **Formpruefung von J am
Spiel-Asset gelaufen** (`stufe3/formpruefung.json` -> `stufe3/formpruefung/bericht.txt`, `form_pruefung.py pruefe`): Bild 1 IoU 0,948,
Pruefpunkte 31/31, Boden/Verdeckung 5/7; Bild 2 IoU 0,935, Pruefpunkte 7/7, Boden/Verdeckung 9/9 - Zahl fuer Zahl der Bericht von J;
`FE.pruefe` jetzt 0 Fehler, 0 Warnungen (J hatte die Warnung "Tick erwartet Clips, die fehlen").

Abweichung von G4 mit Grund, **Clip 10 Spur**: Pose Bild 2 steht in Bild 45-52 still; G4 bewegt die Figur dort um zusammen rund -400.
Dieses Tempo liegt jetzt in Bild 53-60, die Summe (47) ist die von G4. Der Auftritt bedient keine Schwelle des Ticks.

**Wie gebaut:** Gehen = Seitenfolge-Gang, zwei Zyklen je 89 Bilder, Fuesse und Haende per IK in der Welt verriegelt gegen die G4-Spur
(Gleiten <= 8 je Bild), Fuss rollt ab, Schwanz als Welle. Rennen = Sprunggalopp, Glieder je Bild per IK ueber Stuetzpunkte, Basis Pose
Bild 1. Hiebe: Arm hoch und weit zur Seite, dann quer nach vorn unten; der Einstieg des Doppelschlags (Clip 3 Bild 20, Clip 4 Bild 7)
liegt ausserhalb der Trefferprobe (Teil 13/18 Radius 1700, Versatz (0,600,0) Radius 1500, Spieler 2600 voraus, `s3_reichweite.py`),
getroffen wird in Bild 28-30 bzw. 15-17. Biss: Kopf zurueck, Maul auf, Vorstoss in Brusthoehe des Spielers (Kiefer bei y -1460), Schuetteln. Griff: beide Arme greifen (Krallen zu in
Bild 8), Schleudern, Bruellen mit wechselnden Armen. Drohen: Hocke, Gebruell, Pose Bild 2, Aufbaeumen mit beiden Armen.

**Grenzen, gemessen (`s3_pruef.py`):** tiefster Punkt <= 25 unter dem Boden ausser Rennen Bild 11 (Pose Bild 1 selbst: linker Fuss 334
unter der Ebene, so wurde J an Bild 1 gefittet) und Drohen Bild 33-38 (Schienbein streift auf dem Weg in die Hocke bis 242, Bild 35-38; je Bein eigenes Faltfenster, damit der Rueckweg nicht ueber den Boden schleift: Gleiten 245 -> 23). Schweben bis
90 im Ausfall der Hiebe und im Aufbaeumen. Groesste Drehung je Bild: Gehen 12 Grad, Rennen bis 106 Grad (Fuss beim Abheben). Flucht und
Sturz verlassen absichtlich den Boden.

## 4. Angriffsmuster und Ablauf im Spiel

Tick `engine/src/birkin_bahnsteig.c` (Spur R): Sub-Nummern, Entscheidungen, Reichweiten, Kegel, Sperren, Schaden, Trefferzaehler
unveraendert mit ihren @0x-Adressen aus dem G4-Overlay (CDEMD0.EMS rec 1). Die Clipnummern BB_C_* bleiben, die Form bedient sie.
Neu in Stufe 3 (PORT-WAHL nach E5/E6/E17; RE-Zahlen nur, wo genannt):

| Stelle | Was | Beleg / Kennzeichnung |
|---|---|---|
| birkin_formen.c `re15_bf_riesenform_raum`, `re15_bf_bahnsteig_bank_faellig` | Riesenform in 5080/5081 und 5090/5091 ab der Raumladung; in 5090 keine EM30-Runde, keine Mutation (`re15_bf_mutation_offen` = 0) | E5/E6/E17 |
| birkin_bahnsteig.c `bb_flucht` (Sub 21) | Tod in 5080/5081 -> Clip 11: abdrehen (128 je Bild im Gebruell, Gestaltung; danach 32 wie @0x801018ec-908), Galopp, Sprung nach oben; am Clip-Ende Flag (5,0x1c) wie der Birkin-Tod @0x8011aecc (STAGE5), den ROOM5080 sub01 @0x0078A abfragt; Aktor aus. Treffer halten die Flucht nicht auf. Riegel wie im Todeszweig @0x8011a458 / @0x8011a54c-50 | PORT-WAHL |
| birkin_formen.c Rueckstoss Phase 3, `re15_bb_sturz_tick` | am Zugende Clip 12 einmal (Spur schiebt rueckwaerts hinaus), Kamera Cut 11, dann Phase 2 (unsichtbar, Pause), G5 wie bisher | PORT-WAHL |
| enemy_ai_common.c `re15_birkin_root`, `re15_bb_wartet` | solange grid & 0x20 die Wurzel einfriert (@0x80116a7c-a88), spielt die Riesenform Clip 2 im Kreis (5080: Fall vom Dach, 5090: steht im Wagen) | PORT-WAHL |
| birkin_formen.c Messhaken `RE15_BIRKIN_HP_AT` | gilt jetzt auch in 5080 | Messhilfe |

Unveraendert: Schatten 2300 x 1300 (G4 @0x801002cc / @0x80100300) und Kastenversatz 800 (Gestaltung Spur R); die Form ist ohne Schwanz
rund 3600 lang, Haende bei z +-1590. Opfer-Bank EM30. Teile 10/13/18 = Brust / Hand links / Hand rechts.

**Uebergabe an den G5 (Abgrenzung zur Spur r41/g5auftritt, Vorgabe Orchestrator).** Diese Stufe baut in 5090 die Riesenform bis
einschliesslich ihres Sturzes aus dem Zug. Der Uebergabepunkt ist EINE Funktion in birkin_formen.c:
`bf_rs_form_ist_draussen(e, mit_gebruell)` - aufgerufen genau einmal, wenn die Form den Wagen verlassen hat (Riesenform: Ende von
Sturz-Clip 12 in Rueckstoss-Phase 3; ohne Riesenform, z. B. `RE15_BIRKIN_FORM=original`: am Wagenende wie bisher, mit Gebruell). Sie
setzt Phase 2 (Form unsichtbar, Pause `BF_RS_PAUSE`), danach laufen unveraendert `re15_bf_morph_zu_g5` und die Szenenkamera `s_kam_g5`.
Nicht angefasst: G5-Auftritt, G5-Kamera, `enemy_ai_boss_g5.c`. Wer den neuen G5-Auftritt baut, setzt an `bf_rs_form_ist_draussen` an.
Ablauf der Phasen in 5090: 1 Beschuss/Zurueckdraengen (Clip 8 im Kreis) -> 3 Sturz (Clip 12, Kamera Cut 11) -> Uebergabe -> 2 Pause -> G5.

## 5. Einbau und Tests

- Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`; `BAHNSTEIG_G4.EMD/.TIM` aus dem Repo entfernt (nicht mehr geladen;
  `tools/birkin_bahnsteig.py` bleibt als Beleg von Spur R). Lader `pc_enemy_load_bahnsteig` (main.c), debug.log
  `Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips`.
- Paket-Gate `release/make_package.sh`: BAHNSTEIG_FORM.EMD/.TIM statt BAHNSTEIG_G4.* (vorhanden, nicht leer, bytegleich).
- `form_spiel.sh`, `form_emd.py rundlauf` (Vorgabedatei) auf dem neuen Namen.
- `tests/unit/test_birkin_bahnsteig.c`: `test_asset` neu (Formtreue gegen `form_j_posen.h`: MD1/TIM-CRC, drei Posen; Clips gegen RE2-G4:
  Bildzahl, EDD-Bits, Spur; Mindestlaengen 31/21/6/9; Trefferbits 3/4/26; Tempo Gehen >= 31, Rennen + 10 >= 201; Stehen/Flucht/Sturz),
  `test_masse` (Boden mit begruendeten Ausnahmen, Hoehe Clip 5 = 3126 +-60 als Formtreue-Pin), neu `test_flucht` (5080, 5081) und
  `test_wartet` (5080, 5090, 5091). Entfallen: M1/M3/M4 (Tausch EM30 -> Form nach der Mutation in 5090, den es nicht mehr gibt).
  Arrays von 27 auf 32 Knochen.
- `tests/unit/test_birkin_szenen.c`: `test_bahnsteig` ohne EM30-Mutation, prueft den Sturz-Clip; `test_modellwahl` mit 5080/5081/5091/50A0.
- `tests/integration/test_r41_birkin_pistole.cmake`: 32 Meshes, keine Mutation, Sturz (Clip 12) zwischen Rueckstoss und G5.

## 6. Sichtbelege (fuer den Nutzer)

**Kontaktblaetter je Clip** `stufe3/kontakt/clip_NN_name.png` (`stufe3/gen/s3_kontakt.py`, Offline-Buehne `form_buehne.py`, Textur und
Gouraud ohne Raumlicht): Zeilen = rechte Seite, schraeg vorn links, schraeg hinten rechts; Spalten = Bilder des Clips; die Figur laeuft um
ihre Spur weiter, die Kamera folgt, das Bodenraster steht in der Welt (Gleiten waere sichtbar). Marken unter dem Bild: T = Trefferbild,
S<n> = Ton-Id. Flucht und Sturz mit fester Kamera.

**Spiel, echtes Fenster** (`stufe3/gen/s3_spielfolge.sh`: eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER,
ffmpeg gdigrab `hwnd=` auf das eigene Fenster, nur die eigene PID beendet; Blaetter mit `s3_folge_blatt.py`) unter `stufe3/spiel/`:

| Blatt | Lauf | Inhalt |
|---|---|---|
| `5090_leon_1_auftritt_angriffe.png` | Debug-Sprung 5090, Leon laeuft in den Wagen, 10 s ohne Feuer | Auftritt (Drohen) im Gang, Gang, Doppelhieb, Nachhieb, Griff |
| `5090_leon_2_rueckstoss_sturz_g5.png` | danach Zielen + Feuer gehalten (Pistole) | Rueckstoss-Szene, Sturz aus dem Wagenende in Cut 11 (Kamera von Bild 2), RE2-G5 kommt herein |
| `5080_leon_1_fall_kampf.png` | Debug-Sprung 5080, Generator-Folge (Skript lauf1 aus Runde 30) | Riesenform faellt nach dem Generator herab, Auftritt, Angriffe an Leon |
| `5080_leon_2_flucht.png` | Messhaken `RE15_BIRKIN_HP_AT=1100` (HP 0 statt langem Beschuss) | Flucht: abdrehen, Galopp aus dem Bild, danach Raumtext msg 3 wie nach dem Birkin-Tod |
| `5091_elza_uebersicht.png`, `5081_elza_uebersicht.png` | dieselben Laeufe mit Elza (`RE15_TITLE_SHOT_CHAR=1`) | gleicher Ablauf |

Gemessen in diesen Laeufen (birkin_dbg.log, state.log je Lauf unter `re15_port/build/s3_spiel/<name>/`): debug.log `Bahnsteig-Form: 32
Meshes, 32 Knochen, 28 Clips` in allen vier Raeumen. 5090/5091: Auftritt 126 Bilder, Gang, Hieb 3 -> Doppel 4 -> Nachhieb 9, Griff (Leon
fliegt), Treffer bis HP 0, Rueckstoss (Clip 8, HP 1), Sturz (Clip 12 bei x -6290 -> -15320), G5 (Typ 0x36). 5080/5081: nach der Generator-
Folge (msg 0/1 F247/F500) Fall vom Dach, Auftritt, Rennen, Weichen, Griff, Biss, Hieb; Flucht (Sub 21, Clip 11) bei F1694, fort bei
F1749, msg 3 bei F1751 (sub04 nach Flag (5,0x1c)). Elza 5081 und 5091 identisch.

Beobachtet: In Cut 12 (Kamera vor Leon) ist die Form im Nahkampf meist nur angeschnitten (sie kommt von hinter der Kamera); im Gang
(Cut 14/15) und am Generator ist sie ganz zu sehen. Die Form ist so gross, wie J nach den Screenshots ist (Hoehe im Gehen 3126); sie
fuellt den Wagen fast aus.

## 7. Offen

- Sturz-Clip 12 nach dem ersten Spiellauf ueberarbeitet (stand auf der Schwanzspitze senkrecht, weil Pose Bild 2 schon 52 Grad aufgerichtet
  ist): jetzt hoechstens 22 Grad dazu, Fall mit Fallkurve. Im Spiel neu zu sehen (Abschnitt 0).
- Drohen Bild 33-38: Schienbein streift bis 242 unter den Boden (Bild 35-38) (Weg in die Hocke von Bild 2), Rennen Bild 11 = Pose Bild 1 mit Fuss 334
  unter der Ebene (so an Bild 1 gefittet). Beides im Test als begruendete Ausnahme.
- Die Flucht laeuft vom Spieler weg; die Wandklemme der Wurzel bleibt an, an einer Wand rutscht die Form seitlich weiter, bis der Sprung
  sie aus dem Bild nimmt (5080 gemessen: von z -22553 bis -14139).
- Integration in master und Abgleich mit Spur SF (r41/sichel: Mutation am Ende der Sichel-Form, Effekte) stehen aus; beide Zweige aendern
  birkin_formen.c, main.c, test_birkin_szenen.c.
