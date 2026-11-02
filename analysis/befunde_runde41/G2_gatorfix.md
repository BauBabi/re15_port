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
