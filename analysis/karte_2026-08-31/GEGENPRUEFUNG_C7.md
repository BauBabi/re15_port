# Gegenpruefung C7 (Rolle: Skeptiker) — RE2-Kartenscreen: State 4 + Ton 0x04040000 / Abbruch 0x2000 + Ton 0x04050000

**Ergebnis: NICHT widerlegt (refuted = false).** Jede Instruktion der Behauptung wurde
unabhaengig aus dem Binary nachdisassembliert (nicht nur aus dem Ghidra-Textdump), und der
Mechanismus dahinter (Screen-FSM, Pad-Flanke, Se_on) wurde zusaetzlich belegt.
Zwei Praezisierungen und vier port-relevante Luecken stehen in §7.

---

## 1. Roh-Bytes direkt aus dem Binary (nicht aus dem Textdump)

Binary: `info/re2leon/PSX.EXE` — PS-X-EXE-Header: `t_addr = 0x80010000` (@Header 0x18),
`pc0 = 0x80078408` (@0x10), `t_size = 0xf0800` (@0x1c).
Mapping `off = 0x800 + addr - 0x80010000`; fuer `0x8006d9c0` → Datei-Offset `0x5e1c0`.

`md5(info/re2leon/PSX.EXE) == md5(info/re2leon/SLUS_007.48) == 09a9b642cad1fde9cd7c9e2acdf17456`
→ beide Repo-Kopien byte-identisch, kein gepatchter Stand.

Selbst gelesene Worte (little-endian) ab 0x8006d9c0 — in PSX.EXE UND SLUS_007.48 identisch:

```
8006d9c0  9202001a   lbu   v0, 0x1a(s0)        ; = DAT_800d5c0a  (aktueller Blatt-/Etagen-Index)
8006d9c4  00000000   nop
8006d9c8  12420005   beq   s2, v0, 0x8006d9e0  ; s2 = ALTER Index (geladen @0x8006d880)
8006d9cc  24020004   addiu v0, zero, 4         ; li v0,4   <-- DELAY SLOT
8006d9d0  a2020002   sb    v0, 0x2(s0)         ; = DAT_800d5bf2  -> STATE = 4
8006d9d4  3c040404   lui   a0, 0x0404          ; a0 = 0x04040000
8006d9d8  0c016e8a   jal   0x8005ba28          ; (0x016e8a<<2) | 0x80000000 = 0x8005BA28
8006d9dc  00002821   addu  a1, zero, zero      ; a1 = 0 (nicht-positional)
8006d9e0  8e222128   lw    v0, 0x2128(s1)      ; = DAT_800ce310
8006d9e4  00000000   nop
8006d9e8  30422000   andi  v0, v0, 0x2000
8006d9ec  10400005   beq   v0, zero, 0x8006da04
8006d9f0  24020002   addiu v0, zero, 2         ; li v0,2   <-- DELAY SLOT
8006d9f4  a2020002   sb    v0, 0x2(s0)         ; STATE = 2
8006d9f8  3c040405   lui   a0, 0x0405          ; a0 = 0x04050000
8006d9fc  0c016e8a   jal   0x8005ba28
8006da00  00002821   addu  a1, zero, zero
8006da04  92030002   lbu   v1, 0x2(s0)
```

Alle sechs von der Behauptung zitierten Adressen existieren und sagen genau das Behauptete.

## 2. s0 / s1 sind belegt, nicht angenommen

Funktionsprolog `LAB_8006d650` (XREF aus einer Pointer-Tabelle @0x800a9abc):

```
8006d650  lui   v1, 0x800d
8006d654  lbu   v1, 0x5bf2(v1)      ; Switch-Selektor = DAT_800d5bf2
8006d660  lui   s0, 0x800d
8006d664  addiu s0, s0, 0x5bf0      ; s0 = 0x800d5bf0 -> s0+0x2 = 0x800d5bf2 ; s0+0x1a = 0x800d5c0a
8006d66c  lui   s1, 0x800d
8006d670  addiu s1, s1, -0x3e18     ; s1 = 0x800cc1e8 -> s1+0x2128 = 0x800ce310   OK
8006d684  sltiu v0, v1, 0x5
8006d688  beq   v0, zero, default
8006d698  lw    v0, switchdataD_80011c98[v1*4]
8006d6a0  jr    v0
```

Jump-Table @0x80011c98: case0 = 0x8006d6a8, case1 = 0x8006d7c4, case2 = 0x8006d820,
case3 = 0x8006d87c, case4 = 0x8006da24.
→ `+0x2` (0x800d5bf2) IST der Screen-State; "State 4" ist ein realer Case, kein Etikett.
Der zitierte Code liegt in `caseD_3` (0x8006d87c…0x8006da22) = der interaktive Kartenschirm.

## 3. State 4 = "Blatt nachladen" — belegt

`caseD_4` @0x8006da24 … 0x8006db1c:
- `li a0,0xab` @0x8006da24, `lui a1,0x801a`, String `s_MAP_FILE_80011c88` = **"MAP FILE"**
  (@0x8006da90/0x8006da94), `jal FUN_80012fb8` @0x8006dacc (a2 = 2)
- `jal LoadImage` @0x8006daf4 (RECT sp+0x10: x=0x1c0, y=0x100, w=0x40), `jal DrawSync` @0x8006dafc,
  `jal FUN_8006dea0` @0x8006db04
- `li v0,2; sb v0,0x28(s0)` @0x8006db0c/0x8006db10, `sb zero,0x29(s0)` @0x8006db18
- **`li v0,3` @0x8006db14 ; `sb v0, 0x2(s0)` @0x8006db1c → zurueck in State 3**

⇒ State 4 ist ein Ein-Schuss-Nachlader der Kartengrafik, danach wieder der interaktive State 3.
"Blatt nachladen" ist damit belegt, nicht geraten.

## 4. State 2 = Abbruch/Schliessen — belegt

`caseD_2` @0x8006d820: Frame-Zaehler `+0x3` bis `0xd` (`sltiu v0,v0,0xd` @0x8006d82c), Rechteck-Deltas
`+0x3c += 0x16`, `+0x46 += 0x13`, `+0x34 -= 0x15`, `+0x3a -= 0x15` (@0x8006d848-0x8006d86c)
— exakt die Gegenrichtung von `caseD_1` (`-0x16 / -0x13 / +0x15 / +0x15` @0x8006d7f4-0x8006d81c).
Nach Ablauf: `li v0,2; sb v0,0x1(s0)` @0x8006d838/0x8006d83c und `sb zero, 0x2(s0)` @0x8006d840
→ **State 0 = geschlossen**.
⇒ State 2 ist die Schliess-/Abbruch-Animation. Die Etikettierung "Abbruch" stimmt.

Nebenbefund (nicht Teil von C7, aber dieselbe Ton-Familie): `caseD_1` schaltet nach 14 Frames auf
State 3 und spielt dabei `lui a0,0x409` @0x8006d7dc + `jal 0x8005ba28` @0x8006d7ec =
**Ton 0x04090000** (Oeffnungs-Ton).

## 5. "Druckflanke" auf DAT_800ce310 — Mechanismus belegt (und praeziser als behauptet)

**Einziger Schreiber** von 0x800ce310 im gesamten Dump: `sw v0,-0x1cf0(at)` @**0x80039384**
(die uebrigen 34 XREFs sind alle `lw`). Der Rechenweg direkt davor:

```
80039354  lw   v0, 0x800ce314      ; VORIGER virtueller Pad-Wert
80039360  lw   a0, 0x800ce30c      ; AKTUELLER virtueller Pad-Wert
8003936c  xor  v0, v0, a0
80039370  and  v0, v0, a0          ; (prev ^ cur) & cur  = NEU GEDRUECKT
80039384  sw   v0, 0x800ce310
```

⇒ **DAT_800ce310 = Press-Edge. Bestaetigt.**
(Parallel dazu, gleiche Funktion: `800ce300 = (prev ^ cur) & cur` des ROHEN Wortes 0x800ce2fc
@0x80039364/0x80039368/0x80039374; `sh a2 → 0x800ce304` @0x800393a4 = RAW-held lo16.)

Wichtiger Zusatz, den die Behauptung NICHT nennt: 0x800ce30c ist **nicht** das rohe
libetc-PadRead-Wort, sondern das **konfig-remappte "virtuelle"** Wort. Builder @0x8003918c-0x800391e8:

```
80039190  lw   a1, 0x800ce30c            ; alten Wert merken
8003919c  sw   zero, 0x800ce30c          ; neu aufbauen
800391a0  lbu  v0, 0x800d46b2            ; gewaehlter Button-Config-Typ
800391ac  sll  v0, v0, 5                 ; *0x20 (16 x u16 pro Preset)
800391b0  addu t0, v0, 0x800a26a0        ; Remap-Tabelle
800391b4  sw   a1, 0x800ce314            ; PREV := alter virtueller Wert
800391c8  lhu  v0, [t0 + i*2]            ; RAW-Bitmaske fuer virtuelles Bit i
800391d0  and  v0, t1(= RAW-held 0x800ce2fc), v0
800391e8  or   0x800ce30c |= (1<<i)
```

Remap-Tabelle @0x800a26a0 (Datei-Offset 0x92ea0), selbst gelesen, 3 Presets x 16 u16
(RAW-Layout = libetc: 0x10 = Dreieck, 0x20 = Kreis, 0x40 = Kreuz, 0x80 = Viereck):

| virt. Bit | Preset 0 @0x800a26a0 | Preset 1 @0x800a26c0 | Preset 2 @0x800a26e0 |
|---|---|---|---|
| 12 = 0x1000 | RAW 0x0040 = Kreuz | 0x0040 = Kreuz | 0x0040 = Kreuz |
| **13 = 0x2000** | **RAW 0x0010 = Dreieck** | **RAW 0x0090 = Dreieck ODER Viereck** | **RAW 0x0010 = Dreieck** |
| 14 = 0x4000 | RAW 0x0020 = Kreis | 0x0020 = Kreis | 0x0020 = Kreis |
| 15 = 0x8000 | 0x0000 (leer) | 0x0000 | 0x0000 |

(Der vierte 0x20-Block @0x800a2700 ist KEIN Preset — das ist die im Pad-Writer selbst benutzte
Variable DAT_800a2700, beschrieben @0x80039218 / 0x80039254 / 0x80039420. Es gibt nur 3 Presets.)

⇒ v0x2000 ist genau das Bit, das der repo-eigene, unabhaengige Zensus
`analysis/confirm_cancel_mapping.md` als **RE2-Cancel** belegt hat
(RE2-MEM_CARD.BIN: Confirm `andi 0x1000` @0x801c08a8, Cancel `andi 0x2000` @0x801c08c8).
Zwei unabhaengige Screens, gleiche Rolle → bestaetigt.

Der 0x2000-Test ist damit die Flanke eines **konfig-abhaengigen** Bits (Preset 1 = Dreieck ODER
Viereck). Fuer den Port heisst das: nicht auf eine feste physische Taste hart-verdrahten.

## 6. FUN_8005ba28 = Se_on — belegt

Argumentzerlegung im Funktionskopf (Bytes selbst gelesen ab 0x8005ba28):
`srl t1,a0,0x18` @0x8005ba30 = **Bank**; `srl v0,a0,0x10; andi 0xff` @0x8005ba7c/0x8005ba80 = **Record**;
`andi a0,a0,0xff` @0x8005ba9c = positional-Flag.
`lb v0, 0x800d4c48[bank]` @0x8005ba64; `== -1 → return` @0x8005ba6c (Bank nicht geladen).
`lw a1, 0x800dbb78[bank*4] + record*4` @0x8005ba8c-0x8005ba98 = EDT-Record; `== -1 → return` @0x8005baa0.
Danach Prioritaetstest `jal FUN_8005c92c` @0x8005bb94 (vergleicht `0x800d4ca0[...]`) und, wenn frei,
Belegung eines Voice-Slots (`0x800d4f18 + ch*0x20`: Flag = 1 @0x8005bbdc, Record, Prio, Bank, Pitch,
Vol-L/R). Das ist die klassische SE-Anforderungs-/Kanalvergabe-Funktion; einziger `jal` im Rumpf ist
der Prio-Test, Ruecksprung `jr ra` @0x8005bd64.

Deckungsgleich mit der bereits im Repo (unabhaengig, 2026-08-25) getroffenen Identifikation
`analysis/nutzer_batch_2026-08-25/biss-se.md`: "RE2 `Se_on` = FUN_8005ba28", identisches
Argumentformat wie RE1.5 `FUN_80045024` (BANK = a0>>24 @0x80045028, RECORD = (a0>>16)&0xff @0x8004507c).

⇒ `a0 = 0x04040000` = Bank 4 / Record 4; `a0 = 0x04050000` = Bank 4 / Record 5. Toene bestaetigt.
Gegenprobe in einem anderen RE2-Screen derselben Overlay-Familie: der Box-Screen benutzt dieselbe
Bank-4-Familie 0x0404 / 0x0405 / 0x0406 (`analysis/itembox_re2/re2-box-screen.md` §10,
`jal 0x8005ba28` @0x8006ffdc / 0x8006fff8).

---

## 7. Praezisierungen und Luecken (kein Widerspruch, aber port-relevant)

1. **Delay-Slot-Verschiebung (kosmetisch).** "sb 4,2(s0) @0x8006d9d0" bzw. "sb 2,2(s0) @0x8006d9f4"
   ist eine Kurzform: der Store steht an der genannten Adresse, die Konstante wird jeweils 4 Byte
   davor im Branch-Delay-Slot geladen (`li v0,4` @0x8006d9cc, `li v0,2` @0x8006d9f0).
   Semantisch korrekt, die Adresse fuer die Konstante ist genau genommen eine Instruktion frueher.

2. **Zwei Gates fehlen in der Behauptung.** Der komplette Blattwechsel-Block (0x8006d904-0x8006d9d8)
   inklusive State-4-Uebergang wird uebersprungen, wenn:
   - `lw v0, DAT_800cfb74` @0x8006d8e0 ; `bgez v0, 0x8006d9e0` @0x8006d8e8 (Flag >= 0), oder
   - `lbu a1, 0x1a(s0)` @0x8006d8f0 ; `sltiu v0,a1,2` @0x8006d8f8 ; `bne → 0x8006d9e0` @0x8006d8fc
     (weniger als 2 Blaetter → kein Wechsel moeglich).
   Der Cancel-Test @0x8006d9e0 laeuft dagegen IMMER (er ist das Sprungziel beider Gates).

3. **Der Blattwechsel selbst ist HELD, nicht Flanke.** Vor/zurueck liest das ROHE gehaltene
   lo16-Wort: `lhu v0, 0x211c(s1)` = **DAT_800ce304** mit `andi 0x1000` @0x8006d90c und
   `andi 0x4000` @0x8006d964, jeweils hinter einem Wiederhol-/Zeit-Gate `jal FUN_80077360`
   @0x8006d934 / @0x8006d998. Nur der Abbruch nutzt die virtuelle Flanke 0x800ce310.
   Wer das im Port vereinheitlicht, baut eine Divergenz.

4. **State 4 haengt an einem Vorher/Nachher-Vergleich, nicht an einer Taste.** s2 wird am Anfang
   von case 3 geladen (`lbu s2, 0x1a(s0)` @0x8006d880), v0 am Ende (@0x8006d9c0); nur wenn
   `s2 != v0` (der Index also in DIESEM Frame tatsaechlich geaendert wurde) faellt der Code auf
   0x8006d9d0 durch. Das deckt sich mit "nach einem Wechsel", ist aber der Mechanismus, den der
   Port nachbauen muss (Latch auf den alten Index, nicht auf den Tastendruck).

5. Zwischen den beiden Held-Tests liegen zwei zusaetzliche Filter, die den Index erst gueltig
   machen: Lookup-Tabellen `DAT_800a9aec[idx]` @0x8006d920/0x8006d950 (Richtung 0x1000) und
   `DAT_800a9b04[idx]` @0x8006d97c/0x8006d9b4 (Richtung 0x4000), jeweils mit `sltiu ...,2`-Gate
   und einem `jal FUN_80077360`-Test. Der neue Index wird erst @0x8006d958 bzw. @0x8006d9bc
   nach `0x1a(s0)` geschrieben. Wer nur den State-4-Uebergang portiert, hat den Wechsel selbst
   noch nicht.

## 8. Wie geprueft wurde (reproduzierbar, rein statisch)

```
python -c "import struct; d=open('info/re2leon/PSX.EXE','rb').read(); \
  off=lambda a: 0x800+a-0x80010000; \
  print([hex(struct.unpack('<I',d[off(a):off(a)+4])[0]) for a in range(0x8006d9c0,0x8006da08,4)])"
```

plus Quer-Lesen von `ghidra_re2_Leon.txt` Zeilen 208050-208430 (Funktion 0x8006d650),
Zeilen 134780-134960 (Pad-Writer, Schreiber 0x80039384) und `RE2_Quellcode_V2/FUN_8005ba28.c`.
Der Remap-Tabellendump kam aus `info/re2leon/PSX.EXE` @Datei-Offset 0x92ea0 (= 0x800a26a0).
Eine dynamische Messung war nicht noetig — die Behauptung ist rein statisch pruefbar und wurde
statisch aus dem Auslieferungs-Binary bestaetigt.
