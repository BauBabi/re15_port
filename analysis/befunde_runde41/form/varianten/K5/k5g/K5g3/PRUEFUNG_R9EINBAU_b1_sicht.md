# Prüfung R9-Einbau (rechte Hand „Variante 1“) in K5g3, Runde b1, Sicht

Datum: 2026-11-15. Prüfer: unabhängig (Sicht), eigener Code, eigene Spielläufe. Nicht committet.
Stand: Zweig r41/form, HEAD 9b143bf41 (Einbau-Commits 184fa6393, 1ee2cdca4, 7e0a47430).
Arbeitsordner: S = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/b1s`
(Skripte emd.py, boden.py, glatt.py, stoss.py/stosslib.py, tief2.py, tief3.py, rend.py, seq.py, nah.py, lauf.sh, kampf.sh; Messbilder mit Kopfzeile).

## Ergebnis: BESTANDEN (kein roter Punkt)

Im Spiel und in Bild 2 sieht die rechte Hand so aus wie Variante 1 auf dem Nutzerblatt. Sie steht auf dem Boden und taucht nicht ein.
Die Bewegung läuft ohne Sprung, und neues sichtbares Durchstoßen habe ich nicht gefunden. Alles andere ist byte-gleich mit dem abgenommenen
K5g3 Runde 3. Was ich gefunden habe, sieht man nur im Vergleich oder an den Zahlen (siehe unten).

## Wie geprüft

- Eigener Bau: `RE15_BUILD_DIR=re15_port/build_b1s bash re15_port/tools/local_build.sh` (configure + build, LOCAL-BUILD-OK). Danach habe ich ihn gelöscht.
- Spielläufe mit eigener exe-Kopie und eigenen PIDs, ohne AUTOSHOT und ohne SOFTWARE_RENDER. Aufnahme mit gdigrab auf das Handle des eigenen Fensters.
  Sprung über RE15_DEBUG_JUMP, Form fest über RE15_BB_POSE, Kamera über RE15_FORCE_CUT.
- A/B im Spiel: Spiel-Asset gegen K5g3 Runde 3 (1981f97e aus 968001097, über RE15_RE2_ASSET_ROOT).
  Die Bilder unterscheiden sich, der Lauf lädt also ohne Umleitung wirklich das neue Asset.
- Eigener EMD-Leser (emd.py, nach re15_emd.h / re15_md1.h / skeleton_common.c, unabhängig vom Generator):
  - Boden über alle 771 Keys.
  - Glätte der Hand je Fenster.
  - Durchstoßen in beiden Richtungen: Strahl-Parität mit 3 Richtungen und Mehrheit, dazu der genaue Abstand zur Fläche.
  - Eigener Renderer für Bildfolgen.

## (a) Rechte Hand wie Variante 1, im Spiel und in Bild 2: JA

- Bytes:
  - Netz 18 des Spiel-Assets ist gleich Netz 18 von BAHNSTEIG_FORM_kr4r9.EMD (2f807af5, die Datei hinter „Variante 1“ auf dem Blatt).
  - Key 542 (Bild 2) und Key 412: Alle 32 Knochen haben die gleichen Winkel wie auf dem Blatt.
  - Gegen das Blatt anders sind nur die Netze 10/16/21/22 (Spalt B4 von K5g3, abgenommen) und die Textur (K5g3 statt K5e_f1).
- Im Spielfenster (S/vgl_spiel.png, Runde 3 | Spiel-Asset):
  - 5080 Schnitt 1 von vorn, Clip 12 Bild 0: Die Pfote sitzt schräg unter dem Kiefer, die Krallen zeigen nach unten. Das ist die Haltung aus Zeile 4 des Blatts (Variante 1).
  - 5090 Schnitt 11, Bild-2-Pose: gleiche Haltung. Die Hand liegt zum großen Teil hinter dem Raumfeuer, sichtbar sind die Krallenspitzen.
- Bild 2 (S/bild2_vgl.png): Blatt V1 (Zeilen 2/3) gegen Tafel 12 „K5g3 + R9“. Die Pfote kommt schräg unter dem Kiefer hervor, mit Spitzen nach unten links.
  Die Armzone oben ist leer, rechts steht etwas über. Das ist die gewählte Haltung und kein Fehler.
- Die Hand ist bemalt wie die linke Hand (Spiegel von Netz 13). Im Spiel gibt es keine Fremd-Texel und keine Löcher.

## (b) Boden, Bewegung, Durchstoßen: in Ordnung

Boden (eigene Vorwärtskinematik, alle 771 Keys; y > 0 heißt unter dem Boden):
- 0 Keys, in denen Hand, Unterarm oder Krallen (17/18/19/20/29/31) tiefer liegen als max(25, tiefster Punkt der ganzen Figur in Runde 3).
- Clip 9: tiefster Key ist 328 mit 17,3 (Runde 3: 13,3). Keys 332/333/334 liegen jetzt 43/43/49 ÜBER dem Boden.
- Gegenprobe mit meinem Werkzeug auf der alten R9-Abgabe 2f807af5: Key 332 357, 333 240, 328 152. Der Fehler aus R9 wird also gemessen und ist behoben.
- Bild 2 (Key 542): Der tiefste Punkt der Hand liegt 32 über dem Boden. Die Hand sitzt auf und taucht nicht ein.
- Die einzigen großen Werte sind Keys 591-597 (Clip 12 Bild 49-55, bis 2876). Dort sinkt die ganze Figur auch in Runde 3 ein (bis 5876), das kommt aus dem Basis-Clip.

Glätte (Schwerpunkt Hand + Krallen, Weg je Key und zweite Differenz):
- Clip 9, 10 und 27: wie Runde 3 (Unterschiede höchstens etwa 30 Einheiten).
  Der große Sprung zwischen Key 331 und 332 (rund 860) steckt schon in Runde 3 und in der Basis, R9 hat ihn nicht verursacht.
- Clip 12, Ausblendung 542-557: kein Sprung und keine Umkehr, eine glatte Glockenkurve. Die Hand hebt sich aber später und schneller:
  - Spitze 865 je Key gegen 694, zweite Differenz 391 gegen 266.
  - Nach dem Gipfel bei 551/552 gibt es eine kleine zweite Welle (Schritte 137 136 184 169).
  - Bildfolge (S/seq_clip12.png, S/nah_c12.png): Die Hand schwingt vor dem Gesicht hoch auf den Kopf, wie in Runde 3, nur einen Takt später.
- Im Spiel abgespielt (Clip 9/10/12 in 5080 Schnitt 1, S/s_c9.png, S/s_c10.png, S/s_c12.png): kein Zucken, kein Flackern, kein Teil fliegt weg.

Durchstoßen (eigene Strahl-Parität; Tiefe = genauer Abstand zur Fläche; Bezug Runde 3 und K5c-Basis dbd0bd88):
- Die frühere R9-Stelle Key 546 „Kiefer in Hand“: 70 (Runde 3 55). Gemeldet war vorher 173, das ist behoben.
- 547/548 Unterarm in Kiefer: 138/146. Runde 3 hat dort 140/140, das kommt aus der Basis.
- Clip 9, Krallen in der Brust:
  - 312/314: 0 (Runde 3: 64/109).
  - 313: 62 (Runde 3: 11, K5c: 263).
  - 316-318: 13/127/105 (Runde 3: 67/104/86, K5c: 329-399).
- 328 Hand in Kopf: 164 (Runde 3 139, K5c 180).
- 445-447 Kralle 31 in Kopf: 142/155/303 (Runde 3 159/128/289).
- Clip 10, 440: Kopf in Hand 77 (Runde 3 88).
- Clip 27, 704/729 Hand in Kopf: 0.
- In den Bildfolgen sehe ich kein Durchstoßen, das es in Runde 3 nicht ebenso gibt.

## (c) Sonst wie der abgenommene K5g3-Stand: JA (byte-belegt)

- Gegen K5g3 Runde 3 (1981f97e) ist nur Netz 18 anders. Das gilt für alle 32 Netze und alle Blöcke: Ecken, Normalen, Flächen, UV/CLUT.
- Ebenfalls gleich zu Runde 3: EMR-Knochenversätze, Elternliste, EDD und TIM (c00823b3, byte-gleich).
- Winkel anders nur bei den Knochen 17/18, in 91/92 Keys (309-334, 398-425, 440-448, 542-556, 701-707, 726-732).
  Die Krallen 19/20/29/31 sind nur in den 43 Bild-2-Keys (398-425, 542-556) anders.
- Nacken B4, Textur, Augen, Schulter (16) und Kopf (21/22/23) sind damit unverändert. Im Spielfenster stimmen Kopf, Augen und Textur mit Runde 3 überein (S/vgl_spiel.png).

## (d) Tafeln und kampf_5090.mp4: ehrlich, vorlegbar (mit Hinweisen)

- kampf_5090.mp4 zeigt das Spiel-Asset. Mein eigener Kampflauf mit gleichem Ablauf hat den gleichen Höchststand (Dreiecke 7436 bei F322).
  Er deckt sich Bild für Bild mit dem Video, die mittlere Abweichung liegt bei 1 fps-Abtastung bei 6 von 255.
- Hinweise für die Vorlage (kein Fehler am Modell):
  1. Im Video sieht man die rechte Hand nah nur etwa 3 s lang (30-33 s, Schnitt 11). Die ersten etwa 14 s sind Tür- und Raumwechsel, danach steht die Form weit weg.
     In Schnitt 11 deckt das Feuer die Hand oft zu. Das gilt auch für Tafel 12 (5090) und Tafel 04, Zeile Bild-2-Pose.
  2. Die Kopfzeile von Tafel 01/02 nennt die Spalte „K5e_f1 (bis R9 im Spiel) (Spiel-Asset)“. „(Spiel-Asset)“ ist veraltet, Spiel-Asset ist jetzt K5g3 + R9.
     Die Spalte „K5g3“ in 01-09 sagt nicht „+ R9“, gemeint ist aber das neue Asset.
  3. Auf Tafel 13 ist die Form etwa 130 px groß und weiß beleuchtet. Die Hand kann man darauf kaum beurteilen. Die Bewegung zeigen meine
     Bildfolgen besser (S/seq_clip12.png, S/nah_c12.png).
  4. Die Zeile „Kopf von vorn x4“ (Tafel 04) ist roh sehr dunkel. Augen erkennt man kaum (wie in Runde 3).

## Abweichungen, die der Nutzer ablehnen würde

- Keine. Die Haltung von Variante 1 (Pfote schräg unter dem Kiefer, Armzone oben leer, Überstand rechts) hat der Nutzer gewählt.

## Nur im Riegel/Vergleich sichtbar

1. Clip 27, Key 730: Kralle 19 steckt 81 tief im Kopf 22 (4 Ecken). In Runde 3 sind es 5, in K5c 0.
   Ab Key 731 stecken alle drei Stände 163-176 tief, der Einstich beginnt hier also einen Takt früher.
   Bei einem Bezug von max(K5c, Runde 3) + 25 = 30 wäre das nach meinem Maß ein N52-Fall. Der Bauer meldet R12 „0 Fälle“.
   Das sollte der Zahlen-Prüfer mit dem Maß des Riegels nachrechnen. Im Bild sieht man es nicht.
2. Clip 12, Keys 542-557: Die Hand hebt sich später und schneller (Spitze 865 gegen 694 je Key), mit einer kleinen zweiten Welle bei 551/552.
   Weich, aber flotter als in Runde 3.
3. Clip 9, Key 313: Kralle 19 steckt 62 in der Brust (Runde 3: 11). Das liegt weit unter K5c (263).
   Key 328: Hand 18 steckt 164 im Kopf (Runde 3 139, K5c 180).
4. Clip 9, Key 328: Der tiefste Punkt der Figur liegt 17 unter dem Boden (Runde 3: 13). Das liegt unter der Grenze von 25.
5. Tafel-Beschriftung 01/02 (siehe d.2).

## Aufräumen

Eigener Bauordner re15_port/build_b1s und eigene Laufordner gelöscht. Nur eigene PIDs beendet. Keine Commits.
