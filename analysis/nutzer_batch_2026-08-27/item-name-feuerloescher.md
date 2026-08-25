# FINDING B — "will you take ?" statt "will you take fire extinguisher" (ROOM1000)

Status: **GELOEST** (Ursache belegt, Fix-Rezept vollstaendig, echte Datenquelle im Asset-Baum gefunden).

## 1. Symptom (Nutzer-Wortlaut)

> "Beim Nehmen des Feuerloeschers in ROOM1000 steht noch 'will you take ?' statt
> 'will you take fire extinguisher'."

## 2. Was der PORT heute tut

### 2a. Der Prompt-Walker verschluckt den Namen still

`re15_port/engine/src/item_prompt_common.c:32-41`:

```c
        if (b == 0x06) {                            /* insert the item name (blob until 0x07) */
            i++;                                    /* skip the operand */
            if ((int)item_id < RE15_ITEM_PROMPT_NIDS) {
                const unsigned char *nm = re15_item_prompt_name_blob + re15_item_prompt_name_off[item_id];
                for (; *nm != 0x07; nm++) {
                    if (cb && total < max_glyphs) cb(ctx, *nm, attr, 0);
                    total++;
                }
            }
            continue;
        }
```

Ist `item_id >= RE15_ITEM_PROMPT_NIDS`, wird **nichts** emittiert — kein Fallback, keine
Warnung, kein else-Zweig. Das Skript laeuft danach mit `05 00 1b` weiter (Farbe zurueck,
Glyph 0x1b = Fragezeichen), also bleibt exakt `Will you take the ?` stehen. Das ist der
beobachtete Text.

### 2b. Die Tabelle ist bei 48 abgeschnitten

`re15_port/engine/src/gen/item_prompt_data.inc:6-8`:

```c
#define RE15_ITEM_PROMPT_NIDS 48
static const unsigned char re15_item_prompt_name_blob[685] = {0x07,0x1f,0x4b,0x49,...,0x2a,0x51,0x50,0x07};
static const unsigned short re15_item_prompt_name_off[48] = {0x0,0x1,0xe,...,0x296,0x2a9};
```

685 Blob-Bytes, 48 Offsets. Letzter Eintrag `0x2a9` = "Nut" (id 0x2f).
Der Header der Datei nennt als Quelle einen Savestate, nicht ein ausgeliefertes Asset:
`(item prompt data, byte-true from stage_saves/mzd_stage1_briefing.sav)`.

### 2c. Die ID, die der Modal weitergibt, ist der ROHE Item-Typ

`re15_port/engine/src/item_modal_common.c:32,138,226`:

```c
static uint8_t s_type     = 0;      /* DAT_800afbb6 (grant id)              */
    s_type     = item_type;          /* DAT_800afbb6 @0x8004335c      */
    s_reveal_total = re15_item_prompt_walk(s_prompt, s_type, 0, 0, 0);
```

Also `item_id == 0x31` fuer den Feuerloescher (Beleg in §3a).

### 2d. Dieselbe Truncation ein zweites Mal — Status-Screen

`re15_port/engine/src/re15_inv_screen.c:497-499`:

```c
    if (id < 0 || id >= RE15_INV_NAME_NIDS) return;
    p = re15_inv_name_blob + re15_inv_name_off[id];
```

mit `re15_port/engine/src/gen/inv_name_bank.inc:5`: `#define RE15_INV_NAME_NIDS 52`.
52 deckt den Feuerloescher (0x31) noch ab, aber **nicht** die Keycards (0x36..0x39),
Pocket Watch (0x3f), Spark Plug (0x41), Minidisc Player w/ Disc (0x44) und die
Master-Keycards (0x46/0x47) — im Inventar bleibt deren Namenszeile leer.

---

## 3. Was das ORIGINAL tut

### 3a. Die Item-ID des Feuerloeschers — ROOM1000.RDT, Item_aot_set

Datei: `re15_port/shared_assets/PSX/STAGE1/ROOM1000.RDT`, **Datei-Byte-Offset 0x0c24**
(sub-SCD, Region `sub00`). Roh-Bytes (22 Byte Kurzform, weil `pc[3]&0x80 == 0`):

```
0xc24: 50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00
       ^  ^  ^  ^  ^  ^  \___/ \___/ \___/ \___/ \___/ \___/ \___/ \___/
       |  |  |  |  |  |  rectx rectz rectw rectd type  amt   tkbit prop
       |  |  |  |  |  +-- super = 0x00
       |  |  |  |  +----- floor = 0x00
       |  |  |  +-------- sat   = 0x31 (ACTION-gegatet, Bit 0x80 clear => Kurzform)
       |  |  +----------- sce   = 0x09 (Item)
       |  +-------------- slot  = 0x03
       +----------------- op    = 0x50 Item_aot_set
```

- `pc[14..15] = 31 00` (LE u16) -> **item_type = 0x31 = 49**
- `pc[16..17] = 01 00` -> amount = 1
- `pc[18] = 0x86` -> taken-bit 134 (Flag-Zone 9)
- `pc[20] = 0xff` -> kein Prop-Objekt

Feld-Lesart byte-true belegt in `re15_port/engine/src/scd_vm.c:3448-3470`
(Kommentar zitiert `LAB_80043500` fuer `lhu block+14` / `lbu block+16` und
`@0x80040668` fuer die Kurz-/Langform-Verzweigung) und deckt sich mit dem dort bereits
notierten Fakt "ROOM1000 @0xc24 = 0x31 x1".

Der unmittelbar folgende Record @0x0c3a ist
`50 04 09 31 00 00 0e ed e0 fc e8 03 e8 03 24 00 01 00 a6 00 00 00`
= type `0x24` (Green Medicine). Der zeigt (id 36 < 48) im Port korrekt an — genau
deshalb ist der Bug bisher nicht aufgefallen: im selben Raum liegen ein funktionierendes
und ein kaputtes Item nebeneinander.

Cross-Check gegen die Port-Item-Enums:
`grep -rn "EXTING|FIRE_EXT|Extinguisher" re15_port/ --include=*.c --include=*.h -i`
liefert **null Treffer** — der Port hat ueberhaupt kein benanntes Enum fuer Item-Typen;
er reicht die Roh-ID durch.

### 3b. Der Namens-Resolver im Original: FUN_80028840 — **KEINE Grenzpruefung**

`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80028840 10`
(Quelle `info/Re1.5/PSX.EXE`):

```
80028840: andi  a0,a0,0xff          ; id &= 0xff   <-- die EINZIGE "Maskierung"
80028844: sll   a0,a0,1             ; id *= 2
80028848: lui   at,0x800c
8002884c: addiu at,at,18780         ; at = 0x800c495c   (Offset-Tabelle)
80028850: addu  at,at,a0
80028854: lhu   v1,0(at)            ; off = u16[0x800c495c + id*2]
80028858: lui   v0,0x800c
8002885c: addiu v0,v0,18984         ; v0 = 0x800c4a28   (Namens-Blob)
80028860: jr    ra
80028864: addu  v0,v1,v0            ; return 0x800c4a28 + off
```

Roh-Bytes @0x80028840: `ff 00 84 30` (`andi a0,a0,0xff`), @0x80028854: `00 00 23 94`
(`lhu v1,0x0(at)=>DAT_800c495c`) — `ghidra1_V2.txt:113252` bzw. `:113257`.
Xref-Eintrag: `ghidra1_V2.txt:575347` — `DAT_800c495c  XREF[1]: FUN_80028840:80028854(*)`.

**Es gibt im Original weder einen 48er- noch einen 52er-Deckel.** Der Deckel ist rein
ein Port-Artefakt der savestate-extrahierten Teil-Tabelle.

Aufrufer (beide im Message-VM, am 0x06-Insert-Name-Opcode):

```
; Draw-Pfad  (ghidra1_V2.txt:112928)
800283f0: lbu  v0,-13(s4)
800283f4: jal  0x80028840
800283f8: andi a0,v0,0xff        ; delay slot: id aus dem Skript-Operanden
800283fc: j    0x80028424
80028400: addu s0,v0,zero        ; s0 = Zeiger auf den Namen -> Glyph-Schleife @0x80028424

; Mess-/Count-Pfad (ghidra1_V2.txt:113603)
80028aa8: beq  a0,zero,0x80028ab8
80028aac: addu s6,a2,zero
80028ab8: lui  a0,0x800c
80028abc: lbu  a0,-31453(a0)     ; = 0x800b8523 (aktuelles Item, wenn Operand == 0)
80028ac0: jal  0x80028840
80028ac4: andi a0,v0,0xff
80028ac8: j    0x80028bd0
80028acc: addu a2,v0,zero
```

Beide holen den Zeiger und laufen bis Glyph `0x07` — ohne jede Bereichspruefung.

### 3c. WOHER kommen die Tabellen? -> `BIN/DEBUG.BIN`, RAW nach 0x800c0000 geladen

Die RE1.5-`PSX.EXE` hat `t_addr = 0x80010000`, `t_size = 0x000af000`
(EXE-Header `info/Re1.5/PSX.EXE`, Datei-Offset 0x18 / 0x1c) und endet damit bei
**0x800bf000**. `re15_port/shared_assets/PSX/BIN/DEBUG.BIN` ist
**262144 = 0x40000 Bytes** gross und belegt `0x800c0000 .. 0x80100000` — exakt die
Luecke zwischen EXE-Ende und dem Stage-Overlay-Ladepunkt 0x80100000.
Die Adressen 0x800c495c / 0x800c4a28 / 0x800c4fc6 stehen im Ghidra-Dump als `??`
(ausserhalb des EXE-Images), siehe `ghidra1_V2.txt:575348`, `:575552`, `:576990` —
genau deshalb wurden sie frueher aus einem Savestate gezogen.

Die Zuordnung DEBUG.BIN -> 0x800c0000 ist im Repo bereits mehrfach file==RAM
gegengeprueft, also nicht neu geraten:

- `re15_port/engine/src/debug_menu_common.c:81-82` — "Das Original laedt DEBUG.BIN RAW
  nach 0x800c0000; die Tabelle liegt ab RAM 0x800c263a = Datei-Offset ..."
- `re15_port/engine/src/menu_common.c:26-27` — "fresh file disasm of the DEBUG.BIN grid
  handler 0x800c62a0-0x800c65fc (byte-identical to the mzd_inv_open.sav RAM module;
  DEBUG.BIN maps @0x800c0000)"
- `re15_port/tools/gen_inv_name_bank.py:6-9` — "byte-verified against the savestate RAM
  extraction of stage_saves/mzd_inv_open.sav: the per-glyph width table @0x800c4416
  (== include/font_width.h), the name offset table @0x800c495c and blob @0x800c4a28
  (== gen/item_prompt_data.inc entries 0..47) are all byte-identical file<->RAM."

**=> Datei: `re15_port/shared_assets/PSX/BIN/DEBUG.BIN`**

| Tabelle | RAM | Datei-Offset | Laenge |
|---|---|---|---|
| Namens-Offset-Tabelle | 0x800c495c | **0x495c** | 204 Byte (102 u16) |
| Namens-Blob | 0x800c4a28 | **0x4a28** | 0x560 = 1376 Byte |
| Prompt-Skript-Bank | 0x800c4fc6 | **0x4fc6** | 0x118 = 280 Byte |

### 3d. Wie viele Eintraege hat die Tabelle WIRKLICH? -> **102** (ids 0x00..0x65)

Strukturbeweis (gemessen, nicht geschaetzt): Die Offset-Tabelle beginnt @0x800c495c, der
Blob beginnt @0x800c4a28. Der Zwischenraum ist `0x4a28 - 0x495c = 0xCC = 204 Bytes
= 102 u16`. Alle 102 u16 sind monoton steigend (0x000, 0x001, 0x00e, ..., 0x54e, 0x55e);
der letzte Eintrag `off[101] = 0x55e` zeigt auf `00 07`, dessen Terminator den Blob
exakt bei **0x560** schliesst. Danach folgt Padding.

Hexdump der Grenzen (DEBUG.BIN, mit RAM-Adressen):

```
800c4950: 00 00 00 00 59 65 73 01 2e 4e 6f 00 |00 00 01 00   <- Tabelle ab 0x800c495c
                      "Yes" 01 "." "No" 00     (= DAT_800c4954, XREF 0x80028664)
800c4960: 0e 00 13 00 1f 00 28 00 35 00 3e 00 4c 00 5b 00

800c4a28: 07 1f 4b 49 3e 3d 50 00 27 4a 45 42 41 07 2c 45   <- Blob ab hier
          ^id0=leer  C  o  m  b  a  t  _  K  n  i  f  e  ^  P  i
800c4a38: 4c 41 07 1e 4e 4b 53 4a 45 4a 43 00 24 2c 07 2f

800c4f70: 45 48 41 00 14 07 31 49 3e 4e 41 48 48 3d 00 22
800c4f80: 45 48 41 00 15 07 00 07| 00 00 00 00 00 00 00 00  <- Blob endet 0x800c4f88
                        ^id 0x65 = Leerzeichen
800c4f90: 00 00 00 00 ff ff ff ff ff ff ff ff ff ff ff ff
800c4fa0..800c4fc5: durchgehend ff (Fuellung)
800c4fc6: 10 00 2f 00 50 00 6b 00 8b 00 a9 00 c5 00 f8 00   <- Skript-Zeigertabelle
```

`0x800c4a28 + 0x560 = 0x800c4f88` — passt exakt auf das Ende der Nutzdaten.
Blob-Laenge = **1376 Bytes** (der Port hat heute 685).

### 3e. Der Feuerloescher-Name, Glyph fuer Glyph

`off[0x31] = 0x2b4` -> Blob-Adresse `0x800c4a28 + 0x2b4 = 0x800c4cdc`,
**Datei-Offset `0x4a28 + 0x2b4 = 0x4cdc`** in `BIN/DEBUG.BIN`. Bytes:

```
0x4cdc: 22 45 4e 41 00 21 54 50 45 4a 43 51 45 4f 44 41 4e 07
         F  i  r  e  _  E  x  t  i  n  g  u  i  s  h  e  r  END
```

Glyph-Map (aus einem live-verifizierten Eintrag abgeleitet, nicht geraten):
id 0x15 = `24 57 00 23 51 4a 00 1e 51 48 48 41 50 4f` rendert im Port nachweislich als
**"H. Gun Bullets"** (`UNTESTED_IMPLEMENTATIONS.md`, Zeile U11, per Autoshot verifiziert).
Daraus direkt: `0x24='H'`, `0x57='.'`, `0x00=' '`, `0x23='G'`, `0x51='u'`, `0x4a='n'`,
`0x1e='B'`, `0x48='l'`, `0x41='e'`, `0x50='t'`, `0x4f='s'`
=> Grossbuchstaben `0x1d..0x36 = A..Z`, Kleinbuchstaben `0x3d..0x56 = a..z`,
Ziffern `0x0c..0x15 = 0..9` (Gegenprobe id 0x04 = `2f 25 23 00 2c 0e 0e 14` = "SIG P228";
id 0x08 = `2e 41 49 45 4a 43 50 4b 4a 00 29 14 13 0c` = "Remington M870"),
Terminator `0x07`.
Die Zeichensatzzelle ist `u = (code & 0xf) << 4`, `v = ((code >> 4) << 4) + 0x20` auf der
Message-Font-Page (`@0x80028d04-0x80028d3c`, zitiert in
`re15_port/engine/src/re15_inv_screen.c:502-505`).

### 3f. Die Prompt-Skripte stehen ebenfalls in DEBUG.BIN

Zeigertabelle @`0x800c4fc6` (Datei-Offset `0x4fc6`); erster Eintrag `0x10` => **8 Skripte**.
Offsets relativ zur Tabellenbasis: `0x10, 0x2f, 0x50, 0x6b, 0x8b, 0xa9, 0xc5, 0xf8`.
Die Region endet bei `0x800c50de` (= Basis der Beschreibungs-Bank, unabhaengig belegt in
`re15_port/engine/src/re15_inv_screen.c:514-516`).

```
[0] @0x800c4fd6 (31 B) 33 45 48 48 00 55 4b 51 00 50 3d 47 41 00 50 44 41 08 05 01 06 00 05 00 1b 03 02 f9 00 01 00
                       "Will you take the" NL col=1 INS-NAME col=0 "?" PAGE BRANCH(f9,0) END
[1] @0x800c4ff5 (33 B) 35 4b 51 00 3f 3d 4a 3a 50 ...  57 01 00    "You can t carry any/more items."
[2] @0x800c5016 (27 B) 35 4b 51 00 44 3d 52 41 00 51 4f 41 40 ...  "You have used the <X>."
[3] @0x800c5031 (32 B) 33 45 48 48 00 55 4b 51 00 4c 48 3d 3f 41 ..."Will you place the <X>?"   <- Port hat es NICHT
[4] @0x800c5051 (30 B) 33 45 48 48 00 55 4b 51 00 51 4f 41 ...      "Will you use the <X>?"
[5] @0x800c506f (27 B) 30 44 41 00 05 01 06 00 05 00 08 44 3d 4f ..."The <X>/has been filed."   <- Port hat es NICHT
[6] @0x800c508b (51 B) 35 4b 51 00 40 4b 4a 3a 50 ...              "You don t need this key any/more. Discard it?"  <- Port hat es NICHT
[7] @0x800c50be (32 B) 28 4b 3f 47 41 40 00 42 4e 4b 49 ...        "Locked from the other side."  <- Port hat es NICHT
```

Die vier heute vendorten Skripte sind **byte-identisch** mit DEBUG.BIN[0]/[1]/[2]/[4];
die Laengen 31/33/27/30 entsprechen exakt den Tabellendifferenzen
(0x2f-0x10=31, 0x50-0x2f=33, 0x6b-0x50=27, 0xa9-0x8b=30). Die Port-Zuordnung
take=0 / full=1 / used=2 / use=4 (`item_prompt_common.c:16-22`) ist damit bestaetigt.

---

## 4. Die Divergenz (praezise)

| | Original | Port heute |
|---|---|---|
| Namens-Tabelle | 102 Eintraege @0x800c495c, Blob 1376 B @0x800c4a28 (DEBUG.BIN 0x495c / 0x4a28) | 48 Eintraege, Blob 685 B (`gen/item_prompt_data.inc:6-8`) |
| Grenzpruefung | **keine** — `FUN_80028840` @0x80028840 macht nur `andi a0,a0,0xff` | `if (item_id < 48)`, sonst STILL nichts (`item_prompt_common.c:34`) |
| Inventar-Namen | dieselbe 102er-Tabelle | 52 Eintraege (`gen/inv_name_bank.inc:5`) |
| Ergebnis id 0x31 | "Will you take the Fire Extinguisher?" | "Will you take the ?" |

**Ursache in einem Satz:** `RE15_ITEM_PROMPT_NIDS = 48` ist zu klein; die ID des
Feuerloeschers ist `0x31 = 49`; der Guard in `item_prompt_common.c:34` faellt still
durch und laesst die Namensstelle leer.

## 4b. ALLE betroffenen Items (game-weiter Zensus STAGE1..STAGE6)

Methode: der bestehende, disasm-verifizierte SCD-Walker
`re15_port/tools/aot_sce_census.py` (Opcode-Laengen aus `scd_vm.c s_opcode_sizes`;
`0x50 Item_aot_set` = 22 / 30 Byte je `pc[3]&0x80`, belegt @0x8004065c-0x80040668) ueber
alle `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`.
Ergebnis: **164 Item_aot_set-Records, 27 verschiedene Item-Typen.**

Typen < 48 (heute korrekt benannt): 0x04 SIG P228, 0x05 Beretta M93R, 0x07 Super Redhawk,
0x08 Remington M870, 0x0c Ingram M10, 0x0d SPAS-12, 0x13 H&K MC51, 0x15 H. Gun Bullets,
0x16 Shotgun Shells, 0x17 Magnum Bullets, 0x22 First Aid Spray, 0x23 Antidote Spray,
0x24 Green Medicine, 0x25 Red Medicine, 0x26 Blue Medicine.

**Typen >= 48 — Name fehlt heute UEBERALL (12 Typen, 25 Records):**

| id | dec | Name (DEBUG.BIN) | n | Raum @ RDT-Datei-Offset (Script, Slot) |
|---|---|---|---|---|
| 0x30 | 48 | Pliers | 2 | ROOM11B0 @0x0ff2, ROOM11B1 @0x0fe4 (main00, slot 4) |
| 0x31 | 49 | **Fire Extinguisher** | 1 | **ROOM1000 @0x0c24 (sub00, slot 3)** |
| 0x32 | 50 | Head of Akuma | 2 | ROOM2040/2041 @0x1364 (main00, slot 7) |
| 0x36 | 54 | Green Keycard | 2 | ROOM3040/3041 @0x14b2 (sub00, slot 9) |
| 0x37 | 55 | Red Keycard | 4 | ROOM1190/1191 @0x2a30 + @0x2aa2 (sub13, slot 5) |
| 0x38 | 56 | Blue Keycard | 2 | ROOM1110/1111 @0x0b2e (main00, slot 8) |
| 0x39 | 57 | Yellow Keycard | 1 | ROOM1011 @0x0954 (main00, slot 2) |
| 0x3f | 63 | Pocket Watch | 2 | ROOM2090/2091 @0x0a12 (main00, slot 5) |
| 0x41 | 65 | Spark Plug | 2 | ROOM4040/4041 @0x0f66 (main00, slot 6) |
| 0x44 | 68 | Minidisc Player w/ Disc | 2 | ROOM1200/1201 @0x0932 (sub00, slot 3) |
| 0x46 | 70 | Red Master Keycard | 2 | ROOM30A0/30A1 @0x0742 (main00, slot 1) |
| 0x47 | 71 | Blue Master Keycard | 1 | ROOM4010 @0x0592 (main00, slot 9) |

(Alle amount = 1. Offsets sind Datei-Byte-Offsets in der jeweiligen `ROOM*.RDT`.)

**Also: JEDES Schluessel-/Quest-Item im ganzen Spiel zeigt heute "Will you take the ?".**
Der Feuerloescher ist nur das erste, das der Spieler erreicht — der Nutzer-Report ist die
Spitze eines 12-Item-Defekts.

Zusaetzlich nur im Status-Screen betroffen (id >= 52 = `RE15_INV_NAME_NIDS`):
0x36, 0x37, 0x38, 0x39, 0x3f, 0x41, 0x44, 0x46, 0x47 — deren Inventar-Namenszeile bleibt
leer (`re15_inv_screen.c:497` `return;`).
Nicht per Item_aot_set vergeben, aber jenseits beider Deckel: ids 0x48..0x65
(Chris Diary, Operation Report, File 3..10, Charakternamen, Gegnernamen,
Umbrella File 7..9).

## 4c. Volle Original-Tabelle (alle 102 Eintraege, aus DEBUG.BIN dekodiert)

```
id  off    Name                     | id  off    Name
00  0x000  <leer>                   | 33  0x2d4  Minidisc
01  0x001  Combat Knife             | 34  0x2dd  Minidisc Player
02  0x00e  Pipe                     | 35  0x2ed  Timer Bomb
03  0x013  Browning HP              | 36  0x2f8  Green Keycard
04  0x01f  SIG P228                 | 37  0x306  Red Keycard
05  0x028  Beretta M93R             | 38  0x312  Blue Keycard
06  0x035  Glock 18                 | 39  0x31f  Yellow Keycard
07  0x03e  Super Redhawk            | 3a  0x32e  T-Virus
08  0x04c  Remington M870           | 3b  0x336  G-Virus
09  0x05b  Hand Grenade             | 3c  0x33e  Enzyme
0a  0x068  Acid Grenade             | 3d  0x345  T-Vaccine
0b  0x075  Incendiary Grenade       | 3e  0x34f  G-Vaccine
0c  0x088  Ingram M10               | 3f  0x359  Pocket Watch
0d  0x093  SPAS-12                  | 40  0x366  Fuse
0e  0x09b  Flamethrower             | 41  0x36b  Spark Plug
0f  0x0a8  Grenade Launcher         | 42  0x376  Key Disc
10  0x0b9  Grenade Launcher         | 43  0x37f  Communications Card
11  0x0ca  Grenade Launcher         | 44  0x393  Minidisc Player w{38} Disc
12  0x0db  Rocket Launcher          | 45  0x3ab  Water Key
13  0x0eb  H{64}K MC51              | 46  0x3b5  Red Master Keycard
14  0x0f4  Colt Python              | 47  0x3c8  Blue Master Keycard
15  0x100  H. Gun Bullets           | 48  0x3dc  Chris{3a} Diary
16  0x10f  Shotgun Shells           | 49  0x3e9  Operation Report
17  0x11e  Magnum Bullets           | 4a  0x3fa  File 3
18  0x12d  Flamethrower Fuel        | 4b  0x401  File 4
19  0x13f  Explosive Rounds         | 4c  0x408  File 5
1a  0x150  Acid Rounds              | 4d  0x40f  File 6
1b  0x15c  Incendiary Rounds        | 4e  0x416  File 7
1c  0x16e  Remote Detonator         | 4f  0x41d  File 8
1d  0x17f  Empty Grenade Shells     | 50  0x424  File 9
1e  0x194  Nitro Capsule            | 51  0x42b  File 10
1f  0x1a2  Acid Capsule             | 52  0x433  Albert Wesker
20  0x1af  Incendiary Capsule       | 53  0x441  Jill Valentine
21  0x1c2  Memory Card              | 54  0x450  Chris Redfield
22  0x1ce  First Aid Spray          | 55  0x45f  Barry Burton
23  0x1de  Antidote Spray           | 56  0x46c  Rebecca Chambers
24  0x1ed  Green Medicine           | 57  0x47d  Brad Vickers
25  0x1fc  Red Medicine             | 58  0x48a  Enrico Marini
26  0x209  Blue Medicine            | 59  0x498  E. Dewey {64} K. Dooley
27  0x217  G{5b}R Medicine Mix      | 5a  0x4ad  J. Frost {64} F. Speyer
28  0x228  G{5b}G Medicine Mix      | 5b  0x4c2  K. Sullivan {64} R. Aiken
29  0x239  G{5b}B Medicine Mix      | 5c  0x4d9  Cerberus
2a  0x24a  G{5b}G{5b}R Medicine Mix | 5d  0x4e2  Mutated Baboon
2b  0x25d  G{5b}G{5b}G Medicine Mix | 5e  0x4f1  Mutated Alligator
2c  0x270  G{5b}G{5b}B Medicine Mix | 5f  0x503  Gargantuagator
2d  0x283  G{5b}R{5b}R Medicine Mix | 60  0x512  Arachnophobia
2e  0x296  G{5b}R{5b}B Medicine Mix | 61  0x520  Giant Spiders
2f  0x2a9  Nut                      | 62  0x52e  Umbrella File 7
30  0x2ad  Pliers                   | 63  0x53e  Umbrella File 8
31  0x2b4  Fire Extinguisher        | 64  0x54e  Umbrella File 9
32  0x2c6  Head of Akuma            | 65  0x55e  <Leerzeichen>
```

`{xx}` = Glyph-Code, den ich NICHT belegt entschluesselt habe (siehe §7.1). `0x64` liegt
im Digraph-Bereich des Stream-Decoders `FUN_80013160` (Codes 0x60..0xb8, Paar-Tabelle
`0x800c44b8 + (c-0xa0)*2` — bereits vendort als `re15_inv_name_digraph` in
`gen/inv_name_bank.inc`). Fuer die 12 betroffenen Item_aot_set-Typen ist davon nur
`0x38` (id 0x44) relevant, kein Digraph.

---

## 5. Fix-Rezept fuer den Implementierer

**Kernaussage: Die echte Quelle liegt bereits im Asset-Baum
(`re15_port/shared_assets/PSX/BIN/DEBUG.BIN`), und es gibt bereits einen Generator, der
genau diese Tabelle liest (`re15_port/tools/gen_inv_name_bank.py`). Der Fix ist
Regenerierung mit der ECHTEN Laenge — kein Nachtragen von Hand, keine neuen Konstanten.**

### Schritt 1 — `re15_port/tools/gen_inv_name_bank.py:33`

```python
NIDS = 52                          # ids 0x00..0x33 (spec: 52 decoded names)
```
ersetzen durch
```python
# 102 u16 = die VOLLE Tabelle: sie spannt exakt [0x800c495c, 0x800c4a28) = 0xCC Bytes
# (der Blob beginnt unmittelbar danach). Offsets monoton 0x000..0x55e; off[101]=0x55e
# zeigt auf "00 07", dessen Terminator den Blob bei 0x560 exakt schliesst.
# FUN_80028840 @0x80028840 hat KEINE Grenzpruefung (nur `andi a0,a0,0xff`).
NIDS = 102                         # ids 0x00..0x65
```
Der Header-Kommentar in Zeile 47-50 ("Name offsets 52 u16") ist mitzuziehen.
Danach ist `RE15_INV_NAME_NIDS = 102`, und `re15_inv_screen.c:497` deckt Keycards & Co ab.
Der Blob waechst von 685 auf **1376** Bytes, die Offset-Tabelle auf 102 Eintraege.
Aufruf: `python re15_port/tools/gen_inv_name_bank.py`
(liest `re15_port/shared_assets/PSX/BIN/DEBUG.BIN`, schreibt `engine/src/gen/inv_name_bank.inc`).

### Schritt 2 — neuer Generator `re15_port/tools/gen_item_prompt_data.py`

`gen/item_prompt_data.inc` ist heute handgeschnitzt aus einem Savestate. Ersetzen durch
einen Generator aus derselben Datei-Quelle:

```python
BASE     = 0x800C0000
SRC      = re15_port/shared_assets/PSX/BIN/DEBUG.BIN
OFF_TBL  = 0x800C495C - BASE   # = 0x495c, 102 u16
BLOB     = 0x800C4A28 - BASE   # = 0x4a28, laeuft bis 0x4f88 (0x560 Bytes)
SCRIPTS  = 0x800C4FC6 - BASE   # = 0x4fc6, u16-Zeigertabelle, n = tbl[0]/2 = 8,
                               #   Region endet @0x800C50DE (Basis der Desc-Bank)
NIDS     = 102
# Skript-Zuordnung Port-Key -> BSS-Index (unveraendert, item_prompt_common.c:16-22):
#   take = 0, full ("can t carry") = 1, used = 2, use = 4
```
Die vier heute vendorten Skripte sind byte-identisch zu DEBUG.BIN[0]/[1]/[2]/[4]
(Laengen 31/33/27/30 == Tabellendifferenzen 0x2f-0x10, 0x50-0x2f, 0x6b-0x50, 0xa9-0x8b)
— die Regenerierung ist also ein verlustfreier Ersatz. Zusaetzlich verfuegbar und
sinnvoll mitzunehmen: [3] "Will you place the X?", [5] "The X has been filed.",
[6] "You don t need this key any more. Discard it?", [7] "Locked from the other side."

### Schritt 3 — `re15_port/engine/src/item_prompt_common.c:34`

```c
            if ((int)item_id < RE15_ITEM_PROMPT_NIDS) {
```
Nach Schritt 2 ist NIDS = 102 und der Zweig greift fuer alle im Spiel vergebenen ids
(Maximum 0x47 = 71). Zusaetzlich: das Original hat **gar keine** Pruefung
(`FUN_80028840` @0x80028840 = nur `andi a0,a0,0xff`). Der Port-Guard darf als
Speicherschutz bleiben, muss aber LAUT sein statt still:

```c
            if ((int)item_id < RE15_ITEM_PROMPT_NIDS) {
                ...
            } else {
                RE15_LOG("item_prompt: id %u ausserhalb der %d-Namenstabelle "
                         "(Original @0x80028840 hat KEINE Grenze)", item_id,
                         RE15_ITEM_PROMPT_NIDS);
            }
```
Der stille else-Zweig war der Grund, dass der Defekt ueber 12 Items hinweg unbemerkt blieb.

### Schritt 4 — Dedupe (optional, empfohlen)

Nach Schritt 1+2 vendoren `gen/item_prompt_data.inc` und `gen/inv_name_bank.inc`
**denselben** Blob + dieselbe Offset-Tabelle (1376 + 204 Bytes doppelt).
`item_prompt_common.c` kann stattdessen `re15_inv_name_off` / `re15_inv_name_blob`
verwenden; `item_prompt_data.inc` behielte dann nur noch die 8 Skripte.

### Schritt 5 — Header-Kommentare korrigieren

Folgende Stellen behaupten "savestate-derived, nicht aus einem ausgelieferten Asset
gelesen" und sind nach dem Fix falsch:
- `re15_port/engine/src/item_prompt_common.c:3-5`
- `re15_port/include/re15_item_prompt.h:4-6`
- `re15_port/engine/src/item_modal_common.c:7`
- `shots/inv_re_reports.md:158` ("prompt text source (residual)")

Die Quelle ist `shared_assets/PSX/BIN/DEBUG.BIN` @0x495c / 0x4a28 / 0x4fc6 — ein
ausgeliefertes Asset. Damit faellt der dort dokumentierte "einzige Residual"-Punkt weg.

---

## 6. Wie man es verifiziert

**A) Statisch (sofort, ohne Build):**

```python
import struct
d = open("re15_port/shared_assets/PSX/BIN/DEBUG.BIN", "rb").read()
off = struct.unpack_from("<102H", d, 0x495c)
p = 0x4a28 + off[0x31]; s = b""
while d[p] != 0x07:
    s += bytes([d[p]]); p += 1
print(off[0x31] == 0x2b4, s.hex(" "))
# erwartet: True  22 45 4e 41 00 21 54 50 45 4a 43 51 45 4f 44 41 4e
```

**B) Nach dem Fix, Unit:** `re15_port/tests/unit/test_item_modal.c` um einen Fall
`item_type = 0x31` erweitern. `re15_item_prompt_walk(1, 0x31, 0, 0, 0)` muss genau
**17** hoeher liegen als `re15_item_prompt_walk(1, 0x00, 0, 0, 0)` (id 0x00 = leerer
Name; "Fire Extinguisher" = 17 Glyphen inkl. Leerzeichen, siehe §3e).
Heute ist die Differenz **0** — das ist der Regressionstest.

**C) Live (Pflicht, per gdigrab — Skill `re15-port-visual-verify`):**
`RE15_DEBUG_JUMP="1000@<frame>"` -> zum Feuerloescher laufen (`RE15_INPUT_SCRIPT`) ->
SQUARE -> der Modal muss **"Will you take the Fire Extinguisher?"** zeigen, der Name in
der Item-Farbe (0x05-Op, attr 1) des Skripts.
`RE15_AUTOSHOT` / `RE15_SOFTWARE_RENDER` sind laut Memory
`reai-v2-visual-verify-gdigrab` fuer Font-/Textur-Verifikation NICHT zulaessig.

**D) Regressionsbreite:** dieselbe Probe fuer mindestens je einen Raum aus §4b —
ROOM1110 (Blue Keycard 0x38), ROOM11B0 (Pliers 0x30), ROOM1200 (Minidisc Player w/ Disc
0x44 — enthaelt die Sonderglyphe 0x38), plus Status-Screen-Namenszeile fuer 0x38 nach
Schritt 1.

---

## 7. Offene Punkte / NICHT GEFUNDEN

1. **Glyph 0x38 (in id 0x44 "Minidisc Player w{38} Disc") — NICHT GEFUNDEN.**
   Er liegt zwischen Z (0x36) und a (0x3d), also im Interpunktionsblock. Belegt sind dort
   bisher nur `0x3a` = Apostroph und `0x57` = Punkt
   (`re15_port/engine/src/re15_inv_screen.c:517`); `0x3b` (in "SPAS{3b}12", "T{3b}Virus")
   und `0x5b` (in "G{5b}R Medicine Mix") habe ich ebenfalls NICHT verifiziert.
   Ich behaupte deshalb kein ASCII-Zeichen dafuer.
   Fuer den Fix irrelevant — der Port gibt den rohen Code an dieselbe TEX.TIM-Zelle
   (`u=(code&0xf)<<4, v=((code>>4)<<4)+0x20`) weiter wie das Original; die Darstellung
   ist also byte-true, egal wie das Zeichen heisst.
   Naechster Weg, wenn man es benennen will: die Zelle im Font-Atlas nachsehen
   (`re15_port/include/font_atlas_psx.h` bzw. TEX.TIM, u=0x80 / v=0x50 fuer 0x38).

2. **Die 102 sind strukturell belegt, nicht per Code-Bound.** `FUN_80028840` hat keinerlei
   Grenzpruefung, also gibt es im Original keine Instruktion, die "102" nennt. Der Beweis
   ist die exakte Passung der Tabellen-Region [0x800c495c, 0x800c4a28) = 204 B = 102 u16
   plus die luecken- und ueberschneidungsfreie Monotonie bis `off[101] = 0x55e`, dessen
   Name den Blob bei 0x560 exakt schliesst (danach 0x00-Padding, dann 0xff-Fuellung bis
   0x800c4fc6). Die einzige weitere Xref in die Umgebung ist `DAT_800c4954`
   (XREF 0x80028664) = die "Yes"/"No"-Strings unmittelbar DAVOR — die Region ist also
   nach oben und unten sauber begrenzt.

3. **NICHT GEPRUEFT:** ob eine der ids 0x66..0xff jemals vergeben wird. Der Zensus deckt
   nur `Item_aot_set` (Welt-Pickups, Maximum 0x47). Naechster Weg, falls noetig: derselbe
   Walker ueber die Inventar-Grant-Opcodes (`0x4F` und die `Item_set`-Varianten), sowie
   die Skript-Aufrufe des Message-VM-Opcodes 0x06 mit Operand != 0.

4. **NICHT GEPRUEFT:** ob der Port DEBUG.BIN zur LAUFZEIT laedt oder alles vendort.
   `debug_menu_common.c:81-82` liest die Debug-Jump-Tabelle zur Laufzeit aus DEBUG.BIN —
   falls dort bereits ein Loader existiert, waere ein Laufzeit-Lookup (statt Vendoring)
   der sauberere Fix und wuerde Schritt 4 ueberfluessig machen. Das habe ich nicht zu Ende
   verfolgt.

5. **NICHT GEPRUEFT:** ob die 4 fehlenden Skripte ([3] place, [5] filed, [6] key-discard,
   [7] locked-from-other-side) im Port anderweitig (als ASCII oder gar nicht) umgesetzt
   sind. Der Prompt-Dispatcher `item_prompt_common.c:17-23` kennt nur die Keys 1/2/4/5.

---

# 8. Verifikation (unabhaengig nachdisassembliert)

Pruefer: Verifikations-Agent, 2026-08-27. Jede Adresse selbst disassembliert
(`re15_disasm.py`), jede Datei-Byte-Behauptung selbst per `xxd`/`struct` nachgemessen,
alle Port-Zitate gegen `git show HEAD:<datei>` UND gegen den Arbeitsbaum geprueft.

**Gesamturteil: TEILWEISE — der Mechanismus, die Ursache und das Fix-Rezept sind
vollstaendig BESTAETIGT; zwei Zahlen im Dossier sind WIDERLEGT (beide nebensaechlich),
und §2 beschreibt nicht mehr den Arbeitsbaum-Stand.**

## 8.1 BESTAETIGT — Kern-Mechanismus

### FUN_80028840 @0x80028840 — keine Grenzpruefung

Eigene Disasm (`dis 0x80028840 12`, Quelle `info/Re1.5/PSX.EXE`) + Rohbytes
(`bytes 0x80028840 40`) — 1:1 wie im Dossier:

```
80028840: ff 00 84 30   andi  a0,a0,0xff
80028844: 40 20 04 00   sll   a0,a0,1
80028848: 0c 80 01 3c   lui   at,0x800c
8002884c: 5c 49 21 24   addiu at,at,0x495c      -> 0x800c495c
80028850: 21 08 24 00   addu  at,at,a0
80028854: 00 00 23 94   lhu   v1,0(at)
80028858: 0c 80 02 3c   lui   v0,0x800c
8002885c: 28 4a 42 24   addiu v0,v0,0x4a28      -> 0x800c4a28
80028860: 08 00 e0 03   jr    ra
80028864: 21 10 62 00   addu  v0,v1,v0
```

Die Funktion ist **zehn Instruktionen lang und endet mit `jr ra`** — es gibt keinen
weiteren Pfad, keinen toten Code, keine spaetere Ueberschreibung. `ghidra1_V2.txt:113252`
und `:113257` enthalten exakt diese Zeilen; `:575347` = `DAT_800c495c XREF[1]:
FUN_80028840:80028854(*)`. **Kein 48er-/52er-Deckel im Original. BESTAETIGT.**

### Aufrufer 1 @0x800283f4 (Draw-Pfad) — BESTAETIGT, mit einer Ergaenzung

```
800283e0: lbu   v0,1(s0)          ; Skript-Operand
800283e8: bne   v0,zero,0x800283f4
800283ec: addiu v0,v0,-1          ; Delay-Slot: id = operand-1
800283f0: lbu   v0,-13(s4)        ; operand==0 -> DAT_800b8523 (aktuelles Item)
800283f4: jal   0x80028840
800283f8: andi  a0,v0,0xff
```

Das Dossier zeigt die `addiu v0,v0,-1`-Zeile nicht; die Semantik ist "Operand minus 1,
sonst DAT_800b8523". Fuer die Behauptung irrelevant, aber die Delay-Slot-Subtraktion
gehoert dazu.

### Aufrufer 2 @0x80028ac0 (Mess-/Count-Pfad) — BESTAETIGT, Zitat-Tippfehler

```
80028aa8: beq   a0,zero,0x80028ab8
80028aac: addu  s6,a2,zero
80028ab0: j     0x80028ac0
80028ab4: addiu a0,a0,-1
80028ab8: lui   a0,0x800c
80028abc: lbu   a0,-31453(a0)    ; = 0x800b8523
80028ac0: jal   0x80028840
80028ac4: andi  a0,a0,0xff       <-- a0,a0 — NICHT "a0,v0" wie im Dossier zitiert
```

Rohbyte @0x80028ac4 = `ff 00 84 30` = `andi a0,a0,0xff` (`ghidra1_V2.txt:113603` ff.).
Das Dossier schreibt an dieser Stelle `andi a0,v0,0xff`. **Reine Transkription — die
Aussage (kein Bound-Check, id kommt aus 0x800b8523) bleibt korrekt.**

### NEU (im Dossier fehlend, aber der entscheidende Lueckenschluss): pc[14] -> Resolver

Das Dossier belegt `item_type = pc[14..15]` nur indirekt ueber einen Port-Kommentar
(`scd_vm.c`), und dessen `LAB_80043500`-Zitat traegt NICHT (@0x80043500 steht ein
Rect-Test `lh a0,4(a2)` / `slt`, kein `lhu +14`). Der echte Beweis liegt im
sce-Dispatcher und im sce-9-Handler — ich habe ihn nachgetragen:

1. Installer FUN_80040640 speichert **record+2** in den AOT-Slot:

```
80040654: lw    a2,28(a0)          ; a2 = SCD-pc
8004065c: lbu   v0,3(a2)           ; pc[3] = sat
80040664: andi  v0,v0,0x80
80040668: beq   v0,zero,0x80040680
8004066c: addiu v0,a2,30           ; Langform: pc += 30
80040680: lhu   a1,18(a2)          ; Kurzform: taken-bit  = pc[18]
80040684: lbu   s1,20(a2)          ;           prop-index = pc[20]
80040688: addiu v0,a2,22           ; Kurzform: pc += 22
800406b0/cc: addiu v0,a2,2         ; <-- gespeicherter Blockzeiger = pc+2
800406d0: sw    v0,0(s0)           ;     s0 = 0x800ac9b0 + slot*4
```

=> Die 22/30-Behauptung (`@0x8004065c-0x80040668`) ist **BESTAETIGT**.
Anmerkung: die Port-Kommentare in `scd_vm.c` nennen `lhu 18/26` faelschlich
"type/amount" — es sind taken-bit und prop-index. Der daraus abgeleitete +8-Versatz
stimmt trotzdem.

2. Dispatcher @0x80042fc8-0x80043010 (s0 = Blockzeiger = pc+2):

```
80042fc8: lbu   v0,1(s0)          ; = pc[3] = sat
80042fd0: andi  v0,v0,0x80
80042fd4: beq   v0,zero,0x80042ff8
80042fd8: addiu a0,s0,20          ; Langform  -> a0 = pc+22
80042ffc: lbu   v0,0(s0)          ; = pc[2] = sce  (Tabellenindex)
80043004: addu  v0,v0,s5          ; s5 = 0x8007469c
80043008: lw    v0,0(v0)
8004300c: addiu a0,s0,12          ; Kurzform  -> a0 = pc+14   <<<<<<
80043010: jalr  v0
```

`(0x800746c0 - 0x8007469c)/4 = 9` -> sce 9 zeigt auf `LAB_80043328`
(`ghidra1_V2.txt:154272` / `:243233`).

3. sce-9-Handler @0x80043328:

```
8004332c: lbu v0,11579(v0)        ; DAT_80072d3b (Modal-State)
80043334: bne v0,zero,0x80043368  ; laeuft schon -> raus
8004333c: lbu v0,0(a0)            ; <<< = pc[14] = ITEM-TYP
8004334c: sb  v1,11579(at)        ; state = 1
80043354: sb  v0,-31453(at)       ; -> 0x800b8523   (Quelle von Aufrufer 2!)
8004335c: sb  v0,-1098(at)        ; -> 0x800afbb6   (grant id / Modal-Typ)
```

Damit ist die Kette **lueckenlos disassembliert**: `pc[14]` (= 0x31 in ROOM1000) wandert
UNMASKIERT nach 0x800b8523 / 0x800afbb6 und von dort in FUN_80028840, der keinerlei
Bereichspruefung hat. Der `andi 0xff` ist die einzige Maskierung im ganzen Pfad.

### ROOM1000.RDT @0x0c24 — BESTAETIGT (xxd)

```
00000c24: 5003 0931 0000 1450 e0fc 2003 2003 3100
00000c34: 0100 8600 ff00 5004 0931 0000 0eed e0fc
00000c44: e803 e803 2400 0100 a600 0000 ...
```

pc[14..15] = `31 00` -> 0x31 = 49; pc[16..17] = `01 00`; pc[18] = 0x86; pc[20] = 0xff.
Folgerecord @0x0c3a = Typ `24 00` = Green Medicine. **Alles wie behauptet.**

### DEBUG.BIN — BESTAETIGT, Struktur selbst nachgemessen

`re15_port/shared_assets/PSX/BIN/DEBUG.BIN`, 262144 = 0x40000 Byte.
EXE-Header `info/Re1.5/PSX.EXE` @0x18/0x1c = `00 00 01 80` / `00 f0 0a 00`
-> t_addr 0x80010000, t_size 0x000af000, Ende 0x800bf000. 0x800c0000 + 0x40000 = 0x80100000.

```
00004950: 0000 0000 5965 7301 2e4e 6f00 |0000 0100   <- Tabelle ab 0x495c
00004960: 0e00 1300 1f00 2800 3500 3e00 4c00 5b00
00004a28: 071f 4b49 3e3d 5000 274a 4542 4107 2c45   <- Blob ab 0x4a28
00004cdc: 2245 4e41 0021 5450 454a 4351 454f 4441
00004cec: 4e07 ...
00004f80: 4548 4100 1507 0007 0000 0000 ...          <- Blob-Ende 0x4f88
00004f90: 0000 0000 ffff ffff ...
00004fc0: ffff ffff ffff 1000 2f00 5000 6b00 8b00   <- Skript-Tabelle ab 0x4fc6
```

Eigene Messung ueber alle 102 u16 @0x495c:

- **strikt** monoton (nicht nur monoton), 0x000 .. 0x55e
- fuer i = 0..100 gilt `off[i] + laenge_bis_0x07(i) + 1 == off[i+1]` — **0 Luecken,
  0 Ueberlappungen** (selbst geprueft, alle 101 Paare)
- `off[101] = 0x55e`, Name dort 1 Byte (`00`), Terminator schliesst bei
  `0x4a28 + 0x560 = 0x4f88` — exakt der Uebergang zum 0x00-Padding.

=> **102 Eintraege / Blob 0x560 = 1376 Byte: BESTAETIGT.**

**Zusaetzlicher, staerkerer Beweis fuer DEBUG.BIN@0x800c0000 als der Dossier-Beweis:**
Die im Port bei HEAD vendorten, aus dem SAVESTATE gezogenen Daten sind byte-identisch
mit der DATEI: `re15_item_prompt_name_off[48] == unpack("<102H", DEBUG.BIN, 0x495c)[0:48]`
(True) und `re15_item_prompt_name_blob[685] == DEBUG.BIN[0x4a28:0x4a28+685]` (True).
Savestate-RAM == Datei ist damit unabhaengig von jeder Adressarithmetik belegt.

### off[0x31] = 0x2b4, Name @Datei 0x4cdc — BESTAETIGT

`22 45 4e 41 00 21 54 50 45 4a 43 51 45 4f 44 41 4e` = 17 Glyphen, Terminator 0x07.
Glyph-Map gegengeprueft an id 0x15 ("H. Gun Bullets" — live belegt in
`UNTESTED_IMPLEMENTATIONS.md` Zeile 31, U11 VERIFIED 2026-07-12), id 0x04 "SIG P228",
id 0x08 "Remington M870". Ich habe zusaetzlich die **komplette §4c-Tabelle** maschinell
gegen die Datei verglichen (79 gepruefte Eintraege: Offset + dekodierter Name):
**0 Abweichungen.**

### Skript-Bank @0x4fc6 — BESTAETIGT, Selektor selbst disassembliert

Das Dossier belegt die Relativitaet der Offsets nicht; ich habe den Selektor
nachdisassembliert (FUN_80027e68, `ghidra1_V2.txt:576990` = `DAT_800c4fc6 XREF[2]:
FUN_80027e68:80027f38(*)`):

```
80027f20: sll   v0,a2,1
80027f30: addiu at,at,20422      ; at = 0x800c4fc6
80027f34: addu  v0,v0,at
80027f38: lhu   v0,0(v0)
80027f44: addu  v0,v0,at         ; ptr = 0x800c4fc6 + u16[0x800c4fc6 + idx*2]
```

=> Offsets sind relativ zur Tabellenbasis. Tabelle = `10 00 2f 00 50 00 6b 00 8b 00
a9 00 c5 00 f8 00`, kleinster Offset 0x10 = Tabellenlaenge -> **8 Skripte: BESTAETIGT.**
Region-Ende 0x50de bestaetigt: `xxd -s 0x50be` zeigt ab 0x50de `90 00 a8 00 b9 00 ...`
= die Desc-Bank-Zeigertabelle.

**Byte-Identitaet der vier vendorten Skripte (selbst nachgerechnet):**
take == DEBUG[0] (31 B) True, full == DEBUG[1] (33 B) True, used == DEBUG[2] (27 B) True,
use == DEBUG[4] (30 B) True. Mapping `item_prompt_common.c` HEAD:17-21
(`case 2->full, case 4->use, case 5->used, default->take`) — **BESTAETIGT.**

### Port-Zitate gegen HEAD — BESTAETIGT (Zeilennummern selbst nachgezaehlt)

| Dossier-Zitat | HEAD-Wirklichkeit |
|---|---|
| `item_prompt_common.c:32-41` 0x06-Handler | :32 `if (b == 0x06)`, :34 `if ((int)item_id < RE15_ITEM_PROMPT_NIDS)` — **exakt**, kein else-Zweig |
| `gen/item_prompt_data.inc:6-8` NIDS 48 / blob[685] / off[48] | :6 / :7 / :8 — **exakt** |
| `.inc`-Header "savestate mzd_stage1_briefing.sav" | Zeile 1 — **exakt** |
| letzter Eintrag off[47] = 0x2a9 = "Nut" (id 0x2f) | nachgerechnet, stimmt |
| `item_modal_common.c:32,138,226` | :32 `static uint8_t s_type`, :138 `s_type = item_type; /* DAT_800afbb6 @0x8004335c */`, :226 `re15_item_prompt_walk(...)` — **exakt** |
| `re15_inv_screen.c:497-499` | Guard steht auf **:498**, Deref auf :499 (Funktionskopf :495). Der Bereich trifft, die Einzelzeile 497 nicht |
| `gen/inv_name_bank.inc:5` NIDS 52 | **exakt** (blob dort 733 B) |
| `gen_inv_name_bank.py` Header :6-9 | **exakt** |

### Zensus §4b — Raeume / Offsets / Typen BESTAETIGT

`aot_sce_census.py` selbst laufen lassen (Tool meldet 100 % Code-Coverage):
**164 Records mit op 0x50, 27 verschiedene Typen** — beide Zahlen BESTAETIGT.
Erster Record = `{room 1000, sub00, off 3108 (= 0xc24), slot 3, sce 9, p0 49, p1 1, p2 134}`.
Alle 12 Typen >= 48 mit Raum, Datei-Offset und amount = 1 reproduziert; zusaetzlich per
`xxd` an allen 12 Offsets die Rohbytes gegengelesen. Die 15 Typen < 48 stimmen exakt mit
der Dossier-Liste ueberein. Hoechste vergebene id = **0x47** (BESTAETIGT).

## 8.2 WIDERLEGT

### (a) "25 Records" (§4b Ueberschrift, §4b Schlusssatz, Zusammenfassung) -> **23**

Eigene Zaehlung ueber den Zensus: `2+1+2+2+4+2+1+2+2+2+2+1 = 23`.
Die **eigene Tabelle des Dossiers** summiert sich ebenfalls auf 23 — die Zahl 25 im
Fliesstext ist ein Rechenfehler, kein Datenfehler. Betroffen sind
**12 Typen / 23 Item_aot_set-Records**.

### (b) "[5] @0x800c506f (27 B)" (§3f) -> **28 B**

`0xc5 - 0xa9 = 0x1c = 28`. Rohbytes (28, selbst gezaehlt):

```
30 44 41 00 05 01 06 00 05 00 08 44 3d 4f 00 3e 41 41 4a 00 42 45 48 41 40 57 01 00
```

(Die uebrigen Laengen des Dossiers stimmen: 31/33/27/32/30/**28**/51/32.)

### (c) "80028ac4: andi a0,v0,0xff" -> tatsaechlich `andi a0,a0,0xff`

Rohbyte `ff 00 84 30`. Siehe §8.1.

### (d) "`gen_inv_name_bank.py:33`" -> `NIDS = 52` steht bei HEAD auf **Zeile 31**

(`git show HEAD:re15_port/tools/gen_inv_name_bank.py | grep -n "NIDS = 52"` -> `31:`).
Der zitierte Text stimmt woertlich, nur die Zeilennummer nicht.

### (e) Der in §3a weitergereichte Port-Kommentar-Beleg "LAB_80043500 liest `lhu block+14` / `lbu block+16`" traegt NICHT

@0x80043500 steht:

```
80043500: lui a2,0x800c
80043504: lw  a2,-16996(a2)   ; 0x800bbd9c
80043510: lh  a0,4(a2)
80043518: lhu v0,8(a2)
80043520: srl v0,v0,1
80043524: slt v1,v1,v0        ; Rechteck-/Distanztest
```

Das ist ein Rechteck-Test, kein Typ-Lesen. Der Beleg fuer pc[14] ist der Dispatcher
@0x8004300c (`addiu a0,s0,12`, s0 = pc+2) plus @0x8004333c (`lbu v0,0(a0)`) — siehe
§8.1 "NEU". Das Dossier-Ergebnis (pc[14] = Typ) ist **richtig**, seine Quellenangabe
dafuer war es nicht.

## 8.3 VERALTET — §2 beschreibt nicht mehr den Arbeitsbaum

Der Fix ist im Arbeitsbaum bereits (uncommitted) angewendet. Alle §2-Zitate treffen auf
`HEAD` zu, aber nicht mehr auf die Dateien auf der Platte:

```
 M re15_port/engine/src/gen/inv_name_bank.inc
 M re15_port/engine/src/gen/item_prompt_data.inc
 M re15_port/engine/src/item_prompt_common.c
 M re15_port/tools/gen_inv_name_bank.py
?? re15_port/tools/gen_item_prompt_data.py
```

Ist-Stand selbst nachgemessen:

- `item_prompt_data.inc`: `NIDS 102`, `name_blob[1376]` == `DEBUG.BIN[0x4a28:0x4f88]` (True),
  `name_off[102]` == `unpack("<102H", DEBUG.BIN, 0x495c)` (True), `script_blob[290]`
  == `DEBUG.BIN[0x4fc6:0x50e8]` (True), alle 8 Skripte vorhanden.
- `inv_name_bank.inc`: `RE15_INV_NAME_NIDS 102`, blob 1376 == Datei (True).
- `gen_inv_name_bank.py`: `NIDS = (0x800C4A28 - 0x800C495C) // 2` (berechnet, nicht
  hartkodiert) — Schritt 1 des Rezepts ist damit sogar sauberer als vorgeschlagen.
- `item_prompt_common.c`: Guard bleibt (mit Begruendungskommentar), aber **ohne** den in
  Schritt 3 vorgeschlagenen lauten else-Zweig. Fuer id <= 0x47 irrelevant, der stille
  Durchfall bliebe aber fuer id > 101 bestehen.

**Kleine Anmerkung zum bereits eingebauten Fix (kein Dossier-Fehler):**
`script_blob[290]` laeuft von 0x4fc6 bis 0x50e8 und damit **10 Byte ueber** das Ende der
Skript-Region 0x50de hinaus, also in die Desc-Bank-Zeigertabelle (`90 00 a8 00 b9 00 ...`).
Verhaltensneutral, weil jedes Skript vorher am Opcode `0x01` (END) bricht; sauber waere
`0x50de - 0x4fc6 = 0x118 = 280`.

## 8.4 Bewertung der Fix-Empfehlung

Das Fix-Rezept ist inhaltlich korrekt und durch die Nachdisassemblierung gedeckt:

- Die Quelle ist ein ausgeliefertes Asset (DEBUG.BIN) — belegt, nicht nur behauptet
  (Savestate-Auszug == Datei ueber die ersten 48 Eintraege / 685 Blob-Bytes, selbst geprueft).
- 102 ist **strukturell** belegt (Tabellenregion 0xCC = 204 B; lueckenlose Monotonie;
  off[101] schliesst den Blob exakt). Das Dossier sagt selbst korrekt, dass es keinen
  Code-Bound gibt — genau richtig, FUN_80028840 nennt keine Zahl.
- Verifikationsschritt §6-B ("Differenz exakt 17") ist korrekt: id 0x31 = 17 Glyphen,
  id 0x00 = 0 Glyphen (Blob[0] = 0x07 direkt).

## 8.5 Offene Punkte / NICHT GEPRUEFT (von mir)

1. **NICHT GEPRUEFT:** ob DEBUG.BIN im Auslieferungsstand auch im NORMALEN Spiel (nicht
   nur Debug-Modus) nach 0x800c0000 geladen wird. Indirekt sehr stark gestuetzt
   (Savestate-RAM == Datei), aber ich habe den Ladeaufruf nicht disassembliert.
   Naechster Weg: Xrefs auf den CD-Loader mit Ziel 0x800c0000, bzw. einen
   `stage_saves`-RAM-Dump an 0x800c495c gegen die Datei diffen.
2. **NICHT GEPRUEFT:** die Dossier-Aussage, dass ids 0x66..0xff nie vergeben werden
   (das Dossier markiert es selbst als NICHT GEPRUEFT). Mein Zensus deckt nur op 0x50;
   Maximum dort ist 0x47.
3. **NICHT GEPRUEFT:** Glyph 0x38 / 0x3b / 0x5b — das Dossier sagt korrekt NICHT GEFUNDEN
   und behauptet kein Zeichen. Ich habe ebenfalls nichts belegt.
4. **NICHT GEPRUEFT:** die Live-Verifikation (§6-C, gdigrab). Nur statisch geprueft.
