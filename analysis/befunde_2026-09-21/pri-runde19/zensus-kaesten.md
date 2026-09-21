# Zensus A — alle 18 eigenen `quader`-Kaesten (Runde 19, PRI)

Auftrag: pruefen, ob fuer jeden der 18 `quader`-Eintraege aus
`analysis/esp_masken_2026-09-03/auswahl.json` ein Kasten die richtige Form ist, ob der
Unterschied im Spiel wirkt, und was man lassen kann. **Dieser Lauf hat KEINE
Masken-Assets geschrieben** — `re15_port/shared_assets/PSX/MASKS/` ist unveraendert
(`git status` im Worktree zeigt dort nichts).

ROOM10D0 C7 ("Holztisch mit Klappstuhl") wird laut Auftrag von einem anderen Agenten
bearbeitet und hier nur als Vergleichsmass mitgemessen, nicht angefasst.

---

## 0. Messweg

Alle Zahlen kommen aus den ausgelieferten Dateien und dem Engine-Bodendump, nicht aus
einem Modell:

| Schritt | Skript | Was gemessen wird |
|---|---|---|
| 1 | `build/mess_kaesten.py` | Auslieferungsstand je Kasten-Cut: Rechtecke, Deckung, rohe Quader-Silhouette |
| 2 | `build/r19/schiene.py` | Standplatz-Schiene (`abnahme.standplatz_schiene`) gegen die ausgelieferte Maske |
| 3 | `build/r19/farbtreue.py` | geblittete Farbe (`CLUT[idx]<<3`, `bg_pc.c:156-159`) gegen den Hintergrund, mit Nullmodell ueber 76 weitere Cuts |
| 4 | `build/r19/zuordnung.py` | Beitrag je Kasten: eigene Punkte, Kunst, Tiefschwarz |
| 5 | `build/r19/vollstaendig.py` | Deckung == gewollte Vereinigung? Wer liefert die naechste Tiefe? |
| 6 | `build/r19/budget.py` | Neubau mit/ohne die beitragslosen Objekte: Kachelkante, Rechteckzahl, Atlasflaeche |
| 7 | `build/r19/fehlurteil.py` | Urteilsunterschied Kacheltiefe gegen exakte Punkttiefe, je Standplatz |
| 8 | `build/r19/biss.py` | Bisspunkte ohne Quote (⛔ **fehlerhaft, s. §4 Widerlegung 7 — zurueckgezogen**) |
| 9 | `build/r19/wandbasis2.py` | Ueberstand der Wand-Kasten-Unterkante auf den gemalten Boden, je Bildspalte |
| 10 | `build/r19/streifen.py` | wirkt dieser Ueberstand auf begehbare Standplaetze? |
| 11–13 | `build/r19/sprenkel.py`, `funkeln.py`, `duenn.py` | Koernigkeit/Duenne der Maske, mit Nullmodell = die Nutzer-Lassos desselben Cuts |
| 14 | `build/r19/fixdemo.py` | Fixvorschlag am Buerostuhl, gemessen |

Bodenpunkte: `build/floor_dump.txt` (Sonde `probe_r16_pri_masken_audit`, 78 Raeume,
`re15_collision_on_floor` mit gesetztem Band). Hintergruende: `build/bg_ppm/` (Abzug
`probe_bg_dump`). Koerperkasten wie in `test_pri_kopfschnitt.c`: Fuss..Kopf 1500, ±450.

---

## 1. Was ausgeliefert ist

| Cut | Original-Kuenstlermasken | `.PBM` | Rechtecke | Deckung | Objekte |
|---|---|---|---|---|---|
| ROOM10F0 C4 | nein | **fehlt** | 105 (= Engine-Grenze) | 12068 | 8 quader + 3 Lasso |
| ROOM10F0 C5 | nein | **fehlt** | 105 | 9285 | 4 quader + 3 Lasso |
| ROOM1100 C1 | nein | **fehlt** | 105 | 28160 | 3 quader |
| ROOM1100 C2 | nein | **fehlt** | 105 | 25211 | 2 quader |
| ROOM10D0 C7 | nein | ja | 104 | 5435 | 1 quader + 2 Lasso |

Die vier Kasten-Cuts stehen in `raum.py:55-60` als `P2_UNANGETASTET` und sind deshalb beim
Phase-2-Neubau am 2026-09-19 **uebersprungen** worden; ihre Sektionen stammen unveraendert
aus dem `raum.py`/`anwenden.py`-Pfad vom 2026-09-09 (Blob-Vergleich: Sektion Cut 4 und
Cut 5 byte-identisch zwischen `a6d43ebb` und HEAD).

---

## 2. Befunde

### Befund 1 (hoch) — ROOM10F0 C4/C5: die Tiefschwarz-Regel malt PULT-Kantenstriche in Stuhl-Tiefe auf die Figur

**Symptom am Bild:** duenne dunkle Diagonalstriche quer ueber die Figur.
`zensus-kaesten/funkeln_ROOM10F0_C4.png` — Standplatz (4600,7200), Koerperkasten 1683
Punkte, 264 davon werden von der Maske ueberblittet.

**Ursache, belegt:** `raum.py:322`

```python
_dk = _bgL.astype(int).sum(2) < 45
```

Jeder Punkt in der Silhouette einer Stuhl-Kreiszelle, dessen Kanalsumme unter 45 liegt,
gilt als Stuhl (`_dunkel_label`), und `nur_kunst` (`raum.py:413`) gibt ihm die Tiefe
**dieses Stuhls**. Im dunklen ROOM10F0 trifft das nicht nur die schwarzen Lehnen, fuer die
die Regel am 2026-09-09 eingefuehrt wurde, sondern auch die Kanten- und Schattenlinien des
Pults, die Papierschatten und die Fugen darunter.

**Messung — Duenne (Struktur ≤ 2 Punkte, Erosion 3×3), mit Nullmodell:**

| Cut | Maske | davon duenn | Nullmodell: rohe Nutzer-Lassos desselben Cuts |
|---|---|---|---|
| ROOM10F0 C4 | 12068 | **5239 = 43,4 %** | 2131 Punkte, 559 duenn = 26,2 % |
| ROOM10F0 C5 | 9285 | **3873 = 41,7 %** | 3576 Punkte, 679 duenn = 19,0 % |
| ROOM1100 C1 | 28160 | 774 = 2,7 % | — |
| ROOM1100 C2 | 25211 | 747 = 3,0 % | — |
| ROOM10D0 C7 | 5435 | 661 = 12,2 % | 3641 Punkte, 953 duenn = 26,2 % |

⛔ Das Nullmodell ist wichtig, weil ein Rand immer duenne Punkte liefert: eine **kleine**
Flaeche hat deshalb prinzipiell den **hoeheren** Duennen-Anteil. C4 hat die **fuenffache**
Flaeche der Lassos und trotzdem den **hoeheren** Anteil — das ist die Umkehrung des
Randeffekts und damit kein Artefakt des Masses. ROOM10D0 C7 liegt mit 12,2 % **unter**
seinem Nullmodell und ist sauber.

**Wirkung im Spiel (Abdeckung mitgegeben):**

| Cut | Standplaetze projiziert | sehen die Maske | verdeckende Punkte gesamt | davon aus duennen Strukturen | betroffene Standplaetze |
|---|---|---|---|---|---|
| ROOM10F0 C4 | 5774 | 1740 (30,1 %) | 73224 | **42153 = 57,6 %** | 600 |
| ROOM10F0 C5 | 6394 | 1275 (19,9 %) | 83401 | **48052 = 57,6 %** | 752 |

Schlimmster Standplatz C4, Band 0 (4600,7400): 280 von 340 verdeckenden Punkten sind
duenne Striche. C5, Band 0 (4600,5000): 253 von 284.

**Welche Kaesten es sind** (`build/r19/duenn_zuordnung.py`) — die Spalte „d. Schwarz" ist
der Teil, der NICHT im Lasso des Nutzers liegt:

| Objekt | Punkte | duenn | d. Kunst | d. Schwarz |
|---|---|---|---|---|
| Buerostuhl x6700 z10300 (C4) | 112 | **112 = 100 %** | 0 | 112 |
| Buerostuhl x6600 z7500 (C4) | 84 | **82 = 97,6 %** | 0 | 82 |
| Buerostuhl x-1900 z4300 (C5) | 2628 | 1743 = 66,3 % | 0 | 1743 |
| Buerostuhl x1600 z5700 (C4) | 4122 | 2345 = 56,9 % | 0 | 2345 |
| Buerostuhl x1550 z11650 (C4) | 1068 | 375 = 35,1 % | 0 | 375 |
| Buerostuhl x4200 z11800 (C4) | 1010 | 278 = 27,5 % | 0 | 278 |
| Lasso+Zelle Teil 03 (C4) | 2506 | 963 = 38,4 % | 125 | 838 |
| Lasso+Zelle Teil 02 (C4) | 2169 | 908 = 41,9 % | 244 | 664 |

⛔ Die Lasso-Objekte tragen denselben Defekt (`tiefe: "szene"` nimmt in `raum.py:556`
ebenfalls `_dunkel_label == _ki` dazu). **Der Defekt sitzt in der Regel, nicht im Kasten** —
ein Fix nur an den Kaesten wuerde 1761 der 5239 duennen Punkte in C4 stehen lassen.

**Fixvorschlag, gemessen** (`build/r19/fixdemo.py`,
`zensus-kaesten/fixdemo_ROOM10F0_C4.png`): den Tiefschwarz-Anteil morphologisch
**oeffnen** (3×3-Erosion, dann Dilatation, dann Loecher fuellen) — es bleibt, was
mindestens 3 Punkte breit ist, also Koerper statt Striche. Loecher fuellen ist hier
zulaessig und schon einmal belegt (Pflanzen-Huelle ROOM10D0 C6): die Maske malt
Hintergrundpunkte, ein gefuellter Zwischenraum zeigt genau das, was man dort durch das
Moebel saehe.

| | jetzt | geoeffnet |
|---|---|---|
| ROOM10F0 C4, Maske | 12068 Punkte, 43,4 % duenn | 9746 Punkte, **26,9 % duenn** |
| verdeckende Punkte am Standplatz (4600,7400) | 340 | **116** |
| Buerostuhl x6700 z10300 | 112 Punkte | **0** |
| Buerostuhl x6600 z7500 | 84 Punkte | 12 |

26,9 % ist **keine gewuenschte Zahl, sondern der Wert des Nullmodells** (26,2 % bei den
Lassos des Nutzers) — die Schranke lautet also „so duenn wie die Handarbeit des Nutzers,
nicht duenner".

⛔ **Ehrlich zur Grenze dieses Fixes:** das Oeffnen entfernt die Striche, es macht die
Kaesten nicht richtig. Das rechte Bild in `fixdemo_ROOM10F0_C4.png` zeigt, dass die
geoeffneten Flaechen weiter ueber **Pultplatte und Tastatur** liegen, nicht nur ueber
Stuehlen — die Tiefschwarz-Regel ordnet Pultpunkte nach wie vor dem Stuhl zu. Wirklich
byte-naeher waere eine Freistellung des Nutzers fuer diese zwei Winkel; das Oeffnen ist
die messbare Verbesserung bis dahin, kein Beweis von Richtigkeit.
`Buerostuhl x6700 z10300` und `x6600 z7500` sollten unabhaengig davon **entfallen** — sie
liefern nichts als Striche.

---

### Befund 2 (mittel) — ROOM1100 C2: die Unterkante des Westwand-Kastens liegt auf dem gemalten Boden

**Symptom:** `zensus-kaesten/wandkante_ROOM1100_C2.png` (5-fach, gelb = Maskenkante): die
Kante laeuft unterhalb der gemalten Fussleiste auf den Fliesen.

**Messung** (`build/r19/wandbasis2.py`): je Bildspalte von unten der helle Fliesenlauf,
der Zeile 239 erreicht (Fugen bis 2 Zeilen geschlossen); Wandflaeche 15..70,
Fliesenboden 90..250 Kanalsumme.

| Cut | Schwelle 70 | 80 | 100 | 120 | Abdeckung |
|---|---|---|---|---|---|
| **C2** Median-Ueberstand | **+9 px** | **+7** | **+5** | **+4** | 87/82/59/40 von 142 Maskenspalten (61/58/42/28 %) |
| C2 P90 / Max | +14 / +22 | +13 / +22 | +10 / +12 | +5 / +10 | 99 % der Spalten > 1 px |
| C1 Median-Ueberstand | +2 | +1 | −12 | −34 | 89/88/82/75 (57/57/53/48 %) |

C1 liegt mit +1..+2 px im Antialias-Saum → **kein Befund**. C2 hat mit demselben Kasten
(`x-17600..-15600 z-23200..-12100`) median +7 px Ueberstand, 698 Maskenpunkte (2,8 %)
auf gemaltem Boden. Der Unterschied ist der Blickwinkel, nicht der Kasten.

**Wirkung** (`build/r19/streifen.py`, ohne Quote):

| Cut | Streifen | Standplaetze projiziert | mit Koerperpunkten im Streifen | davon verdeckt |
|---|---|---|---|---|
| C1 | 171 Punkte | 6237 | 69 (1,1 %) | **0 Punkte** |
| C2 | 698 Punkte | 25199 | 230 (0,9 %) | **1555 Punkte an 42 Standplaetzen** |

Schlimmste Standplaetze C2, Band 0: (−15700,−11500), (−15900,−11500), (−16100,−11500) mit
je **65 von 476** Koerperkastenpunkten = 13,7 %, unten links, also die Beine. Bild:
`zensus-kaesten/ueberstand_ROOM1100_C2.png`. Der Spieler steht dort im offenen
Querflur noerdlich des Wandendes (`z = -11500` gegen Kastenende `z = -12100`) und bekommt
die Beine von der Wandecke abgeschnitten.

**Fixvorschlag:** die Kollisionszelle ist gepolstert (dieselbe Klasse wie ROOM10D0 C7,
wo `minus 58,0,320,240` den Polster-Ueberstand abschneidet). Hier traegt der gemessene
Weg: die Silhouette je Spalte an der gemessenen Fussleiste beschneiden (die Kurve liegt
in `build/r19/fl_ROOM1100_C2.npy`), oder die Vorderebene der Wand einmessen statt der
Zellkante. Die 42 Standplaetze sind die Abnahme.

---

### Befund 3 (niedrig) — 9 der 18 Kaesten tragen nachweislich NICHTS bei

`build/r19/zuordnung.py` + `vollstaendig.py`. „ALLEIN" = Punkte, die kein anderes Objekt
des Cuts deckt; „naechste Tiefe" = Punkte, an denen dieser Kasten die vorderste Tiefe
liefert.

| Kasten | eigene Punkte | ALLEIN | Punkte mit der naechsten Tiefe |
|---|---|---|---|
| ROOM10F0 C4 Buerostuhl x-1600 z12200 | 997 | **0** | **0** |
| ROOM10F0 C4 Buerostuhl x-1800 z8200 | 2169 | **0** | **0** |
| ROOM10F0 C4 Buerostuhl x-1900 z4300 | 2506 | **0** | **0** |
| ROOM10F0 C5 Buerostuhl x1600 z5700 | 4860 | **0** | **0** |
| ROOM10F0 C5 Buerostuhl x1600 z1900 | 1158 | **0** | **0** |
| ROOM10F0 C5 Buerostuhl x1500 z-2000 | 637 | **0** | **0** |
| ROOM1100 C1 Suedblock | 14426 | **0** | **0** |
| ROOM1100 C1 Suedwestwand | 7566 | **0** | **0** |
| ROOM1100 C2 Nordwand | 13633 | **0** | **0** |

Zwei verschiedene Gruende:

* Die **sechs Stuhl-Kaesten** sind bitgleiche Doppel der `tiefe: "szene"`-Lasso-Objekte.
  `raum.py:552-560` gibt jeder Lasso-Komponente **bereits** den Quader ihrer eigenen Zelle
  (`[_zx, _zz, _zw, _zd, -1950]`); der gleichnamige `nur_kunst`-Kasten bildet danach
  dieselbe Region mit derselben Tiefenkarte.
* **Suedblock, Suedwestwand, Nordwand** liegen vollstaendig in der Silhouette der
  Westwand, und die Westwand ist an jedem gemeinsamen Punkt naeher (28160 von 28160 bzw.
  25211 von 25211 Punkten mit der naechsten Tiefe).

**Was das kostet:** die Engine liest hart hoechstens 105 Rechtecke je Cut
(`re15_port/include/re15_pri.h:50` `RE15_PRI_MAX_MASKS_PER_CUT 105`;
`re15_port/engine/src/pri_common.c:125` verwirft alles danach STILL). Die Kachelung in
`anwenden.py:436-465` startet auf der **groebsten** Kante und verfeinert nur, solange
Rechteckzahl und Atlasflaeche halten — beitragslose Objekte machen also die Kachelung der
uebrigen groeber, und eine groebere Kachel traegt EINE Tiefe ueber bis zu 64×64 Punkte.

| Cut | mit allen | ohne die beitragslosen | Deckung |
|---|---|---|---|
| ROOM10F0 C4 | Kanten 16/24/24 + 5×; 105 Rechtecke, 48102 Atlaspunkte, 49 Tiefenstufen | Kanten **12/12**/24; 105 Rechtecke, **33097** Atlaspunkte, **54** Tiefenstufen | **identisch** |
| ROOM10F0 C5 | Kanten 20/16/20; 105 Rechtecke, 40508, 41 Stufen | Kanten **16/12/12**; 105 Rechtecke, **23232**, **47** Stufen | **identisch** |
| ROOM1100 C1 | 24/24/40; **102** Rechtecke, 56542 | **20**; **83** Rechtecke, **31430** | **identisch** |
| ROOM1100 C2 | 20/24; **105** Rechtecke, 42820 | 20; **76** Rechtecke, **28480** | **identisch** |

⛔ **Und ehrlich zur Wirkung:** das Fehlurteil (`build/r19/fehlurteil.py`, Kacheltiefe
gegen exakte Punkttiefe je Standplatz) aendert sich dadurch kaum — C4 3837 → 3828 falsch
freie Punkte, C5 2423 → 2276, ROOM1100 0 → 0. Die Doppel sind also **Ballast, kein
sichtbarer Fehler**: entfernen ist richtig und risikolos (Deckung bitgleich), aber es ist
**kein** Fix fuer das, was der Nutzer sieht. Prioritaet niedrig, und ROOM1100 C1/C2
brauchen ausser Befund 2 nichts.

---

### Befund 4 (mittel, **eigener Fehler**, latent) — `bau_p2.py` wertet `nur_kunst` nicht aus

`re15_port/tools/maske/bau_p2.py`: `objekt_region()` gibt fuer `"quader"` `None` zurueck
(Kommentar „Silhouette kommt aus der Zelle (unten)"), und `bau_cut()` setzt dann

```python
vz, tr = geometrie.quader_auf_band(R, t, H, q[0], q[0]+q[2], q[1], q[1]+q[3], q[4], y0)
r = kaesten_anwenden(tr.copy(), o, e)
```

— die **rohe** Quader-Silhouette. `nur_kunst` kommt im ganzen Phase-2-Pfad nicht vor
(`grep -n nur_kunst re15_port/tools/maske/*.py` → nur `raum.py:413`). Die vier Cuts
ueberleben allein dadurch, dass `raum.P2_UNANGETASTET` (`raum.py:55-60`) sie
ueberspringt. Wird dieser Eintrag entfernt — der naechstliegende Schritt, wenn man die
Cuts „endlich auch durch die Abnahme" schicken will —, dann gilt:

| Cut | Vereinigung der ROHEN Quader | ausgeliefert | Faktor |
|---|---|---|---|
| ROOM10F0 C4 | **31757** | 12068 | **2,6** |
| ROOM10F0 C5 | **21467** | 9285 | **2,3** |
| ROOM1100 C1 | 28160 | 28160 | 1,0 |
| ROOM1100 C2 | 25211 | 25211 | 1,0 |

Das ist Punkt fuer Punkt die Regression, die am 2026-09-09 behoben wurde („die
Quader-Silhouetten der Stuehle re-blitteten BODEN ueber die Figur (Box breiter als die
Kunst)", `_warum` in `auswahl.json`). **Fixvorschlag:** `nur_kunst` in `bau_p2.py`
nachziehen — oder, solange das nicht geschehen ist, in `bau_p2.bau_cut` bei einem
`quader`-Objekt mit `nur_kunst` **abbrechen** statt es stillschweigend zu ignorieren.

---

### Befund 5 (mittel, **eigener Fehler**) — der Silhouetten-Riegel sieht genau diese vier Cuts nicht

`re15_port/tests/unit/test_pri_silhouette.c:103-104` laeuft ueber die `.PBM`-Dateien:

```c
if (L != strlen("ROOM0000_PRI00.PBM") || strcmp(e->d_name + L - 4, ".PBM") != 0) continue;
if (sscanf(e->d_name, "ROOM%4x_PRI%2d.PBM", &raum, &cut) != 2) continue;
```

`bau_p2` schreibt `TIM` + `PBM` + `STAND` zusammen und ist auf diesen vier Cuts nie
gelaufen — sie haben deshalb **keine `.PBM`** und fallen aus dem Riegel heraus.

* Riegel-Abdeckung: **77 von 323** Cuts mit Sektion im Container = **23,8 %**
* Nicht gesehen (ohne die separat gebaute ROOM2090/2091-Familie): **genau ROOM10F0 C4,
  ROOM10F0 C5, ROOM1100 C1, ROOM1100 C2**

Die vier Cuts, um die es in dieser Runde geht, sind also die vier ohne Regressionsschutz.
**Fixvorschlag:** beim Bau eines Cuts ohne `.PBM` die Soll-Silhouette trotzdem schreiben,
und im Riegel zaehlen, wie viele Container-Cuts **ohne** PBM sind (heute 246) — eine
Schiene ohne Abdeckungszahl haette das nicht gezeigt.

---

### Nebenbefund (niedrig, gehoert dem Parallel-Agenten) — ROOM10D0 C7

`build/r19/fehlurteil.py`: die ausgelieferte Maske urteilt an **einem** Standplatz,
Band 0 (−900, 25750), **209 Punkte zu VERDECKT** gegenueber der exakten Punkttiefe
derselben Objekte (Abdeckung: 23 von 6841 projizierten Standplaetzen sehen die Maske
ueberhaupt, 0,3 %). Alle anderen vier Cuts haben dort 0. Zur Weitergabe an den Agenten,
der Marke 4 untersucht — hier **nicht** angefasst.

---

## 3. Prioritaet — was man im Spiel wirklich sieht

| Rang | Befund | Cut | sichtbar an | Aufwand |
|---|---|---|---|---|
| **1** | duenne Pult-Striche in Stuhl-Tiefe auf der Figur | ROOM10F0 C4, C5 | 600 + 752 Standplaetze, bis 280 von 340 verdeckenden Punkten | Regel oeffnen + 2 Kaesten loeschen |
| **2** | Wand-Kasten-Unterkante auf dem Boden | ROOM1100 C2 | 42 Standplaetze, 65 von 476 Koerperpunkten (Beine) | Silhouette an der gemessenen Fussleiste beschneiden |
| **3** | fehlender Regressionsschutz | alle vier | nicht sichtbar, aber der Grund, warum 1 und 2 durchkamen | PBM mitschreiben, Abdeckung zaehlen |
| **4** | `nur_kunst` in `bau_p2` | latent | erst beim naechsten Neubau, dann Faktor 2,6 | 6 Zeilen |
| **5** | 9 beitragslose Kaesten | alle vier | nichts (Fehlurteil 3837 → 3828) | loeschen |

**Lassen kann man:** ROOM1100 **C1** komplett (Ueberstand +1..+2 px im Antialias-Saum,
Fehlurteil 0/0 ueber 1685 Standplaetze mit Maskensicht, 2,7 % duenn — **ein Kasten ist
fuer eine Wand die richtige Form, und hier ist es belegt**), die ROOM1100-Westwaende als
Form, und ROOM10F0 C5 `Buerostuhl x1500 z-2000` (52 Bisspunkte ueber 313 Standplaetze,
schlimmster Fall 7 Punkte).

---

## 4. Was diese Messung WIDERLEGT hat

1. **„Der Kasten ist die ausgelieferte Silhouette der Stuehle."** Nein. `nur_kunst`
   schneidet ihn auf die Kunst des Nutzers bzw. sein Tiefschwarz zurueck: rohe
   Vereinigung 31757 gegen 12068 ausgeliefert (Faktor 2,6; C5: 21467 gegen 9285). Die
   Frage „ist ein Kasten die richtige Form fuer einen Buerostuhl" ist fuer den
   Auslieferungsstand **gegenstandslos** — der Kasten ist dort Tiefentraeger, nicht
   Silhouette. Der echte Defekt sitzt in der Tiefschwarz-Regel (Befund 1).
2. **„Ein Kasten ist fuer eine Wand die falsche Form."** Nein, gegenteilig belegt:
   ROOM1100 C1/C2 haben Fehlurteil **0 in beiden Richtungen** ueber 1685 bzw. 9496
   Standplaetze mit Maskensicht und nur 2,7 %/3,0 % duenne Struktur.
3. **„Sektion und Atlas sind seit dem Neubau vom 2026-09-19 auseinandergelaufen."** Nein.
   Die `TIM`-Bytes von ROOM10F0_PRI04/05 haben sich geaendert, das **Atlas-Indexbild ist
   bitgleich** (17740 bzw. 15947 opake Punkte), nur die CLUT unterscheidet sich; die
   Kreuzprobe „alte Sektion + neuer Atlas" ergibt dieselben 12068 bzw. 9285 Punkte.
4. **„Die PRI malt den Hintergrund sichtbar falsch nach (Quantisierung auf 255 Farben)."**
   Nein. Farbfehler (max-Kanal, `CLUT[idx]<<3` gegen Hintergrund): Median 2–3 von 255,
   P95 4–5, also **unter** dem 5-Bit-Schritt von 8. Nullmodell 76 weitere Cuts: Median des
   Mittelwerts 3,3, schlechtester ROOM1140 C3 mit 1,8 % der Punkte > 16. Die Kasten-Cuts
   sind **besser** als der Durchschnitt.
5. **„Am 105-Rechteck-Deckel sind Rechtecke abgeschnitten worden, es fehlen Stuecke."**
   Nein. Die ausgelieferte Deckung ist in allen fuenf Cuts **exakt** die gewollte
   Vereinigung (fehlt 0, zuviel 0).
6. **„Der Kasten ueberdeckt den Spieler."** Nein, nach der Standplatz-Schiene:
   `VORverd = 0` **und** `VORteil = 0` in allen vier Cuts (Abdeckung 1740/1275/1685/9496
   Standplaetze mit Maskensicht).
7. **⛔ Mein eigenes erstes Bissmass war falsch und ist zurueckgezogen.**
   `build/r19/biss.py` meldete fuer Suedblock 37533, Suedwestwand 75827 und Nordwand 31219
   Bisspunkte. Der Test nimmt „Standplatz eindeutig VOR dem Kasten" = Fuss-`vz` kleiner
   als die **vorderste** Kastentiefe. Bei einer 11100 Einheiten **tiefen** Wand ist das
   deren nahes Ende; ein Standplatz neben dem fernen Ende gilt damit als „davor", obwohl
   die Wand an **seinem** Pixel naeher ist. Ausserdem tragen diese drei Regionen in der
   ausgelieferten Maske gar nicht ihre eigene Tiefe, sondern die naehere der Westwand
   (Befund 3) — das Mass hat also dem falschen Objekt zugerechnet. Ersetzt durch den
   Streifen-Test (§Befund 2), der nur die Punkte prueft, an denen im Bild **Boden** gemalt
   ist.
8. **⛔ Das Fehlurteil-Mass kann fuer Kasten-Objekte gar keine Ueber-Deckung finden.**
   Die Kacheltiefe ist das **Maximum** der Punkttiefen ihres Fensters
   (`anwenden.py:496`, `_st = np.max` fuer `kollision`/`szene`/`quader`), also immer ≥ der
   exakten Tiefe — ein Kasten kann dadurch nur zu **frei** urteilen. Die „0 falsch
   verdeckt" in Schritt 7 sind deshalb eine **Rechenfolge, kein Guetesiegel**; die
   Abdeckung dieses Masses fuer die Fehlerklasse „ueberdeckt" ist 0. Deswegen die
   separaten Masse Streifen (Befund 2) und Duenne (Befund 1).
9. **„Die 3837 falsch freien Punkte in C4 sind ein Defekt."** Nein, sie sind dieselbe
   Rechenfolge wie in 8 und die bewusst belegte Abwaegung von 2026-09-07 (ROOM10C0 C3,
   Marken F1438/F468/F508): Maximum statt Median, damit eine Kachel nur verdeckt, wenn sie
   GANZ vor dem Spieler liegt. Entfernen der Doppel aendert daran fast nichts
   (3837 → 3828).
10. **„Das erste Nachmessen von `mess_kaesten.py` zeigt den Bau."** Der Neubau aus
    `build/r19/budget.py` kommt fuer ROOM1100 C1 auf **102** Rechtecke, die ausgelieferte
    Sektion hat **105** — der heutige Werkzeugstand reproduziert die Sektion vom
    2026-09-09 also nicht bitgleich. Alle Aussagen hier stuetzen sich deshalb auf die
    **ausgelieferten** Daten; die Neubauten stehen nur fuer den Vergleich mit/ohne
    Doppel, wo beide Seiten mit demselben Werkzeug gebaut sind.

---

## 5. Bildbelege

`analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/`

| Bild | Was es zeigt |
|---|---|
| `funkeln_ROOM10F0_C4.png` | Figur mit den darueber geblitteten Maskenpunkten; die duennen Striche rot markiert (Befund 1) |
| `funkeln_ROOM10F0_C5.png` | dasselbe fuer C5 |
| `fixdemo_ROOM10F0_C4.png` | ausgeliefert (43,4 % duenn) gegen geoeffnet (26,9 % duenn) |
| `kasten_roh_vs_wirksam_ROOM10F0_C4.png` | rohe Quader gegen den Beitrag nach `nur_kunst` (Widerlegung 1) |
| `kasten_roh_vs_wirksam_ROOM10F0_C5.png` | dasselbe fuer C5 |
| `wandkante_ROOM1100_C2.png` | Maskenkante (gelb) gegen die gemalte Fussleiste, 5-fach (Befund 2) |
| `wandkante_ROOM1100_C1.png` | derselbe Schnitt fuer C1 — die Kante sitzt (kein Befund) |
| `ueberstand_ROOM1100_C2.png` | der Ueberstands-Streifen und die 65 gebissenen Koerperpunkte |
| `ausgeliefert_ROOM1100_C1.png`, `_C2.png`, `_ROOM10F0_C4.png`, `_C5.png`, `_ROOM10D0_C7.png` | Auslieferungsstand je Cut |

Die Messskripte liegen mit ab unter
`analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/messung/`; sie erwarten
`build/bg_ppm/` (Abzug `probe_bg_dump`) und `build/floor_dump.txt`
(`probe_r16_pri_masken_audit`) und werden aus dem Repo-Wurzelverzeichnis aufgerufen.
⛔ `messung/biss.py` und `messung/wandbasis.py` sind die **fehlerhaften ersten Fassungen**
und nur als Beleg fuer die Widerlegungen 7 bzw. fuer den Detektor-Fehler in §0 abgelegt —
gueltig sind `streifen.py` und `wandbasis2.py`.
