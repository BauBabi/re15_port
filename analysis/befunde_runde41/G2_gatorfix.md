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
