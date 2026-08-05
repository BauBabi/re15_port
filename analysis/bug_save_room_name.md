# BUG 4 — „Egal in welchem Raum ich speichere, es steht immer Irons Office"

Nutzer-Report (2026-08-05). Vorab-Befund des Auftrags: `platform/pc/main.c`
`pc_slot_title_codes` hat den Ortsnamen fest verdrahtet („room stubbed,
FUN_80026e4c returns 0").

**Status: gemessen + Original vollständig disassembliert.
Kern-Ergebnis: der Ortsname ist im ORIGINAL ebenfalls fest — `FUN_80026e4c`
ist ein `return 0`-STUB (byte-verifiziert auf Disc UND im Savestate-RAM).
Der Port ist beim NAMEN byte-true; er hat aber eine echte, messbare
Byte-Divergenz in der SPACING/Bytefolge derselben Zeile.
Engine-/Plattform-Code NICHT geändert** — nur die Diagnose-Probe
`re15_port/tests/unit/probe_save_room_name.c` + dieses Dossier.

---

## 1. MESSUNG — was Port und Original tatsächlich erzeugen

Probe: `re15_port/tests/unit/probe_save_room_name.c`
(Build-Target `probe_save_room_name`, kein ctest). Sie liest die ECHTE
`shared_assets/PSX/BIN/DEBUG.BIN`, baut die Slot-Zeile **byte-true nach
`FUN_80026658`** nach und vergleicht sie mit der 1:1 replizierten
Port-Funktion `pc_slot_title_codes` (main.c:521-539).

Lauf 2026-08-05:

```
[M1] sysmes-Ortsnamen (FUN_80026658 nutzt 0x1a + FUN_80026e4c()):
  idx 0x1a (= room_idx 0) @DEBUG.BIN 0x6197: "Irons' Office"
  idx 0x1b (= room_idx 1) @DEBUG.BIN 0x61a6: "Medical Room"
  idx 0x1c (= room_idx 2) @DEBUG.BIN 0x61b4: "West Staircase 1F"
  idx 0x1d (= room_idx 3) @DEBUG.BIN 0x61c7: "Sewer Maintainance"
  idx 0x1e (= room_idx 4) @DEBUG.BIN 0x61db: "Sewer Control Room"
  idx 0x1f (= room_idx 5) @DEBUG.BIN 0x61ef: "Factory - Office"
  idx 0x20 (= room_idx 6) @DEBUG.BIN 0x6201: "Security Room"
  idx 0x21 (= room_idx 7) @DEBUG.BIN 0x6210: "Monitor Room"

[M2] Slot-Zeile Leon, Zaehler 7:
  ORIGINAL bytes: 05 07 28 41 4b 4a 05 00 38 0c 13 38 25 4e 4b 4a 4f 3a 00 2b 42 42 45 3f 41 01 00 29
  ORIGINAL text :"Leon/07/Irons' Office"
  PORT     bytes: 05 07 28 41 4b 4a 05 00 00 38 0c 13 38 00 00 25 4e 4b 4a 4f 3a 00 2b 42 42 45 3f 41
  PORT     text :"Leon /07/  Irons' Office"
  FAIL: Bytefolge weicht ab

[M3] (Elza analog) ORIGINAL "Elza/12/Irons' Office" vs PORT "Elza /12/  Irons' Office"

[M4] Port-Ortsname vs sysmes 0x1a: IDENTISCH

[M5] Debug-Menue-Namen der 8 Save-Point-Raeume (DEBUG.BIN 0x2642, 26 B/Satz):
  ROOM1070 @0x26f8: "LOBBY OFFICE"   ROOM1120 @0x2816: "3F WEST SIDE"
  ROOM1150 @0x2864: "CHIEF OFFICE"   ROOM2010 @0x2b56: "BREAK ROOM"
  ROOM30A0 @0x313a: "NORTH OFFICE"   ROOM30B0 @0x3154: "SOUTH OFFICE"
  ROOM4010 @0x354a: "SECURITY ROOM"  ROOM5010 @0x3a44: "SECURITY ROOM"
```

**Damit ist gemessen:**
1. Der Ortsname *selbst* („Irons' Office") ist im Port **byte-identisch mit
   sysmes 0x1a** — und das Original wählt genau diesen Eintrag, immer (§2).
2. Der Port schiebt **drei zusätzliche Leerzeichen** (`0x00`) in die Zeile:
   vor dem ersten `/` und zwei nach dem zweiten `/`. Ergebnis: die Zeile ist
   3·8 = **24 px zu breit** und liest sich „Leon /07/  Irons' Office" statt
   „Leon/07/Irons' Office".

---

## 2. ORIGINAL — der Mechanismus, Instruktion für Instruktion

### 2.1 Der Slot-Zeilen-Bauer `FUN_80026658` (Memory-Card-Screen-Zeichner)

`RE_15_Quellcode_V2/FUN_80026658.c`, disasm-verifiziert:

```
800267cc: lw   v0,404(s7)              ; Kartenpuffer-Basis (DAT_801ff598)
800267d4: addu s0,s2,v0                ; s0 = Blockkopf Slot i (Basis+0x1430+i*0x80)
800267d8: lbu  v0,2(s0)                ; Blockkopf +2 = Flags
800267e0: andi v0,v0,0x3               ;   bit0..1 = Charakter
800267f0: ori  a0,zero,0x19            ;   != 0 -> sysmes 0x19 = "Elza /00/"
800267f4: ori  a0,zero,0x18            ;   == 0 -> sysmes 0x18 = "Leon /00/"
800267f8: jal  0x800c00e4              ; sysmes-Getter (DEBUG.BIN)
80026808: jal  0x8004ee38 (memcpy)     ; memcpy(DAT_80026aa4, sysmes, 0xc)
8002680c: ori  a2,zero,0xc
80026810: jal  0x80026e4c              ; <<<< ORTSNAMEN-INDEX, a0 = Blockkopf
80026814: addu a0,s0,zero
80026818: ori  a0,zero,0x1a            ; Basis-sysmes-Index der Ortsnamen
8002681c: jal  0x800c00e4
80026820: addu a0,a0,v0                ;   -> sysmes(0x1a + FUN_80026e4c())
8002682c: jal  0x8004ee38 (memcpy)     ; memcpy(DAT_80026ab0 = buf+0xc, sysmes, 0x10)
80026830: ori  a2,zero,0x10
80026834: lbu  t0,1(s0)                ; Blockkopf +1 = Save-Zähler
8002683c: divu t0,s5                   ; s5 = 10
80026840: lbu  v1,9(s4)                ; buf[9]  (Template-Ziffer '0' = 0x0c)
80026848: addu v1,v1,v0                ; buf[9]  += zaehler/10
8002684c: sb   v1,9(s4)
80026850: mfhi t0
80026854: lbu  v0,10(s4)
8002685c: addu t0,t0,v0                ; buf[10] += zaehler%10
80026864: sb   t0,10(s4)
; leerer Slot (else-Zweig):
80026868: jal 0x800c00e4 / ori a0,zero,0x10   ; sysmes 0x10 = "NO DATA"
80026878: jal 0x8004ee38 / ori a2,zero,0x12   ; memcpy 0x12
; danach: FUN_80028ec4(0x29, y, &DAT_80026aa4, 0), y = 0x38 + i*0x14
```

Der Zielpuffer `DAT_80026aa4` (PSX.EXE @0x80026aa4, Datei 0x172a4) ist im
Image mit `ff` gefüllt — reiner Scratch.

### 2.2 `FUN_80026e4c` — der gesuchte „Ortsnamen-Index"

**Er ist ein Stub.** PSX.EXE @`0x80026e4c`, Datei-Offset `0x1764c`:

```
80026e4c: 08 00 e0 03    jr   ra
80026e50: 21 10 00 00    addu v0,zero,zero      ; return 0
```

Das ist die ganze Funktion (zwei Instruktionen). Er ignoriert sein Argument
(den Blockkopf) vollständig. Ghidra-XREFs (`ghidra1_V2.txt:110986`): genau
zwei Aufrufer — `FUN_80026658:0x80026810` (Slot-Liste) und
`FUN_80026e54:0x80026efc` (BIOS-Kartentitel).

**Gegenprobe dynamisch** (kein Runtime-Patch): dieselben acht Bytes im
DuckStation-Savestate-RAM `stage_saves/doorA_square.sav` @0x80026e4c:
`08 00 e0 03 21 10 00 00`. Ebenso liegen die sysmes-Tabelle
(@0x800c5f96) und die SJIS-Tabelle (@0x80073628) im RAM byte-identisch
zur Disc.

⇒ **Das Original zeigt in der Slot-Liste IMMER `sysmes[0x1a]` = „Irons' Office"
und im BIOS-Kartentitel IMMER SJIS-Eintrag 0 = „館　小部屋　１Ｆ".**
Eine Zuordnung Raum→Name existiert im ausgelieferten Prototyp NICHT.

### 2.3 Der sysmes-Getter `FUN_800c00e4` (DEBUG.BIN, lädt RAW @0x800c0000)

```
800c00e4: lui  t0,0x800c
800c00e8: addiu t0,t0,24470        ; t0 = 0x800c5f96  (DEBUG.BIN-Datei 0x5f96)
800c00ec: sll  a0,a0,1
800c00f4: lhu  t1,0(t1)            ; u16-Offset-Tabelle
800c0100: addu v0,t0,t1            ; return 0x800c5f96 + u16[idx]
```

34 Einträge (idx 0x00..0x21); der erste String liegt bei 0x5f96+0x44 = 0x5fda.

### 2.4 Der BIOS-Kartentitel `FUN_80026e54` (der zweite Nutzer des Stubs)

```
80026e58: lbu  v0, DAT_800b0fbe     ; Game-State-Block +2, bit0 = Charakter
80026e80: addiu a1,a1,1996          ; -> DAT_800107cc  (bit0=1)
80026e94: addiu a1,a1,2040          ; -> DAT_800107f8  (bit0=0)
80026e98: jal  0x8004ee38 / ori a2,zero,0x2a     ; memcpy(dst, template, 0x2a)
80026eac: lbu  v0, DAT_800b0fbd     ; Save-Zähler
80026ec8: sb   v1,37(s0)            ; dst[0x25] += zaehler/10   (SJIS-Low-Byte '０')
80026ef0/f8: sb  ...,39(s0)         ; dst[0x27] += zaehler%10
80026efc: jal  0x80026e4c           ; <<<< derselbe Stub
80026f28: jal  0x8004ee38           ; memcpy(dst+0x2a, 0x80073628 + idx*0x13, 0x13)
```

Aufgerufen aus dem SAVE-Zweig von `FUN_80025c00` @`0x800261f0`, direkt nach
`memcpy(karte+slot*0x80+0x1430, &DAT_800b0fbc, 0x1230)` (@0x800261c4-d8).

---

## 3. DIE NAMENS-TABELLEN — vollständig, mit Byte-Offsets

### 3.1 In-Game-Slot-Liste: DEBUG.BIN-sysmes (Atlas-Codes), Index `0x1a + n`

Adressierung: `ptr = 0x800c5f96 + u16[0x800c5f96 + idx*2]`.
DEBUG.BIN lädt RAW @`0x800c0000` (KEIN 0x800-Header) → Datei-Offset = Adresse − 0x800c0000.

| n | sysmes-idx | Adresse | DEBUG.BIN-Offset | Länge | Text | Bytes |
|--|--|--|--|--|--|--|
| 0 | 0x1a | 0x800c6197 | 0x6197 | 15 | `Irons' Office` | `25 4e 4b 4a 4f 3a 00 2b 42 42 45 3f 41 01 00` |
| 1 | 0x1b | 0x800c61a6 | 0x61a6 | 14 | `Medical Room` | `29 41 40 45 3f 3d 48 00 2e 4b 4b 49 01 00` |
| 2 | 0x1c | 0x800c61b4 | 0x61b4 | 19 | `West Staircase 1F` | `33 41 4f 50 00 2f 50 3d 45 4e 3f 3d 4f 41 00 0d 22 01 00` |
| 3 | 0x1d | 0x800c61c7 | 0x61c7 | 20 | `Sewer Maintainance` (sic) | `2f 41 53 41 4e 00 29 3d 45 4a 50 3d 45 4a 3d 4a 3f 41 01 00` |
| 4 | 0x1e | 0x800c61db | 0x61db | 20 | `Sewer Control Room` | `2f 41 53 41 4e 00 1f 4b 4a 50 4e 4b 48 00 2e 4b 4b 49 01 00` |
| 5 | 0x1f | 0x800c61ef | 0x61ef | 18 | `Factory - Office` | `22 3d 3f 50 4b 4e 55 00 3b 00 2b 42 42 45 3f 41 01 00` |
| 6 | 0x20 | 0x800c6201 | 0x6201 | 15 | `Security Room` | `2f 41 3f 51 4e 45 50 55 00 2e 4b 4b 49 01 00` |
| 7 | 0x21 | 0x800c6210 | 0x6210 | 14 | `Monitor Room` | `29 4b 4a 45 50 4b 4e 00 2e 4b 4b 49 01 00` |

Atlas: `0x00`=Space, `0x01`=Terminator, `0x05 xx`=Farb-Op (`attr = xx & 7`),
`0x0c..0x15`='0'..'9', `0x1d..0x36`='A'..'Z', `0x38`='/', `0x3a`='\'',
`0x3b`='-', `0x3d..0x56`='a'..'z', `0x57`='.' (Port-Map `render_pc.c:2396-2409`).

Die zugehörigen Namens-Templates:

| sysmes-idx | Offset | Bytes (die ersten 0xc werden kopiert) | Text |
|--|--|--|--|
| 0x18 (Leon) | 0x617d | `05 07 28 41 4b 4a 05 00 38 0c 0c 38` (+`07`) | `Leon/00/` |
| 0x19 (Elza) | 0x618a | `05 06 21 48 56 3d 05 00 38 0c 0c 38` (+`07`) | `Elza/00/` |
| 0x10 (leer) | 0x60ed | 0x12 Bytes | `NO DATA` |

Farb-Attribute: Leon = 7, Elza = 6, danach `05 00` = Default.
**Kein Leerzeichen zwischen Name und `/`, keins zwischen `/` und Ortsnamen.**

**Nebenbefund (Beleg dafür, dass das Feature nie fertig wurde):** der
Ortsnamen-`memcpy` ist nur `0x10` Bytes lang (@0x80026830). Die Einträge
n=2..5 sind 18-20 Bytes lang — sie würden **ohne Terminator abgeschnitten**
(„West Staircase 1" …). Nur n=0,1,6,7 passen.

### 3.2 BIOS-Kartentitel: SJIS-Tabelle PSX.EXE @0x80073628, Stride 0x13

Datei-Offset = `0x800 + Adresse − 0x80010000` (t_addr aus PS-X-Header @0x18 = 0x80010000).

| n | Adresse | PSX.EXE-Offset | Text (Shift-JIS) |
|--|--|--|--|
| 0 | 0x80073628 | 0x63e28 | `館　小部屋　１Ｆ」` |
| 1 | 0x8007363b | 0x63e3b | `館　ホール　１Ｆ」` |
| 2 | 0x8007364e | 0x63e4e | `中庭　部屋　Ｂ１」` |
| 3 | 0x80073661 | 0x63e61 | `　寄宿舎　　１Ｆ」` |
| 4 | 0x80073674 | 0x63e74 | `　研究所　　Ｂ３」` |
| 5 | 0x80073687 | 0x63e87 | `館　　物置　１Ｆ」` |
| 6 | 0x8007369a | 0x63e9a | `中庭　通路　Ｂ１」` |
| — | 0x800736ad | 0x63ead | Tabellenende (`00 00 00` + Format-String `"bu00:"`) |

7 Einträge. Titel-Templates (je 0x2a Bytes, SJIS):

| Symbol | Adresse | PSX.EXE-Offset | Text |
|--|--|--|--|
| DAT_800107cc (`DAT_800b0fbe & 1` ≠ 0) | 0x800107cc | 0x00fcc | `ＢＩＯ　ＨＡＺＡＲＤ　「Ｅｌｚａ　／００／` |
| DAT_800107f8 (`… & 1` == 0) | 0x800107f8 | 0x00ff8 | `ＢＩＯ　ＨＡＺＡＲＤ　「Ｌｅｏｎ　／００／` |

Ziffern: `dst[0x25] += n/10`, `dst[0x27] += n%10` (SJIS-Low-Bytes der
Vollbreiten-Nullen `82 4f`). Der Ortsname wird an `dst+0x2a` angehängt und
schließt das öffnende `「` mit dem `」` am Eintragsende.

**Einordnung (wahrscheinlich, nicht byte-belegbar):** die 7 SJIS-Namen
(Herrenhaus/Halle/Innenhof/Wohnheim/Labor B3/Abstellraum/Innenhof-Gang) sind
die Ortsnamen des **Vorgängers Resident Evil 1** (Spencer-Anwesen,
Guardhouse, Labor B3) — Legacy-Daten in der von RE1 abgeleiteten Engine.
Die RE1.5-Ortsnamen sind die englische DEBUG.BIN-Liste aus §3.1.

### 3.3 Raum→Name gibt es NICHT — was es stattdessen gibt

Der einzige vollständige, im Original vorhandene Raum→Name-Datensatz ist die
**Debug-Menü-JUMP-Tabelle** in DEBUG.BIN: Basis `0x800c2642` (Datei `0x2642`),
Satz = 26 Bytes (14 Byte ASCII-Name + 12 Byte Nutzlast), Stage-Stride
637 Halbwörter = 1274 Bytes, Satz-Stride 13 Halbwörter.
Raum-ID = `(stage+1)<<8 | idx` (Port-`ROOM<id>0`).
Sie speist bereits `re15_port/include/debug_jump_table.h`
(Generator `re15_port/tools/gen_debug_jump_table.py`).

Vollständig (Nicht-leere Sätze), STAGE1 zuerst:

**Stage 0 (ROOM1xx0)** — 0x2642 + idx*26:
`00 BATH-LOCKERS` · `01 INTERROG. ROOM` · `02 MAIN OFFICE` · `03 LOBBY` ·
`04 WEST CORRIDOR` · `05 EAST CORRIDOR` · `06 UPPER STAIRS` ·
**`07 LOBBY OFFICE`** · `08 ELEVATOR` · `09 REAR EXTERIOR` · `0a LOWER STAIRS` ·
`0b FRONT EXTERIOR` · `0c 2F WEST SIDE` · `0d 2F CORRIDOR` · `0e MED. EXAM ROOM` ·
`0f COMMUNIC. ROOM` · `10 EVID. CORRIDOR` · `11 EVIDENCE ROOM` ·
**`12 3F WEST SIDE`** · `13 3F CORRIDOR` · `14 BRIEFING ROOM` ·
**`15 CHIEF OFFICE`** · `16 KENNEL` · `17 HELIPORT` · `18 B1 CORRIDOR` ·
`19 FIRING-ARMORY` · `1a SEWER EXIT` · `1b GARAGE` · `1c PARKING LOT` ·
`1d KENNEL LIGHT` · `1e B2 CORRIDOR` · `1f BOILER ROOM` · `20 MORGUE` ·
`21 DETENTI. AREA` · `22 DETENTI. CELLS` · `23 B1 CORR. LIGHT` · `24 OPENING` ·
`25 LOBBY ERA-1` · `26 SEWER PASSAGE` · `27 EMPTY`

**Stage 1 (ROOM2xx0)** — 0x2b3c + idx*26:
`00 L TUNNEL` · **`01 BREAK ROOM`** · `02 ZOMBIE TUNNEL` · `03 TREATMENT POOL` ·
`04 SEALED TUNNEL` · `05 SEPTIC POOL` · `06 ENGINE ROOM` · `07 TUN. FLOOD DRY` ·
`08 CONTROL ROOM` · `09 ALLIGATOR POOL` · `0a SERVICE TUNNEL` · `0b L TUNNEL FLOOD` ·
`0c-0f EMPTY`

**Stage 2 (ROOM3xx0)** — 0x3036 + idx*26:
`00 FACT. ENTRANCE` · `01 DOWNLOAD AREA` · `02 PARKING AREA` · `03 FACT. CORRIDOR` ·
`04 FACTORY LINE A` · `05 FACTORY LINE B` · `06 FACT. BACKYARD` · `07 WAREHOUSE` ·
`08 WAREHOUSE LIFT` · `09 T CORRIDOR` · **`0a NORTH OFFICE`** · **`0b SOUTH OFFICE`** ·
`0c LOBBY` · `0d LOB.PAR-MARVIN` · `0e FORKLIFT-FAC.L` · `0f EMPTY`

**Stage 3 (ROOM4xx0)** — 0x3530 + idx*26:
`00 LAB ENTRANCE` · **`01 SECURITY ROOM`** · `02 A-2 ELEVATOR` · `03 MAIN SHAFT` ·
`04 EAST PASSAGE` · `05 PRIVATE ROOMS` · `06 EMPTY` · `07 EMERGE. STAIRS` ·
`08 WEST PASSAGE` · `09 FREEZING ROOM` · `0a CT SCAN` · `0b CELL CULTIVE` ·
`0c-0f EMPTY`

**Stage 4 (ROOM5xx0)** — 0x3a2a + idx*26:
`00 A-2 ENTRANCE` · **`01 SECURITY ROOM`** · `02 CARGO STORAGE` · `03 MONITOR ROOM` ·
`04 B-2 CORRIDOR` · `05 SERVER ROOM` · `06 P-4 LABORATORY` · `07 D-2 CORRIDOR` ·
`08 FUEL CONTROL` · `09 TRAIN CAR ABC` · `0a C-2 CORRIDOR` · `0b C-2 HANGAR` ·
`0c A-3 CORRIDOR` · `0d STEAM ROOM` · `0e B3 POWER ROOM` · `0f B4 POWER ROOM` ·
`10 B-3 CORRIDOR` · `11 F MONITOR ROOM` · `12 F B-2 CORRIDOR` · `13 F D-2 CORRIDOR` ·
`14 F C-2 CORRIDOR` · `15-17 EMPTY`

**Stage 5 (ROOM6xx0)** — 0x3f24 + idx*26:
`00 GATE PLATFORM` · `01 MAIN PLATFORM` · `02 SHELTER 01-02` · `03 TRAIN MAIN CAR` ·
`04 ENDING` · `05-07 EMPTY`

(**Fett** = Save-Point-Raum laut `re15_port/engine/src/re15_savepoint.c`.)

### 3.4 Warum die Zuordnung nicht rekonstruierbar ist — und was dagegen spricht zu raten

- Der Resolver ist ein Stub; es gibt keinen zweiten Aufrufer und keine
  Datentabelle, die 8 Raum-IDs auf 0..7 abbildet. Ein Binär-Scan über
  `PSX.EXE` + alle `BIN/*.BIN` nach den Save-Raum-IDs (`0x0107 0x0112 0x0115
  0x0201 0x030a 0x030b 0x0401 0x0501` bzw. `0x1070 …`) als u16-Sequenz
  liefert **null Treffer** (auch nicht für die ersten drei).
- Der Blockkopf, den der Stub bekommt, wird nur für +1 (Zähler) und +2
  (Charakter) gelesen; ein Raum-Feld darin ist nirgends belegt.
- Die 8 sysmes-Namen passen NICHT sauber auf die 8 Save-Point-Räume:
  `Irons' Office`↔`CHIEF OFFICE`(ROOM1150) und `Security Room`↔ROOM4010/5010
  sind plausibel, aber `Medical Room`, `West Staircase 1F`, `Sewer Control
  Room` und `Monitor Room` entsprechen Räumen (`MED. EXAM ROOM` 0x10e0,
  `CONTROL ROOM` 0x2080, `MONITOR ROOM` 0x5030), die **keine** Save-Points
  sind. Die Liste stammt sichtbar aus einem anderen Build-Stand.
- Zusammen mit dem zu kurzen `0x10`-memcpy (§3.1) ⇒ Feature nie verdrahtet.

**OFFEN, ehrlich markiert:** eine byte-belegte Zuordnung Raum→Ortsname
existiert in RE1.5 nicht. Jede Zuordnung wäre erfunden.

---

## 4. FINDINGS + FIX-PLAN

### F1 (belegt) — „immer Irons' Office" ist ORIGINAL-Verhalten, kein Port-Bug
`FUN_80026e4c` @0x80026e4c = `jr ra / addu v0,zero,zero` (Datei 0x1764c,
savestate-bestätigt). Beide Aufrufer (0x80026810, 0x80026efc) bekommen 0.
**Fix: keiner.** Der hartkodierte Name in `main.c:525` ist byte-true
(Probe M4: identisch mit sysmes 0x1a). Der Kommentar dort („room stubbed,
FUN_80026e4c returns 0") stimmt — er sollte nur den Beleg tragen
(`@0x80026e4c: 08 00 e0 03 / 21 10 00 00`, DEBUG.BIN 0x6197).

### F2 (belegt) — echte Divergenz: 3 überzählige Leerzeichen in der Slot-Zeile
`pc_slot_title_codes` (main.c:530-537) fügt `0x00` an drei Stellen ein, die
das Original nicht hat (Probe M2/M3). Der Grund ist eine Fehl-Lesung des
Templates: `05 00` ist der **Farb-Op mit Argument 0** — der Port hat das
`00` als Trenner interpretiert und danach nochmals ein eigenes Space
eingefügt, plus zwei „gap"-Spaces vor dem Ortsnamen.

Byte-true wäre exakt:
```
buf[0..11]  = sysmes(char ? 0x19 : 0x18)[0..11]      (DEBUG.BIN 0x617d / 0x618a)
buf[9]     += count / 10                              (@0x80026840-4c)
buf[10]    += count % 10                              (@0x80026850-64)
buf[12..27] = sysmes(0x1a + 0)[0..15]                 (DEBUG.BIN 0x6197, memcpy 0x10 @0x80026830)
```
d.h. die Codefolge `05 07 28 41 4b 4a 05 00 38 dd dd 38 25 4e 4b 4a 4f 3a 00
2b 42 42 45 3f 41 01 00 29` (Leon) bzw. `05 06 21 48 56 3d …` (Elza).

⚠️ **Muss zusammen mit dem Zeichner gefixt werden:**
`re15_render_pc_game_codes` (render_pc.c:2432-2445) kennt nur `0x05` und
`0x00`; ein `0x01` würde es als GLYPHE zeichnen. Das Original bricht bei
`0x01` ab (Terminator, vgl. jede sysmes-Zeile). Also entweder
(a) im Zeichner `if (code == 0x01) break;` ergänzen — dann kann man die
28 Bytes 1:1 emittieren (empfohlen, deckt auch künftige sysmes-Nutzung ab),
oder (b) nur die 25 Bytes bis vor `01` emittieren.

Am saubersten: die Zeile nicht mehr aus hartkodierten Arrays bauen, sondern
per sysmes-Getter aus der residenten DEBUG.BIN lesen
(`ptr = 0x5f96 + u16[0x5f96 + idx*2]`, §2.3) — dann sind Name, Farben,
Ziffernbasis und Terminator automatisch byte-true.

### F3 (belegt) — BIOS-Kartentitel ohne SJIS/Ortsnamen
`pc_do_save` (main.c:505-513) schreibt ASCII `"BIO HAZARD Leon /07/"`.
Original `FUN_80026e54`: SJIS-Template 0x2a Bytes (DAT_800107f8/cc, Datei
0x00ff8/0x00fcc) + SJIS-Ortsname `0x80073628 + 0*0x13` (Datei 0x63e28),
also `ＢＩＯ　ＨＡＺＡＲＤ　「Ｌｅｏｎ　／０７／館　小部屋　１Ｆ」` (0x3d Bytes,
passt in die 64-Byte-Titelzeile des Kartenblocks). Sichtbar nur im
PSX-BIOS-Kartenmanager / externen Save-Tools.
**Fix (optional, klar abgegrenzt):** in `pc_do_save` die 0x2a-SJIS-Vorlage
+ Ziffern-Patch bei [0x25]/[0x27] + 0x13-Byte-SJIS-Anhang nachbauen.
`RE15_MC_TITLE_LEN` (32) müsste dafür auf ≥ 0x3d wachsen bzw. der Titel als
Bytes statt als C-String durchgereicht werden.

### F4 (belegt) — Zähler ≥ 100 wird im Port anders dargestellt
Original: `buf[9] += cnt/10` **ohne** mod-10 (@0x80026840-4c) und der Zähler
wird ungeklammert inkrementiert (`lbu; addiu +1; sb` @0x80026488-9c, kein
Clamp) — bei cnt=100 zeigt das Original `0x0c+10 = 0x16` = `:`.
Port: `(count/10)%10` (main.c:533) plus `save_count % 100` (main.c:511/600).
Kosmetisch, aber eine echte Byte-Divergenz. **Fix:** `count/10` ohne `%10`
und den Wert ungeklammert durchreichen (nur wenn F2 sowieso angefasst wird).

---

## 5. Regressions-Absicherung

- `re15_port/tests/unit/probe_save_room_name.c` (neu, Target
  `probe_save_room_name`) vergleicht die Port-Bytefolge gegen die aus der
  echten DEBUG.BIN rekonstruierte Original-Bytefolge. Nach dem F2-Fix muss
  sie „Port == Original" melden — dann kann sie als `add_test(NAME
  unit_save_slot_title …)` scharf geschaltet werden.
- Optisch: `RE15_CARD_AUTO=1 RE15_CARD_SHOT=<png>` (main.c:614-615, 669-670)
  rendert die Kartenmaske headless — Zeilenbreite vorher/nachher vergleichen
  (die Zeile wird 24 px schmaler).
- Betroffene bestehende Tests: keine. `pc_slot_title_codes` /
  `re15_render_pc_game_codes` sind ausschließlich von der Kartenmaske
  benutzt (grep: main.c:555, 646). `probe_save_counter` und
  `probe_save_injured` fassen den Titel nicht an.
- Falls `re15_render_pc_game_codes` um den `0x01`-Terminator erweitert wird:
  der einzige weitere Aufrufer ist derselbe Slot-Titel — kein Fremdrisiko.

## 6. Quellen

- `info/Re1.5/PSX.EXE` — `FUN_80026e4c` @0x80026e4c (Datei 0x1764c),
  `FUN_80026658` @0x800267c8-0x80026880, `FUN_80026e54` @0x80026e54-0x80026f40,
  `FUN_80025c00` @0x800261b4-0x80026228 (SAVE), SJIS-Tabelle @0x80073628
  (Datei 0x63e28), Templates @0x800107cc/f8 (Datei 0x00fcc/0x00ff8)
- `info/Re1.5/PSX/BIN/DEBUG.BIN` (RAW @0x800c0000) — `FUN_800c00e4` @0x800c00e4,
  sysmes-Offsettabelle 0x5f96, Strings 0x5fda-0x621f, Debug-Raumnamen 0x2642
- `RE_15_Quellcode_V2/FUN_80026658.c`, `FUN_80026e4c.c`, `FUN_80026e54.c`
- `ghidra1_V2.txt:110986` (XREFs), `:492891` (DAT_800b0fbc/bd/be)
- `stage_saves/doorA_square.sav` — RAM-Gegenprobe 0x80026e4c / 0x800c5f96 / 0x80073628
- Port: `platform/pc/main.c:505-539, 555, 646`,
  `platform/pc/src/render_pc.c:2396-2445`,
  `engine/src/re15_savepoint.c`, `include/debug_jump_table.h`

---

## STATUS 2026-08-06 — KEIN PORT-FEHLER (selbst nachverifiziert)

Die Kernaussage F1 habe ich unabhaengig reproduziert, bevor irgendetwas geaendert wurde:

```
80026e4c: jr ra
80026e50: addu v0,zero,zero        ; Datei-Offset 0x1764c: 08 00 e0 03 21 10 00 00
```

Eigener Opcode-Scan ueber das gesamte .text der PSX.EXE: **genau zwei** Aufrufer
(0x80026810 = die Slot-Liste, 0x80026efc = der BIOS-Kartentitel) und **null** 32-Bit-Datenzeiger
auf die Adresse — kein Tabellen-/vtable-Weg. Der Resolver liefert also immer 0, beide Aufrufer
addieren das auf ihre Basis, und das ORIGINAL zeigt damit selbst in jedem Raum sysmes 0x1a =
"Irons' Office". Der hartkodierte Name im Port ist byte-identisch dazu.

**Geaendert wurde nur der Kommentar** in `platform/pc/main.c` (`pc_slot_title_codes`): er traegt
jetzt die Bytes, die Datei-Offsets, den Aufrufer-Zensus und den Hinweis, dass eine
Raum->Ortsnamen-Zuordnung im Binary nicht existiert (Byte-Scan ueber PSX.EXE + alle BIN/*.BIN
nach den 8 Save-Raum-IDs: null Treffer). Eine solche Zuordnung zu erfinden waere ein
Rate-Defekt und faellt unter das STOP-GATE.

### Nicht eingebaut: F2/F3/F4

F2 (drei ueberzaehlige Space-Bytes + 8-px- statt 4-px-Vorschub fuer Code 0x00, zusammen ~28 px
zu breit), F3 (ASCII- statt SJIS-Kartentitel) und F4 (Zaehler >= 100) sind **echte, belegte
Divergenzen**, aber keine davon ist der gemeldete Fehler. F2 verlangt zusaetzlich einen Eingriff
in `re15_render_pc_game_codes` (Breitentabelle @0x800c4416, Terminator auch auf 0x07) — der
Pruefer hat gezeigt, dass der Fix in der Dossier-Form 4 px Rest laesst. Das gehoert in einen
eigenen, separat verifizierten Durchgang, nicht als Beifang in einen Bugfix.
