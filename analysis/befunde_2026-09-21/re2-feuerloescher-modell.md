# Gibt es in Resident Evil 2 ein Feuerloescher-3D-Modell?

**Antwort: NEIN.** Auf der RE2-Leon-Disc existiert kein Feuerloescher — weder als
Gegenstand, noch als Raum-Objekt, noch ueberhaupt als Wort in den Daten.
**Aber RE1.5 selbst hat den Feuerloescher** — als Gegenstand `0x31` mit eigenem
Inventar-Bild, allerdings **ohne 3D-Modell**.

Alles hier ist gemessen. Jede Zahl hat eine Datei-Adresse oder ein nachrechenbares
Verfahren; die Werkzeuge liegen in `analysis/befunde_2026-09-21/tools/`, die Bilder
in `analysis/befunde_2026-09-21/belege/`.

---

## 0. Kurzfassung

| Frage | Antwort | Sollzahl / Beleg |
|---|---|---|
| RE2: Feuerloescher als **Gegenstand**? | nein | 139 Item-Namen (Tabelle `PSX.EXE` file@0x08ED51…0x08F3AC), 106 Item-Icons (`ITEMALL.PIX`) — alle geprueft |
| RE2: Feuerloescher als **Raum-Objekt**? | nein | 923 Prop-Plaetze in 250 RDTs, 392 verschiedene MD1-Netze auf der ganzen Disc — alle gerendert und angesehen |
| RE2: Wort „extinguisher" irgendwo? | nein | 0 Treffer in **21 602 Dateien** (ASCII + RE-Zeichentabelle) |
| RE1.5: Feuerloescher als Gegenstand? | **ja** | Item `0x31`, Name `DEBUG.BIN` file@0x004CDC (ram 0x801044DC), Bild `ITPS.ITP` file@0x093000 |
| RE1.5: Feuerloescher als 3D-Modell? | **nein** | Aufnahme-Datensatz `ROOM1000.RDT` @0x0C24 traegt `tk_prop = 0xFF` (kein Prop); 570 Prop-Plaetze / 192 verschiedene MD1-Netze angesehen |

---

## 1. Wie ich abgezaehlt habe (die Sollzahlen)

### 1.1 Das Wort

`tools/find_word.py` kodiert ein Wort in die RE-Nachrichten-Zeichentabelle
(0x1D–0x36 = A–Z, 0x3D–0x56 = a–z, 0x00 = Leerzeichen, … — die Tabelle stammt
woertlich aus dem Repo-eigenen Decoder
`src/main/java/de/re15/extractors/MSGParser.java`) und sucht sie zusaetzlich als
reines ASCII. Damit werden RDT-Texte, EXE-Texte und Dateinamen in einem Lauf
abgedeckt.

* Lauf ueber `info/re2leon/`: **21 602 Dateien, 0 Treffer** fuer „extinguisher".
* Gegenprobe, dass das Verfahren wirkt: „fuse" liefert 20 Treffer, darunter die
  **Item-Namenstabelle** `PSX.EXE` file@0x08F096 `Main Fuse` / @0x08F0A0 `Fuse Case`
  und die Raumtexte `ROOM6080.RDT` @0x002CC2 („The main fuse has been removed"),
  `ROOM60D0.RDT` @0x000E79 („I could set the superconductor fuse here if I had a
  fuse case"), `ROOM6110.RDT` @0x004446.
* Derselbe Lauf ueber `info/Re1.5/` liefert **4 Treffer** fuer „extinguisher" —
  siehe §4.

Haeufigkeitsprobe, die belegt, dass die Zeichentabelle fuer RE2-US stimmt
(79 029 Byte aller ausgepackten Raum-Nachrichten): 0x41 3,25 % = `e`, 0x4B 2,13 % = `o`,
0x50 2,10 % = `t`, 0x3D 1,50 % = `a`, 0x45 1,48 % = `i` — englische Buchstaben-
haeufigkeit, Reihenfolge passt.

### 1.2 Die RE2-Gegenstaende

Namenstabelle in `info/re2leon/PSX.EXE` (PS-X EXE, t_addr 0x80010000, Daten ab
file 0x800, also `ram = 0x80010000 + file − 0x800`):

* **Zeiger-Tabelle**: file@0x08EC38 (ram 0x8009E438), 140 × u16 = 0x118 Byte,
  endet unmittelbar vor dem ersten Trenner 0xF7 @0x08ED50.
* **Zeichenketten**: file@0x08ED51 `Knife` … file@0x08F3A1 `Locker Key`,
  0xF7-getrennt, **139 Namen** (Id 0x01…0x8B; Id 0x00 = leer).
* **Beschreibungen**: eigener Block, u. a. file@0x09221F „I can restore the / power
  to the lab / with this." (= Main Fuse).

Vollstaendige Liste: `belege/re2_item_names.txt`. Kein Eintrag ist ein Feuerloescher.

Zweiter, unabhaengiger Zaehlweg: das Icon-Blatt. `COMMON/DATA/ITEMALL.PIX` ist als
**106 TIM-Kacheln** ausgepackt vorhanden; Kontaktbogen `belege/itemall.png`. Ich habe
alle 106 angesehen: Waffen, Munition, Kraeuter, Schluessel, Medaillen, Steine,
Stecker, Filme, Zahnrad, Hauptsicherung, Sicherungskasten, MO-Disk, Dokumente —
**kein Feuerloescher**.

### 1.3 Die RE2-3D-Modelle

Zwei voneinander unabhaengige Zaehlungen, die sich gegenseitig pruefen.

**(a) Deklariert — aus den RDT-Kopfdaten.**
Der RE2-RDT-Kopf ist 0x64 Byte: 8 Zaehlbytes + 23 × u32 Sektions-Offsets.
Gemessen an `ROOM1110.RDT`: `+0x01 nCut = 11`, `+0x24 camera = 0x64` (= Kopfgroesse),
`0x64 + 11×32 = 0x1C4` = der Wert bei `+0x30` → **`+0x30` ist die Objektmodell-
Zeigertabelle**, und `0x1C4 + 12×8 = 0x224` = der Wert bei `+0x28` → **8 Byte je
Eintrag, `{u32 tim_off; u32 md1_off}`**.
Beweis fuer die Feldreihenfolge, nicht behauptet: `md1_off[1] − md1_off[0] =
0x600C − 0x3528 = 10 980` = exakt die Groesse der bereits ausgepackten Datei
`PL0/RDT/room1110/obj/model00.md1`; ebenso fuer model01…model11.
`tim_off[1] − tim_off[0] = 0x4CB54 − 0x44934 = 33 312` = `model00.tim`.

Summe ueber alle **250** RDTs: **923 Prop-Plaetze**, davon **893 mit Geometrie**
(30 Plaetze haben `md1_off = 0`, also gar kein Modell), **243 verschiedene** Netze
(die Raeume teilen sich Geometrie). Zaehlung: `tools/cut_props.py`,
Rohzahlen `belege/re2_rdt_census.json`, Liste `belege/re2_props_index.txt`.
Kontrolle: meine 12 aus `ROOM1110.RDT` geschnittenen Bloecke sind **byte-gleich**
mit der fruehleren Extraktion `room1110/obj/model00…11.md1` (12 von 12).

Nebenbefund: `nItem` und `nDoor` sind in **allen 250** RE2-RDTs `0` — Gegenstaende
und Tueren entstehen dort ausschliesslich ueber das Skript (`Item_aot_set` /
`Door_aot_set`), nicht ueber eine Kopftabelle.

**(b) Roh — Signatursuche ueber die ganze Disc.**
`tools/scan_md1.py` sucht in **jeder** Datei nach dem MD1-Kopf. Signatur nach dem
Repo-eigenen Format-Vertrag `re15_port/include/re15_md1.h`: `+0x08 object_count`
gerade und 2…64, `nmesh = object_count/2`, die Mesh-Koepfe belegen `nmesh×56` Byte
ab `datei+12`, folglich muss der erste Block-Offset (`t_vertex_offset` von Mesh 0)
**genau `nmesh×56`** sein; alle Offsets 4-gerade, alle Zaehler plausibel, das
hoechste Blockende muss in die Datei passen.

* **11 933 Dateien, 512,5 MB durchsucht → 1 732 Fundstellen → 392 verschiedene Netze.**
* **Empfindlichkeit belegt, nicht behauptet:** dieselbe Suche findet **893 von 893**
  der unter (a) deklarierten Props an ihrer bekannten Adresse wieder, **0 verfehlt**
  (`tools/selftest.py`).
* Aufteilung: 243 Netze in den RDTs, 149 ausserhalb (33 Tuerarchive `DO2`,
  113 in `PL0/PLD` = Gegner-`EMS`, Spielermodelle `PLD`, Waffen `PLW`, 3 in `DATA`).

Ehrliche Luecke: 41 der 65 losen `.EMD`-Scheiben in `PL0/PLD/CDEMD{0,1}/` liefern
keinen Treffer. Geprueft (`tools/probe_emd.py`-Lauf): **alle 65** dieser Dateien
kommen byte-gleich in `CDEMD0.EMS` bzw. `CDEMD1.EMS` vor — es sind
Sektor-Schnitte einer frueheren Extraktion aus den Containern, die die Suche
ohnehin abdeckt (55 + 56 Treffer). Neue Geometrie steckt dort nicht.

**Angesehen habe ich alle 392.** Untexturiert `belege/nonrdt_page00…04.png`,
texturiert (Farbe aus der jeweils eigenen TIM des Props, Texel-Adressierung wie die
PSX-GPU sie rechnet: tpage-Bits 0–3 × 64 Halbworte, Bit 4 × 256 Zeilen, Bits 7–8 =
Farbtiefe; CLUT-Bits 0–5 × 16, Bits 6–14 = Zeile) `belege/tex_page00…06.png`.

Was dort steht: Tischlampen (dieselbe Lampe in 75 Raeumen), Kerzen, Rohre,
Gitter, Leitern, Vorhaenge, Rollos, Zahnraeder, Ventilatoren, Kisten, Tabletts,
Teppiche, Tueren, Rolltore, Leichen, Waffen, Gegner. **Kein Zylinder mit Schlauch.**

---

## 2. Wo ein Feuerloescher in RE2 stecken *koennte* — alle drei Faelle geschlossen

**(a) aufsammelbarer Gegenstand mit Weltmodell.** Ausgeschlossen: 139 Namen,
106 Icons, 0 Wort-Treffer.

Nebenbei gemessen, wie RE2 Boden-Gegenstaende ueberhaupt zeigt: ueber die
RDT-Objektmodelle. Beleg ist die Textur, nicht die Vermutung — die Modell-TIM von
`ROOM1010.RDT` @0x2A4A0 traegt die Aufschriften **„ink ribbon"** und
**„JONY'S ARMS"**, also Gegenstands-Etiketten, und die Props, die sie benutzen, sind
die flachen roten Kaestchen (`dx144 dy72 dz297`, 10 Flaechen), die in 90 Prop-
Plaetzen vorkommen. Die Gegenstands-Modelle liegen also **im selben Topf**, den ich
vollstaendig durchgesehen habe.

**(b) Raum-Objekt (`Obj_model_set`-Prop).** Ausgeschlossen: 923 Plaetze,
243 verschiedene Netze, alle gerendert und angesehen.

**(c) nur gemalte Kulisse.** Das bleibt als einzige Moeglichkeit uebrig — und selbst
dafuer gibt es keinen Textbeleg, weil das Wort in keiner der 21 602 Dateien
vorkommt. Ein Feuerloescher an einer RE2-Wand waere also reines Hintergrundbild
(`COMMON/BSS/ROOM*/`), aus dem sich kein Modell gewinnen laesst.

---

## 3. Das naechstliegende vorhandene Modell (falls doch gebaut werden soll)

Extrahiert nach `extracted_re2_feuerloescher/basis_kandidaten/` — je als rohes
`.md1`, rohe `.tim`, `.obj` + `.mtl` + Textur-`.png` und vier Ansichten.
Uebersichtsbild: `belege/basis_kandidaten.png`.

| Datei | Herkunft | Masse (PSX-Einheiten) | Flaechen | was es ist |
|---|---|---|---|---|
| `RE2_ROOM60D0_m02_laborflasche` | `info/re2leon/PL0/RDT/ROOM60D0.RDT` md1@0x1424, tim@0x1F384 | 82 × 408 × 82 | 60 | 10-seitiger Metallzylinder mit geripptem Fuss und Deckel — Laborflasche im Labor-Abschnitt. Formal am naechsten an einem Loescher-Koerper. |
| `RE2_ROOM1010_m03_sprayflasche` | `.../ROOM1010.RDT` md1@0x3394, tim@0x2A4A0 | 158 × 489 × 180 | 40 | sechseckiger Koerper, kegelig zulaufendes Ende mit kleinem Stutzen; benutzt die Gegenstands-Textur des Raums |
| `RE15_ROOM1010_m00_spraydose` | `info/Re1.5/PSX/STAGE1/ROOM1010.RDT` md1@0xAF4, tim@0x22D28 | 158 × 489 × 180 | 40 | **dieselbe Geometrie in RE1.5**; die TIM traegt ein „SPRAY"-Etikett |
| `RE15_ROOM4050_m03_spraydose_rot` | `info/Re1.5/PSX/STAGE4/ROOM4050.RDT` md1@0x3788, tim@0x2DDA8 | 127 × 400 × 147 | 40 | hellgraue Dose mit **rotem** Stutzen, TIM mit gruenem und blauem „SPRAY"-Etikett |

Die beiden RE1.5-Dosen sind die naheliegendste Basis, und zwar aus einem
messbaren Grund: der Item-Zensus (§4.3) bindet in `ROOM1010` den Prop 0 an
Gegenstand `0x22` **First Aid Spray** und in `ROOM4050` die Props 3 und 4 an
Gegenstand `0x23` **Antidote Spray**. Es ist also derselbe Gegenstandstyp —
Druckdose mit Duese — im richtigen Groessenbereich und schon in RE1.5-Daten.

Die `.obj` sind mit `Y' = −Y` gekippt (PSX-Y zeigt nach unten; die Regel steht in
`CLAUDE.md` unter „Technical Notes"), UVs sind in das Pixelraster der jeweiligen
TIM umgerechnet. Werkzeug: `tools/export_obj.py`.

---

## 4. Die RE1.5-Seite — und hier liegt die eigentliche Antwort

### 4.1 Den Feuerloescher gibt es, als Gegenstand `0x31`

Vier Treffer im RE1.5-Baum:

| Datei | Offset | Inhalt |
|---|---|---|
| `info/Re1.5/PSX/BIN/DEBUG.BIN` | file@0x004CDC (ram 0x801044DC) | Eintrag **`Fire Extinguisher`** in der Gegenstands-Namenstabelle |
| `info/Re1.5/PSX/STAGE1/ROOM1090.RDT` | @0x00294E | „…to put out this fire to save that woman!" / **„Will you use the Fire Extinguisher ?"** |
| `info/Re1.5/PSX/STAGE1/ROOM1090.RDT` | @0x00297A | **„You've used the Fire Extinguisher."** |
| `README.TXT` (Beipack, keine Spieldaten) | @0x0045EC | „You will need to get the Fire Extinguisher to put out the fire on the van" |

Die Namenstabelle liegt in `DEBUG.BIN` (0x800-Byte-Kopf, laedt @0x80100000) ab
file@0x004A29 = ram 0x80104229, 0x07-getrennt, Id 1 = `Combat Knife`.
Vollstaendig in `belege/re15_item_names.txt`. Daraus:

* Id **0x15** = `H. Gun Bullets` — deckt sich mit dem Port-Kommentar und der RDT
  (`ROOM1050.RDT` @0xB9A).
* Id **0x31** = `Fire Extinguisher` — deckt sich mit der Konstante
  `ITEM_FIRE_EXTINGUISHER 0x31` in `re15_port/tests/integration/test_item_name_census.c:42`.

Dritter, bildlicher Beleg fuer dieselbe Id-Zuordnung: das Bilder-Blatt
`ITEM/ITPS.ITP` (72 TIMs, eine je Id, Schrittweite 0x3000 — Layout aus dem
Repo-eigenen byte-treuen Decoder `re15_port/engine/src/itps_common.c`). Kachel
**0x31 (file@0x093000)** zeigt einen **roten Feuerloescher mit Chromgriff und
weissem Etikett**. Kontaktbogen aller 72: `belege/itps15.png` — dort stimmen auch
0x2F `Nut`, 0x30 `Pliers`, 0x32 `Head of Akuma`, 0x3F `Pocket Watch`, 0x40 `Fuse`,
0x41 `Spark Plug` mit der Namenstabelle ueberein.

### 4.2 Ein 3D-Modell dazu gibt es nicht

Der Aufnahme-Datensatz steht in `info/Re1.5/PSX/STAGE1/ROOM1000.RDT` @0x0C24:

```
0C24: 50 03 09 31 00 00  14 50  e0 fc  20 03  20 03  31 00  01 00  86 00  ff 00
      |  |  |  |  |  |   x=20500 z=-800 w=800 d=800  typ    anz    tk_bit tk_prop
      |  |  |  |  |  super                            0x31   1      0x86   0xFF
      |  |  |  |  floor
      |  |  |  sat 0x31  (Bit 0x80 nicht gesetzt -> Kurzform, 22 Byte)
      |  |  sce 0x09
      |  slot 3
      Item_aot_set (0x50)
```

Feldlage byte-true aus dem Original, nicht aus dem Port abgeschrieben —
`ghidra1_V2.txt`, Kurzform-Zweig `LAB_80040680`:

```
80040664  andi   v0,v0,0x80        ; sat & 0x80 -> Lang-/Kurzform
80040668  beq    v0,zero,LAB_80040680
80040680  lhu    a1,0x12(a2)       ; +0x12 = taken-bit
80040684  lbu    s1,0x14(a2)       ; +0x14 = tk_prop
80040688  addiu  v0,a2,0x16        ; Kurzform = 22 Byte
...
800406dc  jal    FUN_8004efe4      ; Flag-Zone 0x800b1078, Bit = taken-bit
800406e4  beq    v0,zero,LAB_8004071c   ; nicht genommen -> fertig
800406f4  sb     zero,0x0(v0)      ; genommen -> AOT stilllegen
800406f8  sll    v0,s1,0x3         ; und Prop s1 im Pool 0x800b3f98
800406fc  addu   v0,v0,s1          ;   (Schrittweite 148 Byte)
80040718  sw     v1,0x0(at)        ;   auf 0x80000000 = versteckt setzen
```

`tk_prop` ist also das Byte bei `+0x14`, und im Feuerloescher-Datensatz ist es
**0xFF**, waehrend jeder Gegenstand mit sichtbarem Modell dort einen kleinen
Prop-Index traegt.

Gegenprobe im selben Raum: `ROOM1000.RDT` hat `nOmodel = 2`, und beide Props sind
vergeben — @0x0C3A Gegenstand `0x24` **Green Medicine** mit `tk_prop = 0`, @0x0C50
Gegenstand `0x25` **Red Medicine** mit `tk_prop = 1`. Und genau das zeigen die
Modelle: Prop 0 ist ein **gruener**, Prop 1 ein **roter** Koffer, und ihre
gemeinsame TIM @0x23168 traegt die Aufschrift **„INSTANT FIRST AID CASE"** in
gruen/rot/blau (Bild `belege/detail15_room1000.png`). Fuer den Feuerloescher bleibt
im Raum kein drittes Modell uebrig — er ist eine reine Aufnahmezone mit Text.

### 4.3 Und in keinem anderen Raum auch nicht

`tools/item_aot_census.py` laeuft ueber alle 240 RE1.5-RDTs.

* Das Verfahren ist geeicht: es findet **160** Datensaetze mit `sat = 0x31` — exakt
  die Zahl, die der Port aus seinem byte-treuen Walker meldet
  (`re15_port/engine/src/scd_vm.c`: „alle 164 ausgelieferten Item_aot_set … 160x 0x31
  + 4x 0x51", Zensus 2026-08-19). Bei `sat = 0x51` zaehle ich mehr als die 4 des
  Ports, das sind Fehltreffer meiner Byte-Suche; die 0x31-Menge stimmt.
* **Gegenstand 0x31 kommt genau einmal vor: `ROOM1000` @0x0C24, `tk_prop = 0xFF`.**
* 43 Datensaetze insgesamt tragen 0xFF, also kein Weltmodell — darunter auch
  Beretta M93R, Head of Akuma, Minidisc Player.

Volle Tabelle: `belege/re15_item_aot_census.txt`.

Eine Einschraenkung, damit sie nicht als Behauptung durchgeht: `tk_prop` ist die
`obj_id` aus `Obj_model_set`, nicht zwingend der Index in der RDT-Modelltabelle.
Dass beide in der Praxis zusammenfallen, ist in `ROOM1000` gemessen (Prop 0 =
gruener, Prop 1 = roter Koffer zu den Gegenstaenden 0x24/0x25) und in `ROOM1010` /
`ROOM4050` durch die „SPRAY"-Etiketten gestuetzt. Fuer die Aussage „der
Feuerloescher hat kein Modell" spielt es keine Rolle: dort steht 0xFF und der Raum
hat ueberhaupt nur zwei Modelle, beide anderweitig belegt.

* Zweite, formunabhaengige Zaehlung: **240 RE1.5-RDTs, 570 Prop-Plaetze, alle 570
  lesbar, 122 verschiedene Netze** (`tools/cut_props15.py`; RE1.5-Kopf ist 0x60 Byte,
  sonst dieselbe Tabelle bei `+0x30` — nachgerechnet an `ROOM1000.RDT`:
  `+0x24 = 0x60`, `nCut = 9`, `0x60 + 9×32 = 0x180` = Wert bei `+0x30`,
  `0x180 + 2×8 = 0x190` = Wert bei `+0x28`).
  Roh-Signatursuche ueber `info/Re1.5/PSX`: **580 Dateien, 128,4 MB, 524 Fundstellen,
  192 verschiedene Netze**, und sie findet **570 von 570** deklarierten Props wieder,
  0 verfehlt.
  **Alle 192 angesehen** (`belege/re15_tex_page00…03.png` texturiert,
  `belege/re15_nonrdt_page00…01.png` fuer Tueren/Gegner/Waffen). Kein Feuerloescher.

---

## 5. Was fuer den Port noetig waere

Ein Feuerloescher-Modell muss **gebaut** werden — es gibt in keinem der beiden
Spiele eines zum Ausschlachten. Byte-true ist der heutige Zustand allerdings
**korrekt**: das Original zeigt in ROOM1000 nichts, die Aufnahme laeuft ueber die
Zone plus „Will you take the Fire Extinguisher?". Ein sichtbarer Loescher waere
eine **Zutat**, kein Original-Verhalten — dieselbe Unterscheidung, die auch sonst
gilt.

Was vorhanden und sofort brauchbar ist:

* `extracted_re2_feuerloescher/re15_feuerloescher/RE15_item31_fire_extinguisher_ITPS.*`
  — das **Original-Bild** des Feuerloeschers (112 × 72, 8 bpp, `ITPS.ITP`
  file@0x093000, 8 608 Byte TIM). Das ist die Vorlage fuer jede Textur.
* `…_ITEMALL_tile.raw` — die 40 × 30 Inventar-Kachel aus `ITEMALL.PIX`
  file@0x00E5B0 (1 200 Indexbytes). Sie zeigt dieselbe Silhouette, bestaetigt also
  `Kachel-Index = Item-Id` fuer 0x31. **Offen:** die zugehoerige CLUT steht nicht in
  der Datei; der Port hat bisher nur die Paletten `clut_idx 2` und `6` fuer die Ids
  0x00…0x18 aufgenommen (`re15_port/engine/src/item_icon_common.c`). Deshalb liegt
  hier nur das Indexbild bei, nicht das fertige Farbbild.
* die vier Basis-Kandidaten aus §3, falls ein Modell modelliert wird.

---

## 6. Was die Messung gekippt hat

* **„RE2 hat sicher irgendwo einen Feuerloescher als Kulisse, also auch ein Modell."**
  Falsch. Das Wort kommt in **keiner** der 21 602 Dateien vor, und keines der
  392 Netze ist einer.
* **„Die 756 schon ausgepackten `room*/obj/*.md1` sind der Modellbestand."**
  Zu wenig: die RDT-Kopftabellen deklarieren **923** Plaetze. Die fruehere Extraktion
  hat 167 nicht geschrieben (geteilte Geometrie, fehlende Raumordner).
* **„Ein tall-and-thin-Filter findet den Loescher schon."** Die schlanken Kandidaten
  waren durchweg etwas anderes — Rohre, Kerzen, Lampen, Spraydosen. Erst die
  **Textur** hat sie identifiziert; die Form allein haette mich in die Irre gefuehrt.
* **„In ROOM1000 muss ein drittes Prop liegen."** `nOmodel = 2`, und beide sind
  nachweislich an die Medizinkoffer gebunden.
