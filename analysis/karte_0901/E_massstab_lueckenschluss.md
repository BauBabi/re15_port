# E — Ist die Maßstabs-Lücke schließbar, ohne zu raten?

**Datum:** 2026-09-01 · **Frage:** 39 der 72 Karten-Räume tragen den Stub `{0,0,1,1}`.
Lassen sich deren Zeilen aus den Daten **messen** — oder wäre jede Zahl geraten?

**Antwort in einem Satz:** **Nein, nicht als Messung.** Der Maßstab ist innerhalb einer
Stage nicht konstant (Faktor bis 1,94), von 39 Stub-Räumen ist **kein einziger**
überbestimmt (27 haben überhaupt keinen geeichten Nachbarn auf ihrer Seite), das
Tür-Kriterium selbst hat mit *echten* Zeilen schon 6,4 px Eigenrauschen, und die
unabhängige Gegenprobe zeigt, dass das beste erreichbare Verfahren die Stub-Räume
**nicht besser** trifft als der heutige Rückfall. Zusätzlich hat die Gegenprobe einen
*anderen* Defekt aufgedeckt, der viel größer ist als die fehlende Zeile: bei 7 von 10
prüfbaren Stub-Räumen liegt der Tür-Fehler bei **30–190 px** — das ist kein
Versatzfehler in einem 16–88 px großen Rechteck, das ist das **falsche Rechteck**.

Alle Skripte: `analysis/karte_0901/s/` (`base.py` = gemeinsame Datenbasis; `t1`…`t20`).

---

## 0. Was die Tabelle ist — nachgeschlagen, nicht angenommen

### 0.1 Die Formel (byte-genau zitiert)

`FUN_800473f8`, Marker-Projektion:

| Adresse | Instruktion | Bedeutung |
|---|---|---|
| `0x8004741c` | `lw v0,0x0(s4)=>DAT_800aca88` | Spieler-Welt-x |
| `0x80047424` | `lbu a2,DAT_800b260d` | **Zeilenindex** (Byte!) |
| `0x80047428` | `addiu v0,v0,0x7d00` | `+32000` |
| `0x8004742c/30/48` | `sll v1,v0,2` / `addu v1,v1,v0` / `sll t0,v1,1` | `×5`, dann `×2` = `×10` |
| `0x80047434` | `sll a2,a2,0x3` | Zeilenbreite **8 Byte** |
| `0x80047444` | `lhu v0,DAT_800768b4` | `sx` — **`lhu`, also vorzeichenlos** |
| `0x8004744c` | `mult t0,v0` | |
| `0x8004745c` | `sra t0,t0,0x14` | `>> 20` |
| `0x80047460` | `addiu t0,t0,0x5` | `+5` |
| `0x80047464` | `mult t0,a3` (`a3=0x66666667`) | Magic-Number-Division durch 10 |
| `0x8004746c` | `lw v1,DAT_800aca90` | Spieler-Welt-z |
| `0x8004747c` | `lhu a0,DAT_800768b6` | `sy` |
| `0x80047480–94` | analog, Ergebnis wird **negiert** | z wächst nach Süden, Karten-y nach unten |

```
mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox      (ox = s16 @+0)
my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy      (oy = s16 @+2)
```

### 0.2 Der Zeilenindex ist disassembly-verifiziert

`DAT_800b260d` wird **nicht** gerechnet, sondern von einer Sprungtabelle je Stage gesetzt
(Setzer-Zeiger @`0x80074c0c`…`0x80074c20`). Jeder Case schreibt die rohe Raumnummer und
fällt durch; der gemeinsame Schwanz addiert die Stage-Basis:

| Stage | Setzer | Basis-Addition | Basis |
|---|---|---|---|
| 1 | `LAB_8004b568` | — (keine) | 0 |
| 2 | `LAB_8004b8a0` | `addiu v0,v0,0x26` @`0x8004b9c0` | 38 |
| 3 | `LAB_8004b9d4` | `addiu v0,v0,0x32` @`0x8004bc88` | 50 |
| 4 | `LAB_8004bc9c` | `addiu v0,v0,0x41` @`0x8004bdc0` | 65 |
| 5 | `LAB_8004bdd4` | `addiu v0,v0,0x4d` @`0x8004bf5c` | 77 |
| 6 | `LAB_8004bf70` | `addiu v0,v0,0x62` @`0x8004c044` | 98 |

Zweite, unabhängige Bestätigung: die Tabelle endet bei Index 105
(`0x80076BF8`+8 = `0x80076C00`; ab dort steht Text — `0x304B`, `0x3046` = Hiragana).
106 Zeilen = 38+12+15+12+21+8, und **jede** Stage hat genau so viele Räume, wie ihr
Block Zeilen hat (Stage 1: `ROOM1000`…`ROOM1250` = 38; Stage 3: `ROOM3000`…`ROOM30E0` = 15;
Stage 5: `ROOM5000`…`ROOM5140` = 21; …). Die `BASES`-Annahme des Generators ist damit
belegt, nicht geraten.

### 0.3 ⛔ Zwei Funktionen lesen dieselbe Tabelle mit UNVERTRÄGLICHEN Formeln

`FUN_80046fd8` (aufgerufen @`0x80049540` und @`0x8004c1bc`) liest dieselben vier Felder,
rechnet aber **mit Division** und **+25000**:

| Adresse | Instruktion |
|---|---|
| `0x8004700c` | `lhu v1,DAT_800768b4` |
| `0x80047010` | `addiu a0,a0,0x61a8` (**+25000**, nicht +32000) |
| `0x80047014` | `div a0,v1` (**Division**, nicht mult+shift) |
| `0x80047088` | `lhu a1,DAT_800768b0` (ox hier **`lhu`**, bei `FUN_800473f8` `lh`) |

Das ist die RE2-Form (`FUN_8006e1f0` @`0x8006e1f0`: `(X+28000)/450`), auf einen
Divisor pro Raum verallgemeinert. Beide Deutungen können nicht gleichzeitig stimmen.
**Gemessen** (`t11_formel2.py`, alle 33 Räume mit echter Zeile, SCA-Rasterpunkte):

| | Median Klemmrate | Median Füllgrad |
|---|---|---|
| F1 `FUN_800473f8` (mult/shift, +32000) | **11,9 %** | **4,6 %** |
| F2 `FUN_80046fd8` (div, +25000) | 100,0 % | 0,0 % |

F2 wirft die Räume vollständig aus ihren Rechtecken. **F1 ist die Deutung der Tabelle**;
`FUN_80046fd8` ist ein zweiter Verbraucher mit einer anderen (älteren/RE2-)Konvention.
Das ist als Fußnote wichtig: wer den Marker je auf `FUN_80046fd8` umstellt, zerstört ihn.

### 0.4 Was der Stub im Original TATSÄCHLICH tut

Mit `sx = 1` ist `(wx+32000)*10 ≤ 640000 < 2^20`, also `>>20 = 0`, `+5 = 5`, `/10 = 0`.
Für **jede** erreichbare Weltkoordinate (s16-Bereich; die Grenze läge erst bei
`wx > 72858`) gilt daher:

```
Welt(-32000,·) -> mx=0   Welt(0,·) -> mx=0   Welt(+32000,·) -> mx=0
```

Der Marker-Quad wird trotzdem gezeichnet — `FUN_80049a5c` ruft `FUN_800473f8` ohne
jeden Maßstabs-Test auf (`if ((DAT_800b25c0 & 0xffffff) == 0x10100)`, nur der
UI-Zustand „MAP-Schirm"). **Im Auslieferungsstand klebt der Spielermarker in diesen
38 Räumen in der linken oberen Bildschirmecke (0,0).**

Ausnahme `ROOM11C0`: Zeile `{174,196,1,1}` @`0x80076990` — die Versätze *sind*
eingetragen, nur der Maßstab nie. Der Marker steht dort fest auf (174,196), also 1 px
unter der Unterkante von Seite 0 / Rect 4 (168,100)+72×96 (Unterkante y=195). Ein
Platzhalter, kein Zufall.

> **Konsequenz für das 100-%-Mandat:** Für diese 39 Räume gibt es **keine
> Original-Wahrheit, die man treffen könnte**. Alles, was der Port dort zeichnet — die
> Bbox-Streckung wie jede Schätzung —, ist eine Port-Ergänzung. „Byte-true schließen"
> ist hier begrifflich unmöglich; die einzige ehrliche Frage ist, welche
> Port-Ergänzung am wenigsten falsch ist.

---

## 1. Statistik der 33 echten Zeilen — ist der Maßstab je Stage konstant?

Grundmenge: die **72** Basis-Räume, die im generierten Header `re15_map_zones.h` ein
Rechteck haben. 33 tragen eine echte Zeile, 39 den Stub. (Von den 33 kommen heute nur
21 durch die Wache des Generators; die anderen 12 fallen auf die Bbox-Streckung zurück —
siehe §1.3.)

Umrechnung: `mx = (wx+32000)·sx/2^20 + ox`, also
**Pixel je Welteinheit = sx / 2^20**, **Welteinheiten je Pixel = 2^20 / sx**.

### 1.1 Alle 33 Zeilen

| Raum | idx | Adresse | ox | oy | sx | sy | E/px x | E/px y | Seite | sy/sx |
|---|---|---|---|---|---|---|---|---|---|---|
| 1020 | 2 | 0x800768C0 | 100 | 136 | 2287 | 2287 | 458,5 | 458,5 | 2 | 1,000 |
| 1030 | 3 | 0x800768C8 | 108 | 194 | 2428 | 2229 | 431,9 | 470,4 | 2 | 0,918 |
| 1070 | 7 | 0x800768E8 | 79 | 205 | 2229 | 2088 | 470,4 | 502,2 | 2 | 0,937 |
| 1150 | 21 | 0x80076958 | 111 | 130 | 2296 | 2312 | 456,7 | 453,5 | 4 | 1,007 |
| 1170 | 23 | 0x80076968 | 100 | 206 | 2280 | 2268 | 459,9 | 462,3 | 5 | 0,995 |
| 1190 | 25 | 0x80076978 | 75 | 127 | 1920 | 2301 | 546,1 | 455,7 | 0 | 1,198 |
| 11A0 | 26 | 0x80076980 | 138 | 113 | 1776 | 2048 | 590,4 | 512,0 | 0 | 1,153 |
| 11B0 | 27 | 0x80076988 | 159 | 110 | 2272 | 1954 | 461,5 | 536,6 | 0 | 0,860 |
| 1200 | 32 | 0x800769B0 | 129 | 150 | **3168** | 2305 | **331,0** | 454,9 | 1 | 0,728 |
| 1210 | 33 | 0x800769B8 | 177 | 140 | 2496 | 2250 | 420,1 | 466,0 | 1 | 0,901 |
| 2000 | 38 | 0x800769E0 | 80 | 196 | 2340 | 2308 | 448,1 | 454,3 | 6 | 0,986 |
| 3000 | 50 | 0x80076A40 | 71 | 148 | 2256 | 2257 | 464,8 | 464,6 | 7 | 1,000 |
| 3010 | 51 | 0x80076A48 | 113 | 224 | 2208 | 2737 | 474,9 | 383,1 | 7 | 1,240 |
| 3040 | 54 | 0x80076A60 | 183 | 115 | 2240 | 2240 | 468,1 | 468,1 | 7 | 1,000 |
| 3060 | 56 | 0x80076A70 | 157 | 147 | 2496 | **3984** | 420,1 | **263,2** | 7 | 1,596 |
| 3070 | 57 | 0x80076A78 | 64 | 209 | 2152 | 2344 | 487,3 | 447,3 | 7 | 1,089 |
| 3090 | 59 | 0x80076A88 | 207 | 206 | 2064 | 2337 | 508,0 | 448,7 | 7 | 1,132 |
| 30E0 | 64 | 0x80076AB0 | 93 | 207 | 2056 | 2432 | 510,0 | 431,2 | 7 | 1,183 |
| 4010 | 66 | 0x80076AC0 | 140 | 76 | 1536 | 1617 | 682,7 | 648,5 | 8 | 1,053 |
| 4030 | 68 | 0x80076AD0 | 150 | 121 | **1281** | 1288 | **818,6** | 814,1 | 8 | 1,006 |
| 4090 | 74 | 0x80076B00 | 191 | 143 | 1540 | 1844 | 680,9 | 568,6 | 8 | 1,197 |
| 40A0 | 75 | 0x80076B08 | 197 | 180 | 1793 | 1712 | 584,8 | 612,5 | 8 | 0,955 |
| 5010 | 78 | 0x80076B20 | 159 | 115 | 1537 | 1801 | 682,2 | 582,2 | 9 | 1,172 |
| 5030 | 80 | 0x80076B30 | 125 | 153 | 1792 | 1792 | 585,1 | 585,1 | 9 | 1,000 |
| 5040 | 81 | 0x80076B38 | 74 | 152 | 1793 | 1729 | 584,8 | 606,5 | 9 | 0,964 |
| 5060 | 83 | 0x80076B48 | 86 | 173 | 1857 | 1665 | 564,7 | 629,8 | 9 | 0,897 |
| 50A0 | 87 | 0x80076B68 | 124 | 204 | 1696 | 1681 | 618,3 | 623,8 | 9 | 0,991 |
| 50C0 | 89 | 0x80076B78 | 114 | 122 | 1700 | 1648 | 616,8 | 636,3 | 10 | 0,969 |
| 5100 | 93 | 0x80076B98 | 144 | 184 | 1700 | 1700 | 616,8 | 616,8 | 11 | 1,000 |
| 5110 | 94 | 0x80076BA0 | 125 | 154 | 1792 | 1792 | 585,1 | 585,1 | 9 | 1,000 |
| 5120 | 95 | 0x80076BA8 | 74 | 152 | 1793 | 1729 | 584,8 | 606,5 | 9 | 0,964 |
| 6030 | 101 | 0x80076BD8 | 243 | 137 | 2161 | 2304 | 485,2 | 455,1 | 12 | 1,066 |
| 6040 | 102 | 0x80076BE0 | 202 | 136 | 2193 | 2145 | 478,1 | 488,8 | 12 | 0,978 |

Nebenbefund: **drei Zeilenpaare sind bitgleich** — idx 80/94 (`5030`/`5110`),
81/95 (`5040`/`5120`), 87/97 (`50A0`/`5140`), jeweils Raum *N* und *N*+14. Diese
Räume sind auf der Karte **derselbe Ort**. Der Port weist ihnen heute
**verschiedene** Rechtecke zu (`5030`→9/13, `5110`→9/9; `5040`→9/4, `5120`→9/3) —
das kann nicht beides stimmen.

### 1.2 Streuung je Stage (Welteinheiten je Pixel, sx und sy zusammen)

| Stage | n | min | **Median** | max | Stdabw | **max/min** |
|---|---|---|---|---|---|---|
| 1 | 20 | 331,0 | **460,7** | 590,4 | 51,1 | **1,784** |
| 2 | 2 | 448,1 | 451,2 | 454,3 | 3,1 | 1,014 |
| 3 | 14 | 263,2 | **464,7** | 510,0 | 60,0 | **1,938** |
| 4 | 8 | 568,6 | **664,7** | 818,6 | 89,5 | **1,440** |
| 5 | 18 | 564,7 | **606,5** | 682,2 | 26,9 | **1,208** |
| 6 | 4 | 455,1 | 481,7 | 488,8 | 13,1 | 1,074 |
| **alle** | 66 | 263,2 | 495,5 | 818,6 | 99,3 | **3,110** |

Vergleich mit RE2 (`FUN_8006e1f0` @`0x8006e1f0`, fest `450` Einheiten/px):

| Stage | Median E/px | gegen RE2 450 |
|---|---|---|
| 1 | 460,7 | **+2,4 %** |
| 2 | 451,2 | **+0,3 %** |
| 3 | 464,7 | **+3,3 %** |
| 4 | 664,7 | +47,7 % |
| 5 | 606,5 | +34,8 % |
| 6 | 481,7 | +7,0 % |

Das Polizeirevier (Stages 1–3) liegt im Median praktisch auf RE2s 450 — die Blätter
sind mit demselben Zeichenmaßstab gebaut. Ab Stage 4 (Untergrund/Labor) wird gröber
gezeichnet. **Aber:** die Streuung *innerhalb* jeder Stage ist genau so groß wie der
Unterschied *zwischen* den Stages. 28 von 33 Zeilen sind sogar **anisotrop**
(`sx ≠ sy`, `sy/sx` von 0,728 bis 1,596) — jede Kachel wurde einzeln eingepasst.

> **Ergebnis Schritt 1: Der Maßstab ist NICHT je Stage konstant.** Damit fällt die
> Voraussetzung von Schritt 2 („dann bleiben nur ox/oy offen") weg. Wer für `ROOM1200`
> (331 E/px) den Stage-1-Median 461 einsetzt, staucht den Raum um 28 % — bei 12970
> Welteinheiten Breite sind das 11 px auf einem 32 px breiten Rechteck.

### 1.3 12 der 33 echten Zeilen passen nicht zu ihrem Rechteck

Mit der echten Zeile projizierte SCA-Rasterpunkte, gemessen gegen das vom Port
zugewiesene Rechteck (`t5_fit.py`, `t12_rect_suche.py`, `t14_alleseiten.py`):

| Raum | Klemmrate | Diagnose |
|---|---|---|
| 1020 … 5100 (21 Räume) | 0–22 % | konsistent — die Zeile kommt in den Header |
| 3070 | 25,3 % | knapp an der 25-%-Wache gescheitert |
| 1190 | 37,0 % | Zeile passt besser auf Seite 0 Rect 0 (21 % Klemm) |
| 4090 | 100 % | Zeile zeigt eindeutig auf **Seite 8 Rect 9** (0 % Klemm, 88 % Deckung) |
| 5120 | 81,2 % | Zeile zeigt auf **Rect 4** — dasselbe wie 5040 (Zeilen bitgleich) |
| 5030 / 5110 | 100 % | beide zeigen auf Rect 7 (25 % Klemm, 23 % Deckung) |
| 4030 | 88,6 % | Projektion nur 11×13 px; auf 16 Seiten „passend" = wertlos |
| 3060, 30E0 | 72,6 % / 100 % | trifft auf der eigenen Seite **kein** Rechteck |
| **11A0** | 99,2 % | Projektion x[155..244] y[**−3**..61] — **auf KEINER Seite ein Rechteck** |
| **6030** | 100 % | Projektion x[250..310] y[106..128] — **auf KEINER Seite ein Rechteck** |
| 6040 | 100 % | Projektion 9×8 px, beliebig platzierbar = wertlos |

`11A0` und `6030` sind mit ihrer eigenen Zeile nachweislich **außerhalb jedes
gezeichneten Rechtecks auf jedem Blatt** (y = −3 liegt über dem Bildschirmrand). Das
sind keine Zuordnungsfehler des Ports, das sind **defekte Zeilen im Auslieferungsstand**.
Auch der „geeichte" Teil der Tabelle ist also nicht durchgehend brauchbar.

---

## 2. Schritt 2: Sind ox/oy aus Partner-Türen bestimmbar?

### 2.1 Zuerst das Eigenrauschen des Kriteriums messen

Bevor man mit Türen etwas bestimmt, muss man wissen, wie genau „ein Durchgang ist EIN
Ort" überhaupt gilt. Gemessen an **Tür-Paaren, bei denen BEIDE Räume eine echte Zeile
haben und auf derselben Seite liegen** (`t6_tuerpaare.py`, 30 Paare):

| Abstandsmaß | n | Median | Mittel | max |
|---|---|---|---|---|
| \|Tür_A − Landepunkt_B\| | 30 | 7,44 px | 14,14 | 95,5 |
| \|Tür_A − Gegen-Tür_B\| | 29 | **6,40 px** | 13,70 | 97,4 |

Ohne die Selbst-Türen (ein Raum mit zwei getrennten Bereichen — `1170` 95 px, `11A0`
20–23 px, `6030` 10 px — die sind *nicht* derselbe Ort) bleiben Werte von 2,2 px
(`5120↔5110`) bis 17,9 px (`3040↔3060`).

> **Die Gleichung, mit der man messen will, hat selbst 6,4 px Fehler.** Jede
> Bestimmung daraus kann nicht genauer werden. Zum Vergleich: die Rechtecke sind
> 16×16 bis 88×88 px groß.

### 2.2 Bestimmtheitsgrad: Gleichungen gegen Unbekannte

Unbekannte je Achse: 2 (`ox`+`sx` bzw. `oy`+`sy`). Eine Tür zu einem geeichten Nachbarn
auf derselben Seite liefert je Achse **eine** Gleichung. Überbestimmt = mehr als 2
Stützpunkte mit **verschiedenen** Weltkoordinaten je Achse (`t7_grad.py`):

| Stützpunkte je Achse | Stub-Räume | |
|---|---|---|
| **0** (kein geeichter Nachbar auf der Seite) | **27** | 1000, 1060, 1080, 10A0, 10C0, 10D0, 10E0, 10F0, 1100, 1120, 1140, 1160, 11D0, 11F0, 1240, 1250, 2060, 2080, 2090, 4020, 4050, 4070, 5020, 50E0, 50F0, 6010, 6020 |
| **1** (unterbestimmt) | **9** | 1130, 11C0, 2010, 2050, 30A0, 30B0, 30C0, 5050, 5070 |
| **2** (genau bestimmt) | **3** | 1220, 2070, 3050 |
| **≥3** (überbestimmt) | **0** | — |

> **Ergebnis Schritt 2: Für KEINEN einzigen Stub-Raum ist das System überbestimmt.**
> 69 % haben gar keine Gleichung. Drei Räume sind exakt bestimmt — bei einem Kriterium
> mit 6,4 px Eigenrauschen heißt „exakt bestimmt" aber: das Rauschen geht ungefiltert
> in die Lösung. Das ist ein **Fit**, keine Messung.

---

## 4. Schritt 4: Blindtest — wie nah kommt das Verfahren an die bekannte Wahrheit?

Verfahren: die echte Zeile eines Raums wird **ignoriert**, aus den übrigen Daten
geschätzt und dann mit der echten Zeile verglichen. Fehlermaß = Kartenpixel-Abstand
zwischen geschätzter und echter Projektion, über alle SCA-Rasterpunkte des Raums
(Median und Maximum). Der Stage-Median-Maßstab wird **leave-one-out** gebildet
(sonst würde der geprüfte Raum sich selbst stützen).

Getestete Verfahren:

* **A** — Maßstab = Stage-Median, Versatz aus den Türen (`t15_tuer_blind.py`)
* **B** — Maßstab **und** Versatz aus den Türen, kleinste Quadrate (`t15`)
* **C** — Maßstab = Stage-Median, Versatz = beste Deckung mit der gezeichneten
  Kachel `MAP0x.PIX` (`t16_hybrid.py`)
* **E** — Formregistrierung: (ox,oy,sx,sy) maximieren die Überdeckung (IoU) der
  projizierten SCA-Fläche mit dem gezeichneten Grundriss, Maßstab auf ±20 % um den
  Stage-Median beschränkt (`t16`)
* **E2** — wie E, aber die Maske enthält **keinen Palettenindex 4**; Index 4 sind die
  Wandlinien *und* die gemalten Türblätter, damit ist der Symbolabstand danach eine
  **streng unabhängige** Gegengröße (`t17_E2_blind.py`)
* **F** — Formregistrierung **frei** (Maßstab 768…4352 ohne Vorwissen, `t10_blind_form.py`)
* **BBOX** — der heutige Port-Rückfall (lineare Streckung Zonen-Bbox → Rechteck)

### 4.1 Ergebnis auf den 20/21 Räumen, deren echte Zeile zu ihrem Rechteck passt

| Verfahren | n | Median(dMed) | Median(dMax) | größter dMax |
|---|---|---|---|---|
| **BBOX** (heutiger Port) | 20 | 4,2 px | 6,9 px | **29,0 px** |
| C — Stage-Median + Kachelversatz | 20 | 4,1 px | 5,9 px | 20,4 px |
| **E** — Form, Maßstab ±20 % | 20 | **2,2 px** | **5,5 px** | **17,5 px** |
| E2 — Form ohne Index 4 | 20 | 3,4 px | 6,5 px | 20,4 px |
| F — Form frei | 21 | 3,0 px | 5,4 px | 20,4 px |
| **A** — Tür-Versatz + Stage-Maßstab | 9 | **5,7 px** | 8,1 px | 23,3 px |
| **B** — alles aus Türen | 5 | **12,4 px** | 18,8 px | **70,6 px** |

Auf allen 33 (inkl. der Räume mit defekter/unpassender Zeile) verschlechtert sich
alles gleichermaßen: BBOX 6,2/11,2/211; C 8,1/9,7/215; E 5,2/8,3/215 — die Ausreißer
sind `30E0` (159 px), `4090` (110 px), `5030` (87 px), `6030` (213 px), also die Räume
aus §1.3, bei denen nicht das Verfahren, sondern die *Referenz* falsch liegt.

### 4.2 Die beiden Kernaussagen des Blindtests

**(a) Die Tür-Variante B — genau die, die Schritt 2 als „Messung" vorgeschlagen hat —
ist die SCHLECHTESTE von allen** (12,4 px Median gegen 5,7 px, wenn man den Maßstab
gar nicht erst aus den Türen zieht). Grund: 2–3 Stützpunkte mit 6,4 px Eigenrauschen
bestimmen einen Maßstab nur zum Schein. `30E0` bekommt aus zwei Türen `sx=2758`
statt `2056` und liegt danach **59 px** daneben — bei einem 32×40-Rechteck.

**(b) Die freie Formregistrierung schlägt die WAHRE Zeile in 33 von 33 Fällen
in ihrem eigenen Gütemaß.** Gemessen (`t9_iou_wahr.py` / `t10_blind_form.py`): die IoU
der SCA-Fläche mit dem gezeichneten Grundriss beträgt mit der **echten** Zeile im
Median nur **0,311** (max 0,691) — die Suche findet immer etwas Besseres und sitzt
dabei im Median 3,0 px, im Extremfall 214 px von der Wahrheit entfernt. Beispiel
`ROOM6030`: IoU 0,761 (der drittbeste Wert überhaupt) bei **213 px** Abstand zur
echten Zeile. **Ein hoher IoU-Wert ist also kein Beleg für Richtigkeit.**

Das ist derselbe Fehlertyp wie am 2026-09-01 bei der zurückgenommenen Eichung: ein
Gütemaß, das auf dieselben Daten optimiert wird, an denen es misst.

---

## 3. Schritt 3: Die unabhängige Gegenprobe

Für E2 sind **weder** die Türdaten (RDT `Aot_set` 0x3b/0x68) **noch** die
Palettenindex-4-Pixel (die gemalten Türblätter) Eingabe. Also sind zwei Größen
streng unabhängig:

* **Symbolabstand** — mittlerer Abstand der projizierten RDT-Türen zu den *gemalten*
  Türsymbolen auf demselben Rechteck (`analysis/kartensymbole/symbolkatalog.csv`).
* **Tür-Partner-Abstand** — Abstand der projizierten Tür zu derselben Tür, gezeichnet
  aus dem geeichten Nachbarraum mit dessen **echter** Zeile.
* Dazu **Klemmrate** (Anteil begehbarer Rasterpunkte außerhalb des Rechtecks) und
  **Füllgrad** — beide gehen nicht in E2 ein, weil E2 auf die Kachel-Maske optimiert,
  nicht auf das Rechteck.

### 3.1 Referenzwert: was eine ECHTE Zeile erreicht

| Größe | echte Zeile (21 konsistente Räume) |
|---|---|
| Symbolabstand | **3,2 px** Median (n=15) |
| Tür-Partner-Abstand | **6,4 px** Median (n=29) |
| Klemmrate | 11,9 % Median (0 % bei 7 Räumen) |

### 3.2 Blindtest-Räume: VOR und NACH

| | echte Zeile | E2-Schätzung |
|---|---|---|
| Symbolabstand Median (n=15) | **3,2 px** | **5,4 px** |

**Die Schätzung ist auf einer Größe, die sie nie gesehen hat, messbar schlechter als
die Wahrheit** — genau das, was eine brauchbare Gegenprobe leisten muss. Die
Registrierung findet also nicht die Zeile, sondern nur eine Lage, die die Fläche gut
überdeckt.

### 3.3 Die 39 Stub-Räume: VOR (Bbox) und NACH (E2)

Vollständige Tabelle: `t18_stub.py`. Zusammenfassung:

| Gegengröße | Bbox-Streckung (heute) | E2-Schätzung | Referenz „echte Zeile" |
|---|---|---|---|
| Symbolabstand Median (n=14) | **16,4 px** | **17,2 px** | 3,2 px |
| Tür-Partner-Abstand Median (n=10) | **39,1 px** | **41,4 px** | 6,4 px |
| Klemmrate | 0 % (per Konstruktion — sie beschneidet die Eingabe) | Median 20,0 %, max 80 % | 11,9 % |
| Füllgrad Median | 7,6 % | 5,1 % | 4,6 % |

Einzelwerte (nur Räume, bei denen die Gegengröße existiert):

| Raum | Port pg/rc | Symbol Bbox → E2 | Tür Bbox → E2 |
|---|---|---|---|
| 1130 | 4/4 | 3,2 → 5,7 | 3,6 → 6,4 |
| 11C0 | 0/4 | 11,7 → 23,1 | 12,6 → 22,8 |
| 1220 | 1/4 | 4,1 → 11,7 | 18,7 → 31,6 |
| 2010 | 6/10 | – | **189,2 → 181,2** |
| 2050 | 6/2 | 32,9 → 35,4 | 55,2 → 52,6 |
| 2070 | 6/4 | – | 30,0 → 32,6 |
| 30A0 | 7/3 | 3,0 → 7,0 | 48,0 → 50,2 |
| 30B0 | 7/8 | – | **157,9 → 170,1** |
| 30C0 | 7/10 | – | 30,1 → 28,9 |
| 5070 | 9/7 | 37,6 → 7,3 | 54,0 → 87,0 |
| 10E0 | 3/4 | 29,2 → 34,1 | – |
| 1120 | 4/5 | 7,8 → 10,0 | – |
| 1140 | 4/6 | 2,2 → 18,4 | – |
| 1160 | 4/1 | 11,4 → 15,7 | – |
| 2090 | 6/6 | 30,8 → 20,2 | – |
| 5020 | 9/12 | 21,1 → 17,5 | – |
| 50E0 | 10/1 | 25,1 → 16,8 | – |
| 6020 | 12/6 | 40,3 → 25,5 | – |

> **Ergebnis Schritt 3: Die Gegenprobe verweigert das Verfahren.** E2 verbessert
> weder den Symbol- noch den Tür-Abstand (16,4→17,2 px bzw. 39,1→41,4 px); beide
> bleiben um den Faktor 5–6 schlechter als das, was echte Zeilen erreichen. Was E2
> zusätzlich einhandelt, ist eine Klemmrate von im Median 20 % (max 80 %) — im Port
> heißt das: bei jedem fünften begehbaren Punkt klebt der Marker an der
> Rechteck-Kante (`re15_map_zones.c:157-160` klemmt hart).

### 3.4 Was die Gegenprobe stattdessen aufgedeckt hat

Ein Tür-Abstand von **30–190 px** kann kein Versatzfehler *innerhalb* eines 16–88 px
großen Rechtecks sein. Er bedeutet: der Raum steht auf dem **falschen Rechteck**.
Gegenrechnung (`t20_rectcheck.py`): der Tür-Anker legt eine Position auf der Seite
fest (rechteckunabhängig); man kann fragen, welches Rechteck diese Position enthält.
Bewertet mit Klemmrate und Symbolabstand:

| Raum | Port | bestes Rechteck nach Tür-Anker (Klemm % / Symbol px) | Urteil |
|---|---|---|---|
| 1130 | 4/**4** | **4** (14 % / 1 px) | bestätigt den Port |
| 11C0 | 0/**4** | **4** (37 % / 1 px) | bestätigt den Port |
| 2070 | 6/4 | **0** (0 % / 4 px) | Port-Zuordnung **verdächtig** |
| 30B0 | 7/8 | **9** (0 % / 5 px) | Port-Zuordnung **verdächtig** |
| 30C0 | 7/10 | **12** (0 % / –) | verdächtig |
| 2010 | 6/10 | **0** (6 % / 18 px) | verdächtig |
| 30A0 | 7/3 | 9 (46 % / 4 px) | unklar (9 kollidiert mit 30B0) |
| 1220 | 1/4 | 3 (0 % / 15 px) vs. 5 (75 % / 3 px) | widersprüchlich |
| 2050 | 6/2 | 1 (59 %) | kein Rechteck passt |
| 5070 | 9/7 | 4 (48 %) | kein Rechteck passt |

Der Maßstab ist hier der Stage-Median, also eine Annahme — deshalb sind das
**Verdachtsfälle für eine eigene Untersuchung**, keine Ergebnisse. Aber die
Größenordnung (150–190 px bei `2010`/`30B0`) ist von einer Maßstabsannahme nicht
erklärbar.

---

## 5. Schritt 5: Urteil

### 5.1 Taugt das vorgeschlagene Verfahren?

**Nein.** Vier gemessene Gründe, jeder für sich ausreichend:

1. **Die Voraussetzung stimmt nicht.** Der Maßstab ist innerhalb einer Stage nicht
   konstant: Stage 1 streut um Faktor 1,78 (331…590 E/px), Stage 3 um 1,94, Stage 4
   um 1,44. 28 von 33 Zeilen sind anisotrop. Es gibt keinen Stage-Maßstab, den man
   einsetzen könnte, ohne zu raten.
2. **Es gibt nichts zu überbestimmen.** 0 von 39 Stub-Räumen sind überbestimmt,
   27 von 39 haben überhaupt keinen geeichten Nachbarn auf ihrer Seite, 3 sind exakt
   bestimmt. „Überbestimmt = Messung" ist der richtige Maßstab — und er wird nirgends
   erreicht.
3. **Das Kriterium rauscht stärker als der gesuchte Effekt.** Mit *echten* Zeilen
   auf beiden Seiten liegen dieselben Türen im Median 6,4 px auseinander (max 17,9 px
   ohne Selbst-Türen). Ein aus 1–2 solchen Gleichungen bestimmter Maßstab ist Rauschen.
4. **Der Blindtest bestätigt das quantitativ.** Tür-Variante B: Median 12,4 px,
   größter Fehler 70,6 px — schlechter als der Stage-Median-Maßstab (5,7 px) und
   schlechter als der heutige Bbox-Rückfall (4,2 px).

### 5.2 Für welche Räume liefert überhaupt etwas ein prüfbares Ergebnis?

**Für keinen der 39 Stub-Räume.** Das beste Verfahren (E, Formregistrierung mit
±20-%-Maßstabsfenster) erreicht auf *bekannten* Räumen 2,2 px Median / 17,5 px Maximum
und ist damit im Median doppelt so gut wie die Bbox-Streckung (4,2 px) — aber:

* die unabhängige Gegenprobe zeigt es als **schlechter als die Wahrheit** (Symbol
  5,4 px gegen 3,2 px),
* auf den Stub-Räumen verbessert es **keine** unabhängige Größe (16,4→17,2 px Symbol,
  39,1→41,4 px Tür),
* es gibt **kein Signal**, das eine 2-px-Schätzung von einer 17-px-Schätzung
  unterscheidet — der IoU-Wert tut es nachweislich nicht (`6030`: IoU 0,761 bei
  213 px Fehler),
* und der Gewinn (2 px Median) liegt unter dem Eigenrauschen jeder verfügbaren
  Prüfgröße.

### 5.3 Was stattdessen zu tun ist

1. **Die Lücke bleibt eine Lücke — und das ist die richtige Antwort.** Im Original
   klebt der Marker in 38 dieser Räume auf (0,0) und in `ROOM11C0` auf (174,196).
   Der Bbox-Rückfall des Ports ist eine bewusste Port-Ergänzung; er klemmt und läuft
   nie aus dem Rechteck. Er durch eine Schätzung zu ersetzen, die *aussieht* wie eine
   Original-Zeile (`{ox,oy,sx,sy}` im Header, ununterscheidbar von den echten 21),
   wäre genau der Rate-Defekt im Messgewand, der am 2026-09-01 schon einmal
   zurückgenommen wurde.
2. **Der Nutzer-Report „ab Police Station 2F ist die Karte im Eimer" hat vermutlich
   eine andere Ursache.** Die Gegenprobe misst bei `2010` 189 px und bei `30B0` 158 px
   Tür-Abstand — das ist das falsche **Rechteck**, nicht der fehlende Maßstab. Diese
   Zuordnungen kommen aus dem Hill-Climbing in `gen_map_zones.py` (Kosten aus
   Seitenverhältnis + Flächenrang + Türgraph), nicht aus Daten. Der Türgraph steckt
   dort zwar in der Kostenfunktion, wird aber gegen Seitenverhältnis und Flächenrang
   verrechnet und mit `min(..., 40.0)` gedeckelt — ein 189-px-Fehler kostet dort
   genauso viel wie ein 40-px-Fehler.
3. **Zwei belegte Einzelkorrekturen sind sofort greifbar** (aus §1.3, ganz ohne
   Schätzung, nur aus der ausgelieferten Zeile):
   * `ROOM4090` gehört auf **Seite 8 Rect 9** (0 % Klemmrate, 88 % Deckung), heute 8/5
     (100 % Klemmrate).
   * `ROOM5120` gehört auf **Seite 9 Rect 4** — dasselbe Rechteck wie `ROOM5040`, deren
     Zeilen bitgleich sind (idx 81/95 @`0x80076B38`/`0x80076BA8`). Analog `5030`/`5110`
     (idx 80/94) und `50A0`/`5140` (idx 87/97).
4. **`ROOM11A0` und `ROOM6030` als defekte Zeilen kennzeichnen**, statt sie weiter
   durch die Wache laufen zu lassen: beide projizieren auf **keiner** der 14 Seiten in
   ein Rechteck (`11A0`: y = −3, oberhalb des Bildschirms).
5. **Den irreführenden Kommentar in `re15_port/engine/src/re15_map_zones.c:145-152`
   löschen.** Er behauptet, der Generator bestimme die fehlenden Zeilen aus der
   gezeichneten Fläche und den gemalten Türsymbolen und das sei „beides bestimmt, kein
   freier Vierparameter-Fit". Dieses Verfahren wurde am 2026-09-01 zurückgenommen; der
   Generator setzt heute ausschließlich ausgelieferte Zeilen ein. Wer den Kommentar
   liest, hält die Lücke für geschlossen.

---

## Anhang: Messwerkzeuge

| Skript | misst |
|---|---|
| `s/base.py` | gemeinsame Datenbasis: Zonen aus dem generierten Header + SCA-Zellen **je Zone** |
| `s/t1_tabelle.py`, `s/t13_tabdump.py` | Rohtabelle @`0x800768b0`, Tabellenende, Stub-Liste |
| `s/t4_stat.py` | Maßstabsstatistik je Raum und je Stage, RE2-Vergleich |
| `s/t5_fit.py` | Klemmrate/Füllgrad je Zone mit der echten Zeile |
| `s/t6_tuerpaare.py` | **Eigenrauschen** des Tür-Kriteriums bei echten Zeilen |
| `s/t7_grad.py` | Bestimmtheitsgrad je Raum (Stützpunkte je Achse) |
| `s/t8_kachel.py` | Kachelpixel gegen projizierte SCA-Punkte (ASCII) |
| `s/t9_iou_wahr.py` | IoU der echten Zeile — die **Decke** jedes Form-Verfahrens |
| `s/t10_blind_form.py` | freie Formregistrierung, blind |
| `s/t11_formel2.py` | F1 (`FUN_800473f8`) gegen F2 (`FUN_80046fd8`) |
| `s/t12_rect_suche.py`, `s/t14_alleseiten.py` | auf welches Rechteck zeigt jede echte Zeile |
| `s/t15_tuer_blind.py` | Tür-Varianten A und B, blind |
| `s/t16_hybrid.py` | Varianten C und E gegen BBOX, blind |
| `s/t17_E2_blind.py` | Variante E2 + **streng unabhängiger** Symbolabstand |
| `s/t18_stub.py` | E2 auf allen 39 Stub-Räumen + Gegenproben VOR/NACH |
| `s/t20_rectcheck.py` | passt ein anderes Rechteck zum Tür-Anker? |
