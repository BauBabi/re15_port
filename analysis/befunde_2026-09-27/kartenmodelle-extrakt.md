# RE2-Karten: Weltmodelle, Symbole, Zensus — Extrakt

Status: LAUFEND (Datei wird waehrend der Arbeit gefuellt)
Start: 2026-09-27

## 0. Vorgehen (Vorbild = Dokument-Extraktion der vorletzten Runde)
- [ ] Dokument-Pipeline auffinden (analysis/**, re15_port/tools/**)
- [ ] Karten-Item-IDs in RE2 belegen
- [ ] Item_aot_set-Records ueber alle RE2-RDTs scannen
- [ ] Weltmodelle aufloesen + exportieren
- [ ] Inventar-Symbole exportieren
- [ ] HTML-Auswahlbogen
- [ ] Zensus
- [ ] RE1.5-Gegenprobe

## 1. Vorbild gefunden — die Dokument-Pipeline

Werkzeuge (Hauptbaum, lesend uebernommen):
- `tools/re2_sicherung/re2_doc_worldmodels.py` — Item_aot_set-Zensus ueber alle
  `info/re2leon/PL0/RDT/ROOM*.RDT`, Mesh-Entdopplung per md5, MD1+TIM-Schnitt, OBJ, PNG,
  Kontaktbogen, `_index.txt`.
- `tools/re2_sicherung/re2_scd_walk.py` — opcode-exakter SCD-Walker
- `tools/re2_sicherung/rdt_props.py` — RDT+0x30-Modelltabelle (Anzahl RDT+0x02)
- `tools/re2_sicherung/md1_view.py` — MD1 -> OBJ + texturierte PNG-Ansicht
- `tools/re2_sicherung/re2_items.py` — Item-Namen/Beschreibungen
- `re15_port/tools/re2_dokumente_extrakt.py` — Seiten/Hintergruende/HTML-Auswahlbogen

Belege der Dokument-Runde, die ich UEBERNEHME (und unten selbst nachpruefe):
- Op 0x4E `Item_aot_set`, 22 B: `lbu s2,0x14(s0)` @0x80054CF8 (md1 = Modell-Slot),
  `addiu v0,s0,0x16` @0x80054CFC (Recordlaenge), `lbu s1,0x1(s0)` @0x80054D04 (aot)
- `sltiu v0,s2,0x20` @0x80054D98 — md1 >= 32 (also 255) = KEIN Weltmodell
- Pool 0x800D0324, Schrittweite 0x1F8; Lader FUN_80052D14: `lbu s2,0x2(v0)` @0x80052D70
  (nOmodel = RDT+0x02), `lw s4,0x30(v0)` @0x80052D74 (Tabelle RDT+0x30),
  `addiu s4,s4,0x8` @0x80052DF4 (Paar TIM,MD1)
- Dokument-Grenze: `sltiu v0,a3,0x68` @0x80071BBC

## 2. ⛔ Erster harter Befund: RE2 hat KEIN Karten-ITEM

Vollstaendige Namensbank EN, `NAME_OFFTBL_EN = 0x8009EBAC`, `NAME_BASE_EN = 0x8009E550`
(`lhu v1,-0x1454(at)` @0x80030C24 / `addiu v0,v0,-0x1AB0` @0x80030C2C), alle 140 Indizes
selbst dekodiert: **kein einziger Eintrag heisst "Map" / "Karte" / "Plan"**.
Ids 0..100 = Gegenstaende, 104..128 = Dokumente, 129..139 = Varianten-Namen.
=> Die Karten sind in RE2 **keine Inventar-Gegenstaende**. Der Weg muss ein anderer sein
(AOT-sce-Typ, Flag-Bank). Das wird jetzt disassembliert, nicht vermutet.

## 3. Der Karten-Apparat in RE2 — selbst disassembliert

### 3.1 Es gibt genau 20 Kartenblaetter, als BILDER
- CD-Datei-Id **171** = `MAPS.PIX` (`addiu a0,zero,171` @0x8006D6C0; Dateiname-String
  "MAP FILE" @0x80011C88, geladen @0x8006D734; CD-Lesen `jal 0x80012FB8` @0x8006D76C)
- Slot-Tabelle **0x800A9414**, 20 x 8 B `{u32 groesse; u16 sektor_lo; u8 sektor_hi; u8}`
  gelesen: `lw v0,-27628(at)` @0x8006D718 (groesse), `lhu t0,-27624(at)` @0x8006D754
  (sektor_lo), `lbu v0,-27622(at)` @0x8006D748 (sektor_hi), `lbu v0,-27621(at)` @0x8006D72C
  Rohbytes @0x800A9414: jede Groesse = 0x8000, Sektoren 0x000,0x010,…,0x130 (Schritt 0x10)
  => 20 x 0x8000 = 655360 Byte = **exakt die Dateigroesse von
  `info/re2leon/COMMON/DATA/MAPS.PIX` (655360)**. Gegenprobe geht auf.
- Karten-Kopftabelle **0x800AAA38**, 20 x 8 B `{u32 ptr; u8 n; u8 map_id; u16 pix_slot}`
  (`lhu t0,-21954(at)` @0x8006D6DC liest +6 = pix_slot). Rohbytes selbst gelesen;
  map_id laeuft 0x00..0x13 lueckenlos => **20 Karten**.

### 3.2 ⛔ Die Flag-Bank-Tabelle — Bank 33 ist "Karte im Besitz"
Basis **0x800A78C8** (Xrefs 0x800519DC, 0x80054384, 0x800543E8). Selbst ausgelesen:
Bank 31 = 0x800D4A34, Bank 33 = **0x800D4924**.
Bit-Test `FUN_80077360`: `bits[id>>5] & (0x80000000 >> (id&31))` — MSB zuerst.
Im Kartenbildschirm `FUN_8006dcc0`:
  `FUN_80077360(&DAT_800d4924, DAT_800d5c0a)`  -> "habe ich die Karte dieser Etage"
  `FUN_80077360(&DAT_800d4a34, raum_id)`       -> "war ich in diesem Raum"

### 3.3 Der Karten-Erwerb ist ein FLAG-SETZER, kein Item-Aufheben
SCD-Sprungtabelle 0x800A74C8 (selbst ausgelesen, 143 Eintraege bis op 0x8E):
- op **0x21 = Ck**  -> 0x80054354, 4 B: `+1 bank, +2 bit, +3 sollwert`
  (`lbu v1,1(v0)` @0x8005435C, `lhu a1,2(v0)` @0x80054360, `addiu v0,v0,4` @0x80054364,
   Bank-Tabelle `lw v1,30920(at)` @0x80054384)
- op **0x22 = Set** -> 0x800543B4, 4 B: `+1 bank, +2 bit, +3 wert(0/1/7=toggle)`
  (`lbu v1,1(v0)` @0x800543BC, `lbu a1,2(v0)` @0x800543C0, `lbu a2,3(v0)` @0x800543C4,
   `addiu v0,v0,4` @0x800543C8, Bank-Tabelle `lw v1,30920(at)` @0x800543E8,
   Wert 1 -> @0x800543F8, Wert 0 -> @0x8005440C, Wert 7 = umschalten -> @0x80054420)

**Zensus ueber alle 2553 SCD-Bloecke der 250 RE2-RDTs** (opcode-exakter Walker,
94,55 % der 331419 Bytes gewalkt, 45 Bloecke desynchron):
`Set bank=33` kommt **25 x in 8 Raeumen** vor — das sind ALLE Kartenfunde:

| Raum | Block | Offset | Bits |
|---|---|---|---|
| room20B0 | sub10 | +0x0020 | 2, 3, 4 |
| room2130 | sub05 | +0x0022 | 5 |
| room3040 | sub24 | +0x004E | 2,3,4,5,6,7,8 |
| room3060 | sub22 | +0x0014 | 6 |
| room4040 | sub06 | +0x0014 | 7, 8 |
| room5040 | sub13 | +0x0018 | 9, 10, 11 |
| room5060 | sub04 | +0x0018 | 9, 10, 11 |
| room6120 | sub03 | +0x002A | 13,14,15,16,17 |

Bits 0/1/12/18/19 werden NIE gesetzt (immer verfuegbar bzw. ungenutzt).

### 3.4 Das Weltmodell haengt am OBJEKT-POOL — derselbe wie bei den Dokumenten
`Work_set` (op 0x2E, 3 B) waehlt das "Arbeitsobjekt": `lbu v1,1(v0)` @0x80055920 (Typ),
`lb a1,2(v0)` @0x80055924 (Index), `addiu v0,v0,3` @0x80055928.
Sprungtabelle **0x800111F0**, Index = Typ-1, 5 Faelle (`sltiu v0,v1,0x5` @0x80055934).
**Fall 3 = Typ 4** @0x80055994:
```
80055994  sll   v0,a1,0x6      ; idx*64
80055998  subu  v0,v0,a1       ;  - idx  = idx*63
8005599C  sll   v0,v0,0x3      ;  <<3    = idx*504 = idx*0x1F8
800559A4  addiu v1,v1,804      ; 0x800D0324
800559C4  sw    v0,340(a0)     ; Arbeitsobjekt = Pool[idx]
```
=> **`Work_set 04 <n>` = genau der Objekt-Pool 0x800D0324 mit Schrittweite 0x1F8, den auch
`Item_aot_set.md1` indiziert** (Dokument-Runde, @0x80054DB4). Also: `Work_set 04 n` +
`Pos_set` = das Weltmodell Slot n wegschieben.
`Pos_set` (op 0x32, 8 B) @0x80055BA0: `lh a1,2(v0)/lh a2,4(v0)/lh a3,6(v0)` und
`sw a1,56(v1)/sw a2,60(v1)/sw a3,64(v1)` — x/y/z des Arbeitsobjekts.

### 3.5 Die Namen stehen in den Raumtexten (msg/subNN.msg = EN, mainNN.msg = JP)
- room20B0 sub00: "A *police station map*. Will you take it?"  / sub17: "You've taken a
  *police station map*."
- room2130 sub00: "A *police B1 map*. Will you take it?"

### 3.6 Die sechs Karten mit Namen aus den Raumtexten
| Bank-33-Bits | Name (EN, msg) | Raum | ausloesender AOT | Weltmodell-Slot |
|---|---|---|---|---|
| 2,3,4 | police station map | room20B0 | Aufrufer von sub10 NICHT gefunden | `Work_set 04 08` -> Slot 8, aber **kein Obj_model_set 8** |
| 5 | police B1 map | room2130 | `Aot_set` sub03+0x008C, aot5, sce5, (-17000,-10900) 1200x1070 | Slot 0 @(-16900,-1200,-10400) |
| 6 | sewage disposal map | room3060 | `Aot_set` sub00+0x0178, aot3, sce5, (-14754,-25743) 1800x1800 | Slot 7 @(-13800,-7200,-25250) |
| 7,8 | sewer map | room4040 | `Aot_set` sub05+0x0008, aot4, sce5, (-23641,-12378) 1300x1140 | Slot 2 @(-22814,-2350,-11768) |
| 9,10,11 | factory map | room5040 | `Aot_set` sub00+0x011A, aot3, sce5, (-12249,-22188) 2000x2000 | Slot 1 @(-11599,-2000,-21188) |
| 9,10,11 | factory map | room5060 | `Aot_set` sub00+0x00C8, aot3, sce5, (-12249,-22188) 2000x2000 | Slot 2 @(-10449,-21800,-20988) |
| 13..17 | laboratory map | room6120 | `Aot_set_4p` (op 0x67) sub00+0x000A, aot6, sce5, Viereck | KEIN Work_set — "Will you *file* the laboratory map?" = Konsole |
| 2..8 | (Kinoereignis, kein Fund) | room3040 sub24 | `Evt_exec(0xFF,Gosub,sub24)` sub21+0x00C4 | — |

Verstecken nach dem Fund, zwei Varianten, beide byte-belegt:
- `Work_set 04 n` + `Pos_set` (room20B0 -> 20000/20000/20000, room2130 -> y=-21024,
  room3060 -> y=-32000, room4040 -> 0/0/0)
- `Member_set 0x0C = -32000` (room5040, room5060). Member 0x0C schreibt
  `*(u32*)(obj+0x3C)` (FUN_80055CB0 case 0xC) = **dieselbe Stelle, die `Pos_set` als Y
  beschreibt** (`sw a2,60(v1)` @0x80055BBC). Also: Modell an die Decke schieben.

## 4. Die 20 Kartenblaetter — extrahiert und lesbar
`build/extracted/re2_karten/blaetter/blatt00..19.png` + `.bin` (Rohkoerper) +
`_kontaktbogen_blaetter.png`. Jedes Blatt 256 x 256, 4bpp, 0x8000 B.
Titel stehen IM BILD (selbst angesehen):

| PIX-Slot | Titel im Blatt | Bank-33-Bit (aus Kopftabelle 0x800AAA38 +5/+6) | Raumkacheln (+4) |
|---|---|---|---|
| 0 | CITY AREA | 0 | 8 |
| 1 | CITY AREA | 1 | 4 |
| 4 | POLICE STATION 1F | 2 | 18 |
| 2 | POLICE STATION 2F | 3 | 15 |
| 3 | POLICE STATION 3F | 4 | 3 |
| 5 | POLICE STATION B1 | 5 | 11 |
| 6 | SEWAGE DISPOSAL | 6 | 11 |
| 8 | SEWER B2 | 7 | 6 |
| 7 | SEWER B1 | 8 | 13 |
| 9 | VACANT FACTORY B1 | 9 | 4 |
| 10 | VACANT FACTORY 1F | 10 | 3 |
| 11 | VACANT FACTORY 1F | 11 | 3 |
| 12 | VACANT FACTORY | 12 | 2 |
| 13 | LABORATORY B1 | 13 | 6 |
| 14 | LABORATORY B2 | 14 | 1 |
| 15 | LABORATORY B3 | 15 | 1 |
| 16 | LABORATORY B4 | 16 | 13 |
| 17 | LABORATORY B5 | 17 | 7 |
| 18 | TRANSPORT | 18 | 6 |
| 19 | TRAIN | 19 | 2 |
(Bit == Datensatz-Index, weil Feld +5 lueckenlos 0..19 laeuft; +6 ist der PIX-Slot und
weicht bei 2/3/4 und 7/8 vom Index ab. Summe der Raumkacheln = 137.)

**Ein Fund = MEHRERE Etagen. Die Karten sind NICHT pro Etage zu finden:**
- "police station map" (room20B0) -> Bits 2,3,4 = **1F + 2F + 3F**
- "police B1 map" (room2130) -> Bit 5 = B1
- "sewage disposal map" (room3060) -> Bit 6
- "sewer map" (room4040) -> Bits 7,8 = **B1 + B2**
- "factory map" (room5040 ODER room5060) -> Bits 9,10,11 = **B1 + 1F + 1F**
- "laboratory map" (room6120) -> Bits 13..17 = **B1 + B2 + B3 + B4 + B5**
- Bits 0,1 (CITY AREA), 12 (VACANT FACTORY), 18 (TRANSPORT), 19 (TRAIN) werden von
  KEINEM der 2553 Leon-SCD-Bloecke gesetzt.

## 5. Die Weltmodelle — extrahiert
`build/extracted/re2_karten/weltmodelle/` je Fundort `.md1 .tim .obj _a.png _b.png`.
Schnitt-Gegenprobe (groesster Mesh-Endoffset + 12 == Dateilaenge): **5 von 6 exakt**;
`room5060_slot02` war ueberschnitten (14188 statt 236 B) -> `_exakt.md1` nachgelegt,
dessen md5 ist byte-gleich mit room5040 Slot 1.

**Nur DREI verschiedene Meshes (md5 der MD1-Bytes):**
| md5 | Bytes | Geometrie | benutzt von | angesehen |
|---|---|---|---|---|
| 735097e1 | 1428 | 8 Tri/… | room20B0 Slot 8, room2130 Slot 0 | **zusammengerollte Karte** (Papierrolle, Spirale am Ende sichtbar) |
| 99dae133 |  236 | 4 Tri | room3060 Slot 7, room4040 Slot 2 | **flaches Kartenblatt** mit aufgedrucktem Grundriss |
| 5cdae635 |  236 | 4 Tri | room5040 Slot 1, room5060 Slot 2 | **flaches Kartenblatt**, eigene Textur |

## 6. (b) INVENTAR-SYMBOL: es gibt keines — und das ist belegbar, nicht geschlossen
RE2 hat 101 Item-Records: Eigenschaftstabelle **0x800A9E1C**, 8 B je Item
(`lbu a0,-25057(at)` @0x80069600 = +3 n_mix, `lw v1,-25056(at)` @0x80069618 = +4 mix_ptr,
`addiu v1,v1,4` @0x80069638 = Rezeptschritt); die Tabelle endet bei **0x800AA144**, das ist
die naechste, eigenstaendig gelesene Tabelle (`lhu` @0x8006D0B0, @0x80072520, @0x800727C8,
@0x8007603C, @0x80076224) -> (0x800AA144-0x800A9E1C)/8 = **101**.
Alle 140 Namen der EN-Bank selbst dekodiert: 0..100 Gegenstaende, 104..128 Dokumente,
129..139 Varianten. **Kein Eintrag heisst Map/Plan/Chart.**
=> Keine Item-Id -> kein Eintrag in `ITEMALL.PIX`/`ITPS.ITP` -> **kein Inventar-Symbol.**
Die Karte belegt auch keinen Inventarplatz.

## 7. (c) DAS KARTENBILD: ja, jede Karte hat ein eigenes gezeichnetes Blatt
Siehe Abschnitt 4. Es ist **kein aus Raumdaten konstruierter Grundriss**, sondern ein
vorgezeichnetes 4bpp-Bild mit Titel, N/S-Kompass und Massstabsleiste ("[m] 1:610" bzw.
"1:770"). Die 137 Raumkacheln der Kopftabelle 0x800AAA38 werden als Sprites AUS diesem
Bild geschnitten und je Raumzustand mit einer anderen CLUT gezeichnet
(CLUT-x = 256 `addiu a0,zero,256` @0x8006E74C; CLUT-y 490 @0x8006E3F0, 506 @0x8006E4E4,
510 @0x8006E500, 492 in FUN_8006dea0).

⛔ **Nicht belegt: die Palette meiner PNGs.** MAPS.PIX traegt NUR den 4bpp-Koerper
(20 x 0x8000 = Dateigroesse, nachgerechnet). Ich habe zum Ansehen den CLUT-Block der
bereits im Repo liegenden `MAPS_0NN.TIM` benutzt (cy=480, cw=256, ch=3), Zeile 0,
16-Farben-Block 0. Das ist eine **Ansichtshilfe**, kein Beleg fuer die Spielfarbe.

## 8. Zensus (die harten Zahlen)
```
Kartenblaetter (MAPS.PIX)                      : 20      (Slot-Tab 0x800A9414, 20x0x8000)
Karten-Kopfsaetze (0x800AAA38)                 : 20      (map_id 0..19 lueckenlos)
Raumkacheln ueber alle Blaetter (Feld +4)      : 137
Karten-ITEMS                                   :  0      (140 Namen, 101 Item-Records)
Inventar-Symbole                               :  0
Set(Bank 33) in 250 RDTs / 2553 SCD-Bloecken   : 25 in 8 Raeumen
   Brute-Force-Gegenprobe (jede Byte-Position) : 25, davon NEU 0
   Walker-Abdeckung                            : 313356 / 331419 B = 94,55 %, 45 Bloecke desync
Ck(Bank 33) in den RDTs                        :  0      (nur die EXE liest die Bank)
Schreiber von 0x800D4924 ausser dem Set-Opcode :  0      (alle 8 Xrefs sind Lesezugriffe
                                                          in FUN_8006dcc0/8006e120/8006f1c4
                                                          + der Bankzeiger 0x800A794C)
Karten-Fundstellen mit Weltmodell-Slot         :  6 von 7  (room6120 = Konsole)
davon mit Obj_model_set in PL0                 :  5 von 6  (room20B0 Slot 8 fehlt)
VERSCHIEDENE Meshes (md5 der MD1-Bytes)        :  3
Schnitt-Gegenprobe exakt                       :  5 von 6  (room5060 Slot 2 ueberschnitten,
                                                            exakt = 236 B, dann md5-gleich
                                                            mit room5040 Slot 1)
```

## 9. RE1.5-GEGENPROBE
**Kartenblaetter: VORHANDEN.** `info/Re1.5/PSX/DATA/MAP01.PIX … MAP0D.PIX`, **13 Stueck**,
je 32768 B — **byte-gleiches Format wie ein RE2-Slot** (256x256, 4bpp). Mit derselben
Palette gerendert lesbar, Titel im Bild (selbst angesehen,
`build/extracted/re2_karten/re15_gegenprobe/_kontaktbogen_re15.png`):
MAP01 POLICE STATION B1 · MAP02 …B2 · MAP03 …1F · MAP04 …2F · MAP05 …3F ·
MAP06 POLICE STATION (ohne Etage) · MAP07 DRAINS B2 · MAP08 FACTORY ·
MAP09 LABORATORY B1 · MAP0A …B2 · MAP0B …B3 · MAP0C …B4 · MAP0D SUBWAY.

**Karten-ITEMS: GAR NICHT.** Die RE1.5-Namensliste in `DEBUG.BIN` ab 0x4A28 hat 101
Eintraege (Combat Knife … Umbrella File 9) — **kein einziger ist eine Karte**.

**Fund-Mechanismus: GAR NICHT.** Die angezeigte Kartenseite haengt in RE1.5 allein an der
Raumnummer, ohne jede Flag-Abfrage — selbst disassembliert in `info/Re1.5/PSX.EXE`:
```
8004b56c  lh    v1,4066(v1)        ; 0x800B0FE2 = Raum/Stage-Nummer
8004b574  sltiu v0,v1,0x26         ; < 38
8004b584  addiu at,at,4156         ; Sprungtabelle 0x8001103C
8004b58c  lw    v0,0(at)
```
=> **Weltmodelle von Karten kann es in RE1.5 nicht geben**: es gibt weder eine Item-Id
noch ein Besitz-Bit, an dem ein Fund haengen koennte. Genau dasselbe Bild wie bei den
Dokumenten ("der Mechanismus ist da, aber nichts steht in der Welt") — nur noch eine Stufe
leerer: hier fehlt sogar der Mechanismus, waehrend die 13 Blaetter fertig gezeichnet sind.
RE1.5 hat das System also **TEILWEISE** (Daten ja, Erwerb nein).

## 10. Ausgabe
`build/extracted/re2_karten/`
- `uebersicht_modelle.html` — der Auswahlbogen (Symbol/Weltmodell/Name/Fundort
  nebeneinander, plus alle 20 Blaetter und die 13 RE1.5-Blaetter)
- `weltmodelle/<raum>_slot<NN>.{md1,tim,obj,_a.png,_b.png}` (6 Platzierungen, 3 Meshes)
  + `room5060_slot02_exakt.md1`
- `blaetter/blatt00..19.{bin,png}` + `_kontaktbogen_blaetter.png`
- `re15_gegenprobe/MAP01..MAP0D.png` + `_kontaktbogen_re15.png`
- `karten.csv`, `karten.json`, `zensus.json`
Werkzeuge: `tools/re2_karten/re2_karten_extrakt.py`, `tools/re2_karten/re2_karten_html.py`
(bauen auf `tools/re2_sicherung/{re2_scd_walk,rdt_props,md1_view,msg_decode,
re2_doc_worldmodels,re15_items}.py` auf — dieselbe Pipeline wie die Dokumente).
⛔ Im selben Ordner liegen zusaetzlich `MAP00..MAP19_*.png/.bin` eines PARALLEL laufenden
Agenten (Kartensystem-Mechanik). Die sind nicht von mir.

## 11. ⛔ OFFEN
1. **Wer startet room20B0/sub10?** Kein `Aot_set`/`Aot_set_4p`/`Gosub`/`Evt_exec` in
   room20B0 verweist auf sub10. Versucht: (a) opcode-exakter Walk aller 15 Bloecke,
   (b) Brute-Scan des einzigen desynchronen Blocks sub07 (das ist eine
   Sce_bgm_control-Tabelle, 258 B, vollstaendig ausgedruckt), (c) Suche nach `2d 08`
   (Obj_model_set Slot 8) im GESAMTEN Raum — 0 Treffer. Naechster Weg: Claires RDT (PL1)
   liegt nicht im Repo; sonst die AOT-Liste zur Laufzeit in DuckStation mitschreiben.
2. **Warum hat room20B0 kein `Obj_model_set 8`?** Das Mesh in Slot 8 ist byte-gleich mit
   room2130 Slot 0 (md5 735097e1) — also sehr wahrscheinlich dieselbe Papierrolle. In
   Leons Skript wird sie nie aufgestellt; der Gegenstand steckt dann im
   vorgerenderten Hintergrund. NICHT am Bild belegt (die Projektion aus
   `tools/re2_sicherung/re2_aot_on_bg.py` habe ich nicht gefahren).
3. **room6120 (laboratory map):** kein `Work_set` -> ich kann keinem der 13 Modellslots
   die Karte zuordnen. Der Text sagt "Will you *file* the laboratory map?" — eine Konsole,
   kein Aufheben. Welches der Slots 0..11 die Konsole ist, ist nicht belegt.
4. **Bits 0, 1, 12, 18, 19** (CITY AREA x2, VACANT FACTORY, TRANSPORT, TRAIN) werden von
   keinem Leon-Skript gesetzt und die EXE schreibt die Bank nicht. Ob diese Blaetter
   trotzdem gezeigt werden, haengt an der Anzeige-Logik in FUN_8006dcc0/FUN_8006e120 —
   das gehoert in das Schwester-Dossier `kartensystem-mechanik.md`.
5. **room3040/sub24** setzt die Bits 2..8 in einem Kinoereignis
   (`Cut_chg`, `Plc_ret`, `Sce_bgm_control`, `Save 24 23 00 00`), ausgeloest von
   `Evt_exec(0xFF, Gosub, sub24)` @room3040/sub21+0x00C4. Welches Ereignis das im
   Spielverlauf ist, habe ich nicht bestimmt.
6. **Die echte Spiel-Palette der Kartenblaetter** (s. Abschnitt 7).

## 12. PORT-STAND (nur gelesen, nichts geaendert)
- Der Port zeigt die 13 RE1.5-Blaetter bereits: `re15_port/include/re15_inv_screen.h:82`
  ("MAP0x.PIX floor-plan page. Upload rect (448,256,64,256)") — **dasselbe VRAM-Rechteck
  wie RE2** (`addiu v0,zero,448` @0x8006D77C / `256` @0x8006D780 / `64` @0x8006D788).
- Die Seite folgt der Raumnummer: `re15_inv_screen.c:341` `re15_inv_map_page_shown()`,
  Zuordnung `re15_inv_screen.c:376-422` — byte-true zu RE1.5.
- Blaettern mit Hoch/Runter ist eine **Port-Ergaenzung** und im Code auch so markiert:
  `menu_common.c:1402-1440` ("⛔ PORT-ERGAENZUNG, kein byte-true Befund"), Riegel
  `re15_map_page_known()` `re15_map_zones.c:741-753` — der prueft **besucht**, nicht
  **gefunden**.
- Ein Karten-Besitz gibt es im Port nicht, und in RE1.5 auch nicht.

## 13. UMSETZUNGSPLAN (RE2 als Ziel, weil RE1.5 hier unfertig ist)
1. **Besitz-Bitfeld** wie RE2 Bank 33: 20 (bei RE1.5 13) Bits, MSB-zuerst adressiert
   (`bits[id>>5] & (0x80000000 >> (id&31))`, FUN_80077360 @0x80077360), im Spielstand.
   Anzeige-Riegel: Blatt nur waehlbar, wenn Bit gesetzt — das ersetzt den heutigen
   Besucht-Riegel in `re15_map_page_known()`.
2. **Fundstellen**: je Fund ein `Aot_set` mit sce=5 -> Unterprogramm mit
   Frage-Message, `Set`(Kartenbank, Bits), `Aot_reset`, Bestaetigungs-Message. Genau die
   Reihenfolge der sechs RE2-Bloecke (Abschnitt 3.6). ⛔ Welche RE1.5-Raeume das sein
   sollen, ist NICHT aus RE1.5 ableitbar (es gibt dort keine solchen Records) — das ist
   eine Nutzer-Entscheidung, keine Messung.
3. **Weltmodell**: der Port liest Prop-Modelle schon aus dem RDT
   (`rdt_common.c:51-79`, `pc_load_room_prop_set`). Die drei RE2-Meshes liegen jetzt als
   `.md1/.tim/.obj` in `build/extracted/re2_karten/weltmodelle/`. Einspeisen heisst
   entweder ins RDT schreiben oder portseitig als Zusatz-Prop fuehren — dieselbe
   Entscheidung wie bei den Dokumenten.
4. **Verstecken nach dem Fund**: `Work_set 04 <slot>` + `Pos_set`/`Member_set 0x0C`
   (beide schreiben obj+0x38/0x3C/0x40). Der Port hat dieselbe Pool-Struktur.
5. **Ein Fund = mehrere Etagen** (RE2: 1 bis 5 Bits je Fund). Die Bits-je-Fund-Tabelle aus
   Abschnitt 3.6 ist die Vorlage.
