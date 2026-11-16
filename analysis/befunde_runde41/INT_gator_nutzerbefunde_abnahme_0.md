# INT Gator-Nutzerbefunde 2026-11-09 - UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r41_integration`, Zweig `r41/integration`, HEAD 077481839 (Basis 0b86e36c6; Code-Commits 8d165ee58, 80c7394ab,
6d7a9e121, f8c71298a). Pruefer: Abnahme-Agent, glaubt dem Bericht nichts. Kein Code geaendert; einziger Commit = dieser Bericht.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gator_nb/abn0`
(je Lauf `runs/<name>/` mit state.log, climb.log, gator_boss.log, gator_trag.log, debug.log, itemband.log; Auswerter `wand_ausw.py`,
`vergleich.py`; Laeufer `lauf.sh`, `schl.sh`, `serie_*.sh`).

Nutzer 2026-11-09 (woertlich): "Den Aligator fight habe ich jetzt auch einmal getestet. Ist cool! Es gibt nur 3 winzige kleinigkeiten die mir
aufgefallen sind: 1. Wenn ich direkt an der Kante hochklettere auf die Platform und dann vom Aligator erwischt werden, ist es möglich das er mich in
die Wand schiebt. 2. Ich musste diverse runden mit dem Aligator drehen, bis er letzendlich den Behälter aufnahm. Eigentlich sollte er den Behälter
aufnehmen, sobald die Position erreicht ist, wo der Behälter liegt. Wenn das so nicht so einfach ist, lege den Behälter zentraler in die Mitte, damit
er ihn auf jeden Fall aufnehmen muss, sobald der Spieler den Aligator dort hingelockt hat. 3. Ich bin ausversehen nach dem Tot des Aligators die
Leiter hoch und wieder runter. Ich konnte den Schlüssel sehen, aber aufnehmen konnte ich ihn nicht mehr."

## 0. Urteil

**NICHT BESTANDEN.** Zwei blockierende Befunde:

- **B1:** Befund 3 ist im natuerlichen Kampf nicht behoben. In allen vier eigenen exe-Laeufen (Leon und Elza, je zwei Startlagen,
  echter Kampf bis zum R1-Tod) greift der Rueckfall auf den Stand 0b86e36c6. Die Einstellung zeigt den Schluessel in Kamera 4 bzw. 6,
  am Schluessel laeuft aber Cut 5. Das ist genau der Mechanismus, den der Fix beheben soll.
- **B2:** Kanister-Aufnahme im Biss: der Port bricht den laufenden Biss ab (Phase LUNGE -> CHASE). Der angegebene RE2-Beleg traegt das nicht.
  In RE2 laeuft die Kiefer-Maschine 0x80101388 unabhaengig von +0x226 weiter und ruft in Stufe 5 den Treffer 0x80101578.

Erfuellt sind: Befund 1 (am echten Weg reproduziert und behoben, Leon und Elza), Befund 2 im Kern (Aufnahme im ersten Fensterbild, 6 von 6
betroffenen Laeufen) und der gelobte Kampf bis zum jeweiligen Eingriff, der Bild fuer Bild gleich bleibt.

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| (1) Kletterbiss / Wand | erfuellt | alt: Ende (11948,-1800,-18568) = Nutzer-Sitzung, 1149 Bilder aussen; neu: 0 Bilder aussen, Figur im Wasser (6527,0,-19967); Elza gleich |
| (2) Kanister beim ersten Erreichen | erfuellt, Restfall + B2 | 6/6 Laeufe Aufnahme im ersten Fensterbild (F253 statt F1020 usw.); seitliche Dash-Passagen (lane_innen 3x) bleiben ohne Aufnahme |
| (3) Schluessel nach Tod + Leiter | **nicht erfuellt (B1)** | aufnehmbar, genau einmal, danach weg (Leon/Elza); aber Einstellung Cut 4/6, am Schluessel Cut 5 in 4 von 4 natuerlichen Laeufen |
| (4) Gelobter Kampf unveraendert | erfuellt bis zum Eingriff | 12 Kanister-Laeufe: state.log/gator_trag.log bitgleich mit 0b86e36c6 bis genau zum Aufnahmebild; Klettern ohne Biss bitgleich |
| (5) Belege | **B2**, sonst ok | Adressen selbst gelesen; Bissabbruch widerspricht RE2 @0x801014f8-0x8010155c |

## 1. Bau, eigene exe, Herkunft der Vergleichs-exe

- `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. exe 2026-11-09 15:44:28; die juengste Quelle (enemy_ai_boss_gator.c) ist 15:44:25, der Baum ist sauber.
- Eigene Kopien in `build/platform/pc`: `re15_pc_abn0_neu.exe` (md5 69a41c379404d7141e4450751d2a24d7) und `re15_pc_abn0_alt.exe` =
  Hauptbaum-exe (master 0b86e36c6, 10:30:18, md5 34720fa04ec74df808286d0369bbfcf1; letzter Engine-Commit b8e3ad700 07:35).
- Herkunftsbeweis alt: Lauf `k_alt_stehen_ost` ist mit dem Lauf `stehen_ost` des Untersuchers (Stand 0b86e36c6) **bitgleich** in state.log,
  gator_trag.log und climb.log. Beendet wurden nur eigene PIDs (re15_pc_abn0_*).

## 2. Befund 1 - Kletterbiss an der Kante (erfuellt)

Echter Weg: DEBUG_JUMP 2090/2091, Ausloeseschuss, Szene 2080, Rueckkehr per Messhaken `RE15_K2080_RUECK_LAGE=5506,-18568,3072,2`
(Wasser vor der Rampen-Suedkante, Blick Nord = Lage der Nutzer-Sitzung GTR F758), danach `W<t>,A0.2` (Aktionstaste an der Kante).
Auswerter `wand_ausw.py` je Bild des letzten 2090-Besuchs; anders als `aussenlog` zaehlt er auch Kletterbilder mit.

| Lauf | Treffer beim Klettern | danach Kletterbilder | aussen (hinter Aussenwand) | groesstes x | Lage 60 Bilder nach dem Biss |
|---|---|---|---|---|---|
| v_alt_4.0 (Leon, alt) | F130 sub 9 ph 3, Figur (5948,0,-18568) | 42 | **1149** | 11948 | **(11948,-1800,-18568)** = Nutzer-Sitzung |
| v_neu_4.0 (Leon, neu) | F130 sub 9 ph 3, Figur (5948,0,-18568) | **0** | **0** | 6743 | (6527,0,-19967) im Wasser |
| e_alt_1_5506_4.0 (Elza, alt) | F130 sub 9 ph 3, Figur (5948,0,-18568) | 42 | **549** (erstes F152 (7880,-1800,-18568)) | 11948 | **(11948,-1800,-18568)** |
| e_neu_1_5506_4.0 (Elza, neu) | F130 sub 9 ph 3, Figur (5948,0,-18568) | **0** | **0** | 6743 | (6527,0,-19967) im Wasser |
| e_alt_0_6500_4.0 (Leon x 6500, alt) | F138 sub 9 ph 3, Figur (6732,0,-18568) | 34 | **549** | 11948 | **(11948,-1800,-18568)** |
| e_neu_0_6500_4.0 (Leon x 6500, neu) | F138 sub 9 ph 3, Figur (6732,0,-18568) | **0** | **0** | 6749 | (5283,0,-18927) im Wasser |

- Die Nutzer-Endlage (11948,-1800,-18568) ist mit der alten exe am echten Weg nachgestellt, die neue exe behebt sie.
- **Stoss ins Wasser weiter moeglich:** v_neu_4.0: Biss am Kletteransatz -> RE2-Reaktion Sub 3 nach Osten, Figur bleibt y 0 im Wasser
  (GTR F131-F135 `schritt 800 ... 320`, Klemme an der Ostwand x 6732). Auf dem Deck: w_neu_0_5506_0.3 / v_neu_1.0 / v_neu_2.0 je
  `GTR ... KANTE` + `LANDUNG` (Sturz von der Rampe ins Wasser), gleich wie alt.
- **Ohne Biss beim Klettern bitgleich:** w_*_0.3 (2400 Bilder, 14 Treffer auf dem Deck): state.log 3274/3274 und gator_trag.log 2217/2217
  Zeilen gleich alt/neu. v_*_1.0, v_*_2.0, v_*_3.0: gleiche Treffer, gleiche Kanten; `aussenlog` (Werkzeug des Baums) auf
  w_neu_0_5506_0.3: `aussen 0, Deck verlassen 0, im Block 0 -> ok`.
- Code: `re15_climb_routine_ersetzt()` loescht nur s_sub/s_kind/s_phase (+5/+6/+7); y, Band, Boden bleiben. Aufruf zu Beginn von
  `re15_gator_treffer_r41_einstieg`. Keine neue Konstante.
- Beleg selbst gelesen: RE2 EM23_OVL_0000.BIN `801015b4 05020224 addiu v0,zero,0x205` / `801015c4 fcfb22ac sw v0,-0x404(at)`
  (Wort-Store PL+4 = 0x800cfbfc), Tod `80103948 fcfb22ac`; RE1.5 PSX.EXE `80012ed4 050022a2 sb v0,5(s1)` / `80012ee0 060020a2 sb zero,6(s1)`.
  Die Kletter-Sperre +0x93 ist ebenfalls gelesen: `80038414 01004234 ori v0,v0,1` und `80038748 fe004230 andi v0,v0,0xfe`, Ziel 0x800acae7 = PL+0x93.

## 3. Befund 2 - Kanister (Kern erfuellt; B2; Restfall)

Rezepte des Untersuchers (`kanister/werkzeug/serie1-3.sh`) mit der eigenen exe, Vergleich mit seinen 0b86e36c6-Laeufen (`vergleich.py`) und
`passagen2.py`:

| Lauf | alt Aufnahme | neu Aufnahme | Phase beim Fenstertreffer | erste Abweichung state.log / gator_trag.log |
|---|---|---|---|---|
| stehen_ost | F1020 | **F253** | 12 (DASH) | genau F253 |
| pendel_innen | F884 | **F595** | 12 (DASH) | genau F595 |
| pendel_mitte | F492 | **F180** | 12 (DASH) | genau F180 |
| lane_mitte | F559 | **F196** | 12 (DASH) | genau F196 |
| stehen_sued | F165 | **F145** | 3 (LUNGE) | genau F145 |
| Elza pendel_mitte | - | **F174** (erste Passage) | 12 (DASH) | - |
| lane_drueber, lane_innen, stehen_drauf, stehen_nord, stehen_west | F1155/F2181/F132/F132/F131 | gleich | - | **keine (bitgleich, 3874 Zeilen)** |

- Aufnahme beim ersten Erreichen: in allen 6 Laeufen, in denen das Fenster in Biss/Dash fiel, kommt sie jetzt im ersten Fensterbild.
  Passagen bis zur Aufnahme: stehen_ost 2 -> 1, pendel_innen 3 -> 2 (die erste, laengs ausserhalb, bleibt), pendel_mitte 2 -> 1,
  lane_mitte 3 -> 1, stehen_sued 1 -> 1.
- Gelobter Kampf: in allen 11 Leon-Laeufen ist alles **vor** der Aufnahme Bild fuer Bild gleich (Gang, Tempo, Biss, Treffer, Dash).
- Bewegung nach dem Dash-Abbruch: die Wurzel springt nicht (F252 (-5039,-13958) -> F253 (-5043,-13905)), danach laeuft das authored
  Clip-0-Profil wie nach jedem Dash-Ende (Vorwaertsanteil je Clip-Bild 0-4: +33, 5-20: -14 .. -2, ab 25 positiv; gleich in alt, neu und
  pendel_mitte). Pose-Uebergang Clip 2 -> Clip 0 nicht per gdigrab geprueft (Hinweis H1).
- **Restfall (klein, K1):** lane_innen (bitgleich alt/neu): drei Dash-Passagen mit quer 2477/2812/2965 (Kanister ~1000-1500 neben der
  Flanke) ohne Aufnahme, Aufnahme erst F2181 (~67 s spaeter); lane_drueber: 1 seitliche Passage (quer 2787), Aufnahme 46 Bilder spaeter.
  Das Fenster ist |quer| < 1733 (gr2_kan_takt). Den Ersatzweg des Nutzers ("zentraler in die Mitte") hat der Fixer nicht gemessen.
- Belege selbst gelesen: Fenster `80100ae4 67de4224 addiu v0,v0,-0x2199` / `80100ae8 8f01422c sltiu v0,v0,0x18f` /
  `80100af4 26020292 lbu v0,0x226(s0)`; T1 im Pre `80100974 bf04040c jal 0x801012fc`; Dash-Ende `80100eb0-ec0` (+0x225 = 0, yaw += 0x400),
  `80100ee4 040022ae sw v0,4(s1)`. Das Fenster im Biss ist RE2-belegt, im Dash Port-Wahl mit Nutzer-Wortlaut: das ist korrekt.

### B2 - Bissabbruch bei der Aufnahme widerspricht RE2 (blockierend)

`enemy_ai_boss_gator.c` (Commit 80c7394ab): Fenstertreffer in GBP_LUNGE -> `g->phase = GBP_CHASE; e->motion = 0; e->anim_frame = 0`.
Kommentar und Commit begruenden das mit "Der Biss endet - RE2 schaerft T1 bei Stufe 1..6 nicht (@0x80101360-70)". Selbst gelesen:

- T1 0x801012fc **schaerft** nur: `8010133c lb v0,0x224(a0)` / `80101344 bnez -> jr ra` (laeuft schon ein Biss, nichts) ... `80101360 lbu v1,0x226(a0)`
  / `80101368 beqz` / `80101370 bne v1,7` -> `8010137c sb v0(1),0x224(a0)`. Bei Kanister-Stufe 1..6 wird also kein **neuer** Biss gestartet.
- Der **laufende** Biss ist die Kiefer-Maschine 0x80101388: `80101398 lb v1,0x224(s0)`, Sprungtafel @0x80100004 =
  {0x80101564, 0x801013c4, 0x801013d4, 0x80101418, 0x80101488, 0x801014f8}. Stufe 5 (0x801014f8) endet mit `80101558 sb zero,0x224(s0)`
  und `8010155c jal 0x80101578` (Treffer-Einstieg, darin `801015b4`/`801015c4` der Wort-Store). Keine dieser Stufen liest +0x226.
- Schreiber von +0x224 im ganzen Overlay: `80100664` (Init), `80101058`, `80101558`, `80101f80`, `801022e0`, `80102674`. In T2
  (0x801017c4 ff., Kanister-Stufen) ist keiner.

Folge: in RE2 laeuft ein angefangener Biss bei der Aufnahme zu Ende und kann treffen. Der Port bricht ihn ab; trifft das Fenster vor
dem Trefferbild (Port: BISS af=6), entfaellt der Treffer. Gemessen im Riegel-Lauf des Fixers (Elza, `kanister_vorbei_elza_wd/lauf_8e143b27/a`):
`BISS af=6..8 ... hr=0`, dann `KANISTER-AUFNAHME aus Phase 3 F627` - der Biss endet in Bild 9. In k_stehen_sued fiel der Treffer schon
vorher (GTR F128), dort ist nur die Biss-Animation gekuerzt. Das veraendert den gelobten Biss ohne Beleg (CLAUDE.md RE-Gate, Memory
"gelobter Stand ist Referenz"). Kleinster Weg: im Biss nur das Fenster oeffnen (im_chase = 1) und die Phase LUNGE zu Ende laufen lassen.
Die Kanister-Stufen ab 1 laufen in `gr2_kan_takt` ohnehin phasenunabhaengig. Den Dash-Abbruch (Port-Wahl) betrifft das nicht.

## 4. Befund 3 - Schluessel (B1, blockierend)

Echter Weg (`schl.sh`): Rezept des Riegels integration_r41_alligator2_schluessel (Schussbloecke, RUECK_KANISTER) mit Abschnitten je Raum
`W9|Kampf|W60|Kampf|W30|<Weg>|W30|W3`, `RE15_FIRE_AOT=0@500#2090,1@40#2080,0@560#2090,1@40#2080` (Leiter hoch nach dem Tod, zurueck;
nach der Aufnahme noch einmal hoch und zurueck). Den Weg von der Leiter-Ankunft (-8200,-25550) zum Schluessel schreibt das Werkzeug
`test_r41_alligator2 wegplan`.

| Lauf | Leiche | Schluessel (Einstellung) | Cut am Schluessel (state.log beim Druck) | Aufnahme | nach 2. Leitergang |
|---|---|---|---|---|---|
| s2_leon (Start -6000,-17500) | (-7328,-8628) | (-5700,-7500) **Kamera 4** | **cam=5** (F129-F137) | F137 `ITEM slot 15 ... -> FREI`, `34:1` | keine Zone/kein Modell, `34:1` bleibt |
| s2_elza (2091) | (-7328,-8628) | (-5700,-7500) **Kamera 4** | **cam=5** (F136-F139) | F137 `-> FREI`, `34:1` | keine Zone/kein Modell, `34:1` bleibt |
| s1_leon_west (-8000,-16000) | (-7038,-8628) | (-5800,-8700) **Kamera 6** | Cut 5-Gebiet (Zone 17/19: 5<->6 bei z -14000..-12000) | - | - |
| s1_elza_west | (-7038,-8628) | (-5800,-8700) **Kamera 6** | wie oben | - | - |

- Aufnehmbar, genau einmal, danach weg: **erfuellt** (Leon und Elza; nach dem zweiten Leitergang keine Zeile "Water Key ... bei der Leiche",
  kein zweites `ITEM slot 15`, Inventar `34:1`).
- **Aber:** In 4 von 4 natuerlichen Laeufen greift der Rueckfall (Durchgang 2 = Stand 0b86e36c6). Die Einstellung und das groesste Bild zeigen den
  Schluessel in einer Kamera, deren Gebiet woanders liegt. Kamera 4 steht bei (-7740,-3132,-6462) (Sonde `x` des Untersuchers); ihr Gebiet ist
  das Nordbecken-Ost (Eintritt 3->4 x 1000..8000, 5->4 x -1000..0; Austritt 4->5 x -2000..-1000). Der Schluessel liegt 2,3 k vor ihr,
  also in Cut 5. Das ist dasselbe Muster wie beim Nutzer: Kamera 7 in der SW-Ecke, Schluessel im Suedbecken, Gebiet Westkanal.
  Damit bleibt das Nutzer-Symptom "sehen, aber nicht aufnehmen" im gewoehnlichen Kampfverlauf moeglich. Der Fixer meldet selbst 126 von
  208 Raster-Todeslagen im Rueckfall. Meine vier echten Laeufe liegen alle darin. Der Fix wirkt nur bei Todeslagen wie der des Nutzers
  (Harness-Riegel tod_gebiet).
- Der Zielkonflikt "Gebiet" gegen "10,4 Bildpunkte" ist kein Nutzerentscheid gegen Nutzerentscheid. FG-E7 ist ein Orchestrator-Entscheid
  (AUFTRAG.md 2026-10-26 01:19, "der Nutzer kann widersprechen"); Nutzerbefund 3 ist woertlich. Vorschlaege (Nutzer oder Orchestrator
  entscheiden): (a) im Gebiet bleiben, auch unter 10,4 Bildpunkten; (b) im Gebiet ueber die doppelte Naehe hinaus weitersuchen; (c) im
  Rueckfall die Lage so waehlen, dass die Einstellung den Cut zeigt, der am Schluessel laeuft.
- Code geprueft: `zone_viereck` (Anker-Satz, Boden-Byte 0xFF/0), `gebiet_rechnen` (Flutfuellung, 4er-Nachbarschaft), zwei Durchgaenge in
  `key_lage_bei_tod`. Keine neue Zahl. FUN_80014230 selbst gelesen: `80014254 lbu v1,0x16(a0)` / `8001425c bne v1,v0` / `80014260 addiu s1,a0,0x14`
  / `80014274 beq v1,s2(0xff)` / `80014298 jal 0x80014368` / `800142a8 lbu a0,1(s0)` / `800142ac jal 0x800142f4` - passt zur Beschreibung.
- Riegelaenderung `unit_r41_alligator2_weg` (Boss-Zustand frisch je Lage): die Begruendung (Zustand leckt zwischen Harness-Lagen) ist
  plausibel, die Gegenprobe "auch mit 0b86e36c6 rot" habe ich nicht nachgefahren (H3).

## 5. Gator-Riegel (selbst gefahren)

1. Gruppe `ctest -R "gator|kanister|alligator|2090|2080|re2gator" -j 4 --timeout 1800` gestartet (150 Tests). Unter meinen parallelen
   exe-Laeufen kam sie zu langsam voran. Fertig wurden 3 Tests, alle `Passed`: integration_r41_gatorfix_abn2_rampe_west_elza 822,51 s,
   _abn2_wand_elza 836,21 s, _frei_elza 854,49 s. Danach habe ich sie abgebrochen (eigene PIDs).
2. Ziel-Satz (96 Tests) nach dem Ende aller exe-Laeufe, `ctest -R "unit_r41_gatorfix_(kletterbiss|nb4|raum|gang|re2|v4|nb3|wurf|maul|klemm|fuzz)|
   unit_r41_gatorszene_|unit_r41_alligator2_|integration_r41_gatorfix_kanister|unit_r41_re2gator|unit_r41_kanister2080|unit_gator|
   integration_r41_alligator2_schluessel|integration_r41_restpunkte_gator" -j 6 --timeout 1800`, Summenzeile woertlich:
   **`100% tests passed, 0 tests failed out of 96`** (Total 510,53 s). Darin unter anderem: kletterbiss / _elza (je 206 s),
   kanister_vorbei / _elza (342 / 380 s), gatorszene_tod_gebiet / _elza, alligator2_schluessel (unit und exe, Leon/Elza/West), alligator2_weg,
   restpunkte_gator / _elza, gatorfix_nb4/raum/gang/re2/nb3.
   Die Riegel sind gruen. Sie decken B1 nicht ab: der Rueckfall ist dort erlaubt und nur dokumentiert. B2 decken sie ebenfalls nicht ab:
   ein Biss, der bei der Aufnahme abbricht, wird nicht gegen RE2 geprueft.

## 6. Hinweise / Restpunkte

- **H1** Dash-Abbruch Clip 2 -> Clip 0 ohne yaw += 1024: Lage stetig gemessen, Pose-Kante nicht per gdigrab abgenommen (auch vom Fixer offen).
- **K1** Restfall Kanister: seitliche Passagen (quer 1968-2997) im Dash/Chase ohne Aufnahme; lane_innen braucht 4 Passagen. Den Ersatzweg
  des Nutzers "zentraler in die Mitte" messen, falls der Nutzer erneut "diverse Runden" meldet.
- **H2** Nebenbefund des Fixers (RE1.5-Schadenspfad beendet Klettern nicht, @0x80012ed4) bestaetigt im Code-Lesen; in 2090 unerheblich.
- **H3** Riegel `unit_r41_alligator2_weg` wurde vom Fixer veraendert (Boss-Zustand frisch); nicht gegen 0b86e36c6 nachgefahren.
- Kein master-Merge, kein Paket (nicht Teil des Auftrags).
