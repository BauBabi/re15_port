# Title-Menü NEW-GAME-Confirm: Fade-out + „Biohazard 2"-Stimme (Dossier)

Datum 2026-08-04. Auftrag: (a) Port-Fade nach Menü-Confirm ist zu schnell; (b) die ikonische
Announcer-Stimme („Biohazard 2!") beim Bestätigen fehlt im Port. Beides ORIGINAL disassembliert
(TITLE.BIN + PSX.EXE) **und** dynamisch in DuckStation gemessen (Media-Capture, Video+Audio).

Quellen: `info/Re1.5/PSX/BIN/TITLE.BIN` (lädt @0x80100000, KEIN Header, off = addr−0x80100000),
`info/Re1.5/PSX.EXE` (off = addr−0x80010000+0x800), `RE_15_Quellcode_V2/FUN_*.c`,
Capture `shots/title_confirm_capture.avi` (640×480 @59.82, mit Audiospur),
dekodierte Stimme `shots/title_confirm_voice_core11_vag5_6.wav`.

---

## 1. MESSUNG (DuckStation, MZD-Disc, Boot → Title → Cross auf NEW GAME)

Treiber: vgamepad, RB=ToggleMediaCapture; Script `title_fade_measure.py` (Scratchpad; Ablauf =
boot 64 s → Capture ON → 2 s Menü → Cross → 12 s → Capture OFF). Per-Frame-Luma via
ffmpeg `signalstats` (YAVG), Frames = Vsyncs (59.82 fps NTSC-Fields).

| Capture-Frame (vsync) | Beobachtung |
|---|---|
| f0–f68 | Title-MENÜ steht (NEW GAME weiß, LOAD GAME/OPTION blau), YAVG 35.6. **Kein BGM — Titelmenü ist stumm** (Audiospur exakt 0). |
| f69 | Cross-Confirm ⇒ Screen schlagartig WEISS (YAVG 235 = TV-Weiß) **und** Voice-Onset in der Audiospur (1.16 s). |
| f69–f98 | Weiß-Blitz klingt ADDITIV ab; **jede Stufe exakt 2 Vsyncs** (YAVG-Paare 235.00/235.00, 226.01/226.01, …), 15 sichtbare Stufen, Menü + Titelbild bleiben darunter gezeichnet. |
| f99–~f393 | LANGSAMES subtraktives Abdunkeln (YAVG 35.6 → schwarz); Menü-Zeilen bleiben bis zuletzt sichtbar (fr250: „NEW GAME" noch lesbar, dunkler). f350 fast schwarz, f450 voll schwarz. |
| ~f393–f627 | SCHWARZ GEHALTEN (~3.9 s — CD lädt Player-Select-Assets; latenz-, nicht timer-gebunden). |
| f627–… | Player-Select („PLEASE SELECT MAIN CAST") blendet ein. |
| Audio | Stimme: Onset 1.16 s (=Confirm-Frame), Dauer **≈3.96 s**, danach wieder totale Stille. Kreuzkorrelation gegen dekodiertes CORE11-VAG5: **r=0.611** beim Onset-Offset (mp3-/SPU-bedingt <1; Zufall ≈0). |

**2-Vsync-Paare:** das Title-Screen läuft 480i — ein Engine-Frame = 2 Fields. (In-Game flippt
progressive mit `FUN_80029ac8(2)` @0x8001cad0-d4 = 30 fps; das Title-/Fade-Loop flippt
`FUN_80029ac8(1)` @0x80102d18 — gemessen hält jede Fade-Stufe 2 Vsyncs ⇒ Engine-Tick ≈ 30 Hz.)

---

## 2. ORIGINAL-MECHANISMUS (Disasm)

### 2.1 Menü-Task + Confirm-Pfad (TITLE.BIN @0x80102b00)

```
80102b04  lhu v0, -14494(v0)        ; Pad-Edge-Wort DAT_800ac762
80102b10  andi v0,v0,0x4000         ; DOWN  -> cursor(0x801026ca)+1, SE-Call:
80102b1c  lui  a0,0x404             ;   FUN_80045024(0x04040000, 0)   = CORE-SE 4 (Cursor-Blip)
80102b60  andi v0,v0,0x1000         ; UP    -> cursor-1, ebenfalls FUN_80045024(0x04040000,0) @80102b6c-74
80102c14  andi v0,v0,0x8f0          ; CONFIRM = △|○|✕|□|Start (Maske 0x8f0)
80102c20  lui  a0,0x400             ; !!! DER TITEL-CALL:
80102c24  jal  0x80045024           ;   FUN_80045024(0x04000000, 0)   = CORE-SE 0 (Announcer-Voice)
80102c3c  addiu v0,v0,2             ; Menü-State 0x801026c5 = cursor+2  (2=NEW GAME,3=LOAD,4=OPTION)
80102c48  jal  0x80102ccc           ; -> BLOCKING-FADE (für ALLE drei Menüpunkte!)
80102c50  ori  a0,zero,0x140        ; danach FUN_80043850(0x140,0xf0,0)
80102c60  lbu  v1,9925(v1)          ; nur wenn State==2 (NEW GAME):
80102c80  lw   v0,0(a2)             ;   DAT_800aca38 |= 0x20000  @80102c84-90
80102c8c  jal  0x80021634           ;   FUN_80021634(2,0)  (Clear-Mode schwarz)
80102c9c  jal  0x80029ba4           ;   REPLACE-Task -> Player-Select FUN_80029ba4(0x80101094)
```
- Die Stimme feuert **VOR** dem Fade (0x80102c20 vor 0x80102c48) und bei **jedem** Menü-Confirm
  (Gate ist nur `pad & 0x8f0`, kein Cursor-Check).
- Der Fade läuft für NEW GAME **und** LOAD GAME **und** OPTION (Call @0x80102c48 vor dem Dispatch).

### 2.2 Der Fade selbst (TITLE.BIN FUN @0x80102ccc) — DREI Phasen über die EXE-Fade-Engine

Engine = Kanal-Array @0x800b5458 (Stride 0x44): +0x00 level(u16), +0x02 step(s16), +0x04 ABR,
+0x05..07 RGB-Masken, +0x08 OT-Bucket. Im Port bereits byte-true als `fade_common.c` vorhanden.
- `FUN_800217b0(cfg)` = config: step/ABR/Masken (ABR = arg0>>8, in DR_MODE via `<<5` — Disasm-verifiziert im Decompile FUN_800217b0.c).
- `FUN_800216ec(kick)` @0x800216ec (disasm-verifiziert): step<0 ⇒ level=0x7fff; step>0 ⇒ level=0; step==0 ⇒ level=arg1.
- `FUN_80021880(tick)`, 1×/Engine-Frame aus `main` @0x80020f44: **B = level>>7** (Helligkeits-Byte),
  Fade-Prim RGB=(B,B,B)&Masken in den OT, **danach level += step**; Vorzeichen-Bit = fertig
  (`FUN_8002178c` = level>>31).

```
80102cd4  ori   a0,zero,0x100       ; PHASE A: Kanal 0, ABR 1 (ADDITIV B+F)
80102cd8  addiu a1,zero,-2048       ;   step = -0x800/Tick  (B: 255 -> 0, -16/Tick)
80102cdc  ori   a2,zero,0x7         ;   RGB-Masken alle 0xff (Weiss)
80102ce0  jal   0x800217b0          ;   a3=0 (OT-Bucket 0)
80102cf8  jal   0x800216ec          ;   kick: step<0 => level=0x7fff (B startet 255 = VOLLWEISS)
80102d00  jal   0x8002178c          ;   Warteschleife bis Vorzeichen-Bit:
80102d10  jal   0x80102a10          ;     pro Frame Menü-Zeilen+Copyright neu zeichnen
80102d18  jal   0x80029ac8 (1)      ;     Flip; 0x7fff/0x800 => 16 Ticks à 2 Vsyncs ≈ 0.53 s
80102d28  ori   a1,zero,0xe0        ; PHASE B: FUN_800217b0(0x200, +0xe0, 7, 1)
                                    ;   ABR 2 (SUBTRAKTIV B-F), step +0xe0 => B=(n*0xe0)>>7 = +1.75/Tick
80102d48  jal   0x800216ec          ;   kick: step>0 => level=0 (B startet 0)
80102d50  jal   0x8002178c          ;   Warteschleife (gleiches Redraw+Flip):
                                    ;   fertig bei n*0xe0 >= 0x8000 => 147 Ticks à 2 Vsyncs ≈ 4.9 s
80102d80  jal   0x800217b0          ; PHASE C: FUN_800217b0(0x200, 0, 7, 1)  (step=0)
80102d98  jal   0x800216ec          ;   FUN_800216ec(0, 0x7fff, 0xffffff, 0) => level FEST 0x7fff
80102da0  jal   0x80029ac8 (1)      ;   B=255 subtraktiv dauerhaft = SCHWARZ HALTEN, 1 Flip, return
```

**Frame-Bilanz (Engine-Ticks à 2 Vsyncs / NTSC):**
| Phase | Mechanismus | Ticks | Zeit |
|---|---|---|---|
| A | Weiß-BLITZ, additiv, B=255→15, −16/Tick | 16 | ≈0.53 s |
| B | Fade-to-black, subtraktiv, B=0→255, +1.75/Tick | 147 | ≈4.91 s |
| C | Schwarz halten (level=0x7fff, step=0) | bis Player-Select bereit (CD-Latenz, ~3.9 s gemessen; KEIN Timer) | — |

Messung deckt A (15 sichtbare Weiß-Stufen in exakten 2-Vsync-Paaren; die 16. Stufe B=15 fällt mit
dem Phase-B-Umschalten zusammen), B (fast schwarz bei f350 = 125 Ticks ⇒ B≈218 ✓, schwarz ≤f450)
und C (schwarz bis f627, dann Player-Select).

Randbefund: der Title-**Fade-in** nach Boot/FMV ist dieselbe Engine:
`FUN_800217b0(0x200, -0x400, 7, 3)` @0x80102054-64 = subtraktiv B 255→0, **−8/Tick, 32 Ticks ≈ 1.07 s**.
Menü-Exit-Fades der Unterscreens: `+0x400` (32 Ticks) @0x801024f0-500.

### 2.3 Die Stimme: FUN_80045024(0x04000000, 0) = CORE-Bank SE 0 = CORE11.VB VAG 5+6

`FUN_80045024` (RE_15_Quellcode_V2, + Katalog Z.185): arg = `(bank<<24)|(se_index<<16)|pos_flag`.
Bank 4 = residenter CORE-SE-Table @0x801fbd00, VAB-Handle `DAT_800b21ec[4]` (=DAT_800b21f0); SE-Eintrag
4 Bytes `[vab_override, prog|0x80, tone<<4, chain<<5|voice]`; Play = `SsUtKeyOnV(voice, vab, prog,
tone, note=tone[+6], fine=tone[+5], volL, volR)`; `chain`>0 keyt die Folge-Tones auf Folge-Voices.

**Welche CORE-Bank liegt im Titel?** Die Title-Task-Init lädt sie SELBST:
```
80102704  jal 0x800440c4            ; FUN_800440c4(a0=0x11)  <- TITLE.BIN @0x80102704-08
```
`FUN_800440c4(idx)` = CORE-Bank-Loader (Decompile + „CORE EDH"/„CORE VBD"-Strings @0x8004412c/0x800441a4):
lädt CD-File `u16[0x80073a88][idx]` (EDH, ids 161,163,…,199) nach 0x801fbd00 und `u16[0x80073ab0][idx]`
(VB, ids 162,…,200), `SsVabOpenHeadSticky(EDH+trailer_u32@[size-8], 4, 0x38840)`.
idx **0x11** ⇒ **SOUND/CORE11.EDH + CORE11.VB** (Datei-ids 195/196; CORE00..CORE13 hex-benannt, 20 Paare
= 20 Tabellen-Einträge). In-Game lädt derselbe Loader stattdessen die CHARAKTER-Bank:
`FUN_800440c4(DAT_800aca5c)` @0x800316d8-e8 (0=Leon→CORE00, 4=Elza→CORE04) im Player-Load FUN_800314b0.

**CORE11.EDH byte-geparst** (3176 B; VAB-Header „pBAV" @0x40 via Trailer-u32 @size−8; 1 Prog, 7 Tones,
6 VAGs; Tone-Table @0x40+0x820):

| SE | Rohbytes @EDH | Aufloesung | VAG (@CORE11.VB) | Inhalt |
|---|---|---|---|---|
| **0** (Confirm) | `00 80 17 20` | prog0 **tone1 + chain1 ⇒ tone2** | tone1: vol **127**, pan **0** (hart L), note=center=60 ⇒ **44100 Hz** → **VAG5** @0x1c50, 99856 B; tone2: vol 127, pan **127** (hart R) → **VAG6** @0x1a260, 99952 B | **STEREO-Announcer „Biohazard 2!"**, je 174 748 Samples = **3.96 s** (Messung: 3.9 s, r=0.611) |
| **4** (Cursor UP/DOWN) | `00 00 43 05` | prog0 tone4 | tone4: vol 75, pan 64, note 62/center 74 ⇒ Pitch 2^(−1) = **22050 Hz** → VAG2 @0x30, 1360 B | Menü-Blip |
| 5 | `00 00 53 06` | tone5, vol 80 | VAG3 @0x580, 2944 B, 22050 Hz | (Sub-Screen-SE) |
| **6** (Press-Start-State) | `00 00 63 06` | tone6, vol 90 | VAG4 @0x1100, 2896 B, 22050 Hz | `FUN_80045024(0x04060000,0)` @0x80102ac4-c8 im State-Handler 0x80102a8c |

CORE11.VB ist mit 207 056 B ~5× größer als CORE00.VB (40 464 B) — die Differenz IST das Voice-Paar.
Dekodiert: `shots/title_confirm_voice_core11_vag5_6.wav` (VAG5=L, VAG6=R).

---

## 3. PORT-IST (Divergenzen)

`re15_port/platform/pc/main.c`, Title-Menü-Loop (~Z.1753-1822):

1. **Fade (Z.1777-1785):** `for (int f = 8; f <= 256; f += 8)` mit `re15_render_pc_set_title_fade(f)`
   = 32 Frames à 60 fps (≈0.53 s) **multiplikativer** Alpha-Schwarz-Quad (render_pc.c Z.1054-1059,
   `SDL_BLENDMODE_BLEND`). Der Kommentar zitiert 0x80102ccc nur für den MECHANISMUS-Ort; die **Rate
   „8/frame, 32f" steht NIRGENDS im Disasm** = Rate-Defekt (wie der Card-Fade `fa+=24`).
   Original: Weiß-Blitz (16 Ticks additiv) + 147 Ticks subtraktiv + Schwarz-Halten = ≈5.45 s @30 Hz-Ticks.
2. **Kein Confirm-Sound:** im gesamten Title-Loop existiert kein Audio-Call (grep `re15_audio` in
   main.c: erst ab FMV/Ingame). Es fehlen: SE 0 (Voice) beim Confirm, SE 4 beim Cursor-Move.
3. **CORE-Bank:** `load_core_se_vab_pc()` (audio_pc.c Z.667-703) lädt hart **CORE00** — im Titel müsste
   **CORE11** liegen (@0x80102708), in-game die Charakter-Bank (CORE00 Leon / **CORE04 Elza**,
   @0x800316dc `lbu a0,DAT_800aca5c`). D.h. auch Elza-Ingame-CORE ist derzeit falsch (Nebenbefund).
4. **Mixer kann kein Pan:** `se_play_layers` (audio_pc.c Z.558-594) ignoriert tone[+3] pan — das
   Voice-Paar braucht hart L/R.
5. Nebenbefund **Fade-in** (Z.1813-1814): `tblink<20 ? 255-tblink*13 : 0` = geratene 20f/13er-Rate;
   Original = subtraktiv **−8/Tick, 32 Ticks** (@0x80102054-64).

---

## 4. FIX-PLAN (jede Konstante mit Beleg)

**(a) Fade-out nach Menü-Confirm — 3 Phasen über die vorhandene `fade_common.c`-Engine:**
1. Confirm-Zweig in main.c: statt der 32×(+8)-Schleife die Engine benutzen (sie IST schon byte-true
   portiert): `re15_fade_config(0, /*abr*/1, 7, -0x800, 0)` + `re15_fade_kick(0,0)` (@0x80102cd4-e4,
   kick @0x80102ce8-cfc) → Schleife `while(!re15_fade_done(0))` mit Title+Menü-Redraw und
   **`re15_fade_tick()` alle 2 gerenderten 60-fps-Frames** (Engine-Tick = 2 Vsyncs, Messung §1;
   Flip-Beleg @0x80102d18 `FUN_80029ac8(1)` im 480i-Title vs In-Game `(2)` @0x8001cad0).
2. Danach `re15_fade_config(0, /*abr*/2, 7, +0xe0, 1)` + kick (@0x80102d28-4c), gleiche Schleife
   (147 Ticks ≈ 4.9 s).
3. Danach `re15_fade_config(0, 2, 7, 0, 1)` + `re15_fade_kick(0, 0x7fff)` (@0x80102d78-9c) =
   Schwarz HALTEN, solange der Player-Select lädt (im Port latenzfrei ⇒ Halten entfällt de facto —
   das Original hält nur CD-Latenz, keinen Timer).
4. Renderer: Phase A = vorhandener additiver Weiß-Overlay `re15_render_pc_set_white_fade(B)`
   (render_pc.c, ABR-1-Modell). Phase B/C brauchen einen **SUBTRAKTIVEN** Overlay (PSX ABR 2 = B−F):
   `SDL_ComposeCustomBlendMode(ONE, ONE, REV_SUBTRACT)` als neuer `re15_render_pc_set_sub_fade(B)`;
   der multiplikative `s_title_fade`-Alpha-Quad ist NICHT byte-true (Kurvenform weicht ab).
5. Der Fade gilt für ALLE drei Menüpunkte (Call-Ort @0x80102c48 VOR dem Item-Dispatch @0x80102c60);
   LOAD GAME/OPTION im Port entsprechend durch denselben Fade schicken.

**(b) Stimme + Menü-SEs:**
1. `load_core_se_vab_pc()` → parametrisieren: `re15_audio_prime_core(int idx)` lädt
   `SOUND/CORE%02X.EDH/.VB` (byte-true FUN_800440c4; Title-Init ruft idx **0x11** @0x80102704-08,
   Game-Start ruft **DAT_800aca5c** (0 Leon / 4 Elza) @0x800316d8-e8 — beide Callsites im Port setzen).
2. Title-Loop: bei Cursor-UP/DOWN `re15_audio_core_se(4)` (@0x80102b1c-24 / @0x80102b6c-74 =
   `FUN_80045024(0x04040000,0)`); beim Confirm — VOR Fade-Beginn — `re15_audio_core_se(0)`
   (@0x80102c20-24 = `FUN_80045024(0x04000000,0)`). `se_play_layers` löst Record 0
   (`00 80 17 20` = prog0, tone1, chain1) bereits als tone1+tone2 auf.
3. `se_play_layers`/Mixer: tone[+3]-**Pan** implementieren (0=links, 64=Mitte, 127=rechts; Slots
   kriegen volL/volR) — Voice-Paar VAG5(L)/VAG6(R) mit vol 127; Pitch stimmt schon
   (note2pitch2: tone1 note60/center60 ⇒ 0x1000 = 44100 Hz; Cursor-Blip tone4 62/74 ⇒ 22050 Hz).
4. Die Stimme läuft 3.96 s WEITER über Fade + Player-Select (Original: SPU-Voice, kein Stop-Call im
   Pfad) — der spätere `re15_audio_prime_core(0/4)` beim Game-Start darf die spielende Stimme nicht
   hart abschneiden (Slots spielen dekodierte Kopien — im PC-Mixer bereits der Fall, nur die
   free()-Reihenfolge beachten).

**Nebenbefunde (separat fixbar):** Title-Fade-IN −8/Tick 32 Ticks (@0x80102054-64) statt `255−t*13`;
Elza-Ingame-CORE-Bank = CORE04 (@0x800316dc); Press-Start-SE 6 nur relevant, falls der Press-Start-
State (0x80102a8c) je portiert wird (der MZD-Boot zeigt das Menü direkt — Messung f0, kein Input davor).

---

## 5. OFFEN

- Exakte Task-/Thread-Semantik von `FUN_80029ac8(n)` (Render-Thread-Yield @0x80020c10-f54): die
  2-Vsync-Kadenz des Title ist GEMESSEN (§1, YAVG-Paare) und über Phase-B-Länge gegengeprüft; die
  statische Herleitung (480i-DISPENV des Title vs 240p in-game) ist nicht bis zur DISPENV-Init
  verfolgt.
- Ob der Weiß-Blitz die 16. Stufe (B=15) anzeigt: Messung zeigt 15 Stufen + direkten Übergang in
  Phase B; Tick-Reihenfolge (draw-vor-integrate @0x80021880) legt 16 gezeichnete Stufen nahe —
  ±1 Frame ohne Sichtbarkeitsrelevanz.
- SE 5 (CORE11 tone5/VAG3): Nutzer im Title-Binary nicht lokalisiert (vermutlich Sub-Screen-Exit).
