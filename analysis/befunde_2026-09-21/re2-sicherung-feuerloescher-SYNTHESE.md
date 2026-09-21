# Synthese: Sicherung als Item in RE2? Feuerlöscher-3D-Modell?

**Datum:** 2026-09-21 · **Quellen:** `info/re2leon/` (RE2 Retail Leon, SLUS-00748) und
`info/Re1.5/PSX/` — beide nur gelesen.
**Grundlage:** zwei Ermittler-Dossiers und zwei unabhängige Skeptiker-Prüfungen; jede hier
als *gemessen* markierte Zahl habe ich in dieser Sitzung selbst nachgerechnet
(Skripte im Scratchpad, Quellen unverändert).

| Frage | Antwort |
|---|---|
| RE2: Sicherung als Gegenstand? | **JA, zwei** — Id 76 `Main Fuse`, Id 77 `Fuse Case`, beide für Leon |
| RE2: Feuerlöscher-3D-Modell? | **NEIN** — RE2 hat überhaupt keinen Feuerlöscher |
| RE1.5: Feuerlöscher? | Gegenstand **ja** (0x31, ROOM1000), Modell **nein** |
| RE1.5: Sicherung? | Name + Bild + Dialog **ja** (0x40 `Fuse`), Aufstellung **nein** (0 Platzierungen) |
| RE1.5: das Sicherungs-**Mesh**? | **ja, bereits vorhanden** — siehe §3, das ist der wichtigste Befund |

---

## 1. Die Sollzahlen (nur die, die tragen)

| Menge | Sollzahl | gemessen an |
|---|---|---|
| RE2-Item-Namen EN | **140 Tabelleneinträge / 139 Namen** (Index 0 leer) | Offsettabelle ram `0x8009EBAC` = file `0x08F3AC`, Basis file `0x08ED50`; Monotonie bricht **nach Index 139** |
| RE2-Item-Namen JP | 139, 1:1 zur EN-Liste | Tabelle ram `0x8009E438` = file `0x08EC38`, Basis file `0x08E73C` |
| RE2-Item-Eigenschaften | 101 (Records ab `0x800A9E1C`, Schritt 8) | Obergrenze `0x800AA144` = eigene Tabelle (5 `lhu`-Xrefs) |
| RE2-Inventarsymbole | 106 (`ITEMALL.PIX` 127200 B ÷ 1200) | alle gerendert und angesehen |
| **RE2-Gegenstandsbilder** | **100** (`COMMON/DATA/ITPS.ITP` 1 228 800 B ÷ 0x3000) | **gemessen; alle 100 gerendert und in dieser Sitzung selbst angesehen** |
| RE2-Requisiten-Modelle | 893 Platzierungen in 250 RDTs, 243–252 verschieden | `RDT+0x30` × `nOmodel`; Platzierungszahl in allen vier Läufen identisch |
| RE2-Dateien | 21 602 | Zeichensatz-Bytesuche über den ganzen Baum |
| RE1.5-Item-Namen | **102** (nicht 100) | Offsettabelle `DEBUG.BIN` file `0x495C` … Blob `0x4A28` ⇒ (0xCC)/2 = 102, **gemessen** |
| RE1.5-Item-Platzierungen | **160** Records mit `sat=0x31` über alle **240** RDTs | **gemessen**; deckt sich mit dem byte-treuen Walker des Ports |
| RE1.5-Requisiten-Modelle | 570 Platzierungen, 122 verschieden | alle angesehen |

---

## 2. RE2 hat die Sicherung — zweimal

```
Index 76 (0x4C) @0x8009E896   29 3d 45 4a 00 22 51 4f 41   "Main Fuse"
Index 77 (0x4D) @0x8009E8A0   22 51 4f 41 00 1f 3d 4f 41   "Fuse Case"
Eigenschaften   @0x800AA07C / @0x800AA084   01 00 04 00 e8 9c 0a 80
                (max=1, Variante 0, Klasse 4, 0 Rezepte — geprüft gegen alle 80)
```

Platzierung, **gemessen** in `PL0/RDT/ROOM60D0.RDT` (dem einzigen Raum des Spiels, der sie führt):

```
+0x0008E6  4E 01 02 00 00 00 00 7D 00 7D 01 00 01 00 4C 00 01 00 18 00 03 00
           Item_aot_set     x=z=32000 (geparkt) w=d=1  item=76 flag=24 md1=3
+0x0008FC  69 02 02 31 …    Item_aot_set_4p, 4 Eckpunkte,  item=77 flag=61 md1=1
```

* **Fuse Case (77)** = gewöhnliche Bodenaufnahme, hinlaufen und Aktion drücken.
* **Main Fuse (76)** liegt geparkt und wird per Skript übergeben: `sub05` prüft
  `Ck(0x22,0x18,0)` → `Aot_on(1)`; `Aot_on` (`FUN_800551C8`) führt den sce-Handler sofort
  aus, `sce=2` → `0x80051884` → `sb v1, DAT_800D5C00 = 2` = Item-Get-Bildschirm.
  „Geparkt" heißt hier **nicht** „unerreichbar" — dasselbe Muster tragen auch
  `Joint S/N Plug` und `H. Gun Bullets`.

Unabhängige Bestätigung dieser Sitzung: auf dem Kontaktbogen der 100 RE2-`ITPS`-Bilder ist
**Kachel 76 eine Patronensicherung** (Zylinder mit Metallkappen) und **Kachel 77 die Fassung** —
d. h. Kachelindex = Item-Id, und die Formdeutung der beiden Ermittler stimmt.

⛔ **Das Decompilat taugt hier nicht:** `PL0/RDT/room60D0/scd/sub02.c` enthält wörtlich
`Item_aot_set_4p(/* WIP */);` und verliert den `Fuse Case` vollständig; außerdem gibt es
Halbwörter bytevertauscht aus. Nur die rohen `.scd`/`.RDT`-Bytes sind belastbar.

---

## 3. ⛔ Der wichtigste Befund: RE1.5 hat das Sicherungs-Mesh bereits selbst

Ermittler 1 empfahl, `item_076_main_fuse/weltmodell.md1` in ein Ziel-RDT zu importieren.
Skeptiker 1 hat das gekippt, und **ich habe es unabhängig nachgemessen** — er trägt:

| Mesh | Geometrie-MD5 (Vertices + Tris + Quads) |
|---|---|
| RE2 `ROOM60D0.RDT` md1@`0x2280` (Main Fuse, md1=3) | `1e6ea1c71927310d9e6e78ee8b201e6c` |
| RE1.5 `STAGE4/ROOM4030.RDT` Slot 0, md1@`0x087C` | **dasselbe** |
| RE1.5 `STAGE4/ROOM4040.RDT` Slot 3, md1@`0x1670` | **dasselbe** |

Beide 1364 B lang, 1 Mesh, 12 Dreiecke + 30 Vierecke, Bbox x −68..68 / y −324..324 / z −62..62.
Nur UV-/CLUT-/Page-Wörter unterscheiden sich, weil jeder Raum seine eigene Texturseite hat —
die RE1.5-Fassung ist also **schon auf RE1.5-Texturen bezogen**.

Wem das Mesh in RE1.5 gehört, steht byteweise da (**gemessen**):

```
ROOM4040.RDT @0x000F66
50 06 09 31 00 00 a8 e4 9c 9b e8 03 e8 03 41 00 01 00 56 00 03 00
Item_aot_set, item=0x41 "Spark Plug", n=1, prop-Slot 3
```

**RE1.5s Zündkerzen-Modell ist RE2s Main-Fuse-Modell.** Für eine Sicherung im Port muss
nichts importiert werden. (Der `Fuse Case` hat in RE1.5 **keine** Entsprechung.)

---

## 4. Feuerlöscher: in RE2 nichts, in RE1.5 kein Modell

RE2, fünf voneinander unabhängige Aufzählungen, alle mit Eichung:

| geprüft | Umfang | Treffer |
|---|---|---|
| Item-Namen EN | 140 / 139 | 0 |
| Item-Namen JP | 139 | 0 (auch kein *shoukaki* in Kana) |
| Inventarsymbole `ITEMALL` | 106, einzeln angesehen | 0 |
| **Gegenstandsbilder `ITPS.ITP`** | **100, in dieser Sitzung selbst angesehen** | **0** |
| Requisiten-Modelle | 893 Platzierungen / 243–252 verschieden, alle angesehen | 0 |
| Wortsuche (Zeichensatz + ASCII) | 21 602 Dateien | 0 |

Eichung der Wortsuche: `fuse` findet 20–24 Treffer (u. a. die zwei EXE-Namen und die
Raumtexte ROOM6080/60D0/6110); dieselbe Suche findet in RE1.5 `xtinguish` zweimal.
Nächste Verwandte in RE2: Id 35 `F. Aid Spray` (einziges Sprühdosen-Modell, ROOM60D0 Slot 7),
Id 23 `Fuel` (weinroter Kanister mit Chromringen).

RE1.5 dagegen **hat** den Feuerlöscher — und zwar vollständiger, als beide Dossiers sagten:

* Name Id `0x31` `Fire Extinguisher`, `DEBUG.BIN` file@`0x4CDC` (**gemessen**)
* Gegenstandsbild `ITEM/ITPS.ITP` Kachel `0x31` = roter Löscher mit Chromgriff
  (**gemessen und angesehen**; Kachelindex = Id ist über die Nachbarn 0x2F `Nut`,
  0x30 `Pliers`, 0x32 `Head of Akuma`, 0x3F `Pocket Watch` **bildlich bewiesen**)
* Texte in `ROOM1090.RDT` @`0x294E` / @`0x297A`
* **genau eine** Platzierung, `ROOM1000.RDT` @`0x0C24`, mit `prop = 0xFF`:

```
50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00
```

`0xFF` ist **kein** Sonderzeichen für diesen Gegenstand (auch `H. Gun Bullets` trägt es),
und der Disasm-Block `0x800406f8–0x80040718` prüft nirgends auf `0xFF` — Skeptiker 2 hat
damit recht. Die Aussage „kein Modell" trägt trotzdem, aber über einen anderen Beleg:
`ROOM1000` hat `nOmodel = 2`, und beide Props sind nachweislich die Medizinkoffer
(0x24 grün `prop=0`, 0x25 rot `prop=1`, gemeinsame TIM @`0x23168` „INSTANT FIRST AID CASE") —
**gemessen**. Für den Löscher bleibt kein drittes Modell.

---

## 5. Wo Ergebnis und Skeptiker sich widersprachen

| Punkt | wer trägt | Beleg |
|---|---|---|
| „Der TIM-Kopf der `ITEMALL_*.TIM` ist defekt (4 bpp)" | **Skeptiker 1** | `flags=0x09` → Bits 0–1 = `pmode 1` = **8 bpp**, Bit 3 = CLUT; `w=20` = 20 VRAM-Halbwörter = 40 px; 40×30 = 1200 B Nutzdaten. **Gemessen: der Kopf ist widerspruchsfrei.** Das Ergebnis (8 bpp/40×30) stimmt, die Begründung war falsch |
| „RE2-Mesh in den Port importieren" | **Skeptiker 1** | RE1.5 hat dieselbe Geometrie bereits (§3) |
| „Id 0x40 ist ein freier Slot" | **Skeptiker 1**, verschärft | Name + **Inventarbild (ITPS 0x40)** + Dialog `ROOM2030.RDT` @`0x20A1` „Will you take the Fuse" / „You've taken the Fuse" (auch in ROOM2031) — **gemessen**. 0 Platzierungen in 240 RDTs. Das ist kein leerer Slot, sondern ein **herausgeschnittenes Fahrstuhl-Rätsel** |
| Namenstabelle `file@0x08EC38` | **Skeptiker 2** | `0x08EC38` = ram `0x8009E438` = **japanische** Tabelle; EN liegt bei `file@0x08F3AC` = ram `0x8009EBAC`. **Gemessen.** Ermittler 1 hatte beide von Anfang an richtig; Ermittler 2 hat sie vertauscht beschriftet |
| „140 Namen" vs. „139 Namen" | **beide** | 140 Tabelleneinträge, Index 0 leer ⇒ 139 Namen. **Gemessen** |
| Sollzahl der RE2-Bilder | **Skeptiker 2** | `ITPS.ITP` (100 Bilder) fehlte in beiden Dossiers — genau die Klasse, mit der auf der RE1.5-Seite bewiesen wurde. **In dieser Sitzung geschlossen: 100 gerendert, angesehen, kein Löscher** |
| „`tk_prop 0xFF` = kein Weltmodell" | **Skeptiker 2** (Formulierung) | Deutung, kein Disasm-Befund; die Aussage trägt über `nOmodel=2` |
| RE1.5-Namenszahl 100 | **Skeptiker 1** | **102 gemessen** (Offsettabelle `0x495C`…`0x4A28`) |
| Prop-Entdopplung 252 vs. 243 | unentschieden, unkritisch | dieselbe Rohmenge 893, andere Schärfe beim Schneiden; beide haben die **größere** Menge angesehen |

Keiner der beiden Skeptiker konnte eine **Existenzaussage** umstoßen. Beide Kernantworten
stehen unverändert.

---

## 6. Was für den Port nötig wäre

1. **Sicherung, Modell:** nichts importieren. `STAGE4/ROOM4030.RDT` Slot 0 bzw.
   `ROOM4040.RDT` Slot 3 als Requisite verwenden — der Port liest Requisiten bereits aus
   dem RDT (`rdt_common.c:51-79`, `pc_load_room_prop_set`).
2. **Sicherung, Id:** `0x40 "Fuse"` ist vorbereitet (Name, ITPS-Bild, Dialog in ROOM2030/2031)
   und nirgends aufgestellt. Fehlt allein das `Item_aot_set`.
3. **Fassung (`Fuse Case`):** nur als RE2-Extrakt vorhanden
   (`item_077_fuse_case/weltmodell.md1` + `.tim`) — RE1.5 hat dafür keine Entsprechung.
4. **Symbol:** ⛔ `re15_port/engine/src/item_icon_common.c:48` kennt nur `clut_idx` 2 und 6
   für Ids `0x00..0x18`. Für `0x40` fehlt die gemessene RE1.5-Palette.
5. **Feuerlöscher:** muss **gebaut** werden — in keinem der beiden Spiele existiert ein Mesh.
   Vorlage für die Textur: RE1.5 `ITPS`-Kachel `0x31`. Geometrie-Basis: die RE1.5-Spraydosen
   (`ROOM1010` Slot 0 zu Item 0x22, `ROOM4050` Slots 3/4 zu Item 0x23), 40 Flächen.
   ⛔ Ein sichtbarer Löscher in ROOM1000 wäre eine **Zutat**, kein Original-Verhalten:
   das Original zeigt dort nur Hintergrundbild + Aufnahmezone.

---

## 7. Offen (5)

1. **RE1.5-`ITEMALL`-Kachel zu 0x31 nicht bewiesen.** Das *große* Bild (`ITPS`) ist bewiesen,
   die 40×30-Inventarkachel nicht: der Port belegt `tile == id` nur für Ids < 0x15 und
   musste für 0x15 eine Ausnahme messen. Es fehlt die Upload-Reihenfolge aus dem Framebuffer.
2. **CLUT der RE1.5-Symbole ab Id 0x19 ist nicht gemessen** (`item_icon_common.c:48`) —
   ohne sie hat kein neues Symbol byte-treue Farbe.
3. **Wie `Main Fuse` und `Fuse Case` in RE2 zusammenwirken**, ist nicht verfolgt: kein
   gemeinsames Rezept (alle 80 geprüft), der Mechanismus steckt im Raumskript von ROOM60D0
   (u. a. ein `Aot_set` sub02+0x005C, sce=6, sat=0x21, mit Id 77 in der Nutzlast).
4. **45 RE2- und 28 RE1.5-SCD-Blöcke laufen im Opcode-Walk aus dem Tritt** (1,8 % / 1,4 %);
   fünf RE2-Räume (roomG000…G040) haben gar kein `scd`-Verzeichnis. Für diese Frage durch
   die Bytepositions-Gegenproben gedeckt, für andere Fragen nachzutragen.
5. **RE2-Kulissen nicht abgesucht** (`COMMON/BSS/ROOM*/`, MDEC): ob irgendwo ein Löscher
   *gemalt* ist, ist offen. Für „gibt es ein Modell" irrelevant.

---

## 8. Wo die Extrakte liegen

* Sicherung: `extracted_re2_sicherung/` — Branch `worktree-wf_39ff9f8a-8c1-1`
  (Symbole roh + PNG, CLUT, `weltmodell.md1/.tim/.obj`, `beleg.json`, alle 140 Namen,
  101 Eigenschafts-Records, Kontaktbogen der 106 Symbole, ROOM60D0-Skripte).
  ⛔ Die gelieferten Symbol-PNGs sind **nicht farbtreu** (bis 11 Stufen je Kanal);
  maßgeblich sind die `*_roh_40x30_8bpp.bin` + `symbol_clut_256_bgr555.bin`.
* Feuerlöscher: `extracted_re2_feuerloescher/` — Branch `worktree-wf_39ff9f8a-8c1-2`
  (RE1.5-`ITPS`-Bild des Löschers roh + PNG, `ITEMALL`-Kachel, vier Basis-Kandidaten als
  `.md1`/`.tim`/`.obj` mit Ansichten, Kontaktbögen aller Modelle und Symbole).
