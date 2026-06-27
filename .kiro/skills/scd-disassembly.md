# SCD-Disassembly Skill

## Beschreibung

Dieser Skill übersetzt SCD-Bytecode (Script Bytecode) aus RE 1.5 RDT-Dateien in zeilenweisen Pseudo-Code. Jede Zeile enthält: Offset, Opcode-Name, numerische Parameter und eine einzeilige Beschreibung.

## Kontext laden

Beim Aktivieren dieses Skills lade folgende Quellen als Kontext:

1. **RE15_KNOWLEDGE.md §1.2** — SCD-Formatbeschreibung (Pointer-Tabelle, Opcode-Struktur)
2. **src/main/java/de/re15/extractors/SCDScriptDisassembler.java** — Vollständige Opcode-Tabelle mit Hex-Codes, Byte-Größen und Beschreibungen (Zeilen 191–333 für `registerOpcode`-Aufrufe)

## SCD-Struktur

Jede SCD-Sektion besteht aus:
```
[Pointer-Tabelle | Bytecode]
```

- **Pointer-Tabelle**: N × u16 Little-Endian Offsets. Der erste Eintrag markiert das Ende der Tabelle / Start von sub00.
- **Bytecode**: Opcodes sind 1 Byte + fixe Parameter (je nach Opcode unterschiedlich viele Bytes).

## Disassembly-Ablauf

### Schritt 1: Pointer-Tabelle lesen

1. Lies u16-LE-Werte ab Offset 0 der SCD-Sektion
2. Der erste Pointer-Wert gibt die Anzahl Sub-Routinen an: `n_subs = first_pointer / 2`
3. Jeder Pointer zeigt auf den Start einer Sub-Routine im Bytecode

### Schritt 2: Bytecode sequenziell dekodieren

Für jede Sub-Routine (sub00, sub01, ...):

1. Setze PC (Program Counter) auf den Pointer-Wert der Sub-Routine
2. Lies 1 Byte als Opcode
3. Schlage Opcode in der Opcode-Tabelle nach:
   - **Bekannt**: Lies die dokumentierte Anzahl Bytes (inkl. Opcode-Byte), dekodiere Parameter
   - **Unbekannt**: Gib Hex-Wert aus + rohe Bytes bis zum nächsten erkannten Opcode (siehe Fehlerfall)
4. Gib eine Zeile aus im Format:
   ```
   [OFFSET] OPCODE_NAME  param1, param2, ...  ; Beschreibung
   ```
5. Rücke PC um die Opcode-Länge vor
6. Wiederhole bis `Evt_end` (0x01) erreicht oder Sektionsende

### Schritt 3: Ausgabe formatieren

Ausgabeformat pro Zeile:
```
0x0000  Nop                              ; No operation
0x0001  Ifel_ck       0x0008             ; IF block start (skip 8 bytes if false)
0x0005  Ck            0x03, 0x02         ; Check flag (zone 3, bit 2)
0x0009  Cut_chg       0x05               ; Change camera to cut 5
0x000B  Endif         0x00               ; End IF block
0x000D  Evt_end       0x00               ; Exit current function
```

## Opcode-Tabelle (RE 1.5 — verifiziert)

⚠️ **WICHTIG**: RE 1.5 Opcodes unterscheiden sich ab 0x50 von Retail-RE2. Nutze IMMER diese Tabelle, NICHT RE2-Referenzen.

| Hex  | Name               | Bytes | Beschreibung |
|------|--------------------|-------|--------------|
| 0x00 | Nop                | 1     | No operation |
| 0x01 | Evt_end            | 2     | Exit current function |
| 0x02 | Evt_next           | 1     | Wait for player input |
| 0x03 | Evt_chain          | 4     | Reinitialize script execution |
| 0x04 | Evt_exec           | 4     | Execute script when condition is met |
| 0x05 | Evt_kill           | 2     | Disable event |
| 0x06 | Ifel_ck            | 4     | IF block start |
| 0x07 | Else_ck            | 4     | ELSE block start |
| 0x08 | Endif              | 2     | End IF block |
| 0x09 | Sleep              | 4     | Initialize Sleep |
| 0x0A | Sleeping           | 3     | Execute Sleep loop |
| 0x0B | Wsleep             | 1     | Initialize global sleep |
| 0x0C | Wsleeping          | 1     | Execute global sleep |
| 0x0D | For                | 6     | Initialize FOR loop |
| 0x0E | Next               | 2     | End of FOR block |
| 0x0F | While              | 4     | Start WHILE block |
| 0x10 | Ewhile             | 2     | End WHILE block |
| 0x11 | Do                 | 4     | Start DO block |
| 0x12 | Edwhile            | 2     | End DO block |
| 0x13 | Switch             | 4     | Switch statement |
| 0x14 | Case               | 6     | Case label |
| 0x15 | Default            | 4     | Default label |
| 0x16 | Eswitch            | 2     | End switch block |
| 0x17 | Goto               | 6     | Jump |
| 0x18 | Gosub              | 2     | Call subroutine |
| 0x19 | Return             | 2     | Return from Gosub |
| 0x1A | Break              | 2     | Break out of block |
| 0x1B | For2               | 6     | Secondary loop construct |
| 0x1C | Break_point        | 1     | Breakpoint |
| 0x1D | Work_copy          | 1     | Script padding / work copy stub |
| 0x1E | Nop                | 1     | NOP (padding) |
| 0x20 | Nop                | 1     | NOP (padding) |
| 0x21 | Ck                 | 4     | Check flag |
| 0x22 | Set                | 4     | Set flag |
| 0x23 | Cmp                | 6     | Compare value |
| 0x24 | Save               | 4     | Assign value |
| 0x25 | Copy               | 3     | Copy variable |
| 0x26 | Calc               | 6     | Calculate value |
| 0x27 | Calc2              | 4     | Calculate using variable operand |
| 0x28 | Sce_rnd            | 1     | Randomise scene |
| 0x29 | Cut_chg            | 2     | Change camera |
| 0x2A | Cut_old            | 1     | Previous camera |
| 0x2B | Message_on         | 4     | Show message |
| 0x2C | Aot_set            | 20    | Configure action trigger |
| 0x2D | Obj_model_set      | 34    | Configure room object model |
| 0x2E | Work_set           | 3     | Select work entity |
| 0x2F | Speed_set          | 4     | Set speed |
| 0x30 | Add_speed          | 1     | Apply speed |
| 0x31 | Add_aspeed         | 1     | Apply angular speed |
| 0x32 | Pos_set            | 8     | Set position |
| 0x33 | Dir_set            | 8     | Set direction |
| 0x34 | Member_set         | 4     | Set member value |
| 0x35 | Member_set2        | 3     | Set member (var) |
| 0x36 | Se_on              | 12    | Play sound effect |
| 0x37 | Sca_id_set         | 4     | Change camera trigger |
| 0x38 | Flr_set            | 3     | Update floor flag |
| 0x39 | Sca_floor_set      | 4     | SCA floor-byte set |
| 0x3A | Sce_espr_on        | 16    | Enable sprite animation |
| 0x3B | Door_aot_set       | 32    | Configure door trigger |
| 0x3C | Cut_auto           | 2     | Toggle automatic camera switch |
| 0x3D | Member_copy        | 4     | Copy member to variable |
| 0x3E | Member_cmp         | 6     | Compare entity member |
| 0x3F | Plc_motion         | 4     | Set motion |
| 0x40 | Plc_dest           | 8     | Move player |
| 0x41 | Plc_neck           | 10    | Set neck orientation |
| 0x42 | Plc_ret            | 1     | Return control |
| 0x43 | Plc_flg            | 4     | Set player flag |
| 0x44 | Sce_em_set         | 20    | Spawn enemy |
| 0x45 | Col_chg_set        | 3     | Set collision region |
| 0x46 | Aot_reset          | 10    | Reset action trigger |
| 0x47 | Aot_on             | 2     | Activate trigger |
| 0x48 | Super_set          | 16    | Configure super effect payload |
| 0x49 | Super_reset        | 8     | Reset super effect |
| 0x4A | Plc_gun            | 2     | Set gun state |
| 0x4B | Cut_replace        | 3     | Swap camera transition |
| 0x4C | Sce_espr_kill      | 5     | Kill sprite |
| 0x4D | Op4D               | 22    | Unknown opcode 4D payload |
| 0x4E | Item_aot_set       | 22    | Configure item trigger (legacy) |
| 0x4F | Sce_key_ck         | 4     | Check key state |
| 0x50 | Item_aot_set       | 22    | Configure item trigger |
| 0x51 | Sce_key_ck         | 4     | Check key state |
| 0x52 | Sce_espr_control   | 4     | Flag-AND condition check (RE1.5 specific) |
| 0x53 | Sce_fade_set       | 3     | Fade set (RE1.5 len 3) |
| 0x54 | Sce_bgm_control    | 6     | BGM sequence control: play/stop/pause/fade |
| 0x55 | Member_calc        | 6     | Entity member calculation |
| 0x56 | Member_calc2       | 6     | Member calc variant (RE1.5 len 6) |
| 0x57 | Sce_bgmtbl_set     | 4     | BGM table set (RE1.5 len 4) |
| 0x58 | Plc_rot            | 4     | Rotate entity |
| 0x59 | Xa_on              | 4     | Play XA audio |
| 0x5A | Weapon_chg         | 6     | Weapon change (RE1.5 len 6) |
| 0x5B | Plc_cnt            | 4     | Player count (RE1.5 len 4) |
| 0x5C | Sce_shake_on       | 4     | Screen shake (RE1.5 len 4) |
| 0x5D | Mizu_div_set       | 4     | Water division set (RE1.5 len 4) |
| 0x5E | Keep_Item_ck       | 4     | Keep item check (RE1.5 len 4) |
| 0x5F | Xa_vol             | 2     | Set XA volume |
| 0x60 | Kage_set           | 14    | Configure shadow volume |
| 0x61 | Cut_be_set         | 4     | Configure camera transition |
| 0x62 | Sce_item_lost      | 2     | Remove item from inventory |
| 0x63 | Plc_gun_eff        | 1     | Trigger gun effect |
| 0x64 | Sce_espr_on2       | 16    | Enable secondary sprite animation |
| 0x65 | Sce_espr_kill2     | 2     | Kill secondary sprite animation |
| 0x66 | Plc_stop           | 1     | Stop player movement |
| 0x67 | Aot_set_4p         | 28    | Configure wall trigger (4 point) |
| 0x68 | Door_aot_set_4p    | 40    | Configure door trigger (4 point) |
| 0x69 | Item_aot_set_4p    | 30    | Configure item trigger (4 point) |
| 0x6A | Light_pos_set      | 6     | Adjust light position |
| 0x6B | Light_kido_set     | 4     | Adjust light brightness |
| 0x6C | Rbj_reset          | 1     | Reset RBJ |
| 0x6D | Sce_scr_move       | 4     | Scroll background |
| 0x6E | Parts_set          | 6     | Set parts value |
| 0x6F | Movie_on           | 2     | Play movie |
| 0x70 | Splc_ret           | 1     | Return special cut |
| 0x71 | Splc_sce           | 1     | Special cut execute |
| 0x72 | Super_on           | 16    | Enable super effect |
| 0x73 | Mirror_set         | 8     | Configure mirror |
| 0x74 | Sce_fade_adjust    | 4     | Adjust fade settings |
| 0x75 | Sce_espr3d_on2     | 22    | Enable 3D sprite effect |
| 0x76 | Sce_item_get       | 3     | Give item to player |
| 0x77 | Sce_line_start     | 4     | Start spline line |
| 0x78 | Sce_line_main      | 6     | Configure spline line |
| 0x79 | Sce_line_end       | 1     | End spline line |
| 0x7A | Sce_parts_bomb     | 16    | Parts bomb effect |
| 0x7B | Sce_parts_down     | 16    | Parts down effect |
| 0x7C | Light_color_set    | 6     | Adjust light colour |
| 0x7D | Light_pos_set2     | 6     | Adjust light position (camera-specific) |
| 0x7E | Light_kido_set2    | 6     | Adjust light brightness (camera-specific) |
| 0x7F | Light_color_set2   | 6     | Adjust light colour (camera-specific) |
| 0x80 | Se_vol             | 2     | Set sound effect volume scale |
| 0x81 | Op81               | 3     | Comparison (unknown) |
| 0x82 | Op82               | 3     | Unknown opcode |
| 0x83 | Op83               | 1     | Unknown opcode |
| 0x84 | Op84               | 2     | Unknown opcode |
| 0x85 | Op85               | 6     | Unknown opcode |
| 0x86 | Poison_ck          | 1     | Check poison status |
| 0x87 | Poison_clr         | 1     | Clear poison status |
| 0x88 | Sce_item_ck_lost   | 3     | Check/remove item |
| 0x89 | Op89               | 1     | Unknown opcode |
| 0x8A | Nop8A              | 6     | Padding (6 bytes) |
| 0x8B | Nop8B              | 6     | Padding (6 bytes) |
| 0x8C | Nop8C              | 1     | Padding / alignment (1 byte) |
| 0x8D | Op8D               | 24    | Room object payload |
| 0x8E | Nop8E              | 2     | No-op / alignment |

## Fehlerbehandlung: Unbekannte Opcodes

Wenn ein Opcode-Byte NICHT in der obigen Tabelle enthalten ist:

1. Gib den Opcode als unbekannt aus mit seinem Hex-Wert
2. Zeige die rohen Bytes ab dem unbekannten Opcode (max. 16 Bytes als Hex-Dump)
3. Versuche den nächsten bekannten Opcode zu finden durch vorwärts-Scannen
4. Ausgabeformat:
   ```
   0x0042  UNKNOWN_0xAB  [AB 03 FF 12 00 ...]  ; Unbekannter Opcode 0xAB, 5 rohe Bytes
   ```

## Parameter-Dekodierung

### Allgemeine Regeln
- Alle Multi-Byte-Werte sind **Little-Endian**
- s16 = signed 16-bit (für Koordinaten, Winkel)
- u8 = unsigned 8-bit (für IDs, Flags)
- u16 = unsigned 16-bit (für Offsets, Größen)

### Wichtige Opcode-Parameter im Detail

**Ck (0x21, 4 Bytes)**: `[0x21] [zone] [bit_high] [bit_low]`
- zone: Flag-Zone (0=System, 1=Room, 2=Scenario, 3=Item, 4=Map, 5=Run, 6=Status, 7=Item2)
- bit: bit_high<<8 | bit_low = Flag-Bit-Index

**Set (0x22, 4 Bytes)**: `[0x22] [zone] [bit_high] [bit_low]`
- Gleiche Struktur wie Ck, setzt statt zu prüfen

**Ifel_ck (0x06, 4 Bytes)**: `[0x06] [pad] [size_high] [size_low]`
- size: Sprungweite in Bytes wenn Bedingung falsch

**Door_aot_set_4p (0x68, 40 Bytes)**:
```
[0x68] [aot_slot] [floor_band] [pad]
[trigger_x0 s16] [trigger_z0 s16]
[trigger_x1 s16] [trigger_z1 s16]
[trigger_x2 s16] [trigger_z2 s16]
[trigger_x3 s16] [trigger_z3 s16]
[spawn_x s16] [spawn_y s16] [spawn_z s16]
[spawn_rotation s16]
[dest_stage u8] [dest_room u8]
[dest_cut u8] [reserved u8] [reserved u8] [reserved u8]
```

**Aot_set (0x2C, 20 Bytes)**:
```
[0x2C] [aot_slot] [type] [floor_band]
[rect_x s16] [rect_z s16] [rect_w s16] [rect_h s16]
[data0 u16] [data1 u16] [data2 u16] [data3 u16]
```

**Se_on (0x36, 12 Bytes)**:
```
[0x36] [pad] [bank u8] [sample_id u8]
[volume u8] [pan u8]
[x s16] [y s16] [z s16]
```

**Sce_em_set (0x44, 20 Bytes)**:
```
[0x44] [pad] [em_slot u8] [em_type u8]
[pose u8] [floor u8]
[x s16] [y s16] [z s16]
[rotation s16]
[ai_type u16] [sound_flag u16]
```

## Anwendungsbeispiele

### Beispiel 1: Einfaches Raum-Skript disassemblieren

**Eingabe**: "Disassembliere das mainScd von ROOM1170"

**Vorgehen**:
1. Lade die RDT-Datei (`STAGE1/ROOM1170.RDT`)
2. Lese den mainScd-Offset aus der RDT-Adresstabelle (Offset 0x40)
3. Parse die Pointer-Tabelle der SCD-Sektion
4. Dekodiere jeden Opcode sequenziell ab sub00

### Beispiel 2: Türen in einem Raum finden

**Eingabe**: "Welche Türen hat ROOM1140?"

**Vorgehen**:
1. Disassembliere mainScd + subScd von ROOM1140
2. Filtere nach Opcodes `Door_aot_set_4p` (0x68) und `Door_aot_set` (0x3B)
3. Dekodiere Ziel-Stage, Ziel-Room, Spawn-Position für jede Tür

### Beispiel 3: Unbekannten Opcode melden

**Ausgabe bei unbekanntem Byte 0x9F**:
```
0x0123  UNKNOWN_0x9F  [9F 44 00 12 88 03]  ; Unbekannter Opcode 0x9F, rohe Bytes bis nächster bekannter Opcode
0x0129  Evt_end       0x00                  ; Exit current function
```

## Hinweise

- Die Opcode-Tabelle ab 0x50 unterscheidet sich zwischen RE1.5 und Retail-RE2. Verwende IMMER die hier dokumentierte RE1.5-Tabelle.
- `Obj_model_set` (0x2D) hat in RE1.5 eine fixe Größe von 34 Bytes (im Gegensatz zu älteren Annahmen von variabler Größe).
- Manche Opcodes (0x80–0x8E) sind nur teilweise identifiziert — Größen sind verifiziert, Funktion teilweise unbekannt.
- Der Java-Disassembler (`SCDScriptDisassembler.java`) enthält zusätzliche Logik für Message-Auflösung und C-Code-Generierung, die über diesen Skill hinausgeht.
