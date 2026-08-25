# ROOM1090 — der LOESCH-VORGANG ("nachdem man mit dem ??? die Flammen ausgemacht hat")

Status: **IN ARBEIT** (wird inkrementell fortgeschrieben)

## 0. Das fehlende Wort ist BELEGT: **FEUERLOESCHER (Fire Extinguisher)**

Nicht geraten — der Raum sagt es selbst. `ROOM1090.RDT` MSG-Sektion
(`tbl[0x3C] = 0x00002748`, 10 Eintraege, Pointer-Tabelle @0x2748):

| msg | Datei-Offset | Bytes (Anfang) | Text (RE1.5-Encoding, `A`=0x1D.., `a`=0x3D..) |
|-----|--------------|----------------|------|
| 07 | 0x28E6 | `04 02 25 00 49 51 4f 50 …` | „I must hurry up and get something to put out this fire to save that woman!" |
| 08 | 0x2934 | `04 02 33 45 48 48 00 55 4b 51 00 51 4f 41 00 50 44 41 00 05 01 22 45 4e 41 00 21 54 50 45 4a 43 51 45 4f 44 41 4e 05 00 1b 03 02 01 00` | „Will you use the **Fire Extinguisher** ?" + Y/N-Terminator `1b 03 02 01` |
| 09 | 0x2961 | `04 02 35 4b 51 3a 52 41 00 51 4f 41 40 …` | „You've used the **Fire Extinguisher**." |

Die Y/N-Terminierung `1B 03 02 …` in msg08 ist die dokumentierte Yes/No-Form
(RE15_KNOWLEDGE.md §1.11).

---

## 1. Werkzeug + Laengen-Korrektur (Voraussetzung fuer alles Folgende)

Der Walker `analysis/nutzer_batch_2026-08-27/tools/re15_scd_walk.py` benutzt die
byte-true Laengentabelle. **Drei Opcodes sind in RE1.5 VARIABEL lang** — das hat der
bisherige Port-Tabellenwert (`re15_port/engine/src/scd_vm.c:166`) NICHT:

| Op | Handler | Instruktionen | Laenge |
|----|---------|---------------|--------|
| 0x2C `Aot_set` | 0x80040534 | `@0x80040590 lbu v0,3(v1)` / `@0x80040598 andi v0,v0,0x80` / `@0x8004059c beq` / `@0x800405a8 addiu v0,v1,28` / `@0x800405ac addiu v0,v1,20` / `@0x800405b0 sw v0,28(a0)` | **28** wenn `pc[3]&0x80`, sonst 20 |
| 0x3B `Door_aot_set` | 0x800405bc | `@0x80040618 lbu v0,3(v1)` / `@0x80040620 andi 0x80` / `@0x80040630 addiu v0,v1,40` / `@0x80040634 addiu v0,v1,32` | **40** / 32 |
| 0x50 `Item_aot_set` | 0x80040644 | `@0x8004065c lbu v0,3(a2)` / `@0x80040664 andi 0x80` / `@0x8004066c addiu v0,a2,30` / `@0x80040688 addiu v0,a2,22` | **30** / 22 |

Ohne diese Fallunterscheidung desynchronisiert der Walker in ROOM1090 sub00 genau an
den zwei entscheidenden `Aot_set` (`sat = 0xB1`, Bit 0x80 gesetzt → 28 Byte) — deshalb
war der Loesch-Trigger bisher nicht sichtbar.

Ableitungswerkzeug: `analysis/nutzer_batch_2026-08-27/tools/re15_scd_oplen.py`
(symbolische Ausfuehrung aller 95 Handler der Dispatch-Tabelle @0x800744a8).

`Ck`/`Set`-Operanden (Handler 0x21 @0x8003fcf4):
`[op][bank=pc[1]][u16 = bit_index | (expected<<8)]`; Bank-Zeiger-Tabelle @0x80074664;
Wort = `(u16>>3)&0x1c` (@0x8003fd10/14), Bit = `0x80000000 >> (u16&0x1f)` (@0x8003fd38/40),
Ergebnis `XOR (expected==0)` (@0x8003fd30/50).

## 2. Die Kette — von der Item-Aufnahme bis zum Loeschen

### 2a. ROOM1000: Feuerloescher aufnehmen -> Flag Bank 3 / Bit 0x85

`ROOM1000.RDT`, Datei-Offset **0x000C24**:
```
50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 | 31 00 | 01 00 | 86 00 | ff 00
Item_aot_set  aot=3  sce=0x09(ITEM)  sat=0x31  x=20500 z=-800 w=800 d=800
              item_id = 0x0031  amount = 1   taken_bit = 0x0086  prop = 0xFF
```
`item_id` liegt bei **0x000C32** (= 0x31 Feuerloescher). Die „taken"-Bit-Nummer 0x86 wird
vom Handler @0x80040670 (`lhu a1,26(a2)`) bzw. @0x80040680 (`lhu a1,18(a2)`) gelesen;
die Bank ist fest **Bank 9** (`@0x800406d4 lui a0,0x8007` / `@0x800406d8 lw a0,18056(a0)`
= 0x80074688 = Bank-Tabelle[9] = 0x800b1078).

`ROOM1000.RDT` @0x000CFC-0x000D14 (subScd):
```
0x000CFC  Ifel_ck  skip=20
0x000D00  Ck   bank=9 bit=0x86 val=1      ; Feuerloescher aufgenommen?
0x000D04  Ifel_ck  skip=10
0x000D08  Ck   bank=3 bit=0x85 val=0      ; noch nicht gemerkt?
0x000D0C  Set  bank=3 bit=0x85 val=1      ; <-- „Spieler hat den Feuerloescher"
0x000D10  Endif
0x000D12  Endif
```

### 2b. ROOM1090 sub00 waehlt daraufhin den AOT-Typ (Datei 0x2306-0x2372)

```
0x002306  Ifel_ck  skip=36
0x00230A  Ck    bank=3 bit=0x85 val=0            ; KEIN Feuerloescher
0x00230E  Aot_set aot=2 sce=0x01 sat=0xB1 (28B)  ; MESSAGE, data0 = 0x0007
0x00232A  Else_ck skip=76
0x00232E  Ifel_ck skip=66
0x002332  Ck    bank=3 bit=0x81 val=0            ; Feuer brennt noch
0x002336  Aot_set aot=2 sce=0x03 sat=0xB1 (28B)  ; EVENT, data = ff 00 18 06 00 00
0x002352  Door_aot_set aot=3 sce=0x02 sat=0x31 (32B)
0x002372  Endif
```
* `sce=0x01` Handler @0x80043084: `lhu a2,0(v0)` = msg-Index, `jal 0x80027e68` →
  zeigt **msg 07** = „I must hurry up and get something to put out this fire…"
* `sce=0x03` Handler @0x800430f0: `lhu a0,0(v0)` (=0x00FF) / `lbu a1,3(v0)` (=0x06) /
  `jal 0x8003ee3c` → startet **subScd 06**.

### 2c. sub06 (Datei 0x2702-0x272A) — DAS ist der Loesch-Vorgang

```
0x002702  Message_on  msg=0x07  ff ff
0x002706  Evt_next
0x002708  Message_on  msg=0x08  ff ff     ; „Will you use the Fire Extinguisher ?" (Y/N)
0x00270C  Evt_next
0x00270E  Ifel_ck  skip=24
0x002712  Ck   bank=12 bit=0x1f val=0     ; Antwort = YES
0x002716  Set  bank=2  bit=0x07 val=1     ; Spieler-/Skript-Sperre
0x00271A  Sleep 10
0x00271E  Set  bank=3  bit=0x81 val=1     ; <<<< FEUER AUS
0x002722  Set  bank=3  bit=0x84 val=1     ; <<<< Rettungs-Cutscene scharf
0x002726  Aot_on aot=3                    ; <<<< feuert den Tuer-AOT SOFORT
0x002728  Endif
0x00272A  Evt_end
```
**Bank 12 = `DAT_800b8520`** (Bank-Tabelle @0x80074694 → 0x800b8520). Bit 0x1F ist bei
MSB-erster Zaehlung (`0x80000000 >> 31`) das **Bit 0 des Bytes** = der YES/NO-Cursor
(0 = Yes). Gleiche Adresse/Bedeutung wie im Port dokumentiert
(`re15_port/engine/src/item_modal_common.c:220` „default Yes (DAT_800b8520 bit0 = 0)").
