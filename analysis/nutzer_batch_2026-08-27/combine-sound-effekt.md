# Combine: fehlender Sound + fehlender Effekt (RE1.5 vs RE2-Retail)

Status: **GELOEST** fuer den Sound (Ursache + RE2-Vorbild + RE1.5-Sample belegt).
Fuer den "Effekt" lautet der Befund: **der Effekt fehlt NICHT** — RE1.5 hat einen eigenen,
gerenderten 17-Frame-Puls; RE2 hat stattdessen einen 10-Frame-Slide. Details in §5.

## 1. Symptom (Nutzer-Wortlaut)

> "Beim Combine von Items fehlen Sound und Effekte. Das liegt daran dass 1.5 noch eine Beta ist.
> Schaue wie das echte RE2 das macht und ruest Sound und Effekt nach."

## 2. Was der PORT heute tut

Der Port ist an dieser Stelle eine **korrekte, byte-true Kopie des RE1.5-Originals** — er
schweigt, weil das Original schweigt. Sein eigener Kommentar sagt es bereits:

`re15_port/engine/src/menu_common.c:1054`:

    *     (state 6 slide-out). NO SE anywhere in 0x8004b37c-b404 (zero jal 0x80045024 —
    *     the EXCHANGE cancel/confirm are SILENT, unlike the command stage's SE(4,5)/(4,6));

`re15_port/engine/src/menu_common.c:1060-1082` (`state7_select`) — der Erfolgspfad, ohne jeden
SE-Aufruf:

    static void state7_select(uint16_t pressed, uint16_t held)
    {
        second_cursor_move(held);                    /* jal 0x80048904 @0x8004b384 */
        if (re15_pad_virtual_word(pressed) & 0x8000) {   /* CANCEL */
            g_inv_screen.item_state = 6;             /* @0x8004b3f0-f4 (silent) */
            return;
        }
        if (re15_pad_virtual_word(pressed) & 0x4000) {   /* CONFIRM */
            int action = exchange_match();           /* jal 0x8004e900 @0x8004b3b0 */
            int ret = exchange_exec(action);         /* jal 0x8004e054 @0x8004b3b8 */
            if (ret != 0) {                          /* bne @0x8004b3c4 */
                re15_inv_compact();                  /* jal 0x8004dadc @0x8004b3cc */
                ...
                s_c4 = 0;                            /* sb zero -> 25c4 @0x8004b3dc */
                s_c3 = 1;                            /* sb 1 -> 25c3 @0x8004b3e4 */
            } else {
                g_inv_screen.item_state = 6;         /* @0x8004b3f0-f4 */
            }
        }
    }

Die Port-SE-Helferfunktion existiert bereits — `re15_port/engine/src/menu_common.c:110`:

    /* SE(4,id): FUN_80045024(0x04<<24 | id<<16) = CORE00 bank record id. */
    static void se4(int id) { re15_audio_core_se(id); }

## 3. Was das ORIGINAL RE1.5 tut — BELEGT: KEIN Sound auf dem Combine-Erfolgspfad

### 3.1 Inventar-Hauptzustandsmaschine `FUN_8004a0cc`, State-Byte `@0x800b25c2`

`RE_15_Quellcode_V2/FUN_8004a0cc.c:146` — case 7 ruft die Combine-Engine:

    case '\a':
      FUN_8004b33c();
      break;

Damit ist der Anker "Inventar-Item-State 7 = EXCHANGE/combine @0x8004b33c" VERIFIZIERT.

### 3.2 `FUN_8004b33c` ist nur ein 2-Wege-Dispatcher ueber `@0x800b25c3`

`dis 0x8004b33c`:

    8004b33c: lui  v0,0x800b
    8004b340: lbu  v0,9667(v0)          ; 0x800b25c3  = combine-substate
    8004b34c: sll  v0,v0,2
    8004b350: lui  at,0x8007
    8004b354: addiu at,at,19524         ; 0x80074c44  = handler table
    8004b358: addu at,at,v0
    8004b35c: lw   v0,0(at)
    8004b364: jalr v0

`table 0x80074c44`:

    [ 0] 0x80074c44 -> 0x8004b37c   EXE   ; substate 0 = Auswahl/Confirm
    [ 1] 0x80074c48 -> 0x8004b408   EXE   ; substate 1 = Ergebnis-Walker
    [ 2] 0x80074c4c -> 0x000d000c   data  ; ab hier KEINE Pointer mehr -> nur 2 Substates

### 3.3 Substate 0 = `0x8004b37c` — der Erfolgspfad, Instruktion fuer Instruktion

`dis 0x8004b37c`:

    8004b37c: addiu sp,sp,-24
    8004b380: sw   ra,16(sp)
    8004b384: jal  0x80048904          ; Cursor-Bewegung im Exchange-Screen
    8004b38c: lui  v1,0x800b
    8004b390: lw   v1,-14484(v1)       ; 0x800ac76c = PAD PRESS-EDGE (virtuell)
    8004b398: andi v0,v1,0x8000        ; CANCEL  (physisch X)
    8004b39c: bne  v0,zero,0x8004b3f0
    8004b3a0: ori  v0,zero,0x6         ;   -> state 6 (zurueck)   ** KEIN SE **
    8004b3a4: andi v0,v1,0x4000        ; CONFIRM (physisch [], @0x80073dbc[14])
    8004b3a8: beq  v0,zero,0x8004b3f8  ;   nichts gedrueckt -> return
    8004b3b0: jal  0x8004e900          ; Paar-Lookup: liefert Combine-Aktion in v0
    8004b3b8: jal  0x8004e054          ; APPLY (a0 = aktion & 0xff)
    8004b3bc: andi a0,v0,0xff
    8004b3c0: andi v0,v0,0xff
    8004b3c4: beq  v0,zero,0x8004b3f0  ; 0 = kein gueltiges Paar -> state 6  ** KEIN SE **
    8004b3c8: ori  v0,zero,0x6
    8004b3cc: jal  0x8004dadc          ; Inventar kompaktieren
    8004b3d4: ori  v0,zero,0x1
    8004b3dc: sb   zero,9668(at)       ; 0x800b25c4 = Walker-Frame := 0
    8004b3e4: sb   v0,9667(at)         ; 0x800b25c3 := 1  -> Ergebnis-Walker  ** KEIN SE **
    8004b3f4: sb   v0,9666(at)         ; 0x800b25c2 := 6
    8004b400: jr   ra

**ERFOLGSPFAD = genau 4 `jal`: 0x80048904, 0x8004e900, 0x8004e054, 0x8004dadc.**

### 3.4 Vollstaendige Call-Huelle des Erfolgspfads (rekursiv, jeder `jal` aufgelistet)

    0x8004b37c  (35 instr)  -> 80048904, 8004e900, 8004e054, 8004dadc
    0x8004b408  (88 instr)  -> (KEIN jal/jalr)          <-- der Ergebnis-Walker ruft NICHTS
    0x8004e900  (91 instr)  -> (KEIN jal/jalr)          <-- Paarlisten-Lookup
    0x8004e054  (300 instr) -> 80013b60, 800492b8 x2, 8004947c
    0x8004dadc  (92 instr)  -> 8004df2c, 80049390, 8004947c
    0x8004df2c  (25 instr)  -> (KEIN jal/jalr)
    0x8004947c  (42 instr)  -> 80068d50, 80068a60        (libgpu)
    0x800492b8  (54 instr)  -> 80068c88, 80068a60        (libgpu LoadImage)
    0x80013b60  (60 instr)  -> 80013f80, 80013df0 x3, 80029ac8, 80013fdc   (CD-Loader)

Identifikation der Blaetter — kein Audio darunter:
- `0x80068c88` = **LoadImage** (libgpu) — `RE15_FUN_CATALOG.md:171`
- `0x80068a60`, `0x80068d50` = libgpu-Wrapper (Format-Strings `@0x80011b58` / `@0x80011bb4`)
- `0x80013b60` = **CD-Datei-Loader per File-ID** — belegt in `analysis/leon_injured_model.md:15`.
  Der Aufruf `FUN_80013b60(0x19,&DAT_801a0000,0)` in `FUN_8004e054.c:32` laedt das
  **Icon-Blatt**, nicht Sound (Ziel `0x801a0000`, spaeter `+ (DAT_800b260c-1)*0x4b0` = 1200-Byte-Icon).
- `0x80049390` = Icon-RECT-Bauer (`ori v0,zero,0x14` @0x800493ac / `ori v0,zero,0x1e` @0x800493b4
  = 20x30-Icon)

**=> Auf dem gesamten Combine-Erfolgspfad steht KEIN einziger Aufruf der Sound-Routine.**

### 3.5 Die RE1.5-Sound-Routine — und der Beweis, dass das Inventar sie SCHON benutzt

`FUN_80045024(uint packed, void *pos)` = **Se_on**. `RE_15_Quellcode_V2/FUN_80045024.c:1`:

    void FUN_80045024(uint param_1,undefined4 param_2)
    {
      uVar7  = param_1 >> 0x18;                        // BANK
      uVar11 = (uint)(char)(&DAT_800b21ec)[uVar7];     // Bank geladen? -1 = nein -> return
      if (uVar11 == 0xffffffff) return;
      uVar16 = param_1 >> 0x10 & 0xff;                 // SAMPLE-/RECORD-INDEX
      switch(uVar7) {
      ...
      case 4: if (0x20 < uVar16) return; puVar9 = &DAT_801fbd00; break;   // <-- MENUE-BANK "CORE"

Argument-Konvention: **`a0 = (bank<<24) | (record<<16) | low`, `a1 = 3D-Position oder 0 fuer UI**.
Deckt sich mit `RE15_FUN_CATALOG.md:185`.

Bank 4 wird als VAB geladen — `RE_15_Quellcode_V2/FUN_800440c4.c:10-16`:

    iVar2 = FUN_80013b60(*(undefined2 *)(&DAT_80073a88 + (param_1 & 0xff) * 2),&DAT_801fbd00,1,
                         "CORE EDH");
    DAT_800b25a8 = &DAT_801fbd00 + *(int *)(&DAT_801fbcf8 + iVar2);
    sVar1 = SsVabOpenHeadSticky(DAT_800b25a8,4,0x38840);     // <-- vabId 4 == Se_on-Bank 4

Datei-ID-Tabellen (`read --w 2`):

    0x80073a88 ("CORE EDH"): [161, 163, 165, 167, 169, 171, 173, 175, 177, 179, ...]
    0x80073ab0 ("CORE VBD"): [162, 164, 166, 168, 170, 172, 174, 176, 178, 180, ...]

**Alle Se_on-Stellen des RE1.5-Inventars** (`dis 0x8004a0cc 400`, gefiltert auf `jal 0x80045024`):

    8004a154: lui a0,0x409 / 8004a158: jal 0x80045024   ; Sonderfall (DAT_800ac762 & 0x100)
    8004a478: lui a0,0x404 / 8004a47c: jal 0x80045024   ; Cursor UP
    8004a4a0: lui a0,0x404 / 8004a4a4: jal 0x80045024   ; Cursor DOWN
    8004a4c8: lui a0,0x404 / 8004a4cc: jal 0x80045024   ; Cursor LEFT
    8004a4f0: lui a0,0x404 / 8004a4f4: jal 0x80045024   ; Cursor RIGHT
    8004a51c: lui a0,0x406 / 8004a520: jal 0x80045024   ; CONFIRM  (Gate @0x8004a510 andi 0x4000)
    8004a660: lui a0,0x405 / 8004a664: jal 0x80045024   ; CANCEL

**Damit ist bewiesen:**
1. Die richtige Routine ist `FUN_80045024` @0x80045024, Aufruf mit `a1 = 0` (UI, nicht positional).
2. Bank 4 ("CORE") ist im Inventar geladen und hoerbar in Benutzung (Cursor/Confirm/Cancel).
3. Der Combine-Erfolgspfad (`0x8004b37c`) ruft sie NICHT — die Luecke ist der **fehlende Aufruf**,
   nicht ein fehlendes Sample.

## 4. Was RE2-RETAIL an derselben Stelle tut — mit Sound

### 4.1 Zuordnung der Engine-Funktionen (RE1.5 <-> RE2), strukturell belegt

RE2s Se_on ist `FUN_8005ba28` — bewiesen durch die **identische** gepackte Argumentform:
`grep -rhoE "FUN_8005ba28\(0x4[0-9a-f]{6},0\)" RE2_Quellcode_V2/*.c` liefert `0x4040000`, `0x4050000`
— dieselben Bank-4-Records wie RE1.5.

RE2s Inventar-Array liegt bei `0x800d4a3c` (id) / `0x800d4a3d` (Menge) / `0x800d4a3e` (Flags),
Stride 4 — exakte Entsprechung zu RE1.5s `0x800b10ac/ad/ae`
(Beleg: `RE2_Quellcode_V2/FUN_8003a9f4.c:106-108`).

| Rolle | RE1.5 | RE2 |
|---|---|---|
| Exchange/Combine-Maschine | `0x8004b33c` (2 Substates) | `FUN_8006b358` (12 States, `sltiu a0,0xc` @0x8006b3e8) |
| Cursor A / Cursor B | `0x800b25bd` / `0x800b25be` | `0x800d5bfc` / `0x800d5bfd` |
| Gueltigkeits-Lookup | `FUN_8004e900` | `FUN_800695b0` |
| Item-Prop-Tabelle | `0x80074da8`, Stride 0xc | `0x800a9e1c`, Stride 8 |
| Paar-Record | Stride 4 `{partner,result,action,pic}` | Stride 4 `{partner,action,result,pic}` |
| Se_on | `FUN_80045024` | `FUN_8005ba28` |

`FUN_800695b0` ist der strukturelle Zwilling von `FUN_8004e900` (`RE2_Quellcode_V2/FUN_800695b0.c`):

    pcVar1 = (&PTR_DAT_800a9e20)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 2];
    do {
      uVar2 = uVar2 + 1;
      if (*pcVar1 == (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4]) {
        DAT_800d5c16 = pcVar1[2];      // result item
        DAT_800d5c17 = pcVar1[3];      // icon page
        return pcVar1[1];              // action / next state   (0 = ungueltig)
      }
      pcVar1 = pcVar1 + 4;
    } while (uVar2 < (byte)(&DAT_800a9e1f)[...*8]);
    return '\0';

### 4.2 DER SOUND-AUFRUF — RE2 `@0x8006b58c-0x8006b5c0`, roh disassembliert

`re2_disasm.py dis 0x8006b56c 36` (Binary `info/re2leon/PSX.EXE`), `s3 = 0x800cc1e8`,
`s1 = 0x800d5bf0`:

    8006b57c: beq  a2,v0,0x8006b58c   ; Cursor unveraendert? -> SE ueberspringen
    8006b580: lui  a0,0x404           ;   sonst a0 = 0x04040000  CURSOR-MOVE
    8006b584: jal  0x8005ba28         ;   Se_on
    8006b588: addu a1,zero,zero       ;   a1 = 0

    8006b58c: lw   v1,8488(s3)        ; 0x800ce310 = PAD PRESS-EDGE
    8006b594: andi v0,v1,0x1000       ; CONFIRM
    8006b598: beq  v0,zero,0x8006b5cc ;   nicht gedrueckt -> CANCEL pruefen
    8006b59c: andi v0,v1,0x2000       ;   (Delay-Slot) CANCEL vorbereiten
    8006b5a0: jal  0x800695b0         ; Gueltigkeit / Folge-State
    8006b5a8: sb   v0,4(s1)           ; 0x800d5bf4 := Ergebnis (State)
    8006b5ac: andi v0,v0,0xff
    8006b5b0: beq  v0,zero,0x8006b5bc ; Ergebnis == 0 -> UNGUELTIG
    8006b5b4: lui  a0,0x407           ; DELAY-SLOT, IMMER ausgefuehrt: a0 = 0x04070000  FEHLER
    8006b5b8: lui  a0,0x406           ; nur wenn Ergebnis != 0:        a0 = 0x04060000  ERFOLG
    8006b5bc: jal  0x8005ba28         ; Se_on
    8006b5c0: addu a1,zero,zero       ; a1 = 0

    8006b5cc: beq  v0,zero,0x8006b5ec ; CANCEL nicht gedrueckt -> raus
    8006b5d0: lui  a0,0x405           ; a0 = 0x04050000  ABBRUCH
    8006b5d4: jal  0x8005ba28         ; Se_on
    8006b5d8: addu a1,zero,zero       ; a1 = 0

Der `lui a0,0x407` @0x8006b5b4 steht im **Branch-Delay-Slot** des `beq` @0x8006b5b0 und wird
deshalb IMMER ausgefuehrt; faellt der Branch nicht (Ergebnis != 0), ueberschreibt
`lui a0,0x406` @0x8006b5b8 ihn wieder. Das ist das MIPS-Standardidiom fuer "if/else auf ein Register".

**Ergebnis (byte-belegt):**

| Ereignis im RE2-Exchange-Screen | Se_on-Argument | Bank/Record | Adresse |
|---|---|---|---|
| Zweiter Cursor bewegt sich | `0x04040000` | Bank 4, Record 4 | `@0x8006b580-84` |
| CONFIRM, Paar **gueltig** | `0x04060000` | Bank 4, Record **6** | `@0x8006b5b8`+`@0x8006b5bc` |
| CONFIRM, Paar **ungueltig** | `0x04070000` | Bank 4, Record **7** | `@0x8006b5b4`+`@0x8006b5bc` |
| CANCEL | `0x04050000` | Bank 4, Record 5 | `@0x8006b5d0-d4` |

(Ghidra hat 0x406/0x407 NICHT konstant-gefaltet — im Decompilat stehen sie als Register `uVar6`
(`RE2_Quellcode_V2/FUN_8006b358.c:66-70`). Nur die Roh-Disassembly zeigt die Werte. Deshalb ist
die `grep`-Liste in §4.1 unvollstaendig und die Disasm oben massgeblich.)

## 5. Der "Effekt" — der Befund weicht vom Nutzer-Eindruck ab

### 5.1 RE1.5 HAT einen Combine-Effekt, und er wird gerendert

Substate 1 = `0x8004b408`, Sprungtabelle `@0x80010ff4`, Gate `sltiu v0,v1,0x11` = **17 Frames**:

    8004b408: lui  v1,0x800b
    8004b40c: lbu  v1,9668(v1)      ; 0x800b25c4 = Walker-Frame
    8004b414: sltiu v0,v1,0x11      ; < 17
    8004b41c: sll  v0,v1,2
    8004b424: addiu at,at,4084      ; 0x80010ff4 = Frame-Handler-Tabelle
    8004b434: jr   v0

`table 0x80010ff4 17`:

    [ 0.. 7] -> 0x8004b43c    ; Handler A
    [ 8..15] -> 0x8004b4a8    ; Handler B
    [16]     -> 0x8004b524    ; Terminal

Handler A `@0x8004b43c` (Rechteck zusammenziehen):

    8004b444: lbu  a2,0(a3)         ; 0x800b25d0
    8004b44c: lbu  a0,9681(a0)      ; 0x800b25d1
    8004b454: lbu  a1,9682(a1)      ; 0x800b25d2
    8004b45c: lbu  v0,9683(v0)      ; 0x800b25d3
    8004b468: addiu a2,a2,1         ; d0 += 1
    8004b46c: addiu a0,a0,1         ; d1 += 1
    8004b470: addiu a1,a1,-1        ; d2 -= 1
    8004b474: addiu v0,v0,-1        ; d3 -= 1
    8004b478: addiu v1,v1,1         ; frame += 1

Handler B `@0x8004b4a8` spiegelt das (d0/d1 -1, d2/d3 +1, `@0x8004b4d8-e8`) und kopiert
zusaetzlich jeden Frame `25bd -> 25be` (`lbu @0x8004b4d4`, `sb @0x8004b4f0`).

**Diese Register werden gezeichnet** — `RE_15_Quellcode_V2/FUN_80047648.c:141-149`:

    psVar7[-1] = (short)DAT_800b25d0 + (&DAT_80076274)[(uint)DAT_800b25be * 2] + ...
    cVar2 = DAT_800b25d1;
    psVar7[-1] = (short)DAT_800b25d2 + (&DAT_80076274)[(uint)DAT_800b25be * 2] + ...
    cVar2 = DAT_800b25d3;

d.h. die vier Jitter-Bytes verschieben die Ecken des Ergebnis-Sprites relativ zur Zelle des
ZWEITEN Cursors (`DAT_800b25be`). Der Port macht das ebenfalls —
`re15_port/engine/src/re15_inv_screen.c:1192-1197`:

    int jx = (i == 0) ? (int8_t)st->comb_d0 : (int8_t)st->comb_d2;
    int jy = (i == 0) ? (int8_t)st->comb_d1 : (int8_t)st->comb_d3;
    ...
    bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u) + x + st->list_x + jx,

**=> Der RE1.5-Combine-Effekt existiert, ist 17 Frames lang (8 zusammenziehen / 8 aufziehen /
1 Terminal) und ist im Port korrekt implementiert und sichtbar.**

### 5.2 RE2 hat einen ANDEREN Effekt: einen 10-Frame-Slide

`RE2_Quellcode_V2/FUN_8006b358.c:75-99`, State 1:

    case '\x01':
      if (9 < DAT_800d5bf5) { ... Abschluss ... }          // 10 Frames
      DAT_800d5c0e = DAT_800d5c0e + DAT_800d5c0c;          // prim0 X += dx
      DAT_800d5c0f = DAT_800d5c0f + DAT_800d5c0d;          // prim0 Y += dy
      DAT_800d5c12 = DAT_800d5c12 + DAT_800d5c10;          // prim1 X += dx'
      DAT_800d5c13 = DAT_800d5c13 + DAT_800d5c11;          // prim1 Y += dy'
      DAT_800d5bf5 = DAT_800d5bf5 + 1;

Die Deltas setzt `FUN_8006a38c` (`RE2_Quellcode_V2/FUN_8006a38c.c`) aus der Slot-Geometrie:

    DAT_800d5c0c ∈ { 0, 2, 4, 0xfe(-2), 0xfc(-4) }        // X-Schritt je Frame
    DAT_800d5c0d = ((DAT_800d5bfd >> 1) - (DAT_800d5bfc >> 1)) * '\x03';   // Y-Schritt je Frame

(`>>1` = Zeile im 2-spaltigen Raster, `&1` = Spalte.) Ueber 10 Frames ergibt das eine
Gesamtverschiebung von `10*dx` / `30*(Zeilendifferenz)` — das Icon **wandert von Slot A nach
Slot B**, statt an Ort und Stelle zu pulsieren.

### 5.3 Konsequenz — ehrlich benannt

- **Sound: fehlt wirklich.** RE2 spielt an genau der Stelle 2 verschiedene SEs; RE1.5 keinen.
- **Effekt: fehlt NICHT.** RE1.5 hat einen vollstaendigen, gerenderten 17-Frame-Puls.
  RE2s Slide waere ein **Ersatz**, kein Zusatz. Ihn einzubauen entfernt den Original-Puls.
  Das ist eine Design-Entscheidung des Nutzers, keine Bug-Behebung — deshalb steht sie in §7
  und nicht im Fix-Rezept.

Wahrscheinlichste Erklaerung des Nutzer-Eindrucks "Effekte fehlen": ohne den quittierenden
Sound wirkt der stille 17-Frame-Puls wie "es ist nichts passiert".

## 6. Sound-ID-Mapping RE2 -> RE1.5: Record 7 EXISTIERT in RE1.5

Der Port laedt Bank 4 als `SOUND/CORE%02X.EDH` + `.VB`; die Record-Zahl ist
`pbav/4` (`re15_port/platform/pc/src/audio_pc.c:912-940`).

Gemessen an `re15_port/shared_assets/PSX/SOUND/CORE00.EDH` (3176 Byte,
`pbav = u32 @ size-8 = 0x40`, Magic bei 0x40 = `pBAV`, also **16 Record-Slots**):

    record  4 (Se_on 0x40040000): 00 00 53 00      <- Cursor   (RE1.5 nutzt es, @0x8004a478 ff.)
    record  5 (Se_on 0x40050000): 00 00 63 01      <- Cancel   (RE1.5 nutzt es, @0x8004a660)
    record  6 (Se_on 0x40060000): 00 00 73 01      <- Confirm  (RE1.5 nutzt es, @0x8004a51c)
    record  7 (Se_on 0x40070000): 00 00 83 00      <- FREI, gueltig — RE2s Fehler-Beep
    record 11 (Se_on 0x400B0000): ff ff ff ff      <- erster Leer-Record

Ueber **alle** CORE-Baenke geprueft (Leer-Records = `ff ff ff ff`):

    CORE00..CORE0D, CORE0F : Leer-Records nur bei [11,12,13,14,15]  -> Record 7 GUELTIG
    CORE0E                 : keine Leer-Records                     -> Record 7 GUELTIG
    CORE10..CORE13         : Leer bei [1,2,3,7,8,9,10,...]          -> Record 7 LEER

CORE10-13 sind die Sonder-/Cutscene-Baenke (VB 200+ KB statt ~40 KB); waehrend des normalen
Spiels — und damit im Inventar — ist eine der Ingame-Baenke CORE00..CORE0F aktiv, in denen
Record 7 durchgaengig gueltig ist. Der Port setzt CORE00 als Default
(`audio_pc.c:962`: `if (!s_core_loaded && load_core_se_vab_pc(0) != 0) return;`).

Sollte doch eine Bank ohne Record 7 aktiv sein, ist das **ungefaehrlich**: `se_play_layers`
bricht bei einem Leer-Record ab (`audio_pc.c:687`: `if (re15_edt_decode(...) != 0 || rec.empty) return;`)
— genau wie das Original bei einem `ff`-Record.

**=> Es wird KEIN RE2-Asset und KEIN RE2->RE1.5-Sound-Mapper gebraucht. Die Fix-IDs sind
Records der RE1.5-eigenen CORE-Bank.** (Ein Plattform-Sound-Mapper existiert im Port auch nicht;
`grep re2_sound_map|re2_se_map|platform_sound` in `engine/src`+`include` = 0 Treffer. Der
vorhandene RE2-Audio-Pfad `re15_audio_re2_enemy_se` ist die ENEMSE-Gegnerbank und hier irrelevant.)

## 7. Fix-Rezept fuer den Implementierer

Alle Aenderungen in **`re15_port/engine/src/menu_common.c`**, Funktion `state7_select()`
(ab Zeile 1060). Die SE-Helferfunktion `se4(int)` (Zeile 110) existiert bereits; ein zusaetzlicher
`#include` ist nicht noetig (`re15_audio.h` ist ab Zeile 62 eingebunden).

**Reihenfolge byte-true zu RE2:** RE2 ermittelt erst die Gueltigkeit (`jal 0x800695b0` @0x8006b5a0),
schreibt dann den Folgezustand (`sb v0,4(s1)` @0x8006b5a8) und spielt DANN den SE
(`jal 0x8005ba28` @0x8006b5bc) — die Ausfuehrung passiert erst im Folgeframe. Im Port laeuft
`exchange_exec()` im selben Frame; der naechstliegende Einbau ist deshalb: SE direkt nach
`exchange_match()` bestimmen. Das ist zulaessig, weil `exchange_exec` denselben Wert
zurueckgibt wie `exchange_match` (Tail `@0x8004e8d8: v0 = andi s5,0xff`), die Fallunterscheidung
also identisch ist.

### Aenderung 1 — CONFIRM: Erfolg/Fehler-SE (Kern des Findings)

Im `if (... & 0x4000)`-Zweig, zwischen `exchange_match()` und `exchange_exec()`:

    int action = exchange_match();           /* jal 0x8004e900 @0x8004b3b0 */
    /* RE2-Nachruestung (RE1.5 ist hier stumm — 0 jal 0x80045024 in 0x8004b37c-b404).
     * Vorbild RE2 @0x8006b5b0-c0: beq v0,zero -> Delay-Slot `lui a0,0x407` (FEHLER),
     * sonst `lui a0,0x406` (ERFOLG); jal Se_on @0x8006b5bc mit a1=0.
     * Records in RE1.5s eigener CORE-Bank: 6 = `00 00 73 01`, 7 = `00 00 83 00`
     * (SOUND/CORE00.EDH, Byte-Offset 0x18 bzw. 0x1c). */
    se4(action != 0 ? 6 : 7);
    int ret = exchange_exec(action);         /* jal 0x8004e054 @0x8004b3b8 */

### Aenderung 2 — CANCEL-SE

Im `if (... & 0x8000)`-Zweig, vor `g_inv_screen.item_state = 6;`:

    /* RE2 @0x8006b5d0-d4: lui a0,0x405 ; jal 0x8005ba28 ; a1=0.
     * Record 5 = `00 00 63 01` (SOUND/CORE00.EDH @0x14). RE1.5 nutzt denselben
     * Record fuer Menue-Abbruch @0x8004a660. */
    se4(5);

### Aenderung 3 — Cursor-Bewegungs-SE im Exchange-Screen

RE2 spielt Record 4, wenn sich der zweite Cursor bewegt (`beq a2,v0` @0x8006b57c ->
`lui a0,0x404` @0x8006b580). RE1.5s `FUN_80048904` ist stumm (0 `jal` in der Funktion).
In `state7_select()` um den Cursor-Aufruf herum:

    uint8_t before = g_inv_screen.second_cursor;
    second_cursor_move(held);                /* jal 0x80048904 @0x8004b384 */
    /* RE2 @0x8006b57c-84: nur bei tatsaechlicher Aenderung. Record 4 = `00 00 53 00`
     * (SOUND/CORE00.EDH @0x10); RE1.5 nutzt ihn fuer Menue-Cursor @0x8004a478/4a0/4c8/4f0. */
    if (g_inv_screen.second_cursor != before) se4(4);

### NICHT im Fix-Rezept (bewusst)

Der RE2-**Slide**-Effekt (§5.2) wird NICHT nachgeruestet: er wuerde den funktionierenden
RE1.5-17-Frame-Puls (§5.1) ersetzen, nicht ergaenzen. Falls der Nutzer ihn ausdruecklich will,
sind die Belege in §5.2 vollstaendig (10 Frames `@FUN_8006b358` case 1; Deltas
`@FUN_8006a38c`: X ∈ {0,±2,±4}, Y = `(rowB-rowA)*3`), aber das ist eine bewusste Abweichung
vom Original und braucht eine Nutzer-Entscheidung.

## 8. Wie man es verifiziert

1. **Statische Gegenprobe (kostenlos, sofort):**

       python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x8004b37c 36
       python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py  dis 0x8006b56c 36

   Erwartung: links kein `jal 0x80045024`; rechts `lui a0,0x407` / `lui a0,0x406` /
   `jal 0x8005ba28` an den in §4.2 genannten Adressen.

2. **Sample-Existenz:** `CORE00.EDH` Byte-Offset `0x1c..0x1f` muss `00 00 83 00` sein
   (nicht `ff ff ff ff`). Einzeiler:

       python -c "d=open(r're15_port/shared_assets/PSX/SOUND/CORE00.EDH','rb').read(); print(d[0x1c:0x20].hex())"

3. **Hoerprobe im Port:** `RE15_SE_DEBUG=1` setzen (`audio_pc.c:692`) und im Inventar einen
   Combine ausfuehren. Erwartete stderr-Zeilen: `[se] Stimme: se=6 ...` bei gueltigem Paar,
   `se=7` bei ungueltigem, `se=5` bei Abbruch, `se=4` bei Cursor-Bewegung. Fehlt eine Zeile,
   greift entweder das Prioritaets-Gate (`FUN_80045a18` @0x800454bc-c8, meldet `[se] GATE:`)
   oder der Record ist leer.

4. **Visuelle Verifikation** (falls der Puls beurteilt werden soll) NUR per gdigrab —
   Skill `re15-port-visual-verify`; `RE15_AUTOSHOT`/`RE15_SOFTWARE_RENDER` maskieren
   Textur-/Sprite-Fehler.

## 9. Offene Punkte / NICHT GEFUNDEN

- **Klangcharakter von CORE-Record 7 NICHT verifiziert.** Belegt ist nur: der Record existiert und
  ist gueltig (`00 00 83 00`), und RE2 benutzt den gleichnummerierten Record als Fehler-Quittung.
  Dass RE1.5s Record 7 *klanglich* ein Fehler-Beep ist, ist NICHT bewiesen — RE1.5 ruft ihn
  nirgends auf. Das ist **roh-Byte EXE-weit** geprueft (nicht nur im Decompilat, das bei RE2
  genau diesen Fall verfehlt hat): Suche nach der Instruktion `lui a0,0x40N` = Wort
  `0x3C0400N` ueber `info/Re1.5/PSX.EXE` ab Datei-Offset 0x800 (t_addr `0x80010000` aus
  Header `@0x18`), 4-Byte-aligned:

      lui a0,0x404 (cursor)  -> 0x8004a478, 0x8004a4a0, 0x8004a4c8, 0x8004a4f0
      lui a0,0x405 (cancel)  -> 0x8004a660
      lui a0,0x406 (confirm) -> 0x80033e54, 0x8004a51c
      lui a0,0x407 (FEHLER)  -> KEINE
      lui a0,0x409 (special) -> 0x8004a154

  Record 7 ist in RE1.5 also vorhanden, aber **komplett ungenutzt** — genau die Luecke, die RE2
  fuellt.
  Naechster Weg: `CORE00.VB` ueber den Tone-Index `0x83` in Record 7 aufloesen und das VAG
  dekodieren/anhoeren (der Port kann das bereits: `re15_audio_core_se(7)` + `RE15_SE_DEBUG=1`),
  oder die Records 4-7 als Familie vergleichen (Byte[2] laeuft 0x53/0x63/0x73/0x83 = ein
  aufsteigender Tone-Index derselben Programm-Gruppe, was fuer eine zusammengehoerige
  UI-Beep-Familie spricht — das ist ein Indiz, KEIN Beweis).
- **Welche CORE-Bank im Inventar konkret resident ist, wurde nicht dynamisch gemessen.**
  Statisch geklaert: die Bank kommt aus `FUN_800440c4(param_1)` ueber die ID-Tabellen
  `@0x80073a88`/`@0x80073ab0`; welcher `param_1` pro Stage/Raum gesetzt wird, habe ich NICHT
  verfolgt. Fuer diesen Fix irrelevant, weil Record 4-7 in CORE00..CORE0F identisch belegt sind
  (§6) — aber wer Record 7 klanglich pruefen will, sollte die Bank per Savestate bestaetigen
  (Skill `re15-savestate-ghidra`).
- **RE2s `param_2`/Positionsargument** ist an allen vier Stellen `0` (`addu a1,zero,zero`
  @0x8006b588/b5c0/b5d8) — also der nicht-positionale Zweig. Kein offener Punkt, hier nur
  zur Vollstaendigkeit festgehalten.

---

## 8. Verifikation (unabhaengig nachdisassembliert, 2026-08-25)

Adversariale Nachpruefung. Jede Adresse selbst disassembliert
(`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` bzw. `re2_disasm.py`), jede
Port-Zeile selbst gelesen, jede EDH-Byte-Aussage selbst nachgemessen.

**Gesamturteil: TEILWEISE — der Kern-Befund und ALLE Fix-Konstanten sind BESTAETIGT;
drei Neben-Aussagen sind falsch (§8.4).**

### 8.1 BESTAETIGT — Dispatcher + Erfolgspfad (§3.2/§3.3)

`dis 0x8004b33c 20` — exakt wie im Dossier:

    8004b340: lbu v0,9667(v0)     0x800b25c3
    8004b354: addiu at,at,19524   0x80074c44
    8004b364: jalr v0

`table 0x80074c44 6`:

    [ 0] -> 0x8004b37c  EXE
    [ 1] -> 0x8004b408  EXE
    [ 2] -> 0x000d000c  data/?     <- ab hier keine Pointer
    [ 3] -> 0x000f000e  data/?

`dis 0x8004b37c 36` — Instruktion fuer Instruktion identisch mit §3.3, inkl.
`andi v0,v1,0x8000` @0x8004b398, `andi v0,v1,0x4000` @0x8004b3a4,
`jal 0x8004e900` @0x8004b3b0, `jal 0x8004e054` @0x8004b3b8 (Delay-Slot
`andi a0,v0,0xff` @0x8004b3bc), `beq v0,zero,0x8004b3f0` @0x8004b3c4,
`jal 0x8004dadc` @0x8004b3cc, `sb zero,9668(at)` @0x8004b3dc,
`sb v0,9667(at)` @0x8004b3e4, `jr ra` @0x8004b400. **Genau 4 jal, kein Sound.**

### 8.2 BESTAETIGT — und STAERKER belegt als im Dossier: kein Se_on auf dem Pfad

Statt nur die Call-Huelle abzulaufen, habe ich **alle 41 direkten Aufrufstellen von
Se_on in der ganzen EXE** gesucht (Instruktionswort `jal 0x80045024` =
`0x0C011409`, 4-Byte-aligned ab Datei-Offset 0x800, `t_addr = 0x80010000` aus
Header `@0x18`):

    0x800123d4 0x80017684 0x8001782c 0x800178cc 0x80018064 0x800180e0 0x80018358
    0x80018424 0x800185ec 0x80018d9c 0x80018e3c 0x8002c70c 0x8002c97c 0x8003338c
    0x80033ed0 0x80034488 0x80034a0c 0x80034c44 0x80034e54 0x8003537c 0x800355f8
    0x80035a1c 0x80035c00 0x80035d38 0x80035e80 0x80036004 0x80036184 0x80036274
    0x80036340 0x800364f0 0x800367a8 0x80041730 0x8004a158 0x8004a47c 0x8004a4a4
    0x8004a4cc 0x8004a4f4 0x8004a520 0x8004a664 0x80052ad8 0x80052bcc

KEINE einzige liegt in
`0x8004b33c-0x8004b560` (Combine-Maschine + Walker + Frame-Handler),
`0x8004e054-0x8004e8f8` (Apply), `0x8004e900-0x8004ea68` (Matcher),
`0x8004dadc-0x8004dc44` (Compact), `0x80048904-0x80048a3c` (Cursor),
`0x8004df2c`, `0x8004947c`, `0x800492b8`, `0x80049390`, `0x80013b60` oder deren
Blaettern (`0x80013df0/f80/fdc`, `0x80029ac8`, `0x80068a60/c88/d50`).
Zusaetzlich: in keiner dieser Funktionen steht ein `jalr` (nur der Dispatcher
`jalr v0` @0x8004b364 selbst) — also auch kein indirekter Sound-Aufruf.
Die sieben Inventar-Treffer 0x8004a158/47c/4a4/4cc/4f4/520/664 decken sich mit
§3.5 (Dossier zitiert jeweils das `lui a0` eine Instruktion davor).

**=> Kern-Befund BESTAETIGT: RE1.5s Combine-Erfolgspfad ruft nachweislich keine
Sound-Routine, weder direkt noch indirekt.**

### 8.3 BESTAETIGT — RE2-Vorbild, EDH-Records, Effekt, Port-Zitate

**RE2 @0x8006b56c (`re2_disasm.py dis 0x8006b56c 36`, `info/re2leon/PSX.EXE`)** — Zeile
fuer Zeile identisch mit §4.2, inkl. `lui a0,0x404` @0x8006b580, `jal 0x8005ba28`
@0x8006b584, `jal 0x800695b0` @0x8006b5a0, `sb v0,4(s1)` @0x8006b5a8,
`beq v0,zero,0x8006b5bc` @0x8006b5b0 mit Delay-Slot `lui a0,0x407` @0x8006b5b4,
`lui a0,0x406` @0x8006b5b8, `jal 0x8005ba28` @0x8006b5bc, `addu a1,zero,zero`
@0x8006b5c0, `lui a0,0x405` @0x8006b5d0. Das Delay-Slot-Idiom ist korrekt gelesen.

**Kein toter Code:** Prolog `dis 0x8006b358` bestaetigt `s3=0x800cc1e8` (@0x8006b37c),
`s1=0x800d5bf0` (@0x8006b3d8), State-Byte 0x800d5bf4 (@0x8006b36c),
`sltiu v0,a0,0xc` @0x8006b3e8, Tabelle `0x80011bb0`. `table 0x80011bb0 13`:
`[0]->0x8006b40c`, `[1]->0x8006b640`, ... `[11]->0x8006c1fc`, `[12]` = Daten.
Der SE-Block 0x8006b56c-0x8006b5e8 liegt also **innerhalb von State 0**
(0x8006b40c..0x8006b63c) und wird erreicht. `a2` (Cursor VOR der Bewegung) wird in
State 0 @0x8006b40c-20 geladen — der Vergleich @0x8006b57c ist echt Vorher/Nachher.

**RE2s Se_on = FUN_8005ba28 BESTAETIGT** (`RE2_Quellcode_V2/FUN_8005ba28.c:26-32`):
`uVar10 = param_1 >> 0x18;` (Bank), `uVar18 = param_1 >> 0x10 & 0xff;` (Record),
`pbVar9 = (byte *)((&DAT_800dbb78)[uVar10] + uVar18 * 4);` — identische Packung und
identisches 4-Byte-Record-Layout wie RE1.5.

**RE1.5 Se_on / Bank 4 BESTAETIGT.** `FUN_80045024.c:23-29,56-60` wortgetreu wie
zitiert (`case 4: if (0x20 < uVar16) return; puVar9 = &DAT_801fbd00;`).
Zusatzbeleg, den das Dossier nicht zieht: der Loader
`FUN_800440c4.c:14-15` schreibt `SsVabOpenHeadSticky(DAT_800b25a8,4,0x38840)` nach
**`DAT_800b21f0`** — und Se_on liest fuer Bank 4 genau `(&DAT_800b21ec)[4]`
= 0x800b21f0. Bank-4-Zuordnung damit doppelt geschlossen.

**`lui a0,0x407` in RE1.5: NULL Treffer — BESTAETIGT** (eigener Roh-Scan aller
`lui a0,0x40N`, 4-Byte-aligned):

    0x404 -> 0x8004a478, 0x8004a4a0, 0x8004a4c8, 0x8004a4f0
    0x405 -> 0x8004a660
    0x406 -> 0x80033e54, 0x8004a51c
    0x407 -> (keine)
    0x409 -> 0x8004a154

**CORE00.EDH BESTAETIGT** (3176 Byte, `u32 @ size-8 = 0x40`, Magic bei 0x40 = `pBAV`,
16 Slots):

    rec 4 @0x10: 00 00 53 00
    rec 5 @0x14: 00 00 63 01
    rec 6 @0x18: 00 00 73 01
    rec 7 @0x1c: 00 00 83 00
    rec11 @0x2c: ff ff ff ff   <- erster Leer-Record

**17-Frame-Puls BESTAETIGT.** `dis 0x8004b408`: `sltiu v0,v1,0x11` @0x8004b414,
`addiu at,at,4084` = 0x80010ff4 @0x8004b424, `jr v0` @0x8004b434, und der Walker
enthaelt **kein einziges jal** (Ende `jr ra` @0x8004b560).
`table 0x80010ff4 18`: `[0..7]->0x8004b43c`, `[8..15]->0x8004b4a8`,
`[16]->0x8004b524`, `[17]` = 0. Handler A @0x8004b468-74:
`addiu a2,a2,1 / addiu a0,a0,1 / addiu a1,a1,-1 / addiu v0,v0,-1`.
Handler B @0x8004b4d8-e4 spiegelt: `addiu a2,a2,-1 / addiu a3,a3,-1 /
addiu a0,a0,1 / addiu a1,a1,1`, und kopiert 25bd->25be
(`lbu v1,9661(v1)` @0x8004b4d4 / `sb v1,9662(at)` @0x8004b4f0).
Gezeichnet wird es in `FUN_80047648.c` case 9 (Zeilen 139-152, Dossier sagt 141-149 —
Inhalt stimmt).

**RE2-Slide BESTAETIGT.** `dis 0x8006b640`: `sltiu v0,v0,0xa` @0x8006b64c = 10 Frames,
Zaehler `s1+5` = 0x800d5bf5, Akkumulation `s1+30 += s1+28` (=0x800d5c0e += 0x800d5c0c)
@0x8006b668-6c usw. Deltas in `FUN_8006a38c.c:9,13,19,23,27` = {2,4,0xfe,0xfc,0} und
`:29` = `((DAT_800d5bfd >> 1) - (DAT_800d5bfc >> 1)) * 3` — wie zitiert.

**Paar-Record-Feldreihenfolge BESTAETIGT** (§4.1). RE1.5 `dis 0x8004ea0c-2c`:
`lbu v0,1(v1)` -> `sb v0,9685(at)` (0x800b25d5 = result), `lbu v0,3(v1)` ->
`sb v0,9740(at)` (0x800b260c = pic), Rueckgabe `lbu v0,2(v1)` @0x8004ea2c = **action**
=> `{partner,result,action,pic}`. RE2 `FUN_800695b0.c:16-18`: `DAT_800d5c16 = pcVar1[2]`,
`DAT_800d5c17 = pcVar1[3]`, `return pcVar1[1]` => `{partner,action,result,pic}`.
Stride 0xc in RE1.5 ist im Disasm sichtbar (`sll v0,v1,1 / addu v0,v0,v1 / sll v0,v0,2`
@0x8004e9c0-c8 = *12) mit den Feldern `0x80074dac` (Ptr) und `0x80074db1` (Count).

**§7-Praemisse BESTAETIGT** ("exchange_exec liefert denselben Wert wie exchange_match"):
`dis 0x8004e054` — `addu s5,a0,zero` @0x8004e05c, danach wird s5 in der ganzen
Funktion NUR gelesen (`andi v0,s5,0xff` an 9 Stellen), Rueckgabe
`andi v0,s5,0xff` @0x8004e8d4, Epilog `jr ra` @0x8004e8f8. Also `ret == action & 0xff`
immer. `se4(action != 0 ? 6 : 7)` ist damit fallgleich zum Original-Gate @0x8004b3c4.

**State 7 ist erreichbar, kein toter Zweig.** `FUN_8004a0cc.c:147-148` `case 7:
FUN_8004b33c();`, und das State-Byte 0x800b25c2 (im Decompilat `DAT_800b25c0._2_1_`)
wird an mehreren Stellen inkrementiert (`FUN_8004a0cc.c:73,94`, `FUN_8004c058.c:11`),
erreicht 7 also aus State 6.

**Port-Zitate BESTAETIGT** (Zeilen selbst gelesen):
- `menu_common.c:1054` — "NO SE anywhere in 0x8004b37c-b404 (zero jal 0x80045024 ..." — exakt.
- `menu_common.c:110` — `static void se4(int id) { re15_audio_core_se(id); }` — exakt.
- `re15_inv_screen.c:1192-1197` — `int jx = (i == 0) ? (int8_t)st->comb_d0 : (int8_t)st->comb_d2;`
  ... `bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u) + x + st->list_x + jx,` — exakt.
- `audio_pc.c:687` — `if (re15_edt_decode(edt, se_id, &rec) != 0 || rec.empty) return;` — exakt.
- `second_cursor_move` (`menu_common.c:834`) schreibt `g_inv_screen.second_cursor`
  => Fix-Rezept Aenderung 3 (Vorher/Nachher-Vergleich) ist im Port umsetzbar.
- Kein Plattform-Sound-Mapper: `grep -rE "re2_sound_map|re2_se_map|platform_sound"
  re15_port/engine/src re15_port/include` = 0 Treffer — bestaetigt.

### 8.4 WIDERLEGT / KORRIGIERT

**(a) §3.4, Zeile `0x8004e054  (300 instr) -> 80013b60, 800492b8 x2, 8004947c` ist FALSCH.**
Die Funktion ist **545 Instruktionen** lang (0x8004e054 .. `jr ra` @0x8004e8f8) und
ruft **10 mal**, nicht 4 mal:

    8004e174: jal 0x80013b60
    8004e240: jal 0x800492b8
    8004e2d4: jal 0x800492b8
    8004e310: jal 0x8004947c
    8004e60c: jal 0x800492b8
    8004e71c: jal 0x800492b8
    8004e754: jal 0x8004947c
    8004e7d0: jal 0x80049390     <-- im Dossier NICHT gelistet
    8004e82c: jal 0x800492b8
    8004e894: jal 0x800492b8

Ursache des Fehlers: das `scan`-Kommando des Disasm-Tools bricht schon bei
`8004e1a4: jr v0` ab und meldet das faelschlich als `jr ra (end)` — dort steht aber
eine **Sprungtabellen-Verzweigung**, kein Return. Wer §3.4 als Call-Huelle
uebernimmt, uebernimmt eine unvollstaendige Liste.
**Wirkung auf den Befund: KEINE** — 0x80049390 (Icon-RECT-Bauer -> 0x80068d50/0x80068a60)
war bereits als Blatt der Huelle gelistet, und der EXE-weite Se_on-Aufrufstellen-Scan
in §8.2 deckt den Fall ohnehin ab. **Gleiche Warnung gilt fuer die uebrigen
`scan`-Zeilen in §3.4** (`0x8004e900`, `0x8004dadc`, `0x8004df2c`, `0x80048904`
enden zwar wirklich per `jr ra`, aber die Instruktionszahlen sind durchweg um 1 zu hoch).

**(b) Die Tail-Adresse `@0x8004e8d8: v0 = andi s5,0xff` (§7 und `menu_common.c:1047/1058`)
ist FALSCH.** Roh:

    8004e8d4: andi v0,s5,0xff     <- HIER steht das andi
    8004e8d8: lw ra,48(sp)        <- Epilog-Beginn
    8004e8f8: jr ra

Richtig ist `@0x8004e8d4`. Der Port traegt denselben Zahlendreher im Kommentar
(`menu_common.c:1047`, `:1058`) — beim Fix bitte mitkorrigieren.

**(c) §9-Aussage "Record 4-7 in CORE00..CORE0F identisch belegt" ist WIDERLEGT.**
Eigene Messung ueber alle 20 Baenke (`pbav/4` Slots, Leer = `ff ff ff ff`):

    CORE00..CORE0D, CORE0F : rec7 = 00 00 83 00 ; leer nur [11..15]
    CORE0E                 : rec7 = 00 00 93 14 ; KEINE Leer-Records
    CORE10..CORE13         : rec7 = ff ff ff ff ; leer [1,2,3,7,8,9,10,11..15]

CORE0E weicht **auch bei 4/5/6** ab:

    CORE00: rec4 00 00 53 00 | rec5 00 00 63 01 | rec6 00 00 73 01 | rec7 00 00 83 00
    CORE0E: rec4 00 00 53 10 | rec5 00 00 63 11 | rec6 00 00 73 11 | rec7 00 00 93 14
    CORE10: rec4 00 00 43 05 | rec5 00 00 53 06 | rec6 00 00 63 06 | rec7 ff ff ff ff

Korrekt ist also: Record 7 ist in CORE00..CORE0F **gueltig** (nie `ff ff ff ff`), aber
in CORE0E ist es ein **anderes Sample** (Tone 0x93 / Prog 0x14 statt 0x83 / 0x00) —
und in CORE0E/CORE10..13 sind sogar Cursor/Cancel/Confirm nicht identisch belegt.
Fuer den Fix bleibt es folgenlos (Port-Default ist CORE00, Leer-Record ist stumm), aber
die Behauptung "identisch belegt" darf so nicht stehen bleiben; die offene Frage
"welche Bank ist im Inventar resident" (§9) wird dadurch **wichtiger**, nicht irrelevant.

**(d) Kleinere Zeilennummern-Drift in den Port-Zitaten** (Inhalt jeweils korrekt):
`state7_select()` beginnt bei `menu_common.c:1061/1062`, nicht 1060.
`audio_pc.c`: der CORE00-Default steht auf **Zeile 960** (nicht 962), das
`RE15_SE_DEBUG`-getenv auf **Zeile 694** (nicht 692).
`FUN_8004a0cc.c`: `case 7: FUN_8004b33c();` steht auf **Zeile 147/148**, nicht 146.
`FUN_80047648.c`: der Jitter-Draw steht auf **Zeile 139-152**, nicht 141-149.

### 8.5 Zusaetzlicher Hinweis fuer den Implementierer (aus der eigenen Disasm)

RE2 und RE1.5 unterscheiden sich am Fehlerfall nicht nur im Sound, sondern im
**Folgezustand**: RE2 schreibt `sb v0,4(s1)` @0x8006b5a8, bei Ergebnis 0 also
State := 0 — der Exchange-Screen **bleibt offen** und piept (Record 7). RE1.5 setzt
in diesem Fall `sb v0,9666(at)` @0x8004b3f4 mit `v0 = 6` (@0x8004b3c8) — der Screen
**wird verlassen**. Das Fix-Rezept (se4(7) direkt vor dem Verlassen) uebernimmt RE2s
Sound, aber NICHT RE2s Verbleib — das ist korrekt so (§7 aendert bewusst nur den SE),
sollte aber im Commit-Kommentar stehen, damit es spaeter nicht als "halber Port"
missverstanden wird.

Ebenfalls aus der Disasm, gegen ein moegliches Missverstaendnis: Handler A
@0x8004b43c ist das **Zusammenziehen** (d0/d1 +1, d2/d3 -1), Handler B @0x8004b4a8
das Aufziehen. Der Port-Kommentar in `menu_common.c` labelt `[0..7]` als "GROW" und
`[8..15]` als "SHRINK" — die Reihenfolge ist im Code richtig implementiert, nur das
Etikett im Kommentar ist vertauscht.

### 8.6 Nicht nachgeprueft

- **Klangcharakter von CORE-Record 7** — bleibt offen wie in §9; ich habe das VAG nicht
  dekodiert. Der Byte-Familien-Hinweis (0x53/0x63/0x73/0x83) bleibt ein Indiz, kein Beweis.
- **Welche CORE-Bank im Inventar resident ist** — nicht dynamisch gemessen. Durch §8.4(c)
  ist das jetzt relevanter als im Dossier angenommen (CORE0E belegt Record 4-7 anders).
- **RE2s `FUN_8006a38c` nur aus dem Decompilat verifiziert** (Konstanten per grep bestaetigt),
  nicht roh-disassembliert — betrifft nur den bewusst NICHT uebernommenen Slide-Effekt.
