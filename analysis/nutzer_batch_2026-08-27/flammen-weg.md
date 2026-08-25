# ROOM1090 — der LOESCH-VORGANG ("nachdem man mit dem ??? die Flammen ausgemacht hat")

Status: **GELOEST** — Ausloeser, Flag-Kette, Wirkungs-Kette und der eine Port-Blocker sind belegt.

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

## 3. `Aot_on 3` IST der Loesch-Moment — es feuert eine SELBST-TUER

`Aot_on` (0x47) ist **kein "Trigger aktivieren"**, sondern ein SOFORT-FEUERN.
Handler LAB_800407bc:
```
800407cc  lw  v0,28(a0)          ; pc
800407d4  lbu v0,1(v0)           ; slot
800407ec  lw  v1,0(at)           ; DAT_800ac9b0[slot] = gespeicherter Record (= pc+2)
800407f4  lbu v0,1(v1)           ; rec[1] = sat
800407fc  andi v0,v0,0x80
80040804  addiu a0,v1,20         ; Payload bei rec+20 (Langform)
80040808  addiu a0,v1,12         ; Payload bei rec+12 (Kurzform)
8004080c  lbu v0,0(v1)           ; rec[0] = sce
80040824  lw  v0,0(at)           ; PTR_8007469c[sce]
8004082c  jalr v0                ; <<-- sce-Handler laeuft SOFORT, ohne Geometrie-Test
8004083c  addiu v1,v1,2          ; pc += 2
```

Slot 3 traegt zu diesem Zeitpunkt den **Door**-Record: `main00` @0x215A installiert ihn als
`Aot_set 3 sce=0x03` (20 B), `sub00` @0x2352 ueberschreibt denselben Slot mit
`Door_aot_set 3 sce=0x02` (32 B, sat=0x31 -> Kurzform -> Payload = pc+14):

```
Datei 0x2352:  3b 03 02 31 | 00 00 | 00 00 00 00 00 00 00 00 |
               e4 04  f8 f8  1f f6  b5 0f  00  09  06  01  00 00 00 00 00 00
Payload(+0)  X     = 0x04E4 =  1252
Payload(+2)  Y     = 0xF8F8 = -1800
Payload(+4)  Z     = 0xF61F = -2529
Payload(+6)  Yaw   = 0x0FB5 =  4021
Payload(+8)  STAGE = 0x00
Payload(+9)  ROOM  = 0x09      <<-- ROOM1090 = DERSELBE RAUM
Payload(+A)  CUT   = 0x06
Payload(+B)  Band  = 0x01
```

sce-2-Handler @0x800430bc bewaffnet die Transition:
```
800430bc  ori v0,zero,0x1
800430c4  sw  a0,-13912(at)      ; DAT_800ac9a8 = Payload-Zeiger
800430d4  sb  v0,21337(at)       ; DAT_800b5359 = 1  (Tuer haengt an)
800430dc  lui v1,0xff00
800430e0  or  v0,v0,v1           ; DAT_800aca40 |= 0xFF000000  (Transitions-Freeze)
```

**Der Raumlader kennt KEINEN "gleicher Raum"-Sonderfall.** Warp FUN_8001d600, Tuer-Zweig:
```
8001d94c  lbu v0,9(a0)           ; ZIEL-ROOM
8001d95c  sh  v0,4066(at)        ; -> work_vars[0x09]
8001d930  lbu v1,10(a0)          ; ZIEL-CUT
8001d948  sh  v1,4068(at)        ; -> work_vars[0x0A]
8001d960  lbu v0,8(a0)           ; ZIEL-STAGE
8001d968  beq v1,v0,0x8001d988   ; NUR die STAGE wird verglichen
8001d980  jal 0x80039a30         ; (nur bei Stage-Wechsel) Overlay nachladen
8001d988  jal 0x800396fc         ; <<-- RAUM LADEN — UNBEDINGT
```
`FUN_800396fc` ist der Raumlader (RE15_FUN_CATALOG.md Zeile 169: "Room sprite-subsystem
orchestrator ... From room loader FUN_8001d600"); er zieht die SCD-Raum-Init FUN_8003ef6c
nach, also **main00 + sub00 laufen erneut** — jetzt mit `flag(3,0x81) == 1`.

## 4. Was danach genau verschwindet — jedes Teil einzeln belegt

### 4a. Die sieben Entities vom Typ 0x26 — nicht deaktiviert, sondern NIE WIEDER GESPAWNT

`sub00` Datei 0x21EA-0x22A0:
```
0x0021EA  Ifel_ck  skip=180
0x0021EE  Ck  bank=3 bit=0x81 val=0
0x0021F2  Obj_model_set  slot=3   (34 B)
0x002214  44 00 26 00 ...   0x002228  44 01 26 01 ...   0x00223C  44 02 26 02 ...
0x002250  44 03 26 04 ...   0x002264  44 04 26 03 ...   0x002278  44 05 26 03 ...
0x00228C  44 06 26 04 ...
0x0022A0  Endif
```
Nach dem Reload ist `Ck(3,0x81,0)` FALSCH -> der Sprung ueberspringt exakt diese 180 Byte
(Ziel = 0x0021EE + 0xB4 = 0x0022A2, direkt hinter dem Endif). Kein Spawn, kein Obj-Modell 3.

Layout der Spawn-Zeile (Port `op_sce_em_set`, byte-true): `pc[1]` = Slot, `pc[2]` = Typ,
`pc[3]` = Variante -> `entity+0x9` (grid_id), `pc[8..9]` X, `pc[10..11]` Y, `pc[12..13]` Z,
`pc[16..17]` Yaw (alle LE). Die sieben: Varianten 0,1,2,4,3,3,4; Y durchgehend -1800;
Yaw durchgehend 0x0400.

**Gegenprobe, dass es keinen anderen Weg gibt:** ein Scan ueber das gesamte Typ-0x26-Gehirn
(0x80116288-0x80116E60, STAGE1.BIN) findet **keinen einzigen Zugriff auf Flag-Bank 3
(0x800b0ff8)**. Die einzigen `lui 0x800b`-Ziele dort sind 0x800AC784 (aktuelle Entity),
0x800ACA40 (Pause-Wort), 0x800ACA58/59/5A + 0x800ACAE7/EE (Spieler-Register) und
0x800B1028 (Bank 5, nur Adressrechnung). Das Gehirn KANN das Feuer-Flag also gar nicht lesen —
das Loeschen ist ausschliesslich eine Spawn-Frage.

### 4b. Das Raum-Objektmodell — Modell-Tausch an IDENTISCHER Position

`Obj_model_set`-Handler LAB_80040914: `lbu a3,1(a2)` @0x8004093c -> Slot;
`a1 = 0x800b3f98 + slot*148` (@0x80040944-58, Stride via `sll/addu`-Kette 8+1, *4, +1, *4).
Die beiden Rohzeilen sind **byte-identisch bis auf das Slot-Byte**:
```
brennend  (Ck 3,0x81==0)  @0x21F2: 2d 03 00 00 02 00 08 00 10 00 f4 0b 3e f5 e8 f9 50 00 56 07 00...
geloescht (Ck 3,0x84==1)  @0x22AA: 2d 02 00 00 02 00 08 00 10 00 f4 0b 3e f5 e8 f9 50 00 56 07 00...
                                       ^^ Slot 3 -> Slot 2, alles andere gleich
Transform: (0x0BF4, 0xF53E, 0xF9E8) = (3060, -2754, -1560)
```
(`nOmodel = 4`, RDT-Header Byte 0x02; Slots 0/1 setzt main00 @0x216E/0x2190.)
Ein drittes Vorkommen @0x237E setzt Slot 2 an eine ANDERE Stelle
(0x096A, 0xF5A3, 0xFF2E) und haengt am spaeteren Flag `Ck(3,0xBB,1)`.

### 4c. Die Schadenszone — sie sitzt IM Emitter, nicht in einem AOT

ROOM1090 installiert **keinen** Schadens-AOT: die einzigen `Aot_set`/`Door_aot_set`/
`Item_aot_set` des Raums tragen sce 0x01 (Message), 0x02 (Door) und 0x03 (Event); es gibt
kein `Col_chg_set` (0x45), kein `Super_set` (0x48), kein `Sce_espr_on` (0x3A) und kein
`Se_on` (0x36) im ganzen Raum (vollstaendiger Walk von main00 + 8 sub + 2 extra).

Der Schaden liegt in der Typ-0x26-Wurzel selbst, direkt hinter dem Zustandsdispatch:
```
80116320  lbu  v0,464(a0)        ; entity+0x1D0 (Flammen-Budget/Phase)
80116328  sltiu v0,v0,0xd        ; < 13 ?
8011632c  bne  v0,zero,0x801163dc ;   ja -> KEIN Kontakt (Flamme noch nicht "gross")
8011633c  and  v0,v0,-3          ; word0 &= ~0x02
80116358  and  v0,v0,-65         ; word0 &= ~0x40
80116368  jal  0x8002aec4        ; Kontakt-Test (a0 = Spielerblock 0x800ACA54, a1 = Entity)
80116370  beq  v0,zero,0x80116408 ; kein Kontakt -> raus
8011637c  lbu  v0,-13593(v0)     ; Spieler+0x93 (hit_react)
80116384  bne  v0,zero,0x801163b0 ;   reagiert schon -> nur Schaden
80116390  sb   v0,-13736(at)     ; DAT_800ACA58 = 2      (Spieler-Routine)
80116394  jal  0x8001a780        ; Ausrichtung
8011639c  addiu v0,v0,2
801163a4  sb   v0,-13735(at)     ; DAT_800ACA59 = Ausrichtung+2 (Sub-Routine)
801163ac  sh   zero,-13734(at)   ; DAT_800ACA5A = 0
801163b4  lh   v0,-13586(v0)     ; Spieler-HP (+0x9A)
801163c0  slti v0,v0,4
801163c4  bne  v0,zero,0x80116408 ;   HP < 4 -> KEIN weiterer Abzug
801163c8  addiu v0,v1,-2
801163d0  sh   v0,-13586(at)     ; HP -= 2
```
-> **2 HP pro Kontaktbild, nur solange `entity+0x1D0 >= 13`, Untergrenze HP < 4.**
Weil das an der Entity haengt, faellt die Schadenszone mit dem ausbleibenden Spawn (4a)
automatisch weg. **Niemand "schaltet sie ab" — sie existiert danach nicht mehr.**

### 4d. Die ESP-Sprites — Effekt-Id 0x08 / 0x10, ebenfalls rein spawn-gebunden

Die Flamme wird nur aus dem Emitter heraus erzeugt:
`FUN_80116d00` @0x80116d84 `jal 0x80019700` mit
`a0 = (phase<<8) | (id<<24) | (3<<16)` (@0x80116d7c-80), Id aus der Varianten-Sprungtabelle
@0x80100364 (gemessen: `[0]=0x80116D44, [1]=0x80116D5C, [2]=0x80116D5C, [3]=0x80116D44,
[4]=0x80116D5C` -> Id 0x08 CORE00 fuer Varianten 0/3, Id 0x10 Raum-Bank fuer 1/2/4;
`sltiu v0,v1,0x5` @0x80116d1c schneidet Varianten >= 5 ab).
Der Emerge-Funke: @0x801166e4 `jal 0x80019700` mit `a0 = 0x09031800`.
Es gibt in ROOM1090 **kein `Sce_espr_kill` (0x4C)** — die Sprites sterben mit dem
Raum-Reload (Effekt-/Model-Instanz-Pool DAT_800a73b8, 96 x 132 Byte, Stride @0x80019da4
`addiu v1,v1,132`; im Port `re15_esp_fx_reset()` im Raum-Teardown,
`re15_port/platform/pc/src/room_pc.c:130`).

### 4e. Der Sound — das Feuer-BGM-Programm laeuft nur im brennenden Zweig

```
0x0022E6  Ifel_ck skip=28
0x0022EA  Ck bank=3 bit=0x81 val=0
0x0022EE  54 00 00 01 78 33     ; Sce_bgm_control
0x0022F4  Ifel_ck skip=12
0x0022F8  Ck bank=3 bit=0x80 val=1
0x0022FC  54 00 01 00 00 00
0x002302  Endif
0x002304  Endif
```
Handler LAB_80042998 packt FUENF Operandenbytes:
`a0 = (pc[1]<<28) | (pc[2]<<24) | (pc[3]<<16) | (pc[4]<<8) | pc[5]` (@0x800429b4-e8),
dann `jal 0x80044da4`; dort `srl v0,a0,24` @0x80044dc0 + `andi v1,v0,0xf` @0x80044dcc =
Kommando, Sprungtabelle @0x80010e58 mit 6 Eintraegen
(`[0]=0x80044f28 [1]=0x80044e00 [2]=0x80044e50 [3]=0x80044e88 [4]=0x80044ee8 [5]=0x80044f20`),
`sltiu v0,v1,0x6` @0x80044dd0 als Bereichspruefung.
Damit: Slot `pc[1]`=0 / Kommando `pc[2]`=0 / Part `pc[3]`=1 / `pc[4]`=0x78 / `pc[5]`=0x33.
**Nach dem Reload ist der Zweig tot — das Programm wird gar nicht erst gestartet.** Ein
expliziter Stopper existiert nicht und wird nicht gebraucht; `sub03` setzt danach seinerseits
`54 00 00 01 01 41` (@0x24DA) + `54 00 02 00 00 00` (@0x24E0).

### 4f. Der BSS-Hintergrund — ROOM1090 hat ZWEI vollstaendige Kamera-Haelften

`ROOM1090.RDT` Header Byte 0x01: `nCut = 16`. Die RVD-Zonentabelle (`RDT+0x28 = 0x0280`,
20 Byte/Eintrag, 41 Eintraege bis zum Terminator @0x05B4, Layout laut
`re15_port/engine/src/rdt_common.c:107-125`: `[marker u16][camFrom][camTo][4x(x,z) s16]`)
traegt `camFrom` **0..15**. Gemessen ueber alle 41 Eintraege:
**jeder Uebergang bleibt in seiner Haelfte** — in allen Zeilen mit `camFrom <= 7` ist jedes
`camTo <= 7`, in allen Zeilen mit `camFrom >= 8` ist jedes `camTo >= 8`
(bzw. `camTo = 0` = Gruppenkopf). Die beiden Haelften sind im Kamera-Graph disjunkt.
Paare (+8): 0->1 / 8->9, 0->3 / 8->11, 1->2 / 9->10, 2->1 / 10->9, 3->4 / 11->12,
4->3 / 12->11, 4->5 / 12->13, 5->4 / 13->12, 5->7 / 13->15, 7->5 / 15->13.

Umgeschaltet wird ausschliesslich beim Betreten, im brennenden Zweig:
```
0x0021C4  Ifel_ck skip=34
0x0021C8  Ck bank=3 bit=0x81 val=0
0x0021CC  Switch  work_vars[0x0A]      ; = der aktive Kamera-Cut (@0x800402fc `sh a0,4068(at)`)
0x0021D0  Case 0  -> 0x0021D6 Cut_chg  8    ; 0 + 8
0x0021DA  Case 3  -> 0x0021E0 Cut_chg 11    ; 3 + 8
0x0021E4  Eswitch
0x0021E6  Cut_auto 1
0x0021E8  Endif
```
`work_vars[0x0A]` ist der Eintritts-Cut der durchschrittenen Tuer
(`@0x8001d930 lbu v1,10(a0)` / `@0x8001d948 sh v1,4068(at)`).
-> **Solange es brennt, wird der Eintritt um +8 auf die Feuerschein-Haelfte gehoben;
die Selbst-Tuer bringt den Spieler danach auf Cut 6, also in die normale Haelfte,
und der Switch-Block wird ohnehin nicht mehr ausgefuehrt.** Das ist der "Hintergrund ohne Feuer".

### 4g. Und was dazu KOMMT: die Rettungs-Cutscene

`Set(3,0x84,1)` in sub06 schaltet im neu gelaufenen sub00 den Zweig @0x22A2-0x22E4 frei:
Obj-Modell Slot 2 (4b), `Sce_em_set` Typ 0x40 (NPC) @0x22CC, `Evt_exec ff 18 03` @0x22E0
-> **sub03**. sub03 @0x24CE beginnt mit `Set(3,0x84,0)` + `Set(3,0xBB,1)` + `Set(3,0x6E,1)`
und zeigt @0x2502 `Message_on msg=0x09` = "You've used the Fire Extinguisher." — der Text
schliesst den Kreis zu Teil 1 §0.

## 5. Zusammenfassung der Kette (alles belegt)

```
ROOM1000 @0x0C24  Item_aot_set item 0x31 "Fire Extinguisher", taken-bit Bank9/0x86
ROOM1000 @0x0D0C  Ck(9,0x86,1) -> Set(3,0x85,1)
ROOM1090 @0x230A  Ck(3,0x85,0)? ja  -> AOT2 = sce1 MESSAGE msg07
                                nein-> Ck(3,0x81,0)? ja -> AOT2 = sce3 EVENT sub06
                                                          + AOT3 = Door(SELF, cut 6)
ROOM1090 @0x2702  sub06: msg07, msg08 "Will you use the Fire Extinguisher?" (Y/N)
                        Ck(12,0x1F,0) = YES
                        Set(2,0x07,1) / Sleep 10 / Set(3,0x81,1) / Set(3,0x84,1)
                        Aot_on 3  ->  sce2-Handler 0x800430bc  ->  FUN_8001d600
                                  ->  FUN_800396fc  =  RAUM NEU LADEN (Cut 6)
neuer sub00-Lauf: Feuer-Block uebersprungen (7 Emitter + Obj-Slot 3 + Feuer-Cut + Feuer-BGM),
                  0x84-Block laeuft: Obj-Slot 2 + NPC 0x40 + sub03 (Rettung, msg09)
```

## 6. Was der PORT heute tut — der EINE Blocker

Alles Vorgelagerte ist im Port bereits byte-true:
* `op_aot_set` schiebt konditional 28/20 (`scd_vm.c`, Kommentar
  "PC-Vorschub konditional pc[3]&0x80: 28 vs 20"), Payload-Basis pc+22 in der Langform.
* `op_aot_on` (`scd_vm.c:3669`) ruft `re15_aot_fire_slot()` = SOFORT-Feuern.
* `re15_aot_fire_slot()` -> `case RE15_AOT_TYPE_DOOR: aot_fire_door(slot)`
  (`aot_common.c:618-627`).
* Die Y/N-Antwort ist verdrahtet: `msg_common.c:518`
  `re15_game_flag_set(12, 31, g_scd.message_choice)` — genau das, was `Ck(12,0x1F,0)` liest.

**Der Blocker sitzt in `aot_fire_door()`, `re15_port/engine/src/aot_common.c:518-524`:**

```c
unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                 | ((unsigned)d->dest_room << 4)
                 | (g_current_room_id & 0x000Fu);
if (dest_id != g_current_room_id) {
    re15_room_request_change(dest_id, ...);   /* voller Raum-Reload */
    a->was_inside = 1;
    return 1;
}
/* ...sonst: NUR Kamera-Cut + Spielerposition setzen  */
```

Fuer ROOM1090-AOT-3 gilt `dest_stage=0, dest_room=9` ->
`dest_id = (1<<12)|(9<<4)|0 = 0x1090 == g_current_room_id`.
Der Port faellt also in den **Selbst-Raum-Teleport**: er setzt `g_scd.cam_id = 6` und
verschiebt den Spieler nach (1252,-1800,-2529) — und **laedt den Raum NICHT neu**.
Damit passiert nichts von 4a-4g:
* die 7 Typ-0x26-Aktoren leben weiter (sub00 laeuft nie erneut) -> **Flammen bleiben**
* Obj-Slot 3 bleibt stehen, Slot 2 kommt nie
* die Kontakt-Schadenszone (4c) bleibt aktiv
* der ESP-Pool wird nicht geleert (`re15_esp_fx_reset()` haengt am Cross-Room-Teardown,
  `room_pc.c:130`) -> **Flammen-Sprites bleiben**
* das Feuer-BGM-Programm laeuft weiter
* die Rettungs-Cutscene sub03 startet nie -> flag(3,0x84) bleibt gesetzt

Das ist exakt der Nutzer-Befund "nachdem man die Flammen ausgemacht hat, muss der Flammen-
Effekt weg sein".

**Das Original hat diese Fallunterscheidung NICHT** (§3: `@0x8001d968` vergleicht nur die
STAGE, `@0x8001d988 jal 0x800396fc` laeuft unbedingt). Die Bedingung ist eine Port-Erfindung.

### Reichweite (gezaehlt, nicht geschaetzt)

Voll-Zensus ueber alle 240 ausgelieferten RDTs mit dem korrigierten Walker:
**649 `Door_aot_set`-Records, davon 67 mit `dest_id == room_id`** (Selbst-Tueren):
ROOM1031(1), **ROOM1090(1)**, 1110/1111(je 4), 1170(3)/1171(2), 1190/1191(je 1),
11A0/11A1(je 2), 2040/2041(je 2), 20A0/20A1(je 1), 30E0(1), 4000(1),
4050/4051(je 12), 40A0/40A1(je 1), 5090/5091(je 4), 6030/6031(je 2).
Alle 67 laden im Original den Raum neu.

## 7. FIX-REZEPT

### F1 (der eigentliche Fix) — `re15_port/engine/src/aot_common.c`, `aot_fire_door()`

Den `dest_id != g_current_room_id`-Zweig entfernen: **jede** Door-Fire geht ueber
`re15_room_request_change(dest_id, d->spawn_x, d->spawn_y, d->spawn_z, d->spawn_yaw_4096,
(int)d->target_cut)`.
Beleg fuer die Konstante/Reihenfolge: FUN_8001d600 `@0x8001d94c/95c` (Room),
`@0x8001d930/948` (Cut), `@0x8001d960/968` (nur STAGE-Vergleich),
`@0x8001d988 jal 0x800396fc` (Raumlader, unbedingt).
Der bestehende Cross-Room-Pfad tut bereits genau das Richtige:
`room_common.c:256 re15_actor_init()`, `:303 re15_enemy_reset()`,
`room_pc.c:130 re15_esp_fx_reset()`, `room_common.c:347 scd_room_reenter(..., target_cut)`
— und `scd_room_reenter` laesst die Flag-Baenke stehen (`scd_room_setup.c:106`), was
Voraussetzung dafuer ist, dass `flag(3,0x81)/`flag(3,0x84)` den neuen sub00-Lauf steuern.

Der bestehende Guard `spawn==(0,0,0) -> return 0` bleibt unberuehrt (ROOM1090-AOT-3 hat
Spawn (1252,-1800,-2529)).

### F2 (Absicherung) — Regressionsnetz fuer die 67 Selbst-Tueren

F1 aendert das Verhalten in 22 Raeumen. Vor dem Commit die vorhandenen Selbst-Tuer-Tests
laufen lassen bzw. erweitern: `probe_1090_gate_selfdoor.c` (existiert und beschreibt genau
diese Kette), `probe_door_1170.c`, `probe_elevator_1080.c`, `test_door_traversal.c`.
Neu zu pinnen: nach `Aot_on 3` in ROOM1090 muss gelten
`Anzahl aktiver Typ-0x26-Aktoren == 0`, `ESP-FX mit Id 0x08/0x10 == 0`,
`work_vars[0x0A] == 6`, `flag(3,0x84) == 0` (weil sub03 es @0x24CE selbst loescht).

### F3 (Werkzeug-Korrektur) — variable Opcode-Laengen dokumentieren

`re15_port/engine/src/scd_vm.c:166` `s_opcode_sizes[]` traegt fuer 0x2C/0x3B/0x50 die
KURZform (20/32/22). Die Handler sind konditional (Teil 1 §1). Die Handler-Funktionen des
Ports schieben bereits richtig; die Tabelle ist nur der `op_unknown`-Notnagel. Trotzdem
Kommentar ergaenzen (Adressen: 0x80040590/0x80040618/0x8004065c), damit kein Werkzeug sie
erneut als fix liest — genau dieser Fehler hat den Loesch-Trigger bisher unsichtbar gemacht.

## 8. OFFEN

* `flag(3,0x80)` (sub01 @0x240A, nach `Cmp(work_vars[0x0A]==13)` -> sub02) und das
  zweite `Sce_bgm_control 54 00 01 00 00 00` @0x22FC: die genaue Bedeutung der
  BGM-Kommandos 1 (`0x80044e00`, `jal 0x8005ab5c` + `jal 0x80060030`) und 2
  (`0x80044e50`, `jal 0x800603dc`, danach `DAT_800b52ac[slot*8] = 2`) ist NICHT
  aufgeloest — nur ihre Dispatch-Adressen sind belegt.
* Welche der beiden Tueren in main00 mit Eintritts-Cut 0 bzw. 3 korrespondiert, ist hier
  nicht ausgezaehlt (nur der `Switch`-Zweig selbst ist belegt).
* `Aot_set aot=3 sce=0x03 -> sub07` aus main00 (@0x215A, data `ff 00 18 07 00 00`):
  sub07 macht `Aot_reset 3` + `Work_set kind=3 idx=0` + `Member_set 0x00 = -9836`.
  Was Work-Kind 3 / Member 0x00 in diesem Raum konkret verschiebt, ist nicht ermittelt.
  Fuer den Loesch-Vorgang irrelevant, weil sub00 den Slot vorher mit dem Door-Record
  ueberschreibt.
