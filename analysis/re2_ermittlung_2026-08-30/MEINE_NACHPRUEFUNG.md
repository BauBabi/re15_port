# Meine eigene Nachpruefung der Agenten-Befunde (2026-08-30)

Regel im Projekt: JEDEN Agenten-Befund selbst gegen die Bytes verifizieren, bevor er als
Tatsache weitergegeben wird. Das hier ist das Protokoll. Alles unten habe ICH gefahren,
nicht ein Agent.

## BESTAETIGT

### 1. Die fuenf Spawn-Records in ROOM7040
```
@0x0011ac: 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00
@0x0011c2: 44 00 01 37 01 00 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00
@0x0011d8: 44 00 02 37 ...   @0x0011ee: 44 00 03 37 ...   @0x001204: 44 00 04 37 ...
```
Typ-Byte (+3) = 0x36, 0x37, 0x37, 0x37, 0x37. Slot (+2) = 0,1,2,3,4.
X/Y/Z aus Record 0 = (-32000, 0, -32000) — alle fuenf geparkt.
Wort bei +4 = 0x8000 nur beim 0x36 (Schlafbit -> Entity+0x10E).

### 2. Die Boss-KI liegt wirklich in CDEMD0.EMS
`info/re2leon/PL0/PLD/CDEMD0.EMS` = 11.124.736 B = 5432 Sektoren.
Chunk `[3456*2048, +23476]` geschnitten; Wort bei +0xBC = `0x27bdffc8` = `addiu sp,sp,-56`.
Selbst disassembliert:
```
801000bc: addiu sp,sp,-56
801000c4: addu  s3,a0,zero
801000cc: lw    v0,-1060(v0)     ; 0x800cfbdc
80100164: lbu   v0,4(s3)         ; Zustand = Entity+0x4
8010016c: sll   v0,v0,2
80100178: lw    v0,21964(at)     ; 0x801055cc + zustand*4
80100180: jalr  v0
```
Zustandstabelle @0x801055CC, selbst gedumpt:
`[0]=0x801003cc [1]=0x80100784 [2]=0x801025bc [3]=0x80102bbc [4]=0x80103834 [5]=0 [6]=0 [7]=0x80103878`
`[8]` = 0x7a040309 = Datenmaterial.

### 3. HP 600 / 400
```
801003fc: addiu v0,zero,600
80100400: sh    v0,342(s0)       ; +0x156
80100404: lw    v0,0(v1)
8010040c: andi  v0,v0,0x20
80100410: beq   v0,zero,0x8010041c
80100414: addiu v0,zero,400
80100418: sh    v0,342(s0)
```

### 4. Die vier Begleiter sind unverwundbar
Chunk `[3600*2048, +23024]` (Typ 0x37):
```
8010052c: sw    v0,4(s1)
80100530: addiu v0,zero,-1
80100534: sh    v0,342(s1)       ; HP := -1
```

### 5. Die Gegner-TOC @0x8009ADF4 stimmt — und k=2/k=3 ist TIM/EMD
Selbst gelesen (u32-Paare, Index = (typ-0x10)*4 + k):
```
Typ 0x10: k0 Sekt    0/53068  k1   26/53068  k2   52/ 66592  k3   85/146892
Typ 0x20: k0     1195/22266   k1 1206/22266  k2 1217/ 66592  k3 1250/121144
Typ 0x36: k0     3444/23476   k1 3456/23476  k2 3468/132640  k3 3533/112340
Typ 0x37: k0     3588/23024   k1 3600/23024  k2 3612/ 33312  k3 3629/ 59376
```
BEWEIS fuer k=2 = TIM / k=3 = EMD: `EM_TYPE20.TIM` ist 66592 B, `EM_TYPE20.EMD` ist
121144 B — genau die Groessen von Typ 0x20 k=2 bzw. k=3.

### 6. Die losen BIN-Dateien SIND EMS-Chunks (md5-Gleichheit, von mir gefahren)
```
EMZ0.BIN        Sektor   26, 53068 B  identisch
EMD0G_MOD0.BIN  Sektor 1206, 22266 B  identisch
EMOVL21_S0.BIN  Sektor 1320, 19080 B  identisch
EMS25.BIN       Sektor 1825, 26324 B  identisch
EMS26.BIN       Sektor 1901,  4346 B  identisch
```
Damit ist die TOC-Theorie unabhaengig bewiesen.

### 7. Tuer-Datenlage
- RE2: 55 `.DO2`-Archive, 55 Unterverzeichnisse, **498** `.c`-Routinen.
- RE1.5: **genau eine** Datei, `DOOR00.DO2`, 57016 B.
- RE1.5 DOOR00.DO2 SCD bei Datei-`@0x9A4`: `02 00 01 00` = 1-Eintrag-Offsettabelle
  + Opcode 0x01 (END). Das Tuer-Skript ist wirklich leer.
- `DOOR00/DOOR0001.c` enthaelt woertlich die Choreografie (Obj_model_move, Sce_fade_set,
  Sleep 17920/12800/15360, Evt_exec 11/12).
- **MD1 und TIM von DOOR00 sind zwischen RE1.5 und RE2 BYTE-IDENTISCH**
  (RE1.5[0x18:0x798] == RE2[0x5224:0x59A4]; RE1.5[0x9A8:0x8BC8] == RE2[0x5BB0:0x8DD0+0x5000],
  33312 B). Selbst per md5 geprueft.
- Der Port hat davon **nichts**: `grep -rn "DO2|door_demo|door_seq" re15_port/engine
  re15_port/include` = 0 Treffer.

### 8. Menue-Sound-Zensus (selbst gezaehlt, jal-Wort im ganzen Binaerbild)
`jal 0x80045024` = 0x0c011409 (RE1.5) / `jal 0x8005ba28` = 0x0c016e8a (RE2):
```
RE1.5 PSX.EXE  41      RE2 PSX.EXE     138
RE1.5 TITLE     4      RE2 TITLE        47
RE1.5 DEBUG    23      RE2 CONFIG       34
                       RE2 MEM_CARD     24
                       RE2 SELECT        4
                       RE2 DIEDEMO       1
                       RE2 ENDING        2
                       RE2 RESULT        0
                       RE2 OPENING       0
```
Die 41 RE1.5-Aufrufe liegen auf:
```
800123d4 80017684 8001782c 800178cc 80018064 800180e0 80018358 80018424 800185ec
80018d9c 80018e3c 8002c70c 8002c97c 8003338c 80033ed0 80034488 80034a0c 80034c44
80034e54 8003537c 800355f8 80035a1c 80035c00 80035d38 80035e80 80036004 80036184
80036274 80036340 800364f0 800367a8 80041730 8004a158 8004a47c 8004a4a4 8004a4cc
8004a4f4 8004a520 8004a664 80052ad8 80052bcc
```
Groesste Luecke: **0x80018e3c .. 0x8002c70c = 80080 Byte ohne einen einzigen SE-Aufruf**.
In dieser Luecke liegen Game-Over (0x8001500c), Item-Get-Modal (0x8001db28), Memory-Card
(0x80025c00) und die YES/NO-Box (0x80028134). Zweite Luecke 0x8002c97c..0x8003338c
(26640 B) enthaelt den OPTIONS-Task (0x8002dde4). Die Stumm-Aussage haelt.

### 9. CORE-Baenke
RE1.5 hat CORE00..CORE13 (20 Stueck), RE2 hat CORE00, CORE01, CORE0B..CORE15 (13 Stueck).
- RE2 hat **CORE14 und CORE15 zusaetzlich**; CORE15 ist die einzige Bank mit abweichender
  Groesse (3304 B statt 3176 B).
- RE2 fehlen **CORE02..CORE0A** (9 Baenke).
- Von den 11 gemeinsamen sind nur **4 identisch** (CORE00, CORE0F, CORE12, CORE13).
- **CORE10 und CORE11 sind zwischen den Spielen VERTAUSCHT** — md5 von RE1.5-CORE10 ==
  md5 von RE2-CORE11 und umgekehrt. Selbst geprueft, beide Richtungen.

## NICHT REPRODUZIERT (eine Stelle)

`CORE15.EDH pbav=0xC0 -> nrec=48`: ich lese an Datei-Offset 0x80 den Wert 0xFF, nicht 0xC0.
Entweder liegt das Feld woanders, als ich gesucht habe, oder die Angabe stimmt nicht.
Belegt und von mir gemessen ist nur: CORE15 ist die einzige CORE-Bank mit 3304 statt 3176 B,
also strukturell anders. **Wer die 48 Records benutzen will, muss das Feld erst festnageln.**

## OFFEN GEBLIEBEN (vom Bericht selbst so benannt, von mir nicht nachgeholt)

- Die sechs Zustandsfunktionen des Bosses hinter 0x801055CC — Bytes liegen bereit.
- Wer das 0x8000-Schlafbit loescht und den Boss positioniert (STAGE7-Code-Overlay fehlt).
- "Bank 4 = CORE" fuer RE2 (BSS-Zellen, braucht einen RAM-Abzug).
