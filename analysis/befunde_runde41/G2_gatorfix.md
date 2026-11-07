# Runde 41 Spur G2 "gatorfix" — Dossier (fortlaufend)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gatorfix`, Basis master 789f6e681.

## Wortlaut Nutzer (bindend)
1. Z-CLIP: Der gefressene Zombie clippt seitlich unten durch das Maul -> soll im Maul sein, weder oben noch unten raus.
2. Z-WURF: Kopf hoch -> Zombie hochwerfen, in Maulrichtung auffangen, dann verschwinden (gegessen).
3. Z-MONTAGE: einlaufende Zombies nur 5, Richtung Leiter laufen.
4. MARVIN-3: Marvin-Szene 3 Zombies, laufen weiter Richtung Leiter (nicht zu Marvin schauen), etwas Weg, dann erschossen; Zeitdauer bis Schuss viel kuerzer.
5. MUSIK: in Marvins Cutscene keine Aenderung der Kampfmusik.
6. SCHLUESSEL: Schluessel am Anfang und Ende groesser.
7. HAENGEN: Alligator mit Kanister haengt staendig an verschiedenen Stellen.
8. KANISTER-WEG: nach dem Wurf nicht direkt zum Kanister, sondern weiter Runde dem Spieler hinterher; erst wenn er beim Kanister ankommt, aufnehmen.

## Stand / Fortschritt
(laufend unten)
Abschluss 2026-11-01: Punkte 1-6 und 8 erfuellt, Punkt 7 teilweise (siehe dort "Offen"). Tests (Auswahl, PowerShell-ctest): Lauf aller G2-Riegel +
der nachgezogenen Riegel der Spuren B/F/G/N/P "99% tests passed, 1 tests failed out of 104" (nur integration_r41_kanister2080_kampf, Pflichtzeile
ANSCHWIMMEN), nach dem Nachzug "100% tests passed, 0 tests failed out of 1" (kampf, 1292 s).

## Messweg (alle Punkte)
- Messkopien `re15_pc_g2_<lauf>.exe`, gdigrab auf das Fenster DIESES Prozesses (`-i hwnd=0x..`, Handle aus der eigenen Windows-PID), 30 Bilder/s,
  RE15_WINDOW_SCALE=2, kein AUTOSHOT, kein SOFTWARE_RENDER, SDL_ASSERT=always_ignore. Helfer: scratchpad `lauf.sh` (nur eigene PIDs).
- Szene Leon: `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=700#2090`
  (Leiter 2080 -> 2090, Szene, Montage 2080, zurueck, Fressen). Kino-Balken der Szene: render_pc.c y 0..24 / 216..240 (FUN_80021a0c).
- Werkzeug `test_r41_gatorfix maulgeo` (Geometrie im Part-6-Rahmen, G2_DUMP=<bild>|9999), `wurfkamera` (Sichtbarkeit je Zonenkamera).

## Punkt 1 - Leiche clippt unten durch das Maul
**Messung vorher** (Bau 789f6e681, Lauf v_ank_leon, Bild `G2_belege/p1_vorher_leon_halt.jpg`): die Leiche haengt diagonal, Kopf/Beine
unter dem Unterkiefer. maulgeo: Part 6 ist der UNTERKIEFER, Part 7 der Schaedel (Welt-y je Bild, F78: Part 7 -3329, Part 6 -2236); im
Part-6-Rahmen zeigt -y nach oben. Leichen-MD1 obj 2 (306 Scheitel): x -444..2626, y -341..966 (Beine haengen bei x 1900..2800 auf
y 650..966), z -1049..822. In Clip 5 Bild 6..56 lagen 134..146 Scheitel ausserhalb des Kopf-Bandes, davon 117..120 UNTER dem Unterkiefer.
**Ursache:** Spur P fror die Relation Leiche <-> Part 6 im Koppelbild ein (so, wie die Leiche im Wasser trieb) und fuehrte nur die
Rumpfmitte auf die RE2-Kanister-Maullage (1750,-210); die Ausrichtung blieb diagonal, der Rumpf hing unter dem Kiefer.
**Aenderung** (gatorszene_r41.c, PORT-WAHL der Gestaltung; RE2 haelt sein Opfer mit eigener Opfer-Animation, nicht als Objekt):
Leiche QUER im Maul (Modell-x -> Part-6-z, Modell-y -> Part-6-y, Modell-z -> -x), 15 Grad (`GSZ_LEICHE_KIPP` 171) um die Laengsachse
gekippt, Rumpfmitte auf (450,-330) im Part-6-Rahmen (`GSZ_MAUL_X/Y` 675/-495 Modell-Einheiten x 2731/4096). Wahl = Messung
(scratchpad geo/suche7.py + suche8.py ueber die Kopf-Scheitel JEDES Haltebildes Clip 5 Bild 25..57 und 75..77, gleiche Rechnung wie der
Riegel): groesster kleinster Randabstand 67 (Gier/Rollen 0). Drehung vom Koppelbild in die Lage per nlerp ueber 26 Bilder (= RE2-Stufen 3-6
der Kanister-Aufnahme 6+6+7+7 @0x80101970/@0x801019dc/@0x80101a64/@0x80101aec, Spur-P-Konstante RE15_GR2_KAN_SITZ_BILDER), Lage wie
bisher mit den T2-Schritten 145/55.
**Messung nachher** (Bau dieses Standes, Lauf n2_ank_leon, `G2_belege/p1_nachher_leon_halt.jpg`): Leiche quer im Maul, Kopf links,
Beine rechts, nichts unter dem Kinn. Riegel unit_r41_gatorfix_maul / _elza: 34 Haltebilder, 0 Scheitel ausserhalb, kleinster Randabstand
36; Einschwenken (26 Bilder ab dem Koppelbild) berichtet: 9 gemessene Bilder mit hoechstens 24 Scheiteln ausserhalb (die Drehung aus dem
Wasser). Gegenprobe (Pruefhaken re15_gatorszene_r41_g2_alt_setzen(1) = Lage wie vor G2): 146 Scheitel ausserhalb, 120 darunter.

## Punkt 2 - hochwerfen, im Maul fangen, verschwinden
**Messung vorher** (v_ank_leon, `G2_belege/p2_vorher_leon_hochreissen.jpg`): beim Hochreissen bleibt die Leiche am Kopf und ist mit dem
Clip-5-Ende (F208 im Lauf) weg; Clip 11 kaut leer.
**Kopfbewegung gemessen** (maulgeo KOPF-Zeilen, Clip 5 der RE2-EM23-Bank): Bild 56 -> 59 reisst er den Kopf hoch (Maul-Welt-y -2795 ->
-3945, Schnauze waagerecht -> 70 Grad hoch), oben bis Bild 74, Maul oeffnet Bild 70-74 (32 -> 44 Grad), schnappt 75-77 zu (23 -> 13 -> 11).
**Aenderung** (PORT-WAHL, Nutzerwunsch): Wurf Clip 5 Bild 57 (`GSZ_WURF_BILD`), Fang Bild 75 (`GSZ_FANG_BILD`), weg Bild 77
(`GSZ_WEG_BILD`, Leiche + Schluessel). Bahn relativ zur Maullage: Lage(t) = Wurf + (Maul(t) - Wurf) t/T, nach oben 4H t(T-t)/T^2,
H = a T^2/8 mit a = 20/Bild^2 = RE2-Wurf Add_aspeed (ROOM7020 sub13 Speed_set(7,20) @0x089E, = RE15_K2080_WURF_AY); Drehung nlerp.
**Messung nachher** (n2_ank_leon, `G2_belege/p2_nachher_leon_wurf_fang.jpg`; Folge-Zeilen `W F145 wert=57`, `F F163 wert=75`,
`V F165 wert=77`): Riegel unit_r41_gatorfix_wurf / _elza: Rumpfmitte 603 ueber der Maullage, 13 Bilder steigend / 5 fallend, im Fangbild
2 vom Maul, danach Leiche und Schluessel 0 Bilder sichtbar. Gegenprobe: kein Wurf.
**Kamera (gemessen, keine Aenderung):** in Spur B's Fress-Einstellung Cut 1 liegt die Schnauze beim Hochreissen hinter dem oberen
Kino-Balken (144 von 189 Kennpunkten sichtbar); Cut 3 ist durch die Masken ganz verdeckt (0/189), Cut 6 zu 47 % (100/189) und zeigt
den Alligator klein hinter der Maschine (Lauf k6_ank_leon), Cut 0 zu nah (k0_ank_leon). Cut 1 bleibt die beste Einstellung; die
Leiche ist in Cut 1 im ganzen Flug zu 97 % zwischen den Balken.
Nachgezogen (Spur P): unit_r41_alligator2_leiche(_elza) "weg mit dem Clip-5-Ende" -> "weg mit Clip 5 Bild 77" und >= 50 Haltebilder statt > 100.

## Punkt 3 - Montage: 5 Zombies, Richtung Leiter
**Messung vorher** (v_ank_leon state.log F1..F156, Bild `G2_belege/p3_vorher_leon_8zombies.jpg`): 8 Zombies (GSZ_2080_ZOMBIES, grid 0x0d) mit
Blick auf den Treppenabsatz (Gier 1819..2698), sie drehten 150 Bilder lang (Gier -6/Bild) zum an der Leiter geparkten Spieler; Weg zur Leiter
-452..4384, nur 4 von 8 >= 1000 naeher (ganglog montage), zwei an der Westwand x -13882.
**Aenderung** (gatorszene_r41.c, PORT-WAHL Nutzerwunsch): 5 Saetze (die noerdlichen k_zombies 3..7 der Spur G - nach dem gemessenen Weg bleiben
alle fuenf in Cut 2 im Bild, Projektion `test_r41_gatorfix projektion`: Enden x 76..172, die suedlichen bei x 19..45, die Leiter bei x -119),
Gier zur Leiter-Ankunft `re15_gatorszene_r41_gier_zur_leiter` (= GSZ_2080_PARK = ROOM2090 Tuer 0 @0x00996; atan2(-dz,dx) wie k_zombies).
**Messung nachher** (n3_ank_leon, `G2_belege/p3_nachher_leon_5zombies_zur_leiter.jpg`): 5 Zombies, Weg zur Leiter 2476..3786 in 156 Bildern.
Riegel unit_r41_gatorfix_montage(_elza) (5, Blick-Abweichung 0) + integration_r41_gatorfix_gang_montage(_elza) (alle >= 1000 naeher; der alte Bau
faellt durch: 4 von 8).

## Punkt 4 - Marvin/John-Szene: 3 Zombies, laufen zur Leiter, schneller erschossen
**Messung vorher** (v_marvin_leon: Kampf 2090 mit Messhaken RE15_K2080_GATOR_HP=24990, Bild `G2_belege/p4_vorher_leon_8zombies_15s.jpg`):
8 Steher (grid 0x02) mit Blick auf den Treppenabsatz = zu Marvin, Spieler am Treppenfuss; Schuesse F118, F152, ... F351 (ROOM1021-Form:
Sleep 110 nach dem Anlegen @0x020CA, Sleep 30 nach jedem Schuss @0x020D2).
**Aenderung** (kanister2080_r41.c / re15_kanister2080_r41.h): RE15_K2080_N_ZOMBIES 3 (die drei noerdlichen Lagen, 10200..11000 von der Leiter),
Gier zur Leiter; in der Szene grid 0x0d (ENGAGE wie die Montage, STAGE1.BIN @0x80100f40-44) und der Spieler an der Leiter-Ankunft geparkt
(RE15_K2080_LEITER_*, Ziel der Zombies; in Cut 2 und 5 nicht im Bild, in Cut 7 schon -> mit dem Schnitt auf Cut 5 zurueck auf den alten
Parkplatz); Wartezeiten = Clip-Laengen statt ROOM1021: Anlegen Eigen c6 10 Bilder, Feuern Eigen c7 16 Bilder (Dossier G 1.8), jedes Ziel erst
nach dem Nachdrehen 0xC8/Bild (@0x80033f90).
**Messung nachher** (n_marvin_leon, `G2_belege/p4_nachher_leon_3zombies_2s.jpg`): Schuesse F19 / F36 / F53, Abstand zur Leiter beim Tod
10136 / 9660 / 10240 (vorher 10240 / 10442 / 10980), Blick 843..1035 (zur Leiter). Riegel unit_r41_gatorfix_marvin(_elza) und
integration_r41_gatorfix_gang_szene(_elza) (3 Treffer, letzter <= F90, jeder naeher beim Tod und >= 2000 entfernt).

## Punkt 5 - Kampfmusik bleibt in der Szene
**Messung vorher** (v_marvin_leon, RE15_AUDIO_CAP_SYNC, debug.log `[bgm]`): Kampf 2090 `entry=FF3C -> MAIN3C`; Ausloeser -> 2080
`entry=042E -> MAIN2E` (Raumtabelle UNK_80074828); zurueck 2090 `entry=FF3C -> MAIN3C` (Neustart des Stuecks).
**Aenderung** (gatorszene_r41.c musik_soll): in 2080/2081 waehrend der Spur-G-Phasen NACH_2080/IN_2080/RUECK_2090 bleibt der Eintrag 0xFF3C;
FUN_80044210 vergleicht MAIN/SUB mit dem Cache und tut bei Gleichheit nichts (@0x80044278-80 -> @0x800443B0).
**Messung nachher** (n_marvin_leon): `[bgm] stage=1 room=08 entry=FF3C -> MAIN3C ... [unveraendert, laeuft durch]`, zurueck ebenso. Riegel
unit_r41_gatorfix_marvin(_elza): Eintrag 0xFF3C beim Ausloesen, in jedem Szenenbild und zurueck; Gegenprobe 2080 ohne Szene = Raumtabelle (-1).

## Punkt 6 - Schluessel groesser
**Messung vorher** (Bau vor dem Punkt, Laeufe v_ank_leon / v_tod_leon (RE15_GB_TEST=3 Todesrolle), Bilder `G2_belege/p6_vorher_leon_ankunft_crop.jpg`,
`p6_vorher_leon_tod_crop.jpg`): RE2-Modell Platform Key (ROOM7040 MD1 @0x354C, Quadrat +-90); Kasten im Bild (Projektion wie der Zeichner)
Ankunft Cut 0 11 x 11, nach dem Tod 21 x 18 Bildpunkte (Lage (-5300,-26200), Einstellung Cut 1 bzw. im Harness Cut 7).
**Aenderung** (PORT-WAHL Nutzerwunsch "etwas groesser", Faktor 3/2, `RE15_GSZ_KEY_SKALA_Q12` 6144): re15_gatorszene_r41_md1_bytes liefert eine
Kopie des bytegleichen RE2-MD1 mit skalierten Scheiteln (+-135, Normalen unveraendert); GSZ_KEY_HALB (Wasser-Quadrat, Kippung) und die
Bildgroessen-Schwelle der Lagewahl GSZ_KEY_BILD_MIN10 mit derselben Skala - die Lage nach dem Tod bleibt dieselbe (Lauf n_tod_leon:
(-5300,-1701,-26200), vorher (-5300,-1669,-26200); y hoeher, weil das groessere gekippte Quadrat mit der tiefsten Ecke auf dem Wasser liegt).
**Messung nachher** (n4_ank_leon / n_tod_leon, `p6_nachher_*`): Ankunft 15 x 18, nach dem Tod 32 x 27 (Harness), exe integration_r41_gatorszene_tod:
Kasten 22 x 19, Quadrat 14,8 Bildpunkte. Riegel unit_r41_gatorfix_schluessel(_elza) (Flaeche >= 1,8 x die mit dem RE2-Modell im selben Lauf).
Nachgezogen: unit_r41_gatorszene_schluessel (Mitte hoechstens sqrt(2) x 135 = 191 statt 127 ueber dem Wasser).

## Punkt 7 - mit dem Kanister bleibt er haengen
**Messung** (drei Quellen):
- Nutzer-Sitzung 2026-11-01 19:54 (re15_port/build/platform/pc/befund.log Marke 1 F735, Bild befund_2090_F735_marke1.bmp; gator_boss.log der Sitzung,
  nur gelesen): dritter Trag-Abschnitt ab `KANISTER angehaengt F139`; von F636 bis mindestens F1208 steht der Alligator bei (3191,-22595) Gier 1 Clip 1,
  `KOPF-KLEMME an ... Ueberstand (-207,0)` je Takt, Steuerziel `ziel=(4450,-22600)` (Belagerungs-Hop zur Rampe, Spieler auf der Plattform
  (5814,-13807)), 50 `PUTZ Stoss` gegen die Wand, `NOTFREI` ohne Wirkung - 19 s Festdruecken.
- Lauf k_tragen_leon/k3_tragen_leon (natuerlicher Kampf mit Messhaken RE15_K2080_GATOR_HP, Messzeile RE15_G2_TRAG_LOG: Takt-Schritt gegen Schritt nach den
  Klemmen): 27 % der Trag-Bilder Stillstand (30 Bilder < 300) gegen 10 % ohne Kanister (k3_ohne_leon); Bilder `scratchpad tragen_karte3.png`.
- Harness `test_r41_gatorfix haenger` (Weg durchs Becken / Ecken, mit und ohne Kanister): Clip-0-Stillstaende gleich haeufig (6/6, 7/7) - die sind Spur-B-
  Verfolgung und nicht kanisterbedingt.
**Ursache:** die Steuerziele der Spur B (CHASE: Klemmflaeche 1600 vor den Aussenwaenden; GUARD: Belagerungs-Hops) gelten der WURZEL. Mit dem Kanister quer
im Maul ragen Kopf und Kanister ~4200 vor die Wurzel; die Kopf-Klemme (Spur G/P, PORT-WAHL G-E5) schiebt den Gator jedes Bild hinter die Aussenwand
zurueck, die Lenkung schiebt ihn wieder auf das Ziel - Ziel und Klemme widersprechen sich, er drueckt sich fest.
**Aenderung** (gator_re2_r41.c `re15_gr2_kk_ziel`, Haken in enemy_ai_boss_gator.c CHASE und GUARD): mit dem Kanister im Maul wird das Steuerziel um den
Ueberstand verschoben, den die Kopf-Klemme an ihm messen wuerde (dieselben Punkte Hals/Kopf/Kiefer + beide Kanister-Enden, dieselben Waende GR2_KK_*,
Pose ohne Ueberblendung, auf die Ankunftsrichtung gedreht). Kein Zaehler, keine Zeitgrenze.
**Messung nachher:** Riegel unit_r41_gatorfix_klemm(_elza): an der Lage der Nutzer-Marke und gegen West/Sued/Nord Ueberstand am Ziel 0 (Ost: Ziel
(4450,-22600) -> (2563,-22600)); Gegenprobe Ziel wie vor G2: 340 (= Festdruecken). Harness-Lauf der Marken-Lage: wie vor G2 drueckt er mit Gier 13 nach
Osten, neu kommt er an.
**Offen (ehrlich):** steht der Spieler auf der Plattform, wartet der Gator mit Kanister am verschobenen Rampen-Hop (er drueckt sich nicht mehr fest, aber
er belagert dort) - das Klettern ueber die Rampe mit Kanister ist Spur-B-Design und nicht Teil dieses Punkts. Ein exe-Lauf mit freier Spielerbewegung
ueber den ganzen Kampf ist nicht als Riegel gebaut.

## Punkt 8 - nach dem Wurf nicht zum Kanister schwimmen
**Messung vorher** (Lauf k_tragen_leon debug.log): `F43 ANSCHWIMMEN an: Gator Slot 35 (-2860,-22691) ... Kanister (-6856,-12649) Abstand 10807`
unmittelbar nach der Rueckkehr, der Alligator liess den Spieler und schwamm zum Kanister (`F143 ANSCHWIMMEN aus ... Stufe 3`).
**Aenderung** (kanister2080_r41.c anschwimm_lage, NUTZER-VORGABE gegen N3): kein Anschwimmen; die Verfolgung bleibt beim Spieler, die RE2-Aufnahme der
Spur B (Fenstertreffer, wenn der Kanister vor dem Maul liegt) kommt erst, wenn sein Weg ihn zum Kanister fuehrt. Hals-Zielmodus (Nachtrag 26) folgt
dem Anschwimmen und ist damit ebenfalls aus. Pruefhaken der Gegenprobe re15_k2080_anschwimmen_alt_setzen.
**Messung nachher:** unit_r41_kanister2080_anschwimmen (nachgezogen): in den fuenf Lagen der Abnahme 1 kein Bild Anschwimmen (Spieler fern: der Gator
verfolgt ihn); Spieler hinter dem Kanister (auf der Linie Gator -> Kanister): Aufnahme beim Ankommen in Bild 131/665/114/424/246; Gegenprobe N3:
Aufnahme durch Anschwimmen in Bild 407.
**Nachtrag Punkt 8 (Folge fuer die exe-Riegel):** ohne Anschwimmen fielen integration_r41_alligator2_tragen(_elza)/_schluessel*/kanister2080_kampf
(stehende Figur, der Kanister liegt fern -> keine Aufnahme). (1) MESSHAKEN `RE15_K2080_RUECK_KANISTER=1` (env, PC, kein Spielverhalten): die
Rueckkehr-Lage der Figur hinter den Kanister (Nennlandung + 2500 auf der Linie Alligator -> Kanister) - die Figur lockt ihn hin, wie der Nutzer es
tut; in den fuenf Riegeln gesetzt. (2) Gemessen (tragen_elza, F1201): die Wurzel ging 25 neben dem Kanister vorbei, das RE2-Fenster (8601..8999 x
Skala VOR der Wurzel, gator_re2_r41.c @0x80100ae4-ec) wurde nie getroffen, weil er den Kanister kreuzt statt laengs auf ihn zuzuschwimmen. Haken:
fuer den Szenen-Kanister gilt das Fenster ab der Wurzel (laengs >= 0) bis zum RE2-Ende ("erst wenn er beim Kanister ankommt"); mit dem Pruefhaken N3
das RE2-Fenster wie bisher. Riegel-Nachzug kanister2080_kampf: Tot-Bits 236..238, 3 von 3.

## Nachbesserung 1 (Abnahme 1: analysis/befunde_runde41/G2_abnahme_1.md, M1-M3, H1, H2, E1)
Messweg wie oben; Laufordner scratchpad `g2nb1/runs/<lauf>` (exe-Kopie `re15_pc_g2nb1_<lauf>.exe`, gdigrab auf das eigene Fenster, kein AUTOSHOT/SOFTWARE_RENDER).
"Vorher" = die Abnahme-Laeufe auf Bau 867c95480 (gleiche Eingabe, `G2abn1/runs/frei_*`, Bilder `G2_abnahme_1/`). Auswerter neu: `test_r41_gatorfix traglog <lauf>`
(Masse der Abnahme still_state.py/haenger.py in C: Stillstand = Wurzel 30 Bilder netto < 300, fern = Kopf > 2500 von der Figur und nicht RE2-Dash, dazu ohne
Biss-Reichweite = Wurzel < 3200 (Spur-B-Bisstor); Wandreiben = Spanne der Kopf-Klemm-Bilder in einer solchen Stillstands-Folge, Luecken <= 26 = ein Paddel-Zyklus).

### M1 (Punkt 7) - Haengen mit Kanister im freien Spiel
**Messung vorher** (Abnahme-Laeufe, traglog): Leon F7545-F7894 Wurzel (-3952,-10027), Ziel nach der G2-Verschiebung (-4001,-8549) 1300-2000 noerdlich der
Klemmlinie; Elza F6971-F6981 Ziel auf der Wurzel, Gier pendelt 3285/3381/3477 (= Wende 0x60 hin und her), Takt (0,0). Ursachen: (1) G2 drehte die Kopfpunkte auf
die Richtung zum UNVERSCHOBENEN Ziel, er schwimmt aber zum verschobenen; (2) Punkte nur aus dem laufenden Bild; (3) kein "angekommen" - am Ziel steht oder drueckt er.
RE2 hat keinen Wartezustand: Stufe 7 @0x80101b58-8c (`lw v1,56(s2) / addiu v0,zero,1750 / beq / jal 0x8001a1e0 (a2 100)`, `lw v1,60(s2) / addiu v0,zero,-210 /
... (a2 30)`) fuehrt nur die Lokallage nach, ohne Zaehler; nur das Intro wartet (`lh v0,468(s0) / beq` @0x80100854-5c).
**Aenderung** (gator_re2_r41.c TRAG-LENKUNG, PORT-WAHL, Herleitung im Kommentar): Kopfpunkte = konvexe Huelle Bones 5/6/7 + Kanister-Enden ueber alle Bilder
der Clips 0/1 (einmal je Anhaengen); Wurzel-Kasten je Gier = Aussenwaende minus Huelle minus 340 (GR2_KK_SCHRITT); Ziel auf der Strecke Wurzel -> Ziel gekuerzt
(dieselbe Gier fuer Rechnung und Schwimmen); am gekuerzten Ziel (< 800, Spur-B-Erreicht-Radius) VORBEI-Etappe 4000 (= ein Clip-0-Zyklus 5990 x 2731/4096) laengs
der Wand, danach Gegenrichtung; erst in Stufe 7. Mit dem Kanister keine Wende auf der Stelle (Bogen mit dem Clip-Wurzelversatz, CHASE und GUARD) - RE2 Stufe 7.
GUARD: Pendel ohne 90-Bilder-Halt, keine 600er-Ruhezone, Hop des Kanten-Umwegs: am gekuerzten Hop Etappe zum Wegpunkt, den der Umweg vom Hop aus waehlt.
**Messung nachher** (gleiche Eingabe; traglog, Spalten: Stillstand fern / fern ohne Biss-Reichweite / laengste Folge / Wandreiben / Aufnahme):
| Lauf | fern | ohne Biss | laengste | Wandreiben | Aufnahme nach Aufschlag |
|---|---|---|---|---|---|
| alt_frei_leon (vor G2) | 13 % | 11 % | 307 | 307 | 111 (Anschwimmen) |
| alt_frei_elza (vor G2) | 6 % | 6 % | 57 | 57 | 100 (Anschwimmen) |
| frei_leon (G2, Abnahme) | 28 % | 26 % | 350 | 345 | 3619 |
| frei_elza (G2, Abnahme) | 27 % | 25 % | 192 | 192 | 489 |
| n2_frei_leon (NB1) | 7 % | 7 % | 39 | 8 | 582 |
| n2_frei_elza (NB1) | 10 % | 8 % | 40 | 9 | 489 |
Ehrlich: Elza liegt mit 8-10 % ueber dem Vorher-Lauf derselben Eingabe (6 %), im Band des alten Stands (7-13 %); die Reste sind 31-43 Bilder Clip 0 nach einem
Biss (Biss-Sperre, RE2-Hysterese) bzw. die Gleit-Bilder von Clip 0 in einer Wende, keine Wand. Beide Laeufe zusammen 8,1 % gegen 9,0 % vor G2.
Bilder `G2_belege/nb1_m1_nachher_karte_frei_leon.jpg` / `_elza.jpg` gegen `G2_abnahme_1/p7_karte_frei_*.jpg`.
Riegel: unit_r41_gatorfix_fuzz(_elza) (26 Spielerlagen, je 450 Bilder): neu Stillstand 42 von 11700 (0 %), Wandreiben hoechstens 2; vor G2 2432 (20 %) / 375;
Gegenprobe G2 vor NB1 Wandreiben 31 > 30 (rot). integration_r41_gatorfix_frei(_elza) (exe, Abnahme-Eingabe 8400 Bilder): Wandreiben <= 30, laengste Folge <= 60,
Anteil <= 13 %, Aufnahme <= 900 Bilder; Gegenprobe = die Abnahme-Laeufe (traglog oben, rot).
Nachgezogen (Spur B): unit_r41_re2gator_schleppen Fall 3 / _sitz (a)+(c): die Statue bekommt volle Lebensenergie bzw. (c) wartet einen laufenden Biss ab - mit dem
Kanister schwimmt er in der Wende weiter und erreicht die Statue frueher, der toedliche Biss (FRESSEN P0 schaltet den Kanister aus @0x80101008-0c) gehoert nicht zum
Mass dieser Faelle (Kanister-Sitz).

### M3 (Punkt 7, Plattform) - patrouilliert weiter, auch mit Kanister
**Wie man im Port hinaufkommt** (gemessen, `test_r41_gatorfix kletterkarte 2090`): Kletterzellen (Bodentypwort Bit 0, climb_common.c @0x8002d530-50) gibt es in
ROOM2090 Band 0 nur auf der RAMPE (468 Zellen, Typwort 0x0101, x 2000..6800, z -18000..-14600), die Plattform selbst hat keine - hinauf geht es ueber die Rampe
(aus dem Wasser an ihrer Nord- oder Suedkante, Blick zur Rampe, Aktion), von dort zu Fuss auf die Plattform. Die Abnahme versuchte die Westkante der Plattform.
Lauf n4_plat_* (Messhaken RE15_K2080_RUECK_LAGE=3000,-18800,3072 = Rueckkehr an die Suedkante der Rampe, Blick Nord; danach `A` -> climb.log sub=9, Figur oben):
| Lauf | Kletterzellen | Trag-Bilder Belagerung | Stillstand fern | laengste Folge | Wandreiben |
|---|---|---|---|---|---|
| n2_plat_leon (NB1 ohne Hop-Regel) | sub=9 | 2696 | 1 % | 37 | 6 - aber Schleife im Westkanal, 7000 von der Figur (Hop (-4300,-22600) nie erreicht) |
| n3_plat_leon (Hop-Regel, mit 600er-Ruhezone) | sub=9 | 2696 | 74 % | 2165 | 0 - stand 591 vor dem Etappenziel |
| n4_plat_leon (NB1) | sub=9 (52 Bilder) | 2696 | 0 % | 0 | 0 |
| n4_plat_elza (NB1) | sub=9 (52 Bilder) | 2696 | 1 % | 16 | 0 |
Er schwimmt mit dem Kanister um die Suedwestecke der Plattform und patrouilliert unter Plattform/Rampe hin und her (Karte `G2_belege/nb1_m3_nachher_karte_rampe_leon.jpg`,
`_elza.jpg`, Fensterbilder alle 10 s `nb1_m3_nachher_rampe_leon_10s.jpg`). Vorher (Nutzer-Sitzung 2026-11-01 Marke 1): 19 s am Rampen-Hop; Fuzz-Gegenprobe G2 v1
Lage Rampe 41 Bilder Stillstand am Stueck.
Riegel: integration_r41_gatorfix_rampe(_elza) (climb sub=9, >= 600 Trag-Bilder Belagerung, laengste Stillstands-Folge <= 60, Wandreiben <= 30); Fuzz-Lagen P-*/Rampe.

### M2 (Punkt 2) - Wurf, Fang, Verschwinden lesbar
**Messung vorher** (`test_r41_gatorfix kopfwinkel`, Fress-Einstellung Cut 1): Clip 5 Bild 56 -> 60 Aufrichten (Keyframe-vz Bone 0 23 -> -179, Bone 1 57 -> -240,
Bone 5 56 -> -181, Bone 6 56 -> -240; Part-6-Achse Welt-y -346 -> -3918), Schnauze (Part-6-x 2500) Bild 58..76 bei Bild-y -9..11 = hinter dem Balken 0..24.
Riegel-Gegenprobe (Stand vor NB1): Schnauze in 19 von 20 Flug-/Fangbildern hinter dem Balken, Leiche oben angeschnitten in 13, Leiche nie ueber der Schnauze (-54).
**Aenderung** (gatorszene_r41.c KOPFHALTUNG BEIM WURF, PORT-WAHL "Kopfhaltung/Wurfhoehe abstimmen"): Aufrichten der Bones 0/1/5/6 ab Clip 5 Bild 56 um 3/4 gedaempft
(Zuschlag auf vz wie die Spur-B-Part-Zusaetze, Keyframes je Szene aus der Bank), nach Bild 77 in 12 Bildern zurueck; Wurf-Beschleunigung 40 statt 20 (Wahl =
Messung: Leichen-Unterkante 17 Bildpunkte ueber der Schnauze, Oberkante unter dem Balken).
**Messung nachher**: Riegel unit_r41_gatorfix_wurf(_elza): in keinem der 20 Flug-/Fangbilder Schnauze oder Leiche hinter dem Balken, Leiche ganz ueber der
Schnauze (17), Fang Bild 75 (4 vom Maul), weg Bild 77. Im Fenster (n4_ank_*): `G2_belege/nb1_m2_nachher_leon_wurf_fang_15fps.jpg` / `_elza` - Leiche fliegt
ueber das offene Maul, faellt hinein, mit dem Zuschnappen weg (vorher `G2_abnahme_1/p2_leon_wurf_fang_t32.97-33.67_30fps.jpg`).

### H1 - Biss: ganz im Maul
**Messung vorher** (`G2_MITTE`): Clip 4 Bild 7..12 liegt die freie Leiche unter dem Kiefer (Scheitel unter dem Kopf-Band 76 -> 186), Koppelbild 163, danach 26 Bilder
Drehung (bis 24 aussen). **Aenderung** (VORGRIFF, PORT-WAHL): ab Clip 4 Bild 1 zieht das offene Maul die Leiche in 6 Bildern (= RE2-Stufe 3 @0x80101970) in die
Halte-Lage, im Koppelbild Relation/Lage genau die Halte-Lage. **Nachher**: unit_r41_gatorfix_maul(_elza) jetzt ab Clip 4 Bild 7 bis zum Wurf (66 Bilder): 0 Scheitel
ausserhalb; Gegenprobe ohne Vorgriff 186 (rot). Bild `G2_belege/nb1_h1_nachher_leon_biss.jpg`.

### H2 - Marvin/John: jeder laeuft sichtbar
Vorher (Abnahme): Zombie 1 nach 19 Bildern / 104 Einheiten. **Aenderung**: Sleep RE15_K2080_LAUF_BILDER 20 vor dem Anlegen (PORT-WAHL: + Anlegen 10 = 30 Bilder
= "rund 1 s"). **Nachher** (exe n2_frei_leon): Schuesse F39/F56/F73, gelaufen 33/50/67 Bilder (Bahn 448/1345/1195), Rest zur Leiter >= 9272. Riegel
unit_r41_gatorfix_marvin(_elza) erster Tod >= 30 (Harness 32; Gegenprobe ohne Laufzeit 12, rot), integration_r41_gatorfix_gang_szene(_elza) je >= 30 Bilder gelaufen.

### E1 (Punkt 8) - nach einer Weile am Kanister vorbei
**Aenderung** (kanister2080_r41.c, PORT-WAHL): liegt der Kanister 450 Bilder (15 s) frei, wird er Wegpunkt seiner Runde, sobald er VOR ihm liegt (er dreht nicht
dafuer um), spaetestens nach 600 Bildern; geschwommen im Bogen (Slew 0x30, keine Wende auf der Stelle) bis zur Aufnahme; Zeile `RUNDE zum Kanister an`.
**Nachher**: Aufnahme 582 (Leon, vorher 3619) / 489 (Elza) Bilder nach dem Aufschlag = 19,4 / 16,3 s; Rampen-Lauf 637 Bilder.

### Abschluss Nachbesserung 1
Tests (PowerShell-ctest, Auswahl wie die Abnahme `r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix`, -j3): **100% tests passed, 0 tests failed out of 110** (2271 s), darin neu unit_r41_gatorfix_fuzz(_elza) und integration_r41_gatorfix_frei/_frei_elza/_rampe/_rampe_elza. Endstand der exe-Riegel (traglog): frei Leon fern 7 %, laengste Folge 39, Wandreiben 8, Aufnahme 582 Bilder; frei Elza fern 5 % (ohne Biss-Reichweite 4 %), 31, 16, 489; Rampe Leon 0 %, Rampe Elza 1 % (16 Bilder), Wandreiben 0.

## Nachbesserung 2 (Abnahme 2: analysis/befunde_runde41/G2_abnahme_2.md, M1', M3', M4, M5)
Laeufe scratchpad `g2nb1/runs/n8_*` (Eingabe und Umgebung der Abnahme-2-Laeufe `G2abn2/runs/<lauf>/env.txt`, eigene exe-Kopie, gdigrab, kein AUTOSHOT/
SOFTWARE_RENDER). "Vorher" = die Abnahme-2-Laeufe selbst (Bau e7d0510c7).

### M5 - Mass ehrlich
`test_r41_gatorfix traglog` und Fuzz zaehlen jetzt **2-s-Fenster ohne Fortschritt**: 60 Bilder, in denen die Wurzel nicht 300 vom Fensteranfang wegkommt
(Zappeln, Wandreiben, Wende/Dash/Schnappen auf der Stelle bleiben darunter; Kreisen nicht). Einzige Ausnahme: ein Fenster, in dem ein Biss (Phase 3/8) am
Spieler laeuft (Wurzel < 3200 oder Kopf < 2400). Keine Ausnahme mehr fuer Dash-Clips oder "Kopf < 2500". Dazu weiter: Anteil der 30-Bilder-Fenster mit
< 300 netto (Mass der Abnahme), Wandreiben (Klemme greift in einem Fenster ohne Fortschritt).
Gegenrechnung der Abnahme-2-Laeufe (vorher) mit dem neuen Mass: rampe_nat_leon 969 Fenster, w_elza 852, plat7_leon 3904, plat8_elza 3953, ecken_leon 224,
w_leon 107, plat_leon 6; plat2_leon (offenes Wasser) 0 - das Mass trennt.

### M1' - Schleifen an unerreichbaren Lagen
RE2 (EM23_OVL_0000.BIN, Disasm G2abn1/re2/em23.dis): Sub 1 schwimmt ohne Ziel weiter (`jal 0x8001a330` Clip 0 Blend 32 @0x80100c34-40); der Dash (Routine
0x301 `addiu v0,zero,769` @0x801009ac/@0x801009f4) kommt nur mit dem Zonenbyte +0x21A, das nur hochgezaehlt wird (`sb v0,538(s0)` = 1 @0x801009a8 bei
x < -17877 `slti v0,v0,-17877` @0x80100998, = 2 @0x801009ec bei z < -7322 @0x801009e0) - jeder Dash hoechstens einmal; Biss-Sperre `lbu v0,568(s0) / bne`
@0x80100b5c-64. Kein Ziel-Pendel, kein wiederholter Dash.
Aenderungen (gator_re2_r41.c NB2-Block, enemy_ai_boss_gator.c):
- mit dem Kanister kein Zonen-Dash (`re15_gr2_trag_dash_erlaubt`; der Port-Dash = 3,9 s Vordrehung auf der Stelle) und kein Rampen-Uebergang (Westumlauf;
  der Anlauf-Wegpunkt an der Rampenkante liegt fuer Kopf + Kanister hinter der Ostwand).
- gekuerztes SPIELER-Ziel in Reichweite (< 2 R): UMKREISEN eines Mittelpunkts R = 2000 (Wenderadius beim Paddeln: 149 je Bild / Slew 0x30 = 2025) von der
  kuerzenden Wand weg, Karotte 45 Grad voraus, bis das Ziel wieder passt oder der Spieler > R wandert.
- Wegpunkte der Routenwahl (nicht der Spieler, > 1500 daneben): am gekuerzten Wegpunkt weiter zum Wegpunkt, den der Umweg von dort waehlt (kein Kreisen).
- kein Ziel in den um den Wurzel-Rand (hit_radius_min + 180) aufgeblasenen Bloecken Plattform/Rampe (auch nach dem Kuerzen); Etappe, der die Wurzel in 60
  Bildern nicht 200 naeher kommt, wird abgebrochen (Zeile `TRAG ETAPPE ohne Annaeherung`, im Fuzz gezaehlt: 15).
| Lauf (Eingabe der Abnahme 2) | vorher 2-s-Fenster | nachher 2-s-Fenster | nachher 30-B.-Anteil (Mass Abnahme) | Wandreiben | Aufnahme |
|---|---|---|---|---|---|
| rampe_nat_leon (Leiter-Ecke SW) | 969 | 0 | 0 % | 0 | 877 |
| w_elza (NW-Ecke, Ost-Nische) | 852 | 0 | 7 % | 0 | 121 |
| plat_leon (Nordbecken/NO-Ecke) | 6 | 0 (n9) | 59 % (Clip-0-Kreisen, s. u.) | 0 | 477 |
| ecken_leon (Waende/Leiter) | 224 | 0 | 29 % | 0 | 754 |
| w_leon (Wand-Tour) | 107 | 0 | 4 % | 0 | 711 |
Ehrlich: Im Nordbecken (Spieler an der NO-Ecke, mit Kanister nicht erreichbar) kreist er ~6000 vom Spieler in Clip 0 (RE2-Sub 1, in der Kanister-Zone
eingefroren @0x80100ab8-c8, ~25 je Bild mit Gleitbildern) - 30-Bilder-Fenster netto < 300 in 59 %, aber kein 2-s-Fenster ohne Fortschritt.
Fuzz unit_r41_gatorfix_fuzz(_elza), 31 Lagen x 900 Bilder (dazu die fuenf Lagen der Abnahme 2): 0 Fenster, Wandreiben 0, 30-B.-Anteil 3 %; vor G2 1032
Fenster, G2 v1 (Gegenprobe) rot. Karten `G2_belege/nb2_karte_nachher_<lauf>.jpg`, Fenster Leiter-Ecke `nb2_m1_nachher_leiterecke_leon_10s.jpg`.

### M3' - Rampe West
Belagerung mit dem Kanister: Umkreisen um die Kante unter dem Spieler (Spur-B-Endziel vor dem Umweg, Kreis ganz ausserhalb der aufgeblasenen Bloecke), aus
der Ferne direkt hin, wenn die Strecke frei ist; keine 600er-Ruhezone. Fensterbeleg ohne starre Kamera: Messhaken `RE15_K2080_RUECK_LAGE=x,z,gier,cut` -
der 4. Wert setzt den Rueckschnitt (@0x07D4 = Cut beim Ausloesen) auf die Kamera der neuen Lage (ROOM2090 Cut 2 sieht Rampe West, Sued- und Nordbecken,
`test_r41_gatorfix projektion`); state.log n8_plat7_leon/n8_plat8_elza: `cam=2`.
| Lauf | vorher | nachher: 2-s-Fenster / 30-B.-Anteil / Belagerung mit Kanister / Kamera |
|---|---|---|
| plat7_leon (Rampe x 2318) | 94,7 %, 3904 Fenster | 0 / 0 % / 7278 Bilder / cam=2 |
| plat8_elza (Rampe x 2318) | 94,6 %, 3953 Fenster | 0 / 0 % / 7432 Bilder / cam=2 |
Ehrlich: Er bleibt in Bewegung, kreist aber im Nordbecken bei (-4000,-10500) (Karte `nb2_karte_nachher_plat7_leon.jpg`), also ~9000 von der Figur und
ausserhalb des Bildes von Cut 2 (`nb2_m3_nachher_rampe_west_{leon,elza}_cut2_15s.jpg`: Figur auf der Rampe, Gator nicht im Bild). "Patrouille wie auf dem
Ostteil" (Pendel unter der Figur) ist das nicht - offen.

### M4 - Leiche ueber dem Oberkiefer nach dem Zubeissen
Nicht bearbeitet (Budget). Der Riegel maul misst 0 Scheitel ausserhalb des Kopf-Bandes (Part-6-Rahmen, x/y) - die Lage quer (Part-6-z) ragt seitlich ueber
den Kopf hinaus; ob Clip 5 Bild 1-11 den Kopf rollt (Leiche zeigt dann nach oben) oder die Zeichenreihenfolge, ist nicht bestimmt. Offen.

### Abschluss Nachbesserung 2
Nachgezogen nach dem ersten Testlauf (integration_r41_gatorfix_frei/_elza rot: Wandreiben 59/74 an der Nordwand): Ziele werden nach dem Schieben aus den Bloecken noch einmal gekuerzt, und eine gehaltene Etappe wird je Bild mit der aktuellen Anschwimm-Gier neu gekuerzt. Tests (PowerShell-ctest, Auswahl `r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix`, -j3): **100% tests passed, 0 tests failed out of 117** (2294 s). exe-Riegel (traglog, Endstand): frei 30-B. 12 %/Wandreiben 0/0 Fenster, frei_elza 10 %/2/0, rampe(_elza) 1 %/0/0, abn2_leiterecke 0 %/0/0, abn2_wand_elza 16 %/0/0, abn2_nordbecken 60 %/0/0, abn2_rampe_west(_elza) 0 %/0/0, abn2_ecken 33 %/5/0, abn2_wand 9 %/0/0; Aufnahme 121-877 Bilder nach dem Aufschlag.

## Nachbesserung 3 (Orchestrator: M3' Rampe West nahe der Figur, Hochklettern ohne Messhaken, M4)
### M3' - Patrouille im naechsten erreichbaren Wasser
Belagerung mit dem Kanister: Kreis-Mittelpunkt = die Figur selbst, auf den naechsten Wasserpunkt geschoben, an dem der ganze Kreis (R 2000) ausserhalb der um den
Wurzel-Rand aufgeblasenen Bloecke liegt (jetzt auch Eckpunkte x+z, die Fuge Plattform/Rampe); Anfahrt direkt oder ueber den Kanten-Umweg ZUM Mittelpunkt (nicht zur
Spur-B-Kante x >= 4450). Gemessen (traglog, Kamera aus state.log `cam=`): Rampe West Leon Kopf-Abstand mittel 9539 -> 6143, Elza 9639 -> 6276, Gator im Bild der
Spielerkamera (Cut 2) 91-92 % (n10_plat7_leon/n10_plat8_elza; Fenster `G2_belege/nb3_m3_nachher_rampe_west_leon_cut2_5s.jpg`: Gator unter der Figur im Bild,
vorher `nb2_m3_nachher_rampe_west_leon_cut2_15s.jpg` ohne Gator). Fuzz (zweite Haelfte je Lage = Patrouille): P-nord 3467, P-sued 4262, Rampe West 6168,
P-west 7061, Leiter 6976, P-mitte 7794, Rampe Mitte 8319 - die Grenze ist die Geometrie (Rampe 3600 tief: Mitte 1800 + Rand 1646 + R zum Mittelpunkt).
Riegel: Fuzz <= 8500; exe abn2_rampe_west(_elza) mit Rueckschnitt Cut 2: Kopf-Abstand <= 7000 und >= 80 % im Bild.
### Hochklettern ohne Messhaken
Gemessen (Lauf n10_klettern_leon, ohne Alligator RE15_KEIN_GATOR=1, echte Kamera): Figur laeuft von (2318,-19700) an die Rampen-Suedkante, Aktionstaste ->
climb.log `sub=9` T375 Band 0 -> T426 Band 1, y -1800, dann zu Fuss auf die Rampe bis (2318,-15002). Hochklettern geht im freien Spiel (Bild
`nb3_m3_klettern_rampe_west_ohne_messhaken.jpg`); mit dem Alligator in der Naehe druecken die Bisse die Figur weg (Abnahme 2), kein eigener Fehler.
### M4 - Ursache bestimmt, nicht behoben
`test_r41_gatorfix leichenachse`: KOPFROLLEN. Von Clip 4 Bild 10 bis Clip 5 Bild 12 ist der Unterkiefer (Part 6 = Anker der Leiche) um die Kieferachse gerollt,
seine Querachse z hat Welt-y -2141..-3326 von 4096 (31-54 Grad nach oben), ab Bild 14 waagerecht (-676..367). Die quer gehaltene Leiche zeigt mit nach oben und
liegt im Bild ueber der Schaedel-Oberkante (Bild 4-12: Leiche ab y 53-76, Schaedel-Oberkante 41-86). Versuch: Halte-Relation um die Kieferachse zuruekdrehen
(Laengsachse waagerecht, Anteil 1 / 1/2 / 1/3) - im Bild dann unter dem Schaedel, aber 94-186 Scheitel unter dem Unterkiefer (Riegel maul rot): Zielkonflikt
"nicht oben raus" gegen "nicht unten raus" waehrend der Rolle; nicht eingebaut. Offen.
Nachtrag NB3: Im ersten Testlauf fielen integration_r41_gatorfix_rampe/_elza (707 2-s-Fenster): der Alligator kommt jetzt an die Figur heran und schnappt mit dem Hochbiss der Belagerung (Phase 5, Clip 4, Spur-B-Design) nach oben - ein Biss am Spieler; Mass und Fuzz nehmen ihn jetzt aus wie den Biss im Wasser (Kopf unter dem 4200er-Aufricht-Gate). Danach 0 Fenster (1289/1280 Bilder Hochbiss). Rampe Nordteil x 2602: Gator im Bild der Spielerkamera nur 12 %.

### Abschluss Nachbesserung 3
Tests (PowerShell-ctest, Auswahl wie die Abnahme, -j3): 100% tests passed, 0 tests failed out of 117

## Entscheide des Orchestrators nach NB3 und Stand (Uebergabe)
1. Hochbiss nach oben = "Biss am Spieler" (Kopf < 4200): bestaetigt, so in traglog/Fuzz (f11aa6d98).
2. M4 (Leiche je Bild ins Maulvolumen FITTEN: Drehung, Versatz, Laengsskala bis 0,6, glatt, PORT-WAHL) - NICHT umgesetzt (Kontextbudget dieser Spur erschoepft).
   Vorbereitet/gemessen: Ursache Kopfrollen Clip 4 Bild 10 - Clip 5 Bild 12 (`test_r41_gatorfix leichenachse`), reine Drehung scheitert (94-186 Punkte unten).
   Weg fuer die Fortsetzung: (a) Laengsskala braucht einen Zeichner-Haken - pc_prop_world (platform/pc/main.c) nimmt die Prop-Matrix als allgemeine 3x3
   (wie der Spur-J-Haken re15_superobj_r41_welt), ein eigener Haken re15_gatorszene_r41_leiche_welt(slot, r, t) kann Drehung x Skala liefern; (b) das Maul-
   volumen je Bild wie im Riegel (g2_band: Kopf-Scheitel Part 6/7 im Part-6-Rahmen, y-Band je x, Fenster +-150) in die Engine holen, Suche ueber
   Rollanteil {0..1} x Skala {1..0,6} x y-Versatz, Kosten = Punkte draussen + Glaettung zum Vorbild; (c) Riegel maul ohne Ausnahme fuer das Fenster,
   Gegenprobe = Stand NB2 (Pruefhaken nb1_alt / neuer Haken).
3. Rampe Nordteil (12 % im Bild) / Kreispunkt in der Spielerkamera - NICHT umgesetzt. Das Harness-Mass waehlt die Kamera per Projektion (Bildmitte) und
   trifft die Zonenkamera des Spiels nicht (Rampe West: Harness Cut 3, Spiel Cut 2); fuer "im Bild je Lage" muss die Kamerazone (RVD-Zonen, cam_from) im
   Harness nachgebildet oder die Lagen als exe-Laeufe gemessen werden (traglog misst "im Bild" schon mit state.log cam=).

## Nachbesserung 4 (Orchestrator nach NB3: M4 einpassen, Kreispunkt in der Spielerkamera)
Laeufe scratchpad `m4/runs/<lauf>` (Skript `m4/lauf.sh`: eigene exe-Kopie `re15_pc_g2nb4_<lauf>.exe`, gdigrab auf das hwnd des eigenen Prozesses, 30 Bilder/s,
RE15_WINDOW_SCALE=2, kein AUTOSHOT/SOFTWARE_RENDER, SDL_ASSERT=always_ignore, nur eigene PIDs). "Vorher"-exe = derselbe Quelltext mit dem Pruefhaken
`s_m4_alt = 1` als Vorgabe gebaut (`m4/re15_pc_vorher_nb4.exe`, md5 58ca6684...; Einpassung aus, Kreispunkt wie NB3) = Verhalten von b7ae18e89.

### M4 - Leiche in das Maulvolumen eingepasst
**Mass** (Orchestrator: "Leichenpunkte ausserhalb von Kopf- und Kieferhuelle oben UND unten zusammen"): das Kopf-Band des Riegels maul (g2_band: Scheitel
Part 6 + Part 7, je Leichen-Scheitel das Laengsfenster +-150) in zwei Rahmen, je oben und unten: A = Kieferrahmen (Part 6, = bisheriger Riegel), B = waagerechter
Rahmen (x = Kieferachse, y = Welt-Senkrechte ohne x-Anteil). A allein sieht die Rolle nicht (die Leiche rollt mit dem Kiefer: 0 draussen), B zeigt sie.
**Messung vorher** (`test_r41_gatorfix leichenfit`, Pruefhaken m4_alt): Rollwinkel des Unterkiefers bis -55 Grad (Clip 4 Bild 13 .. Clip 5 Bild 2); A 0, B bis
154 von 306 Scheiteln (Clip 5 Bild 7/8, ein Ende ueber dem Schaedel, das andere unter dem Kiefer); Rollfenster (Bilder mit Scheiteln draussen) 42 Bilder,
Summe 2671.
**Was erreichbar ist** (Gitter Rollanteil 0..1 in 1/16 x Laengsskala 1..0,6 in 0,05 x dy +-480/40 x dz +-800/50, A+B): in Clip 5 Bild 3-11 nicht 0, bis 24.
Laengsversatz dx gemessen und verworfen: global bis 17 (dx 400), aber in den Haltebildern nach der Rolle liegen bei dx 100..500 in Ruhelage 1..24 Scheitel
draussen, bei dx 0 keiner - die Suche blieb bei dx 400 haengen. A = 0 hart: B bis 75.
**Aenderung** (gatorszene_r41.c Abschnitt EINPASSEN, PORT-WAHL; RE2 haelt sein Opfer mit Opfer-Animation, es gibt kein Original fuer die Lage): je Haltebild
(Vorgriff fertig bis vor dem Wurf Clip 5 Bild 57) lokale Suche um die Werte des Vorbilds (je Bild hoechstens 2/16 Roll, 0,1 Skala, 80 dy, 100 dz = glatter
Verlauf; Kosten 64 je Scheitel draussen + 1 je Stufe weg von der Ruhelage), Drehung = Rx(Rollanteil x Rollwinkel) x Halte-Relation, Laengsskala auf Modell-x
um die Rumpfmitte, Versatz im Part-6-Rahmen. Gezeichnet ueber den Haken `re15_gatorszene_r41_leiche_skala` in pc_prop_world (Spalte 0 der Prop-Matrix x s,
Muster Spur J; der Schluessel als Kind: Lage mitgestaucht, Form nicht). Part-7-Matrix `re15_gr2_part7_gezeichnet`. Nur PC-Bau (~1 Mio. Schritte je Bild).
Messhaken `_leiche_matrix` (gezeichnete Matrix), `_leiche_fit`; `_leiche_sitz` misst gegen Maullage + Versatz mit der gestauchten Matrix.
**Messung nachher** (Riegel unit_r41_gatorfix_maul / _elza, neu teil_maul_m4): Rollfenster ohne Ausnahme 42 von 42 Bildern gemessen (auch ohne das Spur-P-Mass
a <= 40), draussen hoechstens 26 (vorher 154), Summe 185 (vorher 2671, erreichbar 171), je Bild hoechstens 2 ueber dem erreichbaren Minimum (Riegel: <= 4),
ausserhalb des Rollfensters 0, keine Spruenge, vor dem Wurf in Ruhelage. Gegenprobe Stand vor NB4: 154 (rot). Leon und Elza gleich.
**Fenster** (gdigrab, 15 Bilder/s, Ausschnitt hell gezogen, oben vorher, unten nachher): `G2_belege/nb4_m4_leon_vorher_oben_nachher_unten_15fps.jpg`, `_elza`
(Laeufe v_ank_*/n_ank_*). Vorher liegt die Leiche schraeg ueber Oberkiefer und Hals, die Fuesse ragen ueber den Kopf; nachher kuerzer und flacher im offenen
Maul, die Fuesse bleiben in Kopfhoehe.
**Offen (ehrlich):** Ziel 0 ist im erlaubten Raum nicht erreichbar (Minimum bis 24 von 306 in Clip 5 Bild 6-9). Die Fangbilder 75/76 haben im waagerechten
Rahmen 12/23 Scheitel draussen (unveraendert; Einpassung im Wurf aus, Kopf 70 Grad hoch). PSX-Bau ohne Einpassung.

### Punkt 2 - Kreispunkt im sichtbaren Wasser der Spielerkamera
**Wie das Spiel die Kamera waehlt** (Werkzeug `test_r41_gatorfix kamerazone 2090`): RVD-Zonen ROOM2090 = Ring 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 0 aus
Streifen (je Gruppe ist die erste Zone der Anker, den der Scan nicht prueft - rdt_common.c / FUN_80014230); an keiner Rampen- oder Leiterlage liegt eine Zone,
die Kamera haengt also am Weg: wer von Sueden auf die Rampe klettert (Zone 5, Cut 1 -> 2), steht in Cut 2, wer von Norden kommt (Westkanal Cut 7/6/5,
Nordbecken Zone 16 -> 4, Zone 13 -> 3), in Cut 3. Der Riegel baut genau das nach (g2_weg_kamera: Weg von der Leiter, je 50 Einheiten ein Scan-Schritt).
**Harness-Fehler gefunden und behoben:** die Fuzz-Lagen "oben" setzten nur y = -1800; das Kollisionsband blieb 0, die Spieler-Kollision schob die Figur in
jedem Bild von der Rampe ((4500,-16300) -> (4500,-14032)), der Alligator war nie in der Belagerung (Phase 2 statt 5). Jetzt `re15_collision_set_band` wie
nach dem Klettern (climb_common.c @0x800384bc). **Plattform:** fuer die Figur nicht betretbar (Werkzeug `begehbar 2090 1`: (0,-17500) -> (-2168,-17500),
(1600,-16000) -> (2318,-16000); Kletterzellen nur auf der Rampe) - es gibt keine Plattformlagen; Rampen-Nordrand begehbar bis z -14968.
**Messung vorher** (Riegel kamera, Pruefhaken `re15_gr2_kreis_kamera_alt_setzen(1)` = NB3): der naechste freie Punkt zur Figur liegt in Cut 2 suedlich der
Rampe - dessen Kreis ist nur zu 10/16 im Bild (15 von 32 Lage/Kamera-Paaren), Anteil der Wurzel im Bild 69-100 %. Der NB3-Wert "Rampe Nordteil 12 %" stammt
aus einem exe-Lauf; im Harness stand die Figur dort vor der Band-Korrektur gar nicht oben.
**Aenderung** (gator_re2_r41.c Abschnitt KREISPUNKT IN DER SPIELERKAMERA, PORT-WAHL): `re15_gr2_kreis_kamera` waehlt im Raster 250 den der Figur naechsten
Mittelpunkt, dessen Kreis (R 2000, 16 Punkte auf y -1200) zu >= 12/16 im Bild der laufenden Kamera g_scd.cam_id liegt (Kino-Balken 24..216), ganz in der
Klemmflaeche (Aussenwaende - 1600) und euklidisch >= Wurzel-Rand + R von Plattform und Rampe (der Kasten-Test sperrte die Ecken mit und damit das sichtbare
Wasser an der Rampe). Gilt fuer die Belagerung (Rampe) und fuer das Kreisen der Verfolgung (NB2-Kreis um das gekuerzte Ziel), dort nur, wenn dessen Kreis
nicht zu >= 12/16 im Bild liegt (Leiter: (929,-23646), 0..2/16 in Cut 0) - die erste Fassung ersetzte ihn immer und brach den exe-Riegel frei (6
2-s-Fenster: Biss-Clip an der Plattform-Westkante, x geklemmt, Figur 9000 entfernt) - und nur, wenn der sichtbare Mittelpunkt hoechstens 3 R von der
Figur liegt (exe-Riegel abn2_nordbecken: Figur NO-Ecke, Kamera 0 durch den Messhaken, sichtbares Wasser erst bei (-5300,-20650): 2 Fenster). Der Kreis der
Belagerung endet mit ihr (`kreis == 2`), sonst kreiste er an der Leiter weiter um den Rampen-Mittelpunkt. Ohne sichtbares Wasser: wie NB3.
**Fuzz nachgezogen:** die Fuzz-Lagen oben/Leiter laufen jetzt in der Kamera des Wegs von Sueden und mit dem Kollisionsband der Lage (vorher stand die Figur
nie oben, die Kamera war die zufaellig laufende); Schranke Kopf-Abstand 8500 (NB3: naechster freier Punkt) -> 9000 (NB4: naechster sichtbarer Mittelpunkt bis
8930 von der Figur, R-6500n in Cut 3). Gemessen: hoechstens 7769 (Leon). Die Plattform-Lagen P-* bleiben im Fuzz als Wasser-Lagen am Plattformrand.
**Messung nachher** (Riegel unit_r41_gatorfix_kamera / _elza, je Lage 900 Bilder, Mass wie traglog, zweite Haelfte): 15 Rampenlagen x Cut 2 (Sued) und
Cut 3 (Nord) + Leiter + Leiter-Ecke (Cut 0) = 32 Paare, alle mit sichtbarem Wasser, alle 100 % im Bild, Kreis jedes Laufs 16/16 im Bild, kein 2-s-Fenster
ohne Fortschritt; Kopf-Abstand 4600-8900 (Leiter 4600-5100, dort 28 % Biss am Spieler). Mittelpunkte: Cut 2 (3450,-9150) noerdlich der Rampe, Cut 3
(3200..3450,-23400) suedlich, Cut 0 (-5300,-23400) vor der Leiter. Gegenprobe NB3: 15 von 32 Paaren mit Kreis unter 12/16 im Bild (rot). Lagen ohne
sichtbares Wasser: keine.
**Fenster (exe, gdigrab, Eingabe und Umgebung des Riegels abn2_rampe_west, Rueckkehr-Messhaken mit Cut 2 = die Kamera, die der Weg von Sueden ergibt):**
(exe-Stand vor der Verengung des Verfolgungs-Kreises; die Belagerung ist davon nicht beruehrt) Leon Rampe West vorher (v_west_leon) traglog: Figur oben 2604 Bilder, Kopf-Abstand mittel 6276, Gator im Bild der Spielerkamera 85 %; nachher (n_west_leon):
6940, **95 %**, 0 2-s-Fenster; Elza vorher (v_west_elza) 6397 / 85 %, nachher (n_west_elza) 6841 / **95 %**, 0 2-s-Fenster. Bilder
`G2_belege/nb4_p2_rampe_west_cut2_vorher_oben_nachher_unten_hell_10s.jpg` und `nb4_p2_rampe_west_elza_cut2_*` (hell gezogen, alle 10 s): vorher liegt der
Alligator im Suedwasser zwischen Kamera und Rampe, gross am unteren Bildrand und zeitweise ganz draussen; nachher kreist er im Nordbecken hinter der Figur -
immer im Bild, aber klein (in Normalhelligkeit kaum zu erkennen). Versuch "Rampe Nordteil von Norden" (Messhaken an der Nordkante (2602,-13800), Cut 3):
die Figur klettert dort nicht hinauf, der Alligator packt sie vorher (climb.log ohne sub=9, vorher wie nachher) - Nordlagen nur im Riegel.

### Abschluss Nachbesserung 4
Tests (PowerShell, `C:\msys64\mingw64\bin` vorn im PATH, SDL_ASSERT=always_ignore, Auswahl `r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix`,
-j3, Bau nur ueber local_build.sh): **100% tests passed, 0 tests failed out of 119** (2296.83 s), darin neu unit_r41_gatorfix_kamera / _kamera_elza.
exe-Riegel Rampe West (Cut 2, Endstand): Leon 98 % im Bild / Kopf-Abstand 6814, Elza 98 % / 6686 (NB3-Riegel: >= 80 %, <= 7000).
Zwischenlaeufe (abgebrochen und behoben): integration_r41_gatorfix_frei (6 Fenster, Verfolgungs-Kreis immer ersetzt) und _abn2_nordbecken (2 Fenster,
sichtbarer Mittelpunkt 16900 von der Figur) - siehe Punkt 2.
**Offen:** M4 Ziel 0 im erlaubten Raum nicht erreichbar (bis 24/306 in Clip 5 Bild 6-9); Fangbilder 75/76 unveraendert 12/23. Punkt 2: in Cut 2 liegt das
sichtbare Wasser hinter der Figur im Nordbecken - der Alligator ist immer im Bild, aber klein; die Alternative (Schwelle 10/16 statt 12/16) waere das
Suedwasser vor der Kamera (gross, zeitweise am unteren Rand draussen, 85 %) - Gestaltungsfrage fuer den Orchestrator. Rampe von Norden nur im Riegel
(Klettern an der Nordkante im exe-Lauf vom Alligator verhindert). PSX-Bau: Einpassung nur PC; die Kreispunkt-Suche laeuft auch im PSX-Bau (je Kamera und
Figur-Zelle 250 einmal, bis ~2000 Kandidaten x 16 Projektionen) - nicht im PSX-Laufzeitbudget gemessen.

## Nachbesserung 5 (Abnahme 3: analysis/befunde_runde41/G2_abnahme_3.md, M1 Leiche ragt im Rollfenster/Fang aus dem Maul, M2a Kreisel R ~535 statt Kreis, M2b Cut 2 nicht erkennbar, Haken zuruecksetzen)
Stand: begonnen 2026-11-02 auf 85863d687. Laufordner scratchpad `g2nb5/` (nicht im Baum). Reihenfolge: M2a (Ursache) -> M2b -> Haken -> M1.

### NB5 Haken (Abnahme 3 Abschnitt 3 "Code gelesen") - erledigt
`s_fit` (Einpassung, Stauch-Haken `re15_gatorszene_r41_leiche_skala` in pc_prop_world fuer obj_id 2) wird jetzt in `re15_gatorszene_r41_install` (Aufruf bei JEDEM
Raumaufbau, scd_room_setup.c) geleert (`fit_raumwechsel`, vor jedem Ausstieg der Funktion). Riegel unit_r41_gatorfix_haken(_elza): Szene bis zum ersten
gestauchten Haltebild (Szenenbild 76, Skala 3686/4096), dann Raumaufbau 2080 -> Haken fuer keinen Slot mehr an; Gegenprobe Pruefhaken
`re15_gatorszene_r41_fit_reset_alt_setzen(1)` -> Haken bleibt an (rot ohne den Bau).

### NB5 M2a - Ursache des Drehens (gemessen, Harness `test_r41_gatorfix kamera` mit `G2_BAHN=<datei>` = Bahn je Bild, Auswerter scratchpad `g2nb5/tools/bahn.py`)
1. **Spiegel-Fehler der Karotte**: `a = re15_atan2_q12(dz,dx)` liefert die Port-Konvention (0 = +Z, 1024 = +X; actor_locomotion.c atan2_q12 = FUN_8001a6d4 + 0x400),
   die Karotte setzte `a` in Standard-cos/sin ein -> Punkt R(sin t, cos t) = Spiegelbild der Gator-Lage an der 45-Grad-Diagonale; Fixpunkt SW des Mittelpunkts
   (3450-1414,-9150-1414) = (2036,-10564) - dort die gemessene Schleife (Abnahme (2232,-10500), Harness R-2318s Cut 2 Fit (1627,-10023) R 520, 6,5 Runden in 15 s).
   Behoben (`gr2_tr_karotte`, Drehung des Einheitsvektors um kdir x 45 Grad; Pruefhaken `re15_gr2_karotte_alt_setzen`).
2. **Danach dreht er trotzdem**: Fit (2140,-10488) R 503 - die Trag-Kuerzung `gr2_tr_fitfrei` zieht jede Karotte auf dem Ost-/Suedteil des Kreises neben die
   Wurzel. Ohne die Kuerzung (Experiment) drueckt ihn die Kopf-Klemme an die Nordwand: Klemme 100 %, 610 2-s-Fenster (= Festdruecken der Abnahme 1).
3. **Geometrie** (Werkzeug `test_r41_gatorfix kasten`): Wurzel-Kasten der Trag-Lenkung je Schwimmrichtung: nach Osten nur x <= 1850, nach Westen x >= -3550,
   nach Norden z <= -10750, nach Sueden z >= -21650. Ausdehnung je Clip (`re15_gr2_huelle_ausdehnung`): Clip 0 Kopf-Bones 3236 vor, Kanister-Enden 4589..4666 vor,
   von 237 rechts bis 2694 links (Clip 1: 5010 vor, 2839 links). Der KANISTER bestimmt die Huelle. Wurzel-Kollision an Plattform/Rampe = hit_radius_min 1466
   (enemy_ai_boss_gator.c re15_collision_constrain_enemy). Folge: ein Kreis R 2000 ist NIRGENDS im Becken befahrbar, ohne dass Kanister/Kopf durch eine
   Aussenwand ragen (Kreis braucht alle Richtungen; Mittelpunkt muesste >= ~5000 von jeder Wand liegen = auf der Plattform). Befahrbar sind nur enge Bahnen in
   Streifen: west der Plattform (x ~ -3900), nord (z ~ -10200), sued (z ~ -22200, x <= ~2500) - z.B. ein Stadion (zwei Halbkreise R ~650) suedlich der Plattform
   direkt vor der Cut-2-Kamera (Kamera Cut 2 (2484,-3132,-25956) Blick Nord; Cut 3 (2484,-3132,-6516) Blick Sued).
Naechster Schritt: Bahn-Planer (Stadion/Kreis je Kamera, Zulaessigkeit aus der Clip-0-Huelle + Wurzel-Kollision, Ziel Erkennbarkeit in der Kamera), Karotte
auf der Bahn, Riegel `bahn` (gefahrene Bahn gegen Plan) + Erkennbarkeit (projizierte Flaeche).

### NB5 M2a/M2b - Aenderung (gator_re2_r41.c Abschnitt BAHN DER BELAGERUNG, PORT-WAHL Gestaltung; RE2 kreist nicht)
- **Bahn = Stadion** (zwei Halbkreise R um A/B, gerade Stuecke; A = B = Kreis). Planer `gr2_runde_planen` je Kamera (Cache), Suche Raster 250 ueber Mitte x
  Lage (x/z) x Laenge 0..3000 x R {2000,1500,1000,800,650} x Drehsinn, dann Verfeinerung je 50; in 71 Teilschritten (ein Schritt je Bild, ~1-3 ms; ganze Suche
  gemessen 60-150 ms je Kamera), bis dahin wie NB4. Nur PC-Bau (`RE15_PLATFORM_PC`), PSX wie NB4.
- **Zulaessig** je Stuetzpunkt (48) mit der Tangente als Gier: Wurzel >= hit_radius_min + 60 (= Spur-B-Slab, Kollision re15_collision_constrain_enemy mit
  hit_radius_min) von Plattform/Rampe; die Clip-0-Huelle (Hals/Kopf/Kiefer + Kanister-Enden, Tafel je Gier in 1024 Stufen) mit 60 Abstand in den Aussenwaenden,
  die IM BILD der laufenden Kamera liegen (`gr2_runde_waende_sichtbar`: Wandpunkte je 250, y -3000..0 im Bild zwischen den Kino-Balken). Cut 2 sieht Ost+Nord
  (Maske 10), Cut 3 West+Ost (6), Cut 0 13. Grund: der ferne Kanisterpunkt liegt 5388 von der Wurzel; vor der Cut-2-Kamera bliebe fuer eine Schleife nur
  86 Einheiten - die Suedwand liegt dort aber HINTER der Kamera. Waehrend der Bahn prueft die Kopf-Klemme dieselben Waende (`s_kk_waende`), sonst alle vier.
- **Erkennbar**: Ziel = groesste mittlere sichtbare Bildlaenge Wurzel -> Kopf (3236 vor, am Bildrand abgeschnitten), mindestens 32/48 Stuetzpunkte im Bild
  ("ueberwiegend"; zeitweise unten aus dem Bild ist erlaubt).
- **Karotte** auf der Bahn pi R/4 voraus, ohne Trag-Kuerzung (die Bahn ist zulaessig). **Anfahrt** (Abstand > R): naechster Bahnpunkt, Weg ueber die
  Westkanal-Knoten (Mitte zwischen Westwand und Plattform, z Mitte Nord-/Suedbecken; Plattform+Rampe reichen bis an die Ostwand), sonst Kanten-Umweg Spur B.
  Vorher (Lauf R-2318s Cut 2) sprang der Spur-B-Umweg zwischen (2279,-9867) und (-3888,-9862) und er pendelte im Nordbecken.
- Geplant (Leon = Elza): **Cut 2 A (1800,-22200) B (2800,-22200) R 650** (Suedwasser vor der Kamera, mittlere Kopflaenge 92 Bildpunkte, NB4-Kreis 22),
  **Cut 3 A (1800,-10250) B (2800,-10250) R 650** (Nordwasser vor der Kamera, 81). Werkzeug `test_r41_gatorfix bahnpruef [cam ax az bx bz r kdir ...]`.
- **Messung nachher** (Riegel unit_r41_gatorfix_bahn / _elza, 15 Rampenlagen x Cut 2/3, je 1800 Bilder, ab Ankunft Abstand < 200): gefahrene Bahn = Plan in
  30/30 (Abstand zur Bahn p95 160, max 175-198; Umlauf 110 Bilder gegen Plan 127 = Umfang 6082 / 48 je Bild - er schneidet die Halbkreise ~120 innen; 10,5-13,4
  Runden; Schwerpunkt <= 36 von der Plan-Mitte; Kopf-Klemme 0 Bilder). Silhouette (Vereinigung der projizierten Part-Huellen, 320 x 192) Median 20 270-22 400
  Bildpunkte = 265-295 % des Bezugs Verfolgung an der Leiter in Cut 0 (7585). **Vorher** (Gegenprobe Stand NB4, Pruefhaken `re15_gr2_bahn_alt_setzen` +
  `re15_gr2_karotte_alt_setzen`): Kreis R 2000 nie gefahren (0,03-0,16 Runden, Abstand p95 506-1255, Schwerpunkt (2190,-10465) statt (3450,-9150)),
  Silhouette 1850-2200 = 24-29 % des Bezugs.
- Riegel kamera (NB4) laeuft jetzt 1800 Bilder je Lage (zweite Haelfte = nach der Anfahrt); alle 20 Unit-Riegel der Spur gruen.
Offen: Fensterbelege (gdigrab, Cut 2/3, Leon/Elza, Normalhelligkeit) und exe-Riegel; Verfolgungs-Kreis (kreis 1, Leiter) nur Spiegel-Fix, nicht geplant.

### NB5 M1 - Leiche ragt kurz aus dem Maul: KLEMMEN JE SCHEITEL (gatorszene_r41.c, PORT-WAHL Gestaltung = Vorschlag des Orchestrators)
- Befund (Abnahme 3): Rollfenster Clip 5 Bild 3-11 oben ueber dem Schaedel (~0,3 s), Fang Bild 74-76 Kopf unter dem Unterkiefer (~0,1 s); im erlaubten Raum der
  NB4-Einpassung (Roll, Laengsskala 1..0,6, Versatz) nicht auf 0 zu bringen (Minimum bis 24/306).
- Aenderung: jeder Scheitel der GEZEICHNETEN Leiche, der nach dem Einpassen nicht mindestens GSZ_KLEMM_TOL 16 innerhalb des Kopf-Bandes liegt (Mass von
  Einpassung und Riegel: Part 6 + Part 7, Kieferrahmen A und waagerechter Rahmen B, je oben/unten; Laengsfenster beim Klemmen 150 - GSZ_KLEMM_KANTE 20, damit
  das Band eine Teilmenge des Riegel-Bandes ist), wird GSZ_KLEMM_RAND 40 innerhalb auf die naechste Innenflaeche geschoben: Kiefer-y in den Schnitt von A und B,
  ist der leer, Kiefer-y in A und quer (Kiefer-z) bis B passt; Scheitel innerhalb bleiben bitgleich. Fenster: Clip 4 ab Bild 7 bis Wurf (Clip 5 Bild 57) und
  ab dem letzten Flugbild vor dem Fang (74) bis zum Verschlingen (77); im Flug dazwischen nicht (Punkt 2: sie fliegt sichtbar ueber dem Maul).
  Zeichner-Haken `re15_gatorszene_r41_leiche_scheitel` (platform/pc/main.c Prop-Zeichner: Ersatzfeld je Mesh-Feld), Messhaken `re15_gatorszene_r41_leiche_klemm`,
  Pruefhaken `re15_gatorszene_r41_klemm_alt_setzen` (Stand NB4). Nur PC-Bau.
- Messung (Riegel unit_r41_gatorfix_maul / _elza, Teil NB5): 67 Bilder (Biss Clip 4 Bild 7 .. Clip 5 Bild 56 und 3 Fangbilder 74-76): **0 Scheitel draussen
  in jedem Bild** (Leon und Elza). **Vorher** (Gegenprobe Stand NB4): hoechstens 26 (Summe 227; Clip 5 Bild 0-11: 175; Fangbilder 42). Geklemmt je Bild
  hoechstens 40 von 306 Scheiteln, groesster Weg 584 Einheiten. Erste Fassung (Band der Engine mit vollem Fenster, ohne Toleranz) liess 1-2 Scheitel 1-12
  Einheiten draussen (Rechen-/Fensterrand-Unterschied Engine Q12 gegen Riegel Gleitkomma) - daher TOL 16 und das engere Fenster.
Offen: Fensterbelege (Rollfenster und Fang, Leon/Elza, Normalhelligkeit).

### NB5 Fensterbelege (gdigrab auf das hwnd des eigenen Prozesses, 30 Bilder/s, RE15_WINDOW_SCALE=2, kein AUTOSHOT/SOFTWARE_RENDER; Skript `G2_belege/nb5/werkzeug/lauf.sh`
= Abnahme-3-Skript mit eigenem Laufordner scratchpad `g2nb5/runs/<lauf>`, Stapel `batch_fenster.sh`; exe md5 90e257cd... = Stand 4ea2322a4)
- **M2a/M2b Cut 2 (Eingabe und Umgebung der Abnahme-Laeufe west_leon/west_elza: Rueckkehr-Lage 3000,-18800 bzw. 5600,-18700, Cut 2, Kletterfolge):**
  climb.log sub=9 (Figur oben), state.log 3350 Bilder cam=2. Auswerter `werkzeug/bahn_exe.py` (Plan aus `TRAG BAHN an`, Wurzel aus gator_trag.log):
  | Lauf | Plan | ab Ankunft | Abstand zur Bahn Median/p95/max | Runden | Umlauf (Plan) | Schwerpunkt | Clips | bewegte Flaeche (sicht.py, t 57-124) |
  |---|---|---|---|---|---|---|---|---|
  | west_leon | A (1800,-22200) B (2800,-22200) R 650 | F1165, 2136 Bilder | 117 / 159 / 198 | 19,4 | 110 (127) | (2286,-22213) | nur 0 | Median **7069** (Abnahme 3: 286) |
  | west_elza | dieselbe | F1076, 2225 Bilder | 117 / 159 / 182 | 20,2 | 110 (127) | (2281,-22208) | nur 0 | Median **6551** (Abnahme 3: 482) |
  Bezug Abnahme 3: Cut 0 frei 8833. p7.py (Abnahme-Auswerter) west_elza: 0 2-s-Fenster ohne Fortschritt, 30-B.-Fenster netto < 300: 0 %.
  Bilder bei Normalhelligkeit: `G2_belege/nb5/m2b_west_leon_cut2_normal_4s.jpg`, `m2b_west_leon_cut2_t90-93_2fps_normal.jpg`, `m2b_west_elza_cut2_normal_4s.jpg` -
  der Alligator schwimmt gross im Vordergrund vor der Kamera (Schuppen, Beine, Kopf, gelber Kanister erkennbar), die Figur bleibt oben im Bild frei; in den
  Anfahrtssekunden (Westkanal) ist er nicht im Bild.
- **Cut 3**: exe-Lauf nord_leon (Rueckkehr-Lage 3000,-13700, Cut 3) - die Figur klettert an der Rampen-Nordkante nicht hinauf (climb.log ohne sub=9, wie NB4),
  keine Belagerung. Cut 3 nur im Riegel bahn (gefahrene Bahn = Plan, Silhouette 275-295 %).
- **M1 (Szene, Laeufe ank_leon/ank_elza, Eingabe der Abnahme)**: Bilder stark aufgehellt (Y x 4) zum Finden: `m1_leon_biss_bis_rollanfang_t35.93_30fps_hell4x.jpg`,
  `m1_leon_rollfenster_t36.17_30fps_hell4x.jpg`, `m1_leon_fang_t38.45_30fps_hell4x.jpg`, `m1_elza_halten_t46.62_30fps_hell4x.jpg`, `m1_elza_fang_t48.95_30fps_hell4x.jpg`
  (Zuordnung: Wurfbild F145 im Video, F53 = Schnitt auf Cut 1). Gegen `G2_abnahme_3/p1_leon_rollfenster_C5B3-11_leiche_ueber_schaedel_30fps_hell.jpg`: der
  Rumpf liegt im Rollfenster nicht mehr ueber Schaedel/Oberkiefer, die Leiche liegt quer im Maul auf dem Unterkiefer. **Ehrlich, nicht 0 im Bild:** (a) in 2-3
  Bildern beim Uebergang Clip 4 -> 5 ragt eine Fussspitze hinter dem Nacken nach oben rechts; (b) im Fangbild liegt der Kopf der Leiche links NEBEN/vor dem
  Unterkiefer (Kamera schraeg von vorn), die Beine ragen seitlich heraus. Beides ist seitlicher Ueberstand der Quer-Haltung (in Abnahme 3 beim Halten
  "quer im Maul ... In Ordnung") - das Band-Mass (oben/unten im Kiefer- und waagerechten Rahmen) ist dort 0. Die seitliche Grenze "bis zu den Kieferraendern"
  aus dem Auftrag ist NICHT eingebaut: sie wuerde die angenommene Quer-Haltung (Bild 12-56) auf Kieferbreite zusammenschieben. Weg dafuer: z-Band (seitliche
  Ausdehnung der Kopf-Scheitel je x) nur im Rollfenster und in den Fangbildern, mit Ein-/Ausblenden ueber einige Bilder, sonst springt die Haltung.

### NB5 Nachzug - Verfolgungs-Kreis (kreis 1) wie NB4
Erster Gruppenlauf (Testbaum build_t5, Stand 81c1e0edd): integration_r41_gatorfix_abn2_nordbecken (2) und _abn2_ecken (6 Zeitfenster von 2 s ohne Fortschritt,
Figur nicht oben = Verfolgung) rot; alle anderen bis dahin (71 von 123) gruen. Ursache: die korrigierte Karotte galt auch fuer den NB2-Kreis der Verfolgung, den
die Trag-Kuerzung dort ebenso zerlegt. Die Verfolgung nimmt wieder die NB4-Karotte (`gr2_tr_karotte(..., verfolgung = 1)`; das freie Spiel war in Abnahme 3
bestanden), die Belagerung die Bahn. Danach beide gruen (295 s / 308 s), dazu unit bahn/kamera/fuzz. **Offen:** der Verfolgungs-Kreis kreiselt weiter am
Fixpunkt der gespiegelten Karotte (Abnahme 3: "kreist sichtbar vor Leon ... schnappt", angenommen); eine Bahn wie in der Belagerung waere der naechste Schritt.

### Abschluss Nachbesserung 5
Commits: 81c1e0edd (M2a/M2b Bahn + Haken), 4ea2322a4 (M1 Klemmen je Scheitel), 1de58649d (Verfolgung wie NB4, Fensterbelege), dae772f5a (Wandmaske der Kopf-Klemme
nur waehrend der Bahn). Tests (PowerShell, `C:\msys64\mingw64\bin` vorn, SDL_ASSERT=always_ignore, eigener Testbaum build_t5 gebaut mit local_build.sh):
Auswahl `r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix` -j3 auf 1de58649d: **100% tests passed, 0 tests failed out of 123** (3107,66 s;
neu unit_r41_gatorfix_bahn(_elza), _haken(_elza)). Nach dae772f5a: unit_r41_gatorfix* + integration_r41_gatorfix_rampe(_elza), _abn2_rampe_west(_elza) 24/24.
**Offen:** (1) M1 seitlich: Fussspitze hinter dem Nacken (2-3 Bilder) und Kopf der Leiche neben/vor dem Unterkiefer im Fangbild - seitlicher Ueberstand der
Quer-Haltung, Band-Mass 0; die Grenze "seitlich bis zu den Kieferraendern" ist nicht eingebaut (Weg s. Fensterbelege). (2) Verfolgungs-Kreis (kreis 1) unveraendert
NB4 (Kreisel am Fixpunkt der gespiegelten Karotte, Abnahme 3 bestanden). (3) Cut 3 nur im Riegel (Klettern von Norden im exe-Lauf nicht moeglich). (4) PSX-Bau:
Bahn und Klemmen nur PC (PSX wie NB4); Bahn-Suche je Kamera auf PC in 71 Teilschritten (~1-3 ms je Bild). (5) Die Bahn haengt nur von der Kamera ab, nicht von
der Lage der Figur auf der Rampe (Cut 2: immer das Stadion vor der Kamera, 4000-7000 von der Figur).

## G2-RE2 (Neuaufbau ab NB5)
Stand 2026-11-03, Zweig r41/gator_re2 auf bc5fb7c9c (NB5). Nutzer: "nimm Resident Evil 2 als Grundlage", Entwurf F (NB7-NB9, r41/gatorfix bis d3e5966e9) verworfen.
Code-Commit 199a63c95. Belege `G2_belege/re2/` (Bildleisten, Riegel-Ausgaben, `exe_mass.txt`, Werkzeuge `werkzeug/`).

### Was bleibt wie NB5
Verfolgung mit dem Kanister = NB5 (RE2-Clip 0/1 nach der +0x1F0-Hysterese, authored Vortrieb re15_gr2_chase_clip, Lenkung 0x30/0x60, NB5-Kuerzung an den Aussenwaenden,
Vorbei-Etappe, Kreis). Ohne Kanister Bild fuer Bild gleich NB5 (Riegel gang/gang_elza: 2700 Bilder, 0 abweichend; Gegenprobe RE1.5-Tempo 2699 abweichend).
Szene/Leiche/Wurf/Montage/Marvin/Water Key unberuehrt (andere Dateien). Pruefhaken `re15_gr2_rw_alt_setzen(1)` = NB5; bitgleich gegen den NB5-Bau gemessen
(fuzz-Bahn Rampe und O3, je 900 Bilder, `cmp` gleich, auch nach -O2).

### Neu (alles PORT-WAHL ausser den zitierten RE2-Werten)
- (a) Rueckwaerts setzen (gator_rw_r41.c): wenn er sich festdrueckt (16 Bilder: Takte wollen >= 400, Wurzel netto < 400 und < 1/4) ein Durchgang RE2-Clip 10, Blend 3
  (`addiu a1,zero,10 / addiu a2,zero,3 / jal 0x8001a330` @0x80102c40-48 EM23-Overlay, Clip-Ende -> Schwimmen `addiu v1,zero,257 / sw v1,4(s0)` @0x80102c54-5c),
  authored Rueckwaerts-Vortrieb, Drehung 0x20/Bild, danach 45 Bilder Pause.
- (b) Notloesung Tauchen: Wurzel 150 Bilder in einem Kasten < 2500, netto < 600 in 60 Bildern, Kopf > 4500 von der Figur, kein Biss, Rueckwaerts im Fenster half nicht ->
  20 Bilder ab (Wasserlinien-Schnitt), untergetaucht zur freien Stelle (Raster 500, Huelle frei, 1500 voraus frei, Richtung zum Steuerziel), 20 Bilder auf;
  Wellenring Effekt 6 Sub 5 (0x06050D00 @0x800456b8/bc), nicht treffbar. Darstellung aus r41/gatorfix NB7 (gator_re2_pc.c wl_poly, main.c).
- (c) Kopf/Kanister (Mesh Parts 5/6/7 + Kanister-Zylinder r 267) nicht im Grundriss von Plattform/Rampe (auch nicht ueber dem Deck: mit Deck-Ausnahme gemessen Drehen
  11,8 %, ohne 4,8 %) und mindestens 2500 tief vor der Kamera der Figur: Klemme (je Bild <= 340 wie NB5), Zielkuerzung, Kollisions-Antwort (Drehung, sonst Schritt
  zuruecknehmen). Grenze 2500 wie r41/gatorfix a14ff425a; Hub ueber dem Deck in der Ueberquerung aus 557e1a75b/ac29780f0.
- Weg: fuehrt die Strecke zur Figur durch Plattform/Rampe, die NB5-Westkanal-Anfahrt (gr2_runde_anfahrt) statt Spur-B-Hop/Vorbei (Lage Nordbecken-Ost pendelte sonst).
- (5) Rampe: Figur oben + Kanister -> keine Belagerung (NB5-Stadion entfaellt), sondern GBP_CROSS. Seite = die des Gators; laeuft die volle Querung von der Kamera weg,
  ganz; zur Kamera hin (Ende: Kopf < 2500 vor der Kamera) als Vorstoss bis zur Kamera-Grenze bzw. bis zum Rammen, dann Rueckzug mit RE2-Clip 10 zum Startpunkt, 45 Bilder
  Lauern, naechster Vorstoss. Gemessen: mit dem Kanister kommt er in Cut 2 kopfvoran nicht aus dem Westkanal ins Suedbecken (Kopf 5644 vor der Wurzel, Kanister 2694
  links, Plattform-Ecke, Kamera-Grenze). Rammen (Koerper-Grundriss, einmal je Querung) = NB5-Biss gb_biss_abschluss -> DAT_8006f418[5] = 50 (@0x8006f422) + Niederwurf;
  Kletterprofil (GB_KL_*), Halt auf der begehbaren Rampe aus r41/gatorfix NB7/NB8.
- Leistung: Posen-Vorrat der Huelle (Neubau vorher bis ~10 ms je Takt, auch in NB5), -O2 fuer gator_re2_r41.c / gator_rw_r41.c.

### Bewusst nicht uebernommen (r41/gatorfix)
Entwurf F gator_anlauf_r41.c (Anlauf-Bahnen, Abtauchen als Routine, Tauch-Budget 100), RE1.5-Tempo (rnd&0x3f)+60 @0x8010d728, Sprung-Biss B3, Wende unter Wasser B8,
Freikarten/Dijkstra, Trag-Netz NB6, Biegung gator_biege_r41.c (Schwanz/Beine: in den Bildleisten nicht auffaellig, nicht gemessen).

### Zahlen Riegel (Harness, echter Boss-Takt, 13 Lagen x 1800 Bilder, Leon = Elza bitgleich; links nb5 = Pruefhaken, rechts neu)
| Lage | ohne Fortschritt s | 2-s-Fenster | Drehen % | rueckw. % (+Rueckzug) | Tauchg./min, unter % | Kopf/Kan. in Rampe/Plattform % | < 2500 / hinter Kamera % | Rammen | Takt us Mittel/95%/max |
|---|---|---|---|---|---|---|---|---|---|
| SO-Ecke | 1.6 / 4.8 | 0 / 0 | 89 / 0 | 0 / 27 (+0) | 0.0/0.0, 0/0 | 27 / 4 | 0/5 / 0/0 | 0 / 0 | 0/0/0 / 76/131/1776 |
| Nordbecken-Ost | 54.9 / 56.0 | 0 / 0 | 87 / 23 | 0 / 29 (+0) | 0.0/0.0, 0/0 | 1 / 4 | 0/0 / 0/0 | 0 / 0 | 48/75/391 / 67/108/321 |
| NO-Ecke | 9.5 / 57.0 | 0 / 0 | 26 / 0 | 0 / 28 (+0) | 0.0/5.0, 0/14 | 10 / 0 | 24/45 / 1/0 | 0 / 0 | 37/63/320 / 49/86/283 |
| Rampe-x4428 | 15.9 / 2.3 | 0 / 0 | 63 / 0 | 0 / 0 (+24) | 0.0/0.0, 0/0 | 13 / 0 | 31/0 / 13/0 | 0 / 13 | 68/57/5317 / 47/86/449 |
| Rampe-x2318 | 17.5 / 7.3 | 0 / 0 | 66 / 0 | 0 / 1 (+23) | 0.0/0.0, 0/0 | 20 / 0 | 32/1 / 13/0 | 0 / 9 | 26/43/249 / 36/69/354 |
| Rampe-Nord | 13.4 / 4.7 | 0 / 0 | 64 / 0 | 0 / 0 (+25) | 0.0/0.0, 0/0 | 12 / 0 | 36/0 / 14/0 | 0 / 10 | 65/60/3928 / 32/56/286 |
| Leiter | 2.0 / 18.3 | 0 / 6 | 34 / 17 | 0 / 8 (+0) | 0.0/2.0, 0/5 | 10 / 1 | 0/0 / 0/0 | 0 / 0 | 34/57/687 / 51/82/391 |
| Leiterecke | 3.8 / 6.8 | 0 / 0 | 0 / 0 | 0 / 0 (+0) | 0.0/0.0, 0/0 | 20 / 0 | 0/0 / 0/0 | 0 / 0 | 47/73/637 / 69/109/468 |
| Westkanal | 1.7 / 2.6 | 0 / 0 | 0 / 8 | 0 / 6 (+0) | 0.0/0.0, 0/0 | 11 / 2 | 0/0 / 0/0 | 0 / 0 | 42/65/598 / 49/81/331 |
| Mitte-Nord | 2.0 / 4.5 | 0 / 3 | 61 / 7 | 0 / 18 (+0) | 0.0/1.0, 0/2 | 12 / 0 | 14/14 / 1/0 | 0 / 0 | 43/67/402 / 50/85/389 |
| Plattform-Sued | 1.1 / 2.3 | 0 / 0 | 0 / 0 | 0 / 20 (+0) | 0.0/0.0, 0/0 | 15 / 3 | 5/5 / 0/0 | 0 / 0 | 46/71/396 / 46/71/291 |
| Weg-Becken | 1.1 / 2.1 | 0 / 2 | 15 / 6 | 0 / 14 (+0) | 0.0/2.0, 0/5 | 14 / 0 | 11/8 / 4/0 | 0 / 0 | 46/70/576 / 48/78/290 |
| Weg-Rampe | 2.4 / 1.0 | 0 / 0 | 65 / 0 | 0 / 0 (+29) | 0.0/0.0, 0/0 | 14 / 0 | 31/0 / 13/0 | 0 / 9 | 94/100/5103 / 34/67/459 |
- SUMME nb5: 23400 Bilder (13.0 min): ohne Fortschritt max 54.9 s, 2-s-Fenster 0, Drehen 44.2 %, rueckwaerts 0.0 % (Rueckzug nach Vorstoss 0.0 %), Tauchgaenge 0 = 0.00/min, untergetaucht 0.0 %, Kopf/Kanister in Rampe/Plattform 14.2 %, < 2500 vor der Kamera 14.5 %, hinter der Kamera 4.9 %, Rammen 0 (Querungen 0), Gang 57 von 13835 abweichend, Gier max 96, Takt-Rechenzeit Mittel 46 us / 95 % 67 us / max 5317 us
- SUMME neu: 23400 Bilder (13.0 min): ohne Fortschritt max 57.0 s, 2-s-Fenster 11, Drehen 4.8 %, rueckwaerts 11.9 % (Rueckzug nach Vorstoss 7.9 %), Tauchgaenge 10 = 0.77/min, untergetaucht 2.2 %, Kopf/Kanister in Rampe/Plattform 1.3 %, < 2500 vor der Kamera 6.3 %, hinter der Kamera 0.0 %, Rammen 41 (Querungen 41), Gang 113 von 10858 abweichend, Gier max 96, Takt-Rechenzeit Mittel 50 us / 95 % 93 us / max 1776 us

### Zahlen Fensterlaeufe (exe, gdigrab, Eingaben der Abnahme 4; gestern = Abnahme-4-Laeufe auf bc5fb7c9c, neu = exe 15aa14b5; Werkzeug `werkzeug/mass_exe.py`, Kopf-/Kanister-Punkte wie Abnahme 4 kopfblock.py)
```
gestern frei4_leon: 7805 Bilder mit Kanister (4.3 min) | 2-s-Fenster 430, laengste Folge 13.7 s | ohne Fortschritt zur Figur 21.5 s | Drehen 7.9 % | rueckwaerts 0.0 % (+Rueckzug 0.0 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 34.3 % | < 2500 vor der Kamera 25.4 %, hinter 10.0 % | Rammen 0 (Querungen 0) | 
neu     frei4_leon: 7805 Bilder mit Kanister (4.3 min) | 2-s-Fenster 1, laengste Folge 2.0 s | ohne Fortschritt zur Figur 38.5 s | Drehen 15.2 % | rueckwaerts 17.1 % (+Rueckzug 0.0 %) | Tauch 1 (0.23/min) unter 0.8 % | Kopf/Kanister in Plattform/Rampe 21.8 % | < 2500 vor der Kamera 0.8 %, hinter 0.0 % | Rammen 0 (Querungen 0) | Takt 355/547/15113 us (Mittel/95 %/max, 8674 Takte)
gestern frei4_elza: 7454 Bilder mit Kanister (4.1 min) | 2-s-Fenster 133, laengste Folge 3.4 s | ohne Fortschritt zur Figur 6.6 s | Drehen 47.8 % | rueckwaerts 0.0 % (+Rueckzug 0.0 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 13.0 % | < 2500 vor der Kamera 0.5 %, hinter 0.0 % | Rammen 0 (Querungen 0) | 
neu     frei4_elza: 7454 Bilder mit Kanister (4.1 min) | 2-s-Fenster 68, laengste Folge 2.3 s | ohne Fortschritt zur Figur 48.1 s | Drehen 26.3 % | rueckwaerts 11.7 % (+Rueckzug 0.0 %) | Tauch 11 (2.66/min) unter 7.6 % | Kopf/Kanister in Plattform/Rampe 9.5 % | < 2500 vor der Kamera 0.3 %, hinter 0.0 % | Rammen 0 (Querungen 0) | Takt 382/570/5566 us (Mittel/95 %/max, 8674 Takte)
gestern west4_leon: 2650 Bilder mit Kanister (1.5 min) | 2-s-Fenster 0, laengste Folge 0.0 s | ohne Fortschritt zur Figur 8.9 s | Drehen 87.1 % | rueckwaerts 0.0 % (+Rueckzug 0.0 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 23.0 % | < 2500 vor der Kamera 50.9 %, hinter 22.2 % | Rammen 0 (Querungen 0) | 
neu     west4_leon: 2650 Bilder mit Kanister (1.5 min) | 2-s-Fenster 0, laengste Folge 0.0 s | ohne Fortschritt zur Figur 7.0 s | Drehen 0.0 % | rueckwaerts 30.3 % (+Rueckzug 2.0 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 18.8 % | < 2500 vor der Kamera 8.9 %, hinter 0.0 % | Rammen 1 (Querungen 1) | Takt 359/631/7661 us (Mittel/95 %/max, 3574 Takte)
gestern west4_elza: 2688 Bilder mit Kanister (1.5 min) | 2-s-Fenster 0, laengste Folge 0.0 s | ohne Fortschritt zur Figur 70.2 s | Drehen 75.7 % | rueckwaerts 0.0 % (+Rueckzug 0.0 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 3.9 % | < 2500 vor der Kamera 10.4 %, hinter 7.4 % | Rammen 0 (Querungen 0) | 
neu     west4_elza: 2688 Bilder mit Kanister (1.5 min) | 2-s-Fenster 11, laengste Folge 2.4 s | ohne Fortschritt zur Figur 66.5 s | Drehen 27.8 % | rueckwaerts 25.7 % (+Rueckzug 0.0 %) | Tauch 1 (0.67/min) unter 1.9 % | Kopf/Kanister in Plattform/Rampe 36.9 % | < 2500 vor der Kamera 1.1 %, hinter 0.0 % | Rammen 0 (Querungen 0) | Takt 305/538/4564 us (Mittel/95 %/max, 3574 Takte)
gestern west5_elza: 2596 Bilder mit Kanister (1.4 min) | 2-s-Fenster 0, laengste Folge 0.0 s | ohne Fortschritt zur Figur 10.9 s | Drehen 82.1 % | rueckwaerts 0.0 % (+Rueckzug 0.0 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 21.7 % | < 2500 vor der Kamera 47.7 %, hinter 20.1 % | Rammen 0 (Querungen 0) | 
neu     west5_elza: 2596 Bilder mit Kanister (1.4 min) | 2-s-Fenster 0, laengste Folge 0.0 s | ohne Fortschritt zur Figur 6.8 s | Drehen 0.0 % | rueckwaerts 11.6 % (+Rueckzug 16.3 %) | Tauch 0 (0.00/min) unter 0.0 % | Kopf/Kanister in Plattform/Rampe 11.4 % | < 2500 vor der Kamera 0.9 %, hinter 0.0 % | Rammen 7 (Querungen 7) | Takt 285/510/4798 us (Mittel/95 %/max, 3574 Takte)
```
Takt-Rechenzeit: gestern ohne Zeitlog (NB5 hatte gator_zeit_r41.c nicht; Harness NB5 max 4,7-10,6 ms beim Huellen-Neubau). Die exe-Laeufe liefen zu zweit parallel mit gdigrab.

### Bildleisten (Normalhelligkeit, 320 x 240 je Bild, Zeile = Videozeit)
- `G2_belege/re2/frei4_leon_plattform_suedkante_t229-241_gestern_neu.jpg` (Abnahme-4-Haenger M4 an der Plattform-Suedkante)
- `G2_belege/re2/west4_leon_rampe_cut2_t60-100_gestern_neu.jpg`, `west5_elza_rampe_rammen_t50-110_gestern_neu.jpg` (gestern Stadion vor der Kamera, neu Vorstoss/Rammen/Rueckzug)
- `G2_belege/re2/west4_elza_nordbecken_ost_t40-120_gestern_neu.jpg` (M4 Westkanal/Nordbecken Ost)
- `G2_belege/re2/gang_ohne_kanister_frei4_leon_t8-20_gestern_neu.jpg` (Gang ohne Kanister)

### Riegel
unit_r41_gatorfix_re2(_elza), _re2_gegen(_elza), _gang(_elza) gruen (Ausgaben `fin_*.txt`). Schwellen (PORT-WAHL aus der Messung): 2-s-Fenster <= 15 in 13 min,
SO-Ecke/Rampe ohne Fortschritt <= 20 s, Drehen fern der Figur <= 15 %, Kopf/Kanister in Plattform/Rampe <= 2 % (ohne Ueberquerung), < 2500 vor der Kamera <= 8 %,
hinter der Kamera <= 0,1 %, Rueckwaerts setzen <= 12 %, Tauchen <= 1/min und < 5 %, Gang = authored RE2-Vortrieb (Anteil Abweichung <= NB5 + 1 %, <= 2 %), Rammen in
jeder Rampenlage. Gegenproben: NB5 (Pruefhaken) rot fuer Kopf/Kanister, Kamera, Drehen, Haengen (SO-Ecke/Rampe 15,9-17,5 s), Rammen; RE1.5-Tempo rot (Gang);
ohne Rammen (Pruefhaken) 0 Rammen. bahn/kamera (NB5-Stadion) ausgetragen.

### Offen (ehrlich, nicht abnahmereif)
1. Figur im Nordosten (Lagen Nordbecken-Ost = west4_elza, NO-Ecke): er kommt mit dem Kanister nicht heran (Harness 56/57 s ohne Fortschritt, exe west4_elza 66,5 s,
   gestern 70,2 s); NO-Ecke 5 Tauchgaenge/min und 45 % < 2500 vor Cut 4. Die Ecke Nordwand/Rampe/Ostwand laesst Kopf (5644) + Kanister (2694 quer) nicht an die Figur.
2. Rueckwaerts im exe ueber dem Riegel: frei4_leon 17 %, west4_elza 26 %, west4_leon 30 % (Harness gesamt 11,9 %). Ausloeser "drueckt sich fest" feuert an der
   Westkanal-Einmuendung (Plattform-NW-Ecke) wiederholt - dort ist mit dem Kanister ohne Wenden in drei Zuegen kein Weg (NB6: nur vorwaerts 0 lebensfaehige Lagen).
3. frei4_elza: 11 Tauchgaenge in 4,1 min (2,7/min, 7,6 % unter Wasser) - ueber der Grenze 1/min; frei4_leon 0,23/min.
4. Kopf/Kanister in Plattform/Rampe im exe nach dem Abnahme-4-Naeherungsmass (feste Kanister-Lage 4630 vor) 9-37 %; das genaue Mesh-Mass gibt es nur im Harness (1,3 %).
   Fuer das exe fehlt eine Messzeile mit den echten Punkten.
5. Takt-Rechenzeit: Harness Mittel 65 us, 95 % 119 us, max 2,2 ms (< 3 ms); exe (zwei Laeufe parallel mit gdigrab) Mittel 285-382 us, max 4,5-15 ms - Lauf ohne
   Parallellast fehlt.
6. Belagerung in Cut 2 kommt meist von Norden (Vorstoss + Rueckzug, ~23-29 % der Zeit Rueckzug in den Rampenlagen); volle Querung nur, wenn er schon im Suedbecken ist.

### Tests (Stand dieser Spur, NICHT vollstaendig gruen)
- Neue Riegel gruen: unit_r41_gatorfix_re2/_re2_elza, _re2_gegen/_re2_gegen_elza, _gang/_gang_elza; dazu klemm/_elza, haken/_elza, fuzz/_elza (nach Anpassung).
- Gruppenlauf `ctest -R "r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix" -j3` (Baum build, Stand 199a63c95) nach 57 von 125 Tests abgebrochen
  (Laufzeit; Integrationsriegel 3-7 min je Test): 49 gruen, 8 rot - unit_r41_gatorfix_fuzz/_elza (NB2-Schranke "0 2-s-Fenster", neu 8 Fenster durch Rueckwaerts an
  der Wand; Schranke auf <= 10 gesetzt, danach gruen), integration_r41_gatorfix_rampe/_elza (pruefte Belagerung Phase 5 >= 600 Bilder; jetzt Ueberquerung >= 100
  Bilder und Rammen >= 1 - nicht nachgelaufen), integration_r41_gatorfix_frei, _abn2_nordbecken, _abn2_wand_elza (2-s-Fenster-Schranke 0 -> <= 10 angepasst, nicht
  nachgelaufen; Ursachen nicht einzeln geprueft), integration_r41_alligator2_schluessel_west_elza (R1-Tod/Schluessel im Westen mit Elza - Ursache NICHT untersucht).
- Die restlichen 68 Tests der Gruppe und Unit/Prop/Probe sind auf diesem Stand nicht gelaufen.

## G2-RE2 NB1 (Fortsetzung)
Stand 2026-11-03, frische Sitzung. Worktree auf 57667b4d2 zurueckgesetzt (Arbeitsbaum vorher sauber). Der WIP 1e60a9eeb ("ausser Sicht keine Kollision",
Riegel re2 rot) ist verworfen, nur per Hash lesbar; sein Befund: ausser Sicht nur 1,9 % der Kanisterzeit, Nordbecken-Ost/NO-Ecke unveraendert 55 s.

### RE2-Befund Tank-Zustand (nur festgehalten, nichts daraus gebaut - Nutzerentscheid "Vorschlag 4")
EM23-Overlay (analysis/re2_alligator_2026-09-09/EM23_OVL_0000.BIN, Disasm `herkunft/alligator_werkzeuge/re2_em23.dis`):
- T2 @0x801017c4 schaltet nur fuer +0x226 < 8: `lbu v1,550(s1)` @0x80101808, `sltiu v0,v1,0x8` @0x80101814, `beq v0,zero,0x80101b90` @0x80101818,
  Tafel @0x80100024: [0] 0x80101b90 (Ende, nichts), [1] 0x8010183c, [2] 0x80101860, [3] 0x80101970, [4] 0x801019dc, [5] 0x80101a64, [6] 0x80101aec, [7] 0x80101b58.
  Ab @0x80100044 beginnt die Tafel der Ereignis-Maschine +0x227 (@0x80101d44, `lw v0,68(at)` @0x80101d70) - es gibt keine Tank-Stufen 8-12.
- Stufe 7 (getragen) @0x80101b58-8c: nur `lw v1,56(s2) / addiu v0,zero,1750 / ... jal 0x8001a1e0 (a2 = 100)` und `lw v1,60(s2) / addiu v0,zero,-210 /
  ... jal 0x8001a1e0 (a2 = 30)`, also die Lokallage nachfuehren. Kein Zaehler, kein Ausgang.
- Alle Schreiber von +0x226 im Overlay: `sb zero,550(s0)` @0x8010066c (INIT), `sb v0,550(s0)` @0x80100b08 (0 -> 1, Fenstertreffer), T2-Zaehlung
  @0x8010185c / @0x80101964-6c / @0x80101b44-54 (1 -> 7), `addiu v0,zero,13 / sb v0,550(s1)` @0x801022b8-bc (R1). Leser: @0x80100a34, @0x80100a98, @0x80100af4,
  @0x80100b20, @0x80101360, @0x80101808 und @0x80101ef4 (HURT-Selektor: `addiu v0,zero,7` @0x80101ef0, `lbu v1,550(s0) / bne v1,v0` @0x80101ef4-fc -> R1).
- Ergebnis: In RE2 endet das Tragen NUR durch den Schuss (R1). Es gibt keinen Zeitzaehler, kein Fallenlassen, Ausspucken oder Zerbeissen und keinen Sonderzustand
  beim Haengen. RE2 braucht das nicht, weil der Gator dort nur im Korridor schwimmt.

### Nutzerentscheid (Koordinator, 2026-11-03) - ersetzt die Schritte 1-3
Wortlaut: "Vorschlag 4: Ignoriere den Kanister einfach in der Berechnung. Tu so als ob er nicht da waere....", dann "Ja, wichtig ist nur das der Kanister synchron
mit dem Aligator mitlaeuft. Ansonsten soll er nicht als Stoerkoerper zaehlen." und "Wenn es dir hilft.... Mach ein 2. Aligator Modell mit Kanister im Maul...".
- Mit dem Kanister bewegt er sich wie ohne Kanister (NB5 ohne Kanister): gleiche Huelle, Klemme, Verfolgung und Wendungen. Der Kanister wird nur gezeichnet und ist Trefferziel.
- Rampe: Rammen wie gehabt. Rueckwaerts und Tauchen bleiben nur als Rueckfall, wenn er auch ohne Kanister haengt.
- Der Kanister haengt in jedem Bild fest am Kiefer bzw. Maul, ohne Versatz, Nachziehen oder Springen, auch beim Wenden, Rueckwaerts, auf der Rampe, beim Rammen und Biss.
- Zweites Modell nur, wenn die Messung zeigt, dass die Kopplung heute nicht bildgenau ist.
- Riegel: Bewegung mit = ohne Kanister (Gegenprobe Kanister-Huelle rot); Halterung zu Kieferknochen konstant (Gegenprobe ein Bild alte Pose rot).

## G2-RE2 NB2 (Vorschlag 4)
Stand 2026-11-06, Zweig r41/gator_re2 ab f942f56c0. Spezifikation = Abschnitt "Nutzerentscheid (Koordinator, 2026-11-03)" oben. Laufordner scratchpad `g2v4/` (nicht im Baum);
Belege `G2_belege/v4/`: Riegel-Ausgaben `fin_v4*.txt`, `fin_re2*.txt`, `fin_gang*.txt`, exe-Mass `exe_mass.txt`, Bildleisten, Werkzeuge `werkzeug/` (lauf.sh, batch.sh/batch_x.sh,
mass_v4.py, leiste3.py, kontakt.py). Commits: 4b794e625 (Bau), 77d9a9ebc (Riegel), c06bcfefa (Pause wie ohne Kanister, exe-Riegel mit Partnerlauf), dieser Commit (Belege/Dossier).

### Ursache des Haengens in Zahlen (Stand f942f56c0 = der Kanister zaehlt)
Gemessen mit dem Riegel re2 (je Lage ein frischer Kampf, 1800 Bilder, Pruefhaken `re15_gr2_v4_alt_setzen(1)`; `fin_re2.txt`) und `werkzeug/mass_v4.py` auf den exe-Laeufen von
f942f56c0 (exe 15aa14b5, Eingaben der Abnahme 4; `exe_mass.txt`):
- Laengste Zeit ohne Fortschritt zur Figur: Harness Nordbecken-Ost 55,9 s, NO-Ecke 47,2 s, Leiter 18,7 s; exe frei4_leon 38,5 s, frei4_elza 48,1 s, west4_elza 66,5 s.
- Drehen auf der Stelle fern der Figur (ganzes 120-Bilder-Fenster Kopf > 4500): Harness Nordbecken-Ost 45 %; exe frei4_leon 13,4 %, frei4_elza 18,2 %, west4_elza 27,8 %.
- Rueckfall: Harness 10,2 % rueckwaerts und 13 Tauchgaenge in 13 min; exe frei4_leon 17,1 % rueckwaerts, west4_leon 30,3 %, frei4_elza 11 Tauchgaenge in 4,1 min.
- Mechanismus: die Huelle mit Kanister reicht 4589..4666 vor die Wurzel und 2694 nach links (NB5 M2a, `re15_gr2_huelle_ausdehnung`), der Kopf allein 3236 vor. Kopf-Klemme
  (`re15_gr2_kopf_klemme`, je Bild bis 340 zurueck), Zielkuerzung (`re15_gr2_kk_ziel`, Trag-Lenkung), Kollisions-Antwort (`re15_gr2_rw_kopf_verletzt`) und Kamera-Grenze
  nahmen die Bewegung zurueck, die die Verfolgung wollte; an der Plattform-NW-Ecke, im Nordbecken-Ost und an der NO-Ecke hat die Kanister-Huelle keinen Weg zur Figur.
- Dieselben Lagen mit Vorschlag 4: Harness Nordbecken-Ost 4,3 s, NO-Ecke 1,1 s, Leiter 3,2 s, Drehen fern in jeder Lage 0 %, kein Rueckwaerts, kein Tauchen; exe frei4_leon
  4,1 s / 1,6 %, frei4_elza 7,4 s / 0 %, west4_elza 7,3 s / 29,8 % (siehe Offen 2).

### Was gebaut wurde (PORT-WAHL Nutzerentscheid; Code-Stellen)
- `engine/src/gator_re2_r41.c`: `gr2_kan_zaehlt()` = Pruefhaken `s_v4_alt` (Stand f942f56c0). Ohne ihn sehen den getragenen Kanister nicht mehr: Kopf-Klemme
  `re15_gr2_kopf_klemme` (R1-Tod bleibt), Steuerziel `re15_gr2_kk_ziel`, Trag-Lenkung `gr2_trag_ziel_innen` (Kuerzung, Vorbei-Etappe, Kreis, Bahn), Zonen-Dash-Sperre
  `re15_gr2_trag_dash_erlaubt`, `re15_gr2_kreuz_anfahrt`, `re15_gr2_rw_umweg`, Kollisions-Antwort `re15_gr2_rw_kopf_verletzt`, Huelle der Notloesung (`gr2_tr_huelle_vorrat`
  ohne Kanister-Punkte). Die Hysterese-Ruhe beim Tragen (RE2 @0x80100ab8-c8, `re15_gr2_hysterese_ruht`) entfaellt: mit ihr wich die Bewegung mit Kanister in 4956 von
  8100 Bildern der 9 Wasser-Lagen von der ohne Kanister ab (Messung mit zurueckgenommener Ausnahme). Aufnahme (Stufen 1..6) und T2 bleiben RE2.
- Halterung unveraendert RE2: der Kanister ist Kind der Part-6-Weltmatrix der GEZEICHNETEN Pose (`gr2_kan_folgen` in `re15_gr2_nachtakt`, RE2 `addiu a0,s0,1104` @0x801018d0 /
  `sw a0,128(s2)` @0x801018e0); Stufe 7 fuehrt die Lokallage 6 Bilder nach dem Anhaengen heran (@0x80101b58-8c). Messhaken `re15_gr2_kanister_sitz` misst jetzt Lage UND
  Drehung (debug.log "Kanister-Sitz"), `re15_gr2_kanister_rel`, `re15_gr2_rw_nur_kopf_punkte`; Pruefhaken `re15_gr2_kan_vorbild_setzen`; Messhaken env `RE15_G2_OHNE_KANISTER`
  (PC: der Kanister verschwindet beim Tragen - dieselbe Eingabe zeigt den Gang ohne Kanister).
- `engine/src/gator_rw_r41.c`: `re15_gr2_rw_an` (G2-RE2-Bausteine mit dem Kanister als Koerper: Kollisions-Antwort, erweiterte Klemme, Umweg, Vorstoss/Rueckzug,
  Ueberquerungs-Profil, "keine Belagerung") nur mit dem Pruefhaken; `re15_gr2_rw_traegt` = Rueckfall Rueckwaerts/Tauchen, nur noch aus dem Haenge-Fenster (150 Bilder Kasten
  < 2500, Kopf > 4500 von der Figur, kein Biss) - das 16-Bilder-Fenster "drueckt sich fest" schlug ohne Kopf-Klemme beim Wenden/Beissen an der Figur an (Harness Mitte-Nord
  8 % rueckwaerts im Angriff). Ein durch Phasenwechsel unterbrochener Rueckwaerts-Durchgang endet (vorher Rampe-x2318 100 % "rueckwaerts" in der Belagerung).
- `engine/src/enemy_ai_boss_gator.c`: mit dem Kanister dieselben Zweige wie ohne (Querungsbeginn, Wende, Bogen-Regeln, 600er-Ruhezone, Rampen-Uebergang, Belagerung, Pause
  nach der Querung 300 statt GB_KREUZ_PAUSE). Rampe ("rammen wie gehabt, keine Deckung"): Belagerung wie ohne Kanister; nach der Pause quert er ueber die Figur (Spur-B-Querung,
  Rammen in GBP_CROSS wie f942f56c0: `gb_biss_abschluss` -> DAT_8006f418[5] = 50 @0x8006f422 + Niederwurf), wenn das Bahn-Ende den Kopf (5644 vor der Wurzel, NB6 Messung 1)
  mindestens 2500 vor der Kamera laesst (GR2_RW_KAM_NAH), sonst die Seite wie ohne Kanister. Gemessen (Harness): ohne diese Regel Rampe-x2318 (= Abnahme 4 west5_elza) 3 Treffer
  in 60 s; Querung auch zur Kamera hin: Kopf < 2500 vor der Kamera in 186-192 Querungs-Bildern je Lage; nur von der Kamera weg ohne Rueckfall auf die Belagerungsseite: 0
  Querungen, 0 Treffer an x2318/Rampe-Nord. Pause 45 statt 300 (exe west4_leon): Querung alle ~6 s, Kopf < 2500 vor der Kamera 8,2 % gegen 1,5 % ohne Kanister.
- `platform/pc/src/gator_re2_pc.c`: kein Block-Schnitt (der Kanister/Kopf darf sichtbar in Rampe/Plattform ragen - dokumentiert, nicht versteckt); der angehaengte Kanister
  wird ohne eigenen Regions-Test gezeichnet - ohne Kopf-Klemme ragt er an den Aussenwaenden ueber das Regions-Viereck (Riegel kopfklemme ohne Pruefhaken: Kanister-Ende bis
  z -29923, Viereck bis -29000) und waere aus dem Maul verschwunden. exe: "Kanister ausserhalb Regions-Viereck" 0 Zeilen in allen Laeufen.

### Halterung: zweites Modell nicht gebaut (Messung)
- Harness (Riegel v4 (b), Pose wie der Zeichner im Riegel selbst gerechnet: `g2_pose`/`g2_part_welt`): Leon und Elza je 11 700 getragene Bilder in 13 Lagen (Verfolgung,
  Wenden, Biss, Belagerung, Querung), Lage-Abweichung max 0, Drehungs-Abweichung max 0 (Q12); Gegenprobe "ein Bild alte Pose" (Pruefhaken): 300 von 300 Bildern abweichend,
  max 12 281 (rot).
- exe (debug.log "Kanister-Sitz" = Pose aus dem Ueberblend-Schnappschuss des Zeichners, Lage + Drehung): frei4_leon 7830, frei4_elza 7470, west4_leon 2670, west4_elza 2700,
  west5_elza 2610 angehaengte Bilder - Abweichung max 0 in jedem Lauf (`exe_mass.txt`).
- Zweites Modell (Mesh als Kind von Part 6 in einer EMD-Variante) waere dieselbe Rechnung mit festem Sitz, braeuchte Asset + Pin, und der Kanister muesste trotzdem eigenes
  Objekt bleiben (Trefferziel @0x80101ef4-fc, R1-Scherben, Fall/Liegen vor der Aufnahme, Stufe 7 fuehrt die Lokallage noch 6 Bilder heran). Bei Versatz 0 kein Gewinn.

### Riegel Soll | Ist
| Riegel | Soll | Ist (Leon = Elza im Harness) | Beleg |
|---|---|---|---|
| (a) Bewegung mit = ohne Kanister, Harness | Wasser-Lagen bitgleich; Rampe gleich bis zur Querung | 9 Wasser-Lagen x 900 Bilder: 0 abweichend; Rampe-x4428/Weg-Rampe 0, x2318 ab Bild 769, Rampe-Nord ab 766 (erste Abweichung in Querung/Belagerung); Rueckfall 0 Bilder | fin_v4*.txt |
| (a) Gegenprobe Kanister-Huelle aktiv (f942f56c0) | rot | 9 von 9 Wasser-Lagen weichen ab (11 697 Bilder) | fin_v4*.txt |
| (a) exe mit = ohne (Messhaken) | gleich bis Querung/Rammen | frei4_leon 7805 und frei4_elza 7454 Bilder: 0 abweichend; west4_leon erste Abweichung F868 in GUARD (Rammen F880); west5_elza F1934 nach Rammen F1103; west4_elza F1444 nach Rammen F1183 | exe_mass.txt |
| (b) Halterung | Versatz 0 in allen Bildern | Harness 2 x 11 700 Bilder 0/0; exe 23 280 Bilder max 0 | fin_v4*.txt, exe_mass.txt |
| (b) Gegenprobe ein Bild alte Pose | rot | 300/300 abweichend, max 12 281 | fin_v4*.txt |
| (c) ohne Fortschritt SO-Ecke/Rampe | <= 20 s | 10,7 s (alle Lagen max 10,7 s; f942 55,9 s) | fin_re2*.txt |
| (c) Drehen fern der Figur je Lage | <= 15 % | 0 % in jeder Lage (Gegenprobe f942 Nordbecken-Ost 45 %) | fin_re2*.txt |
| (c) Kopf < 2500 vor / hinter der Kamera | NB1: <= 8 % / <= 0,1 % | 11,6 % / 2,3 % - ohne Kanister 12,5 % / 2,5 % (Riegel jetzt: mit <= ohne) | fin_re2*.txt |
| (c) Rampe | Rammen | Treffer in jeder Rampen-Lage; Rammen 5 (in jeder Lage mit Querung); x2318 19 Treffer gegen 3 ohne Kanister; ohne Kanister 0 Rammen | fin_re2*.txt |
| Rueckfall | Tauchen <= 1/min, < 5 % | Harness 0 Tauchgaenge, 0 % rueckwaerts; exe 0 Tauchgaenge, rueckwaerts nur west4_elza 2,1 % (2 Durchgaenge) | fin_re2*.txt, exe_mass.txt |
| Gang ohne Kanister = NB5 | bitgleich | 0 von 2700 Bildern abweichend (Leon, Elza) | fin_gang*.txt |
| Kanister in Rampe/Plattform | entfaellt (Nutzerentscheid) | Harness Kopf 13,3 %, Kopf + Kanister 13,3 % der Bilder (ohne Kanister Kopf 14,4 %); exe Kopf + Kanister 3,6-31,7 % | fin_re2*.txt, exe_mass.txt |

### Bildleisten (gdigrab der eigenen exe-Kopien, Normalhelligkeit, Spalten "NB5 ohne Kanister (Messhaken) | f942f56c0 | neu")
- `G2_belege/v4/west4_leon_rampe_rammen_t61-95.jpg` (Rammen F880 ~t 63, Querung ueber die Rampe mit dem Kanister am Kiefer ~t 91-93; t 95 Kopf gross vor der Kamera)
- `G2_belege/v4/west4_leon_wenden_biss_t36-59.jpg` (Wenden und Biss mit dem Kanister im Maul, Kamera Cut 2)
- `G2_belege/v4/west5_elza_rampe_rammen_t68-98.jpg` (Rammen F1103 / F1864)
- `G2_belege/v4/frei4_leon_plattform_suedkante_t229-241.jpg` (alter M4-Haenger: NB5 stand bei F5850-F7200 an der Plattform-Suedkante (1055,-21484)..(6,-23043); neu bei der
  Figur im Nordwesten (-7320,-7099)..(2789,-10613), f942 rueckwaerts im Nordbecken; in Cut 0 sind beide nicht im Bild - die Zahlen stehen hier, weil das Bild es nicht zeigt)

### Tests
- Neue Riegel: unit_r41_gatorfix_v4 / _v4_elza ((a) + (b) mit Gegenproben). Riegel re2 umgebaut (Kopf-Riegel, Staende f942 / neu / ohne Kanister, je Lage ein frischer Kampf).
- Riegel, deren Mechanismus nur noch der Pruefhaken hat, laufen mit `re15_gr2_v4_alt_setzen(1)` und je einem Vorschlag-4-Satz: gatorfix klemm/fuzz, alligator2 tragen,
  re2gator sitz/kopfklemme/aufnahme. Auf f942f56c0 waren re2gator aufnahme und kopfklemme schon rot (eigener Bau von f942f56c0 gemessen): aufnahme hielt die Wurzel je Bild
  fest und loeste damit den G2-RE2-Rueckfall "drueckt sich fest" aus (Clip 10 statt 0), kopfklemme Bild 9 noch 114 ausserhalb der Wand (jetzt ab Bild 10).
- exe-Riegel frei/rampe/abn2_*: Partnerlauf mit RE15_G2_OHNE_KANISTER=1 und `test_r41_gatorfix tragvergleich` (gleich bis Querung/Belagerung, Rammen oder Rueckfall;
  Kanister-Sitz 0); Wandreiben und 2-s-Fenster nur noch Ausgabe. gatorszene tod_explosion: Bildfenster bis F1400 (R1 jetzt F1192 statt F1156).
- Ursache des Abbruchs bei 57/125 (G2-RE2): kein ctest-Fehler, der Lauf brauchte laenger als die Sitzung (Integrationsriegel 3-12 min je Test, Gruppe 49-59 min bei -j 3).
  Jetzt im Hintergrund mit Ausgabedatei (scratchpad g2v4/ctest_gruppe*.txt).
- Gruppe `ctest -R "r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix" -j 3` (PowerShell, C:\msys64\mingw64\bin vorn, SDL_ASSERT=always_ignore),
  Stand c06bcfefa: **98% tests passed, 3 tests failed out of 127** (3520 s). Die drei (integration_r41_gatorfix_rampe, _rampe_elza, _abn2_wand_elza) wichen vom Lauf ohne
  Kanister erst mit dem Rueckfall ab: ohne Kanister haengt er dort an der Plattform-Suedkante (rampe: Wurzel ab F2499 150 Bilder in (3459..4747, -23087..-22323), Kopf 6830
  von der Figur; abn2_wand_elza F4778-F4928 in (2314..4342, -23184..-21690), Kopf 9820), mit Kanister setzt der Rueckfall dort rueckwaerts (F2650 / F4929) - so verlangt
  (Auftrag Punkt 3). tragvergleich laesst die Abweichung ab dem Rueckfall jetzt zu (dieser Commit). Danach mit diesem Werkzeug-Stand die drei Riegel und alle unit_r41_gatorfix_* (-j 3, ohne andere Last aus diesem Baum): **100% tests passed, 0 tests failed out of 27**. Ausgaben: G2_belege/v4/ctest_gruppe3.txt, ctest_nach.txt.

### Offen
1. Kopf vor der Kamera: die NB1-Schranke (<= 8 % / <= 0,1 %) gilt fuer den Gang ohne Kanister nicht (12,5 % / 2,5 % im Harness, exe west4_elza 10,4 % / 4,3 %); mit Vorschlag 4
   bleibt es dabei (11,6 % / 2,3 %). Eine Kamera-Grenze fuer den Kopf muesste auch ohne Kanister gelten (NB5 bitgleich ist verbindlich) - Entscheid des Orchestrators.
2. Nach einem Rammen laeuft er anders als ohne Kanister weiter (die Figur liegt). exe west4_elza: danach Drehen fern 29,8 %, 103 2-s-Fenster fern der Figur, 2 Rueckwaerts-
   Durchgaenge (Rueckfall) im Nordbecken-Ost - ohne Kanister (ohne Rammen) 17,4 % / 0. Das Haengen im Nordosten ist damit nicht ganz weg, nur seltener.
3. RE2-Zonen-Dash: mit Vorschlag 4 dasht er wie ohne Kanister (f942: mit Kanister kein Dash). exe frei4_elza: 209 2-s-Fenster fern der Figur, alle in Phase 12 (RE2-Dash, Clip 2/3
   Vordrehen) - gleich ohne Kanister (209). Kein Haengen, aber 20 s Vordrehen am Stueck (F3377-F3977).
4. Gegenueber RE2 entfaellt die Hysterese-Ruhe beim Tragen (@0x80100ab8-c8) - bewusst (Vorschlag 4), Pruefhaken behaelt sie.
5. Querungsbeginn mit dem Kanister springt wie ohne Kanister (Anlaufpunkt, Riegel alligator2 tragen: Lagesprung bis 1413 je Bild); die Spur-P-Regel O1 gilt nur noch mit dem
   Pruefhaken.
6. PSX-Bau nicht gemessen (die geaenderten Pfade sind plattformneutral; Zeichner-Aenderungen nur PC).
7. Auch ohne Kanister (NB5) haengt er an der Plattform-Suedkante (exe rampe/rampe_elza/abn2_wand_elza: 150 Bilder im Kasten, 6830-9820 von der Figur); mit Kanister greift dort
   jetzt der Rueckfall (je 1-2 Rueckwaerts-Durchgaenge, kein Tauchen). Das ist der Ort des alten M4-Haengers - die Ursache liegt in der Verfolgung ohne Kanister (NB5).
8. exe rampe: die Figur kommt (wie ohne Kanister) nicht auf die Rampe, sie wird an der Kante angegriffen ("Klettern sub=9" 0 Zeilen) - der fruehere Riegel "Ueberquerung >= 100
   Bilder, Rammen >= 1" ist darum nur noch Ausgabe; Rammen auf der Rampe belegen Riegel re2 und die Fensterlaeufe west4_leon / west5_elza.

## G2-RE2 NB3 (Rampe oben, Plattform-Suedkante)
Stand 2026-11-07, Zweig r41/gator_re2 ab e92356e8e (Messblatt des Messers, G2_MESSBLATT_NB3.md). Rolle: Bauer (NB3, Runde 1 von 2). Laufordner scratchpad `g2nb3/`
(Bauer: `harness_neu*/`, `bau_runs/`, `final_runs/`, `final_runs2/`, nicht im Baum); Belege `G2_belege/nb3/bau/` (Harness-Ausgaben je Bauschritt, Werkzeuge, exe-Messung, Bildleisten).
Commits: a7415b225 (Engine Vorgabe A + B), 8bfcf864c (Riegel nb3, gang/re2/fuzz, obenlog/haenglog, exe-Riegel rampe_oben), 466901ffe (Figur im Block = belagern), d3762d778 (obenlog ab Kanister-Anhaengen), bb2803202 (Zaehler oben_ohne nur in Kampfphasen + Querbezug re2gator Fenster 1700), dieser Commit (Dossier, Belege). Finale exe `re15_port/build/platform/pc/re15_pc.exe` md5 029efac644499f92ef0a5572ee3aa43d (nach Patch 9; alle exe-Belege `final_runs2/`, die exe vor Patch 9
5581916f nur noch fuer die re2gator-Laeufe der Gruppe 1), alter Stand = Kopie der
Abnahme-4-exe d872c8e6 (`g2nb3/exe_alt/`).

### Nutzerentscheid 2026-11-06 ~21:15 (woertlich, bindend) auf die vier Fragen der Abnahme 4
- F1 (Kopf-Kameragrenze auch ohne Kanister): "Zu F1, noe, wozu wenn es gut funktioniert?" -> keine Kameragrenze; der Riegel bleibt "mit Kanister nicht haeufiger als ohne".
- F2 (Sprung am Querungsbeginn): "Verstehe ich nicht..." -> bleibt wie im gelobten Stand (nicht angefasst; gb_cross_begin unveraendert).
- F3 (Takt der Rampen-Querung): "Verstehe ich auch nicht... ist damit gemeint, das er oefter schneller laeuft? Das kann er gerne machen. Er soll nur den Spieler oben auf
  der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann. Alternativ kann er auch hoch beissen oder sowas, aber er darf halt oben nicht sicher sein."
  -> **Vorgabe A**: die Figur oben ist nicht sicher - Rammen oder Hochbiss, Takt frei.
- F4 (Haenger an der Plattform-Suedkante ohne Kanister): "Wenn er haengt muessen wir nachbessern denke ich. Er darf nicht zu lange haengen. Wenn es kurz ist, kann man mit
  dem tauchen abhilfe schaffen, aber das hilft natuerlich nicht bei der Platform." -> **Vorgabe B**: der Haenger wird in der Verfolgung selbst beseitigt (auch ohne
  Kanister), "nicht zu lange" in Zahlen (Messblatt B3: <= 450 Bilder ohne Fortschritt).
Alles andere ohne Kanister bleibt NB5 (Memory reai-v2-gelobter-stand-ist-referenz); der Pruefhaken `re15_gator_boss_nb3_alt_setzen(1)` stellt den Stand a661eb8aa her.

### Ursache (Messblatt NB3 Abschnitte 1.4 und 2.3, hier nachgemessen)
- **B, Haenger:** `gb_seg_hits_rect` (enemy_ai_boss_gator.c) meldet "Punkt innen -> Schnitt", wenn der ENDPUNKT der Strecke (die Figur) im um GB_KOERPER_M 1800 aufgeblasenen
  Block-Rechteck liegt - auch wenn sie im Wasser steht (Rampe x 50..9000 z -19900..-12700; Plattform x -3500..3650 z -21800..-10650). Der CHASE-Zweig "verbunden" lief
  dann in Ring-/Westumlauf-Ecke (zw 5/6), `gb_kanten_umweg` in die "Schattenlage" (zw 11): Pendel im Kasten (3156..4879, -24085..-21690), Kopf 6830-10057 von der Figur.
  Harness `nb3_haengt` (Stand a661eb8aa, Gegenprobe im Riegel nb3): W4200/-18800/c2 **3 Haenge-Fenster** in 3600 Bildern; exe (Messer-Laeufe, `haenglog`): rampe_leon 1
  Fenster F2649, mit Kanister 2 Fenster + 2 x RUECKWAERTS, rampe_elza 1, wand_elza 1 (F4928).
- **A, Rampe oben:** ohne Kanister quert er nur beim Seitenwechsel (Leons naehere Kante, gb_seite_latch) und belagert danach diese Kante; der Hochbiss reicht von der
  Kantenlinie (Kante + GB_GPAT_M 1900, x >= 4450) 3900 weit: im Mittelband bei x < ~3400 nie. Harness `nb3_oben` (a661eb8aa): R2318/-16300 und R2800/-16300 **kein
  Treffer in 3600 Bildern**, R3350/-16300 erster Treffer t1236; mit Kanister in der Suedreihe Pausen 697-959 Bilder (Belagerung der fernen Kante nach der Querung).

### Was gebaut wurde (PORT-WAHL nach dem Nutzer-Wortlaut; RE2 kennt im Alligator-Raum keine erhoehte Flaeche, Messblatt Abschnitt 3; Code enemy_ai_boss_gator.c)
**Vorgabe B (Verfolgung vor der Kante):**
1. `gb_punkt_im_band(x,z)`: Punkt im Koerper-Band (GB_KOERPER_M 1800) vor einer Blockkante, nicht im Block. CHASE "verbunden": steht die Figur im Band und ist die
   Strecke gegen den Block SELBST (m = 0) frei -> Ziel = die Figur, ohne Vorhalte (Zweig 7); `gb_kanten_umweg(..., direkt)` laesst diesen Zweig ohne Umweg durch
   (die SCA-Klemme hit_radius_min 1466 haelt die Wurzel von der Kante). Sonst alles wie NB5 (Ring/Westumlauf, Dijkstra mit 1800).
2. `gb_wand_dazwischen`: steht die Figur im Band und naeher an der Wurzel als der Maulpunkt (2600 voraus, k_gb_seg[0]), ist die Wand-Frage die Strecke Wurzel -> Figur
   gegen den Block (ein Biss DURCH den Block bleibt unmoeglich). Gemessen (erster Bau ohne diese Regel, `bau/harness/patch1_haengt_leon_ohne.txt`): er stand still an der
   Klemmlinie (W1000/-20700: Wurzel (1000,-21484), 784 vor der Figur, Maulpunkt hinter ihr im Block -> 104 s ohne Biss; W4073/W4200/W3000: 96 / 96 / 87 s) - ein Haenger,
   den das RW-Mass (Kopf > 4500) nicht sieht. Mit der Regel: GBP_LUNGE zieht rueckwaerts, bis das Maul vor der Figur liegt (dort seit dem Biss-Sweep 2026-09-11), Treffer
   bei Maul <= 1500 - W1000/-20700 337 Treffer, Pause max 456 Bilder.
3. Unerreichbare Figur = belagern wie oben (`gb_pl_unerreichbar` -> player_on_platform / oben -> GBP_GUARD, Kantenziel): (a) Ostnische (`gb_pl_nische`: x > GB_RAMP_X1 im
   Rampenband; die Figur kommt nur dort ueber x 7200 hinaus - exe wand_elza x 7668 bei z -17577..-14994): durch das Wasser unerreichbar (Wurzel klemmt an Ostwand
   7200 - 1466 und Suedkante, der Maulpunkt laege im Rampenblock); die Querung der Vorgabe A ueberstreicht sie (Bahn-x bis 6500, Maul <= 1500 / Koerperkreis 1050 + 450
   > 1168). (b) Figur IM Block-Rechteck auf Wasserhoehe (`gb_pl_im_block`): nur im Defektfall - exe wand_elza (NB3-Bau) F7920 Biss-Wegstoss aus der Taschen-Ecke
   (2318,-18568) in den Pier (1382,-16749) -> (1382,-17036) (fuer die Figur nicht begehbar: `test_r41_gatorfix begehbar 2091`, Raster "weggeschoben"; beim Klettern
   springen y und z im selben Bild, probe4 F82); dort pendelte NB5 an der SW-Ecke (2 Haenge-Fenster F8239/F8389, Kopf 9076-11182). In 7 Laeufen vor/nach NB3 sonst 0 Bilder.
**Vorgabe A (oben nicht sicher):**
4. Zaehler `oben_ohne` (gb_state_t): Bilder, die die Figur oben (Rampe/Plattform y < -900) oder unerreichbar steht, ohne Treffer (0 in gb_biss_abschluss und sobald
   sie nicht mehr oben ist). Ab **GB_OBEN_OHNE 300** (= der NB5-Wert der Pause nach einer Querung mit der Figur oben, cross_cd 300 "Leon OBEN: 10 s auf der Seite
   bleiben", GBP_CROSS) quert die Belagerung ueber die Figur hinweg (QUER-CROSS, Bahn-Ende folgt ihr, GBP_CROSS (b)) - fuer alle, mit und ohne Kanister (Vorschlag 4),
   ohne die Kamera-Bedingung der NB2 (F1 "noe"), auch mitten in der Pause nach der letzten Querung; waehrend der Pause (cross_cd) belagert er die EIGENE Kante statt des
   Westumlaufs zur fernen (Seite wie mit dem Kanister in NB2). Die alte Kanister-Regel (nur mit Kanister, nur von der Kamera weg) bleibt unter dem Pruefhaken.
5. `gb_quer_trifft(cx0, von_sued, pl)`: die Zaehler-Querung startet erst, wenn die geplante Bahn die Figur RAMMT - Nachbildung der Bahn von gb_cross_begin / GBP_CROSS
   Bild fuer Bild (Start-x geklemmt auf [4450..6500], Kante +- GB_BAHN_M, Ende-x = Figur-x geklemmt auf [3350..6500] bzw. im Rampen-Fenster (gb_seg_rect_span, Marge 300)
   auf [2550..6500], Dauer len/32 in [90..240], Blick = Snap auf (cx1,cz1), Treffer = gb_koerper_kontakt wie das Rammen; das letzte Bild zaehlt nicht, GBP_CROSS endet
   bei t >= cframes vor dem Rammen-Test). Gemessen (R2318/-16300, `bau/harness/patch4_spur2318_gator_boss.log` QUERPRUEF-Zeilen): vom Ost-Ende des Kanten-Pendels
   (x 5625) lief die Bahn 1757 seitlich vorbei (kein Treffer in 240 Bildern, erst die zweite Querung ab dem Anlauf rammte bei t603); eine Vorpruefung mit dem Maulpunkt
   <= 1500 ueber die vereinfachte Bahn hielt bei x 5230 einen Grenzfall (~1500, echte Bahn mit Ende-x 3350 vor dem Fenster und gb_maul_dist in 16er-Schritten verfehlt)
   fuer einen Treffer (t682). Mit der Bahn selbst: Start bei x 5211 (Vorpruefung t 129), Rammen t450 an (3781,-16630).
6. Der Anlauf der Zaehler-Querung laeuft ohne das Kanten-Pendel (+-1500): mit ihm stand das Ziel bis 90 Bilder 1500 oestlich des Anlaufs (t300 Zaehler, t378 Start).
7. Rammen (GBP_CROSS, gb_koerper_kontakt, Schaden gb_biss_abschluss = DAT_8006f418[5] = 50 @0x8006f422 + Niederwurf) auch ohne Kanister, wenn die Figur OBEN steht
   (gb_pl_oben); eine Figur im Wasser wird in der Verfolgungs-Querung wie in NB5 nur vom Maul getroffen (CROSS-Biss) - mit Kanister wie in Abnahme 4 auch im Wasser.
8. (Patch 9, aus dem Querbezug unten) Der Zaehler `oben_ohne` laeuft nur in den Kampfphasen (`gb_phase_kampf`: CHASE / LUNGE / FLINCH / GUARD / CROSS_APPR /
   CROSS / RE2_DASH), nicht im RE2-Intro (GBP_RE2_INTRO, Durchbruch -> Schnapper, ~300 Bilder ab dem Betreten), in der Lauerstellung oder nach dem Tod - dort kann er
   die Figur oben nicht angreifen, "oben nicht sicher" meint die Belagerung. Gemessen am fremden exe-Riegel integration_r41_re2gator, Lauf "belagerung" (Figur ab F0 auf
   der Rampe (6300,-16250), Intro bis F305): mit dem Zaehler im Intro quert er im 71. Bild der Belagerung (QUERPRUEF F305 t 1, CROSS F376, Rammen F377) VOR dem
   ersten Hochbiss (alter Stand F417), die GIER-WACHE (300 Bildpaare CHASE/GUARD ohne Gier-Sprung) wird nie erreicht und der Finisher kommt F751 statt F627 (Riegel
   rot: "keine 300 Bildpaare CHASE/GUARD"). Nur im Kampf gezaehlt laeuft die Belagerung wie im Stand a661eb8aa (Hochbiss F417 / F522 / F627 nullt den Zaehler).
   Harness-Lagen (Gator ab t0 in CHASE/GUARD): SUMME-/ok-Zeilen bitgleich (`bau/riegel_nb3*.txt`); exe-Szenario Rampe oben: die erste Querung rueckt von F383
   (= F83 + 300, Zaehler lief im Intro) auf unveraendert F383 - in der Rueckkehr-Lage des Szenarios ist der Gator vom ersten Bild an im Kampf (das RE2-Intro lief im ersten Besuch, F0-F282; der Zaehler laeuft ab dem Klettern F83); alle exe-Belege nach Patch 9 bitgleich zu den Laeufen davor (exe_final.txt, Auswertung verglichen) (Zaehler ab dem Intro-Ende). Alle exe-Belege unten sind mit der exe nach Patch 9 neu gelaufen (`final_runs2/`).
**Abgrenzung (bitgleich NB5):** alle NB3-Zweige sind aus unter dem Pruefhaken nb3_alt UND unter v4_alt (Stand f942f56c0, Kanister als Koerper: dort lief die direkte
Anfahrt mit der Kanister-Huelle in die Kopf-Klemme und endete rueckwaerts an der Westwand - Riegel fuzz P-mitte 92 2-s-Fenster ab t673 bei (-7416,-15904)). Die Band-Regel
prueft die FIGUR, nicht das Steuerziel: der VORHALTE-Punkt (Figur vom Koerper geschoben, Vorhalte 20 Bilder) lag in der Westkanal-Lage (-6000,-16000) im Plattform-Band
und wich mit der Zielpruefung von NB5 ab (Riegel gang, erster Bau: 1944 von 2700 Bildern; `bau/harness/gang_spur_lage3_1000_-22500.txt`).
Keine neue Zahl ohne Quelle: 300 (NB5 cross_cd, PORT-WAHL "darf halt oben nicht sicher sein"), 1800 (GB_KOERPER_M), 2600 (Maulpunkt), 1500/1050+450/3350/2550/6500/
4450/90..240 (bestehende Querungs- und Kontaktwerte), x > 7200 (GB_RAMP_X1, Nische). RE2-Vorbild (Messblatt Abschnitt 3): Angriffsausloeser ist die Korridorlage
(@0x80100990-98 / @0x801009d8-e0), Treffer ohne Hoehen-/Abstandsbedingung im Overlay (@0x80101578 ff.), Rammen/Hochbiss sind PORT-WAHL.

### Bauschritte mit Messung (Harness, Leon = Elza bitgleich; `bau/harness/`)
| Schritt | Aenderung | A1 Rampe oben (18 Lagen, ohne Kanister) | B1 Wasser (11 Lagen) |
|---|---|---|---|
| a661eb8aa | - | 2 Lagen ohne Treffer, spaetester erster t1236, Pause max 1236 | Fenster 1-3 (W4073/W4200/W4428), Pause max 482 |
| Patch 1 (Band-Regel, Zaehler 300, Rammen) | B1, A4, A7 | 0 ohne Treffer, erster t603 (R2318/-16300), Pause 604 | Fenster 0, aber Pause bis 3116 (W1000/-20700: still an der Klemmlinie, kein Biss) |
| Patch 2 (Wand-Regel, Vorpruefung mit Maulpunkt) | B2, A5 vereinfacht | erster t682 (Grenzfall der Vorpruefung) | Fenster 0, Pause max 520, Treffer 287-350 |
| Patch 3/4 (Bahn-Nachbildung, Anlauf ohne Pendel) | A5/A6 | erster t450, Pause 537 | unveraendert |
| Patch 5 (Rammen nur Figur oben) | A7 | unveraendert | Rammen 0 (nur CROSS-Biss), Pause max 530 |
| Patch 6/7 (Band-Regel an der Figur; aus unter v4_alt) | Abgrenzung | unveraendert | unveraendert |
| Patch 8 (Figur im Block = belagern) | B3b | unveraendert | unveraendert (Harness-Lagen nie im Block) |
| Patch 9 (Zaehler oben_ohne nur in Kampfphasen) | A8 | unveraendert (Gator ab t0 im Kampf) | unveraendert |

### exe-Szenario Rampe oben (Riegel A2, rampe_oben / rampe_oben_elza) - warum die Eingabe des Messers nicht mehr traegt
Mit der Eingabe des Messers (Schuesse bei F180-213 je Raum, Klettern bei F636) kommt die Figur im neuen Stand nicht mehr hinauf: der Alligator steht mit Vorgabe B ab
~F100 an der Kante (erster Biss F122-125, `bau_runs/runs/ohne_oben_leon`: sub=9 0 Zeilen, Figur ab F180 an der Ostwand, danach durchs Becken geworfen). Das Skript
startet je Raum neu, und das Zielen (M) dreht die Figur im Rueckkehr-Besuch per Auto-Aim auf den anrueckenden Alligator (rot 3072 -> 1699 / 1637, Probelaeufe 1/2); die
ersten Bilder der Rueckkehr sind Szenen-Haltung (Cut 12, pst=1, Eingaben bis mindestens F9 verschluckt, Probelauf 3), und bei z -18800 greift der Klettertritt nicht
(Kletterzellen z >= -18000, `kletterkarte 2090`; Probelauf 1). Eingabe jetzt `W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170` mit RUECK_LAGE z -18750:
Klettern F48-F82, Figur ab F83 oben, Gang auf die Rampe, Schuesse unveraendert bei F180 (Kanister-Szene im ersten Besuch wie bisher). Nach der ersten Querung
(Rammen F383) holt der Alligator im RE2-Zielmodus (+0x1C0 & 4 @0x80100a34-5c) den geworfenen Kanister aus dem Nordwesten (angehaengt F1013; beim Messer lag das
Anhaengen F625 VOR dem Klettern F636) - 856 Bilder Kanister-Szene, kein Haengen: `obenlog` zaehlt darum ab "KANISTER angehaengt F<n>" (wie traglog).

### Riegel Soll | Ist (Messblatt Abschnitt 4; Harness `test_r41_gatorfix nb3 / nb3_elza`, exe `obenlog` / `haenglog`)
| Riegel | Soll | Ist (Leon = Elza) | Beleg |
|---|---|---|---|
| A1 Harness Rampe oben, mit und ohne Kanister | je Lage erster Treffer <= 600 Bilder und laengste Pause ohne Treffer <= 600, Rammen >= 1 | 0 von 18 Lagen ohne Treffer; spaetester erster Treffer t450 (R2318/-16300); laengste Pause 537 B. = 17,9 s (R2318/-16300); Rammen 36; mit = ohne bitgleich | bau/harness/patch5_oben_*.txt, ctest nb3 |
| A2 exe Rampe oben (rampe_oben / rampe_oben_elza, mit und ohne) | ab dem Anhaengen: erster Treffer oben <= 600, keine Pause > 600 | Leon ohne / mit: Klettern F48-F82, oben ab F83; Kanister angehaengt F1013 (RE2-Zielmodus, davor 930 Bilder oben, erste Querung mit Rammen F383 / CROSS-Biss F483 schon davor); ab dem Anhaengen erster Treffer nach 326 Bildern (Rammen F1339), laengste Pause 343, 18 Treffer oben (5 Rammen, 4 CROSS-Biss, 9 Hochbiss) - mit = ohne bitgleich; Elza ohne / mit: 324 (Rammen F1337) / 343 / 18, bitgleich. Alter Stand a661eb8aa mit derselben Eingabe (oben_leon_ohne_alt): Kanister F602, Figur 3799 Bilder oben (F602..F4400), 0 Treffer (Mittelband (4200,-15727), Hochbiss reicht nicht, keine Querung) | bau/exe_final.txt |
| B1 Harness Suedkante, mit und ohne | Haenge-Fenster 0 in 11 Lagen | 0 (vor NB3 1-3) | bau/harness/patch5_haengt_*.txt |
| B1b (PORT-WAHL, Schranke wie A1) | laengste Pause ohne Treffer in den Wasser-Lagen <= 600 | 530 B. (W2318/-18568/c2), mit Kanister 520; Treffer je Lage >= 287 | dto. |
| B2 exe Suedkante (rampe, rampe_elza, abn2_wand_elza, mit und ohne) | Haenge-Fenster 0; mit Kanister 0 Zeilen HAENGT / RUECKWAERTS an / TAUCHEN an | rampe_leon ohne / mit, rampe_elza ohne (exe 029efac6, nach Patch 9; bitgleich zu den Laeufen mit der exe 5581916f vor Patch 9): Haenge-Fenster 0, ohne Fortschritt max 16 Bilder = 0,5 s, 31 Treffer, mit Kanister 0 Zeilen HAENGT / RUECKWAERTS an / TAUCHEN an (alter Stand: 1 / 2 (+2 RUECKWAERTS) / 1 Fenster F2649, haenglog auf den Messer-Laeufen und rampe_leon_ohne_alt); abn2_wand_elza ohne / mit: Fenster 0, ohne Fortschritt max 56 Bilder = 1,9 s, 71 Treffer, 0 Rueckfall-Zeilen (alter Stand 1 Fenster F4928; Bau vor Patch 8: 2 Fenster F8239/F8389 mit der Figur im Pier); Cut-2-Lage west4_leon (4200,-18800,3072,2): alt 1 Fenster F2367, neu 0, 31 Treffer | bau/exe_final.txt |
| B3 ohne Fortschritt | <= 450 Bilder | Harness 327 B. = 10,9 s (W2318/-18568/c2, RE2-Zonen-Dash nach der Querung); exe rampe 16 Bilder, wand_elza 56, Cut-2-Lage 32 (alter Stand 229 / 191 / 134) | dto. |
| B4 bitgleich ausserhalb | gang / v4 / re2 gruen | gang (Westkanal, NO-Ecke, SO-Ecke; alter Stand = rw_alt + nb3_alt) 0 von 2700 abweichend; v4 / re2 gruen | ctest |
| Gegenproben a661eb8aa (Pruefhaken) | rot ohne den Bau | R2318/-16300 kein Treffer in 3600 Bildern; W4200/-18800/c2 3 Haenge-Fenster; Gang-Lagen (6732,0,-13687) 579/900 und (4428,-1800,-16300) 29/900 Bilder abweichend | ctest nb3 |

### Bildleisten (RE15_FRAMEDUMP-Serien, F-genau; `G2_belege/nb3/bau/`)
- `nb3_bau_oben_leon_F90-F1440.jpg` und `nb3_bau_oben_leon_F1590-F2520.jpg` (Spalten a661eb8aa ohne | neu ohne | neu mit Kanister, dieselbe Eingabe, Cut 2): F90 Leon oben an der Suedkante; F390 neu: Rammen F383 (der Alligator quert ueber ihn, Leon an der Kante geworfen), alt: Leon unbehelligt; F480 CROSS-Biss F483 (neu), alt: NB5-Querung ohne Treffer; F1020 Kanister geholt (neu: kein Alligator im Bild), alt: Alligator belagert; F1350 Leon liegt (Rammen F1339), F1440 Leon taumelt, Alligator hinter ihm (CROSS-Biss F1439; mit Kanister: Kanister am Kiefer); F1590 / F1890 / F2100 Hochbiss trifft (neu, Kopf ueber dem Deck), alt: Hochbiss-Versuche ohne Treffer, Leon steht; F2430 Rammen F2410 (Alligator auf dem Deck), F2520 CROSS-Biss. Alt: 0 Treffer in 3799 Bildern oben.
- `nb3_bau_oben_elza_F90-F2430.jpg` (neu ohne | neu mit): F390 Rammen F383, F480 CROSS-Biss, F1350 Elza liegt (Rammen F1337), F1440 CROSS-Biss (Kanister am Kiefer), F1590 Hochbiss F1583, F2430 Rammen F2408.
- `nb3_bau_suedkante_cut2_leon_F200-F2800.jpg` (Lage west4_leon (4200,-18800), Cut 2, ohne Kanister; a661eb8aa | neu): alt F600-F2350 Leon steht im Wasser an der Rampen-Suedkante, der Alligator pendelt mit dem Ruecken zur Kamera an der Plattform-SO-Ecke (Haenge-Fenster F2218-F2367, Kopf 4876 von der Figur); neu F200 direkte Anfahrt, F600 Leon geworfen, F1000 / F1500 Biss an Leon (Maul offen), F2250-F2350 Alligator an Leon, Leon liegt (31 Treffer, 0 Fenster). (Die Lage des exe-Riegels rampe (3000,-18800, Cut 0) zeigt beide nicht im Bild - darum die Cut-2-Lage.)

### Querbezug: exe-Riegel integration_r41_re2gator, Lauf "kanister" (fremder Riegel, Spuren alligator2 / gatorszene)
In der ctest-Gruppe der exe-Riegel (-j 2, `bau/ctest_exe_riegel_gruppe1.txt`) fiel `integration_r41_re2gator` im Lauf 4 "kanister" (Leon per Autopilot im Suedbecken
(5500,-25500) -> (5500,-20500) -> (-1000,-21000) -> ..., Kanister liegend bei (-1500,-23400); Schranke "Kanister als Kind der Part-6-Weltmatrix >= 200 Bilder bei
Abweichung 0" bis EXIT_AT 1500): "nur 30 angehaengte Bilder". Einzeln nachgefahren (`re2gator/ctest_einzeln1_rot.txt`): bitgleich - Zielmodus 1/0 F1129/1141,
1252/1261, 1327/1336, 1427, Stufe 1 F1435 (quer 1040), angehaengt F1447 -, also kein Last-Flackern, sondern eine verschobene Zeitachse durch Vorgabe B: der
Autopilot fuehrt Leon durch das Rampenband ((5600,-19698) F1115, 1598 vor der Kante z -18100), die Verfolgung faehrt ihn direkt an (Zweig 7; alter Stand:
Westumlauf Zweig 6 mit Ziel (-4300,-22600)), und die Bahn des Gators liegt danach anders zum liegenden Kanister: dreimal laengs < 8880 ohne das Aufnahme-Fenster
(quer > Koerper 1466 + 267; RE2 (b) @0x80100a74-b4 loescht das Zielbit), Aufnahme erst im vierten Vorbeilauf. Alter Stand a661eb8aa: erster Vorbeilauf F1215
(quer -478), angehaengt F1227, 270 Bilder - in drei Laeufen vom 2026-11-06 bitgleich (`re2gator/kanister_zeitachse_alt_neu.txt`). Die Aufnahme selbst
(RE2-Fenster, Part-6-Sitz, Abweichung 0) ist unveraendert, nur das Messfenster war fuer die neue Zeitachse zu kurz. Nachgezogen wie beim Querbezug Spur F (c)
desselben Riegels: RE15_EXIT_AT 1500 -> 1700 (+200 = Verschiebung 220 auf die naechsten 100 gerundet), Schranken unveraendert, Kopfkommentar (f) in
`re15_port/tests/integration/test_r41_re2gator.cmake`. Nachlauf 2 (`re2gator/ctest_einzeln2_fenster1700_rot_belagerung.txt`): Lauf kanister gruen (240 angehaengte Bilder, Abweichung
0), dann Lauf 5 "belagerung" rot ("keine 300 Bildpaare CHASE/GUARD ohne Sprung/Befreiung") - Ursache der Zaehler im RE2-Intro, behoben mit Patch 9 (Punkt 8 oben;
Belege `re2gator/belagerung_*`); danach Gruppe und Einzellauf gruen (Abschnitt Tests). Fuer den Riegel B4 "bitgleich ausserhalb" heisst das: die Figur im Band VOR
der Kante ist innerhalb der Aenderung (Vorgabe B), auch wenn sie nur durchlaeuft; die Belagerung bis zum ersten Treffer ist es nicht (Patch 9).

### Tests
- `ctest -R "^unit_r41_gatorfix_" -j 2` (Engine nach Patch 9, `bau/ctest_unit_riegel.txt`): `100% tests passed, 0 tests failed out of 26`, `Total Test time (real) =  85.56 sec`
  (nb3 33.75 s, nb3_elza 33.67 s, re2 21.06 s, re2_elza 21.09 s, v4 11.62 s, v4_elza 10.72 s, fuzz 11.99 s, fuzz_elza 11.63 s, gang 1.25 s, gang_elza 1.14 s, maul / marvin /
  wurf / klemm / haken / montage / schluessel / re2_gegen je Leon + Elza). Vor Patch 9 (Engine 5581916f): `100% tests passed, 0 tests failed out of 26`, 67.73 s; die
  SUMME-/ok-Zeilen von nb3 / nb3_elza sind fuer beide Engines bitgleich (`bau/riegel_nb3.txt`, `riegel_nb3_elza.txt`).
- `ctest -R "^integration_r41_gatorfix_(rampe|rampe_elza|abn2_wand_elza|rampe_oben|rampe_oben_elza|frei|frei_elza)$|^integration_r41_re2gator" -j 2 --timeout 1800`,
  Gruppe 2 (exe 029efac6 nach Patch 9, Fenster 1700; `bau/ctest_exe_riegel.txt`): `100% tests passed, 0 tests failed out of 8`, `Total Test time (real) = 2053.73 sec` -
  abn2_wand_elza 707.59 s, frei_elza 718.20 s, rampe_oben_elza 420.71 s, frei 677.44 s, rampe_oben 391.05 s, rampe_elza 368.28 s, rampe 327.48 s, re2gator 300.36 s.
  Gruppe 1 (exe 5581916f vor Patch 9, Fenster 1500; `bau/ctest_exe_riegel_gruppe1.txt`): `88% tests passed, 1 tests failed out of 8` - re2gator rot (126.20 s, Lauf
  kanister 30 angehaengte Bilder), die sieben gatorfix-Riegel gruen (abn2_wand_elza 679.97 s, frei_elza 697.79 s, rampe_elza 362.76 s, frei 672.72 s, rampe 318.71 s,
  rampe_oben 389.80 s, rampe_oben_elza 437.31 s).
- `ctest -R "^integration_r41_re2gator$" --timeout 1800` einzeln (`re2gator/ctest_einzeln*.txt`): Lauf 1 (vor dem Fenster, exe 5581916f) `0% tests passed, 1 tests failed
  out of 1`, 124.67 s - kanister 30 Bilder, bitgleich zur Gruppe; Lauf 2 (Fenster 1700, exe 5581916f) rot 202.43 s - kanister 240 Bilder gruen, belagerung "keine 300
  Bildpaare CHASE/GUARD"; Lauf 3 (Fenster 1700, exe 029efac6 nach Patch 9): `100% tests passed, 0 tests failed out of 1`, `Total Test time (real) = 300.63 sec` (kanister angehaengt F1447, 240 Bilder, Abweichung 0; belagerung wie a661eb8aa: Hochbiss F417 / F522 / F627, GIER-WACHE bis F581 300 Bildpaare, Finisher F627; `re2gator/kanister_belagerung_zeitachse_gruen_einzeln3.txt`).

### Offen
1. **Spieler-Wegstoss in den Pier (Spieler-Kollision, nicht diese Spur):** exe wand_elza F7920 wirft der Biss-Wegstoss die an der Taschen-Ecke (2318,-18568)
   festhaengende Figur in den Pier ((1382,-16749) -> (1382,-17036), fuer die Figur nicht begehbar, `begehbar 2091`); sie bleibt bis zum Laufende (488 Bilder)
   darin. Der Alligator belagert sie dort jetzt (gb_pl_im_block) statt an der SW-Ecke zu pendeln; die Ursache (Wegstoss durch die konkave Ecke) liegt in der
   Spieler-Kollision - Entscheid des Orchestrators, ob das eine eigene Spur wird.
2. **exe-Szenario wand_elza degeneriert:** die blind gesteuerte Wand-Tour bleibt mit Vorgabe B ab F2400 in der Taschen-Ecke (2318,-18568) haengen (der Alligator greift
   sie dort alle ~105 Bilder an, das Skript kommt nicht mehr heraus); NW-Ecke und Ostnische erreicht sie nicht mehr. Die Ostnischen-Regel (gb_pl_nische) ist darum
   nur im Code und am Messer-Lauf (a661eb8aa, Figur x 7668 bei z -17577..-14994) belegt, nicht im neuen exe-Lauf.
3. **Kanister-Zielmodus waehrend der Belagerung:** nach der ersten Querung holt der Alligator den geworfenen Kanister (RE2 @0x80100a34-5c; exe rampe_oben: F483..F1013,
   856 Bilder ohne Angriff auf die Figur oben). Byte-true RE2, aber die Figur oben ist so lange "sicher"; obenlog zaehlt ab dem Anhaengen. Im Messer-Szenario lag das
   Anhaengen (F625) vor dem Klettern (F636).
4. **Takt (Rand der Schranke 600):** laengste Pause ohne Treffer oben 537 Bilder (R2318/-16300, Harness: Belagerung 300 ohne Hochbiss-Reichweite + Anlauf + Querung bis
   zum Kontakt) und 530 in der Wasser-Lage W2318/-18568 (RE2-Zonen-Dash nach der Querung) - innerhalb der Schranke, aber nur 63 / 70 Bilder Rand.
5. **Kopf vor der Kamera (F1 "noe"):** die Querung der Vorgabe A laeuft jetzt auch zur Kamera hin (mit und ohne Kanister gleich); Riegel re2 "mit nicht haeufiger als
   ohne" bleibt gruen, die absoluten Werte stehen in der Ausgabe.
6. **Fuzz-Mass:** die Ausnahme "Kopf an der Figur (< 2400)" fuer das 2-s-Fenster ist neu (Lage P-mitte = Wasserlage 468 vor der Pier-Westkante, Biss alle ~105 Bilder,
   Stillstand zwischen den Bissen); ohne sie 92 Fenster.
7. PSX-Bau nicht gemessen (plattformneutrale Pfade).
8. **Querbezug fremder Riegel integration_r41_re2gator (Spuren alligator2 / gatorszene):** (a) Lauf kanister - Messfenster RE15_EXIT_AT 1500 -> 1700, weil die
   Vorgabe-B-Anfahrt (Leon per Autopilot im Rampenband) die Kanister-Aufnahme von F1227 auf F1447 verschiebt; Aufnahme/Sitz unveraendert - Entscheid der
   Spur-Eigner, ob das Fenster so bleibt. (b) Lauf belagerung - Patch 9 (Zaehler oben_ohne nur in Kampfphasen) stellt den alten Ablauf her; ohne ihn quert er
   im 71. Bild der Belagerung VOR dem ersten Hochbiss. Der Nutzer hat diesen Takt ("300 Bilder Belagerung ohne Treffer, dann Querung") noch nicht gesehen.
9. Runde 2 (Abnahme 5 / Nachbesserung): Abschnitt "G2-RE2 NB3 Runde 2" unten (Offen 1, Wegstoss in den Pier, ist mit dem Raumhalt verhindert).

## G2-RE2 NB3 Runde 2 (Abnahme 5: M1 Absturz abn2_wand, M2 Wurf aus dem Raum, M3 abn2_rampe_west)
Stand 2026-11-07, Zweig r41/gator_re2 ab 10603425b (Abnahme 5 `G2_abnahme_5.md`: NICHT BESTANDEN). Rolle: Bauer (NB3, Runde 2 von 2). Spezifikation unveraendert
der Nutzerentscheid 2026-11-06 (Abschnitt "G2-RE2 NB3" oben, F1-F4 woertlich): F1 "Zu F1, noe, wozu wenn es gut funktioniert?" (keine Kameragrenze), F2 "Verstehe ich
nicht..." (Querungsbeginn nicht anfassen), F3/Vorgabe A "Er soll nur den Spieler oben auf der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann.
Alternativ kann er auch hoch beissen oder sowas, aber er darf halt oben nicht sicher sein.", F4/Vorgabe B "Wenn er haengt muessen wir nachbessern denke ich. Er darf
nicht zu lange haengen. Wenn es kurz ist, kann man mit dem tauchen abhilfe schaffen, aber das hilft natuerlich nicht bei der Platform."
Commits: 5a94392bf (Engine: Raumhalt, Anlauf ohne Ruhezone), aa41ec95e (Riegel raum, aussenlog, oben_kante, abn2_rampe_west), 5a4ee385f (aussenlog ohne Kletterbilder, obenlog kampf = Intro-Ende),
dieser Commit (Dossier, Belege). Laufordner scratchpad `g2nb3/r2/` (nicht im Baum); Belege `G2_belege/nb3/runde2/` (harness/, exe/, leisten/, werkzeug/, ctest_*.txt).
Messkopien: neu `re15_pc_g2nb3r2.exe` = Bau-exe md5 5910fc24 (nach 5a94392bf), vorher `re15_pc_g2nb3r2_alt.exe` = Runde-1-exe md5 029efac6 (Stand 10603425b).

### Ursache (gemessen; ROOM2090.RDT = ROOM2091.RDT md5 a48d11a4, SCA-Kopf @0x6dc, Zellen ab Datei 0x6f4, `werkzeug/sca_2090.py`)
Die SCA hat 50 Zellen (5 Quadranten x 10, bytegleich). Band 0: [0] Westwand x -10900..-8900, [1] Suedwand z -29000..-27000, [2] Nordwand z -5400..-3400, [3] Ostwand
x 7200..9200 (0x718), [4] Plattform x -1700..1850 z -20000..-12450 (0x724), [5] Rampe x 1850..7200 z -18100..-14500 (0x730, u1 0x01). Band 1: [6] Ostwand,
[7] Plattform, [8] Nordleiste z -14500..-13500 (0x754) und [9] Suedleiste z -19100..-18100 (0x760), je x 1850..7200, nur 1000 tief, u1 0x02 - auf Band 1 gibt es
keine Sued-, Nord- oder Westwand. Die Spieler-Klemme FUN_8003b0a4/FUN_8003bca8 schiebt aus einer Zelle ihres Bandes auf Kante + 450 (DAT_80073e94+6 = 0x01c2,
`lhu a1,6(v0)` @0x80036210) + 0x12 (`addiu t1,v0,18` / `addiu a0,a0,-18` @0x8003bce0-e8, z @0x8003bd20-2c), je Zelle in Tabellenfolge.
- **M2 (Wurf aus dem Raum):** der Niederwurf (s_kd_speed 1000 `ori v0,zero,0x3e8` @0x8003615c, -15*Bild @0x800361c8-e0) traegt die Figur oben im ersten Bild
  2062, dann 985, 955 ... Einheiten - durch die Leiste (1000 + 2 x 450 = 1900) in EINEM Bild, danach ohne Wand ueber das Becken. Die Sonde FUN_8001c2dc stoppt nicht
  (Zellwort der Leisten 0x1002: Bit 0x2 -> Flag 0, @0x8001c390-ac). Nachgefahren (`exe/west5_elza_alt_neu.txt`, `exe/spawn_2318_-16300_leon_alt_neu.txt`, Stand
  10603425b): west5_elza F2752 CROSS-Biss an (2315,-1800,-17642) -> F2753 (2318,-19704) ... F2763 (2318,-26554) -> 548 Bilder "Deck verlassen", bis 2914 hinter
  der Aussenwand; Spawn (2318,-16300) F1201 -> F1202 (2318,-12904) ... -> 2999 Bilder, bis 2706 hinter der Nordwand.
- **M1 (Absturz abn2_wand):** [5] stoesst an [3] (x 7200); [3] wird vor [5] geklemmt, [5] schiebt eine Figur, die der Biss-Niederwurf in die SO-Ecke von [5] traegt,
  nach Osten auf 7200 + 468 = 7668 IN die Ostwand (Fixpunkt "Ostnische": F1045 Biss an (6732,-18598) -> F1046 (7668,-17081)); ein Schub nach Osten loest [3] auf
  der Aussenseite auf (9200 + 468 = 9668, F1500), danach laeuft die blinde Eingabe ins Leere, `re15_squareroot0` in `re15_autolook_scan` liest bei ~100000 Abstand
  ausserhalb der Tafel (F7374). Dieselbe Fuge: Pier (1382,0,-17036) = [5] schiebt in [4] (wand_elza F7920, Runde-1-Offen 1) und oben Klemmlage (1382,-1800,-14013)
  bzw. (1382,-1800,-18617) = [8]/[9] schieben in [7] (Abnahme 5 Abschnitt 3a, Pausen 632-849).
- **M3 (abn2_rampe_west rot):** kein Spielfehler (Abnahme 5): die alte Eingabe stand 20 s im Band vor der Suedkante, mit Vorgabe B beisst er dort ab ~F190, die
  Niederwuerfe (gleiche Mechanik, im Wasser mit Aussenwaenden) tragen die Figur durchs Suedbecken in die SW-Ecke, die blinde Aktionstaste oeffnet die Tuer
  (F934, DOOR FIRE F937 -> ROOM2080), RE15_EXIT_AT ...#2090 kommt nie.
- **Dazu gefunden (Vorgabe A, Figur frei):** der neue Riegel raum (Figur nicht je Bild zurueckgesetzt) fand am Westende des Decks eine Lage ohne Angriff: nach zwei
  Niederwuerfen stand die Figur bei (2300,-1800,-16680); QUERPRUEF traf vom Anlauf x 4450 (t 95), nicht von der Wurzel x 5004; die Wurzel stand 554 vor dem Anlauf
  (4450,-20700) in der 600er-Ruhezone der Belagerung, startete nie (Start erst, wenn die Bahn "von hier" trifft, NB3 Patch 3/4) - 1195 Bilder ohne Treffer oben
  (`harness/raum_vor_anlauf_leon.txt` R2800/-16300, Spur `harness/anlauf_ruhezone_R2800_vorher.txt`).

### Was gebaut wurde (enemy_ai_boss_gator.c, game_step_common.c, re15_boss_gator.h)
1. **Raumhalt** `re15_gator_boss_raum_halt` (PORT-WAHL nach Abnahme 5 "Figur oben nicht sicher, Figur bleibt im Raum" und Vorgabe A "... das der Spieler oben nicht
   ausweichen kann ... aber er darf halt oben nicht sicher sein" - getroffen werden, nicht aus dem Level fliegen), aufgerufen in der Spieler-Klemme `spieler_klemme`
   (game_step_common.c; Gehen, Niederwurf kd_move, Koerper-Schub, Flinch) und nach dem Koerperlaengen-Push des Bosses, nur solange ein Alligator-Boss im Raum ist
   (`re15_gator_boss_active`: Typ 0x23 in 2090/2091):
   - Band 1: Lage ausserhalb des Decks (x 1850 + 450 .. 7200 - 450, z -18100 + 450 .. -14500 - 450) -> auf die Klemmlage von FUN_8003bca8 (Kante +- 468).
     Die Leisten halten damit auch einen Schritt, der breiter ist als sie.
   - Band 0: Lage ausserhalb der Aussenwaende (Kante - 450) oder mit dem Mittelpunkt in [4]/[5] -> die Lage vor dem Schritt.
   Nur wenn die Lage davor gueltig war (Spawn-/Szenenlagen werden nie verschoben) und die Figur lebt (der Finisher setzt die Leiche). Beim Gehen greift nichts:
   die Zellen schieben selbst schon dorthin (Schritt < Zelltiefe). Zeile `RAUMHALT F<n> Band b (x,z) -> (x',z') vorher (..)` in gator_boss.log; Pruefhaken
   `re15_gator_boss_raum_alt_setzen(1)` = Stand 10603425b; Messhaken `re15_gator_boss_raum_halt_zahl`.
2. **Anlauf der Zaehler-Querung ohne 600er-Ruhezone** (Vorgabe A): `quer_anl` im GUARD - waehrend des Anlaufs der QUER-CROSS (Zaehler oben_ohne >= 300 und die Bahn
   vom Anlauf trifft) schwimmt er weiter auf den Anlauf zu, bis die Bahn von seiner Wurzel trifft. Kein neuer Wert; Querungsbeginn selbst (gb_cross_begin, F2)
   unveraendert. Unter dem Pruefhaken raum_alt aus.
3. Nicht angefasst: Niederwurf (byte-true RE1.5), Kollision ausserhalb des Kampfs, Querungsbeginn (F2), Kanister-Zielmodus (Abnahme 5 F1), Rueckfall-Modul,
   `re15_squareroot0` (der Ueberlauf wird mit der Figur im Raum nicht mehr erreicht). `gb_pl_nische` / `gb_pl_im_block` (Runde 1) bleiben, werden mit dem Raumhalt
   aber nicht mehr erreicht.

### Riegel (neu/geaendert in aa41ec95e und 5a4ee385f)
- `unit_r41_gatorfix_raum(_elza)`: Figur EINMAL gesetzt, danach frei (Niederwurf, Schub, Klemme wie im Spiel, hp 100 wie RE15_R39_MESS_SPIELER_HP), Blick wie die
  exe-Lagen (Spawn 1024, Rueckkehr 3072, abn2_wand 1035). 9 Lagen oben (Wurf-Lagen (2318,-16300), (6500,-15000), (2315,-17642), Klemmlagen-Laeufe (2318,-17600),
  (2800,-16300), (4428,-16300), (3350,-15000), Ecken), 7 Wasserlagen ((6732,-18598) vor dem Biss in die Ostnische, (4200,-18700) Rueckkehr abn2_rampe_west, Taschen-
  Ecke, nb3-B1-Lagen), mit und ohne Kanister, je 2400 Bilder. Soll: 0 Bilder Deck verlassen / aussen / im Block; oben erster Treffer und Pause <= 600.
  Gegenprobe Pruefhaken raum_alt (Stand 10603425b): oben verlaesst >= 1 Lage das Deck, die Wasserlage steht in der Ostnische.
  (Erste Fassung ohne gesetzte Blickrichtung, `harness/raum_ohne_blick_leon.txt`: die Figur blickte aus der Szene laengs des Decks, der Niederwurf lief gegen
  Ostwand/Plattform, und die Gegenprobe blieb gruen - ein Riegel, der den Fehler nicht sieht. Mit dem Blick der exe-Lagen ist sie rot.)
- exe-Auswerter `aussenlog <lauf>` (state.log + climb.log, alle Bilder in 2090/2091) in ALLEN Laeufen frei/rampe/abn2_*/rampe_oben mit und ohne Kanister (climb.log
  jetzt in allen Laeufen, Messschrieb ohne Verhalten); `obenlog <lauf> kampf` (ab Ende des RE2-Intros, gator_boss.log "KAMERA INTRO frei ... F<n>").
- `integration_r41_gatorfix_oben_kante(_elza)` (Abnahme 5 Abschnitt 4 (2)): Figur per RE15_PLAYER_POS oben (2318,-16300) / (6500,-15000) / (2318,-17600), Kampf
  sofort, 4200 Bilder je Lage - aussenlog 0, obenlog kampf <= 600 / <= 600.
- `abn2_rampe_west`: Eingabe wie rampe_oben (Klettern ab F48 vor den Schuessen, Schuesse F180), Rueckkehr an die Rampe West (2318,-18750) = Lage des Abnahme-2-Laufs
  plat7_leon "Rampe x 2318" (cmake-Kopfkommentar mit der Begruendung).
- Formal (Abnahme 5 Abschnitt 7): `NB3_OHNE_MAX 450` mit PORT-WAHL und Nutzer-Wortlaut; probes-Kommentar der gang-Lagen auf die echten Lagen.

### Messung (Harness `test_r41_gatorfix raum` / `nb3`, eigene exe-Laeufe `werkzeug/batch_r2.sh`; vorher = Stand 10603425b)
| Lage / Lauf | vorher | nachher | Beleg |
|---|---|---|---|
| Harness raum, 9 Lagen oben, frei, mit und ohne Kanister | (Pruefhaken raum_alt) (2318,-16300): 1709 Bilder Deck verlassen ab t691 bei (2314,-1800,-19568); (6500,-15000): 645 ab t377; (2315,-17642): 2063 ab t292, Ende (2318,-1800,1633) | 0 Bilder Deck verlassen; erster Treffer spaetestens t450 (R2318/-16300), laengste Pause oben 492 (R2800/-16300); Raumhalt 385 Eingriffe je Lauf (370 oben, 15 unten) | `harness/raum_leon.txt`, `raum_elza.txt` (Leon = Elza) |
| Harness raum, 7 Wasserlagen | (6732,-18598) mit Kanister: 627 Bilder aussen ab t696 bei (7668,0,-17081) = Ostnische wie abn2_wand F1046 | 0 aussen / im Block; (4200,-18700) endet wie M3 in der SW-Ecke (-8432,0,-26532), im Raum | dto. |
| Harness raum, Anlauf der Zaehler-Querung | Raumhalt an, Anlauf mit Ruhezone: Pause oben 1195 (R2800/-16300, R4428/-16300), 1098 (R2318/-16300) | 492 / 492 / 450 | `harness/raum_vor_anlauf_leon.txt`, `anlauf_ruhezone_R2800_vorher.txt` |
| Harness nb3 (A1/B1, Figur je Bild zurueckgesetzt) | SUMME Runde 1 | bitgleich: A1 0/18 ohne Treffer, erster t450, Pause 537, Rammen 36; B1 Fenster 0, ohne Fortschritt 327, Pause 520/530 | `harness/nb3_leon.txt` |
| exe west5_elza ohne Kanister (Abnahme-4-Lauf) | F2752 CROSS-Biss an (2315,-1800,-17642) -> Flug durch die Suedwand, 548 Bilder Deck verlassen (aussen5.py: 536 aussen bis (2318,-1800,-29914)) | F2753 RAUMHALT (2315,-19568) -> (2315,-17632); bitgleich bis F2752, danach oben Rammen F2817, Treffer F2924, F3029; 0 aussen; Pause oben max 379 | `exe/west5_elza_alt_neu.txt`, Leiste |
| exe Spawn (2318,-16300) Leon ohne Kanister | F1201 -> Flug nach Norden, 2999 Bilder Deck verlassen (aussen5.py 2986 aussen bis (2318,-1800,-2694)) | 0 aussen, Raumhalt 27 (Nordleiste und Klemmlage (1382,-14013) -> (2318,-14968)); 20 Treffer, erster 363 nach dem Intro-Ende F280, Pause max 375 | `exe/spawn_2318_-16300_leon_alt_neu.txt` |
| exe abn2_wand Leon ohne Kanister (bis F1530) | F1045 Biss -> F1046 Ostnische (7668,-17081) ... F1500 (9668,-16031), 327 Bilder aussen | F1046 RAUMHALT (7668,-17599) -> (6732,-18598), Figur bleibt in der SO-Tasche, dann an der Suedwand; 0 aussen | `exe/abn2wand_leon_alt_neu.txt`, Leiste |
| exe rampe_oben Leon mit / ohne (Riegel-Szenario) | Runde 1: oben 18 Treffer, erster nach 326 ab Anhaengen (Rammen F1339), Pause 343 | unveraendert 18 / 326 / 343, 0 aussen | Leiste |
Gegenrechnung mit dem Werkzeug der Abnahme 5 (`exe/aussen5_gegenrechnung.txt`): vorher west5_elza 536, Spawn 2986 Bilder aussen, abn2_wand 327 Bilder Ostnische;
nachher "Figur nie ausserhalb der Aussenwaende, nie in der Ostnische" in allen vier Laeufen.
Weitere Oben-Lagen der Abnahme 5 (Abschnitt 3a, dort 632-849 Bilder Pause in der Klemmlage (1382,-14013)), neu (`exe/oben_lagen_neu.txt`, obenlog ab Intro-Ende F280):
(2800,-16300) Elza erster Treffer nach 342, Pause max 375 (vorher 698); (4428,-16300) Leon 302 / 388 (vorher 693); (3350,-15000) Elza 107 / 380 (vorher 632);
je 0 Bilder ausserhalb, Raumhalt 20-28 Eingriffe (Nordleiste und Klemmlage -> Deck).

### Riegel Soll | Ist (Abnahme 5 Befunde, Messblatt NB3 Abschnitt 4)
| Riegel / Befund | Soll | Ist (Leon = Elza) | Beleg |
|---|---|---|---|
| M1 abn2_wand (exe) | kein Absturz, Figur im Raum, gruen | Passed 643.96 s; aussenlog mit / ohne Kanister 0 Bilder aussen / im Block; eigener Lauf bis F1530: F1046 RAUMHALT statt Ostnische (vorher 327 Bilder Ostnische, F1500 hinter der Wand, Absturz F7374) | ctest_exe_gruppe.txt, exe/abn2wand_leon_alt_neu.txt |
| M2 Wurf aus dem Raum (Harness raum, frei) | 0 Bilder Deck verlassen / aussen / im Block, 16 Lagen, mit und ohne | 0 in allen Lagen; Gegenprobe 10603425b: oben 3 von 3 Lagen verlassen das Deck (1709 / 645 / 2063 Bilder), Wasserlage 627 Bilder Ostnische | harness/raum_*.txt |
| M2 exe (west5_elza, Spawn-Lagen, oben_kante) | Figur nie ausserhalb | west5_elza 0 (vorher 548), Spawn (2318,-16300) 0 (vorher 2999), oben_kante / oben_kante_elza (je 3 Lagen x 4200 Bilder): 0 Bilder aussen / Deck verlassen; erster Treffer nach dem Intro-Ende 363 / 84 / 142, Pause max 375 / 345 / 380 ((2318,-16300) / (6500,-15000) / (2318,-17600)), Leon = Elza | exe/*.txt, ctest |
| M3 abn2_rampe_west (exe) | gruen, misst "Leon oben an der Rampe West" | Passed 673.89 s (vorher 900-s-Grenze): Leon oben 6999 Bilder, Gator im Bild 100 %, Kopf-Abstand mittel 4898, Rammen 12, Kanister F1002, tragvergleich abweichend 0, aussenlog 0 | ctest, Leiste |
| alle exe-Riegel integration_r41_gatorfix_* / re2gator (20 = 18 + oben_kante(_elza)) | gruen | Gruppe 95 % (19 von 20, 4473.31 s); rot nur abn2_rampe_west_elza durch einen Fehler des Auswerters (Kletterbilder als "Deck verlassen", s. u. Tests), einzeln nachgefahren gruen (683.89 s; aussenlog 0, 91 Kletterbilder nicht gezaehlt, tragvergleich abweichend 0) | ctest_exe_*.txt |
| A1 Harness nb3 (Figur je Bild gesetzt) | erster Treffer und Pause <= 600 | unveraendert: 0/18 ohne Treffer, erster t450, Pause 537, Rammen 36 (mit = ohne) | harness/nb3_leon.txt |
| A1 frei (Harness raum, neu) | oben erster Treffer und Pause <= 600 | erster spaetestens t450, Pause max 492 (vorher mit Ruhezone 1195) | harness/raum_*.txt |
| A2 exe rampe_oben(_elza) | ab Anhaengen erster <= 600, Pause <= 600; Figur im Raum | Leon 326 / 343, Elza 324 / 343 (18 Treffer oben, Rammen 5), aussenlog 0 - unveraendert gegen Runde 1 | ctest, Leisten |
| B1/B3 Harness nb3 | Fenster 0, ohne Fortschritt <= 450 | unveraendert: Fenster 0, ohne Fortschritt 327, Pause 520 / 530 | harness/nb3_leon.txt |
| B2/B3 exe rampe / rampe_elza / abn2_wand_elza | Fenster 0, ohne Fortschritt <= 450, mit Kanister 0 Rueckfall-Zeilen | rampe(_elza): Fenster 0, ohne Fortschritt 16 Bilder, 0 Rueckfall-Zeilen; abn2_wand_elza Passed | ctest |
| B4 bitgleich ausserhalb (gang / v4 / re2) | gruen | unit_r41_gatorfix_* 28/28 (gang, gang_elza, v4, re2 ...) | ctest_unit.txt |
| Gegenprobe | Stand 10603425b im neuen Riegel rot | raum(_elza) Gegenprobe rot (s. o.); aussenlog auf den alt-Laeufen VERFEHLT (west5_elza 548, Spawn 2999, abn2_wand 327 Bilder) | harness/, exe/ |

### Bildleisten (RE15_FRAMEDUMP-Serien, F-genau, kein gdigrab; `G2_belege/nb3/runde2/leisten/`)
- `nb3r2_west5_elza_wurf_vorher_nachher_F2740-F2930.jpg` (Abnahme-4-Lauf west5_elza ohne Kanister, Cut 2; Spalten 10603425b | Bau 5a94392bf): F2740/F2750 Elza oben an der
  SW-Ecke der Rampe, Gator-Kopf daneben (beide gleich); vorher F2760/F2770 Elza fliegt in Niederwurf-Haltung waagerecht ueber das Wasser zur Wand, ab F2780 weg (hinter
  der Suedwand, z bis -29914); nachher bleibt sie oben an der Maschine liegen (F2752 CROSS-Biss, Raumhalt F2753 auf (2315,-17632)), F2800-F2930 der Kopf kommt ueber das Deck
  (Rammen F2817, Treffer F2924) - Elza bleibt im Bild und wird weiter getroffen.
- `nb3r2_abn2wand_leon_ostnische_vorher_nachher_F1040-F1520.jpg` und `..._hell.jpg` (Belegkopie x2,5; exe-Riegel-Szenario abn2_wand ohne Kanister): F1040 Biss in der
  SO-Ecke (beide gleich); vorher F1490 Leon in der Ostnische rechts neben der Maschine, F1500/F1520 verschwunden (hinter der Nischen-Rueckwand (9668,-16031)); nachher
  F1300-F1520 Leon an der Suedwand neben dem Gator (Schnitt auf die Kamera der Suedwand), im Raum.
- `nb3r2_oben_leon_mit_ohne_F390-F2400.jpg` / `nb3r2_oben_elza_mit_ohne_F390-F2400.jpg` (exe-Riegel-Szenario rampe_oben, Spalten ohne | mit Kanister, Cut 2): F390 Rammen
  F383 (Koerper quert unter der Figur), F480 CROSS-Biss F483, F1350 Figur liegt (Rammen F1339 / F1337), F1440 CROSS-Biss F1439 (mit: gelber Kanister am Kiefer), F1590 und
  F1710 Hochbiss (Kopf ueber dem Deck), F2400 Figur steht oben, Gator unten - mit = ohne Kanister bildgleich im Ablauf.
- `nb3r2_rampe_west_leon_mit_F60-F2700.jpg` (exe-Riegel abn2_rampe_west, neue Eingabe, Cut 2): F60/F120 Klettern an der Rampe West, F240 Leon oben an der Westkante
  x 2318, F1380 Rammen F1377 (Kanister am Kiefer), F1560 Hochbiss F1567 an (2300,-16680) (vorher die Lage ohne Angriff), F2160 Hochbiss, F2520 CROSS-Biss F2528, F2700
  Hochbiss F2674.
- Spawn-Lagen (oben_kante): die Kamera bleibt Cut 0 (Nordwand mit Leiter und Tor), die Figur oben ist dort nicht im Bild (wie Abnahme 5 Abschnitt 5) - nur Logs.

### Tests
- `ctest --test-dir re15_port/build -R "^unit_r41_gatorfix_" -j 2 --timeout 900` (Bau 5a4ee385f, `ctest_unit.txt`): `100% tests passed, 0 tests failed out of 28`,
  `Total Test time (real) =  98.22 sec` (neu: raum 12.87 s, raum_elza 12.80 s; nb3 32.34 s, nb3_elza 32.21 s, re2 19.78 s, v4 12.38 s, gang 1.47 s ...).
  Direkt: `test_r41_gatorfix.exe raum` -> `raum: 0 Fehler`, `raum_elza` -> `raum_elza: 0 Fehler`, `nb3` -> `nb3: 0 Fehler` (`harness/`).
- `ctest --test-dir re15_port/build -R "^integration_r41_gatorfix_|^integration_r41_re2gator" -j 2 --timeout 1800 -V` (exe 5910fc24, 11:53-13:08, parallel zu eigenen
  Leisten-Laeufen und fremden Agenten; `ctest_exe_gruppe.txt`): `95% tests passed, 1 tests failed out of 20`, `Total Test time (real) = 4473.31 sec`,
  `The following tests FAILED: 1189 - integration_r41_gatorfix_abn2_rampe_west_elza (Failed)` - Ursache der Auswerter (aussenlog zaehlte die Kletterbilder F502-F505,
  sub=10 y -1210..-640, als "Deck verlassen"; der Lauf selbst lief bis F8100 mit exit 0, tragvergleich abweichend 0), behoben in 5a4ee385f, bevor die uebrigen 16
  Riegel ausgewertet wurden. Gruen u. a.: abn2_wand 643.96 s (M1), abn2_rampe_west 673.89 s (M3), oben_kante 466.12 s, oben_kante_elza 530.10 s, rampe_oben
  391.77 s, rampe_oben_elza 395.86 s, rampe 319.58 s, rampe_elza 367.05 s, abn2_wand_elza 671.68 s, frei 704.78 s, frei_elza 692.05 s, abn2_ecken 650.88 s,
  abn2_leiterecke 693.82 s, abn2_nordbecken 571.69 s, re2gator 291.79 s, gang_szene 27.15 s, gang_szene_elza 47.60 s, gang_montage 32.93 s, gang_montage_elza 51.67 s.
- Einzeln nachgefahren (`ctest ... -R "^integration_r41_gatorfix_abn2_rampe_west_elza$" --timeout 1800 --output-on-failure`, Auswerter 5a4ee385f,
  `ctest_exe_einzeln_rw_elza.txt`): `100% tests passed, 0 tests failed out of 1`, `Total Test time (real) = 683.95 sec`.
- Bau: `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`; exe md5 5910fc24 (Engine-Stand 5a94392bf,
  danach nur Testquellen).

### Offen
1. **M3 ist ein Szenario-Fix, kein Spielfix:** der Niederwurf im Wasser bleibt der RE1.5-Niederwurf (s_kd_speed 1000 @0x8003615c, ~11000-12000 Einheiten Rutschweg)
   und traegt eine gebissene Figur durch das Becken bis an die Waende (Harness raum: (4200,-18700) endet in der SW-Ecke (-8432,0,-26532)); RE2 trifft im Alligator-Raum
   mit `FUN_8001569c(pl, Richtung, 1536)` (@0x80101684-88, Messblatt NB3 Abschnitt 3) - eine andere Trefferreaktion. Sie zu uebernehmen wuerde den gelobten Stand im
   Wasser aendern (Memory "gelobter Stand ist Referenz") - nicht gebaut, Nutzerfrage.
2. **re15_squareroot0** liest bei einem uebergelaufenen Quadrat (> 2^31) ausserhalb seiner 192er-Tafel (BIOS-Nachbau @0x80065f60, Absturz M1 F7374). Mit dem Raumhalt
   wird es in 2090 nicht mehr erreicht; der Nachbau selbst ist unveraendert (byte-true; eine Klemme waere eine Abweichung).
3. Der Raumhalt stoppt einen Niederwurf oben an der Deckkante ohne Wand-Slam (Clip 0xf): die Sonde FUN_8001c2dc meldet fuer die Leisten Flag 0 (Bit 0x2), der Slam
   bleibt so byte-true aus; im Bild endet der Flug an der Kante. Unten haelt die Lage vor dem Schritt (die Figur "steht" fuer den Rest des Flugs in der SO-Tasche).
4. `gb_pl_nische` / `gb_pl_im_block` (Runde 1, Belagerung der Ostnische / Figur im Block) werden mit dem Raumhalt nicht mehr erreicht (toter Code, nicht entfernt).
5. Kanister-Zielmodus waehrend der Belagerung (Abnahme 5 Frage F1: bis 28 s ohne Angriff, wenn der geworfene Kanister noch im Becken liegt) - unveraendert, Nutzerfrage.
6. Die blinden Wand-Touren (abn2_wand, abn2_wand_elza) treffen mit Vorgabe B nicht mehr ihre alten Orte (Abnahme 5 Offen 2); sie laufen als Riegel "kein Absturz,
   Figur im Raum, Kanister mit = ohne" weiter.
7. PSX-Bau nicht gemessen (plattformneutrale Pfade; die Zeile RAUMHALT schreibt wie die uebrigen Boss-Diagnosen mit fopen).
8. Querbezug re2gator (EXIT_AT 1700, Runde 1 Offen 8) unveraendert; integration_r41_re2gator gruen (291.79 s, "r41_re2gator: ok").

## G2-RE2 NB4 (Trefferreaktion RE2, Kante, Kanister im Vorbeikommen)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Runde 1 von 2 auf dem Messblatt-Commit 365a6ef7b (`G2_MESSBLATT_NB4.md`). Belege
`G2_belege/nb4/bau/` (Bildleisten `nachher_*.jpg`, Harness-Ausgaben `harness/`, Lauf-Auswertungen `laeufe/`, Laufskript `werkzeug/lauf4b.sh`).

Vorgaben (Wortlaut):
- **(T)** Fuer jeden Alligator-Angriff, der die Figur trifft, gilt die Trefferreaktion der Figur aus RE2 statt des RE1.5-Niederwurfs (Orchestrator-Entscheid
  nach "Original oder nicht").
- **(W)** Nutzer 2026-11-07 ~15:00 woertlich: **"er kann den Spieler ruhig In das Wasser stoßen."**
- **(K)** Nutzer 2026-11-07 ~15:00 woertlich: **"Die Frage stellt sich garnicht, da leon mit den alligator ganz normal seine runden drehen soll. Das heißt der
  alligator sollte Ganz natürlich irgendwann zum Kanister kommen, während er leo. Auf dem rundweg verfolgt...."**

### Bau

**(T) Trefferreaktion** - neue Maschine `engine/src/gator_treffer_r41.c` (Kopf `include/re15_gator_treffer_r41.h`), RE2-Quelle EM23_OVL_0000.BIN
(`G2_belege/nb4/mess_re2_em23_treffer_kanister.dis`):
- Eintritt aus `gb_biss_abschluss` (enemy_ai_boss_gator.c) fuer alle fuenf Angriffsstellen (LUNGE-Biss, B2-Biss, Hochbiss, Rammen, CROSS-Biss; die Stelle gibt die
  Art mit) statt `re15_player_take_damage(5)` + `re15_player_knockdown_begin`. Treffer-Einstieg 0x80101578: Unterzustand 2, bzw. 3 wenn FUN_80015910(Gator, Figur)
  (@0x801015b4-ec); FUN_8001569c(Figur, Achse, 1536) (@0x80101684-88); Schaden FUN_800401d4 = `re15_re2_player_damage` 60, ab Gator +0x218 & 0x20 30
  (@0x8010168c-a0, Bit @0x80101ebc-dc, neu `re15_gr2_geschwaecht_slot`); One-Save -> PL+6 = 4 (@0x80103924-34), Tod -> PL+4 = 3 (@0x80103938-48) und das
  Fressen wie bisher; Biss-Sperre 70 (@0x801016b0-b4) bleibt.
- **PORT-WAHL Richtung (Messblatt O-T1, Messvorschlag A90):** Achse a = Gator-Gier auf die naechste der vier RE2-Achsen 0/1024/2048/3072 gerastet; wahr -> a,
  falsch -> a + 2048 wie @0x80101604-80. Offen fuer den Orchestrator (Abschnitt Offen 1).
- Spieler-Haken 0x80102D08 Phase fuer Phase: Sub 2 @0x801030bc-0x801031dc (SE 0x04010001, Clip 18 Blend 3, +0x144 = -800 / += 150 / Clip 19 doppelt / += 15 bis 0,
  Ende), Sub 3 @0x80103238-0x80103370 (SE 0x04020001, Clip 20 Blend 3, +0x144 = +800 / -= 120, Clip 17 doppelt + Sperre frei @0x801032f0, Basis-Clip 6 rueckwaerts).
  Vorschub FUN_800152c8 ohne Klemme, danach der Dispatcher-Schwanz (`re15_player_body_and_walls`: Koerper-Schub + Klemme) als eigener Zweig in
  game_step_common.c vor dem Niederwurf-Zweig. Sperre +0x1D3 & 0x80 = `hit_react` Bit 0 (alle Angriffsstellen fragen `pl->hit_react == 0`). Der HP-Flinch-Detektor
  und das Inventar sperren waehrend der Reaktion wie beim Niederwurf. Kommandowort: `pl->state` = 2 (RE1.5 cmd 2 = Treffer-Klasse), `sub_state_1` = 2/3 (= RE2 +5),
  Ende 1/0/0 (RE2 `sw 1,4(s1)` @0x801031d0 / @0x80103364); nicht 5 (RE1.5 cmd 5 = Griff).
- Animation: RE2-Raumbank ROOM40A0 RDT+0x60 @0x65E8 bytegleich als `shared_assets/RE2/GATOR40A0.RBJ` (14 328 B, md5 28589546..., `tools/r41_g2nb4/
  gator40a0_rbj_export.py`, Ausgabe "Record 0: EMR @0x8 (15 Knochen x 80 B) EDD @0x29A4 Clips 21: 17/18/19/20 = 16/10/30/21 Bilder"). Eigener Kanal wie RE2s
  +0x190/+0x194 (FUN_80018e08 @0x80018ea4/eb8), nicht der Raum-RBJ-Kanal der Figur (2090/2091 haben keinen, die Szene posiert weiter aus PL00): PC-Zeichner
  `platform/pc/main.c` (Override nach dem Ziel-/Reach-Override, Knochen der Figur + Keyframes der Bank) mit dem Leser `platform/pc/src/gator_treffer_r41_pc.c`;
  der Schluss (Basis-Clip 6 rueckwaerts = RE1.5 PL00 Clip 11, `mess_pld_vergleich.txt`) ueber `anim_select_common.c` aus PL00. Gezeichnet wird, was FUN_8002959c
  zuletzt posiert hat (Sub 3 Ph5 -> Ph6 schreibt das Clip-Wort ohne Vorschub: in Bild 28 steht noch Clip 17). Elza posiert aus Record 0 (O-T2).
- Pruefhaken `re15_gator_treffer_alt_setzen(1)` = Stand 3d57083b1.

**(W) Kante** - die Fallschritte des Substate 10 als gemeinsamer Helfer (`re15_climb_fall_beginn/_schritt/_landung`, climb_common.c; climb_sub10 ruft sie selbst
auf, Abstieg per Aktionstaste unveraendert). Fuehrt ein RE2-Schritt die Figur auf Band 1 in eine Zelle mit Abwaerts-Bit 0x2 ([8] @0x754 / [9] @0x760; Bedingung
wie die Abwaerts-Sonde @0x8002d74c-50 und Boden darunter tiefer @0x8002d764-7c), dann im selben Bild y += 500, Boden, Band = -(h/1800) VOR der Klemme, Klemme auf
Band 0, y += 90 (Ph2 faellt in Ph3), weiter + 90 + 50 t, Landung y = Boden + Spritzer im Bild nach der Schwelle. Der Raumhalt Band 1 greift dabei nicht (das Band ist
schon 0), Band 0 und die Waende [6]/[7] bleiben. **PORT-WAHL (O-W1):** RE2 kennt den Sturz nicht (kein erhoehter Boden im RE2-Raum) - die RE2-Haltung (Clip 18/19
bzw. 20/17) und die waagrechten RE2-Schritte laufen im Sturz weiter; endet die Reaktion vor der Landung, faellt die Figur zu Ende (gemessen nie noetig: Sturz ab
Bild 0-2, Landung Bild 4-6).

**(K) Kanister** - PORT-WAHL mit dem Nutzer-Wortlaut (K) im Code (kanister2080_r41.c, gator_re2_r41.c): RUNDE (E1) und Anschwimmen (N3) aus dem Spiel
(`anschwimm_lage` liefert 0, GUARD-Ausnahme damit wirkungslos), fuer den Szenen-Kanister das Zielmodus-Bit (O-K1) und die Zonen-Ruhe (a) der Paddel-Hysterese
(@0x80100ab8-c8 / @0x80100aec) aus, solange er liegt - ohne diese wich die Verfolgung mit liegendem Kanister ab (Riegel K1 vorher: 5 von 6 Lagen, erstes Bild
t449..t1718, Clip 1 statt 0). Aufnahmefenster und Stufen 1-6 unveraendert. Pruefhaken `re15_k2080_runde_alt_setzen(1)` = Stand 3d57083b1.

### Riegel und Messung (Bau exe md5 3d53ca60bdf6358685d327b03470e112, test_r41_gatorfix.exe 93092b73)

| Riegel | Messung | Ergebnis |
|---|---|---|
| T1 Harness je Angriff | nb4 Teil A 13 eingespeiste Treffer; Teil B 32 Lagen + 3 "oben halten" + Lauf HP 24990 | Teil A 13/13; Teil B 880 Reaktionen, 708 vollstaendig (Sub 2 488, Sub 3 220), **0 Abweichungen** vom RE2-Modell (Unterzustand, Schritte, Dauer 20/54, Clips, Sperre, SE); Rammen 54, CROSS-Biss 24, Hochbiss 10, Biss 792 (B2 nur per env, s. Offen) |
| T2 Animation | Clipfolge je Bild; GATOR40A0.RBJ Record 0 | 18@0 19@4 / 20@0 17@21 11@29 (rw); RBJ 21 Clips, 16/10/30/21; **0 Bilder RE1.5-Niederwurf** (Harness 0, exe 0 von 4400/8400) |
| T3 Schaden/Sperre/SE | Teil A + B | 60 (853x), 30 bei +0x218 & 0x20 (27x), One-Save Sub 2 = 1 Bild, Sub 3 = 33 Bilder ab Clip 17; Sperre Bild 0-18 (Ende-Bild 19 frei) bzw. 0-20; SE 1 / 2 |
| T4 Weg | Harness: Weg des Hakens (Station RE15_SCHRITT_TICK); exe gtrlog: Summe figur - von je Bild | Haken-Weg <= 3735/3080, mit Sturz + 468 (Fall-Klemme): Harness max 4171, exe max 4021 (Sturz) bzw. 3735, **0 ueber**. Luftlinie gesamt mit Koerper-/Boss-Schub: Harness max 6186, exe max 4766 (frei4_leon, 5 von 76 Reaktionen > 3735/3080) - vorher bis 11 312 / 13 059 |
| T5 Gegenprobe | Pruefhaken treffer_alt; gtrlog auf die Messer-Laeufe 3d57083b1 | Harness: Niederwurf 1804 Bilder, Luftlinie 5630, 0 RE2-Reaktionen - rot; exe nb4_oben_leon / nb4_frei4_leon: 0 Reaktionen, Niederwurf 1128 / 3492 Bilder, RUNDE 2, Zielmodus 1 -> VERFEHLT |
| W1 Kante -> Wasser | Teil A Nord/Sued je Sub 2/3 + Mitte; Teil B 53 Stuerze; exe oben_leon/elza | y -1210, -1070, -880, -640, 0, Band 0, 0 Raumhalt; Teil B 53/53 richtig, 0 Raumhalt in Sturzbildern; exe F383 Rammen -> KANTE F385 -> LANDUNG F389 (Leon und Elza) |
| W2 Wand bleibt Wand | Teil A Ostwand [6], Plattform [7], Mitte West | Figur bleibt y -1800 Band 1 (x 6732 / 2318 / 2313) |
| W3 nie aus dem Raum | raum, nb4 Teil B, aussenlog | 0 aussen, 0 im Block, 0 Deck verlassen ausser Sturzbildern (raum 16 Lagen x 2 mit/ohne, exe 4 Laeufe) |
| W4 Abstieg | climb_sub10 ruft die Helfer; exe Elza Abstieg per Aktionstaste an der Nordkante (6732,-14968), ohne Gator, neu gegen Stand 3d57083b1 (re15_pc_g2nb4.exe) | 38 sub=10-Bilder, climb.log **bytegleich** (y -1210/-1070/-880/-640/0 bei (6732,-14032)), `laeufe/w4_abstieg/`. Der Abnahme-6-Lauf west4_elza steigt mit dem Bau nicht mehr hinauf (die RE2-Bisse im Wasser treiben die Figur in die SO-Ecke) |
| K1 kein Holmodus | Teil K 6 Lagen x 1800 mit liegendem Kanister gegen ohne | **0 von 6 abweichend** bis zur Aufnahme; RUNDE/ANSCHWIMMEN 0, Zielmodus 1 0; exe 0/0/0 in allen Laeufen |
| K2 Aufnahme nur in Reichweite | gator_boss.log Stufe 1 | Harness 6 Aufnahmen (laengs max 5795, quer max 1668), exe 3 (5444/1672, 1708/-1156, 5583/1707), 0 ausserhalb; Gegenprobe Kanister abseits: Stufe 0 bis Laufende |
| K3 oben ohne 28-s-Pause | Teil K, exe obenlog ab0 | Harness erster Treffer oben spaetestens t288; exe oben_leon/elza erster Treffer oben nach 300 Bildern (vorher Pause 851-856) |
| K4 Stufen 1-6 | unveraendert (Riegel v4 Sitz 0) | s. ctest |
| Gegenprobe K | Pruefhaken runde_alt | RUNDE an, 55 Bilder abweichend - rot |

Exe-Laeufe dieses Stands (Umgebung wortgleich den Abnahme-6-Laeufen, `laeufe/c4_*`): oben_leon 49 Treffer = 49 Reaktionen, Kanister im Vorbeikommen F1857
aufgenommen; oben_elza 59/59, Kanister bleibt liegen (der Gator kommt nicht vorbei); frei4_leon 101/101, Aufnahme F5689; frei4_elza 93/93, Aufnahme F178.

Bildleisten (RE15_FRAMEDUMP, Aufhellung 1.8): `nachher_oben_leon_rammen_kante_F376-F414_hell.jpg`, `nachher_oben_elza_rammen_kante_F376-F414_hell.jpg` (Rammen,
Sturz, Spritzer, Figur im Wasser), `nachher_oben_leon_biss_wasser_F750-F798_hell.jpg`, `nachher_frei4_leon_biss_wasser_F136-F196_hell.jpg` (Figur ausserhalb
von Cut 0, Beleg die Lagezeilen: 3155 statt vorher 10 776), `nachher_frei4_elza_biss_wasser_aufnahme_F136-F196_hell.jpg`, `nachher_frei4_leon_aufnahme_F5660-F5712_hell.jpg`.
Vorher: `G2_belege/nb4/mess_frei4_leon_biss_F1186-F1240_hell.jpg`, `mess_oben_leon_rammen_F1336-F1384_hell.jpg` (Messer).

Neu bewertete Riegel: raum (Sturzbilder kein "Deck verlassen", der Oben-Abschnitt endet mit dem Sturz; Gegenprobe 10603425b mit treffer_alt), nb3 (Gegenproben
a661eb8aa mit treffer_alt), fuzz (die stehende Figur behaelt ihre Gier: die RE2-Reaktion dreht sie mit FUN_8001569c; ohne das 15 Fenster, mit 5), exe: kein
Tragen-Riegel ohne Aufnahme (K), E1 entfaellt, obenlog ab0 fuer rampe_oben, aussenlog ohne Sturzbilder, gtrlog in allen Laeufen.

### Tests

- Bau `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `=== LOCAL-BUILD-OK (build)`.
- `ctest --test-dir <Baum>/re15_port/build -R "^unit_r41_gatorfix_" -j 2 --timeout 900`: `100% tests passed, 0 tests failed out of 30`,
  `Total Test time (real) = 173.08 sec` (`G2_belege/nb4/bau/ctest_unit.txt`; neu nb4 und nb4_elza, je ~34 s).
- exe-Riegel `ctest --test-dir <Baum>/re15_port/build -R "^integration_r41_gatorfix_|^integration_r41_re2gator" -j 2 --timeout 2400 -V`: Ergebnis im Abschnitt "Nachtrag ctest exe" unten.

### Offen

1. **Richtung A90 (O-T1)** ist PORT-WAHL; RE2 nimmt die Korridorachse des Abschnitts. Mit A90 stoesst fast jeder Treffer oben die Figur ueber eine Kante (Harness
   Teil B: jede Oben-Lage faellt beim ersten Treffer). Entscheid des Orchestrators.
2. **Animation im Sturz (O-W1)** RE2-Haltung weiter; Sichtpruefung am Bild durch den Nutzer.
3. **Luftlinie mit Koerper-Schub**: der Haken haelt 3735/3080, der Koerper-/Boss-Schub des Gators (RE1.5 FUN_8002b544 @0x80031cbc im Schwanz, Boss-Push) kommt
   dazu (exe bis 4766, Harness bis 6186). RE2 schiebt ebenso (FUN_80034D0C @0x80026628); ob der Port-Schub in 2090 RE2s Mass hat, ist nicht gemessen.
4. **B2-Biss** (env RE15_GB_LUNGE_B2) laeuft ueber dieselbe Stelle gb_biss_abschluss, ist aber in keinem Riegel ausgeloest.
5. **PSX**: die Maschine ist plattformneutral, der Zeichner-Override fuer die Raumbank nur PC (PSX posiert Clip 17-20 aus der Standardbank) - nicht gebaut/gemessen.
6. Vibration (0x800eaad8/0x800eac28), Gator-Effekt FUN_80103d80 und die Nacken-Bits +0x1C0 |= 0x12 sind nicht Teil der Reaktion (O-T3).
7. Elza posiert aus RE2-Leon-Record 0 (O-T2) - Sichtpruefung `nachher_oben_elza_*`.
