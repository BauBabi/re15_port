# Stufe 3 Bahnsteig-Form - Unabhaengige Abnahme 1

Pruefer: unabhaengig (nicht der Autor). Gegenstand: Kopf 4dd925edd (r41/form), Arbeitsbaum .claude/worktrees/r41_form.
Bilder: stufe3/abnahme1/. Laeufe (Logs, Einzelbilder): re15_port/build/abn1_spiel/<lauf>/ (nicht versioniert).
Bindend: NUTZER_ENTSCHEIDE.md E1-E20. Kopf-/Krallenform (E20, Variante K) wird hier NICHT als Mangel gewertet; der Nutzer hat
Kopfvariante K3 gewaehlt (varianten/K_alligator/), sie kommt mit der Nachbesserung (Abschnitt 6 gilt dafuer).

## 0. Urteil und Stand

**Urteil: NICHT BESTANDEN.** Tick, Clipdaten und Form sind wie behauptet (Abschnitt 3, 4), aber der Kern von E5 ist im
echten Fenster nicht zu sehen: der Sturz aus dem Zug (Clip 12) laeuft ausserhalb des Bildes ab (M1). Dazu kommen die Loecher an
Schulter und Huefte (M2, Nutzerbefund, belegt), stumme Gebruelle (M3), Kamera-Nahsicht (M4), Gleiten (M5, M6) und Spieler
in der Form (M7).

| Schritt | Stand |
|---|---|
| Bau re15_port/build (local_build.sh configure + build) | OK, 818/818 Ziele, 2026-11-02 14:19, Kopf 4dd925edd |
| Gezielte Birkin-/Form-Tests (19 Tests, Liste 5.1) | 19/19 gruen |
| Voller ctest -j 6 --timeout 900 (1408 Tests) | auf Anweisung des Orchestrators abgebrochen: Teilstand 153/1408, 0 rot (Abschnitt 5.3) |
| Sichtlaeufe echtes Fenster, Leon und Elza, 5080/5081/5090/5091 | erledigt (Abschnitt 1) |
| Tick gegen RE2-G4 (Stichproben im Overlay) | erledigt, alle Stichproben stimmen (Abschnitt 3) |
| Code, Paket-Gate, BAHNSTEIG_G4 | erledigt (Abschnitt 4) |

Messweg: eigene exe-Kopie je Lauf (re15_pc_abn1f_*), Renderer des Nutzers, kein RE15_AUTOSHOT, kein RE15_SOFTWARE_RENDER,
ffmpeg gdigrab hwnd= auf das eigene Fenster, Fensterskala 2 (640x480), nur die eigene PID beendet. RE15_BB_POSE (Messhaken
des Autors) nur fuer gezielte Posen/Kameras, so gekennzeichnet.

## 1. Sichtlaeufe (echtes Fenster)

| Lauf | Raum/Figur | Eingabe | Ablauf laut state.log |
|---|---|---|---|
| l5090_feuer | 5090 Leon | wie integration_r41_birkin_pistole (Debug-Sprung, laufen, Zielen+Feuer gehalten) | Auftritt Clip 10 F281, HP 150 -> 1 F645, Rueckstoss F646-F754, Sturz Clip 12 F754-F811, G5 F855, G5 HP -1 F1987 |
| e5091_feuer | 5091 Elza | dasselbe, RE15_TITLE_SHOT_CHAR=1 | Bild fuer Bild derselbe Ablauf (F281/F645/F754/F855) |
| l5090_angriffe | 5090 Leon | laufen, dann stehen ohne Feuer | Hieb 3 Treffer F533 (Bild 26), Hieb 4 F544, Nachhieb, Griff F651 (Bild 8, -35), Biss F1062 (Bild 17), Tod F1277 |
| l5080_leon | 5080 Leon | Generator-Folge lauf1 (Runde 30), dann Feuer | Auftritt F851, Kampf Cut 4, HP 150 -> 1 F1408, Flucht Clip 11 F1408-F1465, Flag (5,0x1c), msg 3 F1468 |
| e5081 | 5081 Elza | dasselbe | identisch (Flucht F1408, msg 3) |

msg 3 = "The power already has been supplied to the train!" ist die Auslieferungsfolge nach dem Birkin-Tod:
ROOM5080 sub01 @0x0078A `Ck 21 05 1c 01` -> Set (3,48)/(3,49), Sce_bgm_control, `Evt_exec 04 ff 18 04` @0x007A6 -> sub04
`Message_on 2b 03` @0x0084C. Der Flag-Setzer im Original: STAGE5 @0x8011aea4-ac `a0 = 0x800b1028, a1 = 0x1c`,
@0x8011aecc `jal 0x8004ef90` (in info/Re1.5 und Version_40% gleich).

Bildleisten: 5090_leon_feuer_ablauf.png, 5090_leon_ohne_feuer.png, 5091_elza_feuer_ablauf.png, 5080_leon_ablauf.png,
5081_elza_kampf_flucht.png; Einzelbelege 5090_sturz_unsichtbar.png, kamera_nahsicht_5090.png, 5081_elza_unter_form.png,
loecher_echtes_fenster.png, loecher_buehne_je_clip.png, bild2_vorlage_gegen_port.png.

**Positiv gesehen:** Form im Spiel = J (Umriss, Textur, Flecken, Schwanz glatt), im 5080-Licht (Cut 4) gut lesbar
(violett-grau, rosa Band, weisse Flecken); Fall nach der Generator-Folge, Auftritt, Gehen, Hiebe, Doppelschlag, Nachhieb,
Griff mit Wurf, Biss; Flucht 5080 mit Aufbaeumen, Galopp, Sprung nach oben aus dem Bild, danach Raumtext; G5 kommt in 5090;
Leon und Elza gleich.

## 2. Maengel

| Nr | Was | Beleg | Schwere | Vorschlag |
|---|---|---|---|---|
| M1 | **Sturz aus dem Zug unsichtbar.** Phase 3 startet erst bei x <= BF_RS_AUSGANG_X-600 = -6283 (birkin_formen.c bf_rueckstoss_tick), also schon ausserhalb der Wagentuer (-5683). Cut 11 zeigt die Form dort nicht: ab ~F733 ist die Tueroeffnung schwarz (auch mit Gamma 0,35), Clip 12 (F754-F811, x -6313 -> -14583) ist nie im Bild. Messhaken in Cut 11: x -3000 ganz, -5000 halb, -6300 gar nicht sichtbar. Das Autorblatt spiel/5090_leon_3_sturz_detail.png zeigt nicht den Sturz, sondern den hereinkommenden G5 (Autorlauf z5090_sturz: Cut 11 F986-F1403, Sturz F1031-~F1088, G5 ab F1132; das Blatt sind die letzten 5,2 s vor dem Schnitt). | 5090_sturz_unsichtbar.png; l5090_feuer/e5091_feuer state.log | **hoch** (E5 "wird aus dem Zug geschossen" ist der Hoehepunkt) | Sturz im Wagen beginnen, wo Cut 11 ihn zeigt (Wurzel um x -2500..-4000; der Clip traegt die Form mit seiner Spur durch die Tuer), oder die Rueckstoss-Grenze fuer die Riesenform entsprechend vorziehen; danach im echten Fenster Bild fuer Bild belegen. Autorblatt 3 neu beschriften. |
| M2 | **Loecher zwischen Bein und Oberkoerper** (Nutzerbefund): an der Achsel (Oberarm-Platte gegen Brust) und an der Huefte (Keil zwischen Oberschenkel und Bauch) sieht man den Hintergrund. Im echten Fenster belegt, 5080 Schnitt 4 (Kampfkamera), Clip 5 Bild 28 Gier 2048 (Achsel ~1060 px), Clip 5 Bild 28 Gier 0 (Huefte ~1070 px), Clip 10 Bild 100 Gier 2048 (Achsel ~1650 px), je 640x480. Buehnen-Suche (gleiche Kamera, 12 Bilder x 8 Gier je Clip, geschlossene Hintergrundinseln >= 300 px, enthaelt auch natuerliche Luecken zwischen den Beinen): Clip 0 15, 2 22, 3 16, 4 19, 5 19, 8 25, 9 21, 10 26, 11 13, 12 13, 26 31, 27 21 Ansichten von 96. Gesichtet als echte Gelenkluecken: Stehen 2/45 (Huefte), Rennen 0/15 (Achsel), Zucken 8/16 (Achsel), Drohen 10/90-100, Biss 26/10 (Achsel), Griff 27/40 (Huefte); Clip 4/16 Gier 1024 ist dagegen die natuerliche Luecke Schwanz-Bein. | loecher_echtes_fenster.png (links echt, rechts Lochrand aus der Buehne eingezeichnet), loecher_buehne_je_clip.png | **hoch** (E19 "nichts zerrissen", Nutzerbefund) | in Variante K (laeuft): Gelenkkoerper an Schulter 11/16 und Huefte 8/9; Abnahmemass = keine geschlossene Hintergrundinsel an Schulter/Huefte in allen Tick-Clips aus 8 Gier, Buehnen-Suche (Skript loecher.py dieses Pruefers) plus Stichprobe im echten Fenster. |
| M3 | **Gebruelle stumm.** Ton-Ids 9, 10, 11, 12 sind in ENEMSE-Bank 24 leer: RE15_RE2SE_LOG im Lauf ton5090 meldet `se=10/11/12/9 bank=24 SILENT`. Betroffen: Auftritt/Drohen Clip 10 (Bild 30/51/81/113), Griff Clip 27 (Bild 36/60), **Flucht Clip 11 Bild 5 (Id 10) und Sturz Clip 12 Bild 2 (Id 10)** - beide PORT-WAHL dieser Stufe. Vor Stufe 3 spielte das Wagenende re15_audio_room_se(10); bf_rs_form_ist_draussen(e, 0) verlaesst sich jetzt auf "Gebruell traegt Clip 12 Bild 2", das stumm ist. | re15_port/build/abn1_spiel/ton5090_re2se.log; debug.log aller Laeufe (bank 24 nur Ids 0-8) | mittel | Fuer die PORT-WAHL-Toene nur hoerbare Ids waehlen (Bank 24: 0-8) oder den Raum-Ton 10 wie vor Stufe 3 (re15_audio_room_se(10), @0x8011a744) an Flucht und Sturz; fuer Clip 10/27 klaeren, woher RE2 die Ids 9-12 des G4 holt (zweite Kartenhaelfte/andere Bank), sonst sind Auftritt und Griff ohne Gebruell. Test: Ton-Ids der Tick-Clips gegen leere Kartenplaetze pruefen. |
| M4 | **Kamera-Nahsicht.** (a) Cut 15 (Kampfstart, F281-F313, Letterbox; RID-Auge x 46, Blick +x): die Form steht 1150 Einheiten vor der Kamera (Pos_set-Lage 1200,-23350), 1 s nur Texturflaeche. (b) Cut 13 (Auge x 11050, Blick +x): nach dem Griff liegt Leon bei x 15191, die Form geht an der Kamera vorbei (x 8223 -> 13380) und steht zwischen Kamera und Leon; sie fuellt rund 8 s 60-90 % des Bildes, Leon ist verdeckt (l5090_angriffe Bild 284-338). Das ist das vom Orchestrator gesehene Bild 61-65. In Bild 2/#145 nimmt das Wesen etwa ein Viertel des Bildes ein, Annette bleibt sichtbar. Das Dunkle selbst ist kein Mangel: im Zug ist die Form wie in Bild 2/#145 dunkel; im 5080-Licht ist sie gut lesbar. | kamera_nahsicht_5090.png, 5090_leon_ohne_feuer.png | mittel | (a) Riesenform im Kampfstart-Schnitt erst ausserhalb des Kamera-Nahbereichs zeigen (Lage oder Schnitt als PORT-WAHL); (b) in 5090 die Form nicht zwischen Kamera 13/15 und Spieler stehen lassen (Abstand zur Kameralage) oder fuer die Riesenform eigene Schnittwahl wie bf_kamera in der Rueckstoss-Szene. |
| M5 | **Rueckstoss gleitet.** Phase 1 schiebt die Form 147 Einheiten je Bild rueckwaerts (F649 x 5994 -> F700 x -1525), waehrend Clip 8 (Zucken, Fuesse stehen) im Kreis laeuft: rund 4400 Einheiten je Sekunde Gleiten auf stehenden Fuessen. Vorher Spur R, gilt jetzt fuer die Riesenform. | l5090_feuer state.log F646-F754 | mittel | Rueckwaerts-Taumeln als Clip mit eigener Spur (Fuesse verriegelt wie im Gehen) oder Vortrieb aus der Clip-Spur statt fester Rate. |
| M6 | **Auf der Stelle laufen an der Wand.** 5080/5081: nach dem Griff steht die Form 8 s (F1158-F1408) bei x -25785..-25670, z -18707 (Wandklemme des Westarms), Weichen (Sub 8, Clip 5 rueckwaerts) und Gehen (Sub 3) wechseln, die Wurzel bewegt sich nicht. Die Beine laufen, der Koerper steht. Flucht: im Gebruell dreht die Form mit 128 je Bild (16 Bilder = 180 Grad) auf stehenden Fuessen. | l5080_leon/e5081 state.log; 5081_elza_kampf_flucht.png Bild 156-192 | mittel | Weichen abbrechen, wenn die Wurzel klemmt (Weg je Bild < Schwelle), und auf Gehen/Angriff wechseln; Drehen in der Flucht ueber Schritte im Clip oder kleinere Rate. |
| M7 | **Spieler in der Form, Glieder in Wand/Gelaender.** Die Form ist breit (Ruhe quer -2258..2162, Hiebe bis +-2964, eigene Messung aus dem Asset), Gaenge 3500 (5090 zwischen den Regalen, SCA z -25200..-21700) bzw. 3200 (5080 Westarm). Klauen stecken in Regalen/hinter dem Gelaender, die Klaue ragt ueber die Bahnsteigkante (5081 Bild 166-181). Elza steht 8 s unter dem Bauch der Form, nur ihre Beine sind zu sehen (5081 Bild 156-192); beim Abdrehen der Flucht laeuft der Schwanz durch Leon/Elza. Koerperzylinder ist der des 0x30 (r 1000). | 5081_elza_unter_form.png, 5081_elza_kampf_flucht.png | mittel | Koerperradius/-kasten der Riesenform an die Form anpassen (Abstand Spieler-Brust >= Kopf-/Brustlaenge), Weichen/Flucht so, dass der Schwanz nicht durch den Spieler schwenkt; in engen Gaengen die Arme enger fuehren. |
| M8 | Kleinere Punkte: (a) Gore: das Treffer-Blut (4 Spawns an Teil 10 je Treffer, @0x80107454-f8) ist in Cut 12 meist hinter dem Spieler verdeckt, an der Form selbst kaum sichtbar (Blutlachen am Boden sichtbar). (b) Der Todeszweig schreibt im Original vor dem Flag noch [*0x800ac784]+0x9c = 300 und +7 = 3 (@0x8011aeb8-d0); die Flucht setzt nur das Flag - ob diese Schreiber in 5080 noetig sind, ist nicht geklaert. (c) tools/birkin_bahnsteig.py schreibt weiter BAHNSTEIG_G4.* nach shared_assets (beim Aufruf entsteht ein verwaistes Asset). (d) Bodenausnahmen bestaetigt: Rennen Bild 11 334, Drohen 242 unter dem Boden (s3_pruef.py selbst gelaufen). (e) Spruenge je Bild (wahre Knochendrehung, eigene Messung): Rennen Bild 15->16 linker Fuss (Kn 25) 106 Grad, Bild 10->11 linker Unterarm (Kn 12) 57 Grad (Einstieg in Pose Bild 1), Drohen Bild 30->31 rechter Fuss (Kn 27) 54 Grad, Hiebe/Nachhieb/Griff Krallen 40-51 Grad. | Abschnitt 1, 3 | niedrig | (a) bei Treffern einen zweiten Ankerpunkt vorn am Kopf/an der Brustvorderseite; (b) nachlesen, wem 0x800ac784 gehoert; (c) Ausgabe des Werkzeugs auf eine Zwischenablage umstellen; (e) Zwischenbilder einfuegen bzw. Pose Bild 1 ueber 2-3 Bilder ein- und ausleiten. |

## 3. Tick gegen RE2-G4 (Stichproben, selbst disassembliert)

Overlay: info/re2leon/PL0/PLD/CDEMD0.EMS, Sektor 3238 (Datei 0x653000), 0xB2D4 Byte, gelinkt @0x80100000, capstone MIPS32 LE.

| Behauptung | Bytes / Instruktion | Ergebnis |
|---|---|---|
| Doppelschlag k_doppel @0x8010b0f0 = {30,7},{19,20} | `1e 07 13 14` | stimmt |
| Trefferbit @0x80101cb8 (Hieb) | `3c030001 lui v1,1` / `00431024 and` / `beqz` | stimmt |
| Trefferbit @0x80104f58 (Biss) | `3c030001 lui v1,1` / `and` / `beqz 0x801050c4` | stimmt |
| Griff Bild 8 @0x80105408 | `9223014d lbu v1,0x14d(s1)` / `24020008 addiu v0,zero,8` / `bne` | stimmt |
| Zucken ab Bild 5 @0x80107224 | `lui v0,7; ori v0,0x508` -> Clip-Wort 0x00070508 (Clip 8, Bild 5, Blende 7), `sw v0,0x14c(s0)` | stimmt |
| Lenkschwellen 21/31 @0x80102924-68 | `slti v0,v0,0x15` @0x8010292c, `slti v0,v0,0x1f` @0x80102950 auf +0x144 | stimmt |
| Ueberrennen ab 201 @0x80102e80 | `lh v0,0x144(s3)` / `slti v0,v0,0xc9` @0x80102e88 | stimmt |
| Griff-Schaden @0x801052a8-b4 | `slti v0,v1,0xab` / `addiu a0,zero,0x46` / `addiu a0,v1,-0x64` -> Port 35 bzw. hp-50 (halbe Skala) | stimmt |
| Hieb/Biss/Rennen-Schaden | RE2: Hieb `addiu a0,zero,0xf` @0x80101dcc, Biss 0x14 @0x8010501c, Rennen 0x14 @0x80102f0c (200-HP-Skala). Port 10/10/5 nach RE1.5-0x30 (Kommentar birkin_bahnsteig.c:245-247) | dokumentierte Wahl aus Spur R, nicht Stufe 3 |

Im Spiel gemessen (l5090_angriffe): Hieb rechts trifft in Clip 3 Bild 26 (Fenster 26-30), Hieb links Bild ~15 (14-17), Griff
Bild 8, Biss Bild 17 (16-30); Reihenfolge Auftritt -> Gehen -> Hieb -> Doppel -> Nachhieb -> Gehen -> Griff -> Biss wie der
Entscheider. Clipdaten: die Tick-Clips tragen laut unit_birkin_bahnsteig (gruen) Bildzahl, EDD-Bits 16..31 und Spur von G4.
Formtreue: Knochen und Meshes des Assets gleich J (form_emd.lese_emd, Listenvergleich), TIM md5 80de69ca... = J;
form_pruefung.py am Spiel-Asset selbst gelaufen: Bild 1 IoU 0,948, 31/31, Boden 5/7; Bild 2 IoU 0,935, 7/7, 9/9 - wie behauptet.

## 4. Code, Paket, Altlasten

- Neue Konstanten der Stufe: Flucht 20000 und 128 je Bild als Gestaltung markiert, 32 @0x801018ec-908, Gewichte 0x200/0x100 mit
  @0x; Phase 3 PORT-WAHL. Kein unmarkierter Wert gefunden.
- re15_bf_mutation_offen() liefert 0; der EM30-Mutationszweig in 5090 ist damit tot (gewollt, E6/E17).
- Paket: release/make_package.sh prueft BAHNSTEIG_FORM.EMD/.TIM (vorhanden, nicht leer, bytegleich) und kopiert RE2/ ganz;
  Android (apk_asset_gate.py) nimmt shared_assets/RE2 ganz. In Ordnung.
- BAHNSTEIG_G4: im Code nur noch in tools/birkin_bahnsteig.py (Werkzeug, s. M8c) und in Kommentaren (main.c:1104,
  form_emd.py:305); nicht geladen, nicht im Paket-Gate, Dateien aus dem Repo entfernt.

## 5. Tests

5.1 Gezielt (Bau 4dd925edd, ctest -j 6): unit_birkin_ai, _bahnsteig, _formen, _opfer, _szenen, unit_r41_form_emd,
unit_r41_birkin_wand_rein/_g5/_griff, unit_g5_boss/_morph/_tentakel, probe_p2_birkin_g5, probe_p3_birkin_rest,
unit_r17_birkin_1zu1_pin, unit_r18_birkin_restluecken_pin, unit_r20_birkin_push_pin, unit_r30_birkin_frost,
unit_r30_n_room5080_birkin_wand: **19/19 gruen**.

5.2 integration_r41_birkin_pistole im vollen Lauf: **gruen** (252,8 s; Leon und Elza: keine Mutation, Form HP 150, Rueckstoss, Sturz Clip 12, G5, G5 faellt).

5.3 Voller ctest (Bau 4dd925edd, `ctest -j 6 --timeout 900`, Start 14:22, unter Last von drei parallelen Suiten): nach 3 h auf
Anweisung des Orchestrators abgebrochen (nur der eigene Prozessbaum, PID 12484). **Teilstand 153 von 1408 gelaufen, 153 gruen,
0 rot, 0 Timeout.** Gelaufen sind zuerst die langen integration_*-Tests (Kostenplanung von ctest), darunter
integration_r41_birkin_pistole. Protokoll: stufe3/abnahme1/ctest_teilstand.log. Die Nachbesserung laesst die volle Suite neu laufen.

## 6. Was ein Alligator-Kopf mit offenem Maul (E20, Variante K/K3) fuer Clips und Tick bedeutet

- Knochen: 32 von 32 belegt (RE15_EMD_MAX_BONES). Oberkiefer/Zaehne muessen an Knochen 22 (Kopf), Unterkiefer an 23 haengen,
  Krallen an den vorhandenen Klauenknochen 14/15/28/30 bzw. 19/20/29/31; neue Knochen gehen nicht.
- Posen-Pins: Bild 2 zeigt das Maul offen. Clip 10 Bild 45-52 und Clip 12 Bild 0 sind heute bytegleich Pose Bild 2 von J
  (Test pose_gleich, form_j_posen.h); mit K muessen Knochen 23 (Kieferwinkel) und die Pins neu (s3_testkopf.py), die
  Formpruefung Bild 2 mit den neuen Masken maske_oberkiefer/maske_unterkiefer.
- Biss Clip 26: der Treffer wird nicht am Kopf gemessen, sondern an Teil 10 (Brust) + (1400,0,0), Radius 1500
  (bb_biss, @0x80104f88-a4). Eine laengere Schnauze verschiebt den sichtbaren Kontakt nach vorn: Kieferspitze in Bild 16-30
  gegen den Trefferpunkt neu vermessen (s3_reichweite.py), sonst beisst das Maul vor oder hinter dem Spieler zu. Das
  Oeffnen/Schliessen des Kiefers (Knochen 23) muss im Trefferfenster 16-30 zu sein, Zaehne duerfen den Spieler nicht
  durchdringen (Kiefer bei y -1460 = Brusthoehe).
- Hiebe: Treffer an Teil 13/18 Radius 1700 bzw. Versatz (0,600,0) Radius 1500. Laengere Krallen vergroessern die sichtbare
  Reichweite; Einstiegsbild des Doppelschlags (Clip 3 Bild 20, Clip 4 Bild 7) muss weiter ausserhalb der Probe liegen.
- Griff Clip 27: Abstand < 3800 (0xed8) am Bild 8, kein Teil; die Opferlage folgt der Opfer-Spur. Mit einem nach vorn
  laengeren Kopf greifen die Haende relativ weiter hinten - Bild 8 gegen die Spielerlage neu pruefen.
- Gebruell-Posen (Clip 9, 10, 11, 27): Maul auf; Toene s. M3.
- Kamera-Nahsicht (M4): die Schnauze ragt weiter vor; in Cut 13/15 schneidet die Nahebene (z < 64 verwirft Flaechen) dann
  frueher Loecher in den Kopf. Mit K Cut 13/15 erneut im echten Fenster pruefen.
- Treffer-Blut: Anker Teil 10 (Brust); fuer sichtbares Gore am Kopf waere ein zweiter Anker am Kopf (Teil 22) zu waehlen.
- Loecher (M2): Gelenkkoerper in K an Schulter und Huefte, dazu die neuen Kiefergelenke (22/23) in der Lochsuche mitnehmen.
