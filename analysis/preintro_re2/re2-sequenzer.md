# RE2-Retail: Standbild-Erzähl-Sequenz („Pre-Intro") — Präsentations-Mechanismus

Status: IN ARBEIT (inkrementell befüllt)
Datum: 2026-08-30
Auftrag: RE2s Standbild-Sequenz finden und den Präsentations-Mechanismus vollständig RE'en
(Bilder, Haltezeiten, Übergänge, Pan/Zoom, Text, Audio, Skip) — mit @0x…-Belegen —
zur Übernahme auf unsere ROOM1240-Montage. NICHTS einbauen, nur Analyse.

Quellen:
- `info/re2leon/` (SLUS-00748, PSX.EXE t_addr 0x80010000)
- `ghidra_re2_Leon.txt`, `RE2_Quellcode_V2/FUN_*.c`, `RE2_Quellcode_Overlays/`
- Disasm: `python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n>`

## Arbeitslog

- [x] CD-Baum sichten (Kandidaten-Dateien für Standbilder)
- [x] OPENING.BIN-Ladebasis + Entry ermittelt (verifiziert 79/79 jal-Konsistenz)
- [ ] Auslöser im Code finden (Titel-Idle / NEW GAME / …)
- [ ] Sequenzer-Funktion(en) disassemblieren
- [ ] Konstanten-Tabelle
- [ ] Mapping-Empfehlung ROOM1240

## 1. Die Standbild-Sequenz: Dateien (gefunden, gesichtet)

RE2 Leon (SLUS-00748-Baum unter `info/re2leon/`) hat GENAU die gesuchte Sequenz:
eine Standbild-Erzähl-Montage („Story so far") mit vorgerenderten Bildern + Textseiten
+ TV-Rausch-Textur. Dateien in `info/re2leon/COMMON/DATA/`:

| Datei | Größe | Inhalt (per BMP-Extrakt gesichtet) |
|---|---|---|
| `OPEN00.TIM` | 102944 | 2 Mikroskop-Standbilder (Virus-Zellen) |
| `OPEN01.TIM` | 154656 | 2 Standbilder: RE1-Villa-Szene (monochrom) + Labor/Tank (hell) |
| `OPEN02.TIM` | 74016 | S.T.A.R.S.-Logo + Umbrella-Logo |
| `OPEN03.TIM` | 65600 | 2 vorgerenderte Erzähltext-Seiten (Story Leon/Claire A, jap.) |
| `OPEN04.TIM` | 65600 | 2 weitere Erzähltext-Seiten (Szenario-B-Story: Leon/Ada/Claire/Sherry) |
| `OPEN05.TIM` | 32832 | 1 Textseite: EXTREME-BATTLE-Story („4 Spezialbomben…") |
| `OPEN06.TIM` | 8736 | kleine graue Fülltextur |
| `OPEN07.TIM` | 33312 | TV-RAUSCH-Textur (Static-Noise — Übergangs-Effekt!) |
| `OPEN10..14.TIM` | je 153600 | exakt 320·240·2 B = headerlose 16bpp-Vollbilder (5 Stück) |
| `OPNC.CPT` / `OPNL.CPT` / `OPNW.CPT` | 17454/17969/23563 | Caption-Daten (komprimiert) — C=Claire/L=Leon/W=? |

Overlay-Code: `info/re2leon/COMMON/BIN/OPENING.BIN` (13524 B = 0x34d4).

## 2. Lade-Mechanismus + Auslöser (EXE-Seite) — BELEGT

**Game-Mode-Dispatch-Tabelle @0x800a20bc** (EXE-Daten, Datei-Offset 0x928bc in
`PSX.EXE`, t_addr 0x80010000): 12-Byte-Einträge `(entry_fn, base_slot_ptr, mode_id)`:

```
@0x800a20bc: 0x80191808, slot@0x800107e8(=0x80190000), id 3   ; Modul @0x80190000
@0x800a20c8: 0x801bfd14, slot@0x800107f4(=0x801bfa18), id 1
@0x800a20d4: 0x8019009c, slot@0x800107ec(=0x80190000), id 6
@0x800a20e0: 0x801bfaa8, slot@0x800107f8(=0x801bfa18), id 0
@0x800a20ec: 0x801bfa84, slot@0x800107fc(=0x801bfa18), id 4
@0x800a20f8: 0x801bfad8, slot@0x80010800(=0x801bfa18), id 2   ; ← OPENING
@0x800a2104: 0x8011a0fc, …, id 5
@0x800a2110: 0x80158020, …, id 7
@0x800a211c: 0x8011a004, …, id 8..0xd                          ; Haupt-Spielschleife
```

**OPENING.BIN lädt KOMPLETT (inkl. 0xC0-Header) nach `0x801bfa18`**;
Entry-Funktion = `0x801bfad8` = Datei-Offset 0xC0 (erster Prolog).
Beweis: alle 79 modul-internen `jal`-Ziele landen unter Basis 0x801bfa18 auf
Funktionsanfängen (Prolog oder direkt nach `jr ra`), 0 Fehltreffer;
Basis 0x801bfa18 steht wörtlich im EXE-Slot @0x80010800, Entry 0x801bfad8
im Tabelleneintrag @0x800a20f8 mit mode_id=2.

Adress-Umrechnung Overlay: `datei_offset = addr - 0x801bfa18`.
Header: word0=0x50, Modul-Debugname „OP_TIM_LD" @file 0x14; die Pointer-Tabelle
@file 0x24 ist die PHASEN-Sprungtabelle des Sequenzers (siehe §3).

## 3. Der Sequenzer (OPENING.BIN) — Struktur, BELEGT per Disasm

### 3.1 Haupt-Tick (Entry @0x801bfad8, Endlosschleife bis Moduswechsel)

Modul-Variablen (RAM):
- `0x801c24c4` (word) — globaler Frame-Zähler der Sequenz (Skip erst ab >= 0x191 = 401)
- `0x801c24ca` (byte) — „Skip erlaubt"-Gate
- `0x801c24d4` (byte) — Skip-Zustand (0=aus, 1=Fade läuft, 2→3=Abschluss)
- `0x801c24d2` (half) — STATE (0..4), Dispatch @0x801c2078
- `0x801c24ce` (byte) — MODE (0..3), Dispatch @0x801c2068
- `0x801c24d0` (half) — Timer INNERHALB der aktuellen Phase (Frames)
- `0x801c24c2` (half) — PHASE (0..10) innerhalb state0/1, Dispatch @0x801bfa3c (=file 0x24)
- `0x801c24c8` (half) — gesicherter Vor-Skip-State

Ablauf pro Frame (@0x801bfb10..0x801bfcd0):
1. SKIP-Check @0x801bfb1c: `sltiu v0,frame,0x191` — Skip nur ab Frame 401;
   Taste: `lhu 0(s4=0x800de304)` & **0x800** (@0x801bfb44) = START;
   bei Skip: `jal 0x8005b2e4(120,24)` (Fade), state:=4, Merker 0x801c24c2 :=
   6 (Phase 0/1) / 7 (Phase 2) / 10 (Phase 3) — d.h. Skip springt ans
   PHASEN-Kapitelende, nicht hart raus.
2. MODE-Dispatch: `jalr *(0x801c2068 + 4*mode)` @0x801bfc68
   Tabelle @0x801c2068: mode0=0x801c0474, mode1=0x801c0630, mode2=0x801c051c, mode3=0x801c0388
3. STATE-Dispatch: `jalr *(0x801c2078 + 4*state)` @0x801bfc90
   Tabelle @0x801c2078: state0/1=0x801c0638, state2=0x801c0d58, state3=0x801c1304, state4=0x801bfcfc
4. `jal 0x801c1a0c` — Render/Prim-Aufbau
5. Frame-Zähler++ (Überlauf-Klemme auf 401 @0x801bfcbc)
6. `jal 0x80031f94(1)` — VSync/Flip-Service (EXE)

### 3.2 state0/1-Handler @0x801c0638: PHASEN-Maschine

`lhu phase@0x801c24c2; sltiu 0xb` (11 Phasen), Sprungtabelle @0x801bfa3c (Header file 0x24):

| Phase | Handler | (Datei-Offset) |
|---|---|---|
| 0 | 0x801c066c | 0xc54 |
| 1 | 0x801c06c0 | 0xca8 |
| 2 | 0x801c07b0 | 0xd98 |
| 3 | 0x801c08c0 | 0xea8 |
| 4 | 0x801c09d0 | 0xfb8 |
| 5 | 0x801c0aa0 | 0x1088 |
| 6 | 0x801c0c48 | 0x1230 |
| 7 | 0x801c0c94 | 0x127c |
| 8 | 0x801c0cb4 | 0x12a0? (=0x129c) |
| 9 | 0x801c0cd8 | 0x12c0 |
| 10 | 0x801c0d08 | 0x12f0 |

### 3.3 EXE-Helfer (identifiziert über RE2_Quellcode_V2)

| Adresse | Funktion | Beleg |
|---|---|---|
| 0x80076a00 | `memcpy(dst,src,len)` (Byte-Schleife) | FUN_80076a00.c |
| 0x8005b6f0 | BGM/SEQ-Steuerung (SsSeqPlay; param>>28=Bank, (param>>24)&0xf=Cmd; 0x01000000=Play) | FUN_8005b6f0.c |
| 0x8005b2e4 | BGM-Master-Fade: `FUN_8007a120(seq,0x7f,a0)`; DAT_800dfadf=a0(=120), DAT_800df340=a1(=24) | FUN_8005b2e4.c |
| 0x800129b4 | XA-/CD-Audio-Cue starten (baut CdlLOC aus Tabelle @0x80010818; a0=1, a1=Cue-Nr 1..5) | FUN_800129b4.c |
| 0x80031f94 | Frame-Service/VSync-Handshake (Mailbox @DAT_800d7824 + ChangeTh) | FUN_80031f94.c |
| 0x8002c350 | XA-Stream-Fertig-Test: Vorzeichen-Bit von `DAT_800dfc1c[a0*0x26]` | FUN_8002c350.c |
| 0x8008de24 / 0x8008de04 | SetGeomScreen / SetGeomOffset (GTE) | FUN_8008de24.c/FUN_8008de04.c |
| 0x80032150 | CD-Lade-Request (a0=3 bzw. 15) — von den Exit-Phasen benutzt | (per Nutzung) |

### 3.4 Element-System

- Element-Records: 12 Byte je Element, Array @`*(0x801c2ed8)`, Anzahl @0x801c24cc (lh).
  `byte0` = Element-Zustand/Kommando (0=aus), `byte1 & 0x10` = „fertig" (wird dann auf 0 gestellt).
- `0x801c1f64(a0=neuer_zustand, a1=hide_others)`: für jedes Element i:
  wenn Bit i in Maske @0x801c2ee8 → `byte0 := a0`; sonst wenn a1 → `byte0 := 0`.
  (Disasm @0x801c1f64..0x801c1fd8.)
- Beobachtete Zustands-Codes: **7** = einblenden/zeigen, **11** = ausblenden,
  23/27/39/43 = Sonder-Anzeige A/B (je nach Element-Typ), 71/75 = Sonder-Anzeige C.
  (Semantik wird am Renderer 0x801c1a0c verifiziert — §3.7.)
- `0x801c1fe0(slot,code,farbe,rectptr)`: konfiguriert Fullscreen-Fade-Prim im
  EXE-Array @0x800efc1c (Stride 76 = 19 Worte); rectptr==0 ⇒ Rect 0/0..320x240
  (Konstante 0x00f00140 @0x801c204c). Phase 0 ruft (3, 0, **0x00ffffff = WEISS**, 0).

### 3.5 state0/1: komplette Phasen-Timeline (t = Frames im Phasen-Timer @0x801c24d0)

**Phase 0** @0x801c066c (läuft sofort):
- Fade-Prim Slot 3 := weiß (0x801c1fe0(3,0,0xffffff,0)) @0x801c066c..0x801c0680
- BGM-Start `0x8005b6f0(0x01000000)` @0x801c0684; `DAT_800d5b50 := 107` (BGM-Id 0x6b) @0x801c068c
- Phase:=1, Timer:=0, byte@0x801c24c0:=3

**Phase 1** @0x801c06c0:
- t==240 (@0x801c06c8): Bild 1 stagen: `memcpy(0x80198000 ← 0x8011a000, 0x25800)` (=320·240·2!) + `jal 0x801c0474` (VRAM-Upload-Kick)
- t==420 (@0x801c0700): XA-Cue 1 (`0x800129b4(1,1)`) — Erzähler-Audio!
- t==430 (@0x801c071c): Maske:=0b11 {El.0,1} → Zustand 7 (einblenden), alle anderen aus
- t==793 (@0x801c0748): {0,1} → 11 (ausblenden)
- t==823 (@0x801c0770): `jal 0x801c051c`; 0x801c2ee4:=8; Maske:=60 {2..5} → 23; → **Phase 2**

**Phase 2** @0x801c07b0:
- t==64: XA-Cue 2
- t==74: {6,7} → 7
- t==349: {6,7} → 11
- t==359: {8,9} → 7
- t==875: {8,9} → 11
- t==905: {2..5} → 27; {10} → 71; → **Phase 3**

**Phase 3** @0x801c08c0:
- t==94: {11..14} → 7; XA-Cue 3
- t==104: {15,16} → 7
- t==204: {10} → 75
- t==321: {15,16} → 11
- t==351: Bild 2 stagen: `memcpy(0x80198000 ← 0x8013f814, 0x25800)` + `jal 0x801c0474`; {11..14} → 11; → **Phase 4**
  (0x8013f814 = 0x8011a000 + 0x25800 + 0x14 ⇒ die Vollbilder liegen als Block
   hintereinander im CD-Staging-Puffer @0x8011a000, je +0x14 Header)

**Phase 4** @0x801c09d0:
- t==94: ALLE Elemente aus (`0x801c1f64(0,1)`); XA-Cue 4
- t==104: {17,18} → 7
- t==550: {17,18} → 11
- t==580: `jal 0x801c051c`; 0x801c2ee4:=8; {19} → 39; → **Phase 5**

**Phase 5** @0x801c0aa0:
- t==96: {20..23} → 7; XA-Cue 5
- t==106: {24,25} → 7
- t==201: {24,25} → 11
- t==211: {26,27} → 7
- t==261: {19} → 43
- t==325: {26,27} → 11
- t==445 (@0x801c0bb0): BGM-Fade `0x8005b2e4(120,24)`; 0x801c2ee4:=8; 0x801c24c0:=0; Skip-Gate 0x801c24ca:=0; **Skip-Flag 0x801c24d4:=2** (⇒ Haupt-Tick schaltet nächsten Frame auf state 4 = Exit/Fade); `jal 0x801c051c`
- t>=446: wenn XA fertig (`0x8002c350(0)`-Vorzeichen): Skip-Gate:=1, alle Elemente aus, → Phase 6

**Exit-Phasen 6..10** (nur via Skip-Rücksprung bzw. Ende):
- Phase 6 @0x801c0c48: t==100, wenn byte@0x800ebb70==0: `DAT_800dfbd8 &= ~0x10`; `jal 0x80032150(3)` (CD-Request); → 7
- Phase 7 @0x801c0c94: warte bis `(DAT_800dfb74 & 0x200)==0`; → 8
- Phase 8 @0x801c0cb4: t==1: `jal 0x80032150(15)`; → 9
- Phase 9 @0x801c0cd8: warte `&0x200==0`; → 10
- Phase 10 @0x801c0d08: t==10: mode:=3, state:=4

SKIP (START ab Frame 401): sichert State, setzt PHASE := 6 (state0/1) / 7 (state2) / 10 (state3) und state:=4 — d. h. Skip fährt über den Fade-Out in die Exit-Phasen, kein harter Abbruch.
