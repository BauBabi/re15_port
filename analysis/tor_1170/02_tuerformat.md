# 02 — Tuerformat DO2 / MD1 / TIM: byte-genau, mit Leser, Schreiber und Verfasser

Stand 2026-09-28. Werkzeug: `re15_port/tools/tor/do2_format.py`. Messwerte:
`build/tor_1170/tuerformat.json`. Probemodell: `build/tor_1170/PROBE_RE15.DO2`,
`PROBE_RE2.DO2`, `PROBE.md1`, `PROBE.tim`, `probe_textur.png`.

Jede Zahl unten traegt ihre Herkunft: `@0x…` = Instruktion, die ich an dieser Adresse selbst
disassembliert habe (`re15_disasm.py` fuer RE1.5, `re2_disasm.py` fuer RE2), `@Datei 0x…` =
Bytes in der genannten Datei, `Messung` = Ausgabe des genannten Befehls. Decompilate
(`RE_15_Quellcode_V2`, `RE2_Quellcode_V2`) dienten nur als Wegweiser; zitiert wird die Disassembly.

| Datei | sha1 |
|---|---|
| `re15_port/shared_assets/PSX/DOOR/DOOR00.DO2` (= `info/Re1.5/PSX/DOOR/DOOR00.DO2`, `cmp` gleich) | 5725afcb8c97aa96dc84bacbe85c0f93b3cc3ea6 |
| `info/re2leon/COMMON/DOOR/DOOR00.DO2` | 2e84a3d0c8c72ec6031c6444a2e5a89691f18434 |
| `info/Re1.5/PSX.EXE` | 2637b76d51088ce2b8cece966b8a596a8e532be4 |
| `info/re2leon/PSX.EXE` | d57bd18e94e28b234fd0309a5048de0f65bc660b |

---

## 0. Ergebnis

1. **Es gibt zwei Container, nicht einen.** RE1.5 hat einen Kopf mit drei Offsets; RE2 hat
   keinen Kopf, die Groessen stehen in der EXE. `DO2Extractor.java` kennt nur den ersten.
2. **MD1 und TIM sind in beiden Spielen dasselbe Format.** RE2 `DOOR00` traegt byte-gleich das
   MD1 (2444 B) und die TIM (33312 B) von RE1.5 `DOOR00`; nur der Ton ist ein anderer. Unter den 56
   Archiven gibt es 33 verschiedene MD1 und 48 verschiedene TIM.
3. **Abnahme bestanden:** 56 von 56 Archiven lesen → schreiben → byte-identisch; 56 von 56 MD1
   lueckenlos und ueberlappungsfrei belegt; alle 56 haben denselben Regelaufbau.
4. **Zwei fremde Leser bestaetigen den eigenen:** die MD1-/TIM-Parser der Engine und
   `md1lib.py`/`timlib.py` stimmen bei 58 von 58 Dateien (56 Originale + 2 Probemodelle) ueberein.
5. **Die Tuer-Zeichenroutinen beider Spiele zeichnen nur Dreiecke.** Das ist der Grund, warum alle
   56 Modelle reine Dreiecksnetze sind.
6. **RE1.5 verschiebt die Texturseite nicht, RE2 schon.** Der RE1.5-Lader laedt die TIM nach
   Seite 0x15, laesst die Textursaetze aber auf Seite 0 stehen. RE2 zaehlt 0x15 / 0x1f dazu.

---

## 1. Container RE1.5

### 1.1 Bytes (`DOOR00.DO2`, 57016 B)

| @Datei | Bytes | Feld | Wert |
|---|---|---|---|
| 0x0000 | `0c 00 00 00` | modell_off | 0x0C |
| 0x0004 | `c8 8b 00 00` | ton_off | 0x8BC8 |
| 0x0008 | `f8 97 00 00` | vb_off | 0x97F8 |
| 0x000C | `0c 00 00 00` | md1_rel | 0x0C → MD1 @0x18 |
| 0x0010 | `98 09 00 00` | scd_rel | 0x998 → SCD @0x9A4 |
| 0x0014 | `9c 09 00 00` | tim_rel | 0x99C → TIM @0x9A8 |
| 0x0018 | `70 07 00 00 00 00 00 00 04 00 00 00` | MD1-Kopf | tex_off 0x770, Merker 0, 4 Gruppen |
| 0x09A4 | `02 00 01 00` | SCD-Block | Tabelle mit 1 Eintrag (0x0002), Skript `01 00` |
| 0x09A8 | `10 00 00 00 09 00 00 00 0c 02 00 00 …` | TIM | Magic 0x10, flags 9 |
| 0x8BC8 | `00 00 13 16 00 00 23 17` | Ton-Vorspann | 2 Eintraege zu 4 B |
| 0x8BD0 | `70 42 41 56 …` | VH | `pBAV`, 0xC20 B |
| 0x97F0 | `08 00 00 00 00 00 00 00` | Ton-Nachspann | u32 = 8 = Offset des VH im Tonblock |
| 0x97F8 | … bis 0xDEB8 | VB | 0x46C0 B |

Lage aller Teile: `python re15_port/tools/tor/do2_format.py zeige re15_port/shared_assets/PSX/DOOR/DOOR00.DO2`.

### 1.2 Lader

Datei laden — `FUN_800171f4`:

```
800171f8: lui a1,0x801a            80017204: ori a1,a1,0x1000     ; Ziel 0x801a1000
8001720c: lbu v0,12(v0)            ; Tuernummer = [DAT_800ac9a8 + 0xc]
80017224: lhu a0,0(at)             ; Dateiindex = u16 @0x80071d2c + nr*2
80017230: jal 0x80013b60
```

`@0x80071d2c` = `25 00 00 00`: Tuer 0 → Datei 0x25, zweiter Eintrag 0. Die Dateitabelle
`@0x8006f43c + 0x25*8` = `b8 de 00 00 07 05 00 dd`: Groesse 0xDEB8 = 57016 = Dateilaenge, LBA 0x0507.
`FUN_80013b60` liest davon Groesse (`@0x80013ba0 lw v0,0(at)`) und LBA (`@0x80013bbc lbu`,
`@0x80013bcc lhu`). Byte +7 = 0xDD ist das XOR jedes 512. Bytes der Datei (Messung: 0xDD), wird in
`FUN_80013b60` aber nicht gelesen.

Aufbau — `FUN_800161e0`:

```
8001629c: lui a2,0x801a   800162a0: ori a2,a2,0x1000
800162b4: lw v1,0(t0)     ; t0 = 0x801a1004  ton_off
800162b8: lw a1,0(a3)     ; a3 = 0x801a1008  vb_off
800162bc: lw v0,0(a2)     ;                  modell_off
800162c0: addu v1,v1,a2   800162c4: addu a1,a1,a2   800162cc: addu v0,v0,a2
800162c8: subu a0,a1,v1   ; a0 = vb - ton = Laenge des Tonblocks
800162d4: sw v0,21348(at) ; DAT_800b5364 = Zeiger auf die drei Modell-Offsets
800162dc: jal 0x800170e0  ; Ton
800162ec: lui a0,0x801a   800162f4: ori a0,a0,0x100c        ; BASIS = Datei + 0xC, KONSTANT
800162f8: addu v0,v0,a0   80016320: addu v0,v0,a0   8001633c: addu v0,v0,a0
80016344: ori v0,zero,0x1f15   8001634c: sh v0,-13748(at)   ; DAT_800aca4c: Seite 0x15, CLUT-Zeile 0x1f
80016350: lw a0,8(v1)     80016354: jal 0x8004ee78           ; TIM hochladen
8001635c: lui v0,0x801a   80016360: ori v0,v0,0xb000   80016378: sw v0,-31408(at)  ; Primitivpuffer 0x801ab000
80016388: lw a1,-8(v1)    80016364: ori a0,zero,0x2    80016370: addu a2,zero,zero
80016398: jal 0x80022150  8001639c: addu a3,zero,zero        ; MD1 verschieben, Zuschlag 0 / 0
800163b0: addiu v0,v0,12                                     ; Gruppentabelle = MD1 + 12
800163c8: lw v0,8(v1)     800163cc: lw a0,4(v1)   800163d4: subu a0,v0,a0   ; tim - scd
800163d0: jal 0x800171b4
800163ec: lw v0,0(v0)     800163f8: sw v0,0(v1)   ; DAT_800b3f70 = SCD-Zeiger
800163f4: jal 0x8003edec                          ; Skript 0 starten
```

Folgen fuer den Schreiber:

- Die drei Modell-Offsets zaehlen ab **Datei + 0xC**, einer Konstanten — nicht ab dem Blockanfang.
  In `DOOR00` faellt beides zusammen. Der Schreiber setzt `modell_off = 0xC` fest; der Leser weist
  jeden anderen Wert als ungemessen zurueck.
- Reihenfolge im Lader: Ton → TIM → MD1 → SCD. TIM und VB sind hochgeladen, bevor der
  Primitivpuffer ab `0x801ab000` (= Dateioffset 0xA000) beschrieben wird. **MD1 und SCD muessen
  unter Dateioffset 0xA000 liegen.** `DOOR00`: SCD endet bei 0x9A8.
- `FUN_800171b4` kopiert `tim - scd` Bytes nach `0x801ab000 - Laenge` (`@0x800171bc lui v0,0x801a`,
  `@0x800171c0 ori v0,v0,0xb000`, `@0x800171c4 subu v1,v0,a0`). Quelle ist `DAT_800b5364`
  (`@0x800171b8 lw a1,21348(a1)`), das zu diesem Zeitpunkt auf das **Offset-Feld** `scd_rel` zeigt
  (`@0x800163bc addiu v0,v1,4`), nicht auf die Skriptdaten. Bei `DOOR00` sind das 4 Bytes. Das Skript
  selbst laeuft an Ort und Stelle (`@0x800163ec`). Der Schreiber prueft, dass das Kopierziel nicht
  im MD1/SCD landet.

Ton — `FUN_800170e0(laenge)`:

```
800170f8: lui a0,0x801f   800170fc: ori a0,a0,0xdd00   80017104: lw a1,4100(a1)   80017108: addu a2,s0,zero
80017110: jal 0x8006e4f8              ; Tonblock nach 0x801fdd00 kopieren
80017120: lui at,0x8020   80017124: addu at,s0,at   80017128: lw v0,-8968(at)   ; = [0x801fdd00 + laenge - 8]
80017130: addu v0,v0,s1   80017138: sw v0,9624(at)  ; VH-Zeiger = 0x801fdd00 + Nachspann-u32
80017160: jal 0x8005bf78  80017164: ori a2,zero,0x1020
80017184: lw a0,4104(a0)  80017188: jal 0x8005ce94  ; VB = [0x801a1008]
```

Der Nachspann ist also der Offset des VH im Tonblock. Die Laenge des Tonblocks muss durch 4
teilbar sein (`lw` bei `0x801fdd00 + laenge - 8`).

---

## 2. Container RE2

### 2.1 Bytes (`DOOR00.DO2`, 56784 B)

| @Datei | Bytes | Feld |
|---|---|---|
| 0x0000 | `00 00 14 16 00 00 24 17 ff ff ff ff ff ff ff ff` | Ton-Vorspann, 16 B (in allen 55 Dateien gleich) |
| 0x0010 | `70 42 41 56 …` | VH, 0xC20 B |
| 0x0C30 | `10 00 00 00 00 00 00 00` | Ton-Nachspann, u32 = 0x10 |
| 0x0C38 | … bis 0x4B08 | VB, 0x3ED0 B |
| 0x4B08 | Nullen bis 0x5000 | Auffuellung bis zur Sektorgrenze |
| 0x5000 | `24 02 00 00` | md1_rel = 0x224 → MD1 @0x5224 |
| 0x5004 | `b0 0b 00 00` | tim_rel = 0xBB0 → TIM @0x5BB0 |
| 0x5008 | `12 00 2e 00 8e 00 04 01 18 01 2c 01 a4 01 d8 01 04 02` | SCD-Tabelle, 9 Eintraege |
| 0x5224 | `70 07 00 00 00 00 00 00 04 00 00 00` | MD1-Kopf |
| 0x5BB0 | `10 00 00 00 09 00 00 00 …` bis 0xDDD0 | TIM |

Reihenfolge im Modellteil: **SCD, MD1, TIM** (RE1.5: MD1, SCD, TIM).

### 2.2 EXE-Tabelle `@0x8009a520`, 12 B je Tuer

`DOOR00`: `08 4b d0 8d 0a 00 00 00 9a 42 00 00`

| Offset | Wert | Bedeutung | Lader |
|---|---|---|---|
| +0 u16 | 0x4B08 | Groesse des Tonteils | `@0x80014d94 lhu s4,0(s1)` |
| +2 u16 | 0x8DD0 | Groesse des Modellteils | `@0x800150f4 lhu v1,2(v1)` → `@0x8001510c sw v1,21256(at)` |
| +4 u32 | 0x0A | Sektor des Modellteils in der Datei | `@0x800150b0 lw t2,4(v1)`, `@0x800150dc addu a2,a2,t2` (auf die LBA) |
| +8 u8 | 0x9A | Pruefsumme Tonteil | `@0x80014d98 lbu a3,8(s1)` → `@0x80014dc0 sb a3,30(s5)` |
| +9 u8 | 0x42 | Pruefsumme Modellteil | `@0x800150d0 lbu v0,9(v1)` → `@0x800150ec sb v0,21278(at)` |

Tabellenzeile = Tuernummer × 12 (`@0x80015094 sll a0,v1,1`, `@0x80015098 addu v1,a0,v1`,
`@0x8001509c sll v1,v1,2`); Ladeziel des Modellteils `0x801a1000` (`@0x80015068`, `@0x8001506c`).

Dateiindex je Tuer: u16 `@0x8009a4b0` (`@0x80014d90 lhu a0,-23376(at)`), `DOOR00` = 0xEA.

**Messung:** alle 12 Bytes lassen sich aus der Datei allein nachrechnen — 55 von 55 Eintraegen
(`python re15_port/tools/tor/do2_format.py tabellen`).

Pruefsumme = XOR des ersten Bytes jedes 512-Byte-Abschnitts. Lesecallback `LAB_8001376c`:

```
80013878: lw v0,0(s0)      8001387c: addiu s0,s0,512   80013880: addiu s3,s3,-1
80013884: lbu v1,31(s1)    8001388c: xor v1,v1,v0      80013894: sb v1,31(s1)
80013898: bne s3,zero,0x80013878        ; s3 = 4 → vier Abschnitte je Sektor (@0x80013774)
```

Der letzte, angebrochene Sektor laeuft ueber dieselbe Rechnung bis zur Dateigroesse
(`@0x8001390c sltu a0,a0,a1`). Verglichen wird in `FUN_80012fb8` (`DAT_800d531f == DAT_800d531e`).

### 2.3 Lader

Ton — `FUN_80014cd0`:

```
80014d78: lui s0,0x801a   80014d7c: ori s0,s0,0x1000   80014da0: subu s0,s0,s4   ; Ziel = 0x801a1000 - ton_groesse
80014dd8: jal 0x80012fb8
80014de0: lui a2,0x801f   80014dec: ori a2,a2,0xb700   80014e48: addiu a3,s0,3120 ; 0xC30 B nach 0x801fb700
80014ea4: lw v0,-15568(v0)  ; [0x801fc330] = Nachspann-u32   80014ebc: addu v0,v0,v1  ; VH-Zeiger
80014f14: jal 0x80085368    80014f18: ori a2,a2,0xdc50       ; a2 = 0x3dc50
80014f80: lui s1,0x801a     80014f84: ori s1,s1,0x1c38       ; VB bei Tonteil + 0xC38
```

Nachspann und VB liegen an **festen** Stellen (0xC30, 0xC38). Das VH ist damit immer 0xC20 B und der
Vorspann immer 16 B.

Modell — `FUN_80013c1c`:

```
80013c7c: jal 0x80015064                                   ; Modellteil nach 0x801a1000
80013d14: lui v1,0x801a   80013d18: ori v1,v1,0x1004   80013d1c: lui s0,0x801a   80013d20: ori s0,s0,0x1000
80013d24: lw a0,0(v1)     80013d34: addu a0,a0,s0      ; TIM = Basis + tim_rel
80013d3c: lw v0,0(s0)     80013d48: addu v0,v0,s0      ; MD1 = Basis + md1_rel
80013d50: jal 0x80076a40  80013d54: sw a0,0(v1)        ; TIM hochladen; [DAT_800c3a80+0] = TIM-Adresse
80013d78: lbu v1,12(v0)   80013d7c: addiu v0,zero,40   80013d80: beq v1,v0,0x80013d94
80013d84: addiu a0,zero,1 80013d88: addiu a2,zero,21   80013d90: addiu a3,zero,31
80013d94: addu a2,zero,zero  80013d98: addu a3,zero,zero     ; Tuer 0x28: Zuschlag 0 / 0
80013d9c: jal 0x80076b60
80013de4: addiu v1,v1,12                                     ; Gruppentabelle = MD1 + 12
80013e10: lui v1,0x801a   80013e14: ori v1,v1,0x1008   80013e2c: sw v1,0(v0)   ; SCD-Basis = Modellteil + 8
```

`FUN_80014b40` nimmt den Primitivpuffer aus `[DAT_800c3a80+0]` (`@0x80014b80 lw a1,0(a2)`,
`@0x80014b98 sw v0,0(v1)`). Das ist die TIM-Adresse: **RE2 schreibt die Primitive ueber die bereits
hochgeladene TIM.** Deshalb steht die TIM am Ende des Modellteils, und deshalb muessen SCD und MD1
davor liegen.

---

## 3. MD1

### 3.1 Kopf und Gruppentabelle

| Offset | Feld | Beleg |
|---|---|---|
| +0 u32 | tex_off = Offset der Texturdaten ab MD1-Anfang | Messung: bei 56/56 = `gruppe[0].tex_off + 12` |
| +4 u32 | Merker „schon verschoben", Datei: 0 | `@0x80022180 lw v0,4(s0)`, `@0x80022188 bne v0,zero`; gesetzt `@0x800228ac sw v0,0(a0)` |
| +8 u32 | Gruppenzahl = 2 × Meshzahl | `@0x800228a8 lw t0,4(a0)` / `@0x8002cfd8 lw a3,4(a0)` |
| +12 | Gruppentabelle, 28 B je Gruppe | `@0x800228c0 addiu a2,a2,28` / `@0x8002cff8 addiu a1,a1,28` |

Gruppe (28 B): `vtx_off, vtx_anz, nrm_off, nrm_anz, prim_off, prim_anz, tex_off`. Je Mesh zwei
Gruppen: zuerst Dreiecke, dann Vierecke (Mesh = 56 B: `@0x8001705c sll v0,a1,3`,
`@0x80017060 subu v0,v0,a1`, `@0x8001706c sll v0,v0,3`; Vierecksgruppe bei +28:
`@0x80017090 addiu v0,v0,28`. RE2 gleich: `@0x80014b60..68`, `@0x80014b74`).

Verschieben (`FUN_8002288c`, RE2 `FUN_8002cfd8`): zu `vtx_off`, `nrm_off`, `prim_off`, `tex_off`
jeder Gruppe wird `MD1 + 12` addiert (`@0x8002289c addiu a2,a0,8` mit `a0 = MD1 + 4`;
`@0x800228b8`, `@0x800228cc`, `@0x800228d8`, `@0x800228e0`). Die Schleife ist do-while
(`@0x800228b4 addiu t0,t0,-1` … `@0x800228e8 bne t0,zero`): **0 Gruppen sind nicht lesbar.**

Der Rueckgabewert von `FUN_80022150` ist MD1+0 (`@0x80022184 lw s6,0(s0)`, `@0x800222d0`); die
Tuer-Initialisierung verwirft ihn.

### 3.2 Regelaufbau (Messung, 56 von 56)

```
Kopf 12 | Gruppentabelle | Vertices mesh0..n | Normalen mesh0..n |
je Mesh: Dreiecke, Vierecke | je Mesh: Dreiecks-Tex, Vierecks-Tex
```

Dreiecks- und Vierecksgruppe eines Mesh zeigen auf denselben Vertex- und Normalenblock. Eine leere
Vierecksgruppe traegt `prim_off` = Ende der eigenen Dreiecke und `tex_off` = Ende der eigenen
Dreiecks-Tex. Belegungskarte `DOOR00` (Offsets ab MD1-Anfang):

| von | bis | Feld |
|---|---|---|
| 0 | 12 | Kopf |
| 12 | 124 | Gruppentabelle (4 × 28) |
| 124 | 188 | mesh0.vertices (8 × 8) |
| 188 | 380 | mesh1.vertices (24 × 8) |
| 380 | 572 | mesh0.normalen (24 × 8) |
| 572 | 1364 | mesh1.normalen (99 × 8) |
| 1364 | 1508 | mesh0.dreiecke (12 × 12) |
| 1508 | 1904 | mesh1.dreiecke (33 × 12) |
| 1904 | 2048 | mesh0.dreiecke_tex (12 × 12) |
| 2048 | 2444 | mesh1.dreiecke_tex (33 × 12) |

Alle 56 Karten stehen in `tuerformat.json` → `abnahme.archive[].md1_belegung`.

**Warum die Texturdaten zusammenhaengen muessen:** der Lader summiert die Groessen aller Gruppen
(`@0x800221c8 andi v0,t0,0x1`, `@0x800221d0 addiu v1,v0,3`, `@0x800221d4 sll v0,t2,2`,
`@0x800221d8 mult v1,v0`), kopiert **einen** Block ab `gruppe[0].tex` (`@0x800221a4 lw a1,24(s3)`)
nach `0x8018fff0 - Summe` (`@0x8002227c lui v0,0x8018`, `@0x80022280 ori v0,v0,0xfff0`,
`@0x8002228c jal 0x800104b0`) und verteilt die Zeiger danach der Reihe nach
(`@0x800222b0 sw s0,4(a0)`, `@0x800222c0 addu s0,s0,v0`). RE2 tut dasselbe nur bei Modus 0
(`@0x80076c28 bne s5,zero`); die Tuer ruft mit Modus 1, dort bleiben die Texturdaten an Ort und
Stelle. `lader_pruefung()` prueft die Reihenfolge.

### 3.3 Saetze

| Satz | Groesse | Aufbau | Beleg |
|---|---|---|---|
| Vertex, Normale | 8 B | s16 x, y, z, pad | `@0x80016be0 sll v0,v0,3` / `@0x80014764` |
| Dreieck | 12 B | u16 n0, v0, n1, v1, n2, v2 | Vertex: `@0x80016bf0 lh v0,2(a0)`, `@0x80016bd0 lh v0,6(a0)`, `@0x80016bdc lh v1,10(a0)`; Schritt `@0x80016c78 addiu s3,s3,12`. Normalen (RE2): `@0x800148b8 lh v0,0(fp)`, `@0x800148a4 lh v0,-6(s2)`, `@0x800148a8 lh v1,-2(s2)` mit `s2 = fp + 10` |
| Dreiecks-Tex | 12 B | u8 u0, v0; u16 clut; u8 u1, v1; u16 tpage; u8 u2, v2; u16 pad | `@0x800259fc lw v0,0(t3)` → Prim +12; `@0x80025a08 lw v0,-4(t1)`, `@0x80025a14 or v0,t7,v0` → +24; `@0x80025a1c lw v0,0(t1)` → +36; Schritt `@0x80025a6c` |
| Viereck | 16 B | u16 n0, v0 … n3, v3 | `@0x8002579c addiu a3,a3,16` (FUN_800256b0) |
| Vierecks-Tex | 16 B | wie Dreieck + u8 u3, v3; u16 pad | `@0x800221c8..d8`: (Gruppe & 1) + 3 Worte |

Indizes sind Satznummern; der Lader rechnet × 8. Das Index-Lesen ist `lh` (vorzeichenbehaftet).

Messung ueber 8605 Dreiecke / 5676 Vertices / 18236 Normalen: `pad` ist ueberall 0.

### 3.4 Texturseite und CLUT im Textursatz

Dateiwerte (Messung): `tpage = 0x0080`, `clut = 0x7800` bei 8371 Dreiecken in 55 Modellen;
`tpage = 0x0095`, `clut = 0x7FC0` bei den 234 Dreiecken von RE2 `DOOR28`
(`@Datei 0x85BC`: `53 55 c0 7f 47 03 95 00 53 03 00 00`).

- 0x0080 = 8 bit, Seite 0. 0x7800 = CLUT-Zeile 480, Spalte 0.
- RE2 zaehlt beim Laden dazu: `@0x80076ba4 sll s1,s1,16` (tpage + a2), `@0x80076ba8 sll s2,s2,22`
  (clut + a3 × 64), angewandt `@0x80076bf8 addu v0,v0,s1`, `@0x80076c08 addu v0,v0,s2`. Mit
  a2 = 0x15, a3 = 0x1F: 0x80 + 0x15 = **0x95**, 0x7800 + 0x7C0 = **0x7FC0**.
- `DOOR28` ist die Tuer 0x28, fuer die der Lader 0 / 0 uebergibt (`@0x80013d7c addiu v0,zero,40`).
  Ihre Datei traegt genau die schon verrechneten Werte. **Bytes und Lader decken sich.**
- RE1.5 uebergibt immer 0 / 0 (`@0x80016370`, `@0x8001639c`). Die TIM liegt danach auf Seite 0x15,
  die Textursaetze zeigen auf Seite 0.

Zusaetzlich verodert der Primitivaufbau `(objekt.w8 & 0xC) << 19` in das tpage-Wort
(`@0x800259c4 andi v0,v0,0xc`, `@0x800259cc sll t7,v0,19`) — die Halbtransparenz-Art aus dem
Aufbau-Satz des Skripts.

### 3.5 Zeichnen

Beide Schleifen rufen genau eine Zeichenroutine (Messung: `jal` in `0x800166c4..0x80016b50` =
`0x80068098`, `0x80016b54`; in `0x80014234..0x80014688` = `0x8008e1f4`, `0x8001468c`). Beide lesen
nur die Dreiecksgruppe:

- RE1.5 `FUN_80016b54`: `@0x80016ba4 lw v1,4(s4)`, `@0x80016bac lw s6,20(v1)`,
  `@0x80016bb0 lw s5,0(v1)`, `@0x80016bb4 lw a0,16(v1)`. Normalen werden nicht gelesen; Farbe ist
  `objekt+0x70 = 0x808080` (`@0x8001704c lui a2,0x80`, `@0x80017050 ori a2,a2,0x8080`,
  `@0x80017098 sw a2,112(s0)`).
- RE2 `FUN_8001468c`: `@0x80014710 lw fp,16(a2)`, `@0x80014718 lw t0,20(a2)`,
  `@0x80014720 lw s5,0(a2)`, `@0x80014724 lw t1,8(a2)`; beleuchtet mit NCCT
  (`@0x80014958 .word 0x4b18043f`).
- Rueckseiten fallen weg: NCLIP (`@0x80016cfc` / `@0x800148d0 .word 0x4b400006`), gezeichnet wird bei
  Ergebnis >= 0 (`@0x80016d24 bltz v0,0x80016ee0` / `@0x800148f8 bgez v0,0x80014938`).
- RE2 setzt das ABE-Bit aus dem Objektflag 0x4000: `@0x8001473c srl v0,v0,13`,
  `@0x80014740 andi v0,v0,0x2`, `@0x80014744 ori v0,v0,0x34`.

Primitivpuffer: 80 B je Dreieck (`@0x80025968..70`: (n×4+n)×16; `@0x80025a64 addiu a3,a3,80`),
104 B je Viereck (`@0x80025ac0..d0`). RE1.5 baut auch die Vierecks-Primitive (`FUN_80017048` ruft
`0x80025940` und `0x80025a98`), zeichnet sie aber nie.

---

## 4. TIM

Messung ueber 56 Dateien (`python re15_port/tools/tor/do2_format.py tim`):

| Groesse | Wert | Anzahl |
|---|---|---|
| flags | 9 (8 bit + CLUT) | 56 |
| CLUT | 256 × 1 bei (0, 480), Block 0x20C | 56 |
| Bild | 64 Worte × 256 = 128 × 256 Punkte bei (0, 0), Block 0x800C | 56 |
| Laenge | 0x8220 = 33312 B | 56 |
| CLUT[0] = 0x0000 | | 48 |
| zeichnet Texel mit Wert 0x0000 | | 7 |
| zeichnet Texel mit STP-Bit | | 8 |
| zeichnet 0x8000 (Schwarz mit STP) | | 6 |

`@Datei 0x9A8` (RE1.5): `10 00 00 00 | 09 00 00 00 | 0c 02 00 00 | 00 00 e0 01 00 01 01 00`.

Der Lader ueberschreibt die Lage (`FUN_8004ee78`, RE2 `FUN_80076a40` gleich):

```
8004eea0: lbu v1,0(s1)      ; s1 = 0x800aca4c, Byte 0 = 0x15
8004eea8: sll a0,v1,6       8004eeac: sltiu v1,v1,0x10   8004eeb8: addiu a0,a0,-1024
8004eebc: sh a0,0(v0)       ; Bild x = 0x15*64 - 1024 = 320
8004eec8: sltiu v0,v0,0x10  8004eecc: xori v0,v0,0x1     8004eed0: sll v0,v0,8
8004eed4: sh v0,2(v1)       ; Bild y = 256
8004ef08..18:               ; Byte 0 += (Breite + 63) >> 6  → 0x16
8004ef30: lbu v0,-13747(v0) 8004ef38: addiu v0,v0,480   8004ef3c: sh v0,2(v1)   ; CLUT y = 480 + 0x1f = 511
```

Bild x/y und CLUT y der Datei sind damit wirkungslos. CLUT x bleibt der Dateiwert (0).

---

## 5. SCD-Block

u16-Tabelle, Anzahl = erster Offset / 2, Offsets ab Blockanfang; dahinter die Skripte. Messung
(`tuerformat.json` → `scd_statistik`): alle 56 Bloecke haben eine durch 4 teilbare Laenge, kein
Skript hat ungerade Laenge, 27 der 55 RE2-Bloecke enden auf `01 00 00 00` (Auffuellpaar `00 00`).
Der Schreiber fuellt auf 4 auf, weil dahinter MD1 (RE2) bzw. TIM (RE1.5) mit `lw` gelesen wird.

Der Aufbau-Satz (22 B) ist in beiden Spielen gleich belegt, nur die Opcode-Nummer nicht:
RE1.5 0x4F (`@0x800745e4` → `0x80016f20`), RE2 0x4D (`@0x800a75fc` → `0x80014ba4`).

| Byte | Feld | RE1.5 | RE2 |
|---|---|---|---|
| 1 | Objektplatz | `@0x80016f30 lbu t1,1(a2)` | `@0x80014bb8` |
| 4 | aktiv → Objekt+0 | `@0x80016f64` | `@0x80014be8` |
| 5 | Meshindex | `@0x80016f70` → +0x8E | `@0x80014bf4` → +0x146 |
| 6..7 | Flags | `@0x80016f7c` → +0x8C | `@0x80014c00` → +0x144 |
| 8..9 | w8 (Halbtransparenz-Art) | `@0x80016f88` | `@0x80014c0c` |
| 10..15 | Lage x, y, z (s16) | `@0x80016f94..b4` | `@0x80014c18..38` |
| 16..21 | Drehung x, y, z (u16, 4096 = 360°) | `@0x80016fb8..dc` | `@0x80014c3c..60` |

Elternmatrix: RE1.5 Flag 0x8 mit Platz `& 7` (`@0x80016fe4`, `@0x80016ff4`); RE2 Flag 0x10 mit Platz
`& 0xF` (`@0x80014c64`, `@0x80014c6c`). Objektplaetze: RE1.5 4 (`@0x80016b24 slti v0,s4,4`), RE2 10
(`@0x80013c8c addiu a2,zero,9`).

---

## 6. Ton

Der Vorspann ist die Klangtabelle der Bank 0, 4 B je Klang. `FUN_80045024`:
`@0x800450c4 lui a0,0x801f` / `@0x800450cc ori a0,a0,0xdd00`, `@0x80045140 sll v0,s4,2`,
Grenze `@0x800450bc sltiu v0,s4,0x21`. Byte 1 & 0x7F = Programm (`@0x80045180`), Byte 2 >> 4 = Ton
(`@0x80045168`), Byte 3 & 0x1F = Stimme (`@0x8004517c`), Byte 3 >> 5 = Folgetoene (`@0x8004516c`).

Der Verfasser erzeugt keinen Ton. Er uebernimmt Vorspann, VH, Nachspann und VB aus einem vorhandenen
Archiv (`do2_bauen(…, ton=<Do2>)`).

---

## 7. Abnahme

```
python re15_port/tools/tor/do2_format.py alles
ABNAHME: 56 Archive, byte-identisch 56, MD1 lueckenlos 56, MD1 Regelaufbau 56, TIM identisch 56, Lader lesbar 56
RE2-TABELLE: 55 von 55 Eintraegen aus der Datei nachgerechnet
GEGENPROBE Engine-Parser: gebaut True, 58 von 58 gleich []
GEGENPROBE md1lib/timlib: geladen True, 58 von 58 gleich []
```

Negativkontrollen (Messung): fremde Datei gegen eigene Kennzahlen → ungleich; ein gekipptes Bit im
MD1 → ungleich; verbogener `tex_off` → `lader_pruefung` meldet „gruppe 1: tex_off 0x7f8, der Lader
setzt 0x7f4"; Merker = 1 → gemeldet; `modell_off = 0x10` → zurueckgewiesen.

Kreuzprobe Schreiber: RE2 `DOOR00` in den RE1.5-Container gesetzt und wieder zurueck ergibt die
Originaldatei byte-gleich.

---

## 8. Verfasser-Konventionen (gemessen an allen 56 Modellen)

### 8.1 Achsen, Ursprung, Masse

Referenzblatt = RE1.5 `DOOR00` mesh0, 8 Vertices, 12 Dreiecke. **Derselbe Quader ist mesh0 in 37 der
56 Modelle.**

| Achse | Bereich | Bedeutung |
|---|---|---|
| x | −145 … +143 | Dicke 288, um 0 gemittelt |
| y | −6600 … +2 | Hoehe 6602; oben ist negativ, Unterkante bei 0 |
| z | −3599 … 0 | Breite 3599; **Angelkante bei z = 0** |

Die Angel ist die lokale y-Achse durch den Ursprung. Beleg aus RE2 `DOOR00`, Skript 5
(`@Datei 0x5134`: `2e 05 00 | 00 | 2f 04 06 00 | 2f 0a ff ff`; Handler aus der Tabelle
`@0x800a7580`: 0x2E → `0x80055904`, 0x2F → `0x80055a84`, 0x30 → `0x80055ab0`):

- `0x2E Work_set(5, 0)` waehlt Tuerobjekt 0 (Sprungtabelle `@0x80011200` → `0x800559b0`;
  `@0x800559b0 sll v0,a1,2`, `@0x800559bc lw v0,19928(at)` = `[0x800d4dd8 + nr*4]`).
- `0x2F Speed_set(4, 6)` schreibt Tempo 4 (`@0x80055aa0 sll v1,v1,1`, `@0x80055aac sh a1,344(v1)`).
- `0x30 Add_speed` addiert Tempo 3, 4, 5 auf Drehung x, y, z: `@0x80055ae8 lh a2,352(a0)`,
  `@0x80055afc lhu v0,118(a1)`, `@0x80055b0c sh v0,118(a1)` — Objekt+118 ist die y-Drehung aus Byte
  18..19 des Aufbau-Satzes.

Der Knauf (mesh1) haengt als Kind am Blatt bei (130, −3224, −3372), also 227 vor der freien Kante
(Aufbau-Satz `@Datei 0x504C` in RE2 `DOOR00`: `4d 01 00 e2 01 01 d0 00 10 00 82 00 68 f3 d4 f2 00 00 00 00 00 00`;
Flags 0x00D0 tragen das Elternbit 0x10 mit Platz 0).

Dasselbe Blatt wird fuer die Gegenseite um 180° um y gedreht aufgestellt: Skript 1 stellt es bei
(2000, 3790, 2048) mit Drehung (0, 0, 0) auf, Skript 2 bei (2000, 3790, −1600) mit (0, 2048, 0)
(`tuerformat.json` → `konventionen.modelle[RE2/DOOR00].saetze`).

Weitere Blaetter (Messung, `tuerformat.json` → `konventionen.modelle`):

| Modell | Mesh | x | y | z | Art (am gerenderten Netz gesehen; `DOOR2E` nur nach Massen) |
|---|---|---|---|---|---|
| DOOR0A | 0 | −166 … 163 | −6001 … −1 | −3000 … 0 | Gittertuer, Rohrstaebe |
| DOOR2E | 0 | −120 … 120 | −5977 … −1 | −3044 … 2 | Blatt, 448 Dreiecke |
| DOOR30 | 0 | −90 … 90 | −5942 … 1 | 0 … 1216 | schmales Blatt |
| DOOR14 | 0 | −13 … 155 | −6071 … −10 | 7 … 5864 | zweifluegelig, ein Netz |
| DOOR26 | 1 / 2 | −68 … 270 | −5642 … −4 | −2989 … 1 / 11 … 3001 | zwei Fluegel, Angel je bei z ≈ 0 |
| DOOR27 | 0 / 1 | −241 … 245 / −300 … 299 | −6009 … −7 / −6000 … 2 | 0 … 3236 / −3186 … 8 | zwei Fluegel |
| DOOR2B | 0 | −60 … 0 | −6000 … 0 | 0 … 9000 | breite Flaeche |
| DOOR10 | 1 | −60 … 60 | −6900 … 0 | −60 … 60 | einzelner Stab |
| DOOR16 | 0 | −78 … 78 | −10269 … 551 | −715 … 719 | Leiter |
| DOOR28 | 0 | −903 … 890 | −1377 … 29 | −1082 … 1071 | Rohrgelaender mit Boden |
| DOOR0E | 0 | −31604 … 31948 | −18000 … 0 | −5578 … 5622 | Treppe |

Gemeinsam: y = 0 ist die Unterkante, oben negativ; ein drehendes Blatt beginnt bei z = 0; die Dicke
liegt in x. Einzelblaetter (Referenzblatt und Tabelle): Hoehe 5638 … 6602, Breite 2990 … 3599
(`DOOR30`: 1216).

### 8.2 Rohre

| Modell | Ecken je Rohrquerschnitt | Durchmesser x / z | Rohre im Schnitt |
|---|---|---|---|
| DOOR0A | 8 | 139 / 136 | 6 |
| DOOR16 | 6 | 156 / 180 | 2 |

(`tuerformat.json` → `rohrquerschnitte`.)

### 8.3 Drehsinn und Normalen

- Normalenlaenge: 4096 bei 17609 von 18236; 4095 bei 434; 4097 bei 189; 0 bei 4.
- Gespeicherte Normale gegen `(p2 − p0) × (p1 − p0)`: gleiche Richtung 8455, entgegen 137,
  senkrecht 4, entartet 9 (von 8605). Die 137 sitzen in 11 Modellen, davon 96 in mesh1 von
  `DOOR1E`/`DOOR33`/`DOOR35`.
- Dieselbe Richtung folgt aus NCLIP >= 0: sichtbar ist die Seite, von der aus die Ecken auf dem
  Bildschirm im Uhrzeigersinn laufen.
- Normalen je Dreieck: drei verschiedene 5622, dreimal derselbe Index 2593, drei Indizes mit
  gleichem Wert 390. Geglaettet ist die Regel, flach die Ausnahme.
- Doppelseitig (dieselben drei Ecken zweimal, gegenlaeufig): nur in 3 Modellen (`DOOR04` 2,
  `DOOR10` 8, `DOOR1F` 6 Flaechen). Blaetter sind geschlossene Koerper, keine doppelseitigen Ebenen.

### 8.4 Textur

- u 0 … 127, v 0 … 255 — genau die 128 × 256 Punkte der TIM.
- Referenzblatt: beide grossen Flaechen nutzen u 0 … 126, v 0 … 217. **u = 126 liegt an der Angel
  (z = 0), u = 0 an der freien Kante; v = 0 oben, v = 217 unten** — auf beiden Seiten gleich, das Bild
  erscheint von hinten also gespiegelt. Die Schmalseiten nutzen Streifen (u 124 … 127 bzw. v 0 … 4).
  Der Knauf nutzt u 0 … 60, v 219 … 255.
- Reines Schwarz: wo die Originale es zeichnen, als 0x8000.

---

## 9. Verfasser-Schnittstelle

```python
import do2_format as F
tim, info = F.tim_aus_bild(bild, schwarz="stp")          # PIL-Bild 128x256, RGB oder RGBA
mesh = F.mesh_aus_dreiecken(dreiecke, tpage=0x80, clut=0x7800, doppelseitig=False)
md1  = F.md1_bauen([mesh, …])
ton  = F.Do2.lesen(open(F.RE15_DOOR, "rb").read())
do2  = F.do2_bauen(md1, tim, [skript0, …], ton=ton, variante="re15")   # oder "re2"
open(ziel, "wb").write(do2.schreiben())
do2.re2_tabelleneintrag()      # nur RE2: die 12 Bytes fuer @0x8009a520
F.lader_pruefung(bytes)        # spielt die Adressrechnung des Laders nach
```

`tim_aus_bild`: erst auf 15 bit, dann — nur wenn noetig — Median-Cut ohne Dithering. Durchsichtige
Punkte → Index 0 mit CLUT[0] = 0x0000. Deckendes Schwarz → 0x8000 (`schwarz="stp"`) oder 0x0421
(`schwarz="anheben"`). Alle anderen Farben STP = 0.

### 9.1 Probemodell

Rechteck 3600 × 6600 in der Blattebene (x = 0), zwei Dreiecke, doppelseitig angelegt = 4 Dreiecke,
4 Vertices, 2 Normalen; MD1 212 B; TIM 33312 B.

| Datei | Laenge | sha1 |
|---|---|---|
| `build/tor_1170/PROBE_RE15.DO2` | 54784 | 5342329420bb248fd81ff3e9a9e947999411e75e |
| `build/tor_1170/PROBE_RE2.DO2` | 54016 | c9f38cf626fc9203dcf0025123d706db76e24127 |

- Mit dem eigenen Leser zurueckgelesen: gleich. `lader_pruefung`: keine Fehler.
- Engine-Parser (`re15_md1_parse`, `re15_tim_parse`) und `md1lib`/`timlib`: gleich.
- TIM gegen das Quellbild: 64 durchsichtige Punkte → Texelwert 0; 13312 schwarze → 0x8000;
  19392 uebrige → 0 Abweichungen, 0 mit STP.
- Quantisierung an einem Bild mit 273 Farben (ROOM11712 auf 128 × 256 verkleinert): groesster Fehler
  1 Stufe von 31, Mittel 0,0002.
- RE2-Tabelleneintrag der Probe: `08 4b 00 83 0a 00 00 00 9a 0c 00 00`.
  RE1.5-Dateitabelle der Probe: Groesse 54784, XOR 0x0D.

---

## 10. Abweichungen von vorhandenen Beschreibungen

| Quelle | Aussage dort | Befund |
|---|---|---|
| `DO2Extractor.java` `Layout.parse` | MD1-Nutzlast beginnt bei `modelOffset + 16`, das Laengenfeld wird rekonstruiert | Das MD1 beginnt bei `0xC + md1_rel` = 0x18; das Feld steht in der Datei (`70 07 00 00`) |
| ebd. | `scriptCount = readInt(vbOffset − 8)` = 8 | Das ist der Ton-Nachspann = Offset des VH (`@0x80017128`). `DOOR00` hat 1 Skript |
| ebd. | ein Aufbau fuer alle DO2 | Gilt nur fuer RE1.5. RE2-Dateien haben keinen Kopf |
| `RE15_KNOWLEDGE.md` §1.13 | „MD1 + TIM + 8 SCDs" | RE1.5 1 Skript, RE2 2 bis 18 |
| `re15_md1.h` | `+4 unknown` | Merker „schon verschoben" |
| `DOORxx/DOORxxxx.c` (alter Extrakt) | `Speed_set(4, 1536)` | Wert ist 6; die 16-bit-Operanden stehen dort bytevertauscht |

---

## 11. Was daraus fuer ein neues Tuermodell folgt

Nur Aussagen, die oben belegt sind:

| Groesse | Wert | Abschnitt |
|---|---|---|
| Primitive | nur Dreiecke; Vierecke werden gebaut, aber nie gezeichnet | 3.5 |
| Sichtbare Seite | Ecken im Uhrzeigersinn; Rueckseite eigens anlegen oder Koerper schliessen | 3.5, 8.3 |
| Normalen | Laenge 4096, Richtung `(p2 − p0) × (p1 − p0)`; RE1.5 liest sie nicht, RE2 beleuchtet damit | 3.5, 8.3 |
| Achsen | x Dicke, y Hoehe (oben negativ, Unterkante 0), z Breite ab der Angel bei 0 | 8.1 |
| Drehachse | lokale y-Achse durch den Ursprung des Mesh | 8.1 |
| Koordinaten | s16; Indizes u16, vom Lader als s16 gelesen → hoechstens 32767 | 3.3 |
| Rohr | 6 oder 8 Ecken im Querschnitt | 8.2 |
| Textur | eine TIM, 128 × 256, 8 bit, CLUT 256 × 1 | 4 |
| UV | u 0 … 127, v 0 … 255 | 8.4 |
| tpage / clut in der Datei | 0x0080 / 0x7800; der Lader nach RE2-Vorbild zaehlt 0x15 / 0x1F × 64 dazu | 3.4 |
| Schwarz | 0x8000, nicht 0x0000 | 4, 9 |
| Durchsichtig | Texelwert 0x0000; ueblich CLUT[0] | 4 |
| Meshzahl | mindestens 1; jedes Mesh = 2 Gruppen | 3.1 |
| Lage im RE1.5-Container | MD1 + SCD unter Dateioffset 0xA000 | 1.2 |
| Lage im RE2-Container | SCD, MD1, dann TIM als letztes | 2.3 |
| Zusatz RE2 | 12 Tabellenbytes `@0x8009a520` je Tuer, aus der Datei berechenbar | 2.2 |
| Zusatz RE1.5 | Dateitabelle `@0x8006f43c + nr*8`: Groesse u32 | 1.2 |

Der RE1.5-Container beschreibt sich selbst (drei Offsets im Kopf). Der RE2-Container ist ohne die
EXE-Tabelle nicht aufzuteilen; der Leser hier behilft sich mit `VabHdr.fsize` und der Sektorgrenze,
was bei 55 von 55 Dateien die Tabelle trifft.

---

## 12. OFFEN

1. **Massstab zur Raumwelt.** Das Blatt misst 3599 × 6602 Einheiten der Tuerszene. In welchem
   Verhaeltnis diese zu den Einheiten von ROOM1170 stehen, ist nicht gemessen. Naechster Weg:
   Kamera und Projektionsabstand der Tuerszene (`FUN_80053ca4`, RE2 `FUN_80076cb0`) disassemblieren.
2. **Kamera der Tuerszene.** Gelesen sind nur die geschriebenen Werte
   (`@0x80016460 ori v0,zero,0x7530` → `0x800b2210`; `@0x80016468 ori v0,zero,0x55f0` → `0x800b221c`).
   Welche davon Standort und welche Blickziel sind, ist nicht belegt.
3. **Obergrenze der Dreiecke.** RE1.5: zwischen Primitivpuffer `0x801ab000` und dem Teilungspuffer
   `0x801b1000` liegen 0x6000 B = 307 Dreiecke zu 80 B, summiert ueber alle aufgestellten Objekte.
   Das ist aus zwei Adressen abgeleitet, nicht im Lauf gemessen. RE2: nicht bestimmt.
4. **`als_wurzel` / `als_kind` in `tuerformat.json`** stammen aus einer Mustersuche nach dem
   Aufbau-Opcode, nicht aus einem SCD-Lauf. Einzelne Saetze koennen fehlen oder zuviel sein.
5. **Vierecke.** Format und Lader sind belegt, aber keine der 56 Dateien enthaelt eines; der
   Schreiber ist dafuer nur gegen den eigenen Leser geprueft.
6. **Ungelesene Felder des Aufbau-Satzes:** Byte 2 und 3.
7. **Java-Leser nicht ausgefuehrt** (Gradle fehlt). Der Vergleich in Abschnitt 10 beruht auf dem
   Quelltext.

---

## 13. Nachmessen

```bash
python re15_port/tools/tor/do2_format.py alles          # alles, schreibt build/tor_1170/tuerformat.json
python re15_port/tools/tor/do2_format.py abnahme        # 56/56
python re15_port/tools/tor/do2_format.py tabellen       # RE2-Tabelle 55/55, RE1.5-Dateitabelle
python re15_port/tools/tor/do2_format.py konventionen   # Abschnitt 8
python re15_port/tools/tor/do2_format.py probe          # Probemodell
python re15_port/tools/tor/do2_format.py gegenprobe     # baut den Pruefer, 58/58
python re15_port/tools/tor/do2_format.py zeige <datei>
S=.claude/skills/re15-psx-disasm/scripts
python $S/re15_disasm.py dis 0x800161e0 190             # RE1.5 Aufbau
python $S/re15_disasm.py dis 0x80022150 110             # MD1 verschieben
python $S/re2_disasm.py  dis 0x80013c1c 170             # RE2 Aufbau
python $S/re2_disasm.py  dis 0x80076b60 84              # RE2 MD1 verschieben + Zuschlag
python $S/re2_disasm.py  dis 0x8001376c 150             # Pruefsumme
python $S/re2_disasm.py  bytes 0x8009a520 660           # Tuertabelle
```

Die Gegenprobe uebersetzt `build/tor_1170/pruefer/pruefer_engine.c` mit `gcc` aus
`C:\msys64\mingw64\bin` gegen `re15_port/engine/src/md1_common.c` und `tim_common.c`. Der Port selbst
wird dabei nicht gebaut.
