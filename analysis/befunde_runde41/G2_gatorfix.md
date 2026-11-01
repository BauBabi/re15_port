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
