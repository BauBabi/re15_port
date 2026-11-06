# K5c Farbe Runde 2: Sicht-Pruefung (Pruefer "Auge des Nutzers", 2026-11-06)

Gegenstand: Tafeln 01-07 (`varianten/K5/farbe/`) und die fuenf Spielbilder `farbe/spiel/spiel_*.png` (K5c neu) gegen `farbe/spiel/heute_*.png` (K4)
und `farbe/spiel_r1/` (Runde 1), Dossier `FARBE.md` (Riegel 24/29, ROT: K1, K2, N1, B1, B2). Massstab: Nutzer 2026-11-06 ("passt von den Farben
und von der Struktur nicht ... nicht zum Rest des Spiels ... wie ein Pokemon"), frueher E8/E10/E12 (Haut mehrfarbig, Wunden fleischig, PSX-Stil,
aus einem Guss, geordnet, keine aufgestempelten Marken). Verglichen mit EM30_HANGAR, RE2-G5, RE2-G4 auf denselben Tafeln und mit Bild 1/Bild 2.
Nichts am Code geaendert, nichts committet. Seiten (W) = Wesen, (B) = Betrachter. Lab = CIE-Lab D65 (OpenCV float), gemessen an den Tafel- bzw.
Spielbild-Pixeln. Ausschnitte (2x/3x/4x, nearest) und Messskript-Ausgaben:
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/r2sicht/`
(`03_seite_x2.png`, `03_34vorn_x2.png`, `04_k5cneu_koerper_x2.png`, `04_em30_x2.png`, `04_k5cneu_kopf_kiefer_x2.png`, `05_*_x3.png`,
`06_kopf_r{0,1,2}_x3.png`, `sp_*_x3.png`, `sp_5090_kopf_x4.png`).

## 0. Ergebnis

**Nicht bestanden.** Zwei Punkte bleiben in der Klasse "Abweichung, die der Nutzer wieder ablehnen wuerde":

1. **P1 Rot als Netz und Saum, nicht als Flaeche.** Runde 2 hat die Adern dunkler gemacht und die Weissflecken, das Blau und das Creme-Maul
   entfernt, aber die Rot-Form ist dieselbe geblieben: im Atlas liegt das Zellnetz weiterhin in allen 46 Koerperfeldern und im Kopffeld, im Render
   erscheint das Rot als Risslinien und als rote Saeume entlang der Umrisskanten (Schwanzunterseite, Bauchlinie, Innenseiten der Beine), im
   Spiel 5080 als rote Kanten an den Polygonraendern der Glieder. Der Bauer misst es selbst: B1 28,0 % (EM30 32,1 %), B2 Seite 2,45 % (Soll 3,2 %).
   Schwarzes Tier mit roten Rissen und roten Kanten = der "Pokemon"-Eindruck; G4 daneben ist schwarzbraun mit grossen Fleischflaechen, EM30 helles
   Fleisch mit wenigen dunklen Adern.
2. **P2 Machart: gekachelte Foto-Fragmente statt gemalter Formen.** Die Koerperfelder sind kleine, gleich aussehende Ausschnitte der G4-Haut mit
   Zellnetz, in Reihe 2-5 des Atlas mit sichtbarer Wiederholung (gleiche rote Senkrechtstreifen am linken Feldrand); nirgends eine gemalte
   Anatomie (Muskel, Knochen, Wundrand) wie bei EM30/G4. Der Kopf ist dieselbe Rissstruktur auf grau-blaugruenem Grund ("Lavastein") statt eines
   gemalten Schaedels wie G4. Das ist genau E8 ("nicht aus einem Guss, hoeher aufgeloest") - die Riegel A6/A7 messen Frequenz, nicht Lesbarkeit.

Behoben gegenueber Runde 1 und K5c heute (alle Klasse "wie Spielfamilie"): Blau-Klasse 0,0 % in allen Kacheln, Neutralweiss 0,00 %, kein
Creme-Kiefer (dreiteilig wie G4), keine Magenta-Augen, kein Gamma. Die Tafeln sind fair (Abschnitt 3), mit zwei Einschraenkungen wie in Runde 1.

## 1. Befunde

Klassen: **F** = wie Spielfamilie, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

| # | Punkt | Soll (Spielfamilie / Original / Nutzer) | Ist K5c neu Runde 2 (gemessen / gesehen) | Klasse |
|---|---|---|---|---|
| P1 | Rot-Struktur: Flaeche gegen Netz/Saum | G4: 24 % Rot in grossen Fleischstufen (Bauch, Brust, Auge, Kiefer), Adern nur Akzent; EM30: dunkle Adern auf hellem Fleisch, Wunde Schulter als Flaeche; Nutzer E10 "Wunden fleischig", E12 "geordnet, mit Richtung"; Bauer-Riegel B1 >= 32,1 %, B2 >= 3,2 % | B1 **28,0 % ROT**, B2 Seite **2,45 % ROT** (`farbe/riegel_K5c_farbe.txt`). Atlas (`04_k5cneu_koerper_x2.png`): Zellnetz in allen Koerperfeldern, Maschenweite 5-8 Texel, nur dunkler als r1. Render 03 Seite (`03_seite_x2.png`): Rot = Saum Schwanzunterseite, Bauchlinie, Innenseite Hinterbein, Lippe; eine Flaeche Schulter. 05 Ruhe 3/4 vorn: dito, Risslinien ueber Ruecken/Flanke. Spiel 5080 Ruhe (`sp_spiel_ruhe_r5080_s1_x3.png`): rote Linien entlang der Polygonkanten von Vorderbein, Hinterbein, Schwanz (wie eine rote Umrisszeichnung); Rot-Klasse im Figurfenster 0,8 % (K4 0,0 %), d. h. im Spiel 5080 bleibt vom Rot nur die Kante | **A** |
| P2 | Machart Atlas / Lesbarkeit | EM30 (`04_em30_x2.png`): je Feld eine gemalte Form (Brust mit Adern, Schulterwunde, Hose), grosse ruhige Flaechen, wenige Farben je Teil; G4-Kopf (06): gemalte Knochenplatten, Streifen, Fleischflaechen. Nutzer E8 "aus einem Guss, PSX-Stil, nicht hoeher aufgeloest", E12 "keine aufgestempelten Marken" | K5c neu Atlas: 46 kleine Felder mit gleichem Foto-Korn (G4-Haut ohne Adern + Zellnetz + rote Streifen am Feldrand), Wiederholung derselben Senkrechtstreifen am linken Rand von mindestens 6 Feldern (Reihen 2-5), rosa-weisse Sprenkel (Reihe 2 links, Reihe 3 Mitte). Kein Feld zeigt eine gemalte Anatomie. Kopffeld: Rissnetz auf grau-blaugruen (06 Seite: Oberkopf teal-grau mit roten Rissen, gruen/teal-Anteil 1,2-1,6 % gegen G4 0,0 %). Im Render (03/05) ist die Figur deshalb ein dunkler Klumpen ohne lesbare Teile; G4/EM30 sind aus 2 m Abstand lesbar (Kopf, Brust, Klauen) | **A** |
| P3 | Blaugraue Haut (r1 P2) | Familie: Blau-Klasse (b* < -6, C* > 8) G4/G5 0,0 % | 03/05: 0,00 % in allen Kacheln (B3), Spiel 5080: 0,0 % (r1: 12,0 %). Behoben | F |
| P4 | Glanz / Neutralweiss (K5c heute) | Engine rein diffus; Familie L* > 85 0,00-0,21 % | 03/05/06: 0,00-0,06 % (06 3/4 vorn 0,47 % = Kieferband). Keine silbrigen Flecken mehr. Behoben | F |
| P5 | Kieferband hellrosa-weiss | G4-Maul: Zahnfleisch rosa, Zaehne beige, kein Weissband | 06 3/4 vorn / Seite (`06_kopf_r0_x3.png`, `06_kopf_r1_x3.png`): das Kinn ist die hellste Stelle des ganzen Modells (Palettenfeld Lab (73,0/24,2/17,6) 3,1 %), ein rosa-weisser Streifen an der Kieferkante, wirkt wie ein Lichtsaum. Quelle ist die G4-Maultextur, also Familienmaterial; im Spiel 5090 unsichtbar (dunkel) | K |
| P6 | Kiefer dreiteilig | Familie: dunkel / Fleischrot / getrennte Zaehne; Bild 2 Band Lab (50, 31, 16) | 06: Maulinneres dunkelrot, Zahnreihen grau getrennt, Band lachsrot; A8 6,1 / 95,7 / 39,2 %. Kein Creme mehr | F |
| P7 | Lesbarkeit im Spiel 5090 Cut 11 | Original Bild 2: dunkle Masse mit Feuerschein, Unterkieferband und helle Zahnflecken erkennbar; K4 heute zeigt den Kiefer | `sp_5090_kopf_x4.png`: K5c neu ist ein rotbrauner Klumpen, Kopf, Maul, Zaehne nicht auffindbar (Maul in Clip 1 offen, aber das Innere liegt im Schatten). Das Original ist dort ebenfalls dunkel; es fehlt aber jeder Anhalt, wo der Kopf ist | K |
| P8 | Helligkeit gesamt | 03: G4 L* 17,0-21,6, EM30 24,5-29,0; Bild 1 verlangt dunkel (K3) | 03: 14,5-16,7, 05: 12,2-19,9; Spiel 5080 Ruhe (Figurfenster diff > 6, 14 948 px): L* Median **6,1**, 71 % der Pixel L* < 10 (K4: 7,4 / 64 %); Nachstellung Bild 1: Median 3,8, 81 % < 10. Im Spiel ist die Figur eine schwarze Silhouette, in der von Mehrtoenigkeit nichts ankommt (C* Median 3,3). Bild 1 verlangt das Dunkle, deshalb nur K; es macht aber jede Textur-Verbesserung im Spiel 5080 unsichtbar | K |
| P9 | Pfoten | Familie: Klauen knochenbeige (G4), Haut der Fuesse wie der Koerper; E26: helle Stellen an den Fuessen sind Boden | 05 vorn / 3/4 vorn / oben (`05_ruhe_34vorn_seite_x3.png`, `05_oben_hinten_x3.png`): beide Vorderpfoten (W) tragen hellgrau-tealfarbene Kappen mit rosa Krallen, wie graue Socken; 03 3/4 vorn dito. Gruen/teal-Anteil gesamt nur 1,1 % (Familie 0,3-1,0 %), deshalb klein, aber als Fleck sichtbar | K |
| P10 | Rosa Fleck Ruecken Z1 | EM30 Wunde Lab (50, 26, 13), G4 (53, 27, 23); Bild 1 rosa Stelle | 05/03: Schulter/Ruecken als warme rote Flaeche (A4 26 Grad), kein Pastell mehr; in 01 Z.2 am richtigen Ort. O3 des Bauers (Video rosa-violett) bleibt Nutzerfrage | F |
| P11 | Augen | Original E24: dunkel, schwacher Lichtpunkt | K10 14,3 / 15,9; in keiner Kachel sichtbar; wie r1 | F |
| P12 | Mehrtoenigkeit / Farbkraft | Familie sigma_ab 7,4-10,7, C* 9,6-12,6 (G4) | A2 13,1, 03 C* 8,8-10,2 wie G4. Erfuellt, aber die Toene liegen im Netz und in den Saeumen (P1), die Grundhaut dazwischen ist ein Schwarz | F |
| P13 | Schwanz aus einem Stueck (E9) | keine Stufe | A11 1,4; 05 Seite / hinten keine Gliedgrenze | F |
| P14 | Kachelnaehte (O5) | je Feld eine Form | im Atlas deutlich (siehe P2), im Render schwach (Huefte, Flanke) | in P2 enthalten |

Zahlen aus meinem Messskript (Figurfenster = Pixel, die sich zwischen K5c-neu- und K4-Spielbild um > 24 unterscheiden, 3x3 geoeffnet):

| Spielbild | K4 L* med / C* / Rot / Blau | r1 L* med / C* / Rot / Blau | **r2 L* med / C* / Rot / Blau** |
|---|---|---|---|
| 5080 Ruhe | 12,7 / 4,3 / 0,0 % / 3,6 % | 7,7 / 7,9 / 2,7 % / 12,0 % | **8,8 / 5,8 / 2,5 % / 0,0 %** (Rot erosionsfest 3,9 %, groesste Rotflaeche 22 px) |
| 5080 Lage Bild 1 | 10,2 / 3,2 / 0,0 / 1,5 | 8,3 / 6,8 / 2,9 / 6,0 | **8,9 / 5,2 / 2,1 / 0,0** (erosionsfest 2,9 %, 31 px) |
| 5080 Nachstellung | 9,7 / 6,2 / 0,0 / 0,2 | 5,6 / 7,2 / 2,1 / 0,0 | **5,7 / 6,2 / 0,1 / 0,0** |

5090-Fenster sind durch das bewegte Feuer verunreinigt (Maske enthaelt Flammen) und werden nicht gewertet.

## 2. Warum P1 und P2 "Pokemon" bleiben

- Im neutralen Licht (03/05, 2x/3x) besteht die Figur aus: fast schwarzem Grund (43-51 % der Figurpixel L* < 12, G4 36-50 %, aber G4 hat daneben
  grosse helle Flaechen), roten Risslinien gleich dicht ueberall, roten Saeumen an den Umrissen, zwei grauen Pfotenkappen und einem hellrosa Kinn.
  Das ist weiterhin eine Zweifarb-Zeichnung (Rot auf Schwarz), nur leiser als in Runde 1. Der Nutzer sah K5c heute (grau + Weissflecken) als
  "Pokemon"; Runde 2 ersetzt das durch "schwarzes Tier mit gluehenden Rissen", was derselben Bildsprache angehoert.
- Die Rot-Form kommt aus dem Verfahren: Fleisch nach Mehrheitsschwelle der Screenshot-Rotpixel (E14) ergibt kleine Inseln, die "verdeckte
  Unterseite" (23,9 % der Hauttexel, G4-Annahme O4) liegt an den Feldraendern und wird im Render zum Saum an jeder Umrisskante. Das ist im Spiel
  5080 das Auffaelligste an der neuen Textur (rote Polygonkanten), obwohl die Riegel B1/B2 nur "zu wenig Flaeche" sagen.
- Die Machart (P2) ist nicht durch mehr Eichung zu loesen: Foto-Fragmente in 46 Feldern bleiben Foto-Fragmente. EM30 und G4 sind je Feld gemalt
  (eine Brust, ein Kopf, eine Klaue), mit wenigen grossen Formen, die man im Render als Koerperteil liest. K5c neu hat im Render keine Stelle, an
  der man "das ist der Bauch / die Brust / die Wunde" sagen kann - ausser dem Kiefer, der aus G4 stammt und deshalb als einziges Teil
  nach Spielfamilie aussieht.

## 3. Fairness der Tafeln

| Pruefung | Ergebnis |
|---|---|
| Gamma / Aufhellung | `generator/kf_tafeln.py`: kein pow, kein Gamma (grep `gamma|pow(|**0.`: nur Beschriftungen "kein Gamma"); jede Kachel traegt "roh" |
| Hintergrund / Licht 03/05/06 | gleiche Buehne, gleiches Licht 0,55 + 0,45 N.L fuer alle fuenf Modelle; L*/C*/L*>85/px in jeder Beschriftung |
| 01/02 | Disc-Licht x k jetzt in der Beschriftung (Befund r1 behoben), fuer K4 und K5c gleich. **Einschraenkung bleibt:** Zeile 1 ist deutlich heller als das echte Spielfenster (07): Figur L* Median 6,1 im Spiel 5080 gegen etwa 15 in der 01-Kachel. Wer nach 01 urteilt, sieht mehr Textur, als das Spiel zeigt |
| 03 Figurgroesse | K5c 15-17 k px gegen G4 35-39 k px, EM30 20-30 k px (Beschriftung sagt es): die K5c-Textur wirkt feiner, als sie ist |
| 07 Spiel | echtes Fenster, gdigrab, `spiel.log` 5/5 rc=0, K4 und K5c in derselben Szene/Pose/Kamera; fair. Die beiden 5080-Bilder unterscheiden sich nur in 6,7 k (diff > 24) bzw. 14,9 k (diff > 6) Pixeln - im Hangar-Licht ist der Texturunterschied zum Grossteil unsichtbar |
| Posen | 03: K5c Clip 2 gegen Familie Clip 0; fuer Farbe/Machart unerheblich |

Urteil: fair; die beiden Einschraenkungen aus Runde 1 (01-Helligkeit gegen Spiel, halbe Pixelzahl in 03) bestehen weiter und sind beschriftet.

## 4. Was Runde 3 aendern muesste (Sicht, keine Zahlenvorgabe)

1. Rot als wenige grosse, gemalte Flaechen an anatomischen Orten (Brust/Bauch, Schulterwunde Z1, Kiefer, Schwanzspitze), Adern nur in diesen
   Flaechen und dunkler als das Fleisch; Zellnetz aus den Koerperfeldern entfernen; die "verdeckte Unterseite" nicht an die Feldraender legen
   (keine Saeume an Umrisskanten) oder in der Grundfarbe lassen.
2. Grundhaut als gemalte grosse Formen mit 2-3 Toenen je Teil (EM30-Machart), nicht als G4-Foto-Korn; Felder ohne Wiederholung.
3. Kopf: Schaedelform mit wenigen Flaechen (Stirn, Schnauze, Augenhoehle) wie der G4-Kopf, kein Rissnetz.
4. Pfotenkappen (P9) und Kinnband (P5) pruefen; 01-Beschriftung um "heller als Spiel 07" ergaenzen.
