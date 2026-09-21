# Runde 19b, Marke 4 — ROOM10D0 Cut 7, „es fehlt noch minimal was von den Rahmen"

Nutzer, 2026-09-21, zuerst: *„Gerade der letzte zeigt jetzt fehlende Stuecke vom Stuhl auf der
rechten Seite. Gerade da bin ich mir SICHER das ich den Stuhl in meiner vorherigen PRI
Implementierung richtig ausgeschnitten habe...."* — und dann, nachdem ich ihm seine eigene
Freistellung rot ueber den Hintergrund gelegt habe: *„Na, der Stuhl ist doch fast korrekt
erkannt, aber es fehlt noch minimal was von den Rahmen"*.

**Ergebnis in einem Satz.** Die Tiefen-Deutung, mit der ich in diese Runde gegangen bin, traegt
nicht — sie beschreibt ein Modell, das dieses Objekt nicht benutzt; und das, was im Bild an
Marke F3843 wirklich fehlt, sind **drei Gegenstaende HINTER dem Spieler**, die er zu Recht
uebermalt. Uebrig bleiben **12 Bildpunkte**, die ich nicht zuordnen kann, ohne zu raten. Ich
habe deshalb **keine Datei geaendert** und stattdessen die Modellklasse in einem Riegel
festgenagelt, damit die beiden widerlegten Erklaerungen nicht wiederkommen.

| | |
|---|---|
| Bild | `re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp`, 960x720 (Vollbild-Readback `SDL_RenderReadPixels` des beschleunigten Renderers, `render_pc.c:1984`, ausgeloest in `main.c:5287` — kein AUTOSHOT-Downscale, kein Softwarerenderer) |
| Protokoll | `befund.log` ab Zeile 13944 |
| Marke | F3843, ROOM10D0 Cut 7, `pos (418,0,26497)`, `rot -2361`, `vz 6073/5883/5692`, `scr (77,179)`, `kasten x59..95 y118..182`, `masken=104` |
| Hintergrund | `build/bg_ppm/ROOM10D07.ppm`, selbst erzeugt mit `build_r19b/tests/unit/probe_bg_dump.exe`; md5 `9890da5a1457d5f647ca73563be6cdb4` |
| Messskripte | `analysis/befunde_2026-09-21/pri-runde19b-marke4/protokoll/mess_01..18` |

---

## 1. Auftrag 1 — traegt die Deutung? NEIN, und zwar aus zwei Gruenden

Die Deutung des Auftrags lautete: *in den Spalten 69..72 enthaelt die Stuhl-Silhouette nur eine
Diagonalstrebe hoch im Bild; die Spaltenregel liest deren unterste Zeile als Bodenkontakt und
setzt diese Spalten dadurch viel zu weit weg.*

### 1.1 Der BILDBEFUND stimmt — ich habe selbst hingesehen

Bilder `18_querstrebe_y190.png` (34-fach) und `02_x55_86_y165_206_mit_freistellung.png` (24-fach).
Gemessene Helligkeiten (r+g+b) im Fenster x62..80 / y176..197, `mess_03`/`mess_08`-Tabellen:

```
y176  x64=0     y180  x66=5  x67=0     y184  x68=1  x69=5     y188  x70=3  x71=1
y177  x65=0     y181  x67=0            y185  x69=2            y189  x70=9  x71=1  x72=68
y178  x65=0     y182  x67=2  x68=0     y186  x69=9  x70=1     y190  x70=0  x71=2
y179  x66=0     y183  x68=9            y187  x70=0            y191  x69=0  x70=17
```

Ein dunkles Rohr laeuft diagonal von (64,176) nach (71,189), knickt dort und laeuft als
**Querstrebe** nach links (y190..192, x62..70, dunkel 0..17 gegen Boden 80..116). **Unter Zeile
192 ist in den Spalten 69..72 nichts mehr** als Boden (88/100/113/110/116). Die Spaltenbelegung
der Freistellung bestaetigt das punktgenau (`mess_01`):

| Spalte | 66 | 67 | 68 | **69** | **70** | **71** | **72** | 73 |
|---|---|---|---|---|---|---|---|---|
| gedeckt y | 176..236 | 178..234 | 180..232 | **182..192** | **184..191** | **186..190** | **189..189** | — |
| Punkte | 27 | 24 | 20 | **11** | **8** | **5** | **1** | 0 |

Und die vier im Auftrag genannten Spruenge des Bodenpunkts sind exakt reproduziert:
7→8 `227→216`, 9→10 `214→232`, 60→61 `213→238`, 68→69 `232→192`.

### 1.2 Die FOLGERUNG stimmt nicht — dieses Objekt benutzt die Spaltenregel gar nicht

`analysis/esp_masken_2026-09-03/auswahl.json`, ROOM10D0 „7", Objekt „Stuhl":
`{"png": "pri/STAGE1/10D0/07_01.png", "x":0, "y":123, "aufrecht": true}`.

`geometrie.standpunkte` (`re15_port/tools/maske/geometrie.py:122-139`) kehrt fuer
`fuss`/`aufrecht` **vor** der Spaltenschleife zurueck: es gibt **einen** Standpunkt (tiefster
Silhouettenpunkt in der Schwerpunktspalte) fuer alle Spalten. Die unterste Zeile einer Spalte
wird in diesem Zweig **nie gelesen**, und `bodenkante` wird erst in Zeile 141 ausgewertet — also
nach dem Return. **`bodenkante` ist auf diesem Objekt ein Nulleingriff.**

Gemessen an den AUSGELIEFERTEN Daten, gelesen ueber `re15_pri_msk_section_offset` →
`re15_pri_parse_section` → `re15_tim_parse` → Blit (`mess_02`):

```
Spalte 69: 57@182 57@183 57@184 57@185 57@186 57@187 57@188 57@189 58@190 58@191 58@192
Spalte 70: 57@184 57@185 57@186 57@187 57@188 57@189 58@190 58@191
Spalte 71: 57@186 57@187 57@188 57@189 58@190
Spalte 72: 57@189
```

Die Spalten 69..72 tragen **57/58** — genau die Tiefe ihrer Zeilennachbarn 66..68. Sie sind
**nicht** zu weit weg. Und die Gegenprobe, dass ich das richtige Modell vor mir habe: mein
Nachbau von `aufrecht: true` stimmt an **2677 von 2677** Punkten mit dem ausgelieferten
`ROOM10D0_PRI07` ueberein — 100,0 % (`mess_04`).

Was die Spaltenregel TAETE, wenn sie eingeschaltet waere (`mess_04`):

| Spalte | ybot | ausgeliefert (`aufrecht`) | Spaltenregel | Spaltenregel + `bodenkante 1..68` |
|---|---|---|---|---|
| 68 | 232 | 57..61 | 59..64 | 59..64 |
| **69** | 192 | **57..58** | **83..84** | 60 |
| **70** | 191 | **57..58** | **84..85** | 60 |
| **71** | 190 | **57..58** | **85..86** | 60 |
| **72** | 189 | **57..57** | **86..86** | 60 |

Der Weltsprung 68→69 betraegt **1451** Einheiten (Bodenpunkt (-1424,25513) → (-204,26299)),
der groesste Sprung innerhalb der Spalten 1..68 ist **775** (60→61). Das ist die Messung, auf
der die Deutung ruhte — sie ist richtig, sie beschreibt nur ein Modell, das hier nicht laeuft.

---

## 2. Auftrag 2 — was richtet die „falsche Tiefe" im Bild an? NICHTS

Es gibt keine falsche Tiefe, also ist die Antwort eine Null — und die habe ich gemessen, nicht
behauptet.

**An der Marke F3843.** Die Figur-Kamera-z je Bildzeile auf der Senkrechten durch (418,26497),
und daraus der Eimer `(1023*vz)>>16` (`mess_04`):

| Bildzeile | 120 | 130 | 150 | 179 | 190 | 200 |
|---|---|---|---|---|---|---|
| Figur vz | 5604 | 5675 | 5824 | 6054 | 6145 | 6232 |
| Figur-Eimer | 87 | 88 | 90 | **94** | 95 | 97 |

Die Maske verdeckt gdw. `Tiefe < Eimer` (`re15_pri.h`: Masken-OT = `depth*1` @0x80039650-64,
Figur-OT = `otz>>4` @0x8002565c, ZSF3 = 341 @0x80066c70). 57 < 94 **und** 85 < 94 — beide Modelle
verdecken hier gleich. Urteilswechsel zwischen `aufrecht` und Spaltenregel an dieser Marke:
**0 Figurpunkte**; zwischen `aufrecht` und Spaltenregel+`bodenkante`: **0**.

**Ueber ALLE begehbaren Standplaetze** dieses Winkels (`mess_05`; Standplaetze aus
`probe_p2_floor_dump`, Herkunftsmarke `# QUELLE klemmpfad`, 200er-Raster, ROOM10D0 Band 0 =
43147 Punkte, davon 6841 in diesem Winkel projizierbar):

| Koerperkasten | Standplaetze | von der Maske beruehrt | verdeckte Kastenpunkte `aufrecht` / Spaltenregel / +`bodenkante` | Plaetze mit abweichendem Urteil |
|---|---|---|---|---|
| KOPF=1500 | 6841 | 49 | **6912** / 6431 / 6612 | 8 (Spaltenregel) / **1** (`bodenkante`) |
| KOPF=3000 | 6587 | 101 | **7096** / 6488 / 6669 | 8 / 1 |

Beide Alternativen verdecken **weniger**, nicht mehr. Der einzige Platz, an dem `bodenkante`
etwas aendert, ist (-900, 25750): 416 → 116 verdeckte Kastenpunkte. Der Nutzer meldet, dass
etwas **fehlt** — `bodenkante` wuerde in dieselbe Richtung weiter gehen.

**Auftrag 3 ist damit beantwortet: der `bodenkante`-Mechanismus wird hier NICHT eingesetzt.**
Nicht, weil er nicht eingerichtet werden koennte, sondern weil kein Defekt vorliegt, den er
behebt, und weil er den einen Platz, an dem er wirkt, messbar verschlechtert. Zu den vier
Spruengen einzeln:

| Sprung | Weltdistanz | gehoert er dazu? | Begruendung mit Zahlen |
|---|---|---|---|
| 7→8 | 381 | nein | Kein Ausreisser: die Schrittverteilung innerhalb 1..68 hat ihren Grossteil unter 120, aber 381 ist kleiner als 641 (9→10) und 775 (60→61) — beides Spalten mit echten Beinen. Das Kriterium waere willkuerlich. |
| 9→10 | 641 | nein | dito; Spalte 10 hat ihren Bodenpunkt auf y232, also tief im Bild — ein Bein, kein Strebenende. |
| 60→61 | 775 | nein | Spalte 61 hat ihren Bodenpunkt auf **y238** (der tiefsten Zeile der ganzen Silhouette) — das ist der klarste Bodenkontakt des Objekts, nicht sein Fehlen. |
| **68→69** | **1451** | ja, aber ohne Folgen | Der einzige Sprung, hinter dem wirklich kein bodenberuehrendes Bein steht (§1.1). Er waere der Grund fuer `bodenkante 1..68` — nur liest ihn niemand, weil das Objekt `aufrecht` ist. |

---

## 3. Auftrag 4 — was fehlt WIRKLICH? Drei Gegenstaende hinter dem Spieler und 12 Punkte

### 3.1 Was im Abzug verloren ist

Bild `30_marke_erklaert.png` (Hintergrund gegen echten Abzug, 14-fach, mit den vier
Gegenstaenden beringt) und `13_ganz_bg_vs_render.png`.

Selbstpruefung zuerst, damit klar ist, dass ich den Abzug richtig lese (`mess_18`):
**5435** gedeckte Punkte, davon weichen **14 (0,26 %)** vom Hintergrund ab. Wo eine Maske deckt,
stellt der Zeichner den Hintergrund also wieder her — der Renderer ist an dieser Marke sauber.
Von 2800 uebermalten Punkten liegen 2786 ohne jede Maske.

Im Stuhlfenster x0..75 / y118..239 sind **777** Punkte uebermalt. Die hellen darunter, aufgeteilt
nach Farbe (b-r; Chrom in der Freistellung hat b-r-Quartile **-1/+1/+3**, der helle Behaelter
dahinter **+15/+21/+25**, der Boden **-8**) — `mess_08`:

| Helligkeit r+g+b | uebermalt+ungedeckt | davon NEUTRAL (Chrom) | davon BLAU (Behaelter) |
|---|---|---|---|
| >= 150 | 190 | 53 | 129 |
| >= 200 | 72 | 21 | 48 |
| >= 250 | 30 | 11 | 19 |

Und diese Gegenstaende stehen HINTER dem Spieler, zweifach belegt (`mess_11`):

| Gegenstand | Sockel-Bildzeile | Weltpunkt | Kamera-z | Tiefe | Urteil |
|---|---|---|---|---|---|
| heller Behaelter, linke Kante | 155 | (1835, 27935) | 7663 | 119 | HINTER |
| derselbe, rechte Kante | 153 | (2109, 27457) | 7837 | 122 | HINTER |
| blaue Tonne | 149 | (2549, 27207) | 8211 | 128 | HINTER |
| **Spieler** | **179** | (418, 26454) | **6049** | **94** | — |
| Stuhl 07_01, Standpunkt | 238 | (-1669, 26016) | 3985 | 62 | VOR |

**Nullmodell ohne Kameramatrix:** ein Gegenstand am Fuss des Spielers haette seinen Sockel auf
Bildzeile 179. Die Sockel oben liegen bei 149..155, also 24..30 Zeilen hoeher — weiter weg.
Gegenprobe der Matrix: Bildpunkt (77.5,179.5) → Kamera-z **6049** und Welt **(418, 26454)**
gegen die von der Engine protokollierten 6073 und (418, 26497) — 0,4 % bzw. 43 Einheiten.

Die 22 hellsten neutralen uebermalten Punkte (x70..75, y125..139, Helligkeit bis 328) gehoeren
zum **Chromrohr eines zweiten Stuhls** — Abstand 8..17 Punkte zur Freistellung, ganz oberhalb
beider Sockelzeilen (Bild `23_zweiter_stuhl.png`). Das ist kein Loch in der Arbeit des Nutzers,
sondern ein Gegenstand, den er nicht freigestellt hat.

### 3.2 Die im Auftrag genannte „obere Rahmenschiene y129..133 / x17..58" ist der Behaelter

`mess_08`: in diesem Rechteck sind 133 Punkte ungedeckt, davon 35 hell (>= 150). Deren b-r-
Quartile sind **+13 / +20 / +24**. Aufgeteilt: **30 BLAU, 5 neutral.** Die Struktur ist der
Rand des hellen Behaelters (Tiefe 119..122), nicht das Chromrohr des Stuhls. Sie gehoert nicht
in die Maske des Stuhls, und der Nutzer hat sie richtig weggelassen.

### 3.3 Der echte Saum: 66 Kandidaten, 12 mit Wirkung, 0 zuordenbar

Chromkandidaten (hell >= 150 **und** neutral |b-r| < 8) im Stuhlfenster: **801**. Davon 332 in
der Freistellung, 350 unter irgendeiner Maske, **451 ungedeckt** — aber nur **66** liegen direkt
(8-Nachbarschaft) an der Freistellung, in 12+ Komponenten von 2..7 Punkten. Ein Ein-Punkt-Saum,
kein Stueck (`mess_07`).

Punktweise Zurechnung ihrer Wirkung ueber die 6841 begehbaren Standplaetze (`mess_13`):
**54 der 66 wirken an keinem einzigen Standplatz.** Wirksam sind genau **12**:

```
(61,142) (61,143) (61,144) (61,145)   je 18 Standplaetze
(60,148) (60,149)                     je 17
(60,150) (60,151)                     je 18
(55,132) (56,132) (57,132) (57,133)   11..13
Summe 191 Kastenpunkte, hoechstens 12 je Standplatz (von ~1000)
```

Und **diese 12 sind es, die ich nicht zuordnen kann.** Drei Wege, drei Fehlschlaege:

1. **Farbe.** Die Regel, die den Behaelter sauber trennt (b-r in -5..+5, geeicht an drei
   unabhaengigen Referenzen), verwirft diese 12 — sie tragen b-r **+3..+6**. Und sie verwirft
   damit konsequent: die 27 Punkte, die die Regel nimmt, wirken an **0 von 6841** Standplaetzen
   (`mess_09`, `mess_12`). ⛔ Entscheidend: derselbe helle Streifen, den der Nutzer in den
   Zeilen 134..141 **selbst genommen** hat, traegt dort b-r **+5..+18**. Eine b-r-Schwelle, die
   die Fortsetzung verwirft, verwirft auch seine eigenen Punkte. **Farbe trennt hier nicht.**
2. **Grat-Fortsetzung** (White-Top-Hat, Keim = Gratpunkte in der Freistellung, Wachstum entlang
   des Grats): liefert bei SE 5 / Schwelle 45 **484** Punkte, darunter **112 auf der eigenen
   Kante des Behaelters** (x41..64, y118..134, b-r bis +30). Der Grat ist ein Netz ueber
   Gegenstandsgrenzen hinweg — ohne Abstandsgrenze unbrauchbar (`mess_14`).
3. **Kammregel ohne jeden Parameter** (ein Nachbarpunkt gehoert dazu, wenn er heller ist als
   jeder Freistellungspunkt in seiner 3x3-Nachbarschaft): 292 Punkte. Nullmodell mit 200
   zufaellig verschobenen Kopien derselben Freistellung: Median **238**, 5..95 % **166..300** —
   an der echten Lage 433. Die Regel feuert also fast genauso stark auf beliebigem Boden
   (`mess_15`).

Der Befund im Bild, so genau wie er geht (Bild `31_offene_12_punkte.png`, 34-fach):

```
Zeile   x59  x60  x61  x62  x63     Freistellung
138       3  429* 339*  126           x61,x62 drin   <- Helligkeit 429/339
141      10  222* 343*  133           x61,x62 drin   <- 222/343
142       0  214  301   142           x61,x62 DRAUSSEN <- 214/301
145       6  246  170   155           draussen
148       0  162  177   150           draussen
151      79  186  123   156           draussen
```

Die rechte Kante der Freistellung springt zwischen Zeile 141 und 142 um zwei Spalten nach links,
waehrend der helle Streifen bei gleicher Helligkeit weiterlaeuft. Ob dieser Streifen der hintere
Pfosten des Stuhls VOR dem Behaelter ist (dann fehlen die 12 Punkte) oder die beleuchtete Kante
des Behaelters selbst (dann gehoeren sie nicht dazu), entscheidet das Bild nicht: der
Rechtskontrast (Helligkeit an der Kante minus zwei Punkte weiter rechts) faellt glatt von 304
(y137) ueber 89 (y141) auf 72 (y142) und 27 (y148) — **kein Schnitt, an dem der Wechsel liegt.**

**Ich habe deshalb nichts angelegt und die Datei des Nutzers nicht angefasst.** Eine
Zusatz-Freistellung habe ich probehalber gebaut (`protokoll/bau_saum.py`, 27 Punkte) und wieder
verworfen, weil sie an **0 von 6841** Standplaetzen wirkt — genau der Fall, fuer den der Auftrag
„sage das mit der Zahl und lass sie liegen" vorsieht. Die 12 wirksamen Punkte sind eine
Rueckfrage, nicht eine Heuristik: 11 in einem Lauf (x61 y142..145, x60 y148..151) und 4 oben
(x55..57 y132..133). Das Lasso ist die Zuordnung, und der Nutzer ist ihr Autor.

---

## 4. Auftrag 5 — Verifikation am echten Bild

⛔ **Es gibt kein Vorher/Nachher, weil ich keinen gerenderten Byte geaendert habe.** Das ist
pruefbar und nicht behauptet: `git status` dieses Zweiges zeigt unter `re15_port/shared_assets/`,
`pri/` und `analysis/esp_masken_2026-09-03/auswahl.json` **keine** Aenderung; neu sind nur der
Riegel, seine Referenz-PBM und dieses Dossier. Ein frischer Lauf an der Marke ergibt deshalb ein
bitgleiches Bild, und ein nachgestelltes „Nachher" waere eine Nachbildung.

Was ich stattdessen am echten Bild verifiziert habe:

* Der Abzug des Nutzers ist der verlangte Pfad: `re15_render_pc_screenshot` liest mit
  `SDL_RenderReadPixels(s_renderer, NULL, ...)` das **ganze** Ausgabebild des beschleunigten
  Renderers (`render_pc.c:1984-2010`), 960x720 = 3x 320x240, `RE15_AUTOSHOT_SMALL` nicht gesetzt;
  ausgeloest aus der F9-Marke in `main.c:5287`.
* Selbstpruefung des Composite: an **5435** gedeckten Punkten weichen **14 (0,26 %)** vom
  Hintergrund ab. Mein Modell liest den Abzug also richtig (§3.1).
* Ich habe die Bilder selbst angesehen, in 14- bis 40-facher Vergroesserung und ohne Gamma-
  Aufhellung als Gegenprobe: `18_querstrebe_y190.png` (die Strebe und ihr Knick),
  `17_lehne_oberkante_bg.png` (der Behaelter mit seinem Rand), `23_zweiter_stuhl.png` (das
  Chromrohr des zweiten Stuhls), `30_marke_erklaert.png` (Hintergrund gegen Abzug, beringt),
  `31_offene_12_punkte.png` (die Rueckfrage).
* `build/bg_ppm/ROOM10D07.ppm` habe ich mit der eigenen Sonde neu erzeugt und gegen den
  vorhandenen Abzug gehasht (md5 gleich) — die Messungen ruhen nicht auf einer fremden Datei.

---

## 5. Auftrag 6/7 — Riegel und Suite

### 5.1 Was der Riegel haelt

`re15_port/tests/unit/probes/r19b_marke4.cmake` (GLOB) →
`re15_port/tests/unit/r19b_marke4_10d0_c7_zeilenmodell.c`, Test
`unit_r19b_marke4_zeilenmodell`. Referenz: `probes/r19b_marke4_stuhl.pbm` (2677 Punkte, die
Freistellung des Nutzers bei Alpha > 110).

Geprueft wird die **Modellklasse**, nicht eine an der Marke gemessene Zahl:

| | Pruefung | woraus sie folgt |
|---|---|---|
| A | 2677 von 2677 Punkten gedeckt | Abdeckungszahl; die Handarbeit des Nutzers kommt an |
| B | jede Bildzeile traegt GENAU EINE Tiefe | ein Standpunkt ⇒ Tiefe = f(Zeile). Keine Konstante |
| C | die Tiefe waechst monoton mit der Zeile, und `R[7] > 0` | `vz(Y) = R[7]/4096 · Y + const` ist linear; das Vorzeichen wird aus dem Kamerasatz GEPRUEFT |
| D | **Naht-Probe**: kein 8-Nachbarpaar derselben Silhouette springt um mehr als EINEN Eimer | Starrheit — ein starrer Gegenstand springt an seiner eigenen Naht nicht (dasselbe Argument wie im `grund` von ROOM10D0 C1) |

Gemessene Werte im gruenen Lauf: `H=208`, `R[7]=1040`, Sektion `@0x9EC`, 104 Masken,
116 Bildzeilen, Tiefen 53..62, Zeilen mit mehreren Tiefen **0**, Monotonie-Rueckschritte **0**,
**9638** Nachbarpaare mit groesstem Tiefensprung **1** und **0** Paaren darueber.

### 5.2 Gegenprobe — der Riegel rechnet sie selbst

Aus DEMSELBEN Kamerasatz und DERSELBEN Silhouette:

```
Gegenprobe Spaltenregel: Zeilen mit mehreren Tiefen 114 von 116, groesster Eimer 86,
  Naht-Sprung 20 bei (68,181)=64 -> (69,182)=84, Paare > 1: 318
Gegenprobe Spaltenregel + bodenkante 1..68: Zeilen mit mehreren Tiefen 114,
  Naht-Sprung 10, Paare > 1: 295
Weltsprung des Bodenpunkts 68->69 = 1458 Einheiten, groesster Sprung innerhalb 1..68 = 779
```

Der Riegel besteht also nur, solange die verworfenen Modelle unterscheidbar bleiben — er kann
nicht wirkungslos gruen stehen. (Die C-Gegenprobe rechnet die vereinfachte Spaltenregel: ein
Eimer je Spalte ohne Zeilenprofil. Mit Profil sind es in Python 24 Eimer Sprung und 438 Paare —
`mess_17`. Die C-Zahlen sind die schwaechere Variante und genuegen.)

### 5.2b Externe Gegenprobe: DIESELBE exe gegen echte Spaltenregel-Dateien

`protokoll/gegenprobe_msk.py` baut Cut 7 mit `"aufrecht": "spalten"` ueber das echte
Bauwerkzeug (`bau_p2.bau_cut`, `--stufe 16 --statistik max` wie der Auslieferungsbau) und legt
`ROOM10D0.MSK` + `ROOM10D0_PRI07.TIM` in `build/r19b/gegenprobe_MASKS/`. Diese zwei Dateien
wurden kurz in `shared_assets/PSX/MASKS/` eingesetzt, die exe lief, danach wurden sie
zurueckgeschrieben — mit Hash-Beleg:

```
vorher / nachher  ROOM10D0.MSK       FCAE4848F4106F6C8746278BC77CAF58
vorher / nachher  ROOM10D0_PRI07.TIM 84F25318E149144D5528141E780BFF82
git status re15_port/shared_assets pri auswahl.json -> leer
```

Ergebnis der exe gegen die Spaltenregel-Daten (**EXIT=1**):

```
  ROOM10D0.MSK Cut 7: Sektion @0x9EC, 105 Masken, 105 gezeichnet
  PASS: jeder Punkt der Nutzer-Freistellung ist gedeckt
  116 Bildzeilen, Tiefen 54..86, Zeilen mit mehreren Tiefen 111, Rueckschritte 16
  FAIL: jede Bildzeile des Stuhls traegt GENAU EINE Tiefe
  FAIL: die Tiefe waechst monoton mit der Bildzeile
  Naht: 9638 Paare, groesster Sprung 24 bei (68,181)=60 -> (69,182)=84, Paare > 1: 2107
  FAIL: der starre Gegenstand springt an seiner eigenen Naht um hoechstens EINEN Eimer
  Spalten 69..72: 25 Texel verglichen, 25 abweichend
  FAIL: die Spalten 69..72 tragen dieselbe Tiefe wie ihre Zeilennachbarn
```

⛔ **Und das ist der eigentliche Nebenbefund dieser Runde:** dieselbe Spaltenregel-Fassung
bestand die ALTE Abnahme mit

```
Deckung fehlt 0 / zuviel 0 ; 105 Rechtecke ; Tiefen 54..105 ; 31 Stufen
Standplatz-Schiene: 30 VORn / 0 VORverd / 24 HINTn / 0 HINTfrei
```

also mit **0 Beanstandungen** — besser als der ausgelieferte Stand (der 2 offene HINTfrei-Faelle
trug). Ein Modell, das den Stuhl an seiner eigenen Naht um 24 OT-Eimer zerreisst, ist fuer
`abnahme.py` einwandfrei. Das ist derselbe blinde Fleck, der schon Marke 1..3 durchgelassen hat,
jetzt mit einer Zahl. (Nebenbei: die Spaltenregel braucht 105 Rechtecke — genau die
Engine-Grenze — gegen 104 beim Zeilenmodell.)

Die Abweichungen zwischen Python- und C-Kamerasatz (R[7] 1037 gegen 1040, Fusspunkt-Kamera-z
3985 gegen 3992, 0,2 %) kommen daher, dass `re15_camera_build_view` die Wurzel in `float`
rechnet und der Python-Spiegel `geom.build_view` in Integer. Beide landen im **gleichen Eimer 62**.

### 5.3 Was NICHT in den Riegel kam — und warum

Eine Schranke „kein Punkt hinter dem eigenen Fussabdruck" haette gut ausgesehen: die tiefste
Silhouettenzeile 238 gibt Kamera-z 3985 → Eimer 62, und die Spaltenregel verletzt das an 1608
von 2677 Punkten. ⛔ Sie ist **selbstbestaetigend**. Modellfrei lautet die Aussage nur „der
Fussabdruck liegt irgendwo unter der Silhouette", und der fernste Bodenpunkt unter irgendeinem
Silhouettenpunkt hat Kamera-z **11895 → Eimer 185** (`mess_17`). Gegen die Spaltenregel (max 86)
hat das keine Trennkraft. Die Zahl 62 entsteht erst, wenn man den EINEN Standpunkt schon
voraussetzt — also genau das, was geprueft werden soll. Sie steht darum nur als Messwert hier
und nicht als Kriterium im Test.

### 5.4 Suite

`RE15_BUILD_DIR=re15_port/build_r19b bash re15_port/tools/local_build.sh all`:
**334/334 Tests gruen** (Ausgangsstand desselben Baums: 333/333; neu ist
`unit_r19b_marke4_zeilenmodell`).

⛔ Nebenbefund zur Werkzeugkette: `cmake --build … --target …` direkt aus Git-Bash stirbt hier
still (exit 1, 0 Byte stderr) — das ist das in CLAUDE.md beschriebene PATH-Shadowing von
`cc1.exe`. Aus PowerShell und ueber `local_build.sh` baut dasselbe Ziel fehlerfrei. Notiert,
weil ich den Fehlschlag zuerst fuer einen Compile-Fehler meines Tests gehalten habe.

---

## 6. Was diese Runde widerlegt hat

1. ⛔ **„Die Spaltenregel setzt die Spalten 69..72 zu weit weg, `bodenkante` hilft."** Nein: das
   Objekt traegt `aufrecht: true`, die Spaltenregel laeuft nicht, `bodenkante` ist ein
   Nulleingriff, die Spalten tragen 57/58 wie ihre Nachbarn, und an F3843 wechselt kein einziger
   Figurpunkt sein Urteil. Ueber alle 6841 Standplaetze verdeckt `bodenkante` an einem Platz
   **300 Kastenpunkte weniger**.
2. ⛔ **„Die obere Rahmenschiene y129..133 / x17..58 fehlt der Freistellung."** Nein: von den 35
   hellen ungedeckten Punkten dort sind **30 blau** (b-r-Median +20) — der Rand des Behaelters
   mit Tiefe 119..122, nicht das Chromrohr des Stuhls.
3. ⛔ **„Die Freistellung kommt nicht vollstaendig an."** Nein: 2677 von 2677, gelesen ueber den
   echten Ladeweg.
4. ⛔ **„Eine Farbregel kann den fehlenden Saum bestimmen."** Nein: der Nutzer hat in denselben
   Pfosten Punkte mit b-r bis +18 aufgenommen; jede Schwelle, die die Fortsetzung verwirft,
   verwirft auch seine eigenen Punkte.
5. ⛔ **„Eine Grat- oder Kammregel kann es ohne Farbe."** Nein: 484 Punkte mit 112 auf dem
   Behaelter, bzw. 292 Punkte gegen ein Nullmodell von 238.

## 7. Offene Punkte

1. **Die 12 Punkte (§3.3).** Rueckfrage an den Nutzer, mit Bild `31_offene_12_punkte.png`:
   gehoeren `x61 y142..145`, `x60 y148..151` und `x55..57 y132..133` zum Stuhl? Wenn ja, waechst
   seine Freistellung um 12 Punkte, und die Wirkung ist gemessen: 18 von 6841 Standplaetzen,
   hoechstens 12 Kastenpunkte je Platz.
2. **Der zweite Stuhl, x68..76 / y120..140.** 22 helle neutrale Punkte, 8..17 Punkte von der
   Freistellung entfernt, ohne eigene Maske. Ein eigener Gegenstand ohne Freistellung — an F3843
   ohne Folgen (er steht hinter dem Spieler), an einem Standplatz weiter hinten nicht.
3. **Der helle Keil x20..30 / y134..147.** 45 helle neutrale ungedeckte Punkte zwischen der
   Lehne und dem linken Stuhl, vom Lasso ausgespart. Nicht zuordenbar (er koennte die
   Sitzflaeche des linken Stuhls oder der Boden dahinter sein) und nicht „minimal" — deshalb
   nicht angefasst, nur festgehalten (`25_linker_keil.png`).
4. **Der Quader „Holztisch mit Klappstuhl"** dieses Cuts (1794 von 5435 Deckungspunkten) ist
   unabhaengig von diesem Befund und bleibt offen wie zuvor.
5. **`abnahme.py` hat kein Kriterium fuer FEHLENDE Verdeckung — und keines fuer die Naht.**
   Belegt in §5.2b: die Spaltenregel-Fassung dieses Cuts besteht die alte Abnahme mit
   `0 VORverd / 0 HINTfrei`, obwohl sie den Stuhl an seiner eigenen Naht um 24 OT-Eimer
   zerreisst. Die Naht-Probe aus §5.1 D waere ein billiges, modellfreies Tor fuer ALLE 77
   Cuts — sie steht bisher nur in diesem einen Riegel. Das ist der naechste Schritt, den ich
   in dieser Runde nicht gemacht habe, weil er die Zahlen jedes Raums verschiebt.

## 8. Dateien

* Dossier: `analysis/befunde_2026-09-21/pri-runde19b-marke4.md`
* Bilder und Messskripte: `analysis/befunde_2026-09-21/pri-runde19b-marke4/`,
  darunter `protokoll/mess_01..18`, `bau_saum.py`, `bild_30.py`, `bild_31_ruecklage.py`
* Riegel: `re15_port/tests/unit/probes/r19b_marke4.cmake`,
  `re15_port/tests/unit/r19b_marke4_10d0_c7_zeilenmodell.c`,
  `re15_port/tests/unit/probes/r19b_marke4_stuhl.pbm`
* **unveraendert**: `pri/STAGE1/10D0/07_01.png`, `analysis/esp_masken_2026-09-03/auswahl.json`,
  `re15_port/shared_assets/PSX/MASKS/*`
