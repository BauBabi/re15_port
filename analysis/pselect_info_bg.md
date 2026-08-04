# Charakterauswahl: „INFO-WERTE UNTEN haben schwarzen Hintergrund" — RE-Dossier

Datum 2026-08-04. Nutzer-Report Nr. 2 zu diesem Screen (Nr. 1 → Fix bf92ef5b: Farbwert-0x0000-Regel
in `re15_render_pc_pselect_text` + `tmoji_strip`). Frage: welcher Draw-Pfad rendert den unteren
Info-Block (PROFILE/HEIGHT/AGE/WEIGHT/BLOOD-Werte), was macht das Original, wo divergiert der Port.

## TL;DR

1. **Der untere Info-Block IST Group C** (die 2 PROFILE-SPRTs des SELECTH3-Atlas) — es gibt im Port
   KEINEN weiteren Draw-Pfad dafür (`re15_render_pc_pselect_text`, render_pc.c ps_text[2]/[3]).
2. **IDLE ist bereits byte-identisch** (Pixel-Beweis unten): die schwarzen Glyph-Schatten/Badges
   (Texel-Farbe 0x8400) zeichnet AUCH das Original opak — 1024/1024 Positionen im Savestate-FB
   = rgb(0,0,8), identisch zum Port-Screenshot.
3. **Die echte Divergenz liegt in der CONFIRM-PHASE**: das Original schaltet Group C beim Confirm
   auf ein SEMI-TRANSPARENTES Prim (Code 0x66) unter **ABR 1 = ADDITIV** und blendet mit einer
   RGB-Rampe (0x80→0, 32 Frames) — additiv kann NIE abdunkeln, die schwarzen Badges sind ab dem
   ersten Confirm-Frame unsichtbar. Der Port färbt stattdessen die Alpha-Textur per Color-Mod nach
   SCHWARZ → **opake schwarze Text-/Badge-Stempel über dem zoomenden Charakter** (gemessen:
   `shots/pselect_series/f090/f110/f130` — schwarze Rechtecke auf Leons Beinen, y=168..216 = „unten").
4. Zweitbefund: die NAMENS-Zeilen (Group B) haben im Original **gar keinen Fade-Mechanismus**
   (kein einziger RGB-Write außer dem 0x80-Init) — der Port dunkelt sie mit derselben erfundenen
   `td`-Rampe ab.

## 1. PORT: Draw-Pfad des unteren Info-Blocks

- `re15_port/platform/pc/main.c` `pc_run_player_select()` lädt `DATA/SELECTH3.TIM` (8bpp 256×256
  + 256er-CLUT) und ruft pro Frame `re15_render_pc_pselect_text(&s_sel_txt, sel)`.
- `re15_port/platform/pc/src/render_pc.c`:
  - Atlas→ARGB-Konvertierung in `re15_render_pc_pselect_text()` (Z. ~1358): Index 0 transparent,
    Farbwert 0x0000 transparent (bf92ef5b), Rest opak. **Diese Konvertierung ist korrekt** (s. §3).
  - Draw in `end_frame` (Z. ~954–988): Tabelle `ps_text[4]` — Namen (16,48)/(168,48) 136×24 UV(0,0)/(0,48)
    und **PROFILE-Blöcke (24,168)/(176,168) 120×48 UV(0,88)/(0,136)** = der untere Info-Block.
  - CONFIRM: `int td = s_pselect_dim * 2; tm = 255 - td; SDL_SetTextureColorMod(...tm)` + Alpha-Blend
    → **der Bug** (s. §4).
- Sonst zeichnet in diesem Screen-Bereich nur noch: SELECTH-Backdrop (Z. 658), Backdrop-Dim-Rect
  (Z. 665, nur Confirm), 3D-Modelle, Half-Screen-Tiles (Z. 996). Kein weiterer Text-Pfad.

## 2. ORIGINAL: Quelle + Mechanismus (TITLE.BIN, lädt @0x80100000, file_off = addr−0x80100000)

### 2.1 Quell-TIM des Info-Blocks
- **`DATA/SELECTH3.TIM`** (66080 B): flag=0x9 (8bpp+CLUT). CLUT-Block: 256×1 @VRAM(0,480) laut Header,
  Einträge @file 0x14. Bild 128×256 Halfwords (=256×256 Texel) @file 0x220, VRAM-Ziel laut Header (0,0);
  zur Laufzeit hochgeladen auf **Texpage (320,256)** und CLUT nach **VRAM (0,502)** — beides
  savestate-verifiziert (`sub_newgame.sav`): VRAM-Zeile 502 == TIM-CLUT (256/256), Texel @(320,256)
  == TIM-Pixel (Stichprobe 180/180). `GetClut(0, 0x1f6)` (a1=0x1f6=502) @0x801008b0 (Group B-Setup)
  bzw. @0x80100d40 (Group C-Setup) → clut-id 0x7d80 (in den RAM-Prims bestätigt).
- **PROFILE-Region** (UV 0,88..183, je 120×48): Histogramm — idx 0 (Farbe 0x0000) 3715 px =
  transparent; **idx 251 = Farbe 0x8400** (r0 g0 b2·8=(0,0,8), STP=1) 1024 px = die schwarzen
  Glyph-Schatten/Badges (CLUT-Byte-Offset in der Datei: 0x14+251·2 = **0x20A** → Bytes `00 84`);
  Text-Indizes 224..254 — **ALLE Nicht-Null-Farben der PROFILE-Region tragen STP=1** (für den
  additiven Confirm-Draw geautort). Die Namens-Region (UV 0,0..) nutzt dagegen STP=0-Farben (idx 1..53)
  + 0x0000-Hintergrund.

### 2.2 Prim-Aufbau Group C (Setup @0x80100d2c)
Pro Charakter (Stride 0x54) VIER SPRT-Prims à 0x14:
- scene+0x270 (dbuf0) / +0x298 (dbuf1): **Code 0x64 = OPAK** (SetSprt-Default), RGB (0x80,0x80,0x80)
  @0x80100d8c-94, XY/UV aus Tabelle @0x8010265c, W/H=0x78/0x30 @0x80100dc0-cc, clut @0x80100e00.
- scene+0x284 / +0x2ac: Kopie mit **`ori v0,v0,0x2` @0x80100e68** auf das Code-Byte (−33(s0)=+0x28b)
  → **Code 0x66 = SEMI-TRANSPARENT (ABE)**.
- Texpage: **`GetTPage(1, 1, 0x140, 0x100)` @0x80100ea8-eb8** → 8bpp, Page (320,256), **ABR=1 =
  ADDITIV (B+F)**; ins DR_MODE-Paar @scene+0x314 geschrieben (Savestate: `e10000b5`, (0xb5>>5)&3=1 ✓).
  (Group B dagegen: `GetTPage(1, 0, ...)` @0x801009e0-f0 → `e1000095`, ABR=0 — für Code-0x64-Prims eh inaktiv.)
- Savestate `sub_newgame.sav` (Idle): alle 8 Prims wie oben, Code 0x64/0x66, XY (24,168)/(176,168),
  UV (0,88)/(0,136), WH 120×48, clut 0x7d80. Scene-Ptr @0x801026c0 = 0x80190000.

### 2.3 Auswahl opak↔semi + Flash-Rampe (Draw @0x80100f14)
- Prim-Wahl: `prim = scene+0x270 + dbuf*0x28 + FLAG*0x14`, **FLAG = u16 scene+0x268**
  (@0x80100f74-9c). FLAG=0 → opakes Prim, FLAG=1 → ADDITIV-Prim.
- **IDLE: FLAG=0** (Init `sh zero,616(s0)` @0x80100d6c; Savestate scene+0x268=0) → opak.
- **CONFIRM (sub3 @0x8010132c): FLAG=1 + Counter scene+0x26a=0x20** (@0x80101374-80).
- Pro Frame: Counter−1 bis 0 (@0x80100f44-50); wenn FLAG≠0: **Prim-RGB = Counter<<2**
  (@0x80100fa0-b4: `sll v0,v0,2; sb v0,4/5/6(a2)`) → additive Rampe 0x80→0 über 32 Frames,
  danach RGB=0 = additives No-Op = unsichtbar. Ein aktives Gate pro Charakter existiert im
  Draw-Loop nicht (beide Seiten werden geadded, 2 Iterationen `bne a1,zero,0x80100f70` @0x80101004).

### 2.4 Zeichen-Reihenfolge (warum idle „transparent wirkt" + wer was abdunkelt)
- Frame-Draw **FUN_8002137c**: `DrawOTag(SMALL-OT 0x800ac6d8+dbuf*0x40 ab [0xf])` →
  `DrawOTag(BIG-OT 0x800aa6d8+dbuf*0x1000 ab [0x3ff])` → `DrawOTag(ROOT-OT 0x800aa698+dbuf*0x20 ab [7])`.
- Sprites: Group B in **BIG[4]** (Insert-Basis 0x800aa6e8 = 0x800aca34−9036, hand-verifiziert;
  Savestate-Tag →0x800aa6e4=&BIG[3] ✓), Group C in **BIG[6]** (0x800aa6f0 = 0x800aca34−9028;
  Tag →0x800aa6ec=&BIG[5] ✓). Modelle liegen bei otz>>4 ≫ 6 → werden FRÜHER gezeichnet
  ⇒ Text IMMER ÜBER den Modellen (auch die opaken Badges — im Idle-FB nachgemessen).
  ⚠️ Der Disasm-Helfer annotiert negative `addiu`-Immediates falsch (zeigte „0x800adcbc"); die
  Registerrechnung ergibt 0x800aa6f0/0x800aa6e8 und der Savestate bestätigt sie.
- CONFIRM-Backdrop-Fade (Treiber @0x8010026c, c4/c6/c7 = scene+0xc4/0xc6/0xc7): rampt c6 +=4 und
  schreibt c6 in die RGB-Bytes von 3 TILE-Prims @scene+0xcc+dbuf*0x30+i*0x10 (@0x80100348-50);
  AddPrim je Typ-Byte (scene+0xc8+i): Typ 2 → **SMALL-OT[0]** (t5 = 0x800aca34−860 = 0x800ac6d8,
  @0x8010030c) = NUR über der Backdrop-Ebene (vor dem BIG-OT gezeichnet → Modelle+Text bleiben hell),
  sonst → **ROOT-OT[0]** (t4 = 0x800aca34−9116 = 0x800aa698, @0x8010031c) = über allem.
  ⇒ Der Port-Aufbau „Dim-Rect nur über dem Backdrop, vor den Modellen" ist damit mechanisch belegt.

### 2.5 Pixel-Beweis IDLE (Savestate `stage_saves/sub_newgame.sav`, Anzeige-Puffer)
Leon-PROFILE (24,168)-(143,215) gegen die TIM-Texel klassifiziert:
- **1024/1024** Positionen mit Texel 0x8400 → FB = **0x0400 = rgb(0,0,8)** (opak gezeichnet!).
- idx-0-Positionen → FB = Backdrop (Dach-Farben).
- Helle Text-Texel → FB = Texel&0x7fff (1:1, RGB-Mod 0x80 = 1.0×).
Port-Screenshot (Frame 60, Readback-Pfad): auf denselben 1024 Positionen ebenfalls rgb(0,0,8)
→ **Idle Port == PSX, Pixel für Pixel** (`shots/pselect_cmp_leon.png` links PSX / rechts Port).

## 3. Bewertung Nutzer-Report

Die Badges hinter den Werten sind im Idle AUCH im Original opak schwarz (§2.5) — dort ist nichts zu
fixen. Der reproduzierbare „schwarze Hintergrund unter den Info-Werten" entsteht in der
**CONFIRM-Phase** (die jeder New-Game-Start durchläuft): der Port stempelt den kompletten Block
(Glyphen+Badges) als nach Schwarz gemodulte, aber weiterhin OPAKE Alpha-Textur über Modell+Szene
(gemessen: `shots/pselect_series/f090/f110/f130.png` — schwarze Kästen auf Leons Beinen), während
das Original ab Confirm-Frame 1 ADDITIV zeichnet (Badges unsichtbar, Text als Glow-Flash, der in
32 Frames ausklingt). Zuordnung des Reports zu dieser Phase = wahrscheinlich (Screenshot des
Nutzers liegt nicht vor); der Mechanismus-Unterschied selbst ist belegt.

## 4. Fix-Plan (byte-true, KEIN Engine-Code geändert — Plan)

In `render_pc.c` `end_frame` (pselect-Text-Block) + `main.c` FSM:
1. **Flag+Counter spiegeln** (scene+0x268/0x26a): `cz==0` (sub3) → `flag=1, counter=0x20`
   (@0x80101374-80); pro Frame `if (flag && counter) counter--` (@0x80100f44-50).
2. **PROFILE-Sprites (ps_text[2]/[3])**: `flag==0` → bisheriger opaker Pfad (byte-belegt korrekt).
   `flag==1` → `SDL_SetTextureBlendMode(s_selecth3_tex, SDL_BLENDMODE_ADD)` (= ABR 1, DR_MODE
   `e10000b5` @0x80100ea8 + Code 0x66 @0x80100e68) mit ColorMod = `min(counter*4,0x80)` skaliert
   auf SDL (0x80→255, d.h. `min(counter<<3,255)`); RGB-Quelle: Counter<<2 @0x80100fa0-b4.
   `counter==0` → Draw entfällt (additiv mit 0 = No-Op). Beide Seiten zeichnen (kein per-Char-Gate
   im Original-Loop).
3. **Namens-Sprites (ps_text[0]/[1])**: `td`-ColorMod ENTFERNEN — TITLE.BIN enthält keinerlei
   RGB-Write auf die Group-B-Prims außer dem 0x80-Init (@0x80100938-940); der Zoom-Animator
   0x80100a50 schreibt nur XY (+8/+10), der Add-Pfad 0x80100bd0 nur Tag-Links. Namen bleiben bis
   zum Pan-Arm voll hell; ab Pan (cz>=2) deaktiviert das Original die Blocks der NICHT gewählten
   Seite (`sw zero,0(t3)`/`sw zero,60(t3)` @0x80100b38-3c) — Port-Äquivalent: Idle-Namen ab
   `cz>=2` nicht mehr zeichnen (statt des jetzigen `dim<255`-Gates, das zufällig zeitgleich greift),
   der gewählte Name läuft über den vorhandenen Slide-Draw weiter.
4. Backdrop-Dim-Rect (Z. 665) und Modell-Helligkeit bleiben — Mechanismus jetzt belegt (§2.4).

## 5. OFFEN

- **gdigrab-IST-Aufnahme des echten Fensters** war in dieser Session nicht möglich (Fenster wird
  nicht komponiert — Capture einfarbig weiß; bekannte Limitation getrennter Display-Sessions).
  Schluss-Verifikation nach Fix gemäß Skill `re15-port-visual-verify` im Nutzer-Flow nachholen;
  ebenso die Bestätigung des Nutzers, dass der Report die Confirm-Phase meint (oder Screenshot).
- Die 3 Fade-TILEs @scene+0xcc: welcher Eintrag Typ 2 (SMALL-OT[0]) vs. ROOT-OT[0] ist + ihre
  Prim-Codes (Setup-Stelle noch nicht disassembliert) — für diesen Fix nicht nötig, für einen
  100%-byte-true Confirm-Fade (Subtraktiv-Kurve) später verifizieren.
- Ob das Original die Group-C-Prims der nicht gewählten Seite nach sub5 noch added (scene+0x26c-
  Semantik) — visuell irrelevant (RGB≈0 ab Frame 32), bei Bedarf per Confirm-Savestate klären.

## Artefakte
- `shots/pselect_port_idle.png` (BMP; Port Idle Frame 60), `shots/pselect_cmp_leon.png`
  (PSX↔Port-Vergleich), `shots/pselect_series/f000..f139.png` (Port-Confirm-Sequenz; f090/f110/f130
  = die schwarzen Stempel), Savestate-FB-Dump s. Scratchpad-Skripte (ps_probe/ps_pixdiff/ps_ot/ps_cmp).
