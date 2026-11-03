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
