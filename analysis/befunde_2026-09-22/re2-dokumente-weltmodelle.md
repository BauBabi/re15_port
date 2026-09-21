# RE2-Dokumente in der WELT — Berichtigung, Weltmodelle, Ablauf

**Auftrag.** Der Nutzer hat meine frühere Antwort korrigiert. Er meinte mit „Modell" nicht
das Inventar, sondern das **Weltobjekt**: „es gibt auch in der normalen Welt/Umgebung die
Textdokumente als sichtbar in Resident Evil 2. Die kann man dann anklicken, dann öffnen sie
als Textdokument und speichern d, und dann sind sie in der normalen Umgebung weg."

Er hat recht. Diese Datei berichtigt den Fehler, extrahiert die Weltmodelle und belegt den
Ablauf.

**Ergebnis in sechs Sätzen.**

1. Meine frühere Aussage „ein 3D-Modell je Dokument kann es nicht geben" ist für die
   **Welt widerlegt** — sie gilt nur fürs Inventar (Abschnitt 0).
2. Der Träger ist `md1` (+20) des `Item_aot_set`-Records: der **Modell-Slot des Raums**
   (`RDT+0x30`, Anzahl `RDT+0x02`) — `lbu s2,0x14(s0)` **@0x80054CF8**,
   Lader `FUN_80052D14` **@0x80052D70/D74/DF4** (Abschnitt 1).
3. **22 Dokument-Platzierungen, 15 mit Weltmodell, 7 ohne, 12 verschiedene Meshes**
   (md5 der MD1-Bytes) — Sollzahl durch zwei unabhängige Verfahren (Abschnitt 2).
4. Die 12 Meshes sind vier Formfamilien: geschlossenes Notizbuch, loses gewelltes Blatt,
   aufgeschlagener Aktenbogen, flaches/stehendes Blatt — plus **ein Ausreißer**
   (`mesh08`, ein Gore-Klumpen, Abschnitt 3.1/4.3).
5. Von den 7 ohne Modell steckt **eines** im Hintergrundbild (Patrol report, am Bild
   belegt), die anderen **sechs** sind Registrierungs-Records, die per `Aot_on` (op 0x47,
   **@0x80055234**) aus einem Skript gezündet werden (Abschnitt 4).
6. **RE1.5 hat an dieser Stelle gar nichts**: 0 von 162 Item-Platzierungen sind Dokumente,
   obwohl 121 davon ein Weltmodell tragen und 13 Dokumentnamen in `DEBUG.BIN` stehen
   (Abschnitt 6).

Alle Adressen in RAM-Schreibweise aus `info/re2leon/PSX.EXE` (Ghidra-Dump
`ghidra_re2_Leon.txt`). Alle Zahlen sind mit den Werkzeugen unten reproduzierbar.

---

## 0. ⛔ Was falsch war

In `analysis/befunde_2026-09-21/re2-dokumente.md` (Kurzfassung Satz 4, Abschnitt 5) und in
der Commit-Message von `f30ace0f` steht wörtlich:

> „Ein 3D-Modell je Dokument gibt es in RE2 **NICHT und kann es nicht geben**: kein
> Gegenstand hat ein Mesh (die Nahansicht ist ein Rechteck 112×72), und die Dokument-Ids
> sind aus den Item-Grafiken ausdrücklich ausgeschlossen (`sltiu v0,a3,0x68` @0x80071BBC)."

**Diese Aussage ist für die WELT widerlegt.** Richtig bleibt nur die Hälfte, die vom
**Inventar** handelt:

| Teilaussage | Stand |
|---|---|
| Die Nahansicht eines Gegenstands ist ein Rechteck 112×72 (`li v0,0x70` @0x80075A5C, `li v0,0x48` @0x80075A64) | ✅ bleibt richtig |
| Dokument-Ids sind aus `ITEMALL.PIX`/`ITPS.ITP` ausgeschlossen (`sltiu v0,a3,0x68` @0x80071BBC) | ✅ bleibt richtig |
| „kein Gegenstand hat ein Mesh" | ❌ **falsch**, sobald es um die Welt geht |
| „ein 3D-Modell je Dokument … kann es nicht geben" | ❌ **falsch** |

**Warum ich es übersehen habe — der konkrete Denkfehler.** Ich habe nur nach einem Pfad
gesucht, der **pro Item-Id** ein Modell nachschlägt. So einen gibt es tatsächlich nicht.
RE2 schlägt das Modell aber **pro Platzierung** nach: das Feld `md1` (+20) des
`Item_aot_set`-Records nennt den Modell-Slot **des Raums**. Ich hatte dieses Feld im
Walker-Kopf (`tools/re2_sicherung/re2_scd_walk.py`) sogar schon korrekt benannt und
seinen Wert in jeder Zeile ausgegeben — und trotzdem nicht ausgewertet. Zusätzlich habe
ich im alten Dossier unter „offene Punkte" Nr. 2 selbst notiert, dass die Hintergrund-These
„ein Schluss aus Abwesenheit, nicht aus Code" sei, und diese Warnung dann in der
Kurzfassung als Tatsache verkauft.

**Was richtig ist:** siehe Abschnitt 1. Das alte Dossier trägt jetzt einen Nachtrag am Kopf
und einen Warnkasten über Abschnitt 5.

---

## 1. Der Beleg: `md1` ist der Modell-Slot des Raums

### 1.1 Op 0x4E = `Item_aot_set`, 22 Byte

SCD-Sprungtabelle `0x800A74C8`; Eintrag 0x4E liegt @`0x800A7600`
(`0x800A7600 − 0x800A74C8 = 0x138 = 78·4`) und zeigt auf `LAB_80054CD4`.

Die Feldlage steht im Handler selbst:

```
80054CEC  lw    s0,0x1c(a0)    ; s0 = Task-PC = &record
80054CF4  lhu   a1,0x12(s0)    ; +18 flag
80054CF8  lbu   s2,0x14(s0)    ; +20 md1
80054CFC  addiu v0,s0,0x16     ; Recordlaenge = 22
80054D00  sw    v0,0x1c(a0)
80054D04  lbu   s1,0x1(s0)     ; +1  aot
80054DD4  lbu   s1,0x15(s0)    ; +21 action
```

→ `+0 op +1 aot +2 sce +3 sat +4 nFloor +5 super +6 x +8 z +10 w +12 d
   +14 i_item +16 n_item +18 flag +20 md1 +21 action`

### 1.2 `md1` indiziert den Objekt-Pool `0x800D0324`, Schrittweite `0x1F8`

```
80054D98  sltiu v0,s2,0x20          ; Schranke: md1 < 32
80054D9C  sll   v0,s2,0x6           ; md1*64
80054DA4  subu  v0,v0,s2            ;  - md1   = md1*63
80054DA8  sll   v0,v0,0x3           ;  <<3     = md1*504 = md1*0x1F8
80054DAC  addu  v0,s3,v0            ; s3 = 0x800CC1E8
80054DB4  sw    0x80000000,0x413c(v0)   ; 0x800CC1E8+0x413C = 0x800D0324 + md1*0x1F8
80054DBC  sb    zero,0x4281(v0)         ; 0x800D0469 + md1*0x1F8  (= Pool[md1]+0x145)
```

**`md1 == 255` fällt durch `sltiu v0,s2,0x20` (@0x80054D98 bzw. @0x80054DC0) und erreicht
den Pool nie — 255 heißt also „kein Weltmodell".**

### 1.3 Dass dieser Pool die Modelltabelle des RAUMS ist — Lader `FUN_80052D14`

```
80052D30  li    s2,0x20         ; 32 Pool-Eintraege
80052D54  sw    zero,0x0(s1)    ; Pool loeschen ...
80052D58  sb    zero,0x145(s1)
80052D64  addiu s1,s1,0x1f8     ; ... mit Schrittweite 0x1F8, ab 0x800CC1E8+0x413C
80052D68  lw    v0,0x213c(s3)   ; v0 = *0x800CE324 = RDT-Basiszeiger
80052D70  lbu   s2,0x2(v0)      ; Schleifenzahl = RDT+0x02 = nOmodel
80052D74  lw    s4,0x30(v0)     ; s4 = RDT+0x30 = Offset der Modelltabelle
80052D8C  lw    a0,0x0(s4)      ; erster u32 des Eintrags (TIM-Offset)
80052DBC  jal   FUN_80076A40    ; Prop laden
80052DF4  addiu s4,s4,0x8       ; Schrittweite 8 = zwei u32 (TIM, MD1)
```

→ **Pool-Eintrag `k` ⟺ Paar `k` in `RDT+0x30`, Anzahl `RDT+0x02`.** Genau diese Struktur
liest `tools/re2_sicherung/rdt_props.py` (und der Port in `engine/src/rdt_common.c:51-79`).

### 1.4 Bit 0 des Zustandsworts entscheidet über „wird gezeichnet"

Der Pro-Bild-Durchlauf über den Objekt-Pool:

```
80052700  lw    v0,-0x1cdc(v0)   ; RDT-Zeiger
8005270C  lbu   v1,0x2(v0)       ; nOmodel
80052714  addiu s0,s0,0x324      ; s0 = 0x800D0324
80052718..24                     ; s1 = Ende = 0x800D0324 + nOmodel*0x1F8
80052734  lw    v0,0x0(s0)       ; Zustandswort
8005273C  andi  v0,v0,0x1        ; Bit 0
80052740  beq   v0,zero,+0x18    ; Bit 0 == 0  -> EINTRAG UEBERSPRINGEN
80052750  jal   FUN_80051088     ; sonst: zeichnen/einreihen (a1 = 4)
80052758  addiu s0,s0,0x1f8
```

Gesetzt wird Bit 0 von `Obj_model_set` (op 0x2D, `LAB_80055260`):
`lhu v0,0xa(s2)` / `ori v0,v0,0x1` / `sw v0,0x0(s1)` **@0x800552D0-E4** — derselbe Pool,
dieselbe Schrittweite (`sll v0,t1,0x6` / `subu` / `sll ..,3` / `addiu v0,v0,0x413c`
**@0x80055298-A8**, `t1 = lbu 0x1(s2)` = Slot).

Gelöscht wird es von `Item_aot_set`, wenn die Speicher-Flagge des Dokuments gesetzt ist
(`sw 0x80000000` @0x80054DB4 — `0x80000000` hat Bit 0 = 0).

**Damit ist die Frage des Nutzers beantwortet: das Dokument verschwindet aus der Umgebung,
weil beim nächsten Raum-Aufbau `Item_aot_set` Bit 0 seines Pool-Eintrags löscht.**

---

## 2. Die Zahlen

`python tools/re2_sicherung/re2_doc_worldmodels.py extracted_re2_dokumente/weltmodelle`
über alle 495 `ROOM*.RDT` von `info/re2leon/PL0/RDT`:

```
Item-AOT-Records gesamt:            269      (265 x op 0x4E + 4 x op 0x69 Item_aot_set_4p)
davon MIT Weltmodell (md1 != 255):  206
davon Dokumente (Id >= 104 = 0x68):  22
davon MIT Weltmodell (md1 != 255):   15
davon ohne (md1 == 255):              7
VERSCHIEDENE Meshes (md5 der MD1):   12
```

Die Schranke „Dokument = Id ≥ 0x68" ist die des Spiels selbst: `sltiu v0,a3,0x68`
**@0x80071BBC** trennt Item-Pfad von Dokument-Pfad; `addiu a0,a3,-0x68` **@0x80071D04**
rechnet die Id in den Dokument-Index 0…24 um.

### 2.1 Dunkelziffer — beziffert, nicht behauptet

Der Opcode-Walker bricht in **45 von 2553** SCD-Blöcken ab; **313 356 von 331 419 Bytes =
94,55 %** werden gewalkt. Damit die fehlenden 5,45 % keine stille Lücke bleiben, läuft im
selben Werkzeug eine **unabhängige Brute-Force-Gegenprobe**: jede Byte-Position jedes
Blocks wird als `Item_aot_set` gelesen und nur akzeptiert, wenn **alle** Feldschranken des
Handlers erfüllt sind (`op == 0x4E`, `sce == 2`, `104 ≤ i_item ≤ 126`, `1 ≤ n_item ≤ 16`,
`md1 < 0x20 oder == 255`, `action ≤ 8`).

```
Brute-Force-Gegenprobe (jede Byte-Position): 22 Treffer, davon NEU: 0
```

**22 ist die Sollzahl.** Zwei Verfahren, dieselbe Menge.

Gegenprobe zur Herkunft: jeder extrahierte `room*/scd/*.scd` wurde byte-gleich im
zugehörigen `ROOM*.RDT` wiedergefunden (2523 von 2553; die 30 Ausnahmen sind
`roomF000`/`roomF010`/`roomF040`, deren RDT nicht in `PL0` liegt — keiner der
13 Dokument-Räume).

### 2.2 Die 22 Platzierungen

Vollständig in `extracted_re2_dokumente/weltmodelle/_index.txt` (mit RDT-Byte-Offset des
Records, Koordinaten, Flag, md1, action, md5).

| Id | Name | Raum | md1 | Mesh |
|---|---|---|---|---|
| 104 | CHRIS's diary | room1150 (2×, Slot 7) | 7 | mesh00 |
| 106 | Memo to LEON | room2020 | 0 | mesh01 |
| 108 | Operation report 1 | room2060 (2×) | 1 | mesh02 |
| 110 | Mail to the chief | room3010 (2×) | **255** | — |
| 112 | Secretary's diary B | room10E0 | 0 | mesh03 |
| 113 | Operation report 2 | room2080 | 3 | mesh02 |
| 114 | User registration | room60A0 | 6 | mesh04 |
| 115/116/117 | Film A / B / C | room2080 | **255** | — |
| 118 | Patrol report | room2030 | **255** | — |
| 119 | Watchman's diary | room2100 | 0 | mesh05 |
| 120 | Chief's diary | room21B0 | 7 | mesh06 |
| 121 | Sewer manager diary | room4090 (2×, Slot 8 / Slot 5) | 8 bzw. 5 | mesh07 / mesh08 |
| 122 | Sewer manager fax | room4030 | 10 | mesh09 |
| 123 | Film D | room2080 | **255** | — |
| 125 | Lab security manual | room60A0 | 8 | mesh10 |
| 126 | P-epsilon report | room6030 | 9 | mesh11 |

*(Die Tabelle in meiner Auftragsbeschreibung nannte nur 18 der 22; 122, 123, 125 und 126
fehlten dort. Die Summen 22/15/7 stimmen überein.)*

---

## 3. Die Extrakte

`extracted_re2_dokumente/weltmodelle/`

Je verschiedenem Mesh vier Dateien: `meshNN_<md5-8>.md1` (Rohdaten, byte-true aus dem RDT
geschnitten), `.tim` (die Textur des Slots, ebenfalls roh), `.obj`, sowie zwei texturierte
Ansichten `_a.png` / `_b.png`. Dazu `_kontaktbogen.png` (alle 12 nebeneinander, mit
Dokumentnamen und Raum#Slot) und `_index.txt`.

**Entdopplung über den MD5 der MD1-Bytes, nicht über den Slot** — wie verlangt. Dass das
nötig war, zeigen zwei Fälle:
* `mesh02` liegt in **room2060 als Slot 1** und in **room2080 als Slot 3** — selber Slot
  wäre falsch gewesen, selbes Mesh ist richtig.
* `room4090` nutzt **zwei verschiedene Slots (8 und 5)** für **dieselbe Dokument-Id 121**.

**Schnitt-Gegenprobe (lückenlos).** `rdt_props.py` schneidet „bis zum nächstgrößeren
Offset". Für alle 12 Meshes gilt: der größte Endoffset aus den MD1-Mesh-Köpfen
(`max(block_off + count·stride) + 12`) ist **exakt gleich der geschnittenen Dateilänge** —
12 von 12, kein Byte zu viel, kein Byte zu wenig. Beispiel mesh00: 372 B Datei, Nutzdaten
enden bei 372.

### 3.1 Was die 12 Meshes zeigen — angesehen, nicht vermutet

Wichtig für die Ansicht: **`md1_view.render` nimmt immer Palettenzeile 0.** Die RDT-TIMs
der Räume tragen aber 1…4 Paletten (TIM-CLUT-Kopf `cw=256, ch=1..4`), und jede MD1-Fläche
nennt ihre Palette im `clut`-Feld des UV-Records (u16 @+2; Bits 0–5 = VRAM-x/16, Bits 6–14
= VRAM-y; Palettenzeile = `(clut>>6) − clut_y`). Mit Zeile 0 sah z.B. mesh08 nach rohem
Fleisch aus. `re2_doc_worldmodels.render_clut` wählt die richtige Zeile; 6 der 12 Meshes
brauchen eine andere als 0 (mesh00 → 2, mesh02 → 1, mesh04 → 1, mesh07 → 2, mesh08 → 1,
mesh10 → 3).

| Mesh | Geometrie | Bounding-Box (x/y/z) | Was zu sehen ist |
|---|---|---|---|
| mesh00 | 0 Tri, 6 Quad, 372 B | 274 × 26 × 378 | **Geschlossenes Notizbuch**, türkiser Einband, weißes Titelschild, kleiner oranger Aufkleber/Verschluss |
| mesh01 | 8 Tri, 0 Quad, 404 B | 286 × 98 × 436 | **Einzelnes, leicht gewelltes Blatt Papier** mit Zeilendruck, an einer Kante angehoben |
| mesh02 | 4 Tri, 0 Quad, 236 B | 540 × 0 × 360 | **Aufgeschlagener Aktenbogen / Formular** auf beigem Grund, mit Linien und Unterschrift; völlig flach (y = 0) |
| mesh03 | 0 Tri, 6 Quad, 372 B | 274 × 26 × 378 | **Geschlossenes Notizbuch**, gleiche Geometrie wie mesh00, olivbrauner Einband |
| mesh04 | 8 Tri, 0 Quad, 404 B | 286 × 98 × 436 | **Einzelblatt**, gleiche Geometrie wie mesh01, hellgraues Papier |
| mesh05 | 4 Tri, 5 Quad, 468 B | 450 × 9 × 324 | **Flaches, graugrünes Heft** mit handschriftlicher Aufschrift auf dem Deckel |
| mesh06 | 4 Tri, 5 Quad, 468 B | 270 × 9 × 180 | **Geschlossenes dunkelrotes Buch** (Ledereinband), kleiner als mesh05 |
| mesh07 | 4 Tri, 0 Quad, 236 B | 540 × 0 × 360 | **Aufgeschlagener Aktenbogen**, gleiche Geometrie wie mesh02, eigene Textur |
| mesh08 | 16 Tri, 113 Quad, 8388 B | 131 × 23 × 124 | **Kein Dokument.** Ein rundlicher, organischer Klumpen. Die Textur des Slots (`mesh08_61be2721.tim`, 2 Paletten) zeigt Haut, eine Armbanduhr mit braunem Lederband und eine Fleisch-/Blutfläche — es ist der Gore-/Leichenteil-Satz von room4090 (Slot 3 desselben Satzes ist ein abgetrennter Arm). Siehe 4.3 |
| mesh09 | 0 Tri, 8 Quad, 468 B | 0 × 535 × 378 | **Aufrecht stehendes bedrucktes Blatt** (Fax), flach in x |
| mesh10 | 0 Tri, 8 Quad, 468 B | 0 × 535 × 378 | **Aufrecht stehendes Blatt**, gleiche Geometrie wie mesh09 |
| mesh11 | 4 Tri, 5 Quad, 468 B | 450 × 9 × 324 | **Flach liegendes weißes Schriftstück** mit Absatzdruck |

Vier Formfamilien, mehr sind es nicht: *geschlossenes Notizbuch* (mesh00/03),
*loses gewelltes Blatt* (mesh01/04), *aufgeschlagener Aktenbogen* (mesh02/07),
*flaches Heft/Blatt* (mesh05/06/11) und *stehendes Blatt* (mesh09/10).

**Kontaktbogen:** `extracted_re2_dokumente/weltmodelle/_kontaktbogen.png`

---

## 4. Die sieben ohne Weltmodell

Vorweg der Fund, der die naheliegende Deutung kippt: **ein 1×1-Rechteck am Ursprung
bedeutet NICHT „unsichtbar".** `CHRIS's diary` (room1150) hat genau so einen Record —
`x=0 z=0 w=1 d=1` — und trotzdem `md1 = 7` mit sichtbarem Mesh. Der Grund steht in 4.1.

### 4.1 `Aot_on` (op 0x47) — der fehlende Auslöser

Es gibt in RE2 **genau einen** Weg, auf dem ein Dokument in die FILE-Liste kommt, und er ist
durch Xref-Erschöpfung belegt:

* `DAT_800D4B68` (die FILE-Liste) hat **einen einzigen Schreiber**: `sb a0,0x0(v1)`
  **@0x80069300** in `FUN_800692DC`.
* `FUN_800692DC` hat **einen einzigen Aufrufer**: `jal` **@0x80071D00**.
* Der liest `DAT_800D4231`, und `DAT_800D4231` hat **einen einzigen Schreiber**:
  `sb a1,offset DAT_800d4231(at)` **@0x800518A8** — im **sce=2-Handler** `LAB_80051884`.

Ein AOT mit `sce = 2` muss also feuern. Das geht auf zwei Wegen:

1. **Der Spieler steht drin.** Die AOT-Schleife in `FUN_80051088` testet die Box
   (`lbu v1,0x1(s0)` = sat gegen die Maske @0x80051160-6C), ruft dann
   `lbu v0,0x0(s0)` = sce, `lw v0,PTR_LAB_800a73c4[sce]`, `jalr v0` mit
   `addiu a0,s0,0xc` **@0x80051484-9C**. `a0 = AOT+0x0C = record+14 = &i_item` — genau
   deshalb liest `LAB_80051884` mit `lbu a1,0x0(a0)` die Item-Id.
2. **Das Skript ruft `Aot_on <index>`** (op 0x47, `LAB_800551C8`, 2 Byte):
   ```
   800551E0  lbu  v0,0x1(v0)          ; aot-Index aus dem Record
   800551F4  lw   v1,-0x1aa8(at)      ; AOT_PTR[aot]  (Basis 0x800CE558)
   80055218  lbu  v0,0x0(v1)          ; sce
   8005522C  lw   v0,PTR_LAB_800a73c4[sce]
   80055234  jalr v0                  ; Handler AUFRUFEN — ganz ohne Kollision
   ```

Beispiel room1150 (`sub00`), byte-genau:
```
+0x0174  2c 01 05 31 ... 4d ad 31 bb 38 09 a6 04 ff 00 18 02 00 00
         Aot_set  aot 1, sce=5, Box (-21171,-17615) 2360x1190   -> der SCHREIBTISCH
+0x0188  4e 07 02 31 ... 00 00 00 00 01 00 01 00 68 00 01 00 14 00 07 00
         Item_aot_set aot 7, Box (0,0) 1x1, Id 104, flag 20, md1=7
+0x019E  2d 07 ... 0a 00 10 00 e9 af d6 fa 13 be ...
         Obj_model_set Slot 7 an (-20503,-1322,-16877)           -> das MESH auf dem Tisch
```
und `sub02` (das von sce=5 gestartete Unterprogramm — `LAB_80051980` liest
`lhu a0,0x0(a1)` / `lbu a1,0x3(a1)` und ruft `FUN_80053138` @0x800519A8):
```
+0x001C  47 07     Aot_on 7      <- hier wird der Dokument-AOT gezuendet
```

Damit ist auch klar, warum die Box 1×1 am Ursprung liegen darf: sie wird nie berührt.

### 4.2 Die sieben im Einzelnen

**(a) Id 118 „Patrol report", room2030 — im vorgerenderten Hintergrund. Belegt am Bild.**

Der Record hat **echte Koordinaten** und eine **flächige** Box:
`4e 06 02 31 00 00 c6 a5 9d 9a 9c 09 dc 1e 76 00 01 00 99 00 ff 00`
→ x = −23098, z = −25955, w = 2460, d = 7900, Id 118, flag 153, **md1 = 255**.
Direkt danach dieselbe Fläche noch einmal als `Aot_set 2c 07 04 31 ... c6 a5 9d 9a 9c 09 dc 1e`
(sce = 4). Zum Vergleich: alle Platzierungen **mit** Modell haben item-große Boxen
(z.B. room2060/room4090: `08 07 08 07` = 1800 × 1800). 2460 × 7900 ist eine ganze
Bodenzone, kein Gegenstand.

Ich habe die Box in jede Kamera des Raums projiziert
(`tools/re2_sicherung/re2_aot_on_bg.py`, Kameramodell 1:1 aus
`re15_port/engine/src/camera_common.c`, dort gegen `FUN_80053ca4` @0x80053ca4 belegt;
RID-Record 32 B nach `re15_port/include/re15_camera.h`, `H = fov>>7`,
`sx = 160 + H·x/z`). Gegenprobe der Projektion: das Tür-AOT (op 0x3B) landet in cam00
exakt auf der Tür, das Ink-Ribbon-AOT auf der Kiste.

Ergebnis: das Rechteck liegt auf dem Bodenstreifen, auf dem im Hintergrundbild
**lose Blätter und türkisfarbene Hefte gemalt sind** (room2030 = das Archiv/Aktenzimmer des
RPD). → `extracted_re2_dokumente/weltmodelle/belege/room2030_cam00.png` und `…_cam02.png`.

**Der Gegenstand steckt also im Hintergrundbild** — für diesen Fall ist das jetzt am Bild
belegt und nicht mehr „ein Schluss aus Abwesenheit".

**(b) Id 110 „Mail to the chief", room3010 (2×) — Ereignis-Übergabe, gar kein Weltobjekt.**

`4e 03 02 31 00 00 00 00 00 00 01 00 01 00 6e 00 01 00 2c 00 ff 00`
→ AOT-Index **3**, Box **1×1 am Ursprung**, md1 = 255. Im selben Zweig stehen
`Obj_model_set 2d 01 …` und `2d 02 …` mit Position **(0,0,0)** und danach
`Aot_set 2c 04 05 31 ... 0c 9a 5c 95 08 07 08 07 ff 00 18 1f` — ein sce=5-Ereignis
(Unterprogramm 0x1F) an der echten Stelle (−26100, −27300), 1800 × 1800.
room3010 ist der Zellentrakt; in der Projektion auf alle 15 Kameras liegt an diesen
Stellen im Hintergrund **nichts Dokumentartiges** — kahler Boden bzw. Gitter.

**Wer AOT 3 zündet, ist gefunden:** `room3010/sub04 +0x04CC  47 03  Aot_on 3` — mitten in
einer Folge von `Message_on`-Dialogzeilen (`2b 00 17` … `2b 00 21`, +0x0014 … +0x0520).
Das Dokument wird also **während eines Dialogs übergeben**, nicht vom Boden aufgehoben.

**(c) Ids 115/116/117/123 „Film A/B/C/D", room2080 — geparkt auf der Nirgendwo-Koordinate.**

Alle vier: `20 4e 20 4e e8 03 e8 03` → **x = z = 20000**, Box 1000 × 1000, md1 = 255.

Dass (20000, 20000) die spieleigene „Nirgendwo"-Ablage ist, steht im selben Block:
`Obj_model_set 2d 04 ... 20 4e 20 4e 20 4e` **@room2080/sub00+0x00EA** parkt Modell-Slot 4
auf **(20000, 20000, 20000)**. Über alle Räume: **6 Item-AOTs** liegen auf (20000, 20000) —
Id 13 (Colt S.A.A.), 80 (Film), 115, 116, 117, 123. Alles Dinge, die nicht im Raum liegen.

Erreichbarkeit, unabhängig gemessen:
* `collision.sca` von room2080 (Kopf 16 B + 16-B-Einträge; (256−16)/16 = 15 geht exakt auf):
  Hülle **x −25395…−9805, z −29571…−11111**.
* Header-unabhängige Gegenprobe über `camera.rid`: die 11 Kameras stehen bei
  x −24138…−16146, z −23760…−14022; die übrigen AOTs des Raums bei x −22176…−15306.

(20000, 20000) liegt **rund 30 000 Einheiten außerhalb**. Der Spieler kann dort nie stehen,
also feuern diese AOTs nur über `Aot_on`. Kein anderer Opcode in room2080 setzt die
AOT-Indizes 6/7/8/0x10 neu (alle 19 Blöcke durchsucht).

**Und genau das steht im Skript — room2080 ist die Dunkelkammer** (Hintergrundbilder 3–5
zeigen die rot beleuchtete Entwicklerwanne). Vier Unterprogramme, byte-genau, jeweils
20 Byte lang und bis auf zwei Bytes identisch:

| Block | Bytes | Bedeutung |
|---|---|---|
| `sub09 +0x0008` | `62 44` / `+0x000A` `47 06` | `Sce_Item_lost(0x44 Film)` → `Aot_on 6` = Id 115 **Film A** |
| `sub10 +0x0008` | `62 45` / `+0x000A` `47 07` | `Sce_Item_lost(0x45 Film)` → `Aot_on 7` = Id 116 **Film B** |
| `sub11 +0x0008` | `62 46` / `+0x000A` `47 08` | `Sce_Item_lost(0x46 Film)` → `Aot_on 8` = Id 117 **Film C** |
| `sub12 +0x0008` | `62 50` / `+0x000A` `47 10` | `Sce_Item_lost(0x50 Film)` → `Aot_on 0x10` = Id 123 **Film D** |

Die Filmrolle wird verbraucht, dann wird der geparkte Dokument-AOT gezündet und der
FILE-Eintrag entsteht. Dieselbe Bauweise nutzt room2080 auch für die übrigen geparkten
Records: `sub08 +0x0016 47 04`, `sub07 +0x001F 47 0c` (Colt S.A.A.),
`sub03 +0x000C 47 0e` (Id 80 „Film"), `sub16/sub17 +0x000C 47 1e`.

Die drei `Aot_set … sce=6` an der Wanne (−15306, −24197, 1800 × 1760) setzen beim
Betreten die Bits 68/69/70 in Bank 10 — sce=6 ist der Flag-Setzer/-Löscher
(`lhu v1,0x0(a0)` = Bank, `lhu a1,0x2(a0)` = Bit, `lhu a0,0x4(a0)` = setzen/löschen,
**@0x800519C0 … @0x80051A24**); das sind genau die Item-Ids der drei Filmrollen.

**Zusammengefasst auf die Frage des Nutzers:** von den sieben ohne Modell steckt **eines**
(Patrol report, room2030) im Hintergrundbild; die übrigen **sechs** sind gar keine
Weltobjekte, sondern Registrierungs-Records, die per `Aot_on` aus einem Skript gezündet
werden — bei den vier Filmen nach dem Entwickeln (`Sce_Item_lost` → `Aot_on`), bei
„Mail to the chief" (2×) mitten im Dialog. Unsichtbar sind sie also nicht, weil etwas
fehlt, sondern weil sie nie im Raum liegen sollen.

### 4.3 room4090: zwei Varianten für dasselbe Dokument

```
+0x00C4  06 00 44 00     ; Ifel-Block, Laenge 0x44 -> Koerper [+0x00C4, +0x0108)
+0x00C8  21 01 00 00     ; Ck(bank=1, bit=0, erwartet=0)
+0x00CC  2d 08 ...       ; Obj_model_set Slot 8
+0x00F2  4e 0a ... md1=08
+0x0108  07 00 42 00     ; Else_ck, Laenge 0x42 -> Koerper [+0x0108, +0x014A)
+0x010C  2d 05 ...       ; Obj_model_set Slot 5   (gleiche Transformation)
+0x0132  4e 0a ... md1=05
```
Die Blocklängen gehen exakt auf, die Zuordnung ist damit eindeutig. `Ck` ist 4 Byte,
`21 <bank:u8> <u16>` mit `bit = u16 & 0x1F`, `erwartet = u16 >> 8`
(`lbu v1,0x1(v0)` / `lhu a1,0x2(v0)` / `andi a0,a1,0x1f` / `sra a1,a1,0x8`
**@0x8005435C-8C**); `bank` indiziert `PTR_DAT_800a78c8` **@0x80054384**, Eintrag 1 =
`DAT_800CFBD8`.

Slot 8 ist der aufgeschlagene Aktenbogen (mesh07), Slot 5 der Gore-Klumpen (mesh08).

⛔ **Was Bank 1 / Bit 0 bedeutet, ist NICHT belegt.** Gemessen ist nur: dieses Bit wird
spielweit abgefragt — `== 0` in 30 Räumen (68 Stellen), `== 1` in 13 Räumen (25 Stellen).
Das Profil einer globalen Modus-Flagge (A/B-Szenario wäre die naheliegende Vermutung), aber
`DAT_800CFBD8` hat über 20 Schreiber, und ich habe den richtigen nicht isoliert. Das bleibt
offen und wird hier **nicht** als Tatsache verkauft.

---

## 5. Der Ablauf, den der Nutzer meint — mit Adressen

Vollständige Kette, jede Stufe belegt:

**(1) Das Objekt liegt sichtbar im Raum.**
`Obj_model_set` (op 0x2D) setzt Position und Zustandswort des Pool-Eintrags:
`lbu t1,0x1(s2)` = Slot **@0x80055290**, `lhu v0,0xa(s2)` / `ori v0,v0,0x1` /
`sw v0,0x0(s1)` **@0x800552D0-E4** (Bit 0 = sichtbar),
Position `lh` +0x0E/+0x10/+0x12 → Pool +0x38/+0x3C/+0x40 **@0x8005530C-28**.
`Item_aot_set` schreibt zusätzlich das action-Byte nach Pool +0x145
(`sb s1,0x4281(v1)` **@0x80054DE4**) und blendet vorsorglich aus, wenn `action & 2`
(`andi v0,s1,0x2` **@0x80054DDC**, `sw` **@0x80054DEC**).

**(2) Untersuchen.** Entweder steht der Spieler in der AOT-Box (AOT-Schleife
**@0x80051130-9C**, Aufruf **@0x8005149C** mit `a0 = AOT+0x0C` **@0x80051498**), oder das
Skript ruft `Aot_on` (**@0x80055234**). In beiden Fällen läuft der **sce=2-Handler**
`LAB_80051884` (Tabelle `0x800A73C4`, Eintrag 2 @`0x800A73CC`):

```
80051884  lbu  a1,0x0(a0)        ; a0 = &i_item -> die Item-Id
800518A8  sb   a1,DAT_800d4231   ; <- der EINZIGE Schreiber dieser Zelle
800518B0  sw   v1,DAT_800ce5d8   ; der ausgeloeste AOT (aus DAT_800ead94)
800518CC  lbu  v0,0x7(a0)        ; = record+21 = action
800518D4  andi v0,v0,0x1
800518D8  bne  v0,zero,LAB_80051924   ; action-Bit 0 gesetzt -> anderer Spielerzustand
800518F8  sb   2,DAT_800d5c00         ; sonst: Gegenstands-/FILE-Bildschirm anfordern
80051904  sb   1,DAT_800df348
80051918  sw   a0,DAT_800cfbd8        ; mit `ori a0,a0,0x8000` @0x80051908
```
(Der `action & 1`-Zweig setzt `DAT_800CFBFC+1 = 6` **@0x8005192C** — ein anderer
Spieler-Zustand; welcher genau, ist hier nicht belegt. Von den 22 Dokument-Records haben
21 `action = 0`, nur Id 126 in room6030 hat `action = 1`.)

**(3) Der Textbildschirm.** Der Zustandseintrag `0x800A9384` → `LAB_80071BA0`:

```
80071BA4  lbu   a3,DAT_800d4231
80071BBC  sltiu v0,a3,0x68
80071BC0  beq   v0,zero,LAB_80071D00      ; Id >= 0x68 -> DOKUMENT
  (Id < 0x68: ITPS.ITP 0xAA laden, FUN_80075A00 = die 112x72-Nahansicht)
80071D00  jal   FUN_800692DC
80071D04  _addiu a0,a3,-0x68              ; doc = id - 0x68, an die FILE-Liste anhaengen
80071D10  srl   v1,v0,0x3 / andi v0,v0,0x7 -> Zeile/Spalte (DAT_800D5C01/02)
80071D70  jal   FUN_80076A40 (0x917)      ; Seitenbilder laden
80071D84  jal   FUN_80076A40 (0xA17)
80071DDC  jal   FUN_800761B8              ; Seiten-Zeichner, in Schleife
80071DE4  jal   FUN_8002C350              ; bis Tastendruck (beq v0,zero,LAB_80071DD4)
```

**(4) Speichern + aus der Umgebung nehmen.** `switchD_8007208c::caseD_3`:

```
80072280  lw    v1,DAT_800ce5d8   ; der AOT des aufgenommenen Gegenstands
80072298  sb    zero,0x0(v1)      ; AOT+0 = sce := 0  -> die Untersuchungsstelle ist tot
800722A4  addiu s0,v1,0xc         ; s0 = &i_item
80072348  lhu   a1,0x4(s0)        ; AOT+0x10 = record+18 = FLAG   (bzw. @0x80072354)
8007235C  jal   FUN_8007730C      ; a0 = Flag-Bank -> Bit SETZEN
```
`FUN_8007730C(bank,n)` = `bank[n>>5] |= 0x80000000 >> (n&31)` (@0x8007730C-30),
`FUN_80077334` löscht, `FUN_80077360` prüft (@0x80077360-80). Die Bank wählt
`lh v0,DAT_800d481c` / `slti v0,v0,0x4` → `0x800D48BC` bzw. `0x800D48DC`
(@0x80072330-58; identisch in `Item_aot_set` @0x80054D48-74).

**(5) Beim nächsten Raum-Aufbau ist es weg.** `Item_aot_set` prüft dieselbe Flagge:

```
80054D78  jal   FUN_80077360
80054D80  beq   v0,zero,LAB_80054DC0     ; Flagge NICHT gesetzt -> Gegenstand liegt noch da
80054D94  sb    zero,0x0(v0)             ; gesetzt: AOT abschalten
80054DB4  sw    0x80000000,0x413c(v0)    ; Pool[md1] Bit 0 = 0
80054DBC  sb    zero,0x4281(v0)          ; action-Byte = 0 (kein Glitzer)
```
und der Pro-Bild-Durchlauf **@0x8005273C/40** überspringt den Eintrag. **Das ist genau das,
was der Nutzer beschreibt.**

Für den Port heißt das: `md1` aus dem `Item_aot_set`-Record in den Prop-Pool tragen, dort
Bit 0 des Zustandsworts als Sichtbarkeit führen, und beim Laden des Raums gegen die
Speicher-Flagge (+18) prüfen. `Aot_on` (op 0x47) muss den sce-Handler direkt aufrufen
können, sonst funktionieren 6 der 22 Dokumente nicht.

---

## 6. Was RE1.5 an dieser Stelle hat: **nichts**

`python tools/re2_sicherung/re15_doc_check.py` über alle 240 `info/Re1.5/PSX/STAGE*/ROOM*.RDT`.
RE1.5 hat `Item_aot_set` als **Opcode 0x50**; das Gegenstück zu `md1` heißt dort `prop`
(Kurzform +20, Langform +28; Feldlage nach `re15_port/engine/src/scd_vm.c:3800-3841`).

```
Item_aot_set (0x50) gesamt:              162
davon MIT Weltmodell (prop != 255):      121
davon ohne (prop == 255):                 41
verschiedene Item-Ids:                    27  (0x04 .. 0x47)
DOKUMENT-Platzierungen (Id 0x48..0x51, 0x62..0x64):   0
Brute-Force-Gegenprobe ueber JEDE Byte-Position:      0
```

* **Das Weltmodell-Feld selbst ist in RE1.5 rege in Gebrauch: 121 von 162 Platzierungen
  (74,7 %) haben ein `prop != 255`.** Der Mechanismus existiert also schon; er wird nur für
  Dokumente nie benutzt.
* **Kein einziges Dokument ist in RE1.5 in einem Raum platziert.** Die höchste vorkommende
  Item-Id ist `0x47` (Blue Master Keycard). Der Walker bricht in 28 Blöcken ab, deshalb
  zusätzlich die Brute-Force-Gegenprobe über jede Byte-Position aller 240 RDTs — ebenfalls
  0 Treffer. (Das ist hier besonders nötig, weil RE1.5-Textblöcke voller `0x50` = `'m'`
  stecken; ein naiver Bytescan wäre wertlos.)
* **Die Namen existieren trotzdem** — im Namensblock von `info/Re1.5/PSX/BIN/DEBUG.BIN`
  ab Datei-`0x4A28` (`re15_items.py`):
  `0x48 Chris' Diary`, `0x49 Operation Report`, `0x4A…0x51 File 3 … File 10`,
  `0x62…0x64 Umbrella File 7…9` — 13 Dokument-Plätze, alle unbelegt.

Für den Port bedeutet das: die Dokument-Weltobjekte müssen **neu gesetzt** werden; RE1.5
liefert nur die Namen. Die 12 RE2-Meshes aus Abschnitt 3 sind die Auswahlgrundlage dafür.

---

## 7. Werkzeuge

| Werkzeug | Zweck |
|---|---|
| `tools/re2_sicherung/re2_doc_worldmodels.py` | Zensus + Schnitt + OBJ + CLUT-richtige Ansichten + Kontaktbogen; enthält die Belegkette im Kopf, die Brute-Force-Gegenprobe und `render_clut` |
| `tools/re2_sicherung/re2_aot_on_bg.py` | AOT-Rechtecke in die Kameras des Raums projizieren und auf den Hintergrund zeichnen (`--only-item=<id>`) |
| `tools/re2_sicherung/re15_doc_check.py` | dieselbe Frage für RE1.5, mit eigener Brute-Force-Gegenprobe |
| vorhanden: `re2_scd_walk.py`, `re2_items.py`, `rdt_props.py`, `md1_view.py`, `re15_item_census.py`, `re15_items.py` | unverändert weiterverwendet |

```bash
python tools/re2_sicherung/re2_doc_worldmodels.py extracted_re2_dokumente/weltmodelle
python tools/re2_sicherung/re2_aot_on_bg.py room2030 ROOM203 \
       extracted_re2_dokumente/weltmodelle/belege 0 --only-item=118
python tools/re2_sicherung/re15_doc_check.py
```

`info/re2leon/` und `info/Re1.5/` wurden ausschließlich gelesen.

---

## 8. Was offen bleibt

1. **Bedeutung von `Ck(bank 1, bit 0)`** (room4090-Variante, Abschnitt 4.3). Geprüft:
   Bank-Zeigertabelle `0x800A78C8` → Bank 1 = `DAT_800CFBD8`; Häufigkeitsprofil über alle
   Räume (30 bzw. 13 Räume). Nicht geprüft: welcher der >20 Schreiber von `DAT_800CFBD8`
   Bit 0 setzt. Nächster Weg: PCSX-Redux-Watchpoint auf `0x800CFBD8` (Skill
   `re15-pcsx-watchpoint`).
2. **Was `sub09`–`sub12` in room2080 startet** (also wodurch der Spieler die Filmrolle in
   die Wanne gibt). Belegt ist die Kette ab dem Unterprogramm (4.2c) und dass die drei
   `sce=6`-AOTs an der Wanne die Bits 68/69/70 in Bank 10 setzen. Nicht belegt ist, welcher
   Item-Use-Pfad der EXE daraus den Sprung in `sub09`–`sub12` macht. Nächster Weg:
   `FUN_80053138` (SCD-Task-Start) rückwärts über seine Aufrufer im Inventar-Code.
3. **Bedeutung von `DAT_800CFBFC+1 = 6`** im `action & 1`-Zweig des sce=2-Handlers
   (@0x8005192C). Betrifft von den Dokumenten nur Id 126 (room6030).
4. **45 von 2553 SCD-Blöcken** werden vom Opcode-Walker nicht durchlaufen (5,45 % der
   Bytes). Für die Dokument-Frage durch die Brute-Force-Gegenprobe abgedeckt (0 neue
   Treffer), für andere Fragen nicht.
