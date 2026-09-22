# ROOM10F0 C4/C5 — Umsetzung: „Leon ist da großteils transparent"

Runde 22, Umsetzungsphase zur Nutzer-Marke `befund_10F0_F335_marke1.bmp`
(`befund.log` ab Zeile 15602). Vorarbeiten: `10f0-doppel-masken.md` (Untersuchung A)
und `10f0-quader-silhouette.md` (Untersuchung B).
Bilder, Werkzeuge und Rohausgaben: `analysis/befunde_2026-09-21/10f0-umsetzung/`.

---

## Kurzfassung

Der Fehler ist weg, und er ist nicht mit der Ursache repariert worden, die Untersuchung B
benannt hatte. **Alle 154 falsch verdeckten Figurpunkte an der Marke tragen das Etikett
EINER Zelle — 17, also genau des Stuhls, für den eine Freistellung des Nutzers vorliegt**
(`messung/herkunft_marke.txt`). Und die Freistellungen haben **null** dunkle Löcher
(C4 genau 0, C5 genau 1 Bildpunkt) — die Begründung der Saum-Regel („Lehnen-Lücke,
schwarz auf schwarz im Lasso-Loch") trägt für diese Cuts also nicht.

Daraus folgt die Regel, die jetzt im Werkzeug steht:
**wo der Nutzer freigestellt hat, ist die Freistellung die Wahrheit.**
Tiefschwarz darf nur noch die *Löcher* der eigenen Kunst füllen, und die Zuordnung „nächste
enthaltende Zelle" bedient nur noch Zellen **ohne** Freistellung.

| an der Marke F335 (837 gezeichnete Figurpunkte) | vorher | nachher |
|---|---:|---:|
| verdeckt | **426 = 50,9 %** | **272 = 32,5 %** |
| davon durch die Freistellung (richtig — der Stuhl steht davor) | 272 | 272 |
| davon durch die Tiefschwarz-Zugabe (falsch) | **154** | **0** |

| auf dem wirklich gespielten Weg (`befund.log`) | vorher | nachher |
|---|---:|---:|
| C4: Bilder mit mehr als der halben Figur weg | **12 von 30** | **0 von 30** |
| C4: Anteil aller Figurpunkte | 17,00 % | 11,22 % |
| C5: Anteil aller Figurpunkte | 8,04 % | 6,01 % |

Und — das ist der Punkt, an dem der naheliegende Vorschlag scheitert — die Pulte,
die Konsolenbank und die schwarzen Stühle **behalten ihre Maske**: 8527 von 12068 Texeln
in C4, 6205 von 9285 in C5. Die Zahl der begehbaren Standplätze mit Maskenberührung
bleibt praktisch gleich (C4 152 → 153, C5 516 → 498).

---

## 0. Eichung — ohne die wäre jede Zahl hier wertlos

ROOM10F0 C4/C5 standen in `P2_UNANGETASTET` (`raum.py:55`, Stand vor dieser Runde) und wurden seit dem
2026-09-19 von keinem Lauf mehr gebaut. Bevor ich irgendetwas geändert habe, musste
der Bauweg reproduzieren, was ausgeliefert ist:

```
CUT 4: Rechtecke 105 | Sollflaeche 12068 px
   Sektion  ausgeliefert 2104 B, nachgebaut 2104 B, BITGLEICH=True
   TIM      ausgeliefert 66080 B, nachgebaut 66080 B, BITGLEICH=True
CUT 5: Rechtecke 105 | Sollflaeche 9285 px
   Sektion  ausgeliefert 2104 B, nachgebaut 2104 B, BITGLEICH=True
   TIM      ausgeliefert 66080 B, nachgebaut 66080 B, BITGLEICH=True
```

`werkzeug/nachbau.py`, Weg = `raum.objekt_regionen` → `anwenden.bau_objektweise`, also
exakt der Pfad, den `raum.main()` für `rid >= 0x2000` nimmt. Dasselbe Skript zeigt heute
Bitgleichheit mit dem **neuen** Auslieferungsstand (`messung/eichung_nachbau.txt`) — die
ausgelieferten Assets sind also genau das, was das Werkzeug erzeugt, ohne Handarbeit.

Dieselbe Eichung noch einmal für die Gegenmodelle: `werkzeug/variante.py eich` baut mit
neutralen Schaltern Sektion und TIM beider Cuts **bitgleich** zur Auslieferung. Erst
danach wurden Schalter umgelegt.

---

## 1. Die Messkette

* **Figur:** `re15_port/tests/unit/probe_r22_10f0_figur.c` (aus Untersuchung B, hier
  erweitert). `PLD/PL00.MD1` + `PL00.EMR` posiert, mit dem Renderer-Transform aus
  `main.c` (`PROJECT_VERT`, GTE-Divide) projiziert und in einen 320×240-Puffer mit
  Kamera-z je Bildpunkt gerastert.
* **Maske:** texelgenau aus `MASKS/ROOM10F0.MSK` + `_PRI0x.TIM`, Palettenindex 0 =
  durchsichtig (`bg_pc.c:145`).
* **Urteil:** das Original-Modell aus `re15_pri.h` — verdeckt ⇔
  `depth < (1023·vz) >> 16` (`@0x8002565c sra v1,v1,4` auf `otz = AVSZ3`,
  `ZSF3 = 341 @0x80066c70`; Maskenwort ×1 `@0x80039658`).
* **Neu hier:** `R22_MASKEN_DIR` — die Sonde liest einen *anderen* Maskenbaum. Nur so
  lässt sich ein umgebautes Asset mit **derselben** Figur messen, ohne die Auslieferung
  anzufassen. Der alte `R22_MASKENFILTER` konnte nur Texel wegnehmen und hätte jede
  Änderung an Tiefe oder Kachelung verschluckt.

Die Python-Seite (`werkzeug/marke.py`) rechnet dasselbe Urteil und kommt auf **dieselben**
Zahlen wie die Sonde (426 / 272 / 154) — zwei unabhängige Implementierungen derselben
Regel.

---

## 2. Die Doppel: entfernt, und sie waren nicht die Ursache

Untersuchung A hatte belegt, dass sechs `quader`-Einträge Zellen bedienen, die schon ein
`tiefe:"szene"`-Lasso tragen. Sie sind aus `analysis/esp_masken_2026-09-03/auswahl.json`
entfernt (C4: `Buerostuhl x-1600 z12200`, `x-1800 z8200`, `x-1900 z4300`;
C5: `x1600 z5700`, `x1600 z1900`, `x1500 z-2000`). Objektzahl C4 11 → 8, C5 7 → 4.

**Bitgenauer Nachweis, dass kein gedeckter Punkt verloren geht** — gegen dieselbe
Auslieferungsregel gebaut, nur ohne die sechs Einträge (`v1_ohne_doppel`):

| | Auslieferung | ohne Doppel | Differenz |
|---|---:|---:|---:|
| C4 Deckung (Texel) | 12068 | 12068 | **0** |
| C5 Deckung (Texel) | 9285 | 9285 | **0** |
| C4 Rechtecke | 105 | 105 | — |
| C4 Marke verdeckt | 426 | 426 | **0** |

Die gedeckte Fläche ist punktgenau dieselbe — jedes Bit. Und genauso punktgenau
**unwirksam**: an der Marke ändert sich kein einziger Figurpunkt. Wer die Doppel für die
Ursache gehalten hätte, hätte den Fehler ausgeliefert.

Nach der Regeländerung fällt die Maskenzahl in C4 auf **104** (C5 bleibt 105), die
Atlasfläche von 48102 auf 22536 Punkte (C4) bzw. von 40508 auf 13297 (C5), und die
Kachelkanten werden feiner (C4 `[16,24,24,12,24,24,24,24,24,24,24]` →
`[10,12,12,12,12,12,12,24]`).

---

## 3. Warum es die Zelle MIT Freistellung ist — die Messung, die alles entschied

`werkzeug/herkunft_marke.py` trägt das Etikett jedes Maskentexels (`_kunst_label` /
`_dunkel_label` wie `raum.py` sie baut) an die Marke heran:

```
MARKE: gezeichnet 837, verdeckt 426, davon ohne Nutzer-Kunst 154

Zugabe nach ETIKETT:
   Zelle 17 (x -1600 z 12200, MIT Nutzer-Kunst)  154 Punkte, Zeilen 80..113,
                                                 Regel1 146 / Regel2 8
```

**Eine einzige Zelle. Und es ist die, für die eine Freistellung existiert.** Von den acht
Stuhlzellen, die C4 bedient, trägt keine der sieben *ohne* Freistellung auch nur einen
einzigen Punkt zum Fehler bei.

Dazu die zweite Messung, die die Saum-Regel entwertet — Gegenmodell „nur die Löcher der
eigenen Kunst füllen":

| | Freistellung allein | + dunkle Löcher der Freistellung |
|---|---:|---:|
| C4 | 2131 Texel | **2131** |
| C5 | 3576 Texel | **3577** |

In C4 hat die Freistellung des Nutzers **kein einziges** dunkles Loch, in C5 genau **eines**.
Der Saum von 7 Bildpunkten hat also nie Löcher gefüllt — er hat die Kunst um ihren
Antialias-Rand in die dunkle Wand hinein aufgeweitet.

---

## 4. Was NICHT die Ursache war — vier Sackgassen, jede gemessen

Ich habe vier naheliegende Erklärungen geprüft und alle vier verworfen. Jede steht hier,
weil sie sonst in der nächsten Runde wieder auftaucht.

**(a) „Regel 2 ist die Ursache" (Vorschlag 8.1.1 aus Untersuchung B) — widerlegt.**
Gegenmodell mit abgeschalteter Regel 2 und beibehaltenem Saum: **418 statt 426** an der
Marke, 5 Bilder noch über der Hälfte. Umgekehrt (Regel 1 aus, Regel 2 an): **426**,
also gar keine Änderung — Regel 2 fängt dieselben Bildpunkte ohnehin ein. B hatte die
Zahl selbst in der Tabelle stehen (§2, „Lasso + Tiefschwarz-Saum ≤ 7 → 49,9 %"), im
Vorschlagstext aber die Wirkung des Nullmodells („nur Lasso") zugeschrieben.

**(b) „Die Quaderhöhe −1950 ist zu hoch" — teilweise richtig, als Fix zu schwach.**
Der Verdacht war begründet: die Höhe stammt laut `auswahl.json` `_warum` aus einer Messung
„an den Schwarz-Säulen aufgelöst", also an genau dem Tiefschwarz, das die Regel danach als
Stuhl führt — ein Zirkelschluss. Zwei unabhängige Messungen an der **Kunst**:

```
kleinste Hoehe, die die Freistellung ganz enthaelt:  Z17 -1675  Z18 -1550  Z20 -1375  Z21 -1450
Stuhloberkante ueber der Zellmitte (Projektion):     -1800 / -1765 / -1775 / -1760 / -1640 / -1795
```
(`messung/hoehe_aus_kunst.txt`, `messung/stuhlhoehe.txt`; die zweite Messung eicht sich
selbst an Höhe 0 gegen die unterste Kunstzeile und besteht diese Eichung bei 1 von 4
Stühlen auf ±3 Bildzeilen — die anderen drei stehen außermittig in ihrer 1500er-Zelle.)

Die Höhe erklärt genau den waagerechten Schnitt: bei −1950 reicht die Silhouette von
Zelle 17 bis Bildzeile **80**, bei −1675 erst bis **85** — und die Zeilen 80..84 sind
der Hüftschnitt. Als Fix reicht sie trotzdem nicht: mit der gemessenen Höhe bleiben
**356 von 837** verdeckt (42,5 %) statt 272. Sie ist deshalb **nicht** geändert worden.

**(c) „Der Deckel des Quaders ist schuld" — widerlegt.** `geom.quader_tiefe` setzt jede
Zelle als Quader **mit Deckfläche** an. Eine 1500×1500 große waagerechte Platte 1950
Einheiten über dem Boden hat ein Bürostuhl nicht. Gemessen (`messung/deckel.txt`, das
Werkzeug eicht sich vorher gegen `geom.quader_tiefe` über alle Typ-3-Zellen): von den
154 falschen Punkten gehen **57 (37 %)** auf Deckel-Bildpunkte zurück. Ein Drittel ist
keine Ursache.

**(d) „Die Freistellung ganz allein reicht" — visuell widerlegt.** Das Nullmodell „nur
Lasso" trifft an der Marke exakt (272), kostet aber in C4 **9937** und in C5 **5709**
Texel. `04_maskenvergleich_c4.png`/`05_maskenvergleich_c5.png` zeigen, was dann fehlt:
das linke Pult mit Monitoren und Tastaturen, die ganze Konsolenbank im Hintergrund, das
rechte Pult mit den Ordnern, die schwarzen Stühle. Keines dieser Möbel hat eine eigene
Freistellung; ihre Maske kommt ausschließlich aus Regel 2. Sie ersatzlos zu streichen
hieße, den Befund vom 2026-09-09 („ganz viele Stühle decken mich nicht") wieder
aufzumachen.

---

## 5. Die Änderung

### 5.1 `re15_port/tools/maske/raum.py`

```python
# 1) EIGENE LOECHER statt Saum:
for _ki in sorted(set(int(v) for v in _kunst_label[_kunst_label >= 0])):
    _eigen = _kunst_label == _ki
    _loch  = _nd.binary_fill_holes(_eigen) & ~_eigen
    _dunkel_label[_dk & _sil[_ki][1] & _loch] = _ki
# 2) naechste enthaltende Zelle — nur noch OHNE Freistellung:
for _ki, (_vzq, _trq) in _sil.items():
    if (_kunst_label == _ki).any():
        continue
    ...
```

Beide Regeln wirken ausschließlich über `nur_kunst` und `tiefe:"szene"`.
`grep -n "_dunkel_label" re15_port/tools/maske/*.py` → `raum.py:480/482` (`nur_kunst`)
und `raum.py:599` (`szene`); und ein Lauf über die ganze `auswahl.json` findet diese
beiden Schlüssel **nur** in ROOM10F0 C4 und C5. **Die Änderung kann keinen anderen
Winkel des Spiels berühren.** Das schließt die größte offene Frage aus Untersuchung B
(„ihre Wirkung in den anderen 97 Cuts habe ich NICHT gemessen") — sie hat dort keine.

### 5.2 `P2_UNANGETASTET` → `P2_ALTE_KETTE`

Die Begründung des Eintrags war „vom Nutzer abgenommen 2026-09-09". Diese Abnahme ist
durch die Marke hinfällig. `bau_p2` kann die beiden Cuts nicht bauen (es kennt weder
`tiefe:"szene"` noch `nur_kunst`), also laufen sie weiter über die alte Kette — aber sie
werden bei jedem Lauf **neu geschrieben**, damit eine Regeländerung überhaupt ankommt.
Vor dem Schreiben prüft `raum.alte_kette` dreifach:

1. **TREUE** — die fertige Maske deckt die gewollte Fläche punktgenau (fehlt 0, zuviel 0).
2. **BELEGT** — kein Texel liegt in der Silhouette einer Zelle mit Freistellung, aber
   außerhalb dieser Freistellung (das neue Maß, §6).
3. Der Bau läuft in ein Nebenverzeichnis; erst nach bestandener Prüfung wird kopiert.
   (Ohne das schrieb `bau_objektweise` das TIM schon vor der Prüfung — beim ersten Lauf
   stand dadurch kurz ein neues TIM neben einer abgelehnten Sektion.)

Punkt 2 hat sich sofort bewährt: im ersten Anlauf war das BELEGT-Feld ohne die Löcher der
eigenen Kunst definiert, C5 hatte genau **1** unbelegten Texel, und der Cut wurde
abgelehnt statt ausgeliefert.

### 5.3 Neue Beiblätter

`raum.alte_kette` schreibt jetzt `.PBM` (Ziel-Silhouette), `.STAND` (Standlinie je
Bildspalte) und `.BELEG`. Damit fallen ROOM10F0 C4/C5 erstmals unter
`test_pri_silhouette` (**77 → 79 PBM-Cuts, alle bitgenau**) und unter
`test_pri_kopfschnitt` (VORverd 0 in beiden Cuts) — Nachtrag 6.4 aus Untersuchung B, zwei
der dort genannten 22 riegellosen Cuts sind erledigt.
Die Standlinie kommt **exakt** aus `geom.quader_tiefe` (alle Objekte dieser Cuts sind
quaderbasiert), nicht aus der gerasterten Tiefe zurückgerechnet; hat ein Objekt keinen
Quader, wird der Cut abgelehnt statt mit einer genäherten Standlinie ausgeliefert.

### 5.4 Was NICHT angefasst wurde

Nur die Cuts 4 und 5. Nachgewiesen (`messung/kollateral_altes_werkzeug.txt`):

```
Cut | Sektion ausgeliefert/neu | gleich
  0 |  1724  1724 |  True
  1 |   724   724 |  True
  2 |   544   544 |  True
  3 |   704   704 |  True
  4 |  2104  2084 | False
  5 |  2104  2104 | False
  6 |  2044  2044 |  True
```

⛔ **Nebenbefund, der in kein Dossier gehört und trotzdem wichtig ist:** baut man
ROOM10F0 mit dem **unveränderten** Werkzeug komplett neu, kommen die Cuts **1, 2 und 3**
anders heraus als ausgeliefert (724→824, 544→684, 704→1004 Bytes). Das ist ein
vorhandener Auseinanderlauf zwischen Auslieferung und heutigem Werkzeug, **nicht** von
dieser Runde verursacht — die Kontrollmessung lief mit `git show HEAD:raum.py`. Deshalb
wurde hier cut-weise gebaut (`raum.py ROOM10F0 4`, dann `5`) und nicht der ganze Raum.
Siehe §9.

---

## 6. Das Erkennungsmaß, das der Abnahme gefehlt hat

Die Aufgabe verlangt ein Maß für „die Figur darf nicht von Masken verdeckt werden, ohne
dass dort wirklich ein Gegenstand steht". Der Schiedsrichter für „dort steht wirklich
etwas" muss unabhängig von der Regel sein, die er beschränken soll. Das stärkste
verfügbare Zeugnis ist die **Freistellung des Nutzers** — sie ist in diesem Projekt
ausdrücklich die Wahrheit über die Form eines Gegenstands (`raum.py:412`).

> **UNBELEGTE VERDECKUNG.**
> Ein Maskentexel heißt **belegt**, wenn er in einer Freistellung des Nutzers dieses
> Winkels (oder in einem Loch davon) liegt **oder** außerhalb jeder Quader-Silhouette
> einer Zelle, für die eine Freistellung vorliegt. Sonst **unbelegt**: er liegt in der
> Silhouette einer Zelle, deren Aussehen BEKANNT ist, und gehört nicht dazu.
> Ein Figurpunkt ist **unbelegt verdeckt**, wenn ihn ein unbelegter Texel nach dem
> Original-Urteil verdeckt.
> **Schranke: 0.**

Kein freier Parameter. Die Schranke heißt nicht „höchstens x %", sondern „gar nicht" —
und genau deshalb ist sie nicht an den Nutzerfall angepasst. (Eine Prozentschranke hätte
ich an seinen 50,9 % geeicht; das wäre eine selbstbestätigende Metrik gewesen.)

Das BELEGT-Feld hängt **nur** an der Freistellung und an der Raumgeometrie, nicht an den
Dunkel-Regeln — es liegt als `ROOM####_PRI##.BELEG` (P4-PBM) neben der Maske.

### Abdeckung — ehrlich

* **2 von 439 Winkeln mit Masken (0,46 %).** Das klingt wenig und ist trotzdem
  vollständig: es sind genau die Winkel, in denen die Dunkel-Regeln überhaupt wirken
  (§5.1). In den übrigen 437 gibt es keine Zelle mit Freistellung, deren Silhouette
  etwas beisteuern könnte — das Maß wäre dort leer, nicht bestanden.
* Je Winkel **alle begehbaren Standplätze des Raums** im 200er-Raster über den
  Klemmpfad `re15_collision_constrain` (C4 2788, davon 1287 in der Anker-Zone;
  C5 4441 / 2301), die Anker-Zone getrennt gezählt.
* Dazu **jedes protokollierte Bild** aus `befund.log` (30 in C4, 20 in C5) und die
  Nutzer-Marke F335 selbst — der einzige Teil ohne Standplatz-Ersatz.
* Fehlt das Beiblatt `.BELEG`, schlägt der Riegel an. Kein stilles Grün.
* **Was das Maß NICHT kann:** es sagt nichts über zu WENIG Verdeckung. Dafür bleibt
  `test_pri_kopfschnitt` mit seinem HINTER-Kriterium zuständig; in dessen Tabelle stehen
  die beiden Cuts jetzt erstmals überhaupt (§9 Punkt 2).

---

## 7. Riegel und Gegenprobe

`re15_port/tests/unit/probes/r22_10f0.cmake` registriert `unit_r22_10f0_figur`
(`probe_r22_10f0_figur riegel <pfaddatei>`).

**Gegenprobe am Auslieferungsstand vom 2026-09-21** — Maskenbaum über `R22_MASKEN_DIR`,
BELEGT-Feld unverändert (`messung/riegel_gegenprobe_auslieferung.txt`):

```
BELEG cut=4 ... unbelegte_texel=3541
RIEGEL MARKE cut=4 F335 punkte=837 unbelegt_verdeckt=154
RIEGEL PFAD  cut=4 bilder=30 mit_unbelegter_verdeckung=17 punkte=2213
RIEGEL STAND cut=4 plaetze=2788 davon_ankerzone=1287 mit_unbelegter_verdeckung=158 punkte=5480
BELEG cut=5 ... unbelegte_texel=3080
RIEGEL PFAD  cut=5 bilder=20 mit_unbelegter_verdeckung=11 punkte=560
RIEGEL STAND cut=5 plaetze=4441 davon_ankerzone=2301 mit_unbelegter_verdeckung=244 punkte=14107
RIEGEL r22_10f0: GEFALLEN                                            EXIT 1
```

**Nach dem Umbau** (`messung/riegel_nachher.txt`):

```
BELEG cut=4 ... unbelegte_texel=0
RIEGEL MARKE cut=4 F335 punkte=837 unbelegt_verdeckt=0
RIEGEL PFAD  cut=4 bilder=30 mit_unbelegter_verdeckung=0 punkte=0
RIEGEL STAND cut=4 plaetze=2788 davon_ankerzone=1287 mit_unbelegter_verdeckung=0 punkte=0
BELEG cut=5 ... unbelegte_texel=0
RIEGEL PFAD  cut=5 bilder=20 mit_unbelegter_verdeckung=0 punkte=0
RIEGEL STAND cut=5 plaetze=4441 davon_ankerzone=2301 mit_unbelegter_verdeckung=0 punkte=0
RIEGEL r22_10f0: GEHALTEN                                            EXIT 0
```

---

## 8. Das Bild aus dem laufenden Spiel

Lauf: `re15_pc.exe` mit `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_DEBUG_JUMP=10F0@gp
RE15_PLAYER_POS=-448,14087,13040 RE15_FORCE_CUT=4 RE15_BEFUND_MARKE=1
RE15_FRAMEDUMP=20-400/10:...` — beschleunigter Renderer, **kein** `RE15_AUTOSHOT`,
**kein** `RE15_SOFTWARE_RENDER`; das Bild ist der Vollbild-Readback unmittelbar vor
`SDL_RenderPresent`. Beide Läufe unterscheiden sich in genau einer Sache: dem Inhalt von
`MASKS/ROOM10F0.MSK` + `_PRI04.TIM`.

Dass es wirklich der Standpunkt der Marke ist, steht im `befund.log` des Laufs — Zeile
für Zeile identisch mit der des Nutzers:

```
F14  R10F0 C4  pos=(-448,0,14087) rot=13040  vz=11118/10876/10634  kasten=x99..121,y81..118
     vorher  masken=105 | [1:(92,80)16x16 t=154 DECKT] [2:(108,80)8x16 t=160 DECKT] ...
     nachher masken=104 | [0:(98,85)10x10 t=139 DECKT] [1:(108,85)5x10 t=139 DECKT] ...
```

Bilder: `01_spiel_vorher.png`, `02_spiel_nachher.png`, Ausschnitt 9×
`03_marke_vorher_nachher.png`.

**Was man sieht — selbst angesehen.**
*Vorher:* Leons Oberkörper mit der R.P.D.-Jacke steht vor der hellen Tapete, und etwa in
Gürtelhöhe hört er einfach auf. Dort beginnt die dunkle Holzvertäfelung, und sie läuft
ungebrochen durch ihn hindurch; rechts neben der weißen Stuhllehne, wo sein Bein sein
müsste, sieht man Wand und Bürostuhl. Zwischen Gürtel und Lehnenoberkante klafft eine
Lücke, in der nichts von ihm übrig ist.
*Nachher:* Der Gürtel ist da, die Hose ist da, seine rechte Hand hängt an der Hüfte, und
sein rechtes Bein läuft neben der Stuhllehne bis zum Boden durch. Die Lehne deckt seine
Beine — das tut sie vorher wie nachher, und das ist richtig, sie steht wirklich vor ihm.
Der waagerechte Schnitt ist weg.

`06_modell_vorher.png` / `07_modell_nachher.png` zeigen dieselbe Stelle als Messbild:
blau = frei, orange = durch die Freistellung verdeckt (richtig), rot = durch Tiefschwarz
verdeckt (falsch). Das rote Band quer über der Hüfte verschwindet vollständig.

---

## 9. Offen

1. **ROOM10F0 C1/C2/C3 laufen mit dem heutigen Werkzeug auseinander** (§5.4). Nicht von
   dieser Runde verursacht, aber ungeklärt: entweder ist die Auslieferung älter als eine
   Änderung in `geometrie.py`/`bau_p2.py`, oder eine Eingangsgröße hat sich verschoben.
   Solange das offen ist, darf ROOM10F0 nur cut-weise gebaut werden.
2. **Die HINTER-Seite dieser beiden Cuts ist jetzt sichtbar und unschön.**
   `test_pri_kopfschnitt` meldet für C4 `HINTER 981, frei 917` und für C5 `729, 215` —
   an den meisten Standplätzen hinter der Standlinie wird die Figur also **nicht**
   verdeckt. Der Test prüft nur `VORverd == 0` und ist deshalb grün. Ob das ein echter
   Mangel ist oder ein Artefakt der Standlinie (sie kommt hier aus Quadern, die als
   Stellvertreter für Stühle stehen), habe ich **nicht** gemessen.
3. **Die Quaderhöhe −1950 steht weiter im Baum**, obwohl ihr Beleg zirkulär ist und zwei
   unabhängige Messungen auf −1640…−1800 zeigen (§4b). Sie zu ändern war für diesen
   Befund nicht nötig und hätte die Maske der Pulte weiter verkleinert (Berührung an
   begehbaren Standplätzen C4 153 → 77). Wer sie anfasst, muss beides zusammen messen.
4. **Die übrigen 20 riegellosen Cuts** (ROOM1100 C1/C2, ROOM2090 C0–C9, ROOM2091 C0–C7)
   haben weiterhin weder `.PBM` noch `.STAND`.
5. **Die Pose an der Marke ist nicht die des Nutzers.** Die Sonde posiert Keyframe 0; über
   sieben Keyframes schwankt der Anteil vorher zwischen 45,5 und 51,3 % (Untersuchung B
   §0.3). Der Befund ist posen-robust, nicht posen-identisch.
6. **Das linke Pult in C4 und die linke Pultreihe in C5 verlieren ihre Maske.**
   Das ist der Preis der Regel: dieser Bereich lag in der Silhouette der Zellen 18/19 bzw.
   20, für die Freistellungen existieren. Über die begehbaren Standplätze gemessen kostet
   es nichts (Berührung C4 152 → 153, C5 516 → 498, Anteil 0,20 → 0,19 % bzw.
   1,03 → 1,01 %). Wenn der Nutzer dort trotzdem durch ein Pult sichtbar wird, ist die
   Abhilfe eine Freistellung für das Pult — nicht das Zurückdrehen der Regel.

---

## 10. Dateien

```
analysis/befunde_2026-09-21/10f0-umsetzung/
  01_spiel_vorher.png            Vollbild-Readback, Auslieferungsmaske
  02_spiel_nachher.png           derselbe Lauf mit der neuen Maske
  03_marke_vorher_nachher.png    Ausschnitt 9x, nebeneinander
  04_maskenvergleich_c4.png      blau = beide, rot = faellt weg (ganzer Winkel)
  05_maskenvergleich_c5.png      dasselbe fuer Cut 5
  06_modell_vorher.png           Figur: blau frei, orange Freistellung, rot Tiefschwarz
  07_modell_nachher.png          dieselbe Figur mit der neuen Maske
  08_zelle17_silhouette.png      was in der Quader-Silhouette von Zelle 17 liegt
  messung/   eichung_nachbau.txt, herkunft_marke.txt, hoehe_aus_kunst.txt,
             stuhlhoehe.txt, deckel.txt, kollateral_altes_werkzeug.txt,
             nachher_c4.txt, nachher_c5.txt,
             riegel_gegenprobe_auslieferung.txt, riegel_nachher.txt,
             auswahl_vorher.json
  werkzeug/  nachbau.py (Eichung), variante.py (Gegenmodelle als echte Assets),
             messreihe.py, marke.py, herkunft_marke.py, hoehe.py, stuhlhoehe.py,
             deckel.py, zellenbild.py, vergleichsbild.py, kollateral.py,
             auswahl_pflegen.py
re15_port/tools/maske/raum.py                     Regel 1/2, P2_ALTE_KETTE, alte_kette()
analysis/esp_masken_2026-09-03/auswahl.json       6 Doppel-Eintraege weg, _warum ergaenzt
re15_port/tests/unit/probe_r22_10f0_figur.c       R22_MASKEN_DIR, .BELEG, riegel()
re15_port/tests/unit/probes/r22_10f0.cmake        ctest unit_r22_10f0_figur
re15_port/shared_assets/PSX/MASKS/                ROOM10F0.MSK, _PRI04/_PRI05 .TIM
                                                  + neu .PBM .STAND .BELEG
```
