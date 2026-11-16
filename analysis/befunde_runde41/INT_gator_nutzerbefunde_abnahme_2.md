# INT Gator-Nutzerbefunde 2026-11-09 - UNABHAENGIGE ABNAHME 2 (Nachtrag K1 / K2 / H3)

Baum `.claude/worktrees/r41_integration`, Zweig `r41/integration`, HEAD 696a42f42. Geprueft: der Nachtrag nach Abnahme 1 - e5cd784dc (K1 Kanister auf
der Mittellinie des Westkanals), ae9e17c9e (K2 Schluessel am der Leiche naechsten erreichbaren Punkt), 3061fe397 (H3 Dash-Abbruch mit Clip-0-Blend 32),
05c8851e6 (Riegel-Nachzug Kanister-Kampf), 696a42f42 (Berichtsabschnitt). Vergleichsstand: b29263046 (Stand der Abnahme 1). Der Pruefer glaubt dem
Bericht des Fixers nichts und hat alles selbst gemessen. Kein Code geaendert, der einzige Commit ist dieser Bericht.

Laufordner (nicht im Baum): `<S>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gator_nb/abn2`
(`runs/<lauf>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, debug.log; Laeufer `lauf.sh`, `schl.sh`, `tr.sh`, `gdg.sh` (gdigrab nur auf
das Fenster der eigenen exe-Kopie), `serie_*.sh`; Auswerter `passage.py`, `fenster.py`, `besuch.py`, `erstdiff.py`, `proj.py`, `h3b_ausw.py`,
`zoomkey.py`, `bogen.py`, `sca_mitte.py`; Bilder unter `sicht/`; `ctest_gruppe.log`, `ctest_einzel_tod_1..3.log`, `einzel_tod_1..3.log`).
Eigene exe-Kopien `re15_pc_abn2.exe`, `re15_pc_abn2g.exe`, `re15_pc_abn2h.exe` sind nach der Abnahme geloescht, es laeuft kein eigener Prozess mehr.

Nutzer 2026-11-09 (woertlich), Punkt 2: "Ich musste diverse runden mit dem Aligator drehen, bis er letzendlich den Behälter aufnahm. Eigentlich sollte er
den Behälter aufnehmen, sobald die Position erreicht ist, wo der Behälter liegt. Wenn das so nicht so einfach ist, lege den Behälter zentraler in die
Mitte, damit er ihn auf jeden Fall aufnehmen muss, sobald der Spieler den Aligator dort hingelockt hat." Zu K2: "na er soll näher bei der Leiche
liegen, immerhin hat der Aligator ja den Schlüssel samt Zombie gefressen." Punkt 3: "Ich bin ausversehen nach dem Tot des Aligators die Leiter hoch
und wieder runter. Ich konnte den Schlüssel sehen, aber aufnehmen konnte ich ihn nicht mehr."

## 0. Urteil

**NICHT BESTANDEN.** Ein blockierender Befund:

- **B1 (blockierend) - `unit_r41_gatorszene_tod` ist rot, einzeln und wiederholbar, und der Riegel hat in der Sache recht.** Gruppenlauf:
  `99% tests passed, 1 tests failed out of 154`. Einzeln sind 6 von 6 Laeufen rot (3x die Test-exe direkt, 3x per ctest), jedes Mal `tod: 6 Fehler`.
  Bei Abnahme 0 und 1 war der Riegel gruen. Rot ist er seit ae9e17c9e (K2); der Fixer hat ihn nach K2 nicht gefahren (sein Gruppenlauf endet bei
  17/154, sein K2-Lauf enthielt nur `tod_gebiet`, `tod_gebiet_elza` und `alligator2_schluessel`). Zwei der sechs Fehler sagen "von der Leiche
  verdeckt" (Todesrolle Leon und Elza). Am echten Weg bestaetigt: in der Todesrolle (Rezept von `integration_r41_gatorszene_tod`, RE15_GB_TEST=3)
  ist vom Schluessel in der Einstellung genau EIN Bildpunkt zu sehen, bei Leon und bei Elza. Die Einstellung, die dem Spieler den Schluessel zeigen
  soll, zeigt dort nur die Leiche. Der Integrations-Riegel merkt es nicht mehr, weil ae9e17c9e seine Bildschranke von 10 x 9 auf 0 x 0 gesetzt hat.
  Einzelheiten in §5.

Der Rest des Nachtrags haelt der Pruefung stand: K1 und H3 sind erfuellt, der gelobte Kampf ist unveraendert, Befund 1 und 2 bleiben behoben, Befund 3
bleibt in der Aufnehmbarkeit behoben (zur Sichtbarkeit siehe B1 und K-2), der Riegel-Nachzug 05c8851e6 schwaecht nichts.

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| Bau, Belege | erfuellt | HEAD sauber, `=== LOCAL-BUILD-OK (build)`. K1: Kanalmitte aus der SCA selbst gemessen. H3: RE2 Sub 3 / Sub 1 selbst disassembliert. K1 und K2 sind Port-Wahlen mit Nutzer-Wortlaut, keine neue Zahl ohne Beleg |
| (1) K1 Kanister | erfuellt, Restfall K-1 (klein) | Landung Leon (-5084,-13316), Elza (-5056,-13306), 216 / 244 oestlich der Kanalmitte -5300. 20/20 Standardlaeufe und 28/28 Wandlaeufe mit Aufnahme. Der offene Fall tritt in 5 von 28 Wandlaeufen auf: eine verpasste Passage, dann Aufnahme |
| (2) K2 Schluessel | **nicht erfuellt (B1)**, dazu K-2 (klein) | Abstand zur Leiche 740-812 in den natuerlichen Laeufen (vorher 1021-1214 bzw. 3528-4910), aufnehmbar, nach Leiter genau einmal. Aber: Todesrolle 1 Bildpunkt sichtbar; andere Todeslagen 3 bis etwa 7 Bildpunkte |
| (2b) H3 Dash-Abbruch | erfuellt | Disassembly bestaetigt. Pose-Kante: alt ein Sprung in einem Bild (Bilddifferenz 1,68 gegen 0,43-0,92), neu kein Sprung. state.log und gator_trag.log gleich |
| (3) Gelobter Kampf, Befunde 1-3 | erfuellt | Mit beiden Messhaken (alte Wurfgier, alter Blend) 6 Laeufe ueber die volle Laenge bitgleich mit b29263046. Kletterbiss: 6 Laeufe, 0 Bilder in der Wand |
| (4) Riegel-Gruppe | **rot (B1)** | `99% tests passed, 1 tests failed out of 154`, `Total Test time (real) = 5436.62 sec`; rot: `1231 - unit_r41_gatorszene_tod (Failed)`, einzeln 6/6 rot |
| (5) Riegel-Nachzug 05c8851e6 | erfuellt | Nur Erwartungswerte 3072/1024 -> 3342/888. Die alte Wurfrichtung bleibt rot (0 Treffer in alten Logs), die neue ist gruen |

## 1. Bau, Stand, Belege

- `git status`: sauber bis auf das unversionierte `Testing/` (bestand vorher). `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash
  re15_port/tools/local_build.sh build`: `=== LOCAL-BUILD-OK (build)`. Die Test-exe `test_r41_gatorszene.exe` (2026-11-09 22:57:12) ist juenger als
  ihre Quellen (22:42:30), die Binaries sind der Stand HEAD.
- **K1** (`re15_kanister2080_r41.h`): `RE15_K2080_WURF_GIER_NORD 3342`, `_SUED 888`. Port-Wahl mit Nutzer-Wortlaut (Punkt 2). Die Kanalmitte habe ich
  selbst aus `ROOM2090.RDT` gelesen: SCA @0x6DC, Kasten [0] @0x6F4 x -10900..-8900, Kasten [4] @0x724 x -1700..1850 z -20000..-12450, also Kanal
  x -8900..-1700, Breite 7200, Mitte -5300. Das Aufnahmefenster ist unveraendert (8601 / 0x18f @0x80100ae4-e8).
- **K2** (`gatorszene_r41.c`): keine neue Zahl. Die Wahl nimmt unter den Punkten, die die bisherigen Bedingungen erfuellen, den der Leiche naechsten
  statt des groessten Bildes. Port-Wahl mit Nutzer-Wortlaut. Die Bildschwelle FG-E7 (10,4 Bildpunkte) entscheidet nichts mehr, sie wird nur gemessen.
- **H3** (`enemy_ai_boss_gator.c`): selbst disassembliert (`<S>/re2_sub3.txt`, `<S>/re2_sub1_kopf.txt`): Sub 3 0x80100d30-0x80100ee8, `yaw += 0x400`
  @0x80100ebc-c0 nur am normalen Ende; Sub 1 `addiu a2,zero,0x20` @0x80100c38 vor `jal 0x8001a330` @0x80100c3c. Der Bau (anim_frac 32,
  anim_blend_rate 4096/33, kein yaw += 1024 beim Abbruch) passt dazu.

## 2. (1) K1 - Kanister zentraler

Alle Laeufe am echten Weg (Szene 2080/2081 mit Wurf, Rueckkehr nach 2090/2091), Leon und Elza.

- **Landung:** `F284 Wurfrichtung 3342 (Nordbecken, Alligator beim Ausloesen (-2754,-22840)) -> Nennlandung (-5021,-13324)`. Kanister Leon
  (-5084,-1200,-13316), Elza (-5056,-1200,-13306): 216 bzw. 244 oestlich der selbst gemessenen Kanalmitte. Vorher lag er 1508 westlich der Mitte.
- **Standardserie (10 Rezepte x Leon/Elza): 20/20 Aufnahmen in der ersten Passage, 0 Wurzel-Querungen vor der Aufnahme.** Leon: pendel_mitte F131,
  lane_mitte F131, stehen_ost F127, stehen_sued F132, pendel_innen F553, pendel_west F809, pendel_wand_o F181, pendel_wand_w F600, lane_wand_w F415,
  lane_wand_o F251. Elza: pendel_mitte F131, pendel_innen F608, pendel_west F484, lane_mitte F131, stehen_ost F127, stehen_sued F132, pendel_wand_o
  F276, pendel_wand_w F412, lane_wand_w F416, lane_wand_o F214. Zum Vergleich b29263046: stehen_ost F253, lane_mitte F196, pendel_innen F595,
  pendel_west F812.
- **Aufnahmefenster nachgemessen** (`gator_re2_r41.c:621-633`): nur in der Verfolgung, mit dem Modellmass 2/3 skaliert: wirksam laengs 0 bis etwa
  6000, |quer| < 1733. In allen Laeufen faellt die Aufnahme auf das erste Bild in diesem Fenster. Kein Bild im Fenster ohne Aufnahme.
- **Offener Fall (dicht an der Westwand / dicht an der Plattform vorbei), 28 gezielte Laeufe:** Spieler an der Westwand oder an der Plattform,
  verschiedene z, stehend und laufend. **28/28 mit Aufnahme. In 5 Laeufen tritt der offene Fall auf:** die Wurzel kreuzt die z-Linie des Kanisters im
  Dash an der Westseite bei x etwa -7415 mit quer -2292 bis -2349, ohne Aufnahme. Die Annaeherung davor (F214, quer -1752 bis -1905) lag 19 bis 172
  ausserhalb des Fensters.

| Lauf | Querung ohne Aufnahme | Aufnahme |
|---|---|---|
| wand_0_w_z150 (Leon) | F293 bei (-7414,-13297), quer -2292 | F1651 |
| wand_1_w_z150 (Elza) | F293 bei (-7414,-13297), quer -2321 | F1651 |
| wand_0_w_z160 (Leon) | F290 bei (-7418,-13298), quer -2320 | F526 |
| wand_1_w_z160 (Elza) | F290 bei (-7418,-13298), quer -2349 | F526 |
| wand_0_w80_z150 (Leon) | F294 bei (-7423,-13276), quer -2297 | F526 |

  Also je eine verpasste Passage, dann Aufnahme bei der naechsten Annaeherung: 232 bis 236 Bilder spaeter in drei Laeufen, 1358 Bilder spaeter in den
  zwei z150-Laeufen. Die uebrigen 23 Laeufe: Aufnahme F124-F178 (19 Laeufe), F477 (o_z190, Leon und Elza), F660 (o_lauf, Leon und Elza), jeweils ohne
  Querung davor. An der Plattformseite trat keine verpasste Passage auf.
  Geometrie dazu: das Fenster ist 3466 breit, der Kanal 7200. Ein Alligator, der gerade durch den Kanal schwimmt, nimmt nur auf, wenn seine Wurzel
  hoechstens 1733 neben dem Kanister liegt. Es bleiben ein Streifen von etwa 2083 an der Westwand und etwa 1651 an der Plattform. -> **K-1 (klein)**:
  "auf jeden Fall" ist geometrisch nicht garantiert; "diverse Runden" liess sich nicht nachstellen. Der Fixer hat die Geometrie offengelegt.
  Schliessen liesse es sich nur mit einem breiteren Fenster, das waere ein Nutzer-Entscheid gegen den RE2-Wert.
  (Meine ersten Laeufe lang_wand_* waren als Rezept untauglich und zaehlen nicht.)
- **Suedwurf (Gier 888):** die natuerliche Wahl fiel in allen 77 Laeufen mit Wurf auf den Nordwurf (Alligator beim Ausloesen (-2754,-22840) oder
  (-2980,-22854)). Den Suedwurf habe ich darum ueber den Messhaken RE15_K2080_WURF_GIER=888 gefahren, 6 Laeufe: Landung Leon (-5255,-1200,-21505),
  Elza (-5227,-1200,-21495), 45 bzw. 73 oestlich der Mitte, im Wasser. 6/6 Aufnahmen: F43 (5 Laeufe), F119 (Spieler an der Plattform; davor ein
  Biss-Anlauf mit quer 2530 an der Linie vorbei). -> **Hinweis H-2**: der Suedwurf mit natuerlicher Auswahl (Alligator im Norden beim Verlassen) und
  sein Bild sind am echten Weg nicht geprueft.
- **Fall, Wurf, Szene Spur P** (gdigrab `sicht/wurf_leon`, `sicht/wurf_elza`): Szene stimmig (Marvin bzw. John am Gelaender, Text, Totale, Fall vom
  Gelaender; Werfer frontal: `Versatz -113, frontal 4035/4096 (RE2 ROOM7020 Ada: Versatz -111, frontal 4034)`). Mit der neuen Richtung faellt der
  Kanister in der tiefen Einstellung hinter Kopf und Schulter der Figur vorbei (Leon Bilder 1317-1321, Elza 2014-2018); mit der alten Richtung fiel
  er rechts neben der Figur. Das Aussehen des Kanisters ist gleich. -> **Hinweis H-1**.
  Bilder: `<S>/sicht/wurf_leon_fall_gross.png`, `<S>/sicht/wurf_altgier_fall.png`, `<S>/sicht/wurf_elza_szene.png`, `<S>/sicht/wurf_leon_aufn.png`.

## 3. (2) K2 - Schluessel bei der Leiche

- **Abstand (natuerliche Laeufe, Kanister-Explosion):** der Schluessel liegt 740 bis 812 vom naechsten Knochen der Leiche. l1/l2 Leon und Elza 762
  bei (-2500,-1779,-6100), Tiefe 4989; Varianten k1 781 (Tiefe 4651), k2 783 (Tiefe 4328); west_leon / west_elza 751 (Tiefe 4883); tod_kanal_n 812
  (Kamera 5, Tiefe 7432); tod_wand_w 740 (Kamera 5, Tiefe 6640, Schluessel (-4300,-1775,-6200)); tod_plattf (Elza) 789 (Kamera 0, Tiefe 7600,
  Schluessel (-2900,-1742,-20500)). Das ist naeher als vor bf345ea49 (1021-1214) und als b29263046 (3528-4910). Der Wortlaut "näher bei der Leiche"
  ist im Abstand erfuellt.
- **Einstellung:** jede Lage meldet "Kamera N (an der Leiche ...)", die Einstellung laeuft mit dieser Kamera (90 Bilder), danach laeuft am Schluessel
  dieselbe Zonenkamera.
- **Aufnehmbar, nach Leiter genau einmal:** zu Fuss aufgenommen ITEMBAND F151 (l2_leon), F151 (l2_elza), F154 (l2_leon_k1), F156 (l2e_elza), F168
  (l2e_elza_k2), Item 0x34, Inventar 34:1. Die Schluessel-Zeile steht einmal beim Tod und einmal beim Wiedereintritt nach Leiter hoch/runter, nach der
  Aufnahme keine mehr. Befund 3 ("sehen, aber nicht aufnehmen") ist in der Aufnehmbarkeit behoben.
- **Bildgroesse und Erkennbarkeit (gdigrab, 320 x 240):**

| Todeslage | Kamera, Tiefe | im Bild | Bilder |
|---|---|---|---|
| natuerlich (l2_leon) | 5, 4989 | Ring etwa 9 x 11 Bildpunkte bei (254,123): erkennbar, klein und blass | `<S>/sicht/einstellung_leon.png`, `<S>/sicht/am_schluessel_leon.png`, `<S>/sicht/zoom_schluessel_leon.png` |
| tod_kanal_n | 5, 7432 | etwa 7 helle Bildpunkte | `<S>/sicht/tod_kanal_n_einst.png`, `<S>/sicht/tod_kanal_n_zoom.png` |
| tod_wand_w | 5, 6640 | 2 bis 3 pulsierende Bildpunkte | `<S>/sicht/tod_wand_w_einst.png`, `<S>/sicht/tod_wand_w_zoom.png` |
| tod_plattf (Elza) | 0, 7600 | 4 Bildpunkte | `<S>/sicht/tod_plattf_einst.png`, `<S>/sicht/tod_plattf_zoom.png` |
| **Todesrolle (GB_TEST=3), Leon und Elza** | 0, 9625 | **1 Bildpunkt bei (75,94)**, der Rest liegt hinter der Leiche | `<S>/sicht/tr_leon_einst.png`, `<S>/sicht/tr_leon_zoom.png`, `<S>/sicht/tr_leon_bogen.png` |

  Rasterlauf (`<S>/scan_neu.txt`, `<S>/scan_alt.txt`): neue Regel Median 4,5 Bildpunkte (575 von 1688 Lagen unter 4,0), alte Regel Median 10,2. Da die
  Kamera der Einstellung die Zonenkamera am Ort ist, bleibt der Schluessel so klein, wenn die Figur dort steht. -> **K-2 (klein)**: ausser in der
  natuerlichen Lage erkennt ein Spieler den Schluessel im Bild nicht als Schluessel (2 bis 7 Bildpunkte). Das ist die Kehrseite des Wortlauts "näher
  bei der Leiche" und vom Fixer offengelegt. Die Todesrolle faellt nicht darunter, sie ist B1.
  (Rezept-Artefakt, kein Code-Befund: im Lauf tod_plattf schneidet mein eigenes RE15_FIRE_AOT die Einstellung vorzeitig ab.)

## 4. (2b) H3, (3) gelobter Kampf, Befunde 1 bis 3, (5) Riegel-Nachzug

- **H3 Pose-Kante** (Laeufe h3b_neu / h3b_alt, alt = RE15_G2_DASH_BLEND_ALT=1, Aufnahme im Dash): state.log und gator_trag.log sind gleich, es
  unterscheidet sich nur `KANISTER angehaengt lokal=(3631,263,3196)` (neu) gegen `(3659,49,3163)` (alt), also nur die Darstellung. Bilddifferenz
  aufeinanderfolgender Bilder im Ausschnitt des Alligators: alt hat bei Bild +3 eine Spitze 1,68 (Nachbarn 0,43-0,92), neu keine Spitze (1,05-1,37).
  Bilder: `<S>/sicht/h3b_zoom_klein.png`, `<S>/sicht/h3b_vergleich.png`. Kein Befund.
- **Gelobter Kampf:** mit beiden Messhaken (RE15_K2080_WURF_GIER=3072 und RE15_G2_DASH_BLEND_ALT=1) sind state.log und gator_trag.log ueber die volle
  Lauflaenge BITGLEICH mit b29263046: id_stehen_ost (3874 / 2958 Zeilen), id_pendel_west (4474 / 3558), id_pendel_innen, id_lane_mitte,
  id_elza_pendel_innen, id_elza_pendel_mitte. idb_stehen_ost (alte Gier, NEUER Blend) ist ebenfalls bitgleich. Gang, Tempo, Angriffe, Biss und
  Trefferreaktion sind damit unveraendert; der Nachtrag wirkt nur ueber die Lage des Kanisters und die Darstellung des Dash-Abbruchs.
- **Befund 1 (Kletterbiss an der Kante):** 6 Laeufe (Leon und Elza): in oder hinter einer festen Wand 0 Bilder, kleinster Abstand zur Wand 450-455.
  Vier Laeufe sind bitgleich mit den Laeufen w_neu der Abnahme 1, die beiden 6500-Laeufe weichen erst ab der neuen Kanister-Aufnahme (F1113) ab.
- **Befund 2:** siehe §2. **Befund 3:** siehe §3 (aufnehmbar, genau einmal; Sichtbarkeit B1 / K-2).
- **(5) Riegel-Nachzug 05c8851e6:** geaendert sind nur die Erwartungswerte der Wurfrichtung (3072/1024 -> 3342/888), keine Schranke ist gelockert. Die
  alte Wurfrichtung bleibt rot: die neuen Muster finden in Logs mit "Wurfrichtung 3072" 0 Treffer, in neuen Logs 1/1. Das Log des Fixers zeigt den
  Riegel mit der alten Erwartung rot (`integration_r41_kanister2080_kampf ***Failed 268.28 sec`). In meinem Gruppenlauf: `Passed 1350.82 sec`.

## 5. (4) Riegel-Gruppe und Befund B1

`cd re15_port/build && ctest -R "gator|kanister|alligator|2090|2080|re2gator" -j 4 --timeout 2400`, Start 14:18:30, Ende 15:49:07, Log
`<S>/ctest_gruppe.log`. Summenzeile woertlich:

    99% tests passed, 1 tests failed out of 154
    Total Test time (real) = 5436.62 sec
    The following tests FAILED:
        1231 - unit_r41_gatorszene_tod (Failed)

Kein Fenster-Riegel war rot. Der rote Riegel ist ein Unit-Riegel ohne Fenster (0,63 s), Last scheidet als Ursache aus. Einzeln wiederholt:
`tests/unit/test_r41_gatorszene.exe tod` 3x: exit 1, exit 1, exit 1. `ctest -R unit_r41_gatorszene_tod` (genau dieser eine Riegel) 3x:
`***Failed 0.49 sec`, `***Failed 0.48 sec`, `***Failed 0.42 sec`. In der Abnahme 1: `84/152 Test #1229: unit_r41_gatorszene_tod ... Passed`.

Ausgabe (jedes Mal gleich; die FEHLER-Zeilen woertlich, die Szenario-Zeilen mit "--" von mir zur Zuordnung davorgesetzt):

    -- todesrolle (ROOM2090, Leiche (-5416,-22578) Clip 7, laufende Kamera Cut 0)
    FEHLER: Quadrat 5.1 Bildpunkte >= Nachbesserung 1 in der laufenden Kamera (9.7)
    FEHLER: von der Leiche nicht verdeckt (22 Knochen-Kugeln 800), naechster Knochen 757 entfernt (<= doppelter Kopf-Wurzel-Abstand der Leiche 3029)
    -- r1 (ROOM2090, Leiche (3413,-25001) Clip 9, laufende Kamera Cut 1)
    FEHLER: Quadrat 4.6 Bildpunkte >= Nachbesserung 1 in der laufenden Kamera (7.5)
    -- r1_sprung (ROOM2090, Leiche (3413,-25001) Clip 9, laufende Kamera Cut 0)
    FEHLER: Quadrat 4.6 Bildpunkte >= Nachbesserung 1 in der laufenden Kamera (7.5)
    -- todesrolle_elza (ROOM2091, Leiche (-5416,-22578) Clip 7, laufende Kamera Cut 0)
    FEHLER: Quadrat 5.1 Bildpunkte >= Nachbesserung 1 in der laufenden Kamera (9.7)
    FEHLER: von der Leiche nicht verdeckt (22 Knochen-Kugeln 800), naechster Knochen 757 entfernt (<= doppelter Kopf-Wurzel-Abstand der Leiche 3029)
    tod: 6 Fehler

Zwei Arten von Fehlern:

1. **"Quadrat ... >= Nachbesserung 1" (4x):** der Riegel verlangt noch, dass der Schluessel mindestens so gross ist wie nach der alten Regel. Mit K2
   ist er mit Absicht kleiner (5,1 gegen 9,7; 4,6 gegen 7,5). Der Riegel wurde in ae9e17c9e nicht nachgezogen. Das ist dieselbe Sache wie K-2.
2. **"von der Leiche nicht verdeckt" (2x, Todesrolle Leon und Elza):** das ist ein Sachfehler, kein veralteter Riegel. Ursache, aus dem Code gelesen
   (`gatorszene_r41.c`, `kugeln_verdeckt` Zeile 2001, Aufruf Zeile 2198): die Lagewahl prueft die Verdeckung am Kandidaten auf der Hoehe y -1620.
   Danach wird der Schluessel gehoben und geneigt (Endlage y -1742 bis -1779). Der Riegel prueft die Endlage (`test_r41_gatorszene.c` Zeile 609-635,
   dasselbe Kugelmodell 800 @0x80118b98). Die neue Regel waehlt den der Leiche NAECHSTEN nicht verdeckten Punkt, also einen Punkt am Rand des
   verdeckten Bereichs; nach der Hebung liegt er im Bild hinter der Leiche. Die alte Regel (groesstes Bild) waehlte keine Randpunkte, darum war der
   Riegel bis b29263046 gruen.

**Am echten Weg bestaetigt** (eigene exe-Kopie, Rezept von `integration_r41_gatorszene_tod`: RE15_GB_TEST=3, Leon 2090 und Elza 2091, Laeufe tr_leon /
tr_elza, gdigrab): `Schluessel-Lage: Kamera 0 (an der Leiche, laufende 0), Lage (-7500,-1742,-25900) Tiefe 9625, 773 von der Leiche`. Die Lage faellt
im Bild auf (78,96), auf die Leiche direkt neben der Figur an der Westwand. Im Feld von 18 x 18 Bildpunkten um diese Stelle aendert sich ueber 130
(Leon) bzw. 200 (Elza) Fensterbilder genau EIN Bildpunkt im Takt des Blinkens, bei (75,94). Ein sichtbarer Schluessel pulsiert als Gruppe (natuerliche
Lage: Ring 9 x 11). Im Bild ist an der Stelle nur die Leiche zu sehen (`<S>/sicht/tr_leon_einst.png`, `<S>/sicht/tr_leon_zoom.png`).

Der Integrations-Riegel misst dasselbe und laesst es durch. Aus `re15_port/build/Testing/Temporary/LastTest.log` meines Gruppenlaufs:

    r41_gatorszene: Lauf tod - Schluessel-Quadrat 38/10 Bildpunkte (unter 10,4, nur gemessen - Nachtrag K2)
    bilddiff: 43 Bilder, 43 mit Schluessel-Unterschied, Kasten (75,94)-(75,94) = 1 x 1 (mindestens 0 x 0), hellster min 49 max 71 (x1.44), 2 Bilder am Hoechstwert -> OK

Ebenso `Lauf tod_elza`. `Lauf tod_r1` und `tod_r1_elza`: `Kasten (114,98)-(116,99) = 3 x 2`. `alligator2 k3` und `k3_elza`:
`Kasten (220,107)-(226,111) = 7 x 5 (mindestens 0 x 0)`. Vor ae9e17c9e verlangte der Aufruf mindestens 10 x 9 (`bilddiff ... 2 10 9`); ae9e17c9e hat
ihn auf `2 0 0` gesetzt ("Kasten nur gemessen") und die 10,4-Schwelle von Fehler auf Statusmeldung. Die Schranke, die genau diesen Fall gefangen
haette, ist damit weg.

Folge fuer den Spieler: stirbt der Alligator wie in der Todesrolle an der Westwand des Suedbeckens, zeigt die Einstellung keinen Schluessel. Er liegt
bei der Leiche, ist aber nicht zu sehen. Ob er dort zu Fuss aufgenommen wird, habe ich in diesem Rezept nicht geprueft.

Was zum Bestehen noetig ist (Richtung, kein Code von mir):
- Die Verdeckung an der ENDLAGE des Schluessels pruefen (nach Hebung und Neigung), nicht am Kandidaten, damit "naechster nicht verdeckter Punkt" auch
  im Bild nicht verdeckt ist. Dafuer ist keine neue Zahl noetig.
- `unit_r41_gatorszene_tod` gruen bekommen: die Verdeckungs-Pruefung ohne Lockerung; die Groessen-Pruefung gegen "Nachbesserung 1" braucht einen
  begruendeten Nachzug (Nutzer-Wortlaut), kein stilles Entfernen.
- In `test_r41_gatorszene.cmake` und `test_r41_alligator2.cmake` wieder eine Bildschranke groesser als 0 x 0 setzen, damit ein Schluessel mit einem
  Bildpunkt nicht als OK gilt. Welche Mindestgroesse nach K2 gilt, ist ein Nutzer-Entscheid (siehe K-2).
- Danach die Gruppe mit 154 Riegeln vollstaendig fahren.

## 6. Befunde

| Nr | Schwere | Befund |
|---|---|---|
| B1 | blockierend | `unit_r41_gatorszene_tod` rot (Gruppe 153/154, einzeln 6/6 rot, 6 Fehler). In der Todesrolle (Leon und Elza) liegt der Schluessel nach K2 hinter der Leiche: am echten Weg 1 sichtbarer Bildpunkt in der Einstellung. Der Integrations-Riegel laesst es durch, weil ae9e17c9e seine Bildschranke von 10 x 9 auf 0 x 0 gesetzt hat |
| K-1 | klein | K1 offener Fall: in 5 von 28 Wandlaeufen schwimmt der Alligator im Dash einmal an der Westseite am Kanister vorbei (quer -2292 bis -2349, Fenster 1733) und nimmt ihn erst bei der naechsten Annaeherung auf (232 bis 236 bzw. 1358 Bilder spaeter). "Auf jeden Fall" ist nicht garantiert; "diverse Runden" nicht nachstellbar |
| K-2 | klein | K2 Bildgroesse: ausserhalb der natuerlichen Lage zeigt die Einstellung den Schluessel mit 2 bis 7 Bildpunkten, fuer einen Spieler nicht als Schluessel erkennbar. Kehrseite des Nutzer-Wortlauts, vom Fixer offengelegt; Nutzer-Spieltest oder Entscheid noetig |
| H-1 | hinweis | Mit der neuen Wurfrichtung faellt der Kanister in der tiefen Einstellung hinter Kopf und Schulter von Leon / Elza vorbei, vorher rechts daneben |
| H-2 | hinweis | Der Suedwurf (Gier 888) ist am echten Weg nur ueber den Messhaken geprueft (6/6 Aufnahmen, Landung 45 / 73 neben der Mitte); die natuerliche Auswahl fiel in allen 77 Laeufen auf den Nordwurf |
