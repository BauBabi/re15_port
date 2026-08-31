# GEGENPRUEFUNG B5 (Rolle: Skeptiker) — "Das Original hat innerhalb eines Raums keinen Etagenbegriff"

Datum 2026-08-31. Auftrag: die Behauptung **widerlegen**. Ergebnis: **NICHT widerlegt** —
im Gegenteil, ich kann sie **staerker** belegen als der Bericht, weil ich nicht auf den vom
Bericht gewaehlten Adressbereich angewiesen bin, sondern die **ganze EXE + alle Overlays**
gescannt habe. Alles unten ist selbst gemessen (eigene Scanner, eigene Disassemblate,
eigener RDT-Walker) — keine Zahl aus dem Bericht uebernommen.

Datei-Mapping selbst geprueft: `PSX.EXE` PS-X-EXE-Header `t_addr=0x80010000` (@0x18),
`t_size=0xaf000` (@0x1c), Dateigroesse 0xaf800 ⇒ `off(a) = a - 0x80010000 + 0x800`. Passt.

Werkzeuge (Scratchpad `.../scratchpad/b5/`): `skep_xref.py` (absoluter lui+lo/store-XREF-Scanner
ueber den kompletten EXE-Text), `region_globals.py` (alle absoluten UND indirekten Speicher-
zugriffe eines Bereichs), `jal.py` (Aufrufer/Aufgerufene), `fnrange.py` (Funktionsende + Treffer
im Koerper), `ovl_xref.py` (dieselbe Analyse fuer STAGE*.BIN, Load @0x80100000 OHNE 0x800-Header),
`scdwalk.py` (SCD-Walker fuer Door_aot_set).

---

## 1. Teilbehauptung A — Seiten-Setzer liest nur die Raumnummer: BESTAETIGT

Vollstaendig aus den Bytes disassembliert (nicht aus `ghidra1_V2.txt` uebernommen),
Funktion `0x8004b568 .. 0x8004b898` (`jr ra` @0x8004b898):

```
8004b568 lui   v1,0x800b
8004b56c lh    v1,4066(v1)      ; DAT_800b0fe2 = Raum-Index
8004b574 sltiu v0,v1,0x26       ; 38 Faelle
8004b578 beq   v0,zero,0x8004b898
8004b584 addiu at,at,4156       ; Sprungtabelle 0x8001103c
8004b58c lw    v0,0(at)
8004b594 jr    v0
...
8004b88c sb    v0,9742(at)      ; DAT_800b260e = SEITE
8004b894 sb    v1,9741(at)      ; DAT_800b260d = Skalen-Zeile
```

Sprungtabelle `0x8001103c` selbst gelesen: **`[23] -> 0x8004b75c`**, und dort steht
`8004b75c lui v1,0x800b / 8004b760 lbu v1,DAT_800b0fe2 / 8004b764 j 0x8004b888 /
8004b768 ori v0,zero,0x5` ⇒ **Raum-Index 23 (= ROOM1170) ⇒ Seite 5**. Deckt sich mit dem Bericht.

Im ganzen Funktionskoerper 0x8004b568..0x8004b898 kommt **kein einziger anderer Ladebefehl**
vor als `lh/lbu DAT_800b0fe2`. Geprueft: `region_globals.py` meldet fuer diesen Bereich
ausschliesslich 0x800b0fe2 (lesend) und 0x800b260d/0x800b260e (schreibend).

**Praezisierung gegenueber dem Bericht:** die Seite haengt nicht nur vom Raum, sondern
zusaetzlich von der **Stage** ab — der Setzer wird ueber eine Stage-Tabelle erreicht:
```
80049968 lui v0,0x800b
8004996c lh  v0,4064(v0)         ; DAT_800b0fe0 = STAGE
80049984 addiu at,at,19468       ; Tabelle 0x80074c0c
8004998c lw  v0,0(at)
80049994 jalr v0                 ; -> 0x8004b568 / 8a0 / 9d4 / bc9c / bdd4 / bf70 (6 Stages)
```
(Tabelle `0x80074c0c` durch Wortsuche in der EXE gefunden: die sechs Setzer-Adressen stehen
byte-fuer-byte an File-Offset 0x6540c..0x65420.) Stage + Raum — **kein Band**.

## 2. Teilbehauptung B — kein Band im Kartencode: BESTAETIGT UND VERSCHAERFT

Der Bericht argumentiert nur ueber den Bereich 0x80046a1c..0x8004c3fc. Das ist angreifbar
(wer den Bereich waehlt, waehlt das Ergebnis). Ich habe deshalb **die ganze EXE** gescannt:

**`DAT_800acad6` (Spieler-Band; Spieler-Struct-Basis 0x800aca54, +0x82 — bestaetigt ueber
`0x800aca88 = player+0x34 = playerX`) hat im gesamten EXE-Text 32 absolute Zugriffe:**

```
0x80014280 0x8001d2f8 0x8001d378 0x8001d454 0x8001d45c 0x8001d50c 0x8001d7a4 0x8001d7ac
0x8001d8f8 0x8001d900 0x8002bf38 0x8002d330 0x8002d364 0x8002d4fc 0x8002d574 0x8002d5b0
0x8002d5d0 0x8002d760 0x80031d04 0x80035b08 0x800366d4 0x80036858 0x800368e4 0x800381c8
0x80038208 0x80038438 0x800384bc 0x800385e0 0x80038a28 0x80038c40 0x80038ce4 0x80038edc
```
Dazu zwei weitere ueber ein gehaltenes Basisregister (`lbu a3,0(s4)` mit s4 = 0x800acad6,
gesetzt @0x8002d574): **0x8002d694, 0x8002d6c4**. ⇒ **34 Stellen, ALLE im Fenster
0x80014280..0x80038edc** = Kollision / Tuer-Transition / AOT / Treppe. **Keine einzige** liegt
in einer Kartenfunktion (die liegen bei 0x800467a8..0x8004c3fc bzw. 0x8004b568..0x8004c058).

**Gegenprobe von der anderen Seite** — die Kartentabellen haben nur je 2 Leser:
| Tabelle | Referenzen im ganzen EXE-Text |
|---|---|
| Rect-Paartabelle 0x80076840 | 0x80047068, 0x800474c4 |
| Skalentabelle 0x800768b0/b2/b4/b6 | 0x80047004/80/a0/108 und 0x8004743c/74/e0/510 |
| Seite DAT_800b260e | 7 Stores (0x8004b7dc, 0x8004b88c, 0x8004b9bc, 0x8004bc84, 0x8004bdbc, 0x8004bf58, 0x8004c040 = je Stage-Switch einer), 3 Loads (0x80047048, 0x800474a0, 0x8004c334) |
| Zeile DAT_800b260d | 112 Stores (nur Stage-Switches), 2 Loads (0x80046ff0, 0x80047424) |
| PIX-ID-Tabelle 0x80074c4c | 1 Ref: 0x8004c348 |

Der **komplette** Kartendatenpfad besteht also aus `0x80046fd8` (Kartenzeichner),
`0x800473f8` (Marker), `0x8004c328` (PIX-Lader) und den 6 Stage-Switches. In keiner davon
kommt ein Band vor.

**Indirekter Zugriff ausgeschlossen** (der eigentliche Angriffspunkt, den der Bericht nicht
schliesst): im Bereich 0x80046a1c..0x8004c3fc sind die **einzigen** absoluten `0x800ac*`-
Zugriffe 0x800ac760/762/766/76c (Pad), 0x800aca34 (Doppelpuffer), 0x800aca38 (Flags),
**0x800aca88 (X)**, **0x800aca90 (Z)**, 0x800acaec/caee (HP). `0x800acad6` fehlt,
`0x800aca8c` (Spieler-Y) fehlt ebenfalls. Es wird dort **kein Zeiger auf die Spieler-Struktur
geladen** (kein `lw` von 0x800ac784 / 0x800aca54 / 0x800acc2c), und von den 595 Zugriffen mit
untrackbarer Basis hat **keiner** einen Offset ueber 34 Byte (Histogramm: 0,±2,±4,±6,8,10,12,
13,14,16,18,34 — die "grossen" Offsets 27157/-236xx sind falsch decodierte Sprungtabellen-
Woerter, per Disasm nachgeprueft). `+0x82 = 130` kommt nirgends vor.

**Aufgerufene Funktionen ausserhalb des Bereichs** (`jal`-Liste): 0x80013b60, 0x800142f4,
0x80021634, 0x800216ec, 0x8002178c, 0x800217b0, 0x80027e68, 0x80029a98/ac8/afc, 0x8002bd44,
0x8002c18c, 0x8003ee3c, 0x80045024, 0x80068a60/c88/cec/d50, 0x8006b3d8/538.
Per `fnrange.py` (Koerper bis erstes `jr ra`) enthaelt davon **genau eine** einen Band-Zugriff:
`0x8002bd44..0x8002c188` (`8002bf38 lbu v1,846(s6)` mit s6=0x800ac788 ⇒ 0x800acad6, verglichen
mit `lbu v0,130(s2)` = entity+0x82 — das bekannte AOT-Band-Gate). Diese Funktion wird
**nur von 0x8004c7d8 und 0x8004c808** gerufen, also aus der Inventar-Oeffnen/Schliessen-Routine
(0x8004c5xx), **nicht** aus dem Kartenzeichner (0x80049540/0x8004c1bc → 0x80046fd8) und
**nicht** aus dem Marker (0x80049bcc → 0x800473f8). Kein Kartenbezug.

## 3. Teilbehauptung C — Markerformel: Instruktion fuer Instruktion nachgeprueft

`FUN_800473f8` = 0x800473f8 .. 0x80047644 (`jr ra` @0x80047640), komplett disassembliert.
Jede im Bericht zitierte Instruktion stimmt woertlich:

```
8004741c lw    v0,0(s4)            s4=0x800aca88  -> playerX
80047424 lbu   a2,9741(v0)         DAT_800b260d   -> Skalen-Zeile
80047428 addiu v0,v0,32000         (= 0x7d00)
8004742c sll/addu/sll -> t0 = X*10 (8004742c/30/48)
80047444 lhu   v0,0(at)            0x800768b4 + idx*8   -> sx
8004744c mult t0,v0 / 80047458 mflo t0 / 8004745c sra t0,t0,20
80047460 addiu t0,t0,5
80047464 mult t0,a3                a3 = 0x66666667  -> /10
800474e8 lh    v0,0(at)            0x800768b0 + idx*8   -> ox
800474f4 addu  t0,t0,v0            -> mx
8004746c lw    v1,-13680(v1)       playerZ
80047480 addiu v1,v1,32000
8004747c lhu   a0,0(at)            0x800768b6 + idx*8   -> sy
800474ac addiu v1,v1,5
800474b0 subu  v1,zero,v1          Y-Spiegelung
800474b4 mult  v1,a3               /10
80047518 lhu   v1,0(at)            0x800768b2 + idx*8   -> oy
80047520 addu  v0,v0,v1 / 80047528 sh v0,9734(at)  -> DAT_800b2606 = my
```

**Vollstaendige Eingangsliste der Funktion** (alle Loads im Koerper aufgezaehlt):
playerX 0x800aca88, playerZ 0x800aca90, Zeile 0x800b260d, **Seite 0x800b260e (@0x800474a0,
fuer die Rect-Anzahl aus 0x80076840 @0x800474c4)**, Doppelpuffer 0x800aca34
(@0x800474f0/0x800475cc/0x800475fc), Markerfarbe 0x800b2602 (@0x80047540/50/60).
⇒ **kein Y, kein Band, kein Etagenterm.** Behauptung haelt.
(Kleine Ungenauigkeit des Berichts: "Eingaenge: X, Z, Raum-Index. **Sonst nichts.**" —
die Seite und der Doppelpuffer-Index sind ebenfalls Eingaenge. Beide sind aber selbst
rein raum-/stage-abgeleitet, die Schlussfolgerung bleibt unberuehrt.)

## 4. Zwei Luecken, die der Bericht NICHT geschlossen hat — ich habe sie geschlossen

### 4a. Overlays (STAGE*.BIN) — vom Bericht gar nicht geprueft
Die Seite/Zeile sind EXE-Globals; ein Overlay koennte sie ueberschreiben. Scan aller
`info/Re1.5/PSX/BIN/*.BIN` (Load @0x80100000, **kein** 0x800-Header) nach
0x800b260d / 0x800b260e / 0x800b0fe2 / 0x80076840 / 0x800768b0:
**0 Treffer in DEBUG, STAGE1..6, TITLE.** Die einzigen Overlay-Referenzen auf 0x800acad6 sind
das Gegner-KI-Band-Gate (STAGE1: 0x80102154, 0x80102d80, 0x80102fcc, 0x8010365c, 0x801046d0,
0x801047c0, 0x80105718, 0x801078d8; STAGE2..5 analog). **Kein Overlay fasst die Karte an.**

### 4b. Woher DAT_800b0fe2 kommt — das ist der schaerfste Beleg FUER B5
Die Raumnummer der Karte wird nicht "aus dem Raum" gelesen, sondern beim Tuerdurchgang aus
dem **Tuer-Datensatz** kopiert (Zeiger 0x800ac9a8). Selbst disassembliert:

```
8001d87c lh  v0,0(a0)   -> playerX          (dest X)
8001d89c lh  v0,2(a0)   -> 0x800aca8c       (dest Y)
8001d8bc lh  v0,4(a0)   -> playerZ          (dest Z)
8001d8dc lhu v0,6(a0)   -> 0x800acabe       (dest Yaw)
8001d8ec lbu v0,11(a0)
8001d8f8 sb  v0,-13610(at)  ==> 0x800acad6  ***SPIELER-BAND aus Tuer-Byte +11***
8001d92c lhu v0,4066(v0)    (altes 0x800b0fe2 -> 0x800b0fe6 = Vorraum)
8001d930 lbu v1,10(a0)  -> 0x800b0fe4 (Cut) + 0x800afbb5
8001d94c lbu v0,9(a0)
8001d95c sh  v0,4066(at)    ==> 0x800b0fe2  ***KARTEN-RAUM aus Tuer-Byte +9***
8001d960 lbu v0,8(a0)   -> 0x800b0fe0 (Stage)
```

Die Etage (+11) und die Karten-Raumnummer (+9) liegen **im selben Datensatz, 2 Byte
auseinander, und werden im selben Augenblick gesetzt** — und die Karte nimmt ausschliesslich
+9. Deutlicher kann man "die Karte kennt die Etage, benutzt sie aber nicht" nicht belegen.
(0x800b0fe2 hat EXE-weit nur **4** Stores: 0x8001d2b0 und 0x8001d32c = Konstanten 0x17/3 im
Neustart-Init, 0x8001d660 = Startraum-Tabelle 0x800bbe5f, 0x8001d95c = Tuer-Byte. Keiner davon
liest ein Band.)

### 4c. Der einzige denkbare Schlupfwinkel — und er ist fuer ROOM1170 zugemauert
Weil 0x800b0fe2 aus einem **Tuer**-Byte kommt, koennte eine **Selbst-Tuer** theoretisch eine
ANDERE Raumnummer eintragen und so die Kartenseite "im selben Raum" umschalten. Fuer ROOM1170
habe ich das direkt aus der RDT geprueft (`ROOM1170.RDT`, main_scd @0x11f4, Opcode 0x3B
Door_aot_set, 32 Byte; Felder pc[22]=Stage pc[23]=Raum pc[24]=Cut pc[25]=Etage):

| Datei-Offset | aot | sce | pc4 (Band) | Rect (x,y w x h) | Ziel-Pos (x,y,z) yaw | Stage | **Raum** | Cut | Etage pc25 |
|---|---|---|---|---|---|---|---|---|---|
| 0x1206 | 0 | 2 | 4 | (1500, 14400 2100x1700) | (-11710,-7200,-26500) 2957 | 0 | **0x17** | 11 | 4 |
| 0x1226 | 1 | 0 | 4 | (5040, -9400 1240x2920) | (16490,-9000,-8110) 544 | 0 | 0x0b | 0 | 5 |
| 0x129c | 2 | 2 | 0 | (0,0 0x0) | (-26214,0,-3861) 0 | 0 | 0x24 | 0 | 0 |
| 0x12cc | 3 | 2 | 0 | (0,0 0x0) | (2300,-7200,14365) 1024 | 0 | **0x17** | 0 | 4 |
| 0x12f8 | 4 | 2 | 0 | (-22370,-18380 2280x910) | (2650,0,15550) 3072 | 0 | 0x13 | 7 | 0 |
| 0x1338 | 5 | 2 | 0 | (-22740,-27610 2280x1210) | (-7250,0,-1200) 1024 | 0 | 0x14 | 6 | 0 |
| 0x135a | 6 | 2 | 4 | (-11940,-28450 1750x1200) | (2300,-7200,14365) 1024 | 0 | **0x17** | 0 | 4 |

(Records mechanisch gesucht: `d[p]==0x3B && d[p+1]==laufender aot && d[p+3]==0x31` im
main-SCD-Fenster 0x11f4..0x13cc — genau 7 Treffer, aot 0..6, luecklos.)

Beide Selbst-Tueren (aot 0 = Hof→2. Bereich, aot 6 = 2. Bereich→Hof) tragen Raum-Byte
**0x17 = 23**. ⇒ Seite bleibt 5, Skalen-Zeile bleibt 23. **Das Original schaltet die Karte
in ROOM1170 nachweislich nicht um** — weder ueber ein Band noch ueber die Hintertuer
"Selbst-Tuer mit anderer Raumnummer".

## 5. Gegenprobe der Zahlenbasis des Berichts (Stichproben, alle korrekt)

* Skalen-Zeile 23 @0x80076968 (File 0x67168): `{100, 206, 2280, 2268}` — exakt wie im Bericht.
* Paartabelle @0x80076840: Seite 4 → `count=7, ptr=0x80076468`; Seite 5 → `count=2,
  ptr=0x800764bc` — exakt wie im Bericht.

## 6. Gefundene Ungenauigkeiten (keine kippt B5)

1. **"Alle 27 Xrefs von DAT_800acad6"** — es sind **34** (32 absolut + 2 ueber gehaltene Basis).
   Die Liste des Berichts enthaelt 0x8002d694/0x8002d6c4 (die ein reiner Absolut-Scan verfehlt)
   und **fehlt** u.a. 0x8001d2f8, 0x8001d378, 0x8001d454/45c, 0x8001d50c, 0x8002d330,
   0x8002d574, 0x80038208/438/4bc/5e0, 0x80038a28/c40/ce4. Das **Fenster**
   (0x80014280..0x80038edc) stimmt trotzdem, und keiner der Nachtraege liegt im Kartencode.
2. **§4c "Eingaenge: X, Z, Raum-Index. Sonst nichts."** — unvollstaendig, siehe §3 oben
   (zusaetzlich Seite 0x800b260e, Doppelpuffer 0x800aca34, Farbe 0x800b2602).
3. **§4a "die Seite wird nur an sechs Stellen geschrieben"** — sechs *Case-Labels* im
   Stage-1-Switch; EXE-weit sind es **7 Store-Instruktionen** (eine je Stage-Switch). Ausserdem
   fehlt im Bericht, dass die **Stage** (DAT_800b0fe0 @0x8004996c) die zweite Eingabe ist.
4. **§5, Zeile "idx 38 — jenseits des Switch"** — das ist keine Ueberlaufzeile, sondern
   **Stage 2, Raum 0**: der Stage-2-Setzer rechnet `8004b9c0 addiu v0,v0,38` und schreibt erst
   dann DAT_800b260d. Die Skalentabelle ist also stage-uebergreifend durchnummeriert
   (Stage 1: Zeile = Raum; Stage 2: Zeile = Raum + 38).

## 7. Fazit

**refuted = false.** Die Behauptung B5 ist korrekt und haelt sogar einer schaerferen Pruefung
stand als der Bericht selbst gefuehrt hat: nicht nur im Bereich 0x80046a1c..0x8004c3fc, sondern
im **gesamten EXE-Text und in allen sieben Overlays** existiert kein Pfad vom Spieler-Band
(0x800acad6) oder Spieler-Y (0x800aca8c) zur Kartenseite (0x800b260e), zur Skalen-Zeile
(0x800b260d) oder in die Markerformel (0x800473f8). Die Etage steckt im Original einzig im
**Seiten-Titelbild**, das ueber (Stage, Raumnummer) fest gewaehlt wird — und die Raumnummer
kommt aus dem Tuer-Byte +9, waehrend das Etagen-Byte +11 desselben Datensatzes ausschliesslich
in den Spieler-Band geht. Jede rauminterne Etagen-/Zonen-Umschaltung im Port
(`re15_map_zones.h`: "die aktive Zone folgt der Spielerposition", X/Z-Bbox-Aufloesung, kein
Band-Feld) ist damit belegt eine **PORT-ERGAENZUNG**.
