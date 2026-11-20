# SF V8 Runde 7 - Sicht-Pruefung (Auge des Nutzers, Ziel: widerlegen)

Stand geprueft: Zweig r41/sichel_v8, HEAD d5e3cc06e, Spiel-Asset re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
sha256 5c902d493e6815f8551e0297eb1ef7e8c6cbda4b11ed3ab5191cf6a62ac11f30 (= Pin release/em30_hangar.sha256).
Vergleich: V7 (Framedumps sfv4/r/V7_szene/fd), Stand 6d (zuletzt vom Nutzer gesehen, Framedumps sfv8/r6d/szene/fd),
NEU (Framedumps sfv8/r7d/szene/fd und eigener gdigrab-Lauf).

## Eigener Lauf
- Skript C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr7s/gdi_pr7s.sh
  (Muster runde7d/gdi.sh: lauf_v4.sh, BAUM = Worktree, eigene exe-Kopie re15_sfv4_33983, Fenster ueber eigene PID, kein AUTOSHOT,
  kein SOFTWARE_RENDER, gdigrab 960x720 60 fps angefordert, 50 s).
- Ergebnis: Video .../pr7s/gdi/gdi_pr7s.mp4 (2181 Bilder, sha256 dbced6e5ff9110c22d4d170dd7387ee194f9eeafed0a9c87d022843144edc7af);
  debug.log "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)", "EM30 loaded: 23 meshes, 23 bones, 21 clips";
  state.log bytegleich sfv4/r/V7_szene/state.log (cmp ohne Unterschied).
- Zuordnung Fensterbild -> Spielbild gemessen mit runde7d/gdi_zuordnung.py (.../pr7s/zuordnung.txt, zuordnung2.txt): Treffer
  0,84-0,92 mittlere Abweichung gegen 1,2-10,5 beim naechsten anderen Bild. F368 und F313 fehlen in meiner Aufnahme (1,92 bzw. 4,91);
  fuer die Hand-Paare nehme ich F367 (eigen) und zusaetzlich das Fensterbild F368 des Bauers 7d.
- Eigene Kacheln (alle roh, nearest, gleicher Ausschnitt je Zeile), Ordner .../scratchpad/sfv8/pr7s/:
  k_hand_paare_0.png / _1 / _2 (Spielfenster F367/F368, F369, F371, F373, F375 neben birkin20, 20, 22, 23, 21, 24, gleiche
  Figurhoehe 600 px, dazu Hand nah im selben relativen Ausschnitt), k_nacken_griff.png (F262-F300), k_nacken_griff2.png
  (F306-F314), k_nacken_vorn.png (F346-F372), k_nacken_hinten.png (F318-F340) je V7 FD | 6d FD | NEU FD | NEU eigenes Fenster,
  Kopfmitte aus dem KO-Marker-Lauf; k_folge_a.png (F266-F285) und k_folge_b.png (F300-F314) jedes Bild V7 | NEU (Ersatz fuer
  "Echtzeit": jedes einzelne Bild der Griffphase); k_arm_0.png / k_arm_1.png (F278, F311, F314, F318, F320, F322, F324, F332
  ganze Figur V7 | 6d | NEU); t19_oben.png (Tafel 19 Zeilen 1-3 vergroessert).
- Angesehen: alle Tafeln 01-19 (19_nacken_p5, 19_armhaltung_r7a, 16, 17, 18 einzeln), Darstellung.png, Beschreibung1/2.png,
  birkin20-24, scratchpad/hand/drei_posen.png.

## Urteile

| Frage | Bewertung | Beleg |
|---|---|---|
| (1) Koerperbau | wie gewuenscht | Tafel 01/02/04: Schwanz breit aus dem oberen Ruecken, Bogen bis zum Boden, Spitze spitz; Kontur auf gruen/rot der Nutzerlinien |
| (1c) P5 Luecke Ruecken/Kopf | wie gewuenscht an der Nutzerstelle; Abweichung klein in der Griffphase | t19_oben.png PV/P2: Kontur vom Huegel glatt zum Kopf auf der weissen Linie, kein Hintergrund dazwischen (Tafel 15: PV 13/0/1,4, P2 20/1/2,0). k_folge_a/b: keine helle Zacke mit dunklem V in keinem Bild F266-F314. Links des Kopfes (F311/F312) Hintergrund zwischen Kopf und Schulter wie bei V7 (Kopf ragt ueber die Schultern) |
| (1d) Kragen weg | wie gewuenscht; Abweichung klein (Plattenwand) | k_nacken_vorn F346-F372: kein Rahmen, kein Bogen, keine Spitze ueber dem Kopf, kein blaeulicher Keil, kein oranger Fleck, Gesicht frei; der dunkelrote Schlitz links oben vom Kopf (6d) ist weg. Rechts neben dem Kopf in der Griffphase eine flache helle Platte mit senkrechten Streifen bis etwa Augenhoehe (F270-F310) - r4/6d-Erbe, liest sich als Platte, nicht als Huegel |
| (1b) P4 Schwanz-Unterseite | Abweichung klein | Tafel 14: Unterseite bis Spitze rot, Oberseite grau, keine grauen Reifen; feine Laengsstreifen gegen 6d leicht mehr Zeichnung, aber in F264/F276 weiter eine satt dunkelrote, glatte Klinge (Video f1372 heller rosa-rot). Tafel 14b F315-F324: roter Kasten mit harten Kanten, unveraendert gegen 6d |
| (2) Huegel, Linie zum Kopf | wie gewuenscht | Tafel 01/02/19: kleiner roter Huegel, kein Kamm, keine Kappe |
| (3) Rechte Seite / Brust von vorn | wie gewuenscht; Abweichung klein | Tafel 05: Original-Adern; Tafel 13/k_nacken_vorn: keine graue Platte, keine weissen Keile; oben links vom Kopf heller Glanz auf der Schulter (Riegel S14 offline 1804 Bildpunkte, Fenster S14s 22 = V7) |
| (4) Gesicht = V7 | wie gewuenscht | Riegel S5 Mesh 9 bytegleich (6501 Texel); in allen eigenen Kacheln gleiches Gesicht |
| (4b) P6 Hand breit | wie gewuenscht in der Form; Abweichung klein (Farbe = E9, Krallen-/Daumenform) | k_hand_paare: breiter Unterarm bis zum Handgelenk, breite Hand, haengt auf Oberschenkelhoehe wie birkin20-24; nicht mehr "klein, zierlich" wie drei_posen.png. Hand + unteres Unterarm-Drittel dunkelrot-violett statt grau (E9, Nutzerfrage). Krallen enden als 2-3 rote Spitzen statt rundem Faeustling; Daumen als duenne helle Nadel, in der Vorlage kurzer kraeftiger Haken |
| (4c) P7 richtig herum, Arm haengend | wie gewuenscht | k_hand_paare: Handruecken zur Kamera, Daumen zur Koerpermitte, Krallen nach unten, keine Kreuzung; Ellbogen/Handgelenk/Spitze auf Hoehe der Vorlage. "Verkehrt herum" sagt hier niemand mehr |
| (4d) Schadet der Arm der Gestalt | Abweichung klein | k_arm: F315-F324 steht der Unterarm seitlich ab (wie 6d, V7 hing); keine Hand im Oberschenkel, kein Arm durch den Rumpf in den angesehenen Bildern; Stand/Gang von vorn wie in Tafel 18 Zeile 3 ruhig |
| (5) Szene in Bewegung | Abweichung klein | Jedes Bild F266-F314 angesehen (k_folge): kein Flackern, keine Zacke in Folge, kein Keil, keine Naht; Kasten F315-F324 und abstehender Unterarm wie 6d |
| (6) Fortschritt V7 / nicht schlechter als r4 | wie gewuenscht | gegen V7 klar besser (Schwanz, Huegel, Adern, Hand); gegen r4: Luecke zu, Hand richtig, Unterseite rot |
| (7) Fertige V8 annehmbar, nichts schlechter als 6d | wie gewuenscht | gegen 6d nur Verbesserungen sichtbar: dunkelroter Schlitz neben dem Kopf weg, Zacke F310 weg, Krallen keine Nadel; nichts sichtbar schlechter |

Keine Bewertung "Abweichung, die der Nutzer ablehnen wuerde".

## Einzelheiten

### (1c)/(1d) Ruecken - Kopf
- Nutzerstelle (Darstellung.png, weisse Linie "smooth straight to head"): in t19_oben.png laeuft NEU PV vom hellblau markierten
  Huegel ohne Hintergrund-Spalt zum Kopf; P2 ebenso. V8 r4 hatte dort den Schlitz, V7 die dunkle Kerbe.
- Griffphase jedes Bild (k_folge_a F266-F285, k_folge_b F300-F314): rechts neben dem Kopf steht die helle Rueckenplatte hoeher als
  die flache V7-Schulter, mit gestreckten senkrechten hellen Streifen. Keine Spitze, kein dunkles V zwischen Platte und Kopf in
  keinem Bild. In 6d (k_nacken_griff2, F310) stand dort noch eine kantige Spitze; NEU ist flacher.
- Links vom Kopf F311/F312 Hintergrund zwischen Kopf und Schulter: V7 hat ihn genauso (k_folge_b F311/F312 V7 | NEU). Klasse nach E9.
- Von vorn (F346-F372): kein Kragen, kein Rahmen, keine Haube, kein oranger Fleck. Ueber dem Kopf laeuft eine gerade Plattenkante
  schraeg nach rechts oben (auch in 6d); sie liest sich nicht als Spitze. Oben links vom Kopf ein heller Glanzfleck auf der Schulter.
- F313/F314: nichts liegt ueber dem Gesicht (FD F313/F314 NEU wie 6d, Kopf frei).

### (1b) Schwanz
- Tafel 14 F264/F276: Unterseite jetzt mit feinen dunkleren Laengsstreifen statt glatter Baender von 6d - im Spielbild nur
  schwach sichtbar, Gesamteindruck bleibt "dunkelrote Klinge". Das Video zeigt dort helleres, rosa Fleisch; Farbe wird nach E4
  nicht geeicht, und der Nutzer hat 6d mit derselben Wirkung gelobt -> klein.
- Tafel 14b F315-F324: roter glatter Kasten mit harten Kanten auf dem Ruecken, bildgleich in der Wirkung zu 6d. Das Video zeigt dort
  an derselben Stelle einen rosa-roten Fleck mit weissem Glanz - kein Widerspruch zur Lage, nur zur Form (eckig).
- Von hinten (Tafel 08/12 F322-F340): Wurzelplatte mit gestreckten rot-weissen Streifen und sichtbarer Kante zum Ruecken - schon in
  r4 so (Tafel 12 linke Spalte).

### (4b)/(4c) Hand
- Paare (k_hand_paare_0..2): F367/F368 - birkin20, F369 - birkin22, F371 - birkin23, F373 - birkin21, F375 - birkin24.
  Handruecken zur Kamera, Daumen innen (Bildseite zum Koerper hin wie in der Vorlage), drei rote Krallen nach unten, faechern nicht
  und kreuzen nicht. Breite Hand an breitem Unterarm, keine Spindel; der Arm haengt mit dem Abstand zum Koerper wie in der Vorlage.
- Was noch anders ist: (a) Farbe: Hand und unteres Unterarm-Drittel dunkelrot-violett gesprenkelt; dadurch beginnt die "rote Klaue"
  optisch schon mitten am Unterarm und die Hand wirkt laenger als in der Vorlage (grauer Handruecken, nur Finger rot). Entscheid E9 -
  hier nur genannt. (b) Ende: zwei bis drei getrennte rote Spitzen statt eines runden roten Faeustlings mit einer hellen Kralle.
  (c) Daumen: duenne helle Nadel nach unten-innen, in der Vorlage ein kurzer, kraeftiger rot-grauer Haken.
  Wuerde der Nutzer "irgendwas passt nicht" sagen? Am ehesten wegen (a) - das ist die offene Nutzerfrage E9.
- Vergleich drei_posen.png (Stand vor P6): dort winzige Hand mit duennen, sich kreuzenden Krallen - das ist behoben.

### (4d)/(5) Arm in Bewegung
- k_arm_0/1: F278 Arm haengt seitlich frei; F311/F314 Hand neben der Huefte, nicht im Bein; F318-F324 Unterarm steht seitlich wie ein
  Fluegel ab (V7 hing nach unten) - gleich wie 6d, im Ablauf nur rund 1/3 s. F332 von hinten: Arm unauffaellig.

## Rest (keine dritte Klasse)
1. Schwanz von der Seite satt dunkelrote, glatte Klinge (Tafel 02, 14 F264/F276) - wie 6d.
2. Roter eckiger Kasten der Schwanzwurzel F315-F324 (Tafel 14b) - wie 6d.
3. Helle Plattenwand mit gestreckten Streifen rechts neben dem Kopf F264-F310 (k_folge_a/b) - r4/6d-Erbe, keine Spitze.
4. Hand-Farbe dunkelrot-violett (E9, Nutzerfrage); Krallenende spitz statt rund, Daumen duenn.
5. Unterarm F315-F324 abstehend (wie 6d).
6. Heller Glanzfleck oben links vom Kopf von vorn (F346-F372).

Bestanden (keine Bewertung der dritten Klasse).
