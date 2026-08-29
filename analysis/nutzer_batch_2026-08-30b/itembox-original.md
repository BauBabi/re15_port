# Itembox im RE1.5-Original — hat der Auslieferungsstand einen deaktivierten Ansatz?

**Datum:** 2026-08-30 · **Status:** RECON abgeschlossen, alle tragenden Aussagen byte-belegt
**Quellen:** `info/Re1.5/PSX.EXE` (Disasm via `re15_disasm.py`), alle 240 `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`,
`re15_port/shared_assets/PSX/STAGE*/ROOM*.BSS` (byte-identisch zu `info/Re1.5/PSX/`), Vorbefund
`shots/itembox_verdict.md` (wf_773c1d9f, 4 Agenten-Reports, spot-verifiziert), `analysis/save_injured_state.md` §1,
`analysis/nutzer_batch_2026-08-29/schlafender-content.md`, Port-Gegenstellen (`re15_itembox.c/h`, `scd_vm.c`, `aot_common.c`).
Decoder: Message-Charset = verifizierte ASCII-Region (`msg_common.c:177-201`, byte = ASCII−0x24);
SCD-Walker mit RE1.5-Opcode-Längentabelle (`.claude/skills/scd-disassembly/SKILL.md`); BMP-Decodes der
BSS-Frames aus dem Vorgänger-Repo `C:/workspace/git/reAi/extracted/` (Quell-Frames gegen `info/Re1.5` byte-verifiziert).

---

## 0. Kurzantwort

**JA — aber nur als CONTENT, nicht als CODE.** Der Auslieferungsstand enthält einen kompletten,
bewusst stillgelegten Itembox-**Interaktions-Layer**:

1. **8 Save-Raum-Paare** tragen je einen Box-Interaktionspunkt: Examine-AOT → eigener Kamera-Cut →
   Message „Itembox is not available in this preview." In **7 von 8 Paaren ist das AKTIV verdrahtet**
   (im Spiel auslösbar); nur 6020/6021 ist eine Waisen-Szene ohne Trigger (§2).
2. **NEUFUND dieses Laufs:** Der Kamera-Cut jeder Box-Szene zeigt als vorgerendertes BSS-Hintergrundbild
   ein **Mockup des Itembox-UI-Screens** („Item Storage" / „Item list" / fünf „NO DATA"-Zeilen) —
   byte-identisch in allen 8 Räumen dupliziert (§3). Die frühere Verdikt-Aussage „box-UI art did not ship"
   (`shots/itembox_verdict.md` Report stringsAssets) ist damit **widerlegt** — die Box-UI-Grafik ist
   ausgeliefert, sie liegt nur nicht als TIM, sondern als MDEC-Frame in den BSS-Dateien.
3. **Im Save-Block liegen vier dormante 8-Slot×4-Byte-Arrays** (@0x800b1444/1484/14a4/14c4), die von
   New-Game-Init genullt und vom (dormanten) Card-FSM mitserialisiert werden — Geometrie passt auf
   4 Box-Seiten × 8 Slots im RE1.5-Slot-Format; **kein einziger Leser/Schreiber existiert** (Zweck
   statisch unentscheidbar, §4.1).
4. **Box-CODE existiert NICHT** — kein Screen, kein Transfer, kein sce-Handler, keine Strings
   (instruction-shape-Negative über alle 9 Binaries, §4; Vorbefund `shots/itembox_verdict.md`,
   tragende Punkte in diesem Lauf nachdisassembliert).

Der Port hat seine RE2-portierte Box **bereits an den originalen Interaktionspunkten angebunden**
(Message-Intercept `scd_vm.c:1381`); offene Punkte sind nur 6020/6021 (kein Original-Trigger → Box dort
unerreichbar) und die Frage, was während der Box-Szene mit dem Mockup-Hintergrund passiert (§6).

---

## 1. Der Skript-Layer: Box-Interaktionspunkte in allen 8 Save-Raum-Paaren

### 1.1 Das Szenen-Template (alle 16 RDTs identisch aufgebaut)

Jede Box-Szene ist ein eigener SCD-sub mit exakt diesem Bytecode (Beispiel ROOM6020 sub02, Datei-Offsets):

```
0x0BC4  29 08          Cut_chg 8         ; Wechsel auf den dedizierten Box-Cut
0x0BC6  2b 00 ff ff    Message_on id=0   ; „Itembox is not available in this preview."
0x0BCA  02             Evt_next          ; auf Bestätigung warten
0x0BCB  00             Nop
0x0BCC  2a             Cut_old           ; Kamera zurück
0x0BCD  00             Nop
0x0BCE  3c 01          Cut_auto 1
0x0BD0  01 00          Evt_end
```

Gleiche Struktur (nur Cut-/msg-Nummern verschieden) in allen anderen Räumen — vollständig gewalkt
mit der RE1.5-Opcode-Längentabelle, null Desyncs in diesen 16 Dateien.

### 1.2 Der Trigger-Mechanismus (sce=3-AOT mit eingebettetem Gosub)

`Aot_set` (0x2C, 20 B): `[op][slot][sce][floor] [pad u16][x s16][z s16][w s16][h s16] [payload u16×3]`.
Für sce=3 verarbeitet der Original-Handler @`0x800430f0` die Payload als
`FUN_8003ee3c(cond = u16@payload+0, sub = u8@payload+3)`:

```
800430fc: lhu a0,0(v0)      ; cond  (bei allen Box-AOTs 0x00FF)
80043100: lbu a1,3(v0)      ; sub-Nummer = High-Byte des 2. Payload-Worts
80043104: jal 0x8003ee3c    ; Event-Exec auf sub N
```

(Port-Gegenstelle, deckungsgleich: `aot_common.c:204` und `:245` „sub u8@payload+3 (@0x80043100)".)
Das 2. Payload-Wort ist dabei literal das Byte-Paar `18 NN` — als u16 gelesen `0x0N18`, d.h. die
sub-Nummer steht im High-Byte.

### 1.3 Vollständige Ortstabelle (Ergebnis des Sweeps über alle 240 RDTs)

Die Preview-Message existiert in **exakt 16 RDTs = 8 Save-Raum-Paaren** (Charset-dekodierter Sweep über
die Message-Sektion `RDT+0x3C` aller 240 RDTs; deckungsgleich mit dem encoded-pattern-Census in
`shots/itembox_verdict.md` Report stringsAssets):

| Raum | Aot_set (Datei-Offset, main00) | Slot | AOT-Rect (x, z, w, h) | → sub | Cut_chg | msg | msg-Body | Status |
|---|---|---|---|---|---|---|---|---|
| 1150 | @0x0DCE | 5 | (−25100, −12600, 1600, 1000) | sub07 @0x10F8 | 8 | 03 | @0x1426 | **verdrahtet** |
| 1151 | @0x0DCE | 5 | dito | sub07 @0x10D6 | 8 | 03 | @0x1204 | **verdrahtet** |
| 2010 | @0x07A8 | 5 | (3650, −6600, 1600, 1000) | sub03 @0x08EE | 5 | 02 | @0x0947 | **verdrahtet** |
| 2011 | @0x07A8 | 5 | dito | sub03 @0x090C | 5 | 02 | @0x0967 | **verdrahtet** |
| 30A0 | @0x0758 | 2 | (−19250, 18500, 1600, 1000) | sub02 @0x0876 | 6 | 00 | @0x08A0 | **verdrahtet** |
| 30A1 | @0x0758 | 2 | dito | sub02 @0x0876 | 6 | 00 | @0x08A0 | **verdrahtet** |
| 30B0 | @0x0932 | 1 | (−23750, −16250, 1600, 1000) | sub02 @0x098A | 7 | 00 | @0x09A4 | **verdrahtet** |
| 30B1 | @0x0932 | 1 | dito | sub02 @0x098A | 7 | 00 | @0x09A4 | **verdrahtet** |
| 4010 | @0x057E | 8 | (−22450, −26000, 1600, 1000) | sub07 @0x0F92 | 5 | 42 (0x2A) | @0x1906 | **verdrahtet** |
| 4011 | @0x0596 | 9 | dito | sub02 @0x0634 | 5 | 06 | @0x07D7 | **verdrahtet** |
| 5010 | @0x044A | 8 | (−24050, −16550, 1600, 1000) | sub02 @0x04E8 | 4 | 05 | @0x0691 | **verdrahtet** |
| 5011 | @0x045E | 10 | dito | sub07 @0x0E26 | 4 | 44 (0x2C) | @0x174A | **verdrahtet** |
| 6020 | **— kein AOT —** | — | — | sub02 @0x0BC4 | 8 | 00 | @0x0BDE | **WAISE** |
| 6021 | **— kein AOT —** | — | — | sub02 @0x0BC4 | 8 | 00 | @0x0BDE | **WAISE** |
| 6030 | @0x0CDA | 6 | (−18600, −26000, 1600, 1000) | sub04 @0x0FF6 | 13 | 09 | @0x157A | **verdrahtet** |
| 6031 | @0x0CDA | 6 | dito | sub04 @0x0E1C | 13 | 03 | @0x11CE | **verdrahtet** |

Payload-Belege (2. Wort = `18 NN`): 1150/1151 slot5 = 0x0718→sub07; 2010/2011 slot5 = 0x0318→sub03;
30A0/30B0 = 0x0218→sub02; 4010 slot8 = 0x0718→sub07; 4011 slot9 = 0x0218→sub02; 5010 slot8 = 0x0218;
5011 slot10 = 0x0718; 6030/6031 slot6 = 0x0418→sub04.

**Wichtig — Korrektur zweier Vorbefunde:**
- `shots/itembox_verdict.md` („all 16 safe rooms wiring the box object to a message-only AOT") ist für
  **6020/6021 falsch**: dort existiert im gesamten SCD **kein einziger `Aot_set`** (main00 = nur zwei
  `Door_aot_set` @0x0B46/@0x0B66 + `Evt_end` @0x0B86; sub00 = zwei bedingte NPC-Spawns; sub01 leer).
  `schlafender-content.md` §0/§6 hatte die 6020-Waise bereits korrekt.
- Umgekehrt gilt aber NICHT „die Box-Meldung ist generell schlafend": in den anderen **7 Paaren ist der
  Interaktionspunkt im ausgelieferten Spiel aktiv** — Hinlaufen + Action am AOT zeigt Cut + Meldung.

### 1.4 Kontext: dieselben Räume sind die Save-Räume

In 6 der 8 Paare liegt direkt daneben der Save-Interaktionspunkt („It's a phone/computer… Save is not
available in this preview", z.B. 1150 msg01 @0x1385, 2010 msg03 @0x0974, 30A0 msg01 @0x08CD,
4010 msg43 @0x1933, 5010 msg06 @0x06BE). Die 8 Ortsnamen im dormanten Save-Textbank (DEBUG.BIN @0x5f96,
Einträge [26..33]) mappen 1:1 auf diese 8 Paare (`shots/itembox_verdict.md` Report stringsAssets).
6020/6021 hat als einziges Paar WEDER Save- noch (verdrahtete) Box-Interaktion.

---

## 2. NEUFUND: Das Itembox-UI-Mockup in den BSS-Hintergründen

### 2.1 Der Box-Cut ist ein Platzhalter-Kamera-Record + ein echtes vorgerendertes Bild

Der `Cut_chg`-Zielcut jeder Box-Szene ist immer der **letzte Eintrag** der RID-Kameratabelle
(`RDT+0x24`, 32-B-Einträge: u16 flag, u16 fov, s32 cam x/y/z, s32 target x/y/z, u32 pri) und trägt in
allen 8 Räumen denselben Stock-Datensatz:

```
flag=0x0001  fov=26684  cam=(−20500,−5400,−11000)  target=(0,5400,0)
```

(z.B. ROOM6020 cut8 @RDT 0x0160; 1150 cut8 @0x0160; 2010 cut5 @0x0100; 30A0 cut6 @0x0120; 30B0 cut7
@0x0140; 4010 cut5 @0x0100; 5010 cut4 @0x00E0; 6030 cut13 @0x0200.) Derselbe Stock-Record dient auch
anderen Close-up-Szenen (ROOM1180 cut9/10 = Kartenleser-Close-ups, ROOM1230 cut10 = Keypad; 4010 cut4
ist ein ungenutzter Extra-Cut mit echtem Raum-BG) — der 3D-Kamera-Record ist bei solchen Close-ups
bedeutungslos, das BILD kommt aus dem BSS-Frame des Cuts.

### 2.2 Der BSS-Frame zeigt das Box-UI

Die BSS-Datei jedes Raums enthält pro Cut einen 64-KB-MDEC-Frame (`ROOM602.BSS` = 0x90000 = 9 Frames für
nCut=9). Der Frame des Box-Cuts ist in **allen 8 Räumen das byte-identische Bild** (roh-Frame-md5
`a4e1ab9ca9d0…`, verifiziert für 1150-Frame 8 == 6020-Frame 8; alle 8 dekodierten BMPs md5-identisch
`45cb902f5baa…`):

**Ein Mockup des Itembox-Screens** (siehe `itembox-mockup-bss.png` neben diesem Bericht):
- zwei Fenster im Windows-9x-Stil mit Titelleiste „ITEM LIST",
- links ein Panel **„Item Storage"** mit fünf Listenzeilen **„NO DATA"** (eine Zeile blau markiert),
- rechts ein schmaleres Panel **„Item list"**,
- Fußzeile beider Fenster: „© J…gy system .19xx" (Kleinschrift, nicht sicher lesbar — vermutlich der
  Name des internen Windows-Tools, aus dem der Screenshot stammt).

Fundstellen (Frame-Offset in der BSS-Datei; `re15_port/shared_assets/PSX/` == `info/Re1.5/PSX/`):

| Datei | Frame | Byte-Offset |
|---|---|---|
| STAGE1/ROOM115.BSS | 8 | 0x80000 |
| STAGE2/ROOM201.BSS | 5 | 0x50000 |
| STAGE3/ROOM30A.BSS | 6 | 0x60000 |
| STAGE3/ROOM30B.BSS | 7 | 0x70000 |
| STAGE4/ROOM401.BSS | 5 | 0x50000 |
| STAGE5/ROOM501.BSS | 4 | 0x40000 |
| STAGE6/ROOM602.BSS | 8 | 0x80000 |
| STAGE6/ROOM603.BSS | 13 | 0xD0000 |

**Bedeutung:** Im ausgelieferten Spiel schaltet die Box-Szene die Kamera auf diesen Frame — der Spieler
sieht das UI-Mockup als Standbild, darüber die Preview-Meldung. Das ist der visuell konkreteste
Beleg für den gestrichenen Itembox-Plan: zwei-Panel-Layout „Storage ↔ list", genau die
RE2-Box-Semantik. (Decode-Kette: BMPs aus `C:/workspace/git/reAi/extracted/PSX/...` [BssExtractor des
Vorprojekts]; deren Quell-Frames byte-identisch zu `info/Re1.5/PSX/STAGE6/ROOM602.BSS` Frame 8 und
`.../STAGE1/ROOM115.BSS` Frame 8, md5-geprüft in diesem Lauf.)

---

## 3. ROOM6020/6021 im Detail (die Waisen-Szene)

- **SCD komplett** (Voll-Walk, alle Opcodes): main00 = `Door_aot_set` @0x0B46 (→ Ziel per Payload) +
  `Door_aot_set` @0x0B66 + `Evt_end` @0x0B86. sub00 @0x0B8E: `Ifel_ck`/`Ck(3,0x14C)` → 2× `Sce_em_set`
  (NPC-Typen 0x40/0x42) → `Endif`. sub01 = leer. sub02 = die Box-Szene (§1.1). **Kein `Aot_set`, kein
  `Obj_model_set`, kein Item-AOT im ganzen Raum.**
- **sub02 hat keinerlei Trigger** (kein Evt_exec/Evt_chain/Gosub, kein sce-3-AOT — `sub_refs` leer;
  deckungsgleich `schlafender-content.md` §0 Punkt 2).
- Der Raum hat **nCut=9** (Header-Byte 1), Kamera 8 = Stock-Record (§2.1), ROOM602.BSS Frame 8 = das
  Mockup — d.h. Cut, Bild und Message sind fertig, NUR der AOT wurde nie gesetzt (bzw. entfernt).
- **Kein Box-Objekt/Prop:** nOmodel=1 (Header-Byte 2), im SCD kein `Obj_model_set` — die Box wäre hier
  (wie in allen 8 Räumen) nur im vorgerenderten Hintergrund sichtbar gewesen; ein 3D-Requisit gibt es
  nicht. Aktivierung per `RE15_FORCE_EVENT=2` in Raum 6020 möglich (schlafender-content.md §5).

---

## 4. EXE-Seite — was existiert, was fehlt

Vorbefund `shots/itembox_verdict.md` (4 unabhängige Reports); die tragenden Punkte wurden in diesem
Lauf nachdisassembliert:

### 4.1 Save-Block: vier dormante Box-förmige Arrays (nachverifiziert)

`FUN_8003e4f4` (New-Game-Init, einziger Caller `jal @0x8001d570`) nullt vier 8×4-Byte-Arrays — in diesem
Lauf nachdisassembliert:

```
8003e50c: addiu t0,v0,5316   ; 0x800b14c4
8003e510: addiu a3,v0,5284   ; 0x800b14a4
8003e514: addiu a2,v0,5252   ; 0x800b1484
8003e518: addiu a1,v0,5188   ; 0x800b1444
8003e52c: sw zero,0(a1) … 8003e538: sw zero,0(t0)   ; 4× sw zero
8003e54c: sltiu v0,a0,0x8                            ; 8 Iterationen à 4 Byte
```

- Lage: im 0x1230-Byte-GSB @0x800b0fbc (Offsets +0x488/+0x4C8/+0x4E8/+0x508), den das dormante
  Card-FSM als Ganzes serialisiert (`memcpy` @0x800261c4–d4, `ori a2,zero,0x1230` @0x800261d0); im
  tatsächlichen 0x800-Byte-Kartenfile liegen sie bei +0x688..+0x727 — **innerhalb** des geschriebenen
  Fensters (`ori v0,zero,0x800` @0x80026220; Blob-Layout `analysis/save_injured_state.md` §1.1).
- **Null Leser/Schreiber** in allen 9 Binaries außer dem Zero-Loop (Ghidra-Xref + Konstanten-Propagations-Scan,
  itembox_verdict Report dataHunt); in 10 Savestates durchgehend 0x00.
- Geometrie = 4 Seiten × 8 Slots × 4 Byte (RE2 erzwingt 8 sichtbare Box-Slots; RE1.5-Slot = {id,qty,kind,pad}) —
  aber 0x20 Byte ist auch exakt eine 256-Bit-Flag-Zone. **Zweck statisch unentscheidbar**; der 0x20-Lücken-Gap
  0x800b1464–1484 bleibt ungenullt (offene Frage im Verdikt).
- Der Port nutzt genau diese Form als Box-Storage (RE15_BOX_PAGES=4 × 8, `re15_itembox.h:45-48`,
  Provenienz-Flag „RE1.5-plausible only").

### 4.2 Menü-System: kein toter Box-Zweig, aber ein dormantes Verb-Palette-Screen-2

Status-Master-Task LAB_8004603c, Phasentabelle @0x80074bdc (in diesem Lauf gedumpt: [0..2] State-Fns
0x800460b8/0x80046500/0x80046540, [3..5] Init {0x80049524, 0x8004c54c, **0x8004d7cc**}, [6..8] Run
{0x8004974c, 0x8004cca4, **0x8004d904**}, [9..11] Draw {0x80049a5c, 0x8004c830, **0x8004d96c**},
[12..19] weitere Subscreen-Phasen). Screen-Selector-Byte 0x800b25c0 dispatcht OHNE Bounds-Check:
- Screen 0 = Status-Screen, Screen 1 = Close-up/MAP-Viewer (Selector=1 wird einzig @0x800431a8 gesetzt).
- **Screen 2 = dormante Item-KOMMANDO-Palette** (Chrome + ARMS + 10-Slot-Grid + TEX.TIM-Buttons
  CHECK/COMBN/USE/TAKE/EXIT) — Run-FSM ist auf „draw + beliebige Taste = exit" ausgeweidet
  (26 Instruktionen @0x8004d904), **liest nur das EINE Inventar-Array 0x800b10ac** — keine Box.
  Selector-Wert 2 wird **nirgends** geschrieben (Scan über EXE + DEBUG/TITLE/STAGE1-6.BIN).

### 4.3 sce-Dispatch @0x8007469c: kein Handler ist ein Box-Einstieg

Tabelle in diesem Lauf gedumpt — 14 Handler {0x8004305c, 0x80043084, 0x800430bc, 0x800430f0,
0x80043120, 0x8004318c, 0x80043194, 0x800431cc, 0x8004330c, 0x80043328, 0x80043370, 0x800434b4,
0x80043500, 0x800435cc}. RDT-Nutzungscensus (itembox_verdict Report stringsAssets, 1568 AOTs):
verwendete sce-Werte 0,1,2,3,4,5,7,8,9,12,13 → **unbenutzt: 5-Stub, 6, 10, 11**. Identität der
Kandidaten (diesmal disassembliert):

| sce | Handler | Befund |
|---|---|---|
| 5 | @0x8004318c | **NOP-Stub**: `jr ra / addu v0,zero,zero` (2 Instr.) — Examine-Marker |
| 6 | @0x80043194 | echtes dormantes Code: öffnet Screen 1 (`sb 1 → 0x800b25c0` @0x800431a8 + Freeze-Bit 0x8000 → DAT_800aca3c) — Close-up-Viewer, **kein** Box-Screen |
| 10 | @0x80043370 | **DAMAGE-Handler**: liest entity+0x93-Latch (@0x80043380), `lhu v0,154(a1)` HP−payload (@0x80043394-a4), bei HP<0 State 3 (Tod, @0x800433d4), sonst State 2 |
| 11 | @0x800434b4 | **Positions-Add**: payload u16@+0/+2/+4 += entity+0x34/+0x38/+0x3c |
| 8 | @0x8004330c | 7-Instr-Setter: `sh payload@+0 → entity+0x88` (32× von RDTs genutzt) |

→ **Keiner der 14 Handler öffnet eine Box oder berührt ein zweites Item-Array.** RE2s dediziertem
Box-AOT-Typ entspricht in RE1.5 nichts; die RE1.5-Nummerierung weicht ohnehin ab (Tür=2, Item=9).

### 4.4 Kein Box-Code (Shape-Negative, Vorbefund)

Alle fünf RE2-Box-Instruktionsformen fehlen in allen 9 Binaries (cmd-0x0e/0x0f-Paar, Capacity-forced-8
— Capacity-Byte 0x800b0fbc hat 19 Xrefs, alle Reads; Zwei-Array-Transfer-Loop — 18-Instr-Fenster-Scan;
Free-Slot-Scanner FUN_8004df2c hat 3 Caller, keiner Box; kein Box-Array-Analog). Alle 157 Code-Stellen
des Inventar-Arrays 0x800b10ac liegen in 0x8004641c–0x8004ed54, keine paart es mit einem zweiten
Item-Array. RE2-Referenz: Box-Screen FUN_80068f9c/FUN_80069714/FUN_80069668, Storage DAT_800a93d0
(3-Byte-Stride, in .data VOR-initialisiert — RE1.5 hat nirgends ein vorinitialisiertes Item-Array).
(`shots/itembox_verdict.md` Reports codeShape/dataHunt.)

### 4.5 Texte/Grafik

- **Keine Box-Strings** in EXE/DEBUG.BIN/TITLE.BIN/Overlays (encoded + ASCII; Kalibrierung: auch RE2s
  ECHTER Box-Screen ist textlos — das Negativ ist notwendig, nicht hinreichend).
- **DEBUG.BIN-sysmes @0x5f96** (34 Einträge) = ausschließlich Save-System-Texte + 8 Save-Ortsnamen;
  der Dev-Status-Tool liest Items nur (Scan @0x800c63e0/@0x800c6600-28).
- ITEM/-Verzeichnis (ITPS.ITP, STPIC_00-0F.TIM), ITEMALL.PIX, MIXITEM.PIX: Inventar-/CHECK-Grafiken,
  nichts Box-artiges; ST_00.TIM-Restzeilen = ID-Card + Dev-Menü-Labels.
- **KORREKTUR (§2): die Box-UI-Grafik existiert doch** — als dupliziertes BSS-Mockup-Frame in den
  8 Save-Räumen, vom Asset-Hunt des Verdikts übersehen (der suchte nach Box-benannten Dateien/TIMs,
  nicht in BSS-Frame-INHALTEN).

---

## 5. Antworten auf die Nutzer-Frage

**(a) Hat das Original einen deaktivierten Itembox-Ansatz? — JA, dreiteilig, aber ohne Code:**
1. Aktive Interaktionspunkte (7/8 Paare) mit Preview-Meldung, dediziertem Cut und ausgeliefertem
   UI-Mockup-Bild (§1, §2) — der „Box-Platz" ist im Spiel praktisch markiert.
2. Save-persistierte, genullte 4×8-Slot-Storage-Arrays im GSB (§4.1) — Form passt, Zweck unbeweisbar.
3. KEIN Box-Code: kein Screen, kein Transfer, kein Handler, keine Strings (§4.2–4.5). Eine
   „Reaktivierung" ist unmöglich — es gibt nichts zu reaktivieren; jede funktionierende Box ist
   zwingend Neubau oder RE2-Port (so bereits `shots/itembox_verdict.md` VERDICT).

**(b) Box-Orte** → Tabelle §1.3 (AOT-Rects, Datei-Offsets, subs, Cuts, msg-Offsets) + Mockup-Frames §2.2.
Props/TIMs: keine — kein `Obj_model_set`-Box-Modell, keine Box-TIM; die Box war nur Hintergrund-Kunst
+ AOT. 6020/6021 = einziges Paar ohne Trigger (und ohne Save-Punkt).

**(c) Port-Empfehlung:**
1. **Anbindung ist bereits korrekt platziert:** Der Intercept in `scd_vm.c:1381`
   (`re15_itembox_is(g_current_room_id, index)` → `re15_itembox_set_pending(1)`) ersetzt genau die
   originale Preview-Message durch den Box-Screen — an exakt den 16 originalen Punkten
   (`re15_itembox.c:56-66 s_boxpoints[]` == meine Sweep-Tabelle, unabhängig verifiziert).
   `RE15_BOX_PREVIEW_MSG=1` behält das byte-true Auslieferungsverhalten. Daran nichts ändern.
2. **Lücke 6020/6021:** Da das Original dort keinen Trigger liefert, feuert auch der Port-Intercept nie
   — die Box ist in diesen Räumen unerreichbar. Optionen (Design-Entscheidung des Nutzers, kein
   byte-true-Fix): (i) einen sce-3-AOT-Analog im Port injizieren (Szene sub02 inkl. Cut 8 + Mockup-BG
   ist komplett vorhanden — es fehlt NUR der Auslöser; Vorlage: die 20-B-`Aot_set`-Records der
   Schwesterräume, Position müsste am BG/Türlayout gewählt werden — das Original definiert keine);
   (ii) byte-true belassen (Waise bleibt Waise). Empfehlung: (i) klar als Port-Ergänzung flaggen,
   analog zur Save-Nachrüstung.
3. **Mockup-Hintergrund entscheiden:** Die Box-Subs führen `Cut_chg` VOR `Message_on` aus — der Port
   schaltet also auf den Mockup-Cut, bevor der Intercept den Box-Screen öffnet. Ob das 2001er-Tool-
   Mockup dabei sichtbar wird (1-Frame-Blitz oder während der Fade-Out-Stage von
   `re15_menu_request_box`, `menu_common.c:1782-1788`), sollte einmal visuell geprüft werden
   (gdigrab-Skill). Falls ja: entweder den Cut_chg für Box-Punkte unterdrücken (sauberer Übergang)
   oder bewusst zeigen (historisches Artefakt) — Nutzer-Entscheidung.
4. **Als „Platzhalter entfernen" gibt es sonst nichts:** Die Preview-Message ist bereits default-mäßig
   ersetzt (Punkt 1); die Mockup-BSS-Frames und Stock-Kamera-Records sind Originaldaten und sollten
   unangetastet bleiben (sie stören nicht, solange der Box-Cut im Port nicht gezeigt wird).
5. **Storage-Mapping beibehalten:** Das 4×8-Layout des Ports spiegelt die einzigen originalen
   Storage-Kandidaten (@0x800b1444/1484/14a4/14c4) — für ein späteres byte-Mapping des
   Original-Save-Blobs die richtige Wahl (`re15_savedata.h:33-38` dokumentiert das bereits).

---

## 6. Offene Punkte (nicht blockierend)

1. Fußzeilen-Text des Mockups („© J…gy system .19xx") bei besserer Skalierung/CLUT lesbar machen —
   würde das interne Tool benennen (reine Neugier).
2. Visuelle Prüfung, ob die realen Raum-Hintergründe an den AOT-Positionen eine gezeichnete Kiste
   zeigen (z.B. 1150 cut1/cut6 decken den AOT-Bereich; auf cut1 nicht eindeutig erkennbar).
3. Der Verdikt-OQ „wer initialisiert Capacity-Byte 0x800b0fbc=0x0a" und der ungenullte Gap
   0x800b1464–1484 bleiben offen (siehe `shots/itembox_verdict.md`).
4. Port-Verhalten aus §5.3 (Mockup-Sichtbarkeit beim Box-Öffnen) dynamisch verifizieren.
