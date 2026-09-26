# RE2 ROOM2130/213 Schalter-Rätsel — gefunden: ROOM2130 und ROOM213 sind derselbe Raum (Reserve Power Control Panel), der "Cursor auf 80" ist der Leistungswert in SCD-Variable 5 und der Zeiger bewegt sich mit 9 Einheiten pro Punkt, ein Punkt pro Bild; Klick = Raum-SE Gruppe 2/Index 0x0A, Bestätigung = 0x0C, Auswahl-Cursor = Se_on(0x04040000)/0x04060000/0x04050000.

> Alles unter §2 habe ich selbst aus den Bytes gelesen (RE2 PSX.EXE + ROOM2130.RDT),
> kein Decompilat. Sub04 liegt absolut bei **ROOM2130.RDT @0x01110**
> (SCD-MAIN-Basis 0xEA4 + Sub-Offset 0x26C); alle sub04-Offsets unten sind relativ dazu.

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Der Bewegungs- und Bestätigungssound des Cursors bei Rätseln etc. ist grundlegend
falsch. Der Klick Sound soll so sein, wie bei Resident Evil 2 Room 2130/213 ... Außerdem
möchte ich in ROOM 11F0 so einen roten Cursor mit der Bewegungslogik ... Nach der Eingabe
der korrekten Position, muss der Cursor auf 80 stehen."

Mein Auftrag: das **RE2-Original** aufklären (a)–(e). Der Port/RE1.5-Teil ist ein anderer Agent.

### 1.1 ROOM2130 und ROOM213 sind DERSELBE Raum
`info/re2leon/PL0/RDT/room2130/room2130.ini` (aus ROOM2130.RDT extrahiert) Zeile 2–4:
`Stage 2 / Room 13 / Player 0`. RE2-Namensschema = `ROOM<stage><room><player>`:
`2`+`13`+`0`. Die Hintergründe liegen unter `info/re2leon/COMMON/BSS/ROOM213/` (ohne
Player-Ziffer) bzw. `ROOM213.BSS`. **Es gibt nur einen Raum.** Das Schalter-Rätsel liegt in
`info/re2leon/PL0/RDT/ROOM2130.RDT` (139448 Bytes).

### 1.2 Der Raum ist das "Reserve Power Control Panel"
Entschlüsselte RDT-Messages (RE2-Font-Codepage: `0x00`=Space, `0x01`='.', `0x1D`='A'…`0x36`='Z',
`0x3D`='a'…`0x56`='z', `0x3B`='"', `0x3A`="'"), Datei
`info/re2leon/PL0/RDT/room2130/msg/sub*.msg`:

| Datei | Bytes (Anfang) | Klartext |
|---|---|---|
| sub01…sub05 | `25 50 3a 4f 00 3d 00 4f 53 45 50 3f 44 01 …` | "It's a switch. Will you move it?" (+`fb 03` = Auswahlbox) |
| sub06 | `2c 4b 53 41 4e 00 4f 51 4c 4c 48 55 00 50 4b 4b 00 44 45 43 44 01` | "Power supply too high." |
| sub07 | `… 2b 27 01` | "Power supply OK." |
| sub08 | `… 4f 48 45 43 44 50 48 55 00 48 4b 53 01` | "Power supply slightly low." |
| sub09 | `… 50 4b 4b 00 48 4b 53 01` | "Power supply too low." |
| sub11 | `3b 2e 41 4f 41 4e 52 41 00 2c 4b 53 41 4e …` | «"Reserve Power Control Panel" … Power can be routed to the areas with insufficient power using these five switches. Using all the power may cause a short and cut the power supply.» |

## 2. Das Original — mit Adressen und Bytes

### 2.0 Werkzeug (selbst nachgelesen, kein Decompilat)
* RE2-SCD-Längentabelle: aus den 143 Handlern der Dispatch-Tabelle **@0x800a74c8**
  abgeleitet (`re2_scd_oplen.py`, symbolische Ausführung der Handler; RE2 hat KEINE
  Längen-Datentabelle). Ich habe die Tabelle neu erzeugt und die für dieses Dossier
  benutzten Handler **einzeln disassembliert** (siehe 2.2).
* MIPS-Disassembly: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` auf
  `info/re2leon/PSX.EXE`.

### 2.1 (a) Das Rätsel-Sub: ROOM2130.RDT → SCD-MAIN, sub04 (1808 Bytes)
`info/re2leon/PL0/RDT/room2130/scd/sub04.scd` (identisch mit dem SCD-MAIN-Block sub04 in
ROOM2130.RDT). Grobstruktur, Offsets = Byte-Offsets IM SUB:

```
0000: 22 02 07 01                 set(bank 0x02, bit 0x07, 1)   "Panel aktiv"-Sperre
0004: 2b 00 0b 00 ff ef           message_on(msg 0x0b)          Panel-Beschreibung
000E: 29 06                       cut_chg(6)                    Kamera auf die Panel-Nahaufnahme
0010: 2e 01 00 / 32 00 00 00 30 f8 00 00   work_set(1,0)+pos_set  (Spieler-Work)
001C: 2e 04 02 / 32 00 ac 9f 3e f5 c2 d4   work_set(4,OBJ 2)+pos_set(x=0x9FAC, y=0xF53E, z=0xD4C2)
0028: 24 05 00 00                 save(var5 = 0)                POWER-Wert   <- "der Cursor"
002C: 24 06 00 00                 save(var6 = 0)                alter POWER-Wert
0030: 24 07 ac 9f                 save(var7 = 0x9FAC = -24660)  CURSOR-X (Weltkoordinate)
0034..006F: 5x work_set(4, OBJ 3..7) + dir_set(.., 0x0400)      die 5 Hebel in AUS-Stellung
0070..0079: 65 01 … 65 05         sce_espr_kill2(1..5)          5 Effekt-Slots löschen
007A: 2b 00 01 00 ff ef           message_on(msg 0x01)  "It's a switch. Will you move it?"
0082: 36 02 0a 01 00 00 9b a0 00 fc f4 d3   se_on(edt 0x010A)   <<< KLICK beim Betätigen
008E: 06 00 7c 00 / 0092: 21 0b 1f 00       if( ck(bank 0x0b, bit 0x1f) )
  [EIN-Zweig 0x0096..0x0107]  … s. 2.3
010A: 07 00 7a 00                 else  [AUS-Zweig 0x010E..0x0181]
0184: 64 01 16 02 00 00 ba 02 c2 a0 ce f5 c2 d4 00 00  sce_espr_on2(slot 1, ESP 0x16, x=0xA0C2)
0194: 09 / 0a 1e 00               sleep 30 Frames
0198: 2b 00 02 00 ff ef           message_on(msg 0x02)  -> Schalter 2 … identisch bis Schalter 5
…
0610: 2e 01 00 / 32 00 5b a0 00 00 ae d3   Spieler zurück
061C: 29 04                       cut_chg(4)  Kamera zurück
061E: 06 00 1c 00 / 0622: 23 00 05 00 64 00   if(var5 == 100)  -> msg 6 "too high" + se_on(edt 0x010B)
063E: 06 00 80 00 / 0642: 23 00 05 00 50 00   if(var5 == 80)   -> msg 7 "OK"
064E: 22 04 3c 01               set(bank 0x04, bit 0x3c, 1)     RÄTSEL GELÖST
0652: 36 02 0c 01 …             se_on(edt 0x010C)               <<< BESTÄTIGUNGS-/Erfolgssound
0664/0674: 64 0b/0c 16 00 …     sce_espr_on2(slot 0x0b/0x0c, ESP 0x16, Typ 0x0000)
0688: 36 02 0f 01 …             se_on(edt 0x010F)
0698/06A8: 64 0d/0e 16 10 …     sce_espr_on2(slot 0x0d/0x0e, ESP 0x16, Typ 0x0010)
06C6: 23 00 05 00 00 00         if(var5 == 0)   -> msg 9 "too low"
06D8: 23 00 05 01 50 00         if(var5 >  80)  -> msg 6 "too high" else msg 8 "slightly low"
06F0: 36 02 0b 01 …             se_on(edt 0x010B)               <<< FEHLSCHLAG-Sound
0708: 22 02 07 00               set(bank 0x02, bit 0x07, 0)     Sperre lösen
070E: 01 00                     evt_end
```

### 2.2 Die Semantik der benutzten Opcodes — aus den Handlern, nicht aus einer Liste

**`cmp` (0x23), Handler @0x80054474** (selbst disassembliert):
```
80054474: lw   v0,28(a0)          ; task->pc
8005447c: lhu  v1,2(v0)           ; +2 = var_index (low), +3 = operator (high)
80054480: lh   a1,4(v0)           ; +4 = s16 Vergleichswert
80054484: addiu v0,v0,6           ; Länge 6
8005448c: andi v0,v1,0xff         ; var index
80054490: sll  v0,v0,1
800544a4: lh   a0,18412(at)       ; at=0x800d0000 -> SCD-VAR-ARRAY @0x800D47EC, s16-Einträge
80054498: sltiu v1,a2,0x7         ; operator < 7
800544bc: lw   v0,4512(at)        ; Sprungtabelle @0x800111A0
800544cc: xor v0,a0,a1 / sltiu v0,v0,1      ; op 0 : ==
800544d8: slt v0,a1,a0                       ; op 1 : var >  wert
800544e0: slt v0,a0,a1 / xori v0,v0,1        ; op 2 : var >= wert
```
=> **Das SCD-Variablenfeld liegt @0x800D47EC** (s16). `var5` = `0x800D47EC + 10`.

**`calc` (0x26), Handler @0x8005458C** (selbst disassembliert):
```
8005459c: lhu v1,2(v0)     ; +2 = OPERATOR (low byte), +3 = VAR-INDEX (high byte)
800545a0: lh  a2,4(v0)     ; +4 = s16 Operand
800545a4: addiu v0,v0,6    ; Länge 6
800545b0: addiu v0,v0,18412 ; 0x800D47EC
800545b4: srl a1,v1,8 / sll a1,a1,1 / addu a1,a1,v0   ; &var[+3]
800545bc: andi a0,v1,0xff  ; Operator
800545c0: jal 0x80054634   ; ALU
```
=> `26 00 00 10 24 00` = `var[0x10] += 36`, `26 00 01 10 0e 00` = `var[0x10] -= 14`.


### 2.3 (b) Teil 1 — Die Bewegungslogik des ZEIGERS ("Cursor auf 80")

Der Zeiger ist **Objekt-Modell 2** des Raums (`room2130/obj/model02.md1`, 124 Bytes,
ein einziges Dreieck, s. 2.6). Seine X-Weltkoordinate steht in **SCD-Variable 7**
(`0x800D47EC + 7*2`), der Leistungswert in **SCD-Variable 5**.

Startwerte, sub04 @0x0028…0x0033 (Bytes `24 05 00 00 / 24 06 00 00 / 24 07 ac 9f`):
`var5 = 0`, `var6 = 0`, `var7 = 0x9FAC = -24660`.

Pro Schalter (5x identisch, Blöcke @0x008E, 0x01AC, 0x02CA, 0x03E8, 0x0506):

| Zweig | Bytes | Wirkung |
|---|---|---|
| EIN (`ck`=true) | `26 00 00 10 24 00` @0x00AA/0x01C8/0x02E6/0x0404/0x0522 | `var5 += 36` |
| Deckel | `23 00 05 02 64 00` + `24 05 64 00` @0x00B8 | `if (var5 >= 100) var5 = 100` |
| AUS (`ck`=false) | `26 00 01 10 0e 00` @0x0122/0x0240/0x035E/0x047C/0x059A | `var5 -= 14` |
| Boden | `23 00 05 04 00 00` + `24 05 00 00` @0x0130 | `if (var5 <= 0) var5 = 0` |

Danach wird der **tatsächlich angewandte Unterschied** gebildet
(`25 10 05 / 27 01 10 06 / 25 04 10` @0x00C4 → `var4 = var5_neu − var5_alt`) und in einer
Schleife Bild für Bild abgearbeitet (`0f 06 36 00` = `while`, @0x00D0):

```
00D4: 23 00 04 01 00 00   while (var4 > 0)                 [cmp-Operator 1 = ">" @0x800544d8]
00DA: 25 10 04 / 26 00 01 10 01 00 / 25 04 10     var4 -= 1
00E8: 25 10 07 / 26 00 00 10 09 00 / 25 07 10     var7 += 9      <<< SCHRITTWEITE = 9
00F6: 2e 04 02                                    work_set(4, Objekt 2)
00FA: 1d 07 02 01                                 work_copy: var7 (u16) -> Byte pc+4+2
00FE: 32 00 00 00 3e f5 c2 d4                     pos_set(x = <gepatcht>, y=0xF53E, z=0xD4C2)
0106: 02                                          evt_next  -> EIN Bild warten
0108: 10 00                                       while_end
```
Im AUS-Zweig steht an derselben Stelle `26 00 01 10 09 00` (@0x0164) = `var7 -= 9`.

**work_copy (0x1D) belegt** — Handler @0x800542B4 (selbst disassembliert):
```
800542bc: lbu v1,1(a1)    ; +1 = Variablennummer (hier 7)
800542c0: lbu a2,2(a1)    ; +2 = Byte-Offset IN DIE FOLGENDE Instruktion (hier 2)
800542c4: lbu v0,3(a1)    ; +3 = 0 -> Byte, !=0 -> Halbwort  (hier 1)
800542c8: addiu a1,a1,4   ; pc += 4
800542e0: lhu v1,18412(at); var[v1]  (0x800D47EC)
800542e4: addu v0,a1,a2   ; Ziel = pc_danach + a2
800542ec: sh v1,0(v0)     ; SELBSTMODIFIKATION des naechsten Opcodes
```
=> `1d 07 02 01` schreibt var7 in das X-Feld des unmittelbar folgenden `pos_set`.

**Das ist die ganze "Bewegungslogik" des Zeigers: 9 Weltkoordinaten-Einheiten pro
Leistungspunkt, ein Leistungspunkt pro Bild.** Es gibt KEIN Pad-Polling, KEIN Wrap-around
und KEINE Blink-Sperre fuer diesen Zeiger — er folgt allein `var5`.

Zeigerposition = `-24660 + 9 * var5`:

| var5 | X |
|---|---|
| 0 | -24660 (0x9FAC) |
| 80 (Ziel) | **-23940 (0xA23C)** |
| 100 (Maximum) | -23760 (0xA350) |

### 2.4 (b) Teil 2 — Die Bewegungslogik des AUSWAHL-CURSORS (Ja/Nein-Kaestchen)

Jeder Schalter fragt ueber `message_on` mit dem Steuercode `fb 03` (Auswahlbox, Variante 3)
nach; das Kaestchen selbst faehrt die Engine. Funktion **@0x80030844**
(Zustand 4 der Message-Statemaschine, Sprungtabelle @0x80010A24 =
`800302d0,80030328,80030618,800306ec,80030728,8003079c,800307c4`).

Eingang `a1` = 6-Byte-Layoutsatz aus der Tabelle **@0x8009DEE4** (Variante 3 = @0x8009DEF6,
Bytes `02 ca 2a 00 0b 00`) → **Byte 0 = Zahl der Optionen = 2** (Ja/Nein).
Alternativtabelle @0x8009DF10, gewaehlt ueber `0x800CFB74 & 0x840 == 0x40` (@0x80030728–3C).

```
8003087c: lbu v0,23692(s3)   ; Statusbyte = 0x800E873C   (s3 = Message-Work @0x800E2AB0)
80030884: lbu s5,0(s0)       ; s5 = Zahl der Optionen
8003089c: andi s4,v0,0xf     ; s4 = AKTUELLER CURSOR-INDEX (untere 4 Bit des Statusbytes)
80030880: lb  v1,23722(s3)   ; Wiederhol-Sperre @0x800E874A
800308b4: addiu v0,v0,-1 / sb v0,23722(s3)     ; Sperre jedes Bild um 1 herunterzaehlen
800308c0: lw v1,0x800CE310   ; PAD-TRIGGER (neu gedrueckt)
800308c8: andi v0,v1,0x1000  ; BESTAETIGEN
800308cc: beq v0,zero,0x80030964
800308dc: bne v0,zero,0x800309b8   ; Sperre aktiv -> Bestaetigen ignorieren
80030964: beq v0,zero,0x80030988   ; v0 = pad & 0x0800  -> CURSOR VOR
80030968: lui a0,0x404
8003096c: jal 0x8005ba28           ; Se_on(0x04040000, 0)   <<< BEWEGUNGS-KLICK
80030974: addiu v0,s5,-1
80030978: bne v0,s4,0x80030984
8003097c: addiu s4,s4,1            ; cursor++
80030980: addu s4,zero,zero        ; WRAP: am Ende zurueck auf 0
80030984: sb zero,23697(s3)        ; Blink-/Anzeigezaehler @0x800E8749 zuruecksetzen
80030994: andi v0,v0,0x400         ; CURSOR ZURUECK
8003099c: lui a0,0x404
800309a0: jal 0x8005ba28           ; Se_on(0x04040000, 0)   <<< BEWEGUNGS-KLICK
800309a8: bne s4,zero,0x800309b4
800309ac: addiu s4,s4,-1           ; cursor--
800309b0: addiu s4,s5,-1           ; WRAP: unter 0 -> letzte Option
80030b54: lbu v0,23692(s3) / andi v0,v0,0xf0 / or v0,v0,s4 / sb v0,23692(s3)
                                   ; Cursor-Index wieder in die unteren 4 Bit
```
**Bestaetigungston, abhaengig von der gewaehlten Option** (@0x80030918):
```
80030918: andi v0,s1,0x60          ; s1 = Aufruf-Flags
8003092c: andi v1,v0,0xf           ; gewaehlter Index
80030930: beq v1,zero,0x80030948   ; Index 0 ("Ja")
80030938: beq v1,v0,0x80030954     ; Index 1 ("Nein")
8003093c: lui a0,0x405             ;   -> Se_on(0x04050000)   <<< ABBRUCH-Ton
80030944: lui a0,0x406             ; sonst  Se_on(0x04060000) <<< BESTAETIGUNGS-Ton
80030948: andi v0,s1,0x20 / bne -> stumm
80030950: lui a0,0x406             ; Index 0 -> Se_on(0x04060000)
80030954: jal 0x8005ba28
```

**Pad-Bits belegt.** `0x800CE310` ist der Trigger: @0x80039358–0x80039384
`v0 = (0x800CE314 ^ 0x800CE30C) & 0x800CE30C`. `0x800CE30C` wird @0x800391BC–0x800391E8
aus der Roh-Pad-Maske ueber die Umsetztabelle **@0x800A26A0** (3 Konfigurationen a 16 u16,
Zugriff `sll v0,cfg,5` + `sll v0,bit,1`) gebildet: `wenn (roh & tab[cfg][n]) dann logisch |= (1<<n)`.
Konfiguration 0 (@0x800A26A0) = `1000 2000 4000 8000 1000 4000 0040 0040 0008 0080 8000 2000 0040 0010 0020 0000`,
Rohmasken in der `PadRead()`-Anordnung (UP=0x1000, RIGHT=0x2000, DOWN=0x4000, LEFT=0x8000,
TRIANGLE=0x0010, CIRCLE=0x0020, CROSS=0x0040, SQUARE=0x0080, SELECT=0x0100, START=0x0008):

| logisch | Tabellenwert | Taste |
|---|---|---|
| 0x0400 | 0x8000 | **D-Pad LINKS** -> Cursor zurueck |
| 0x0800 | 0x2000 | **D-Pad RECHTS** -> Cursor vor |
| 0x1000 | 0x0040 | **KREUZ (X)** -> Bestaetigen |
| 0x2000 | 0x0010 | DREIECK |
| 0x4000 | 0x0020 | KREIS |

=> **Das Ja/Nein-Kaestchen ist WAAGERECHT: links/rechts bewegt, X bestaetigt, mit
Wrap-around in beide Richtungen und einer herunterzaehlenden Wiederhol-Sperre
@0x800E874A.**

### 2.5 Wie der Zweig gewaehlt wird — `ck(0x0b, 0x1f, 0)` belegt

Handler **@0x80054354** (selbst disassembliert):
```
8005435c: lbu v1,1(v0)          ; +1 = Flag-BANK               (hier 0x0b)
80054360: lhu a1,2(v0)          ; +2 = Bit-Nr, +3 = Sollwert   (hier 0x1f / 0x00)
80054370: srl v0,a1,3 / andi v0,v0,0x1c     ; Wortoffset
80054378: andi a0,a1,0x1f                    ; Bit im Wort
80054384: lw v1,30920(at)       ; Bank-Zeigertabelle @0x800A78C8 -> Bank 0x0B = 0x800E873C
80054390: lui v0,0x8000 / srlv v0,v0,a0      ; Maske = 0x80000000 >> Bit
800543a0: bne a1,zero,... ; sltu v0,zero,v1 ; xori v0,v0,1
```
Bank 0x0B = **0x800E873C** (Zeiger @0x800A78F4; im Ghidra-Dump Zeile 331280 als
`800a78f4  addr DAT_800e873c`). Bit 0x1F -> Maske `0x80000000>>31 = 1` = **Bit 0 des
Statusbytes**, und Bit 0 ist das unterste Bit des Cursor-Index, der @0x80030B64
(`andi v0,v0,0xf0 / or v0,v0,s4 / sb`) dort hineingeschrieben wird. Sollwert 0 =
"Bit ist NICHT gesetzt".

=> `21 0b 1f 00` heisst: **"die erste Option (Index 0 = Ja) wurde gewaehlt"**.
Ja -> Hebel auf 0x0640 und `var5 += 36`; Nein -> Hebel auf 0x0203 und `var5 -= 14`.

### 2.6 (c) Die Grafik

**Der Zeiger** = Objekt-Modell 2, `room2130/obj/model02.md1`, 124 Bytes, OHNE eigene TIM
(eine `model02.tim` existiert nicht). Eigene Auswertung der Bytes:
```
0x00  70 00 00 00   Laenge 0x70 (Daten 0x0C..0x7C = Dateiende)
0x08  02 00 00 00   nObj = 2 (Dreiecks- und Vierecksteil)
0x0C  38,3, 50,1, 58,1, 64     Teil 0: vtx@0x38 (3), nor@0x50 (1), tri@0x58 (1)
0x28  38,3, 50,1, 64,0, 70     Teil 1: Vierecks-Zaehler = 0
Vertices (rel. 0x0C+0x38 = Datei 0x44), s16 x,y,z,pad:
  v0 = ( 0, -18,  -7)
  v1 = ( 0, -18,   6)
  v2 = ( 0,  18,   0)
Normale (Datei 0x5C): (0x1000, 0, 0) = +X
Dreieck (Datei 0x64): 00 00 02 00 | 00 00 00 00 | 00 00 01 00
  = 3x {u16 Normal-Index, u16 Vertex-Index} -> (n0,v2),(n0,v0),(n0,v1)
```
Ein einzelnes Dreieck, 36 Einheiten hoch, 13 tief, in der Y/Z-Ebene, Normale nach +X:
eine **Pfeilspitze**, die quer zur Bewegungsrichtung (X) steht.

**Das einzige ESP des Raums** ist Nr. **0x16** (`room2130.ini`: `Esp 16 FF FF FF FF FF FF FF`;
erstes Byte von `room2130/effect.esp` = `16`). Die Textur `esp16.tim` (4256 Bytes) ist
4-bit, **256x32 Pixel** (Bildblock @Datei 0x94, `0c 10 00 00 00 00 00 00 40 00 20 00`
= Laenge 4108, VRAM (0,0), 64 Woerter breit = 256 Pixel, 32 Zeilen); der CLUT-Block
@Datei 0x08 (`8c 00 00 00 00 00 e0 01 10 00 04 00`) liegt im VRAM bei **(480,16), 16x4**,
also **vier Paletten**:

| CLUT | Charakter | Stuetzstellen (R,G,B) |
|---|---|---|
| **0** | **ROT** | 248,248,248 / 216,120,88 / **208,24,40** / 112,8,16 / 16,0,0 |
| 1 | Blau | 248,248,248 / 128,160,200 / 40,72,136 / 0,0,24 |
| 2 | Gruen | 248,248,248 / 144,232,144 / 32,160,32 / 0,8,0 |
| 3 | Gelb | 248,248,248 / 216,216,112 / 160,160,0 / 8,8,0 |

Das rote Material dieses Raums kommt also aus **CLUT 0 von ESP 0x16**.

`sce_espr_on2` (0x64, Handler @0x80056644, Laenge 16) wird im Raetsel so benutzt:
```
0184: 64 01 16 02 00 00 ba 02 c2 a0 ce f5 c2 d4 00 00   Schalter 1
02A2: 64 02 16 02 00 00 ba 02 14 a1 ce f5 c2 d4 00 00   Schalter 2
03C0: 64 03 …                        6e a1 …            Schalter 3
04DE: 64 04 …                        c8 a1 …            Schalter 4
05FC: 64 05 …                        22 a2 …            Schalter 5
0664: 64 0b 16 00 00 00 b4 0b 2c a2 88 f5 96 ce 00 00   nach "OK"
0674: 64 0c 16 00 …                  1c a3 …
0698: 64 0d 16 10 00 00 b4 0b 2c a2 88 f5 96 ce 00 00   nach "OK", Variante 0x10
06A8: 64 0e 16 10 …                  1c a3 …
```
Feldbelegung aus dem Handler (`lhu s0,2(s1)` / `lb a0,4(s1)` / `lhu 6,8,10,12,14(s1)`):
`+1` = Slot, `+2` = **ESP-Nummer (0x16)**, `+3` = Variante, `+4`/`+5` -> `FUN_80056A38`,
`+6..7` = Wert (0x02BA bzw. 0x0BB4), `+8/+10/+12` = **x/y/z**, `+14` -> High-Wort von a1.
Die fuenf Schalter-Slots liegen bei x = 0xA0C2, 0xA114, 0xA16E, 0xA1C8, 0xA222
(Abstand **82**), y = 0xF5CE, z = 0xD4C2. Aufgeraeumt werden sie mit `65 01`…`65 05`
(`sce_espr_kill2`, Laenge 2) am Anfang von sub04 (@0x0070…0x0079).

### 2.7 (d) Die Toene — Ids, Bank und Wellenform-Offset

**`se_on` (0x36), Handler @0x80056428 + Kern `Se_on` @0x8005BA28 (beide selbst disassembliert).**
Aus `36 VV IL IH DD DD XX XX YY YY ZZ ZZ` baut @0x80056518–0x80056534
`a0 = (VV<<24) | (IL<<16) | IH` und ruft `Se_on(a0, &pos)`. Dort:
```
8005ba30: srl t1,a0,24        ; t1 = VV  = EDT-GRUPPE            (hier 0x02)
8005ba64: lb v0,19528(at)     ; 0x800D4C48[t1]  (-1 => Ton verworfen)
8005ba7c: srl v0,a0,16 / andi s7,v0,0xff   ; s7 = EDT-INDEX      (hier 0x0A/0x0B/0x0C/0x0F)
8005ba8c: lw a1,-17544(at)    ; EDT-Tabellenzeiger @0x800DBB78[t1]
8005ba94: addu a1,a1,s7*4     ; -> 4-Byte-Eintrag
8005baa8: lbu v1,0(a1)        ; Byte 0, Bit 0x80 -> Art ueberschreiben
8005bad4: lbu v0,1(a1) / andi s6,v0,0x7f   ; s6 = VAB-PROGRAMM
8005bad0: lbu a0,2(a1) / srl s2,a0,4       ; s2 = VAB-TON
8005bacc: lbu v1,3(a1) / andi s1,v1,0x1f / srl s4,v1,5
8005bae8: sll v1,s6,9 / sll v0,s2,5 / addiu v0,v0,2080 / addu s0,a2,v1+v0
          ; s0 = VabHdr + 2080 + Programm*512 + Ton*32  = &VagAtr[prog][tone]
```
`2080 = 0x820 = 32 + 128*16` ist exakt der Offset der Ton-Attributtabelle im VAB-Header —
damit ist **Byte 2 >> 4 = Tonnummer** und **Byte 1 & 0x7F = Programmnummer** belegt.

**Gruppe 2 = die raumeigene EDT/VAB** — @0x80059F44 wird der EDT-Zeiger nach
`0x800DBB80` (= `0x800DBB78 + 2*4`) und @0x80059F70 der VAB-Header nach `0x800D75A8`
(= `0x800D75A0 + 2*4`) geschrieben, beides aus dem Raum-Sounddeskriptor
(`lw a1,8(a2)` = EDT, `lw a0,12(a2)` = VAB-Header).

`room2130/snd0.edt` (192 Bytes = 48 Eintraege a 4 Bytes) — belegte Eintraege:
`0x0A = 00 00 74 00`, `0x0B = 00 00 83 01`, `0x0C = 00 00 93 01`, `0x0F = 00 00 c3 00`,
`0x1A..0x1F = 00 00 43/53/63/13/23/33 16`. **Genau die vier im Skript benutzten Indizes
0x0A/0x0B/0x0C/0x0F sind belegt.**

`room2130/snd0.vh` (3104 Bytes): `VABp`, Version 7, 1 Programm, 13 Toene, **11 VAGs**.
VAG-Groessentabelle @Datei 0x0A20; die Praefixsumme ergibt (Summe = 41744 = exakt die
Groesse von `snd0.vb`):

| VAG | Offset in `snd0.vb` | Groesse |
|---|---|---|
| 1 | 0x00000 | 48 |
| 2 | 0x00030 | 1904 |
| 3 | 0x007A0 | 2560 |
| 4 | 0x011A0 | 4288 |
| 5 | 0x02260 | 3360 |
| 6 | 0x02F80 | 3296 |
| 7 | 0x03C60 | 3840 |
| 8 | 0x04B60 | 6800 |
| 9 | 0x065F0 | 7440 |
| 10 | 0x08300 | 3568 |
| 11 | 0x090F0 | 4640 |

Ton-Attribute aus `snd0.vh` (Ton -> VAG, Mittelton, Shift, Lautstaerke):
Ton 7 -> VAG 10 (84/57/127) · Ton 8 -> VAG 8 (85/57/80) · Ton 9 -> VAG 9 (86/57/100) ·
Ton 12 -> VAG 11 (89/57/100).

**Ergebnis — die vier Toene des Raetsels:**

| SCD-Bytes | Gruppe/Index | EDT-Eintrag | Prog/Ton | VAG | Datei + Offset | Rolle |
|---|---|---|---|---|---|---|
| `36 02 0a 01 00 00 9b a0 00 fc f4 d3` @0x0082/0x01A0/0x02BE/0x03DC/0x04FA | 2 / 0x0A | `00 00 74 00` | 0 / 7 | 10 | `snd0.vb` **@0x08300, 3568 B** | **KLICK beim Betaetigen eines Schalters** |
| `36 02 0b 01 …` @0x062E und @0x06F0 | 2 / 0x0B | `00 00 83 01` | 0 / 8 | 8 | `snd0.vb` **@0x04B60, 6800 B** | Fehlschlag ("too high"/"low") |
| `36 02 0c 01 …` @0x0652 | 2 / 0x0C | `00 00 93 01` | 0 / 9 | 9 | `snd0.vb` **@0x065F0, 7440 B** | **BESTAETIGUNG "Power supply OK"** |
| `36 02 0f 01 …` @0x0688 | 2 / 0x0F | `00 00 c3 00` | 0 / 12 | 11 | `snd0.vb` **@0x090F0, 4640 B** | Strom fliesst / Nachspiel |

Die Raum-VAB liegt im RDT selbst; extrahiert unter
`info/re2leon/PL0/RDT/room2130/snd0.vh` + `snd0.vb` (der VAG-Block ist reines
PSX-ADPCM, 16-Byte-Bloecke — direkt extrahierbar).

**Die UI-Toene des Auswahl-Cursors** (aus 2.4): `Se_on(0x04040000)` = **Gruppe 4, Index 4**
(Cursor bewegen), `Se_on(0x04060000)` = Gruppe 4, Index 6 (bestaetigen, Option 0),
`Se_on(0x04050000)` = Gruppe 4, Index 5 (Option 1). Gruppe 4 wird @0x80059A90
(`sw s0,-17528(at)` -> 0x800DBB88) aus einem der `COMMON/SOUND/CORE**.EDH`-Saetze geladen.
Genau diese drei Indizes sind in `CORE10/11/12/13/14.EDH` die EINZIGEN belegten Eintraege:
`CORE10/11`: 4 = `00 00 43 05` (Ton 4), 5 = `00 00 53 06` (Ton 5), 6 = `00 00 63 06` (Ton 6);
`CORE12/13/14`: 4 = Ton 5, 5 = Ton 6, 6 = Ton 7.

### 2.8 (e) Die 80

`sub04 @0x0642`, Bytes **`23 00 05 00 50 00`** = `cmp(var5, Operator 0 "==", 0x0050)`.
Operator 0 ist @0x800544CC (`xor v0,a0,a1; sltiu v0,v0,1`) belegt. Trifft er zu:
```
0648: 2b 00 07 00 ff ff   message_on(msg 7) = "Power supply OK."
064E: 22 04 3c 01         set(Bank 0x04, Bit 0x3c, 1)     <- Raetsel-geloest-Flag
0652: 36 02 0c 01 …       se_on(Gruppe 2, Index 0x0C)     <- Bestaetigungston
```
**Die 80 ist der LEISTUNGSWERT in SCD-Variable 5 (`0x800D47EC + 10`)** — keine
Bildkoordinate und kein Cursor-Index. Der Zeiger "steht auf 80", weil seine X-Koordinate
dann `-24660 + 9*80 = -23940` ist (2.3). Die uebrigen Ausgaenge:
`var5 == 100` -> msg 6 "too high" (@0x0622, `23 00 05 00 64 00`);
`var5 == 0` -> msg 9 "too low" (@0x06C6, `23 00 05 00 00 00`);
`var5 > 80` -> msg 6, sonst msg 8 "slightly low" (@0x06D8, `23 00 05 01 50 00`,
Operator 1 = ">" @0x800544D8).

Loesbarkeit, allein aus diesen Konstanten: jeder Schalter EIN = +36, AUS = -14, ein
zweimal umgelegter Schalter also netto +22; `36*a + 22*c = 80` mit a=1, c=2 —
z. B. ein(36) / ein(72) / aus(58) / ein(94) / aus(80). Die Deckel bei 100 und 0 werden
dabei nie beruehrt.

### 2.9 Die weiteren cmp-Operatoren (Sprungtabelle @0x800111A0)

| Nr | Adresse | Bedeutung |
|---|---|---|
| 0 | 0x800544CC | `var == wert` |
| 1 | 0x800544D8 | `var >  wert` |
| 2 | 0x800544E0 | `var >= wert` |
| 3 | 0x800544EC | `var <  wert` |
| 4 | 0x800544F4 | `var <= wert` |
| 5 | 0x80054500 | `var != wert` |
| 6 | 0x8005450C | `var &  wert` |

## 3. Was der Port tut — mit datei.c:zeile

(Der Port-/RE1.5-Teil gehoert einem zweiten Agenten. Was ich lesend gepruefte habe, als
Anschlusspunkte:)

* `re15_port/engine/src/msg_select_common.c` (63 Zeilen) baut das RE1.5-Ja/Nein-Kaestchen
  bereits byte-true aus DEBUG.BIN nach: waagerechter Cursor bei X 160/230
  (`SEL_CURSOR_X0 0xa0` @0x80028650, `SEL_CURSOR_DX 0x46` @0x8002863c-4c), Y 180
  (@0x80027f14), Zeilenabstand 0x10 (@0x80028630), Blinkmaske 0x18 (@0x80028600),
  Blinkzaehler-Regel (@0x800285d4 / @0x800285f0). **Es gibt in dieser Datei KEINEN
  einzigen Tonaufruf** — weder beim Bewegen noch beim Bestaetigen. Das deckt sich mit
  der Nutzermeldung "Bewegungs- und Bestaetigungssound ... grundlegend falsch".
* Die passenden Einstiegspunkte existieren schon:
  `re15_port/include/re15_audio.h:126  void re15_audio_room_se(int se_id);`
  `re15_port/include/re15_audio.h:174  void re15_audio_core_se(int se_id);`
  (CORE-Bank = die RE1.5-Entsprechung zur RE2-EDT-Gruppe 4.)

## 4. Der Unterschied, in einem Satz

In RE2 ROOM2130 haengt an JEDER Cursorbewegung ein Ton (`Se_on(0x04040000)`,
EDT-Gruppe 4/Index 4) und an der Bestaetigung ein zweiter (Index 6 fuer Option 0,
Index 5 fuer Option 1), und der rote Zeiger ist ein eigenes Raum-Objektmodell, dessen
X-Koordinate mit **9 Einheiten pro Leistungspunkt, einem Punkt pro Bild** geschoben wird,
bis der Leistungswert (SCD-Variable 5) genau **80** ist — im Port ist die Auswahlbox
stumm und ein solcher Zeiger existiert nicht.

## 5. Umsetzungsplan (fuer den ROOM11F0-/Port-Agenten)

Alles hier stammt aus §2; jede Zahl traegt ihre Adresse.

1. **Toene der Auswahlbox** (`re15_port/engine/src/msg_select_common.c`, dort wo
   `lr_edge` verarbeitet wird, Zeile 58–62):
   * bei jedem angenommenen Links/Rechts-Schritt: CORE-SE **Index 4**
     (RE2: `Se_on(0x04040000)` @0x8003096C und @0x800309A0).
   * beim Bestaetigen mit Option 0: CORE-SE **Index 6** (RE2 @0x80030950/0x80030944),
     mit Option 1: **Index 5** (RE2 @0x8003093C).
   * Der RE1.5-Gegenwert der CORE-Indizes ist NICHT Teil dieses Dossiers (s. §6) — der
     RE1.5-Agent muss die RE1.5-CORE-EDT an derselben Stelle nachlesen, bevor eine Zahl
     in den Code geht.
2. **Klick des Raetsels selbst**: in RE2 ist das KEIN UI-Ton, sondern ein **Raum-SE**:
   `se_on(Gruppe 2, Index 0x0A)` direkt vor der Abfrage (sub04 @0x0082, absolut
   `ROOM2130.RDT` @0x01192). Wer den RE2-Klang wirklich will, extrahiert
   `snd0.vb` **@0x08300, 3568 Bytes** (VAG 10, Ton 7, center 84, shift 57, vol 127) aus
   ROOM2130.RDT; der Bestaetigungston ist **@0x065F0, 7440 Bytes** (VAG 9, Ton 9),
   der Fehlschlagton **@0x04B60, 6800 Bytes** (VAG 8, Ton 8).
   Beide Dateien liegen bereits entpackt unter
   `info/re2leon/PL0/RDT/room2130/snd0.vh` + `snd0.vb`.
3. **Roter Zeiger**: als eigenes Raum-Objektmodell fuehren, nicht als HUD-Sprite —
   im Original ist es Objekt 2 mit 3 Vertices und 1 Dreieck
   ((0,-18,-7)/(0,-18,6)/(0,18,0), Normale +X). Die rote Farbe des Raums liegt in
   **CLUT 0 von ESP 0x16** (`esp16.tim`, VRAM-CLUT (480,16)).
4. **Bewegung**: `x = x0 + 9 * wert`, pro Bild genau EIN Wert-Schritt
   (`26 00 00 10 09 00` @0x00EC bzw. `26 00 01 10 09 00` @0x0164, `evt_next` @0x0106).
   Kein Pad-Polling, kein Wrap.
5. **Zielwert**: die Gleichheitsabfrage `cmp(var5, "==", 80)` (sub04 @0x0642, absolut
   `ROOM2130.RDT` @0x01752, Bytes `23 00 05 00 50 00`). Die Randfaelle 100 / 0 / >80
   (siehe 2.8) gehoeren mit uebernommen, sonst fehlen drei der vier Rueckmeldungen.
6. **Auswahllogik**: Optionszahl aus dem Layoutsatz (RE2 @0x8009DEF6 Byte 0 = 2),
   Cursor-Index in den unteren 4 Bit eines Statusbytes, Wrap in beide Richtungen,
   herunterzaehlende Wiederhol-Sperre. Im Port ist das Gegenstueck
   `msg_select_common.c` + die RE1.5-Adressen, die dort schon stehen.

## 6. Offen / nicht belegt

* **Welcher `CORE**.EDH`-Satz in RE2 tatsaechlich als Gruppe 4 geladen wird**, haengt von
  `s3` am Ladeaufruf @0x80059A90 ab (Dateikennung aus der Tabelle @0x800A80B8). Die
  Kandidaten `CORE10/11/12/13/14.EDH` haben alle GENAU die Indizes 4/5/6 belegt — welcher
  fuer Stage 2 gilt, habe ich nicht aufgeloest. Naechster Weg: die Tabelle @0x800A80B8
  und den Loader `FUN_80012FB8` ueber die Datei-Kennungen der RE2-TOC aufloesen.
* **Die RE1.5-Gegenstuecke der CORE-Indizes 4/5/6** habe ich bewusst NICHT geraten —
  das ist der Teil des zweiten Agenten (RE1.5 `re15_audio_core_se`).
* **`sce_espr_on2`-Feld `+3`** (Werte 0x02 / 0x00 / 0x10) habe ich als "Variante" gelesen,
  aber NICHT bis zur CLUT-Auswahl in `FUN_8001C8C4` durchdisassembliert. Ob die fuenf
  Schalterlampen also rot (CLUT 0) oder eine der anderen drei Paletten benutzen, ist
  damit **nicht belegt**. Belegt ist nur: der Raum hat genau ein ESP (0x16) und dessen
  CLUT 0 ist rot. Naechster Weg: `FUN_8001C8C4` (Sce_espr_on) und das ESP-Kopfformat
  (`effect.esp` Bytes 0x08..0x6F, Spritetabelle @0x70: `0000 f0f0 / 2000 f0f0 / 4000 f0f0
  / 6000 f0f0 / 8000 f0f0 / a000 f4f4 / b800 f0f0 / d800 f0f0`).
* **Die Farbe des Zeigers (Objekt 2)** ist in der MD1 NICHT enthalten — kein Textur- und
  kein Farbfeld, nur Vertex-/Normalindizes. Welche Farbe der RE2-Renderer einem
  unstrukturierten Raum-Objektmodell gibt, habe ich nicht disassembliert. **Dass der
  Zeiger rot ist, ist damit NICHT belegt** — nur, dass die rote Palette des Raums in
  ESP 0x16 CLUT 0 liegt. Naechster Weg: der Obj_model-Zeichner in der RE2-EXE
  (Einstieg ueber `work_set(4,n)` / Handler @0x80055904).
* **Welche AOT das Raetsel startet**: in sub03 steht @0x005C
  `2c 03 05 31 00 00 f8 9e 14 d3 6c 07 0e 06 ff 00 18 04 00 00` (aot_set, 20 Bytes) und
  @0x008C die Parallelstelle mit `… ff 00 18 05 00 00`. Die `04`/`05` an derselben
  Byte-Position deuten auf die Sub-Nummer; den `aot_set`-Handler (@0x800558xx) habe ich
  dafuer NICHT gelesen — also Hinweis, kein Beleg.
* Die Zuordnung der drei Pad-Konfigurationen zu den Optionsmenue-Eintraegen
  (@0x800A26A0/C0/E0; Konfig 1 weicht nur bei logisch 0x2000 ab: 0x0090 statt 0x0010)
  ist nicht weiter verfolgt.
