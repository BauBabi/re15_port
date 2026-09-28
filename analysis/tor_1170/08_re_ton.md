# 08 — Der Ton der Türsequenz DOOR2E (RE2) und wie der Port ihn spielt

Vorbild: RE2-Retail `info/re2leon/COMMON/DOOR/DOOR2E.DO2` (75772 B, sha1 `3461160c…`), Tür 0x2E.
Alle Adressen sind RE2 `info/re2leon/PSX.EXE` (t_addr 0x80010000), selbst disassembliert mit
`re2_disasm.py`. Werkzeug: `build/tor_1170/re_ton/ton_door2e.py` (Bericht, WAVs, JSON), `huelle.py` (Hüllkurve).
Vorarbeit: 03 §3.5/§3.6, 02 §2, 04 (DOOR2E-Skripte).

---

## 0. Ergebnis

1. Die Türsequenz hat **zwei Töne**, beide aus der Tür-Tonbank (Bank 0), beide **nicht positional**:
   - **Ton 0** (Kopfeintrag 0, `00 00 14 16`): vom Skript per `Se_on` in **Bild 100** der Türszene
     (Skript 1 und 2 identisch). VAG 2, 8304 B, 11025 Hz, **1,316 s**. Zwei Anschläge (0–0,2 s und 0,5–1,0 s).
   - **Ton 1** (Kopfeintrag 1, `00 00 24 17`): von Door_exit, sobald die Ausblende fertig ist. VAG 3,
     7696 B, 11025 Hz, **1,219 s**. Ein Anschlag (0–0,5 s), klingt bis 1,2 s aus.
2. Lage und Kamera spielen keine Rolle: die Lageberechnung `FUN_8005c970` läuft nur, wenn das
   Lagebyte ≠ 0 ist; bei beiden Tönen ist es 0. Pegel = Tonpegel, Panorama = Mitte.
   RE2-Stimmpegel L = R = **10157** (Ton 0) bzw. **11198** (Ton 1) von 0x3FFF.
3. Der Tonteil (`DOOR2E.DO2[0x0000..0x4AE8)`, 19176 B) hat **dasselbe Satzformat** wie die Port-Mini-Bank
   `TUERSE.VBS`, nur mit VH-Versatz 0x10 statt 0x20. Er ist **unverändert** als Mini-Bank ladbar
   (TOC 0/3128/3128/16048). Die 4-B-Einträge bedeuten dasselbe (dieselbe RE2-Funktion liest beide).
4. Der Port spielt beide Töne über den vorhandenen `se_play_layers` auf den richtigen Stimmen (SPU 22/23),
   mit der richtigen Tonhöhe (0x400) — aber **2,08 dB bzw. 1,65 dB zu laut** (lineares statt RE2-Pegelgesetz)
   und der Schließton würde vom Port-Raumwechsel **abgeschnitten** (`re15_audio_load_room_banks` stoppt alle
   Stimmen; RE2 verschont die Türbank ausdrücklich). Rezept in §5.

---

## 1. `Se_on` (0x36, 12 B), Handler `0x80056428`

Tabelle `0x800a74c8[0x36]` = `0x80056428` (`re2_disasm.py table 0x800a74c8 0x90`, Zeile [54]).

```
8005643c lw  s0,28(s1)      ; pc
80056444 lh  a1,4(s0)       ; s16@4
80056448 lbu a3,1(s0)       ; pc[1]
8005644c lh  a0,2(s0)       ; s16@2
80056450 andi a2,a1,0xff    ; Bezug = Low-Byte von s16@4
80056454 sltiu v0,a2,0x5 / 80056458 beq -> 0x800564e0   ; Sprungtabelle 0x80011458 (5 Einträge)
  [0] 80056478: a1=a2=v1=0, j 0x800564ec               ; Nullpunkt
  [1] 80056488: v1 = [0x800cfe14]                       ; Objekt
  [2] 80056498: v1 = [0x800cfe18]
  [3] 800564a8: v1 = [0x800cfe1c + (s16@4 >> 8)*4]
  [4] 800564c4: v1 = 0x800d0324 + (s16@4 >> 8)*0x1F8
800564e0 lw a1,56(v1) / lw a2,60(v1) / lw v1,64(v1)     ; Objekt +0x38/+0x3C/+0x40
800564ec..8005651c  sp+16/20/24 = s16@6 + a1, s16@8 + a2, s16@10 + v1
80056518 sll v1,a3,24 ; 80056520 andi v0,a0,0xff ; 80056524 sll v0,v0,16 ; 80056528 or v1,v1,v0
8005652c srl a0,a0,8  ; 80056530 jal 0x8005ba28 ; 80056534 or a0,v1,a0 (Delay-Slot) ; 80056510 addiu a1,sp,16
8005653c addiu v1,s0,12 ; 80056540 sw v1,28(s1)                                  ; PC += 12
```

| Feld | Bedeutung | DOOR2E (`36 00 00 00 01 00 00 00 00 00 00 00` @Datei 0x0507A und 0x050F0) |
|---|---|---|
| pc[1] | **Bank** (Index in `0x800d4c48`/`0x800dbb78`/`0x800d75a0`) → a0 Bit 24..31 | 0 = Tür-Bank |
| pc[2] (s16@2 & 0xFF) | **Satz** = Kopfeintrag → a0 Bit 16..23 | 0 |
| pc[3] (s16@2 >> 8, logisch) | **Lagebyte** → a0 Bit 0..7, in `0x8005ba28` Satz +18 | 0 → nicht positional |
| s16@4 Low-Byte | **Bezug** der Lage (0 Nullpunkt, 1 `[0x800cfe14]`, 2 `[0x800cfe18]`, 3/4 indiziert über das High-Byte) | 1 |
| s16@6/8/10 | **Versatz** zur Bezugslage | (0,0,0) |

`0x8005ba28(a0, a1)`: a0 = `pc[1]<<24 | pc[2]<<16 | pc[3]`, a1 = Zeiger auf (x,y,z) s32 auf dem Stapel.
Für DOOR2E: **a0 = 0x00000000**, a1 = Lage von Objekt `[0x800cfe14]`. Door_exit ruft dieselbe Funktion
direkt: `@0x800141f4 lui a0,0x1` → **a0 = 0x00010000** (Bank 0, Satz 1, Lagebyte 0),
`@0x800141e8/ec` a1 = `0x800cfc30`, nur wenn Arbeitsbereich+0x248 ≠ 0 (`@0x800141d8 lhu v0,584(v0)`,
`@0x800141e0 beq`). Den Merker setzt `Door_model_set` bei Flag 0x800 (`@0x80014c00 lhu v1,6(a1)`,
`@0x80014c90 andi v0,v1,0x800`, `@0x80014ca8 sh v0,584(v1)` mit v0 = 1); DOOR2E Objekt 0 trägt 0x0A80
in beiden Aufbauskripten → Ton 1 spielt bei beiden Türseiten.

Korrektur zu 03 §3.5: „`s16@4` = Positionsquelle (0 = Ursprung)“ stimmt für den Wert 0; DOOR2E benutzt
Bezug 1. Die Lage wird aber nie ausgewertet (§2).

---

## 2. Wie ein Ton gespielt wird

### 2.1 `0x8005ba28` — Satz vormerken

```
8005ba30 srl t1,a0,24            ; Bank
8005ba64 lb  v0,0x800d4c48[t1]   ; VAB-Nummer der Bank; -1 -> Ende (8005ba6c)
8005ba7c srl v0,a0,16 / 8005ba80 andi s7,v0,0xff   ; Satz
8005ba8c lw  a1,0x800dbb78[t1] / 8005ba94 addu a1,a1,s7*4   ; Kopfeintrag
8005ba98 lw  v0,0(a1) / 8005baa0 beq v0,-1 -> Ende           ; ff ff ff ff = leer
8005bab0 andi v0,v1,0x80 / 8005babc andi s5,v1,0x7f          ; b0 Bit 7: VAB-Ersatz
8005bac8 lw  a2,0x800d75a0[t1]                               ; VH der Bank
8005bad8 andi s1,b3,0x1f   ; Stimme        8005badc srl s2,b2,4 ; Ton
8005bae0 srl  s4,b3,5      ; Folgelagen    8005bae4 andi s6,b1,0x7f ; Programm
8005bb04 andi fp,b2,0xf    ; Prio-Nibble
8005baf8 s0 = VH + prog*0x200 + ton*0x20 + 0x820             ; Tonsatz
8005bb94 jal 0x8005c92c (Stimme, Nibble) ; ≠0 -> verworfen   ; Prio-Tor
8005bbc4 sb fp&7 -> 0x800d4ca0[s1*2] ; 8005bbd4 sb s1 -> 0x800d4ca1[s1*2]
Satz 0x800d4f18 + s1*32:  +0=1 (vorgemerkt) +1=Satznr +4=VAB +6=prog +8=ton
  +10 = tone[6] (8005bbec)  +12 = tone[5] (8005bbf8)  +14 = +16 = tone[2] (8005bc04..18)
  +18 = a0 & 0xFF (Lagebyte, 8005bc40)  +20/24/28 = Lage (8005bc1c..48)
8005bc44 beq s4,zero -> Ende      ; keine Folgelagen
```

Die Tür-Bank wird von `FUN_80014cd0` eingerichtet: Kopf+VH nach `0x801fb700` kopiert (`@0x80014de0..e74`),
`@0x80014eb4 sw v1,0x800dbb78` (Kopf), `@0x80014ec4 sw v0,0x800d75a0` (VH = Kopf + Nachspann-u32
`@0x80014ea4`), `@0x80014f14 jal SsVabOpenHeadSticky` mit a1 = 0 (`@0x80014ef4`/`@0x80014f78`) und
a2 = 0x3DC50 (`@0x80014f08/18`), Ergebnis `@0x80014f20 sb v0,0x800d4c48`. Mit a1 = 0 nimmt
`SsVabOpenHeadWithMode` Nummer 0, sofern frei (Dekompilat `RE2_Quellcode_V2/SsVabOpenHeadWithMode.c:40-47`);
die Vorgängertür wird vorher geschlossen (`@0x80014ec8..dc`). VAB-Nummer 0 ≠ 2 und ≠ 4 → in
`0x8005ba28` nur Hall-Bit aus (`@0x8005bb50..84`, `andi v0,v0,0xfb`), in der Pumpe Normalpfad.

Prio-Tor `0x8005c92c`: neu & 7 < laufend → verwerfen; ungleich → spielen; gleich → verwerfen, wenn
Nibble-Bit 3 (`@0x8005c940..964`).

### 2.2 Pumpe `0x8005c5e4` — einmal je Bild, nach VSync

Aufruf `@0x8002b9a0 jal 0x8005c5e4` direkt nach `@0x8002b998 jal VSync(0x800dfc1a)`. Schleife über die
24 Sätze, rückwärts:

```
8005c658 lh a1,6(s0)        ; Satz +18 = Lagebyte
8005c660 beq a1,zero -> 8005c6cc
8005c668 jal 0x8005c970(Satz+20, Lagebyte)      ; NUR wenn Lagebyte != 0
8005c6cc [0x800d7598] = Satz+14 ; [0x800d759a] = Satz+16   ; voll = volr = tone[2]
8005c6e4 lbu v1,[0x800e8768] ; 0 -> kein Anschlag
8005c700..44 voll,volr = voll*v1/100, volr*v1/100   ; 0x51eb851f, sra 5
8005c758 sltiu v0,Stimme,0x10 ; Stimme >= 16 -> 8005c798
8005c7a0 VAB != 2 -> 8005c87c
8005c8b0 jal SsUtKeyOnV(Stimme, VAB, prog, ton, note=+10, fine=+12, voll, volr) ; 8005c8b8 Satz+0 = 0
```

`[0x800e8768]` = 100 ab Start (`@0x8002b608 addiu v0,zero,100`, `@0x8002b618 sb`); Opcode 0x80
(`0x800a74c8[0x80]` = `0x800580cc`) schaltet es zwischen „mal pc[1]/100“ und dem gesicherten Wert
(`@0x800580e0..0x80058140`). In den 9 DOOR2E-Skripten kommt 0x80 nicht vor (`tuerskript_dump.py dis`).

### 2.3 `SsUtKeyOnV 0x800802b8` und `_SsVmKeyOnNow 0x80083760` — der Stimmpegel

- `@0x80080330 sltiu v0,v0,0x18` Stimme < 24; `@0x80080344 jal 0x80084da8` (_SsVmVSetUp: VH-, Programm-,
  Tonzeiger der VAB `@0x80084e48/50/58`).
- `@0x80080350/58` Kennung 0x21; `@0x80080384 bne v1,a0` voll == volr → Tastenpan 64 (`@0x8008038c/94`),
  Tastenpegel = voll (`@0x8008039c`).
- Programm: `@0x80080448 lbu 1(v0)` mvol → 0x800dcc3a, `@0x80080454 lbu 4(v0)` mpan → 0x800dcc3b.
  Ton: `@0x800804ac lbu 2` vol → 0x800dcc3d, `@0x800804b8 lbu 3` pan → 0x800dcc3e,
  `@0x800804c4 lbu 4` center, `@0x800804d0 lbu 5` shift.
- Tonhöhe `note2pitch2 0x80083010`: `@0x80083038 lbu v0,5(v1)` fine += tone[5]; `>>3`, Übertrag ab 16;
  `@0x80083074 lbu v1,4(v1)` sem = note + 60 − center; Tabelle `0x800aba40` = RE1.5 `0x80077520` (Werkzeug:
  md5 `6b1d457a…` beide).
- Pegel:

```
8008376c lbu a0,24(v0)               ; VH+0x18 (Master)
80083778 sll v0,a0,14 / subu         ; *0x3FFF
80083780 mult v1(=voll),v0 ; 80083788 0x82061029 ... sra 13  -> /0x3F01
800837b0 mult *prog.mvol ; 800837c4 mult *tone.vol ; 800837cc 0x040c2051 ... srl 13 -> /0x3F01
80083844 beq Kennung,33 -> kein Kanal-Pegel
800838b0 tone.pan (0x800dcc3e) ; 80083928 prog.mpan (0x800dcc3b) ; 80083998 Tastenpan (0x800dcc35):
         p < 0x40: R = R*p/0x3F   sonst: L = L*(0x7F-p)/0x3F    (0x04104105 >> 5 = /63)
80083a08 [0x800d6c40]==1 -> Mono (max)
80083a38 Kennung 33 -> kein Quadrieren (80083a40..94 nur fuer Sequenzen)
80083ac4 sh a2 -> 0x800dccb0+Stimme*16 (L) ; 80083ac8 sh a1 -> +2 (R) ; 80083abc sh t0 -> +4 (Tonhöhe)
```

**Formel** (voll = volr = tone.vol · M / 100, M = `[0x800e8768]`):

`V = ⌊⌊voll · VH.mvol · 0x3FFF / 0x3F01⌋ · prog.mvol · tone.vol / 0x3F01⌋`, danach die drei Pan-Stufen.

### 2.4 Lage (nur bei Lagebyte ≠ 0) — `FUN_8005c970`

Kamera = `[[0x800ce324]+0x24] + [0x800d4820]·32` (`@0x8005c970..9a8`) = **Raumkamera** aus dem RDT, nicht
die Türszenen-Kamera. Abstand über zwei `SquareRoot0` (`@0x8005ca48`, `@0x8005ca70`), `/250`
(`@0x8005ca78 0x10624dd3`), geklemmt auf 127; Winkel und Tabellen `0x800a7fb0` (Pan) / `0x800a8030`
(Dämpfung), Verdeckung −35 % (Dekompilat `RE2_Quellcode_V2/FUN_8005c970.c:24-101`, nur Kopf und Division
selbst gelesen). **Für die Tür ohne Belang.**

### 2.5 Die beiden Türtöne ausgerechnet

| | Ton 0 (Skript) | Ton 1 (Door_exit) |
|---|---|---|
| Aufruf | a0 = 0x00000000, Bezug 1, Versatz 0 | a0 = 0x00010000, a1 = 0x800cfc30 |
| Lagebyte → Lage ausgewertet? | 0 → nein (`@0x8005c660`) | 0 → nein |
| Kopfeintrag | `00 00 14 16`: prog 0, Ton 1, Prio 4, Stimme 22, 0 Folgelagen | `00 00 24 17`: prog 0, Ton 2, Prio 4, Stimme 23 |
| Tonsatz | vol 100, pan 0x40, center 84, shift 0, note 60 | vol 105, pan 0x40, center 85, shift 0, note 61 |
| voll = volr | 100 | 105 |
| VH.mvol / prog.mvol / prog.mpan | 0x7F / 0x7F / 0x40 | 0x7F / 0x7F / 0x40 |
| **Stimmpegel L = R** | **10157** / 0x3FFF (−4,15 dB) | **11198** / 0x3FFF (−3,31 dB) |
| Panorama | Mitte (alle drei Pan = 0x40 → L·63/63) | Mitte |
| Tonhöhe | 0x0400 = 11025 Hz | 0x0400 = 11025 Hz |
| ADSR | 0x80FF / 0x1FC0: voll nach 2 Takten, Sustain = Maximum → keine Wirkung | gleich |

Die Kamera (10000,0,0)→(0,0,0) der Türszene geht in keinen der beiden Töne ein.

---

## 3. Das Tonteil-Format

### 3.1 Größe aus der EXE

`re2_disasm.py bytes 0x8009a748 12` (= `0x8009a520 + 0x2E·12`): `e8 4a fc d7 0a 00 00 00 4f 9d 00 00`
→ **Tonteil 0x4AE8 = 19176 B** (`@0x80014d94 lhu s4,0(s1)`), Modellteil 0xD7FC, Sektor 10,
Prüfsumme Ton 0x4F. Nachgerechnet: `10·0x800 + 0xD7FC = 75772` = Dateigröße; XOR des ersten Bytes je
512 B über den Tonteil = 0x4F; 0x4AE8..0x5000 ist Null. Dateinummer `0x8009a50c` = 0x118.
`do2_format.py tabellen`: 55 von 55 Einträgen aus den Dateien nachgerechnet.

### 3.2 Aufbau

| @Tonteil | Bytes | Inhalt | Lader |
|---|---|---|---|
| 0x0000 | `00 00 14 16 00 00 24 17 ff ff ff ff ff ff ff ff` | Kopf, 4 × 4 B | `0x800dbb78[0]` |
| 0x0010 | `70 42 41 56 07 00 00 00 00 00 00 00 d0 4a 00 00 ee ee 01 00 03 00 03 00 7f 40 …` | VH: ps 1, ts 3, vs 3, mvol 0x7F, fsize 0x4AD0 | VH-Zeiger = Kopf + u32 @0xC30 |
| 0x0030 | `03 7f ff ff 40 ff 00 00 …` | Programm 0: 3 Töne, mvol 0x7F, mpan 0x40 | |
| 0x0830 | `00 00 00 40 53 3b 3b 3b … 01 00` | Ton 0: vol 0, VAG 1 (48 B) — **unbenutzt** | |
| 0x0850 | `00 00 64 40 54 00 3c 3c … ff 80 c0 1f 00 00 02 00` | Ton 1 → VAG 2 | |
| 0x0870 | `00 00 69 40 55 00 3d 3d … ff 80 c0 1f 00 00 03 00` | Ton 2 → VAG 3 | |
| 0x0C30 | `10 00 00 00 00 00 00 00` | Nachspann: u32 VH-Versatz, u32 0 | `@0x80014ea4` |
| 0x0C38 | VAG 1 (48 B), VAG 2 (8304 B @VB+0x30), VAG 3 (7696 B @VB+0x20A0) | VB, 16048 B | `@0x80014f84 ori s1,s1,0x1c38` |

VH-Größe 32 + 2048 + 1·512 + 512 = 0xC20; VH.fsize 0x4AD0 = 0xC20 + 0x3EB0. Summe der VAG-Größen = VB.

### 3.3 Gegen die Port-Mini-Bank `TUERSE.VBS` (Zweig `r30/tuer-verschlossen`)

`load_re2_door_se_pc` (audio_pc.c r30:1218-1262): Map = `vbs[edt_off .. edt_off+edt_size)`,
VH-Versatz = u32 @`edt_size−8`, `re15_vab_parse(edt+vh_off, edt_size−vh_off)`, VAGs aus
`vbs[vbd_off ..]`. `re2_door_se_cut.py` baut `[Map 0x20][VH][u32 0x20, u32 0][VBD]`.

| | TUERSE.VBS | DOOR2E-Tonteil |
|---|---|---|
| Map | 0x20 B: `00 00 04 16 / 00 00 14 16 / 00 00 24 00 / 00 00 33 00`, Rest 0 | 0x10 B: 2 Einträge + 2 × `ff ff ff ff` |
| VH @ | 0x20 (ps 1, ts 16, vs 4) | 0x10 (ps 1, ts 3, vs 3) |
| Nachspann | @0xC40 `20 00 00 00 00 00 00 00` | @0xC30 `10 00 00 00 00 00 00 00` |
| VB @ / Größe | 3144 / 26832 | **3128 / 16048** |

Gleiches Satzformat. Mit **TOC (edt_off 0, edt_size 3128, vbd_off 3128, vbd_size 16048)** bestehen die
Prüfungen des Port-Laders (in `ton_door2e.py` [5] nachgestellt, nicht im Port ausgeführt): vh_off 16,
16 + 0x20 ≤ 3128, 1 Tonsegment, Größentabelle endet bei 16 + 3104 = 3120 ≤ 3128. Die VAG-Liste
kompaktiert {1:48, 2:8304, 3:7696} → `samples[0..2]`; Ton 1 (vag 2) → `samples[1]` @VB+0x30.

**Bedeutung der 4-B-Einträge:** gleich. Beide Arten liest dieselbe RE2-Funktion `0x8005ba28`
(Tür: Bank 0; Schloss-Ton: Bank 2, `@0x800516a0 lui a0,0x216`). Der Port-Leser `re15_edt_decode`
(vab_common.c:252) zerlegt Feld für Feld gleich (b0 Bit 7, b1&0x7F, b2>>4, b2&0xF, b3>>5). Unterschiede:

| | RE2 `0x8005ba28` | Port `re15_edt_decode` | Folge für die Tür |
|---|---|---|---|
| Stimme | b3 & 0x1F = SPU-Stimme (22/23) und Satzindex | (b3 & 0x1F) − 16 = 6/7, SPU 16 + v | gleiche Hardware-Stimme |
| leer | Wort == 0xFFFFFFFF (`@0x8005baa0`) | b2 == 0 && b3 == 0 | Einträge 2/3 würden nicht als leer erkannt; `resolve_layers` liefert für `ff ff ff ff` 0 Lagen (vag_index 0) → still. Nur 0/1 werden gerufen |
| Folgelagen | ohne Obergrenze (`@0x8005bd30`) | bricht ab Stimme > 7 | 0 Folgelagen |
| Hall-Bit | tone[1] &= 0xFB bei VAB 0 | kein Hall | gleich |

---

## 4. Die beiden Töne

`python build/tor_1170/re_ton/ton_door2e.py` (0 Fehler), `python build/tor_1170/re_ton/huelle.py`.
Dekodiert wie `re15_vag_adpcm_decode` (Ende bei Flag 0x01; letzter Block beide Male Flags 0x01 =
Ende + Stumm, kein Loop).

| | Ton 0 | Ton 1 |
|---|---|---|
| VAG | 2, sha1 `6eec904c29a6…`, 518 Blöcke | 3, sha1 `8e9ba1072a6e…`, 480 Blöcke |
| Abtastwerte / Rate | 14504 / 11025 Hz | 13440 / 11025 Hz |
| **Dauer** | **1,316 s** (≈ 79 Bilder bei VSync(0)) | **1,219 s** |
| **Pegel roh** | Spitze 30812 (−0,53 dBFS), RMS −12,81 dBFS | Spitze 28672 (−1,16 dBFS), RMS −12,96 dBFS |
| Hüllkurve (RMS je 0,1 s) | −13/−10/−20 dB, 0,3–0,4 s −40 dB, 0,5–0,8 s −11…−8 dB, dann bis 1,3 s aus | 0–0,4 s −13…−8 dB, dann gleichmäßig bis 1,2 s aus |
| Nulldurchgänge | 1,3–2,9 kHz | 2,0–3,4 kHz |

Dateien in `build/tor_1170/ton/`:
- `DOOR2E_ton0_roh.wav`, `DOOR2E_ton1_roh.wav` — mono, 11025 Hz, Verstärkung 1 (so dekodiert der Port).
- `DOOR2E_ton0_re2pegel.wav`, `DOOR2E_ton1_re2pegel.wav` — stereo, mit RE2-Stimmpegel (V/0x4000) und
  ADSR-Hülle; das ist der Stimmausgang vor SPU-Hauptpegel.

„Öffnen“/„Schließen“ ist weiter nur aus dem Zeitpunkt gedeutet; die Hüllkurve zeigt bei Ton 0 zwei
Anschläge, bei Ton 1 einen.

---

## 5. Rezept für den Port

**Datei.** `re15_port/shared_assets/RE2/TORSE.VBS` = `DOOR2E.DO2[0x0000 .. 0x4AE8)` **unverändert**,
19176 B, sha1 `5565fb172cd783d21bb31be802535273e629fb51`. Ein Schnittwerkzeug (neu, Muster
`re2_door_se_cut.py`) prüft vorher: Tabelle `0x8009a748` = `e8 4a fc d7 0a 00 00 00 4f 9d 00 00`,
XOR 0x4F, sha1 der Datei `3461160c…`. Keine Umnummerierung nötig (anders als TUERSE.VBS).

**TOC** (`gen/re2_tor_bank.inc`): `EDT_OFF 0`, `EDT_SIZE 3128`, `VBD_OFF 3128`, `VBD_SIZE 16048`,
`SE_AUF 0` (Kopfeintrag 0), `SE_ZU 1` (Kopfeintrag 1).

**Slot.** Kopie von Slot 5b (`load_re2_door_se_pc` / `re15_audio_re2_door_se`, audio_pc.c r30:1211-1272)
mit eigenen `s_tor_*`-Feldern; Abspielen über `se_play_layers(s_tor_edt, &s_tor_vab, s_tor_decoded,
s_tor_decoded_len, se)`. Das ergibt ohne weitere Änderung: Stimme 6/7 (= SPU 22/23), Prio 4, Ton 1/2,
keine Folgelagen, Tonhöhe `re15_vab_note2pitch2(60,0,84,0)` = `(61,0,85,0)` = 0x400.

**Aufrufe** (Bildzähler der Türszene = Arbeitsbereich +0x22E, `@0x80013de0`, +1 je Durchlauf `@0x8001401c`):

| Bild | Aufruf | Beleg |
|---|---|---|
| 100 | `re15_audio_re2_tor_se(0)` | Skript 1 und 2: `Sleep 70`, `Sleep 30`, `Gosub 4` (wartet, solange var 13 == 1; im Port ist der Ton sofort geladen → kein Warten), `Se_on` @Datei 0x0507A/0x050F0; `[SIM] tuerskript_dump.py sim … DOOR2E.DO2 0|1` → Bild 100 |
| Door_exit | `re15_audio_re2_tor_se(1)` | im ersten Durchlauf, in dem Blende 0 fertig ist (`@0x80014184..94`), nach Pegel 0x7FFF (`@0x800141a4..c4`), wenn Merker (Objekt 0 Flag 0x800) gesetzt; `[SIM]` Ausblende ab Bild 260, +8 je Bild, 0x8000 in Bild 292 |
| jedes Bild | `re15_audio_tick()` nach `re15_render_end_frame()` in der Türszenen-Schleife | RE2 keyt in der Pumpe `@0x8002b9a0` nach VSync; der Port keyt in `se_voice_pump` in `re15_audio_tick` (audio_pc.c master :3229/:3280, r30 :3304/:3355; Muster Menüschleife `platform/pc/main.c:1593`). Ohne diesen Aufruf bleibt der Ton vorgemerkt, bis die Schleife endet |

**Nicht vergessen — der Schließton darf den Raumwechsel überleben.** RE2: der Raum-Bank-Lader
`FUN_80059e54` (`@0x80059e90 jal 0x800597a4`) schaltet nur Stimmen ab, deren Startadresse in
0x14441..0x3DC4F liegt (`@0x800597bc/c0` 0xFFFEBBBF, `@0x800597c8/cc` 0x2980E, `@0x80059804 sltu`,
`@0x8005980c/10` `SpuSetKey(0, Stimme)`); die Türbank beginnt bei 0x3DC50 (`@0x80014f08/18`) und bleibt.
Port: `re15_audio_load_room_banks` (audio_pc.c master :3400-3410, r30 :3475-3485) setzt **alle**
`s_active[].active = 0` und löscht `s_se_pend`. Folgt der Raumlader im Port der Türszene, schneidet er
Ton 1 ab. Gegenstück zur RE2-Regel: dort Stimmen auslassen, deren `pcm` in `s_tor_decoded[]` liegt
(Vergleichsmuster audio_pc.c master :1215, r30 :1290), und deren Vormerkung nicht löschen.

**Pegel (Abweichung, bewusst benennen).** `se_play_layers` rechnet `vol = tone.vol·0x4000/127 >> 1`
(audio_pc.c r30:713), linear. RE2 rechnet `tone.vol² · mvol · mvol` (§2.3). Für die Tür:

| | Port heute | RE2 in Port-Einheit ((V·0x4000/0x3FFF) >> 1) | Unterschied |
|---|---|---|---|
| Ton 0 | 6450 | 5078 | Port +2,08 dB |
| Ton 1 | 6772 | 5599 | Port +1,65 dB |

Byte-nah wird es nur mit einem Pegel-Übersteuer für diese zwei Aufrufe (L = R = 5078 bzw. 5599);
das ist eine Änderung an `se_play_layers` oder ein eigener Abspielweg, keine Datenfrage.

**Abnahme (Vorschlag).** Sonde ohne SDL: TORSE.VBS laden → `re15_vab_parse` = 0, `vag_count` 3,
`re15_edt_decode(map,0)` = {prog 0, tone 1, prio 4, voice 6, extra 0}, `(map,1)` = {…, tone 2, voice 7},
`re15_vab_note2pitch2` = 0x400 beide, dekodierte Längen 14504 und 13440.

---

## 6. Nachmessen

```
S=.claude/skills/re15-psx-disasm/scripts
python $S/re2_disasm.py dis 0x80056428 80       # Se_on
python $S/re2_disasm.py table 0x80011458 5      # Bezug-Sprungtabelle
python $S/re2_disasm.py dis 0x8005ba28 200      # Satz vormerken
python $S/re2_disasm.py dis 0x8005c5e4 150      # Pumpe
python $S/re2_disasm.py dis 0x800802b8 150      # SsUtKeyOnV
python $S/re2_disasm.py dis 0x80083760 110      # _SsVmKeyOnNow
python $S/re2_disasm.py dis 0x8001417c 42       # Door_exit
python $S/re2_disasm.py dis 0x80014cd0 190      # Tür-Tonlader
python $S/re2_disasm.py dis 0x800597a4 30       # Stimmen im Raumbank-Bereich abschalten
python $S/re2_disasm.py bytes 0x8009a748 12     # Tabelle Tür 0x2E
python build/tor_1170/re_ton/ton_door2e.py      # alles andere, 0 Fehler
python build/tor_1170/re_ton/huelle.py
```

---

## 7. OFFEN

1. **Wann `FUN_80059e54` im Türablauf läuft** (Aufrufer `FUN_80049e48 @0x8004a2f4`), relativ zu Ton 0 und
   Door_exit — nicht verfolgt. Für das Rezept genügt, dass die Türbank außerhalb des Abschaltbereichs liegt.
   Weitere Abschaltwege (Musik-/Raumwechsel) nicht gesucht. Nächster Weg: Aufrufkette von `FUN_80049e48`,
   dynamisch mit Haltepunkt auf `0x80079498`.
2. **Exaktes Bild von Ton 1.** Reihenfolge von Blendtakt und Door_exit im selben Bild nicht gelesen; Bild 292
   ist aus der Simulation fortgeschrieben.
3. **Ob die VAB-Nummer der Tür wirklich 0 ist**, stützt sich für die Vergabe auf das Dekompilat
   `SsVabOpenHeadWithMode.c:40-47` (Aufrufargument 0 selbst gelesen). Wäre sie 2, nähme die Pumpe für Satz 0
   einen anderen Weg (`@0x8005c7a0..0x8005c874`).
4. **Klang nicht abgehört.** Beschrieben sind Hüllkurve und Nulldurchgänge; die WAVs liegen zum Anhören bereit.
5. **Pegel im Port.** Welche Lösung (Übersteuer oder Pegelgesetz in `se_play_layers` für alle Bänke) ist eine
   Port-Entscheidung; `se_play_layers` gilt auch für RE1.5-Bänke, deren Pegelweg hier nicht geprüft ist.
