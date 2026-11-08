# Runde 41 - Spur G2 "gatorfix" - UNABHAENGIGE ABNAHME 2 (2026-11-02)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gatorfix`, HEAD e7d0510c7 (Nachbesserung 1: 49457eb0f + Doku e7d0510c7). Pruefer: Abnahme-Agent,
glaubt dem Bau nichts. Kein Code geaendert. Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G2abn2`
(je Lauf `runs/<name>/` mit video.mp4, debug.log, state.log, gator_boss.log, gator_trag.log, climb.log, env.txt, exe_md5.txt). Belegbilder: `G2_abnahme_2/`.

## 0. Urteil

**NICHT BESTANDEN.** Der Kern von M1 ist behoben (Spieler im offenen Wasser / an der Rampen-Suedkante / an der Lage der Nutzer-Marke: 0 % Stillstand, vorher
153,5 s Festdruecken), aber mit dem Kanister im Maul entstehen an Stellen, die der Kopf mit Kanister nicht erreicht, deterministische Schleifen ohne Fortschritt:
**M3 nicht behoben** (Spieler auf dem Westteil der Rampe: Leon und Elza je 94 % Stillstand ueber 4 min, 26/27 gleiche GUARD-Zyklen), **M1 teilweise**
(Leiter-Ecke SW, NW-Ecke, Ost-Nische, NO-Ecke: Schleifen mit 57-80 % Dash-Clips bzw. 55 % Pendel-Stopps; einmal 2,2 s Wandreiben). Die Messgroesse
der Bau-Riegel (`test_r41_gatorfix traglog`) sieht die GUARD-Schleife nicht. M2 behoben (lesbar), H2/E1/Musik/Raumkette in Ordnung, Riegel 110/110 gruen,
die geaenderten fremden Riegel re2gator sitz/schleppen sind sachlich richtig.

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| M1 Haengen mit Kanister, freies Spiel | **teilweise** | offenes Wasser 0 % (plat2_leon, plat7_elza); Ecken: Leiter SW 20 gleiche 12-s-Schleifen mit je 3,9 s Dash-Stillstand + 3 kurzen Stopps, NW-Ecke/Ost-Nische Dash-Schleife (57 % Dash), Nordbecken/NO-Ecke 55 % Pendel-Stopps; Suedwand 2,2 s Wandreiben |
| M2 Wurf/Fang/Verschwinden | erfuellt | 15 Bilder/s: Kopf hoch, Leiche ~6 Bilder ueber dem offenen Maul, faellt hinein, Maul schnappt zu, weg (Leon und Elza gleich) |
| M3 Plattform/Rampe, dort bleiben | **nicht erfuellt** | Rampe x 2318 (Leon und Elza): GUARD-Schleife 8,9 s, davon ~4,3 s regungslos (Takt 48/Bild, nach den Klemmen 0) an (1673,-10966), 26 bzw. 27 x identisch, 94 % der Trag-Zeit; Rampe Ostteil x 6500: patrouilliert |
| H1 Biss ohne Clipping | Rest | ~10 Bilder (Clip 5 Bild ~1-11) liegt die Leiche sichtbar quer ueber Oberkiefer/Hals, Beine ragen nach oben (Leon und Elza) |
| H2 Marvin-Szene | erfuellt | Zombies laufen 31/47/65 Bilder (456/1367/1221 Einheiten) zur Leiter, Schuesse F39/F56/F73 (2,4 s), im Bild sichtbar |
| E1 Aufnahme nach rund 30 s | erfuellt | 4,0-29,2 s nach dem Aufschlag in 9 Laeufen, 0 x `ANSCHWIMMEN` |
| Musik | erfuellt | `FF3C -> MAIN3C ... [unveraendert, laeuft durch]` in 2080/2081 und zurueck |
| Raumkette, Wiedereintritt | erfuellt | R1-Tod durch Beschuss F1051, Water Key bei der Leiche, 5 Wiedereintritte ohne Gator, Tor gesperrt, MAIN2E laeuft durch |
| Riegel r41_gatorszene/kanister2080/alligator2/re2gator/gatorfix | gruen | `100% tests passed, 0 tests failed out of 110` (1763 s) |
| re2gator sitz/schleppen (Nachzug) | sachlich richtig | alter Riegel gegen neue Engine: toedlicher Biss F51-53 (hp 100->50->0->-50), FRESSEN, Kanister -1 = RE2 @0x8010100c; Sitz bis dahin 0 Abweichung |

## 1. Bau, Messweg

- `bash re15_port/tools/local_build.sh build` im Spurbaum: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`; exe 2026-11-02 00:46:05, md5
  85659e46bbe32a7c7df066d665b6d957; juengste Quelle enemy_ai_boss_gator.c 00:46:02, Arbeitsbaum ohne Aenderung an verfolgten Dateien.
- Vorher-exe selbst gebaut: `git archive 789f6e681 re15_port` nach `<SP>/alt`, `RE15_TESTS=OFF bash re15_port/tools/local_build.sh configure` + `build`
  (`LOCAL-BUILD-OK`), md5 a8814d437acb7d730e42c212363b94e9 (die Vorher-exe hat die Messzeile RE15_G2_TRAG_LOG und den Messhaken RE15_K2080_RUECK_LAGE nicht).
- Jeder Lauf: eigene exe-Kopie `re15_pc_g2abn2_<lauf>.exe` (nach dem Lauf geloescht), gdigrab auf das hwnd DIESES Prozesses (aus der eigenen Windows-PID),
  30 Bilder/s, RE15_WINDOW_SCALE=2, kein AUTOSHOT, kein SOFTWARE_RENDER, SDL_ASSERT=always_ignore, Ton an; beendet nur eigene PIDs (`kill` der eigenen
  msys-PID bei plat3/plat4/plat5/plat6 (Ausloesung bzw. Kletterweg verfehlt) und kette_leon (Leiter-Pendel)). Helfer `<SP>/lauf.sh`. Zwei Vorher-Laeufe
  (alt_w_leon, alt_rampe_nat_leon) ohne Video (hwnd 0), nur Logs.
- **Eigene Eingaben** (nicht walk.py Saat 1/2 der Abnahme 1 / der Bau-Riegel): `<SP>/wand.py` "Wand- und Ecken-Tour" (rennen bis an die Wand, 6-18 s stehen,
  an der Wand entlang in die Ecke, Zappeln, Rueckwaertsschritte; Saat 41 Leon, 77 Elza); `ecken_leon.txt` (gezielte Ecken-Tour); Rampe: Messhaken
  RE15_K2080_RUECK_LAGE an eine andere Stelle als der Bau (Leon 4200,-18700 / Elza 2500,-18700 statt 3000,-18800) + eigene Kletterfolge
  `U0.1,A0.2,W0.3,D0.1,W0.3` x40-60 (netto kein Weg), dann 200 s stehen; `wand41_klettern.txt` = w_leon-Eingabe bis F5800, danach Dreh-/Aktions-Pulse
  (Rampe ohne Messhaken, echte Kamera).
- Messgroessen (`<SP>/epis.py`, `<SP>/still2.py`, Mass der Abnahme 1): Stillstand = Wurzel bewegt sich in 30 Bildern netto < 300. Klassen: "am Spieler"
  (Kopf = Wurzel + 4200 voraus <= 2500 vom Spieler), "Dash" (Clip 2/3, RE2-Zonen-Dash), "fern" (sonst). Wandreiben = Takt-Schritt gross, nach den Klemmen
  ~0 mit `KOPF-KLEMME an`. Gegenrechnung jedes Laufs mit dem Auswerter des Baus `test_r41_gatorfix.exe traglog <lauf>`. Meldungsboxen (msg a=1) in keinem
  der Schleifen-Laeufe (state.log `msg(a=0` in allen Bildern).

## 2. M1 - freies Spiel mit Kanister im Maul

| Lauf (G2-exe, eigene Eingabe) | Spieler-Lage | Trag-Zeit | Stillstand | davon am Spieler / Dash / fern | laengste Folge | Bau-traglog (fern o. Biss / laengste / Wandreiben) |
|---|---|---|---|---|---|---|
| w_leon (wand41) | Waende/Ecken Sued+Ost, Rampen-Suedkante | 255 s | 17 % | 7 / 4 / 6 % | fern 1,3 s; Dash 3,9 s | 5 % / 39 / 0 |
| w_elza (wand77) | Ost-Nische (7668,-16257) 110 s, NW-Ecke 80 s | 275 s | 34 % | 1 / **32** / 2 % (Dash-Clips 57 %) | Dash 4,0 s (32 Folgen) | 1 % / 32 / 15 |
| ecken_leon | Westwand, Leiter, Sued-/Ostwand | 223 s | 45 % | 15 / 11 / 19 % | **Wandreiben 2,2 s** Suedwand | 18 % / **77** / **75** |
| rampe_nat_leon (Leiter-Ecke SW) | (-8437,-26507) 230 s | 236 s | 60 % | 1 / **55** / 5 % (Dash-Clips 80 %) | Dash 3,9 s (56 Folgen) | 4 % / 31 / 1 |
| plat_leon (Rueckkehr Rampen-Nordkante, Biss traegt ihn ins Nordbecken) | NO-Ecke/Ostwand | 226 s | 62 % | 7 / 0 / **55** % | 4,5 s (75 Folgen) | 55 % / **134** / 7 |
| plat2_leon (Rampen-Suedkante, Wasser) | (2318..4600,-18568..-24600) | 199 s | **0 %** | - | - | 0 / 0 / 0 |
| plat7_elza (Rampen-Suedkante x 5800, Wasser) | (2577,-19382) | 247 s | **0 %** | - | - | 0 / 0 / 0 |
| w_leon_klettern (Rampe Ostteil, echte Kamera) | Rampe (6463..6655,-15865..-16339) | 255 s | 19 % | 4 / 4 / 11 % | Ende 9,0 s = Meldungsbox id 42 (eigene A-Taste), kein Gator-Befund | 10 % / 270 / 6 |
| **Vorher** alt_w_leon (wand41) | wie oben (Weg weicht ab) | 275 s | 56 % | 32 / 4 / 20 % | **11,5 s** (2663,-22605) | - |
| **Vorher** alt_w_elza (wand77) | blieb im Suedbecken | 275 s | 71 % | 54 / 5 / 12 % | 1,7 s | - |
| **Vorher** alt_rampe_nat_leon | Rampen-Suedkante (5768,-18568) | 262 s | 84 % | 15 / 10 / **60** % | **153,5 s** (2873,-22595) = Nutzer-Marke | - |

Gleiche Eingabe, aber die Wege laufen auseinander (Bisse, andere Aufnahmezeit) - Vergleich, kein A/B im selben Weg.

**Behoben:** Spieler im offenen Wasser, an der Rampen-Suedkante und an der Lage der Nutzer-Marke: 0 % Stillstand ueber 199/247 s (plat2_leon, plat7_elza);
Vorher an genau dieser Lage 153,5 s Festdruecken (alt_rampe_nat_leon F3396-F8000, Gator (2873,-22595), Spieler (5768,-18568)). Wand-Reiben nur noch einmal.

**Nicht behoben - Schleifen an Stellen, die der Kopf mit Kanister nicht erreicht** (alle ohne Meldungsbox, Gator-Takt laeuft):
- **Leiter-Ecke SW (rampe_nat_leon, Leon):** Ziel = Ring-Wegpunkt (-7300,-25400) -> `gekuerzt auf (-3555,-21579)` -> erreicht -> `TRAG VORBEI ... Etappe laengs z
  nach (-4206,-17843)` (18 x wortgleich) -> ueber die Zonenlinie -> RE2-Zonen-Dash Clip 2/3 (`DASH Vordrehung` 21 x) mit ~3,9 s an (-4250,-20277) -> CHASE
  zurueck. Periode 364 Bilder, 20 Zyklen (F1196-F7865), je Zyklus 3,9 s Dash + drei Stopps 1,0-1,3 s, Leon kommt nie dran. Im Fenster (Cut 0, Blick nach Westen): der Gator liegt mit dem Kanister zwischen Kamera und Leon (Wurzel ~7500, Kopf ~6600 von Leon) im Bild vor der Leiter,
  schnappt auf der Stelle, schwimmt eine Runde, kommt zurueck - `m1_leon_leiter_schleife_F2288-F2656_2fps_hell.jpg`, `m1_leon_leiter_dash_3,9s_3fps_hell.jpg`,
  Karte `m1_karte_leon_leiter_rampe_nat.jpg` (Vorher: `m1_karte_VORHER_leon_leiter_alt.jpg`).
- **Ost-Nische / NW-Ecke (w_elza, Elza):** dasselbe Muster - `TRAG VORBEI ... Ziel (-4300,-22600) ... Etappe von der Wand weg` bzw. `Ziel (-7300,-7000) gekuerzt auf
  (-3677,-11118) ... Etappe laengs z`, danach Zonen-Dash; Periode ~430-440 Bilder, rund 14 Zyklen (F2591-F8400), 57 % der Trag-Zeit in Clip 2/3, nur 1 % am Spieler.
  Im Fenster schwimmt der Gator mit Kanister vor Elza in der NW-Ecke hin und her, oeffnet das Maul, beisst nie (`m1_elza_nw_ecke_pendel_0,5fps.jpg`).
- **Nordbecken / NO-Ecke (plat_leon):** 75 Pendel-Stopps zu je 1,3-4,5 s (Clip 0, Takt 18-23/Bild, netto < 300/s) an der Nordkante der Huelle
  (x -2200..3600, z -9800..-11000), Leon 5000-9000 entfernt; 55 % der Trag-Zeit (Bau-traglog: 55 %, laengste 134 Bilder). Kamera bleibt mit dem Messhaken
  auf Cut 0 (s. Abschnitt 4), im Fenster nicht sichtbar.
- **Wandreiben (ecken_leon F6624-F6690, 2,2 s):** Gier 1021 (Sued), Takt -200/Bild, nach den Klemmen ~0, `KOPF-KLEMME an ... Ueberstand (0,183..207)` je
  Paddelzyklus (F6626/F6652/F6678) an der Suedwand bei (-3251,-22780); Leon an der Ostwand (6732,-18572). Ausserhalb des Kamerabilds (Cut 2,
  `m1_leon_suedwand_F6614-F6690_ausserhalb_bild_3fps.jpg`). Bau-traglog: `laengstes Wandreiben 75 Bilder` (> 30 = Schwelle des eigenen Riegels).
- Kein Haenger: w_leon F5801-F7285 sieht nach Stillstand aus (10 x 80 Bilder), ist aber Biss -> Runde -> Biss am Spieler an der Rampen-Suedkante (pst/mo des
  Spielers wechseln je Biss).

**Messgroesse des Baus:** Die Bau-Riegel (integration_r41_gatorfix_frei/_rampe: Anteil <= 13 %, laengste <= 60, Wandreiben <= 30) fielen auf ecken_leon (18 %/77/75)
und plat_leon (55 %/134) durch, bestehen aber rampe_nat_leon (60 % Stillstand, 55 % Dash) und die GUARD-Schleife (94 %, Abschnitt 4) - Dash-Clips sind
ausgenommen, und "Kopf < 2500 vom Spieler" zaehlt als Biss-Reichweite, auch wenn der Spieler erhoeht und unerreichbar steht.

## 3. M2 - Wurf, Fang, Verschwinden (ank_leon, ank_elza; 30 Bilder/s, CRF 16)

Log `Folge W F145 wert=57`, `F F163 wert=75`, `V F165 wert=77` (Leon und Elza gleich). Im Fenster (Cut 1, aufgehellt fuer die Bildfolge):
`m2_leon_wurf_fang_weg_15fps_hell.jpg` - Bild 1 Leiche quer im Maul, 2 Kopf reisst hoch, 3-8 Leiche fliegt ueber dem weit offenen Maul (ganz sichtbar,
Schnauze nicht hinter dem Balken), 9-10 faellt ins Maul, 11-12 Maul schliesst sich darum, 13 weg, Kauen. Ungefiltert dunkel, aber lesbar
(`m2_leon_wurf_fang_4fps_originalhelligkeit.jpg`). Elza: `m2_h1_elza_biss_halt_wurf_5fps_hell.jpg` Reihe 3-4. **Erfuellt** (M2 der Abnahme 1 behoben).

## 4. M3 - auf der Rampe bleiben

- **Rampe Westteil x 2318 (plat7_leon, plat8_elza):** Rueckkehr per Messhaken an die Suedkante (4200,-18700 bzw. 2500,-18700), Bisse druecken die Figur an
  (2318,-18568), Klettern `sub=9` (T1511 bzw. T1421), danach steht die Figur auf der Rampe (2318,-1800,-16685..-17045), Band 1. Der Gator geht in GUARD (Phase 5)
  und bleibt dort 243 s bzw. 248 s: je Zyklus `TRAG VORBEI ... Hop (4450,-9850) gekuerzt auf (2973,-10605) Seiten 2 -> Etappe zum Wegpunkt nach dem Hop
  (1673,-11900), Endziel (5950,-11900)` (23+2 bzw. 24+2 x fast wortgleich), dann steht er F4330-F4460 (130 Bilder) bei (1673,-10966) Gier 1024 mit
  `takt=(0,-48) nach_klemmen=(0,0)` - er paddelt gegen die Plattform-Nordkante, der Wegpunkt (1673,-11900) liegt 934 dahinter -, bis die Etappe nach
  150 Bildern ablaeuft, und von vorn. Periode 266-270 Bilder, 26/27 Zyklen, **Stillstand 94,7 % / 94,6 %**. Bau-traglog dazu: "fern ohne Biss-Reichweite 20 % /
  18 %, laengster 31/30 Bilder, Wandreiben 6" - der eigene Rampen-Riegel wuerde bestehen. Karten `m3_karte_leon_rampe_x2318.jpg`, `m3_karte_elza_rampe_x2318.jpg`.
- **Rampe Ostteil x 6500 (w_leon_klettern, ohne Messhaken, echte Kamera Cut 2):** Kletterweg ueber die Ost-Nische, GUARD 1105 Bilder: Pendel im Suedbecken
  (1314,-22391) <-> (-2407,-21831), kein Stillstand >= 1 s im GUARD-Abschnitt. Dort **patrouilliert** er.
- **Fenster:** Mit dem Messhaken RE15_K2080_RUECK_LAGE bleibt die Kamera den ganzen Besuch auf Cut 0 (state.log plat7_leon: `cam=0` 7943 Bilder; Leon am
  oberen Bildrand `screen=(616.9,38.1)`), der Gator im Nordbecken ist nicht im Bild (`m3_leon_rampe_fenster_cut0_0,5fps.jpg`). Dasselbe gilt fuer den
  M3-Fensterbeleg des Baus (`G2_belege/nb1_m3_nachher_rampe_leon_10s.jpg` zeigt Cut 0 ohne Leon; n4_plat_leon `cam=0` 3543 Bilder). Die GUARD-Schleife ist
  deshalb nur im Log belegt; der Weg ohne Messhaken auf den Westteil gelang mir nicht (rampe_nat_leon: Biss schon vor F170, Figur in der SW-Ecke).

## 5. Hinweise

- **H1-Rest (Biss):** Die Halte-Phase sitzt quer im Maul (`h1_leon_biss_halt_5fps_hell.jpg`), aber ~10 Bilder (Fenster t 30,15-30,5 s ~ F89-F99 =
  Clip 5 Bild 1-11, nach dem Koppelbild F87) liegt die Leiche sichtbar quer ueber Oberkiefer/Hals, Beine ragen nach oben
  (`h1_leon_biss_leiche_ueber_schaedel_30fps_hell.jpg` Reihe 2 Bild 3 bis Reihe 3 Bild 4; Elza `m2_h1_elza_biss_halt_wurf_5fps_hell.jpg` Reihe 2 Bild 1).
  Der Riegel unit_r41_gatorfix_maul meldet fuer dieses Intervall 0 Scheitel ausserhalb - ob Kopf-Band (Part-6-Rahmen) oder Zeichenreihenfolge, habe ich
  nicht bestimmt. Nutzerwortlaut "ohne Clipping oben/unten".
- **H2 Marvin/John:** state.log der Szene (Leon/Elza bitgleich): Slot 1/2/3 bewegen sich 31/47/65 Bilder, Weg 456/1367/1221, Leiter-Abstand 10275->9849,
  10487->9318, 11000->9850; Schuesse F39/F56/F73. Im Fenster laufen die drei sichtbar an Marvin vorbei nach links (`h2_leon_marvin_3zombies_8fps.jpg`).
- **E1:** Aufnahme nach dem Aufschlag (F43): 711 / 121 / 754 / 877 / 477 / 590 / 654 / 560 / 600 Bilder (w_leon, w_elza, ecken_leon, rampe_nat_leon, plat_leon,
  plat2_leon, plat7_leon, plat7_elza, plat8_elza) = 4,0-29,2 s; `RUNDE zum Kanister an` bei 450-600 Bildern; 0 x `ANSCHWIMMEN an`.
- **Musik:** `[bgm] stage=1 room=08 entry=FF3C -> MAIN3C ... [unveraendert, laeuft durch]` und zurueck in 2090/2091 (w_leon, w_elza, kette_leon).
- Testgeschirr re2gator: `leon_statue` setzt hit_react je Takt zurueck, ein Biss trifft dadurch in drei Takten hintereinander (hp 100->50->0->-50). Kein G2-Befund.

## 6. Raumkette und Wiedereintritt (kette_leon, kette_elza; eigene Schussfolge `M0.8,MA0.1,W1.8` x45, kein RE15_K2080_RUECK_KANISTER)

- Szene -> Marvin/John -> Rueckkehr -> Aufnahme -> Beschuss -> `F1051 Kanister verbraucht (R1-Tod) -> Bit 116 = 0`, `RE2 R1 ende (Leiche) pos=(-3568,-14113)`,
  `Water Key (Item 0x34, Bits (9,111)/(9,112)) bei der Leiche (-6100,-1742,-8400) ... (Kamera 4), Zone Slot 15`, Musik `entry=042E -> MAIN2E`.
- Leiter hoch (`fire-aot slot=0 at F2400`), in 2080 Leiter zurueck (`DOOR FIRE slot=1 ... spawn=(-8200,0,-25550)`), danach pendelte die Eingabe Leiter hoch/runter:
  5 Wiedereintritte 2090 a 328 Bilder, in keinem eine Gator-Zeile (Slot 35), keine Szene, Zombie-Zeilen 0, `Slot 1 (Tor) gesperrt`, Water-Key-Zeile je Besuch,
  `MAIN2E [unveraendert, laeuft durch]`. Elza (kette_elza, 2091/2081) bitgleich: `F1051 Kanister verbraucht (R1-Tod)`, `R1 ende (Leiche) pos=(-3575,-13915)`,
  Water Key bei (-6100,-1742,-8400), 3 Wiedereintritte 2091 ohne Gator-Zeile, Tor gesperrt, MAIN2E laeuft durch (Lauf nach dem 3. Wiedereintritt per `kill` beendet).
- Aufheben des Water Key: integration_r41_alligator2_schluessel / _elza / _schluessel_west_elza in Abschnitt 7 (gruen).

## 7. Riegel und die geaenderten fremden Riegel

PowerShell, `C:\msys64\mingw64\bin` vorn im PATH (gcc `C:\msys64\mingw64\bin\gcc.exe`), SDL_ASSERT=always_ignore:
`ctest --test-dir re15_port/build -R "r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix" -j3 --timeout 1500 --output-on-failure`
(01:41-02:10, neben eigenen Fensterlaeufen) -> **`100% tests passed, 0 tests failed out of 110`**, `Total Test time (real) = 1763.33 sec`.

**unit_r41_re2gator_sitz / _schleppen (Nachzug NB1):** Kopie von HEAD nach `<SP>/neu_alttest`, darin der Riegel von 867c95480 (vor NB1) plus Diagnosezeilen,
`local_build.sh configure/build`. Ergebnis gegen die NEUE Engine:
- schleppen Fall 3 und sitz (a): `f=51 hp 100->50 ... phase 3 mo 4`, `f=52 hp 50->0`, `f=53 hp 0->-50 ... phase 8` (FRESSEN), `f=54 ... stufe 7->-1` ->
  `FAIL ... Lokallage nach Stufe 7 + 40 Takte (0,0,0)` bzw. `Stufe 7 nicht erreicht (16 Takte)` / `Kanister nicht mehr angehaengt`. Bis zum Tod: `42 angehaengte
  Bilder, groesster Abstand zu Part6(Bild) x lokal 0`.
- RE2 (EM23_OVL_0000.BIN, Disasm `G2abn1/re2/em23.dis`): FRESSEN P0 `80101008: lui at,0x800d` / `8010100c: sw zero,1308(at)` -> 0x800d051c = 0; dieselbe Basis
  nutzt Stufe 7 (`80101810: addiu s2,s2,1308`, `80101b58: lw v1,56(s2)` / `80101b5c: addiu v0,zero,1750`). Der toedliche Biss loescht den Kanister also wie RE2.
- Mit dem Nachzug (HEAD-Riegel, gleiche Kopie): sitz (a) 66 angehaengte Bilder inkl. Bisse, Abstand 0; schleppen Fall 3 (1750,-210,-262) - gemessen wird
  weiter der Sitz. Die Warteschleife vor (c) ist im heutigen Ablauf ein Leerlauf (`DIAGC Ende (b): phase 2` = schon CHASE; mit und ohne Schleife
  `sitz: OK`). **Urteil:** Die Aenderung ist sachlich richtig und kaschiert keinen Fehler; Ursache ist der neue Bogen statt Wende (der Gator erreicht die
  Statue frueher), der toedliche Biss gehoert nicht zum Mass "Kanister-Sitz".

## 8. Maengel

- **M1' (blockierend):** Mit dem Kanister im Maul laeuft der Gator an fuer ihn unerreichbaren Spielerlagen in deterministische Schleifen ohne Fortschritt:
  Leiter-Ecke SW 20 x 12 s (je 3,9 s Dash auf der Stelle, im Fenster vor der Leiter schnappend), NW-Ecke/Ost-Nische rund 14 x ~14,5 s Dash-Schleife (57 % Dash, Elza nie
  erreicht), Nordbecken/NO-Ecke 75 Pendel-Stopps (55 %). Muster jeweils: `TRAG VORBEI` am gekuerzten Ring-Wegpunkt -> Etappe zurueck ueber die Zonenlinie ->
  RE2-Zonen-Dash -> CHASE zum selben Wegpunkt. Dazu einmal 2,2 s Wandreiben an der Suedwand (ecken_leon, Bau-traglog 75 Bilder > 30).
- **M3' (blockierend):** Spieler auf dem Westteil der Rampe (x 2318, die Stelle, an die Bisse die Figur druecken und an der sie klettert): GUARD-Schleife
  Hop (4450,-9850) -> Wegpunkt (1673,-11900) hinter der Plattform-Nordkante, ~4,3 s je Zyklus regungslos paddelnd, 94-95 % Stillstand ueber 4 min,
  Leon und Elza identisch. Kein Patrouillieren.
- **M5 (Riegel-Mass):** `test_r41_gatorfix traglog` und die Riegel _frei/_rampe nehmen Clip 2/3 und "Kopf < 2500" aus - die GUARD-Schleife (94 %) und die
  Leiter-Schleife (60 % Stillstand) bestehen die Riegel. Die Fensterbelege mit RE15_K2080_RUECK_LAGE zeigen Cut 0 und weder Leon noch (im Nordbecken) den Gator.
- **M4 (gering):** H1-Rest - ~10 Bilder Leiche sichtbar quer ueber Oberkiefer/Hals (Clip 5 Bild ~1-11), Leon und Elza.

## 9. Nicht geprueft

PSX-Bau, volle Suite, Hoeren (nur Audio-Log), Aufheben des Water Key im Fenster (nur Riegel), Plattform-Mitte (von der Rampe aus nicht betreten: die Figur
blieb bei x 2318 an der Plattformkante stehen), Ursache der starren Kamera mit RE15_K2080_RUECK_LAGE.
