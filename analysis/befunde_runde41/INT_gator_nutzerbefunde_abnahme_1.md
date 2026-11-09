# INT Gator-Nutzerbefunde 2026-11-09 - UNABHAENGIGE ABNAHME 1

Baum `.claude/worktrees/r41_integration`, Zweig `r41/integration`, HEAD 3f12a9b4e (Basis master 0b86e36c6). Geprueft: Nachbesserung zu Abnahme 0
(6b221f0d6), Code-Commits 16f7b402b (B2) und bf345ea49 (B1), dazu der Gesamtstand gegen 0b86e36c6. Pruefer: Abnahme-Agent. Er glaubt dem Bericht
nichts. Kein Code geaendert, der einzige Commit ist dieser Bericht.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gator_nb/abn1`
(`runs/<lauf>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, debug.log, itemband.log; Laeufer `lauf.sh`, `schl.sh`, `serie_*.sh`, `sicht.sh`;
eigene Auswerter `sca_pruef.py` (Spieler-Lage je Bild gegen die SCA-Kaesten der RDT), `schl_ausw.py` (Schluessel-Ablauf), `mipsdis1.py` (Capstone);
Bilder unter `sicht/`; `ctest_gruppe.log`).

Nutzer 2026-11-09 (woertlich): "Den Aligator fight habe ich jetzt auch einmal getestet. Ist cool! Es gibt nur 3 winzige kleinigkeiten die mir
aufgefallen sind: 1. Wenn ich direkt an der Kante hochklettere auf die Platform und dann vom Aligator erwischt werden, ist es möglich das er mich in
die Wand schiebt. 2. Ich musste diverse runden mit dem Aligator drehen, bis er letzendlich den Behälter aufnahm. Eigentlich sollte er den Behälter
aufnehmen, sobald die Position erreicht ist, wo der Behälter liegt. Wenn das so nicht so einfach ist, lege den Behälter zentraler in die Mitte, damit
er ihn auf jeden Fall aufnehmen muss, sobald der Spieler den Aligator dort hingelockt hat. 3. Ich bin ausversehen nach dem Tot des Aligators die
Leiter hoch und wieder runter. Ich konnte den Schlüssel sehen, aber aufnehmen konnte ich ihn nicht mehr."

## 0. Urteil

**BESTANDEN.** Kein blockierender Befund. Beide blockierenden Befunde der Abnahme 0 sind am echten Weg behoben:
- **B2 (Biss bei der Kanister-Aufnahme):** Faellt die Aufnahme in einen Biss, laeuft der Biss jetzt Bild fuer Bild wie bei master zu Ende und trifft
  (stehen_sued Leon/Elza `BISSLAUF F121-F165 treffer 1`, Elza pendel_innen `F617-F661 treffer 1`). Belege selbst gelesen.
- **B1 (Schluessel-Kamera):** In allen natuerlichen Kampfenden liegt der Schluessel jetzt im Gebiet der Kamera der Einstellung (Kamera 5), am Schluessel
  laeuft Cut 5. Er ist sichtbar (gdigrab), genau einmal aufnehmbar und nach dem zweiten Leitergang weg (Leon und Elza).
Die Punkte 1 und 2 der Nutzer-Meldung sind erfuellt, ebenso der gelobte Kampf und die Belege. Offen bleiben nur Hinweise: der Schluessel liegt weiter weg
und kleiner als vorher (H1, Nutzer-Entscheid), andere Kameras sind nicht vermessen (H2), der Kanister-Restfall K1 und H3 aus Abnahme 0.

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| (1) Kletterbiss an der Kante / Wand | erfuellt | 9 neue Laeufe (Leon/Elza, x 4000/5506/6500, Biss in Kletterphase 2 und 3): 0 Bilder in oder hinter einem festen SCA-Kasten. master in 3 Laeufen je 1149 Bilder, Ende (11948,-1800,-18568) |
| (2) Kanister beim ersten Erreichen | erfuellt (Restfall K1) | 9 Lagen: Aufnahme in der ersten Passage mit dem Kanister im Fenster. Passagen bis zur Aufnahme 3/3/2/3 -> 1/2/1/1, Elza pendel_innen: master nie, jetzt F627 |
| (3) Schluessel nach Tod + Leiter | erfuellt (Hinweise H1, H2) | Leon/Elza: Einstellung Kamera 5, am Schluessel Cut 5, Aufnahme genau einmal (F158/F162). Nach dem 2. Leitergang keine Schluessel-Zeile, Inventar 34:1. gdigrab-Bild am Schluessel |
| (4) Gelobter Kampf unveraendert | erfuellt | Natuerlicher Kampf bis zur Einstellung bitgleich mit master (state.log 1231 Zeilen). Kanister-Laeufe bitgleich bis zum Aufnahmebild. Biss im Fenster F121-F165 wie master. 406 von 414 Bissen treffer 1, 8 treffer 0, nie mehr |
| (5) Belege | erfuellt | Keine neue Konstante. RE2- und PSX.EXE-Adressen selbst mit Capstone gelesen |

## 1. Bau, eigene exe

- `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`.
  Die exe ist von 18:24:19. Die juengsten Engine-Quellen sind aelter: gatorszene_r41.c / re15_gatorszene_r41.h 18:23:13, enemy_ai_boss_gator.c 18:09:09.
  Die Commit-Zeit 19:29 liegt zwar nach der exe, die Dateien aber davor. Die exe ist also der Stand HEAD. Beide Fix-Kennzeilen ("Biss laeuft weiter",
  "im Gebiet unter der") stehen in der exe (strings).
- Eigene Kopien in `build/platform/pc`: `re15_pc_abn1_neu.exe` (md5 f66a57183abca330d9e42551b298678a) und `re15_pc_abn1_alt.exe` = Hauptbaum-exe master
  0b86e36c6 (md5 34720fa04ec74df808286d0369bbfcf1, dieselbe wie in Abnahme 0). Beendet habe ich nur eigene PIDs (vier eigene Laeufe mit unpassendem
  Rezept, s. 4.).
- Herkunft: Meine Laeufe w_alt_0_5506_4.0 / w_neu_0_5506_4.0 sind in state.log, gator_trag.log und climb.log bitgleich mit v_alt_4.0 / v_neu_4.0 der
  Abnahme 0. Meine s2_leon / s2_elza sind in state.log und itemband.log bitgleich mit den Laeufen des Fixers (nb1/runs). Die Laeufe sind deterministisch.

## 2. Befund 1 - Kletterbiss an der Kante (erfuellt)

Echter Weg: DEBUG_JUMP 2090/2091, Ausloeseschuss, Szene 2080, Rueckkehr per Messhaken `RE15_K2080_RUECK_LAGE=<x>,-18568,3072,2` (Wasser vor der
Rampen-Suedkante, Blick Nord), danach `W<t>,A0.2,W40` (Aktionstaste an der Kante), 1300 Bilder. Eigener Auswerter `sca_pruef.py`. Er liest die SCA der
RDT (Offset 0x20, 10 verschiedene Kaesten: Aussenwaende floor 3, Plattform [4] floor 3, Rampe [5] floor 1, Ostwand und Plattform floor 19,
Kletterstreifen [8]/[9] floor 16) und prueft je Bild die Spielerlage (state.log PL x/z, climb.log y/sub). Kletterbilder zaehlt er mit.

| Lauf | Biss beim Klettern | in/hinter fester Wand | unten im Rampenblock | oben neben dem Deck | groesstes x | Lage 60 Bilder nach dem Biss |
|---|---|---|---|---|---|---|
| w_alt_0_5506_4.0 (Leon, master) | F130 sub 9 ph 3 | **1149** (erstes F152 (7880,-1800,-18568), Kletterbild) | 0 | 1128 | 11948 | **(11948,-1800,-18568)** |
| w_alt_1_5506_4.0 (Elza, master) | F130 sub 9 ph 3 | **1149** | 0 | 1128 | 11948 | **(11948,-1800,-18568)** |
| w_alt_0_4000_4.0 (Leon, master) | F123 sub 9 ph 2 | **1149** | 0 | 1128 | 11948 | **(11948,-1800,-18568)** |
| w_neu_0_5506_4.0 (Leon) | F130 sub 9 ph 3 | 0 | 0 | 0 | 6749 | (6527,0,-19967) Wasser |
| w_neu_1_5506_4.0 (Elza) | F130 sub 9 ph 3 | 0 | 0 | 0 | 6749 | (6527,0,-19967) Wasser |
| w_neu_0_4000_4.0 (Leon) | F123 sub 9 ph 2 | 0 | 0 | 0 | 6745 | (6736,0,-18609) Wasser |
| w_neu_0_6500_4.0 (Leon) | F138 sub 9 ph 3 | 0 | 0 | 0 | 6749 | (5283,0,-18927) Wasser |
| w_neu_1_6500_4.0 (Elza) | F138 sub 9 ph 3 | 0 | 0 | 0 | 6749 | (5283,0,-18927) Wasser |
| w_neu_0_5506_3.5 (Leon, oben) | - (Bisse auf dem Deck) | 0 | 0 | 2 = F668-F672, Sturz GTR KANTE -> LANDUNG | 6747 | - |
| w_neu_0_5506_5.0 / w_neu_1_4000_4.5 | - | 0 | 0 | 0 | 6749 / 6745 | - |

- Die Endlage der Nutzer-Sitzung (11948,-1800,-18568) habe ich mit master in drei Lagen nachgestellt (Leon, Elza, Biss in Kletterphase 2 und 3). Die neue
  exe hat in keiner Lage ein Bild in oder hinter einer Wand. Kleinster Abstand des Mittelpunkts zur Innenflaeche der Aussenwand: 450 (Klemme der Kollision).
- **Stoss ins Wasser weiter moeglich:** Biss am Kletteransatz: RE2-Reaktion sub 3, die Figur bleibt im Wasser und wird geschoben (z. B. GTR F131-F135, Klemme
  an der Ostwand x 6732). Bisse auf dem Deck (w_neu_0_5506_3.5): F330 Treffer, die Figur bleibt oben (6333,-1800,-17420). F667 Treffer an der Nordkante,
  `GTR F668 KANTE band 1 -> 0`, `GTR F672 LANDUNG y 0`: Sturz ins Wasser. Das Werkzeug `test_r41_gatorfix aussenlog` zaehlt den Sturz nicht und meldet
  `aussen 0, Deck verlassen 0, im Block 0 -> ok`.
- **Ohne Kletterbiss bitgleich mit master:** w_*_0_5506_3.5 und w_*_0_5506_5.0: state.log 2174/2174 und gator_trag.log 1258/1258 Zeilen gleich. Beim Lauf 3.5
  (mit Klettern, Deckbissen und Sturz) ist auch climb.log gleich.

## 3. Befund 2 - Kanister (erfuellt, Restfall K1)

Rezepte der Untersuchung (`kanister/werkzeug/serie*.sh`) mit der eigenen exe, dazu neue Lagen (pendel_west, Elza stehen_sued, Elza pendel_innen, je mit
eigenem master-Lauf). Verglichen habe ich mit den master-Laeufen der Untersuchung (`kanister/runs`, in Abnahme 0 als master-bitgleich belegt), mit eigenen
master-Laeufen und mit den Laeufen der Abnahme 0 (Stand 80c7394ab). Die Passagen zaehlt `kanister/werkzeug/passagen2.py`.

| Lauf | master: Aufnahme / Passagen bis dahin | HEAD: Aufnahme / Passagen | Phase beim Fenstertreffer | erste Abweichung zu master | zu 80c7394ab |
|---|---|---|---|---|---|
| stehen_ost | F1020 / 3 | **F253 / 1** | 12 DASH | F253 (Aufnahmebild) | keine |
| pendel_innen | F884 / 3 (1. laengs ausserhalb) | **F595 / 2** (1. laengs ausserhalb) | 12 DASH | F595 | keine |
| pendel_mitte | F492 / 2 | **F180 / 1** | 12 DASH | F180 | keine |
| lane_mitte | F559 / 3 | **F196 / 1** | 12 DASH | F196 | keine |
| stehen_sued | F165 / 1 (nach dem Biss) | **F145 / 1** | 3 LUNGE | F198 (nach dem Biss) | F145 (dort brach der Biss ab) |
| Elza stehen_sued | F165 / 1 | **F145 / 1** | 3 LUNGE | F198 | - |
| Elza pendel_innen | **keine** in 3600 Bildern | **F627 / 2** (1. laengs ausserhalb) | 3 LUNGE | F661 (Biss-Ende) | - |
| Elza pendel_mitte | - | F174 / 1 | 12 DASH | - | keine |
| pendel_west (neu) | F812 / 2 (1. quer 1755 > 1733) | F812 / 2 | 2 CHASE | **keine (bitgleich)** | - |

- Aufnahme beim ersten Erreichen: In jeder Lage faellt die Aufnahme in die erste Passage, in der der Kanister im Fenster liegt. Aus den "diversen Runden"
  (master 3/3/2/3 Passagen) werden 1/2/1/1. Bei pendel_innen ist die zweite Passage die erste mit dem Kanister im Fenster.
- **B2 behoben (der Biss laeuft zu Ende):** stehen_sued, Leon und Elza: `KANISTER-AUFNAHME aus Phase 3 F145 ... - Biss laeuft weiter`, `BISSLAUF F121-F165 art 4
  treffer 1`. Das ist genau der Biss von master (`BISSLAUF F121-F165 ... treffer 1`, Treffer GTR F128). Stand 80c7394ab hatte dort `BISSLAUF F121-F146`.
  state.log und gator_trag.log sind mit master bis F198 gleich, also durch den ganzen Biss (Gier, Lage, Clip, Bild). Elza pendel_innen: Aufnahme F627 im Biss,
  `BISSLAUF F617-F661 treffer 1` wie master, gleich bis F661.
- **Restfall K1 (klein, unveraendert):** Passagen seitlich am Fenster vorbei bleiben ohne Aufnahme (pendel_west F803-F805: quer 1755, Fenster |quer| < 1733;
  Aufnahme 7 Bilder spaeter in der naechsten Passage). Liegt der Kanister laengs ausserhalb (hinter der Wurzel), gibt es ebenfalls keine Aufnahme. Den
  Ersatzweg des Nutzers ("zentraler in die Mitte") hat niemand umgesetzt oder gemessen. Noetig ist er erst, wenn der Nutzer wieder "diverse Runden" meldet.

## 4. Befund 3 - Schluessel (erfuellt)

Echter Weg (`schl.sh`, Rezept der Abnahme 0): Kampf bis zum R1-Tod, Leiter hoch (`RE15_FIRE_AOT=0@500`), zurueck, zu Fuss zum Schluessel
(Wegskript des Fixers `nb1/plan/skript.txt` bzw. `plan_elza/skript_elza.txt`), Aktionstaste, Leiter hoch und zurueck. Ausgewertet mit `schl_ausw.py`.

| Lauf | Leiche | Schluessel (Einstellung) | Cut-Folge Einstellung / Weg | Aufnahme | nach 2. Leitergang |
|---|---|---|---|---|---|
| s2_leon | (-7328,-8628) | (-1100,-1779,-5900) Kamera 5, Durchgang 1, 4910 von der Leiche | Einstellung cam 5 F322-F448. Weg 0 -> 7 (F77) -> 6 (F95) -> 5 (F110), am Schluessel cam 5 (naechste Naehe 286 bei F153) | `F158 ... ITEM slot 15 ... item 0x34 ... -> FREI`, Inventar 34:1 | Eintritt ohne `Water Key`-Zeile, 34:1 bleibt |
| s2_elza | (-7328,-8628) | (-1100,-1779,-5900) Kamera 5 | Einstellung cam 5. Weg 0 -> 7 -> 6 -> 5 (F113), am Schluessel cam 5 (393 bei F157) | F162 `-> FREI`, 34:1 | wie Leon |
| a1_leon_k1 / a1_elza_k1 (eigener Kampf: erst L0.5, U1.5) | (-6956,-8628) | (-1100,-5900) Kamera 5, 4492 von der Leiche | Einstellung cam 5 F322-F448 | (nur Lauf 1) | - |
| a1_leon_k2 / a1_elza_k2 (eigener Kampf: erst R0.6, U2.0) | (-6079,-8623) | (-1100,-5900) Kamera 5, 3528 von der Leiche | Einstellung cam 5 F310-F436 | (nur Lauf 1) | - |

- Ablauf s2_leon (debug.log): `Eintritt 2090 > 2080 > 2090 > KEY > FolgeO > 2080 > KEY > Eintritt 2090 [34:-] > 2080 [34:1] > 2090 [34:1]`. Nach der Aufnahme
  erscheint keine Schluessel-Zeile mehr. Der Schluessel ist sichtbar, aufnehmbar, genau einmal, danach weg. Bei Elza gleich.
- Mit master (`alt_s2_leon` / `alt_s2_elza`, gleiche Eingabe) ist der ganze Lauf bis F358 der Einstellung bitgleich (state.log 1231 Zeilen). Dort zeigt master
  Kamera 4 (`Folge O ... wert=0/4`), HEAD Kamera 5 (`wert=0/5`). Das ist die Kamera, in deren Gebiet der Schluessel liegt und die am Schluessel laeuft.
- **Sichtbar (gdigrab, echtes Fenster, beschleunigter Renderer):** `sicht.sh` faehrt das Rezept s2_leon bis Besuch 5, Bild 157 (direkt vor der Aufnahme).
  `<SP>/sicht/montage_weg.png` und `zoom_262.png`: In Cut 5 liegt der Ring des Schluessels auf dem Wasser rechts vom Bildrand der Plattform (Bilder 258/260).
  Im letzten Bild steht Leon daran. Er ist klein (Ring etwa 10 Bildpunkte bei 320x240), aber zu sehen.
- Vier eigene Laeufe mit anderen Startlagen (-3000,-22000) und (-5000,-9000) habe ich abgebrochen (eigene PIDs): Dort loest die 2080-Szene nicht aus, das
  Rezept passt nicht. Die zwei Kampf-Varianten k1/k2 ergeben drei verschiedene Leichen, alle im NW-Becken. In allen liegt der Schluessel in
  (-1100,-5900), Kamera 5. Andere Gebiete (Suedbecken, Nutzer-Leiche) decken nur die Riegel ab (tod_gebiet, alligator2_schluessel, s. 6.).

## 5. Belege der Nachbesserung (selbst gelesen)

Eigenes Capstone-Skript `mipsdis1.py` auf `info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN` (Basis 0x80100000) und `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Datei +0x800):
- Wurzel: `80100320 f105040c jal 0x801017c4` / `80100328 e204040c jal 0x80101388`. T2 laeuft, danach unbedingt die Kiefer-Maschine. Stimmt.
- Kiefer-Maschine: `80101398 24020382 lb v1,0x224(s0)`, `801013a0 sltiu v0,v1,6`, Sprungtafel (Worte @0x80100004..0x80100018) = 0x80101564, 0x801013c4,
  0x801013d4, 0x80101418, 0x80101488, 0x801014f8. Im ganzen Bereich 0x80101388-0x80101578 gibt es nur Zugriffe auf +0x224, keinen auf +0x225..+0x227.
  Stufe 5 endet mit `80101558 240200a2 sb zero,0x224(s0)` / `8010155c 5e05040c jal 0x80101578`. Stimmt.
- T1: `8010133c lb v0,0x224(a0)` / `80101344 bnez -> 0x80101380 (jr ra)` / `80101360 lbu v1,0x226(a0)` / `80101368 beqz` / `80101370 bne v1,7` /
  `8010137c 240282a0 sb v0,0x224(a0)`. +0x226 sperrt nur den neuen Biss. Stimmt.
- FUN_80014230 (PSX.EXE): `80014254 16008390 lbu v1,0x16(a0)`, `8001425c bne v1,v0`, `80014260 addiu s1,a0,0x14`, `80014274 beq v1,s2(0xff)`,
  `80014298 jal 0x80014368`, `800142a8 lbu a0,1(s0)`, `800142ac jal 0x800142f4`. Stimmt.
- Neue Konstanten: keine. Im Engine-Diff 0b86e36c6..HEAD steht ausser Jahreszahlen in Kommentaren keine neue Zahl. B1 nutzt die bestehenden
  GSZ_SUCH_STUFEN (4) und GSZ_KEY_BILD_MIN10. B2 schaltet nur die Phase nicht mehr um. Die Port-Wahl "Fenster im Zonen-Dash" traegt den Nutzer-Wortlaut
  aus Punkt 2, die Wahl "Schluessel im Gebiet seiner Kamera" den Wortlaut aus Punkt 3.
- Code gelesen: `key_lage_bei_tod`. Durchgang 2 (ohne Gebiet) laeuft nur noch im Pruefhaken. Erreicht keine Stufe die Schwelle, gelten `l` und `cut`
  der Stufe 4: die Lage mit der kleinsten Tiefe im ganzen Ring bis zur doppelten Naehe (die Ringe wachsen). Die Beschreibung "beste Lage im Gebiet bis zur
  doppelten Naehe" trifft also zu. `gb_takt`: `abbruch = phase == GBP_RE2_DASH || Pruefhaken`. In LUNGE geht nur das Fenster auf.

## 6. Gator-Riegel (selbst gefahren)

Bau wie in 1. Danach, ohne andere eigene Laeufe:

`ctest -R "gator|kanister|alligator|2090|2080|re2gator" -j 4 --timeout 2400` (alle 152 Tests der Gruppe, unit und exe), Summenzeile woertlich:
**`100% tests passed, 0 tests failed out of 152`** (`Total Test time (real) = 5410.25 sec`, unter Fremdlast). Darin unter anderem:
unit_r41_gatorfix_kanbiss / _elza (13,8 / 13,7 s), integration_r41_gatorfix_kanister_vorbei / _elza (465 / 490 s; kanpass Lauf a Elza: Aufnahme F627,
erstes Fensterbild F626 in Phase 3 -> OK), integration_r41_alligator2_schluessel / _elza / _west_elza (172 / 247 / 263 s), unit_r41_alligator2_schluessel
(236 s), unit_r41_gatorszene_tod_gebiet / _elza, integration_r41_gatorszene_tod*, unit_r41_gatorfix_kletterbiss / _elza (228 / 234 s),
unit_r41_alligator2_weg, integration_r41_re2gator, kanister2080*. Die 23 exe-Riegel, die der Fixer nach der Nachbesserung nicht gefahren hat, sind darin
enthalten (gatorfix_frei/rampe/abn2_*/gang_*/oben_kante/rampe_oben, alligator2_kamera/uebergabe/tragen/finisher, gatorszene_leon/elza/flucht,
kanister2080*, re2gator). Alle sind gruen. Protokoll: `<SP>/ctest_gruppe.log`.

## 7. Hinweise / Restpunkte (nicht blockierend)

- **H1 Schluessel weiter weg und kleiner (Nutzer-Entscheid offen, so auch vom Fixer gemeldet):** In allen fuenf natuerlichen Kampfenden liegt der Schluessel in
  (-1100,-5900), 3528-4910 von der Leiche (vorher 1021-1214). Er liegt dort im Durchgang 1, also unter der Bildschwelle 10,4 (FG-E7, Orchestrator-Entscheid;
  natuerlich 10,2). Ob der Nutzer ihn naeher an der Leiche haben will, zeigt erst der Spieltest. Nutzerbefund 3 selbst ist erfuellt.
- **H2 Andere Kameras:** Geprueft habe ich die Kamera der Einstellung und die Kamera am Schluessel. Ob der Schluessel aus Cut 6/7 von weitem zu sehen
  ist, habe ich nicht vermessen. In den gdigrab-Bildern aus Cut 6/7 sind Wasserspritzer an den Fuessen, die ich nicht sicher vom Schluessel trennen kann.
- **K1 Kanister:** seitliche Passagen ohne Aufnahme (s. 3.). Den Ersatzweg "zentraler" erst messen, wenn der Nutzer es wieder meldet.
- **H3 aus Abnahme 0:** Dash-Abbruch ohne yaw += 1024, die Pose-Kante ist nicht per gdigrab geprueft. Den Riegel alligator2_weg habe ich nicht gegen
  0b86e36c6 nachgefahren.
- Kein master-Merge, kein Paket (nicht Teil des Auftrags). Die eigenen exe-Kopien re15_pc_abn1_*.exe habe ich nach der Abnahme geloescht.
