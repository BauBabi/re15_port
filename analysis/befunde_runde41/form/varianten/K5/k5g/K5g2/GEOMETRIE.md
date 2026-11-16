# K5g2 Runde 2 - Mulde (O1): Arbeitsnotiz des Mulden-Bauers

Stand: abgegeben 2026-11-10. Gemessenes Modell: `K5/k5g/K5g2/geo/MULDE.EMD`,
sha256 `7726e46f17c78501f7a40db1280b98be8bede5517e0df0d869ce3163e9687409` (353428 Byte).
Basis: `K5/ruhe/K5e_f1/BAHNSTEIG_FORM.EMD`, sha256 `f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1`.

## Wo liegt was

| Was | Pfad (unter `analysis/befunde_runde41/form/varianten/`) |
|---|---|
| Skript (Tabelle, EMD, Pruefung) | `K5/generator/kf_k5g2_mulde.py` |
| Messwerkzeug (Streifen, alle Paare, Dreieckszahl) | `K5/generator/kf_k5g2_mulde_mass.py` |
| Stufe 1: Netze 10, 21, 22 aus der Orchestrator-Rechnung m2_a | `K5/generator/k5g2_mulde_stufe1.json` |
| Tabelle: fertige Netze 10, 16, 21, 22 | `K5/generator/k5g2_mulde_tabelle.json` |
| Modell (K5e_f1 + nur diese Netze) | `K5/k5g/K5g2/geo/MULDE.EMD` |
| Messlauf alle Bilder, Modell und Vergleich | `K5/k5g/K5g2/geo/lauf_mulde.json`, `lauf_k5e_f1.json` (+ `.log`), `summe_mulde.txt` |
| Pruefprotokoll (zweiter Lauf byte-gleich, Huell-Bedingung) | `K5/k5g/K5g2/geo/pruef.log` |
| Belegbilder | `K5/k5g/K5g2/geo/*.png` |
| Hilfsskripte der Versuche und Bilder | `K5/k5g/K5g2/geo/werkzeug/` |

Nachbau (im Ordner `K5/generator/`): `python kf_k5g2_mulde.py tabelle`, dann `emd`, dann `pruef huelle`.
Der zweite Lauf ist byte-gleich (Tabelle sha256 ueber LF-Zeilenenden
`e33d192f0695497a5b5678de4a569c6053fcd949abcd797ceae10972ecaff7b0`; das Repo checkt Textdateien mit CRLF aus,
deshalb rechnet das Skript die Pruefsumme von Textquellen ueber LF). Die Stufe 1 wird nicht neu gerechnet
(77 Minuten, Orchestrator-Lauf mulde2.py Lauf a, Rand 16, drei Runden); sie liegt als Datei bei, und
`pruef huelle` rechnet ihre Bedingung nach: Teil 10 (934 Ecken) und Teil 21 (731 Ecken) liegen in allen 799
Bildern mindestens 16 Einheiten ausserhalb der Kopf-/Kieferflaechen (groesste Tiefe -16.0, Clip 12 Bild 15 und 16).
Die Krallen-Netze 19, 20, 28, 29, 31 fasst das Skript nie an (Basis ist K5e_f1; `pruef` vergleicht alle Netze
ausserhalb 10/16/21/22 mit K5e_f1).

`*.EMD` und `*.png` stehen in `analysis/befunde_runde41/form/.gitignore`; sie sind mit `git add -f` aufgenommen.

Messlauf: `python kf_k5g2_mulde_mass.py lauf <aus.json> 6 1 <EMD>` (alle 799 Bilder aller Clips x 5 Ansichten
= 3995 Ansichten, 1671 s), Auswertung `python kf_k5g2_mulde_mass.py summe lauf_mulde.json lauf_k5e_f1.json`.
Beide Laufdateien enthalten je Ansicht ALLE Teilpaare (nicht auf die zehn groessten gekuerzt).

## Was in der Tabelle steht

| Teil | Inhalt | Ecken | Port-Dreiecke (K5e_f1) |
|---|---|---|---|
| 10 Buckel | Mulde aus Stufe 1, Textur auf der Mulde neu verteilt | 934 | 1259 (408) |
| 21 Hals | Mulde aus Stufe 1, Textur auf der Mulde neu verteilt | 731 | 876 (192) |
| 22 Kopf | Hinterkopf-Ecken zurueckgenommen (Runde 1, aus Stufe 1), sonst K5e_f1 | 402 | 618 (618) |
| 16 Schulterkugel + Oberarm | unveraendert K5e_f1 (Entscheid siehe unten) | 134 | 256 (256) |
| 23 Unterkiefer | nicht in der Tabelle, bleibt K5e_f1 (Entscheid siehe unten) | - | 196 (196) |

Je Teil vollstaendig: Ecken, Normalen, Flaechen mit Ecken-/Normalen-Nummern, UV, CLUT-Zeile, Texturseite
(`teile`), dazu `herkunft` (Mutterflaeche in K5e_f1 je Flaeche) und `uv_neu` (alle Flaechen mit neuer Texturlage).

## Ergebnis der Messung (alle Bilder, 3995 Ansichten, Toleranz 2 Bildpunkte)

| Riegel | K5e_f1 | MULDE.EMD | erfuellt |
|---|---|---|---|
| (i) {22,23} gegen {10,21}: Summe / Ansichten ueber 20 / groesste | 1043153 / 2388 / 2380 | 547 / 0 / 17 (spiel5080_vorn Clip 12 Bild 2) | ja |
| (ii) {22,23} gegen {10,21,16}: Summe / ueber 20 / groesste | 1047541 / 2452 / 2400 | 41765 / 302 / 618 (spiel5080_vorn Clip 12 Bild 53) | Ziel nein, Mindestmass ja |
| (ii) Mindestmass 22>16 und 16>22 | 24198 und 12691 | 13350 und 10846 | ja |
| (iii) Summe aller Paare mit 22 oder 23 | 1102720 | 144494 (13 Prozent) | ja |
| (vi) meiste gezeichnete Form-Dreiecke in einem Bild | 3320 | 4510 (spiel5090 Clip 12 Bild 40) | ja |

K5e_f1 ist in (i) und (ii) rot (Gegenprobe). Zu (i), Stichprobe: schon die Summe ueber ALLE 3995 Ansichten (547)
liegt unter der Schranke 3346, also auch jede Teilmenge. Die Regel aus dem Messwerkzeug (Clips 10/12 ganz, sonst
jedes vierte Bild) ergibt bei mir 1685 Ansichten, nicht 1425: K5e_f1 407945, MULDE.EMD 525 (0,13 Prozent).
Die 1425er Auswahl des Orchestrators konnte ich nicht nachstellen.

Die 302 Ansichten ueber 20 in (ii) liegen alle in den Clips 10 (78) und 12 (224) und kommen alle aus Teil 16
(ohne 16 ist keine Ansicht ueber 20).

Paare mit Kopf oder Unterkiefer, alle Bilder (links K5e_f1, rechts MULDE.EMD):

| Paar | K5e_f1 | MULDE.EMD | groesste Ansicht MULDE.EMD |
|---|---|---|---|
| 22>21 | 533480 | 49 | 2 |
| 21>22 | 370483 | 88 | 13 |
| 22>10 | 21718 | 6 | 3 |
| 10>22 | 20118 | 0 | 0 |
| 23>22 | 55991 | 56050 | 313 (spiel5080_vorn Clip 11 Bild 7) |
| 22>23 | 18101 | 18102 | 178 (spiel5080_vorn Clip 11 Bild 6) |
| 17>22 (Arm vor Kopf, nur gezeigt) | 24469 | 24493 | 447 (spiel5080_vorn Clip 12 Bild 20) |
| 22>17 | 4799 | 4783 | 161 |
| 22>16 | 24198 | 13350 | 237 (spiel5080_vorn Clip 12 Bild 1) |
| 16>22 | 12691 | 10846 | 270 (spiel5080_vorn Clip 12 Bild 53) |
| 23>16 | 3553 | 3590 | 68 |
| 16>23 | 2929 | 3086 | 88 |
| 18>22 | 6732 | 6732 | 820 (spiel5080_vorn Clip 9 Bild 26) |

Nebenwirkung auf Paare ohne Kopf: 16>10 faellt von 1026986 auf 242842, 10>16 von 703758 auf 103887,
21>10 von 357355 auf 169421, 10>21 von 395653 auf 271350. Leicht schlechter: 21>16 955 -> 2347,
11>10 2394 -> 3082, 21>7 13718 -> 14427. Die vollstaendige Liste steht in `geo/summe_mulde.txt`.

Hinweis zur Vorgabe: Die Paarlisten der Orchestrator-Stichprobe waren auf die zehn groessten Paare je Ansicht
gekuerzt. Der dort gelesene Anstieg von 23>22 und 22>23 ist ein Artefakt dieser Kuerzung; ungekuerzt sind beide
Paare gegen K5e_f1 gleich (56050 gegen 55991, 18102 gegen 18101).

(iv) Umriss gegen die Nutzermasken (`k5g2r2/silh.py`): Bild 2 IoU 0.6529 (K5e_f1 0.6529; 1 Bildpunkt weg,
0 dazu), Bild 1 IoU 0.7232 (K5e_f1 0.7231; 6 weg, 1 dazu). Bild: `geo/umriss_nutzermasken.png`.

(vi) Dreiecke: Form hoechstens 4510 in einem Bild. Spielermodelle PL00 748, PL04 724, PL08 773 Port-Dreiecke,
groesste Waffe 86; ROOM5080/5081/5090/5091 haben keine Raum-Objektmodelle (nOmodel 0). 4510 + 773 + 86 = 5369
von 8192 (render_pc.c:226 TEXTRI_QUEUE_MAX), 2823 frei. Das ist aus den Dateien gezaehlt, nicht in einem
laufenden Spiel gemessen (weitere Figuren, Effekte nicht mitgezaehlt).

## Entscheid Schulterkugel 16: bleibt K5e_f1

Gemessen: Der Kopf ueberstreicht in den 799 Bildern 88 Prozent des Inneren von Teil 16 (Schulterkugel und
Oberarm in einem Netz). Eine Mulde nach der Regel von 10 und 21 (Rand 16) wuerde den Oberarm aufloesen.
Seit der Mulde in 10 ist Teil 16 ausserdem die sichtbare Oberflaeche des Lappens (in K5e_f1 lag der Buckel 10
knapp darueber); jede Delle in 16 ist deshalb sofort zu sehen. Nur unterteilen ohne zurueckzunehmen bringt rund
10 Prozent. Deshalb: 16 bleibt Byte fuer Byte K5e_f1, das Ziel von (ii) ist nicht erreicht, das Mindestmass ist
erreicht (22>16 13350 gegen 24198, 16>22 10846 gegen 12691). Der Rest steht unter "Offen" und ist als Bild
belegt: `geo/rest_schulter_spiel5080_vorn_12_53_und_12_1.png`.

## Entscheid Unterkiefer 23: bleibt K5e_f1

23>22 und 22>23 sind eine Eigenschaft von K5e_f1 (Kieferflaechen kreuzen die Kopfflaechen am Maulwinkel);
die Mulde aendert daran nichts (56050 gegen 55991, 18102 gegen 18101). Versuche an den 60 schlimmsten Ansichten:

| Versuch | 23>22 | 22>23 | Summe aller Paare mit 22/23 | sichtbares Maul |
|---|---|---|---|---|
| MULDE.EMD (Abgabe) | 12517 | 5204 | 17753 | unveraendert |
| j1: Kopf und Kiefer ganz unterteilt (+588 Dreiecke) | 3342 | 3621 | 6995 | untere Zahnreihe aendert sich: 182 Bildpunkte in der Nutzerpose Bild 2 (Clip 12 Bild 0), 28 in Bild 1 (Clip 0 Bild 11) |
| Teilunterteilung nur texelgenauer Flaechen (23 von 126) | 11829 | 5381 | - | 72 Bildpunkte geaendert |
| k1: nur Kopf unterteilt | 11150 | 4979 | - | - |

Grund der Bildaenderung bei j1: PSX-UV sind ganze Bytes; die Mittelpunkte neuer Kanten runden auf ganze Texel,
und auf den kleinen Zaehnen ist ein Texel sichtbar. Die Vorgabe sagt: beheben, wenn das sichtbare Maul
unveraendert bleibt. Das ist bei j1 nicht der Fall, also bleibt 23 K5e_f1. Bilder:
`geo/kiefer_versuch_zaehne_bild2_12_0.png`, `geo/rest_kiefer_spiel5080_vorn_11_7_und_27_71.png`.
Der Versuch ist mit `geo/werkzeug/tj.py`, `tj2.py`, `tj3.py`, `kj.py` nachvollziehbar.

## Textur auf der Mulde, Riegel (vii)

Verfahren (im Skript `textur_neu`, in der Tabelle unter `textur`): Die Kinder einer Mutterflaeche werden im
UV-Vieleck der Mutter neu gelegt, Rand nach 3D-Bogenlaenge, Inneres nach Mittelwert-Koordinaten, gerundet auf
ganze Texel. CLUT-Zeile und Texturseite bleiben, es werden nur Original-Texel desselben Texturstuecks benutzt.
Normalen bleiben. Teil 10: 62 Muetter, 54 neu gelegt, 8 nicht konvexe behalten die vererbte Lage
(Mutterflaechen 127, 135, 136, 139, 143, 149, 150, 158). Teil 21: 65 von 65 neu gelegt.

Dehnung je Muldenflaeche relativ zur Mutter (Verhaeltnis 3D-Laenge je Texel, Kind gegen Mutter):

| Teil | Stand | ueber 2 | ueber 4 | ueber 8 | groesste | p99 |
|---|---|---|---|---|---|---|
| 10 | vererbt (Stufe 1) | - | 58 | 31 | 46.67 | 13.81 |
| 10 | neu verteilt | - | 20 | 2 | 13.82 | 5.09 |
| 21 | vererbt (Stufe 1) | 52 | 8 | - | 5.19 | - |
| 21 | neu verteilt | 14 | 0 | - | 4.0 | - |

Zu Punkten oder Linien entartete UV-Dreiecke in Teil 10: 41 vorher, 88 nachher (Rundung auf ganze Texel in
kleinen Texturstuecken; die Flaechen sind klein und zeigen dann eine Farbe).

Flaechen mit neuer Texturlage: 693 in Teil 10, 726 in Teil 21, alles Dreiecke. Die vollstaendige Liste steht
in der Tabelle `k5g2_mulde_tabelle.json` unter `uv_neu`: je Teil Paare [Flaechenart, Flaechennummer]
(Flaechenart 0 = Dreieck, 1 = Viereck; Nummer im abgegebenen Netz unter `teile`). Die neue Lage steht im Netz
selbst (`teile` -> `dreiecke` -> `uv`), die vererbte Lage in `k5g2_mulde_stufe1.json`; die Mutterflaeche in
K5e_f1 je Flaeche steht unter `herkunft`. Die Dehnungs-Kennzahlen (vererbt gegen neu) stehen unter `dehnung`.
Teil 22 und Teil 16 haben keine neue Texturlage.

Bildvergleich unter weissem Licht (ohne Mulde = K5e_f1 gegen MULDE.EMD):

| Ansicht | Bild | geaenderte Bildpunkte |
|---|---|---|
| spiel5080_vorn (Pflichtbild) | Clip 11 Bild 5 | 7728 (Vorgabe-Bezug 7729, Boden 2367 nicht erreicht) |
| bild1 | Clip 26 Bild 22 (groesster Abstand Kopf-Mulde, 1487.7) | 78 |
| bild2 | Clip 26 Bild 22 | 5038 |
| spiel5080 | Clip 26 Bild 22 | 86 |
| spiel5080_vorn | Clip 26 Bild 22 | 6789 |
| spiel5090 | Clip 26 Bild 22 | 4047 |
| bild2 (Nutzerpose) | Clip 12 Bild 0 | 1491 |
| bild1 (Nutzerpose) | Clip 0 Bild 11 | 62 |

Ursache im Pflichtbild ist nicht die Dehnung, sondern ein Teilwechsel: Wo in K5e_f1 der Buckel 10 lag, sieht man
jetzt Teil 16 (8306 Bildpunkte wechseln von 10 auf 16, 1075 von 10 auf 21, 601 von 21 auf 10, 526 von 21 auf
Hintergrund, 429 von 21 auf 22). Teil 16 traegt sein eigenes Muster; am linken Rand des Lappens steht eine
dunklere Kerbe. Blendet man Teil 16 aus, sieht man, was die neue Texturlage auf dem Krater von 10 leistet:
die radialen Schlieren sind weg (1909 Bildpunkte). Diesen Rest kann die Texturlage von 10 und 21 nicht beheben,
er steht unter "Offen". Bilder: `geo/pflicht_vorher_nachher_spiel5080_vorn_11_5.png`,
`geo/textur_neu_ohne_teil16_spiel5080_vorn_11_5.png`, `geo/teilwechsel_pflicht_und_26_22.png`,
`geo/abstand_<ansicht>_26_22.png` (fuenf Ansichten), `geo/nutzerpose_bild2_12_0.png`, `geo/nutzerpose_bild1_0_11.png`.

## Riegel (v): ist die Mulde zu sehen? Ja, in Posen mit gedrehtem Kopf.

Umrissverlust K5e_f1 -> MULDE.EMD ueber alle 3995 Ansichten: 456973 von 223500076 Figur-Bildpunkten
(0,204 Prozent). 603 Ansichten verlieren mehr als 200 Bildpunkte, 278 mehr als 500, 55 mehr als 1000;
groesste 1392 (spiel5080_vorn Clip 11 Bild 23, gleiche Pose wie Clip 0 Bild 8). Dort ist es als Spalt zwischen
Hinterkopf und Rumpf zu sehen: `geo/umriss_verlust_groesste.png`.

Anteile an den 100 schlimmsten Ansichten: Kopf 22 (zurueckgenommener Hinterkopf, Netz aus Runde 1) 67944 = 65
Prozent, Hals 21 26244 = 25 Prozent, Buckel 10 10296 = 10 Prozent. Groesster Verlust je Teil: 22: 1162
(spiel5080_vorn Clip 0 Bild 7), 21: 568 (spiel5080_vorn Clip 27 Bild 68), 10: 823 (spiel5080_vorn Clip 26
Bild 22, unterer Rand des Lappens).

Gegenprobe k22 (Original-Kopf 22 aus K5e_f1 mit der Mulde in 10 und 21, `geo/werkzeug/k22.py`): an denselben
100 Ansichten Streifen {22,23} gegen {10,21} 91572 (MULDE.EMD 22, K5e_f1 149709), alle 100 Ansichten ueber 20;
Figur-Bildpunkte 7492660 (MULDE.EMD 7400044, K5e_f1 7504338). Die Streifen verschwinden also nur mit dem
zurueckgenommenen Hinterkopf, und der kostet den Spalt. Das ist ein Zielkonflikt, den dieser Auftrag nicht
aufloest: Der Hinterkopf muesste eine eigene, engere Rueckwand bekommen (Sache des Geometrie-Bauers).

In den beiden Nutzerposen aendern sich 62 (Bild 1) und 1491 (Bild 2) Bildpunkte; der Umriss gegen die
Nutzermasken bleibt gleich (siehe (iv)). Stirn, Augenring, Augen und Maul sind in Teil 22 und 23 nicht
angefasst: 23 ist Byte fuer Byte K5e_f1, in 22 sind nur Hinterkopf-Ecken zurueckgenommen (Runde 1).
Nachgerechnet gegen K5e_f1: in Teil 22 sind 36 von 402 Ecken verschoben (groesste Verschiebung 1214 Einheiten),
Normalen, Flaechen und UV von Teil 22 sind gleich; Teil 16 ist in der Tabelle gleich K5e_f1.

## Offen (nicht behoben, nicht versteckt)

1. (ii) Ziel nicht erreicht: 302 Ansichten ueber 20, groesste 618, nur Clips 10 und 12, alle mit Teil 16.
2. (v) Spalt/Einschnuerung zwischen Hinterkopf und Rumpf bei gedrehtem Kopf, Umrissverlust bis 1392 Bildpunkte;
   65 Prozent davon aus dem Hinterkopf-Netz der Runde 1. Ohne dieses Netz kommen die Streifen zurueck (k22).
3. (vii) Lappen zeigt jetzt das Muster von Teil 16 statt der Mischung 10/16; Pflichtbild 7728 Bildpunkte,
   Boden 2367 nicht erreicht; dunklere Kerbe am linken Lappenrand.
4. Unterer Lappenrand von Teil 10: Verlust bis 823 Bildpunkte (spiel5080_vorn Clip 26 Bild 22).
5. 23>22 (56050) und 22>23 (18102) unveraendert K5e_f1. Wahl j1 liegt gemessen vor, kostet 182 Bildpunkte
   an der unteren Zahnreihe in der Nutzerpose und 588 Dreiecke.
6. Leicht schlechter als K5e_f1: 21>16 (955 -> 2347), 11>10 (2394 -> 3082), 10>11 (4148 -> 4296),
   21>7 (13718 -> 14427), 23>16 (3553 -> 3590), 16>23 (2929 -> 3086). 17>22 unveraendert (24493).
7. Acht nicht konvexe Muetter in Teil 10 behalten die vererbte Texturlage; entartete UV-Dreiecke 41 -> 88.
8. Dreiecksbudget aus Dateien gezaehlt, nicht im laufenden Spiel gemessen.
9. Die 1425er Stichprobe des Orchestrators ist nicht nachgestellt (meine Nachbildung: 1685 Ansichten).

## Hinweise fuer den Nachbau

- `geo/werkzeug/*.py` sind die Hilfsskripte der Versuche und Bilder. Sie tragen den Pfad dieses Arbeitsbaums
  fest im Kopf (GEN) und lesen `lauf_*.json` aus dem Ordner, in dem man sie startet. Die Versuchsmodelle
  (j1, k1, k22, ohne Mulde) sind nicht eingecheckt; die Skripte bauen sie neu.
- Fremde Werkzeuge (`k5g2r2/silh.py`) mit `PYTHONDONTWRITEBYTECODE=1` aufrufen.
- Das Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*` ist nicht angefasst.
- `kf_k5g2_geo.py` und `kf_k5g2_unterteil.py` sind unveraendert eingecheckt.

---

# K5g2 Runde 2 - Geometrie-Stufe (Mulde + Krallen): Arbeitsnotiz des Geometrie-Bauers

Stand: abgegeben 2026-11-10. Alles oberhalb der Linie ist die Notiz des Mulden-Bauers und gilt fuer `geo/MULDE.EMD`
(nur Mulde). Ab hier gilt alles fuer das Modell der Geometrie-Stufe:

`K5/k5g/K5g2/geo/BAHNSTEIG_FORM.EMD`, 354004 Byte,
sha256 `4164d55a259abfbfdc3051659a66ad552498c9c1834b1288b3797f341a74e598`.
Es ist K5e_f1 + neue Krallen (O2/E2) + Mulde (O1). Textur ist weiter die von K5e_f1
(`K5/ruhe/K5e_f1/BAHNSTEIG_FORM.TIM`); Textur, Helligkeit, Augen, Haende sind Sache des Bauers nach mir.
Das Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*` ist nicht angefasst (EMD f277916d..., TIM 31a35e66...).

## Ein Aufruf

Im Ordner `K5/generator/`:

    python kf_k5g2_geo.py bauen            # schreibt K5/k5g/K5g2/geo/BAHNSTEIG_FORM.EMD, nennt sha256
    python kf_k5g2_geo.py pruef            # baut im Speicher und vergleicht mit der Datei
    python riegel_k5g2_geo.py              # 15 Riegel, 22 Gegenproben, Exit 1 bei ROT oder STUMPF

Eingaben, alle eingecheckt: `K5/ruhe/K5e_f1/BAHNSTEIG_FORM.EMD` (Basis), `generator/k5g2_krallen.json` (Krallen-Rezept),
`generator/k5g2_mulde_tabelle.json` (Mulden-Tabelle des Mulden-Bauers). Kein Zufall, keine Uhr; zweiter Lauf byte-gleich.

Reihenfolge im Generator: erst Krallen, dann Mulde. Fuer den Textur-Bauer gibt es zwei Tueren:

- `G.geometrie_form(stufen=('krallen',))` liefert die Form mit neuen Krallen, aber mit den Flaechen von K5e_f1
  in den Teilen 10, 21, 22 (gleiche Flaechennummern wie K5e_f1 - darauf laesst sich texturieren wie bisher).
- `G.mulde_auf_form(form)` setzt danach die Mulde ein. Die Funktion weigert sich, wenn Teil 10/16/21/22 nicht die
  Flaechenzahl von K5e_f1 hat ("erst texturieren, dann mulde_auf_form"). Die Kinder erben CLUT-Zeile und
  Texturseite der Mutterflaeche; die UV der Kinder stehen in der Tabelle (Texel desselben Texturstuecks der Mutter).
  Wer die UV der Muetter in 10 oder 21 aendert, muss die Kinder-UV neu ableiten - die Tabelle kennt nur die
  K5e_f1-Lage.

Der Riegel der Geometrie steht als eigene Datei `generator/riegel_k5g2_geo.py` und nicht in `riegel_k5g2.py`:
dieser Entwurf laedt das nicht eingecheckte Textur-Modul und ist deshalb fuer sich nicht lauffaehig.
Die Entwuerfe `kf_k5g2.py`, `riegel_k5g2.py`, `kf_k5g2_tex.py`, `kf_k5g2_traeger.py`, `kf_k5g2_tafeln.py` sind
nicht eingecheckt. In `kf_k5g2.py` habe ich nur die zwei Stellen umgestellt, die die Geometrie holen
(`geometrie()` nimmt `G.geometrie_form(stufen=('krallen',))`, `bauen()` ruft `G.mulde_auf_form(form)`);
das ist nur auf Syntax geprueft, nicht gelaufen.

## Krallen (O2 / E2)

Rezept `generator/k5g2_krallen.json`:

- Rechte Krallen sind Spiegelnetze der linken: 14 -> 19, 15 -> 20, 28 -> 29, 30 -> 31.
  Regel ganzzahlig wie der Port: `v_r = (M * (x, y, -z) + 2048) >> 12`, M in Q12 (4096 = 1,0), Drehpunkt ist die
  Wurzel der rechten Kralle (Ursprung des Krallenraums); Normalen genauso; Wicklung umgedreht.
  Flaechen, UV, CLUT-Zeile und Texturseite sind die der linken Kralle.
- Drehung an der Wurzel je Kralle: 19 um 10 Grad, 20 um 80 Grad, 29 um 90 Grad, 31 keine.
- Kralle 28 (links) hat wieder die Ecken und Normalen von K5c (`stufe3/basis_k5c/FORM.EMD` Teil 28); Flaechen und
  Textur bleiben wie in K5e_f1.
- Hinten nichts geaendert. Knochen, Clips, EDD/EMR byte-gleich (123976 Byte vor den Netzen).

Gemessen (Riegel E2): Abweichung der rechten Netze vom gedrehten Spiegelbild hoechstens 1,13 Einheiten (Rundung),
Laengen links/rechts 542,0/541,5 - 620,6/620,5 - 730,5/730,7 - 572,6/572,6.

Warum gedreht: Nur gespiegelt (Vergleich A, `geo/vergleich/KRALLEN_VERGLEICH_A.EMD`) sieht die rechte Hand in
Ruhe aus wie die linke, aber in der Pose von Bild 2 stehen zwei Krallen als Stachel aus der Figur
(Kralle 20: 15,8 Bildpunkte, Kralle 29: 63,0 Bildpunkte ausserhalb der Videofigur). Mit der Drehung:
Bild 2: 19: 853 sichtbar / 0 ausserhalb; 20: 491 / 1 (1,0 Bildpunkte weit); 29: 223 / 30 (4,1); 31: 233 / 38 (5,0).
Bild 1: nur 29 sichtbar, 61 Bildpunkte, 53 ausserhalb, 3,2 weit. Schranke im Riegel 6 Bildpunkte.

Preis der Drehung (ehrlich): Die Krallen 20 und 29 liegen jetzt quer ueber dem Handruecken statt nach vorn.

- Die rechte Hand sieht in Ruhe NICHT aus wie die linke: links vier Krallen nach vorn, rechts zwei nach vorn (19, 31)
  und zwei auf dem Handruecken (20, 29).
- In der Seitenansicht von Raum 5080 (Ruhestellung) steht Kralle 29 als schmale Zacke vom Handruecken nach oben
  (Tafel 10, Zeile "von der Seite"). Der Riegel "kein Stachel" misst nur Bild 1 und Bild 2, diese Ansicht nicht.
- Neue Malfehler Hand 18 gegen Krallen (alle Bilder, 3995 Ansichten, mit 2 Bildpunkten Toleranz; K5e_f1 fast 0):
  18>20 42791 (groesste Ansicht 632, spiel5080_vorn Clip 27 Bild 3), 29>18 36577 (541, Clip 27 Bild 8),
  18>29 22203 (915, Clip 27 Bild 7), 20>18 18618 (223, Clip 27 Bild 64), 20>29 5074, 19>18 3445, 29>20 2969,
  18>19 2322, 20>22 2261 (251, Clip 9 Bild 23), 22>20 710. Fast gleich: 18>31 6535 (K5e_f1 7115).
  Besser: 31>18 839 (K5e_f1 4110).
  Zum Vergleich links: 14>13 19140, 13>14 8159, 15>13 13126, 30>13 12815, 13>30 8855.
  Hand gegen Krallen zusammen: rechts 133330, links 62095 (die aufgefuehrten Paare) - rechts rund das Doppelte.

## Messung am fertigen Modell (alle 799 Bilder x 5 Ansichten = 3995 Ansichten, Toleranz 2 Bildpunkte)

Lauf: `python kf_k5g2_mulde_mass.py lauf geo/lauf_geo.json 6 1 geo/BAHNSTEIG_FORM.EMD` (rund 12,5 Minuten),
Auswertung `geo/summe_geo.txt`. Die Laufdatei traegt die sha256 des Modells; der Riegel lehnt eine fremde Messung ab.

| Riegel | K5e_f1 | Geometrie-Stufe | erfuellt |
|---|---|---|---|
| (i) Kopf/Kiefer gegen Brust/Hals: Summe / Ansichten ueber 20 / groesste | 1043153 / 2388 / 2380 | 547 / 0 / 17 (spiel5080_vorn Clip 12 Bild 2) | ja |
| (ii) dazu Schulter 16: Summe / ueber 20 / groesste | 1047541 / 2452 / 2400 | 41765 / 302 / 618 (spiel5080_vorn Clip 12 Bild 53) | Ziel nein, Mindestmass ja |
| (ii) Mindestmass 22>16 und 16>22 | 24198 und 12691 | 13350 und 10801 | ja |
| (iii) Summe aller Paare mit 22 oder 23 | 1102720 | 146626 (13,3 Prozent, Schranke 25) | ja |
| (iv) Umriss gegen Nutzermasken, IoU Bild 2 / Bild 1 | - | 0.6490 / 0.7229, ohne Mulde 0.6490 / 0.7228 | ja |
| (vi) gezeichnete Form-Dreiecke, groesstes Bild | 3320 | 4473 (spiel5090 Clip 12 Bild 40) | ja |
| (vii) Dehnung Teil 10: groesste / ueber 8 / ueber 4 | vererbt 46.67 / 31 / 58 | 13.82 / 2 / 20 | ja (Schranke siehe unten) |
| (vii) Dehnung Teil 21: groesste / ueber 4 / ueber 2 | vererbt 5.19 / 8 / 52 | 4.00 / 0 / 14 | ja |

Zu (i), Stichprobe 1425: nicht nachstellbar (Nachbildung der Regel ergibt 1685 Ansichten: 525 gegen 407945).
Der Riegel verlangt deshalb zusaetzlich die Summe ueber ALLE 3995 Ansichten (547) unter der Schranke 3346;
damit haelt jede Teilmenge, auch die 1425er.

Zu (iv): Die IoU-Werte sind mit dem Raster des Riegels gerechnet und weichen deshalb leicht von den Zahlen des
Mulden-Bauers (0.6529 / 0.7232, Werkzeug silh.py) ab. Die Aussage ist dieselbe: Die Mulde nimmt in Bild 2 einen
Bildpunkt weg und bringt keinen dazu, in Bild 1 sechs weg und einen dazu.

Zu (vi), Budget TEXTRI_QUEUE_MAX 8192 (render_pc.c:226): Das Modell hat 7945 Port-Dreiecke (K5e_f1 6386, nur neue
Krallen 6410). Gezeichnet werden davon je Bild hoechstens 4473. Gerechnet: 4473 + Spielerin 773 + Waffe 86 = 5332.
Im laufenden Spiel gezaehlt (RE15_TRILOG, `geo/dreiecke_spiel.json`, Auswertung `geo/werkzeug/tri_*.py`):
Raum 5090, Clip 12 Bild 40: hoechstens 5190 je Bild (Form 4454, Spielerin 712, Flammen 24; 308 Bilder).
Raum 5080 von vorn, Clip 12 Bild 37: 4417 (nur Form, Spielerin steht ausserhalb des Bilds; 356 Bilder).
Es bleiben rund 3000 frei.

Zu (vii), Schranken: Sie sind eine Rueckfall-Sperre am gemessenen Stand der Tabelle (Teil 10: groesste hoechstens
16, ueber 8 hoechstens 2, ueber 4 hoechstens 20; Teil 21: groesste hoechstens 4,5, ueber 4 keine, ueber 2
hoechstens 14). Sie trennen die beiden gemessenen Staende (vererbte UV = ROT, neu verteilte UV = gruen).
Sie sind kein Wahrnehmungsgesetz; ob man die Mulde sieht, entscheidet Tafel 13, nicht diese Zahl.
Rest: 2 Port-Dreiecke in Teil 10 ueber Dehnung 8 (groesste 13,82), 88 UV-Dreiecke in Teil 10 zu Punkt oder Linie
entartet (zeigen eine einzige Farbe). Flaechen mit neuer UV: 693 in Teil 10, 726 in Teil 21 (Liste `uv_neu` in der
Tabelle).

Unveraendert gegen K5e_f1 (nur gezeigt, nicht behoben): 23>22 55970 (55991), 22>23 17858 (18101),
17>22 24493 (24469), 18>22 6056 (6732).

## Riegel der Geometrie

`python riegel_k5g2_geo.py`, Protokoll `geo/riegel_geo.txt`. Schlusszeile:
"RIEGEL K5g2 GEOMETRIE: 15 von 15 gruen, 22 von 22 Gegenproben rot wie verlangt".
Jeder Riegel hat mindestens eine Gegenprobe, die rot sein muss (sonst meldet er STUMPF und Exit 1):

| Riegel | Gegenprobe (muss rot sein) |
|---|---|
| Technik Clips (123976 Byte vor den Netzen gleich K5e_f1) | ein Bit gekippt |
| Technik Generator (Datei = Generator) | Modell ohne Krallen-Stufe |
| Technik uebrige Netze (24 Netze und Knochen gleich K5e_f1) | eine Ecke in Schulter 16 um 1 versetzt; eine Ecke im hinteren Teil 24 um 1 versetzt |
| Technik Kopf 22 (Flaechen/UV/CLUT/Normalen gleich, 36 von 402 Ecken bewegt) | UV + 1, eine 37. Ecke |
| Technik Textur-Erbe (CLUT-Zeile und Seite der Mutter, Liste uv_neu vollstaendig) | eine Flaeche mit fremder CLUT-Zeile; ein Eintrag aus uv_neu gestrichen |
| Technik Tabelle (Netze 10/16/21/22 = Tabelle auf K5e_f1) | K5e_f1 ohne Mulde |
| O1 (i) | K5e_f1; Messung eines anderen Modells |
| O1 (ii) Mindestmass | K5e_f1 |
| O1 (iii) | K5e_f1 |
| O1 (vi) | Mulde zweimal weiter unterteilt (hochgerechnet 8938); Laufzeit-Datei fehlt; Laufzeit-Liste voll (8192) |
| O1 (iv) | Kopf ganz eingezogen (Bild 2 IoU 0.6455) |
| O1 (vii) | vererbte UV (Stufe 1) |
| E2 Spiegelnetze rechts | K5e_f1 |
| E2 Kralle 28 = K5c-Form | K5e_f1 |
| O2 kein Stachel in Bild 1/Bild 2 | nur gespiegelt (Vergleich A); K5e_f1 |

O1 (v) (Mulde sichtbar?) hat keinen Riegel, das ist ein Bildurteil: Tafel 13.
O1 (ii) prueft nur das Mindestmass; das Ziel "keine Ansicht ueber 20" ist nicht erreicht und steht unter Offen.

## Spiellauf mit dem Geometrie-Modell

`re15_port/tools/form_spiel.sh` (eigene exe-Kopie, eigenes Fenster per gdigrab, kein AUTOSHOT, kein
SOFTWARE_RENDER), Modell ueber RE15_RE2_ASSET_ROOT, Textur K5e_f1. Acht Laeufe am 2026-11-10
(Ordner `re15_port/build/form_spiel/1941_204140` bis `3147_210821`): sieben mit dem Geometrie-Modell
(fuenf Fensterbilder in den Raeumen 5080 und 5090, zwei Zaehllaeufe fuer die Dreieckszahl, siehe O1 (vi)),
einer mit K5e_f1 als Vergleich an der schlimmsten Stelle. Kein Absturz; in jedem Lauf meldet das Spiel
"Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips". Fensterbilder: `geo/spiel/geo_*.png`
(Raum 5080 Seite, 5080 von vorn, 5080 von vorn Clip 27 Bild 56, 5090 zweimal); die Vergleichsbilder von
K5e_f1 liegen als `geo/spiel/heute_*.png` daneben.

Gesehen:

- Figur vollstaendig, keine Loecher, keine fehlenden Teile.
- Schlimmste Stelle von heute (Raum 5080 von vorn, Clip 27 Bild 56): Im Fenster von K5e_f1 liegen rote
  Flecken (Maulinneres, Kopfflaechen) quer ueber der Brust; im Fenster des Geometrie-Modells sind sie weg.
- Pose wie Bild 2 und Bild 1: Im Fenster ist die Figur mit der K5e_f1-Textur so dunkel, dass man die
  Streifen am Kopf weder vorher noch nachher erkennt. Dort belegt nur das Modellbild unter weissem Licht
  den Unterschied.
- Die neuen Krallen rechts sind im Fenster als helle Sprenkel auf dem Handruecken zu erkennen, nicht als
  Krallenform.

## Tafeln (roh) und was darauf zu sehen ist

Gebaut mit `python geo/werkzeug/tafeln_geo.py tafeln` (im Ordner `K5/k5g/K5g2/`). Modellbilder: Buehne des
Ports unter weissem Licht, Textur K5e_f1, keine Aufhellung. Marken rot/gelb sind roh gezaehlt, ohne die
2 Bildpunkte Toleranz der Riegel - deshalb sind die Zahlen in den Kacheln groesser als die Riegelzahlen.

`tafeln/10_krallen.png` - Krallen vorher / nachher / Vergleich A, Hand vorn rechts und links.

- Bild 2: rechts vorher nur eine Kralle zu sehen (31), nachher vier: 19 lang nach unten, 31 am Rand,
  20 und 29 klein auf dem Handruecken. Vergleich A: 20 und 29 stehen als lange Stachel nach links oben
  aus der Hand. Links kommt Kralle 28 (K5c-Form) dazu.
- Bild 1: rechts nur ein schmaler Streifen von Kralle 29 am Handrand, kein Stachel.
- Raum 5080 von der Seite, Ruhe: Kralle 29 steht als schmale Zacke vom Handruecken nach oben. Offen.
- Preis-Zeile (Raum 5080 vorn, Clip 27 Bild 7, Hand gross im Bild): vorher 0/0, nachher rot 1099 und
  gelb 831 Bildpunkte (Hand ueber die Krallen gemalt und umgekehrt), Vergleich A 0/475. In Ruhe 13/12.
- Echte Fenster: Haende sehr dunkel, Krallen kaum zu unterscheiden.

`tafeln/11_streifen_weg.png` - Streifen am Kopf, K5e_f1 gegen Geometrie-Modell, je ohne und mit Marken.

- Bild 2 (Clip 12 Bild 0): rot 883 / gelb 366 -> 299 / 4. Der Rest ist ein Keil, an dem der Kopf ueber
  den grauen Schulterlappen gemalt wird (Teil 16, offen).
- Bild 1 (Clip 0 Bild 11): 0 / 0 vorher und nachher.
- Raum 5080 vorn (Clip 12 Bild 0): 929 / 661 -> 253 / 5. Raum 5090 (Clip 12 Bild 0): 726 / 291 -> 252 / 2.
- Schlimmste Stelle von heute (Clip 27 Bild 56): 1570 / 1060 -> 0 / 0; die roten Flaechen ueber der Brust
  sind weg, auch im echten Fensterpaar.
- Groesste verbliebene Stelle (Clip 12 Bild 53, Schulterlappen): 485 / 609 -> 325 / 478. Nicht behoben.
- Kopf gegen Brust/Hals allein (Clip 12 Bild 2): 677 / 414 -> 41 / 0.
- Kachel Unterkiefer gegen Kopf (Clip 11 Bild 7): 478 / 64 vorher und nachher. Nicht geaendert.
- Kachel Arm rechts ueber Kopf (Clip 12 Bild 20): 542 / 66 vorher und nachher. Nur gezeigt.
- Hand rechts ueber Kopf (Clip 9 Bild 26): 901 / 79 -> 976 / 122 (mit den neuen Krallen etwas mehr).
- Unter weissem Licht sind die Streifen in den Posen von Bild 2 auch vorher unauffaellig (Kopf und Rumpf
  haben fast dieselbe Farbe); deutlich ist der Unterschied dort, wo das rote Maulinnere ueber die Brust
  gemalt wurde.

`tafeln/13_mulde_sichtbar.png` - K5e_f1 | m2_a (nur dessen Mulde) | Geometrie-Modell | rot = anders als
dasselbe Modell ohne Mulde. Zahlen: Bildpunkte im ganzen Bild mit Farbunterschied ueber 24 (R+G+B
zusammen); das ist die Zaehlweise der Tafel, nicht die des Mulden-Bauers (dort 7728 im Pflichtbild).

- Pflichtbild Raum 5080 vorn, Clip 11 Bild 5: 11632 anders (ohne Kopf 6644); m2_a 11609 (6617).
  Nicht besser als m2_a.
- Kopf weit weg (Clip 26 Bild 22): Bild 1 78, Raum 5080 Seite 86, Bild 2 6561 (ohne Kopf 4441),
  Raum 5080 vorn 8990 (5880), Raum 5090 5202 (3601).
- Ruhe: Raum 5080 vorn Clip 2 Bild 0 6499 (5215), Raum 5080 Seite 537.
- Ehrlich beschrieben: kein Loch (nirgends Hintergrund durch die Figur), keine Delle und kein Knick im
  Umriss in diesen Ansichten. ABER die graue Schulterkugel sieht anders aus als heute: Die dunklen
  Flecken auf der Kugel sind weg, sie ist heller und zeigt ihr eigenes streifiges Muster. Ursache ist
  nicht die Dehnung, sondern dass man jetzt Teil 16 sieht, wo vorher Stuecke von Teil 10 darueber gemalt
  wurden (im Pflichtbild 4894 Bildpunkte von 10 auf 16). Das sieht man in jeder Pose von vorn, auch wenn
  der Kopf weit weg ist.
- Groesster Umrissverlust (Clip 11 Bild 23, 7684 anders, ohne Kopf 4669): Zwischen Hinterkopf und Rumpf
  ist die Kerbe breiter als heute.

## Rueckweg-Versuch Hinterkopf (nicht uebernommen)

Versuch, die eingezogenen Hinterkopf-Ecken nur so weit einzuziehen, wie es die Bedingung verlangt
(Skript im Scratch, nicht eingecheckt): Summe der Wege 24501 -> 22375 (9 Prozent weniger). Das aendert die
Kerbe nicht merklich; es bleibt bei der Tabelle des Mulden-Bauers. Der Zielkonflikt (Streifen weg nur mit
eingezogenem Hinterkopf, der Hinterkopf kostet Umriss) bleibt offen.

## Offen nach der Geometrie-Stufe (nicht behoben, nicht versteckt)

1. O1 (ii): Ziel nicht erreicht. 302 Ansichten ueber 20, groesste 618 (spiel5080_vorn Clip 12 Bild 53),
   alle mit Schulter 16. Mindestmass erreicht. Kachel in Tafel 11.
2. O1 (v)/(vii): Die Schulterkugel sieht anders aus als heute (Teil 16 statt Mischung 10/16), in jeder
   Pose von vorn. Pflichtbild nicht besser als m2_a, Boden 2367 nicht erreicht. Teil 16 ist jetzt
   sichtbare Flaeche - seine Textur zaehlt (Hinweis an den Textur-Bauer). Tafel 13.
3. O1 (v): Kerbe zwischen Hinterkopf und Rumpf bei gedrehtem Kopf, Umrissverlust bis 1392 Bildpunkte
   (spiel5080_vorn Clip 11 Bild 23). Tafel 13.
4. O2: Die rechte Hand sieht in Ruhe nicht aus wie die linke (20 und 29 liegen quer auf dem
   Handruecken); Kralle 29 steht in der Seitenansicht von Raum 5080 als Zacke nach oben; Malfehler Hand
   gegen Krallen rechts rund doppelt so viel wie links (133330 gegen 62095). Vergleich A haette das
   nicht, hat aber den Stachel in Bild 2 (Kralle 29 ragt 63 Bildpunkte weit hinaus). Das kann der Nutzer am Bild entscheiden (Tafel 10).
5. Unterkiefer gegen Kopf unveraendert (23>22 55970, 22>23 17858). Kachel in Tafel 11.
6. Arm 17 ueber Kopf (24493) und Hand 18 ueber Kopf (6056) unveraendert, nur gezeigt.
7. O1 (vii): 2 Port-Dreiecke in Teil 10 ueber Dehnung 8, 88 entartete UV-Dreiecke.
8. Der Riegel "kein Stachel" misst nur Bild 1 und Bild 2, keine Spielkamera.
9. Die 1425er Stichprobe ist nicht nachgestellt (Ersatz: Summe ueber alle 3995 Ansichten 547, Schranke 3346).
10. Im Fenster sind die Streifen in den Posen von Bild 1/Bild 2 wegen der dunklen Textur weder vorher
    noch nachher zu erkennen; der Fenster-Beleg "Streifen weg" gilt nur fuer Clip 27 Bild 56.
11. `generator/k5g2_krallen.json` nennt Basis und K5c-Quelle mit dem absoluten Pfad dieses Arbeitsbaums
    (nur Herkunftsangabe; der Generator rechnet mit eigenen Pfaden und den Zahlen des Rezepts).
12. Der Entwurf `generator/kf_k5g2.py` (Textur-Stufe, nicht eingecheckt) ist auf die neue Reihenfolge
    umgestellt (erst Krallen, dann Textur, dann `G.mulde_auf_form`), aber nur auf Syntax geprueft.
