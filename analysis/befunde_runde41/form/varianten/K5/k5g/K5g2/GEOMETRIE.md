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
