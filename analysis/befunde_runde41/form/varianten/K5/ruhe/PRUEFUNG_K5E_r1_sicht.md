# Pruefung K5e ruhige Haut, Runde 1 - Sicht-Pruefer (2026-11-08)

Stand: Worktree r41_form, Zweig r41/form, HEAD ed31d11d8 (Bauer-Commits f817fb41a, a95cf39e5, ed31d11d8 nach a4108c761).
Variante: C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/K5e/BAHNSTEIG_FORM.EMD/.TIM.
Gesehen: Tafeln 01-07 (Read), riegel_k5e.txt (25/25 GRUEN), riegel_gegenprobe.txt (K5c 8/24, K5d 9/24; R1-R4 an beiden ROT).
Eigene Kacheln (roh, kein Gamma, Nearest) unter
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5e/pruef/:
- kachel_eigen.png: 4 eigene gdigrab-Laeufe (form_spiel.sh, eigene exe-Kopie, RE15_RE2_ASSET_ROOT, Verfahren "gdigrab (Versuch 1)", rc 0 je Lauf):
  K5e 5090 Schnitt 11 Gier 2850 Clip 2 | K5c (Asset heute) gleiche Lage | K5e 5090 Gier 802 | K5e 5080 Schnitt 1 Gier 1780.
- zoom06.png (Tafel 06 K5e x3), buckel_k5e.png, kralle_k5e.png, pfote_weiss.png; Messskript lab.py.

## (1) Ruhe / Sauberkeit - wie gewuenscht
- Weisses Licht (Tafel 06/07): K5e hat keine aufgemalten Flecken, keine farbfremden Gruppen mehr. Die Texelstruktur (dunklere
  G4-Adern) ist tonal, gleicher Farbton wie die Haut. Eigene Lab-Messung Tafel 06 (3/4 vorn, Median): Rumpf-Haut h -34,5 C 9,2,
  Kopf-Haut h -34,7 C 10,9, Vorderpfote h -35,0 C 5,7, Schulter-Gelenk h -35,1 C 4,8 -> ein Farbton auf allen Hautteilen.
  K5c/K5d daneben: braune Vorderbeine, violetter Rumpf, blaugraue Inseln (K5d) - genau das "bunt" des Nutzers.
- Riegel bestaetigt: R2 farbfremde Flecken 0,0 (K5c 16,0, K5d 16,9), R3 Farbtonspanne 3,4 Grad (K5c 88,1, K5d 88,3),
  H1 TIM bytegleich K5c (0 Texel), H2 UV gleich 3828/3828 -> keine gemalte Insel.
- Spiel (04_spiel.png und eigene Kachel): K5e ist in 5080 und 5090 ein ruhiger dunkler Schiefer-Grauton; K5c daneben fast schwarz
  mit violetten/braunen Teilen.

## (2) Wuensche
- Gelenke hell/weisslich: wie gewuenscht (W1 dL min 5,53 / Mittel 6,63; W1b C* 5,8, +9,2). Sichtbar als helle Kugeln an Knie/Huefte
  (Tafel 03 Zeile 2). Eigene Messung Schulter-Gelenk L* 30,2 C 4,8 gegen Rumpf L* 19,4. Hinweis: das Schultergelenk r (mit Muffe) ist ein
  grosser grauer Block; unter weissem Licht wirkt er gegen das Mauve leicht gruenlich (Simultankontrast, gemessen neutral a* 3,9 b* -2,7,
  Adern a* 0,0 b* 0,0). Abweichung klein.
- Schwanzspitze fleischroetlich: wie gewuenscht (Tafel 06/07 sichtbar rosa; eigene Messung h -10,4 C 13,7 gegen Haut h -34,5; W2 85,4 %, +8,6).
- Rote fleischige Stelle vorn auf dem Buckel: Abweichung klein. Unter weissem Licht nur schwach roetlich (eigene Messung h -15,9 C 8,9 gegen
  Kopf-Haut C 10,9 - kaum bunter als die Haut). In Bild 2 ohne Filter (Tafel 03) hellste 20 % Lab [13,0 8,7 2,0], a*>8 nur 20 % der Pixel
  (Vorlage [19,1 32,1 3,9], 96 %); mit Filter a* 30,0 gegen Vorlage 32,2. Info-Zeile riegel_k5d (e) B2-F01 "nicht erfuellt" (L 31,7 gegen 41,4).
  Der Nutzer koennte "die rote Stelle fehlt immer noch" sagen; sie ist vorhanden, aber zurueckhaltend.
- Lippe und Unterkiefer aussen dunkel/grau ohne Leuchten: wie gewuenscht (Tafel 05: dunkle Lippenlinie, Unterkiefer hautfarben; W4 Rot 0 %).
  Rest: in der Seitenansicht (Tafel 06, Clip 2) ein feiner dunkelroter Strich an der Maullinie, 12 Pixel RGB (99,33,38) (K5c 72 Pixel
  (128,44,51)); vermutlich Maulinneres (G4-Zeile 0 bytegleich) im Spalt. Abweichung klein.
- Krallen vorn wenige, Handfarbe: wie gewuenscht (W5 dE 0,00, Rot 0 %; Tafel 03/06: keine roten Krallen, eingezogene Geometrie).
  Die G4-Krallentexel tragen helle Striche (pfote_weiss.png), blass-lila, keine gemalten Flecken.
- Helligkeit: Bild 2 erfuellt knapp (W6 15,48 >= 15,37; ohne Filter 8,65 >= 7,33). Bild 1 deutlich ueber der Vorlage (Video 43,31 gegen
  18,29, ohne Filter 16,53 gegen 7,62, also x2,4 / x2,2). Abweichung klein, Begruendung siehe (3).

## (3) Flach / plastisch / neu falsch - Abweichung klein
- Keine Mosaik-, Naht- oder Zeilenfehler gefunden; T2 alle 3828 Flaechen auf gueltiger Zeile, keine neue 0x0000. Keine neuen schwarzen
  Flaechen (die dunkle flache Hand in Bild 2 ist im K5c-Bild genauso, Szenenlicht).
- Risiko 1 (groesstes): In der Bild-1-Kette mit Videofilter (Tafel 01 unten rechts, Tafel 03 Zeilen 1-2) wirkt K5e hell pastell-lila/rosa,
  waehrend die Vorlagen-Figur dunkel blau-violett ist. Das ist eine Folge der EINEN globalen Hebung (+21,4 L*, an Bild 2 gesetzt).
  Im echten Spielfenster 5080 (04_spiel.png Zeilen 1-2, eigene Kachel unten rechts) bleibt K5e dunkel schiefergrau - deshalb nicht dritte Klasse.
  Falls der Nutzer die Tafel 01 betrachtet, kann "zu lila / Pokemon" wiederkommen.
- Risiko 2: Unter weissem Licht (Tafel 07) ist K5e einheitlich blass mauve (Riegel-Info C*-Std 3,18 gegen K5c 8,92); neben G4 (schwarz-rot)
  und Hangar-Birkin (fleischrosa mit roten Adern) wirkt K5e kontrastarm, leicht "Knete". Im Spiel faellt das nicht auf.
- Szene F_fremd Bild 1 3,3 (Gelenke im 5080-Licht kuehl), ueber dem Original-Bereich 0-1,7; im Spielbild nicht als Fleck sichtbar.

## (4) Leuchtstelle am Becken F1 (nur melden)
- Unter weissem Licht (Tafel 06 Seite, zoom06.png) ein schwarzer Schlitz mit rosa Sprenkeln (70 Pixel RGB ca. 136,88,107). Im neuen ruhigen Umfeld
  ist sie die auffaelligste Einzelstelle und liest sich eher als Wunde/Oeffnung denn als Haut; im Spiel (eigene Kachel oben links, 5090 Gier 2850)
  sichtbar als roetliche Oeffnung an der Huefte wie bei K5c. Wirkt nicht wie ein neu aufgemalter Fleck, hebt sich aber staerker ab als in K5c.
  Bytegleich K5c-Zeile 5 (W7). Keine Aenderung, nur Meldung.

## Ergebnis
Keine Abweichung der dritten Klasse. Die Kernklage ("aufgemalt, unruhig, frei verteilt, bunt") ist behoben. Offene kleine Punkte:
Buckel-Rot zurueckhaltend, Bild-1-Kette zu hell/lila, blasses Mauve unter weissem Licht, feiner roter Maulstrich, F1 tritt hervor.
bestanden: ja.
