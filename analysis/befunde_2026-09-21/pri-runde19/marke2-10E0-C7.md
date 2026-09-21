# Runde 19 / Marke 2 — ROOM10E0 Cut 7 (Liege / Pult mit Schreibmaschine)

**Nutzer-Meldung 2026-09-21:** „Das meiste an PRIs sieht gut aus, aber es gibt ein paar PRIs
die sind zu falsch. Siehe F9 Marker."
**Marke 2:** `befund_10E0_F185_marke2.bmp`, `befund.log` Zeile 13196.
Spieler Welt (−2360, 0, −164), Bildkasten x99..133 y120..181, 103 Masken,
vz Fuss/Körper/Kopf = 6361 / 6165 / 5970.

**Ergebnis in einem Satz:** Die Maske der **Liege** war 1739 Welteinheiten **zu nah**, weil
ihre Tiefe aus dem **umschließenden Rechteck** einer **Typ-5-Diagonal-Kollisionszelle**
gerechnet wurde. Der Spieler steht in der *freien* Hälfte dieser Zelle, wird deshalb komplett
verdeckt und **in zwei Stücke zerrissen**: Oberkörper über der Matratze sichtbar, Schuhe unter
dem Gestell sichtbar, alles dazwischen gelöscht. Behoben, indem die Zelle als ihre **echte
Form** geraycastet wird — die Bedingung dafür steht byte-true im Kollisionscode des Originals.

---

## 1. Reproduktion und Befund am BILD

Der Abzug des Nutzers, auf 320×240 zurückgerechnet (Faktor 3), und der reine Hintergrund aus
dem BSS-Klotz (`probe_bg_dump` → `build/bg_ppm/ROOM10E07.ppm`):

* `marke2-10E0-C7/01_vollbild.png` — ganzer Abzug
* `marke2-10E0-C7/02_spieler_zuschnitt.png` — Zuschnitt x90..175 y110..200, 6×
* `marke2-10E0-C7/09_tafel_bg_spiel_maske.png` — Hintergrund | Spielbild | Maskensilhouette
* `marke2-10E0-C7/10_figur_verdeckt_vorher_nachher.png` — **der Kernbeleg**

Am Bild ist zu sehen: Leon steht hinter der Liege. Sein Rücken samt R.P.D.-Logo ist über der
Matratze zu sehen, seine Schuhe als dunkler Klumpen unter dem Gestell auf den Bodenfliesen —
und dazwischen **fehlt der Körper vollständig**, mit einer harten geraden Kante an der
Matratzenoberseite.

**Diff gegen das Hintergrundbild** (Schwelle 24 je Kanal): 1755 Bildpunkte unterscheiden sich
vom Hintergrund, davon **309 im Spielerkasten**. Ein aufrechter Spieler füllt in diesem Kasten
(35 × 62 = 2170 Punkte) normalerweise ein Mehrfaches davon — die 309 sind Kopf/Schultern plus
Schuhe. Bild `10_…` zeigt rot, welche Figurpunkte die Maske löscht: **1140 Punkte**, ein
zusammenhängender Block von der Matratzenkante bis unmittelbar über die Schuhe.

---

## 2. Die Ursache, belegt

### 2.1 Die Silhouette ist NICHT der Fehler

| Messung | Zahl |
|---|---|
| Ausgelieferte Deckung (pixelgenau, wie `render_pc.c` blittet) | 41 239 Punkte |
| `ROOM10E0_PRI07.PBM` (Soll) | 41 239 — **fehlt 0, zuviel 0** |
| Vereinigung der drei Nutzer-PNGs bei Alpha ≥ 110 | 41 248 |
| Deckung gegen diese Vereinigung | fehlt 9, zuviel 0 |

Die 9 Punkte sind ein Artefakt **meiner** schnellen Nachbildung der Platzierung
(2 + 5 + 2 Punkte Unterschied zu `maske_aus_png.setze`), kein Verlust.
⛔ **Die Silhouette entspricht der Freistellung des Nutzers bitgenau.** Der Fehler liegt
ausschließlich in der **TIEFE**.

### 2.2 Die Tiefe der Liege kommt aus dem Hüllrechteck einer Diagonale

`auswahl.json` → `ROOM10E0` → `"7"` → Objekt „Liege" (`pri/STAGE1/10E0/07_04.png`, x52 y97)
trägt `"tiefe": "kollision"`. Die Zuordnung wählt:

```
zelle x-3700..-850 z-1400..2300 (Typ 5, 59 % der Standpunkte): Hoehe -1775
  (IoU 0.44, Oberkante 3.0 px), Quader, 5510 von 5712 Punkten getroffen, Tiefe 66..128
```

**Die Zelle ist eine Diagonale, kein Quader.** SCA-Eintrag 21,
**`ROOM10E0.RDT` Datei-Offset `0x758`**, 12 Bytes:

```
22 0b 74 0e 8c f1 88 fa 05 ff 00 03
│    │    │    │    └─ type = 0x05  (&0x0f = 5)
│    │    │    └────── z      = 0xfa88 = -1400
│    │    └─────────── x      = 0xf18c = -3700
│    └──────────────── density= 0x0e74 =  3700
└───────────────────── width  = 0x0b22 =  2850
                       u0=0xff (solide), floor=0x03 -> Band 0
```

(Derselbe Eintrag steht fünfmal in der Sektion — Offsets `0x758`, `0x86C`, `0x980`, `0xA94`,
`0xBA8`, je ein Kamera-Satz.)

**Typ 5 ist eine „/"-Diagonale.** Die Kollisionsbedingung des Originals, mit Spielerradius
r = 0, aus `re15_port/engine/src/re15_collision.c` `push_diag5`
(**@LAB_8003c734**, `ghidra1_V2.txt:144830-145138`):

```c
int32_t LINE  = (DZR * (PX - X)) / DXR;   /* DXR = W, DZR = D */
int32_t ZTERM = PZ - (Z - r);
if (!(LINE < ZTERM)) return 0;            /* kein Treffer = FREI */
```

also solide genau dort, wo `pz > Z + (D/W)·(px − X)`. Die solide Hälfte ist das Dreieck
(X,Z) – (X,Z<sub>max</sub>) – (X<sub>max</sub>,Z<sub>max</sub>); die andere Hälfte ist **begehbar**.

`geometrie.tiefe_geometrie` (Zeile 397) holt die Zellen aus `geom.sca_sperrzellen`. Deren
eigener Docstring (`geom.py:575-578`) sagt wörtlich:

> „Kreis/Diagonale/Kapsel als ihr umschliessendes Rechteck. Fuer ERREICHBARKEIT (wo kann der
> Spieler stehen) ist das konservativ genug; **fuer TIEFEN-Raycasts weiter sca_wandzellen
> benutzen (eine Diagonale als volles Rechteck geraycastet waere zu nah)**."

Genau das ist passiert. **Der vorhergesagte Fehler, in Zahlen:**

| an der Spielerspalte px = −2360 | z |
|---|---|
| echte Diagonalfläche `Z + (D/W)(px−X)` | **+340** |
| Nahkante des Hüllrechtecks | **−1400** |
| **Differenz** | **1739 Einheiten zu nah** |
| Spieler | **−164** → **504 Einheiten VOR der echten Fläche** |

### 2.3 Drei unabhängige Zeugen für dieselbe Ursache

**(a) Die eigene Fußlinie der Liege.** Untester Silhouettenpunkt je Spalte, auf die Bodenebene
y=0 projiziert, über die Spalten x99..133: Kamera-z **6065 .. 7352** (Tiefe 94,7 .. 114,8).
Die Hüllrechteck-Tiefe dort ist 67..96 — also **näher als der eigene Bodenkontakt der Liege**.
Für einen Gegenstand, der auf dem Boden steht, ist das unmöglich.

**(b) Die Silhouette der Zellenform gegen die Freistellung des Nutzers.**

| Modell | IoU gegen `07_04.png` |
|---|---|
| Hüllrechteck | 0,441 |
| Dreiecksprisma (echte Typ-5-Form) | **0,611** |

Die echte Form beschreibt die Freistellung des Nutzers **besser**. ⛔ Nullmodell-Prüfung: das
ist kein freier Parameter, der monoton besser wird — die korrekte Form ist **kleiner** als das
Hüllrechteck, sie könnte die IoU also auch senken (kleinerer Schnitt bei kleinerer Vereinigung).
Die gemessene Höhe −1775 und der Oberkanten-Abstand 3,0 px sind in beiden Läufen **identisch**,
von derselben Routine neu gemessen.

**(c) Alle drei aktenkundigen Meldungen des Nutzers in diesem Winkel sind derselbe Fall.**

| Marke | Welt (x,z) | im Hüllrechteck? | echte Diagonale bei z | Lage |
|---|---|---|---|---|
| F4141 2026-09-07 „schnitt ihn mittendrin" | −2360, −164 | **innen** | +340 | **503 VOR** |
| F185 2026-09-21 (diese Marke) | −2360, −164 | **innen** | +340 | **503 VOR** |
| F523 2026-09-07 „Leon muss davor sein" | −1137, 1375 | **innen** | +1927 | **552 VOR** |
| F367 2026-09-08 „Zombies durch die Wand" | −1422, 485 | **innen** | +1557 | **1072 VOR** |

Der Nutzer hat in diesem Winkel **dreimal dasselbe** gemeldet, und jedes Mal wurde am
Tiefenmodell des Objekts gedreht statt an der Zellenform. Das erklärt die lange
`_warum`-Kette in `auswahl.json` („fuss 162" → „tiefe kollision" → …).

---

## 3. Frage 3 der Aufgabe: Maske 9

`[9:(110,180)40x60 t=50 z=3267 DECKT]` — **sie ist nicht der Täter, und „DECKT" im Log ist ein
Kasten-Urteil, kein Pixel-Urteil.**

**Woher sie kommt:** Sie ist eines von **zehn** Rechtecken mit Tiefe 50. Alle zehn gehören zum
Objekt „Pult mit Schreibmaschine (Nutzer-Original)" = `07_01.png` bei x=110, y=97 (210×143, also
x110..319 y97..239). Die zehn Rechtecke überdecken genau diese Spanne:
(310,99)10×141, (290,135)20×105, (270,172)20×68, (250,179)20×61, (230,154)20×86, (210,149)20×91,
(190,162)20×78, (170,163)20×77, (150,177)20×63, **(110,180)40×60**.
`z = 3267 = (50+1)·65536/1023`. Die **50** ist der feste Wert `"tiefe": 50` aus `auswahl.json`
— eine **Nutzer-Entscheidung vom 2026-09-08** („Alles wo nicht alpha transparent ist soll
ueberdecken"; später „Leons komplette Beine schauen immer noch durch, statt dass das PRI mit der
Schreibmaschine einfach alles umfassend deckt"), begründet damit, dass die Vorderseite des Pults
unerreichbar ist: die Vorderwand-Zelle klemmt den Spieler bei z = −982, die Pult-Vorderkante
liegt bei −958. 50 liegt unter dem kleinsten je gezeichneten Spieler-Dreieck (3466 = t54), deckt
also immer.

**Darf sie den Spieler decken?** Gemessen am *Pixel*, nicht am Rechteck:

| | Punkte |
|---|---|
| Kasten (110,180)40×60 ∩ Spielerkasten | 48 |
| davon im Atlas wirklich deckend | 25 |
| davon mit Tiefe 50 | **21** (x123..133, y180..181) |
| davon auf **gezeichneter Figur** | **0** |

⛔ `befund.log` prüft nur die Rechteck-Überlappung (`main.c:5307-5308`) und kennt die
Pixel-Deckung im Atlas nicht — deshalb steht dort „DECKT", obwohl die Kunst den Spieler an
dieser Stelle nicht berührt. Der Riss kommt von **t=72/75/78/84** (die Liege), zusammen
**1500 Punkte** im Kasten, auf denen **0** gezeichnete Figurpunkte übrig sind.

Welche Tiefenstufe was tut (alter Satz, im Spielerkasten):

| t | Kamera-z | Punkte im Kasten | davon auf gezeichneter Figur |
|---|---|---|---|
| 50 | 3267 | 21 | 0 |
| 72 | 4677 | 925 | **0** ← gelöscht |
| 75 | 4869 | 38 | **0** ← gelöscht |
| 78 | 5061 | 362 | **0** ← gelöscht |
| 84 | 5445 | 175 | **0** ← gelöscht |
| 90 | 5830 | 140 | 92 (ferner als die Figur, richtig frei) |
| 96 | 6214 | 140 | 97 (ferner als die Figur, richtig frei) |
| 154 | 9930 | 1 | 0 |

---

## 4. Die gelöschten Nutzer-Dateien: KEIN Inhaltsverlust

Auftrag war zu prüfen, ob beim „PRI-Aufräumen" (Commit `91284cac`, 2026-09-09) Inhalt verloren
ging. Alle Fassungen aus der Historie geholt und pixelweise gemessen:

**`07_01.png` wurde in `91284cac` MODIFIZIERT (`M`), nicht nur die Geschwister gelöscht** — und
in `692b4f10` (2026-09-19) wieder zurückgesetzt:

| Fassung | Alpha > 0 | sha1 des Alphakanals |
|---|---|---|
| `61607499` (Original des Nutzers) | 14 303 | `1f56d0d73935` |
| vor `91284cac` | 14 303 | `1f56d0d73935` |
| **`91284cac`** | **14 291** | `a1839f20f745` ← = `07_01_pult_gefixed.png` |
| vor `692b4f10` | 14 291 | `a1839f20f745` |
| `692b4f10` / **HEAD** | **14 303** | `1f56d0d73935` ← **wieder das Original** |

**Alle gelöschten Geschwister sind in der heutigen Fassung enthalten:**

| gelöschte Datei | Punkte | in HEAD nicht gedeckt |
|---|---|---|
| `07_01_pult_bereinigt.png` | 14 268 | **0** |
| `07_01_pult_gefixed.png` | 14 291 | **0** |
| `07_01a_teppich.png` | 7 700 | **0** |
| `07_01b_schreibmaschine.png` (80×76, Einbettung 56,63) | 3 853 | **0** |
| `07_01c_schrank.png` (22×133, 187,2) | 1 566 | **0** |
| `07_01d_stuhllehne.png` (27×60, 97,54) | 963 | **0** |
| `07_01e_kachelkante.png` (18×60, 110,51) | 229 | **0** |
| `07_01g_teppich_vorn.png` (171×85, 28,51) | 518 | **0** |

**`07_02.png`** (die Freistellung, in der Wand *und* Liege zusammen steckten, x52 y25) gegen die
heutige Vereinigung `07_01 ∪ 07_03 ∪ 07_04` bei Alpha ≥ 110: **27 268 Punkte, davon 14 nicht
gedeckt (0,05 %)**. Alle 14 liegen auf der Schnittlinie, an der der Nutzer die Datei in
`07_03` (Wand) und `07_04` (Liege) geteilt hat, und sind reine Antialias-Saumpunkte: ihr Alpha
liegt in `07_02` bei 111..200, in *jeder* der beiden Teildateien aber unter der Schwelle 110
(20..107) — die Teilung halbiert die Deckkraft der Saumpunkte.

⛔ **Befund: es ging kein Inhalt verloren.** Die Wiederherstellung, die der Nutzer in so einem
Fall verlangt hat, ist hier nicht nötig — sie ist bereits in `692b4f10` erfolgt. Die 14
Saumpunkte sind ein (kleiner, benannter) Nebeneffekt von `ALPHA_SCHWELLE = 110` an Teilungsnähten;
nicht behoben, weil ein Alpha-Summen-Modell an Nähten eine eigene Messung bräuchte.

---

## 5. Der Fix

`geom.py` bekommt zwei Funktionen, die die **Form** einer SCA-Zelle liefern, jede Bedingung mit
ihrer Adresse (r = 0, `re15_port/engine/src/re15_collision.c`):

| Typ | Routine | Adresse | solide, wenn |
|---|---|---|---|
| 3 Kreis | `push_circle` | @FUN_8003d6a8 | `dist((px,pz), (x+w/2, z+w/2)) ≤ w/2` |
| 4 | `push_diag4` | @LAB_8003beb0 | `pz > z_max − (D/W)(px−X0)` |
| 5 | `push_diag5` | @LAB_8003c734 | `pz > Z0 + (D/W)(px−X0)` |
| 6 | `push_diag6` | @LAB_8003cb9c | `pz < Z0 + (D/W)(px−X0)` |
| 7 | `push_diag7` | @LAB_8003c2cc | `pz < z_max − (D/W)(px−X0)` |

`geometrie.quader_auf_band` nimmt einen `typ`-Parameter: die vier Seitenflächen und der Deckel
werden auf die solide Hälfte beschnitten, und die **Schräge** bzw. der **Zylindermantel** kommt
als eigene Schnittfläche hinzu (ohne sie fände der Sehstrahl dort nichts). `hoehe_messen` und
`tiefe_zelle` geben `z[4]` durch.

**Nicht modelliert, laut gemeldet statt still:** Typ 2 (@LAB_8003d00c) ist keine Halbebene,
sondern ein Viererkonus um den Zellenmittelpunkt; Typ 8/9 (@LAB_8003d7e8/@LAB_8003d930) sind
Kapseln und im Port selbst als unverifiziert markiert. Für die bleibt es beim Hüllrechteck. In
STAGE1 tritt keine davon als Tiefenzelle auf. Ebenso bleibt der **Säulen**-Weg
(`kollisionstiefe_schnell`) rechteckig — keine der betroffenen Zellen ist eine Säule.

### 5.1 `typ == 1` ist bitgleich — gemessen, nicht behauptet

`zellen_solid_test(1, …)` gibt `None` zurück, keine neue Zeile greift. Gegenprobe: ROOM10E0 und
ROOM10F0 komplett neu gebaut, `--stufe 16 --statistik max`, und gegen den Auslieferungsstand
gehascht. Von **16 gebauten Cuts** weichen **genau 4 TIMs** ab — die vier mit Nicht-Typ-1-Zelle.
**Alle PBM bitgleich** (keine Silhouette ändert sich irgendwo). Sektionsweiser Bytevergleich der
`ROOM10E0.MSK`: Cuts 0,1,2,3,4,5,6,8,9 bitgleich, nur Cut 7 abweichend.

Die Bauparameter sind **gemessen, nicht geraten**: von 3 Statistiken reproduziert `max` 24 von 25
ausgelieferten TIMs (med/min je 23), und `out_vorher/ROOM10E0_PRI07.{TIM,PBM,STAND}` ist
**bitgleich mit dem Auslieferungsstand** — die Vorher-Seite des Vergleichs ist also wirklich der
Auslieferungsstand.

### 5.2 Wirkung

| | vorher (Hüllrechteck) | nachher (echte Form) |
|---|---|---|
| Liege IoU | 0,441 | **0,611** |
| Liege Tiefe | 66..128 | **85..128** |
| Tiefe im Spielerkasten | 67..96 | **93..108** |
| Rechtecke / Atlaspunkte | 103 / 58 213 | 105 / 55 966 |
| Soll / Deckung | 41 239, fehlt 0 zuviel 0 | 41 239, **fehlt 0 zuviel 0** |
| VOR-Standplätze / davon verdeckt | 41 / **0** | 95 / **0** |
| HINTER-Standplätze / davon frei | 2077 / 274 | 2069 / 385 |
| **Figurpunkte gelöscht (F185)** | **1140** | **0** |

Ehrlich dazu: `HINTfrei` steigt von 274 auf 385 — hinter der Liege bleiben 111 Standplätze mehr
unverdeckt. Das ist die notwendige Kehrseite einer *ferneren* Maske und in der Abnahme
ausdrücklich **kein** Riegel („eine Maske deckt nur ihre eigene Silhouette"). `VORverd`, der
Riegel, bleibt 0.

### 5.3 Die Zerreiß-Schiene (neuer Riegel)

`re15_port/tests/unit/test_r19_zerreiss_10e0c7.c`, registriert in
`re15_port/tests/unit/probes/r19_marke2-10E0-C7.cmake` (eigene Datei, GLOB).

Das Maß ist eine **geometrische Unmöglichkeit**, kein Wunschbild und keine Schwelle nach Gefühl:
die Kamera schaut von oben, die Füße sind ferner als der Kopf (6361 gegen 5970), und eine Maske
verdeckt alles, was **ferner** ist als sie. Sie kann deshalb nur ein **unteres Endstück** der
Figur nehmen, niemals ein Mittelstück. Eine **freie Zeile zwischen Fuß und der ersten
Verdeckung** heißt, dieselbe Maske behauptet oben „näher" und unten „ferner" als die Figur.
Die Schiene gibt ihre **Abdeckung** aus: 112 Figurspalten über 4 Standorte.

**Gegenprobe (Pflicht):**

```
### (1) NEUER Satz
  F185   Spalten 29 (Abdeckung), mit Verdeckung  0, ZERRISSEN  0
  F4141  Spalten 29 (Abdeckung), mit Verdeckung  0, ZERRISSEN  0
  F523   Spalten 25 (Abdeckung), mit Verdeckung  5, ZERRISSEN  0
  F367   Spalten 29 (Abdeckung), mit Verdeckung  0, ZERRISSEN  0
  PASS   -> rc=0

### (2) ALTER Satz (bitgleich mit dem Auslieferungsstand)
  F185   Spalten 29, mit Verdeckung 29, ZERRISSEN 26   (erste Spalte x=101)
  F4141  Spalten 29, mit Verdeckung 29, ZERRISSEN 26
  F523   Spalten 25, mit Verdeckung 23, ZERRISSEN 18   (erste Spalte x=157)
  F367   Spalten 29, mit Verdeckung 25, ZERRISSEN 25   (erste Spalte x=137)
  FAIL   -> rc=1
```

F523 behält 5 Spalten mit Verdeckung und **0** Risse — die Liege verdeckt dort weiter von den
Füßen aufwärts, was der Nutzer für „dahinter stehen" verlangt hat.

Zwei unabhängige Implementierungen (Python-Messschiene und der C-Riegel) liefern dieselben
Spaltenzahlen 29/29/25/29 und 0/0/5/0.

### 5.4 Testsuite

`bash re15_port/tools/local_build.sh test` im eigenen Baum `re15_port/build_r19_marke21007`:
**331/331 bestanden** (330 vorher + der neue Riegel), 155,7 s.
Darunter grün: `unit_pri_hashes` (Freistellungen unverändert), `unit_pri_silhouette`
(PBM gegen Sektion bitgenau), `unit_pri_kopfschnitt`, `unit_pri_eingemessen`,
`integration_pri_masken`, `unit_r19_zerreiss_10e0c7`.

---

## 6. Abdeckung: wie weit reicht der Befund?

Zensus über **alle** Objekte in `auswahl.json`, mit Abdeckungszahl:

| | Anzahl |
|---|---|
| Objekte in `auswahl.json` | 146 |
| von der Schiene gesehen (Silhouette + Kamera ok, Zellenpfad) | 113 |
| Tiefe tatsächlich aus einer Zelle | 82 |
| **Zelle NICHT Typ 1** | **7** |

| Raum/Cut | Objekt | Typ | Zelle | Anteil | IoU vorher → nachher |
|---|---|---|---|---|---|
| ROOM10E0 C7 | Liege | **5** | x−3700..−850 z−1400..2300 | 0,59 | 0,44 → **0,61** |
| ROOM10F0 C1 | Nutzer-Lasso | 3 | x6800..8300 z3800..5300 | 1,00 | 0,31 → **0,38** |
| ROOM10F0 C2 | Nutzer-Lasso | 3 | x6600..8100 z7500..9000 | 0,86 | 0,29 → **0,34** |
| ROOM10F0 C3 | Nutzer-Lasso | 3 | x1550..3050 z11650..13150 | 0,91 | 0,29 → **0,34** |
| ROOM10F0 C4 | Bürostuhl (1×) | 3 | — | — | Cut nicht neu gebaut |
| ROOM10F0 C5 | Bürostuhl (2×) | 3 | — | — | Cut nicht neu gebaut |

⛔ **ROOM10F0 ist NICHT eingebaut, und zwar aus einem gemessenen Grund.** Der ausgelieferte
`ROOM10F0.MSK` trägt Sektionen für die Cuts **0,1,2,3,4,5,6** (Cut 4 und 5 mit je 105 Masken —
die vom Nutzer am 2026-09-09 abgenommenen Bürostuhl-Quader). `raum.py` hält diese Cuts für
„UNANGETASTET … hat auch bisher keine Sektion" und erzeugt einen Container mit nur
**0,1,2,3,6**. Ein Einbau hätte **C4 und C5 gelöscht**. Das ist ein eigener, hier nur gemeldeter
Defekt in der `alt`-Erkennung von `raum.py`; die 10F0-Tiefen bleiben bis dahin, wie sie sind.
(Außerdem tragen die ausgelieferten 10F0-Sektionen C1/C2/C3 36/27/35 Masken, der Neubau 41/34/50
— sie stammen also ohnehin aus einem anderen Bau-Stand.)

---

## 7. Was diese Runde WIDERLEGT hat

1. ⛔ **„Die Rechtecke sind grobe Bounding-Boxen, die Kunst geht verloren."** Falsch. Die zehn
   t=50-Rechtecke sehen im Log wie 20-px-Spalten aus, tragen aber **rechteck-eigene
   Pixel-Deckung** im Atlas. Gemessen: Deckung == PBM == Nutzer-PNG, **fehlt 0 / zuviel 0**.
2. ⛔ **„Maske 9 deckt den Spieler."** Falsch. Ihr Rechteck berührt seinen Kasten in 48 Punkten,
   davon decken 25 wirklich, 21 mit t=50 — und **0** liegen auf gezeichneter Figur. „DECKT" im
   `befund.log` ist ein **Kasten**-Urteil (`main.c:5307`), kein Pixel-Urteil.
3. ⛔ **„`ALPHA_SCHWELLE = 110` ist die Ursache."** Falsch. Sie verwirft 529 Saumpunkte von
   41 768 (1,3 %), verteilt über den ganzen Rand. Der Riss ist ein 1140 Punkte großer Block
   mitten in der Figur.
4. ⛔ **„Beim PRI-Aufräumen ging Inhalt des Nutzers verloren."** Falsch — messbar 0 Punkte aus
   allen acht gelöschten `07_01*`-Geschwistern fehlen, und die Änderung an `07_01.png` selbst
   ist längst zurückgenommen (HEAD = `61607499`, sha1 `1f56d0d73935`). Aus `07_02.png` fehlen 14
   Antialias-Saumpunkte (0,05 %) an der Teilungsnaht, jeder einzeln mit Alphawert belegt.
5. ⛔ **„Die feste Tiefe 50 des Pults ist zu nah."** Nein — sie ist eine belegte
   Nutzer-Entscheidung, und sie berührt in diesem Bild **keinen** gezeichneten Figurpunkt.
6. ⛔ **„ROOM10E0 C7 passt nicht in den 256×256-Atlas"** (Stand des Phase-2-Berichts in
   `692b4f10`). Nicht mehr zutreffend: HEAD baut den Cut mit 103 Rechtecken / 58 213
   Atlaspunkten und schreibt ihn — `out_vorher` ist bitgleich mit der ausgelieferten Datei.
7. ⛔ **Der Fehler war nie „ein Justagefehler des Tiefenmodells".** Drei Runden lang wurde am
   Modell der Liege gedreht (`fuss 162` → `tiefe: kollision` → …), während die Ursache in der
   **Form der Zelle** lag. Die Warnung dazu stand die ganze Zeit im Docstring der Funktion, die
   die Zellen liefert.

## 8. Offen / nicht gemacht

* **Live-Abzug aus dem laufenden Port** ist mir hier **nicht** gelungen: `re15_pc.exe` liefert in
  dieser Sitzung weder über `--headless` noch über `RE15_FRAMEDUMP` Ausgabe (0 Byte, Zeitablauf).
  Die Bilder in `marke2-10E0-C7/` sind deshalb **gerechnet** — auf Grundlage des Abzugs des
  Nutzers, des Hintergrundbilds und der beiden Maskensätze — und als solche benannt. Kein
  Spielabzug ist nachgestellt. Eine Sichtprobe im echten Fenster fehlt und sollte nachgeholt
  werden.
* **`raum.py` verliert Sektionen** von Cuts, die in `P2_UNANGETASTET` stehen, aber im
  ausgelieferten Container eine Sektion haben (ROOM10F0 C4/C5). Eigener Defekt, hier nur gemeldet.
* **Typ 2 / 8 / 9** bleiben im Tiefen-Raycast Hüllrechtecke (Begründung in §5).
* Die **14 Saumpunkte** an der `07_02`-Teilungsnaht.
