# INT Gator-Nutzerbefunde 2026-11-09 - UNABHAENGIGE ABNAHME 3 (Nachbesserung zu Abnahme 2: B1)

Baum `.claude/worktrees/r41_integration`, Zweig `r41/integration`, HEAD 5116d63ed. Geprueft: die Nachbesserung nach Abnahme 2 - 61aeffeb8
(Schluessel-Verdeckung an der Endlage pruefen, `key_endlage_frei()`), 190c4b1da (Riegel `tod` in drei Staenden, Becken-Scan mit Stand ae9e17c9e),
04aa0e7db (Bildschranke wieder groesser als 0 x 0), cceb11646 (Endlagen-Pruefung auf die Mitte zurueckgenommen), 3a532ee7e (Messwerte im Kommentar),
5116d63ed (Berichtsabschnitt) - und damit der ganze Nachtrag K1 / K2 / H3 noch einmal am echten Weg. Vergleichsstaende: b29263046 (Abnahme 1),
ae9e17c9e (K2 vor B1), der Stand vor bf345ea49. Der Pruefer glaubt dem Bericht des Fixers nichts und hat alles selbst gemessen. Kein Code geaendert,
der einzige Commit ist dieser Bericht.

Laufordner (nicht im Baum): `<S>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gator_nb/abn3`
(`runs/<lauf>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, itemband.log, debug.log; Laeufer `lauf.sh`, `schl.sh`, `tr.sh`, `trk.sh`,
`gdg.sh` (gdigrab nur auf das Fenster der eigenen exe-Kopie), `serie_a.sh`, `serie_b.sh`, `serie_tod.sh`; Auswerter `ausw.sh`, `erstdiff.py`,
`fenster.py`, `h3b_ausw.py`, `bogen.py`, `blink.py`, `zoom.py`, `streifen.py`, `riegel5.cmake`; Ergebnisse `ausw_k1.txt`, `ausw_wand.txt`; Bilder
unter `sicht/`; `bau.log`, `ctest_gruppe.log`, `ctest_gruppe.start`, `ctest_gruppe.done`, `einzel_tod_1..3.log`, `einzel_a2_schl.log`,
`einzel_a2_schl_b.log`). Die Vergleichslaeufe der Abnahmen 1 und 2 liegen daneben unter `.../gator_nb/abn1` und `.../gator_nb/abn2`. Eigene
exe-Kopien `re15_pc_abn3.exe`, `re15_pc_abn3g.exe`, `re15_pc_abn3h.exe`, `re15_pc_abn3k.exe` sind nach der Abnahme geloescht, es laeuft kein eigener
Prozess mehr.

Nutzer 2026-11-09 (woertlich): "Den Aligator fight habe ich jetzt auch einmal getestet. Ist cool! Es gibt nur 3 winzige kleinigkeiten die mir
aufgefallen sind: 1. Wenn ich direkt an der Kante hochklettere auf die Platform und dann vom Aligator erwischt werden, ist es möglich das er mich in
die Wand schiebt. 2. Ich musste diverse runden mit dem Aligator drehen, bis er letzendlich den Behälter aufnahm. Eigentlich sollte er den Behälter
aufnehmen, sobald die Position erreicht ist, wo der Behälter liegt. Wenn das so nicht so einfach ist, lege den Behälter zentraler in die Mitte, damit
er ihn auf jeden Fall aufnehmen muss, sobald der Spieler den Aligator dort hingelockt hat. 3. Ich bin ausversehen nach dem Tot des Aligators die
Leiter hoch und wieder runter. Ich konnte den Schlüssel sehen, aber aufnehmen konnte ich ihn nicht mehr." Zu K2: "na er soll näher bei der Leiche
liegen, immerhin hat der Aligator ja den Schlüssel samt Zombie gefressen."

## 0. Urteil

**BESTANDEN.** Kein blockierender Befund.

- B1 aus Abnahme 2 ist behoben. `unit_r41_gatorszene_tod` ist gruen: einzeln 3 von 3 Laeufen der Test-exe mit `tod: 0 Fehler`, im Gruppenlauf
  gruen. Am echten Weg ist der Schluessel in der Todesrolle nicht mehr von der Leiche verdeckt (Abnahme 2: 1 Bildpunkt; jetzt 8 Bildpunkte, Kasten
  8 x 5, in jedem Bild der Einstellung), und er ist an der neuen Lage zu Fuss erreichbar und aufnehmbar, bei Leon und bei Elza.
- Der Gruppenlauf ist vollstaendig gefahren und gruen: `100% tests passed, 0 tests failed out of 154`.
- Der gelobte Kampf ist unveraendert: mit den beiden Messhaken auf den alten Werten sind 9 von 9 Laeufen ueber die volle Laenge bitgleich mit dem
  Stand b29263046.
- Offen bleiben zwei kleine Befunde und drei Hinweise, keiner blockierend: K-1 (Alligator dicht an der Westwand schwimmt einmal am Kanister
  vorbei), K-2 (ausserhalb der natuerlichen Todeslage ist der Schluessel nur wenige Bildpunkte gross), H-1, H-2 (alle vier aus Abnahme 2 bekannt
  und vom Fixer offen gelassen) und H-3 (Schlusszeile dreier aelterer Commits).

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| Bau, Stand, Belege | erfuellt | HEAD 5116d63ed sauber (nur `Testing/` unversioniert), `=== LOCAL-BUILD-OK (build)`. Seit 696a42f42 nur `gatorszene_r41.c`, sein Header, zwei Tests mit ihren cmake-Dateien und zwei Berichte geaendert; K1- und H3-Code unveraendert. Keine neue Verhaltenskonstante ohne Beleg |
| (1) K1 Kanister | erfuellt, Restfall K-1 (klein) | Landung Leon (-5084,-13316), Elza (-5056,-13306), 216 / 244 oestlich der selbst gemessenen Kanalmitte -5300 (Stand b29263046: 1570 westlich). 20/20 Standardlaeufe und 18/18 Wand- und Plattformlaeufe mit Aufnahme. Der offene Fall tritt in 9 von 18 gezielten Laeufen auf, nur an der Westwand: eine verpasste Passage, dann Aufnahme |
| (2) K2 Schluessel | erfuellt, Restfall K-2 (klein) | Natuerlicher Kampf: 762 von der Leiche (Stand b29263046 3528-4910, vor bf345ea49 1214), Kasten 17 x 15, als Ring erkennbar, nach Leiter hoch und runter genau einmal aufgenommen, Leon und Elza. Todesrolle: 789, 8 Bildpunkte, aufnehmbar. Drei andere Todeslagen 740-812 von der Leiche; an der einen ausgezaehlten 4 bis 5 Bildpunkte |
| (2b) H3 | erfuellt | RE2 Sub 3 @0x80100ea8-ee4 und Sub 1 @0x80100c34-3c selbst disassembliert. Posenkante im Bild: alt 3.28 (Sprung), neu 2.09 (kein Sprung) |
| (3) gelobter Kampf, Befunde 1-3 | erfuellt | 9/9 Laeufe mit Haken bitgleich mit b29263046. Kletterbiss 8/8 Laeufe mit 0 Bildern in fester Wand |
| (4) Riegel-Gruppe | erfuellt | `100% tests passed, 0 tests failed out of 154`, `Total Test time (real) = 5019.56 sec`, kein roter Riegel, nichts nachzufahren |
| (5) Riegel-Nachzug 05c8851e6 | erfuellt | Nur drei Erwartungszeilen geaendert; das Protokoll mit der alten Wurfrichtung 3072 faellt am Riegel weiter durch |

## 1. Bau, Stand, Belege

- `git log --oneline -8` und `git status`: HEAD 5116d63ed ist der letzte Commit, der Baum ist sauber bis auf das unversionierte `Testing/` (ctest-
  Ablage, nicht Teil dieses Commits). Bau mit `RE15_BUILD_DIR=.../re15_port/build bash re15_port/tools/local_build.sh build`: `=== LOCAL-BUILD-OK
  (build)`, keine Warnung (`<S>/bau.log`).
- `git diff --stat 696a42f42..HEAD`: `gatorszene_r41.c` (+71), `re15_gatorszene_r41.h` (+4), `test_r41_gatorszene.c`, `test_r41_alligator2.c`, die
  beiden zugehoerigen cmake-Dateien und zwei Berichte. Der Code von K1 (Wurfrichtung) und H3 (Dash-Abbruch) ist seit Abnahme 2 unveraendert.
- Diffs selbst gelesen. Die B1-Behebung `key_endlage_frei()` bringt keine neue Zahl: sie nutzt 800 (@0x80118b98), `GSZ_SPIELER_RADIUS` und
  `GSZ_KEY_HALB` 135 wieder. K1 (Wurfrichtung 3342 / 888 statt 3072 / 1024) und K2 (der Leiche naechster erreichbarer Punkt) sind Port-Wahlen mit
  dem Nutzer-Wortlaut oben als Beleg. Das Aufnahmefenster des Alligators ist unveraendert (8601 / 0x18f @0x80100ae4-e8).
- H3-Beleg selbst nachdisassembliert (EM23_OVL_0000.BIN, Basis 0x80100000). Sub 3 @0x80100d30: `@0x80100ea8 beq s0,zero,0x80100ee8`,
  `@0x80100eb0 lhu v0,0x76(s1)`, `@0x80100ebc addiu v0,v0,0x400`, `@0x80100ec0 sh v0,0x76(s1)`, `@0x80100ecc addiu v0,zero,0x101`,
  `@0x80100ee4 sw v0,0x4(s1)` - yaw += 0x400 steht nur am regulaeren Ende von Clip 3. Sub 1 @0x80100c20: `@0x80100c34 addu a1,zero,zero` (Clip 0),
  `@0x80100c38 addiu a2,zero,0x20` (Blend 32), `@0x80100c3c jal 0x8001a330`. Der Beleg des Fixers stimmt.
- Formal (H-3): drei aeltere Commits der Kette (e5cd784dc, ae9e17c9e, 3061fe397) enden mit "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
  statt der vorgegebenen Zeile. Die sechs Commits der Nachbesserung und 05c8851e6, 696a42f42 sind richtig.

## 2. (1) K1 - Kanister zentraler

**Lage gegen die Kanalmitte.** Kanalmitte selbst aus der SCA von ROOM2090.RDT gemessen: Quader 0 @0x6F4 x -10900..-8900 (Westwand), Quader 4 @0x724
x -1700..1850 (Plattform) -> Kanal x -8900..-1700, Breite 7200, Mitte -5300. Landung am echten Weg: Leon (-5084,-1200,-13316), 216 von der Mitte
("Wurfrichtung 3342 (Nordbecken, Alligator beim Ausloesen (-2754,-22840)) -> Nennlandung (-5021,-13324)", Landung nach 38 Bildern); Elza
(-5056,-1200,-13306), 244 von der Mitte. Stand b29263046: (-6870,-1200,-12651), 1570 westlich der Mitte.

**Standardserie** (10 Rezepte je Leon und Elza): 20 von 20 Laeufen mit Aufnahme, in keinem quert die Wurzel des Alligators vor der Aufnahme die
Kanisterlinie ausserhalb des Fensters. Alle 20 sind in state.log, gator_trag.log und gator_boss.log bitgleich mit den gleichnamigen Laeufen der
Abnahme 2 (der Code ist unveraendert, die Laeufe sind bildgenau wiederholbar).

| Rezept | Leon Aufnahme | Elza Aufnahme |
|---|---|---|
| lane_mitte | F131 | F131 |
| lane_wand_o | F251 | F214 |
| lane_wand_w | F415 | F416 |
| pendel_innen | F553 | F608 |
| pendel_mitte | F131 | F131 |
| pendel_wand_o | F181 | F276 |
| pendel_wand_w | F600 | F412 |
| pendel_west | F809 | F484 |
| stehen_ost | F127 | F127 |
| stehen_sued | F132 | F132 |

Darin stecken die seitliche Passage (lane_wand_*, pendel_wand_*) und die Aufnahme nach Biss und Dash (pendel_*).

**Der offene Fall, gezielt versucht** (18 eigene Laeufe: Spieler steht an der Westwand bei x -8432 / -8000 / -7500 oder an der Plattform bei
x -2150 / -2500 / -3000, in verschiedenen z-Hoehen, Leon und Elza): 18 von 18 mit Aufnahme. **Der Fall tritt auf, in 9 von 18 Laeufen, alle an der
Westwand, 0 von 7 an der Plattformseite.** Die Wurzel quert die z-Linie des Kanisters bei x etwa -7387..-7425 mit Querabstand -2292..-2354 (das
Fenster reicht bis 1733) ohne Aufnahme, die Aufnahme folgt beim naechsten Anlauf:

| Lauf | verpasste Querung | Aufnahme |
|---|---|---|
| wand_0_w_z150 | F293, quer -2292 | F1651 |
| wand_1_w_z150 | F293, quer -2321 | F1651 |
| wand_0_w_z155 | F295, quer -2319 | F1077 |
| wand_0_w_z160 | F290, quer -2320 | F526 |
| wand_1_w_z160 | F290, quer -2349 | F526 |
| wand_0_w_z170 | F295, quer -2328 | F531 |
| wand_1_w_z170 | F296, quer -2354 | F531 |
| wand_0_w80_z150 | F294, quer -2297 | F526 |
| wand_1_w80_z160 | F292, quer -2331 | F529 |

Ohne verpasste Passage: wand_0_w75_z155 F199, wand_0_o25_z155 F225, wand_0_o30_z165 F168, wand_0_o_z150 F436, wand_0_o_z170 F168, wand_0_o_z180
F472, wand_1_o_z150 F436, wand_1_o_z170 F168, sowie wand_0_w_z180 F2348 (keine Querung: der Alligator folgt dem Spieler, den die Bisse nach Sueden
ziehen, und kommt erst spaet zum Kanister). Die fuenf aus Abnahme 2 wiederholten Laeufe sind bitgleich.

Wertung **K-1 (klein)**: Es ist Geometrie. Das Fenster ist 3466 breit, der Kanal 7200; ein Alligator, der dem dicht an der Westwand stehenden
Spieler nachschwimmt, kommt 560 bis 620 am Fenster vorbei. "Diverse Runden" wie im Nutzerbefund liessen sich nicht mehr erzeugen, es bleibt bei einer
verpassten Passage; im unguenstigsten Lauf dauert es danach allerdings bis F1651, bis er den Kanister nimmt. Schliessen liesse sich der Fall nur mit
einem breiteren Fenster, also gegen den RE2-Wert @0x80100ae4-e8 - das ist eine Nutzerentscheidung, kein Fehler der Nachbesserung.

**Suedwurf** (Haken RE15_K2080_WURF_GIER=888, 3 Laeufe, bitgleich mit Abnahme 2): Landung Leon (-5255,-1200,-21505), Elza (-5227,-1200,-21495),
Aufnahme F43 in 3 von 3. Auf dem natuerlichen Weg fiel die Wahl in allen Laeufen auf Nord, weil der Alligator beim Verlassen des Raums im Sueden
steht - der Suedwurf ist nur ueber den Haken belegt (**H-2, Hinweis**).

**Fall, Wurf, Spur P** (Aufnahmen l2_leon, l2_elza): Marvin "Hey, Leon, try with this!" bzw. John "Hey, Elza, try with this!", der Kanister faellt
vor dem Tor ins Wasser, der Ablauf ist stimmig. Protokoll: "Werfer Cut 7: ... Versatz -113, frontal 4035/4096 (RE2 ROOM7020 Ada: Versatz -111,
frontal 4034)". Unveraendert aus Abnahme 2: in der tiefen Einstellung zieht der Kanister kurz hinter Kopf und Schulter des Werfers durch (**H-1,
Hinweis**, Code unveraendert, in dieser Abnahme nicht neu vermessen).

## 3. (2) K2 - Schluessel bei der Leiche

| Lauf | Schluessel | Kamera | Abstand zur Leiche | Ecken-Kasten | selbst gemessen im Bild |
|---|---|---|---|---|---|
| l2_leon, l2_elza (natuerlicher Kampf bis zum Tod) | (-2500,-1779,-6100) | 5 (Einstellung Cut 5, vorher Cut 0, F358-F450) | 762 | (247,116)-(263,130) = 17 x 15 | Ring etwa 9 x 11 Bildpunkte bei (254,123), als Schluesselring erkennbar |
| tr_leon, tr_elza (Todesrolle, RE15_GB_TEST=3) | (-2200,-1779,-19700) | 0 (Cut 0, F469-F561) | 789 | (269,100)-(280,110) = 12 x 11 | 8 Bildpunkte, Kasten (271,102)-(278,106) = 8 x 5, hellster Punkt 75-112, in jedem Bild ab 778 |
| tod_kanal_n | (-2700,-1769,-12300) | 5 (Cut 5, vorher Cut 2, F247) | 812 | (59,100)-(70,109) = 12 x 10 | nicht eigens ausgezaehlt |
| tod_wand_w | (-4300,-1775,-6200) | 5 (Cut 5, F358) | 740 | (244,103)-(255,114) = 12 x 12 | nicht eigens ausgezaehlt |
| tod_plattf (Elza) | (-2900,-1742,-20500) | 0 (Cut 0, F481) | 789 | (237,101)-(247,109) = 11 x 9 | etwa 4 bis 5 graue Bildpunkte bei (240-244,102-105) |

Der "Ecken-Kasten" ist die Protokollzeile des Ports (die projizierten Ecken des Schluessel-Quaders); die letzte Spalte ist, was der Pruefer in der
gdigrab-Aufnahme tatsaechlich an Schluessel-Bildpunkten gefunden hat. Die drei Lagen tod_* sind dieselben wie in Abnahme 2.

**Abstand.** Natuerlicher Kampf jetzt 762 von der Leiche. Stand b29263046: Schluessel (-1100,-1779,-5900), 3528 / 4492 / 4910 von der Leiche. Stand
vor bf345ea49: Schluessel (-5700,-1778,-7500) Kamera 4, 1214. Der Nutzerwunsch "näher bei der Leiche" ist erfuellt.

**Einstellung und Kamera.** Die Einstellung zeigt den Schluessel jeweils mit der Kamera, die an seiner Lage laeuft: state.log hat an der
Schluessellage cam=5 (natuerlicher Kampf) bzw. cam=0 (Todesrolle), gleich dem Cut der Einstellung.

**Aufnahme nach Leiter hoch und runter, genau einmal** (natuerlicher Kampf bis zum Tod, Leiter hoch, zurueck, zu Fuss zum Schluessel; beide Laeufe
exit=0): itemband.log hat je genau eine Zeile - Leon "F151 room=2090 ITEM slot 15 floor 0 item 0x34 tk 112 ... spieler=(-2563,0,-6399) -> FREI",
Elza "F156 room=2091 ... spieler=(-2770,0,-6197) -> FREI". Im Bild steht die Abfrage "Will you take the Water Key?". Nach einem zweiten Leitergang
liegt kein Schluessel mehr da. Befund 3 des Nutzers bleibt behoben.

**Todesrolle, neu gegenueber Abnahme 2.** Die Lage des Fixers ist bestaetigt ((-2200,-1779,-19700), Kamera 0, Tiefe 7283, 789 von der Leiche; in
Abnahme 2 lag der Schluessel bei (-7500,-1742,-25900) und war bis auf einen Bildpunkt verdeckt). Weil B1 die Endlage verschoben hat, hat der Pruefer
die Aufnahme dort zusaetzlich zu Fuss gefahren (`trk.sh`, Wegplan; Leon exit=0, Elza exit=0): Flag (9,112) = 0 bei F741; Ziel (-2200,-19700)
erreicht bei F883 an (-2282,-19772); itemband.log "F884 room=2090 (Elza 2091) ITEM slot 15 floor 0 item 0x34 tk 112 spieler-band 0
spieler=(-2282,0,-19772) -> FREI"; Flag (9,112) = 1 bei F1033; cam=0 an der Schluessellage. Dieser Lauf beginnt mit dem Testhaken der Todesrolle,
nicht mit einem natuerlichen Kampf, und ohne Leitergang.

**Becken-Scan** (`test_r41_alligator2.exe schluessel`, zweimal exit 0, woertlich): "groesster Abstand zur Leiche 2560, mittlerer 834, Bild
kleinstes 2.9 / mittleres 4.8"; Stand b29263046 "groesster Abstand zur Leiche 6036, mittlerer 3808, Bild kleinstes 6.3 / mittleres 9.8"; Stand
ae9e17c9e groesster 1348, mittlerer 823, kleinstes 2.9 / mittleres 4.9. "Endlagen-Pruefung (Abnahme 2 B1) gegen Stand ae9e17c9e: 20 von 208 Lagen
mit anderer Lage ... weiter als Stand b29263046 nur wegen der Endlage: 0"; "Je Lage gegen Stand b29263046: 208 Lagen, naeher 207, gleich 1, weiter
0". Die Zahlen des Fixers fuer HEAD (834 / 2560, 20 von 208, keine Lage weiter als b29263046) stimmen. Seinen Zwischenstand 61aeffeb8 (876 / 2560)
hat der Pruefer nicht nachgefahren. Die beiden weiteren Lagen aus seinem Bericht (tod_r1, k3) sind nur ueber den gruenen Riegel `tod` geprueft,
nicht eigens am echten Weg. Der Scan rechnet die Vergleichsstaende im Test nach; die Abstaende 3528-4910 und 1214 oben stammen dagegen aus echten
Laeufen der Abnahmen 1 und 2.

**Bildgroesse und Erkennbarkeit - K-2 (klein).** Am natuerlichen Todesort ist der Schluessel als Ring erkennbar (Bilder
`<S>/sicht/z_l2_leon.png`: Einstellung Bild 1490 und am Schluessel Bild 2262; `<S>/sicht/z_l2_elza.png`: Einstellung Bilder 2100 / 2160, am
Schluessel 2823). An den beiden ausgezaehlten anderen Lagen (Todesrolle, tod_plattf) ist er sichtbar und blinkt, aber nur 4 bis 8 Bildpunkte gross und fuer einen
Spieler nicht als Schluessel zu erkennen (`<S>/sicht/z_tr_leon.png`, `<S>/sicht/z_tod_plattf.png`); im Scan liegen 205 von 208 Lagen unter 10,4 Bildpunkten, und das mittlere Bild
ist gegenueber b29263046 von 9.8 auf 4.8 Bildpunkte geschrumpft. Das ist der Preis des Nutzer-Wortlauts "näher bei der Leiche" - nahe an der Leiche
heisst oft fern von der Kamera - und der Fixer nennt ihn offen. Kein Fehler der Nachbesserung, aber eine Sache fuer den Spieltest des Nutzers.

## 4. (2b) H3, (3) gelobter Kampf, Befunde 1 bis 3

**H3 Posenkante** (h3b_neu gegen h3b_alt = RE15_G2_DASH_BLEND_ALT=1, gdigrab 60 Bilder je Sekunde, beide exit=0): state.log und gator_trag.log sind
gleich, gator_boss.log weicht nur in "RE2 KANISTER angehaengt lokal=(3631,263,3196) F265" (neu) gegen "(3659,49,3163)" (alt) ab. Bild-zu-Bild-
Unterschied im Ausschnitt des Alligators: vor dem Abbruch in beiden 1.57 / 1.73 / 1.73 / 1.57; im ersten Bild nach dem Abbruch alt 3.28 (Sprung,
danach 1.20 / 1.03 / 0.85 / 0.83 / 0.76), neu 2.09 (danach 1.94 / 2.10 / 1.92 / 2.16, kein Sprung). Im Streifen `<S>/sicht/h3b_zoom_klein.png`
schliesst das Maul neu allmaehlich, alt schnappt es in einem Bild zu. Kein Befund.

**Gelobter Kampf.** Mit beiden Haken auf den alten Werten (RE15_K2080_WURF_GIER=3072, RE15_G2_DASH_BLEND_ALT=1) sind 9 von 9 Laeufen ueber die volle
Laenge in state.log, gator_trag.log, gator_boss.log und climb.log bitgleich mit dem Stand b29263046 (`.../abn1/runs/k_*`): id_stehen_ost
(3874 / 2958 / 1877 Zeilen), id_pendel_west (4474 / 3558 / 2284), id_pendel_innen (4474 / 3558 / 2816), id_lane_mitte (3874 / 2958 / 1990),
id_pendel_mitte, id_stehen_sued, id_elza_pendel_innen, id_elza_pendel_mitte, id_elza_stehen_sued. Ohne Haken liegt die erste Abweichung von
b29263046 genau im Bild der neuen Aufnahme (stehen_ost F127, pendel_west F809, pendel_innen F553, lane_mitte F131, Elza pendel_innen F608);
gator_boss.log weicht zuerst in "RE2 KANISTER gesetzt (-5084,-1200,-13316)" gegen "(-6870,-1200,-12651)" ab. Gang, Tempo, Angriffe, Biss,
Trefferreaktion und Szenen sind damit in den Protokollen bis zum ersten Eingriff bildgenau unveraendert; nach dem Eingriff wirkt nur der gewollte
Unterschied. Der Vergleich laeuft ueber die Zustandsprotokolle (Lage, Zustand, Clip je Bild), nicht ueber einen Bildpunktvergleich der Aufnahmen.

**Befund 1 (Kletterbiss an der Kante)**, 8 Laeufe (w_0_5506_4.0, w_1_5506_4.0, w_0_6500_4.0, w_1_6500_4.0, w_0_4000_4.0, w_0_5506_3.5, w_0_5506_5.0,
w_1_4000_4.5; alle exit=0), Wandpruefung gegen die SCA von ROOM2090.RDT / ROOM2091.RDT: "in/hinter fester Wand 0" in 8 von 8, "unten im Rampenblock
0", kleinster Abstand zur Aussenwand 450-455. In w_0_5506_3.5 meldet die Pruefung "oben neben dem Deck 2" Bilder; das ist wie in Abnahme 1
(F668-F672, Sturz von der Kante bis zur Landung) kein Bild in einer Wand und unveraendert. Sechs Laeufe sind bitgleich mit Abnahme 1; die beiden 6500er weichen von Abnahme 1 erst ab state.log
Zeile 1987 ab (neue Kanisteraufnahme) und sind bitgleich mit Abnahme 2. Bleibt behoben.

**Befund 2** = K1 (§2), **Befund 3** = K2-Aufnahme nach der Leiter (§3). Bleiben behoben.

## 5. (4) Riegel-Gruppe, (5) Riegel-Nachzug

**(4)** `cd .../re15_port/build && ctest -R "gator|kanister|alligator|2090|2080|re2gator" -j 4 --timeout 2400`, Start 2026-11-10 18:36:49 auf HEAD
5116d63ed, Ende 20:00:30, rc=0, unter Fremdlast (zwei weitere Spuren bauen und testen nebenher). Summenzeile woertlich:

    100% tests passed, 0 tests failed out of 154

    Total Test time (real) = 5019.56 sec

154 von 154 "Passed", kein "Failed", kein "Timeout", kein "Not Run". Es gab keinen roten Fenster-Riegel, also nichts einzeln nachzufahren. Laengste
Riegel: integration_r41_kanister2080_kampf 1268.20 s, integration_r41_gatorfix_frei_elza 858.18 s, integration_r41_gatorfix_abn2_wand 846.35 s,
integration_r41_gatorfix_abn2_wand_elza 845.53 s. Der Riegel der Abnahme 2, `unit_r41_gatorszene_tod`: im Gruppenlauf "Passed 2.75 sec", dazu
einzeln dreimal die Test-exe mit exit 0 und `tod: 0 Fehler` (`<S>/einzel_tod_1..3.log`). Die Angabe des Fixers (154 von 154 auf 3a532ee7e) ist auf
HEAD bestaetigt.

**(5) Riegel-Nachzug 05c8851e6.** Der Commit aendert nur drei Erwartungszeilen (3072 / 1024 -> 3342 / 888). Gegenprobe mit `riegel5.cmake`: Ein
Protokoll mit der alten Wurfrichtung ("Wurfrichtung 3072") ergibt "ALT: Wurfrichtung-Zeile Treffer 0, Wurf-Zeile Treffer 0" - der Riegel waere rot.
Das Protokoll von HEAD ergibt "NEU: Treffer 1, Treffer 1, Bahn ok (v_z 26, 3342)". Der Messhaken ist im Riegel nicht gesetzt. Die Aussage des Riegels
ist nicht geschwaecht.

## 6. Befunde

| Nr | Schwere | Befund | Beleg |
|---|---|---|---|
| B1 (Abnahme 2) | behoben | `unit_r41_gatorszene_tod` gruen; Schluessel in der Todesrolle nicht mehr verdeckt und aufnehmbar | §3, §5 |
| K-1 | klein | Alligator dicht an der Westwand schwimmt einmal am Kanister vorbei (Querabstand 2292-2354, Fenster 1733), Aufnahme beim naechsten Anlauf (F526 bis F1651); 9 von 18 gezielten Laeufen, 0 von 20 Standardlaeufen | §2 |
| K-2 | klein | Ausserhalb der natuerlichen Todeslage ist der Schluessel nur wenige Bildpunkte gross (ausgezaehlt: Todesrolle 8, tod_plattf 4 bis 5), sichtbar und blinkend, aber nicht als Schluessel erkennbar | §3 |
| H-1 | Hinweis | Kanister zieht in der tiefen Wurf-Einstellung kurz hinter Kopf und Schulter des Werfers durch | §2, Abnahme 2 |
| H-2 | Hinweis | Suedwurf (888) nur ueber den Messhaken belegt, auf dem natuerlichen Weg fiel die Wahl immer auf Nord | §2 |
| H-3 | Hinweis | Drei aeltere Commits (e5cd784dc, ae9e17c9e, 3061fe397) tragen die Schlusszeile "Claude Opus 5.5" statt der vorgegebenen | §1 |

K-1 und K-2 sind Folgen der Nutzer-Wortlaute selbst (Fenster nach RE2, Schluessel "näher bei der Leiche") und brauchen eine Entscheidung oder einen
Spieltest des Nutzers, keine weitere Nachbesserung vor dem Release.
