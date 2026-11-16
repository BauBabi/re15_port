# Riesenform (Vierbeiner): Zähne bis vorn ins Maul - Runde 1, berichtigt in Runde 2

Stand 16.11.2026, Zweig r41/form (Worktree r41_form), Commit siehe unten. Kein Push.

## In Kürze

Die vierbeinige Birkin hat jetzt auch vorne im Maul Zähne, oben und unten. Insgesamt sind 16 Zähne dazugekommen:
- oben 11: je 5 links und rechts, dazu einer genau in der Mitte,
- unten 5: je 2 links und rechts, dazu einer in der Mitte.

Die Reihen laufen jetzt von den Seiten ohne Lücke um die Schnauzenspitze herum, wie beim Vorbild RE2-G4. Die neuen Zähne sind dasselbe Bauteil wie
die alten, sitzen am selben Kieferknochen und tragen dieselbe Zahnfarbe.

**Wichtig zur Größe (berichtigt in Runde 2, die erste Fassung stimmte hier nicht):** Die neuen Zähne sind deutlich kleiner als die alten
Seitenzähne, vor allem unten.
- Unten ragt das erste neue Paar nur 25 heraus, der alte Nachbar 144. Danach kommen 59 und in der Mitte 86. Die unteren werden zur
  Mitte hin also wieder **größer**, nicht kleiner.
- Oben sind es 65, 69, 34, 50, 63 und in der Mitte 47 (alte Nachbarn 69 und 115).

Der Grund: Beim Zubeißen liegen die Kiefer an den Seiten fast aufeinander (Abstand 12 bis 24). Ein längerer Zahn stäche dort in den
anderen Kiefer. Zur Spitze hin ist mehr Platz, deshalb dürfen die mittleren unteren länger sein. Ob die unteren Vorderzähne so kurz
bleiben, entscheidet der Nutzer (Abschnitt 8).

Alles andere ist geblieben, wie es abgenommen war: Textur, Haut, Augen, Hand, Bewegungen und alle alten Zähne sind Byte für Byte gleich.
Neu sind nur die angehängten Zahnflächen.

Bei geschlossenem Maul sticht nichts durch. Das habe ich in allen 771 Posen der Figur nachgemessen: Kein neuer Zahn steckt im anderen Kiefer
oder kommt auf der anderen Seite heraus. Kein unterer Zahn steht vor der Nase hoch. Kein Zahn dringt tiefer in einen Zahn der Gegenreihe ein,
als es die alten Zähne untereinander tun. Damit das klappt, sind einige vordere Zähne kürzer, als das Muster es vorgesehen hätte. Am
deutlichsten trifft das den ersten neuen unteren Zahn links und rechts: Er ist nur 39 statt 180 lang, weil er sonst beim Zubeißen oben
seitlich aus der Schnauze käme.

Bilder:
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/riesenform_zaehne/neu/1_maul_vorher_nachher.png`
  zeigt das Maul von vorn, schräg und von der Seite, jeweils zu und offen, links vorher und rechts nachher, bei gleichem Licht.
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/riesenform_zaehne/neu/2_im_spiel.png`
  zeigt das echte Spielfenster in 5080 (von vorn, Maul offen) und in 5090 (von vorn mit Maul zu sowie die Lage aus dem Video). Daneben
  steht jeweils der Kopf vergrößert, vorher und nachher.
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/riesenform_zaehne/neu/3_kampf_5090.mp4`
  ist ein Kampf in 5090, 40 Sekunden lang, aufgenommen aus dem echten Fenster.

## 1 Was gebaut wurde (nur im Generator)

Neue Stufe `zaehne_vorn` in `analysis/befunde_runde41/form/varianten/K5/generator/kf_k5g3_zaehne.py`. `kf_k5g3.py` ruft sie am Ende des Baus
auf, nach Textur, Spalt, Hand 18 und Augen. Kurz vor dem Abtasten der Texelzugriffe hängt sie die Zähne an Netz 22 und 23 an.
`python kf_k5g3.py bauen` erzeugt das Asset. `kf_k5g3.py pruef` liefert: zwei Läufe und Abgabe byte-gleich, JA.

Port-Gestaltung (die Riesenform gibt es auf keiner Disc), daher keine @0x-Adresse. Jede Zahl ist mit Regel oder Messung belegt:

| Regel | Inhalt | Zahl und Beleg |
|---|---|---|
| Bauteil | `k_form.zahn`, Ecken `round`, Normalen 4096 x Einheit, Windung wie `form_emd.orientiere` | `pruef_alt` baut alle 34 alten Zähne auf diesem Weg nach; Ecken und Normalen stimmen ganzzahlig mit dem Netz überein (sonst Abbruch) |
| Material | UV/CLUT/Seite je Dreieckslage vom vordersten alten Zahn derselben Seite (O11 bzw. U6) | Zahnzeile 2 `zahn0`, CLUT 30848, 3 UV-Muster (alle 34 alten Zähne haben genau diese) |
| Lage | Kopfvorgabe `K5/K5c/kopf.json` (sha256 e064f8e9 geprüft) | vordere Ecken von 22/23 == `k5_kopf.kopf/kiefer(K5c)` ±0,75 |
| R1 Linie | Lippenumriss der Stationen bis L bzw. Lj, dann Vorderkante quer, nach innen versetzt um den Abstand der alten Reihe an O1/U1 | oben 0,07 w = 18,6, unten 0,12 w_k = 29,0 (k5_kopf z = 0,93 w / 0,88 w_k). Der Fächer davor ist Nase bzw. Kinn; Messbild m1: dort saßen die Mittelzähne auf der Nase |
| R2 Abstand | Abstand der zwei vordersten alten Zähne in der Draufsicht | O1-O2 51,2 → 6 Abstände auf 311,8; U1-U2 103,3 → 3 Abstände auf 294,4; Mitte besetzt, links und rechts gleich |
| R3 Höhe | oben Unterseite Kopfrohr minus Wurzeltiefe O1, unten Oberseite Kieferrohr + 15 | Wurzeltiefe O1 gemessen 6,94; unten wie k5_kopf `yr + 15` |
| R4 Nenngröße | Muster der alten Reihe (oben groß bei i%3==0, unten lang bei i%2==0) mal f; f linear 1 → 0,708. **Nur die Nennlänge**: die gebaute Länge setzt R6 (Tabelle unten, Abschnitt 8) | 0,708 = Schnauzenhöhe an L 237,8 / an O1 335,9 |
| R5 Richtung | wie die alte Reihe, auf die Linie gedreht | oben 0,12 t + y + 0,22 n, unten 0,1 t - y + 0,18 n (k5_kopf.py:128/:166) |
| R6 Kappe | größte Länge ≤ Nennlänge ohne Verletzung in einem der 771 Keys (siehe 3) | `generator/k5g3_zaehne_vorn.json`, Bisektion 10 Schritte, RAND 2 = Ganzzahl-Rundung (Ecke ±0,5, Pose >> 12 bis 1) |

Neue Zähne (Seite +1; die Seite −1 ist gespiegelt und gleich lang):

| Zahn | Basis (x, y, z) | Nennlänge → Länge | Breite |
|---|---|---|---|
| oben 11 | 1568, 235, 227 | 71,2 → 71,2 | 30,4 |
| oben 12 | 1611, 235, 202 | 109,7 → 75,3 | 36,1 |
| oben 13 | 1629, 243, 153 | 63,9 → 39,0 | 27,3 |
| oben 14 | 1645, 229, 104 | 60,2 → 55,9 | 25,8 |
| oben 15 | 1645, 229, 52 | 92,0 → 69,2 | 30,3 |
| oben Mitte | 1645, 229, 0 | 53,0 → 53,0 | 22,7 |
| unten 6 | 815, −120, 172 | 180,5 → 39,3 | 41,5 |
| unten 7 | 880, −119, 98 | 128,9 → 75,1 | 37,0 |
| unten Mitte | 880, −91, 0 | 141,6 → 102,7 | 32,6 |

Kappen-Gründe bei Nennlänge, gezählte Verletzungen über alle Keys:
- unten 6: Austritt, steckt im Kopf, Zahn-in-Zahn (643/582).
- unten 7: steht vor der Nase (251/255).
- unten Mitte: steht vor der Nase (239).
- oben 12: Austritt, steckt im Unterkiefer (376/388).
- oben 13–15: stecken im Unterkiefer (175–215).

## 2 Beweis "nur angehängt" (Diff-Zählung gegen den abgenommenen Stand 88c49ce1)

Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD`: 88c49ce1 → **115f8721**, 586 588 → 588 764 Byte. TIM **c00823b3 unverändert**.
- Sektionen 0–6 (Morph, EDD/EMR aller drei Bänke) bytegleich, alle Netze außer 22/23 gleich.
- Netz 22: Ecken 2768 → 2812 (+44), Normalen 2635 → 2679 (+44), Dreiecke 5073 → 5106 (+33), Vierecke 0. Die ersten 2768/2635/5073 Einträge sind gleich.
- Netz 23: Ecken 138 → 158 (+20), Normalen 138 → 158 (+20), Dreiecke 56 → 71 (+15), Vierecke 70 → 70. Die alten Einträge sind gleich.
- Zusammen 16 Zähne = 48 Dreiecke, 64 Ecken, 64 Normalen. Jedes neue Dreieck liegt nur auf neuen Ecken und Normalen und trägt CLUT, Seite und
  UV eines alten Zahndreiecks (Riegel ZV2).
- Pins `re15_port/tests/unit/form_j_posen.h` (aus `stufe3/gen/s3_testkopf.py` neu erzeugt): MD1 462 572 / 0x565363c6 → 464 748 / 0x6f15e974.
  TIM-Pin und Höhe im Gehen sind unverändert.

## 3 Durchdringung (eigener Scanner, alle 771 Keys, Ganzzahl-Pose wie das Spiel)

`generator/kf_k5g3_zaehne_scan.py`. Gerechnet wird im Kopfraum, der Unterkiefer wird je Key wie im Spiel hineingedreht. Die Gegenfläche ist
das geschlossene Rohr des anderen Kiefers. Das Netz 22 selbst taugt dafür nicht, weil Augen, Augenhöcker, Kehlendeckel und der Spalt B4 innere
Flächen bilden.

| | alte obere | alte untere | neue obere | neue untere |
|---|---|---|---|---|
| tiefster Punkt im Gegenkiefer | 0,0 | 117,6 (gewollt im Kopf versteckt) | **0,0** | **0,0** |
| Austritt (durch den Gegenkiefer hindurch) | 0 Keys | **378 Keys** (Befund unten) | **0** | **0** |
| vor der Nase über der Mundlinie | – | 0 | – | **0** |
| Zahn in Zahn der Gegenreihe, tiefster Wert | 15,8 | 12,9 | 9,2 | 7,6 |
| Keys über den alten im selben Key | – | – | **0** | **0** |

Grenze: Ein neuer Zahn berührt den Gegenkiefer in keinem Key. Er darf nicht tiefer in einen Zahn der Gegenreihe eindringen, als die alten
Zähne es untereinander im selben Key tun. Für die alten oberen Zähne gilt das ohnehin: Sie berühren den Unterkiefer in keinem Key. Die alten
unteren dürfen tief im Kopf stecken. Für die neuen unteren habe ich das zuerst ebenfalls erlaubt (bis 29,7). Der Maler-Scan zeigte dann am
Zahn 23|−1|6 Spitzen, die durch die Schnauze gemalt wurden (nah schräg 32 Pixel neu gegen 6 alt). Deshalb gilt jetzt für beide Reihen: kein
Kontakt mit dem Gegenkiefer.

Maler-Prüfung im Port-Zeichenweg: Dreiecke werden nach mittlerer Tiefe ohne Tiefenpuffer gemalt. Geprüft habe ich 5 Nahansichten und 4
Spielansichten (5080, 5080 von vorn, 5090 Schnitt 11, Bild 2) über alle Keys. Gezählt sind die Pixel, an denen ein neuer Zahn über den
Gegenkiefer gemalt wird. Der Gegenkiefer müsste dort davor liegen, der Zahn sticht also sichtbar durch.

| Ansicht | nah vorn | nah schräg links | nah schräg rechts | nah Seite links | nah Seite rechts | 5080 | 5080 vorn | 5090 | Bild 2 |
|---|---|---|---|---|---|---|---|---|---|
| neu | 0 | 0 | 4 | 28 | 0 | 1 | 0 | 9 | 4 |
| alt | 60 299 | 1 972 | 6 | 6 256 | 3 936 | 6 | 2 539 | 1 037 | 1 426 |

Die Zahlen sind jeweils über 771 Keys summiert. In jeder Ansicht liegt "neu" unter "alt".

Boden- und N52-Regel bleiben grün. R12/R13 (Geometrie) sind gleich, ZV5 rechnet am Asset (siehe 4). Dreiecke im Spiel (G3-Zähler):
- Kampf 7457 (vorher 7436), s11 7770 (7748), v5080 7234 (7212).
- Verworfen 0, Grenze 16 384 (`geo/messung/spiel_dreiecke.json`, Asset-sha).

## 4 Riegel

- **Neu `generator/riegel_k5g3_zaehne.py` "Zähne bis vorn": 5 von 5 grün, 5 von 5 Gegenproben rot** (`K5g3/zaehne_vorn/riegel_zaehne.txt`).
  - ZV1 Zähne bis vorn: Die Mitte ist besetzt, die größte Lücke oben ist 70,2 (Grenze 1,5 × 51,2) und unten 118,3 (Grenze 1,5 × 103,3).
    **Gegenprobe 88c49ce1 ROT**: keine Mitte, Lücke 493,7 / 424,7.
  - ZV2 nur angehängt (siehe 2). Gegenprobe: eine alte Ecke +1 ergibt ROT.
  - ZV3 kein Durchstich (siehe 3). Gegenprobe ohne die R6-Kappen ergibt ROT: 284 Keys Austritt bzw. im Gegenkiefer, 250 vor der Nase, 135
    Zahn-in-Zahn.
  - ZV4 Maler (siehe 3). Gegenprobe ohne Kappen ergibt ROT, zum Beispiel nah Seite rechts 10 054 gegen 3 936.
  - ZV5 N52 beidseitig und Boden am Asset. Gegenprobe R9-Abgabe ergibt ROT.
    - Messfehler gefunden: Die Strahl-Parität von `kf_k5g3_n52` zählt die offenen Zahn-Pyramiden als Volumen. Schon am Abnahme-Stand ergibt
      Key 724 "Kralle 19 in Kopf 22" 419,6 tief, ohne Pyramiden sind es 2,5.
    - ZV5 rechnet "Arm in 22/23" deshalb ohne Pyramiden nach: 0 Fälle über der Grenze. Zusätzlich prüft es, ob ein Arm-Punkt in einem neuen
      Zahn liegt: 0 Fälle.
- **Geometrie `riegel_k5g3.py`: 14 von 15 grün, 19 von 19 Gegenproben rot.** Das ist dasselbe Ergebnis wie beim Abnahme-Stand: R8 ist wie
  bisher nur an den K5c-Krallen rot, R8b ist grün.
  - Die Geometrie-Stufe `geo/` ist unverändert (f833a905).
  - R9 nimmt die neue Spielmessung mit Asset-sha. Dafür liest `kf_k5g3_spielzahl.py` jetzt die sha des Spiel-Assets.
- **Textur `riegel_k5g3.py textur --neu_spiel`: 48 von 48 grün, 42 von 42 Gegenproben rot.**
  - Angepasst: TG und UV1 vergleichen das Asset mit "Geometrie-Stufe + Stufe zaehne_vorn".
  - G5 Asset == Variante (115f8721 / c00823b3).
  - Z1–Z9 neu im Spiel gemessen.
- **ctest-Gruppe Form** (`ctest -R "birkin|form|n2_griff"`, eigener Bau-Baum): **27 von 27 grün**. Dazu gehören:
  - unit_birkin_bahnsteig mit den neuen Pins,
  - unit_r41_form_emd, unit_birkin_szenen, unit_n2_griff_start,
  - integration_r41_form_dreiecke, integration_r41_birkin_pistole, integration_r41_form_wagenweg.

## 5 Befund nebenbei (nicht geändert, abgenommener Stand)

- Die **alten** unteren Zähne U2 (i 4, x 641, |z| 271) treten bei geschlossenem Kiefer in 378 Keys seitlich aus der Oberkiefer-Flanke.
  - Beispiel Key 73 (Clip 2 Bild 55): Spitze bei (1443, 100, −310), die Strahl-Parität des Kopfrohrs zeigt sie außerhalb der Flanke.
  - Im Maler-Scan sind das die großen "alt"-Zahlen.
  - Nach der Regel "Kritik an einem Teil nicht aufs Ganze ausweiten" habe ich das nicht angefasst.
- Wurzel-Pixel (Info, kein Durchstich): Der Maler zeigt an der Zahnwurzel ein paar Pixel Zahn über der eigenen Lippe.
  - Die Wurzeltiefe ist dieselbe wie bei O1 (6,9) bzw. unten 15 (wie bei den alten Zähnen).
  - Weil die neuen Zähne kürzer sind, ist der Anteil an ihren sichtbaren Pixeln größer: in den Spielansichten 0,07–0,12 neu gegen 0,03–0,04 alt.

## 6 Offen

1. Der erste neue untere Zahn ist links und rechts nur 39 lang (R6). Entscheidung des Nutzers, siehe Abschnitt 8.
2. Gezählt sind es 16 statt der genannten etwa 12–16, also am oberen Rand. Die Schätzung von 12–16 maß die Lücke nur in x (182 / 223). Der
   Bogen um die Spitze ist länger (311,8 / 294,4), und der Abstand der alten Zähne ist beibehalten.
3. Befund U2 (5): Er gehört nicht zu diesem Auftrag. Ob er behoben werden soll, entscheidet der Nutzer.

## 7 Dateien

- Generator: `generator/kf_k5g3_zaehne.py` (neu), `generator/k5g3_zaehne_vorn.json` (R6-Kappen mit Protokoll, neu),
  `generator/kf_k5g3.py` (Aufruf der Stufe, sha256 in bau.json).
- Prüfung: `generator/kf_k5g3_zaehne_scan.py` (Scanner und Maler-Prüfung, neu), `generator/riegel_k5g3_zaehne.py` (neu),
  `generator/riegel_k5g3_tex.py` (TG/UV1), `generator/kf_k5g3_spielzahl.py` (Asset-sha).
- Stand: `K5/k5g/K5g3/BAHNSTEIG_FORM.EMD`, `FORM.EMD`, `bau.json`, `spiel_z.json`, die Riegel-Protokolle, `zaehne_vorn/riegel_zaehne.*`,
  `geo/messung/spiel_dreiecke.json` und `n52_asset_115f8721_*.json`.
- Spiel: `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (115f8721), `re15_port/tests/unit/form_j_posen.h`,
  `stufe3/gen/s3_testkopf.py` (Kommentar).

## 8 Runde 2: Größe der Vorderzähne und Entscheidung (Prüfer r1 Zahlen ROT)

Der Prüfer hat zu Recht beanstandet: Die unteren Vorderzähne sind Stummel, und die Kurzfassung behauptete "kleiner im Verhältnis der
Schnauze" (Faktor bis 0,708). Gemessen ist der erste untere neue Zahn auf 0,22 seiner Nennlänge gekürzt (39 statt 180). Die Kurzfassung oben
ist berichtigt. Das Spiel-Asset ist in Runde 2 **unverändert** (EMD 115f8721, TIM c00823b3), nur Text und zwei Mess-Befehle im Generator
sind neu (`kf_k5g3_zaehne.py platz` und `variante_lang`; die Ausgabe von `kf_k5g3.py bauen` bleibt bytegleich 115f8721).

**Gebaute Größe** (Prüfer `p4_eigen.py`: Länge / sichtbar außerhalb der eigenen Haut):

| oben | alt O2 | alt O1 | neu 1 | neu 2 | neu 3 | neu 4 | neu 5 | Mitte |
|---|---|---|---|---|---|---|---|---|
| Länge | 121,7 | 75,5 | 70,9 | 75,6 | 39,5 | 56,6 | 68,7 | 52,9 |
| sichtbar | 115,0 | 69,1 | 64,6 | 68,9 | 34,0 | 50,4 | 62,9 | 47,1 |

| unten | alt U2 | alt U1 | neu 1 | neu 2 | Mitte |
|---|---|---|---|---|---|
| Länge | 200,1 | 160,4 | 40,2 | 74,5 | 102,3 |
| sichtbar | 184,2 | 144,4 | 24,6 | 59,3 | 86,0 |

**Warum unten nicht länger ohne Durchstechen** (`python kf_k5g3_zaehne.py platz`): größte Länge eines unteren Zahns nach R6 (kein Kontakt
mit dem Oberkiefer in allen 771 Keys, kein Austritt, nicht vor der Nase) entlang der unteren Zahnlinie, s = Bogen ab U1:

| s | 40 | 60 | 80 | 100 | 120 | 140 | 160 | 180 | 200 | 220 | 240 | 260 | 280 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| größte Länge | 32 | 34 | 36 | 39 | 41 | 53 | 73 | 91 | 76 | 81 | 86 | 92 | 98 |

Der erste neue untere Zahn steht bei s 98. Beim Zubeißen (Key 25, Kiefer 0°) liegt die Unterkiefer-Oberseite dort nur 12 bis 24 unter der
Schnauzen-Unterseite (`r2/e5_spalt.py`); zur Spitze steigt die Schnauzen-Unterseite an, zur Mitte fällt der Mundboden ab. Daher wachsen die
unteren Vorderzähne zur Mitte. Eine andere Lage hilft nicht: weiter innen (z 80) wären es höchstens etwa 60, weiter vorn (ab s 160) entsteht
neben U1 eine Lücke von über 155 (Riegel ZV1).

**Variante B für den Nutzerentscheid** (`python kf_k5g3_zaehne.py variante_lang <88c49ce1.EMD> <aus.EMD>`, sha256 01f8b0d7…, nicht im
Spiel): Der erste untere Vorderzahn je Seite darf beim Zubeißen im Oberkiefer stecken wie die alten unteren, tritt aber nie auf der anderen
Seite aus und steht nie vor der Nase. Das ergibt 92,4 statt 39 (ohne Austritt: +1 bis 92,41, −1 bis 95,45). Länger geht nicht: mit 110 tritt
er in 4 Keys seitlich aus der Oberkiefer-Flanke, mit 163 in 76 Keys.

Maler-Scan (Port-Zeichenweg, alle 771 Keys; Pixel, an denen ein neuer Zahn über den Oberkiefer gemalt wird, also sichtbar durchsticht):

| Ansicht | nah vorn | nah schräg l | nah schräg r | nah Seite l | nah Seite r | 5080 | 5080 vorn | 5090 | Bild 2 |
|---|---|---|---|---|---|---|---|---|---|
| A Spiel-Asset (39) | 0 | 0 | 4 | 28 | 0 | 1 | 0 | 9 | 4 |
| B (92,4) | 403 | 160 | 31 | 30 | 0 | 1 | 0 | 21 | 28 |
| Zwischenlänge 60 | 553 | 1 | 208 | 28 | 0 | 1 | 0 | 13 | 7 |
| Zwischenlänge 80 | 431 | 17 | 62 | 28 | 0 | 1 | 0 | 20 | 11 |
| alte Zähne (zum Vergleich) | 60 299 | 1 972 | 6 | 6 256 | 3 936 | 6 | 2 539 | 1 035 | 1 421 |

- B: höchstens 3 bis 4 Pixel je Pose, in nah vorn in 223 von 771 Posen, in 5090 in 13 Posen. Es ist die Spitze des Zahns, die beim
  Zubeißen durch die Oberlippe schimmert.
- Jede Länge über der R6-Kappe zeigt solche Pixel (schon 60). Ein "langer, aber unsichtbarer" Zahn geht an dieser Stelle also nicht.
- B würde Riegel ZV3 (kein Kontakt) und ZV4 (nah schräg rechts 31 > 6 der alten) rot machen. Deshalb bleibt A im Spiel, bis der Nutzer
  entscheidet.

**Frage an den Nutzer** (Blatt `riesenform_zaehne/neu/4_wahl_unten_vorn.png`): Unten vorn so kurz lassen (A, sticht nie durch), oder den
ersten unteren Vorderzahn links und rechts gut doppelt so lang machen (B, 92 statt 39), wobei aus der Nähe beim Zubeißen ein paar Pixel der
Spitze durch die Oberlippe zu sehen sind? Mit "Länge wie die alten" (180) käme die Spitze sichtbar seitlich aus der Schnauze; das wird nicht
angeboten.
