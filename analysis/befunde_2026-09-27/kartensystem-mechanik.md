# RE2-Kartensystem — was ein gefundener Plan aendert

## Kurzfassung (alles selbst nachgelesen, Belege unten)
1. **RE2 hat KEIN Karten-Item.** Die komplette Namensbank (129 Eintraege, Ids 0x00..0x80,
   Zeigertabelle 0x8009EBAC / Basis 0x8009E550) enthaelt kein "Map". Karten sind
   **Ereignisse in einem Raum** — ein Wandplan, den man untersucht; das SCD setzt ein Bit.
2. **Das Bit ist Bank 33 = `0x800D4924`, Bit = Karten-Id 0..0x13 (20 Bereiche).**
3. **Was der Fund aendert:** (a) unbesuchte Raeume des Bereichs werden ueberhaupt erst
   gezeichnet (schwarz), (b) die 14 Gegenstandsmarken des Bereichs erscheinen. Sonst nichts:
   Reiter, Blatt, Spielerpfeil, Etagenpfeil und Massstab haengen NICHT am Besitz,
   und es gibt keinen FILE-Eintrag dazu.
4. **Die Farben stimmen nur halb:** aktueller Raum = dunkelrot `680808`, besuchte Raeume =
   **BLAU `1040b0`** (nicht gruen), unbesucht = schwarz, aber nur MIT Karte gezeichnet.
   Vier Zustaende, nicht drei.
5. **Fundstellen: 8 Raeume** (nicht streng einer je Etage) — ROOM20B0, ROOM2130, ROOM3040,
   ROOM3060, ROOM4040, ROOM5040, ROOM5060, ROOM6120. Fuer 5 der 20 Bereiche gibt es gar
   keine Karte.
6. **RE1.5 hat das System GAR NICHT** — kein Karten-Item unter 102 Namen, und der
   RE1.5-Kartenzeichner malt jedes Rechteck ohne jedes Flag.
7. **Port:** kennt keinen Besitz, zeichnet nur Besuchtes (= RE2 ohne Karte) und benutzt
   GRUEN statt BLAU.
8. **Extrahiert:** `build/extracted/re2_karten/` — 20 Karten roh + 60 PNG.

---

## Fragen
1. Farbzustaende der Karte (rot/gruen/schwarz) — Annahme pruefen
2. Was aendert der Fund einer Karte
3. Karten pro Etage? Anzahl/Namen/Fundorte
4. Ohne Karte — was sieht der Spieler
5. RE1.5-Seite
6. Port-Stand
7. Umsetzungsplan

## Log

### Befund A (2026-09-27): RE2 hat KEIN Karten-Item
Item-Namensbank (lateinisch) `0x8009E550`, Zeigertabelle `u16[0x8009EBAC + id*2]`
(gelesen `lhu v1,-0x1454(at)` @0x80030C24, Basis `addiu v0,v0,-0x1AB0` @0x80030C2C).
129 Eintraege = Item-Ids 0x00..0x80, alle dekodiert. **Kein einziger Name enthaelt
"Map"/"map".** 0x00..0x63 = Waffen/Munition/Schluessel/Kraut, 0x64..0x67 = "no item",
0x68..0x80 = die 25 Dokumente.
=> RE2 retail (Leon) kennt keinen aufsammelbaren Stadtplan als Gegenstand.
CD-Datei-Id 171 = `MAPS.PIX` (Tabelle `0x800988A4`, Record 8 B) ist die Kartengrafik.

### Befund B: die Kartenanzeige — Tabellen und Kartenwahl
* **Kartenwahl aus Stage+Raum**, nicht aus Besitz: `FUN_8006e7f0(stage,room)` liefert
  Karten-Id 0..0x13. Aufruf `lh a0,0x800d481c` / `lh a1,0x800d481e`, `jal 0x8006e7f0`
  **@0x8006d6b8**. 20 Karten-Ids = 20 Bilder in `MAPS.PIX`.
* **Karten-Record, 20x8 B @0x800AAA38** (Tabellenende bewiesen: Eintrag 20 ist
  `ptr=0x000F000F`, Muell):
  `{u32 zeigerAufRaumfeld; u8 anzahlRaeume; u8 kartenFlagBit; u16 pixSlot}`
  Gelesen: `(&PTR_DAT_800aaa38)[id*2]` in FUN_8006e120; `lhu t0,-21954(at)` **@0x8006d6dc**
  (= 0x800AAA3E + id*8, der pixSlot); Anzahl `(&DAT_800aaa3c)[id*8]`;
  Flagbit `(&DAT_800aaa3d)[id*8]`.
* **MAPS.PIX-Unter-TOC @0x800A9414**, 8-B-Records `{u32 groesse; u16 lba_lo; u8 lba_hi;
  u8 xor}`: `lw v0,-27628(at)` **@0x8006d718**, `lhu t0,-27624(at)` **@0x8006d754**,
  `lbu v0,-27622(at)` **@0x8006d748**. Basis-LBA von CD-Id 171 (`MAPS.PIX`) aus
  `0x80098E00/02` (`lhu v0,-29184(v0)` **@0x8006d6f8**, `lbu v1,-29182(v1)` **@0x8006d6e4**).
* Summe der Raumkacheln ueber alle 20 Karten: **137**.

### Befund C: Flag-Baenke (SCD-Bank-Zeigertabelle @0x800A78C8, Index 0 = 0x800CFB74)
| Bank | Adresse | Bedeutung | Beleg |
|---|---|---|---|
| 9 (0x09) | 0x800D490C | **Raum besucht** | Setzer `FUN_8006931c`: `FUN_8007730c(&DAT_800d490c, base[stage]+room)` |
| 31 (0x1F) | 0x800D4A34 | Kartenmarke erledigt (Gegenstand weg) | `FUN_8006dcc0` |
| 32 (0x20) | 0x800D4920 | Raum-Zusatzzustand (Kachel-Variante) | `FUN_8006e120` |
| 33 (0x21) | 0x800D4924 | **KARTE DIESES BEREICHS VORHANDEN** | `FUN_80077360(&DAT_800d4924, (&DAT_800aaa3d)[id*8])` |
| 35 (0x23) | 0x800D4908 | Bereich bekannt -> Etagenpfeil aktiv | Setzer `FUN_8006931c`: `FUN_8007730c(&DAT_800d4908, map_id)` |
Bit-Test `FUN_80077360` **@0x80077360**: `srl v1,a1,5 / sll v1,v1,2 / addu v1,v1,a0 /
lui v0,0x8000 / srlv v0,v0,a1 / and v0,v1,v0` — MSB-zuerst im Wort.
SCD-Opcodes (Sprungtabelle `0x800A74C8`, 256 Eintraege, nach Scratchpad kopiert in
`FUN_80053528`): **0x21 = ck(bank,bit,soll)** Handler `0x80054354` (4 B:
`lbu v1,1(v0)` Bank, `lhu a1,2(v0)` Bit+Soll); **0x22 = set(bank,bit,op)** Handler
`0x800543B4` (4 B: `lbu v1,1(v0)` Bank, `lbu a1,2(v0)` Bit, `lbu a2,3(v0)` Op).

### Befund D: die Farbwahl der Raumkacheln — Rohdisasm FUN_8006e120
```
8006e60c  lbu  a1,13(s1)            ; Raum-Record +0x0D  (Bank 32 = 0x800D4920)
8006e610  jal  0x80077360
8006e614  _addiu s5,zero,501        ; Grundfarbe CLUT-Y 501
8006e618  beq  v0,zero,0x8006e624
8006e620  addiu s5,zero,506         ; Variante CLUT-Y 506
8006e630  bne  v0,a3,...            ; v0 = Karte dieses Raums, a3 = angezeigte Karte
8006e640  bne  s2,a3,...            ; s2 = Kachelindex, a3 = Index des Spielerraums
8006e648  addiu s5,s5,1             ; <<< AKTUELLER RAUM = Grundfarbe + 1
8006e660  lbu  a1,-21955(at)        ; Karten-Flagbit  (0x800AAA3D + id*8)
8006e668  addiu a0,a0,18724         ; Bank 33 = 0x800D4924  "Karte vorhanden"
8006e66c  jal  0x80077360
8006e674  beq  v0,zero,0x8006e730   ; KEINE KARTE -> Zweig 0x8006E730
8006e67c  lbu  a1,12(s1)            ; Raum-Record +0x0C
8006e68c  _addiu a0,a3,-24          ; Bank 9 = 0x800D490C "Raum besucht"
8006e690  bne  v0,zero,0x8006e74c   ; besucht -> zeichnen mit 501/502/506/507
8006e71c  addiu s5,zero,498         ; KARTE + UNBESUCHT
8006e72c  addiu s5,zero,503         ; KARTE + UNBESUCHT + Bank32-Bit
; Zweig OHNE Karte:
8006e730  lbu  a1,12(s1)
8006e740  _addiu a0,a3,-24          ; Bank 9 "Raum besucht"
8006e744  beq  v0,zero,0x8006e768   ; NICHT besucht -> KACHEL WIRD GAR NICHT GEZEICHNET
8006e74c  addiu a0,zero,256
8006e750  jal  0x8008f828           ; GetClut(256, s5)   (0x8008F828 = GetClut, verifiziert)
8006e760  jal  0x8008f918           ; AddPrim
```
**Die Palette liegt in `ST0/ST1/ST1_.TIM`, zweites TIM @Datei-0x10820**, CLUT 16x21,
geladen mit Slot/Cursor-Wort **`addiu v0,zero,2587` = 0x0A1B @0x80068588** ->
Slot 0x1B, CLUT-Cursor 10 -> CLUT-Y = 480+10 = **490**, also Zeilen 490..510
(`addiu v0,v0,480` **@0x80076B08** setzt CLUT-Y = 480+Cursor). Zeilenindex k = y-490.
Alle drei ST-Dateien sind in diesen Zeilen **bitgleich** (geprueft).

| CLUT-Y | k | Farbe Index 1 | Bedeutung laut Code |
|---|---|---|---|
| 498 | 8 | `000000` schwarz | Karte da, Raum UNBESUCHT |
| 501 | 11 | `1040b0` halbtransparent = **BLAU** | Karte/keine Karte, Raum BESUCHT |
| 502 | 12 | `680808` halbtransparent = **DUNKELROT** | **aktueller Raum** (501+1) |
| 503 | 13 | `000000` schwarz | Karte da, unbesucht, Bank32-Bit |
| 506 | 16 | `1040b0` BLAU (+Index5 gefaerbt) | besucht, Bank32-Bit |
| 507 | 17 | `680808` DUNKELROT | aktueller Raum, Bank32-Bit (506+1) |
| 508 | 18 | `40e0f8/a020c8/088808/b81008` | Legende der **Gegenstandsmarken** (`GetClut(0x100,0x1fc)` in FUN_8006db44) |
| 509/510 | 19/20 | BLAU / DUNKELROT | Sonderfall Karte 2, Kachel 14 |

**=> Die Annahme des Nutzers stimmt nur zur Haelfte:** aktueller Raum = ROT (ja, dunkelrot
`680808`), besuchte Raeume = **BLAU `1040b0`, nicht gruen**, unbesuchte = schwarz — aber
unbesuchte werden **ohne Karte ueberhaupt nicht gezeichnet** (Sprung 0x8006E744) und
**mit Karte schwarz** gezeichnet (CLUT-Y 498/503). Es gibt also **vier** Zustaende, nicht drei.

### Befund E: WAS der Kartenbesitz aendert — alle Lesestellen von Bank 33
Bank 33 (`0x800D4924`) wird an **drei** Stellen gelesen, nirgends in der EXE gesetzt:
1. `FUN_8006e120` @0x8006E66C — **Raumkacheln**: mit Karte werden auch UNBESUCHTE
   Raeume gezeichnet (schwarz, CLUT 498/503); ohne Karte werden sie uebersprungen
   (`beq v0,zero,0x8006e768` @0x8006E744).
2. `FUN_8006dcc0` — **Gegenstandsmarken**: 14 Marken, Tabelle `0x800A9B10` (8 B je
   Eintrag: `s16 x; s16 y; u8 flagbit(Bank 31=0x800D4A34); u8 kartenId; u8 szenarioMaske;
   u8 pad`). Sie werden nur gezeichnet, wenn (a) Bank33-Bit der ANGEZEIGTEN Karte gesetzt,
   (b) `(&UNK_800a9b15)[i] == angezeigte Karte`, (c) Bank-31-Bit NOCH NICHT gesetzt
   (Gegenstand noch da), (d) Szenariomaske passt. CLUT `GetClut(0x100,0x1fc)` = Zeile 508
   (cyan/magenta/gruen/rot).
3. `FUN_8006f1c4` — dieselbe Auswahl fuer die zweite Zeichenschleife.
**Nicht** vom Kartenbesitz abhaengig:
* Der **MAP-Reiter selbst** und das Blatt der aktuellen Etage: die Karten-Id kommt aus
  `FUN_8006e7f0(stage,room)` @0x8006D6B8, nicht aus einem Flag.
* Der **Spielerpfeil**: erster Block in FUN_8006e120, nur `if (angezeigte Karte ==
  eigene Karte)`; Lage aus `DAT_800cfc30/38` (Spieler X/Z) / 0x1c2, Richtung
  `((DAT_800cfc6e + 0x100) >> 9) & 7) * 12` = 8 Sektoren.
* Der **Etagenpfeil** hoch/runter: Bank 35 (`0x800D4908`), Bit = Karten-Id, gesetzt beim
  Betreten (`FUN_8006931c`) — also "Etage schon betreten", nicht "Karte gefunden".
* Es gibt **keinen FILE-Eintrag** fuer Karten: die FILE-Liste fuehrt Item-Ids 0x68..0x80,
  und das sind ausschliesslich die 25 Dokumente.
Massstab/Ausschnitt sind fest je Karte (Kachelkoordinaten stehen im Raumfeld).

### Befund F: Wie viele Karten, wo bekommt man sie — Zensus ueber 250 RE2-RDTs
Opcode `22 21 <bit> 01` = set(Bank 33). Zensus ueber **alle 250 `info/re2leon/PL0/RDT/*.RDT`**,
Treffer per Nachbarschaft zu weiteren 0x22-Opcodes als echt bestaetigt (Rest = Bilddaten):
| RDT (Stage,Raum) | Datei-Offset | gesetzte Karten-Bits |
|---|---|---|
| ROOM20B0 (1,0x0B) | 0x37DA | 2,3,4 (+Bank35 2,3,4) |
| ROOM2130 (1,0x13) | 0x1842 | 5 (+Bank35 5) |
| ROOM3040 (2,0x04) | 0x1B90 | 2,3,4,5,6,7,8 (+Bank32 0x0D) |
| ROOM3060 (2,0x06) | 0x2852 | 6 |
| ROOM4040 (3,0x04) | 0x0FBC | 7,8 |
| ROOM5040 (4,0x04) | 0x217C | 9,10,11 |
| ROOM5060 (4,0x06) | 0x1032 | 9,10,11 |
| ROOM6120 (5,0x12) | 0x1504 | 0x0D,0x0E,0x0F,0x10,0x11 — szenariogegattert (`21 01 01 01` = ck(Bank1,Bit1,1)) |
**Nie gesetzt (Leon-A-RDTs): Bits 0x00, 0x01, 0x0C, 0x12, 0x13** — fuer diese Bereiche gibt
es keine Karte, dort sieht man immer nur die besuchten Raeume.
**=> Karten sind in RE2 KEINE Gegenstaende, sondern Ereignisse in einem Raum** (Wandplan
auslesen); sie schalten je Fund **einen oder mehrere Bereiche** frei, nicht streng "eine
Karte pro Etage". Die 20 Karten-Ids sind Bereiche/Etagen, die Fundstellen sind 8.

### Befund G: OHNE Karte — was der Spieler sieht (belegter Zweig)
Der MAP-Reiter, das Blatt der aktuellen Etage, die gemalte Grundriss-Grafik aus
`MAPS.PIX` und der Spielerpfeil sind IMMER da. Fehlt das Bank-33-Bit, springt der
Kachelzeichner in `0x8006E730` und zeichnet **nur** die Raeume mit gesetztem
Besucht-Bit (Bank 9); alles andere bleibt ungezeichnet
(`beq v0,zero,0x8006e768` **@0x8006E744**). Besucht = blau `1040b0`, der Raum, in dem
man steht = dunkelrot `680808`. Gegenstandsmarken: keine (Befund E.2).

### Befund H: RE1.5 — GAR NICHT vorhanden
* **Kein Karten-Gegenstand.** Vollstaendige Namenstabelle: Offsettabelle 102 u16
  @`0x800c495c`, Blob @`0x800c4a28`, 0x07-terminiert, Reader `FUN_80028840` @0x80028840
  (`andi a0,a0,0xff` / `sll a0,a0,1` / `lhu v1,0(at)` / `addu v0,v1,v0`). Quelle:
  `re15_port/shared_assets/PSX/BIN/DEBUG.BIN` (laedt @0x800C0000). Alle 102 Namen
  dekodiert — 0x01 "Combat Knife" bis 0x64 "Umbrella File 9"; **kein Eintrag enthaelt
  "Map"**. (Die EXE selbst enthaelt die Tabelle nicht: 0x800c4a28 liegt hinter
  `t_addr+t_size` = 0x800BF000.)
* **Kein Besucht- und kein Besitz-Zustand in der Zeichnung.** RE1.5-Kartenzeichner:
  Seite aus `lbu v1,0(a3)` mit a3=`0x800B260E` **@0x80047048**, Rechteckliste ueber
  `addiu at,at,26688` = `0x80076840` **@0x80047068** (u16 Anzahl) / `0x80076844`
  **@0x8004705C** (Zeiger), Massstabszeilen `addiu at,at,26800` = `0x800768B0`
  **@0x80047080**. Die Zeichenschleife 0x80047128..0x800471FC (Rueckwaertssprung
  `bne v0,zero,0x80047128` **@0x800471FC**) liest ausschliesslich Rechteckfelder
  (`lhu`) — **kein einziges Flag-Byte, kein Aufruf eines Bit-Tests**. RE1.5 malt also
  immer ALLE Rechtecke der Seite.
* **=> RE1.5 hat das System GAR NICHT. RE2-Retail ist massgeblich.**

### Befund I: PORT-STAND (heute)
* **Kein Karten-BESITZ.** Der Port kennt nur "besucht": `re15_map_page_known()`
  `re15_port/engine/src/re15_map_zones.c:741` prueft ausschliesslich
  `re15_map_zone_visited()` bzw. das Etagenbit; ein Besitz-Flag existiert nirgends
  (Volltextsuche ueber engine/src + include: keine Treffer).
* **Zeichnung:** `re15_port/engine/src/re15_inv_screen.c:2282`
  `if (rs == RE15_MAP_RECT_UNVISITED) continue;` — unbesuchte Raeume werden gar nicht
  gemalt. Das entspricht **RE2 OHNE Karte** und ist damit heute schon der richtige
  "Grundzustand"; was fehlt, ist der Zweig MIT Karte.
* **Farben weichen ab.** `re15_inv_screen.c:2307-2308`: besucht `(40,144,40)` = GRUEN,
  aktuell `(192,24,24)` = HELLROT; `re2_ton()` `re15_inv_screen.c:511-513`:
  aktuell `(80,16,0)`, besucht `(0,64,40)`.
  Byte-true RE2 (Befund D): besucht = `0x1040b0` = **(16,64,176) BLAU** (halbtransparent),
  aktuell = `0x680808` = **(104,8,8) DUNKELROT** (halbtransparent).
  Daher der Eindruck des Nutzers "gruen" — das ist Port-Farbe, nicht RE2.
* **Zustand wird gespeichert:** `re15_savedata.c:171` `re15_map_visited_export(out->visited)`,
  `re15_savedata.c:251` Import — v6-Feld `visited[32]`, ein Bit je Zone.
* Etagen/Seiten: 14 Rechtecklisten @0x800762A0 + Seitentabelle @0x80076840 + 106
  Massstabszeilen @0x800768B0, verbatim in `re15_inv_ui_tables.c:397ff`.

### Befund J: Weltmodelle der Karten
In RE2 gibt es **kein Karten-Item** (Befund A) und damit auch keinen
`Item_aot_set`-Record (Op 0x4E, 22 B, `+14 i_item`, `+20 md1`; Handler LAB_80054CD4,
`lbu s2,0x14(s0)` @0x80054CF8) — ein `md1`-Weltmesh, wie es die 15 Dokumentplatzierungen
haben, kann eine Karte also gar nicht tragen. Der Wandplan ist gemalter Hintergrund plus
ein Untersuchen-AOT, der den Bank-33-Bit setzt (Befund F).
**Extrahiert ist stattdessen die Kartenkunst selbst:** `MAPS.PIX` (CD-Id 171, 655 360 B)
= **20 rohe 4bpp-Bloecke a 32 768 B** (kein TIM-Kopf; `LoadImage(RECT{448,256,64,256},
0x801A0000)` **@0x8006D794**). Ausgabe: `build/extracted/re2_karten/` — je Karte
`MAPnn.bin` (roh) plus drei PNG mit den byte-true Paletten
(`_besucht` = CLUT-Y 501, `_aktuell` = 502, `_unbesucht` = 498).
Palettenzensus bestaetigt das Modell: Index 1 = Raumkoerper (die einzige Farbe, die je
Zustand wechselt), Index 4 = Wandlinie, Index 0 = durchsichtig.

### Befund K: Umsetzungsplan fuer den Port
1. **Besitz-Bank einfuehren** — 20 Bits, Analog zu RE2 Bank 33 (`0x800D4924`), Bit =
   Karten-Id 0..0x13. Ablage in `re15_savedata.c` neben `visited[32]` (neue Version v8),
   Export/Import wie `re15_map_visited_export/import`.
2. **Seiten-Id = Karten-Id.** RE1.5 hat 14 Seiten (Tabelle @0x80076840), RE2 hat 20
   Bereiche. Die Zuordnung Raum -> Seite existiert im Port schon
   (`s_map_zones[].page`); ein Besitz-Bit je Seite genuegt.
3. **Zeichner umbauen** — `re15_inv_screen.c:2282`: statt `continue` bei UNVISITED
   pruefen, ob die Seite im Besitz ist; wenn ja, Kachel in SCHWARZ (RE2 CLUT-Y 498)
   mit heller Wandlinie zeichnen, sonst wie bisher ueberspringen.
4. **Farben korrigieren** — `re15_inv_screen.c:2307-2308` und `re2_ton()` Zeile 511-513
   auf die gemessenen Werte `1040b0` (besucht) / `680808` (aktuell) setzen, halbtransparent.
5. **Gegenstandsmarken** — RE2-Analogon zu `FUN_8006dcc0`: Marke nur zeigen, wenn Seite
   im Besitz UND Gegenstand noch da. Der Port hat die Markenliste bereits
   (`re15_map_mark_get`), ihr fehlt nur das Besitz-Gatter.
6. **Fundstellen** — RE1.5 hat keine; entweder RE2-Stellen sinngemaess auf RE1.5-Raeume
   uebertragen (Wandplan im Flur / Wachraum) oder die Karte je Etage beim ersten
   Betreten vergeben. Das ist eine Setzung, keine Messung — dem Nutzer vorlegen.
**Einordnung: RE1.5 hat das System GAR NICHT -> RE2-Retail ist massgeblich.**
