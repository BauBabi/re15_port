# Runde 41 - Spur G2 "gatorfix" - UNABHAENGIGE ABNAHME 3 (2026-11-02)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gatorfix`, HEAD b78387a6c (Nachbesserung 4: 60d7c2671 Code + b78387a6c Doku). Pruefer:
Abnahme-Agent, glaubt dem Bau nichts. Kein Spielcode geaendert. Laufordner (nicht im Baum): `<SP>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G2abn3`
(je Lauf `runs/<name>/` mit video.mp4, debug.log, state.log, gator_boss.log, gator_trag.log, climb.log, env.txt, exe_md5.txt).
Belege: `G2_belege/abnahme3/` (Bilder, `messung_abnahme3.txt` = alle Messausgaben, `werkzeug/` = Laufskript, Eingaben, Auswerter).

## 0. Urteil

**NICHT BESTANDEN.** Sechs der acht Teilpunkte des Nutzerwortlauts sind im echten Fenster erfuellt (Wurf/Fang, Montage 5, Marvin/John 3,
Musik, Schluessel, kein Anschwimmen), das Haengen mit Kanister ist im freien Spiel behoben. Zwei Maengel bleiben:
**M1** die Leiche ragt weiterhin sichtbar aus dem Maul (Rollfenster Clip 5 Bild 3-11 ~0,3 s oben ueber dem Schaedel, Fang Bild 74-76 ~0,1 s
Kopf unter dem Unterkiefer; Leon und Elza, bei Normalhelligkeit erkennbar); **M2** in der Belagerung mit Figur auf der Rampe (Cut 2) ist der
Alligator bei normaler Helligkeit nicht als Alligator zu erkennen, und er schwimmt nicht den geplanten Kreis R 2000, sondern dreht sich
auf einer Schleife R ~535 auf der Stelle (28-29 gleiche Runden in 70 s).

| Nutzerpunkt | Urteil | Beleg (kurz) |
|---|---|---|
| 1 Leiche im Maul, weder oben noch unten raus | **nicht bestanden (Rest)** | Halten quer im Maul ok; Clip 5 Bild 3-11 (F91-F99) Leiche oben auf Schaedel/Oberkiefer, Bild 74-76 Kopf unter dem Kiefer; `p1_*` |
| 2 hochwerfen, im Maul fangen, verschwinden | bestanden | Bild 57 Wurf, ~0,4 s ganz sichtbar ueber dem offenen Maul, Bild 75 Fang, Bild 77 weg; bei Normalhelligkeit lesbar; `p2_*` |
| 3 Montage: 5 Zombies, Richtung Leiter | bestanden | 5 Zombies (t=16), Bewegungsrichtung <= 3,4 Grad neben der Richtung zur Leiter, 2507-3830 naeher; `p3_*` |
| 4 Marvin/John: 3 Zombies, zur Leiter, schneller erschossen | bestanden | 3 Zombies, Gier bleibt zur Leiter, 31/47/65 Bilder gelaufen, Schuesse F39/F56/F73 = 2,4 s; `p4_*` |
| 5 Kampfmusik in Marvins Szene unveraendert | bestanden (Log) | `FF3C -> MAIN3C ... [unveraendert, laeuft durch]` in 2080/2081 und zurueck, kein `loaded` dazwischen |
| 6 Schluessel Anfang/Ende groesser | bestanden | Ankunft Schluessel ~24 x 21 Fensterpunkte (Abnahme 1 vorher 14 x 12), nach dem Tod ~33-35 Punkte; `p6_*` |
| 7 mit Kanister nicht haengen | **bestanden im freien Spiel / Mangel M2 in Cut 2** | frei Leon 273 s / Elza 259 s: 0 2-s-Fenster ohne Fortschritt, laengste 30-B.-Folge 2,9 / 2,0 s (Luftbisse); Rampe: kein Stillstand, aber Schleife R ~535 auf der Stelle, im Bild nicht erkennbar |
| 8 nach dem Wurf nicht direkt zum Kanister | bestanden | 6 Laeufe 0 x `ANSCHWIMMEN an`, erste 3-5 s Blick auf die Figur (Abw. 0..+-10 Grad), Aufnahme 4,7-26,0 s nach dem Aufschlag |
| Riegel der Spur (119) | gruen | `100% tests passed, 0 tests failed out of 119` (2291 s) - der Riegel maul laesst den M1-Rest zu (<= erreichbares Minimum + 4), kamera prueft fuer M2 den geplanten Kreis, nicht die Bahn |
| Nachbarraeume 2060-20B0 (+Elza), Raumkette | in Ordnung | 12 Ladelaeufe exit 0, 400 Bilder; Wiedereintritt 2090 nach dem R1-Tod ohne Gator, Tor gesperrt, Water Key je Besuch |

## 1. Bau, Messweg

- `bash re15_port/tools/local_build.sh build` im Spurbaum: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`; exe 2026-11-02 08:16:20,
  md5 84f886b1e1eddc0d3dc10e498f992bf7 (= exe_md5.txt jedes Laufs). Juengste Quelle gator_re2_r41.c 08:16, danach nur der Doku-Commit b78387a6c.
- Jeder Lauf: eigene exe-Kopie `re15_pc_g2abn3_<lauf>.exe` (nach dem Lauf geloescht), gdigrab auf das hwnd DIESES Prozesses (aus der eigenen
  Windows-PID), 30 Bilder/s, RE15_WINDOW_SCALE=2 (640 x 480), kein AUTOSHOT, kein SOFTWARE_RENDER, SDL_ASSERT=always_ignore, Ton an. Beendet nur
  eigene PIDs (kette_* per Zeitgrenze des eigenen Skripts). Skript `werkzeug/lauf.sh`, Stapel `werkzeug/batch1..3.sh`.
- Laeufe: `ank_leon`/`ank_elza` (Szene 2080 -> 2090, CRF 16), `kette_leon`/`kette_elza` (Kampf, Marvin/John, Aufnahme, Beschuss bis R1-Tod,
  Wiedereintritte), `frei_leon`/`frei_elza` (**eigene Eingabe**: `wand.py` Saat 53 / 91 statt 41 / 77 der Abnahme 2, 8150 Bilder, kein
  RUECK_KANISTER-Haken), `west_leon`/`west_elza` (Rueckkehr-Messhaken an **andere Stellen als der Bau**: 3000,-18800 bzw. 5600,-18700, Cut 2;
  geklettert bei x 2318 bzw. **x 4432**), `raum_20X0/20X1` (12 Ladelaeufe).
- Video <-> Spielbild: Schnittspitzen der Bilddifferenz (`werkzeug/cutfind.py`) gegen `cam=`-Wechsel im state.log, dazu die Gator-Zeile
  `[35 ... mo= af=]` je Bild. Beispiel ank_leon: Schnitt auf Cut 1 bei t 27,867 = F53; Wurf (Kopf geht hoch) t 31,13 = F145 Clip 5 Bild 57.
- Auswertung mit eigenen Skripten (`zomb.py`, `p7.py`, `p8.py`, `sicht.py`, `karte.py`); zur Gegenrechnung der Bau-Auswerter
  `test_r41_gatorfix traglog` auf meinen Laeufen (Ausgaben in `messung_abnahme3.txt`).

## 2. Die Punkte im Fenster

### Punkt 1 - Leiche im Maul (ank_leon, ank_elza)
Bildfolge 30 bzw. 15 Bilder/s um Biss, Rollfenster und Fang, aufgehellt zum Finden, Urteil am Normalbild:
- **Biss, offenes Maul** (Leon t 28,93-29,26, Clip 4 Ende/Clip 5 Anfang): Leiche liegt im weit offenen Maul, Kopf am Mundwinkel, Beine zwischen
  den Kiefern nach vorn - nichts ober- oder unterhalb (`p1_leon_biss_offenes_maul_15fps_hell.jpg`). Gegen Abnahme 2 (Kopf weit unter dem Kiefer,
  Beine senkrecht hoch, `G2_abnahme_2/h1_leon_biss_leiche_ueber_schaedel_30fps_hell.jpg`) deutlich besser.
- **Rollfenster** (Leon t 29,33-29,60 = 9 Videobilder; state.log F91-F99 `mo=5 af=3..11`, also Clip 5 Bild 3-11): die Leiche liegt OBEN auf dem
  Schaedel/Oberkiefer - Rumpf und Beine sind ueber dem Kopf zu sehen, die Fuesse hinter dem Nacken (`p1_leon_rollfenster_C5B3-11_leiche_ueber_schaedel_30fps_hell.jpg`
  Bild 3-10). Bei Normalhelligkeit erkennbar (`p1_leon_kernbilder_normalhelligkeit.png`, t 29,393 / 29,46: blau-graue Leiche quer ueber dem Kopf).
  Elza identisch, t 46,08-46,35 (`p1_elza_rollfenster_30fps_hell.jpg`, `p1_elza_kernbilder_normalhelligkeit.png` oben). Deckt sich mit dem Bau
  (Dossier NB4 "Clip 5 Bild 3-11 nicht 0, bis 24", Bild 6-9 Minimum 24 von 306) - die Zahl ist im Bild sichtbar.
- **Halten** (Clip 5 ab Bild ~12 bis 56, Leon t 29,63-31,06): quer im Maul, Kopf der Leiche am Mundwinkel auf Kieferhoehe, Rumpf zwischen den
  Kiefern, nichts ueber dem Schaedel oder unter dem Kinn (`p1_leon_kernbilder_normalhelligkeit.png` t 29,797). In Ordnung.
- **Fang** (Clip 5 Bild 74-76 = F162-F164, Leon t 31,69-31,76, 3 Videobilder): der Kopf der Leiche haengt UNTER dem Unterkiefer, die Beine
  ragen seitlich aus dem Maul (`p1p2_leon_fang_C5B74-77_kopf_unter_kiefer_30fps_hell.jpg` Bild 5-7; normal: `p1_leon_kernbilder_normalhelligkeit.png`
  t 31,693/31,727). Elza 1-2 Videobilder (t 48,48-48,52, `p1_elza_kernbilder_normalhelligkeit.png` unten links, `p1p2_elza_wurf_fang_30fps_hell.jpg` Bild 7-8). Bau: "Fangbilder 75/76 bei 12 und 23".
- **Urteil: nicht bestanden (Rest).** Nutzerwortlaut "im Maul ... ohne oben oder unten raus zu clippen": oben ~0,3 s je Biss, unten ~0,1 s je Fang.

### Punkt 2 - Wurf, Fang, Verschwinden (ank_leon, ank_elza)
`Folge W F145 wert=57`, `F F163 wert=75`, `V F165 wert=77` (Leon und Elza gleich). Fenster Leon (`p2_leon_halt_wurf_fang_weg_15fps_normal.jpg`, hell
`..._hell.jpg`): t 31,13 Kopf reisst hoch, t 31,20-31,60 Leiche fliegt ganz sichtbar ueber dem weit offenen Maul (6 Bilder bei 15/s), t 31,66 faellt
hinein, t 31,73 Maul schliesst, t 31,80 weg, Kauen. Schnauze und Leiche nicht hinter dem Balken. Elza: `p1p2_elza_wurf_fang_30fps_hell.jpg`, t 48,28-48,55.
**Bestanden** (Rest im Fangbild siehe Punkt 1).

### Punkt 3 - Montage: 5 Zombies, Richtung Leiter (ank_leon, ank_elza)
`zomb.py` (Leiter-Ankunft (-9950,-25700)): 5 Zombies (Slot 1-5), alle ab F8 in Bewegung, Bewegungsrichtung 71,3/86,0/76,6/82,2/93,7 Grad gegen
Richtung zur Leiter 72,4/84,8/74,5/82,6/90,3 Grad, 2507-3830 naeher in 156 Bildern; Leon und Elza bitgleich. Fenster: 5 Zombies gehen von Marvin
weg nach links unten (`p3_leon_montage_5zombies.jpg`, `p3_elza_montage_5zombies.jpg`). **Bestanden.**

### Punkt 4 - Marvin/John: 3 Zombies, zur Leiter, schneller (kette_leon, kette_elza)
`ROOM2080 SZENE (Marvin 0x40, 3 lebende Zombies` / `ROOM2081 SZENE (John 0x49, 3 lebende Zombies`; `Schuss 1/2/3: Treffer` F39/F56/F73
(= 1,3 / 1,9 / 2,4 s nach Szenenstart; Dossier vorher F118 ... F351 = 11,7 s). `zomb.py`: Gier 847/939/1029 -> 837/935/1021 (= Richtung Leiter
74,5/82,6/90,3 Grad, nie zu Marvin), gelaufen 31/47/65 Bilder, Bahn 455/1367/1221, 425/1168/1150 naeher; Zombie 1 laeuft nur 455 (Bewegungsrichtung 92
gegen 74,5 Grad). Fenster: die drei gehen mit dem Ruecken zu Marvin/John nach unten, Schuesse t 20,3/20,9/21,5 (`p4_leon_marvin_3zombies_5fps.jpg`,
`p4_elza_john_3zombies_5fps.jpg`). **Bestanden.**

### Punkt 5 - Musik (alle Kampflaeufe)
`[bgm] stage=1 room=08 entry=FF3C -> MAIN3C ... [unveraendert, laeuft durch]` beim Wechsel in die Szene und `room=09 ... FF3C [unveraendert, laeuft
durch]` zurueck, kein `[bgm] loaded` dazwischen (kette_leon debug.log Z. 269/437, kette_elza Z. 281/456, frei_leon, frei_elza). **Bestanden nach Log**;
gehoert habe ich es nicht.

### Punkt 6 - Schluessel groesser
Ankunft (ank_leon t 26,8, Cut 0): Ring ~15 x 21, mit Bart ~24 x 21 Fensterpunkte (`p6_leon_schluessel_ankunft_zoom8x.png`; Abnahme 1 vorher 14 x 12).
Nach dem R1-Tod: Leon `Water Key ... bei der Leiche (1100,-1749,-9600) (Kamera 3)`, Ring ~35 x 32 Punkte im Wasser rechts unten
(`p6_leon_schluessel_nach_tod_t67.png`); Elza `(-7000,-1749,-23100) (Kamera 7)`, ~33 Punkte vor Elza (`p6_elza_schluessel_nach_tod_1fps.jpg`). **Bestanden.**

### Punkt 7 - mit dem Kanister nicht haengen
**(b) Freies Spiel, nach Marvins Wurf** (frei_leon / frei_elza, eigene Eingabe, Figur steht 6-18 s an Waenden/Ecken, laeuft Waende ab):
| Lauf | Trag-Zeit | 2-s-Fenster ohne Fortschritt | 30-B.-Fenster netto < 300 (am Spieler / fern) | laengste Folge | Bahn / Umlauf (Summe Winkel) | Bau-traglog |
|---|---|---|---|---|---|---|
| frei_leon | 273 s (F209-F8400) | **0** | 10,1 % (4,4 / 5,7 %) | 2,9 s = Biss-Clip 4 + Clip 0, Kopf 2700 vom Spieler | 1,04 Mio / 18,0 Runden | 0 Fenster, laengste 40, Wandreiben 3 |
| frei_elza | 259 s (F645-F8400) | **0** | 11,4 % (5,6 / 5,8 %) | 2,0 s = Clip 0/4, Kopf 3600 vom Spieler | 1,02 Mio / 20,0 Runden | 0 Fenster |
Karten mit Lage ueber die Zeit: `p7_leon_frei_karte_trag.png`, `p7_elza_frei_karte_trag.png` (rot = 30-B.-Stillstand; nur Biss-Enden, keine Wand).
Im Fenster (Cut 0, Leon an der Leiter): der Alligator kreist mit dem Kanister sichtbar vor Leon, kommt heran, schnappt (`p7_leon_frei_kreisen_cut0_normal_2s.jpg`).
Ist die Figur fuer den Kopf mit Kanister unerreichbar (Elza in der Ost-Nische), pendelt er 120 s zwischen West- und Ostende des Suedbeckens
(Periode ~5 s) und beisst am Ostende in die Luft (Kopf 3600-4100 von Elza) - in Bewegung, kein Haengen.
**(b) Rampe** (west_leon x 2318, west_elza x 4432, Cut 2): 0 Stillstand, 0 Fenster (p7.py und Bau-traglog). **Haengen: behoben.**

**(c) Cut 2, Normalhelligkeit - Mangel M2.** Bau-traglog auf meinen Laeufen: "Gator im Bild der Spielerkamera 95 %" (Leon) / 93 % (Elza). Im Fenster:
- Vollbild normal (`p7c_leon_cut2_normal_4s.jpg`, `p7c_elza_cut2_normal_4s.jpg`, alle 4 s): Figur auf der Rampe vor der Maschine, untere Bildhaelfte
  schwarz; ein Alligator ist in keinem der 16 Bilder je Lauf als solcher zu erkennen. Bester Moment (`p7c_cut2_vollbild_normal_beste_momente_leon116_elza96.png`):
  Leon - ein rosa Fleck an Leons Fuessen (das offene Maul), Elza - eine dunkle Kruemmung links von ihr an der Rampenkante.
- 2-fach vergroessert (`p7c_*_cut2_ausschnitt2x_normal_hell.png`): der Alligator liegt im schmalen Wasserstreifen hinter der Rampe, zu einem
  grossen Teil von den Beinen der Figur verdeckt, erst vergroessert erkennbar.
- Gemessen (`sicht.py`: Pixel mit Luma-Aenderung > 12 in 0,2 s, 320 x 216 ohne Balken, je Sekunde): Cut 2 Median **286** (Leon) / **482** (Elza) von
  69120 (0,4 / 0,7 %), mittlere Luma der bewegten Pixel 13-31 von 255. Vergleich Cut 0 im freien Spiel: Median 8833 (12,8 %).
- **Bahn statt Kreis**: geplant `TRAG KREISEN an F919 (Belagerung) ... -> Mittelpunkt (3450,-9150) R 2000`; gefahren (gator_trag.log F1200-F3300):
  Schleife um (2232,-10500), Radius 460-632 (Leon) / 415-687 (Elza), Mittel ~535, Periode 2,4-2,5 s, **28,7 / 28,4 Runden** an derselben Stelle,
  Tempo 47 je Bild, Phase 5 Clip 0, Abstand zur Figur 5700-7600. Um den geplanten Mittelpunkt ueberstreicht die Wurzel nur 34-56 Grad
  (Abstand 1130-2780), faehrt den R-2000-Kreis also nie. Karten `p7c_leon_rampe_karte.png`, `p7c_elza_rampe_karte.png`. Beide Kletterlagen
  (x 2318 und x 4432) landen auf demselben Kreisel.

### Punkt 8 - nach dem Wurf nicht direkt zum Kanister
`p8.py` (je 1 s Abstand und Blickabweichung zu Figur und Kanister, Aufschlag F43 bis `KANISTER angehaengt`):
| Lauf | erste Sekunden | `RUNDE zum Kanister an` | Aufnahme nach Aufschlag | ANSCHWIMMEN |
|---|---|---|---|---|
| frei_leon | Blick zur Figur -7..+10 Grad, Figur steht nahe am Kanister | - | 4,7 s (vorbeigeschwommen) | 0 |
| frei_elza | beisst Elza (F43-F133), folgt ihr 15000 vom Kanister weg | F500 "liegt voraus" | 19,2 s | 0 |
| west_leon / west_elza | Verfolgung | F532 / F492 "liegt voraus" | 21,2 / 19,6 s | 0 |
| kette_leon / kette_elza | Blick zur Figur 0..-10 Grad, Biss F73 | F642 "spaet (liegt hinten)" | 26,0 s | 0 |
**Bestanden.** Hinweis: die Port-Wahl E1 (NB1, in Abnahme 2 angenommen) macht den Kanister nach 450-600 Bildern zum Wegpunkt; in den kette-Laeufen
dreht er dafuer nach 20 s um ("liegt hinten") - das ist nicht woertlich "erst wenn er beim Kanister ankommt", war aber die Antwort auf "Kampf kann stocken".

## 3. Nichts anderes kaputt

- **Riegel der Spur**: PowerShell, `C:\msys64\mingw64\bin` vorn im PATH (gcc `C:\msys64\mingw64\bin\gcc.exe`), SDL_ASSERT=always_ignore,
  `ctest --test-dir re15_port/build -R "r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix" -j3 --timeout 1500 --output-on-failure`
  (09:08:55-09:47:07, neben meinen Fensterlaeufen): **`100% tests passed, 0 tests failed out of 119`**, `Total Test time (real) = 2291.19 sec`
  (darin die NB4-Riegel unit_r41_gatorfix_kamera/_elza, maul/_elza, die exe-Riegel frei/rampe/abn2_*; kein Lauf musste wiederholt werden).
- **Nachbarraeume** (`batch3.sh`, RE15_DEBUG_JUMP, 400 Bilder, ohne Video): 2060/2061 ENGINE ROOM, 2070/2071 TUN. FLOOD DRY, 2080/2081 CONTROL ROOM,
  2090/2091 ALLIGATOR POOL, 20A0/20A1 SERVICE TUNNEL, 20B0/20B1 L TUNNEL FLOOD: alle `exit=0`, `[room] PC loaded`, F400 erreicht, keine Zeile
  error/assert/abort im debug.log.
- **Raumkette** (kette_leon/kette_elza): Szene -> Rueckkehr -> Aufnahme F822 -> Beschuss -> `F972 Kanister verbraucht (R1-Tod)` -> Water Key ->
  Leiter hoch (`DOOR FIRE slot=0`), 2080, Leiter zurueck (`slot=1`): 3 (Leon) bzw. 2 (Elza) Wiedereintritte 2090/2091 ohne Gator-Zeile (Slot 35) im
  state.log, `Slot 1 (Tor) gesperrt` und Water-Key-Zeile je Besuch (kette_leon debug.log Z. 937/1125/1313), `MAIN2E [unveraendert, laeuft durch]`.
- Code gelesen (nicht gemessen): der neue Zeichner-Haken in `platform/pc/main.c` (pc_prop_world) staucht jeden Prop mit `obj_id == 2`
  (`prop_slot(RE15_GSZ_LEICHE_OBJ)`), solange `s_fit.ks != 0`; `s_fit` wird nur bei `ga_start` und Szenenstart geleert (gatorszene_r41.c:1030, :2330),
  nicht beim Raumwechsel. Im gemessenen Ablauf ist ks vor dem Wurf 0 (Bau-Riegel); wirksam waere es nur nach einem Raumwechsel mitten im Halten.

## 4. Maengel (mit Wiederholungsweg)

**M1 (Punkt 1, mittel): Leiche ragt beim Zubeissen oben und beim Fang unten aus dem Maul.**
- Rollfenster Clip 5 Bild 3-11 (~0,3 s): Rumpf und Beine liegen sichtbar ueber Schaedel/Oberkiefer. Fang Clip 5 Bild 74-76 (~0,1 s): Kopf der
  Leiche unter dem Unterkiefer. Leon und Elza, bei Normalhelligkeit erkennbar.
- Wiederholen: `bash werkzeug/lauf.sh ank_leon 75 RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245
  RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=700#2090 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2` (Elza: 2081/2091 und
  RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 statt TITLE_SHOT). Im letzten 2090-Besuch Schnitt auf Cut 1 = F53;
  Rollfenster F91-F99, Fang F162-F164 (state.log `[35 ... mo=5 af=..]`); Video Leon t 29,33-29,60 und 31,69-31,76.
- Der Bau nennt die Ursache selbst (Kopfrollen, Ziel 0 im erlaubten Raum nicht erreichbar, Fangbilder ohne Einpassung) - offen ist die Entscheidung,
  wie das Bild ohne Herausragen aussehen soll.

**M2 (Punkt 7c, mittel): Belagerung auf der Rampe (Cut 2) - Alligator nicht erkennbar, dreht sich auf der Stelle.**
- Kreispunkt im Nordbecken hinter der Figur: im Bild nur ein dunkler, von der Figur teils verdeckter Fleck (bewegte Flaeche 0,4-0,7 % des Bildes,
  Luma 13-31; Cut 0 im freien Spiel 12,8 %). Der Bau-Wert "95 % im Bild" misst die Projektion, nicht die Erkennbarkeit.
- Die Wurzel faehrt nicht den geplanten Kreis (3450,-9150) R 2000, sondern eine Schleife R ~535 um (2232,-10500), 2,4 s je Runde, 28-29 Runden in 70 s,
  47 je Bild - auch von zwei verschiedenen Kletterlagen aus dieselbe Stelle. Der Riegel unit_r41_gatorfix_kamera prueft den geplanten Kreis (16/16 im Bild),
  nicht die gefahrene Bahn.
- Wiederholen: `bash werkzeug/lauf.sh west_leon 125` mit der Umgebung aus `runs/west_leon/env.txt` (RE15_DEBUG_JUMP=2090@250, RE15_SET_FLAG=9:110,3:245,
  RE15_GIVE=3:1, RE15_EQUIP=3, RE15_K2080_GATOR_HP=24990, RE15_R39_MESS_SPIELER_HP=100, RE15_PLAYER_POS=-6000,-17500,3072,
  RE15_K2080_RUECK_LAGE=3000,-18800,3072,2, RE15_G2_TRAG_LOG=1, Kletterfolge `U0.1,A0.2,W0.3,D0.1,W0.3` x 40 nach `W6.0,M0.6,MA0.1,M0.3,MA0.1,W14.0`,
  RE15_EXIT_AT=3300#2090); Elza mit 5600,-18700. Auswerten: `python werkzeug/p7.py runs/west_leon`, `python werkzeug/karte.py runs/west_leon k.png`,
  `python werkzeug/sicht.py runs/west_leon/video.mp4 57 124`.

## 5. Nicht geprueft

PSX-Bau (Einpassung nur PC; Kreispunkt-Suche laeuft auch auf PSX, Laufzeit nicht gemessen), volle Suite ausserhalb der fuenf Gruppen, Hoeren (Musik nur
per Audio-Log), Aufheben des Water Key im Fenster (nur Riegel), Rampen-Nordkante (laut Dossier im exe-Lauf nicht kletterbar), Raumwechsel mitten in der
Haltephase.
